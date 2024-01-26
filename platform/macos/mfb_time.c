
// Copyright (c) 2014 Daniel Collin. All rights reserved.
// https://github.com/emoon/minifb MIT License

#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <unistd.h>

#include "mfb_time.h"
#include "platform/modules/px_time.h"

// ------------------------------------
double g_timer_frequency;
double g_timer_resolution;
double g_time_for_frame = 1.0 / 60.0;
int g_use_hardware_sync = 0;
int g_target_fps_changed = 1;

// ------------------------------------
void mfb_timer_reset(mfb_timer* tmr);

// ------------------------------------
void set_target_fps_aux() { g_target_fps_changed = 1; }

// ------------------------------------
void mfb_set_target_fps(unsigned int fps) {
    if (fps == 0) {
        g_time_for_frame = 0;
    } else {
        g_time_for_frame = 1.0 / fps;
    }
    set_target_fps_aux();
}

// ------------------------------------
unsigned mfb_get_target_fps() {
    if (g_time_for_frame == 0) {
        return 0;
    } else {
        return (unsigned)(1.0 / g_time_for_frame);
    }
}

// ------------------------------------
mfb_timer* mfb_timer_create() {
    static int once = 1;
    mfb_timer* tmr;

    if (once) {
        once = 0;
        mfb_timer_init();
    }

    tmr = malloc(sizeof(mfb_timer));
    mfb_timer_reset(tmr);

    return tmr;
}

// ------------------------------------
void mfb_timer_destroy(mfb_timer* tmr) {
    if (tmr != 0x0) {
        free(tmr);
    }
}

// ------------------------------------
void mfb_timer_reset(mfb_timer* tmr) {
    if (tmr == 0x0) return;

    tmr->start_time = mfb_timer_tick();
    tmr->delta_counter = tmr->start_time;
    tmr->time = 0;
}

// ------------------------------------
double mfb_timer_now(mfb_timer* tmr) {
    unsigned long long counter;

    if (tmr == 0x0) return 0.0;

    counter = mfb_timer_tick();
    tmr->time += (counter - tmr->start_time);
    tmr->start_time = counter;

    return tmr->time * g_timer_resolution;
}

// ------------------------------------
double mfb_timer_delta(mfb_timer* tmr) {
    unsigned long long counter;
    unsigned long long delta;

    if (tmr == 0x0) return 0.0;

    counter = mfb_timer_tick();
    delta = (counter - tmr->delta_counter);
    tmr->delta_counter = counter;

    return delta * g_timer_resolution;
}

// ------------------------------------
double mfb_timer_get_frequency() { return g_timer_frequency; }

// ------------------------------------
double mfb_timer_get_resolution() { return g_timer_resolution; }

// ------------------------------------
int PX_TimeGetYear() {
    time_t timep;
    struct tm p;
    time(&timep);
    gmtime_r(&timep, &p); // Using gmtime_r for thread safety

    p.tm_hour += 8; // Convert to the target timezone (e.g., UTC+8)
    mktime(&p); // Recalculate the tm structure
    return 1900 + p.tm_year;
}

// ------------------------------------
int PX_TimeGetMonth() {
    time_t timep;
    struct tm p;
    time(&timep);
    gmtime_r(&timep, &p);
    
    p.tm_hour += 8;
    mktime(&p);
    return p.tm_mon + 1;
}

// ------------------------------------
int PX_TimeGetDay() {
    time_t timep;
    struct tm p;
    time(&timep);
    gmtime_r(&timep, &p);
    
    p.tm_hour += 8;
    mktime(&p);
    return p.tm_mday;
}

// ------------------------------------
int PX_TimeGetHour() {
    time_t timep;
    struct tm p;
    time(&timep);
    gmtime_r(&timep, &p);
    
    p.tm_hour += 8;
    mktime(&p);
    return p.tm_hour;
}

// ------------------------------------
int PX_TimeGetMinute() {
    time_t timep;
    struct tm p;
    time(&timep);
    gmtime_r(&timep, &p);
    
    p.tm_hour += 8;
    mktime(&p);
    return p.tm_min;
}

// ------------------------------------
int PX_TimeGetSecond() {
    time_t timep;
    struct tm p;
    time(&timep);
    gmtime_r(&timep, &p);
    
    p.tm_hour += 8;
    mktime(&p);
    return p.tm_sec;
}

// ------------------------------------
unsigned int PX_TimeGetTime() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);

    // Convert time to milliseconds
    return (unsigned int)(ts.tv_sec * 1000 + ts.tv_nsec / 1000000);
}

// ------------------------------------
void PX_Sleep(unsigned int ms) { usleep(ms * 1000); }
