
// Copyright (c) 2014 Daniel Collin. All rights reserved.
// https://github.com/emoon/minifb MIT License

#ifndef MFB_DISPLAY_H
#define MFB_DISPLAY_H

#include "mfb.h"

// ------------------------------------
#define kCall(func, ...) \
    if (window_data && window_data->func) window_data->func((struct mfb_opaque_window*)window_data, __VA_ARGS__);
#define kUnused(var) (void)var;

// ------------------------------------
struct mfb_opaque_window* mfb_open_ex(const char* title, unsigned width, unsigned height, unsigned flags);

// ------------------------------------
mfb_state mfb_window_update(struct mfb_opaque_window* window, void* buffer, unsigned width, unsigned height);
mfb_state mfb_window_update_events(struct mfb_opaque_window* window);
void mfb_window_close(struct mfb_opaque_window* window);
mfb_state mfb_window_timer_reset(struct mfb_opaque_window* window);
unsigned mfb_get_screen_width();
unsigned mfb_get_screen_height();

// ------------------------------------
int mfb_is_window_active(struct mfb_opaque_window* window);
unsigned mfb_get_window_width(struct mfb_opaque_window* window);
unsigned mfb_get_window_height(struct mfb_opaque_window* window);
int mfb_get_mouse_x(struct mfb_opaque_window* window);          // Last mouse pos X
int mfb_get_mouse_y(struct mfb_opaque_window* window);          // Last mouse pos Y
float mfb_get_mouse_scroll_x(struct mfb_opaque_window* window); // Mouse wheel X as a sum. When you call this function it resets.
float mfb_get_mouse_scroll_y(struct mfb_opaque_window* window); // Mouse wheel Y as a sum. When you call this function it resets.
const unsigned char* mfb_get_mouse_button_buffer(
    struct mfb_opaque_window* window); // One byte for every button. Press (1), Release 0. (up to 8 buttons)
const unsigned char* mfb_get_key_buffer(struct mfb_opaque_window* window); // One byte for every key. Press (1), Release 0.

// ------------------------------------
void mfb_set_target_fps(unsigned int fps);
unsigned mfb_get_target_fps(void);
int mfb_wait_sync(struct mfb_opaque_window* window);
double mfb_get_window_elapsed(struct mfb_opaque_window* window);
#endif
