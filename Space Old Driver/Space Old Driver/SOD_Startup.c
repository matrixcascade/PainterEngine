#include "SOD_Startup.h"

extern px_void SOD_GamePlay();

px_void SOD_Startup_OnStartGameClick(PX_Object *pobj,PX_Object_Event e,px_void *ptr)
{
	SOD_Startup *Startup=(SOD_Startup *)ptr;
	SOD_PlaySoundFromResource(Startup->pRuntime,SOD_KEY_BTN_CLICK,PX_FALSE);
	SOD_GamePlay();
}
extern px_void SOD_GameAbout();
px_void SOD_Startup_OnAboutClick(PX_Object *pobj,PX_Object_Event e,px_void *ptr)
{
	//待添加
	SOD_Startup *Startup=(SOD_Startup *)ptr;
	SOD_PlaySoundFromResource(Startup->pRuntime,SOD_KEY_BTN_CLICK,PX_FALSE);
	SOD_GameAbout();
}

px_void SOD_Startup_OnExitClick(PX_Object *pobj,PX_Object_Event e,px_void *ptr)
{
	SOD_Startup *Startup=(SOD_Startup *)ptr;
	SOD_PlaySoundFromResource(Startup->pRuntime,SOD_KEY_BTN_CLICK,PX_FALSE);
	exit(0);
}

px_void SOD_Startup_OnStartGameOver(PX_Object *pobj,PX_Object_Event e,px_void *ptr)
{
	SOD_Startup *Startup=(SOD_Startup *)ptr;
	SOD_PlaySoundFromResource(Startup->pRuntime,SOD_KEY_BTN_ALERT,PX_FALSE);
}

px_void SOD_Startup_OnReplayOver(PX_Object *pobj,PX_Object_Event e,px_void *ptr)
{
	//待添加
	SOD_Startup *Startup=(SOD_Startup *)ptr;
	SOD_PlaySoundFromResource(Startup->pRuntime,SOD_KEY_BTN_ALERT,PX_FALSE);
}

px_void SOD_Startup_OnExitOver(PX_Object *pobj,PX_Object_Event e,px_void *ptr)
{
	SOD_Startup *Startup=(SOD_Startup *)ptr;
	SOD_PlaySoundFromResource(Startup->pRuntime,SOD_KEY_BTN_ALERT,PX_FALSE);
}


px_bool SOD_StartupInitialize(SOD_Runtime *pruntime,SOD_Startup *Startup)
{
	Startup->pRuntime=pruntime;

	//加载背景贴图
	if(!PX_LoadTextureFromFile(&pruntime->runtime.mp_resources,&Startup->background,SOD_PATH_STARTUP_BACKGROUND)) return PX_FALSE;

	//加载About
	if(!PX_LoadShapeToResource(&pruntime->runtime,SOD_PATH_ABOUT,SOD_KEY_TEX_ABOUT)) return PX_FALSE;

	//ui 根节点
	Startup->root=PX_ObjectCreate(&pruntime->runtime.mp_ui,PX_NULL,0,0,0,0,0,0);
	if(!Startup->root) return PX_FALSE;

	//创建按钮
	Startup->btn_startgame=PX_Object_CursorButtonCreate(&pruntime->runtime.mp_ui,Startup->root,300,220,200,45,"",PX_COLOR(255,0,0,0));
	Startup->btn_Aboutgame=PX_Object_CursorButtonCreate(&pruntime->runtime.mp_ui,Startup->root,300,280,200,45,"",PX_COLOR(255,0,0,0));
	Startup->btn_exitgame=PX_Object_CursorButtonCreate(&pruntime->runtime.mp_ui,Startup->root,300,340,200,45,"",PX_COLOR(255,0,0,0));
	
	PX_ObjectRegisterEvent(PX_Object_GetCursorButtonPushButton(Startup->btn_startgame),PX_OBJECT_EVENT_CURSORCLICK,SOD_Startup_OnStartGameClick,Startup);
	PX_ObjectRegisterEvent(PX_Object_GetCursorButtonPushButton(Startup->btn_Aboutgame),PX_OBJECT_EVENT_CURSORCLICK,SOD_Startup_OnAboutClick,Startup);
	PX_ObjectRegisterEvent(PX_Object_GetCursorButtonPushButton(Startup->btn_exitgame),PX_OBJECT_EVENT_CURSORCLICK,SOD_Startup_OnExitClick,Startup);

	PX_ObjectRegisterEvent(PX_Object_GetCursorButtonPushButton(Startup->btn_startgame),PX_OBJECT_EVENT_CURSOROVER,SOD_Startup_OnStartGameOver,Startup);
	PX_ObjectRegisterEvent(PX_Object_GetCursorButtonPushButton(Startup->btn_Aboutgame),PX_OBJECT_EVENT_CURSOROVER,SOD_Startup_OnReplayOver,Startup);
	PX_ObjectRegisterEvent(PX_Object_GetCursorButtonPushButton(Startup->btn_exitgame),PX_OBJECT_EVENT_CURSOROVER,SOD_Startup_OnExitOver,Startup);

	return PX_TRUE;
}

px_void SOD_StartupUpdate(SOD_Startup *Startup,px_dword elpased)
{
	PX_ObjectUpdate(Startup->root,elpased);
}

px_void SOD_StartupRender(SOD_Startup *Startup,px_dword elpased)
{
	PX_TextureRender(&Startup->pRuntime->runtime.RenderSurface,&Startup->background,0,0,PX_TEXTURERENDER_REFPOINT_LEFTTOP,PX_NULL);
	PX_ObjectRender(&Startup->pRuntime->runtime.RenderSurface,Startup->root,elpased);
	PX_FontModuleDrawText(&Startup->pRuntime->runtime.RenderSurface,335,252,(px_uchar *)SOD_TEXT_START_GAME,PX_COLOR(255,0,0,0),&Startup->pRuntime->fontmodule);
	PX_FontModuleDrawText(&Startup->pRuntime->runtime.RenderSurface,360,314,(px_uchar *)SOD_TEXT_ABOUT,PX_COLOR(255,0,0,0),&Startup->pRuntime->fontmodule);
	PX_FontModuleDrawText(&Startup->pRuntime->runtime.RenderSurface,335,372,(px_uchar *)SOD_TEXT_EXIT_GAME,PX_COLOR(255,0,0,0),&Startup->pRuntime->fontmodule);
}

px_void SOD_StartupPostEvent(SOD_Startup *Startup,PX_Object_Event e)
{
	PX_ObjectPostEvent(Startup->root,e);
}

