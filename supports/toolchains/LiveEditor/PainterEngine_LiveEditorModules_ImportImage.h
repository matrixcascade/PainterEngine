#ifndef PX_LIVEFRAMEWORKMODULES_IMPORTIMAGE_H
#define PX_LIVEFRAMEWORKMODULES_IMPORTIMAGE_H

#include "PainterEngine_Startup.h"



#define  PX_LIVEEDITORMODULE_IMPORTIMAGE_EVENT_MESSAGE 0x00040000
#define  PX_LIVEEDITORMODULE_IMPORTIMAGE_EVENT_EXIT 0x00040001

typedef struct
{
	PX_LiveFramework *pLiveFramework;
	PX_Runtime *pruntime;
	PX_Object *explorer;
	PX_FontModule *fontmodule;
	PX_Json *pLanguageJson;
}PX_LiveEditorModule_ImportImage;

PX_Object * PX_LiveEditorModule_ImportImageInstall(PX_Object *parent,PX_Runtime *pruntime,PX_FontModule *fm,PX_LiveFramework *pLiveFramework,PX_Json *pLanguageJson);
px_void PX_LiveEditorModule_ImportImageUninstall(PX_Object *pObject);

px_void PX_LiveEditorModule_ImportImageEnable(PX_Object *pObject);
px_void PX_LiveEditorModule_ImportImageDisable(PX_Object *pObject);
#endif   