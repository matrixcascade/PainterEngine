#ifndef PLATFORM_WINDOWS
#define PLATFORM_WINDOWS

#include <Windows.h>
#pragma comment(lib, "User32.lib")
#pragma comment(lib, "Shell32.lib")
#pragma comment(lib, "Ole32.lib")
#pragma comment(lib, "winmm.lib")

#define PX_DIK_ESCAPE          0x01
#define PX_DIK_1               0x02
#define PX_DIK_2               0x03
#define PX_DIK_3               0x04
#define PX_DIK_4               0x05
#define PX_DIK_5               0x06
#define PX_DIK_6               0x07
#define PX_DIK_7               0x08
#define PX_DIK_8               0x09
#define PX_DIK_9               0x0A
#define PX_DIK_0               0x0B
#define PX_DIK_MINUS           0x0C    /* - on main keyboard */
#define PX_DIK_EQUALS          0x0D
#define PX_DIK_BACK            0x0E    /* backspace */
#define PX_DIK_TAB             0x0F
#define PX_DIK_Q               0x10
#define PX_DIK_W               0x11
#define PX_DIK_E               0x12
#define PX_DIK_R               0x13
#define PX_DIK_T               0x14
#define PX_DIK_Y               0x15
#define PX_DIK_U               0x16
#define PX_DIK_I               0x17
#define PX_DIK_O               0x18
#define PX_DIK_P               0x19
#define PX_DIK_LBRACKET        0x1A
#define PX_DIK_RBRACKET        0x1B
#define PX_DIK_RETURN          0x1C    /* Enter on main keyboard */
#define PX_DIK_LCONTROL        0x1D
#define PX_DIK_A               0x1E
#define PX_DIK_S               0x1F
#define PX_DIK_D               0x20
#define PX_DIK_F               0x21
#define PX_DIK_G               0x22
#define PX_DIK_H               0x23
#define PX_DIK_J               0x24
#define PX_DIK_K               0x25
#define PX_DIK_L               0x26
#define PX_DIK_SEMICOLON       0x27
#define PX_DIK_APOSTROPHE      0x28
#define PX_DIK_GRAVE           0x29    /* accent grave */
#define PX_DIK_LSHIFT          0x2A
#define PX_DIK_BACKSLASH       0x2B
#define PX_DIK_Z               0x2C
#define PX_DIK_X               0x2D
#define PX_DIK_C               0x2E
#define PX_DIK_V               0x2F
#define PX_DIK_B               0x30
#define PX_DIK_N               0x31
#define PX_DIK_M               0x32
#define PX_DIK_COMMA           0x33
#define PX_DIK_PERIOD          0x34    /* . on main keyboard */
#define PX_DIK_SLASH           0x35    /* / on main keyboard */
#define PX_DIK_RSHIFT          0x36
#define PX_DIK_MULTIPLY        0x37    /* * on numeric keypad */
#define PX_DIK_LMENU           0x38    /* left Alt */
#define PX_DIK_SPACE           0x39
#define PX_DIK_CAPITAL         0x3A
#define PX_DIK_F1              0x3B
#define PX_DIK_F2              0x3C
#define PX_DIK_F3              0x3D
#define PX_DIK_F4              0x3E
#define PX_DIK_F5              0x3F
#define PX_DIK_F6              0x40
#define PX_DIK_F7              0x41
#define PX_DIK_F8              0x42
#define PX_DIK_F9              0x43
#define PX_DIK_F10             0x44
#define PX_DIK_NUMLOCK         0x45
#define PX_DIK_SCROLL          0x46    /* Scroll Lock */
#define PX_DIK_NUMPAD7         0x47
#define PX_DIK_NUMPAD8         0x48
#define PX_DIK_NUMPAD9         0x49
#define PX_DIK_SUBTRACT        0x4A    /* - on numeric keypad */
#define PX_DIK_NUMPAD4         0x4B
#define PX_DIK_NUMPAD5         0x4C
#define PX_DIK_NUMPAD6         0x4D
#define PX_DIK_ADD             0x4E    /* + on numeric keypad */
#define PX_DIK_NUMPAD1         0x4F
#define PX_DIK_NUMPAD2         0x50
#define PX_DIK_NUMPAD3         0x51
#define PX_DIK_NUMPAD0         0x52
#define PX_DIK_DECIMAL         0x53    /* . on numeric keypad */
#define PX_DIK_OEM_102         0x56    /* <> or \| on RT 102-key keyboard (Non-U.S.) */
#define PX_DIK_F11             0x57
#define PX_DIK_F12             0x58
#define PX_DIK_F13             0x64    /*                     (NEC PC98) */
#define PX_DIK_F14             0x65    /*                     (NEC PC98) */
#define PX_DIK_F15             0x66    /*                     (NEC PC98) */
#define PX_DIK_KANA            0x70    /* (Japanese keyboard)            */
#define PX_DIK_ABNT_C1         0x73    /* /? on Brazilian keyboard */
#define PX_DIK_CONVERT         0x79    /* (Japanese keyboard)            */
#define PX_DIK_NOCONVERT       0x7B    /* (Japanese keyboard)            */
#define PX_DIK_YEN             0x7D    /* (Japanese keyboard)            */
#define PX_DIK_ABNT_C2         0x7E    /* Numpad . on Brazilian keyboard */
#define PX_DIK_NUMPADEQUALS    0x8D    /* = on numeric keypad (NEC PC98) */
#define PX_DIK_PREVTRACK       0x90    /* Previous Track (PX_DIK_CIRCUMFLEX on Japanese keyboard) */
#define PX_DIK_AT              0x91    /*                     (NEC PC98) */
#define PX_DIK_COLON           0x92    /*                     (NEC PC98) */
#define PX_DIK_UNDERLINE       0x93    /*                     (NEC PC98) */
#define PX_DIK_KANJI           0x94    /* (Japanese keyboard)            */
#define PX_DIK_STOP            0x95    /*                     (NEC PC98) */
#define PX_DIK_AX              0x96    /*                     (Japan AX) */
#define PX_DIK_UNLABELED       0x97    /*                        (J3100) */
#define PX_DIK_NEXTTRACK       0x99    /* Next Track */
#define PX_DIK_NUMPADENTER     0x9C    /* Enter on numeric keypad */
#define PX_DIK_RCONTROL        0x9D
#define PX_DIK_MUTE            0xA0    /* Mute */
#define PX_DIK_CALCULATOR      0xA1    /* Calculator */
#define PX_DIK_PLAYPAUSE       0xA2    /* Play / Pause */
#define PX_DIK_MEDIASTOP       0xA4    /* Media Stop */
#define PX_DIK_VOLUMEDOWN      0xAE    /* Volume - */
#define PX_DIK_VOLUMEUP        0xB0    /* Volume + */
#define PX_DIK_WEBHOME         0xB2    /* Web home */
#define PX_DIK_NUMPADCOMMA     0xB3    /* , on numeric keypad (NEC PC98) */
#define PX_DIK_DIVIDE          0xB5    /* / on numeric keypad */
#define PX_DIK_SYSRQ           0xB7
#define PX_DIK_RMENU           0xB8    /* right Alt */
#define PX_DIK_PAUSE           0xC5    /* Pause */
#define PX_DIK_HOME            0xC7    /* Home on arrow keypad */
#define PX_DIK_UP              0xC8    /* UpArrow on arrow keypad */
#define PX_DIK_PRIOR           0xC9    /* PgUp on arrow keypad */
#define PX_DIK_LEFT            0xCB    /* LeftArrow on arrow keypad */
#define PX_DIK_RIGHT           0xCD    /* RightArrow on arrow keypad */
#define PX_DIK_END             0xCF    /* End on arrow keypad */
#define PX_DIK_DOWN            0xD0    /* DownArrow on arrow keypad */
#define PX_DIK_NEXT            0xD1    /* PgDn on arrow keypad */
#define PX_DIK_INSERT          0xD2    /* Insert on arrow keypad */
#define PX_DIK_DELETE          0xD3    /* Delete on arrow keypad */
#define PX_DIK_LWIN            0xDB    /* Left Windows key */
#define PX_DIK_RWIN            0xDC    /* Right Windows key */
#define PX_DIK_APPS            0xDD    /* AppMenu key */
#define PX_DIK_POWER           0xDE    /* System Power */
#define PX_DIK_SLEEP           0xDF    /* System Sleep */
#define PX_DIK_WAKE            0xE3    /* System Wake */
#define PX_DIK_WEBSEARCH       0xE5    /* Web Search */
#define PX_DIK_WEBFAVORITES    0xE6    /* Web Favorites */
#define PX_DIK_WEBREFRESH      0xE7    /* Web Refresh */
#define PX_DIK_WEBSTOP         0xE8    /* Web Stop */
#define PX_DIK_WEBFORWARD      0xE9    /* Web Forward */
#define PX_DIK_WEBBACK         0xEA    /* Web Back */
#define PX_DIK_MYCOMPUTER      0xEB    /* My Computer */
#define PX_DIK_MAIL            0xEC    /* Mail */
#define PX_DIK_MEDIASELECT     0xED    /* Media Select */

typedef struct
{
	UINT uMsg;
	LPARAM lparam;
	WPARAM wparam;
} WM_MESSAGE;

BOOL PX_CreateWindow(int Width, int Height, const char *name, BOOL bfullScreen);
HWND PX_GetWindowHwnd(void);
VOID PX_SystemReadDeviceState(void);
BOOL PX_SystemLoop(void);
BOOL PX_SystemRender(void *raw, int width, int height);
BOOL PX_SystemIsActivated(void);
BOOL PX_KeyboardDown(unsigned char X);
char *PX_KeyboardString(void);
char *PX_DragfileString(void);
BOOL PX_MouseLButtonDown(void);
BOOL PX_MouseRButtonDown(void);
BOOL PX_MouseMButtonDown(void);
POINT PX_MousePosition(void);
BOOL PX_KeyDown(unsigned char key);
BOOL PX_MouseWheel(int *x, int *y, int *delta);
BOOL PX_GetWinMessage(WM_MESSAGE *Msg);
char *PX_OpenFileDialog(const char Filter[]);
char *PX_MultFileDialog(const char Filter[]);
char *PX_GetFileName(const char filePath[]);
char *PX_SaveFileDialog(const char Filter[], const char ext[]);
#endif
