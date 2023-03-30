#ifndef PX_LIVEFRAMEWORKMODULES_BUILDMESH_H
#define PX_LIVEFRAMEWORKMODULES_BUILDMESH_H

#include "PainterEngine_Startup.h"

#define PX_LIVEFRAMEWORKMODULES_BUILDMESH_EVENT_EXIT 0x00020000
#define PX_LIVEEDITORMODULE_BUILDMESH_EVENT_MESSAGE 0x00020001
#define PX_LIVEFRAMEWORKMODULES_BUILDMESH_MAX_SUPPORT_POINTS 256


typedef struct
{
	PX_Object *widget_controller,*button_reset,*button_finish;
	PX_Object *widget_coefficient,*edit_coefficient;

	PX_LiveFramework *pLiveFramework;
	PX_Runtime *pruntime;
	PX_FontModule *fontmodule;
	PX_Json *pLanguageJson;
	PX_Object *messagebox;
	px_point2D cursor;
	px_point2D lastdragcursor;
	px_point2D limitpoints[PX_LIVEFRAMEWORKMODULES_BUILDMESH_MAX_SUPPORT_POINTS];
	px_int limitpoints_count;
	px_point2D buildpoints[PX_LIVEFRAMEWORKMODULES_BUILDMESH_MAX_SUPPORT_POINTS];
	px_int buildpoints_count;
	px_bool OnFocusEvent;
	px_bool OnFocusRender;
}PX_LiveEditorModule_BuildMesh;

PX_Object *PX_LiveEditorModule_BuildMeshInstall(PX_Object *pparent,PX_Runtime *pruntime,PX_FontModule *fm,PX_LiveFramework *pLiveFramework,PX_Json *pLanguageJson);
px_void PX_LiveEditorModule_BuildMeshUninstall(PX_Object *pObject);
px_void PX_LiveEditorModule_BuildMeshEnable(PX_Object *pObject);
px_void PX_LiveEditorModule_BuildMeshDisable(PX_Object *pObject);
#endif   