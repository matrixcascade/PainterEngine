#ifndef PX_LIVEFRAMEWORKMODULES_CREATEPROJECT_H
#define PX_LIVEFRAMEWORKMODULES_CREATEPROJECT_H

#include "PainterEngine_Startup.h"

#define PX_LIVEEDITORMODULE_EVENT_CREATEPROJECT_EXIT		0x00030000
#define PX_LIVEEDITORMODULE_EVENT_CREATEPROJECT_CONFIRM     0x00030001
typedef struct
{
	PX_LiveFramework *pLiveFramework;
	PX_Runtime *pruntime;
	PX_Object *widget,*edit_name,*edit_height,*edit_width,*button_ok;
	PX_FontModule *fontmodule;
	PX_Json *pLanguageJson;
}PX_LiveEditorModule_CreateProject;

//////////////////////////////////////////////////////////////////////////
//standard interface
PX_Object * PX_LiveEditorModule_CreateProjectInstall(PX_Object *parent,PX_Runtime *pruntime,PX_FontModule *fm,PX_LiveFramework *pLiveFramework,PX_Json *pLanguageJson);
px_void     PX_LiveEditorModule_CreateProjectUninstall(PX_Object *pObject);

//////////////////////////////////////////////////////////////////////////
px_void		PX_LiveEditorModule_CreateProjectEnable(PX_Object *pObject);
px_void		PX_LiveEditorModule_CreateProjectDisable(PX_Object *pObject);
#endif   