#ifndef PX_LIVEFRAMEWORKMODULES_DISPLAY_H
#define PX_LIVEFRAMEWORKMODULES_DISPLAY_H

#include "PainterEngine_Startup.h"

typedef struct
{
	PX_LiveFramework *pLiveFramework;
	PX_Runtime *pruntime;
	PX_Json *pLanguageJson;
	PX_FontModule *fm;
	px_bool showFPS;
	px_bool showHelpline;
	px_point2D helpPoint;
	px_float fps;
	px_int  _tRenderFrames;
	px_dword elpased;
}PX_LiveEditorModule_Display;

PX_Object* PX_LiveEditorModule_DisplayInstall(PX_Object *pparent,PX_Runtime *pruntime,PX_FontModule *fm,PX_LiveFramework *pLiveFramework,PX_Json *pLanguageJson);
px_void PX_LiveEditorModule_DisplayUninstall(PX_Object *pObject);

px_void PX_LiveEditorModule_DisplayShowFPS(PX_Object *pObject);
px_void PX_LiveEditorModule_DisplayShowHelperLine(PX_Object *pObject);
px_void PX_LiveEditorModule_DisplayResetPosition(PX_Object *pObject);
px_void PX_LiveEditorModule_DisplayEnable(PX_Object *pObject);
px_void PX_LiveEditorModule_DisplayDisable(PX_Object *pObject);
#endif