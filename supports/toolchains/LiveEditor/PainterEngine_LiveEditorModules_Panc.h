#ifndef PX_LIVEFRAMEWORKMODULES_PANC_H
#define PX_LIVEFRAMEWORKMODULES_PANC_H

#include "PainterEngine_Startup.h"

#define PX_LIVEFRAMEWORKMODULES_PANC_EVENT_EXIT 0x00C10000
#define PX_LIVEFRAMEWORKMODULES_PANC_EVENT_MESSAGE 0x000C1001

typedef struct
{
	PX_LiveFramework *pLiveFramework;
	PX_Runtime *pruntime;
	PX_FontModule *fontmodule;
	PX_Json *pLanguageJson;
	PX_Object *root,*panc,*button_finish,*button_reset;
	px_point2D Cursor;
}PX_LiveEditorModule_Panc;

PX_Object * PX_LiveEditorModule_PancInstall(PX_Object *pparent,PX_Runtime *pruntime,PX_FontModule *fm,PX_LiveFramework *pLiveFramework,PX_Json *pLanguageJson);
px_void PX_LiveEditorModule_PancUninstall(PX_Object *pObject);

px_void PX_LiveEditorModule_PancEnable(PX_Object *pObject);
px_void PX_LiveEditorModule_PancDisable(PX_Object *pObject);
#endif   