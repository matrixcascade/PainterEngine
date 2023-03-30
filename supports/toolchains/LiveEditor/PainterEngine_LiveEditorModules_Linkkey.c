#include "PainterEngine_LiveEditorModules_Linkkey.h"
px_void PX_LiveEditorModule_LinkkeyRender(px_surface *psurface,PX_Object *pObject,px_dword elpased)
{
	px_int i;
	PX_LiveEditorModule_Linkkey *plinkkey=(PX_LiveEditorModule_Linkkey *)pObject->pObject;
	
	for (i=0;i<plinkkey->pLiveFramework->layers.size;i++)
	{
		PX_LiveLayer *pLayer=PX_VECTORAT(PX_LiveLayer,&plinkkey->pLiveFramework->layers,i);
		if (PX_isPoint2DInCircle(plinkkey->cursor,PX_POINT2D(pLayer->keyPoint.x,pLayer->keyPoint.y),5))
		{
			PX_GeoDrawCircle(psurface,(px_int)(pLayer->keyPoint.x+plinkkey->pLiveFramework->refer_x),(px_int)(pLayer->keyPoint.y+plinkkey->pLiveFramework->refer_y),6,1,PX_COLOR(255,255,0,255));
			PX_FontModuleDrawText(psurface,plinkkey->fontmodule,(px_int)(pLayer->keyPoint.x+plinkkey->pLiveFramework->refer_x),(px_int)(pLayer->keyPoint.y+plinkkey->pLiveFramework->refer_y)-6,PX_ALIGN_MIDBOTTOM,pLayer->id,PX_COLOR(255,0,0,255));
		}
	}

	if (plinkkey->startlinkIndex>=0&&plinkkey->startlinkIndex<plinkkey->pLiveFramework->layers.size)
	{
		PX_LiveLayer *pLayer=PX_VECTORAT(PX_LiveLayer,&plinkkey->pLiveFramework->layers,plinkkey->startlinkIndex);
		px_point2D startpt=PX_POINT2D((pLayer->keyPoint.x+plinkkey->pLiveFramework->refer_x),(pLayer->keyPoint.y+plinkkey->pLiveFramework->refer_y));
		px_point2D endpt;
		endpt=plinkkey->cursor;
		endpt.x+=plinkkey->pLiveFramework->refer_x;
		endpt.y+=plinkkey->pLiveFramework->refer_y;
		PX_GeoDrawArrow(psurface,startpt,endpt,1,PX_COLOR(255,255,0,0));
	}


}

px_void PX_LiveEditorModule_LinkkeyOnCursorMove(PX_Object *pObject,PX_Object_Event e,px_void *ptr)
{

	PX_LiveEditorModule_Linkkey *plinkkey=(PX_LiveEditorModule_Linkkey *)pObject->pObject;

	plinkkey->cursor=PX_POINT2D(PX_Object_Event_GetCursorX(e)-plinkkey->pLiveFramework->refer_x,PX_Object_Event_GetCursorY(e)-plinkkey->pLiveFramework->refer_y);
	
}


px_void PX_LiveEditorModule_LinkkeyOnCursorDown(PX_Object *pObject,PX_Object_Event e,px_void *ptr)
{
	PX_LiveEditorModule_Linkkey *plinkkey=(PX_LiveEditorModule_Linkkey *)pObject->pObject;
	px_float x=PX_Object_Event_GetCursorX(e)-plinkkey->pLiveFramework->refer_x;
	px_float y=PX_Object_Event_GetCursorY(e)-plinkkey->pLiveFramework->refer_y;
	px_int i;

	if (plinkkey->startlinkIndex>=0&&plinkkey->startlinkIndex<plinkkey->pLiveFramework->layers.size)
	{
		PX_LiveLayer *pResLayer=PX_VECTORAT(PX_LiveLayer,&plinkkey->pLiveFramework->layers,plinkkey->startlinkIndex);
		for (i=0;i<plinkkey->pLiveFramework->layers.size;i++)
		{
			PX_LiveLayer *pLayer=PX_VECTORAT(PX_LiveLayer,&plinkkey->pLiveFramework->layers,i);
			if (PX_isPoint2DInCircle(PX_POINT2D(x,y),PX_POINT2D(pLayer->keyPoint.x,pLayer->keyPoint.y),5))
			{
				if (i!=plinkkey->startlinkIndex)
				{
					if (PX_LiveFrameworkLinkLayerSearchSubLayer(plinkkey->pLiveFramework,pLayer,pResLayer))
					{
						PX_Object_MessageBoxAlertOk(plinkkey->messagebox,PX_JsonGetString(plinkkey->pLanguageJson,"linkkey.closedlink"),0,0);
					}
					else
					{
						PX_LiveFrameworkLinkLayer(plinkkey->pLiveFramework,pResLayer,pLayer);
						plinkkey->startlinkIndex=-1;
					}
					return;
				}
			}
		}
	}
	else
	{
		plinkkey->startlinkIndex=-1;

		for (i=0;i<plinkkey->pLiveFramework->layers.size;i++)
		{
			PX_LiveLayer *pLayer=PX_VECTORAT(PX_LiveLayer,&plinkkey->pLiveFramework->layers,i);
			if (PX_isPoint2DInCircle(PX_POINT2D(x,y),PX_POINT2D(pLayer->keyPoint.x,pLayer->keyPoint.y),5))
			{
				plinkkey->startlinkIndex=i;
				return;
			}
		}
	}
	plinkkey->startlinkIndex=-1;
}


px_void PX_LiveEditorModule_LinkkeyOnButtonReset(PX_Object *pObject,PX_Object_Event e,px_void *ptr)
{
	PX_Object *pLinkObject=(PX_Object *)ptr;
	PX_LiveEditorModule_Linkkey *plinkkey=(PX_LiveEditorModule_Linkkey *)pLinkObject->pObject;
	PX_LiveFrameworkClearLinker(plinkkey->pLiveFramework);
}

px_void PX_LiveEditorModule_LinkkeyOnButtonFinish(PX_Object *pObject,PX_Object_Event e,px_void *ptr)
{
	PX_Object *pLinkkeyObject=(PX_Object *)ptr;
	PX_LiveEditorModule_Linkkey *pLinkkey=(PX_LiveEditorModule_Linkkey *)pLinkkeyObject->pObject;
	pLinkkey->pLiveFramework->showlinker=PX_FALSE;
	PX_ObjectExecuteEvent(pLinkkeyObject->pParent,PX_OBJECT_BUILD_EVENT(PX_LIVEFRAMEWORKMODULES_LINKKEY_EVENT_EXIT));
}


PX_Object * PX_LiveEditorModule_LinkkeyInstall(PX_Object *pparent,PX_Runtime *pruntime,PX_FontModule *fm,PX_LiveFramework *pLiveFramework,PX_Json *pLanguageJson)
{
	PX_Object *pObject;
	PX_LiveEditorModule_Linkkey linkkey,*plinkkey;
	PX_memset(&linkkey,0,sizeof(PX_LiveEditorModule_Linkkey));
	pObject=PX_ObjectCreateEx(&pruntime->mp_ui,pparent,0,0,0,0,0,0,0,0,PX_LiveEditorModule_LinkkeyRender,0,&linkkey,sizeof(linkkey));
	plinkkey=(PX_LiveEditorModule_Linkkey *)pObject->pObject;

	plinkkey->pruntime=pruntime;
	plinkkey->fontmodule=fm;
	plinkkey->pLanguageJson=pLanguageJson;
	plinkkey->pLiveFramework=pLiveFramework;
	plinkkey->messagebox= PX_Object_MessageBoxCreate(&pruntime->mp_ui,pObject,fm);
	plinkkey->root=PX_Object_WidgetCreate(&pruntime->mp_ui,pObject,pruntime->surface_width-280,64,176,62,"",fm);
	PX_Object_WidgetShowHideCloseButton(plinkkey->root,PX_FALSE);
	plinkkey->button_reset=PX_Object_PushButtonCreate(&pruntime->mp_ui,plinkkey->root,3+85*0,3,84,32,PX_JsonGetString(pLanguageJson,"linkkey.reset"),fm);
	plinkkey->button_finish=PX_Object_PushButtonCreate(&pruntime->mp_ui,plinkkey->root,3+85*1,3,84,32,PX_JsonGetString(pLanguageJson,"linkkey.finish"),fm);

	PX_ObjectRegisterEvent(plinkkey->button_reset,PX_OBJECT_EVENT_EXECUTE,PX_LiveEditorModule_LinkkeyOnButtonReset,pObject);
	PX_ObjectRegisterEvent(plinkkey->button_finish,PX_OBJECT_EVENT_EXECUTE,PX_LiveEditorModule_LinkkeyOnButtonFinish,pObject);
	PX_ObjectRegisterEvent(pObject,PX_OBJECT_EVENT_CURSORMOVE,PX_LiveEditorModule_LinkkeyOnCursorMove,pObject);
	PX_ObjectRegisterEvent(pObject,PX_OBJECT_EVENT_CURSORDOWN,PX_LiveEditorModule_LinkkeyOnCursorDown,pObject);
	return pObject;
}


px_void PX_LiveEditorModule_LinkkeyEnable(PX_Object *pObject)
{
	PX_LiveEditorModule_Linkkey *pLinkKey=(PX_LiveEditorModule_Linkkey *)pObject->pObject;
	pLinkKey->pLiveFramework->showlinker=PX_TRUE;
	pLinkKey->startlinkIndex=-1;
	PX_LiveFrameworkReset(pLinkKey->pLiveFramework);
	PX_ObjectSetFocus(pObject);
	pObject->Enabled=PX_TRUE;
	pObject->Visible=PX_TRUE;
}

px_void PX_LiveEditorModule_LinkkeyDisable(PX_Object *pObject)
{
	PX_LiveEditorModule_Linkkey *pLinkKey=(PX_LiveEditorModule_Linkkey *)pObject->pObject;
	pLinkKey->pLiveFramework->showlinker=PX_FALSE;
	pObject->Enabled=PX_FALSE;
	pObject->Visible=PX_FALSE;
}