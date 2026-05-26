#include "../modules/px_keyboard.h"

#include <ApplicationServices/ApplicationServices.h>

/*
 * Mapping of PX_VK_* virtual-key codes to macOS HIToolbox kVK_* hardware
 * key codes. -1 means "no mapping". Mouse buttons are handled separately
 * via CGEventSourceButtonState below.
 */
static int PX_VKToCGKeyCode(unsigned char vk)
{
    switch (vk)
    {
        case PX_VK_BACK:      return 0x33; /* kVK_Delete (Backspace) */
        case PX_VK_TAB:       return 0x30;
        case PX_VK_RETURN:    return 0x24;
        case PX_VK_SHIFT:     return 0x38;
        case PX_VK_CONTROL:   return 0x3B;
        case PX_VK_MENU:      return 0x3A; /* Option */
        case PX_VK_CAPITAL:   return 0x39;
        case PX_VK_ESCAPE:    return 0x35;
        case PX_VK_SPACE:     return 0x31;
        case PX_VK_PRIOR:     return 0x74; /* PageUp */
        case PX_VK_NEXT:      return 0x79; /* PageDown */
        case PX_VK_END:       return 0x77;
        case PX_VK_HOME:      return 0x73;
        case PX_VK_LEFT:      return 0x7B;
        case PX_VK_UP:        return 0x7E;
        case PX_VK_RIGHT:     return 0x7C;
        case PX_VK_DOWN:      return 0x7D;
        case PX_VK_SNAPSHOT:  return 0x69; /* F13 ~= PrintScreen */
        case PX_VK_INSERT:    return 0x72; /* Help on mac */
        case PX_VK_DELETE:    return 0x75; /* Forward Delete */
        case PX_VK_HELP:      return 0x72;
        case PX_VK_LWIN:      return 0x37; /* Command */
        case PX_VK_RWIN:      return 0x36; /* Right Command */

        case PX_VK_0:         return 0x1D;
        case PX_VK_1:         return 0x12;
        case PX_VK_2:         return 0x13;
        case PX_VK_3:         return 0x14;
        case PX_VK_4:         return 0x15;
        case PX_VK_5:         return 0x17;
        case PX_VK_6:         return 0x16;
        case PX_VK_7:         return 0x1A;
        case PX_VK_8:         return 0x1C;
        case PX_VK_9:         return 0x19;

        case PX_VK_A:         return 0x00;
        case PX_VK_B:         return 0x0B;
        case PX_VK_C:         return 0x08;
        case PX_VK_D:         return 0x02;
        case PX_VK_E:         return 0x0E;
        case PX_VK_F:         return 0x03;
        case PX_VK_G:         return 0x05;
        case PX_VK_H:         return 0x04;
        case PX_VK_I:         return 0x22;
        case PX_VK_J:         return 0x26;
        case PX_VK_K:         return 0x28;
        case PX_VK_L:         return 0x25;
        case PX_VK_M:         return 0x2E;
        case PX_VK_N:         return 0x2D;
        case PX_VK_O:         return 0x1F;
        case PX_VK_P:         return 0x23;
        case PX_VK_Q:         return 0x0C;
        case PX_VK_R:         return 0x0F;
        case PX_VK_S:         return 0x01;
        case PX_VK_T:         return 0x11;
        case PX_VK_U:         return 0x20;
        case PX_VK_V:         return 0x09;
        case PX_VK_W:         return 0x0D;
        case PX_VK_X:         return 0x07;
        case PX_VK_Y:         return 0x10;
        case PX_VK_Z:         return 0x06;

        case PX_VK_NUMPAD0:   return 0x52;
        case PX_VK_NUMPAD1:   return 0x53;
        case PX_VK_NUMPAD2:   return 0x54;
        case PX_VK_NUMPAD3:   return 0x55;
        case PX_VK_NUMPAD4:   return 0x56;
        case PX_VK_NUMPAD5:   return 0x57;
        case PX_VK_NUMPAD6:   return 0x58;
        case PX_VK_NUMPAD7:   return 0x59;
        case PX_VK_NUMPAD8:   return 0x5B;
        case PX_VK_NUMPAD9:   return 0x5C;
        case PX_VK_MULTIPLY:  return 0x43;
        case PX_VK_ADD:       return 0x45;
        case PX_VK_SUBTRACT:  return 0x4E;
        case PX_VK_DECIMAL:   return 0x41;
        case PX_VK_DIVIDE:    return 0x4B;

        case PX_VK_F1:        return 0x7A;
        case PX_VK_F2:        return 0x78;
        case PX_VK_F3:        return 0x63;
        case PX_VK_F4:        return 0x76;
        case PX_VK_F5:        return 0x60;
        case PX_VK_F6:        return 0x61;
        case PX_VK_F7:        return 0x62;
        case PX_VK_F8:        return 0x64;
        case PX_VK_F9:        return 0x65;
        case PX_VK_F10:       return 0x6D;
        case PX_VK_F11:       return 0x67;
        case PX_VK_F12:       return 0x6F;
        case PX_VK_F13:       return 0x69;
        case PX_VK_F14:       return 0x6B;
        case PX_VK_F15:       return 0x71;
        case PX_VK_F16:       return 0x6A;
        case PX_VK_F17:       return 0x40;
        case PX_VK_F18:       return 0x4F;
        case PX_VK_F19:       return 0x50;
        case PX_VK_F20:       return 0x5A;

        case PX_VK_LSHIFT:    return 0x38;
        case PX_VK_RSHIFT:    return 0x3C;
        case PX_VK_LCONTROL:  return 0x3B;
        case PX_VK_RCONTROL:  return 0x3E;
        case PX_VK_LMENU:     return 0x3A;
        case PX_VK_RMENU:     return 0x3D;

        case PX_VK_OEM_1:     return 0x29; /* ; */
        case PX_VK_OEM_PLUS:  return 0x18; /* = */
        case PX_VK_OEM_COMMA: return 0x2B;
        case PX_VK_OEM_MINUS: return 0x1B;
        case PX_VK_OEM_PERIOD:return 0x2F;
        case PX_VK_OEM_2:     return 0x2C; /* / */
        case PX_VK_OEM_3:     return 0x32; /* ` */
        case PX_VK_OEM_4:     return 0x21; /* [ */
        case PX_VK_OEM_5:     return 0x2A; /* \ */
        case PX_VK_OEM_6:     return 0x1E; /* ] */
        case PX_VK_OEM_7:     return 0x27; /* ' */
    }
    return -1;
}

int PX_KeyBoardKeyDown(unsigned char vk_code)
{
    int key;
    if (vk_code == PX_VK_LBUTTON)
        return CGEventSourceButtonState(kCGEventSourceStateHIDSystemState, kCGMouseButtonLeft) ? 1 : 0;
    if (vk_code == PX_VK_RBUTTON)
        return CGEventSourceButtonState(kCGEventSourceStateHIDSystemState, kCGMouseButtonRight) ? 1 : 0;
    if (vk_code == PX_VK_MBUTTON)
        return CGEventSourceButtonState(kCGEventSourceStateHIDSystemState, kCGMouseButtonCenter) ? 1 : 0;

    key = PX_VKToCGKeyCode(vk_code);
    if (key < 0) return 0;
    return CGEventSourceKeyState(kCGEventSourceStateHIDSystemState, (CGKeyCode)key) ? 1 : 0;
}
