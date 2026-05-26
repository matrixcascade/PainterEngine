#include "../modules/px_keyboard.h"

/*
 * Android does not provide a synchronous polling API for the physical
 * keyboard from native code; key events are delivered asynchronously
 * from the Java/Kotlin layer via the activity's onKeyDown/onKeyUp
 * callbacks. We therefore mirror the press/release state into a local
 * 256-byte table and let PX_KeyBoardKeyDown read from it.
 *
 * The Java side (or any C code wired to InputEvent dispatching) should
 * forward state changes through PX_KeyBoardSetKeyState. Until that
 * wiring is in place every query simply returns 0, which matches the
 * "no key pressed" default.
 */
static unsigned char g_px_kb_state[256] = {0};

void PX_KeyBoardSetKeyState(unsigned char vk_code, int isDown)
{
    g_px_kb_state[vk_code] = isDown ? 1 : 0;
}

int PX_KeyBoardKeyDown(unsigned char vk_code)
{
    return g_px_kb_state[vk_code] ? 1 : 0;
}
