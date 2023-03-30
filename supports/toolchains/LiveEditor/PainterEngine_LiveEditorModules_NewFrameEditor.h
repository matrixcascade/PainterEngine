#ifndef PX_LIVEFRAMEWORKMODULES_NEWFRAMEEDITOR_H
#define PX_LIVEFRAMEWORKMODULES_NEWFRAMEEDITOR_H

#include "PainterEngine_Startup.h"


#define PX_LIVEEDITORMODULE_NEWFRAMEEDITOR_EVENT_EXIT 0x000A0000

typedef struct
{
	PX_Object *window_widget;
	PX_Object *pFrameList;
	PX_Object *button_ok,*button_cancel,*edit_id;
	PX_FontModule *fontmodule;
	PX_Json *pLanguageJson;
	PX_Runtime *pruntime;
	PX_LiveFramework *pLiveFramework;
	px_bool bCopy;
}PX_LiveEditorModule_NewFrameEditor;

PX_Object * PX_LiveEditorModule_NewFrameEditorInstall(PX_Object *pparent,PX_Runtime *pruntime,PX_FontModule *fm,PX_LiveFramework *pLiveFramework,PX_Json *pLanguageJson);
px_void    PX_LiveEditorModule_NewFrameEditorUninstall(PX_Object *pObject);


px_bool    PX_LiveEditorModule_NewFrameEditorEnable(PX_Object *pObject,px_bool bCopy);
px_void    PX_LiveEditorModule_NewFrameEditorDisable(PX_Object *pObject);

#endif