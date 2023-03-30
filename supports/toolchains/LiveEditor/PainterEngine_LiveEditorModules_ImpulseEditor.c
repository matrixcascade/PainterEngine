#include "PainterEngine_LiveEditorModules_ImpulseEditor.h"

static px_void PX_LiveEditorModule_ImpulseEditorEnterEditMode(PX_Object *pObject,px_int layerIndex)
{
	PX_LiveEditorModule_ImpulseEditor *pDesc=(PX_LiveEditorModule_ImpulseEditor *)pObject->pObject;
	pDesc->pLiveFramework->showKeypoint=PX_FALSE;
	pDesc->pLiveFramework->showFocusLayer=PX_TRUE;
	pDesc->pLiveFramework->currentEditLayerIndex=layerIndex;
	PX_ObjectSetFocus(pObject);
}

static px_void PX_LiveEditorModule_ImpulseEditorEnterSelectMode(PX_Object *pObject)
{
	PX_LiveEditorModule_ImpulseEditor *pDesc=(PX_LiveEditorModule_ImpulseEditor *)pObject->pObject;
	pDesc->pLiveFramework->showKeypoint=PX_TRUE;
	pDesc->pLiveFramework->showFocusLayer=PX_FALSE;
	pDesc->pLiveFramework->currentEditLayerIndex=-1;
	PX_ObjectReleaseFocus(pObject);
}

px_void PX_LiveEditorModule_ImpulseEditorOnCursorRDown(PX_Object *pObject,PX_Object_Event e,px_void *ptr)
{
	PX_LiveEditorModule_ImpulseEditor *pDesc=(PX_LiveEditorModule_ImpulseEditor *)pObject->pObject;
	PX_LiveEditorModule_ImpulseEditorEnterSelectMode(pObject);
}

px_void PX_LiveEditorModule_ImpulseEditorOnCursorMove(PX_Object *pObject,PX_Object_Event e,px_void *ptr)
{
	PX_LiveEditorModule_ImpulseEditor *pDesc=(PX_LiveEditorModule_ImpulseEditor *)pObject->pObject;
	px_float x,y;

	if (pDesc->pLiveFramework->currentEditLayerIndex>=0&&pDesc->pLiveFramework->currentEditLayerIndex<pDesc->pLiveFramework->layers.size)
	{
		x=PX_Object_Event_GetCursorX(e);
		y=PX_Object_Event_GetCursorY(e);

		x-=pDesc->pLiveFramework->refer_x;
		y-=pDesc->pLiveFramework->refer_y;

		pDesc->Cursor=PX_POINT2D(x,y);
	}
}

px_void PX_LiveEditorModule_ImpulseEditorOnCursorDown(PX_Object *pObject,PX_Object_Event e,px_void *ptr)
{
	PX_LiveEditorModule_ImpulseEditor *pDesc=(PX_LiveEditorModule_ImpulseEditor *)pObject->pObject;
	px_float x,y;
	px_int i;

	x=PX_Object_Event_GetCursorX(e);
	y=PX_Object_Event_GetCursorY(e);

	x-=pDesc->pLiveFramework->refer_x;
	y-=pDesc->pLiveFramework->refer_y;

	if (pDesc->pLiveFramework->currentEditLayerIndex>=0&&pDesc->pLiveFramework->currentEditLayerIndex<pDesc->pLiveFramework->layers.size)
	{
		//edit mode
		PX_LiveAnimationFramePayload *pPayload=PX_LiveFrameworkGetCurrentEditAnimationFramePayloadIndex(pDesc->pLiveFramework,pDesc->pLiveFramework->currentEditLayerIndex);
		pPayload->impulse=PX_PointSub(PX_POINT(x,y,0),PX_POINT(pDesc->TagPoint.x,pDesc->TagPoint.y,0));
		pDesc->pLiveFramework->currentEditLayerIndex=-1;
		PX_LiveEditorModule_ImpulseEditorEnterSelectMode(pObject);
		return;
	}
	else
	{
		for (i=0;i<pDesc->pLiveFramework->layers.size;i++)
		{
			//
			PX_LiveLayer *pLayer;
			px_point layerCurrentPt;
			pLayer=PX_VECTORAT(PX_LiveLayer,&pDesc->pLiveFramework->layers,i);
			layerCurrentPt=pLayer->currentKeyPoint;
			if(PX_isPointInCircle(layerCurrentPt,PX_POINT(x,y,0),5))
			{
				pDesc->TagPoint.x=x;
				pDesc->TagPoint.y=y;
				pDesc->Cursor.x=x;
				pDesc->Cursor.y=y;
				PX_LiveEditorModule_ImpulseEditorEnterEditMode(pObject,i);
				return;
			}
		}
	}
}


px_void PX_LiveEditorModule_ImpulseEditorRender(px_surface *pRenderSurface,PX_Object *pObject,px_dword elpased)
{
	PX_LiveEditorModule_ImpulseEditor *pDesc=(PX_LiveEditorModule_ImpulseEditor *)pObject->pObject;


	if (!(pDesc->pLiveFramework->currentEditLayerIndex>=0&&pDesc->pLiveFramework->currentEditLayerIndex<pDesc->pLiveFramework->layers.size))
	{
		px_int i;
		
		for(i=0;i<pDesc->pLiveFramework->layers.size;i++)
		{
			PX_LiveAnimationFramePayload *pPayload=PX_LiveFrameworkGetCurrentEditAnimationFramePayloadIndex(pDesc->pLiveFramework,i);
			PX_LiveLayer *pLayer;
			if (!pPayload)
			{
				return;
			}
			if (pPayload->impulse.x||pPayload->impulse.y)
			{
				px_point2D p1,p2;
				pLayer=PX_LiveFrameworkGetLayer(pDesc->pLiveFramework,i);
				p1.x=pDesc->pLiveFramework->refer_x+pLayer->currentKeyPoint.x;
				p1.y=pDesc->pLiveFramework->refer_y+pLayer->currentKeyPoint.y;

				p2.x=pDesc->pLiveFramework->refer_x+pLayer->currentKeyPoint.x+pPayload->impulse.x;
				p2.y=pDesc->pLiveFramework->refer_y+pLayer->currentKeyPoint.y+pPayload->impulse.y;

				PX_GeoDrawArrow(pRenderSurface,p1,p2,1,PX_COLOR(255,0,0,255));
			}
		}
	}
	else
	{
		PX_LiveAnimationFramePayload *pPayload=PX_LiveFrameworkGetCurrentEditAnimationFramePayload(pDesc->pLiveFramework);
		PX_LiveLayer *pLayer;
		px_point2D p1,p2;
		if (!pPayload)
		{
			return;
		}
		pLayer=PX_LiveFrameworkGetLayer(pDesc->pLiveFramework,pDesc->pLiveFramework->currentEditLayerIndex);
		p1.x=pDesc->pLiveFramework->refer_x+pLayer->currentKeyPoint.x;
		p1.y=pDesc->pLiveFramework->refer_y+pLayer->currentKeyPoint.y;

		p2.x=pDesc->pLiveFramework->refer_x+pDesc->Cursor.x;
		p2.y=pDesc->pLiveFramework->refer_y+pDesc->Cursor.y;

		PX_GeoDrawArrow(pRenderSurface,p1,p2,1,PX_COLOR(255,0,0,255));
	}
}


PX_Object * PX_LiveEditorModule_ImpulseEditorInstall(PX_Object *pparent,PX_Runtime *pruntime,PX_FontModule *fm,PX_LiveFramework *pLiveFramework,PX_Json *pLanguageJson)
{
	PX_Object *pObject;
	PX_LiveEditorModule_ImpulseEditor Desc,*pDesc;
	PX_memset(&Desc,0,sizeof(Desc));
	pObject=PX_ObjectCreateEx(&pruntime->mp_ui,pparent,0,0,0,0,0,0,0,PX_NULL,PX_LiveEditorModule_ImpulseEditorRender,PX_NULL,&Desc,sizeof(Desc));
	pDesc=(PX_LiveEditorModule_ImpulseEditor *)pObject->pObject;
	pDesc->pLiveFramework=pLiveFramework;
	pDesc->pruntime=pruntime;
	pDesc->pLiveFramework->currentEditLayerIndex=-1;

	PX_ObjectRegisterEvent(pObject,PX_OBJECT_EVENT_CURSORDOWN,PX_LiveEditorModule_ImpulseEditorOnCursorDown,PX_NULL);
	PX_ObjectRegisterEvent(pObject,PX_OBJECT_EVENT_CURSORRDOWN,PX_LiveEditorModule_ImpulseEditorOnCursorRDown,PX_NULL);
	PX_ObjectRegisterEvent(pObject,PX_OBJECT_EVENT_CURSORMOVE,PX_LiveEditorModule_ImpulseEditorOnCursorMove,PX_NULL);
	return pObject;
}

px_void PX_LiveEditorModule_ImpulseEditorUninstall(PX_Object *pObject)
{
	PX_ObjectDelete(pObject);
}

px_void PX_LiveEditorModule_ImpulseEditorEnable(PX_Object *pObject)
{
	PX_LiveEditorModule_ImpulseEditor *pDesc=(PX_LiveEditorModule_ImpulseEditor *)pObject->pObject;
	pObject->Enabled=PX_TRUE;
	pObject->Visible=PX_TRUE;

	pDesc->pLiveFramework->showRootHelperLine=PX_FALSE;
	pDesc->pLiveFramework->currentEditLayerIndex=-1;
	pDesc->pLiveFramework->showlinker=PX_FALSE;
	pDesc->pLiveFramework->showKeypoint=PX_TRUE;
	pDesc->pLiveFramework->showFocusLayer=PX_TRUE;

}

px_void PX_LiveEditorModule_ImpulseEditorDisable(PX_Object *pObject)
{
	PX_LiveEditorModule_ImpulseEditor *pDesc=(PX_LiveEditorModule_ImpulseEditor *)pObject->pObject;
	if (pObject->Enabled)
	{
		pObject->Enabled=PX_FALSE;
		pObject->Visible=PX_FALSE;

		pDesc->pLiveFramework->currentEditLayerIndex=-1;
		pDesc->pLiveFramework->showlinker=PX_FALSE;
	}
}