/* pcm_io.h
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

#ifndef TINYALSA_SRC_PCM_IO_H
#define TINYALSA_SRC_PCM_IO_H

#include <poll.h>
#include <sound/asound.h>

struct snd_node;

struct pcm_ops {
    int (*open) (unsigned int card, unsigned int device,
                 unsigned int flags, void **data, struct snd_node *node);
    void (*close) (void *data);
    int (*ioctl) (void *data, unsigned int cmd, ...);
    void *(*mmap) (void *data, void *addr, size_t length, int prot, int flags,
                   off_t offset);
    int (*munmap) (void *data, void *addr, size_t length);
    int (*poll) (void *data, struct pollfd *pfd, nfds_t nfds, int timeout);
};

extern const struct pcm_ops hw_ops;
extern const struct pcm_ops plug_ops;

#endif /* TINYALSA_SRC_PCM_IO_H */
