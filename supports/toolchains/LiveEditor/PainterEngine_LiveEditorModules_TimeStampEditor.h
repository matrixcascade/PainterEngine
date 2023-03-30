

#ifndef PX_LIVEFRAMEWORKMODULES_TIMESTAMPEDITOR_H
#define PX_LIVEFRAMEWORKMODULES_TIMESTAMPEDITOR_H

#include "PainterEngine_Startup.h"


#define PX_LIVEEDITORMODULE_TIMESTAMPEDITOR_EVENT_EXIT 0x00090000

typedef struct
{
	PX_Object *window_widget;
	PX_Object *pFrameList;
	PX_Object *button_ok,*button_cancel,*edit_duration;
	PX_FontModule *fontmodule;
	PX_Json *pLanguageJson;
	PX_Runtime *pruntime;
	PX_LiveFramework *pLiveFramework;
}PX_LiveEditorModule_TimeStampEditor;

PX_Object * PX_LiveEditorModule_TimeStampEditorInstall(PX_Object *pparent,PX_Runtime *pruntime,PX_FontModule *fm,PX_LiveFramework *pLiveFramework,PX_Json *pLanguageJson);
px_void    PX_LiveEditorModule_TimeStampEditorUninstall(PX_Object *pObject);


px_bool    PX_LiveEditorModule_TimeStampEditorEnable(PX_Object *pObject);
px_void    PX_LiveEditorModule_TimeStampEditorDisable(PX_Object *pObject);

#endif