
// Copyright (c) 2014 Daniel Collin. All rights reserved.
// https://github.com/emoon/minifb MIT License

#ifndef MFB_ENUM_H
#define MFB_ENUM_H

#include <pthread.h>

// ------------------------------------
typedef enum {
    STATE_OK = 0,
    STATE_EXIT = -1,
    STATE_INVALID_WINDOW = -2,
    STATE_INVALID_BUFFER = -3,
    STATE_INTERNAL_ERROR = -4,
} mfb_state;

// ------------------------------------
typedef enum {
    MOUSE_BTN_0, // No mouse button
    MOUSE_BTN_1,
    MOUSE_BTN_2,
    MOUSE_BTN_3,
    MOUSE_BTN_4,
    MOUSE_BTN_5,
    MOUSE_BTN_6,
    MOUSE_BTN_7,
} mfb_mouse_button;
#define MOUSE_LEFT MOUSE_BTN_1
#define MOUSE_RIGHT MOUSE_BTN_2
#define MOUSE_MIDDLE MOUSE_BTN_3

// ------------------------------------
typedef enum {
    KB_KEY_UNKNOWN = -1,

    KB_KEY_SPACE = 0x20,
    // KB_KEY_APOSTROPHE = 39,
    // KB_KEY_COMMA = 44,
    // KB_KEY_MINUS = 45,
    // KB_KEY_PERIOD = 46,
    // KB_KEY_SLASH = 47,
    KB_KEY_0 = 0x30,
    KB_KEY_1 = 0x31,
    KB_KEY_2 = 0x32,
    KB_KEY_3 = 0x33,
    KB_KEY_4 = 0x34,
    KB_KEY_5 = 0x35,
    KB_KEY_6 = 0x36,
    KB_KEY_7 = 0x37,
    KB_KEY_8 = 0x38,
    KB_KEY_9 = 0x39,
    // KB_KEY_SEMICOLON = 59,
    // KB_KEY_EQUAL = 61,
    KB_KEY_A = 0x41,
    KB_KEY_B = 0x42,
    KB_KEY_C = 0x43,
    KB_KEY_D = 0x44,
    KB_KEY_E = 0x45,
    KB_KEY_F = 0x46,
    KB_KEY_G = 0x47,
    KB_KEY_H = 0x48,
    KB_KEY_I = 0x49,
    KB_KEY_J = 0x4A,
    KB_KEY_K = 0x4B,
    KB_KEY_L = 0x4C,
    KB_KEY_M = 0x4D,
    KB_KEY_N = 0x4E,
    KB_KEY_O = 0x4F,
    KB_KEY_P = 0x50,
    KB_KEY_Q = 0x51,
    KB_KEY_R = 0x52,
    KB_KEY_S = 0x53,
    KB_KEY_T = 0x54,
    KB_KEY_U = 0x55,
    KB_KEY_V = 0x56,
    KB_KEY_W = 0x57,
    KB_KEY_X = 0x58,
    KB_KEY_Y = 0x59,
    KB_KEY_Z = 0x5A,
    // KB_KEY_LEFT_BRACKET = 91,
    // KB_KEY_BACKSLASH = 92,
    // KB_KEY_RIGHT_BRACKET = 93,
    // KB_KEY_GRAVE_ACCENT = 96,
    // KB_KEY_WORLD_1 = 161,
    // KB_KEY_WORLD_2 = 162,

    KB_KEY_ESCAPE = 0x1B,
    KB_KEY_ENTER = 0x0D,
    KB_KEY_TAB = 0x09,
    KB_KEY_BACKSPACE = 0x08,
    KB_KEY_INSERT = 0x2D,
    KB_KEY_DELETE = 0x2E,
    KB_KEY_RIGHT = 0x27,
    KB_KEY_LEFT = 0x25,
    KB_KEY_DOWN = 0x28,
    KB_KEY_UP = 0x26,
    // KB_KEY_PAGE_UP = 266,
    // KB_KEY_PAGE_DOWN = 267,
    KB_KEY_HOME = 0x24,
    KB_KEY_END = 0x23,
    // KB_KEY_CAPS_LOCK = 280,
    KB_KEY_SCROLL_LOCK = 0x91,
    KB_KEY_NUM_LOCK = 0x90,
    KB_KEY_PRINT_SCREEN = 0x2A,
    KB_KEY_PAUSE = 0x13,
    KB_KEY_F1 = 0x70,
    KB_KEY_F2 = 0x71,
    KB_KEY_F3 = 0x72,
    KB_KEY_F4 = 0x73,
    KB_KEY_F5 = 0x74,
    KB_KEY_F6 = 0x75,
    KB_KEY_F7 = 0x76,
    KB_KEY_F8 = 0x77,
    KB_KEY_F9 = 0x78,
    KB_KEY_F10 = 0x79,
    KB_KEY_F11 = 0x7A,
    KB_KEY_F12 = 0x7B,
    KB_KEY_F13 = 0x7C,
    KB_KEY_F14 = 0x7D,
    KB_KEY_F15 = 0x7E,
    KB_KEY_F16 = 0x7F,
    KB_KEY_F17 = 0x80,
    KB_KEY_F18 = 0x81,
    KB_KEY_F19 = 0x82,
    KB_KEY_F20 = 0x83,
    KB_KEY_F21 = 0x84,
    KB_KEY_F22 = 0x85,
    KB_KEY_F23 = 0x86,
    KB_KEY_F24 = 0x87,
    // KB_KEY_F25 = 314,
    KB_KEY_KP_0 = 0x60,
    KB_KEY_KP_1 = 0x61,
    KB_KEY_KP_2 = 0x62,
    KB_KEY_KP_3 = 0x63,
    KB_KEY_KP_4 = 0x64,
    KB_KEY_KP_5 = 0x65,
    KB_KEY_KP_6 = 0x66,
    KB_KEY_KP_7 = 0x67,
    KB_KEY_KP_8 = 0x68,
    KB_KEY_KP_9 = 0x69,
    KB_KEY_KP_DECIMAL = 0x6E,
    KB_KEY_KP_DIVIDE = 0x6F,
    KB_KEY_KP_MULTIPLY = 0x6A,
    KB_KEY_KP_SUBTRACT = 0x6D,
    KB_KEY_KP_ADD = 0x6B,
    // KB_KEY_KP_ENTER = 335,
    // KB_KEY_KP_EQUAL = 336,
    KB_KEY_LEFT_SHIFT = 0xA0,
    KB_KEY_LEFT_CONTROL = 0xA2,
    // KB_KEY_LEFT_ALT = 342,
    // KB_KEY_LEFT_SUPER = 343,
    KB_KEY_RIGHT_SHIFT = 0xA1,
    KB_KEY_RIGHT_CONTROL = 0xA3,
    // KB_KEY_RIGHT_ALT = 346,
    // KB_KEY_RIGHT_SUPER = 347,
    KB_KEY_MENU = 0x12
} mfb_key;
#define KB_KEY_LAST KB_KEY_MENU

// ------------------------------------
typedef enum {
    KB_MOD_SHIFT = 0x01,
    KB_MOD_CONTROL = 0x02,
    KB_MOD_ALT = 0x04,
    KB_MOD_SUPER = 0x08,
    KB_MOD_CAPS_LOCK = 0x10,
    KB_MOD_NUM_LOCK = 0x20
} mfb_key_mod;

// ------------------------------------
typedef enum {
    WF_RESIZABLE = 0x01,
    WF_FULLSCREEN = 0x02,
    WF_FULLSCREEN_DESKTOP = 0x04,
    WF_BORDERLESS = 0x08,
    WF_ALWAYS_ON_TOP = 0x10,
} mfb_window_flags;

// ------------------------------------
extern pthread_mutex_t _eventMutex;

// ------------------------------------
// Opaque pointer
struct mfb_opaque_window;
extern struct mfb_opaque_window* window;

// ------------------------------------
// Event callback function
typedef void (*mfb_active_func)(struct mfb_opaque_window* window, int isActive);
typedef void (*mfb_resize_func)(struct mfb_opaque_window* window, int width, int height);
typedef int (*mfb_close_func)(struct mfb_opaque_window* window);
typedef void (*mfb_keyboard_func)(struct mfb_opaque_window* window, mfb_key key, mfb_key_mod mod, int isPressed);
typedef void (*mfb_char_input_func)(struct mfb_opaque_window* window, unsigned int code);
typedef void (*mfb_mouse_button_func)(struct mfb_opaque_window* window, mfb_mouse_button button, mfb_key_mod mod, int isPressed);
typedef void (*mfb_mouse_move_func)(struct mfb_opaque_window* window, int x, int y);
typedef void (*mfb_mouse_drag_func)(struct mfb_opaque_window* window, int x, int y);
typedef void (*mfb_mouse_scroll_func)(struct mfb_opaque_window* window, mfb_key_mod mod, float deltaX, float deltaY, float deltaZ);

#endif
