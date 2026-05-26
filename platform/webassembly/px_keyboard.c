#include "../modules/px_keyboard.h"

#include <SDL.h>

static px_byte px_key_state[256];

void PX_KeyBoardSetState(unsigned char vk_code, int down)
{
    px_key_state[vk_code] = down ? 1 : 0;
}

int PX_KeyBoardKeyDown(unsigned char vk_code)
{
    if (vk_code == PX_VK_LBUTTON || vk_code == PX_VK_RBUTTON || vk_code == PX_VK_MBUTTON)
    {
        px_dword mouse = SDL_GetMouseState(NULL, NULL);
        if (vk_code == PX_VK_LBUTTON) return (mouse & SDL_BUTTON(SDL_BUTTON_LEFT))   ? 1 : 0;
        if (vk_code == PX_VK_RBUTTON) return (mouse & SDL_BUTTON(SDL_BUTTON_RIGHT))  ? 1 : 0;
        if (vk_code == PX_VK_MBUTTON) return (mouse & SDL_BUTTON(SDL_BUTTON_MIDDLE)) ? 1 : 0;
    }
    return px_key_state[vk_code];
}
