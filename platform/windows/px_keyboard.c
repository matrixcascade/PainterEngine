#include "../modules/px_keyboard.h"
#include <Windows.h>

int PX_KeyBoardKeyDown(unsigned char vk_code)
{
    return (GetAsyncKeyState((int)vk_code) & 0x8000) ? 1 : 0;
}
