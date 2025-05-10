/* plugin.h
** Copyright (c) 2019-2020, The Linux Foundation.
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

#ifndef TINYALSA_PLUGIN_H
#define TINYALSA_PLUGIN_H

#include <poll.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/types.h>
#include <time.h>

#include <sound/asound.h>

/* static initializers */

#define SND_VALUE_ENUM(etexts, eitems)    \
    {.texts = etexts, .items = eitems}

#define SND_VALUE_BYTES(csize)    \
    {.size = csize }

#define SND_VALUE_INTEGER(icount, imin, imax, istep) \
    {.count = icount, .min = imin, .max = imax, .step = istep }

#define SND_VALUE_TLV_BYTES(csize, cget, cput)       \
    {.size = csize, .get = cget, .put = cput }

/* pointer based initializers */
#define INIT_SND_CONTROL_INTEGER(c, cname, cget, cput, cint, pval, pdata)   \
    {                                                                       \
        c->iface = SNDRV_CTL_ELEM_IFACE_MIXER;                              \
        c->access = SNDRV_CTL_ELEM_ACCESS_READWRITE;                        \
        c->type = SNDRV_CTL_ELEM_TYPE_INTEGER;                              \
        c->name = cname; c->value = &cint; c->get = cget; c->put = cput;    \
        c->private_value = pval; c->private_data = pdata;                   \
    }

#define INIT_SND_CONTROL_BYTES(c, cname, cget, cput, cint, pval, pdata)     \
    {                                                                       \
        c->iface = SNDRV_CTL_ELEM_IFACE_MIXER;                              \
        c->access = SNDRV_CTL_ELEM_ACCESS_READWRITE;                        \
        c->type = SNDRV_CTL_ELEM_TYPE_BYTES;                                \
        c->name = cname; c->value = &cint; c->get = cget; c->put = cput;    \
        c->private_value = pval; c->private_data = pdata;                   \
    }

#define INIT_SND_CONTROL_ENUM(c, cname, cget, cput, cenum, pval, pdata)     \
    {                                                                       \
        c->iface = SNDRV_CTL_ELEM_IFACE_MIXER;                              \
        c->access = SNDRV_CTL_ELEM_ACCESS_READWRITE;                        \
        c->type = SNDRV_CTL_ELEM_TYPE_ENUMERATED;                           \
        c->name = cname; c->value = cenum; c->get = cget; c->put = cput;    \
        c->private_value = pval; c->private_data = pdata;                   \
    }

#define INIT_SND_CONTROL_TLV_BYTES(c, cname, cbytes, priv_val, priv_data)  \
    {                                                                      \
        c->iface = SNDRV_CTL_ELEM_IFACE_MIXER;                             \
        c->access = SNDRV_CTL_ELEM_ACCESS_TLV_READWRITE;                   \
        c->type = SNDRV_CTL_ELEM_TYPE_BYTES;                               \
        c->name = cname; c->value = &cbytes;                               \
        c->private_value = priv_val; c->private_data = priv_data;          \
    }

struct mixer_plugin;
struct pcm_plugin;
struct snd_node;

/** Operations that are required to be registered by the plugin.
 * @ingroup libtinyalsa-pcm
 */
struct pcm_plugin_ops {
    /** open the pcm plugin */
    int (*open) (struct pcm_plugin **plugin, unsigned int card,
                 unsigned int device, unsigned int flags);
    /** close the pcm plugin */
    int (*close) (struct pcm_plugin *plugin);
    /** Set the PCM hardware parameters to the plugin */
    int (*hw_params) (struct pcm_plugin *plugin,
                      struct snd_pcm_hw_params *params);
    /** Set the PCM software parameters to the plugin */
    int (*sw_params) (struct pcm_plugin *plugin,
                      struct snd_pcm_sw_params *params);
    /** Synchronize the pointer */
    int (*sync_ptr) (struct pcm_plugin *plugin,
                     struct snd_pcm_sync_ptr *sync_ptr);
    /** Write frames to plugin to be rendered to output */
    int (*writei_frames) (struct pcm_plugin *plugin,
                          struct snd_xferi *x);
    /** Read frames from plugin captured from input */
    int (*readi_frames) (struct pcm_plugin *plugin,
                         struct snd_xferi *x);
    /** Obtain the timestamp for the PCM */
    int (*ttstamp) (struct pcm_plugin *plugin,
                    int *tstamp);
    /** Prepare the plugin for data transfer */
    int (*prepare) (struct pcm_plugin *plugin);
    /** Start data transfer from/to the plugin */
    int (*start) (struct pcm_plugin *plugin);
    /** Drop pcm frames */
    int (*drop) (struct pcm_plugin *plugin);
    /** Any custom or alsa specific ioctl implementation */
    int (*ioctl) (struct pcm_plugin *plugin,
                  int cmd, void *arg);
    void *(*mmap) (struct pcm_plugin *plugin, void *addr, size_t length,
                   int prot, int flags, off_t offset);
    int (*munmap) (struct pcm_plugin *plugin, void *addr, size_t length);
    int (*poll) (struct pcm_plugin *plugin, struct pollfd *pfd, nfds_t nfds,
                 int timeout);
};

/** Minimum and maximum values for hardware parameter constraints.
 * @ingroup libtinyalsa-pcm
 */
struct pcm_plugin_min_max {
    /** Minimum value for the hardware parameter */
    unsigned int min;
    /** Maximum value for the hardware parameter */
    unsigned int max;
};

/** Encapsulate the hardware parameter constraints
 * @ingroup libtinyalsa-pcm
 */
struct pcm_plugin_hw_constraints {
    /** Value for SNDRV_PCM_HW_PARAM_ACCESS param */
    uint64_t access;
    /** Value for SNDRV_PCM_HW_PARAM_FORMAT param.
     * As of this implementation ALSA supports 52 formats */
    uint64_t format;
    /** Value for SNDRV_PCM_HW_PARAM_SAMPLE_BITS param */
    struct pcm_plugin_min_max bit_width;
    /** Value for SNDRV_PCM_HW_PARAM_CHANNELS param */
    struct pcm_plugin_min_max channels;
    /** Value for SNDRV_PCM_HW_PARAM_RATE param */
    struct pcm_plugin_min_max rate;
    /** Value for SNDRV_PCM_HW_PARAM_PERIODS param */
    struct pcm_plugin_min_max periods;
    /** Value for SNDRV_PCM_HW_PARAM_PERIOD_BYTES param */
    struct pcm_plugin_min_max period_bytes;
};

struct pcm_plugin {
    /** Card number for the pcm device */
    unsigned int card;
    /** device number for the pcm device */
    unsigned int device;
    /** pointer to the contraints registered by the plugin */
    struct pcm_plugin_hw_constraints *constraints;
    /** Indicates read/write mode, etc.. */
    int mode;
    /* Pointer to hold the plugin's private data */
    void *priv;
    /* Tracks the plugin state */
    unsigned int state;
};

typedef void (*mixer_event_callback)(struct mixer_plugin *);

struct mixer_plugin_ops {
    int (*open) (struct mixer_plugin **plugin, unsigned int card);
    void (*close) (struct mixer_plugin **plugin);
    int (*subscribe_events) (struct mixer_plugin *plugin,
                             mixer_event_callback event_cb);
    ssize_t (*read_event) (struct mixer_plugin *plugin,
                           struct snd_ctl_event *ev, size_t size);
};

struct snd_control {
    snd_ctl_elem_iface_t iface;
    unsigned int access;
    const char *name;
    snd_ctl_elem_type_t type;
    void *value;
    int (*get) (struct mixer_plugin *plugin,
                struct snd_control *control,
                struct snd_ctl_elem_value *ev);
    int (*put) (struct mixer_plugin *plugin,
                struct snd_control *control,
                struct snd_ctl_elem_value *ev);
    uint32_t private_value;
    void *private_data;
};

struct mixer_plugin {
    unsigned int card;
    void *priv;

    int eventfd;
    int subscribed;
    int event_cnt;

    struct snd_control *controls;
    unsigned int num_controls;
};

struct snd_value_enum {
    unsigned int items;
    char **texts;
};

struct snd_value_bytes {
    unsigned int size;
};

struct snd_value_tlv_bytes {
    unsigned int size;
    int (*get) (struct mixer_plugin *plugin,
                struct snd_control *control,
                struct snd_ctl_tlv *tlv);
    int (*put) (struct mixer_plugin *plugin,
                struct snd_control *control,
                struct snd_ctl_tlv *tlv);
};

struct snd_value_int {
    unsigned int count;
    int min;
    int max;
    int step;
};

/** Operations defined by the plugin.
 * */
struct snd_node_ops {
    /** Function pointer to get card definition */
    void* (*open_card)(unsigned int card);
    /** Function pointer to release card definition */
    void (*close_card)(void *card);
    /** Get interger type properties from device definition */
    int (*get_int)(void *node, const char *prop, int *val);
    /** Get string type properties from device definition */
    int (*get_str)(void *node, const char *prop, char **val);
    /** Function pointer to get mixer definition */
    void* (*get_mixer)(void *card);
    /** Function pointer to get PCM definition */
    void* (*get_pcm)(void *card, unsigned int id);
    /** Reserved for other nodes such as compress */
    void* reserved[4];
};

#endif /* end of TINYALSA_PLUGIN_H */
