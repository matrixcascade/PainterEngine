/* mixer.h
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

/** @file */

/** @defgroup libtinyalsa-mixer Mixer Interface
 * @brief All macros, structures and functions that make up the mixer interface.
 */

#ifndef TINYALSA_MIXER_H
#define TINYALSA_MIXER_H

#include <sys/time.h>
#include <stddef.h>

#if defined(__cplusplus)
extern "C" {
#endif

struct mixer;

struct mixer_ctl;

// mixer_ctl_event is a mirroring structure of snd_ctl_event
struct mixer_ctl_event {
    int type;
    union {
        struct {
            unsigned int mask;
            struct {
                unsigned int numid;
                int iface;
                unsigned int device;
                unsigned int subdevice;
                unsigned char name[44];
                unsigned int index;
            } id;
        } element;
        unsigned char data[60];
    } data;
};

/** Mixer control type.
 * @ingroup libtinyalsa-mixer
 */
enum mixer_ctl_type {
    /** boolean control type */
    MIXER_CTL_TYPE_BOOL,
    /** integer control type */
    MIXER_CTL_TYPE_INT,
    /** an enumerated control type */
    MIXER_CTL_TYPE_ENUM,
    MIXER_CTL_TYPE_BYTE,
    MIXER_CTL_TYPE_IEC958,
    /** a 64 bit integer control type */
    MIXER_CTL_TYPE_INT64,
    /** unknown control type */
    MIXER_CTL_TYPE_UNKNOWN,
    /** end of the enumeration (not a control type) */
    MIXER_CTL_TYPE_MAX,
};

struct mixer *mixer_open(unsigned int card);

void mixer_close(struct mixer *mixer);

int mixer_add_new_ctls(struct mixer *mixer);

const char *mixer_get_name(const struct mixer *mixer);

unsigned int mixer_get_num_ctls(const struct mixer *mixer);

unsigned int mixer_get_num_ctls_by_name(const struct mixer *mixer, const char *name);

const struct mixer_ctl *mixer_get_ctl_const(const struct mixer *mixer, unsigned int id);

struct mixer_ctl *mixer_get_ctl(struct mixer *mixer, unsigned int id);

struct mixer_ctl *mixer_get_ctl_by_name(struct mixer *mixer, const char *name);

struct mixer_ctl *mixer_get_ctl_by_name_and_index(struct mixer *mixer,
                                                  const char *name,
                                                  unsigned int index);

int mixer_subscribe_events(struct mixer *mixer, int subscribe);

int mixer_wait_event(struct mixer *mixer, int timeout);

unsigned int mixer_ctl_get_id(const struct mixer_ctl *ctl);

const char *mixer_ctl_get_name(const struct mixer_ctl *ctl);

enum mixer_ctl_type mixer_ctl_get_type(const struct mixer_ctl *ctl);

const char *mixer_ctl_get_type_string(const struct mixer_ctl *ctl);

unsigned int mixer_ctl_get_num_values(const struct mixer_ctl *ctl);

unsigned int mixer_ctl_get_num_enums(const struct mixer_ctl *ctl);

const char *mixer_ctl_get_enum_string(struct mixer_ctl *ctl, unsigned int enum_id);

/* Some sound cards update their controls due to external events,
 * such as HDMI EDID byte data changing when an HDMI cable is
 * connected. This API allows the count of elements to be updated.
 */
void mixer_ctl_update(struct mixer_ctl *ctl);

int mixer_ctl_is_access_tlv_rw(const struct mixer_ctl *ctl);

/* Set and get mixer controls */
int mixer_ctl_get_percent(const struct mixer_ctl *ctl, unsigned int id);

int mixer_ctl_set_percent(struct mixer_ctl *ctl, unsigned int id, int percent);

int mixer_ctl_get_value(const struct mixer_ctl *ctl, unsigned int id);

int mixer_ctl_get_array(const struct mixer_ctl *ctl, void *array, size_t count);

int mixer_ctl_set_value(struct mixer_ctl *ctl, unsigned int id, int value);

int mixer_ctl_set_array(struct mixer_ctl *ctl, const void *array, size_t count);

int mixer_ctl_set_enum_by_string(struct mixer_ctl *ctl, const char *string);

/* Determine range of integer mixer controls */
int mixer_ctl_get_range_min(const struct mixer_ctl *ctl);

int mixer_ctl_get_range_max(const struct mixer_ctl *ctl);

int mixer_read_event(struct mixer *mixer, struct mixer_ctl_event *event);

int mixer_consume_event(struct mixer *mixer);
#if defined(__cplusplus)
}  /* extern "C" */
#endif

#endif

