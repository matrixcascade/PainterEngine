#include "platform/modules/px_keyboard.h"

#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <stddef.h>

static Display *g_px_kb_display = NULL;

static unsigned int PX_VKToKeySym(unsigned char vk)
{
    /* Letters: VK 'A'-'Z' map directly to ASCII */
    if (vk >= PX_VK_A && vk <= PX_VK_Z) return (unsigned int)(XK_a + (vk - PX_VK_A));
    /* Top-row digits: VK '0'-'9' map directly to ASCII */
    if (vk >= PX_VK_0 && vk <= PX_VK_9) return (unsigned int)(XK_0 + (vk - PX_VK_0));

    switch (vk)
    {
        case PX_VK_BACK:      return XK_BackSpace;
        case PX_VK_TAB:       return XK_Tab;
        case PX_VK_CLEAR:     return XK_Clear;
        case PX_VK_RETURN:    return XK_Return;
        case PX_VK_SHIFT:     return XK_Shift_L;
        case PX_VK_CONTROL:   return XK_Control_L;
        case PX_VK_MENU:      return XK_Alt_L;
        case PX_VK_PAUSE:     return XK_Pause;
        case PX_VK_CAPITAL:   return XK_Caps_Lock;
        case PX_VK_ESCAPE:    return XK_Escape;
        case PX_VK_SPACE:     return XK_space;
        case PX_VK_PRIOR:     return XK_Page_Up;
        case PX_VK_NEXT:      return XK_Page_Down;
        case PX_VK_END:       return XK_End;
        case PX_VK_HOME:      return XK_Home;
        case PX_VK_LEFT:      return XK_Left;
        case PX_VK_UP:        return XK_Up;
        case PX_VK_RIGHT:     return XK_Right;
        case PX_VK_DOWN:      return XK_Down;
        case PX_VK_SELECT:    return XK_Select;
        case PX_VK_PRINT:     return XK_Print;
        case PX_VK_EXECUTE:   return XK_Execute;
        case PX_VK_SNAPSHOT:  return XK_Print;
        case PX_VK_INSERT:    return XK_Insert;
        case PX_VK_DELETE:    return XK_Delete;
        case PX_VK_HELP:      return XK_Help;
        case PX_VK_LWIN:      return XK_Super_L;
        case PX_VK_RWIN:      return XK_Super_R;
        case PX_VK_APPS:      return XK_Menu;
        case PX_VK_NUMPAD0:   return XK_KP_0;
        case PX_VK_NUMPAD1:   return XK_KP_1;
        case PX_VK_NUMPAD2:   return XK_KP_2;
        case PX_VK_NUMPAD3:   return XK_KP_3;
        case PX_VK_NUMPAD4:   return XK_KP_4;
        case PX_VK_NUMPAD5:   return XK_KP_5;
        case PX_VK_NUMPAD6:   return XK_KP_6;
        case PX_VK_NUMPAD7:   return XK_KP_7;
        case PX_VK_NUMPAD8:   return XK_KP_8;
        case PX_VK_NUMPAD9:   return XK_KP_9;
        case PX_VK_MULTIPLY:  return XK_KP_Multiply;
        case PX_VK_ADD:       return XK_KP_Add;
        case PX_VK_SEPARATOR: return XK_KP_Separator;
        case PX_VK_SUBTRACT:  return XK_KP_Subtract;
        case PX_VK_DECIMAL:   return XK_KP_Decimal;
        case PX_VK_DIVIDE:    return XK_KP_Divide;
        case PX_VK_F1:        return XK_F1;
        case PX_VK_F2:        return XK_F2;
        case PX_VK_F3:        return XK_F3;
        case PX_VK_F4:        return XK_F4;
        case PX_VK_F5:        return XK_F5;
        case PX_VK_F6:        return XK_F6;
        case PX_VK_F7:        return XK_F7;
        case PX_VK_F8:        return XK_F8;
        case PX_VK_F9:        return XK_F9;
        case PX_VK_F10:       return XK_F10;
        case PX_VK_F11:       return XK_F11;
        case PX_VK_F12:       return XK_F12;
        case PX_VK_F13:       return XK_F13;
        case PX_VK_F14:       return XK_F14;
        case PX_VK_F15:       return XK_F15;
        case PX_VK_F16:       return XK_F16;
        case PX_VK_F17:       return XK_F17;
        case PX_VK_F18:       return XK_F18;
        case PX_VK_F19:       return XK_F19;
        case PX_VK_F20:       return XK_F20;
        case PX_VK_F21:       return XK_F21;
        case PX_VK_F22:       return XK_F22;
        case PX_VK_F23:       return XK_F23;
        case PX_VK_F24:       return XK_F24;
        case PX_VK_NUMLOCK:   return XK_Num_Lock;
        case PX_VK_SCROLL:    return XK_Scroll_Lock;
        case PX_VK_LSHIFT:    return XK_Shift_L;
        case PX_VK_RSHIFT:    return XK_Shift_R;
        case PX_VK_LCONTROL:  return XK_Control_L;
        case PX_VK_RCONTROL:  return XK_Control_R;
        case PX_VK_LMENU:     return XK_Alt_L;
        case PX_VK_RMENU:     return XK_Alt_R;
        case PX_VK_OEM_1:     return XK_semicolon;
        case PX_VK_OEM_PLUS:  return XK_equal;
        case PX_VK_OEM_COMMA: return XK_comma;
        case PX_VK_OEM_MINUS: return XK_minus;
        case PX_VK_OEM_PERIOD:return XK_period;
        case PX_VK_OEM_2:     return XK_slash;
        case PX_VK_OEM_3:     return XK_grave;
        case PX_VK_OEM_4:     return XK_bracketleft;
        case PX_VK_OEM_5:     return XK_backslash;
        case PX_VK_OEM_6:     return XK_bracketright;
        case PX_VK_OEM_7:     return XK_apostrophe;
    }
    return 0;
}

int PX_KeyBoardKeyDown(unsigned char vk_code)
{
    char keymap[32];
    KeySym keysym;
    KeyCode keycode;

    if (g_px_kb_display == NULL)
    {
        g_px_kb_display = XOpenDisplay(NULL);
        if (g_px_kb_display == NULL) return 0;
    }

    /* Mouse buttons can't be queried through XQueryKeymap. */
    if (vk_code == PX_VK_LBUTTON || vk_code == PX_VK_RBUTTON || vk_code == PX_VK_MBUTTON)
    {
        Window root_ret, child_ret;
        int root_x, root_y, win_x, win_y;
        unsigned int mask = 0;
        Window root = DefaultRootWindow(g_px_kb_display);
        if (!XQueryPointer(g_px_kb_display, root, &root_ret, &child_ret,
                           &root_x, &root_y, &win_x, &win_y, &mask))
        {
            return 0;
        }
        if (vk_code == PX_VK_LBUTTON) return (mask & Button1Mask) ? 1 : 0;
        if (vk_code == PX_VK_MBUTTON) return (mask & Button2Mask) ? 1 : 0;
        if (vk_code == PX_VK_RBUTTON) return (mask & Button3Mask) ? 1 : 0;
    }

    keysym = (KeySym)PX_VKToKeySym(vk_code);
    if (keysym == 0) return 0;

    keycode = XKeysymToKeycode(g_px_kb_display, keysym);
    if (keycode == 0) return 0;

    XQueryKeymap(g_px_kb_display, keymap);
    return (keymap[keycode >> 3] & (1 << (keycode & 7))) ? 1 : 0;
}
