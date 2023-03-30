#ifndef PX_LIVEFRAMEWORKMODULES_TRANSLATION_H
#define PX_LIVEFRAMEWORKMODULES_TRANSLATION_H

#include "PainterEngine_Startup.h"

typedef struct
{
	px_float cursor_last_x,cursor_last_y;
	px_bool bSelect;
	PX_LiveFramework *pLiveFramework;
	PX_Runtime *pruntime;
}PX_LiveEditorModule_Translation;

PX_Object *PX_LiveEditorModule_TranslationInstall(PX_Object *pparent,PX_Runtime *pruntime,PX_FontModule *fm,PX_LiveFramework *pLiveFramework,PX_Json *pLanguageJson);
px_void PX_LiveEditorModule_TranslationUninstall(PX_Object *pObject);
px_void PX_LiveEditorModule_TranslationEnable(PX_Object *pObject);
px_void PX_LiveEditorModule_TranslationDisable(PX_Object *pObject);

#endif