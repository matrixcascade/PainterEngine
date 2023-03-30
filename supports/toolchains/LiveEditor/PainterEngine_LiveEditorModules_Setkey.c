#include "PainterEngine_LiveEditorModules_SetKey.h"

px_void PX_LiveEditorModule_SetKeyOnButtonFinish(PX_Object *pObject,PX_Object_Event e,px_void *ptr)
{
	PX_Object *pSetkeyObject=(PX_Object *)ptr;
	PX_LiveEditorModule_SetKey *pSetkey=(PX_LiveEditorModule_SetKey *)pSetkeyObject->pObject;
	PX_LiveLayer *pLayer=PX_LiveFrameworkGetLayer(pSetkey->pLiveFramework,pSetkey->pLiveFramework->currentEditLayerIndex);

	pLayer->keyPoint.x=pSetkey->point.x;
	pLayer->keyPoint.y=pSetkey->point.y;

	PX_ObjectExecuteEvent(pSetkeyObject->pParent,PX_OBJECT_BUILD_EVENT(PX_LIVEFRAMEWORKMODULES_SETKEY_EVENT_EXIT));
}


px_void PX_LiveEditorModule_SetKeyRender(px_surface *pRenderSurface, PX_Object *pObject,px_dword elpased)
{
	PX_LiveEditorModule_SetKey *pBindBone=(PX_LiveEditorModule_SetKey *)pObject->pObject;
	PX_LiveLayer *pLayer=PX_LiveFrameworkGetLayer(pBindBone->pLiveFramework,pBindBone->pLiveFramework->currentEditLayerIndex);
	px_color ptColor;

	if (PX_isPoint2DInCircle(pBindBone->cursor,pBindBone->point,5))
	{
		ptColor=PX_COLOR(255,255,0,0);
	}
	else
	{
		ptColor=PX_COLOR(255,255,0,255);
	}



	PX_GeoDrawSolidCircle(pRenderSurface,(px_int)(pBindBone->point.x+pBindBone->pLiveFramework->refer_x),\
		(px_int)(pBindBone->point.y+pBindBone->pLiveFramework->refer_y),\
		3,
		ptColor
		);
	PX_GeoDrawCircle(pRenderSurface,(px_int)(pBindBone->point.x+pBindBone->pLiveFramework->refer_x),\
		(px_int)(pBindBone->point.y+pBindBone->pLiveFramework->refer_y),\
		5,
		1,
		ptColor
		);
	PX_FontModuleDrawText(pRenderSurface,pBindBone->fontmodule,(px_int)(pBindBone->point.x+pBindBone->pLiveFramework->refer_x),\
		(px_int)(pBindBone->point.y+pBindBone->pLiveFramework->refer_y-6),\
		PX_ALIGN_MIDBOTTOM,\
		PX_JsonGetString(pBindBone->pLanguageJson,"bindbone.keypoint"),
		PX_COLOR(255,255,255,255)
		);
}

px_void PX_LiveEditorModule_SetKeyPostEvent_CursorLDown(PX_LiveEditorModule_SetKey *pSetkey,px_float x,px_float y)
{
	pSetkey->bSelect=-1;

	if (PX_isPoint2DInCircle(PX_POINT2D(x,y),pSetkey->point,5))
	{
		pSetkey->bSelect=PX_TRUE;
		pSetkey->lastdragcursor=PX_POINT2D(x,y);
	}
	else
	{
		pSetkey->bSelect=PX_FALSE;
	}
}

px_void PX_LiveEditorModule_SetKeyPostEvent_CursorDrag(PX_LiveEditorModule_SetKey *pSetkey,px_float x,px_float y)
{
	if (pSetkey->bSelect)
	{
		pSetkey->point.x+=x-pSetkey->lastdragcursor.x;
		pSetkey->point.y+=y-pSetkey->lastdragcursor.y;
		pSetkey->lastdragcursor.x=x;
		pSetkey->lastdragcursor.y=y;
	}
}

px_void PX_LiveEditorModule_SetkeyHandleEvent(PX_Object *pObject,PX_Object_Event e,px_void *ptr)
{
	px_float x,y;
	PX_LiveEditorModule_SetKey *pSetkey;

	pSetkey=(PX_LiveEditorModule_SetKey *)pObject->pObject;

	switch(e.Event)
	{
	case PX_OBJECT_EVENT_CURSORMOVE:
		{
			x=PX_Object_Event_GetCursorX(e);
			y=PX_Object_Event_GetCursorY(e);
			x-=(pSetkey->pLiveFramework->refer_x);
			y-=(pSetkey->pLiveFramework->refer_y);
			pSetkey->cursor.x=x;
			pSetkey->cursor.y=y;
		}
		break;
	case PX_OBJECT_EVENT_CURSORDOWN:
		{
			x=PX_Object_Event_GetCursorX(e);
			y=PX_Object_Event_GetCursorY(e);
			x-=(pSetkey->pLiveFramework->refer_x);
			y-=(pSetkey->pLiveFramework->refer_y);
			PX_LiveEditorModule_SetKeyPostEvent_CursorLDown(pSetkey,x,y);
		}
		break;
	case PX_OBJECT_EVENT_CURSORDRAG:
		{
			x=PX_Object_Event_GetCursorX(e);
			y=PX_Object_Event_GetCursorY(e);
			x-=(pSetkey->pLiveFramework->refer_x);
			y-=(pSetkey->pLiveFramework->refer_y);
			pSetkey->cursor.x=x;
			pSetkey->cursor.y=y;
			PX_LiveEditorModule_SetKeyPostEvent_CursorDrag(pSetkey,x,y);
		}
		break;
	default:
		break;
	}
}

PX_Object * PX_LiveEditorModule_SetKeyInstall(PX_Object *pparent,PX_Runtime *pruntime,PX_FontModule *fm,PX_LiveFramework *pLiveFramework,PX_Json *pLanguageJson)
{
	PX_Object *pObject;
	PX_LiveEditorModule_SetKey BindBone,*pSetkey;
	PX_memset(&BindBone,0,sizeof(PX_LiveEditorModule_SetKey));
	pObject=PX_ObjectCreateEx(&pruntime->mp_ui,pparent,0,0,0,0,0,0,0,0,PX_LiveEditorModule_SetKeyRender,0,&BindBone,sizeof(BindBone));
	pSetkey=(PX_LiveEditorModule_SetKey *)pObject->pObject;

	pSetkey->pruntime=pruntime;
	pSetkey->fontmodule=fm;
	pSetkey->pLanguageJson=pLanguageJson;
	pSetkey->pLiveFramework=pLiveFramework;
	
	pSetkey->root=PX_Object_WidgetCreate(&pruntime->mp_ui,pObject,pruntime->surface_width-280,64,94,62,"",fm);
	pSetkey->button_finish=PX_Object_PushButtonCreate(&pruntime->mp_ui,pSetkey->root,3,3,84,32,PX_JsonGetString(pLanguageJson,"setkey.finish"),fm);
	PX_ObjectRegisterEvent(pSetkey->button_finish,PX_OBJECT_EVENT_EXECUTE,PX_LiveEditorModule_SetKeyOnButtonFinish,pObject);
	PX_ObjectRegisterEvent(pObject,PX_OBJECT_EVENT_ANY,PX_LiveEditorModule_SetkeyHandleEvent,pObject);

	return pObject;
}

px_void PX_LiveEditorModule_SetKeyUninstall(PX_Object *pObject)
{
	PX_ObjectDelete(pObject);
}

px_void PX_LiveEditorModule_SetKeyEnable(PX_Object *pObject)
{
	PX_LiveEditorModule_SetKey *pSetkey=(PX_LiveEditorModule_SetKey *)pObject->pObject;
	PX_LiveLayer *pLayer;

	pLayer=PX_LiveFrameworkGetLayer(pSetkey->pLiveFramework,pSetkey->pLiveFramework->currentEditLayerIndex);
	if (!pLayer)
	{
		return;
	}
	pSetkey->point.x=pLayer->keyPoint.x;
	pSetkey->point.y=pLayer->keyPoint.y;

	pObject->Visible=PX_TRUE;
	pObject->Enabled=PX_TRUE;

	PX_ObjectSetFocus(pObject);
	PX_LiveFrameworkStop(pSetkey->pLiveFramework);
}

px_void PX_LiveEditorModule_SetKeyDisable(PX_Object *pObject)
{
	pObject->Visible=PX_FALSE;
	pObject->Enabled=PX_FALSE;

	PX_ObjectClearFocus(pObject);
}

