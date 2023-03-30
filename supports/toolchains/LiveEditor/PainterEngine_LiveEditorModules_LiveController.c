#include "PainterEngine_LiveEditorModules_LiveController.h"


px_void  PX_LiveEditorModule_LiveControllerLayerListItem_Update(PX_Object *pObject,px_uint elpased)
{
	PX_Object_ListItem *pListItem=PX_Object_GetListItem(pObject);
	
}

px_void  PX_LiveEditorModule_LiveControllerResourceListItem_Update(PX_Object *pObject,px_uint elpased)
{
	PX_Object_ListItem *pListItem=PX_Object_GetListItem(pObject);

}

px_void  PX_LiveEditorModule_LiveControllerAnimationListItem_Update(PX_Object *pObject,px_uint elpased)
{
	PX_Object_ListItem *pListItem=PX_Object_GetListItem(pObject);

}

px_void  PX_LiveEditorModule_LiveControllerLayerListItem_Render(px_surface *psurface,PX_Object *pObject,px_uint elpased)
{
	PX_Object_ListItem *pListItem=PX_Object_GetListItem(pObject);
	PX_LiveLayer *pLiveLayer;
	px_float x,y;
	px_float inhertX,inhertY;
	px_texture *pTexture=PX_NULL;
	PX_Object *pLiveControllerObject=(PX_Object *)pObject->User_ptr;
	PX_LiveEditorModule_LiveController *pLayerController=(PX_LiveEditorModule_LiveController *)pLiveControllerObject->pObject;
	PX_LiveTexture *pLiveTexture;

	pLiveLayer=(PX_LiveLayer *)pListItem->pdata;

	if (!pLiveLayer)
	{
		return;
	}

	PX_ObjectGetInheritXY(pObject,&inhertX,&inhertY);

	x=pObject->x+inhertX;
	y=pObject->y+inhertY;

	//draw id
	do 
	{
		px_char content[64];
		PX_sprintf2(content,sizeof(content),"%1  z=%2.1",PX_STRINGFORMAT_STRING(pLiveLayer->id),PX_STRINGFORMAT_FLOAT(pLiveLayer->keyPoint.z));
		PX_FontModuleDrawText(psurface,pLayerController->fontmodule,(px_int)x+68,(px_int)y+8,PX_ALIGN_LEFTTOP,content,PX_OBJECT_UI_DEFAULT_FONTCOLOR);
	} while (0);
	
	pLiveTexture=PX_LiveFrameworkGetLiveTexture(pLayerController->pLiveFramework,pLiveLayer->LinkTextureIndex);
	if (pLiveTexture)
	{
		px_float scale;
		px_float largeSize;
		pTexture=&pLiveTexture->Texture;
		largeSize=pTexture->width>pTexture->height?(px_float)pTexture->width:(px_float)pTexture->height;
		scale=48.f/largeSize;
		PX_TextureRenderEx(psurface,pTexture,(px_int)x+32,(px_int)y+32,PX_ALIGN_CENTER,PX_NULL,scale,0);
		
	}
	
	PX_GeoDrawLine(psurface,(px_int)(x),(px_int)(y+pObject->Height),(px_int)(x+pObject->Width-1),(px_int)(y+pObject->Height),1,PX_OBJECT_UI_DEFAULT_BORDERCOLOR);
}

px_void  PX_LiveEditorModule_LiveControllerResourceListItem_Render(px_surface *psurface,PX_Object *pObject,px_uint elpased)
{
	PX_Object_ListItem *pListItem=PX_Object_GetListItem(pObject);
	PX_LiveTexture *pLiveTexture;
	px_float x,y;
	px_float inhertX,inhertY;
	px_texture *pTexture=PX_NULL;
	PX_Object *pLiveControllerObject=(PX_Object *)pObject->User_ptr;
	PX_LiveEditorModule_LiveController *pLayerController=(PX_LiveEditorModule_LiveController *)pLiveControllerObject->pObject;

	pLiveTexture=(PX_LiveTexture *)pListItem->pdata;

	if (!pLiveTexture)
	{
		return;
	}

	PX_ObjectGetInheritXY(pObject,&inhertX,&inhertY);

	x=pObject->x+inhertX;
	y=pObject->y+inhertY;

	//draw id
	do 
	{
		px_char content[64];
		PX_sprintf1(content,sizeof(content),"%1",PX_STRINGFORMAT_STRING(pLiveTexture->id));
		PX_FontModuleDrawText(psurface,pLayerController->fontmodule,(px_int)x+68,(px_int)y+6,PX_ALIGN_LEFTTOP,content,PX_OBJECT_UI_DEFAULT_FONTCOLOR);
	} while (0);


	if (pLiveTexture->Texture.surfaceBuffer)
	{
		px_float scale;
		px_float largeSize;
		pTexture=&pLiveTexture->Texture;
		largeSize=pTexture->width>pTexture->height?(px_float)pTexture->width:(px_float)pTexture->height;
		scale=48.f/largeSize;
		PX_TextureRenderEx(psurface,pTexture,(px_int)x+32,(px_int)y+32,PX_ALIGN_CENTER,PX_NULL,scale,0);

	}

	PX_GeoDrawLine(psurface,(px_int)(x),(px_int)(y+pObject->Height),(px_int)(x+pObject->Width-1),(px_int)(y+pObject->Height),1,PX_OBJECT_UI_DEFAULT_BORDERCOLOR);
}

px_void  PX_LiveEditorModule_LiveControllerAnimationListItem_Render(px_surface *psurface,PX_Object *pObject,px_uint elpased)
{
	PX_Object_ListItem *pListItem=PX_Object_GetListItem(pObject);
	PX_LiveAnimation *pLiveAnimation;
	px_float x,y;
	px_float inhertX,inhertY;
	px_texture *pTexture=PX_NULL;
	PX_Object *pLiveControllerObject=(PX_Object *)pObject->User_ptr;
	PX_LiveEditorModule_LiveController *pLayerController=(PX_LiveEditorModule_LiveController *)pLiveControllerObject->pObject;

	pLiveAnimation=(PX_LiveAnimation *)pListItem->pdata;

	
	PX_ObjectGetInheritXY(pObject,&inhertX,&inhertY);

	x=pObject->x+inhertX;
	y=pObject->y+inhertY;

	//draw ico
	PX_ShapeRender(psurface,&pLayerController->shape_animation,(px_int)x+5,(px_int)y+5,PX_ALIGN_LEFTTOP,PX_OBJECT_UI_DEFAULT_FONTCOLOR);


	//draw id
	do 
	{
		px_char content[64];
		PX_sprintf1(content,sizeof(content),"%1",PX_STRINGFORMAT_STRING(pLiveAnimation->id));
		PX_FontModuleDrawText(psurface,pLayerController->fontmodule,(px_int)x+36,(px_int)y+8,PX_ALIGN_LEFTTOP,content,PX_OBJECT_UI_DEFAULT_FONTCOLOR);
	} while (0);

	do 
	{
		px_char content[64];
		PX_sprintf1(content,sizeof(content),"Frames:%1",PX_STRINGFORMAT_INT(pLiveAnimation->framesMemPtr.size));
		PX_FontModuleDrawText(psurface,pLayerController->fontmodule,(px_int)x+6,(px_int)y+36,PX_ALIGN_LEFTTOP,content,PX_OBJECT_UI_DEFAULT_FONTCOLOR);
	} while (0);

	

	PX_GeoDrawLine(psurface,(px_int)(x),(px_int)(y+pObject->Height),(px_int)(x+pObject->Width-1),(px_int)(y+pObject->Height),1,PX_OBJECT_UI_DEFAULT_BORDERCOLOR);
}

px_void PX_LiveEditorModule_LiveControllerLayerListItem_OnLinkIdConfirm(PX_Object *pmessagebox,PX_Object_Event e,px_void * ptr)
{
	PX_Object *pListItemObject=(PX_Object *)ptr;
	PX_Object *pLiveControllerObject=(PX_Object *)pListItemObject->User_ptr;
	PX_LiveEditorModule_LiveController *pLiveController=(PX_LiveEditorModule_LiveController *)pLiveControllerObject->pObject;
	PX_LiveLayer *pLayer=(PX_LiveLayer *)PX_Object_GetListItem(pListItemObject)->pdata;
	px_int index;
	PX_LiveTexture *pLiveTexture;
	px_char *id;

	id=PX_Object_MessageBoxGetInput(pmessagebox);

	if (id[0]==0)
	{
		pLayer->LinkTextureIndex=-1;
		pLayer->RenderTextureIndex=-1;
	}
	else
	{
		index=PX_LiveFrameworkGetLiveTextureIndexById(pLiveController->pLiveFramework,id);
		if (index==-1)
		{
			PX_Object_MessageBoxAlertOk(pLiveController->messagebox,PX_JsonGetString(pLiveController->pLanguageJson,"livecontroller.texture not found"),0,0);
			return;
		}
		pLayer->LinkTextureIndex=index;
		pLayer->RenderTextureIndex=index;
		pLiveTexture=PX_LiveFrameworkGetLiveTexture(pLiveController->pLiveFramework,index);
		pLayer->keyPoint.x=pLiveTexture->textureOffsetX+pLiveTexture->Texture.width/2.0f;
		pLayer->keyPoint.y=pLiveTexture->textureOffsetY+pLiveTexture->Texture.height/2.0f;
		PX_LiveFrameworkUpdateLayerSourceVerticesUV(pLiveController->pLiveFramework,pLayer);
	}

	

}

px_void PX_LiveEditorModule_LiveControllerLayerListItem_OnLinkButton(PX_Object *pObject,PX_Object_Event e,px_void *ptr)
{
	PX_Object *pListItemObject=(PX_Object *)ptr;

	PX_Object *pLiveControllerObject=(PX_Object *)pListItemObject->User_ptr;
	PX_LiveEditorModule_LiveController *pLiveController=(PX_LiveEditorModule_LiveController *)pLiveControllerObject->pObject;

	PX_LiveLayer *pLayer=(PX_LiveLayer *)PX_Object_GetListItem(pListItemObject)->pdata;
	if (pLayer)
	{
		PX_Object_MessageBoxInputBox(pLiveController->messagebox,PX_JsonGetString(pLiveController->pLanguageJson,"livecontroller.link texture id"),PX_LiveEditorModule_LiveControllerLayerListItem_OnLinkIdConfirm,ptr,PX_NULL,PX_NULL);
	}

}


px_void PX_LiveEditorModule_LiveControllerLayerListItem_OnZConfirm(PX_Object *pmessagebox,PX_Object_Event e,px_void * ptr)
{
	PX_Object *pListItemObject=(PX_Object *)ptr;
	PX_Object *pLiveControllerObject=(PX_Object *)pListItemObject->User_ptr;
	PX_LiveEditorModule_LiveController *pLiveController=(PX_LiveEditorModule_LiveController *)pLiveControllerObject->pObject;
	PX_LiveLayer *pLayer=(PX_LiveLayer *)PX_Object_GetListItem(pListItemObject)->pdata;
	pLayer->keyPoint.z= PX_atof(PX_Object_MessageBoxGetInput(pmessagebox));
	if (pLayer->keyPoint.z<1)
	{
		PX_Object_MessageBoxAlertOk(pmessagebox,PX_JsonGetString(pLiveController->pLanguageJson,"livecontroller.z should not less than 1"),PX_NULL,PX_NULL);
		pLayer->keyPoint.z=1;
	}

}

px_void PX_LiveEditorModule_LiveControllerLayerListItem_OnZButton(PX_Object *pObject,PX_Object_Event e,px_void *ptr)
{
	PX_Object *pListItemObject=(PX_Object *)ptr;
	PX_Object *pLiveControllerObject=(PX_Object *)pListItemObject->User_ptr;
	PX_LiveEditorModule_LiveController *pLiveController=(PX_LiveEditorModule_LiveController *)pLiveControllerObject->pObject;
	PX_LiveLayer *pLayer=(PX_LiveLayer *)PX_Object_GetListItem(pListItemObject)->pdata;
	if (pLayer)
	{
		PX_Object_MessageBoxInputBox(pLiveController->messagebox,PX_JsonGetString(pLiveController->pLanguageJson,"livecontroller.set z"),PX_LiveEditorModule_LiveControllerLayerListItem_OnZConfirm,ptr,PX_NULL,PX_NULL);
	}
	
}

px_void PX_LiveEditorModule_LiveControllerLayerListItem_OnViewButton(PX_Object *pObject,PX_Object_Event e,px_void *ptr)
{
	PX_Object *pListItemObject=(PX_Object *)ptr;
	PX_Object *pLiveControllerObject=(PX_Object *)pListItemObject->User_ptr;
	PX_LiveEditorModule_LiveController *pLiveController=(PX_LiveEditorModule_LiveController *)pLiveControllerObject->pObject;
	PX_LiveLayer *pLayer=(PX_LiveLayer *)PX_Object_GetListItem(pListItemObject)->pdata;
	if (pLayer)
	{
		pLayer->visible=!pLayer->visible;
	}
}

px_void PX_LiveEditorModule_LiveControllerResourceListItem_OnTrashButton(PX_Object *pObject,PX_Object_Event e,px_void *ptr)
{
	PX_Object *pListItemObject=(PX_Object *)ptr;
	PX_Object *pLiveControllerObject=(PX_Object *)pListItemObject->User_ptr;
	PX_LiveEditorModule_LiveController *pLiveController=(PX_LiveEditorModule_LiveController *)pLiveControllerObject->pObject;
	PX_LiveTexture *pTexture=(PX_LiveTexture *)PX_Object_GetListItem(pListItemObject)->pdata;
	PX_LiveFrameworkDeleteLiveTextureById(pLiveController->pLiveFramework,pTexture->id);
}


px_void PX_LiveEditorModule_LiveControllerAnimationListItem_OnTrashButton(PX_Object *pObject,PX_Object_Event e,px_void *ptr)
{
	PX_Object *pListItemObject=(PX_Object *)ptr;
	PX_Object *pLiveControllerObject=(PX_Object *)pListItemObject->User_ptr;
	PX_LiveEditorModule_LiveController *pLiveController=(PX_LiveEditorModule_LiveController *)pLiveControllerObject->pObject;
	PX_LiveAnimation *pAnimation=(PX_LiveAnimation *)PX_Object_GetListItem(pListItemObject)->pdata;
	PX_LiveFrameworkDeleteLiveAnimationById(pLiveController->pLiveFramework,pAnimation->id);
}

px_void PX_LiveEditorModule_LiveControllerLayerListItemOnValueChanged(PX_Object *pObject,PX_Object_Event e,px_void *ptr)
{
	PX_LiveEditorModule_LiveController *pLayerController=(PX_LiveEditorModule_LiveController *)ptr;
	px_int index=PX_Object_ListGetCurrentSelectIndex(pObject);
	if (index>=0&&index<pLayerController->pLiveFramework->layers.size)
	{
		px_int i,editIndex;
		PX_LiveLayer *pLayer,*pSelectLayer;
		pSelectLayer=(PX_LiveLayer *)PX_Object_ListGetItemData(pObject,index);
		for (i=0;i<pLayerController->pLiveFramework->layers.size;i++)
		{
			pLayer=PX_VECTORAT(PX_LiveLayer,&pLayerController->pLiveFramework->layers,i);
			if (pLayer==pSelectLayer)
			{
				editIndex=i;
				break;
			}
		}
		pLayerController->pLiveFramework->currentEditLayerIndex=editIndex;
		pLayerController->pLiveFramework->showFocusLayer=PX_TRUE;
	}
	else
	{
		px_int i;
		PX_LiveLayer *pLayer;
		for (i=0;i<pLayerController->pLiveFramework->layers.size;i++)
		{
			pLayer=PX_VECTORAT(PX_LiveLayer,&pLayerController->pLiveFramework->layers,i);
		}
		pLayerController->pLiveFramework->currentEditLayerIndex=-1;
	}
}

px_void PX_LiveEditorModule_LiveControllerAnimationListItemOnValueChanged(PX_Object *pObject,PX_Object_Event e,px_void *ptr)
{
	PX_LiveEditorModule_LiveController *pLayerController=(PX_LiveEditorModule_LiveController *)ptr;
	px_int index=PX_Object_ListGetCurrentSelectIndex(pObject);
	if (index>=0&&index<pLayerController->pLiveFramework->liveAnimations.size)
	{
		pLayerController->pLiveFramework->currentEditAnimationIndex=index;
		pLayerController->pLiveFramework->currentEditFrameIndex=-1;
		pLayerController->pLiveFramework->currentEditVertexIndex=-1;
	}
	else
	{
		pLayerController->pLiveFramework->currentEditAnimationIndex=-1;
		pLayerController->pLiveFramework->currentEditFrameIndex=-1;
		pLayerController->pLiveFramework->currentEditVertexIndex=-1;
	}
}

px_void PX_LiveEditorModule_LiveControllerLayerListItemOnSetKey(PX_Object *pObject,PX_Object_Event e,px_void *ptr)
{
	PX_Object *pListItemObject=(PX_Object *)ptr;
	PX_Object *pLiveControllerObject=(PX_Object *)pListItemObject->User_ptr;
	PX_ObjectExecuteEvent(pLiveControllerObject->pParent,PX_OBJECT_BUILD_EVENT(PX_LIVEFRAMEWORKMODULES_LIVECONTROLLER_EVENT_SETKEY));
}

px_void PX_LiveEditorModule_LiveControllerLayerListItemOnBuildMesh(PX_Object *pObject,PX_Object_Event e,px_void *ptr)
{
	PX_Object *pListItemObject=(PX_Object *)ptr;
	PX_Object *pLiveControllerObject=(PX_Object *)pListItemObject->User_ptr;
	PX_ObjectExecuteEvent(pLiveControllerObject->pParent,PX_OBJECT_BUILD_EVENT(PX_LIVEFRAMEWORKMODULES_LIVECONTROLLER_EVENT_BUILDMESH));
}

px_void PX_LiveEditorModule_LiveControllerLayerListItemOnTrash(PX_Object *pObject,PX_Object_Event e,px_void *ptr)
{
	PX_Object *pListItemObject=(PX_Object *)ptr;
	PX_Object *pLiveControllerObject=(PX_Object *)pListItemObject->User_ptr;
	PX_LiveEditorModule_LiveController *pLiveController=(PX_LiveEditorModule_LiveController *)pLiveControllerObject->pObject;
	if (pLiveController->pLiveFramework->liveAnimations.size)
	PX_ObjectExecuteEvent(pLiveControllerObject->pParent,PX_OBJECT_BUILD_EVENT_STRING(PX_LIVEEDITORMODULE_LIVECONTROLLER_EVENT_MESSAGE,PX_JsonGetString(pLiveController->pLanguageJson,"livecontroller.layer lock down")));
	else
	PX_LiveFrameworkDeleteLayer(pLiveController->pLiveFramework,pLiveController->pLiveFramework->currentEditLayerIndex);;
}

px_bool PX_LiveEditorModule_LiveControllerLayerListItemOnCreate(px_memorypool *mp,PX_Object *ItemObject,px_void *userptr)
{
	PX_Object *pObject;
	PX_Object *pLiveControllerObject=(PX_Object *)userptr;
	PX_LiveEditorModule_LiveController *pLayerController=(PX_LiveEditorModule_LiveController *)pLiveControllerObject->pObject;
	ItemObject->Func_ObjectRender=PX_LiveEditorModule_LiveControllerLayerListItem_Render;
	ItemObject->Func_ObjectUpdate=PX_LiveEditorModule_LiveControllerLayerListItem_Update;
	ItemObject->User_ptr=userptr;
	//link
	pObject=PX_Object_PushButtonCreate(mp,ItemObject,68+18*0,36,16,16,"",PX_NULL);//z
	PX_Object_PushButtonSetShape(pObject,&pLayerController->shape_link);
	PX_ObjectRegisterEvent(pObject,PX_OBJECT_EVENT_EXECUTE,PX_LiveEditorModule_LiveControllerLayerListItem_OnLinkButton,ItemObject);
	//z
	pObject=PX_Object_PushButtonCreate(mp,ItemObject,68+18*1,36,16,16,"z",PX_NULL);//z
	PX_ObjectRegisterEvent(pObject,PX_OBJECT_EVENT_EXECUTE,PX_LiveEditorModule_LiveControllerLayerListItem_OnZButton,ItemObject);

	pObject=PX_Object_PushButtonCreate(mp,ItemObject,68+18*2,36,16,16,"",PX_NULL);
	PX_Object_PushButtonSetShape(pObject,&pLayerController->shape_eye);
	PX_ObjectRegisterEvent(pObject,PX_OBJECT_EVENT_EXECUTE,PX_LiveEditorModule_LiveControllerLayerListItem_OnViewButton,ItemObject);

	pObject=PX_Object_PushButtonCreate(mp,ItemObject,68+18*3,36,16,16,"",PX_NULL);
	PX_Object_PushButtonSetShape(pObject,&pLayerController->shape_setkey);
	PX_ObjectRegisterEvent(pObject,PX_OBJECT_EVENT_EXECUTE,PX_LiveEditorModule_LiveControllerLayerListItemOnSetKey,ItemObject);

	pObject=PX_Object_PushButtonCreate(mp,ItemObject,68+18*4,36,16,16,"",PX_NULL);
	PX_Object_PushButtonSetShape(pObject,&pLayerController->shape_buildmesh);
	PX_ObjectRegisterEvent(pObject,PX_OBJECT_EVENT_EXECUTE,PX_LiveEditorModule_LiveControllerLayerListItemOnBuildMesh,ItemObject);

	pObject=PX_Object_PushButtonCreate(mp,ItemObject,68+18*5,36,16,16,"",PX_NULL);
	PX_Object_PushButtonSetShape(pObject,&pLayerController->shape_trash);
	PX_ObjectRegisterEvent(pObject,PX_OBJECT_EVENT_EXECUTE,PX_LiveEditorModule_LiveControllerLayerListItemOnTrash,ItemObject);

	return PX_TRUE;
}

px_bool PX_LiveEditorModule_LiveControllerResourceListItemOnCreate(px_memorypool *mp,PX_Object *ItemObject,px_void *userptr)
{
	PX_Object *pObject;
	PX_Object *pLiveControllerObject=(PX_Object *)userptr;
	PX_LiveEditorModule_LiveController *pLayerController=(PX_LiveEditorModule_LiveController *)pLiveControllerObject->pObject;
	ItemObject->Func_ObjectRender=PX_LiveEditorModule_LiveControllerResourceListItem_Render;
	ItemObject->Func_ObjectUpdate=PX_LiveEditorModule_LiveControllerResourceListItem_Update;
	ItemObject->User_ptr=userptr;

	pObject=PX_Object_PushButtonCreate(mp,ItemObject,68+18*0,36,16,16,"",PX_NULL);
	PX_Object_PushButtonSetShape(pObject,&pLayerController->shape_trash);
	PX_ObjectRegisterEvent(pObject,PX_OBJECT_EVENT_EXECUTE,PX_LiveEditorModule_LiveControllerResourceListItem_OnTrashButton,ItemObject);

	return PX_TRUE;
}

px_void PX_LiveEditorModule_LiveControllerAnimationListItem_OnRenameConfirm(PX_Object *pObject,PX_Object_Event e,px_void *ptr)
{
	PX_Object *pListItemObject=(PX_Object *)ptr;
	PX_Object *pLiveControllerObject=(PX_Object *)pListItemObject->User_ptr;
	PX_LiveEditorModule_LiveController *pLiveController=(PX_LiveEditorModule_LiveController *)pLiveControllerObject->pObject;
	PX_LiveAnimation *pAnimation=(PX_LiveAnimation *)PX_Object_GetListItem(pListItemObject)->pdata;

	if (PX_LiveFrameworkGetAnimationById(pLiveController->pLiveFramework,PX_Object_MessageBoxGetInput(pObject)))
	{
		PX_Object_MessageBoxAlertOk(pObject,PX_JsonGetString(pLiveController->pLanguageJson,"livecontroller.repeat layer id"),PX_NULL,PX_NULL);
		return;
	}

	if (PX_strlen(PX_Object_MessageBoxGetInput(pObject))<sizeof(pAnimation->id)-1)
	{
		PX_memset(pAnimation->id,0,sizeof(pAnimation->id));
		PX_strcpy(pAnimation->id,PX_Object_MessageBoxGetInput(pObject),sizeof(pAnimation->id));
	}
}

px_void PX_LiveEditorModule_LiveControllerAnimationListItem_OnRenameButton(PX_Object *pObject,PX_Object_Event e,px_void *ptr)
{
	PX_Object *pListItemObject=(PX_Object *)ptr;
	PX_Object *pLiveControllerObject=(PX_Object *)pListItemObject->User_ptr;
	PX_LiveEditorModule_LiveController *pLiveController=(PX_LiveEditorModule_LiveController *)pLiveControllerObject->pObject;
	PX_LiveAnimation *pAnimation=(PX_LiveAnimation *)PX_Object_GetListItem(pListItemObject)->pdata;
	PX_Object_MessageBoxInputBox(pLiveController->messagebox,PX_JsonGetString(pLiveController->pLanguageJson,"livecontroller.rename animation"),PX_LiveEditorModule_LiveControllerAnimationListItem_OnRenameConfirm,pListItemObject,PX_NULL,PX_NULL);
}


px_bool PX_LiveEditorModule_LiveControllerAnimationListItemOnCreate(px_memorypool *mp,PX_Object *ItemObject,px_void *userptr)
{
	PX_Object *pObject;
	PX_Object *pLiveControllerObject=(PX_Object *)userptr;
	PX_LiveEditorModule_LiveController *pLayerController=(PX_LiveEditorModule_LiveController *)pLiveControllerObject->pObject;
	ItemObject->Func_ObjectRender=PX_LiveEditorModule_LiveControllerAnimationListItem_Render;
	ItemObject->Func_ObjectUpdate=PX_LiveEditorModule_LiveControllerAnimationListItem_Update;
	ItemObject->User_ptr=userptr;

	pObject=PX_Object_PushButtonCreate(mp,ItemObject,80+18*0,36,16,16,"",PX_NULL);
	PX_Object_PushButtonSetShape(pObject,&pLayerController->shape_trash);
	PX_ObjectRegisterEvent(pObject,PX_OBJECT_EVENT_EXECUTE,PX_LiveEditorModule_LiveControllerAnimationListItem_OnTrashButton,ItemObject);

	pObject=PX_Object_PushButtonCreate(mp,ItemObject,80+18*1,36,16,16,"",PX_NULL);
	PX_Object_PushButtonSetShape(pObject,&pLayerController->shape_add);
	PX_ObjectRegisterEvent(pObject,PX_OBJECT_EVENT_EXECUTE,PX_LiveEditorModule_LiveControllerAnimationListItem_OnRenameButton,ItemObject);

	return PX_TRUE;
}



#include "PainterEngine_Resource_eye_no_traw.h"
#include "PainterEngine_Resource_eye_traw.h"
#include "PainterEngine_Resource_bindbone_traw.h"
#include "PainterEngine_Resource_setkey_traw.h"
#include "PainterEngine_Resource_buildmesh_traw.h"
#include "PainterEngine_Resource_trash_traw.h"
#include "PainterEngine_Resource_import_traw.h"
#include "PainterEngine_Resource_new_traw.h"
#include "PainterEngine_Resource_link_traw.h"
#include "PainterEngine_Resource_linkbone_traw.h"
#include "PainterEngine_Resource_animation_traw.h"
#include "PainterEngine_Resource_newframe_traw.h"

px_void PX_LiveEditorModule_LayerControllerUpdate(PX_Object *pObject,px_dword elpased)
{
	px_int i,count;
	PX_LiveEditorModule_LiveController *pLiveController=(PX_LiveEditorModule_LiveController *)pObject->pObject;
	PX_QuickSortAtom sAtom[PX_LIVEFRAMEWORK_MAX_SUPPORT_LAYER];
	PX_LiveFramework *plive=pLiveController->pLiveFramework;

	//////////////////////////////////////////////////////////////////////////
	//layer
	if (pLiveController->pLayerList->Visible)
	{
		for (i=0;i<plive->layers.size;i++)
		{
			PX_LiveLayer *pLayer=PX_VECTORAT(PX_LiveLayer,&plive->layers,i);
			sAtom[i].weight=pLayer->keyPoint.z;
			sAtom[i].pData=pLayer;
			if (i>=PX_LIVEFRAMEWORK_MAX_SUPPORT_LAYER-1)
			{
				break;
			}
		}
		count=i;
		PX_Quicksort_MinToMax(sAtom,0,count-1);

		PX_Object_ListClear(pLiveController->pLayerList);
		for (i=0;i<count;i++)
		{
			PX_LiveLayer *pLayer=(PX_LiveLayer *)sAtom[i].pData;
			PX_Object_ListAdd(pLiveController->pLayerList,pLayer);
		}
	}

	
	//resources
	if (pLiveController->pResourceList->Visible)
	{
		count=pLiveController->pLiveFramework->livetextures.size;
		PX_Object_ListClear(pLiveController->pResourceList);
		for (i=0;i<count;i++)
		{
			PX_LiveTexture *pliveTexture=PX_VECTORAT(PX_LiveTexture,&pLiveController->pLiveFramework->livetextures,i);
			PX_Object_ListAdd(pLiveController->pResourceList,pliveTexture);
		}
	}
	

	//animation
	if (pLiveController->pAnimationList->Visible)
	{
		count=pLiveController->pLiveFramework->liveAnimations.size;
		PX_Object_ListClear(pLiveController->pAnimationList);
		for (i=0;i<count;i++)
		{
			PX_LiveAnimation *pAnimation=PX_VECTORAT(PX_LiveAnimation,&pLiveController->pLiveFramework->liveAnimations,i);
			PX_Object_ListAdd(pLiveController->pAnimationList,pAnimation);
		}
	}


	switch (pLiveController->state)
	{
	case PX_LIVEEDITORMODULE_LIVECONTROLLER_STATE_LAYER:
		PX_Object_PushButtonSetBackgroundColor(pLiveController->button_viewLayer,PX_COLOR(255,16,16,16));
		PX_Object_PushButtonSetBackgroundColor(pLiveController->button_viewResource,PX_OBJECT_UI_DEFAULT_BACKGROUNDCOLOR);
		PX_Object_PushButtonSetBackgroundColor(pLiveController->button_viewAnimation,PX_OBJECT_UI_DEFAULT_BACKGROUNDCOLOR);

		pLiveController->pLayerList->Visible=PX_TRUE;
		pLiveController->pResourceList->Visible=PX_FALSE;
		pLiveController->pAnimationList->Visible=PX_FALSE;
		
		pLiveController->button_newlayer->Visible=PX_TRUE;
		pLiveController->button_linkkey->Visible=PX_TRUE;
		pLiveController->button_importImage->Visible=PX_FALSE;
		pLiveController->button_newanimation->Visible=PX_FALSE;
		break;
	case PX_LIVEEDITORMODULE_LIVECONTROLLER_STATE_RESOURCE:
		PX_Object_PushButtonSetBackgroundColor(pLiveController->button_viewLayer,PX_OBJECT_UI_DEFAULT_BACKGROUNDCOLOR);
		PX_Object_PushButtonSetBackgroundColor(pLiveController->button_viewResource,PX_COLOR(255,16,16,16));
		PX_Object_PushButtonSetBackgroundColor(pLiveController->button_viewAnimation,PX_OBJECT_UI_DEFAULT_BACKGROUNDCOLOR);

		pLiveController->pLayerList->Visible=PX_FALSE;
		pLiveController->pResourceList->Visible=PX_TRUE;
		pLiveController->pAnimationList->Visible=PX_FALSE;

		pLiveController->button_newlayer->Visible=PX_FALSE;
		pLiveController->button_linkkey->Visible=PX_FALSE;
		pLiveController->button_importImage->Visible=PX_TRUE;
		pLiveController->button_newanimation->Visible=PX_FALSE;
		break;
	case PX_LIVEEDITORMODULE_LIVECONTROLLER_STATE_ANIMATION:
		PX_Object_PushButtonSetBackgroundColor(pLiveController->button_viewLayer,PX_OBJECT_UI_DEFAULT_BACKGROUNDCOLOR);
		PX_Object_PushButtonSetBackgroundColor(pLiveController->button_viewResource,PX_OBJECT_UI_DEFAULT_BACKGROUNDCOLOR);
		PX_Object_PushButtonSetBackgroundColor(pLiveController->button_viewAnimation,PX_COLOR(255,16,16,16));


		pLiveController->pLayerList->Visible=PX_FALSE;
		pLiveController->pResourceList->Visible=PX_FALSE;
		pLiveController->pAnimationList->Visible=PX_TRUE;

		pLiveController->button_newlayer->Visible=PX_FALSE;
		pLiveController->button_linkkey->Visible=PX_FALSE;
		pLiveController->button_importImage->Visible=PX_FALSE;
		pLiveController->button_newanimation->Visible=PX_TRUE;
		break;
	}
}


static px_void PX_LiveEditorModule_LiveController_OnStateChanged(PX_Object *pObject,PX_LIVEEDITORMODULE_LIVECONTROLLER_STATE state)
{
	PX_LiveEditorModule_LiveController *pLiveController=(PX_LiveEditorModule_LiveController *)(pObject->pObject);
	pLiveController->state=state;
	PX_ObjectExecuteEvent(pObject->pParent,PX_OBJECT_BUILD_EVENT(PX_LIVEFRAMEWORKMODULES_LIVECONTROLLER_EVENT_STATECHANGE));
}

px_void PX_LiveEditorModule_LiveController_OnButtonViewLayer(PX_Object *pObject,PX_Object_Event e,px_void *ptr)
{
	PX_LiveEditorModule_LiveController *pLiveController=(PX_LiveEditorModule_LiveController *)(((PX_Object *)ptr)->pObject);
	PX_LiveEditorModule_LiveController_OnStateChanged(((PX_Object *)ptr),PX_LIVEEDITORMODULE_LIVECONTROLLER_STATE_LAYER);
	PX_Object_ListSetCurrentSelectIndex(pLiveController->pLayerList,-1);

	pLiveController->pLiveFramework->currentEditAnimationIndex=-1;
	pLiveController->pLiveFramework->currentEditFrameIndex=-1;
	pLiveController->pLiveFramework->currentEditVertexIndex=-1;
	pLiveController->pLiveFramework->currentEditLayerIndex=-1;

	pLiveController->pLiveFramework->showFocusLayer=PX_TRUE;
	pLiveController->pLiveFramework->showKeypoint=PX_FALSE;
	pLiveController->pLiveFramework->showlinker=PX_FALSE;
	pLiveController->pLiveFramework->showRange=PX_FALSE;
	pLiveController->pLiveFramework->showRootHelperLine=PX_FALSE;

	PX_Object_ListClear(pLiveController->pLayerList);
	PX_LiveFrameworkStop(pLiveController->pLiveFramework);
}
px_void PX_LiveEditorModule_LiveController_OnButtonViewResources(PX_Object *pObject,PX_Object_Event e,px_void *ptr)
{
	PX_LiveEditorModule_LiveController *pLiveController=(PX_LiveEditorModule_LiveController *)(((PX_Object *)ptr)->pObject);
	PX_LiveEditorModule_LiveController_OnStateChanged(((PX_Object *)ptr),PX_LIVEEDITORMODULE_LIVECONTROLLER_STATE_RESOURCE);
	PX_Object_ListSetCurrentSelectIndex(pLiveController->pResourceList,-1);
	pLiveController->pLiveFramework->currentEditAnimationIndex=-1;
	pLiveController->pLiveFramework->currentEditFrameIndex=-1;
	pLiveController->pLiveFramework->currentEditVertexIndex=-1;
	pLiveController->pLiveFramework->currentEditLayerIndex=-1;

	pLiveController->pLiveFramework->showFocusLayer=PX_FALSE;
	pLiveController->pLiveFramework->showKeypoint=PX_FALSE;
	pLiveController->pLiveFramework->showlinker=PX_FALSE;
	pLiveController->pLiveFramework->showRange=PX_FALSE;
	pLiveController->pLiveFramework->showRootHelperLine=PX_FALSE;

	PX_Object_ListClear(pLiveController->pResourceList);
	PX_LiveFrameworkStop(pLiveController->pLiveFramework);

}
px_void PX_LiveEditorModule_LiveController_OnButtonViewAnimations(PX_Object *pObject,PX_Object_Event e,px_void *ptr)
{
	PX_LiveEditorModule_LiveController *pLiveController=(PX_LiveEditorModule_LiveController *)(((PX_Object *)ptr)->pObject);
	PX_LiveEditorModule_LiveController_OnStateChanged(((PX_Object *)ptr),PX_LIVEEDITORMODULE_LIVECONTROLLER_STATE_ANIMATION);
	PX_Object_ListSetCurrentSelectIndex(pLiveController->pAnimationList,-1);
	pLiveController->pLiveFramework->currentEditAnimationIndex=-1;
	pLiveController->pLiveFramework->currentEditFrameIndex=-1;
	pLiveController->pLiveFramework->currentEditVertexIndex=-1;
	pLiveController->pLiveFramework->currentEditLayerIndex=-1;

	pLiveController->pLiveFramework->showFocusLayer=PX_FALSE;
	pLiveController->pLiveFramework->showKeypoint=PX_FALSE;
	pLiveController->pLiveFramework->showlinker=PX_FALSE;
	pLiveController->pLiveFramework->showRange=PX_FALSE;
	pLiveController->pLiveFramework->showRootHelperLine=PX_FALSE;
	PX_Object_ListClear(pLiveController->pAnimationList);
	PX_LiveFrameworkStop(pLiveController->pLiveFramework);
}

px_void PX_LiveEditorModule_LiveController_OnLayerIDConfirm(PX_Object *pObject,PX_Object_Event e,px_void *ptr)
{
	PX_LiveEditorModule_LiveController *pLiveController=(PX_LiveEditorModule_LiveController *)(((PX_Object *)ptr)->pObject);
	px_char *id=PX_Object_MessageBoxGetInput(pObject);
	px_int index;
	PX_LiveTexture *pTexture;
	PX_LiveLayer *pLayer;
	if (PX_LiveFrameworkGetLayerById(pLiveController->pLiveFramework,id))
	{
		PX_Object_MessageBoxAlertOk(pObject,PX_JsonGetString(pLiveController->pLanguageJson,"livecontroller.repeat layer id"),PX_NULL,PX_NULL);
		return;
	}
	if(!PX_LiveFrameworkCreateLayer(pLiveController->pLiveFramework,id))
	{
		PX_Object_MessageBoxAlertOk(pObject,PX_JsonGetString(pLiveController->pLanguageJson,"livecontroller.could not create layer"),PX_NULL,PX_NULL);
	}
	pLayer=PX_LiveFrameworkGetLastCreateLayer(pLiveController->pLiveFramework);
	pTexture=PX_LiveFrameworkGetLiveTextureById(pLiveController->pLiveFramework,id);
	if (!pTexture)
	{
		pLayer->LinkTextureIndex=-1;
		return;
	}

	index=PX_LiveFrameworkGetLiveTextureIndexById(pLiveController->pLiveFramework,id);
	pLayer->LinkTextureIndex=index;
	pLayer->keyPoint.x=pTexture->textureOffsetX+pTexture->Texture.width/2.0f;
	pLayer->keyPoint.y=pTexture->textureOffsetY+pTexture->Texture.height/2.0f;

}

px_void PX_LiveEditorModule_LiveController_OnAnimationIDConfirm(PX_Object *pObject,PX_Object_Event e,px_void *ptr)
{
	PX_LiveEditorModule_LiveController *pLiveController=(PX_LiveEditorModule_LiveController *)(((PX_Object *)ptr)->pObject);
	px_char *id=PX_Object_MessageBoxGetInput(pObject);
	if (!id[0])
	{
		PX_Object_MessageBoxAlertOk(pObject,PX_JsonGetString(pLiveController->pLanguageJson,"livecontroller.invalid empty animation id"),PX_NULL,PX_NULL);
		return;
	}

	if (PX_LiveFrameworkGetAnimationById(pLiveController->pLiveFramework,id))
	{
		PX_Object_MessageBoxAlertOk(pObject,PX_JsonGetString(pLiveController->pLanguageJson,"livecontroller.repeat animation id"),PX_NULL,PX_NULL);
		return;
	}
	if(!PX_LiveFrameworkCreateAnimation(pLiveController->pLiveFramework,id))
	{
		PX_Object_MessageBoxAlertOk(pObject,PX_JsonGetString(pLiveController->pLanguageJson,"livecontroller.could not create animation"),PX_NULL,PX_NULL);
	}

}



px_void PX_LiveEditorModule_LiveController_OnButtonNewLayer(PX_Object *pObject,PX_Object_Event e,px_void *ptr)
{

	PX_LiveEditorModule_LiveController *pLiveController=(PX_LiveEditorModule_LiveController *)(((PX_Object *)ptr)->pObject);
	if (pLiveController->pLiveFramework->liveAnimations.size)
	{
		PX_Object_MessageBoxAlertOk(pObject,PX_JsonGetString(pLiveController->pLanguageJson,"livecontroller.layer lock down"),PX_NULL,PX_NULL);
	}
	else
	{
		PX_Object_MessageBoxInputBox(pLiveController->messagebox,PX_JsonGetString(pLiveController->pLanguageJson,"livecontroller.layer id"),PX_LiveEditorModule_LiveController_OnLayerIDConfirm,ptr,PX_NULL,PX_NULL);
		PX_LiveEditorModule_LiveController_OnStateChanged(((PX_Object *)ptr),PX_LIVEEDITORMODULE_LIVECONTROLLER_STATE_LAYER);
	}
	
	
}

px_void PX_LiveEditorModule_LiveController_OnButtonImportImage(PX_Object *pObject,PX_Object_Event e,px_void *ptr)
{
	PX_Object *pControllerObject=(PX_Object *)ptr;

	PX_ObjectExecuteEvent(pControllerObject->pParent,PX_OBJECT_BUILD_EVENT(PX_LIVEFRAMEWORKMODULES_LIVECONTROLLER_EVENT_LOADIMAGE));
}

px_void PX_LiveEditorModule_LiveController_OnButtonLinkKey(PX_Object *pObject,PX_Object_Event e,px_void *ptr)
{
	PX_Object *pControllerObject=(PX_Object *)ptr;

	PX_ObjectExecuteEvent(pControllerObject->pParent,PX_OBJECT_BUILD_EVENT(PX_LIVEFRAMEWORKMODULES_LIVECONTROLLER_EVENT_LINKKEY));
}


px_void PX_LiveEditorModule_LiveController_OnButtonNewLiveAnimation(PX_Object *pObject,PX_Object_Event e,px_void *ptr)
{
	PX_LiveEditorModule_LiveController *pLiveController=(PX_LiveEditorModule_LiveController *)(((PX_Object *)ptr)->pObject);

	PX_Object_MessageBoxInputBox(pLiveController->messagebox,PX_JsonGetString(pLiveController->pLanguageJson,"livecontroller.animation id"),PX_LiveEditorModule_LiveController_OnAnimationIDConfirm,ptr,PX_NULL,PX_NULL);
	PX_LiveEditorModule_LiveController_OnStateChanged(((PX_Object *)ptr),PX_LIVEEDITORMODULE_LIVECONTROLLER_STATE_ANIMATION);

}


PX_Object * PX_LiveEditorModule_LiveControllerInstall(PX_Object *pparent,PX_Runtime *pruntime,PX_FontModule *fm,PX_LiveFramework *pLiveFramework,PX_Json *pLanguageJson)
{
	PX_Object *pObject;
	PX_LiveEditorModule_LiveController liveController,*pLiveController;
	PX_memset(&liveController,0,sizeof(PX_LiveEditorModule_LiveController));

	pObject=PX_ObjectCreateEx(&pruntime->mp_ui,pparent,0,0,0,0,0,0,0,PX_LiveEditorModule_LayerControllerUpdate,PX_NULL,PX_NULL,&liveController,sizeof(liveController));
	pLiveController=(PX_LiveEditorModule_LiveController *)pObject->pObject;

	pLiveController->pruntime=pruntime;
	pLiveController->pLiveFramework=pLiveFramework;
	pLiveController->fontmodule=fm;
	pLiveController->pLanguageJson=pLanguageJson;
	pLiveController->state=PX_LIVEEDITORMODULE_LIVECONTROLLER_STATE_LAYER;
	pLiveController->window_widget=PX_Object_WidgetCreate(&pruntime->mp_ui,pObject,pruntime->surface_width-284,64,256,392,"",PX_NULL);
	if(!pLiveController->window_widget)return PX_FALSE;
	PX_Object_WidgetShowHideCloseButton(pLiveController->window_widget,PX_FALSE);
	if(!PX_ShapeCreateFromMemory(&pruntime->mp_resources,(px_void *)eye_traw,sizeof(eye_traw),&pLiveController->shape_eye)) return PX_FALSE;
	if(!PX_ShapeCreateFromMemory(&pruntime->mp_resources,(px_void *)eye_no_traw,sizeof(eye_no_traw),&pLiveController->shape_eye_no)) return PX_FALSE;
	if(!PX_ShapeCreateFromMemory(&pruntime->mp_resources,(px_void *)bindbone_traw,sizeof(bindbone_traw),&pLiveController->shape_bindbone)) return PX_FALSE;
	if(!PX_ShapeCreateFromMemory(&pruntime->mp_resources,(px_void *)setkey_traw,sizeof(setkey_traw),&pLiveController->shape_setkey)) return PX_FALSE;
	if(!PX_ShapeCreateFromMemory(&pruntime->mp_resources,(px_void *)buildmesh_traw,sizeof(buildmesh_traw),&pLiveController->shape_buildmesh)) return PX_FALSE;
	if(!PX_ShapeCreateFromMemory(&pruntime->mp_resources,(px_void *)new_traw,sizeof(new_traw),&pLiveController->shape_new)) return PX_FALSE;
	if(!PX_ShapeCreateFromMemory(&pruntime->mp_resources,(px_void *)trash_traw,sizeof(trash_traw),&pLiveController->shape_trash)) return PX_FALSE;
	if(!PX_ShapeCreateFromMemory(&pruntime->mp_resources,(px_void *)link_traw,sizeof(link_traw),&pLiveController->shape_link)) return PX_FALSE;
	if(!PX_ShapeCreateFromMemory(&pruntime->mp_resources,(px_void *)import_traw,sizeof(import_traw),&pLiveController->shape_import)) return PX_FALSE;
	if(!PX_ShapeCreateFromMemory(&pruntime->mp_resources,(px_void *)linkkey_traw,sizeof(linkkey_traw),&pLiveController->shape_linkkey)) return PX_FALSE;
	if(!PX_ShapeCreateFromMemory(&pruntime->mp_resources,(px_void *)animation_traw,sizeof(animation_traw),&pLiveController->shape_animation)) return PX_FALSE;
	if(!PX_ShapeCreateFromMemory(&pruntime->mp_resources,(px_void *)newframe_traw,sizeof(newframe_traw),&pLiveController->shape_add)) return PX_FALSE;

	pLiveController->pLayerList=PX_Object_ListCreate(&pruntime->mp_ui,PX_Object_WidgetGetRoot(pLiveController->window_widget),0,24,
		(px_int)PX_Object_GetWidget(pLiveController->window_widget)->renderTarget.width-1,
		(px_int)PX_Object_GetWidget(pLiveController->window_widget)->renderTarget.height-1-48,
		64,
		PX_LiveEditorModule_LiveControllerLayerListItemOnCreate,
		pObject
		);

	pLiveController->pResourceList=PX_Object_ListCreate(&pruntime->mp_ui,PX_Object_WidgetGetRoot(pLiveController->window_widget),0,24,
		(px_int)PX_Object_GetWidget(pLiveController->window_widget)->renderTarget.width-1,
		(px_int)PX_Object_GetWidget(pLiveController->window_widget)->renderTarget.height-1-48,
		64,
		PX_LiveEditorModule_LiveControllerResourceListItemOnCreate,
		pObject
		);

	pLiveController->pAnimationList=PX_Object_ListCreate(&pruntime->mp_ui,PX_Object_WidgetGetRoot(pLiveController->window_widget),0,24,
		(px_int)PX_Object_GetWidget(pLiveController->window_widget)->renderTarget.width-1,
		(px_int)PX_Object_GetWidget(pLiveController->window_widget)->renderTarget.height-1-48,
		64,
		PX_LiveEditorModule_LiveControllerAnimationListItemOnCreate,
		pObject
		);

	pLiveController->messagebox=PX_Object_MessageBoxCreate(&pruntime->mp_ui,pObject,fm);

	PX_ObjectRegisterEvent(pLiveController->pLayerList,PX_OBJECT_EVENT_VALUECHANGED,PX_LiveEditorModule_LiveControllerLayerListItemOnValueChanged,pLiveController);
	PX_ObjectRegisterEvent(pLiveController->pAnimationList,PX_OBJECT_EVENT_VALUECHANGED,PX_LiveEditorModule_LiveControllerAnimationListItemOnValueChanged,pLiveController);
	

	pLiveController->button_viewLayer=PX_Object_PushButtonCreate(&pruntime->mp_ui,PX_Object_WidgetGetRoot(pLiveController->window_widget),64*0+1,0,64,24,PX_JsonGetString(pLanguageJson,"livecontroller.layer"),fm);
	PX_ObjectRegisterEvent(pLiveController->button_viewLayer,PX_OBJECT_EVENT_EXECUTE,PX_LiveEditorModule_LiveController_OnButtonViewLayer,pObject);
	pLiveController->button_viewResource=PX_Object_PushButtonCreate(&pruntime->mp_ui,PX_Object_WidgetGetRoot(pLiveController->window_widget),64*1+1,0,64,24,PX_JsonGetString(pLanguageJson,"livecontroller.resources"),fm);
	PX_ObjectRegisterEvent(pLiveController->button_viewResource,PX_OBJECT_EVENT_EXECUTE,PX_LiveEditorModule_LiveController_OnButtonViewResources,pObject);
	pLiveController->button_viewAnimation=PX_Object_PushButtonCreate(&pruntime->mp_ui,PX_Object_WidgetGetRoot(pLiveController->window_widget),64*2+1,0,64,24,PX_JsonGetString(pLanguageJson,"livecontroller.animations"),fm);
	PX_ObjectRegisterEvent(pLiveController->button_viewAnimation,PX_OBJECT_EVENT_EXECUTE,PX_LiveEditorModule_LiveController_OnButtonViewAnimations,pObject);

	
	pLiveController->button_newlayer=PX_Object_PushButtonCreate(&pruntime->mp_ui,PX_Object_WidgetGetRoot(pLiveController->window_widget),24*0+1,PX_Object_WidgetGetRenderTargetHeight(pLiveController->window_widget)-24,24,24,"",fm);
	PX_Object_PushButtonSetShape(pLiveController->button_newlayer,&pLiveController->shape_new);
	PX_ObjectRegisterEvent(pLiveController->button_newlayer,PX_OBJECT_EVENT_EXECUTE,PX_LiveEditorModule_LiveController_OnButtonNewLayer,pObject);

	pLiveController->button_linkkey=PX_Object_PushButtonCreate(&pruntime->mp_ui,PX_Object_WidgetGetRoot(pLiveController->window_widget),24*1+2,PX_Object_WidgetGetRenderTargetHeight(pLiveController->window_widget)-24,24,24,"",fm);
	PX_Object_PushButtonSetShape(pLiveController->button_linkkey,&pLiveController->shape_linkkey);
	PX_ObjectRegisterEvent(pLiveController->button_linkkey,PX_OBJECT_EVENT_EXECUTE,PX_LiveEditorModule_LiveController_OnButtonLinkKey,pObject);

	pLiveController->button_importImage=PX_Object_PushButtonCreate(&pruntime->mp_ui,PX_Object_WidgetGetRoot(pLiveController->window_widget),24*0+1,PX_Object_WidgetGetRenderTargetHeight(pLiveController->window_widget)-24,24,24,"",fm);
	PX_Object_PushButtonSetShape(pLiveController->button_importImage,&pLiveController->shape_import);
	PX_ObjectRegisterEvent(pLiveController->button_importImage,PX_OBJECT_EVENT_EXECUTE,PX_LiveEditorModule_LiveController_OnButtonImportImage,pObject);

	pLiveController->button_newanimation=PX_Object_PushButtonCreate(&pruntime->mp_ui,PX_Object_WidgetGetRoot(pLiveController->window_widget),24*0+2,PX_Object_WidgetGetRenderTargetHeight(pLiveController->window_widget)-24,24,24,"",fm);
	PX_Object_PushButtonSetShape(pLiveController->button_newanimation,&pLiveController->shape_add);
	PX_ObjectRegisterEvent(pLiveController->button_newanimation,PX_OBJECT_EVENT_EXECUTE,PX_LiveEditorModule_LiveController_OnButtonNewLiveAnimation,pObject);
	return pObject;
}

px_void PX_LiveEditorModule_LiveControllerUninstall(PX_Object *pObject)
{
	PX_LiveEditorModule_LiveController *pdesc=(PX_LiveEditorModule_LiveController *)pObject->pObject;
	PX_ShapeFree(&pdesc->shape_animation);
	PX_ShapeFree(&pdesc->shape_bindbone);
	PX_ShapeFree(&pdesc->shape_buildmesh);
	PX_ShapeFree(&pdesc->shape_eye);
	PX_ShapeFree(&pdesc->shape_eye_no);
	PX_ShapeFree(&pdesc->shape_import);
	PX_ShapeFree(&pdesc->shape_link);
	PX_ShapeFree(&pdesc->shape_linkkey);
	PX_ShapeFree(&pdesc->shape_new);
	PX_ShapeFree(&pdesc->shape_setkey);
	PX_ShapeFree(&pdesc->shape_trash);
	PX_ObjectDelete(pObject);
}

px_void PX_LiveEditorModule_LiveControllerEnable(PX_Object *pObject)
{
	pObject->Enabled=PX_TRUE;
	pObject->Visible=PX_TRUE;
}

px_void PX_LiveEditorModule_LiveControllerDisable(PX_Object *pObject)
{
	pObject->Enabled=PX_FALSE;
	pObject->Visible=PX_FALSE;
}

px_int PX_LiveEditorModule_LiveControllerGetCurrentEditLayer(PX_Object *pObject)
{
	PX_LiveEditorModule_LiveController *pLiveController=(PX_LiveEditorModule_LiveController *)(pObject->pObject);
	return PX_Object_ListGetCurrentSelectIndex(pLiveController->pLayerList);
}

