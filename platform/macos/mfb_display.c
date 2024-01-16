
#include "mfb_display.h"
#include "platform/modules/px_thread.h"

// ------------------------------------
struct mfb_opaque_window* window;

// ------------------------------------
int PX_CreateWindow(int surfaceWidth, int surfaceHeight, int windowWidth, int windowHeight, const char* name) {
    window = mfb_open_ex(name, windowWidth, windowHeight, 0);
    return 1;
}

// ------------------------------------
int PX_GetScreenHeight() { return mfb_get_screen_height(); }

// ------------------------------------
int PX_GetScreenWidth() { return mfb_get_screen_width(); }
