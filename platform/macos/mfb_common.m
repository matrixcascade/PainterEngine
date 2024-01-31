
// Copyright (c) 2014 Daniel Collin. All rights reserved.
// https://github.com/emoon/minifb MIT License

#include "mfb_common.h"
#include "mfb_window.h"
#include "mfb_display.h"

// ------------------------------------
void mfb_set_active_callback(struct mfb_opaque_window* window, mfb_active_func callback) {
    if (window != 0x0) {
        mfb_window_data* window_data = (mfb_window_data*)window;
        window_data->active_func = callback;
    }
}

// ------------------------------------
void mfb_set_resize_callback(struct mfb_opaque_window* window, mfb_resize_func callback) {
    if (window != 0x0) {
        mfb_window_data* window_data = (mfb_window_data*)window;
        window_data->resize_func = callback;
    }
}

// ------------------------------------
void mfb_set_close_callback(struct mfb_opaque_window* window, mfb_close_func callback) {
    if (window != 0x0) {
        mfb_window_data* window_data = (mfb_window_data*)window;
        window_data->close_func = callback;
    }
}

// ------------------------------------
void mfb_set_keyboard_callback(struct mfb_opaque_window* window, mfb_keyboard_func callback) {
    if (window != 0x0) {
        mfb_window_data* window_data = (mfb_window_data*)window;
        window_data->keyboard_func = callback;
    }
}

// ------------------------------------
void mfb_set_char_input_callback(struct mfb_opaque_window* window, mfb_char_input_func callback) {
    if (window != 0x0) {
        mfb_window_data* window_data = (mfb_window_data*)window;
        window_data->char_input_func = callback;
    }
}

// ------------------------------------
void mfb_set_mouse_button_callback(struct mfb_opaque_window* window, mfb_mouse_button_func callback) {
    if (window != 0x0) {
        mfb_window_data* window_data = (mfb_window_data*)window;
        window_data->mouse_btn_func = callback;
    }
}

// ------------------------------------
void mfb_set_mouse_move_callback(struct mfb_opaque_window* window, mfb_mouse_move_func callback) {
    if (window != 0x0) {
        mfb_window_data* window_data = (mfb_window_data*)window;
        window_data->mouse_move_func = callback;
    }
}

// ------------------------------------
void mfb_set_mouse_drag_callback(struct mfb_opaque_window* window, mfb_mouse_drag_func callback) {
    if (window != 0x0) {
        mfb_window_data* window_data = (mfb_window_data*)window;
        window_data->mouse_drag_func = callback;
    }
}

// ------------------------------------
void mfb_set_mouse_scroll_callback(struct mfb_opaque_window* window, mfb_mouse_scroll_func callback) {
    if (window != 0x0) {
        mfb_window_data* window_data = (mfb_window_data*)window;
        window_data->mouse_wheel_func = callback;
    }
}

// ------------------------------------
void mfb_set_user_data(struct mfb_opaque_window* window, void* user_data) {
    if (window != 0x0) {
        mfb_window_data* window_data = (mfb_window_data*)window;
        window_data->user_data = user_data;
    }
}

// ------------------------------------
void* mfb_get_user_data(struct mfb_opaque_window* window) {
    if (window != 0x0) {
        mfb_window_data* window_data = (mfb_window_data*)window;
        return window_data->user_data;
    }

    return 0x0;
}

// ------------------------------------
void mfb_window_close(struct mfb_opaque_window* window) {
    if (window != 0x0) {
        mfb_window_data* window_data = (mfb_window_data*)window;
        window_data->close = true;
    }
}

// ------------------------------------
void keyboard_default(struct mfb_opaque_window* window, mfb_key key, mfb_key_mod mod, int isPressed) {
    kUnused(mod);
    kUnused(isPressed);
    if (key == KB_KEY_ESCAPE) {
        mfb_window_data* window_data = (mfb_window_data*)window;
        if (!window_data->close_func || window_data->close_func((struct mfb_opaque_window*)window_data)) {
            window_data->close = true;
        }
    }
}

// ------------------------------------
int mfb_set_viewport_best_fit(struct mfb_opaque_window* window, unsigned old_width, unsigned old_height) {
    if (window != 0x0) {
        mfb_window_data* window_data = (mfb_window_data*)window;

        unsigned new_width = window_data->window_width;
        unsigned new_height = window_data->window_height;

        float scale_x = new_width / (float)old_width;
        float scale_y = new_height / (float)old_height;
        if (scale_x >= scale_y)
            scale_x = scale_y;
        else
            scale_y = scale_x;

        unsigned finalWidth = (old_width * scale_x) + 0.5f;
        unsigned finalHeight = (old_height * scale_y) + 0.5f;

        unsigned offset_x = (new_width - finalWidth) >> 1;
        unsigned offset_y = (new_height - finalHeight) >> 1;

        mfb_get_monitor_scale(window, &scale_x, &scale_y);
        return mfb_set_viewport(window, offset_x / scale_x, offset_y / scale_y, finalWidth / scale_x, finalHeight / scale_y);
    }

    return false;
}

// ------------------------------------
int mfb_is_window_active(struct mfb_opaque_window* window) {
    if (window != 0x0) {
        mfb_window_data* window_data = (mfb_window_data*)window;
        return window_data->is_active;
    }
    return false;
}

// ------------------------------------
double mfb_get_window_elapsed(struct mfb_opaque_window* window) {
    double elapsed = 0;
    if (window != 0x0) {
        mfb_window_data* window_data = (mfb_window_data*)window;
        elapsed = window_data->elapsed * 1000;
        window_data->elapsed = (elapsed - (unsigned int)elapsed) / 1000;
        return elapsed;
    }
    return 0;
}

// ------------------------------------
unsigned mfb_get_window_width(struct mfb_opaque_window* window) {
    if (window != 0x0) {
        mfb_window_data* window_data = (mfb_window_data*)window;
        return window_data->window_width;
    }
    return 0;
}

// ------------------------------------
unsigned mfb_get_window_height(struct mfb_opaque_window* window) {
    if (window != 0x0) {
        mfb_window_data* window_data = (mfb_window_data*)window;
        return window_data->window_height;
    }
    return 0;
}

// ------------------------------------
int mfb_get_mouse_x(struct mfb_opaque_window* window) {
    if (window != 0x0) {
        mfb_window_data* window_data = (mfb_window_data*)window;
        return window_data->mouse_pos_x;
    }
    return 0;
}

// ------------------------------------
int mfb_get_mouse_y(struct mfb_opaque_window* window) {
    if (window != 0x0) {
        mfb_window_data* window_data = (mfb_window_data*)window;
        return window_data->mouse_pos_y;
    }
    return 0;
}

// ------------------------------------
float mfb_get_mouse_scroll_x(struct mfb_opaque_window* window) {
    if (window != 0x0) {
        mfb_window_data* window_data = (mfb_window_data*)window;
        return window_data->mouse_wheel_x;
    }
    return 0;
}

// ------------------------------------
float mfb_get_mouse_scroll_y(struct mfb_opaque_window* window) {
    if (window != 0x0) {
        mfb_window_data* window_data = (mfb_window_data*)window;
        return window_data->mouse_wheel_y;
    }
    return 0;
}

// ------------------------------------
const uint8_t* mfb_get_mouse_button_buffer(struct mfb_opaque_window* window) {
    if (window != 0x0) {
        mfb_window_data* window_data = (mfb_window_data*)window;
        return window_data->mouse_button_status;
    }
    return 0;
}

// ------------------------------------
const uint8_t* mfb_get_key_buffer(struct mfb_opaque_window* window) {
    if (window != 0x0) {
        mfb_window_data* window_data = (mfb_window_data*)window;
        return window_data->key_status;
    }
    return 0;
}

// ------------------------------------
const char* mfb_get_key_name(mfb_key key) {
    switch (key) {
        case KB_KEY_SPACE:
            return "Space";

        // case KB_KEY_APOSTROPHE:
        //     return "Apostrophe";

        // case KB_KEY_COMMA:
        //     return "Comma";

        // case KB_KEY_MINUS:
        //     return "Minus";

        // case KB_KEY_PERIOD:
        //     return "Period";

        // case KB_KEY_SLASH:
        //     return "Slash";

        case KB_KEY_0:
            return "0";

        case KB_KEY_1:
            return "1";

        case KB_KEY_2:
            return "2";

        case KB_KEY_3:
            return "3";

        case KB_KEY_4:
            return "4";

        case KB_KEY_5:
            return "5";

        case KB_KEY_6:
            return "6";

        case KB_KEY_7:
            return "7";

        case KB_KEY_8:
            return "8";

        case KB_KEY_9:
            return "9";

        // case KB_KEY_SEMICOLON:
        //     return "Semicolon";

        // case KB_KEY_EQUAL:
        //     return "Equal";

        case KB_KEY_A:
            return "A";

        case KB_KEY_B:
            return "B";

        case KB_KEY_C:
            return "C";

        case KB_KEY_D:
            return "D";

        case KB_KEY_E:
            return "E";

        case KB_KEY_F:
            return "F";

        case KB_KEY_G:
            return "G";

        case KB_KEY_H:
            return "H";

        case KB_KEY_I:
            return "I";

        case KB_KEY_J:
            return "J";

        case KB_KEY_K:
            return "K";

        case KB_KEY_L:
            return "L";

        case KB_KEY_M:
            return "M";

        case KB_KEY_N:
            return "N";

        case KB_KEY_O:
            return "O";

        case KB_KEY_P:
            return "P";

        case KB_KEY_Q:
            return "Q";

        case KB_KEY_R:
            return "R";

        case KB_KEY_S:
            return "S";

        case KB_KEY_T:
            return "T";

        case KB_KEY_U:
            return "U";

        case KB_KEY_V:
            return "V";

        case KB_KEY_W:
            return "W";

        case KB_KEY_X:
            return "X";

        case KB_KEY_Y:
            return "Y";

        case KB_KEY_Z:
            return "Z";

        // case KB_KEY_LEFT_BRACKET:
        //     return "Left_Bracket";

        // case KB_KEY_BACKSLASH:
        //     return "Backslash";

        // case KB_KEY_RIGHT_BRACKET:
        //     return "Right_Bracket";

        // case KB_KEY_GRAVE_ACCENT:
        //     return "Grave_Accent";

        // case KB_KEY_WORLD_1:
        //     return "World_1";

        // case KB_KEY_WORLD_2:
        //     return "World_2";

        case KB_KEY_ESCAPE:
            return "Escape";

        case KB_KEY_ENTER:
            return "Enter";

        case KB_KEY_TAB:
            return "Tab";

        case KB_KEY_BACKSPACE:
            return "Backspace";

        case KB_KEY_INSERT:
            return "Insert";

        case KB_KEY_DELETE:
            return "Delete";

        case KB_KEY_RIGHT:
            return "Right";

        case KB_KEY_LEFT:
            return "Left";

        case KB_KEY_DOWN:
            return "Down";

        case KB_KEY_UP:
            return "Up";

        // case KB_KEY_PAGE_UP:
        //     return "Page_Up";

        // case KB_KEY_PAGE_DOWN:
        //     return "Page_Down";

        case KB_KEY_HOME:
            return "Home";

        case KB_KEY_END:
            return "End";

        // case KB_KEY_CAPS_LOCK:
        //     return "Caps_Lock";

        case KB_KEY_SCROLL_LOCK:
            return "Scroll_Lock";

        case KB_KEY_NUM_LOCK:
            return "Num_Lock";

        case KB_KEY_PRINT_SCREEN:
            return "Print_Screen";

        case KB_KEY_PAUSE:
            return "Pause";

        case KB_KEY_F1:
            return "F1";

        case KB_KEY_F2:
            return "F2";

        case KB_KEY_F3:
            return "F3";

        case KB_KEY_F4:
            return "F4";

        case KB_KEY_F5:
            return "F5";

        case KB_KEY_F6:
            return "F6";

        case KB_KEY_F7:
            return "F7";

        case KB_KEY_F8:
            return "F8";

        case KB_KEY_F9:
            return "F9";

        case KB_KEY_F10:
            return "F10";

        case KB_KEY_F11:
            return "F11";

        case KB_KEY_F12:
            return "F12";

        case KB_KEY_F13:
            return "F13";

        case KB_KEY_F14:
            return "F14";

        case KB_KEY_F15:
            return "F15";

        case KB_KEY_F16:
            return "F16";

        case KB_KEY_F17:
            return "F17";

        case KB_KEY_F18:
            return "F18";

        case KB_KEY_F19:
            return "F19";

        case KB_KEY_F20:
            return "F20";

        case KB_KEY_F21:
            return "F21";

        case KB_KEY_F22:
            return "F22";

        case KB_KEY_F23:
            return "F23";

        case KB_KEY_F24:
            return "F24";

        // case KB_KEY_F25:
        //     return "F25";

        case KB_KEY_KP_0:
            return "KP_0";

        case KB_KEY_KP_1:
            return "KP_1";

        case KB_KEY_KP_2:
            return "KP_2";

        case KB_KEY_KP_3:
            return "KP_3";

        case KB_KEY_KP_4:
            return "KP_4";

        case KB_KEY_KP_5:
            return "KP_5";

        case KB_KEY_KP_6:
            return "KP_6";

        case KB_KEY_KP_7:
            return "KP_7";

        case KB_KEY_KP_8:
            return "KP_8";

        case KB_KEY_KP_9:
            return "KP_9";

        case KB_KEY_KP_DECIMAL:
            return "KP_Decimal";

        case KB_KEY_KP_DIVIDE:
            return "KP_Divide";

        case KB_KEY_KP_MULTIPLY:
            return "KP_Multiply";

        case KB_KEY_KP_SUBTRACT:
            return "KP_Subtract";

        case KB_KEY_KP_ADD:
            return "KP_Add";

        // case KB_KEY_KP_ENTER:
        //     return "KP_Enter";

        // case KB_KEY_KP_EQUAL:
        //     return "KP_Equal";

        case KB_KEY_LEFT_SHIFT:
            return "Left_Shift";

        case KB_KEY_LEFT_CONTROL:
            return "Left_Control";

        // case KB_KEY_LEFT_ALT:
        //     return "Left_Alt";

        // case KB_KEY_LEFT_SUPER:
        //     return "Left_Super";

        case KB_KEY_RIGHT_SHIFT:
            return "Right_Shift";

        case KB_KEY_RIGHT_CONTROL:
            return "Right_Control";

        // case KB_KEY_RIGHT_ALT:
        //     return "Right_Alt";

        // case KB_KEY_RIGHT_SUPER:
        //     return "Right_Super";

        case KB_KEY_MENU:
            return "Menu";

        case KB_KEY_UNKNOWN:
            return "Unknown";
    }

    return "Unknown";
}

// ------------------------------------
const int mfb_get_key_status(struct mfb_opaque_window* window, int key_index) {
    if (window == 0x0) {
        return 0;
    }
    mfb_window_data* window_data = (mfb_window_data*)window;
    return window_data->key_status[key_index];
}
