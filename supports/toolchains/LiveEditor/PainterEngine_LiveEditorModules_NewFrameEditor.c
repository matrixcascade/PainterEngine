#include "PainterEngine_LiveEditorModules_NewFrameEditor.h"

px_void PX_LiveEditorModule_NewFrameEditorOnOk(PX_Object *pObject,PX_Object_Event e,px_void *ptr)
{
	PX_Object *pNewFrameEditorObject=(PX_Object *)ptr;
	PX_LiveEditorModule_NewFrameEditor *pNewFrame=(PX_LiveEditorModule_NewFrameEditor *)pNewFrameEditorObject->pObject;
	
	PX_LiveFrameworkNewEditFrame(pNewFrame->pLiveFramework,PX_Object_EditGetText(pNewFrame->edit_id),pNewFrame->bCopy);
	PX_ObjectExecuteEvent(pNewFrameEditorObject->pParent,PX_OBJECT_BUILD_EVENT(PX_LIVEEDITORMODULE_NEWFRAMEEDITOR_EVENT_EXIT));
}

px_void PX_LiveEditorModule_NewFrameEditorOnEnter(PX_Object *pObject,PX_Object_Event e,px_void *ptr)
{
	if (e.Event==PX_OBJECT_EVENT_KEYDOWN)
	{
		if (PX_Object_Event_GetKeyDown(e)=='\r')
		{
			PX_LiveEditorModule_NewFrameEditorOnOk(pObject,e,ptr);
		}
	}
}
px_void PX_LiveEditorModule_NewFrameEditorOnCancel(PX_Object *pObject,PX_Object_Event e,px_void *ptr)
{
	PX_Object *pNewFrameEditorObject=(PX_Object *)ptr;
	PX_ObjectExecuteEvent(pNewFrameEditorObject->pParent,PX_OBJECT_BUILD_EVENT(PX_LIVEEDITORMODULE_NEWFRAMEEDITOR_EVENT_EXIT));
}

PX_Object * PX_LiveEditorModule_NewFrameEditorInstall(PX_Object *pparent,PX_Runtime *pruntime,PX_FontModule *fm,PX_LiveFramework *pLiveFramework,PX_Json *pLanguageJson)
{
	PX_Object *pObject;
	PX_LiveEditorModule_NewFrameEditor NewFrame,*pNewFrame;
	PX_memset(&NewFrame,0,sizeof(PX_LiveEditorModule_NewFrameEditor));

	pObject=PX_ObjectCreateEx(&pruntime->mp_ui,pparent,0,0,0,0,0,0,0,PX_NULL,PX_NULL,PX_NULL,&NewFrame,sizeof(NewFrame));
	pNewFrame=(PX_LiveEditorModule_NewFrameEditor *)pObject->pObject;

	pNewFrame->pruntime=pruntime;
	pNewFrame->pLiveFramework=pLiveFramework;
	pNewFrame->fontmodule=fm;
	pNewFrame->pLanguageJson=pLanguageJson;
	pNewFrame->window_widget=PX_Object_WidgetCreate(&pruntime->mp_ui,pObject,0,0,256,128,"",PX_NULL);
	if(!pNewFrame->window_widget)return PX_FALSE;
	PX_Object_WidgetShowHideCloseButton(pNewFrame->window_widget,PX_FALSE);
	PX_Object_WidgetSetModel(pNewFrame->window_widget,PX_TRUE);

	PX_Object_LabelCreate(&pruntime->mp_ui,pNewFrame->window_widget,16,16,100,24,PX_JsonGetString(pLanguageJson,"newframeeditor.newframeid"),fm,PX_OBJECT_UI_DEFAULT_FONTCOLOR);
	pNewFrame->edit_id=PX_Object_EditCreate(&pruntime->mp_ui,pNewFrame->window_widget,136,16,100,24,fm);

	pNewFrame->button_ok=PX_Object_PushButtonCreate(&pruntime->mp_ui,pNewFrame->window_widget,PX_Object_WidgetGetRenderTargetWidth(pNewFrame->window_widget)-64*2,
		PX_Object_WidgetGetRenderTargetHeight(pNewFrame->window_widget)-32,
		60,24,"Ok",fm);

	PX_ObjectRegisterEvent(pNewFrame->button_ok,PX_OBJECT_EVENT_EXECUTE,PX_LiveEditorModule_NewFrameEditorOnOk,pObject);

	pNewFrame->button_cancel=PX_Object_PushButtonCreate(&pruntime->mp_ui,pNewFrame->window_widget,PX_Object_WidgetGetRenderTargetWidth(pNewFrame->window_widget)-64,
		PX_Object_WidgetGetRenderTargetHeight(pNewFrame->window_widget)-32,
		60,24,"Cancel",fm);

	PX_ObjectRegisterEvent(pNewFrame->button_cancel,PX_OBJECT_EVENT_EXECUTE,PX_LiveEditorModule_NewFrameEditorOnCancel,pObject);

	PX_ObjectRegisterEvent(pObject,PX_OBJECT_EVENT_KEYDOWN,PX_LiveEditorModule_NewFrameEditorOnEnter,pObject);

	pObject->x=pruntime->surface_width/2-pNewFrame->window_widget->Width/2;
	pObject->y=pruntime->surface_height/2-pNewFrame->window_widget->Height/2;
	return pObject;
}

px_void PX_LiveEditorModule_NewFrameEditorUninstall(PX_Object *pObject)
{
	PX_ObjectDelete(pObject);
}

px_bool PX_LiveEditorModule_NewFrameEditorEnable(PX_Object *pObject,px_bool bCopy)
{
	PX_Object *pNewFrameEditorObject=(PX_Object *)pObject;
	PX_LiveEditorModule_NewFrameEditor *pNewFrame=(PX_LiveEditorModule_NewFrameEditor *)pNewFrameEditorObject->pObject;

	if (pNewFrame->pLiveFramework->currentEditAnimationIndex>=0&&pNewFrame->pLiveFramework->currentEditAnimationIndex<pNewFrame->pLiveFramework->liveAnimations.size)
	{
		pObject->Visible=PX_TRUE;
		pNewFrame->bCopy=bCopy;
		PX_ObjectSetFocus(pObject);
	}

	
	return PX_FALSE;
	
}

px_void PX_LiveEditorModule_NewFrameEditorDisable(PX_Object *pObject)
{
	pObject->Visible=PX_FALSE;
	PX_ObjectClearFocus(pObject);
}
