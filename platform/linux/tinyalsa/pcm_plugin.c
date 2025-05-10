/* pcm_plugin.c
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

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <fcntl.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <poll.h>
#include <dlfcn.h>

#include <sys/ioctl.h>
#include <linux/ioctl.h>
#include <time.h>
#include <sound/asound.h>
#include <tinyalsa/asoundlib.h>
#include <tinyalsa/plugin.h>

#include "pcm_io.h"
#include "snd_card_plugin.h"

/* 2 words of uint32_t = 64 bits of mask */
#define PCM_MASK_SIZE (2)
#define ARRAY_SIZE(a)         \
    (sizeof(a) / sizeof(a[0]))

#define PCM_PARAM_GET_MASK(p, n)    \
    &p->masks[n - SNDRV_PCM_HW_PARAM_FIRST_MASK];

enum {
    PCM_PLUG_HW_PARAM_SELECT_MIN,
    PCM_PLUG_HW_PARAM_SELECT_MAX,
    PCM_PLUG_HW_PARAM_SELECT_VAL,
};

enum {
    PCM_PLUG_STATE_OPEN,
    PCM_PLUG_STATE_SETUP,
    PCM_PLUG_STATE_PREPARED,
    PCM_PLUG_STATE_RUNNING,
};

struct pcm_plug_data {
    unsigned int card;
    unsigned int device;
    unsigned int fd;
    unsigned int flags;

    void *dl_hdl;
    /** pointer to plugin operation */
    const struct pcm_plugin_ops *ops;
    struct pcm_plugin *plugin;
    void *dev_node;
};

static unsigned int param_list[] = {
    SNDRV_PCM_HW_PARAM_SAMPLE_BITS,
    SNDRV_PCM_HW_PARAM_CHANNELS,
    SNDRV_PCM_HW_PARAM_RATE,
    SNDRV_PCM_HW_PARAM_PERIOD_SIZE,
    SNDRV_PCM_HW_PARAM_PERIODS,
};

static int convert_plugin_to_pcm_state(int plugin_state)
{
    switch (plugin_state) {
    case PCM_PLUG_STATE_SETUP:
        return PCM_STATE_SETUP;
    case PCM_PLUG_STATE_RUNNING:
        return PCM_STATE_RUNNING;
    case PCM_PLUG_STATE_PREPARED:
        return PCM_STATE_PREPARED;
    case PCM_PLUG_STATE_OPEN:
        return PCM_STATE_OPEN;
    default:
        break;
    }

    return PCM_STATE_OPEN;
}

static void pcm_plug_close(void *data)
{
    struct pcm_plug_data *plug_data = data;
    struct pcm_plugin *plugin = plug_data->plugin;

    plug_data->ops->close(plugin);
    dlclose(plug_data->dl_hdl);

    free(plug_data);
}

static int pcm_plug_info(struct pcm_plug_data *plug_data,
                struct snd_pcm_info *info)
{
    int stream = SNDRV_PCM_STREAM_PLAYBACK;
    int ret = 0, val = -1;
    char *name;

    memset(info, 0, sizeof(*info));

    if (plug_data->flags & PCM_IN) {
        stream = SNDRV_PCM_STREAM_CAPTURE;
        ret = snd_utils_get_int(plug_data->dev_node, "capture", &val);
        if (ret || !val) {
            fprintf(stderr, "%s: not a capture device\n", __func__);
            return -EINVAL;
        }
    } else {
        stream = SNDRV_PCM_STREAM_PLAYBACK;
        ret = snd_utils_get_int(plug_data->dev_node, "playback", &val);
        if (ret || !val) {
            fprintf(stderr, "%s: not a playback device\n", __func__);
            return -EINVAL;
        }
    }

    info->stream = stream;
    info->card = plug_data->card;
    info->device = plug_data->device;

    ret = snd_utils_get_str(plug_data->dev_node, "name", &name);
    if (ret) {
        fprintf(stderr, "%s: failed to get pcm device name\n", __func__);
        return ret;
    }

    strncpy((char *)info->id, name, sizeof(info->id) - 1);
    ((char *)info->id)[sizeof(info->id) - 1] = '\0';
    strncpy((char *)info->name, name, sizeof(info->name) - 1);
    ((char *)info->name)[sizeof(info->name) - 1] = '\0';
    strncpy((char *)info->subname, name, sizeof(info->subname) - 1);
    ((char *)info->subname)[sizeof(info->subname) - 1] = '\0';

    info->subdevices_count = 1;

    return ret;
}

static void pcm_plug_set_mask(struct snd_pcm_hw_params *p, int n, uint64_t v)
{
    struct snd_mask *mask;

    mask = PCM_PARAM_GET_MASK(p, n);

    mask->bits[0] |= (v & 0xFFFFFFFF);
    mask->bits[1] |= ((v >> 32) & 0xFFFFFFFF);
    /*
     * currently only supporting 64 bits, may need to update to support
     * more than 64 bits
     */
}

static void pcm_plug_set_interval(struct snd_pcm_hw_params *params,
                    int p, struct pcm_plugin_min_max *v, int is_integer)
{
    struct snd_interval *i;

    i = &params->intervals[p - SNDRV_PCM_HW_PARAM_FIRST_INTERVAL];

    i->min = v->min;
    i->max = v->max;

    if (is_integer)
        i->integer = 1;
}

static int pcm_plug_frames_to_bytes(unsigned int frames,
                                    unsigned int frame_bits)
{
    return (frames * (frame_bits / 8));
}

static int pcm_plug_bytes_to_frames(unsigned int size,
                                    unsigned int frame_bits)
{
    return (size * 8)  / frame_bits;
}

static int pcm_plug_get_params(struct pcm_plugin *plugin,
                struct snd_pcm_hw_params *params)
{
    struct pcm_plugin_min_max bw, ch, pb, periods;
    struct pcm_plugin_min_max val;
    struct pcm_plugin_min_max frame_bits, buffer_bytes;

    /*
     * populate the struct snd_pcm_hw_params structure
     * using the hw_param constraints provided by plugin
     * via the plugin->constraints
     */

    /* Set the mask params */
    pcm_plug_set_mask(params, SNDRV_PCM_HW_PARAM_ACCESS,
                      plugin->constraints->access);
    pcm_plug_set_mask(params, SNDRV_PCM_HW_PARAM_FORMAT,
                      plugin->constraints->format);
    pcm_plug_set_mask(params, SNDRV_PCM_HW_PARAM_SUBFORMAT,
                      SNDRV_PCM_SUBFORMAT_STD);

    /* Set the standard interval params */
    pcm_plug_set_interval(params, SNDRV_PCM_HW_PARAM_SAMPLE_BITS,
                          &plugin->constraints->bit_width, 1);
    pcm_plug_set_interval(params, SNDRV_PCM_HW_PARAM_CHANNELS,
                          &plugin->constraints->channels, 1);
    pcm_plug_set_interval(params, SNDRV_PCM_HW_PARAM_RATE,
                          &plugin->constraints->rate, 1);
    pcm_plug_set_interval(params, SNDRV_PCM_HW_PARAM_PERIOD_BYTES,
                          &plugin->constraints->period_bytes, 0);
    pcm_plug_set_interval(params, SNDRV_PCM_HW_PARAM_PERIODS,
                          &plugin->constraints->periods, 1);

    /* set the calculated interval params */

    bw.min = plugin->constraints->bit_width.min;
    bw.max = plugin->constraints->bit_width.max;

    ch.min = plugin->constraints->channels.min;
    ch.max = plugin->constraints->channels.max;

    pb.min = plugin->constraints->period_bytes.min;
    pb.max = plugin->constraints->period_bytes.max;

    periods.min = plugin->constraints->periods.min;
    periods.max = plugin->constraints->periods.max;

    /* Calculate and set frame bits */
    frame_bits.min = bw.min * ch.min;
    frame_bits.max = bw.max * ch.max;
    pcm_plug_set_interval(params, SNDRV_PCM_HW_PARAM_FRAME_BITS,
                          &frame_bits, 1);


    /* Calculate and set period_size in frames */
    val.min = pcm_plug_bytes_to_frames(pb.min, frame_bits.min);
    val.max = pcm_plug_bytes_to_frames(pb.max, frame_bits.min);
    pcm_plug_set_interval(params, SNDRV_PCM_HW_PARAM_PERIOD_SIZE,
                          &val, 1);

    /* Calculate and set buffer_bytes */
    buffer_bytes.min = pb.min * periods.min;
    buffer_bytes.max = pb.max * periods.max;
    pcm_plug_set_interval(params, SNDRV_PCM_HW_PARAM_BUFFER_BYTES,
                          &buffer_bytes, 1);

    /* Calculate and set buffer_size in frames */
    val.min = pcm_plug_bytes_to_frames(buffer_bytes.min, frame_bits.min);
    val.max = pcm_plug_bytes_to_frames(buffer_bytes.max, frame_bits.min);
    pcm_plug_set_interval(params, SNDRV_PCM_HW_PARAM_BUFFER_SIZE,
                          &val, 1);
    return 0;
}

static int pcm_plug_masks_refine(struct snd_pcm_hw_params *p,
                struct snd_pcm_hw_params *c)
{
    struct snd_mask *req_mask;
    struct snd_mask *con_mask;
    unsigned int idx, i, masks;

    masks = SNDRV_PCM_HW_PARAM_LAST_MASK - SNDRV_PCM_HW_PARAM_FIRST_MASK;

    for (idx = 0; idx <= masks; idx++) {

        if (!(p->rmask & (1 << (idx + SNDRV_PCM_HW_PARAM_FIRST_MASK))))
            continue;

        req_mask = PCM_PARAM_GET_MASK(p, idx);
        con_mask = PCM_PARAM_GET_MASK(c, idx);

        /*
         * set the changed mask if requested mask value is not the same as
         * constrained mask value
         */
        if (memcmp(req_mask, con_mask, PCM_MASK_SIZE * sizeof(uint32_t)))
            p->cmask |= 1 << (idx + SNDRV_PCM_HW_PARAM_FIRST_MASK);

        /* Actually change the requested mask to constrained mask */
        for (i = 0; i < PCM_MASK_SIZE; i++)
            req_mask->bits[i] &= con_mask->bits[i];
    }

    return 0;
}

static int pcm_plug_interval_refine(struct snd_pcm_hw_params *p,
                struct snd_pcm_hw_params *c)
{
    struct snd_interval *ri;
    struct snd_interval *ci;
    unsigned int idx;
    unsigned int intervals;
    int changed = 0;

    intervals = SNDRV_PCM_HW_PARAM_LAST_INTERVAL -
                SNDRV_PCM_HW_PARAM_FIRST_INTERVAL;

    for (idx = 0; idx <= intervals; idx++) {
        ri = &p->intervals[idx];
        ci = &c->intervals[idx];

        if (!(p->rmask & (1 << (idx + SNDRV_PCM_HW_PARAM_FIRST_INTERVAL)) ))
            continue;

        if (ri->min < ci->min) {
            ri->min = ci->min;
            ri->openmin = ci->openmin;
            changed = 1;
        } else if (ri->min == ci->min && !ri->openmin && ci->openmin) {
            ri->openmin = 1;
            changed = 1;
        }

        if (ri->max > ci->max) {
            ri->max = ci->max;
            ri->openmax = ci->openmax;
            changed = 1;
        } else if (ri->max == ci->max && !ri->openmax && ci->openmax) {
            ri->openmax = 1;
            changed = 1;
        };

        if (!ri->integer && ci->integer) {
            ri->integer = 1;
            changed = 1;
        }

        if (ri->integer) {
            if (ri->openmin) {
                ri->min++;
                ri->openmin = 0;
            }
            if (ri->openmax) {
                ri->max--;
                ri->openmax = 0;
            }
        } else if (!ri->openmin && !ri->openmax && ri->min == ri->max) {
            ri->integer = 1;
        }

        /* Set the changed mask */
        if (changed)
            p->cmask |= (1 << (idx + SNDRV_PCM_HW_PARAM_FIRST_INTERVAL));
    }

    return 0;
}


static int pcm_plug_hw_params_refine(struct snd_pcm_hw_params *p,
                struct snd_pcm_hw_params *c)
{
    int rc;

    rc = pcm_plug_masks_refine(p, c);
    if (rc) {
        fprintf(stderr, "%s: masks refine failed %d\n", __func__, rc);
        return rc;
    }

    rc = pcm_plug_interval_refine(p, c);
    if (rc) {
        fprintf(stderr, "%s: interval refine failed %d\n", __func__, rc);
        return rc;
    }

    /* clear the requested params */
    p->rmask = 0;

    return rc;
}

static int __pcm_plug_hrefine(struct pcm_plug_data *plug_data,
                struct snd_pcm_hw_params *params)
{
    struct pcm_plugin *plugin = plug_data->plugin;
    struct snd_pcm_hw_params plug_params;
    int rc;

    memset(&plug_params, 0, sizeof(plug_params));
    rc = pcm_plug_get_params(plugin, &plug_params);
    if (rc) {
        fprintf(stderr, "%s: pcm_plug_get_params failed %d\n",
               __func__, rc);
        return -EINVAL;
    }

    return pcm_plug_hw_params_refine(params, &plug_params);

}

static int pcm_plug_hrefine(struct pcm_plug_data *plug_data,
                struct snd_pcm_hw_params *params)
{
    return __pcm_plug_hrefine(plug_data, params);
}

static int pcm_plug_interval_select(struct snd_pcm_hw_params *p,
        unsigned int param, unsigned int select, unsigned int val)
{
    struct snd_interval *i;

    if (param < SNDRV_PCM_HW_PARAM_FIRST_INTERVAL ||
        param > SNDRV_PCM_HW_PARAM_LAST_INTERVAL)
        return -EINVAL;

    i = &p->intervals[param - SNDRV_PCM_HW_PARAM_FIRST_INTERVAL];

    if (!i->min)
        return -EINVAL;

    switch (select) {

    case PCM_PLUG_HW_PARAM_SELECT_MIN:
        i->max = i->min;
        break;

    case PCM_PLUG_HW_PARAM_SELECT_MAX:
        i->min = i->max;
        break;

    case PCM_PLUG_HW_PARAM_SELECT_VAL:
        i->min = i->max = val;
        break;

    default:
        return -EINVAL;
    }

    return 0;
}

static void pcm_plug_hw_params_set(struct snd_pcm_hw_params *p)
{
    unsigned int i, select;
    unsigned int bw, ch, period_sz, periods;
    unsigned int val1, val2, offset;

    offset = SNDRV_PCM_HW_PARAM_FIRST_INTERVAL;

    /* Select the min values first */
    select = PCM_PLUG_HW_PARAM_SELECT_MIN;
    for (i = 0; i < ARRAY_SIZE(param_list); i++)
        pcm_plug_interval_select(p, param_list[i], select, 0);

    /* Select calculated values */
    select = PCM_PLUG_HW_PARAM_SELECT_VAL;
    bw = (p->intervals[SNDRV_PCM_HW_PARAM_SAMPLE_BITS - offset]).min;
    ch = (p->intervals[SNDRV_PCM_HW_PARAM_CHANNELS - offset]).min;
    period_sz = (p->intervals[SNDRV_PCM_HW_PARAM_PERIOD_SIZE - offset]).min;
    periods = (p->intervals[SNDRV_PCM_HW_PARAM_PERIODS - offset]).min;

    val1 = bw * ch;        // frame_bits;
    pcm_plug_interval_select(p, SNDRV_PCM_HW_PARAM_FRAME_BITS, select, val1);

    val2 = pcm_plug_frames_to_bytes(period_sz, val1); // period_bytes;
    pcm_plug_interval_select(p, SNDRV_PCM_HW_PARAM_PERIOD_BYTES, select,
                             val2);

    val2 = period_sz * periods; //buffer_size;
    pcm_plug_interval_select(p, SNDRV_PCM_HW_PARAM_BUFFER_SIZE, select, val2);

    val2 = pcm_plug_frames_to_bytes(period_sz * periods, val1); //buffer_bytes;
    pcm_plug_interval_select(p, SNDRV_PCM_HW_PARAM_BUFFER_BYTES, select, val2);
}

static int pcm_plug_hparams(struct pcm_plug_data *plug_data,
                struct snd_pcm_hw_params *params)
{
    struct pcm_plugin *plugin = plug_data->plugin;
    int rc;

    if (plugin->state != PCM_PLUG_STATE_OPEN)
            return -EBADFD;

    params->rmask = ~0U;

    rc = __pcm_plug_hrefine(plug_data, params);
    if (rc) {
        fprintf(stderr, "%s: __pcm_plug_hrefine failed %d\n",
               __func__, rc);
        return rc;
    }

    pcm_plug_hw_params_set(params);

    rc = plug_data->ops->hw_params(plugin, params);
    if (!rc)
        plugin->state = PCM_PLUG_STATE_SETUP;

    return rc;
}

static int pcm_plug_sparams(struct pcm_plug_data *plug_data,
                struct snd_pcm_sw_params *params)
{
    struct pcm_plugin *plugin = plug_data->plugin;

    if (plugin->state != PCM_PLUG_STATE_SETUP)
        return -EBADFD;

    return plug_data->ops->sw_params(plugin, params);
}

static int pcm_plug_sync_ptr(struct pcm_plug_data *plug_data,
                struct snd_pcm_sync_ptr *sync_ptr)
{
    struct pcm_plugin *plugin = plug_data->plugin;
    int ret = -EBADFD;

    if (plugin->state >= PCM_PLUG_STATE_SETUP) {
        ret = plug_data->ops->sync_ptr(plugin, sync_ptr);
        if (ret == 0)
            sync_ptr->s.status.state = convert_plugin_to_pcm_state(plugin->state);
    }

    return ret;
}

static int pcm_plug_writei_frames(struct pcm_plug_data *plug_data,
                struct snd_xferi *x)
{
    struct pcm_plugin *plugin = plug_data->plugin;

    if (plugin->state != PCM_PLUG_STATE_PREPARED &&
        plugin->state != PCM_PLUG_STATE_RUNNING)
        return -EBADFD;

    return plug_data->ops->writei_frames(plugin, x);
}

static int pcm_plug_readi_frames(struct pcm_plug_data *plug_data,
                struct snd_xferi *x)
{
    struct pcm_plugin *plugin = plug_data->plugin;

    if (plugin->state != PCM_PLUG_STATE_RUNNING)
        return -EBADFD;

    return plug_data->ops->readi_frames(plugin, x);
}

static int pcm_plug_ttstamp(struct pcm_plug_data *plug_data,
                int *tstamp)
{
    struct pcm_plugin *plugin = plug_data->plugin;

    if (plugin->state < PCM_PLUG_STATE_SETUP)
        return -EBADFD;

    return plug_data->ops->ttstamp(plugin, tstamp);
}

static int pcm_plug_prepare(struct pcm_plug_data *plug_data)
{
    struct pcm_plugin *plugin = plug_data->plugin;
    int rc;

    if (plugin->state != PCM_PLUG_STATE_SETUP)
        return -EBADFD;

    rc = plug_data->ops->prepare(plugin);
    if (!rc)
        plugin->state = PCM_PLUG_STATE_PREPARED;

    return rc;
}

static int pcm_plug_start(struct pcm_plug_data *plug_data)
{
    struct pcm_plugin *plugin = plug_data->plugin;
    int rc;

    if (plugin->state != PCM_PLUG_STATE_PREPARED)
        return -EBADFD;

    rc = plug_data->ops->start(plugin);
    if (!rc)
        plugin->state = PCM_PLUG_STATE_RUNNING;

    return rc;
}

static int pcm_plug_drop(struct pcm_plug_data *plug_data)
{
    struct pcm_plugin *plugin = plug_data->plugin;
    int rc;

    rc = plug_data->ops->drop(plugin);
    if (!rc)
        plugin->state = PCM_PLUG_STATE_SETUP;

    return rc;
}

static int pcm_plug_ioctl(void *data, unsigned int cmd, ...)
{
    struct pcm_plug_data *plug_data = data;
    struct pcm_plugin *plugin = plug_data->plugin;
    int ret;
    va_list ap;
    void *arg;

    va_start(ap, cmd);
    arg = va_arg(ap, void *);
    va_end(ap);

    switch (cmd) {
    case SNDRV_PCM_IOCTL_INFO:
        ret = pcm_plug_info(plug_data, arg);
        break;
    case SNDRV_PCM_IOCTL_TTSTAMP:
        ret = pcm_plug_ttstamp(plug_data, arg);
        break;
    case SNDRV_PCM_IOCTL_HW_REFINE:
        ret = pcm_plug_hrefine(plug_data, arg);
        break;
    case SNDRV_PCM_IOCTL_HW_PARAMS:
        ret = pcm_plug_hparams(plug_data, arg);
        break;
    case SNDRV_PCM_IOCTL_SW_PARAMS:
        ret = pcm_plug_sparams(plug_data, arg);
        break;
    case SNDRV_PCM_IOCTL_SYNC_PTR:
        ret = pcm_plug_sync_ptr(plug_data, arg);
        break;
    case SNDRV_PCM_IOCTL_PREPARE:
        ret = pcm_plug_prepare(plug_data);
        break;
    case SNDRV_PCM_IOCTL_START:
        ret = pcm_plug_start(plug_data);
        break;
    case SNDRV_PCM_IOCTL_DROP:
        ret = pcm_plug_drop(plug_data);
        break;
    case SNDRV_PCM_IOCTL_WRITEI_FRAMES:
        ret = pcm_plug_writei_frames(plug_data, arg);
        break;
    case SNDRV_PCM_IOCTL_READI_FRAMES:
        ret = pcm_plug_readi_frames(plug_data, arg);
        break;
    default:
        ret = plug_data->ops->ioctl(plugin, cmd, arg);
        break;
    }

    return ret;
}

static int pcm_plug_poll(void *data, struct pollfd *pfd, nfds_t nfds,
        int timeout)
{
    struct pcm_plug_data *plug_data = data;
    struct pcm_plugin *plugin = plug_data->plugin;

    return plug_data->ops->poll(plugin, pfd, nfds, timeout);
}

static void *pcm_plug_mmap(void *data, void *addr, size_t length, int prot,
                       int flags, off_t offset)
{
    struct pcm_plug_data *plug_data = data;
    struct pcm_plugin *plugin = plug_data->plugin;

    if (plugin->state != PCM_PLUG_STATE_SETUP)
        return NULL;

    return plug_data->ops->mmap(plugin, addr, length, prot, flags, offset);
}

static int pcm_plug_munmap(void *data, void *addr, size_t length)
{
    struct pcm_plug_data *plug_data = data;
    struct pcm_plugin *plugin = plug_data->plugin;

    if (plugin->state != PCM_PLUG_STATE_SETUP)
        return -EBADFD;

    return plug_data->ops->munmap(plugin, addr, length);
}

static int pcm_plug_open(unsigned int card, unsigned int device,
                         unsigned int flags, void **data, struct snd_node *pcm_node)
{
    struct pcm_plug_data *plug_data;
    void *dl_hdl;
    int rc = 0;
    char *so_name;

    plug_data = calloc(1, sizeof(*plug_data));
    if (!plug_data) {
        return -ENOMEM;
    }

    rc = snd_utils_get_str(pcm_node, "so-name", &so_name);
    if (rc) {
        fprintf(stderr, "%s: failed to get plugin lib name\n", __func__);
        goto err_get_lib;
    }

    dl_hdl = dlopen(so_name, RTLD_NOW);
    if (!dl_hdl) {
        fprintf(stderr, "%s: unable to open %s\n", __func__, so_name);
        goto err_dl_open;
    } else {
        fprintf(stderr, "%s: dlopen successful for %s\n", __func__, so_name);
    }

    dlerror();

    plug_data->ops = dlsym(dl_hdl, "pcm_plugin_ops");
    if (!plug_data->ops) {
        fprintf(stderr, "%s: dlsym to open fn failed, err = '%s'\n",
                __func__, dlerror());
        goto err_dlsym;
    }

    rc = plug_data->ops->open(&plug_data->plugin, card, device, flags);
    if (rc) {
        fprintf(stderr, "%s: failed to open plugin\n", __func__);
        goto err_open;
    }

    plug_data->dl_hdl = dl_hdl;
    plug_data->card = card;
    plug_data->device = device;
    plug_data->dev_node = pcm_node;
    plug_data->flags = flags;

    *data = plug_data;

    plug_data->plugin->state = PCM_PLUG_STATE_OPEN;

    return 0;

err_open:
err_dlsym:
    dlclose(dl_hdl);
err_get_lib:
err_dl_open:
    free(plug_data);

    return rc;
}

const struct pcm_ops plug_ops = {
    .open = pcm_plug_open,
    .close = pcm_plug_close,
    .ioctl = pcm_plug_ioctl,
    .mmap = pcm_plug_mmap,
    .munmap = pcm_plug_munmap,
    .poll = pcm_plug_poll,
};
