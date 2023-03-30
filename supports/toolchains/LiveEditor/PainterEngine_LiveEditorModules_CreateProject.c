#include "PainterEngine_LiveEditorModules_CreateProject.h"

px_void PX_LiveEditorModule_CreateProjectEnable(PX_Object *pObject)
{
	PX_LiveEditorModule_CreateProject *pCreateProject=(PX_LiveEditorModule_CreateProject *)pObject->pObject;
	PX_Object_WidgetShow(pCreateProject->widget);
	pObject->Enabled=PX_TRUE;
	PX_Object_EditSetText(pCreateProject->edit_width,PX_JsonGetString(pCreateProject->pLanguageJson,"createproject.default width"));
	PX_Object_EditSetText(pCreateProject->edit_height,PX_JsonGetString(pCreateProject->pLanguageJson,"createproject.default height"));
	PX_ObjectSetFocus(pObject);
}


px_void PX_LiveEditorModule_CreateProjectDisable(PX_Object *pObject)
{
	pObject->Enabled=PX_FALSE;
	PX_ObjectReleaseFocus(pObject);
}

px_void PX_LiveEditorModule_OnButtonConfirm(PX_Object *pObject,PX_Object_Event e,px_void *ptr)
{
	PX_Object *pModuleObject=(PX_Object *)ptr;
	PX_LiveEditorModule_CreateProject *pCreateProject=(PX_LiveEditorModule_CreateProject *)pModuleObject->pObject;
	px_int width,height;

	width=PX_atoi(PX_Object_EditGetText(pCreateProject->edit_width));
	height=PX_atoi(PX_Object_EditGetText(pCreateProject->edit_height));

	if (width>0&&height>0)
	{
		PX_LiveFrameworkFree(pCreateProject->pLiveFramework);
		PX_memset(pCreateProject->pLiveFramework,0,sizeof(PX_LiveFramework));
		PX_LiveFrameworkInitialize(&pCreateProject->pruntime->mp_game,pCreateProject->pLiveFramework,width,height);
		PX_strcpy(pCreateProject->pLiveFramework->id,PX_Object_EditGetText(pCreateProject->edit_name),sizeof(pCreateProject->pLiveFramework->id));
		PX_Object_WidgetHide(pCreateProject->widget);
	}
	PX_ObjectExecuteEvent(pModuleObject->pParent,PX_OBJECT_BUILD_EVENT(PX_LIVEEDITORMODULE_EVENT_CREATEPROJECT_CONFIRM));
}

px_void PX_LiveEditorModule_CreateProjectEditorOnEnter(PX_Object *pObject,PX_Object_Event e,px_void *ptr)
{
	if (e.Event==PX_OBJECT_EVENT_KEYDOWN)
	{
		if (PX_Object_Event_GetKeyDown(e)=='\r')
		{
			PX_LiveEditorModule_OnButtonConfirm(pObject,e,ptr);
		}
	}
}

px_void PX_LiveEditorModule_OnButtonExit(PX_Object *pObject,PX_Object_Event e,px_void *ptr)
{
	PX_Object *pModuleObject=(PX_Object *)ptr;
	PX_LiveEditorModule_CreateProject *pCreateProject=(PX_LiveEditorModule_CreateProject *)pModuleObject->pObject;
	PX_ObjectExecuteEvent(pModuleObject->pParent,PX_OBJECT_BUILD_EVENT(PX_LIVEEDITORMODULE_EVENT_CREATEPROJECT_EXIT));
}

PX_Object * PX_LiveEditorModule_CreateProjectInstall(PX_Object *parent,PX_Runtime *pruntime,PX_FontModule *fm,PX_LiveFramework *pLiveFramework,PX_Json *pLanguageJson)
{
	PX_Object *pObject,*pWidget,*pSubObject;
	PX_LiveEditorModule_CreateProject createproject;
	PX_LiveEditorModule_CreateProject *pCreateProject;
	PX_memset(&createproject,0,sizeof(PX_LiveEditorModule_CreateProject));
	pObject=PX_ObjectCreateEx(&pruntime->mp_ui,parent,0,0,0,0,0,0,0,0,0,0,&createproject,sizeof(createproject));
	pCreateProject=(PX_LiveEditorModule_CreateProject *)pObject->pObject;

	pCreateProject->pruntime=pruntime;
	pCreateProject->fontmodule=fm;
	pCreateProject->pLanguageJson=pLanguageJson;
	pCreateProject->pLiveFramework=pLiveFramework;

	pWidget=PX_Object_WidgetCreate(&pruntime->mp_ui,pObject,pruntime->surface_width/2-128,pruntime->surface_height/2-80,
		256,160,PX_JsonGetString(pLanguageJson,"createproject.createproject"),fm);
	PX_Object_WidgetSetModel(pWidget,PX_TRUE);
	pCreateProject->widget=pWidget;

	pSubObject=PX_Object_LabelCreate(&pruntime->mp_ui,pWidget,5,5,80,24,PX_JsonGetString(pLanguageJson,"createproject.projectname"),fm,PX_OBJECT_UI_DEFAULT_FONTCOLOR);
	PX_Object_LabelSetAlign(pSubObject,PX_ALIGN_RIGHTMID);
	pSubObject=PX_Object_LabelCreate(&pruntime->mp_ui,pWidget,5,5+26*1,80,24,PX_JsonGetString(pLanguageJson,"createproject.width"),fm,PX_OBJECT_UI_DEFAULT_FONTCOLOR);
	PX_Object_LabelSetAlign(pSubObject,PX_ALIGN_RIGHTMID);
	pSubObject=PX_Object_LabelCreate(&pruntime->mp_ui,pWidget,5,5+26*2,80,24,PX_JsonGetString(pLanguageJson,"createproject.height"),fm,PX_OBJECT_UI_DEFAULT_FONTCOLOR);
	PX_Object_LabelSetAlign(pSubObject,PX_ALIGN_RIGHTMID);

	pCreateProject->edit_name=PX_Object_EditCreate(&pruntime->mp_ui,pWidget,100,5,128,24,fm);
	PX_Object_EditSetText(pCreateProject->edit_name,"");
	
	pCreateProject->edit_width=PX_Object_EditCreate(&pruntime->mp_ui,pWidget,100,5+26*1,128,24,fm);
	PX_Object_EditSetText(pCreateProject->edit_width,"512");

	pCreateProject->edit_height=PX_Object_EditCreate(&pruntime->mp_ui,pWidget,100,5+26*2,128,24,fm);
	PX_Object_EditSetText(pCreateProject->edit_height,"512");

	pCreateProject->button_ok=PX_Object_PushButtonCreate(&pruntime->mp_ui,pWidget,200,100,48,24,PX_JsonGetString(pLanguageJson,"createproject.ok"),fm);
	PX_ObjectRegisterEvent(pCreateProject->button_ok,PX_OBJECT_EVENT_EXECUTE,PX_LiveEditorModule_OnButtonConfirm,pObject);
	PX_ObjectRegisterEvent(pWidget,PX_OBJECT_EVENT_CLOSE,PX_LiveEditorModule_OnButtonExit,pObject);
	
	PX_Object_WidgetHide(pWidget);


	return pObject;

}

px_void PX_LiveEditorModule_CreateProjectUnstall(PX_Object *pObject)
{
	PX_ObjectDelete(pObject);
}
