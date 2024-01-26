
#include "px_display.h"
#include "runtime/PainterEngine_Application.h"

// ------------------------------------
int main(int argc, char** argv) {
#ifdef PX_AUDIO_H
    do {
        extern int mfb_audio_device_start();
        if (!mfb_audio_device_start()) return 0;
    } while (0);
#endif

    if (!PX_ApplicationInitialize(&App, PX_GetScreenWidth(), PX_GetScreenHeight())) {
        return 0;
    }

    if (!PX_CreateWindow(App.runtime.surface_width, App.runtime.surface_height, App.runtime.window_width, App.runtime.window_height,
                         PX_APPLICATION_NAME)) {
        return 0;
    }

    PX_SystemLoop();
    
#ifdef PX_AUDIO_H
    do {
        extern int mfb_audio_device_stop();
        if (!mfb_audio_device_stop()) return 0;
    } while (0);
#endif
    return 0;
}
