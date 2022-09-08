#ifndef PX_KEYBOARD_H
#define PX_KEYBOARD_H
#include "PX_Core.h"


#define PX_VK_LBUTTON        0x01
#define PX_VK_RBUTTON        0x02
#define PX_VK_CANCEL         0x03
#define PX_VK_MBUTTON        0x04    /* NOT contiguous with L & RBUTTON */
/*
 * 0x07 : reserved
 */
#define PX_VK_BACK           0x08
#define PX_VK_TAB            0x09
 /*
  * 0x0A - 0x0B : reserved
  */

#define PX_VK_CLEAR          0x0C
#define PX_VK_RETURN         0x0D
  /*
   * 0x0E - 0x0F : unassigned
   */
#define PX_VK_SHIFT          0x10
#define PX_VK_CONTROL        0x11
#define PX_VK_MENU           0x12
#define PX_VK_PAUSE          0x13
#define PX_VK_CAPITAL        0x14
#define PX_VK_KANA           0x15
#define PX_VK_HANGEUL        0x15  /* old name - should be here for compatibility */
#define PX_VK_HANGUL         0x15
#define PX_VK_IME_ON         0x16
#define PX_VK_JUNJA          0x17
#define PX_VK_FINAL          0x18
#define PX_VK_HANJA          0x19
#define PX_VK_KANJI          0x19
#define PX_VK_IME_OFF        0x1A
#define PX_VK_ESCAPE         0x1B
#define PX_VK_CONVERT        0x1C
#define PX_VK_NONCONVERT     0x1D
#define PX_VK_ACCEPT         0x1E
#define PX_VK_MODECHANGE     0x1F
#define PX_VK_SPACE          0x20
#define PX_VK_PRIOR          0x21
#define PX_VK_NEXT           0x22
#define PX_VK_END            0x23
#define PX_VK_HOME           0x24
#define PX_VK_LEFT           0x25
#define PX_VK_UP             0x26
#define PX_VK_RIGHT          0x27
#define PX_VK_DOWN           0x28
#define PX_VK_SELECT         0x29
#define PX_VK_PRINT          0x2A
#define PX_VK_EXECUTE        0x2B
#define PX_VK_SNAPSHOT       0x2C
#define PX_VK_INSERT         0x2D
#define PX_VK_DELETE         0x2E
#define PX_VK_HELP           0x2F
   /*
	* PX_VK_0 - PX_VK_9 are the same as ASCII '0' - '9' (0x30 - 0x39)
	* 0x3A - 0x40 : unassigned
	* PX_VK_A - PX_VK_Z are the same as ASCII 'A' - 'Z' (0x41 - 0x5A)
	*/
#define PX_VK_LWIN           0x5B
#define PX_VK_RWIN           0x5C
#define PX_VK_APPS           0x5D
	/*
	 * 0x5E : reserved
	 */
#define PX_VK_SLEEP          0x5F
#define PX_VK_NUMPAD0        0x60
#define PX_VK_NUMPAD1        0x61
#define PX_VK_NUMPAD2        0x62
#define PX_VK_NUMPAD3        0x63
#define PX_VK_NUMPAD4        0x64
#define PX_VK_NUMPAD5        0x65
#define PX_VK_NUMPAD6        0x66
#define PX_VK_NUMPAD7        0x67
#define PX_VK_NUMPAD8        0x68
#define PX_VK_NUMPAD9        0x69
#define PX_VK_MULTIPLY       0x6A
#define PX_VK_ADD            0x6B
#define PX_VK_SEPARATOR      0x6C
#define PX_VK_SUBTRACT       0x6D
#define PX_VK_DECIMAL        0x6E
#define PX_VK_DIVIDE         0x6F
#define PX_VK_F1             0x70
#define PX_VK_F2             0x71
#define PX_VK_F3             0x72
#define PX_VK_F4             0x73
#define PX_VK_F5             0x74
#define PX_VK_F6             0x75
#define PX_VK_F7             0x76
#define PX_VK_F8             0x77
#define PX_VK_F9             0x78
#define PX_VK_F10            0x79
#define PX_VK_F11            0x7A
#define PX_VK_F12            0x7B
#define PX_VK_F13            0x7C
#define PX_VK_F14            0x7D
#define PX_VK_F15            0x7E
#define PX_VK_F16            0x7F
#define PX_VK_F17            0x80
#define PX_VK_F18            0x81
#define PX_VK_F19            0x82
#define PX_VK_F20            0x83
#define PX_VK_F21            0x84
#define PX_VK_F22            0x85
#define PX_VK_F23            0x86
#define PX_VK_F24            0x87
#define PX_VK_NUMLOCK        0x90
#define PX_VK_SCROLL         0x91
	 /*
	  * NEC PC-9800 kbd definitions
	  */
#define PX_VK_OEM_NEC_EQUAL  0x92   // '=' key on numpad
	  /*
	   * Fujitsu/OASYS kbd definitions
	   */
#define PX_VK_OEM_FJ_JISHO   0x92   // 'Dictionary' key
#define PX_VK_OEM_FJ_MASSHOU 0x93   // 'Unregister word' key
#define PX_VK_OEM_FJ_TOUROKU 0x94   // 'Register word' key
#define PX_VK_OEM_FJ_LOYA    0x95   // 'Left OYAYUBI' key
#define PX_VK_OEM_FJ_ROYA    0x96   // 'Right OYAYUBI' key
	   /*
		* 0x97 - 0x9F : unassigned
		*/
		/*
		 * PX_VK_L* & PX_VK_R* - left and right Alt, Ctrl and Shift virtual keys.
		 * Used only as parameters to GetAsyncKeyState() and GetKeyState().
		 * No other API or message will distinguish left and right keys in this way.
		 */
#define PX_VK_LSHIFT         0xA0
#define PX_VK_RSHIFT         0xA1
#define PX_VK_LCONTROL       0xA2
#define PX_VK_RCONTROL       0xA3

typedef struct  
{
	px_char content1[8];
	px_char content2[8];
	px_byte vk_code;
}PX_KeyboardKey;

///////////////////////////////////////////////////////////////////////////////
//keyboard
px_byte PX_KeyboardGetScanCode(const px_char content[]);
px_byte PX_KeyboardScanCodeToVK(px_byte scan_code);
px_byte PX_KeyboardVKToScanCode(px_byte vk);
#endif