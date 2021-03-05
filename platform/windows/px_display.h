#ifndef PX_PLATFORM_WINDOWS
#define PX_PLATFORM_WINDOWS

#include <Windows.h>
#include "stdio.h"
#include "time.h"
#pragma comment(lib,"User32.lib")
#pragma comment(lib,"Shell32.lib")
#pragma comment(lib,"Ole32.lib")
#pragma comment(lib,"winmm.lib" )


typedef struct
{
	UINT uMsg;
	LPARAM lparam;
	WPARAM wparam;
}WM_MESSAGE;

extern BOOL PX_CreateWindow( int surfaceWidth,int surfaceHeight,int windowWidth,int windowHeight, const char *name);
extern BOOL PX_WindowResize(int surfaceWidth,int surfaceHeight,int windowWidth,int windowHeight);
extern int  PX_GetScreenHeight();
extern int  PX_GetScreenWidth();
extern int  PX_SetWindowResizeable();

extern HWND PX_GetWindowHwnd();
extern VOID PX_SystemReadDeviceState();
extern BOOL PX_SystemLoop();
extern BOOL PX_SystemRender(void *raw,int width,int height);
extern BOOL PX_SystemisAvtivated();
extern BOOL PX_KeyboardDown(unsigned char X );
extern char *PX_KeyboardString();
extern char *PX_DragfileString();
extern BOOL PX_MouseLButtonDown();
extern BOOL PX_MouseRButtonDown();
extern BOOL PX_MouseMButtonDown();
extern POINT PX_MousePosition();
extern BOOL PX_KeyDown(unsigned char key);
extern BOOL PX_MouseWheel(int *x,int *y,int *delta);
extern BOOL PX_GetWinMessage(WM_MESSAGE *Msg);
extern double PX_GetWindowScale();

extern char *PX_OpenFileDialog(const char Filter[]);
extern char *PX_MultFileDialog(const char Filter[]);
extern char *PX_SaveFileDialog(const char Filter[],const char ext[]);
#endif