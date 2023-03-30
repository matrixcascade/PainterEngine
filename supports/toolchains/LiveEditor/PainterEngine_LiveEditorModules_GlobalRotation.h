#ifndef PX_LIVEFRAMEWORKMODULES_GLOBALROTATION_H
#define PX_LIVEFRAMEWORKMODULES_GLOBALROTATION_H

#include "PainterEngine_Startup.h"


typedef struct
{
	px_float cursor_last_x,cursor_last_y;
	PX_LiveFramework *pLiveFramework;
	PX_Runtime *pruntime;
	PX_Object *pTranform;
}PX_LiveEditorModule_GlobalRotation;

PX_Object *PX_LiveEditorModule_GlobalRotationInstall(PX_Object *pparent,PX_Runtime *pruntime,PX_FontModule *fm,PX_LiveFramework *pLiveFramework,PX_Json *pLanguageJson);
px_void PX_LiveEditorModule_GlobalRotationUninstall(PX_Object *pObject);
px_void PX_LiveEditorModule_GlobalRotationEnable(PX_Object *pObject);
px_void PX_LiveEditorModule_GlobalRotationDisable(PX_Object *pObject);


#endif