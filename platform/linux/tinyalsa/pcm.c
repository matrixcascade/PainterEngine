/* pcm.c
**
** Copyright 2011, The Android Open Source Project
**
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are met:
**     * Redistributions of source code must retain the above copyright
**       notice, this list of conditions and the following disclaimer.
**     * Redistributions in binary form must reproduce the above copyright
**       notice, this list of conditions and the following disclaimer in the
**       documentation and/or other materials provided with the distribution.
**     * Neither the name of The Android Open Source Project nor the names of
**       its contributors may be used to endorse or promote products derived
**       from this software without specific prior written permission.
**
** THIS SOFTWARE IS PROVIDED BY The Android Open Source Project ``AS IS'' AND
** ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
** IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
** ARE DISCLAIMED. IN NO EVENT SHALL The Android Open Source Project BE LIABLE
** FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
** DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
** SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
** CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
** LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
** OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
** DAMAGE.
*/

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <poll.h>

#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <time.h>
#include <limits.h>

#include <linux/ioctl.h>

#ifndef __force
#define __force
#endif

#ifndef __bitwise
#define __bitwise
#endif

#ifndef __user
#define __user
#endif

#include <sound/asound.h>

#include <tinyalsa/pcm.h>
#include <tinyalsa/limits.h>
#include "pcm_io.h"
#include "snd_card_plugin.h"

#ifndef PARAM_MAX
#define PARAM_MAX SNDRV_PCM_HW_PARAM_LAST_INTERVAL
#endif /* PARAM_MAX */

#ifndef SNDRV_PCM_HW_PARAMS_NO_PERIOD_WAKEUP
#define SNDRV_PCM_HW_PARAMS_NO_PERIOD_WAKEUP (1<<2)
#endif /* SNDRV_PCM_HW_PARAMS_NO_PERIOD_WAKEUP */

/* Logs information into a string; follows snprintf() in that
 * offset may be greater than size, and though no characters are copied
 * into string, characters are still counted into offset. */
#define STRLOG(string, offset, size, ...) \
    do { int temp, clipoffset = offset > size ? size : offset; \
         temp = snprintf(string + clipoffset, size - clipoffset, __VA_ARGS__); \
         if (temp > 0) offset += temp; } while (0)

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))
#endif

/* refer to SNDRV_PCM_ACCESS_##index in sound/asound.h. */
static const char * const access_lookup[] = {
        "MMAP_INTERLEAVED",
        "MMAP_NONINTERLEAVED",
        "MMAP_COMPLEX",
        "RW_INTERLEAVED",
        "RW_NONINTERLEAVED",
};

/* refer to SNDRV_PCM_FORMAT_##index in sound/asound.h. */
static const char * const format_lookup[] = {
        /*[0] =*/ "S8",
        "U8",
        "S16_LE",
        "S16_BE",
        "U16_LE",
        "U16_BE",
        "S24_LE",
        "S24_BE",
        "U24_LE",
        "U24_BE",
        "S32_LE",
        "S32_BE",
        "U32_LE",
        "U32_BE",
        "FLOAT_LE",
        "FLOAT_BE",
        "FLOAT64_LE",
        "FLOAT64_BE",
        "IEC958_SUBFRAME_LE",
        "IEC958_SUBFRAME_BE",
        "MU_LAW",
        "A_LAW",
        "IMA_ADPCM",
        "MPEG",
        /*[24] =*/ "GSM",
        /* gap */
        [31] = "SPECIAL",
        "S24_3LE",
        "S24_3BE",
        "U24_3LE",
        "U24_3BE",
        "S20_3LE",
        "S20_3BE",
        "U20_3LE",
        "U20_3BE",
        "S18_3LE",
        "S18_3BE",
        "U18_3LE",
        /*[43] =*/ "U18_3BE",
#if 0
        /* recent additions, may not be present on local asound.h */
        "G723_24",
        "G723_24_1B",
        "G723_40",
        "G723_40_1B",
        "DSD_U8",
        "DSD_U16_LE",
#endif
};

/* refer to SNDRV_PCM_SUBFORMAT_##index in sound/asound.h. */
static const char * const subformat_lookup[] = {
        "STD",
};

static inline int param_is_mask(int p)
{
    return (p >= SNDRV_PCM_HW_PARAM_FIRST_MASK) &&
        (p <= SNDRV_PCM_HW_PARAM_LAST_MASK);
}

static inline int param_is_interval(int p)
{
    return (p >= SNDRV_PCM_HW_PARAM_FIRST_INTERVAL) &&
        (p <= SNDRV_PCM_HW_PARAM_LAST_INTERVAL);
}

static inline const struct snd_interval *param_get_interval(const struct snd_pcm_hw_params *p, int n)
{
    return &(p->intervals[n - SNDRV_PCM_HW_PARAM_FIRST_INTERVAL]);
}

static inline struct snd_interval *param_to_interval(struct snd_pcm_hw_params *p, int n)
{
    return &(p->intervals[n - SNDRV_PCM_HW_PARAM_FIRST_INTERVAL]);
}

static inline struct snd_mask *param_to_mask(struct snd_pcm_hw_params *p, int n)
{
    return &(p->masks[n - SNDRV_PCM_HW_PARAM_FIRST_MASK]);
}

static void param_set_mask(struct snd_pcm_hw_params *p, int n, unsigned int bit)
{
    if (bit >= SNDRV_MASK_MAX)
        return;
    if (param_is_mask(n)) {
        struct snd_mask *m = param_to_mask(p, n);
        m->bits[0] = 0;
        m->bits[1] = 0;
        m->bits[bit >> 5] |= (1 << (bit & 31));
    }
}

static void param_set_min(struct snd_pcm_hw_params *p, int n, unsigned int val)
{
    if (param_is_interval(n)) {
        struct snd_interval *i = param_to_interval(p, n);
        i->min = val;
    }
}

static unsigned int param_get_min(const struct snd_pcm_hw_params *p, int n)
{
    if (param_is_interval(n)) {
        const struct snd_interval *i = param_get_interval(p, n);
        return i->min;
    }
    return 0;
}

static unsigned int param_get_max(const struct snd_pcm_hw_params *p, int n)
{
    if (param_is_interval(n)) {
        const struct snd_interval *i = param_get_interval(p, n);
        return i->max;
    }
    return 0;
}

static void param_set_int(struct snd_pcm_hw_params *p, int n, unsigned int val)
{
    if (param_is_interval(n)) {
        struct snd_interval *i = param_to_interval(p, n);
        i->min = val;
        i->max = val;
        i->integer = 1;
    }
}

static unsigned int param_get_int(struct snd_pcm_hw_params *p, int n)
{
    if (param_is_interval(n)) {
        struct snd_interval *i = param_to_interval(p, n);
        if (i->integer)
            return i->max;
    }
    return 0;
}

static void param_init(struct snd_pcm_hw_params *p)
{
    int n;

    memset(p, 0, sizeof(*p));
    for (n = SNDRV_PCM_HW_PARAM_FIRST_MASK;
         n <= SNDRV_PCM_HW_PARAM_LAST_MASK; n++) {
            struct snd_mask *m = param_to_mask(p, n);
            m->bits[0] = ~0;
            m->bits[1] = ~0;
    }
    for (n = SNDRV_PCM_HW_PARAM_FIRST_INTERVAL;
         n <= SNDRV_PCM_HW_PARAM_LAST_INTERVAL; n++) {
            struct snd_interval *i = param_to_interval(p, n);
            i->min = 0;
            i->max = ~0;
    }
    p->rmask = ~0U;
    p->cmask = 0;
    p->info = ~0U;
}

static unsigned int pcm_format_to_alsa(enum pcm_format format)
{
    switch (format) {

    case PCM_FORMAT_S8:
        return SNDRV_PCM_FORMAT_S8;

    default:
    case PCM_FORMAT_S16_LE:
        return SNDRV_PCM_FORMAT_S16_LE;
    case PCM_FORMAT_S16_BE:
        return SNDRV_PCM_FORMAT_S16_BE;

    case PCM_FORMAT_S24_LE:
        return SNDRV_PCM_FORMAT_S24_LE;
    case PCM_FORMAT_S24_BE:
        return SNDRV_PCM_FORMAT_S24_BE;

    case PCM_FORMAT_S24_3LE:
        return SNDRV_PCM_FORMAT_S24_3LE;
    case PCM_FORMAT_S24_3BE:
        return SNDRV_PCM_FORMAT_S24_3BE;

    case PCM_FORMAT_S32_LE:
        return SNDRV_PCM_FORMAT_S32_LE;
    case PCM_FORMAT_S32_BE:
        return SNDRV_PCM_FORMAT_S32_BE;

    case PCM_FORMAT_FLOAT_LE:
        return SNDRV_PCM_FORMAT_FLOAT_LE;
    case PCM_FORMAT_FLOAT_BE:
        return SNDRV_PCM_FORMAT_FLOAT_BE;
    };
}

#define PCM_ERROR_MAX 128

/** A PCM handle.
 * @ingroup libtinyalsa-pcm
 */
struct pcm {
    /** The PCM's file descriptor */
    int fd;
    /** Flags that were passed to @ref pcm_open */
    unsigned int flags;
    /** The number of (under/over)runs that have occured */
    int xruns;
    /** Size of the buffer */
    unsigned int buffer_size;
    /** The boundary for ring buffer pointers */
    unsigned long boundary;
    /** Description of the last error that occured */
    char error[PCM_ERROR_MAX];
    /** Configuration that was passed to @ref pcm_open */
    struct pcm_config config;
    struct snd_pcm_mmap_status *mmap_status;
    struct snd_pcm_mmap_control *mmap_control;
    struct snd_pcm_sync_ptr *sync_ptr;
    void *mmap_buffer;
    unsigned int noirq_frames_per_msec;
    /** The delay of the PCM, in terms of frames */
    long pcm_delay;
    /** The subdevice corresponding to the PCM */
    unsigned int subdevice;
    /** Pointer to the pcm ops, either hw or plugin */
    const struct pcm_ops *ops;
    /** Private data for pcm_hw or pcm_plugin */
    void *data;
    /** Pointer to the pcm node from snd card definition */
    struct snd_node *snd_node;
};

static int oops(struct pcm *pcm, int e, const char *fmt, ...)
{
    va_list ap;
    int sz;

    va_start(ap, fmt);
    vsnprintf(pcm->error, PCM_ERROR_MAX, fmt, ap);
    va_end(ap);
    sz = strlen(pcm->error);

    if (e)
        snprintf(pcm->error + sz, PCM_ERROR_MAX - sz,
                 ": %s", strerror(e));
    return -1;
}

/** Gets the buffer size of the PCM.
 * @param pcm A PCM handle.
 * @return The buffer size of the PCM.
 * @ingroup libtinyalsa-pcm
 */
unsigned int pcm_get_buffer_size(const struct pcm *pcm)
{
    return pcm->buffer_size;
}

/** Gets the channel count of the PCM.
 * @param pcm A PCM handle.
 * @return The channel count of the PCM.
 * @ingroup libtinyalsa-pcm
 */
unsigned int pcm_get_channels(const struct pcm *pcm)
{
    return pcm->config.channels;
}

/** Gets the PCM configuration.
 * @param pcm A PCM handle.
 * @return The PCM configuration.
 *  This function only returns NULL if
 *  @p pcm is NULL.
 * @ingroup libtinyalsa-pcm
 * */
const struct pcm_config * pcm_get_config(const struct pcm *pcm)
{
    if (pcm == NULL)
        return NULL;
    return &pcm->config;
}

/** Gets the rate of the PCM.
 * The rate is given in frames per second.
 * @param pcm A PCM handle.
 * @return The rate of the PCM.
 * @ingroup libtinyalsa-pcm
 */
unsigned int pcm_get_rate(const struct pcm *pcm)
{
    return pcm->config.rate;
}

/** Gets the format of the PCM.
 * @param pcm A PCM handle.
 * @return The format of the PCM.
 * @ingroup libtinyalsa-pcm
 */
enum pcm_format pcm_get_format(const struct pcm *pcm)
{
    return pcm->config.format;
}

/** Gets the file descriptor of the PCM.
 * Useful for extending functionality of the PCM when needed.
 * @param pcm A PCM handle.
 * @return The file descriptor of the PCM.
 * @ingroup libtinyalsa-pcm
 */
int pcm_get_file_descriptor(const struct pcm *pcm)
{
    return pcm->fd;
}

/** Gets the error message for the last error that occured.
 * If no error occured and this function is called, the results are undefined.
 * @param pcm A PCM handle.
 * @return The error message of the last error that occured.
 * @ingroup libtinyalsa-pcm
 */
const char* pcm_get_error(const struct pcm *pcm)
{
    return pcm->error;
}

/** Sets the PCM configuration.
 * @param pcm A PCM handle.
 * @param config The configuration to use for the
 *  PCM. This parameter may be NULL, in which case
 *  the default configuration is used.
 * @returns Zero on success, a negative errno value
 *  on failure.
 * @ingroup libtinyalsa-pcm
 * */
int pcm_set_config(struct pcm *pcm, const struct pcm_config *config)
{
    if (pcm == NULL)
        return -EFAULT;
    else if (config == NULL) {
        config = &pcm->config;
        pcm->config.channels = 2;
        pcm->config.rate = 48000;
        pcm->config.period_size = 1024;
        pcm->config.period_count = 4;
        pcm->config.format = PCM_FORMAT_S16_LE;
        pcm->config.start_threshold = config->period_count * config->period_size;
        pcm->config.stop_threshold = config->period_count * config->period_size;
        pcm->config.silence_threshold = 0;
        pcm->config.silence_size = 0;
    } else
        pcm->config = *config;

    struct snd_pcm_hw_params params;
    param_init(&params);
    param_set_mask(&params, SNDRV_PCM_HW_PARAM_FORMAT,
                   pcm_format_to_alsa(config->format));
    param_set_min(&params, SNDRV_PCM_HW_PARAM_PERIOD_SIZE, config->period_size);
    param_set_int(&params, SNDRV_PCM_HW_PARAM_CHANNELS,
                  config->channels);
    param_set_int(&params, SNDRV_PCM_HW_PARAM_PERIODS, config->period_count);
    param_set_int(&params, SNDRV_PCM_HW_PARAM_RATE, config->rate);

    if (pcm->flags & PCM_NOIRQ) {

        if (!(pcm->flags & PCM_MMAP)) {
            oops(pcm, EINVAL, "noirq only currently supported with mmap().");
            return -EINVAL;
        }

        params.flags |= SNDRV_PCM_HW_PARAMS_NO_PERIOD_WAKEUP;
        pcm->noirq_frames_per_msec = config->rate / 1000;
    }

    if (pcm->flags & PCM_MMAP)
        param_set_mask(&params, SNDRV_PCM_HW_PARAM_ACCESS,
                   SNDRV_PCM_ACCESS_MMAP_INTERLEAVED);
    else
        param_set_mask(&params, SNDRV_PCM_HW_PARAM_ACCESS,
                   SNDRV_PCM_ACCESS_RW_INTERLEAVED);

    if (pcm->ops->ioctl(pcm->data, SNDRV_PCM_IOCTL_HW_PARAMS, &params)) {
        int errno_copy = errno;
        oops(pcm, errno, "cannot set hw params");
        return -errno_copy;
    }

    /* get our refined hw_params */
    pcm->config.period_size = param_get_int(&params, SNDRV_PCM_HW_PARAM_PERIOD_SIZE);
    pcm->config.period_count = param_get_int(&params, SNDRV_PCM_HW_PARAM_PERIODS);
    pcm->buffer_size = config->period_count * config->period_size;

    if (pcm->flags & PCM_MMAP) {
        pcm->mmap_buffer = pcm->ops->mmap(pcm->data, NULL, pcm_frames_to_bytes(pcm, pcm->buffer_size),
                                PROT_READ | PROT_WRITE, MAP_SHARED, 0);
        if (pcm->mmap_buffer == MAP_FAILED) {
            int errno_copy = errno;
            oops(pcm, errno, "failed to mmap buffer %d bytes\n",
                 pcm_frames_to_bytes(pcm, pcm->buffer_size));
            return -errno_copy;
        }
    }

    struct snd_pcm_sw_params sparams;
    memset(&sparams, 0, sizeof(sparams));
    sparams.tstamp_mode = SNDRV_PCM_TSTAMP_ENABLE;
    sparams.period_step = 1;
    sparams.avail_min = config->period_size;

    if (!config->start_threshold) {
        if (pcm->flags & PCM_IN)
            pcm->config.start_threshold = sparams.start_threshold = 1;
        else
            pcm->config.start_threshold = sparams.start_threshold =
                config->period_count * config->period_size / 2;
    } else
        sparams.start_threshold = config->start_threshold;

    /* pick a high stop threshold - todo: does this need further tuning */
    if (!config->stop_threshold) {
        if (pcm->flags & PCM_IN)
            pcm->config.stop_threshold = sparams.stop_threshold =
                config->period_count * config->period_size * 10;
        else
            pcm->config.stop_threshold = sparams.stop_threshold =
                config->period_count * config->period_size;
    }
    else
        sparams.stop_threshold = config->stop_threshold;

    sparams.xfer_align = config->period_size / 2; /* needed for old kernels */
    sparams.silence_size = config->silence_size;
    sparams.silence_threshold = config->silence_threshold;

    if (pcm->ops->ioctl(pcm->data, SNDRV_PCM_IOCTL_SW_PARAMS, &sparams)) {
        int errno_copy = errno;
        oops(pcm, errno, "cannot set sw params");
        return -errno_copy;
    }

    pcm->boundary = sparams.boundary;
    return 0;
}

/** Gets the subdevice on which the pcm has been opened.
 * @param pcm A PCM handle.
 * @return The subdevice on which the pcm has been opened */
unsigned int pcm_get_subdevice(const struct pcm *pcm)
{
    return pcm->subdevice;
}

/** Determines the number of bits occupied by a @ref pcm_format.
 * @param format A PCM format.
 * @return The number of bits associated with @p format
 * @ingroup libtinyalsa-pcm
 */
unsigned int pcm_format_to_bits(enum pcm_format format)
{
    switch (format) {
    case PCM_FORMAT_S32_LE:
    case PCM_FORMAT_S32_BE:
    case PCM_FORMAT_S24_LE:
    case PCM_FORMAT_S24_BE:
    case PCM_FORMAT_FLOAT_LE:
    case PCM_FORMAT_FLOAT_BE:
        return 32;
    case PCM_FORMAT_S24_3LE:
    case PCM_FORMAT_S24_3BE:
        return 24;
    default:
    case PCM_FORMAT_S16_LE:
    case PCM_FORMAT_S16_BE:
        return 16;
    case PCM_FORMAT_S8:
        return 8;
    };
}

/** Determines how many frames of a PCM can fit into a number of bytes.
 * @param pcm A PCM handle.
 * @param bytes The number of bytes.
 * @return The number of frames that may fit into @p bytes
 * @ingroup libtinyalsa-pcm
 */
unsigned int pcm_bytes_to_frames(const struct pcm *pcm, unsigned int bytes)
{
    return bytes / (pcm->config.channels *
        (pcm_format_to_bits(pcm->config.format) >> 3));
}

/** Determines how many bytes are occupied by a number of frames of a PCM.
 * @param pcm A PCM handle.
 * @param frames The number of frames of a PCM.
 * @return The bytes occupied by @p frames.
 * @ingroup libtinyalsa-pcm
 */
unsigned int pcm_frames_to_bytes(const struct pcm *pcm, unsigned int frames)
{
    return frames * pcm->config.channels *
        (pcm_format_to_bits(pcm->config.format) >> 3);
}

static int pcm_sync_ptr(struct pcm *pcm, int flags)
{
    if (pcm->sync_ptr == NULL) {
        /* status and control are mmapped */
        if (flags & SNDRV_PCM_SYNC_PTR_HWSYNC) {
            if (pcm->ops->ioctl(pcm->data, SNDRV_PCM_IOCTL_HWSYNC) == -1) {
                return oops(pcm, errno, "failed to sync hardware pointer");
            }
        }
    } else {
        pcm->sync_ptr->flags = flags;
        if (pcm->ops->ioctl(pcm->data, SNDRV_PCM_IOCTL_SYNC_PTR,
                            pcm->sync_ptr) < 0) {
            return oops(pcm, errno, "failed to sync mmap ptr");
        }
    }

    return 0;
}

int pcm_state(struct pcm *pcm)
{
    // Update the state only. Do not sync HW sync.
    int err = pcm_sync_ptr(pcm, SNDRV_PCM_SYNC_PTR_APPL | SNDRV_PCM_SYNC_PTR_AVAIL_MIN);
    if (err < 0)
        return err;

    return pcm->mmap_status->state;
}

static int pcm_hw_mmap_status(struct pcm *pcm)
{
    if (pcm->sync_ptr)
        return 0;

    int page_size = sysconf(_SC_PAGE_SIZE);
    pcm->mmap_status = pcm->ops->mmap(pcm->data, NULL, page_size, PROT_READ, MAP_SHARED,
                            SNDRV_PCM_MMAP_OFFSET_STATUS);
    if (pcm->mmap_status == MAP_FAILED)
        pcm->mmap_status = NULL;
    if (!pcm->mmap_status)
        goto mmap_error;

    pcm->mmap_control = pcm->ops->mmap(pcm->data, NULL, page_size, PROT_READ | PROT_WRITE,
                             MAP_SHARED, SNDRV_PCM_MMAP_OFFSET_CONTROL);
    if (pcm->mmap_control == MAP_FAILED)
        pcm->mmap_control = NULL;
    if (!pcm->mmap_control) {
        pcm->ops->munmap(pcm->data, pcm->mmap_status, page_size);
        pcm->mmap_status = NULL;
        goto mmap_error;
    }

    return 0;

mmap_error:

    pcm->sync_ptr = calloc(1, sizeof(*pcm->sync_ptr));
    if (!pcm->sync_ptr)
        return -ENOMEM;
    pcm->mmap_status = &pcm->sync_ptr->s.status;
    pcm->mmap_control = &pcm->sync_ptr->c.control;

    return 0;
}

static void pcm_hw_munmap_status(struct pcm *pcm) {
    if (pcm->sync_ptr) {
        free(pcm->sync_ptr);
        pcm->sync_ptr = NULL;
    } else {
        int page_size = sysconf(_SC_PAGE_SIZE);
        if (pcm->mmap_status)
            pcm->ops->munmap(pcm->data, pcm->mmap_status, page_size);
        if (pcm->mmap_control)
            pcm->ops->munmap(pcm->data, pcm->mmap_control, page_size);
    }
    pcm->mmap_status = NULL;
    pcm->mmap_control = NULL;
}

static struct pcm bad_pcm = {
    .fd = -1,
};

/** Gets the hardware parameters of a PCM, without created a PCM handle.
 * @param card The card of the PCM.
 *  The default card is zero.
 * @param device The device of the PCM.
 *  The default device is zero.
 * @param flags Specifies whether the PCM is an input or output.
 *  May be one of the following:
 *   - @ref PCM_IN
 *   - @ref PCM_OUT
 * @return On success, the hardware parameters of the PCM; on failure, NULL.
 * @ingroup libtinyalsa-pcm
 */
struct pcm_params *pcm_params_get(unsigned int card, unsigned int device,
                                  unsigned int flags)
{
    struct snd_pcm_hw_params *params;
    void *snd_node = NULL, *data;
    const struct pcm_ops *ops;
    int fd;

    ops = &hw_ops;
    fd = ops->open(card, device, flags, &data, snd_node);

#ifdef TINYALSA_USES_PLUGINS
    if (fd < 0) {
        int pcm_type;
        snd_node = snd_utils_open_pcm(card, device);
        pcm_type = snd_utils_get_node_type(snd_node);
        if (!snd_node || pcm_type != SND_NODE_TYPE_PLUGIN) {
            fprintf(stderr, "no device (hw/plugin) for card(%u), device(%u)",
                 card, device);
            goto err_open;
        }
        ops = &plug_ops;
        fd = ops->open(card, device, flags, &data, snd_node);
    }
#endif
    if (fd < 0) {
        fprintf(stderr, "cannot open card(%d) device (%d): %s\n",
                card, device, strerror(errno));
        goto err_open;
    }

    params = calloc(1, sizeof(struct snd_pcm_hw_params));
    if (!params)
        goto err_calloc;

    param_init(params);
    if (ops->ioctl(data, SNDRV_PCM_IOCTL_HW_REFINE, params)) {
        fprintf(stderr, "SNDRV_PCM_IOCTL_HW_REFINE error (%d)\n", errno);
        goto err_hw_refine;
    }

#ifdef TINYALSA_USES_PLUGINS
    if (snd_node)
        snd_utils_close_dev_node(snd_node);
#endif
    ops->close(data);

    return (struct pcm_params *)params;

err_hw_refine:
    free(params);
err_calloc:
#ifdef TINYALSA_USES_PLUGINS
    if (snd_node)
        snd_utils_close_dev_node(snd_node);
#endif
    ops->close(data);
err_open:
    return NULL;
}

/** Frees the hardware parameters returned by @ref pcm_params_get.
 * @param pcm_params Hardware parameters of a PCM.
 *  May be NULL.
 * @ingroup libtinyalsa-pcm
 */
void pcm_params_free(struct pcm_params *pcm_params)
{
    struct snd_pcm_hw_params *params = (struct snd_pcm_hw_params *)pcm_params;

    if (params)
        free(params);
}

static int pcm_param_to_alsa(enum pcm_param param)
{
    switch (param) {
    case PCM_PARAM_ACCESS:
        return SNDRV_PCM_HW_PARAM_ACCESS;
    case PCM_PARAM_FORMAT:
        return SNDRV_PCM_HW_PARAM_FORMAT;
    case PCM_PARAM_SUBFORMAT:
        return SNDRV_PCM_HW_PARAM_SUBFORMAT;
    case PCM_PARAM_SAMPLE_BITS:
        return SNDRV_PCM_HW_PARAM_SAMPLE_BITS;
        break;
    case PCM_PARAM_FRAME_BITS:
        return SNDRV_PCM_HW_PARAM_FRAME_BITS;
        break;
    case PCM_PARAM_CHANNELS:
        return SNDRV_PCM_HW_PARAM_CHANNELS;
        break;
    case PCM_PARAM_RATE:
        return SNDRV_PCM_HW_PARAM_RATE;
        break;
    case PCM_PARAM_PERIOD_TIME:
        return SNDRV_PCM_HW_PARAM_PERIOD_TIME;
        break;
    case PCM_PARAM_PERIOD_SIZE:
        return SNDRV_PCM_HW_PARAM_PERIOD_SIZE;
        break;
    case PCM_PARAM_PERIOD_BYTES:
        return SNDRV_PCM_HW_PARAM_PERIOD_BYTES;
        break;
    case PCM_PARAM_PERIODS:
        return SNDRV_PCM_HW_PARAM_PERIODS;
        break;
    case PCM_PARAM_BUFFER_TIME:
        return SNDRV_PCM_HW_PARAM_BUFFER_TIME;
        break;
    case PCM_PARAM_BUFFER_SIZE:
        return SNDRV_PCM_HW_PARAM_BUFFER_SIZE;
        break;
    case PCM_PARAM_BUFFER_BYTES:
        return SNDRV_PCM_HW_PARAM_BUFFER_BYTES;
        break;
    case PCM_PARAM_TICK_TIME:
        return SNDRV_PCM_HW_PARAM_TICK_TIME;
        break;

    default:
        return -1;
    }
}

/** Gets a mask from a PCM's hardware parameters.
 * @param pcm_params A PCM's hardware parameters.
 * @param param The parameter to get.
 * @return If @p pcm_params is NULL or @p param is not a mask, NULL is returned.
 *  Otherwise, the mask associated with @p param is returned.
 * @ingroup libtinyalsa-pcm
 */
const struct pcm_mask *pcm_params_get_mask(const struct pcm_params *pcm_params,
                                     enum pcm_param param)
{
    int p;
    struct snd_pcm_hw_params *params = (struct snd_pcm_hw_params *)pcm_params;
    if (params == NULL) {
        return NULL;
    }

    p = pcm_param_to_alsa(param);
    if (p < 0 || !param_is_mask(p)) {
        return NULL;
    }

    return (const struct pcm_mask *)param_to_mask(params, p);
}

/** Get the minimum of a specified PCM parameter.
 * @param pcm_params A PCM parameters structure.
 * @param param The specified parameter to get the minimum of.
 * @returns On success, the parameter minimum.
 *  On failure, zero.
 */
unsigned int pcm_params_get_min(const struct pcm_params *pcm_params,
                                enum pcm_param param)
{
    struct snd_pcm_hw_params *params = (struct snd_pcm_hw_params *)pcm_params;
    int p;

    if (!params)
        return 0;

    p = pcm_param_to_alsa(param);
    if (p < 0)
        return 0;

    return param_get_min(params, p);
}

/** Get the maximum of a specified PCM parameter.
 * @param pcm_params A PCM parameters structure.
 * @param param The specified parameter to get the maximum of.
 * @returns On success, the parameter maximum.
 *  On failure, zero.
 */
unsigned int pcm_params_get_max(const struct pcm_params *pcm_params,
                                enum pcm_param param)
{
    const struct snd_pcm_hw_params *params = (const struct snd_pcm_hw_params *)pcm_params;
    int p;

    if (!params)
        return 0;

    p = pcm_param_to_alsa(param);
    if (p < 0)
        return 0;

    return param_get_max(params, p);
}

static int pcm_mask_test(const struct pcm_mask *m, unsigned int index)
{
    const unsigned int bitshift = 5; /* for 32 bit integer */
    const unsigned int bitmask = (1 << bitshift) - 1;
    unsigned int element;

    element = index >> bitshift;
    if (element >= ARRAY_SIZE(m->bits))
        return 0; /* for safety, but should never occur */
    return (m->bits[element] >> (index & bitmask)) & 1;
}

static int pcm_mask_to_string(const struct pcm_mask *m, char *string, unsigned int size,
                              char *mask_name,
                              const char * const *bit_array_name, size_t bit_array_size)
{
    unsigned int i;
    unsigned int offset = 0;

    if (m == NULL)
        return 0;
    if (bit_array_size < 32) {
        STRLOG(string, offset, size, "%12s:\t%#08x\n", mask_name, m->bits[0]);
    } else { /* spans two or more bitfields, print with an array index */
        for (i = 0; i < (bit_array_size + 31) >> 5; ++i) {
            STRLOG(string, offset, size, "%9s[%d]:\t%#08x\n",
                   mask_name, i, m->bits[i]);
        }
    }
    for (i = 0; i < bit_array_size; ++i) {
        if (pcm_mask_test(m, i)) {
            STRLOG(string, offset, size, "%12s \t%s\n", "", bit_array_name[i]);
        }
    }
    return offset;
}

int pcm_params_to_string(struct pcm_params *params, char *string, unsigned int size)
{
    const struct pcm_mask *m;
    unsigned int min, max;
    unsigned int clipoffset, offset;

    m = pcm_params_get_mask(params, PCM_PARAM_ACCESS);
    offset = pcm_mask_to_string(m, string, size,
                                 "Access", access_lookup, ARRAY_SIZE(access_lookup));
    m = pcm_params_get_mask(params, PCM_PARAM_FORMAT);
    clipoffset = offset > size ? size : offset;
    offset += pcm_mask_to_string(m, string + clipoffset, size - clipoffset,
                                 "Format", format_lookup, ARRAY_SIZE(format_lookup));
    m = pcm_params_get_mask(params, PCM_PARAM_SUBFORMAT);
    clipoffset = offset > size ? size : offset;
    offset += pcm_mask_to_string(m, string + clipoffset, size - clipoffset,
                                 "Subformat", subformat_lookup, ARRAY_SIZE(subformat_lookup));
    min = pcm_params_get_min(params, PCM_PARAM_RATE);
    max = pcm_params_get_max(params, PCM_PARAM_RATE);
    STRLOG(string, offset, size, "        Rate:\tmin=%uHz\tmax=%uHz\n", min, max);
    min = pcm_params_get_min(params, PCM_PARAM_CHANNELS);
    max = pcm_params_get_max(params, PCM_PARAM_CHANNELS);
    STRLOG(string, offset, size, "    Channels:\tmin=%u\t\tmax=%u\n", min, max);
    min = pcm_params_get_min(params, PCM_PARAM_SAMPLE_BITS);
    max = pcm_params_get_max(params, PCM_PARAM_SAMPLE_BITS);
    STRLOG(string, offset, size, " Sample bits:\tmin=%u\t\tmax=%u\n", min, max);
    min = pcm_params_get_min(params, PCM_PARAM_PERIOD_SIZE);
    max = pcm_params_get_max(params, PCM_PARAM_PERIOD_SIZE);
    STRLOG(string, offset, size, " Period size:\tmin=%u\t\tmax=%u\n", min, max);
    min = pcm_params_get_min(params, PCM_PARAM_PERIODS);
    max = pcm_params_get_max(params, PCM_PARAM_PERIODS);
    STRLOG(string, offset, size, "Period count:\tmin=%u\t\tmax=%u\n", min, max);
    return offset;
}

int pcm_params_format_test(struct pcm_params *params, enum pcm_format format)
{
    unsigned int alsa_format = pcm_format_to_alsa(format);

    if (alsa_format == SNDRV_PCM_FORMAT_S16_LE && format != PCM_FORMAT_S16_LE)
        return 0; /* caution: format not recognized is equivalent to S16_LE */
    return pcm_mask_test(pcm_params_get_mask(params, PCM_PARAM_FORMAT), alsa_format);
}

/** Closes a PCM returned by @ref pcm_open.
 * @param pcm A PCM returned by @ref pcm_open.
 *  May not be NULL.
 * @return Always returns zero.
 * @ingroup libtinyalsa-pcm
 */
int pcm_close(struct pcm *pcm)
{
    if (pcm == &bad_pcm)
        return 0;

    pcm_hw_munmap_status(pcm);

    if (pcm->flags & PCM_MMAP) {
        pcm_stop(pcm);
        pcm->ops->munmap(pcm->data, pcm->mmap_buffer, pcm_frames_to_bytes(pcm, pcm->buffer_size));
    }

    snd_utils_close_dev_node(pcm->snd_node);
    pcm->ops->close(pcm->data);
    pcm->buffer_size = 0;
    pcm->fd = -1;
    free(pcm);
    return 0;
}

/** Opens a PCM by it's name.
 * @param name The name of the PCM.
 *  The name is given in the format: <i>hw</i>:<b>card</b>,<b>device</b>
 * @param flags Specify characteristics and functionality about the pcm.
 *  May be a bitwise AND of the following:
 *   - @ref PCM_IN
 *   - @ref PCM_OUT
 *   - @ref PCM_MMAP
 *   - @ref PCM_NOIRQ
 *   - @ref PCM_MONOTONIC
 * @param config The hardware and software parameters to open the PCM with.
 * @returns A PCM structure.
 *  If an error occurs, the pointer of bad_pcm is returned.
 *  Otherwise, it returns the pointer of PCM object.
 *  Client code should check that the PCM opened properly by calling @ref pcm_is_ready.
 *  If @ref pcm_is_ready returns false, check @ref pcm_get_error for more information.
 * @ingroup libtinyalsa-pcm
 */
struct pcm *pcm_open_by_name(const char *name,
                             unsigned int flags,
                             const struct pcm_config *config)
{
    unsigned int card, device;
    if (name[0] != 'h' || name[1] != 'w' || name[2] != ':') {
        oops(&bad_pcm, 0, "name format is not matched");
        return &bad_pcm;
    } else if (sscanf(&name[3], "%u,%u", &card, &device) != 2) {
        oops(&bad_pcm, 0, "name format is not matched");
        return &bad_pcm;
    }
    return pcm_open(card, device, flags, config);
}

/** Opens a PCM.
 * @param card The card that the pcm belongs to.
 *  The default card is zero.
 * @param device The device that the pcm belongs to.
 *  The default device is zero.
 * @param flags Specify characteristics and functionality about the pcm.
 *  May be a bitwise AND of the following:
 *   - @ref PCM_IN
 *   - @ref PCM_OUT
 *   - @ref PCM_MMAP
 *   - @ref PCM_NOIRQ
 *   - @ref PCM_MONOTONIC
 * @param config The hardware and software parameters to open the PCM with.
 * @returns A PCM structure.
 *  If an error occurs, the pointer of bad_pcm is returned.
 *  Otherwise, it returns the pointer of PCM object.
 *  Client code should check that the PCM opened properly by calling @ref pcm_is_ready.
 *  If @ref pcm_is_ready returns false, check @ref pcm_get_error for more information.
 * @ingroup libtinyalsa-pcm
 */
struct pcm *pcm_open(unsigned int card, unsigned int device,
                     unsigned int flags, const struct pcm_config *config)
{
    struct pcm *pcm;
    struct snd_pcm_info info;
    int rc;

    pcm = calloc(1, sizeof(struct pcm));
    if (!pcm) {
        oops(&bad_pcm, ENOMEM, "can't allocate PCM object");
        return &bad_pcm;
    }

    /* Default to hw_ops, attemp plugin open only if hw (/dev/snd/pcm*) open fails */
    pcm->ops = &hw_ops;
    pcm->fd = pcm->ops->open(card, device, flags, &pcm->data, NULL);

#ifdef TINYALSA_USES_PLUGINS
    if (pcm->fd < 0) {
        int pcm_type;
        pcm->snd_node = snd_utils_open_pcm(card, device);
        pcm_type = snd_utils_get_node_type(pcm->snd_node);
        if (!pcm->snd_node || pcm_type != SND_NODE_TYPE_PLUGIN) {
            oops(&bad_pcm, ENODEV, "no device (hw/plugin) for card(%u), device(%u)",
                 card, device);
            goto fail_close_dev_node;
        }
        pcm->ops = &plug_ops;
        pcm->fd = pcm->ops->open(card, device, flags, &pcm->data, pcm->snd_node);
    }
#endif
    if (pcm->fd < 0) {
        oops(&bad_pcm, errno, "cannot open device (%u) for card (%u)",
             device, card);
        goto fail_close_dev_node;
    }

    pcm->flags = flags;

    if (pcm->ops->ioctl(pcm->data, SNDRV_PCM_IOCTL_INFO, &info)) {
        oops(&bad_pcm, errno, "cannot get info");
        goto fail_close;
    }
    pcm->subdevice = info.subdevice;

    if (pcm_set_config(pcm, config) != 0) {
        memcpy(bad_pcm.error, pcm->error, sizeof(pcm->error));
        goto fail_close;
    }

    rc = pcm_hw_mmap_status(pcm);
    if (rc < 0) {
        oops(&bad_pcm, errno, "mmap status failed");
        goto fail;
    }

#ifdef SNDRV_PCM_IOCTL_TTSTAMP
    if (pcm->flags & PCM_MONOTONIC) {
        int arg = SNDRV_PCM_TSTAMP_TYPE_MONOTONIC;
        rc = pcm->ops->ioctl(pcm->data, SNDRV_PCM_IOCTL_TTSTAMP, &arg);
        if (rc < 0) {
            oops(&bad_pcm, errno, "cannot set timestamp type");
            goto fail;
        }
    }
#endif

    pcm->xruns = 0;
    return pcm;

fail:
    pcm_hw_munmap_status(pcm);
    if (flags & PCM_MMAP)
        pcm->ops->munmap(pcm->data, pcm->mmap_buffer, pcm_frames_to_bytes(pcm, pcm->buffer_size));
fail_close:
    pcm->ops->close(pcm->data);
fail_close_dev_node:
#ifdef TINYALSA_USES_PLUGINS
    if (pcm->snd_node)
        snd_utils_close_dev_node(pcm->snd_node);
#endif
    free(pcm);
    return &bad_pcm;
}

/** Checks if a PCM file has been opened without error.
 * @param pcm A PCM handle.
 *  May be NULL.
 * @return If a PCM's file descriptor is not valid or the pointer is NULL, it returns zero.
 *  Otherwise, the function returns one.
 * @ingroup libtinyalsa-pcm
 */
int pcm_is_ready(const struct pcm *pcm)
{
    if (pcm != NULL) {
        return pcm->fd >= 0;
    }
    return 0;
}

/** Links two PCMs.
 * After this function is called, the two PCMs will prepare, start and stop in sync (at the same time).
 * If an error occurs, the error message will be written to @p pcm1.
 * @param pcm1 A PCM handle.
 * @param pcm2 Another PCM handle.
 * @return On success, zero; on failure, a negative number.
 * @ingroup libtinyalsa-pcm
 */
int pcm_link(struct pcm *pcm1, struct pcm *pcm2)
{
    int err = ioctl(pcm1->fd, SNDRV_PCM_IOCTL_LINK, pcm2->fd);
    if (err == -1) {
        return oops(pcm1, errno, "cannot link PCM");
    }
    return 0;
}

/** Unlinks a PCM.
 * @see @ref pcm_link
 * @param pcm A PCM handle.
 * @return On success, zero; on failure, a negative number.
 * @ingroup libtinyalsa-pcm
 */
int pcm_unlink(struct pcm *pcm)
{
    int err = ioctl(pcm->fd, SNDRV_PCM_IOCTL_UNLINK);
    if (err == -1) {
        return oops(pcm, errno, "cannot unlink PCM");
    }
    return 0;
}

/** Prepares a PCM, if it has not been prepared already.
 * @param pcm A PCM handle.
 * @return On success, zero; on failure, a negative number.
 * @ingroup libtinyalsa-pcm
 */
int pcm_prepare(struct pcm *pcm)
{
    if (pcm->ops->ioctl(pcm->data, SNDRV_PCM_IOCTL_PREPARE) < 0)
        return oops(pcm, errno, "cannot prepare channel");

    /* get appl_ptr and avail_min from kernel */
    pcm_sync_ptr(pcm, SNDRV_PCM_SYNC_PTR_APPL|SNDRV_PCM_SYNC_PTR_AVAIL_MIN);

    return 0;
}

/** Starts a PCM.
 * @param pcm A PCM handle.
 * @return On success, zero; on failure, a negative number.
 * @ingroup libtinyalsa-pcm
 */
int pcm_start(struct pcm *pcm)
{
    if (pcm_state(pcm) == PCM_STATE_SETUP && pcm_prepare(pcm) != 0) {
        return -1;
    }

    /* set appl_ptr and avail_min in kernel */
    if (pcm_sync_ptr(pcm, 0) < 0)
        return -1;

    if (pcm->mmap_status->state != PCM_STATE_RUNNING) {
        if (pcm->ops->ioctl(pcm->data, SNDRV_PCM_IOCTL_START) < 0)
            return oops(pcm, errno, "cannot start channel");
    }

    return 0;
}

/** Stops a PCM.
 * @param pcm A PCM handle.
 * @return On success, zero; on failure, a negative number.
 * @ingroup libtinyalsa-pcm
 */
int pcm_stop(struct pcm *pcm)
{
    if (pcm->ops->ioctl(pcm->data, SNDRV_PCM_IOCTL_DROP) < 0)
        return oops(pcm, errno, "cannot stop channel");

    return 0;
}

static inline long pcm_mmap_playback_avail(struct pcm *pcm)
{
    long avail = pcm->mmap_status->hw_ptr + (unsigned long) pcm->buffer_size -
            pcm->mmap_control->appl_ptr;

    if (avail < 0) {
        avail += pcm->boundary;
    } else if ((unsigned long) avail >= pcm->boundary) {
        avail -= pcm->boundary;
    }

    return avail;
}

static inline long pcm_mmap_capture_avail(struct pcm *pcm)
{
    long avail = pcm->mmap_status->hw_ptr - pcm->mmap_control->appl_ptr;
    if (avail < 0) {
        avail += pcm->boundary;
    }

    return avail;
}

int pcm_mmap_avail(struct pcm *pcm)
{
    pcm_sync_ptr(pcm, SNDRV_PCM_SYNC_PTR_HWSYNC);
    if (pcm->flags & PCM_IN) {
        return (int) pcm_mmap_capture_avail(pcm);
    } else {
        return (int) pcm_mmap_playback_avail(pcm);
    }
}

static void pcm_mmap_appl_forward(struct pcm *pcm, int frames)
{
    unsigned long appl_ptr = pcm->mmap_control->appl_ptr;
    appl_ptr += frames;

    /* check for boundary wrap */
    if (appl_ptr >= pcm->boundary) {
        appl_ptr -= pcm->boundary;
    }
    pcm->mmap_control->appl_ptr = appl_ptr;
}

int pcm_mmap_begin(struct pcm *pcm, void **areas, unsigned int *offset,
                   unsigned int *frames)
{
    unsigned int continuous, copy_frames, avail;

    /* return the mmap buffer */
    *areas = pcm->mmap_buffer;

    /* and the application offset in frames */
    *offset = pcm->mmap_control->appl_ptr % pcm->buffer_size;

    avail = pcm_mmap_avail(pcm);
    if (avail > pcm->buffer_size)
        avail = pcm->buffer_size;
    continuous = pcm->buffer_size - *offset;

    /* we can only copy frames if the are available and continuos */
    copy_frames = *frames;
    if (copy_frames > avail)
        copy_frames = avail;
    if (copy_frames > continuous)
        copy_frames = continuous;
    *frames = copy_frames;

    return 0;
}

static int pcm_areas_copy(struct pcm *pcm, unsigned int pcm_offset,
                          char *buf, unsigned int src_offset,
                          unsigned int frames)
{
    int size_bytes = pcm_frames_to_bytes(pcm, frames);
    int pcm_offset_bytes = pcm_frames_to_bytes(pcm, pcm_offset);
    int src_offset_bytes = pcm_frames_to_bytes(pcm, src_offset);

    /* interleaved only atm */
    if (pcm->flags & PCM_IN)
        memcpy(buf + src_offset_bytes,
               (char*)pcm->mmap_buffer + pcm_offset_bytes,
               size_bytes);
    else
        memcpy((char*)pcm->mmap_buffer + pcm_offset_bytes,
               buf + src_offset_bytes,
               size_bytes);
    return 0;
}

int pcm_mmap_commit(struct pcm *pcm, unsigned int offset, unsigned int frames)
{
    int ret;

    /* not used */
    (void) offset;

    /* update the application pointer in userspace and kernel */
    pcm_mmap_appl_forward(pcm, frames);
    ret = pcm_sync_ptr(pcm, 0);
    if (ret != 0){
        printf("%d\n", ret);
        return ret;
    }

    return frames;
}

static int pcm_mmap_transfer_areas(struct pcm *pcm, char *buf,
                                unsigned int offset, unsigned int size)
{
    void *pcm_areas;
    int commit;
    unsigned int pcm_offset, frames, count = 0;

    while (pcm_mmap_avail(pcm) && size) {
        frames = size;
        pcm_mmap_begin(pcm, &pcm_areas, &pcm_offset, &frames);
        pcm_areas_copy(pcm, pcm_offset, buf, offset, frames);
        commit = pcm_mmap_commit(pcm, pcm_offset, frames);
        if (commit < 0) {
            oops(pcm, commit, "failed to commit %d frames\n", frames);
            return commit;
        }

        offset += commit;
        count += commit;
        size -= commit;
    }
    return count;
}

int pcm_get_poll_fd(struct pcm *pcm)
{
    return pcm->fd;
}

int pcm_avail_update(struct pcm *pcm)
{
    pcm_sync_ptr(pcm, SNDRV_PCM_SYNC_PTR_APPL|SNDRV_PCM_SYNC_PTR_AVAIL_MIN);
    return pcm_mmap_avail(pcm);
}

/** Returns available frames in pcm buffer and corresponding time stamp.
 * The clock is CLOCK_MONOTONIC if flag @ref PCM_MONOTONIC was specified in @ref pcm_open,
 * otherwise the clock is CLOCK_REALTIME.
 * For an input stream, frames available are frames ready for the application to read.
 * For an output stream, frames available are the number of empty frames available for the application to write.
 * @param pcm A PCM handle.
 * @param avail The number of available frames
 * @param tstamp The timestamp
 * @return On success, zero is returned; on failure, negative one.
 */
int pcm_get_htimestamp(struct pcm *pcm, unsigned int *avail,
                       struct timespec *tstamp)
{
    int checking;
    int tmp;

    if (!pcm_is_ready(pcm))
        return -1;

    checking = 0;

again:

    tmp = pcm_avail_update(pcm);
    if (tmp < 0)
        return tmp; /* error */

    if (checking && (unsigned int) tmp == *avail)
        return 0;

    *avail = (unsigned int) tmp;
    *tstamp = pcm->mmap_status->tstamp;

    /*
     * When status is mmapped, get avail again to ensure
     * valid timestamp.
     */
    if (!pcm->sync_ptr) {
        checking = 1;
        goto again;
    }

    /* SYNC_PTR ioctl was used, no need to check avail */
    return 0;
}

/** Waits for frames to be available for read or write operations.
 * @param pcm A PCM handle.
 * @param timeout The maximum amount of time to wait for, in terms of milliseconds.
 * @returns If frames became available, one is returned.
 *  If a timeout occured, zero is returned.
 *  If an error occured, a negative number is returned.
 * @ingroup libtinyalsa-pcm
 */
int pcm_wait(struct pcm *pcm, int timeout)
{
    struct pollfd pfd;
    int err;

    pfd.fd = pcm->fd;
    pfd.events = POLLIN | POLLOUT | POLLERR | POLLNVAL;

    do {
        /* let's wait for avail or timeout */
        err = pcm->ops->poll(pcm->data, &pfd, 1, timeout);
        if (err < 0)
            return -errno;

        /* timeout ? */
        if (err == 0)
            return 0;

        /* have we been interrupted ? */
        if (errno == -EINTR)
            continue;

        /* check for any errors */
        if (pfd.revents & (POLLERR | POLLNVAL)) {
            switch (pcm_state(pcm)) {
            case PCM_STATE_XRUN:
                return -EPIPE;
            case PCM_STATE_SUSPENDED:
                return -ESTRPIPE;
            case PCM_STATE_DISCONNECTED:
                return -ENODEV;
            default:
                return -EIO;
            }
        }
    /* poll again if fd not ready for IO */
    } while (!(pfd.revents & (POLLIN | POLLOUT)));

    return 1;
}

/*
 * Transfer data to/from mmapped buffer. This imitates the
 * behavior of read/write system calls.
 *
 * However, this doesn't seems to offer any advantage over
 * the read/write syscalls. Should it be removed?
 */
static int pcm_mmap_transfer(struct pcm *pcm, void *buffer, unsigned int frames)
{
    int is_playback;

    int state;
    unsigned int avail;
    unsigned int user_offset = 0;

    int err;
    int transferred_frames;

    is_playback = !(pcm->flags & PCM_IN);

    if (frames == 0)
        return 0;

    /* update hardware pointer and get state */
    err = pcm_sync_ptr(pcm, SNDRV_PCM_SYNC_PTR_HWSYNC |
                            SNDRV_PCM_SYNC_PTR_APPL |
                            SNDRV_PCM_SYNC_PTR_AVAIL_MIN);
    if (err == -1)
        return -1;
    state = pcm->mmap_status->state;

    /*
     * If frames < start_threshold, wait indefinitely.
     * Another thread may start capture
     */
    if (!is_playback && state == PCM_STATE_PREPARED &&
            frames >= pcm->config.start_threshold) {
        if (pcm_start(pcm) < 0) {
            return -1;
        }
    }

    while (frames) {
        avail = pcm_mmap_avail(pcm);

        if (!avail) {
            if (pcm->flags & PCM_NONBLOCK) {
                errno = EAGAIN;
                break;
            }

            /* wait for interrupt */
            err = pcm_wait(pcm, -1);
            if (err < 0) {
                errno = -err;
                break;
            }
        }

        transferred_frames = pcm_mmap_transfer_areas(pcm, buffer, user_offset, frames);
        if (transferred_frames < 0) {
            break;
        }

        user_offset += transferred_frames;
        frames -= transferred_frames;

        /* start playback if written >= start_threshold */
        if (is_playback && state == PCM_STATE_PREPARED &&
                pcm->buffer_size - avail >= pcm->config.start_threshold) {
            if (pcm_start(pcm) < 0) {
                break;
            }
        }
    }

    return user_offset ? (int) user_offset : -1;
}

int pcm_mmap_write(struct pcm *pcm, const void *data, unsigned int count)
{
    if ((~pcm->flags) & (PCM_OUT | PCM_MMAP))
        return -EINVAL;

    unsigned int frames = pcm_bytes_to_frames(pcm, count);
    int res = pcm_writei(pcm, (void *) data, frames);

    if (res < 0) {
        return res;
    }

    return (unsigned int) res == frames ? 0 : -EIO;
}

int pcm_mmap_read(struct pcm *pcm, void *data, unsigned int count)
{
    if ((~pcm->flags) & (PCM_IN | PCM_MMAP))
        return -EINVAL;

    unsigned int frames = pcm_bytes_to_frames(pcm, count);
    int res = pcm_readi(pcm, data, frames);

    if (res < 0) {
        return res;
    }

    return (unsigned int) res == frames ? 0 : -EIO;
}

/* Returns current read/write position in the mmap buffer with associated time stamp. */
int pcm_mmap_get_hw_ptr(struct pcm* pcm, unsigned int *hw_ptr, struct timespec *tstamp)
{
    int rc;

    if (pcm == NULL || hw_ptr == NULL || tstamp == NULL)
        return oops(pcm, EINVAL, "pcm %p, hw_ptr %p, tstamp %p", pcm, hw_ptr, tstamp);

    if (!pcm_is_ready(pcm))
        return oops(pcm, errno, "pcm_is_ready failed");

    rc = pcm_sync_ptr(pcm, SNDRV_PCM_SYNC_PTR_HWSYNC);
    if (rc < 0)
        return oops(pcm, errno, "pcm_sync_ptr failed");

    if (pcm->mmap_status == NULL)
        return oops(pcm, EINVAL, "pcm %p, mmap_status is NULL", pcm);

    if ((pcm->mmap_status->state != PCM_STATE_RUNNING) &&
            (pcm->mmap_status->state != PCM_STATE_DRAINING))
        return oops(pcm, ENOSYS, "invalid stream state %d", pcm->mmap_status->state);

    *tstamp = pcm->mmap_status->tstamp;
    if (tstamp->tv_sec == 0 && tstamp->tv_nsec == 0)
        return oops(pcm, errno, "invalid time stamp");

    *hw_ptr = pcm->mmap_status->hw_ptr;

    return 0;
}

static int pcm_rw_transfer(struct pcm *pcm, void *data, unsigned int frames)
{
    int is_playback;

    struct snd_xferi transfer;
    int res;

    is_playback = !(pcm->flags & PCM_IN);

    transfer.buf = data;
    transfer.frames = frames;
    transfer.result = 0;

    res = pcm->ops->ioctl(pcm->data, is_playback
                          ? SNDRV_PCM_IOCTL_WRITEI_FRAMES
                          : SNDRV_PCM_IOCTL_READI_FRAMES, &transfer);

    return res == 0 ? (int) transfer.result : -1;
}

static int pcm_generic_transfer(struct pcm *pcm, void *data,
                                unsigned int frames)
{
    int res;

#if UINT_MAX > TINYALSA_FRAMES_MAX
    if (frames > TINYALSA_FRAMES_MAX)
        return -EINVAL;
#endif
    if (frames > INT_MAX)
        return -EINVAL;

    if (pcm_state(pcm) == PCM_STATE_SETUP && pcm_prepare(pcm) != 0) {
        return -1;
    }

again:

    if (pcm->flags & PCM_MMAP)
        res = pcm_mmap_transfer(pcm, data, frames);
    else
        res = pcm_rw_transfer(pcm, data, frames);

    if (res < 0) {
        switch (errno) {
        case EPIPE:
            pcm->xruns++;
            /* fallthrough */
        case ESTRPIPE:
            /*
             * Try to restart if we are allowed to do so.
             * Otherwise, return error.
             */
            if (pcm->flags & PCM_NORESTART || pcm_prepare(pcm))
                return -1;
            goto again;
        case EAGAIN:
            if (pcm->flags & PCM_NONBLOCK)
                return -1;
            /* fallthrough */
        default:
            return oops(pcm, errno, "cannot read/write stream data");
        }
    }

    return res;
}

/** Writes audio samples to PCM.
 * If the PCM has not been started, it is started in this function.
 * This function is only valid for PCMs opened with the @ref PCM_OUT flag.
 * @param pcm A PCM handle.
 * @param data The audio sample array
 * @param frame_count The number of frames occupied by the sample array.
 *  This value should not be greater than @ref TINYALSA_FRAMES_MAX
 *  or INT_MAX.
 * @return On success, this function returns the number of frames written; otherwise, a negative number.
 * @ingroup libtinyalsa-pcm
 */
int pcm_writei(struct pcm *pcm, const void *data, unsigned int frame_count)
{
    if (pcm->flags & PCM_IN)
        return -EINVAL;

    return pcm_generic_transfer(pcm, (void*) data, frame_count);
}

/** Reads audio samples from PCM.
 * If the PCM has not been started, it is started in this function.
 * This function is only valid for PCMs opened with the @ref PCM_IN flag.
 * @param pcm A PCM handle.
 * @param data The audio sample array
 * @param frame_count The number of frames occupied by the sample array.
 *  This value should not be greater than @ref TINYALSA_FRAMES_MAX
 *  or INT_MAX.
 * @return On success, this function returns the number of frames written; otherwise, a negative number.
 * @ingroup libtinyalsa-pcm
 */
int pcm_readi(struct pcm *pcm, void *data, unsigned int frame_count)
{
    if (!(pcm->flags & PCM_IN))
        return -EINVAL;

    return pcm_generic_transfer(pcm, data, frame_count);
}

/** Writes audio samples to PCM.
 * If the PCM has not been started, it is started in this function.
 * This function is only valid for PCMs opened with the @ref PCM_OUT flag.
 * This function is not valid for PCMs opened with the @ref PCM_MMAP flag.
 * @param pcm A PCM handle.
 * @param data The audio sample array
 * @param count The number of bytes occupied by the sample array.
 * @return On success, this function returns zero; otherwise, a negative number.
 * @deprecated
 * @ingroup libtinyalsa-pcm
 */
int pcm_write(struct pcm *pcm, const void *data, unsigned int count)
{
    unsigned int requested_frames = pcm_bytes_to_frames(pcm, count);
    int ret = pcm_writei(pcm, data, requested_frames);

    if (ret < 0)
        return ret;

    return ((unsigned int )ret == requested_frames) ? 0 : -EIO;
}

/** Reads audio samples from PCM.
 * If the PCM has not been started, it is started in this function.
 * This function is only valid for PCMs opened with the @ref PCM_IN flag.
 * This function is not valid for PCMs opened with the @ref PCM_MMAP flag.
 * @param pcm A PCM handle.
 * @param data The audio sample array
 * @param count The number of bytes occupied by the sample array.
 * @return On success, this function returns zero; otherwise, a negative number.
 * @deprecated
 * @ingroup libtinyalsa-pcm
 */
int pcm_read(struct pcm *pcm, void *data, unsigned int count)
{
    unsigned int requested_frames = pcm_bytes_to_frames(pcm, count);
    int ret = pcm_readi(pcm, data, requested_frames);

    if (ret < 0)
        return ret;

    return ((unsigned int )ret == requested_frames) ? 0 : -EIO;
}

/** Gets the delay of the PCM, in terms of frames.
 * @param pcm A PCM handle.
 * @returns On success, the delay of the PCM.
 *  On failure, a negative number.
 * @ingroup libtinyalsa-pcm
 */
long pcm_get_delay(struct pcm *pcm)
{
    if (ioctl(pcm->fd, SNDRV_PCM_IOCTL_DELAY, &pcm->pcm_delay) < 0)
        return -1;

    return pcm->pcm_delay;
}

// TODO: Currently in Android, there are some libraries using this function to control the driver.
//   We should remove this function as soon as possible.
int pcm_ioctl(struct pcm *pcm, int request, ...)
{
    va_list ap;
    void * arg;

    if (!pcm_is_ready(pcm))
        return -1;

    va_start(ap, request);
    arg = va_arg(ap, void *);
    va_end(ap);

    // FIXME Does not handle plugins
    return ioctl(pcm->fd, request, arg);
}
