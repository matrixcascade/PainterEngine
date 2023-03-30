#include "PainterEngine_LiveEditorModules_AnimationController.h"


static px_void PX_LiveEditorModule_AnimationControllerUpdate(PX_Object *pObject,px_dword elpased)
{
	PX_LiveEditorModule_AnimationController *pDesc=(PX_LiveEditorModule_AnimationController *)pObject->pObject;
	px_float targetx,targety;
	if (elpased==0)
	{
		return;
	}
	if (pObject->Enabled&&pDesc->pLiveFramework->currentEditAnimationIndex!=-1)
	{
		targetx=pDesc->pruntime->surface_width/2-pObject->Width/2;
		targety=pDesc->pruntime->surface_height-pObject->Height-1;
		if (pDesc->pLiveFramework->currentEditFrameIndex!=-1)
		{
			pDesc->button_clock->Enabled=PX_TRUE;
			PX_Object_PushButtonSetTextColor(pDesc->button_clock,PX_OBJECT_UI_DEFAULT_FONTCOLOR);
			pDesc->button_globalrotation->Enabled=PX_TRUE;
			PX_Object_PushButtonSetTextColor(pDesc->button_globalrotation,PX_OBJECT_UI_DEFAULT_FONTCOLOR);
			pDesc->button_impulse->Enabled=PX_TRUE;
			PX_Object_PushButtonSetTextColor(pDesc->button_impulse,PX_OBJECT_UI_DEFAULT_FONTCOLOR);
			pDesc->button_verticeseditor->Enabled=PX_TRUE;
			PX_Object_PushButtonSetTextColor(pDesc->button_verticeseditor,PX_OBJECT_UI_DEFAULT_FONTCOLOR);
			pDesc->button_rotation->Enabled=PX_TRUE;
			PX_Object_PushButtonSetTextColor(pDesc->button_rotation,PX_OBJECT_UI_DEFAULT_FONTCOLOR);
			pDesc->button_smarttranslation->Enabled=PX_TRUE;
			PX_Object_PushButtonSetTextColor(pDesc->button_smarttranslation,PX_OBJECT_UI_DEFAULT_FONTCOLOR);
			pDesc->button_stretch->Enabled=PX_TRUE;
			PX_Object_PushButtonSetTextColor(pDesc->button_stretch,PX_OBJECT_UI_DEFAULT_FONTCOLOR);
			pDesc->button_switchtexture->Enabled=PX_TRUE;
			PX_Object_PushButtonSetTextColor(pDesc->button_switchtexture,PX_OBJECT_UI_DEFAULT_FONTCOLOR);
			pDesc->button_pan->Enabled=PX_TRUE;
			PX_Object_PushButtonSetTextColor(pDesc->button_pan,PX_OBJECT_UI_DEFAULT_FONTCOLOR);
		}
		else
		{
			pDesc->button_clock->Enabled=PX_FALSE;
			PX_Object_PushButtonSetTextColor(pDesc->button_clock,PX_COLOR(255,192,0,0));
			pDesc->button_globalrotation->Enabled=PX_FALSE;
			PX_Object_PushButtonSetTextColor(pDesc->button_globalrotation,PX_COLOR(255,192,0,0));
			pDesc->button_impulse->Enabled=PX_FALSE;
			PX_Object_PushButtonSetTextColor(pDesc->button_impulse,PX_COLOR(255,192,0,0));
			pDesc->button_verticeseditor->Enabled=PX_FALSE;
			PX_Object_PushButtonSetTextColor(pDesc->button_verticeseditor,PX_COLOR(255,192,0,0));
			pDesc->button_rotation->Enabled=PX_FALSE;
			PX_Object_PushButtonSetTextColor(pDesc->button_rotation,PX_COLOR(255,192,0,0));
			pDesc->button_smarttranslation->Enabled=PX_FALSE;
			PX_Object_PushButtonSetTextColor(pDesc->button_smarttranslation,PX_COLOR(255,192,0,0));
			pDesc->button_stretch->Enabled=PX_FALSE;
			PX_Object_PushButtonSetTextColor(pDesc->button_stretch,PX_COLOR(255,192,0,0));
			pDesc->button_switchtexture->Enabled=PX_FALSE;
			PX_Object_PushButtonSetTextColor(pDesc->button_switchtexture,PX_COLOR(255,192,0,0));
			pDesc->button_pan->Enabled=PX_FALSE;
			PX_Object_PushButtonSetTextColor(pDesc->button_pan,PX_COLOR(255,192,0,0));
		}
	}
	else
	{
		targetx=pDesc->pruntime->surface_width/2-pObject->Width/2;
		targety=pDesc->pruntime->surface_height+1.0f;
		PX_LiveEditorModule_AnimationControllerFocusButton(pObject,PX_LIVEEDITORMODULE_ANIMATIONCONTROLLER_FOCUS_NONE);
	}


	if (PX_ABS(pObject->x-targetx)>elpased/10.f)
	{
		pObject->x+=(targetx-pObject->x)/PX_ABS(targetx-pObject->x)*elpased/10.f;
	}
	else
	{
		pObject->x=targetx;
	}


	if (PX_ABS(pObject->y-targety)>elpased/10.f)
	{
		pObject->y+=(targety-pObject->y)/PX_ABS(targety-pObject->y)*elpased/10.f;
	}
	else
	{
		pObject->y=targety;
	}

	if (pDesc->pLiveFramework->status==PX_LIVEFRAMEWORK_STATUS_PLAYING)
	{
		pDesc->button_play->Visible=PX_FALSE;
		pDesc->button_stop->Visible=PX_TRUE;
	}
	else
	{
		pDesc->button_play->Visible=PX_TRUE;
		pDesc->button_stop->Visible=PX_FALSE;
	}

}

static px_void PX_LiveEditorModule_AnimationControllerRender(px_surface *psurface, PX_Object *pObject,px_dword elpased)
{
	PX_LiveEditorModule_AnimationControllerUpdate(pObject,elpased);
	PX_GeoDrawRect(psurface,(px_int)pObject->x,(px_int)pObject->y,(px_int)(pObject->x+pObject->Width-1),(px_int)(pObject->y+pObject->Height-1),PX_OBJECT_UI_DEFAULT_BACKGROUNDCOLOR);
	PX_GeoDrawBorder(psurface,(px_int)pObject->x,(px_int)pObject->y,(px_int)(pObject->x+pObject->Width-1),(px_int)(pObject->y+pObject->Height-1),1,PX_OBJECT_UI_DEFAULT_BORDERCOLOR);
}
#include "PainterEngine_Resource_play_traw.h"
#include "PainterEngine_Resource_stop_traw.h"
#include "PainterEngine_Resource_smarttranslation_traw.h"
#include "PainterEngine_Resource_pointtranslation_traw.h"
#include "PainterEngine_Resource_stretch_traw.h"
#include "PainterEngine_Resource_rotation_traw.h"
#include "PainterEngine_Resource_clock_traw.h"
#include "PainterEngine_Resource_switchtexture_traw.h"
#include "PainterEngine_Resource_impulse_traw.h"
#include "PainterEngine_Resource_globalrotation_traw.h"
#include "PainterEngine_Resource_pan_traw.h"

px_void PX_LiveEditorModule_AnimationControllerOnButtonPlay(PX_Object *pObject,PX_Object_Event e,px_void *ptr)
{
	PX_Object *pAnimationControllerObject=(PX_Object *)ptr;
	PX_LiveEditorModule_AnimationController *pDesc=(PX_LiveEditorModule_AnimationController *)pAnimationControllerObject->pObject;
	pDesc->pLiveFramework->showFocusLayer=PX_FALSE;
	pDesc->pLiveFramework->showRange=PX_FALSE;
	pDesc->pLiveFramework->showRootHelperLine=PX_FALSE;

	pDesc->pLiveFramework->currentEditVertexIndex=-1;
	pDesc->pLiveFramework->currentEditFrameIndex=-1;
	pDesc->pLiveFramework->currentEditLayerIndex=-1;


	PX_LiveFrameworkPlayAnimation(pDesc->pLiveFramework,pDesc->pLiveFramework->currentEditAnimationIndex);
	PX_ObjectExecuteEvent(pAnimationControllerObject->pParent,PX_OBJECT_BUILD_EVENT(PX_LIVEFRAMEWORKMODULES_ANIMATIONCONTROLLER_EVENT_PLAY));

}

px_void PX_LiveEditorModule_AnimationControllerOnButtonStop(PX_Object *pObject,PX_Object_Event e,px_void *ptr)
{
	PX_Object *pAnimationControllerObject=(PX_Object *)ptr;
	PX_LiveEditorModule_AnimationController *pDesc=(PX_LiveEditorModule_AnimationController *)pAnimationControllerObject->pObject;
	PX_LiveFrameworkStop(pDesc->pLiveFramework);
	PX_ObjectExecuteEvent(pAnimationControllerObject->pParent,PX_OBJECT_BUILD_EVENT(PX_LIVEFRAMEWORKMODULES_ANIMATIONCONTROLLER_EVENT_STOP));
}

px_void PX_LiveEditorModule_AnimationControllerOnButtonClock(PX_Object *pObject,PX_Object_Event e,px_void *ptr)
{
	PX_Object *pAnimationControllerObject=(PX_Object *)ptr;
	PX_LiveEditorModule_AnimationController *pDesc=(PX_LiveEditorModule_AnimationController *)pAnimationControllerObject->pObject;

	PX_ObjectExecuteEvent(pAnimationControllerObject->pParent,PX_OBJECT_BUILD_EVENT(PX_LIVEFRAMEWORKMODULES_ANIMATIONCONTROLLER_EVENT_CLOCK));
}

px_void PX_LiveEditorModule_AnimationControllerOnButtonTranslation(PX_Object *pObject,PX_Object_Event e,px_void *ptr)
{
	PX_Object *pAnimationControllerObject=(PX_Object *)ptr;
	PX_LiveEditorModule_AnimationController *pDesc=(PX_LiveEditorModule_AnimationController *)pAnimationControllerObject->pObject;

	PX_ObjectExecuteEvent(pAnimationControllerObject->pParent,PX_OBJECT_BUILD_EVENT(PX_LIVEFRAMEWORKMODULES_ANIMATIONCONTROLLER_EVENT_SMARTTRANSFORM));
}

px_void PX_LiveEditorModule_AnimationControllerOnButtonPointTranslation(PX_Object *pObject,PX_Object_Event e,px_void *ptr)
{
	PX_Object *pAnimationControllerObject=(PX_Object *)ptr;
	PX_LiveEditorModule_AnimationController *pDesc=(PX_LiveEditorModule_AnimationController *)pAnimationControllerObject->pObject;

	PX_ObjectExecuteEvent(pAnimationControllerObject->pParent,PX_OBJECT_BUILD_EVENT(PX_LIVEFRAMEWORKMODULES_ANIMATIONCONTROLLER_EVENT_VERTEXTRANSFORM));
}

px_void PX_LiveEditorModule_AnimationControllerOnButtonGlobalRotation(PX_Object *pObject,PX_Object_Event e,px_void *ptr)
{
	PX_Object *pAnimationControllerObject=(PX_Object *)ptr;
	PX_LiveEditorModule_AnimationController *pDesc=(PX_LiveEditorModule_AnimationController *)pAnimationControllerObject->pObject;

	PX_ObjectExecuteEvent(pAnimationControllerObject->pParent,PX_OBJECT_BUILD_EVENT(PX_LIVEFRAMEWORKMODULES_ANIMATIONCONTROLLER_EVENT_GLOBALROTATION));
}

px_void PX_LiveEditorModule_AnimationControllerOnButtonImpulse(PX_Object *pObject,PX_Object_Event e,px_void *ptr)
{
	PX_Object *pAnimationControllerObject=(PX_Object *)ptr;
	PX_LiveEditorModule_AnimationController *pDesc=(PX_LiveEditorModule_AnimationController *)pAnimationControllerObject->pObject;

	PX_ObjectExecuteEvent(pAnimationControllerObject->pParent,PX_OBJECT_BUILD_EVENT(PX_LIVEFRAMEWORKMODULES_ANIMATIONCONTROLLER_EVENT_IMPULSE));
}

px_void PX_LiveEditorModule_AnimationControllerOnButtonPan(PX_Object *pObject,PX_Object_Event e,px_void *ptr)
{
	PX_Object *pAnimationControllerObject=(PX_Object *)ptr;
	PX_LiveEditorModule_AnimationController *pDesc=(PX_LiveEditorModule_AnimationController *)pAnimationControllerObject->pObject;

	PX_ObjectExecuteEvent(pAnimationControllerObject->pParent,PX_OBJECT_BUILD_EVENT(PX_LIVEFRAMEWORKMODULES_ANIMATIONCONTROLLER_EVENT_PANC));
}

px_void PX_LiveEditorModule_AnimationControllerOnButtonStretch(PX_Object *pObject,PX_Object_Event e,px_void *ptr)
{
	PX_Object *pAnimationControllerObject=(PX_Object *)ptr;
	PX_LiveEditorModule_AnimationController *pDesc=(PX_LiveEditorModule_AnimationController *)pAnimationControllerObject->pObject;

	PX_ObjectExecuteEvent(pAnimationControllerObject->pParent,PX_OBJECT_BUILD_EVENT(PX_LIVEFRAMEWORKMODULES_ANIMATIONCONTROLLER_EVENT_STRETCH));
}

px_void PX_LiveEditorModule_AnimationControllerOnButtonRotation(PX_Object *pObject,PX_Object_Event e,px_void *ptr)
{
	PX_Object *pAnimationControllerObject=(PX_Object *)ptr;
	PX_LiveEditorModule_AnimationController *pDesc=(PX_LiveEditorModule_AnimationController *)pAnimationControllerObject->pObject;

	PX_ObjectExecuteEvent(pAnimationControllerObject->pParent,PX_OBJECT_BUILD_EVENT(PX_LIVEFRAMEWORKMODULES_ANIMATIONCONTROLLER_EVENT_ROTATION));
}

px_void PX_LiveEditorModule_AnimationControllerOnButtonSwitchTexture(PX_Object *pObject,PX_Object_Event e,px_void *ptr)
{
	PX_Object *pAnimationControllerObject=(PX_Object *)ptr;
	PX_LiveEditorModule_AnimationController *pDesc=(PX_LiveEditorModule_AnimationController *)pAnimationControllerObject->pObject;

	PX_ObjectExecuteEvent(pAnimationControllerObject->pParent,PX_OBJECT_BUILD_EVENT(PX_LIVEFRAMEWORKMODULES_ANIMATIONCONTROLLER_EVENT_SWITCHTEXTURE));
}

PX_Object * PX_LiveEditorModule_AnimationControllerInstall(PX_Object *pparent,PX_Runtime *pruntime,PX_FontModule *fm,PX_LiveFramework *pLiveFramework,PX_Json *pLanguageJson)
{
	PX_Object *pObject;
	PX_LiveEditorModule_AnimationController AnimationController,*pLiveController;
	PX_memset(&AnimationController,0,sizeof(PX_LiveEditorModule_AnimationController));

	pObject=PX_ObjectCreateEx(&pruntime->mp_ui,pparent,0,0,0,330,68,0,0,PX_NULL,PX_LiveEditorModule_AnimationControllerRender,PX_NULL,&AnimationController,sizeof(AnimationController));
	pLiveController=(PX_LiveEditorModule_AnimationController *)pObject->pObject;

	pLiveController->pruntime=pruntime;
	pLiveController->pLiveFramework=pLiveFramework;
	pLiveController->fontmodule=fm;
	pLiveController->pLanguageJson=pLanguageJson;

	if(!PX_ShapeCreateFromMemory(&pruntime->mp_resources,(px_void *)play_traw,sizeof(play_traw),&pLiveController->shape_play)) return PX_FALSE;
	if(!PX_ShapeCreateFromMemory(&pruntime->mp_resources,(px_void *)stop_traw,sizeof(stop_traw),&pLiveController->shape_stop)) return PX_FALSE;
	if(!PX_ShapeCreateFromMemory(&pruntime->mp_resources,(px_void *)smarttranslation_traw,sizeof(smarttranslation_traw),&pLiveController->shape_smarttranslation)) return PX_FALSE;
	if(!PX_ShapeCreateFromMemory(&pruntime->mp_resources,(px_void *)pointtranslation_traw,sizeof(pointtranslation_traw),&pLiveController->shape_pointtranslation)) return PX_FALSE;
	if(!PX_ShapeCreateFromMemory(&pruntime->mp_resources,(px_void *)stretch_traw,sizeof(stretch_traw),&pLiveController->shape_stretch)) return PX_FALSE;
	if(!PX_ShapeCreateFromMemory(&pruntime->mp_resources,(px_void *)rotation_traw,sizeof(rotation_traw),&pLiveController->shape_rotation)) return PX_FALSE;
	if(!PX_ShapeCreateFromMemory(&pruntime->mp_resources,(px_void *)clock_traw,sizeof(clock_traw),&pLiveController->shape_clock)) return PX_FALSE;
	if(!PX_ShapeCreateFromMemory(&pruntime->mp_resources,(px_void *)switchtexture_traw,sizeof(clock_traw),&pLiveController->shape_switchtexture)) return PX_FALSE;
	if(!PX_ShapeCreateFromMemory(&pruntime->mp_resources,(px_void *)impulse_traw,sizeof(impulse_traw),&pLiveController->shape_impulse)) return PX_FALSE;
	if(!PX_ShapeCreateFromMemory(&pruntime->mp_resources,(px_void *)globalrotation_traw,sizeof(globalrotation_traw),&pLiveController->shape_globalrotation)) return PX_FALSE;
	if(!PX_ShapeCreateFromMemory(&pruntime->mp_resources,(px_void *)pan_traw,sizeof(pan_traw),&pLiveController->shape_pan)) return PX_FALSE;
	pLiveController->messagebox=PX_Object_MessageBoxCreate(&pruntime->mp_ui,pObject,fm);


	pLiveController->button_play=PX_Object_PushButtonCreate(&pruntime->mp_ui,pObject,32*0+1,0,32,32,"",fm);
	PX_Object_PushButtonSetShape(pLiveController->button_play,&pLiveController->shape_play);
	PX_ObjectRegisterEvent(pLiveController->button_play,PX_OBJECT_EVENT_EXECUTE,PX_LiveEditorModule_AnimationControllerOnButtonPlay,pObject);

	pLiveController->button_stop=PX_Object_PushButtonCreate(&pruntime->mp_ui,pObject,32*0+1,0,32,32,"",fm);
	PX_Object_PushButtonSetShape(pLiveController->button_stop,&pLiveController->shape_stop);
	PX_ObjectRegisterEvent(pLiveController->button_stop,PX_OBJECT_EVENT_EXECUTE,PX_LiveEditorModule_AnimationControllerOnButtonStop,pObject);

	pLiveController->button_clock=PX_Object_PushButtonCreate(&pruntime->mp_ui,pObject,32*1+1,0,32,32,"",fm);
	PX_Object_PushButtonSetShape(pLiveController->button_clock,&pLiveController->shape_clock);
	PX_ObjectRegisterEvent(pLiveController->button_clock,PX_OBJECT_EVENT_EXECUTE,PX_LiveEditorModule_AnimationControllerOnButtonClock,pObject);

	pLiveController->button_smarttranslation=PX_Object_PushButtonCreate(&pruntime->mp_ui,pObject,32*2+1,0,32,32,"",fm);
	PX_Object_PushButtonSetShape(pLiveController->button_smarttranslation,&pLiveController->shape_smarttranslation);
	PX_ObjectRegisterEvent(pLiveController->button_smarttranslation,PX_OBJECT_EVENT_EXECUTE,PX_LiveEditorModule_AnimationControllerOnButtonTranslation,pObject);

	pLiveController->button_rotation=PX_Object_PushButtonCreate(&pruntime->mp_ui,pObject,32*3+1,0,32,32,"",fm);
	PX_Object_PushButtonSetShape(pLiveController->button_rotation,&pLiveController->shape_globalrotation);
	PX_ObjectRegisterEvent(pLiveController->button_rotation,PX_OBJECT_EVENT_EXECUTE,PX_LiveEditorModule_AnimationControllerOnButtonRotation,pObject);

	pLiveController->button_stretch=PX_Object_PushButtonCreate(&pruntime->mp_ui,pObject,32*4+1,0,32,32,"",fm);
	PX_Object_PushButtonSetShape(pLiveController->button_stretch,&pLiveController->shape_stretch);
	PX_ObjectRegisterEvent(pLiveController->button_stretch,PX_OBJECT_EVENT_EXECUTE,PX_LiveEditorModule_AnimationControllerOnButtonStretch,pObject);

	pLiveController->button_globalrotation=PX_Object_PushButtonCreate(&pruntime->mp_ui,pObject,32*5+1,0,32,32,"",fm);
	PX_Object_PushButtonSetShape(pLiveController->button_globalrotation,&pLiveController->shape_rotation);
	PX_ObjectRegisterEvent(pLiveController->button_globalrotation,PX_OBJECT_EVENT_EXECUTE,PX_LiveEditorModule_AnimationControllerOnButtonGlobalRotation,pObject);

	pLiveController->button_switchtexture=PX_Object_PushButtonCreate(&pruntime->mp_ui,pObject,32*6+1,0,32,32,"",fm);
	PX_Object_PushButtonSetShape(pLiveController->button_switchtexture,&pLiveController->shape_switchtexture);
	PX_ObjectRegisterEvent(pLiveController->button_switchtexture,PX_OBJECT_EVENT_EXECUTE,PX_LiveEditorModule_AnimationControllerOnButtonSwitchTexture,pObject);

	pLiveController->button_verticeseditor=PX_Object_PushButtonCreate(&pruntime->mp_ui,pObject,32*7+1,0,32,32,"",fm);
	PX_Object_PushButtonSetShape(pLiveController->button_verticeseditor,&pLiveController->shape_pointtranslation);
	PX_ObjectRegisterEvent(pLiveController->button_verticeseditor,PX_OBJECT_EVENT_EXECUTE,PX_LiveEditorModule_AnimationControllerOnButtonPointTranslation,pObject);

	pLiveController->button_impulse=PX_Object_PushButtonCreate(&pruntime->mp_ui,pObject,32*8+1,0,32,32,"",fm);
	PX_Object_PushButtonSetShape(pLiveController->button_impulse,&pLiveController->shape_impulse);
	PX_ObjectRegisterEvent(pLiveController->button_impulse,PX_OBJECT_EVENT_EXECUTE,PX_LiveEditorModule_AnimationControllerOnButtonImpulse,pObject);

	pLiveController->button_pan=PX_Object_PushButtonCreate(&pruntime->mp_ui,pObject,32*9+1,0,32,32,"",fm);
	PX_Object_PushButtonSetShape(pLiveController->button_pan,&pLiveController->shape_pan);
	PX_ObjectRegisterEvent(pLiveController->button_pan,PX_OBJECT_EVENT_EXECUTE,PX_LiveEditorModule_AnimationControllerOnButtonPan,pObject);

	pObject->Height=33;
	pObject->Width=32*9+1;

	pObject->x=pruntime->surface_width/2-pObject->Width/2;
	pObject->y=pruntime->surface_height+1.0f;
	pObject->Enabled=PX_FALSE;
	

	return pObject;
}

px_void PX_LiveEditorModule_AnimationControllerUninstall(PX_Object *pObject)
{
	PX_LiveEditorModule_AnimationController *pac=(PX_LiveEditorModule_AnimationController *)pObject->pObject;
	PX_ShapeFree(&pac->shape_play);
	PX_ShapeFree(&pac->shape_pointtranslation);
	PX_ShapeFree(&pac->shape_rotation);
	PX_ShapeFree(&pac->shape_stop);
	PX_ShapeFree(&pac->shape_stretch);
	PX_ShapeFree(&pac->shape_smarttranslation);
	PX_ShapeFree(&pac->shape_switchtexture);
	PX_ShapeFree(&pac->shape_clock);
	PX_ShapeFree(&pac->shape_impulse);
	PX_ShapeFree(&pac->shape_pan);
	PX_ObjectDelete(pObject);
}

px_void PX_LiveEditorModule_AnimationControllerFocusButton(PX_Object *pObject,PX_LIVEEDITORMODULE_ANIMATIONCONTROLLER_FOCUS focus)
{
	PX_LiveEditorModule_AnimationController *pDesc=(PX_LiveEditorModule_AnimationController *)pObject->pObject;

	if (pDesc->focus==focus)
	{
		return;
	}
	pDesc->focus=focus;

	PX_Object_PushButtonSetBackgroundColor(pDesc->button_clock,PX_OBJECT_UI_DEFAULT_BACKGROUNDCOLOR);
	PX_Object_PushButtonSetBackgroundColor(pDesc->button_smarttranslation,PX_OBJECT_UI_DEFAULT_BACKGROUNDCOLOR);
	PX_Object_PushButtonSetBackgroundColor(pDesc->button_globalrotation,PX_OBJECT_UI_DEFAULT_BACKGROUNDCOLOR);
	PX_Object_PushButtonSetBackgroundColor(pDesc->button_stretch,PX_OBJECT_UI_DEFAULT_BACKGROUNDCOLOR);
	PX_Object_PushButtonSetBackgroundColor(pDesc->button_rotation,PX_OBJECT_UI_DEFAULT_BACKGROUNDCOLOR);
	PX_Object_PushButtonSetBackgroundColor(pDesc->button_switchtexture,PX_OBJECT_UI_DEFAULT_BACKGROUNDCOLOR);
	PX_Object_PushButtonSetBackgroundColor(pDesc->button_verticeseditor,PX_OBJECT_UI_DEFAULT_BACKGROUNDCOLOR);
	PX_Object_PushButtonSetBackgroundColor(pDesc->button_impulse,PX_OBJECT_UI_DEFAULT_BACKGROUNDCOLOR);
	PX_Object_PushButtonSetBackgroundColor(pDesc->button_pan,PX_OBJECT_UI_DEFAULT_BACKGROUNDCOLOR);

	switch (focus)
	{
	case PX_LIVEEDITORMODULE_ANIMATIONCONTROLLER_FOCUS_NONE:
		break;
	case PX_LIVEEDITORMODULE_ANIMATIONCONTROLLER_FOCUS_TIMESTAMP:
		PX_Object_PushButtonSetBackgroundColor(pDesc->button_clock,PX_COLOR(255,0,0,0));
		break;
	case PX_LIVEEDITORMODULE_ANIMATIONCONTROLLER_FOCUS_SMART:
		PX_Object_PushButtonSetBackgroundColor(pDesc->button_smarttranslation,PX_COLOR(255,0,0,0));
		break;
	case PX_LIVEEDITORMODULE_ANIMATIONCONTROLLER_FOCUS_GLOBALROTATION:
		PX_Object_PushButtonSetBackgroundColor(pDesc->button_globalrotation,PX_COLOR(255,0,0,0));
		break;
	case PX_LIVEEDITORMODULE_ANIMATIONCONTROLLER_FOCUS_STRETCH:
		PX_Object_PushButtonSetBackgroundColor(pDesc->button_stretch,PX_COLOR(255,0,0,0));
		break;
	case PX_LIVEEDITORMODULE_ANIMATIONCONTROLLER_FOCUS_ROTATION:
		PX_Object_PushButtonSetBackgroundColor(pDesc->button_rotation,PX_COLOR(255,0,0,0));
		break;
	case PX_LIVEEDITORMODULE_ANIMATIONCONTROLLER_FOCUS_SWITCHTEXTURE:
		PX_Object_PushButtonSetBackgroundColor(pDesc->button_switchtexture,PX_COLOR(255,0,0,0));
		break;
	case PX_LIVEEDITORMODULE_ANIMATIONCONTROLLER_FOCUS_IMPULSE:
		PX_Object_PushButtonSetBackgroundColor(pDesc->button_impulse,PX_COLOR(255,0,0,0));
		break;
	case PX_LIVEEDITORMODULE_ANIMATIONCONTROLLER_FOCUS_VERTICES:
		PX_Object_PushButtonSetBackgroundColor(pDesc->button_verticeseditor,PX_COLOR(255,0,0,0));
		break;
	case PX_LIVEEDITORMODULE_ANIMATIONCONTROLLER_FOCUS_PANC:
		PX_Object_PushButtonSetBackgroundColor(pDesc->button_pan,PX_COLOR(255,0,0,0));
		break;
	}

}

px_void PX_LiveEditorModule_AnimationControllerEnable(PX_Object *pObject)
{
	pObject->Enabled=PX_TRUE;
}

px_void PX_LiveEditorModule_AnimationControllerDisable(PX_Object *pObject)
{
	pObject->Enabled=PX_FALSE;
}
