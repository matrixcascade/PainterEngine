#include "PainterEngine_LiveEditorModules_Panc.h"

static px_void PX_LiveEditorModule_PancEnterEditMode(PX_Object* pObject, px_int layerIndex)
{
	PX_LiveEditorModule_Panc* pDesc = (PX_LiveEditorModule_Panc*)pObject->pObject;
	PX_LiveAnimationFramePayload* pPayload = PX_LiveFrameworkGetCurrentEditAnimationFramePayloadIndex(pDesc->pLiveFramework, layerIndex);
	pDesc->pLiveFramework->showKeypoint = PX_FALSE;
	pDesc->pLiveFramework->currentEditLayerIndex = layerIndex;
	if (pPayload->panc_width&&pPayload->panc_height)
	{
		PX_Object_Panc* pPanc = PX_ObjectGetDesc(PX_Object_Panc, pDesc->panc);
		PX_Object_PancReset(pDesc->panc, pDesc->pLiveFramework->refer_x+pPayload->panc_x, pDesc->pLiveFramework->refer_y + pPayload->panc_y, (px_float)pPayload->panc_width, (px_float)pPayload->panc_height);
		pPanc->sourceX = pDesc->pLiveFramework->refer_x + pPayload->panc_sx;
		pPanc->sourceY = pDesc->pLiveFramework->refer_y + pPayload->panc_sy;

		pPanc->currentX = pDesc->pLiveFramework->refer_x + pPayload->panc_endx;
		pPanc->currentY = pDesc->pLiveFramework->refer_y + pPayload->panc_endy;
	}
	else
	{
		PX_Object_PancReset(pDesc->panc, pDesc->pLiveFramework->refer_x, pDesc->pLiveFramework->refer_y, (px_float)pDesc->pLiveFramework->width, (px_float)pDesc->pLiveFramework->height);
	}
	pDesc->panc->Visible = PX_TRUE;
	PX_ObjectSetFocus(pObject);
}

static px_void PX_LiveEditorModule_PancEnterSelectMode(PX_Object* pObject)
{
	PX_LiveEditorModule_Panc* pDesc = (PX_LiveEditorModule_Panc*)pObject->pObject;
	pDesc->pLiveFramework->showKeypoint = PX_TRUE;
	pDesc->pLiveFramework->currentEditLayerIndex = -1;
	pDesc->panc->Visible = PX_FALSE;
	PX_ObjectReleaseFocus(pObject);
}

px_void PX_LiveEditorModule_PancUpdateFramePayload(PX_LiveFramework* plive, px_int LayerIndex, PX_PancMatrix mat)
{
	PX_LiveLayer* pLayer = PX_VECTORAT(PX_LiveLayer, &plive->layers, LayerIndex);
	PX_LiveAnimationFramePayload* pPayload = PX_LiveFrameworkGetCurrentEditAnimationFramePayloadIndex(plive, LayerIndex);

	px_int i;
	pPayload->panc_x = mat.x;
	pPayload->panc_y = mat.y;
	pPayload->panc_width = mat.width;
	pPayload->panc_height = mat.height;
	pPayload->panc_sx = mat.sourceX;
	pPayload->panc_sy= mat.sourceY;
	pPayload->panc_endx = mat.currentX;
	pPayload->panc_endy= mat.currentY;
	for (i = 0; i < PX_COUNTOF(pLayer->child_index); i++)
	{
		if (pLayer->child_index[i] == -1)
		{
			break;
		}

		PX_LiveEditorModule_PancUpdateFramePayload(plive, pLayer->child_index[i], mat);
	}
}

px_void PX_LiveEditorModule_PancOnTransformChanged(PX_Object* pObject, PX_Object_Event e, px_void* ptr)
{
	PX_Object* pDescObject = (PX_Object*)ptr;
	PX_LiveEditorModule_Panc* pDesc = (PX_LiveEditorModule_Panc*)pDescObject->pObject;
	PX_LiveAnimationFramePayload* pPayload = (PX_LiveAnimationFramePayload*)PX_LiveFrameworkGetCurrentEditAnimationFramePayload(pDesc->pLiveFramework);
	PX_LiveLayer* pLayer = PX_VECTORAT(PX_LiveLayer, &pDesc->pLiveFramework->layers, pDesc->pLiveFramework->currentEditLayerIndex);
	PX_PancMatrix mat;

	mat = PX_Object_PancGetMatrix(pDesc->panc);
	mat.x-=pDesc->pLiveFramework->refer_x;
	mat.y-=pDesc->pLiveFramework->refer_y;

	mat.currentX-=pDesc->pLiveFramework->refer_x;
	mat.currentY-=pDesc->pLiveFramework->refer_y;

	mat.sourceX-=pDesc->pLiveFramework->refer_x;
	mat.sourceY-=pDesc->pLiveFramework->refer_y;

	PX_LiveEditorModule_PancUpdateFramePayload(pDesc->pLiveFramework, PX_LiveFrameworkGetLayerIndex(pDesc->pLiveFramework, pLayer), mat);

	PX_LiveFrameworkRunCurrentEditFrame(pDesc->pLiveFramework);

}

px_void PX_LiveEditorModule_PancOnCursorMove(PX_Object* pObject, PX_Object_Event e, px_void* ptr)
{
	PX_LiveEditorModule_Panc* pDesc = (PX_LiveEditorModule_Panc*)pObject->pObject;
	px_float x, y;

	if (pDesc->pLiveFramework->currentEditLayerIndex >= 0 && pDesc->pLiveFramework->currentEditLayerIndex < pDesc->pLiveFramework->layers.size)
	{
		x = PX_Object_Event_GetCursorX(e);
		y = PX_Object_Event_GetCursorY(e);

		x -= pDesc->pLiveFramework->refer_x;
		y -= pDesc->pLiveFramework->refer_y;

		pDesc->Cursor = PX_POINT2D(x, y);
	}
}

px_void PX_LiveEditorModule_PancOnCursorDown(PX_Object* pObject, PX_Object_Event e, px_void* ptr)
{
	PX_LiveEditorModule_Panc* pDesc = (PX_LiveEditorModule_Panc*)pObject->pObject;
	px_float x, y;
	px_int i;

	x = PX_Object_Event_GetCursorX(e);
	y = PX_Object_Event_GetCursorY(e);

	x -= pDesc->pLiveFramework->refer_x;
	y -= pDesc->pLiveFramework->refer_y;

	if (pDesc->pLiveFramework->currentEditLayerIndex >= 0 && pDesc->pLiveFramework->currentEditLayerIndex < pDesc->pLiveFramework->layers.size)
	{
		//edit mode
		return;
	}
	else
	{
		for (i = 0; i < pDesc->pLiveFramework->layers.size; i++)
		{
			//
			PX_LiveLayer* pLayer;
			px_point layerCurrentPt;
			pLayer = PX_VECTORAT(PX_LiveLayer, &pDesc->pLiveFramework->layers, i);
			layerCurrentPt = pLayer->currentKeyPoint;
			if (PX_isPointInCircle(layerCurrentPt, PX_POINT(x, y, 0), 5))
			{
				pDesc->Cursor.x = x;
				pDesc->Cursor.y = y;
				PX_LiveEditorModule_PancEnterEditMode(pObject, i);
				return;
			}
		}
	}
}


px_void PX_LiveEditorModule_PancRender(px_surface* pRenderSurface, PX_Object* pObject, px_dword elpased)
{
	PX_LiveEditorModule_Panc* pDesc = (PX_LiveEditorModule_Panc*)pObject->pObject;

}



px_void PX_LiveEditorModule_PancOnButtonReset(PX_Object* pObject, PX_Object_Event e, px_void* ptr)
{
	px_int i;
	PX_Object* pLinkObject = (PX_Object*)ptr;
	PX_LiveEditorModule_Panc* pDesc = (PX_LiveEditorModule_Panc*)pLinkObject->pObject;
	for (i = 0; i < pDesc->pLiveFramework->layers.size; i++)
	{
		PX_LiveAnimationFramePayload* pPayload = PX_LiveFrameworkGetCurrentEditAnimationFramePayloadIndex(pDesc->pLiveFramework, i);
		PX_LiveLayer* pLivelayer = PX_LiveFrameworkGetLayer(pDesc->pLiveFramework, i);
		if (pPayload)
		{
			pPayload->panc_endx = 0;
			pPayload->panc_endy = 0;
			pPayload->panc_height = 0;
			pPayload->panc_width = 0;
			pPayload->panc_sx = 0;
			pPayload->panc_sy = 0;
			pPayload->panc_x = 0;
			pPayload->panc_y = 0;
		}
	}
	PX_LiveFrameworkRunCurrentEditFrame(pDesc->pLiveFramework);

}

px_void PX_LiveEditorModule_PancOnButtonFinish(PX_Object* pObject, PX_Object_Event e, px_void* ptr)
{
	PX_Object* pLinkkeyObject = (PX_Object*)ptr;
	PX_LiveEditorModule_Panc* pDesc = (PX_LiveEditorModule_Panc*)pLinkkeyObject->pObject;
	pDesc->pLiveFramework->showlinker = PX_FALSE;
	PX_ObjectExecuteEvent(pLinkkeyObject->pParent, PX_OBJECT_BUILD_EVENT(PX_LIVEFRAMEWORKMODULES_PANC_EVENT_EXIT));
}


PX_Object* PX_LiveEditorModule_PancInstall(PX_Object* pparent, PX_Runtime* pruntime, PX_FontModule* fm, PX_LiveFramework* pLiveFramework, PX_Json* pLanguageJson)
{
	PX_Object* pObject;
	PX_LiveEditorModule_Panc Desc, * pDesc;
	PX_memset(&Desc, 0, sizeof(Desc));
	pObject = PX_ObjectCreateEx(&pruntime->mp_ui, pparent, 0, 0, 0, 0, 0, 0, 0, PX_NULL, PX_LiveEditorModule_PancRender, PX_NULL, &Desc, sizeof(Desc));
	pDesc = (PX_LiveEditorModule_Panc*)pObject->pObject;
	pDesc->pLiveFramework = pLiveFramework;
	pDesc->pruntime = pruntime;
	pDesc->pLiveFramework->currentEditLayerIndex = -1;
	pDesc->fontmodule = fm;
	pDesc->pLanguageJson = pLanguageJson;

	pDesc->panc = PX_Object_PancCreate(&pruntime->mp_ui, pObject, 0, 0, (px_float)pDesc->pLiveFramework->width, (px_float)pDesc->pLiveFramework->height);
	pDesc->panc->Visible = PX_FALSE;
	PX_ObjectRegisterEvent(pDesc->panc, PX_OBJECT_EVENT_VALUECHANGED, PX_LiveEditorModule_PancOnTransformChanged, pObject);

	pDesc->root = PX_Object_WidgetCreate(&pruntime->mp_ui, pObject, pruntime->surface_width - 280, 64, 176, 62, "", fm);
	
	PX_Object_WidgetShowHideCloseButton(pDesc->root, PX_FALSE);
	pDesc->button_reset = PX_Object_PushButtonCreate(&pruntime->mp_ui, pDesc->root, 3 + 85 * 0, 3, 84, 32, PX_JsonGetString(pLanguageJson, "panc.reset"), fm);
	pDesc->button_finish = PX_Object_PushButtonCreate(&pruntime->mp_ui, pDesc->root, 3 + 85 * 1, 3, 84, 32, PX_JsonGetString(pLanguageJson, "panc.finish"), fm);

	PX_ObjectRegisterEvent(pDesc->button_reset, PX_OBJECT_EVENT_EXECUTE, PX_LiveEditorModule_PancOnButtonReset, pObject);
	PX_ObjectRegisterEvent(pDesc->button_finish, PX_OBJECT_EVENT_EXECUTE, PX_LiveEditorModule_PancOnButtonFinish, pObject);

	PX_ObjectRegisterEvent(pObject, PX_OBJECT_EVENT_CURSORDOWN, PX_LiveEditorModule_PancOnCursorDown, PX_NULL);
	PX_ObjectRegisterEvent(pObject, PX_OBJECT_EVENT_CURSORMOVE, PX_LiveEditorModule_PancOnCursorMove, PX_NULL);

	
	

	return pObject;
}

px_void PX_LiveEditorModule_PancUninstall(PX_Object* pObject)
{
	PX_ObjectDelete(pObject);
}

px_void PX_LiveEditorModule_PancEnable(PX_Object* pObject)
{
	PX_LiveEditorModule_Panc* pDesc = (PX_LiveEditorModule_Panc*)pObject->pObject;
	pObject->Enabled = PX_TRUE;
	pObject->Visible = PX_TRUE;

	pDesc->pLiveFramework->showRootHelperLine = PX_FALSE;
	pDesc->pLiveFramework->currentEditLayerIndex = -1;
	pDesc->pLiveFramework->showlinker = PX_FALSE;
	pDesc->pLiveFramework->showKeypoint = PX_TRUE;
	pDesc->pLiveFramework->showFocusLayer = PX_FALSE;

	PX_LiveEditorModule_PancEnterSelectMode(pObject);

}

px_void PX_LiveEditorModule_PancDisable(PX_Object* pObject)
{
	PX_LiveEditorModule_Panc* pDesc = (PX_LiveEditorModule_Panc*)pObject->pObject;
	if (pObject->Enabled)
	{
		pObject->Enabled = PX_FALSE;
		pObject->Visible = PX_FALSE;

		pDesc->pLiveFramework->currentEditLayerIndex = -1;
		pDesc->pLiveFramework->showlinker = PX_FALSE;
	}
}