#include "PX_Object_Menu.h"
static PX_Object_Menu *PX_Object_GetMenu(PX_Object *pMenuObject)
{
	if (pMenuObject->Type==PX_OBJECT_TYPE_MENU)
	{
		return (PX_Object_Menu *)pMenuObject->pObject;
	}
	return PX_NULL;
}
static px_void PX_MenuSubMenuUpdateEx(PX_Object_Menu *pMenu,PX_Object_Menu_Item *pItem,px_int startX,px_int startY)
{
	px_int index=0;
	px_int maxWidth=0,maxHeight=0;
	px_list_node *pNode=PX_NULL;


	for (pNode=PX_ListNodeAt(&pItem->Items,0);pNode;pNode=PX_ListNodeNext(pNode))
	{
		px_int cWidth,cHeight;
		PX_Object_Menu_Item *pSubItem=PX_LIST_NODETDATA(PX_Object_Menu_Item,pNode);
		PX_FontModuleTextGetRenderWidthHeight(pMenu->fontmodule,pSubItem->Text,&cWidth,&cHeight);
		if (cWidth>maxWidth)
		{
			maxWidth=cWidth;
		}
		if (cHeight>maxHeight)
		{
			maxHeight=cHeight;
		}
	}

	maxWidth+=PX_MENU_ITEM_SPACER_SIZE*2+18+16;
	maxHeight+=PX_MENU_ITEM_SPACER_SIZE*2;


	if (maxWidth<pMenu->minimumSize)
	{
		maxWidth=pMenu->minimumSize;
	}

	for (pNode=PX_ListNodeAt(&pItem->Items,0);pNode;pNode=PX_ListNodeNext(pNode))
	{
		PX_Object_Menu_Item *pSubItem=PX_LIST_NODETDATA(PX_Object_Menu_Item,pNode);
		pSubItem->x=startX;
		pSubItem->y=startY+index*maxHeight;
		pSubItem->width=maxWidth;
		pSubItem->height=maxHeight;
		index++;
	}


	for (pNode=PX_ListNodeAt(&pItem->Items,0);pNode;pNode=PX_ListNodeNext(pNode))
	{
		PX_Object_Menu_Item *pSubItem=PX_LIST_NODETDATA(PX_Object_Menu_Item,pNode);
		PX_MenuSubMenuUpdateEx(pMenu,pSubItem,pSubItem->x+pSubItem->width,pSubItem->y);
	}
}
static px_void PX_MenuSubMenuUpdate(px_int x,px_int y,PX_Object_Menu *pMenu)
{
	px_int index=0;
	px_int maxWidth=0,maxHeight=0;
	px_list_node *pNode=PX_NULL;


	for (pNode=PX_ListNodeAt(&pMenu->root.Items,0);pNode;pNode=PX_ListNodeNext(pNode))
	{
		px_int cWidth,cHeight;
		PX_Object_Menu_Item *pSubItem=PX_LIST_NODETDATA(PX_Object_Menu_Item,pNode);
		PX_FontModuleTextGetRenderWidthHeight(pMenu->fontmodule,pSubItem->Text,&cWidth,&cHeight);
		if (cWidth>maxWidth)
		{
			maxWidth=cWidth;
		}
		if (cHeight>maxHeight)
		{
			maxHeight=cHeight;
		}
	}

	maxWidth+=PX_MENU_ITEM_SPACER_SIZE*2+18+16;
	maxHeight+=PX_MENU_ITEM_SPACER_SIZE*2;


	if (maxWidth<pMenu->minimumSize)
	{
		maxWidth=pMenu->minimumSize;
	}

	for (pNode=PX_ListNodeAt(&pMenu->root.Items,0);pNode;pNode=PX_ListNodeNext(pNode))
	{
		PX_Object_Menu_Item *pSubItem=PX_LIST_NODETDATA(PX_Object_Menu_Item,pNode);
		pSubItem->x=x+index*maxWidth;
		pSubItem->y=y;
		pSubItem->width=maxWidth;
		pSubItem->height=maxHeight;
		index++;
	}


	for (pNode=PX_ListNodeAt(&pMenu->root.Items,0);pNode;pNode=PX_ListNodeNext(pNode))
	{
		PX_Object_Menu_Item *pSubItem=PX_LIST_NODETDATA(PX_Object_Menu_Item,pNode);
		PX_MenuSubMenuUpdateEx(pMenu,pSubItem,pSubItem->x,pSubItem->y+pSubItem->height);
	}
}
static px_bool PX_Object_MenuInitialize(px_memorypool *mp,PX_Object_Menu *pMenu,px_int x,int y,px_int width,PX_FontModule *fontmodule)
{
	PX_memset(pMenu,0,sizeof(PX_Object_Menu));
	pMenu->fontmodule=fontmodule;
	pMenu->minimumSize=width;
	pMenu->mp=mp;
	pMenu->activating=PX_FALSE;
	pMenu->backgroundColor=PX_OBJECT_UI_DEFAULT_BACKGROUNDCOLOR;
	pMenu->cursorColor=PX_OBJECT_UI_DEFAULT_CURSORCOLOR;
	pMenu->fontColor=PX_OBJECT_UI_DEFAULT_FONTCOLOR;
	pMenu->disableColor=PX_OBJECT_UI_DISABLE_FONTCOLOR;
	PX_ListInitialize(mp,&pMenu->root.Items);
	pMenu->root.Activated=PX_TRUE;
	return PX_TRUE;
}
typedef enum
{
	PX_MENU_CURSORACTION_MOVE,
	PX_MENU_CURSORACTION_DOWN,
}PX_MENU_CURSORACTION;
static px_void PX_MenuClearParent(PX_Object_Menu *pMenu,PX_Object_Menu_Item *pItem)
{
	px_list_node *pNode=PX_NULL;

	for (pNode=PX_ListNodeAt(&pItem->pParent->Items,0);pNode;pNode=PX_ListNodeNext(pNode))
	{
		PX_Object_Menu_Item *pSubItem=PX_LIST_NODETDATA(PX_Object_Menu_Item,pNode);
		pSubItem->onCursor=PX_FALSE;
		pSubItem->Activated=PX_FALSE;
	}
}
static px_void PX_MenuClearChild(PX_Object_Menu *pMenu,PX_Object_Menu_Item *pItem)
{
	px_list_node *pNode=PX_NULL;

	for (pNode=PX_ListNodeAt(&pItem->Items,0);pNode;pNode=PX_ListNodeNext(pNode))
	{
		PX_Object_Menu_Item *pSubItem=PX_LIST_NODETDATA(PX_Object_Menu_Item,pNode);
		pSubItem->onCursor=PX_FALSE;
		pSubItem->Activated=PX_FALSE;
	}
}
static px_void PX_MenuClearState(PX_Object *pMenuObject)
{
	PX_Object_Menu *pMenu=PX_Object_GetMenu(pMenuObject);
	px_list_node *pNode=PX_NULL;

	for (pNode=PX_ListNodeAt(&pMenu->root.Items,0);pNode;pNode=PX_ListNodeNext(pNode))
	{
		PX_Object_Menu_Item *pSubItem=PX_LIST_NODETDATA(PX_Object_Menu_Item,pNode);
		pSubItem->onCursor=PX_FALSE;
		pSubItem->Activated=PX_FALSE;
	}
	PX_ObjectClearFocus(pMenuObject);
	pMenu->activating=PX_FALSE;
}
px_bool PX_Menu_CursorAction(PX_Object *pObject,PX_Object_Menu_Item *pItem,px_float x,px_float y,PX_MENU_CURSORACTION action)
{
	px_list_node *pNode=PX_NULL;
	PX_Object_Menu *pMenu=PX_Object_GetMenu(pObject);

	if (PX_isXYInRegion(x,y,(px_float)pItem->x,(px_float)pItem->y,(px_float)pItem->width,(px_float)pItem->height))
	{
		
		PX_MenuClearParent(pMenu,pItem);
		switch (action)
		{
		case PX_MENU_CURSORACTION_MOVE:
			{
				if (!pItem->enable)
				{
					return PX_TRUE;
				}
				pItem->onCursor=PX_TRUE;
				if (pMenu->activating&&pItem->Items.size!=0)
				{
					pItem->Activated=PX_TRUE;
					PX_MenuClearChild(pMenu,pItem);
				}
			}
			break;
		case PX_MENU_CURSORACTION_DOWN:
			{
				if (!pItem->enable)
				{
					return PX_TRUE;
				}
				pItem->onCursor=PX_TRUE;
				pItem->Activated=PX_TRUE;

				if (pItem->Items.size==0)
				{
					PX_MenuClearState(pObject);
				}
				else
				{
					PX_MenuClearChild(pMenu,pItem);
				}

				if (pItem->func_callback)
				{
					pItem->func_callback(pItem->user_ptr);
				}

				return PX_TRUE;
			}
			break;
		}
	}
	else
	{
		pItem->onCursor=PX_FALSE;
	}

	if (!pItem->Activated)
	{
		return PX_FALSE;
	}

	for (pNode=PX_ListNodeAt(&pItem->Items,0);pNode;pNode=PX_ListNodeNext(pNode))
	{
		PX_Object_Menu_Item *pSubItem=PX_LIST_NODETDATA(PX_Object_Menu_Item,pNode);
		if(PX_Menu_CursorAction(pObject,pSubItem,x,y,action))
			return PX_TRUE;
	}
	return PX_FALSE;
}
px_void PX_MenuPostEvent_OnCursorDown(PX_Object *pObject,PX_Object_Event e)
{
	px_list_node *pNode=PX_NULL;
	px_float x=PX_Object_Event_GetCursorX(e);
	px_float y=PX_Object_Event_GetCursorY(e);
	PX_Object_Menu *pMenu=PX_Object_GetMenu(pObject);

	if (!pMenu->activating)
	{
		for (pNode=PX_ListNodeAt(&pMenu->root.Items,0);pNode;pNode=PX_ListNodeNext(pNode))
		{
			PX_Object_Menu_Item *pSubItem=PX_LIST_NODETDATA(PX_Object_Menu_Item,pNode);
			if (PX_isXYInRegion(x,y,(px_float)pSubItem->x,(px_float)pSubItem->y,(px_float)pSubItem->width,(px_float)pSubItem->height)&&pSubItem->enable)
			{
				pMenu->activating=PX_TRUE;
				PX_ObjectSetFocus(pObject);
				pSubItem->Activated=PX_TRUE;
				PX_MenuClearChild(pMenu,pSubItem);
				return;
			}
		}
		PX_ObjectClearFocus(pObject);
		pMenu->activating=PX_FALSE;
		return;
	}
	else
	{
		if (!PX_Menu_CursorAction(pObject,&pMenu->root,x,y,PX_MENU_CURSORACTION_DOWN))
		{
			PX_MenuClearState(pObject);
		}
	}
}
px_void PX_MenuPostEvent_OnCursorMove(PX_Object *pObject,PX_Object_Event e)
{
	PX_Object_Menu *pMenu=PX_Object_GetMenu(pObject);
	px_float x=PX_Object_Event_GetCursorX(e);
	px_float y=PX_Object_Event_GetCursorY(e);
	PX_Menu_CursorAction(pObject,&pMenu->root,x,y,PX_MENU_CURSORACTION_MOVE);	
}
px_void PX_Object_MenuOnCursorEventEx(PX_Object *pObject,PX_Object_Event e)
{

	switch(e.Event)
	{
	case PX_OBJECT_EVENT_CURSORMOVE:
	case PX_OBJECT_EVENT_CURSORDRAG:
		{
			PX_MenuPostEvent_OnCursorMove(pObject,e);
		}
		break;
	case PX_OBJECT_EVENT_CURSORDOWN:
		{
			PX_MenuPostEvent_OnCursorDown(pObject,e);
		}
		break;
	default:
		break;
	}

}
static px_void PX_MenuRenderItem(px_surface *pSurface,PX_Object_Menu *pMenu,PX_Object_Menu_Item *pItem,px_bool broot,px_dword elapsed)
{
	if (pItem->width&&pItem->height)
	{
		if (pItem->onCursor||pItem->Activated)
		{
			PX_GeoDrawRect(pSurface,pItem->x,pItem->y,pItem->x+pItem->width-1,pItem->y+pItem->height-1,pMenu->cursorColor);
		}
		else
		{
			PX_GeoDrawRect(pSurface,pItem->x,pItem->y,pItem->x+pItem->width-1,pItem->y+pItem->height-1,pMenu->backgroundColor);
		}

		if (broot)
			PX_FontModuleDrawText(pSurface,pMenu->fontmodule,pItem->x+pItem->width/2,pItem->y+pItem->height/2,PX_ALIGN_CENTER,pItem->Text,pMenu->fontColor);
		else
		{
			if (!pItem->enable)
			{
				PX_FontModuleDrawText(pSurface,pMenu->fontmodule,pItem->x+18,pItem->y+pItem->height/2,PX_ALIGN_LEFTMID,pItem->Text,pMenu->disableColor);
			}
			else
			{
				PX_FontModuleDrawText(pSurface,pMenu->fontmodule,pItem->x+18,pItem->y+pItem->height/2,PX_ALIGN_LEFTMID,pItem->Text,pMenu->fontColor);
			}
			
			if (pItem->Items.size!=0)
			{
				PX_GeoDrawTriangle(pSurface,
					PX_POINT2D((px_float)(pItem->x+pItem->width-6),(px_float)(pItem->y+pItem->height/2)),
					PX_POINT2D((px_float)(pItem->x+pItem->width-12),(px_float)(pItem->y+pItem->height/2-4)),
					PX_POINT2D((px_float)(pItem->x+pItem->width-12),(px_float)(pItem->y+pItem->height/2+4)),
					pMenu->fontColor);
			}
		}

	}


	if (pItem->Activated)
	{
		if(pItem->Items.size)
		{
			px_list_node *pNode=PX_NULL;
			for (pNode=PX_ListNodeAt(&pItem->Items,0);pNode;pNode=PX_ListNodeNext(pNode))
			{
				PX_Object_Menu_Item *pSubItem=PX_LIST_NODETDATA(PX_Object_Menu_Item,pNode);
				PX_MenuRenderItem(pSurface,pMenu,pSubItem,PX_FALSE,elapsed);
			}
		}
	}
}
px_void PX_Object_MenuRenderEx(px_surface *pSurface,PX_Object_Menu *pMenu,px_dword elapsed)
{
	px_list_node *pNode=PX_NULL;

	for (pNode=PX_ListNodeAt(&pMenu->root.Items,0);pNode;pNode=PX_ListNodeNext(pNode))
	{
		PX_Object_Menu_Item *pSubItem=PX_LIST_NODETDATA(PX_Object_Menu_Item,pNode);
		PX_MenuRenderItem(pSurface,pMenu,pSubItem,PX_TRUE,elapsed);
	}

}
static px_void PX_MenuItemFree(PX_Object_Menu_Item *pItem)
{
	px_list_node *pNode=PX_NULL;

	for (pNode=PX_ListNodeAt(&pItem->Items,0);pNode;pNode=PX_ListNodeNext(pNode))
	{
		PX_Object_Menu_Item *pSubItem=PX_LIST_NODETDATA(PX_Object_Menu_Item,pNode);
		PX_MenuItemFree(pSubItem);
	}
	PX_ListFree(&pItem->Items);
}
px_void PX_Object_MenuFreeEx(PX_Object_Menu *pMenu)
{
	PX_MenuItemFree(&pMenu->root);
}
PX_Object_Menu_Item * PX_Object_MenuAddItem(PX_Object *pObject,PX_Object_Menu_Item *parent,const px_char Text[],PX_MenuExecuteFunc _callback,px_void *ptr)
{
	PX_Object_Menu_Item item,*ret;
	PX_Object_Menu *pMenu=PX_Object_GetMenu(pObject);
	px_float objx,objy,objWidth,objHeight;
	px_float inheritX,inheritY;
	if (parent==PX_NULL)
	{
		parent = PX_Object_MenuGetRootItem(pObject);
	}

	PX_ObjectGetInheritXY(pObject,&inheritX,&inheritY);

	objx=(pObject->x+inheritX);
	objy=(pObject->y+inheritY);
	objWidth=pObject->Width;
	objHeight=pObject->Height;

	PX_memset(&item,0,sizeof(PX_Object_Menu_Item));
	PX_strcpy(item.Text,Text,sizeof(item.Text));
	item.func_callback=_callback;
	PX_ListInitialize(pMenu->mp,&item.Items);
	item.pParent=parent;
	item.user_ptr=ptr;
	item.enable=PX_TRUE;
	ret=(PX_Object_Menu_Item *)PX_ListPush(&parent->Items,&item,sizeof(item));
	if (ret!=PX_NULL)
	{
		PX_MenuSubMenuUpdate((px_int)objx,(px_int)objy,pMenu);
	}
	return ret;
}

px_void PX_Object_MenuFree(PX_Object *Obj)
{
	PX_Object_MenuFreeEx(PX_Object_GetMenu(Obj));
}

px_void PX_Object_MenuRender(px_surface *psurface, PX_Object *Obj,px_uint elapsed)
{
	PX_Object_MenuRenderEx(psurface,PX_Object_GetMenu(Obj),elapsed);
}

px_void PX_Object_MenuOnCursorEvent(PX_Object *pObject,PX_Object_Event e,px_void *ptr)
{
	PX_Object_MenuOnCursorEventEx(pObject,e);
}

PX_Object * PX_Object_MenuCreate(px_memorypool *mp,PX_Object *Parent,px_int x,int y,px_int width,PX_FontModule *fontmodule)
{
	PX_Object *pObject;
	PX_Object_Menu *pMenu=(PX_Object_Menu *)MP_Malloc(mp,sizeof(PX_Object_Menu));

	if (pMenu==PX_NULL)
	{
		return PX_NULL;
	}

	if (!PX_Object_MenuInitialize(mp,pMenu,x,y,width,fontmodule))
	{
		return PX_NULL;
	}


	pObject=PX_ObjectCreate(mp,Parent,(px_float)x,(px_float)y,0,0,0,0);
	if (pObject==PX_NULL)
	{
		MP_Free(pObject->mp,pMenu);
		return PX_NULL;
	}


	pObject->pObject=pMenu;
	pObject->Enabled=PX_TRUE;
	pObject->Visible=PX_TRUE;
	pObject->Type=PX_OBJECT_TYPE_MENU;
	pObject->Func_ObjectFree=PX_Object_MenuFree;
	pObject->Func_ObjectRender=PX_Object_MenuRender;
	pObject->OnLostFocusReleaseEvent=PX_TRUE;
	PX_ObjectRegisterEvent(pObject,PX_OBJECT_EVENT_CURSORDOWN,PX_Object_MenuOnCursorEvent,PX_NULL);
	PX_ObjectRegisterEvent(pObject,PX_OBJECT_EVENT_CURSORMOVE,PX_Object_MenuOnCursorEvent,PX_NULL);
	PX_ObjectRegisterEvent(pObject,PX_OBJECT_EVENT_CURSORDRAG,PX_Object_MenuOnCursorEvent,PX_NULL);
	return pObject;
}

PX_Object_Menu_Item * PX_Object_MenuGetRootItem(PX_Object *pMenuObject)
{
	return &PX_Object_GetMenu(pMenuObject)->root;
}