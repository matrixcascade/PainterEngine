#ifndef PX_LIVEFRAMEWORKMODULES_IMPULSEEDITOR_H
#define PX_LIVEFRAMEWORKMODULES_IMPULSEEDITOR_H

#include "PainterEngine_Startup.h"

#define PX_LIVEFRAMEWORKMODULES_IMPULSEEDITOR_EVENT_EXIT 0x000a0000

typedef struct
{
	px_point2D TagPoint,Cursor;
	PX_LiveFramework *pLiveFramework;
	PX_Runtime *pruntime;
}PX_LiveEditorModule_ImpulseEditor;

PX_Object * PX_LiveEditorModule_ImpulseEditorInstall(PX_Object *pparent,PX_Runtime *pruntime,PX_FontModule *fm,PX_LiveFramework *pLiveFramework,PX_Json *pLanguageJson);
px_void PX_LiveEditorModule_ImpulseEditorUninstall(PX_Object *pObject);

px_void PX_LiveEditorModule_ImpulseEditorEnable(PX_Object *pObject);
px_void PX_LiveEditorModule_ImpulseEditorDisable(PX_Object *pObject);
#endif   