#ifndef PX_LIVEFRAMEWORKMODULES_SETKEY_H
#define PX_LIVEFRAMEWORKMODULES_SETKEY_H

#include "PainterEngine_Startup.h"

#define PX_LIVEFRAMEWORKMODULES_SETKEY_EVENT_EXIT 0x00080000

typedef struct
{
	PX_LiveFramework *pLiveFramework;
	PX_Runtime *pruntime;
	PX_FontModule *fontmodule;
	PX_Json *pLanguageJson;
	px_point2D point;
	px_point2D cursor,lastdragcursor;
	px_bool bSelect;
	PX_Object *root,*button_finish;
}PX_LiveEditorModule_SetKey;

PX_Object * PX_LiveEditorModule_SetKeyInstall(PX_Object *pparent,PX_Runtime *pruntime,PX_FontModule *fm,PX_LiveFramework *pLiveFramework,PX_Json *pLanguageJson);
px_void PX_LiveEditorModule_SetKeyUninstall(PX_Object *pObject);

px_void PX_LiveEditorModule_SetKeyEnable(PX_Object *pObject);
px_void PX_LiveEditorModule_SetKeyDisable(PX_Object *pObject);
#endif   