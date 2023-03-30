#ifndef PX_LIVEFRAMEWORKMODULES_BINDBONE_H
#define PX_LIVEFRAMEWORKMODULES_BINDBONE_H

#include "PainterEngine_Startup.h"

#define PX_LIVEFRAMEWORKMODULES_LINKKEY_EVENT_EXIT 0x00050000
#define PX_LIVEFRAMEWORKMODULES_LINKKEY_EVENT_MESSAGE 0x00050001
typedef struct
{
	PX_LiveFramework *pLiveFramework;
	PX_Runtime *pruntime;
	PX_FontModule *fontmodule;
	PX_Json *pLanguageJson;
	PX_Object *root,*button_finish,*button_reset;
	PX_Object *messagebox;
	px_int     startlinkIndex;
	px_point2D startPoint;
	px_point2D cursor;

}PX_LiveEditorModule_Linkkey;

PX_Object * PX_LiveEditorModule_LinkkeyInstall(PX_Object *pparent,PX_Runtime *pruntime,PX_FontModule *fm,PX_LiveFramework *pLiveFramework,PX_Json *pLanguageJson);
px_void PX_LiveEditorModule_LinkkeyUninstall(PX_Object *pObject);

px_void PX_LiveEditorModule_LinkkeyEnable(PX_Object *pObject);
px_void PX_LiveEditorModule_LinkkeyDisable(PX_Object *pObject);
#endif   