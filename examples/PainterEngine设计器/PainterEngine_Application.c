#include "PainterEngine_Application.h"
#include "./architecture/PainterEngine_Designer.h"
PX_Application App;

PX_FontModule fm;
PX_Object *root,*ui_root,*designer;

px_void PX_Application_DesignerExport(PX_Object *pObject,PX_Object_Event e,px_void *ptr)
{
	px_string string;
	PX_StringInitialize(&App.runtime.mp_game,&string);
	PX_DesignerExport(pObject,&string);
	PX_SaveDataToFile(string.buffer,PX_strlen(string.buffer)+1,"/sdcard/AiCoding/ui.json");
	PX_StringFree(&string);
	return;
}



px_bool PX_ApplicationInitialize(PX_Application *pApp,px_int screen_width,px_int screen_height)
{
	PX_ApplicationInitializeDefault(&pApp->runtime, screen_width, screen_height);
	PX_FontModuleInitialize(&pApp->runtime.mp_resources,&fm);
	PX_LoadFontModuleFromFile(&fm,"assets/gbk_18.pxf");
	root=PX_ObjectCreate(&pApp->runtime.mp_ui,PX_NULL,0,0,0,0,0,0);
	ui_root=PX_ObjectCreate(&pApp->runtime.mp_ui,root,0,0,0,0,0,0);
	PX_ObjectSetId(ui_root,"ui");
	designer=PX_DesignerCreate(&pApp->runtime.mp_game,&pApp->runtime.mp_ui,&pApp->runtime.mp_game,root,ui_root,PX_NULL,&fm);
	PX_ObjectRegisterEvent(designer,PX_OBJECT_EVENT_EXECUTE,PX_Application_DesignerExport,PX_NULL);
	PX_DesignerActivate(designer);
	return PX_TRUE;
}

px_void PX_ApplicationUpdate(PX_Application *pApp,px_dword elpased)
{
	PX_ObjectUpdate(root,elpased);
}

px_void PX_ApplicationRender(PX_Application *pApp,px_dword elpased)
{
	px_surface *pRenderSurface=&pApp->runtime.RenderSurface;
	PX_RuntimeRenderClear(&pApp->runtime,PX_OBJECT_UI_DEFAULT_BACKGROUNDCOLOR);
	PX_ObjectRender(pRenderSurface,root,elpased);
}

px_void PX_ApplicationPostEvent(PX_Application *pApp,PX_Object_Event e)
{
	PX_ApplicationEventDefault(&pApp->runtime, e);
	PX_ObjectPostEvent(root,e);
}

