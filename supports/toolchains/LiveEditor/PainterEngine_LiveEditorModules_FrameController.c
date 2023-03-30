#include "PainterEngine_LiveEditorModules_FrameController.h"

px_void PX_LiveEditorModule_FrameController_FrameOnButtonNewFrame(PX_Object *pObject,PX_Object_Event e,px_void *ptr)
{
	PX_Object *pFrameControllerObject=(PX_Object *)ptr;
	PX_LiveEditorModule_FrameController *pFrameController=(PX_LiveEditorModule_FrameController *)pFrameControllerObject->pObject;

	PX_ObjectExecuteEvent(pFrameControllerObject->pParent,PX_OBJECT_BUILD_EVENT(PX_FRAMECONTROLLERMODULE_EVENT_NEWFRAME));
}

px_void PX_LiveEditorModule_FrameController_FrameOnButtonCopyFrame(PX_Object *pObject,PX_Object_Event e,px_void *ptr)
{
	PX_Object *pFrameControllerObject=(PX_Object *)ptr;
	PX_LiveEditorModule_FrameController *pFrameController=(PX_LiveEditorModule_FrameController *)pFrameControllerObject->pObject;

	PX_ObjectExecuteEvent(pFrameControllerObject->pParent,PX_OBJECT_BUILD_EVENT(PX_FRAMECONTROLLERMODULE_EVENT_COPYFRAME));
}

px_void PX_LiveEditorModule_FrameController_FrameOnButtonUp(PX_Object *pObject,PX_Object_Event e,px_void *ptr)
{
	PX_Object *pFrameControllerObject=(PX_Object *)ptr;
	PX_LiveEditorModule_FrameController *pFrameController=(PX_LiveEditorModule_FrameController *)pFrameControllerObject->pObject;

	PX_LiveFrameworkCurrentEditMoveFrameUp(pFrameController->pLiveFramework);
}

px_void PX_LiveEditorModule_FrameController_FrameOnButtonDown(PX_Object *pObject,PX_Object_Event e,px_void *ptr)
{
	PX_Object *pFrameControllerObject=(PX_Object *)ptr;
	PX_LiveEditorModule_FrameController *pFrameController=(PX_LiveEditorModule_FrameController *)pFrameControllerObject->pObject;

	PX_LiveFrameworkCurrentEditMoveFrameDown(pFrameController->pLiveFramework);
}


px_void PX_LiveEditorModule_FrameController_FrameOnButtonDeleteFrame(PX_Object *pObject,PX_Object_Event e,px_void *ptr)
{
	PX_Object *pFrameControllerObject=(PX_Object *)ptr;
	PX_LiveEditorModule_FrameController *pFrameController=(PX_LiveEditorModule_FrameController *)pFrameControllerObject->pObject;

	PX_LiveFrameworkDeleteCurrentEditAnimationFrame(pFrameController->pLiveFramework);
}

px_void  PX_LiveEditorModule_FrameControllerFrameListItem_Render(px_surface *psurface,PX_Object *pObject,px_uint elpased)
{
	PX_Object_ListItem *pListItem=PX_Object_GetListItem(pObject);
	PX_LiveAnimationFrameHeader *pFrame;
	px_float x,y;
	px_float inhertX,inhertY;
	PX_Object *FrameControllerObject=(PX_Object *)pObject->User_ptr;
	PX_LiveEditorModule_FrameController *pFrameController=(PX_LiveEditorModule_FrameController *)FrameControllerObject->pObject;
	pFrame=(PX_LiveAnimationFrameHeader *)(*(px_void **)pListItem->pdata);


	PX_ObjectGetInheritXY(pObject,&inhertX,&inhertY);

	x=pObject->x+inhertX;
	y=pObject->y+inhertY;

	do 
	{
		static const px_char *pjsonTagContent=PX_NULL;
		px_char content[32]={0};
		if (pjsonTagContent==PX_NULL)
		{
			pjsonTagContent=PX_JsonGetString(pFrameController->pLanguageJson,"framecontroller.clock");
		}
		
		PX_memset(content,0,sizeof(content));
		PX_strcat(content,pFrame->frameid);
		PX_FontModuleDrawText(psurface,pFrameController->fontmodule,(px_int)x+48,(px_int)y+5,PX_ALIGN_LEFTTOP,content,PX_OBJECT_UI_DEFAULT_FONTCOLOR);

		PX_memset(content,0,sizeof(content));
		PX_strcat(content,pjsonTagContent);
		PX_itoa(pFrame->duration_ms,content+PX_strlen(pjsonTagContent),32-PX_strlen(pjsonTagContent),10);
		PX_FontModuleDrawText(psurface,pFrameController->fontmodule,(px_int)x+48,(px_int)y+24,PX_ALIGN_LEFTTOP,content,PX_OBJECT_UI_DEFAULT_FONTCOLOR);

		PX_ShapeRender(psurface,&pFrameController->shape_animationframe,(px_int)x+24,(px_int)y+24,PX_ALIGN_CENTER,PX_OBJECT_UI_DEFAULT_FONTCOLOR);
		
	} while (0);

	PX_GeoDrawLine(psurface,(px_int)(x),(px_int)(y+pObject->Height),(px_int)(x+pObject->Width-1),(px_int)(y+pObject->Height),1,PX_OBJECT_UI_DEFAULT_BORDERCOLOR);
}

px_bool PX_LiveEditorModule_FrameControllerAnimationListItemOnCreate(px_memorypool *mp,PX_Object *ItemObject,px_void *userptr)
{
	
	PX_Object *pLiveControllerObject=(PX_Object *)userptr;
	PX_LiveEditorModule_FrameController *pFrameController=(PX_LiveEditorModule_FrameController *)pLiveControllerObject->pObject;
	ItemObject->Func_ObjectRender=PX_LiveEditorModule_FrameControllerFrameListItem_Render;
	ItemObject->User_ptr=userptr;

	return PX_TRUE;
}


#include "PainterEngine_Resource_play_traw.h"
#include "PainterEngine_Resource_stop_traw.h"
#include "PainterEngine_Resource_translation_traw.h"
#include "PainterEngine_Resource_pointtranslation_traw.h"
#include "PainterEngine_Resource_stretch_traw.h"
#include "PainterEngine_Resource_rotation_traw.h"
#include "PainterEngine_Resource_clock_traw.h"
#include "PainterEngine_Resource_switchtexture_traw.h"
#include "PainterEngine_Resource_trash_traw.h"
#include "PainterEngine_Resource_newframe_traw.h"
#include "PainterEngine_Resource_animationframe_traw.h"
#include "PainterEngine_Resource_up_traw.h"
#include "PainterEngine_Resource_down_traw.h"
#include "PainterEngine_Resource_copy_traw.h"

px_void PX_LiveEditorModule_FrameControllerUpdate(PX_Object *pObject,px_dword elpased)
{
	px_int i,count;
	PX_LiveEditorModule_FrameController *pFrameController=(PX_LiveEditorModule_FrameController *)pObject->pObject;
	PX_LiveFramework *plive=pFrameController->pLiveFramework;
	PX_LiveAnimation *pAnimation;                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                          
	
	pAnimation=PX_LiveFrameworkGetCurrentEditAnimation(plive);
	if (pAnimation!=pFrameController->pLastPerformanceAnimation)
	{
		PX_Object_ListMoveToTop(pFrameController->pFrameList);
		pFrameController->pLastPerformanceAnimation=pAnimation;
	}

	if (pAnimation)
	{
		count=pAnimation->framesMemPtr.size;
		PX_Object_ListClear(pFrameController->pFrameList);
		for (i=0;i<count;i++)
		{
			px_void *pFrameMemories=PX_VECTORAT(px_void *,&pAnimation->framesMemPtr,i);
			PX_Object_ListAdd(pFrameController->pFrameList,pFrameMemories);
		}
		pObject->Visible=PX_TRUE;
		if (PX_LiveFrameworkGetCurrentEditAnimationFrame(plive))
		{
			pFrameController->button_trash->Visible=PX_TRUE;
			pFrameController->button_up->Visible=PX_TRUE;
			pFrameController->button_down->Visible=PX_TRUE;
		}
		else
		{
			pFrameController->button_trash->Visible=PX_FALSE;
			pFrameController->button_up->Visible=PX_FALSE;
			pFrameController->button_down->Visible=PX_FALSE;
		}

		if (PX_Object_ListGetCurrentSelectIndex(pFrameController->pFrameList)!=plive->currentEditFrameIndex)
		{
			PX_Object_ListSetCurrentSelectIndex(pFrameController->pFrameList,plive->currentEditFrameIndex);
		}
	}
	else
	{
		pObject->Visible=PX_FALSE;
	}
	


}

px_void PX_LiveEditorModule_FrameControllerOnListChanged(PX_Object *pObject,PX_Object_Event e,px_void *ptr)
{
	PX_Object *pLiveControllerObject=(PX_Object *)ptr;
	PX_LiveEditorModule_FrameController *pFrameController=(PX_LiveEditorModule_FrameController *)pLiveControllerObject->pObject;

	pFrameController->pLiveFramework->currentEditFrameIndex=PX_Object_ListGetCurrentSelectIndex(pFrameController->pFrameList);
	
	if (pFrameController->pLiveFramework->currentEditFrameIndex<0)
	{
		pFrameController->pLiveFramework->currentEditFrameIndex=-1;
		return;
	}

	if (pFrameController->pLiveFramework->currentEditFrameIndex>=PX_LiveFrameworkGetCurrentEditAnimation(pFrameController->pLiveFramework)->framesMemPtr.size)
	{
		pFrameController->pLiveFramework->currentEditFrameIndex=-1;
		return;
	}

	PX_LiveFrameworkRunCurrentEditFrame(pFrameController->pLiveFramework);
}

PX_Object * PX_LiveEditorModule_FrameControllerInstall(PX_Object *pparent,PX_Runtime *pruntime,PX_FontModule *fm,PX_LiveFramework *pLiveFramework,PX_Json *pLanguageJson)
{
	PX_Object *pObject;
	PX_LiveEditorModule_FrameController FrameController,*pFrameController;
	PX_memset(&FrameController,0,sizeof(PX_LiveEditorModule_FrameController));

	pObject=PX_ObjectCreateEx(&pruntime->mp_ui,pparent,0,0,0,0,0,0,0,PX_LiveEditorModule_FrameControllerUpdate,PX_NULL,PX_NULL,&FrameController,sizeof(FrameController));
	pFrameController=(PX_LiveEditorModule_FrameController *)pObject->pObject;

	pFrameController->pruntime=pruntime;
	pFrameController->pLiveFramework=pLiveFramework;
	pFrameController->fontmodule=fm;
	pFrameController->pLanguageJson=pLanguageJson;
	pFrameController->window_widget=PX_Object_WidgetCreate(&pruntime->mp_ui,pObject,8,96,224,328,"",PX_NULL);
	if(!pFrameController->window_widget)return PX_FALSE;
	PX_Object_WidgetShowHideCloseButton(pFrameController->window_widget,PX_FALSE);

	if(!PX_ShapeCreateFromMemory(&pruntime->mp_resources,(px_void *)translation_traw,sizeof(translation_traw),&pFrameController->shape_translation)) return PX_FALSE;
	if(!PX_ShapeCreateFromMemory(&pruntime->mp_resources,(px_void *)pointtranslation_traw,sizeof(pointtranslation_traw),&pFrameController->shape_pointtranslation)) return PX_FALSE;
	if(!PX_ShapeCreateFromMemory(&pruntime->mp_resources,(px_void *)stretch_traw,sizeof(stretch_traw),&pFrameController->shape_stretch)) return PX_FALSE;
	if(!PX_ShapeCreateFromMemory(&pruntime->mp_resources,(px_void *)rotation_traw,sizeof(rotation_traw),&pFrameController->shape_rotation)) return PX_FALSE;
	if(!PX_ShapeCreateFromMemory(&pruntime->mp_resources,(px_void *)clock_traw,sizeof(clock_traw),&pFrameController->shape_clock)) return PX_FALSE;
	if(!PX_ShapeCreateFromMemory(&pruntime->mp_resources,(px_void *)switchtexture_traw,sizeof(switchtexture_traw),&pFrameController->shape_switchtexture)) return PX_FALSE;
	if(!PX_ShapeCreateFromMemory(&pruntime->mp_resources,(px_void *)trash_traw,sizeof(trash_traw),&pFrameController->shape_trash)) return PX_FALSE;
	if(!PX_ShapeCreateFromMemory(&pruntime->mp_resources,(px_void *)newframe_traw,sizeof(newframe_traw),&pFrameController->shape_newframe)) return PX_FALSE;
	if(!PX_ShapeCreateFromMemory(&pruntime->mp_resources,(px_void *)animationframe_traw,sizeof(animationframe_traw),&pFrameController->shape_animationframe)) return PX_FALSE;
	if(!PX_ShapeCreateFromMemory(&pruntime->mp_resources,(px_void *)copy_traw,sizeof(copy_traw),&pFrameController->shape_copy)) return PX_FALSE;
	if(!PX_ShapeCreateFromMemory(&pruntime->mp_resources,(px_void *)up_traw,sizeof(up_traw),&pFrameController->shape_up)) return PX_FALSE;
	if(!PX_ShapeCreateFromMemory(&pruntime->mp_resources,(px_void *)down_traw,sizeof(down_traw),&pFrameController->shape_down)) return PX_FALSE;


	pFrameController->pFrameList=PX_Object_ListCreate(&pruntime->mp_ui,PX_Object_WidgetGetRoot(pFrameController->window_widget),0,0,
		(px_int)PX_Object_GetWidget(pFrameController->window_widget)->renderTarget.width,
		(px_int)PX_Object_GetWidget(pFrameController->window_widget)->renderTarget.height-1-24,
		48,
		PX_LiveEditorModule_FrameControllerAnimationListItemOnCreate,
		pObject
		);



	pFrameController->button_newframe=PX_Object_PushButtonCreate(&pruntime->mp_ui,(pFrameController->window_widget),24*0+1,PX_Object_WidgetGetRenderTargetHeight(pFrameController->window_widget)-24,24,24,"",fm);
	PX_Object_PushButtonSetShape(pFrameController->button_newframe,&pFrameController->shape_newframe);

	pFrameController->button_copyframe=PX_Object_PushButtonCreate(&pruntime->mp_ui,(pFrameController->window_widget),24*1+1,PX_Object_WidgetGetRenderTargetHeight(pFrameController->window_widget)-24,24,24,"",fm);
	PX_Object_PushButtonSetShape(pFrameController->button_copyframe,&pFrameController->shape_copy);

	pFrameController->button_up=PX_Object_PushButtonCreate(&pruntime->mp_ui,(pFrameController->window_widget),24*2+1,PX_Object_WidgetGetRenderTargetHeight(pFrameController->window_widget)-24,24,24,"",fm);
	PX_Object_PushButtonSetShape(pFrameController->button_up,&pFrameController->shape_up);

	pFrameController->button_down=PX_Object_PushButtonCreate(&pruntime->mp_ui,(pFrameController->window_widget),24*3+1,PX_Object_WidgetGetRenderTargetHeight(pFrameController->window_widget)-24,24,24,"",fm);
	PX_Object_PushButtonSetShape(pFrameController->button_down,&pFrameController->shape_down);

	pFrameController->button_trash=PX_Object_PushButtonCreate(&pruntime->mp_ui,(pFrameController->window_widget),24*4+1,PX_Object_WidgetGetRenderTargetHeight(pFrameController->window_widget)-24,24,24,"",fm);
	PX_Object_PushButtonSetShape(pFrameController->button_trash,&pFrameController->shape_trash);


	PX_ObjectRegisterEvent(pFrameController->button_newframe,PX_OBJECT_EVENT_EXECUTE,PX_LiveEditorModule_FrameController_FrameOnButtonNewFrame,pObject);
	PX_ObjectRegisterEvent(pFrameController->button_copyframe,PX_OBJECT_EVENT_EXECUTE,PX_LiveEditorModule_FrameController_FrameOnButtonCopyFrame,pObject);
	PX_ObjectRegisterEvent(pFrameController->button_up,PX_OBJECT_EVENT_EXECUTE,PX_LiveEditorModule_FrameController_FrameOnButtonUp,pObject);
	PX_ObjectRegisterEvent(pFrameController->button_down,PX_OBJECT_EVENT_EXECUTE,PX_LiveEditorModule_FrameController_FrameOnButtonDown,pObject);

	PX_ObjectRegisterEvent(pFrameController->button_trash,PX_OBJECT_EVENT_EXECUTE,PX_LiveEditorModule_FrameController_FrameOnButtonDeleteFrame,pObject);
	PX_ObjectRegisterEvent(pFrameController->pFrameList,PX_OBJECT_EVENT_VALUECHANGED,PX_LiveEditorModule_FrameControllerOnListChanged,pObject);
	return pObject;
}

px_void PX_LiveEditorModule_FrameControllerUninstall(PX_Object *pObject)
{
	PX_LiveEditorModule_FrameController *pdesc=(PX_LiveEditorModule_FrameController *)pObject->pObject;
	PX_ShapeFree(&pdesc->shape_pointtranslation);
	PX_ShapeFree(&pdesc->shape_rotation);
	PX_ShapeFree(&pdesc->shape_stretch);
	PX_ShapeFree(&pdesc->shape_translation);
	PX_ShapeFree(&pdesc->shape_switchtexture);
	PX_ShapeFree(&pdesc->shape_clock);
	PX_ObjectDelete(pObject);
}

px_void PX_LiveEditorModule_FrameControllerEnable(PX_Object *pObject)
{
	PX_LiveEditorModule_FrameController *pdesc=(PX_LiveEditorModule_FrameController *)pObject->pObject;
	pObject->Enabled=PX_TRUE;
	PX_Object_ListSetCurrentSelectIndex(pdesc->pFrameList,-1);
	pdesc->pLiveFramework->currentEditLayerIndex=-1;

}

px_void PX_LiveEditorModule_FrameControllerDisable(PX_Object *pObject)
{
	PX_LiveEditorModule_FrameController *pdesc=(PX_LiveEditorModule_FrameController *)pObject->pObject;
	pObject->Enabled=PX_FALSE;
	pObject->Visible=PX_FALSE;
	PX_Object_ListSetCurrentSelectIndex(pdesc->pFrameList,-1);
	pdesc->pLiveFramework->currentEditLayerIndex=-1;
}
