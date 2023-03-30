#include "PainterEngine_Application.h"

PX_Application App;

static px_void PX_ApplicationSwitchEditMode(PX_Application *pApp)
{
	PX_LiveEditorModule_MenuEditMode(pApp->module_menu);

	PX_LiveEditorModule_DisplayEnable(pApp->module_display);
	PX_LiveEditorModule_DisplayResetPosition(pApp->module_display);

	PX_LiveEditorModule_LiveControllerEnable(pApp->module_livecontroller);
	PX_LiveEditorModule_TranslationEnable(pApp->module_translation);
}


px_void PX_ApplicationOnScheduleEvent_CreateProject(PX_Application *pApp,PX_Object_Event e,px_void *ptr)
{
	PX_LiveEditorModule_CreateProjectEnable(pApp->module_createproject);
}

px_void PX_ApplicationOnScheduleEvent_Import(PX_Application *pApp,PX_Object_Event e,px_void *ptr)
{
	PX_LiveEditorModule_ImportLiveEnable(pApp->module_importlive);
}
px_void PX_ApplicationOnScheduleEvent_ImportLive_exit(PX_Application *pApp,PX_Object_Event e,px_void *ptr)
{
	PX_LiveEditorModule_ImportLiveDisable(pApp->module_importlive);
}

px_void PX_ApplicationOnScheduleEvent_ImportLive_confirm(PX_Application *pApp,PX_Object_Event e,px_void *ptr)
{
	PX_LiveEditorModule_ImportLiveDisable(pApp->module_importlive);
	PX_ApplicationSwitchEditMode(pApp);
}


px_void PX_ApplicationOnScheduleEvent_Export(PX_Application *pApp,PX_Object_Event e,px_void *ptr)
{
	PX_LiveEditorModule_ExportLiveEnable(pApp->module_exportlive);
}

px_void PX_ApplicationOnScheduleEvent_ExportLive_exit(PX_Application *pApp,PX_Object_Event e,px_void *ptr)
{
	PX_LiveEditorModule_ExportLiveDisable(pApp->module_exportlive);
}

px_void PX_ApplicationOnScheduleEvent_CreateProject_exit(PX_Application *pApp,PX_Object_Event e,px_void *ptr)
{
	PX_LiveEditorModule_CreateProjectDisable(pApp->module_createproject);
}

px_void PX_ApplicationOnScheduleEvent_CreateProject_confirm(PX_Application *pApp,PX_Object_Event e,px_void *ptr)
{
	PX_ApplicationSwitchEditMode(pApp);
	PX_LiveEditorModule_CreateProjectDisable(pApp->module_createproject);
}



px_void PX_ApplicationOnScheduleEvent_LoadImage(PX_Application *pApp,PX_Object_Event e,px_void *ptr)
{
	PX_LiveEditorModule_ImportImageEnable(pApp->module_importimage);
}

px_void PX_ApplicationOnScheduleEvent_Message(PX_Application *pApp,PX_Object_Event e,px_void *ptr)
{
	PX_Object_MessageBoxAlertOk(pApp->module_messagebox,PX_Object_Event_GetStringPtr(e),PX_NULL,PX_NULL);
}

px_void PX_ApplicationOnScheduleEvent_MenuLayer(PX_Application *pApp,PX_Object_Event e,px_void *ptr)
{
	if (pApp->module_livecontroller->Enabled)
	{
		PX_LiveEditorModule_LiveControllerDisable(pApp->module_livecontroller);
	}
	else
	{
		PX_LiveEditorModule_LiveControllerEnable(pApp->module_livecontroller);
	}
}
px_void PX_ApplicationOnScheduleEvent_ImportImage_exit(PX_Application *pApp,PX_Object_Event e,px_void *ptr)
{
	PX_LiveEditorModule_ImportImageDisable(pApp->module_importimage);
}

px_void PX_ApplicationOnScheduleEvent_BuildMesh(PX_Application *pApp,PX_Object_Event e,px_void *ptr)
{
	px_int index=PX_LiveEditorModule_LiveControllerGetCurrentEditLayer(pApp->module_livecontroller);
	if (index>=0&&index<pApp->liveFramework.layers.size)
	{
		PX_LiveEditorModule_LiveControllerDisable(pApp->module_livecontroller);
		PX_LiveEditorModule_BuildMeshEnable(pApp->module_buildmesh);
	}
}

px_void PX_ApplicationOnScheduleEvent_BuildMeshExit(PX_Application *pApp,PX_Object_Event e,px_void *ptr)
{
	PX_LiveEditorModule_LiveControllerEnable(pApp->module_livecontroller);
	PX_LiveEditorModule_BuildMeshDisable(pApp->module_buildmesh);
}

px_void PX_ApplicationOnScheduleEvent_SetKey(PX_Application *pApp,PX_Object_Event e,px_void *ptr)
{
	px_int index=PX_LiveEditorModule_LiveControllerGetCurrentEditLayer(pApp->module_livecontroller);
	if (index>=0&&index<pApp->liveFramework.layers.size)
	{
		PX_LiveEditorModule_LiveControllerDisable(pApp->module_livecontroller);
		PX_LiveEditorModule_SetKeyEnable(pApp->module_setkey);
	}
}

px_void PX_ApplicationOnScheduleEvent_SetKeyExit(PX_Application *pApp,PX_Object_Event e,px_void *ptr)
{
	PX_LiveEditorModule_LiveControllerEnable(pApp->module_livecontroller);
	PX_LiveEditorModule_SetKeyDisable(pApp->module_setkey);
}

px_void PX_ApplicationOnScheduleEvent_LinkKey(PX_Application *pApp,PX_Object_Event e,px_void *ptr)
{
	
	PX_LiveEditorModule_LiveControllerDisable(pApp->module_livecontroller);
	PX_LiveEditorModule_LinkkeyEnable(pApp->module_linkkey);
	
}

px_void PX_ApplicationOnScheduleEvent_LinkKeyExit(PX_Application *pApp,PX_Object_Event e,px_void *ptr)
{
	PX_LiveEditorModule_LiveControllerEnable(pApp->module_livecontroller);
	PX_LiveEditorModule_LinkkeyDisable(pApp->module_linkkey);
}
//////////////////////////////////////////////////////////////////////////
//Animation Controller actions

static px_void PX_ApplicationOnScheduleEvent_DisableAllAnimationEditorControllers(PX_Application *pApp)
{
	PX_LiveEditorModule_TimeStampEditorDisable(pApp->module_timestampeditor);
	PX_LiveEditorModule_KeyTransformDisable(pApp->module_keytransform);
	PX_LiveEditorModule_GlobalRotationDisable(pApp->module_globalrotation);
	PX_LiveEditorModule_VertexTransformDisable(pApp->module_vertextransform);
	PX_LiveEditorModule_ImpulseEditorDisable(pApp->module_impulse);
	PX_LiveEditorModule_FrameLiveTextureEditorDisable(pApp->module_framelivetextureeditor);
	PX_LiveEditorModule_PancDisable(pApp->module_panc);
}

px_void PX_ApplicationOnScheduleEvent_LiveControllerStateChanged(PX_Application *pApp,PX_Object_Event e,px_void *ptr)
{
	PX_LiveEditorModule_LiveController *pController=(PX_LiveEditorModule_LiveController *)pApp->module_livecontroller->pObject;
	
	PX_ApplicationOnScheduleEvent_DisableAllAnimationEditorControllers(pApp);

	if (pController->state==PX_LIVEEDITORMODULE_LIVECONTROLLER_STATE_ANIMATION)
	{
		PX_LiveEditorModule_AnimationControllerEnable(pApp->module_animationcontroller);
		PX_LiveEditorModule_FrameControllerEnable(pApp->module_framecontroller);
		PX_LiveEditorModule_TranslationDisable(pApp->module_translation);

	}
	else
	{
		PX_LiveEditorModule_AnimationControllerDisable(pApp->module_animationcontroller);
		PX_LiveEditorModule_FrameControllerDisable(pApp->module_framecontroller);
		PX_LiveEditorModule_TranslationEnable(pApp->module_translation);
	}
	
}

px_void PX_ApplicationOnScheduleEvent_NewFrame(PX_Application *pApp,PX_Object_Event e,px_void *ptr)
{
	PX_LiveEditorModule_NewFrameEditorEnable(pApp->module_newframeeditor,PX_FALSE);
}

px_void PX_ApplicationOnScheduleEvent_CopyFrame(PX_Application *pApp,PX_Object_Event e,px_void *ptr)
{
	PX_LiveEditorModule_NewFrameEditorEnable(pApp->module_newframeeditor,PX_TRUE);
}

px_void PX_ApplicationOnScheduleEvent_NewFrameEditorExit(PX_Application *pApp,PX_Object_Event e,px_void *ptr)
{
	PX_LiveEditorModule_NewFrameEditorDisable(pApp->module_newframeeditor);
}




//////////////////////////////////////////////////////////////////////////
//TimeStampEditor

//////////////////////////////////////////////////////////////////////////
//play / stop
px_void PX_ApplicationOnScheduleEvent_AnimationControllerPlayStop(PX_Application *pApp,PX_Object_Event e,px_void *ptr)
{
	PX_ApplicationOnScheduleEvent_DisableAllAnimationEditorControllers(pApp);
	PX_LiveEditorModule_AnimationControllerFocusButton(pApp->module_animationcontroller,PX_LIVEEDITORMODULE_ANIMATIONCONTROLLER_FOCUS_NONE);
}
//////////////////////////////////////////////////////////////////////////
//time stamp
px_void PX_ApplicationOnScheduleEvent_AnimationControllerTimeStamp(PX_Application *pApp,PX_Object_Event e,px_void *ptr)
{
	PX_ApplicationOnScheduleEvent_DisableAllAnimationEditorControllers(pApp);
	PX_LiveEditorModule_TimeStampEditorEnable(pApp->module_timestampeditor);
	PX_LiveEditorModule_AnimationControllerFocusButton(pApp->module_animationcontroller,PX_LIVEEDITORMODULE_ANIMATIONCONTROLLER_FOCUS_TIMESTAMP);
}

px_void PX_ApplicationOnScheduleEvent_TimeStampEditorExit(PX_Application *pApp,PX_Object_Event e,px_void *ptr)
{
	PX_ApplicationOnScheduleEvent_DisableAllAnimationEditorControllers(pApp);
	PX_LiveEditorModule_AnimationControllerFocusButton(pApp->module_animationcontroller,PX_LIVEEDITORMODULE_ANIMATIONCONTROLLER_FOCUS_NONE);
}


//////////////////////////////////////////////////////////////////////////
//smart
px_void PX_ApplicationOnScheduleEvent_SmartTransform(PX_Application *pApp,PX_Object_Event e,px_void *ptr)
{
	PX_ApplicationOnScheduleEvent_DisableAllAnimationEditorControllers(pApp);
	PX_LiveEditorModule_KeyTransformEnable(pApp->module_keytransform,PX_LIVEEDITORMODULE_KEYTRANSFORM_MODE_ALL);
	PX_LiveEditorModule_AnimationControllerFocusButton(pApp->module_animationcontroller,PX_LIVEEDITORMODULE_ANIMATIONCONTROLLER_FOCUS_SMART);
}


//////////////////////////////////////////////////////////////////////////
//global rotation
px_void PX_ApplicationOnScheduleEvent_GlobalRotation(PX_Application *pApp,PX_Object_Event e,px_void *ptr)
{
	PX_ApplicationOnScheduleEvent_DisableAllAnimationEditorControllers(pApp);
	PX_LiveEditorModule_GlobalRotationEnable(pApp->module_globalrotation);
	PX_LiveEditorModule_AnimationControllerFocusButton(pApp->module_animationcontroller,PX_LIVEEDITORMODULE_ANIMATIONCONTROLLER_FOCUS_GLOBALROTATION);
}


//////////////////////////////////////////////////////////////////////////
//stretch
px_void PX_ApplicationOnScheduleEvent_Stretch(PX_Application *pApp,PX_Object_Event e,px_void *ptr)
{
	PX_ApplicationOnScheduleEvent_DisableAllAnimationEditorControllers(pApp);
	PX_LiveEditorModule_KeyTransformEnable(pApp->module_keytransform,PX_LIVEEDITORMODULE_KEYTRANSFORM_MODE_STRETCH);
	PX_LiveEditorModule_AnimationControllerFocusButton(pApp->module_animationcontroller,PX_LIVEEDITORMODULE_ANIMATIONCONTROLLER_FOCUS_STRETCH);
}


//////////////////////////////////////////////////////////////////////////
//rotation
px_void PX_ApplicationOnScheduleEvent_Rotation(PX_Application *pApp,PX_Object_Event e,px_void *ptr)
{
	PX_ApplicationOnScheduleEvent_DisableAllAnimationEditorControllers(pApp);
	PX_LiveEditorModule_KeyTransformEnable(pApp->module_keytransform,PX_LIVEEDITORMODULE_KEYTRANSFORM_MODE_ROTATION);
	PX_LiveEditorModule_AnimationControllerFocusButton(pApp->module_animationcontroller,PX_LIVEEDITORMODULE_ANIMATIONCONTROLLER_FOCUS_ROTATION);
}


//////////////////////////////////////////////////////////////////////////
//impulse
px_void PX_ApplicationOnScheduleEvent_Impulse(PX_Application *pApp,PX_Object_Event e,px_void *ptr)
{
	PX_ApplicationOnScheduleEvent_DisableAllAnimationEditorControllers(pApp);
	PX_LiveEditorModule_ImpulseEditorEnable(pApp->module_impulse);
	PX_LiveEditorModule_AnimationControllerFocusButton(pApp->module_animationcontroller,PX_LIVEEDITORMODULE_ANIMATIONCONTROLLER_FOCUS_IMPULSE);
}



//////////////////////////////////////////////////////////////////////////
//vertices

px_void PX_ApplicationOnScheduleEvent_VertexTransfom(PX_Application *pApp,PX_Object_Event e,px_void *ptr)
{
	PX_ApplicationOnScheduleEvent_DisableAllAnimationEditorControllers(pApp);
	PX_LiveEditorModule_VertexTransformEnable(pApp->module_vertextransform);
	PX_LiveEditorModule_AnimationControllerFocusButton(pApp->module_animationcontroller,PX_LIVEEDITORMODULE_ANIMATIONCONTROLLER_FOCUS_VERTICES);
}


//////////////////////////////////////////////////////////////////////////
//texture

px_void PX_ApplicationOnScheduleEvent_FrameLiveTextureEditor(PX_Application *pApp,PX_Object_Event e,px_void *ptr)
{
	PX_ApplicationOnScheduleEvent_DisableAllAnimationEditorControllers(pApp);
	PX_LiveEditorModule_FrameLiveTextureEditorEnable(pApp->module_framelivetextureeditor);
	PX_LiveEditorModule_AnimationControllerFocusButton(pApp->module_animationcontroller,PX_LIVEEDITORMODULE_ANIMATIONCONTROLLER_FOCUS_SWITCHTEXTURE);
}


//////////////////////////////////////////////////////////////////////////
//Panc

px_void PX_ApplicationOnScheduleEvent_Panc(PX_Application* pApp, PX_Object_Event e, px_void* ptr)
{
	PX_ApplicationOnScheduleEvent_DisableAllAnimationEditorControllers(pApp);
	PX_LiveEditorModule_PancEnable(pApp->module_panc);
	PX_LiveEditorModule_AnimationControllerFocusButton(pApp->module_animationcontroller, PX_LIVEEDITORMODULE_ANIMATIONCONTROLLER_FOCUS_PANC);
}

px_void PX_ApplicationOnScheduleEvent_PancExit(PX_Application* pApp, PX_Object_Event e, px_void* ptr)
{
	PX_ApplicationOnScheduleEvent_DisableAllAnimationEditorControllers(pApp);
	PX_LiveEditorModule_AnimationControllerFocusButton(pApp->module_animationcontroller, PX_LIVEEDITORMODULE_ANIMATIONCONTROLLER_FOCUS_NONE);
}
//////////////////////////////////////////////////////////////////////////
//showfps
px_void PX_ApplicationOnScheduleEvent_ShowFPS(PX_Application *pApp,PX_Object_Event e,px_void *ptr)
{
	PX_LiveEditorModule_DisplayShowFPS(pApp->module_display);
}

//////////////////////////////////////////////////////////////////////////
//reset position
px_void PX_ApplicationOnScheduleEvent_ResetPosition(PX_Application *pApp,PX_Object_Event e,px_void *ptr)
{
	PX_LiveEditorModule_DisplayResetPosition(pApp->module_display);
}


//////////////////////////////////////////////////////////////////////////
//show helper line
px_void PX_ApplicationOnScheduleEvent_ShowHelperLine(PX_Application *pApp,PX_Object_Event e,px_void *ptr)
{
	PX_LiveEditorModule_DisplayShowHelperLine(pApp->module_display);
}

px_void PX_ApplicationOnScheduleEvent(PX_Object *pObject,PX_Object_Event e,px_void *ptr)
{
	PX_Application *pApp=(PX_Application *)ptr;
	switch(e.Event)
	{
	case PX_LIVEFRAMEWORKMODULES_MENU_EVENT_CREATEPROJECT:
		{
			PX_ApplicationOnScheduleEvent_CreateProject(pApp,e,ptr);
		}
		break;
	case PX_LIVEFRAMEWORKMODULES_MENU_EVENT_IMPORT:
		{
			PX_ApplicationOnScheduleEvent_Import(pApp,e,ptr);
		}
		break;
	case PX_LIVEFRAMEWORKMODULES_MENU_EVENT_EXPORT:
		{
			PX_ApplicationOnScheduleEvent_Export(pApp,e,ptr);
		}
		break;
	case PX_LIVEEDITORMODULE_EVENT_CREATEPROJECT_EXIT:
		{
			PX_ApplicationOnScheduleEvent_CreateProject_exit(pApp,e,ptr);
		}
		break;
	case PX_LIVEEDITORMODULE_EVENT_CREATEPROJECT_CONFIRM:
		{
			PX_ApplicationOnScheduleEvent_CreateProject_confirm(pApp,e,ptr);
		}
		break;
	case PX_LIVEEDITORMODULE_IMPORTLIVE_EVENT_EXIT:
		{
			PX_ApplicationOnScheduleEvent_ImportLive_exit(pApp,e,ptr);
		}
		break;
	case PX_LIVEEDITORMODULE_IMPORTLIVE_EVENT_CONFIRM:
		{
			PX_ApplicationOnScheduleEvent_ImportLive_confirm(pApp,e,ptr);
		}
		break;
	case PX_LIVEFRAMEWORKMODULES_LIVECONTROLLER_EVENT_LOADIMAGE:
	case PX_LIVEFRAMEWORKMODULES_MENU_EVENT_LOADIMAGE:
		{
			PX_ApplicationOnScheduleEvent_LoadImage(pApp,e,ptr);
		}
		break;
	case PX_LIVEEDITORMODULE_IMPORTLIVE_EVENT_MESSAGE:
	case PX_LIVEEDITORMODULE_IMPORTIMAGE_EVENT_MESSAGE:
	case PX_LIVEEDITORMODULE_EXPORTLIVE_EVENT_MESSAGE:
	case PX_LIVEEDITORMODULE_BUILDMESH_EVENT_MESSAGE:
	case PX_LIVEEDITORMODULE_LIVECONTROLLER_EVENT_MESSAGE:
	case PX_LIVEFRAMEWORKMODULES_MENU_EVENT_MESSAGE:
		{
			PX_ApplicationOnScheduleEvent_Message(pApp,e,ptr);
		}
		break;
	case PX_LIVEFRAMEWORKMODULES_MENU_EVENT_WINDOW_LAYER:
		{
			PX_ApplicationOnScheduleEvent_MenuLayer(pApp,e,ptr);
		}
		break;
	case PX_LIVEEDITORMODULE_IMPORTIMAGE_EVENT_EXIT:
		{
			PX_ApplicationOnScheduleEvent_ImportImage_exit(pApp,e,ptr);
		}
		break;
	case PX_LIVEFRAMEWORKMODULES_LIVECONTROLLER_EVENT_BUILDMESH:
		{
			PX_ApplicationOnScheduleEvent_BuildMesh(pApp,e,ptr);
		}
		break;
	case PX_LIVEFRAMEWORKMODULES_LIVECONTROLLER_EVENT_LINKKEY:
		{
			PX_ApplicationOnScheduleEvent_LinkKey(pApp,e,ptr);
		}
		break;
	case PX_LIVEFRAMEWORKMODULES_BUILDMESH_EVENT_EXIT:
		{
			PX_ApplicationOnScheduleEvent_BuildMeshExit(pApp,e,ptr);
		}
		break;
	case PX_LIVEFRAMEWORKMODULES_LIVECONTROLLER_EVENT_SETKEY:
		{
			PX_ApplicationOnScheduleEvent_SetKey(pApp,e,ptr);
		}
		break;
	case PX_LIVEFRAMEWORKMODULES_SETKEY_EVENT_EXIT:
		{
			PX_ApplicationOnScheduleEvent_SetKeyExit(pApp,e,ptr);
		}
		break;
	case PX_LIVEFRAMEWORKMODULES_LINKKEY_EVENT_EXIT:
		{
			PX_ApplicationOnScheduleEvent_LinkKeyExit(pApp,e,ptr);
		}
		break;
	case PX_LIVEFRAMEWORKMODULES_LIVECONTROLLER_EVENT_STATECHANGE:
		{
			PX_ApplicationOnScheduleEvent_LiveControllerStateChanged(pApp,e,ptr);
		}
		break;
	case PX_FRAMECONTROLLERMODULE_EVENT_NEWFRAME:
		{
			PX_ApplicationOnScheduleEvent_NewFrame(pApp,e,ptr);
		}
		break;
	case PX_FRAMECONTROLLERMODULE_EVENT_COPYFRAME:
		{
			PX_ApplicationOnScheduleEvent_CopyFrame(pApp,e,ptr);
		}
		break;
	case PX_LIVEEDITORMODULE_NEWFRAMEEDITOR_EVENT_EXIT:
		{
			PX_ApplicationOnScheduleEvent_NewFrameEditorExit(pApp,e,ptr);
		}
		break;
	//////////////////////////////////////////////////////////////////////////
	case PX_LIVEFRAMEWORKMODULES_ANIMATIONCONTROLLER_EVENT_PLAY:
		{
			PX_ApplicationOnScheduleEvent_AnimationControllerPlayStop(pApp,e,ptr);
		}
		break;
	case PX_LIVEFRAMEWORKMODULES_ANIMATIONCONTROLLER_EVENT_STOP:
		{
			PX_ApplicationOnScheduleEvent_AnimationControllerPlayStop(pApp,e,ptr);
		}
		break;

	case PX_LIVEFRAMEWORKMODULES_ANIMATIONCONTROLLER_EVENT_CLOCK:
		{
			PX_ApplicationOnScheduleEvent_AnimationControllerTimeStamp(pApp,e,ptr);
		}
		break;
	case PX_LIVEEDITORMODULE_TIMESTAMPEDITOR_EVENT_EXIT:
		{
			PX_ApplicationOnScheduleEvent_TimeStampEditorExit(pApp,e,ptr);
		}
		break;

	case PX_LIVEFRAMEWORKMODULES_ANIMATIONCONTROLLER_EVENT_SMARTTRANSFORM:
		{
			PX_ApplicationOnScheduleEvent_SmartTransform(pApp,e,ptr);
		}
		break;
	case PX_LIVEFRAMEWORKMODULES_ANIMATIONCONTROLLER_EVENT_STRETCH:
		{
			PX_ApplicationOnScheduleEvent_Stretch(pApp,e,ptr);
		}
		break;
	case PX_LIVEFRAMEWORKMODULES_ANIMATIONCONTROLLER_EVENT_ROTATION:
		{
			PX_ApplicationOnScheduleEvent_Rotation(pApp,e,ptr);
		}
		break;
	case PX_LIVEFRAMEWORKMODULES_ANIMATIONCONTROLLER_EVENT_VERTEXTRANSFORM:
		{
			PX_ApplicationOnScheduleEvent_VertexTransfom(pApp,e,ptr);
		}
		break;
	case PX_LIVEFRAMEWORKMODULES_ANIMATIONCONTROLLER_EVENT_SWITCHTEXTURE:
		{
			PX_ApplicationOnScheduleEvent_FrameLiveTextureEditor(pApp,e,ptr);
		}
		break;
	case PX_LIVEFRAMEWORKMODULES_ANIMATIONCONTROLLER_EVENT_GLOBALROTATION:
		{
			PX_ApplicationOnScheduleEvent_GlobalRotation(pApp,e,ptr);
		}
		break;
	case PX_LIVEFRAMEWORKMODULES_ANIMATIONCONTROLLER_EVENT_IMPULSE:
		{
			PX_ApplicationOnScheduleEvent_Impulse(pApp,e,ptr);
		}
		break;
	case PX_LIVEFRAMEWORKMODULES_ANIMATIONCONTROLLER_EVENT_PANC:
		{
		PX_ApplicationOnScheduleEvent_Panc(pApp, e, ptr);
		}
		break;
	case PX_LIVEFRAMEWORKMODULES_PANC_EVENT_EXIT:
		{
		PX_ApplicationOnScheduleEvent_PancExit(pApp, e, ptr);
		}
		break;
	case PX_LIVEFRAMEWORKMODULES_MENU_EVENT_SHOWFPS:
		{
			PX_ApplicationOnScheduleEvent_ShowFPS(pApp,e,ptr);
		}
		break;
	case PX_LIVEFRAMEWORKMODULES_MENU_EVENT_RESETPOSITION:
		{
			PX_ApplicationOnScheduleEvent_ResetPosition(pApp,e,ptr);
		}
		break;
	case PX_LIVEFRAMEWORKMODULES_MENU_EVENT_SHOWHELPERLINE:
		{
			PX_ApplicationOnScheduleEvent_ShowHelperLine(pApp,e,ptr);
		}
		break;
	}

}

px_bool PX_ApplicationInitialize(PX_Application *pApp,px_int screen_width,px_int screen_height)
{
	
	PX_ApplicationInitializeDefault(&pApp->runtime, screen_width, screen_height);

	if(!PX_FontModuleInitialize(&pApp->runtime.mp_resources,&pApp->fontmodule))goto _ERROR;
	if(!PX_LoadFontModuleFromFile(&pApp->fontmodule,"assets/font18.pxf"))goto _ERROR;
	if(!PX_LiveFrameworkInitialize(&pApp->runtime.mp_game,&pApp->liveFramework,0,0)) goto _ERROR;
	
	if(!PX_JsonInitialize(&pApp->runtime.mp_resources,&pApp->languageJson))goto _ERROR;
	if(!PX_LoadJsonFromFile(&pApp->languageJson,"assets/language.json")) goto _ERROR;

	//Modules Initialize
	pApp->root=PX_ObjectCreate(&pApp->runtime.mp_ui,0,0,0,0,0,0,0);
	PX_ObjectRegisterEvent(pApp->root,PX_OBJECT_EVENT_ANY,PX_ApplicationOnScheduleEvent,pApp);

	//////////////////////////////////////////////////////////////////////////
	//static modules
	if ((pApp->module_display=PX_LiveEditorModule_DisplayInstall(pApp->root,&pApp->runtime,&pApp->fontmodule,&pApp->liveFramework,&pApp->languageJson))==PX_NULL)
		goto _ERROR;
	PX_LiveEditorModule_DisplayDisable(pApp->module_display);

	if((pApp->module_menu=PX_LiveEditorModule_MenuInstall(pApp->root,&pApp->runtime,&pApp->fontmodule,&pApp->liveFramework,&pApp->languageJson))==PX_NULL)
		goto _ERROR;
	PX_LiveEditorModule_MenuStandbyMode(pApp->module_menu);

	//////////////////////////////////////////////////////////////////////////
	//temporary
	if ((pApp->module_translation=PX_LiveEditorModule_TranslationInstall(pApp->root,&pApp->runtime,&pApp->fontmodule,&pApp->liveFramework,&pApp->languageJson))==PX_NULL)
		goto _ERROR;
	PX_LiveEditorModule_TranslationDisable(pApp->module_translation);

	if ((pApp->module_buildmesh=PX_LiveEditorModule_BuildMeshInstall(pApp->root,&pApp->runtime,&pApp->fontmodule,&pApp->liveFramework,&pApp->languageJson))==PX_NULL)
		goto _ERROR;
	PX_LiveEditorModule_BuildMeshDisable(pApp->module_buildmesh);

	if ((pApp->module_setkey=PX_LiveEditorModule_SetKeyInstall(pApp->root,&pApp->runtime,&pApp->fontmodule,&pApp->liveFramework,&pApp->languageJson))==PX_NULL)
		goto _ERROR;
	PX_LiveEditorModule_SetKeyDisable(pApp->module_setkey);

	if ((pApp->module_linkkey=PX_LiveEditorModule_LinkkeyInstall(pApp->root,&pApp->runtime,&pApp->fontmodule,&pApp->liveFramework,&pApp->languageJson))==PX_NULL)
		goto _ERROR;
	PX_LiveEditorModule_LinkkeyDisable(pApp->module_linkkey);


	//////////////////////////////////////////////////////////////////////////
	//auto adapt editor modules
	
	//static
	if((pApp->module_animationcontroller=PX_LiveEditorModule_AnimationControllerInstall(pApp->root,&pApp->runtime,&pApp->fontmodule,&pApp->liveFramework,&pApp->languageJson))==PX_NULL)
		goto _ERROR;
	PX_LiveEditorModule_AnimationControllerDisable(pApp->module_animationcontroller);

	if((pApp->module_livecontroller=PX_LiveEditorModule_LiveControllerInstall(pApp->root,&pApp->runtime,&pApp->fontmodule,&pApp->liveFramework,&pApp->languageJson))==PX_NULL)
		goto _ERROR;
	PX_LiveEditorModule_LiveControllerDisable(pApp->module_livecontroller);

	if((pApp->module_framecontroller=PX_LiveEditorModule_FrameControllerInstall(pApp->root,&pApp->runtime,&pApp->fontmodule,&pApp->liveFramework,&pApp->languageJson))==PX_NULL)
		goto _ERROR;
	PX_LiveEditorModule_FrameControllerDisable(pApp->module_framecontroller);


	//////////////////////////////////////////////////////////////////////////
	//on focus editor modules

	if((pApp->module_createproject=PX_LiveEditorModule_CreateProjectInstall(pApp->root,&pApp->runtime,&pApp->fontmodule,&pApp->liveFramework,&pApp->languageJson))==PX_NULL)
		goto _ERROR;
	PX_LiveEditorModule_CreateProjectDisable(pApp->module_createproject);

	if((pApp->module_importimage=PX_LiveEditorModule_ImportImageInstall(pApp->root,&pApp->runtime,&pApp->fontmodule,&pApp->liveFramework,&pApp->languageJson))==PX_NULL)
		goto _ERROR;
	PX_LiveEditorModule_ImportImageDisable(pApp->module_importimage);

	if((pApp->module_importlive=PX_LiveEditorModule_ImportLiveInstall(pApp->root,&pApp->runtime,&pApp->fontmodule,&pApp->liveFramework,&pApp->languageJson))==PX_NULL)
		goto _ERROR;
	PX_LiveEditorModule_ImportLiveDisable(pApp->module_importlive);

	if((pApp->module_exportlive=PX_LiveEditorModule_ExportLiveInstall(pApp->root,&pApp->runtime,&pApp->fontmodule,&pApp->liveFramework,&pApp->languageJson))==PX_NULL)
		goto _ERROR;
	PX_LiveEditorModule_ExportLiveDisable(pApp->module_exportlive);

	if((pApp->module_timestampeditor=PX_LiveEditorModule_TimeStampEditorInstall(pApp->root,&pApp->runtime,&pApp->fontmodule,&pApp->liveFramework,&pApp->languageJson))==PX_NULL)
		goto _ERROR;
	PX_LiveEditorModule_TimeStampEditorDisable(pApp->module_timestampeditor);

	if((pApp->module_newframeeditor=PX_LiveEditorModule_NewFrameEditorInstall(pApp->root,&pApp->runtime,&pApp->fontmodule,&pApp->liveFramework,&pApp->languageJson))==PX_NULL)
		goto _ERROR;
	PX_LiveEditorModule_NewFrameEditorDisable(pApp->module_newframeeditor);

	if((pApp->module_keytransform=PX_LiveEditorModule_KeyTransformInstall(pApp->root,&pApp->runtime,&pApp->fontmodule,&pApp->liveFramework,&pApp->languageJson))==PX_NULL)
		goto _ERROR;
	PX_LiveEditorModule_KeyTransformDisable(pApp->module_keytransform);

	if((pApp->module_vertextransform=PX_LiveEditorModule_VertexTransformInstall(pApp->root,&pApp->runtime,&pApp->fontmodule,&pApp->liveFramework,&pApp->languageJson))==PX_NULL)
		goto _ERROR;
	PX_LiveEditorModule_VertexTransformDisable(pApp->module_vertextransform);

	if((pApp->module_framelivetextureeditor=PX_LiveEditorModule_FrameLiveTextureEditorInstall(pApp->root,&pApp->runtime,&pApp->fontmodule,&pApp->liveFramework,&pApp->languageJson))==PX_NULL)
		goto _ERROR;
	PX_LiveEditorModule_FrameLiveTextureEditorDisable(pApp->module_framelivetextureeditor);

	if((pApp->module_globalrotation=PX_LiveEditorModule_GlobalRotationInstall(pApp->root,&pApp->runtime,&pApp->fontmodule,&pApp->liveFramework,&pApp->languageJson))==PX_NULL)
		goto _ERROR;
	PX_LiveEditorModule_GlobalRotationDisable(pApp->module_globalrotation);

	if((pApp->module_impulse=PX_LiveEditorModule_ImpulseEditorInstall(pApp->root,&pApp->runtime,&pApp->fontmodule,&pApp->liveFramework,&pApp->languageJson))==PX_NULL)
		goto _ERROR;
	PX_LiveEditorModule_ImpulseEditorDisable(pApp->module_impulse);

	if ((pApp->module_panc = PX_LiveEditorModule_PancInstall(pApp->root, &pApp->runtime, &pApp->fontmodule, &pApp->liveFramework, &pApp->languageJson)) == PX_NULL)
		goto _ERROR;
	PX_LiveEditorModule_PancDisable(pApp->module_panc);


	//////////////////////////////////////////////////////////////////////////
	//top modules
	if ((pApp->module_messagebox=PX_Object_MessageBoxCreate(&pApp->runtime.mp_ui,PX_NULL,&pApp->fontmodule))==PX_NULL)
		goto _ERROR;





	return PX_TRUE;
_ERROR:
	return PX_FALSE;
}

px_void PX_ApplicationUpdate(PX_Application *pApp,px_dword elpased)
{
	PX_ObjectUpdate(pApp->module_messagebox,elpased);
	PX_ObjectUpdate(pApp->root,elpased);
}

px_void PX_ApplicationRender(PX_Application *pApp,px_dword elpased)
{
	px_surface *pRenderSurface=&pApp->runtime.RenderSurface;
	PX_RuntimeRenderClear(&pApp->runtime,PX_OBJECT_UI_DEFAULT_BACKGROUNDCOLOR);
	if (pApp->module_messagebox->Visible)
	{
		PX_ObjectRender(pRenderSurface,pApp->module_messagebox,elpased);
	}
	else
	{
		PX_ObjectRender(pRenderSurface,pApp->root,elpased);
	}
	

}


px_void PX_ApplicationPostEvent(PX_Application *pApp,PX_Object_Event e)
{
	PX_ApplicationEventDefault(&pApp->runtime, e);
	//////////////////////////////////////////////////////////////////////////
	//
	if (pApp->module_messagebox->Visible)
	{
		PX_ObjectPostEvent(pApp->module_messagebox,e);
	}
	else
	{
		PX_ObjectPostEvent(pApp->root,e);
	}
	
}

