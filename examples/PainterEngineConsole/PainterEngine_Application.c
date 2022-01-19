#include "PainterEngine_Application.h"
#include "./architecture/PainterEngine_Console.h"
PX_Application App;
PX_Console console;
px_bool PX_ApplicationInitialize(PX_Application *pApp,px_int screen_width,px_int screen_height)
{
	PX_ApplicationInitializeDefault(&pApp->runtime, screen_width, screen_height);
	PX_ConsoleInitializeEx(&pApp->runtime,&console,pApp->runtime.surface_width/2-240,pApp->runtime.surface_height/2-160,480,320);
	return PX_TRUE;
}

px_void PX_ApplicationUpdate(PX_Application *pApp,px_dword elpased)
{
	PX_ConsoleUpdate(&console,elpased);
}

px_void PX_ApplicationRender(PX_Application *pApp,px_dword elpased)
{
	px_surface *pRenderSurface=&pApp->runtime.RenderSurface;
	PX_RuntimeRenderClear(&pApp->runtime,PX_OBJECT_UI_DEFAULT_BACKGROUNDCOLOR);
	PX_ConsoleRender(pRenderSurface,&console,elpased);
}

px_void PX_ApplicationPostEvent(PX_Application *pApp,PX_Object_Event e)
{
	PX_ApplicationEventDefault(&pApp->runtime, e);
	PX_ConsolePostEvent(&console,e);

}

