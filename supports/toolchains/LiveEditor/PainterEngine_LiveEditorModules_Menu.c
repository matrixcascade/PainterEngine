#include "PainterEngine_LiveEditorModules_Menu.h"
#include "PainterEngine_LiveEditorModules_CreateProject.h"


px_void PX_LiveEditorModule_Menu_OnMenuCreateProject(px_void *userPtr)
{
	PX_Object *pMenuObject=(PX_Object *)userPtr;
	PX_ObjectExecuteEvent(pMenuObject->pParent,PX_OBJECT_BUILD_EVENT(PX_LIVEFRAMEWORKMODULES_MENU_EVENT_CREATEPROJECT));

}

px_void PX_LiveEditorModule_Menu_OnMenuImportLive(px_void *userPtr)
{
	PX_Object *pMenuObject=(PX_Object *)userPtr;
	PX_ObjectExecuteEvent(pMenuObject->pParent,PX_OBJECT_BUILD_EVENT(PX_LIVEFRAMEWORKMODULES_MENU_EVENT_IMPORT));

}

px_void PX_LiveEditorModule_Menu_OnMenuExportLive(px_void *userPtr)
{
	PX_Object *pMenuObject=(PX_Object *)userPtr;
	PX_ObjectExecuteEvent(pMenuObject->pParent,PX_OBJECT_BUILD_EVENT(PX_LIVEFRAMEWORKMODULES_MENU_EVENT_EXPORT));

}

px_void PX_LiveEditorModule_Menu_OnMenuImportImageFiles(px_void *userPtr)
{
	PX_Object *pMenuObject=(PX_Object *)userPtr;
	PX_ObjectExecuteEvent(pMenuObject->pParent,PX_OBJECT_BUILD_EVENT(PX_LIVEFRAMEWORKMODULES_MENU_EVENT_LOADIMAGE));

}
px_void PX_LiveEditorModule_Menu_OnMenuLayer(px_void *userPtr)
{
	PX_Object *pMenuObject=(PX_Object *)userPtr;
	PX_ObjectExecuteEvent(pMenuObject->pParent,PX_OBJECT_BUILD_EVENT(PX_LIVEFRAMEWORKMODULES_MENU_EVENT_WINDOW_LAYER));
}

px_void PX_LiveEditorModule_Menu_OnShowKeyPoints(px_void *userPtr)
{
	PX_Object *pMenuObject=(PX_Object *)userPtr;
	PX_LiveEditorModule_Menu *pDesc=(PX_LiveEditorModule_Menu *)pMenuObject->pObject;
	pDesc->pLiveFramework->showKeypoint=!pDesc->pLiveFramework->showKeypoint;
}

px_void PX_LiveEditorModule_Menu_OnShowLinker(px_void *userPtr)
{
	PX_Object *pMenuObject=(PX_Object *)userPtr;
	PX_LiveEditorModule_Menu *pDesc=(PX_LiveEditorModule_Menu *)pMenuObject->pObject;
	pDesc->pLiveFramework->showlinker=!pDesc->pLiveFramework->showlinker;
}

px_void PX_LiveEditorModule_Menu_OnShowMesh(px_void *userPtr)
{
	px_int i;
	PX_Object *pMenuObject=(PX_Object *)userPtr;
	PX_LiveEditorModule_Menu *pDesc=(PX_LiveEditorModule_Menu *)pMenuObject->pObject;
	pDesc->showMesh=!pDesc->showMesh;
	for (i=0;i<pDesc->pLiveFramework->layers.size;i++)
	{
		PX_LiveLayer *pLayer=PX_VECTORAT(PX_LiveLayer,&pDesc->pLiveFramework->layers,i);
		pLayer->showMesh=pDesc->showMesh;
	}
}

px_void PX_LiveEditorModule_Menu_OnResetFrame(px_void *userPtr)
{
	px_int i;
	PX_Object *pMenuObject=(PX_Object *)userPtr;
	PX_LiveEditorModule_Menu *pDesc=(PX_LiveEditorModule_Menu *)pMenuObject->pObject;

	for (i=0;i<pDesc->pLiveFramework->layers.size;i++)
	{
		PX_LiveAnimationFramePayload *pPayload=PX_LiveFrameworkGetCurrentEditAnimationFramePayloadIndex(pDesc->pLiveFramework,i);
		PX_LiveLayer *pLivelayer=PX_LiveFrameworkGetLayer(pDesc->pLiveFramework,i);
		if (pPayload)
		{
			px_byte *p=(px_byte *)pPayload;
			pPayload->impulse=PX_POINT(0,0,0);
			pPayload->mapTexture=pLivelayer->LinkTextureIndex;
			pPayload->rotation=0;
			pPayload->stretch=1;
			pPayload->panc_endx = 0;
			pPayload->panc_endy = 0;
			pPayload->panc_height = 0;
			pPayload->panc_width = 0;
			pPayload->panc_sx = 0;
			pPayload->panc_sy = 0;
			pPayload->panc_x = 0;
			pPayload->panc_y = 0;
			pPayload->translation=PX_POINT(0,0,0);
			PX_memset(p+sizeof(PX_LiveAnimationFramePayload),0,sizeof(px_point)*pPayload->translationVerticesCount);
			
		}
	}
	PX_LiveFrameworkRunCurrentEditFrame(pDesc->pLiveFramework);
}


px_void PX_LiveEditorModule_Menu_OnResetTranslation(px_void *userPtr)
{
	px_int i;
	PX_Object *pMenuObject=(PX_Object *)userPtr;
	PX_LiveEditorModule_Menu *pDesc=(PX_LiveEditorModule_Menu *)pMenuObject->pObject;
	PX_LiveAnimationFramePayload *pPayload=PX_LiveFrameworkGetCurrentEditAnimationFramePayload(pDesc->pLiveFramework);
	PX_LiveLayer *pLivelayer=PX_LiveFrameworkGetCurrentEditLiveLayer(pDesc->pLiveFramework);
	for (i=0;i<pDesc->pLiveFramework->layers.size;i++)
	{
		PX_LiveAnimationFramePayload *pPayload=PX_LiveFrameworkGetCurrentEditAnimationFramePayloadIndex(pDesc->pLiveFramework,i);
		PX_LiveLayer *pLivelayer=PX_LiveFrameworkGetLayer(pDesc->pLiveFramework,i);
		if (pPayload)
		{
			pPayload->translation=PX_POINT(0,0,0);
		}
	}
	PX_LiveFrameworkRunCurrentEditFrame(pDesc->pLiveFramework);
}

px_void PX_LiveEditorModule_Menu_OnResetVertices(px_void *userPtr)
{
	px_int i;
	PX_Object *pMenuObject=(PX_Object *)userPtr;
	PX_LiveEditorModule_Menu *pDesc=(PX_LiveEditorModule_Menu *)pMenuObject->pObject;
	PX_LiveAnimationFramePayload *pPayload=PX_LiveFrameworkGetCurrentEditAnimationFramePayload(pDesc->pLiveFramework);
	PX_LiveLayer *pLivelayer=PX_LiveFrameworkGetCurrentEditLiveLayer(pDesc->pLiveFramework);
	for (i=0;i<pDesc->pLiveFramework->layers.size;i++)
	{
		PX_LiveAnimationFramePayload *pPayload=PX_LiveFrameworkGetCurrentEditAnimationFramePayloadIndex(pDesc->pLiveFramework,i);
		PX_LiveLayer *pLivelayer=PX_LiveFrameworkGetLayer(pDesc->pLiveFramework,i);
		if (pPayload)
		{
			px_byte *p=(px_byte *)pPayload;
			PX_memset(p+sizeof(PX_LiveAnimationFramePayload),0,sizeof(px_point)*pPayload->translationVerticesCount);
		}
	}
	PX_LiveFrameworkRunCurrentEditFrame(pDesc->pLiveFramework);
}

px_void PX_LiveEditorModule_Menu_OnResetRotation(px_void *userPtr)
{
	px_int i;
	PX_Object *pMenuObject=(PX_Object *)userPtr;
	PX_LiveEditorModule_Menu *pDesc=(PX_LiveEditorModule_Menu *)pMenuObject->pObject;
	PX_LiveAnimationFramePayload *pPayload=PX_LiveFrameworkGetCurrentEditAnimationFramePayload(pDesc->pLiveFramework);
	PX_LiveLayer *pLivelayer=PX_LiveFrameworkGetCurrentEditLiveLayer(pDesc->pLiveFramework);
	for (i=0;i<pDesc->pLiveFramework->layers.size;i++)
	{
		PX_LiveAnimationFramePayload *pPayload=PX_LiveFrameworkGetCurrentEditAnimationFramePayloadIndex(pDesc->pLiveFramework,i);
		PX_LiveLayer *pLivelayer=PX_LiveFrameworkGetLayer(pDesc->pLiveFramework,i);
		if (pPayload)
		{
			pPayload->rotation=0;
		}
	}
	PX_LiveFrameworkRunCurrentEditFrame(pDesc->pLiveFramework);
}

px_void PX_LiveEditorModule_Menu_OnResetStretch(px_void *userPtr)
{
	px_int i;
	PX_Object *pMenuObject=(PX_Object *)userPtr;
	PX_LiveEditorModule_Menu *pDesc=(PX_LiveEditorModule_Menu *)pMenuObject->pObject;
	PX_LiveAnimationFramePayload *pPayload=PX_LiveFrameworkGetCurrentEditAnimationFramePayload(pDesc->pLiveFramework);
	PX_LiveLayer *pLivelayer=PX_LiveFrameworkGetCurrentEditLiveLayer(pDesc->pLiveFramework);
	for (i=0;i<pDesc->pLiveFramework->layers.size;i++)
	{
		PX_LiveAnimationFramePayload *pPayload=PX_LiveFrameworkGetCurrentEditAnimationFramePayloadIndex(pDesc->pLiveFramework,i);
		PX_LiveLayer *pLivelayer=PX_LiveFrameworkGetLayer(pDesc->pLiveFramework,i);
		if (pPayload)
		{
			pPayload->stretch=1;
		}
	}
	PX_LiveFrameworkRunCurrentEditFrame(pDesc->pLiveFramework);
}

px_void PX_LiveEditorModule_Menu_OnResetImpulse(px_void *userPtr)
{
	px_int i;
	PX_Object *pMenuObject=(PX_Object *)userPtr;
	PX_LiveEditorModule_Menu *pDesc=(PX_LiveEditorModule_Menu *)pMenuObject->pObject;
	PX_LiveAnimationFramePayload *pPayload=PX_LiveFrameworkGetCurrentEditAnimationFramePayload(pDesc->pLiveFramework);
	PX_LiveLayer *pLivelayer=PX_LiveFrameworkGetCurrentEditLiveLayer(pDesc->pLiveFramework);
	for (i=0;i<pDesc->pLiveFramework->layers.size;i++)
	{
		PX_LiveAnimationFramePayload *pPayload=PX_LiveFrameworkGetCurrentEditAnimationFramePayloadIndex(pDesc->pLiveFramework,i);
		PX_LiveLayer *pLivelayer=PX_LiveFrameworkGetLayer(pDesc->pLiveFramework,i);
		if (pPayload)
		{
			pPayload->impulse=PX_POINT(0,0,0);
		}
	}
	PX_LiveFrameworkRunCurrentEditFrame(pDesc->pLiveFramework);
}

px_void PX_LiveEditorModule_Menu_OnResetTexture(px_void *userPtr)
{
	px_int i;
	PX_Object *pMenuObject=(PX_Object *)userPtr;
	PX_LiveEditorModule_Menu *pDesc=(PX_LiveEditorModule_Menu *)pMenuObject->pObject;
	PX_LiveAnimationFramePayload *pPayload=PX_LiveFrameworkGetCurrentEditAnimationFramePayload(pDesc->pLiveFramework);
	PX_LiveLayer *pLivelayer=PX_LiveFrameworkGetCurrentEditLiveLayer(pDesc->pLiveFramework);
	for (i=0;i<pDesc->pLiveFramework->layers.size;i++)
	{
		PX_LiveAnimationFramePayload *pPayload=PX_LiveFrameworkGetCurrentEditAnimationFramePayloadIndex(pDesc->pLiveFramework,i);
		PX_LiveLayer *pLivelayer=PX_LiveFrameworkGetLayer(pDesc->pLiveFramework,i);
		if (pPayload)
		{
			pPayload->mapTexture=pLivelayer->LinkTextureIndex;
		}
	}
	PX_LiveFrameworkRunCurrentEditFrame(pDesc->pLiveFramework);
}

px_void PX_LiveEditorModule_Menu_OnShowFPS(px_void *userPtr)
{
	PX_Object *pMenuObject=(PX_Object *)userPtr;
	PX_LiveEditorModule_Menu *pDesc=(PX_LiveEditorModule_Menu *)pMenuObject->pObject;
	PX_ObjectExecuteEvent(pMenuObject->pParent,PX_OBJECT_BUILD_EVENT(PX_LIVEFRAMEWORKMODULES_MENU_EVENT_SHOWFPS));
}

px_void PX_LiveEditorModule_Menu_OnResetPosition(px_void *userPtr)
{
	PX_Object *pMenuObject=(PX_Object *)userPtr;
	PX_LiveEditorModule_Menu *pDesc=(PX_LiveEditorModule_Menu *)pMenuObject->pObject;
	PX_ObjectExecuteEvent(pMenuObject->pParent,PX_OBJECT_BUILD_EVENT(PX_LIVEFRAMEWORKMODULES_MENU_EVENT_RESETPOSITION));
}

px_void PX_LiveEditorModule_Menu_OnShowHelperLine(px_void *userPtr)
{
	PX_Object *pMenuObject=(PX_Object *)userPtr;
	PX_LiveEditorModule_Menu *pDesc=(PX_LiveEditorModule_Menu *)pMenuObject->pObject;
	PX_ObjectExecuteEvent(pMenuObject->pParent,PX_OBJECT_BUILD_EVENT(PX_LIVEFRAMEWORKMODULES_MENU_EVENT_SHOWHELPERLINE));
}


px_void PX_LiveEditorModule_Menu_OnAboutPainterEngine(px_void *userPtr)
{
	PX_Object *pMenuObject=(PX_Object *)userPtr;
	PX_LiveEditorModule_Menu *pDesc=(PX_LiveEditorModule_Menu *)pMenuObject->pObject;
	PX_ObjectExecuteEvent(pMenuObject->pParent,PX_OBJECT_BUILD_EVENT_STRING(PX_LIVEFRAMEWORKMODULES_MENU_EVENT_MESSAGE,PX_JsonGetString(pDesc->pLanguageJson,"menu.about painterengine content")));
}

px_void PX_LiveEditorModule_Menu_OnAboutApp(px_void *userPtr)
{
	PX_Object *pMenuObject=(PX_Object *)userPtr;
	PX_LiveEditorModule_Menu *pDesc=(PX_LiveEditorModule_Menu *)pMenuObject->pObject;
	PX_ObjectExecuteEvent(pMenuObject->pParent,PX_OBJECT_BUILD_EVENT_STRING(PX_LIVEFRAMEWORKMODULES_MENU_EVENT_MESSAGE,PX_JsonGetString(pDesc->pLanguageJson,"menu.about app content")));
}



PX_Object * PX_LiveEditorModule_MenuInstall(PX_Object *pparent,PX_Runtime *pruntime,PX_FontModule *fm,PX_LiveFramework *pLiveFramework,PX_Json *pLanguageJson)
{
	PX_LiveEditorModule_Menu Menu,*pMenu;
	PX_Object *pObject;

	pObject=PX_ObjectCreateEx(&pruntime->mp_ui,pparent,0,0,0,0,0,0,0,0,0,0,&Menu,sizeof(Menu));
	pMenu=(PX_LiveEditorModule_Menu *)pObject->pObject;
	PX_memset(pMenu,0,sizeof(PX_LiveEditorModule_Menu));

	pMenu->pruntime=pruntime;
	pMenu->fontmodule=fm;
	pMenu->pLanguageJson=pLanguageJson;
	pMenu->pLiveFramework=pLiveFramework;
	pMenu->Menu=PX_Object_MenuCreate(&pruntime->mp_ui,pObject,0,0,128,fm);
	if (pMenu->Menu==PX_NULL)
	{
		goto _ERROR;
	}

	//CreateMenu Items
	do 
	{
		PX_Object_Menu_Item *pItem;
		//File
		pItem=PX_Object_MenuAddItem(pMenu->Menu,PX_Object_MenuGetRootItem(pMenu->Menu),PX_JsonGetString(pLanguageJson,"menu.file"),PX_NULL,PX_NULL);
		//File->new project
		pMenu->file_new_project=PX_Object_MenuAddItem(pMenu->Menu,pItem,PX_JsonGetString(pLanguageJson,"menu.new project"),PX_LiveEditorModule_Menu_OnMenuCreateProject,pObject);
		//File->import live
		pMenu->file_import_live=PX_Object_MenuAddItem(pMenu->Menu,pItem,PX_JsonGetString(pLanguageJson,"menu.import live"),PX_LiveEditorModule_Menu_OnMenuImportLive,pObject);
		
		//File->Import image
		pMenu->file_import_image_file=PX_Object_MenuAddItem(pMenu->Menu,pItem,PX_JsonGetString(pLanguageJson,"menu.import image file"),PX_LiveEditorModule_Menu_OnMenuImportImageFiles,pObject);

		//File->export live
		pMenu->file_export_live=PX_Object_MenuAddItem(pMenu->Menu,pItem,PX_JsonGetString(pLanguageJson,"menu.export live"),PX_LiveEditorModule_Menu_OnMenuExportLive,pObject);

		//View
		pItem=PX_Object_MenuAddItem(pMenu->Menu,PX_Object_MenuGetRootItem(pMenu->Menu),PX_JsonGetString(pLanguageJson,"menu.view"),PX_NULL,PX_NULL);
		//View->Controller
		pMenu->view_controller=PX_Object_MenuAddItem(pMenu->Menu,pItem,PX_JsonGetString(pLanguageJson,"menu.controller"),PX_LiveEditorModule_Menu_OnMenuLayer,pObject);
		//View->ShowFPS
		pMenu->view_showfps=PX_Object_MenuAddItem(pMenu->Menu,pItem,PX_JsonGetString(pLanguageJson,"menu.showfps"),PX_LiveEditorModule_Menu_OnShowFPS,pObject);
		//View->ShowFPS
		pMenu->view_showhelpline=PX_Object_MenuAddItem(pMenu->Menu,pItem,PX_JsonGetString(pLanguageJson,"menu.showhelperline"),PX_LiveEditorModule_Menu_OnShowHelperLine,pObject);
		//View->Show Position
		pMenu->view_resetposition=PX_Object_MenuAddItem(pMenu->Menu,pItem,PX_JsonGetString(pLanguageJson,"menu.reset position"),PX_LiveEditorModule_Menu_OnResetPosition,pObject);

		//Render
		pItem=PX_Object_MenuAddItem(pMenu->Menu,PX_Object_MenuGetRootItem(pMenu->Menu),PX_JsonGetString(pLanguageJson,"menu.render"),PX_NULL,PX_NULL);
		//Render->show keypoints
		pMenu->render_showkeypoints=PX_Object_MenuAddItem(pMenu->Menu,pItem,PX_JsonGetString(pLanguageJson,"menu.show keypoints"),PX_LiveEditorModule_Menu_OnShowKeyPoints,pObject);
		//Render->show linker
		pMenu->render_showlinker=PX_Object_MenuAddItem(pMenu->Menu,pItem,PX_JsonGetString(pLanguageJson,"menu.show linker"),PX_LiveEditorModule_Menu_OnShowLinker,pObject);
		//Render->show keypoints
		pMenu->render_showmesh=PX_Object_MenuAddItem(pMenu->Menu,pItem,PX_JsonGetString(pLanguageJson,"menu.show mesh"),PX_LiveEditorModule_Menu_OnShowMesh,pObject);

		//Animation
		pItem=PX_Object_MenuAddItem(pMenu->Menu,PX_Object_MenuGetRootItem(pMenu->Menu),PX_JsonGetString(pLanguageJson,"menu.animation"),PX_NULL,PX_NULL);
		//Animation->reset frame
		pMenu->animation_resetframe=PX_Object_MenuAddItem(pMenu->Menu,pItem,PX_JsonGetString(pLanguageJson,"menu.reset frame"),PX_LiveEditorModule_Menu_OnResetFrame,pObject);
		//Animation->reset Translation
		pMenu->animation_resettranslation=PX_Object_MenuAddItem(pMenu->Menu,pItem,PX_JsonGetString(pLanguageJson,"menu.reset translation"),PX_LiveEditorModule_Menu_OnResetTranslation,pObject);
		//Animation->reset Translation
		pMenu->animation_resetvertices=PX_Object_MenuAddItem(pMenu->Menu,pItem,PX_JsonGetString(pLanguageJson,"menu.reset vertices"),PX_LiveEditorModule_Menu_OnResetVertices,pObject);
		//Animation->reset Rotation
		pMenu->animation_resetrotation=PX_Object_MenuAddItem(pMenu->Menu,pItem,PX_JsonGetString(pLanguageJson,"menu.reset rotation"),PX_LiveEditorModule_Menu_OnResetRotation,pObject);
		//Animation->reset Stretch
		pMenu->animation_resetstretch=PX_Object_MenuAddItem(pMenu->Menu,pItem,PX_JsonGetString(pLanguageJson,"menu.reset stretch"),PX_LiveEditorModule_Menu_OnResetStretch,pObject);
		//Animation->reset Impulse
		pMenu->animation_resetimpulse=PX_Object_MenuAddItem(pMenu->Menu,pItem,PX_JsonGetString(pLanguageJson,"menu.reset impulse"),PX_LiveEditorModule_Menu_OnResetImpulse,pObject);
		//Animation->reset texture
		pMenu->animation_resettexture=PX_Object_MenuAddItem(pMenu->Menu,pItem,PX_JsonGetString(pLanguageJson,"menu.reset texture"),PX_LiveEditorModule_Menu_OnResetTexture,pObject);

		//About
		pItem=PX_Object_MenuAddItem(pMenu->Menu,PX_Object_MenuGetRootItem(pMenu->Menu),PX_JsonGetString(pLanguageJson,"menu.about"),PX_NULL,PX_NULL);
		//Animation->reset frame
		pMenu->about_painterengine=PX_Object_MenuAddItem(pMenu->Menu,pItem,PX_JsonGetString(pLanguageJson,"menu.about painterengine"),PX_LiveEditorModule_Menu_OnAboutPainterEngine,pObject);
		//Animation->reset Translation
		pMenu->about_app=PX_Object_MenuAddItem(pMenu->Menu,pItem,PX_JsonGetString(pLanguageJson,"menu.about app"),PX_LiveEditorModule_Menu_OnAboutApp,pObject);
	} while (0);
	return pObject;
_ERROR:
	return PX_NULL;
}

px_void PX_LiveEditorModule_MenuStandbyMode(PX_Object *pObject)
{
	PX_LiveEditorModule_Menu *pMenu=(PX_LiveEditorModule_Menu *)pObject->pObject;

	pMenu->file_import_image_file->enable=PX_FALSE;
	pMenu->view_controller->enable=PX_FALSE;
	pMenu->file_export_live->enable=PX_FALSE;

	pMenu->view_showfps->enable=PX_FALSE;
	pMenu->view_showhelpline->enable=PX_FALSE;
	pMenu->view_resetposition->enable=PX_FALSE;

	pMenu->render_showkeypoints->enable=PX_FALSE;
	pMenu->render_showlinker->enable=PX_FALSE;
	pMenu->render_showmesh->enable=PX_FALSE;

	pMenu->animation_resetframe->enable=PX_FALSE;
	pMenu->animation_resettranslation->enable=PX_FALSE;
	pMenu->animation_resetvertices->enable=PX_FALSE;
	pMenu->animation_resetrotation->enable=PX_FALSE;
	pMenu->animation_resetstretch->enable=PX_FALSE;
	pMenu->animation_resetimpulse->enable=PX_FALSE;
	pMenu->animation_resettexture->enable=PX_FALSE;

	
}

px_void PX_LiveEditorModule_MenuEditMode(PX_Object *pObject)
{
	PX_LiveEditorModule_Menu *pMenu=(PX_LiveEditorModule_Menu *)pObject->pObject;
	pMenu->file_import_image_file->enable=PX_TRUE;
	pMenu->view_controller->enable=PX_TRUE;
	pMenu->file_export_live->enable=PX_TRUE;

	pMenu->view_showfps->enable=PX_TRUE;
	pMenu->view_showhelpline->enable=PX_TRUE;
	pMenu->view_resetposition->enable=PX_TRUE;

	pMenu->render_showkeypoints->enable=PX_TRUE;
	pMenu->render_showlinker->enable=PX_TRUE;
	pMenu->render_showmesh->enable=PX_TRUE;

	pMenu->animation_resetframe->enable=PX_TRUE;
	pMenu->animation_resettranslation->enable=PX_TRUE;
	pMenu->animation_resetvertices->enable=PX_TRUE;
	pMenu->animation_resetrotation->enable=PX_TRUE;
	pMenu->animation_resetstretch->enable=PX_TRUE;
	pMenu->animation_resetimpulse->enable=PX_TRUE;
	pMenu->animation_resettexture->enable=PX_TRUE;
}

