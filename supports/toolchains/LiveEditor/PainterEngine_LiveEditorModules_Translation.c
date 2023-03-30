#include "PainterEngine_LiveEditorModules_Translation.h"



px_void PX_LiveEditorModule_TranslationOnCursorDown(PX_Object *pObject,PX_Object_Event e,px_void *ptr)
{
	PX_LiveEditorModule_Translation *pTranslation=(PX_LiveEditorModule_Translation *)pObject->pObject;
	px_float x,y,livex,livey,livewidth,liveheight;
	x=PX_Object_Event_GetCursorX(e);
	y=PX_Object_Event_GetCursorY(e);
	livex=pTranslation->pLiveFramework->refer_x;
	livey=pTranslation->pLiveFramework->refer_y;
	livewidth=(px_float)pTranslation->pLiveFramework->width;
	liveheight=(px_float)pTranslation->pLiveFramework->height;

	if(PX_isXYInRegion(x,y,livex,livey,livewidth,liveheight))
	{
		pTranslation->cursor_last_x=x;
		pTranslation->cursor_last_y=y;
		pTranslation->bSelect=PX_TRUE;
		pTranslation->pLiveFramework->showRange=PX_TRUE;
		PX_ObjectSetFocus(pObject);
	}
	else
	{
		pTranslation->bSelect=PX_FALSE;
		pTranslation->pLiveFramework->showRange=PX_FALSE;
		PX_ObjectReleaseFocus(pObject);
	}
}


px_void PX_LiveEditorModule_TranslationOnCursorDrag(PX_Object *pObject,PX_Object_Event e,px_void *ptr)
{
	px_float x,y;
	PX_LiveEditorModule_Translation *pTranslation=(PX_LiveEditorModule_Translation *)pObject->pObject;
	if (!pTranslation->bSelect)
	{
		return;
	}

	x=PX_Object_Event_GetCursorX(e);
	y=PX_Object_Event_GetCursorY(e);

	pTranslation->pLiveFramework->refer_x+=x-pTranslation->cursor_last_x;
	pTranslation->pLiveFramework->refer_y+=y-pTranslation->cursor_last_y;

	pTranslation->cursor_last_x=x;
	pTranslation->cursor_last_y=y;

}


PX_Object * PX_LiveEditorModule_TranslationInstall(PX_Object *pparent,PX_Runtime *pruntime,PX_FontModule *fm,PX_LiveFramework *pLiveFramework,PX_Json *pLanguageJson)
{
	PX_Object *pObject;
	PX_LiveEditorModule_Translation translation,*ptranslation;
	PX_memset(&translation,0,sizeof(translation));
	pObject=PX_ObjectCreateEx(&pruntime->mp_ui,pparent,0,0,0,0,0,0,0,PX_NULL,PX_NULL,PX_NULL,&translation,sizeof(translation));
	ptranslation=(PX_LiveEditorModule_Translation *)pObject->pObject;
	ptranslation->pLiveFramework=pLiveFramework;
	ptranslation->pruntime=pruntime;

	PX_ObjectRegisterEvent(pObject,PX_OBJECT_EVENT_CURSORDOWN,PX_LiveEditorModule_TranslationOnCursorDown,PX_NULL);
	PX_ObjectRegisterEvent(pObject,PX_OBJECT_EVENT_CURSORDRAG,PX_LiveEditorModule_TranslationOnCursorDrag,PX_NULL);
	return pObject;
}

px_void PX_LiveEditorModule_TranslationUninstall(PX_Object *pObject)
{
	PX_ObjectDelete(pObject);
}

px_void PX_LiveEditorModule_TranslationEnable(PX_Object *pObject)
{
	pObject->Enabled=PX_TRUE;
	pObject->Visible=PX_TRUE;
}

px_void PX_LiveEditorModule_TranslationDisable(PX_Object *pObject)
{
	pObject->Enabled=PX_FALSE;
	pObject->Visible=PX_FALSE;
}