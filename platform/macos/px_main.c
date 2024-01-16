
#include "px_display.h"
#include "runtime/PainterEngine_Application.h"

// ------------------------------------
int main(int argc, char** argv) {

    if (!PX_ApplicationInitialize(&App, PX_GetScreenWidth(), PX_GetScreenHeight())) {
        return 0;
    }

    if (!PX_CreateWindow(App.runtime.surface_width, App.runtime.surface_height, App.runtime.window_width, App.runtime.window_height,
                         PX_APPLICATION_NAME)) {
        return 0;
    }

    PX_SystemLoop();
    return 0;
}
