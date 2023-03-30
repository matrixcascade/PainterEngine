#ifndef PX_LIVEFRAMEWORKMODULES_VERTEXTRANSFORM_H
#define PX_LIVEFRAMEWORKMODULES_VERTEXTRANSFORM_H

#include "PainterEngine_Startup.h"


typedef struct
{
	px_point2D last_cursor;
	px_float cursor_last_x,cursor_last_y;
	PX_LiveFramework *pLiveFramework;
	PX_Runtime *pruntime;
}PX_LiveEditorModule_VertexTransform;

PX_Object *PX_LiveEditorModule_VertexTransformInstall(PX_Object *pparent,PX_Runtime *pruntime,PX_FontModule *fm,PX_LiveFramework *pLiveFramework,PX_Json *pLanguageJson);
px_void PX_LiveEditorModule_VertexTransformUninstall(PX_Object *pObject);
px_void PX_LiveEditorModule_VertexTransformEnable(PX_Object *pObject);
px_void PX_LiveEditorModule_VertexTransformDisable(PX_Object *pObject);


#endif