#ifndef PX_LIVEFRAMEWORKMODULES_IMPORTLIVE_H
#define PX_LIVEFRAMEWORKMODULES_IMPORTLIVE_H

#include "PainterEngine_Startup.h"



#define  PX_LIVEEDITORMODULE_IMPORTLIVE_EVENT_MESSAGE 0x000F0000
#define  PX_LIVEEDITORMODULE_IMPORTLIVE_EVENT_EXIT 0x000F0001
#define  PX_LIVEEDITORMODULE_IMPORTLIVE_EVENT_CONFIRM 0x000F0002
typedef struct
{
	PX_LiveFramework *pLiveFramework;
	PX_Runtime *pruntime;
	PX_Object *explorer;
	PX_FontModule *fontmodule;
	PX_Json *pLanguageJson;
}PX_LiveEditorModule_ImportLive;

PX_Object * PX_LiveEditorModule_ImportLiveInstall(PX_Object *parent,PX_Runtime *pruntime,PX_FontModule *fm,PX_LiveFramework *pLiveFramework,PX_Json *pLanguageJson);
px_void PX_LiveEditorModule_ImportLiveUninstall(PX_Object *pObject);

px_void PX_LiveEditorModule_ImportLiveEnable(PX_Object *pObject);
px_void PX_LiveEditorModule_ImportLiveDisable(PX_Object *pObject);
#endif   