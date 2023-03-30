#ifndef PX_LIVEFRAMEWORKMODULES_KEYTRANSFORM_H
#define PX_LIVEFRAMEWORKMODULES_KEYTRANSFORM_H

#include "PainterEngine_Startup.h"


typedef struct
{
	px_float cursor_last_x,cursor_last_y;
	PX_LiveFramework *pLiveFramework;
	PX_Runtime *pruntime;
	PX_Object *pTranform;
	px_bool bRootDraging;
}PX_LiveEditorModule_KeyTransform;

typedef enum
{
	PX_LIVEEDITORMODULE_KEYTRANSFORM_MODE_ALL,
	PX_LIVEEDITORMODULE_KEYTRANSFORM_MODE_ROTATION,
	PX_LIVEEDITORMODULE_KEYTRANSFORM_MODE_STRETCH,
}PX_LIVEEDITORMODULE_KEYTRANSFORM_MODE;

PX_Object *PX_LiveEditorModule_KeyTransformInstall(PX_Object *pparent,PX_Runtime *pruntime,PX_FontModule *fm,PX_LiveFramework *pLiveFramework,PX_Json *pLanguageJson);
px_void PX_LiveEditorModule_KeyTransformUninstall(PX_Object *pObject);
px_void PX_LiveEditorModule_KeyTransformEnable(PX_Object *pObject,PX_LIVEEDITORMODULE_KEYTRANSFORM_MODE mode);
px_void PX_LiveEditorModule_KeyTransformDisable(PX_Object *pObject);


#endif