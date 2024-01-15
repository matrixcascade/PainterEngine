
// Copyright (c) 2014 Daniel Collin. All rights reserved.
// https://github.com/emoon/minifb MIT License

#ifndef MFB_TIME_H
#define MFB_TIME_H

// ------------------------------------
typedef struct {
    long long start_time;
    long long delta_counter;
    unsigned long long time;
} mfb_timer;

// ------------------------------------
extern double g_timer_frequency;
extern double g_timer_resolution;
extern double g_time_for_frame;
extern int g_use_hardware_sync;
extern int g_target_fps_changed;

// ------------------------------------
extern unsigned long long mfb_timer_tick(void);
extern void mfb_timer_init(void);

// ------------------------------------
mfb_timer* mfb_timer_create(void);
void mfb_timer_destroy(mfb_timer* tmr);
void mfb_timer_reset(mfb_timer* tmr);
double mfb_timer_now(mfb_timer* tmr);
double mfb_timer_delta(mfb_timer* tmr);
double mfb_timer_get_frequency(void);
double mfb_timer_get_resolution(void);

#endif
