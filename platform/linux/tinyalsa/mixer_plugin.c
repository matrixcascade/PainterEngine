/* mixer_plugin.c
** Copyright (c) 2019, The Linux Foundation.
**
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above
**     copyright notice, this list of conditions and the following
**     disclaimer in the documentation and/or other materials provided
**     with the distribution.
**   * Neither the name of The Linux Foundation nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
** THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED
** WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
** MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT
** ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
** BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
** CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
** SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
** BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
** WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
** OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
** IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**/

#include <tinyalsa/plugin.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <ctype.h>
#include <poll.h>
#include <dlfcn.h>
#include <string.h>
#include <sys/eventfd.h>
#include <sys/ioctl.h>

#include <linux/ioctl.h>
#include <sound/asound.h>

#include "snd_card_plugin.h"
#include "mixer_io.h"

/** Encapulates the mixer plugin specific data */
struct mixer_plug_data {
    /** Card number associated with the plugin */
    int card;
    /** Device node for mixer */
    void *mixer_node;
    /** Pointer to the plugin's ops */
    const struct mixer_plugin_ops *ops;
    /** Pointer to plugin responsible to service the controls */
    struct mixer_plugin *plugin;
    /** Handle to the plugin library */
    void *dl_hdl;
};

static int mixer_plug_get_elem_id(struct mixer_plug_data *plug_data,
                struct snd_ctl_elem_id *id, unsigned int offset)
{
    struct mixer_plugin *plugin = plug_data->plugin;
    struct snd_control *ctl;

    if (offset >= plugin->num_controls) {
        fprintf(stderr, "%s: invalid offset %u\n",
				__func__, offset);
        return -EINVAL;
    }

    ctl = plugin->controls + offset;
    id->numid = offset;
    id->iface = ctl->iface;

    strncpy((char *)id->name, (char *)ctl->name,
            sizeof(id->name) - 1);
    ((char *)id->name)[sizeof(id->name) - 1] = '\0';

    return 0;
}

static int mixer_plug_info_enum(struct snd_control *ctl,
                struct snd_ctl_elem_info *einfo)
{
    struct snd_value_enum *val = ctl->value;

    einfo->count = 1;
    einfo->value.enumerated.items = val->items;

    if (einfo->value.enumerated.item >= val->items)
        return -EINVAL;

    strncpy(einfo->value.enumerated.name,
            val->texts[einfo->value.enumerated.item],
            sizeof(einfo->value.enumerated.name) - 1);
    einfo->value.enumerated.name[sizeof(einfo->value.enumerated.name) - 1] = '\0';

    return 0;
}

static int mixer_plug_info_bytes(struct snd_control *ctl,
                struct snd_ctl_elem_info *einfo)
{
    struct snd_value_bytes *val;
    struct snd_value_tlv_bytes *val_tlv;

    if (ctl->access & SNDRV_CTL_ELEM_ACCESS_TLV_READWRITE) {
        val_tlv = ctl->value;
        einfo->count = val_tlv->size;
    } else {
        val = ctl->value;
        einfo->count = val->size;
    }

    return 0;
}

static int mixer_plug_info_integer(struct snd_control *ctl,
                struct snd_ctl_elem_info *einfo)
{
    struct snd_value_int *val = ctl->value;

    einfo->count = val->count;
    einfo->value.integer.min = val->min;
    einfo->value.integer.max = val->max;
    einfo->value.integer.step = val->step;

    return 0;
}

void mixer_plug_notifier_cb(struct mixer_plugin *plugin)
{
    plugin->event_cnt++;
    eventfd_write(plugin->eventfd, 1);
}

/* In consume_event/read, do not call eventfd_read until all events are read from list.
   This will make poll getting unblocked until all events are read */
static ssize_t mixer_plug_read_event(void *data, struct snd_ctl_event *ev, size_t size)
{
    struct mixer_plug_data *plug_data = data;
    struct mixer_plugin *plugin = plug_data->plugin;
    eventfd_t evfd;
    ssize_t result = 0;

    result = plug_data->ops->read_event(plugin, ev, size);

    if (result > 0) {
        plugin->event_cnt -=  result / sizeof(struct snd_ctl_event);
        if (plugin->event_cnt == 0)
            eventfd_read(plugin->eventfd, &evfd);
    }

    return result;
}

static int mixer_plug_subscribe_events(struct mixer_plug_data *plug_data,
                int *subscribe)
{
    struct mixer_plugin *plugin = plug_data->plugin;
    eventfd_t evfd;

    if (*subscribe < 0 || *subscribe > 1) {
        *subscribe = plugin->subscribed;
        return -EINVAL;
    }

    if (*subscribe && !plugin->subscribed) {
        plug_data->ops->subscribe_events(plugin, &mixer_plug_notifier_cb);
    } else if (plugin->subscribed && !*subscribe) {
        plug_data->ops->subscribe_events(plugin, NULL);

        if (plugin->event_cnt)
            eventfd_read(plugin->eventfd, &evfd);

        plugin->event_cnt = 0;
    }

    plugin->subscribed = *subscribe;
    return 0;
}

static int mixer_plug_get_poll_fd(void *data, struct pollfd *pfd, int count)
{
    struct mixer_plug_data *plug_data = data;
    struct mixer_plugin *plugin = plug_data->plugin;

    if (plugin->eventfd != -1) {
        pfd[count].fd = plugin->eventfd;
        return 0;
    }
    return -ENODEV;
}

static int mixer_plug_tlv_write(struct mixer_plug_data *plug_data,
                struct snd_ctl_tlv *tlv)
{
    struct mixer_plugin *plugin = plug_data->plugin;
    struct snd_control *ctl;
    struct snd_value_tlv_bytes *val_tlv;

    ctl = plugin->controls + tlv->numid;
    val_tlv = ctl->value;

    return val_tlv->put(plugin, ctl, tlv);
}

static int mixer_plug_tlv_read(struct mixer_plug_data *plug_data,
                struct snd_ctl_tlv *tlv)
{
    struct mixer_plugin *plugin = plug_data->plugin;
    struct snd_control *ctl;
    struct snd_value_tlv_bytes *val_tlv;

    ctl = plugin->controls + tlv->numid;
    val_tlv = ctl->value;

    return val_tlv->get(plugin, ctl, tlv);
}

static int mixer_plug_elem_write(struct mixer_plug_data *plug_data,
                struct snd_ctl_elem_value *ev)
{
    struct mixer_plugin *plugin = plug_data->plugin;
    struct snd_control *ctl;
    int ret;

    ret = mixer_plug_get_elem_id(plug_data, &ev->id, ev->id.numid);
    if (ret < 0)
        return ret;

    ctl = plugin->controls + ev->id.numid;

    return ctl->put(plugin, ctl, ev);
}

static int mixer_plug_elem_read(struct mixer_plug_data *plug_data,
                struct snd_ctl_elem_value *ev)
{
    struct mixer_plugin *plugin = plug_data->plugin;
    struct snd_control *ctl;
    int ret;

    ret = mixer_plug_get_elem_id(plug_data, &ev->id, ev->id.numid);
    if (ret < 0)
        return ret;

    ctl = plugin->controls + ev->id.numid;

    return ctl->get(plugin, ctl, ev);

}

static int mixer_plug_get_elem_info(struct mixer_plug_data *plug_data,
                struct snd_ctl_elem_info *einfo)
{
    struct mixer_plugin *plugin = plug_data->plugin;
    struct snd_control *ctl;
    int ret;

    ret = mixer_plug_get_elem_id(plug_data, &einfo->id,
                    einfo->id.numid);
    if (ret < 0)
        return ret;

    ctl = plugin->controls + einfo->id.numid;
    einfo->type = ctl->type;
    einfo->access = ctl->access;

    switch (einfo->type) {
    case SNDRV_CTL_ELEM_TYPE_ENUMERATED:
        ret = mixer_plug_info_enum(ctl, einfo);
        if (ret < 0)
            return ret;
        break;
    case SNDRV_CTL_ELEM_TYPE_BYTES:
        ret = mixer_plug_info_bytes(ctl, einfo);
        if (ret < 0)
            return ret;
        break;
    case SNDRV_CTL_ELEM_TYPE_INTEGER:
        ret = mixer_plug_info_integer(ctl, einfo);
        if (ret < 0)
            return ret;
        break;
    default:
        fprintf(stderr,"%s: unknown type %d\n",
				__func__, einfo->type);
        return -EINVAL;
    }

    return 0;
}

static int mixer_plug_get_elem_list(struct mixer_plug_data *plug_data,
                struct snd_ctl_elem_list *elist)
{
    struct mixer_plugin *plugin = plug_data->plugin;
    unsigned int avail;
    struct snd_ctl_elem_id *id;
    int ret;

    elist->count = plugin->num_controls;
    elist->used = 0;
    avail = elist->space;

    while (avail > 0) {
        id = elist->pids + elist->used;
        ret = mixer_plug_get_elem_id(plug_data, id, elist->used);
        if (ret < 0)
            return ret;

        avail--;
        elist->used++;
    }

    return 0;
}

static int mixer_plug_get_card_info(struct mixer_plug_data *plug_data,
                struct snd_ctl_card_info *card_info)
{
    /*TODO: Fill card_info here from snd-card-def */
    memset(card_info, 0, sizeof(*card_info));
    card_info->card = plug_data->card;

    return 0;
}

static void mixer_plug_close(void *data)
{
    struct mixer_plug_data *plug_data = data;
    struct mixer_plugin *plugin = plug_data->plugin;
    eventfd_t evfd;

    if (plugin->event_cnt)
        eventfd_read(plugin->eventfd, &evfd);

    plug_data->ops->close(&plugin);
    dlclose(plug_data->dl_hdl);

    free(plug_data);
    plug_data = NULL;
}

static int mixer_plug_ioctl(void *data, unsigned int cmd, ...)
{
    struct mixer_plug_data *plug_data = data;
    int ret;
    va_list ap;
    void *arg;

    va_start(ap, cmd);
    arg = va_arg(ap, void *);
    va_end(ap);

    switch (cmd) {
    case SNDRV_CTL_IOCTL_CARD_INFO:
        ret = mixer_plug_get_card_info(plug_data, arg);
        break;
    case SNDRV_CTL_IOCTL_ELEM_LIST:
        ret = mixer_plug_get_elem_list(plug_data, arg);
        break;
    case SNDRV_CTL_IOCTL_ELEM_INFO:
        ret = mixer_plug_get_elem_info(plug_data, arg);
        break;
    case SNDRV_CTL_IOCTL_ELEM_READ:
        ret = mixer_plug_elem_read(plug_data, arg);
        break;
    case SNDRV_CTL_IOCTL_ELEM_WRITE:
        ret = mixer_plug_elem_write(plug_data, arg);
        break;
    case SNDRV_CTL_IOCTL_TLV_READ:
        ret = mixer_plug_tlv_read(plug_data, arg);
        break;
    case SNDRV_CTL_IOCTL_TLV_WRITE:
        ret = mixer_plug_tlv_write(plug_data, arg);
        break;
    case SNDRV_CTL_IOCTL_SUBSCRIBE_EVENTS:
        ret = mixer_plug_subscribe_events(plug_data, arg);
        break;
    default:
        /* TODO: plugin should support ioctl */
        ret = -EFAULT;
        break;
    }

    return ret;
}

static const struct mixer_ops mixer_plug_ops = {
    .close = mixer_plug_close,
    .read_event = mixer_plug_read_event,
    .get_poll_fd = mixer_plug_get_poll_fd,
    .ioctl = mixer_plug_ioctl,
};

int mixer_plugin_open(unsigned int card, void **data,
                      const struct mixer_ops **ops)
{
    struct mixer_plug_data *plug_data;
    struct mixer_plugin *plugin = NULL;
    void *dl_hdl;
    char *so_name;
    int ret;

    plug_data = calloc(1, sizeof(*plug_data));
    if (!plug_data)
        return -ENOMEM;

    plug_data->mixer_node = snd_utils_open_mixer(card);
    if (!plug_data->mixer_node) {
        /* Do not print error here.
         * It is valid for card to not have virtual mixer node
         */
        goto err_get_mixer_node;
    }

    ret = snd_utils_get_str(plug_data->mixer_node, "so-name",
                               &so_name);
    if(ret) {
        fprintf(stderr, "%s: mixer so-name not found for card %u\n",
                __func__, card);
        goto err_get_lib_name;

    }

    dl_hdl = dlopen(so_name, RTLD_NOW);
    if (!dl_hdl) {
        fprintf(stderr, "%s: unable to open %s\n",
                __func__, so_name);
        goto err_dlopen;
    }

    dlerror();
    plug_data->ops = dlsym(dl_hdl, "mixer_plugin_ops");
    if (!plug_data->ops) {
        fprintf(stderr, "%s: dlsym open fn failed: %s\n",
                __func__, dlerror());
        goto err_ops;
    }

    ret = plug_data->ops->open(&plugin, card);
    if (ret) {
        fprintf(stderr, "%s: failed to open plugin, err: %d\n",
                __func__, ret);
        goto err_ops;
    }

    plug_data->plugin = plugin;
    plug_data->card = card;
    plug_data->dl_hdl = dl_hdl;
    plugin->eventfd = eventfd(0, 0);

    *data = plug_data;
    *ops = &mixer_plug_ops;

    return 0;

err_ops:
    dlclose(dl_hdl);
err_dlopen:
err_get_lib_name:
    snd_utils_close_dev_node(plug_data->mixer_node);
err_get_mixer_node:
    free(plug_data);
    return -1;
}
