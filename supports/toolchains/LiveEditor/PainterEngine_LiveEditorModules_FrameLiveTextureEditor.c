#include "PainterEngine_LiveEditorModules_FrameLiveTextureEditor.h"

px_void PX_LiveEditorModule_FrameLiveTextureEditorOnCursorRDown(PX_Object *pObject,PX_Object_Event e,px_void *ptr)
{
	PX_LiveEditorModule_FrameLiveTextureEditor *pDesc=(PX_LiveEditorModule_FrameLiveTextureEditor *)pObject->pObject;
	PX_ObjectReleaseFocus(pObject);
	pDesc->window_widget->Visible=PX_FALSE;
	pDesc->pLiveFramework->showKeypoint=PX_TRUE;
	pDesc->pLiveFramework->currentEditLayerIndex=-1;
}


static px_void PX_LiveEditorModule_FrameLiveTextureEditorOnCursorDown_selectMode(PX_Object *pObject,px_float x,px_float y)
{
	PX_LiveEditorModule_FrameLiveTextureEditor *pDesc=(PX_LiveEditorModule_FrameLiveTextureEditor *)pObject->pObject;
	px_int i;
	PX_LiveTexture *pLiveTexture;
	for (i=0;i<pDesc->pLiveFramework->layers.size;i++)
	{
		PX_LiveLayer *pLayer;
		px_point layerCurrentPt;
		pLayer=PX_VECTORAT(PX_LiveLayer,&pDesc->pLiveFramework->layers,i);
		layerCurrentPt=pLayer->currentKeyPoint;
		if(PX_isPointInCircle(layerCurrentPt,PX_POINT(x,y,0),5))
		{
			pDesc->pLiveFramework->currentEditLayerIndex=i;
			pDesc->pLiveFramework->showKeypoint=PX_FALSE;
			pDesc->window_widget->Visible=PX_TRUE;
			pLiveTexture=PX_LiveFrameworkGetLiveTexture(pDesc->pLiveFramework,pLayer->RenderTextureIndex);
			if (pLiveTexture!=PX_NULL)
			{
				PX_Object_EditSetText(pDesc->edit_textureid,pLiveTexture->id);
			}
			else
			{
				PX_Object_EditSetText(pDesc->edit_textureid,"Null");
			}
			PX_ObjectSetFocus(pObject);
			return;
		}
		else
		{
			PX_ObjectReleaseFocus(pObject);
			pDesc->window_widget->Visible=PX_FALSE;
			pDesc->pLiveFramework->currentEditLayerIndex=-1;
		}
	}
}

px_void PX_LiveEditorModule_FrameLiveTextureEditorOnCursorDown(PX_Object *pObject,PX_Object_Event e,px_void *ptr)
{
	PX_LiveEditorModule_FrameLiveTextureEditor *pDesc=(PX_LiveEditorModule_FrameLiveTextureEditor *)pObject->pObject;
	px_float x,y;

	x=PX_Object_Event_GetCursorX(e);
	y=PX_Object_Event_GetCursorY(e);

	x-=pDesc->pLiveFramework->refer_x;
	y-=pDesc->pLiveFramework->refer_y;


	PX_LiveEditorModule_FrameLiveTextureEditorOnCursorDown_selectMode(pObject,x,y);
}

px_void PX_LiveEditorModule_FrameLiveTextureEditorOnIdChanged(PX_Object *pObject,PX_Object_Event e,px_void *ptr)
{
	PX_Object *pDescObject=(PX_Object *)ptr;
	PX_LiveEditorModule_FrameLiveTextureEditor *pdesc=(PX_LiveEditorModule_FrameLiveTextureEditor *)pDescObject->pObject;
	PX_LiveAnimationFramePayload *pPayload;

	pPayload=PX_LiveFrameworkGetCurrentEditAnimationFramePayload(pdesc->pLiveFramework);
	if (pPayload)
	{
		pPayload->mapTexture=PX_LiveFrameworkGetLiveTextureIndexById(pdesc->pLiveFramework,PX_Object_EditGetText(pdesc->edit_textureid));
		PX_LiveFrameworkRunCurrentEditFrame(pdesc->pLiveFramework);
	}


}


PX_Object * PX_LiveEditorModule_FrameLiveTextureEditorInstall(PX_Object *pparent,PX_Runtime *pruntime,PX_FontModule *fm,PX_LiveFramework *pLiveFramework,PX_Json *pLanguageJson)
{
	PX_Object *pObject;
	PX_LiveEditorModule_FrameLiveTextureEditor Desc,*pDesc;
	PX_memset(&Desc,0,sizeof(Desc));
	pObject=PX_ObjectCreateEx(&pruntime->mp_ui,pparent,0,0,0,0,0,0,0,PX_NULL,PX_NULL,PX_NULL,&Desc,sizeof(Desc));
	pDesc=(PX_LiveEditorModule_FrameLiveTextureEditor *)pObject->pObject;
	pDesc->pLiveFramework=pLiveFramework;
	pDesc->pruntime=pruntime;


	pDesc->window_widget=PX_Object_WidgetCreate(&pruntime->mp_ui,pObject,pruntime->surface_width-290,480,256,80,"",PX_NULL);
	if(!pDesc->window_widget)return PX_FALSE;
	PX_Object_WidgetShowHideCloseButton(pDesc->window_widget,PX_FALSE);

	PX_Object_LabelCreate(&pruntime->mp_ui,pDesc->window_widget,16,16,128,24,PX_JsonGetString(pLanguageJson,"livetextureeditor.textureid"),fm,PX_OBJECT_UI_DEFAULT_FONTCOLOR);
	pDesc->edit_textureid=PX_Object_EditCreate(&pruntime->mp_ui,pDesc->window_widget,80,16,150,24,fm);
	PX_Object_EditSetMaxTextLength(pDesc->edit_textureid,16);
	PX_ObjectRegisterEvent(pDesc->edit_textureid,PX_OBJECT_EVENT_VALUECHANGED,PX_LiveEditorModule_FrameLiveTextureEditorOnIdChanged,pObject);
	pDesc->window_widget->Visible=PX_FALSE;

	PX_ObjectRegisterEvent(pObject,PX_OBJECT_EVENT_CURSORDOWN,PX_LiveEditorModule_FrameLiveTextureEditorOnCursorDown,PX_NULL);
	PX_ObjectRegisterEvent(pObject,PX_OBJECT_EVENT_CURSORRDOWN,PX_LiveEditorModule_FrameLiveTextureEditorOnCursorRDown,PX_NULL);

	return pObject;
}

px_void PX_LiveEditorModule_FrameLiveTextureEditorUninstall(PX_Object *pObject)
{
	PX_ObjectDelete(pObject);
}

px_void PX_LiveEditorModule_FrameLiveTextureEditorEnable(PX_Object *pObject)
{
	PX_LiveEditorModule_FrameLiveTextureEditor *pDesc=(PX_LiveEditorModule_FrameLiveTextureEditor *)pObject->pObject;
	pDesc->pLiveFramework->showKeypoint=PX_TRUE;
	pDesc->pLiveFramework->currentEditLayerIndex=-1;
	pDesc->pLiveFramework->currentEditVertexIndex=-1;
	pDesc->pLiveFramework->showFocusLayer=PX_TRUE;
	pObject->Enabled=PX_TRUE;
	pObject->Visible=PX_TRUE;	
}

px_void PX_LiveEditorModule_FrameLiveTextureEditorDisable(PX_Object *pObject)
{
	PX_LiveEditorModule_FrameLiveTextureEditor *pDesc=(PX_LiveEditorModule_FrameLiveTextureEditor *)pObject->pObject;
	pObject->Enabled=PX_FALSE;
	pObject->Visible=PX_FALSE;
}