#include "PainterEngine_LiveEditorModules_GlobalRotation.h"

static px_void PX_LiveEditorModule_GlobalRotationEnterEditMode(PX_Object *pObject,px_int layerIndex)
{
	PX_LiveEditorModule_GlobalRotation *pDesc=(PX_LiveEditorModule_GlobalRotation *)pObject->pObject;
	pDesc->pLiveFramework->showKeypoint=PX_FALSE;
	pDesc->pLiveFramework->showlinker=PX_FALSE;
	pDesc->pLiveFramework->showRange=PX_FALSE;
	pDesc->pLiveFramework->showFocusLayer=PX_FALSE;
	pDesc->pTranform->Visible=PX_TRUE;
	pDesc->pLiveFramework->currentEditLayerIndex=layerIndex;
	PX_ObjectSetFocus(pObject);
}

static px_void PX_LiveEditorModule_GlobalRotationEnterSelectMode(PX_Object *pObject)
{
	PX_LiveEditorModule_GlobalRotation *pDesc=(PX_LiveEditorModule_GlobalRotation *)pObject->pObject;
	pDesc->pLiveFramework->showKeypoint=PX_TRUE;
	pDesc->pLiveFramework->showlinker=PX_FALSE;
	pDesc->pLiveFramework->showRange=PX_FALSE;
	pDesc->pLiveFramework->showFocusLayer=PX_FALSE;
	pDesc->pLiveFramework->currentEditLayerIndex=-1;
	pDesc->pTranform->Visible=PX_FALSE;
	PX_ObjectReleaseFocus(pObject);
}


px_void PX_LiveEditorModule_GlobalRotationUpdateFramePayload(PX_LiveFramework *plive,px_int LayerIndex,px_float incRotation)
{
	PX_LiveLayer *pLayer=PX_VECTORAT(PX_LiveLayer,&plive->layers,LayerIndex);
	PX_LiveAnimationFramePayload *pPayload=PX_LiveFrameworkGetCurrentEditAnimationFramePayloadIndex(plive,LayerIndex);

	px_int i;
	pPayload->rotation+=incRotation;
	for (i=0;i<PX_COUNTOF(pLayer->child_index);i++)
	{
		if (pLayer->child_index[i]==-1)
		{
			break;
		}
		
		PX_LiveEditorModule_GlobalRotationUpdateFramePayload(plive,pLayer->child_index[i],incRotation);
	}
}

px_void PX_LiveEditorModule_GlobalRotationOnTransformChanged(PX_Object *pObject,PX_Object_Event e,px_void *ptr)
{
	PX_Object *pDescObject=(PX_Object *)ptr;
	PX_LiveEditorModule_GlobalRotation *pDesc=(PX_LiveEditorModule_GlobalRotation *)pDescObject->pObject;
	PX_LiveAnimationFramePayload *pPayload=(PX_LiveAnimationFramePayload *)PX_LiveFrameworkGetCurrentEditAnimationFramePayload(pDesc->pLiveFramework);
	PX_LiveLayer *pLayer=PX_VECTORAT(PX_LiveLayer,&pDesc->pLiveFramework->layers,pDesc->pLiveFramework->currentEditLayerIndex);
	px_float currentRotation;
	px_float incRotation;
	currentRotation=PX_Object_TransformAdapterGetRotation(pDesc->pTranform);
	incRotation=currentRotation-pPayload->rotation;
	PX_LiveEditorModule_GlobalRotationUpdateFramePayload(pDesc->pLiveFramework,PX_LiveFrameworkGetLayerIndex(pDesc->pLiveFramework,pLayer),incRotation);
	PX_LiveFrameworkRunCurrentEditFrame(pDesc->pLiveFramework);

}

px_void PX_LiveEditorModule_GlobalRotationOnCursorRDown(PX_Object *pObject,PX_Object_Event e,px_void *ptr)
{
	PX_LiveEditorModule_GlobalRotationEnterSelectMode(pObject);
}

px_void PX_LiveEditorModule_GlobalRotationOnCursorDown(PX_Object *pObject,PX_Object_Event e,px_void *ptr)
{
	PX_LiveEditorModule_GlobalRotation *pTransform=(PX_LiveEditorModule_GlobalRotation *)pObject->pObject;
	px_float x,y;
	px_int i;

	if (pTransform->pLiveFramework->currentEditLayerIndex>=0&&pTransform->pLiveFramework->currentEditLayerIndex<pTransform->pLiveFramework->layers.size)
	{
		return;
	}

	x=PX_Object_Event_GetCursorX(e);
	y=PX_Object_Event_GetCursorY(e);

	x-=pTransform->pLiveFramework->refer_x;
	y-=pTransform->pLiveFramework->refer_y;

	for (i=0;i<pTransform->pLiveFramework->layers.size;i++)
	{
		//
		PX_LiveLayer *pLayer;
		px_point layerCurrentPt;
		pLayer=PX_VECTORAT(PX_LiveLayer,&pTransform->pLiveFramework->layers,i);
		layerCurrentPt=pLayer->currentKeyPoint;
		if(PX_isPointInCircle(layerCurrentPt,PX_POINT(x,y,0),5))
		{
			pTransform->cursor_last_x=x;
			pTransform->cursor_last_y=y;
			pTransform->pLiveFramework->currentEditLayerIndex=i;
			if (pLayer->child_index[0]!=-1&&!pLayer->child_index[1]==-1)
			{
				PX_LiveLayer *pLinkLayer0=PX_LiveFrameworkGetLayerChild(pTransform->pLiveFramework,pLayer,pLayer->child_index[0]);
				PX_LiveAnimationFramePayload *pPayload=(PX_LiveAnimationFramePayload *)PX_LiveFrameworkGetCurrentEditAnimationFramePayloadIndex(pTransform->pLiveFramework,i);
				if (!pPayload)
				{
					return;
				}
				

				PX_Object_TransformAdapterResetState(pTransform->pTranform,\
					pLayer->currentKeyPoint.x+pTransform->pLiveFramework->refer_x,\
					pLayer->currentKeyPoint.y+pTransform->pLiveFramework->refer_y,\
					PX_POINT2D(\
					pLayer->currentKeyPoint.x+pTransform->pLiveFramework->refer_x+(pLinkLayer0->keyPoint.x-pLayer->keyPoint.x),\
					pLayer->currentKeyPoint.y+pTransform->pLiveFramework->refer_y+(pLinkLayer0->keyPoint.y-pLayer->keyPoint.y)\
					),\
					pPayload->rotation,\
					pPayload->stretch\
					);
			}
			else
			{
				PX_LiveAnimationFramePayload *pPayload=(PX_LiveAnimationFramePayload *)PX_LiveFrameworkGetCurrentEditAnimationFramePayloadIndex(pTransform->pLiveFramework,i);
				if (!pPayload)
				{
					return;
				}

				PX_Object_TransformAdapterResetState(pTransform->pTranform,\
					pLayer->currentKeyPoint.x+pTransform->pLiveFramework->refer_x,\
					pLayer->currentKeyPoint.y+pTransform->pLiveFramework->refer_y,\
					PX_POINT2D(\
					pTransform->pLiveFramework->refer_x+pLayer->currentKeyPoint.x,\
					pTransform->pLiveFramework->refer_y+pLayer->currentKeyPoint.y-80\
					),\
					pPayload->rotation,\
					pPayload->stretch\
					);
			}
			PX_LiveEditorModule_GlobalRotationEnterEditMode(pObject,i);
			return;
		}
	}
	PX_LiveEditorModule_GlobalRotationEnterSelectMode(pObject);
}


PX_Object * PX_LiveEditorModule_GlobalRotationInstall(PX_Object *pparent,PX_Runtime *pruntime,PX_FontModule *fm,PX_LiveFramework *pLiveFramework,PX_Json *pLanguageJson)
{
	PX_Object *pObject;
	PX_LiveEditorModule_GlobalRotation Transform,*pTransform;
	PX_memset(&Transform,0,sizeof(Transform));
	pObject=PX_ObjectCreateEx(&pruntime->mp_ui,pparent,0,0,0,0,0,0,0,PX_NULL,PX_NULL,PX_NULL,&Transform,sizeof(Transform));
	pTransform=(PX_LiveEditorModule_GlobalRotation *)pObject->pObject;
	pTransform->pLiveFramework=pLiveFramework;
	pTransform->pruntime=pruntime;
	pTransform->pLiveFramework->currentEditLayerIndex=-1;
	pTransform->pTranform=PX_Object_TransformAdapterCreate(&pruntime->mp_ui,pObject,0,0,PX_POINT2D(0,1));
	PX_Object_TransformAdapterSetMode(pTransform->pTranform,PX_OBJECT_TRANSFORMADAPTER_MODE_ROTATION);
	pTransform->pTranform->Visible=PX_FALSE;
	pObject->OnLostFocusReleaseEvent=PX_TRUE;
	
	PX_ObjectRegisterEvent(pTransform->pTranform,PX_OBJECT_EVENT_VALUECHANGED,PX_LiveEditorModule_GlobalRotationOnTransformChanged,pObject);
	PX_ObjectRegisterEvent(pObject,PX_OBJECT_EVENT_CURSORDOWN,PX_LiveEditorModule_GlobalRotationOnCursorDown,PX_NULL);
	PX_ObjectRegisterEvent(pObject,PX_OBJECT_EVENT_CURSORRDOWN,PX_LiveEditorModule_GlobalRotationOnCursorRDown,PX_NULL);
	return pObject;
}

px_void PX_LiveEditorModule_GlobalRotationUninstall(PX_Object *pObject)
{
	PX_ObjectDelete(pObject);
}

px_void PX_LiveEditorModule_GlobalRotationEnable(PX_Object *pObject)
{
	PX_LiveEditorModule_GlobalRotation *pTransform=(PX_LiveEditorModule_GlobalRotation *)pObject->pObject;
	pObject->Enabled=PX_TRUE;
	pObject->Visible=PX_TRUE;
	PX_LiveEditorModule_GlobalRotationEnterSelectMode(pObject);
}

px_void PX_LiveEditorModule_GlobalRotationDisable(PX_Object *pObject)
{
	PX_LiveEditorModule_GlobalRotation *pTransform=(PX_LiveEditorModule_GlobalRotation *)pObject->pObject;
	if (pObject->Enabled==PX_TRUE)
	{
		pObject->Enabled=PX_FALSE;
		pObject->Visible=PX_FALSE;

		pTransform->pLiveFramework->currentEditLayerIndex=-1;
		pTransform->pLiveFramework->showlinker=PX_FALSE;

	}
	
}