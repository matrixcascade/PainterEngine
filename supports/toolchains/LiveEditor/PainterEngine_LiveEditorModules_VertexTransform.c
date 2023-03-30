#include "PainterEngine_LiveEditorModules_VertexTransform.h"

static px_void PX_LiveEditorModule_VertexTransformEnterEditMode(PX_Object *pObject,px_int layerIndex)
{
	PX_LiveEditorModule_VertexTransform *pDesc=(PX_LiveEditorModule_VertexTransform *)pObject->pObject;
	PX_LiveLayer *pLayer;
	pDesc->pLiveFramework->showKeypoint=PX_FALSE;
	pDesc->pLiveFramework->showFocusLayer=PX_TRUE;
	pDesc->pLiveFramework->currentEditLayerIndex=layerIndex;
	pLayer=PX_LiveFrameworkGetCurrentEditLiveLayer(pDesc->pLiveFramework);
	if (pLayer)
	{
		pLayer->showMesh=PX_TRUE;
	}
	PX_ObjectSetFocus(pObject);
}

static px_void PX_LiveEditorModule_VertexTransformEnterSelectMode(PX_Object *pObject)
{
	PX_LiveEditorModule_VertexTransform *pDesc=(PX_LiveEditorModule_VertexTransform *)pObject->pObject;
	PX_LiveLayer *pLayer;
	pLayer=PX_LiveFrameworkGetCurrentEditLiveLayer(pDesc->pLiveFramework);
	pDesc->pLiveFramework->showFocusLayer=PX_FALSE;
	pDesc->pLiveFramework->showKeypoint=PX_TRUE;
	pDesc->pLiveFramework->currentEditVertexIndex=-1;
	pDesc->pLiveFramework->currentEditLayerIndex=-1;
	if (pLayer)
	{
		pLayer->showMesh=PX_FALSE;
	}

	PX_ObjectReleaseFocus(pObject);
}


px_void PX_LiveEditorModule_VertexTransformOnCursorRDown(PX_Object *pObject,PX_Object_Event e,px_void *ptr)
{
	PX_LiveEditorModule_VertexTransformEnterSelectMode(pObject);
}

static px_void PX_LiveEditorModule_VertexTransformOnCursorDown_editMode(PX_Object *pObject,px_float x,px_float y)
{
	PX_LiveEditorModule_VertexTransform *pDesc=(PX_LiveEditorModule_VertexTransform *)pObject->pObject;
	PX_LiveLayer *pLayer;
	px_int i;
	
	pLayer=PX_LiveFrameworkGetCurrentEditLiveLayer(pDesc->pLiveFramework);
	if (!pLayer)
	{
		PX_ASSERT();
		return;
	}

	for (i=0;i<pLayer->vertices.size;i++)
	{
		PX_LiveVertex *pVertex;
		pVertex=PX_VECTORAT(PX_LiveVertex,&pLayer->vertices,i);
		if(PX_isPointInCircle(pVertex->currentPosition,PX_POINT(x,y,0),5))
		{
			pDesc->pLiveFramework->currentEditVertexIndex=i;
			pDesc->cursor_last_x=x;
			pDesc->cursor_last_y=y;
			return;
		}
	}
	pDesc->pLiveFramework->currentEditVertexIndex=-1;
}

static px_void PX_LiveEditorModule_VertexTransformOnCursorDown_selectMode(PX_Object *pObject,px_float x,px_float y)
{
	PX_LiveEditorModule_VertexTransform *pDesc=(PX_LiveEditorModule_VertexTransform *)pObject->pObject;
	px_int i;
	for (i=0;i<pDesc->pLiveFramework->layers.size;i++)
	{
		PX_LiveLayer *pLayer;
		px_point layerCurrentPt;
		pLayer=PX_VECTORAT(PX_LiveLayer,&pDesc->pLiveFramework->layers,i);
		layerCurrentPt=pLayer->currentKeyPoint;
		if(PX_isPointInCircle(layerCurrentPt,PX_POINT(x,y,0),5))
		{
			pDesc->cursor_last_x=x;
			pDesc->cursor_last_y=y;
			PX_LiveEditorModule_VertexTransformEnterEditMode(pObject,i);
			return;
		}
	}
}

px_void PX_LiveEditorModule_VertexTransformOnCursorDown(PX_Object *pObject,PX_Object_Event e,px_void *ptr)
{
	PX_LiveEditorModule_VertexTransform *pDesc=(PX_LiveEditorModule_VertexTransform *)pObject->pObject;
	px_float x,y;
	

	x=PX_Object_Event_GetCursorX(e);
	y=PX_Object_Event_GetCursorY(e);

	x-=pDesc->pLiveFramework->refer_x;
	y-=pDesc->pLiveFramework->refer_y;

	if (PX_LiveFrameworkGetCurrentEditLiveLayer(pDesc->pLiveFramework))
	{
		PX_LiveEditorModule_VertexTransformOnCursorDown_editMode(pObject,x,y);
	}
	else
	{
		PX_LiveEditorModule_VertexTransformOnCursorDown_selectMode(pObject,x,y);
	}
	
	
}

px_void PX_LiveEditorModule_VertexTransformOnCursorDrag(PX_Object *pObject,PX_Object_Event e,px_void *ptr)
{
	px_float x,y;
	PX_LiveEditorModule_VertexTransform *pDesc=(PX_LiveEditorModule_VertexTransform *)pObject->pObject;


	px_point *ppoint=PX_LiveFrameworkGetCurrentEditAnimationFramePayloadVertex(pDesc->pLiveFramework);
	if (ppoint)
	{
		PX_LiveLayer *pLayer=PX_LiveFrameworkGetCurrentEditLiveLayer(pDesc->pLiveFramework);
		px_point presult;
		x=PX_Object_Event_GetCursorX(e);
		y=PX_Object_Event_GetCursorY(e);

		x-=pDesc->pLiveFramework->refer_x;
		y-=pDesc->pLiveFramework->refer_y;

		presult.x=x-pDesc->cursor_last_x;
		presult.y=y-pDesc->cursor_last_y;
		presult.z=0;

		presult=PX_PointRotate(presult,-pLayer->rel_currentRotationAngle);

		ppoint->x+=presult.x;
		ppoint->y+=presult.y;

		pDesc->cursor_last_x=x;
		pDesc->cursor_last_y=y;
		PX_LiveFrameworkRunCurrentEditFrame(pDesc->pLiveFramework);
	}

}


PX_Object * PX_LiveEditorModule_VertexTransformInstall(PX_Object *pparent,PX_Runtime *pruntime,PX_FontModule *fm,PX_LiveFramework *pLiveFramework,PX_Json *pLanguageJson)
{
	PX_Object *pObject;
	PX_LiveEditorModule_VertexTransform Transform,*pTransform;
	PX_memset(&Transform,0,sizeof(Transform));
	pObject=PX_ObjectCreateEx(&pruntime->mp_ui,pparent,0,0,0,0,0,0,0,PX_NULL,PX_NULL,PX_NULL,&Transform,sizeof(Transform));
	pTransform=(PX_LiveEditorModule_VertexTransform *)pObject->pObject;
	pTransform->pLiveFramework=pLiveFramework;
	pTransform->pruntime=pruntime;
	PX_ObjectRegisterEvent(pObject,PX_OBJECT_EVENT_CURSORDOWN,PX_LiveEditorModule_VertexTransformOnCursorDown,PX_NULL);
	PX_ObjectRegisterEvent(pObject,PX_OBJECT_EVENT_CURSORRDOWN,PX_LiveEditorModule_VertexTransformOnCursorRDown,PX_NULL);
	PX_ObjectRegisterEvent(pObject,PX_OBJECT_EVENT_CURSORDRAG,PX_LiveEditorModule_VertexTransformOnCursorDrag,PX_NULL);
	return pObject;
}

px_void PX_LiveEditorModule_VertexTransformUninstall(PX_Object *pObject)
{
	PX_ObjectDelete(pObject);
}

px_void PX_LiveEditorModule_VertexTransformEnable(PX_Object *pObject)
{
	PX_LiveEditorModule_VertexTransform *pDesc=(PX_LiveEditorModule_VertexTransform *)pObject->pObject;
	pDesc->pLiveFramework->showKeypoint=PX_TRUE;
	pDesc->pLiveFramework->currentEditLayerIndex=-1;
	pDesc->pLiveFramework->currentEditVertexIndex=-1;
	pObject->Enabled=PX_TRUE;
	pObject->Visible=PX_TRUE;	
}

px_void PX_LiveEditorModule_VertexTransformDisable(PX_Object *pObject)
{
	PX_LiveEditorModule_VertexTransform *pDesc=(PX_LiveEditorModule_VertexTransform *)pObject->pObject;
	pDesc->pLiveFramework->currentEditLayerIndex=-1;
	pDesc->pLiveFramework->currentEditVertexIndex=-1;
	pObject->Enabled=PX_FALSE;
	pObject->Visible=PX_FALSE;
}