#ifndef PX_LIVEFRAMEWORKMODULES_FRAMECONTROLLER_H
#define PX_LIVEFRAMEWORKMODULES_FRAMECONTROLLER_H

#include "PainterEngine_Startup.h"

#define PX_FRAMECONTROLLERMODULE_EVENT_NEWFRAME 0x000B0000
#define PX_FRAMECONTROLLERMODULE_EVENT_COPYFRAME 0x000B0001

typedef struct
{
	PX_Object *window_widget;
	PX_Object *pFrameList;
	PX_Object *button_newframe,*button_copyframe,*button_up,*button_down,*button_trash,*messagebox;
	PX_FontModule *fontmodule;
	PX_Json *pLanguageJson;
	PX_Runtime *pruntime;
	PX_LiveFramework *pLiveFramework;
	PX_LiveAnimation *pLastPerformanceAnimation;
	px_shape shape_translation,shape_pointtranslation,shape_stretch,shape_rotation,shape_clock,shape_switchtexture,shape_trash,shape_newframe,shape_animationframe,shape_copy,shape_up,shape_down;
}PX_LiveEditorModule_FrameController;

PX_Object * PX_LiveEditorModule_FrameControllerInstall(PX_Object *pparent,PX_Runtime *pruntime,PX_FontModule *fm,PX_LiveFramework *pLiveFramework,PX_Json *pLanguageJson);
px_void    PX_LiveEditorModule_FrameControllerUninstall(PX_Object *pObject);


px_void    PX_LiveEditorModule_FrameControllerEnable(PX_Object *pObject);
px_void    PX_LiveEditorModule_FrameControllerDisable(PX_Object *pObject);

#endif