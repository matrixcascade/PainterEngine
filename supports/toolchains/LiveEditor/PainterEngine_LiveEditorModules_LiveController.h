#ifndef PX_LIVEFRAMEWORKMODULES_LAYERCONTROLLER_H
#define PX_LIVEFRAMEWORKMODULES_LAYERCONTROLLER_H

#include "PainterEngine_Startup.h"


#define PX_LIVEFRAMEWORKMODULES_LIVECONTROLLER_EVENT_SETKEY 0x00060000
#define PX_LIVEFRAMEWORKMODULES_LIVECONTROLLER_EVENT_BUILDMESH 0x00060001
#define PX_LIVEFRAMEWORKMODULES_LIVECONTROLLER_EVENT_LOADIMAGE    0x00060002
#define PX_LIVEFRAMEWORKMODULES_LIVECONTROLLER_EVENT_LINKKEY   0x00060003
#define PX_LIVEEDITORMODULE_LIVECONTROLLER_EVENT_MESSAGE   0x00060005

#define PX_LIVEFRAMEWORKMODULES_LIVECONTROLLER_EVENT_STATECHANGE 0x3004


typedef enum
{
	PX_LIVEEDITORMODULE_LIVECONTROLLER_STATE_LAYER,
	PX_LIVEEDITORMODULE_LIVECONTROLLER_STATE_RESOURCE,
	PX_LIVEEDITORMODULE_LIVECONTROLLER_STATE_ANIMATION,
}PX_LIVEEDITORMODULE_LIVECONTROLLER_STATE;

typedef struct
{
	PX_Object *window_widget;
	PX_Object *pLayerList,*pResourceList,*pAnimationList;
	PX_Object *button_viewLayer,*button_viewResource,*button_viewAnimation,*button_newlayer,*button_importImage,*button_linkkey,*button_newanimation,*messagebox;
	PX_FontModule *fontmodule;
	PX_Json *pLanguageJson;
	PX_Runtime *pruntime;
	PX_LiveFramework *pLiveFramework;
	PX_LIVEEDITORMODULE_LIVECONTROLLER_STATE state;
	px_shape shape_eye,shape_eye_no,shape_setkey,shape_buildmesh,shape_bindbone,shape_import,shape_new,shape_link,shape_trash,shape_linkkey,shape_animation,shape_add;
}PX_LiveEditorModule_LiveController;

PX_Object * PX_LiveEditorModule_LiveControllerInstall(PX_Object *pparent,PX_Runtime *pruntime,PX_FontModule *fm,PX_LiveFramework *pLiveFramework,PX_Json *pLanguageJson);
px_void    PX_LiveEditorModule_LiveControllerUninstall(PX_Object *pObject);


px_void    PX_LiveEditorModule_LiveControllerEnable(PX_Object *pObject);
px_void    PX_LiveEditorModule_LiveControllerDisable(PX_Object *pObject);


px_int     PX_LiveEditorModule_LiveControllerGetCurrentEditLayer(PX_Object *pObject);
#endif