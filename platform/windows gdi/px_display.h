#ifndef PX_DISPLAY_H
#define PX_DISPLAY_H
#include "windows.h"

int PX_DisplayInitialize(HWND hwdn, int width, int height);
void PX_DisplayRender(void* bgra,int x,int y);
void* PX_DisplayCaptureScreenBitmap(int* outSize);
#endif