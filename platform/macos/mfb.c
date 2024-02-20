
// Copyright (c) 2014 Daniel Collin. All rights reserved.
// https://github.com/emoon/minifb MIT License

#include <stdio.h>
#include <stdint.h>
#include <CoreGraphics/CoreGraphics.h>

#include "mfb_display.h"
#include "mfb_common.h"

#include "px_display.h"
#include "platform/modules/px_thread.h"
#include "PainterEngine.h"

static px_bool g_active = PX_TRUE;
static px_thread mfb_appThread;
static PX_Object_Event mfb_event;
static px_float cursor_x = -1, cursor_y = -1, cursor_z = -1;
static px_float cursor_x_scale, cursor_y_scale;
static CGPoint LastDownPoint;
static mfb_key mfb_press_keys[31] = {KB_KEY_SPACE, KB_KEY_A, KB_KEY_B, KB_KEY_C,    KB_KEY_D,     KB_KEY_E,    KB_KEY_F, KB_KEY_G,
                                     KB_KEY_H,     KB_KEY_I, KB_KEY_J, KB_KEY_K,    KB_KEY_L,     KB_KEY_M,    KB_KEY_N, KB_KEY_O,
                                     KB_KEY_P,     KB_KEY_Q, KB_KEY_R, KB_KEY_S,    KB_KEY_T,     KB_KEY_U,    KB_KEY_V, KB_KEY_W,
                                     KB_KEY_X,     KB_KEY_Y, KB_KEY_Z, KB_KEY_LEFT, KB_KEY_RIGHT, KB_KEY_DOWN, KB_KEY_UP};
pthread_mutex_t _eventMutex;

// ------------------------------------
extern void PX_ThreadDestroy(px_thread* thread);

// ------------------------------------
px_void PX_PressKeysUpdate();

// ------------------------------------
static void reset_cursor_scale() {
    cursor_x_scale = App.runtime.surface_width * 1.0f / App.runtime.window_width;
    cursor_y_scale = App.runtime.surface_height * 1.0f / App.runtime.window_height;
}

// ------------------------------------
static void active(struct mfb_opaque_window* window, px_bool isActive) {
    const char* window_title = "";
    if (window) {
        window_title = (const char*)mfb_get_user_data(window);
    }
    // fprintf(stdout, "%s > active: %d\n", window_title, isActive);
    g_active = isActive;
}

// ------------------------------------
static void resize(struct mfb_opaque_window* window, int width, int height) {
    uint32_t x = 0;
    uint32_t y = 0;
    const char* window_title = "";
    if (window) {
        window_title = (const char*)mfb_get_user_data(window);
    }

    // fprintf(stdout, "%s > resize: %d, %d\n", window_title, width, height);
    if (width > PX_GetScreenWidth()) {
        x = (width - PX_GetScreenWidth()) >> 1;
        width = PX_GetScreenWidth();
    }
    if (height > PX_GetScreenHeight()) {
        y = (height - PX_GetScreenHeight()) >> 1;
        height = PX_GetScreenHeight();
    }
    mfb_set_viewport(window, x, y, width, height);
}

// ------------------------------------
static px_bool window_close(struct mfb_opaque_window* window) {
    const char* window_title = "";
    if (window) {
        window_title = (const char*)mfb_get_user_data(window);
    }
    // fprintf(stdout, "%s > close\n", window_title);
    return PX_TRUE; // true => confirm close
                    // false => don't close
}

// ------------------------------------
static void keyboard(struct mfb_opaque_window* window, mfb_key key, mfb_key_mod mod, px_bool isPressed) {
    const char* window_title = "";
    if (window) {
        window_title = (const char*)mfb_get_user_data(window);
    }
    // fprintf(stdout, "%s > keyboard: key: %s (pressed: %d) [key_mod: %x]\n", window_title, mfb_get_key_name(key), isPressed, mod);

    mfb_event.Event = PX_OBJECT_EVENT_ANY;
    if (key == KB_KEY_ESCAPE) {
        mfb_window_close(window);
    }

    switch (key) {
        case KB_KEY_BACKSPACE:
            if (isPressed) {
                static px_char text[2] = {'\b', '\0'};
                mfb_event.Event = PX_OBJECT_EVENT_STRING;
                PX_Object_Event_SetStringPtr(&mfb_event, &text);
            }
            break;
        case KB_KEY_ENTER:
            if (isPressed) {
                static px_char text[2] = {'\n', '\0'};
                ;
                mfb_event.Event = PX_OBJECT_EVENT_STRING;
                PX_Object_Event_SetStringPtr(&mfb_event, &text);
            }
            break;
        case KB_KEY_SPACE:
        case KB_KEY_A:
        case KB_KEY_B:
        case KB_KEY_C:
        case KB_KEY_D:
        case KB_KEY_E:
        case KB_KEY_F:
        case KB_KEY_G:
        case KB_KEY_H:
        case KB_KEY_I:
        case KB_KEY_J:
        case KB_KEY_K:
        case KB_KEY_L:
        case KB_KEY_M:
        case KB_KEY_N:
        case KB_KEY_O:
        case KB_KEY_P:
        case KB_KEY_Q:
        case KB_KEY_R:
        case KB_KEY_S:
        case KB_KEY_T:
        case KB_KEY_U:
        case KB_KEY_V:
        case KB_KEY_W:
        case KB_KEY_X:
        case KB_KEY_Y:
        case KB_KEY_Z:
        case KB_KEY_LEFT:
        case KB_KEY_RIGHT:
        case KB_KEY_DOWN:
        case KB_KEY_UP:
            break;
        default:
            mfb_event.Event = PX_OBJECT_EVENT_KEYDOWN;
            PX_Object_Event_SetKeyDown(&mfb_event, (px_uint)key);
            break;
    }

    if (mfb_event.Event != PX_OBJECT_EVENT_ANY) {
        PX_ApplicationPostEvent(&App, mfb_event);
    }
}

// ------------------------------------
static void char_input(struct mfb_opaque_window* window, unsigned int charCode) {
    const char* window_title = "";
    if (window) {
        window_title = (const char*)mfb_get_user_data(window);
    }
    // fprintf(stdout, "%s > charCode: %#X\n", window_title, charCode);

    // Little-Endian byte order
    static px_char text[4] = {0};
    text[0] = (char)(charCode & 0xFF);         // Lowest byte
    text[1] = (char)((charCode >> 8) & 0xFF);  // Next byte
    text[2] = (char)((charCode >> 16) & 0xFF); // Next byte
    text[3] = (char)((charCode >> 24) & 0xFF); // Highest byte

    mfb_event.Event = PX_OBJECT_EVENT_STRING;
    PX_Object_Event_SetStringPtr(&mfb_event, text);

    PX_ApplicationPostEvent(&App, mfb_event);
}

// ------------------------------------
static void mouse_btn(struct mfb_opaque_window* window, mfb_mouse_button button, mfb_key_mod mod, px_bool isPressed) {
    const char* window_title = "";
    int x, y;
    if (window) {
        window_title = (const char*)mfb_get_user_data(window);
    }
    x = mfb_get_mouse_x(window);
    y = mfb_get_mouse_y(window);
    // fprintf(stdout, "%s > mouse_btn: button: %d (pressed: %d) (at: %d, %d) [key_mod: %x]\n", window_title, button, isPressed, x, y, mod);

    mfb_event.Event = PX_OBJECT_EVENT_ANY;
    switch (button) {
        case MOUSE_BTN_1: {
            if (isPressed) {
                mfb_event.Event = PX_OBJECT_EVENT_CURSORDOWN;
                cursor_x = (px_float)x;
                cursor_y = (px_float)y;
                PX_Object_Event_SetCursorX(&mfb_event, cursor_x * cursor_x_scale);
                PX_Object_Event_SetCursorY(&mfb_event, cursor_y * cursor_y_scale);

                LastDownPoint.x = x;
                LastDownPoint.y = y;
            } else {
                if (x == LastDownPoint.x && y == LastDownPoint.y) {
                    mfb_event.Event = PX_OBJECT_EVENT_CURSORCLICK;
                    cursor_x = (px_float)x;
                    cursor_y = (px_float)y;
                    PX_Object_Event_SetCursorX(&mfb_event, cursor_x * cursor_x_scale);
                    PX_Object_Event_SetCursorY(&mfb_event, cursor_y * cursor_y_scale);

                    LastDownPoint.x = -1;
                    LastDownPoint.y = -1;
                    PX_ApplicationPostEvent(&App, mfb_event);
                }
                mfb_event.Event = PX_OBJECT_EVENT_CURSORUP;

                cursor_x = (px_float)x;
                cursor_y = (px_float)y;
                PX_Object_Event_SetCursorX(&mfb_event, cursor_x * cursor_x_scale);
                PX_Object_Event_SetCursorY(&mfb_event, cursor_y * cursor_y_scale);
            }
        } break;
        case MOUSE_BTN_2: {
            if (isPressed) {
                mfb_event.Event = PX_OBJECT_EVENT_CURSORRDOWN;
                cursor_x = (px_float)x;
                cursor_y = (px_float)y;
                PX_Object_Event_SetCursorX(&mfb_event, cursor_x * cursor_x_scale);
                PX_Object_Event_SetCursorY(&mfb_event, cursor_y * cursor_y_scale);
            } else {
                mfb_event.Event = PX_OBJECT_EVENT_CURSORRUP;
                cursor_x = (px_float)x;
                cursor_y = (px_float)y;
                PX_Object_Event_SetCursorX(&mfb_event, cursor_x * cursor_x_scale);
                PX_Object_Event_SetCursorY(&mfb_event, cursor_y * cursor_y_scale);
            }
        } break;
        default:
            break;
    }

    if (mfb_event.Event != PX_OBJECT_EVENT_ANY) {
        PX_ApplicationPostEvent(&App, mfb_event);
    }
}

// ------------------------------------
static void mouse_move(struct mfb_opaque_window* window, int x, int y) {
    kUnused(window);
    kUnused(x);
    kUnused(y);
    const char* window_title = "";
    if (window) {
        window_title = mfb_get_user_data(window);
    }
    // fprintf(stdout, "%s > mouse_move: %d, %d\n", window_title, x, y);

    mfb_event.Event = PX_OBJECT_EVENT_CURSORMOVE;
    cursor_x = (px_float)x;
    cursor_y = (px_float)y;

    PX_Object_Event_SetCursorX(&mfb_event, cursor_x * cursor_x_scale);
    PX_Object_Event_SetCursorY(&mfb_event, cursor_y * cursor_y_scale);

    PX_ApplicationPostEvent(&App, mfb_event);
}

// ------------------------------------
static void mouse_drag(struct mfb_opaque_window* window, int x, int y) {
    kUnused(window);
    kUnused(x);
    kUnused(y);
    const char* window_title = "";
    if (window) {
        window_title = mfb_get_user_data(window);
    }
    // fprintf(stdout, "%s > mouse_drag: %d, %d\n", window_title, x, y);

    mfb_event.Event = PX_OBJECT_EVENT_CURSORDRAG;
    cursor_x = (px_float)x;
    cursor_y = (px_float)y;

    PX_Object_Event_SetCursorX(&mfb_event, cursor_x * cursor_x_scale);
    PX_Object_Event_SetCursorY(&mfb_event, cursor_y * cursor_y_scale);

    PX_ApplicationPostEvent(&App, mfb_event);
}

// ------------------------------------
static void mouse_scroll(struct mfb_opaque_window* window, mfb_key_mod mod, float deltaX, float deltaY, float deltaZ) {
    const char* window_title = "";
    if (window) {
        window_title = (const char*)mfb_get_user_data(window);
    }
    // fprintf(stdout, "%s > mouse_scroll: x: %f, y: %f [key_mod: %x]\n", window_title, deltaX, deltaY, mod);

    mfb_event.Event = PX_OBJECT_EVENT_CURSORWHEEL;
    cursor_x = deltaX;
    cursor_y = deltaY;
    cursor_z = deltaZ;
    PX_Object_Event_SetCursorX(&mfb_event, cursor_x * cursor_x_scale);
    PX_Object_Event_SetCursorY(&mfb_event, cursor_y * cursor_y_scale);
    PX_Object_Event_SetCursorZ(&mfb_event, cursor_z);

    PX_ApplicationPostEvent(&App, mfb_event);
}

// ------------------------------------
px_void PX_app_thread_func(px_void* ptr) {
    unsigned int elapsed = 0;
    unsigned int width, height = 0;
    void* renderBuffer = NULL;
    px_surface* pRenderSurface = NULL;

    LastDownPoint.x = -1;
    LastDownPoint.y = -1;
    reset_cursor_scale();

    // Register event
    mfb_set_active_callback(window, active);
    mfb_set_resize_callback(window, resize);
    mfb_set_close_callback(window, window_close);
    mfb_set_keyboard_callback(window, keyboard);
    mfb_set_char_input_callback(window, char_input);
    mfb_set_mouse_button_callback(window, mouse_btn);
    mfb_set_mouse_move_callback(window, mouse_move);
    mfb_set_mouse_drag_callback(window, mouse_drag);
    mfb_set_mouse_scroll_callback(window, mouse_scroll);

    mfb_set_user_data(window, (void*)"Input Events");

    while (PX_TRUE) {
        if (mfb_window_timer_reset(window) != STATE_OK) window = NULL;

        elapsed = mfb_get_window_elapsed(window);
        pRenderSurface = &App.runtime.RenderSurface;
        renderBuffer = pRenderSurface->surfaceBuffer;
        width = App.runtime.surface_width;
        height = App.runtime.surface_height;

        pthread_mutex_lock(&_eventMutex);
        PX_PressKeysUpdate();
        PX_ApplicationUpdate(&App, elapsed);
        PX_ApplicationRender(&App, elapsed);
        pthread_mutex_unlock(&_eventMutex);

        if (mfb_window_update(window, renderBuffer, width, height) != STATE_OK) window = NULL;
    }
}

// ------------------------------------
px_void PX_PressKeysUpdate() {
    // check keyboard press keys
    for (unsigned int i = 0; i < sizeof(mfb_press_keys) / sizeof(mfb_press_keys[0]); i++) {
        mfb_key press_key = mfb_press_keys[i];
        if (mfb_get_key_status(window, press_key)) {
            mfb_event.Event = PX_OBJECT_EVENT_KEYDOWN;
            PX_Object_Event_SetKeyDown(&mfb_event, (px_uint)press_key);
            PX_ApplicationPostEvent(&App, mfb_event);
        }
    }
}

// ------------------------------------
int PX_SystemLoop() {
    pthread_mutex_init(&_eventMutex, NULL);

    // Start rendering thread
    PX_ThreadCreate(&mfb_appThread, PX_app_thread_func, NULL);

    // Start main thread
    mfb_wait_sync(window);

    // Destroy rendering thread
    PX_ThreadDestroy(&mfb_appThread);

    pthread_mutex_destroy(&_eventMutex);
    return PX_TRUE;
}
