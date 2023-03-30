#include "PainterEngine_LiveEditorModules_KeyTransform.h"

static px_void PX_LiveEditorModule_KeyTransformEnterEditMode(PX_Object *pObject,px_int layerIndex)
{
	PX_LiveEditorModule_KeyTransform *pDesc=(PX_LiveEditorModule_KeyTransform *)pObject->pObject;
	pDesc->pLiveFramework->showKeypoint=PX_FALSE;
	pDesc->pLiveFramework->showlinker=PX_FALSE;
	pDesc->pLiveFramework->showRange=PX_FALSE;
	pDesc->pLiveFramework->showFocusLayer=PX_FALSE;
	pDesc->pLiveFramework->currentEditLayerIndex=layerIndex;
	PX_ObjectSetFocus(pObject);
}

static px_void PX_LiveEditorModule_KeyTransformEnterSelectMode(PX_Object *pObject)
{
	PX_LiveEditorModule_KeyTransform *pDesc=(PX_LiveEditorModule_KeyTransform *)pObject->pObject;
	pDesc->pLiveFramework->showKeypoint=PX_TRUE;
	pDesc->pLiveFramework->showlinker=PX_FALSE;
	pDesc->pLiveFramework->showRange=PX_FALSE;
	pDesc->pLiveFramework->showFocusLayer=PX_FALSE;
	pDesc->pLiveFramework->currentEditLayerIndex=-1;
	pDesc->pTranform->Visible=PX_FALSE;
	PX_ObjectReleaseFocus(pObject);
}

static px_void PX_LiveEditorModule_KeyTransformUpdateFramePayload(PX_LiveFramework *plive,PX_LiveLayer *pLayer,px_float incRotation)
{
	PX_LiveAnimationFramePayload *pPayload=PX_LiveFrameworkGetCurrentEditAnimationFramePayloadIndex(plive,PX_LiveFrameworkGetLayerIndex(plive,pLayer));

	px_int i;
	pPayload->rotation+=incRotation;
	for (i=0;i<PX_COUNTOF(pLayer->child_index);i++)
	{
		PX_LiveLayer *pChild=PX_LiveFrameworkGetLayerChild(plive,pLayer,pLayer->child_index[i]);
		if(pChild)
			PX_LiveEditorModule_KeyTransformUpdateFramePayload(plive,pChild,incRotation);
	}
}

px_void PX_LiveEditorModule_KeyTransformOnTransformChanged(PX_Object *pObject,PX_Object_Event e,px_void *ptr)
{
	PX_Object *pDescObject=(PX_Object *)ptr;
	PX_LiveEditorModule_KeyTransform *pDesc=(PX_LiveEditorModule_KeyTransform *)pDescObject->pObject;
	PX_LiveAnimationFramePayload *pPayload=(PX_LiveAnimationFramePayload *)PX_LiveFrameworkGetCurrentEditAnimationFramePayload(pDesc->pLiveFramework);
	PX_LiveLayer *pLayer=PX_VECTORAT(PX_LiveLayer,&pDesc->pLiveFramework->layers,pDesc->pLiveFramework->currentEditLayerIndex);
	if (pLayer->parent_index!=-1)
	{
		px_float incRotation;
		PX_LiveLayer *pParentLayer=PX_LiveFrameworkGetLayerParent(pDesc->pLiveFramework,pLayer);
		PX_LiveAnimationFramePayload *pParentPayload=(PX_LiveAnimationFramePayload *)PX_LiveFrameworkGetCurrentEditAnimationFramePayloadIndex(pDesc->pLiveFramework,pLayer->parent_index);
		pPayload->stretch=PX_Object_TransformAdapterGetStretch(pDesc->pTranform);
		incRotation=PX_Object_TransformAdapterGetRotation(pDesc->pTranform)-pParentPayload->rotation;
		PX_LiveEditorModule_KeyTransformUpdateFramePayload(pDesc->pLiveFramework,pParentLayer,incRotation);
	}
	

	PX_LiveFrameworkRunCurrentEditFrame(pDesc->pLiveFramework);

}

px_void PX_LiveEditorModule_KeyTransformOnCursorRDown(PX_Object *pObject,PX_Object_Event e,px_void *ptr)
{
	PX_LiveEditorModule_KeyTransform *pTransform=(PX_LiveEditorModule_KeyTransform *)pObject->pObject;
	pTransform->pLiveFramework->currentEditLayerIndex=-1;
	pTransform->pTranform->Visible=PX_FALSE;;
	PX_LiveEditorModule_KeyTransformEnterSelectMode(pObject);
}

px_void PX_LiveEditorModule_KeyTransformOnCursorDown(PX_Object *pObject,PX_Object_Event e,px_void *ptr)
{
	PX_LiveEditorModule_KeyTransform *pTransform=(PX_LiveEditorModule_KeyTransform *)pObject->pObject;
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
			if (pLayer->parent_index!=-1)
			{
				PX_LiveLayer *pParentLayer=PX_LiveFrameworkGetLayerParent(pTransform->pLiveFramework,pLayer);
				PX_LiveAnimationFramePayload *pPayload=(PX_LiveAnimationFramePayload *)PX_LiveFrameworkGetCurrentEditAnimationFramePayloadIndex(pTransform->pLiveFramework,pLayer->parent_index);
				PX_LiveAnimationFramePayload *pCurPayload=(PX_LiveAnimationFramePayload *)PX_LiveFrameworkGetCurrentEditAnimationFramePayloadIndex(pTransform->pLiveFramework,i);
				if (!pPayload)
				{
					return;
				}

				PX_Object_TransformAdapterResetState(pTransform->pTranform,\
					pParentLayer->currentKeyPoint.x+pTransform->pLiveFramework->refer_x,\
					pParentLayer->currentKeyPoint.y+pTransform->pLiveFramework->refer_y,\
					PX_POINT2D(\
					pTransform->pLiveFramework->refer_x+pParentLayer->currentKeyPoint.x+pLayer->keyPoint.x-pParentLayer->keyPoint.x,\
					pTransform->pLiveFramework->refer_y+pParentLayer->currentKeyPoint.y+pLayer->keyPoint.y-pParentLayer->keyPoint.y\
					),\
					pPayload->rotation,\
					pCurPayload->stretch\
					);
				pTransform->pTranform->Visible=PX_TRUE;
			}
			else
			{
				pTransform->pTranform->Visible=PX_FALSE;;
			}
			PX_LiveEditorModule_KeyTransformEnterEditMode(pObject,i);
			return;
		}
	}
	
	
}

px_void PX_LiveEditorModule_KeyTransformOnCursorDrag(PX_Object *pObject,PX_Object_Event e,px_void *ptr)
{
	px_float x,y;
	PX_LiveEditorModule_KeyTransform *pDesc=(PX_LiveEditorModule_KeyTransform *)pObject->pObject;
	PX_LiveLayer *pLayer=PX_LiveFrameworkGetCurrentEditLiveLayer(pDesc->pLiveFramework);
	if (pLayer)
	{
		PX_LiveAnimationFramePayload *pPayload=(PX_LiveAnimationFramePayload *)PX_LiveFrameworkGetCurrentEditAnimationFramePayload(pDesc->pLiveFramework);

		if (pPayload&&pLayer->parent_index==-1)
		{
			px_float translate_x,translate_y;
			x=PX_Object_Event_GetCursorX(e);
			y=PX_Object_Event_GetCursorY(e);

			x-=pDesc->pLiveFramework->refer_x;
			y-=pDesc->pLiveFramework->refer_y;

			translate_x=x-pDesc->cursor_last_x;
			translate_y=y-pDesc->cursor_last_y;

			pPayload->translation.x+=translate_x;
			pPayload->translation.y+=translate_y;
			
			pDesc->cursor_last_x=x;
			pDesc->cursor_last_y=y;

			PX_LiveFrameworkRunCurrentEditFrame(pDesc->pLiveFramework);
		}
	}
}

PX_Object * PX_LiveEditorModule_KeyTransformInstall(PX_Object *pparent,PX_Runtime *pruntime,PX_FontModule *fm,PX_LiveFramework *pLiveFramework,PX_Json *pLanguageJson)
{
	PX_Object *pObject;
	PX_LiveEditorModule_KeyTransform Transform,*pTransform;
	PX_memset(&Transform,0,sizeof(Transform));
	pObject=PX_ObjectCreateEx(&pruntime->mp_ui,pparent,0,0,0,0,0,0,0,PX_NULL,PX_NULL,PX_NULL,&Transform,sizeof(Transform));
	pTransform=(PX_LiveEditorModule_KeyTransform *)pObject->pObject;
	pTransform->pLiveFramework=pLiveFramework;
	pTransform->pruntime=pruntime;
	pTransform->pLiveFramework->currentEditLayerIndex=-1;
	pTransform->pTranform=PX_Object_TransformAdapterCreate(&pruntime->mp_ui,pObject,0,0,PX_POINT2D(0,1));
	pTransform->pTranform->Visible=PX_FALSE;

	pObject->OnLostFocusReleaseEvent=PX_TRUE;
	PX_ObjectRegisterEvent(pTransform->pTranform,PX_OBJECT_EVENT_VALUECHANGED,PX_LiveEditorModule_KeyTransformOnTransformChanged,pObject);
	PX_ObjectRegisterEvent(pObject,PX_OBJECT_EVENT_CURSORDOWN,PX_LiveEditorModule_KeyTransformOnCursorDown,PX_NULL);
	PX_ObjectRegisterEvent(pObject,PX_OBJECT_EVENT_CURSORRDOWN,PX_LiveEditorModule_KeyTransformOnCursorRDown,PX_NULL);
	PX_ObjectRegisterEvent(pObject,PX_OBJECT_EVENT_CURSORDRAG,PX_LiveEditorModule_KeyTransformOnCursorDrag,PX_NULL);
	return pObject;
}

px_void PX_LiveEditorModule_KeyTransformUninstall(PX_Object *pObject)
{
	PX_ObjectDelete(pObject);
}

px_void PX_LiveEditorModule_KeyTransformEnable(PX_Object *pObject,PX_LIVEEDITORMODULE_KEYTRANSFORM_MODE mode)
{
	PX_LiveEditorModule_KeyTransform *pTransform=(PX_LiveEditorModule_KeyTransform *)pObject->pObject;
	pObject->Enabled=PX_TRUE;
	pObject->Visible=PX_TRUE;
	pTransform->pLiveFramework->showRootHelperLine=PX_TRUE;
	pTransform->pLiveFramework->currentEditLayerIndex=-1;
	pTransform->pLiveFramework->showlinker=PX_FALSE;
	pTransform->pLiveFramework->showKeypoint=PX_TRUE;
	pTransform->pLiveFramework->showFocusLayer=PX_FALSE;

	switch(mode)
	{
	case PX_LIVEEDITORMODULE_KEYTRANSFORM_MODE_ALL:
		PX_Object_TransformAdapterSetMode(pTransform->pTranform,PX_OBJECT_TRANSFORMADAPTER_MODE_ANY);
		break;
	case PX_LIVEEDITORMODULE_KEYTRANSFORM_MODE_ROTATION:
		PX_Object_TransformAdapterSetMode(pTransform->pTranform,PX_OBJECT_TRANSFORMADAPTER_MODE_ROTATION);
		break;
	case PX_LIVEEDITORMODULE_KEYTRANSFORM_MODE_STRETCH:
		PX_Object_TransformAdapterSetMode(pTransform->pTranform,PX_OBJECT_TRANSFORMADAPTER_MODE_STRETCH);
		break;
	}
	
}

px_void PX_LiveEditorModule_KeyTransformDisable(PX_Object *pObject)
{
	PX_LiveEditorModule_KeyTransform *pTransform=(PX_LiveEditorModule_KeyTransform *)pObject->pObject;
	pObject->Enabled=PX_FALSE;
	pObject->Visible=PX_FALSE;
	
}