#ifndef PX_KEYBOARD_MODULE_H
#define PX_KEYBOARD_MODULE_H

/*
 * Standard virtual-key (VK) code macros (PX_VK_*) are defined in
 * core/PX_Keyboard.h and reused here as-is. The numeric values follow
 * the Win32 virtual-key convention so the same vk_code works on every
 * platform implementation of PX_KeyBoardKeyDown.
 */
#include "../../core/PX_Typedef.h"

/*
 * Returns 1 if the given virtual-key is currently held down on the
 * physical keyboard, 0 otherwise. Each platform implements this in
 * platform/<platform>/px_keyboard.c.
 */
int PX_KeyBoardKeyDown(unsigned char vk_code);

#ifdef __EMSCRIPTEN__
void PX_KeyBoardSetState(unsigned char vk_code, int down);
#endif

#endif /* PX_KEYBOARD_MODULE_H */
