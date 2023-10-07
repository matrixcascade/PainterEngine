#include "PainterEngine_Application.h"
#include "platform/modules/px_time.h"
#include "px_display.h"

void setCurrentDirectory()
{
	px_char szExeFilePathFileName[MAX_PATH];
	px_char path[MAX_PATH];
	GetModuleFileNameA(NULL, szExeFilePathFileName, MAX_PATH);
	PX_FileGetPath(szExeFilePathFileName, path, sizeof(path));
	if (path[PX_strlen(path) - 1] == ':')
	{
		PX_strcat(path, "\\");
	}

	SetCurrentDirectoryA(path);
}


int main()
{
	RECT rc;
	HWND hwnd=(HWND)0;
	px_dword timelasttime=0;
	px_int width,height;
	GetClientRect(hwnd, &rc);
	width = rc.right - rc.left;
	height = rc.bottom - rc.top;
	PX_DisplayInitialize(hwnd, 328, 237);
	setCurrentDirectory();
	if (!PX_ApplicationInitialize(&App,width,height))
	{
		return 0;
	}
	timelasttime=PX_TimeGetTime();
	while (1)
	{
		px_dword now=PX_TimeGetTime();
		px_dword elapsed=now-timelasttime;
		timelasttime= now;
		PX_ApplicationUpdate(&App,elapsed);
		PX_ApplicationRender(&App,elapsed);
		PX_DisplayRender(App.runtime.RenderSurface.surfaceBuffer, 1560, 800);
	}
}