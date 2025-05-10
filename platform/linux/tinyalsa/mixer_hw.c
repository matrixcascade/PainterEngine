/* mixer_hw.c
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
#include <stdarg.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <ctype.h>
#include <poll.h>

#include <sys/ioctl.h>

#include <linux/ioctl.h>
#include <time.h>
#include <sound/asound.h>

#include "mixer_io.h"

/** Store the hardware (kernel interface) mixer data */
struct mixer_hw_data {
    /* Card number for the mixer */
    unsigned int card;
    /* File descriptor of the mixer device node */
    int fd;
};

static void mixer_hw_close(void *data)
{
    struct mixer_hw_data *hw_data = data;

    if (!hw_data)
        return;

    if (hw_data->fd >= 0)
        close(hw_data->fd);

    hw_data->fd = -1;
    free(hw_data);
    hw_data = NULL;
}

static int mixer_hw_ioctl(void *data, unsigned int cmd, ...)
{
    struct mixer_hw_data *hw_data = data;
    va_list ap;
    void *arg;

    va_start(ap, cmd);
    arg = va_arg(ap, void *);
    va_end(ap);

    return ioctl(hw_data->fd, cmd, arg);
}

static ssize_t mixer_hw_read_event(void *data, struct snd_ctl_event *ev,
                                   size_t size)
{
    struct mixer_hw_data *hw_data = data;

    return read(hw_data->fd, ev, size);
}

static const struct mixer_ops mixer_hw_ops = {
    .close = mixer_hw_close,
    .ioctl = mixer_hw_ioctl,
    .read_event = mixer_hw_read_event,
};

int mixer_hw_open(unsigned int card, void **data,
                  const struct mixer_ops **ops)
{
    struct mixer_hw_data *hw_data;
    int fd;
    char fn[256];

    snprintf(fn, sizeof(fn), "/dev/snd/controlC%u", card);
    fd = open(fn, O_RDWR);
    if (fd < 0)
        return fd;

    hw_data = calloc(1, sizeof(*hw_data));
    if (!hw_data)
        return -1;

    hw_data->card = card;
    hw_data->fd = fd;
    *data = hw_data;
    *ops = &mixer_hw_ops;

    return fd;
}
