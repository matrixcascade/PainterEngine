#ifndef PX_LIVEFRAMEWORKMODULES_FRAMELIVETEXTUREEDITOR_H
#define PX_LIVEFRAMEWORKMODULES_FRAMELIVETEXTUREEDITOR_H

#include "PainterEngine_Startup.h"


#define PX_LIVEEDITORMODULE_FRAMELIVETEXTUREEDITOR_EVENT_EXIT 0x000C0000

typedef struct
{
	PX_Object *window_widget;
	PX_Object *pFrameList;
	PX_Object *button_ok,*edit_textureid;
	PX_FontModule *fontmodule;
	PX_Json *pLanguageJson;
	PX_Runtime *pruntime;
	PX_LiveFramework *pLiveFramework;
}PX_LiveEditorModule_FrameLiveTextureEditor;

PX_Object * PX_LiveEditorModule_FrameLiveTextureEditorInstall(PX_Object *pparent,PX_Runtime *pruntime,PX_FontModule *fm,PX_LiveFramework *pLiveFramework,PX_Json *pLanguageJson);
px_void    PX_LiveEditorModule_FrameLiveTextureEditorUninstall(PX_Object *pObject);


px_void    PX_LiveEditorModule_FrameLiveTextureEditorEnable(PX_Object *pObject);
px_void    PX_LiveEditorModule_FrameLiveTextureEditorDisable(PX_Object *pObject);

#endif