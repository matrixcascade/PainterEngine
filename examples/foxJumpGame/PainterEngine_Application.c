#include "PainterEngine_Application.h"
PX_Application App;

px_bool PX_ApplicationInitialize(PX_Application *pApp,px_int screen_width,px_int screen_height)
{
	PX_ApplicationInitializeDefault(&pApp->runtime, screen_width, screen_height);
	pApp->Module_Game = PX_Module_GameInstall(&pApp->runtime);
	return PX_TRUE;
}

px_void PX_ApplicationUpdate(PX_Application *pApp,px_dword elpased)
{
	PX_ObjectUpdate(pApp->Module_Game, elpased);
}

px_void PX_ApplicationRender(PX_Application *pApp,px_dword elpased)
{
	px_surface *pRenderSurface=&pApp->runtime.RenderSurface;
	PX_RuntimeRenderClear(&pApp->runtime,PX_COLOR_WHITE);
	PX_ObjectRender(pRenderSurface, pApp->Module_Game, elpased);
}

px_void PX_ApplicationPostEvent(PX_Application *pApp,PX_Object_Event e)
{
	PX_Module_Game* pDesc = PX_ObjectGetDesc(PX_Module_Game, pApp->Module_Game);
	PX_ApplicationEventDefault(&pApp->runtime, e);
	if (pDesc->state!=PX_Module_Game_State_Stop)
	{
		PX_ObjectPostEvent(pApp->Module_Game, e);
	}
	else
	{
		PX_RuntimeReset(&pApp->runtime);
		pApp->Module_Game = PX_Module_GameInstall(&pApp->runtime);
	}
	
}

