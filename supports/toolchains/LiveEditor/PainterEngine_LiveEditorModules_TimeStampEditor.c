#include "PainterEngine_LiveEditorModules_TimeStampEditor.h"

px_void PX_LiveEditorModule_TimeStampEditorOnOk(PX_Object *pObject,PX_Object_Event e,px_void *ptr)
{
	PX_Object *pTimeStampEditorObject=(PX_Object *)ptr;
	PX_LiveEditorModule_TimeStampEditor *pTimeStamp=(PX_LiveEditorModule_TimeStampEditor *)pTimeStampEditorObject->pObject;
	
	if (pTimeStamp->pLiveFramework->currentEditAnimationIndex>=0&&pTimeStamp->pLiveFramework->currentEditAnimationIndex<pTimeStamp->pLiveFramework->liveAnimations.size)
	{
		PX_LiveAnimation *pLiveAnimation=PX_VECTORAT(PX_LiveAnimation,&pTimeStamp->pLiveFramework->liveAnimations,pTimeStamp->pLiveFramework->currentEditAnimationIndex);
		if (pTimeStamp->pLiveFramework->currentEditFrameIndex>=0&&pTimeStamp->pLiveFramework->currentEditFrameIndex <pLiveAnimation->framesMemPtr.size)
		{
			PX_LiveAnimationFrameHeader *pFrame;
			px_void *memptr=*PX_VECTORAT(px_void *,&pLiveAnimation->framesMemPtr,pTimeStamp->pLiveFramework->currentEditFrameIndex);
			pFrame=(PX_LiveAnimationFrameHeader *)memptr;
			pFrame->duration_ms=PX_atoi(PX_Object_EditGetText(pTimeStamp->edit_duration));
			
		}
	}
	PX_ObjectExecuteEvent(pTimeStampEditorObject->pParent,PX_OBJECT_BUILD_EVENT(PX_LIVEEDITORMODULE_TIMESTAMPEDITOR_EVENT_EXIT));
}

px_void PX_LiveEditorModule_TimeStampEditorOnEnter(PX_Object *pObject,PX_Object_Event e,px_void *ptr)
{
	if (e.Event==PX_OBJECT_EVENT_KEYDOWN)
	{
		if (PX_Object_Event_GetKeyDown(e)=='\r')
		{
			PX_LiveEditorModule_TimeStampEditorOnOk(pObject,e,ptr);
		}
	}
}

px_void PX_LiveEditorModule_TimeStampEditorOnCancel(PX_Object *pObject,PX_Object_Event e,px_void *ptr)
{
	PX_Object *pTimeStampEditorObject=(PX_Object *)ptr;
	PX_ObjectExecuteEvent(pTimeStampEditorObject->pParent,PX_OBJECT_BUILD_EVENT(PX_LIVEEDITORMODULE_TIMESTAMPEDITOR_EVENT_EXIT));
}

PX_Object * PX_LiveEditorModule_TimeStampEditorInstall(PX_Object *pparent,PX_Runtime *pruntime,PX_FontModule *fm,PX_LiveFramework *pLiveFramework,PX_Json *pLanguageJson)
{
	PX_Object *pObject;
	PX_LiveEditorModule_TimeStampEditor TimeStamp,*pTimeStamp;
	PX_memset(&TimeStamp,0,sizeof(PX_LiveEditorModule_TimeStampEditor));

	pObject=PX_ObjectCreateEx(&pruntime->mp_ui,pparent,0,0,0,0,0,0,0,PX_NULL,PX_NULL,PX_NULL,&TimeStamp,sizeof(TimeStamp));
	pTimeStamp=(PX_LiveEditorModule_TimeStampEditor *)pObject->pObject;

	pTimeStamp->pruntime=pruntime;
	pTimeStamp->pLiveFramework=pLiveFramework;
	pTimeStamp->fontmodule=fm;
	pTimeStamp->pLanguageJson=pLanguageJson;
	pTimeStamp->window_widget=PX_Object_WidgetCreate(&pruntime->mp_ui,pObject,0,0,256,128,"",PX_NULL);
	if(!pTimeStamp->window_widget)return PX_FALSE;
	PX_Object_WidgetShowHideCloseButton(pTimeStamp->window_widget,PX_FALSE);
	PX_Object_WidgetSetModel(pTimeStamp->window_widget,PX_TRUE);

	PX_Object_LabelCreate(&pruntime->mp_ui,pTimeStamp->window_widget,16,16,100,24,PX_JsonGetString(pLanguageJson,"timestampeditor.timeduration"),fm,PX_OBJECT_UI_DEFAULT_FONTCOLOR);
	pTimeStamp->edit_duration=PX_Object_EditCreate(&pruntime->mp_ui,pTimeStamp->window_widget,136,16,100,24,fm);

	pTimeStamp->button_ok=PX_Object_PushButtonCreate(&pruntime->mp_ui,pTimeStamp->window_widget,PX_Object_WidgetGetRenderTargetWidth(pTimeStamp->window_widget)-64*2,
		PX_Object_WidgetGetRenderTargetHeight(pTimeStamp->window_widget)-32,
		60,24,"Ok",fm);

	PX_ObjectRegisterEvent(pTimeStamp->button_ok,PX_OBJECT_EVENT_EXECUTE,PX_LiveEditorModule_TimeStampEditorOnOk,pObject);

	pTimeStamp->button_cancel=PX_Object_PushButtonCreate(&pruntime->mp_ui,pTimeStamp->window_widget,PX_Object_WidgetGetRenderTargetWidth(pTimeStamp->window_widget)-64,
		PX_Object_WidgetGetRenderTargetHeight(pTimeStamp->window_widget)-32,
		60,24,"Cancel",fm);

	PX_ObjectRegisterEvent(pTimeStamp->button_cancel,PX_OBJECT_EVENT_EXECUTE,PX_LiveEditorModule_TimeStampEditorOnCancel,pObject);
	PX_ObjectRegisterEvent(pTimeStamp->button_ok,PX_OBJECT_EVENT_KEYDOWN,PX_LiveEditorModule_TimeStampEditorOnEnter,pObject);

	pObject->x=pruntime->surface_width/2-pTimeStamp->window_widget->Width/2;
	pObject->y=pruntime->surface_height/2-pTimeStamp->window_widget->Height/2;
	return pObject;
}

px_void PX_LiveEditorModule_TimeStampEditorUninstall(PX_Object *pObject)
{
	PX_ObjectDelete(pObject);
}

px_bool PX_LiveEditorModule_TimeStampEditorEnable(PX_Object *pObject)
{
	PX_Object *pTimeStampEditorObject=(PX_Object *)pObject;
	PX_LiveEditorModule_TimeStampEditor *pTimeStamp=(PX_LiveEditorModule_TimeStampEditor *)pTimeStampEditorObject->pObject;

	if (pTimeStamp->pLiveFramework->currentEditAnimationIndex>=0&&pTimeStamp->pLiveFramework->currentEditAnimationIndex<pTimeStamp->pLiveFramework->liveAnimations.size)
	{
		PX_LiveAnimation *pLiveAnimation=PX_VECTORAT(PX_LiveAnimation,&pTimeStamp->pLiveFramework->liveAnimations,pTimeStamp->pLiveFramework->currentEditAnimationIndex);
		if (pTimeStamp->pLiveFramework->currentEditFrameIndex>=0&&pTimeStamp->pLiveFramework->currentEditFrameIndex <pLiveAnimation->framesMemPtr.size)
		{
			PX_LiveAnimationFrameHeader *pFrame;
			px_char content[32]={0};
			px_void *memptr=*PX_VECTORAT(px_void *,&pLiveAnimation->framesMemPtr,pTimeStamp->pLiveFramework->currentEditFrameIndex);
			pFrame=(PX_LiveAnimationFrameHeader *)memptr;
			PX_itoa(pFrame->duration_ms,content,sizeof(content),10);
			PX_Object_EditSetText(pTimeStamp->edit_duration,content);
			pObject->Visible=PX_TRUE;
			PX_ObjectSetFocus(pObject);
			PX_Object_EditSetFocus(pTimeStamp->edit_duration,PX_TRUE);

			pTimeStamp->pLiveFramework->showRootHelperLine=PX_FALSE;
			pTimeStamp->pLiveFramework->currentEditLayerIndex=-1;
			pTimeStamp->pLiveFramework->currentEditVertexIndex=-1;
			pTimeStamp->pLiveFramework->showlinker=PX_FALSE;
			pTimeStamp->pLiveFramework->showKeypoint=PX_FALSE;
			pTimeStamp->pLiveFramework->showFocusLayer=PX_FALSE;

			return PX_TRUE;
		}
	}
	return PX_FALSE;
	
}

px_void PX_LiveEditorModule_TimeStampEditorDisable(PX_Object *pObject)
{
	pObject->Visible=PX_FALSE;
	PX_ObjectClearFocus(pObject);
}
