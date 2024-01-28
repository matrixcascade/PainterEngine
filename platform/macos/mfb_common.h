
// Copyright (c) 2014 Daniel Collin. All rights reserved.
// https://github.com/emoon/minifb MIT License

#ifndef MFB_COMMON_H
#define MFB_COMMON_H

#include "mfb.h"

// ------------------------------------
void mfb_set_user_data(struct mfb_opaque_window* window, void* user_data);
void* mfb_get_user_data(struct mfb_opaque_window* window);

// ------------------------------------
int mfb_set_viewport(struct mfb_opaque_window* window, unsigned offset_x, unsigned offset_y, unsigned width, unsigned height);
int mfb_set_viewport_best_fit(struct mfb_opaque_window* window, unsigned old_width, unsigned old_height);

// ------------------------------------
void mfb_get_monitor_scale(struct mfb_opaque_window* window, float* scale_x, float* scale_y);

// ------------------------------------
void keyboard_default(struct mfb_opaque_window* window, mfb_key key, mfb_key_mod mod, int isPressed);

// ------------------------------------
const char* mfb_get_key_name(mfb_key key);
const int mfb_get_key_status(struct mfb_opaque_window* window, int key_index);

// ------------------------------------
void mfb_set_active_callback(struct mfb_opaque_window* window, mfb_active_func callback);
void mfb_set_resize_callback(struct mfb_opaque_window* window, mfb_resize_func callback);
void mfb_set_close_callback(struct mfb_opaque_window* window, mfb_close_func callback);
void mfb_set_keyboard_callback(struct mfb_opaque_window* window, mfb_keyboard_func callback);
void mfb_set_char_input_callback(struct mfb_opaque_window* window, mfb_char_input_func callback);
void mfb_set_mouse_button_callback(struct mfb_opaque_window* window, mfb_mouse_button_func callback);
void mfb_set_mouse_move_callback(struct mfb_opaque_window* window, mfb_mouse_move_func callback);
void mfb_set_mouse_drag_callback(struct mfb_opaque_window* window, mfb_mouse_drag_func callback);
void mfb_set_mouse_scroll_callback(struct mfb_opaque_window* window, mfb_mouse_scroll_func callback);

#endif
