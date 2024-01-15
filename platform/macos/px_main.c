
#include "px_display.h"
#include "PainterEngine.h"

#define SCREEN_WIDTH 600
#define SCREEN_HEIGHT 400

// ------------------------------------
PX_Object* pPanelObject;

// ------------------------------------
px_int px_main() {
    PainterEngine_Initialize(SCREEN_WIDTH, SCREEN_HEIGHT);

    pPanelObject = PX_Object_PanelCreate(mp, root, 0, 0, PX_GetScreenWidth(), PX_GetScreenHeight()); // 创建一个画板
    PX_SurfaceClearAll(PX_Object_PanelGetSurface(pPanelObject), PX_COLOR_NONE);
    PX_FontModuleDrawText(PX_Object_PanelGetSurface(pPanelObject), 0, 300, 200, PX_ALIGN_CENTER,
                          "Welcome to the world of PainterEngine!", PX_COLOR_GREEN);

    return PX_TRUE;
}

// ------------------------------------
int main(int argc, char** argv) {
    if (!PX_CreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_WIDTH, SCREEN_HEIGHT, PX_APPLICATION_NAME)) {
        return 0;
    }
    if (!PX_ApplicationInitialize(&App, SCREEN_WIDTH, SCREEN_HEIGHT)) {
        return 0;
    }

    PX_SystemLoop();
    return 0;
}
