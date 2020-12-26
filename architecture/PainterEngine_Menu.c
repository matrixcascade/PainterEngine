#include "PainterEngine_Menu.h"

px_void PX_MenuSubMenuUpdateEx(PX_Menu *pMenu,PX_Menu_Item *pItem,px_int startX,px_int startY)
{
	px_int index=0;
	px_int maxWidth=0,maxHeight=0;
	px_list_node *pNode=PX_NULL;


	for (pNode=PX_ListNodeAt(&pItem->Items,0);pNode;pNode=PX_ListNodeNext(pNode))
	{
		px_int cWidth,cHeight;
		PX_Menu_Item *pSubItem=PX_LIST_NODETDATA(PX_Menu_Item,pNode);
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

	maxWidth+=PX_MENU_ITEM_SPACER_SIZE*2+16;
	maxHeight+=PX_MENU_ITEM_SPACER_SIZE*2;


	if (maxWidth<pMenu->minimumSize)
	{
		maxWidth=pMenu->minimumSize;
	}

	for (pNode=PX_ListNodeAt(&pItem->Items,0);pNode;pNode=PX_ListNodeNext(pNode))
	{
		PX_Menu_Item *pSubItem=PX_LIST_NODETDATA(PX_Menu_Item,pNode);
		pSubItem->x=startX;
		pSubItem->y=startY+index*maxHeight;
		pSubItem->width=maxWidth;
		pSubItem->height=maxHeight;
		index++;
	}
	

	for (pNode=PX_ListNodeAt(&pItem->Items,0);pNode;pNode=PX_ListNodeNext(pNode))
	{
		PX_Menu_Item *pSubItem=PX_LIST_NODETDATA(PX_Menu_Item,pNode);
		PX_MenuSubMenuUpdateEx(pMenu,pSubItem,pSubItem->x+pSubItem->width,pSubItem->y);
	}
}
px_void PX_MenuSubMenuUpdate(PX_Menu *pMenu)
{
	px_int index=0;
	px_int maxWidth=0,maxHeight=0;
	px_list_node *pNode=PX_NULL;


	for (pNode=PX_ListNodeAt(&pMenu->root.Items,0);pNode;pNode=PX_ListNodeNext(pNode))
	{
		px_int cWidth,cHeight;
		PX_Menu_Item *pSubItem=PX_LIST_NODETDATA(PX_Menu_Item,pNode);
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

	maxWidth+=PX_MENU_ITEM_SPACER_SIZE*2;
	maxHeight+=PX_MENU_ITEM_SPACER_SIZE*2;


	if (maxWidth<pMenu->minimumSize)
	{
		maxWidth=pMenu->minimumSize;
	}

	for (pNode=PX_ListNodeAt(&pMenu->root.Items,0);pNode;pNode=PX_ListNodeNext(pNode))
	{
		PX_Menu_Item *pSubItem=PX_LIST_NODETDATA(PX_Menu_Item,pNode);
		pSubItem->x=pMenu->x+index*maxWidth;
		pSubItem->y=pMenu->y;
		pSubItem->width=maxWidth;
		pSubItem->height=maxHeight;
		index++;
	}


	for (pNode=PX_ListNodeAt(&pMenu->root.Items,0);pNode;pNode=PX_ListNodeNext(pNode))
	{
		PX_Menu_Item *pSubItem=PX_LIST_NODETDATA(PX_Menu_Item,pNode);
		PX_MenuSubMenuUpdateEx(pMenu,pSubItem,pSubItem->x,pSubItem->y+pSubItem->height);
	}
}




px_bool PX_MenuInitialize(PX_Runtime *runtime,px_memorypool *mp,PX_Menu *pMenu,px_int x,int y,px_int width,PX_FontModule *fontmodule)
{
	PX_memset(pMenu,0,sizeof(PX_Menu));
	pMenu->enabled=PX_TRUE;
	pMenu->fontmodule=fontmodule;
	pMenu->minimumSize=width;
	pMenu->mp=mp;
	pMenu->OnFocus=PX_FALSE;
	pMenu->x=x;
	pMenu->y=y;
	pMenu->backgroundColor=PX_COLOR(255,255,255,255);
	pMenu->cursorColor=PX_COLOR(255,192,192,192);
	pMenu->fontColor=PX_COLOR(255,0,0,0);
	PX_ListInitialize(mp,&pMenu->root.Items);
	pMenu->root.Activated=PX_TRUE;
	return PX_TRUE;
}


typedef enum
{
	PX_MENU_CURSORACTION_MOVE,
	PX_MENU_CURSORACTION_DOWN,
}PX_MENU_CURSORACTION;


static px_void PX_MenuClearParent(PX_Menu *pMenu,PX_Menu_Item *pItem)
{
	px_list_node *pNode=PX_NULL;

	for (pNode=PX_ListNodeAt(&pItem->pParent->Items,0);pNode;pNode=PX_ListNodeNext(pNode))
	{
		PX_Menu_Item *pSubItem=PX_LIST_NODETDATA(PX_Menu_Item,pNode);
		pSubItem->onCursor=PX_FALSE;
		pSubItem->Activated=PX_FALSE;
	}
}

static px_void PX_MenuClearChild(PX_Menu *pMenu,PX_Menu_Item *pItem)
{
	px_list_node *pNode=PX_NULL;

	for (pNode=PX_ListNodeAt(&pItem->Items,0);pNode;pNode=PX_ListNodeNext(pNode))
	{
		PX_Menu_Item *pSubItem=PX_LIST_NODETDATA(PX_Menu_Item,pNode);
		pSubItem->onCursor=PX_FALSE;
		pSubItem->Activated=PX_FALSE;
	}
}

static px_void PX_MenuClearState(PX_Menu *pMenu)
{
	px_list_node *pNode=PX_NULL;

	for (pNode=PX_ListNodeAt(&pMenu->root.Items,0);pNode;pNode=PX_ListNodeNext(pNode))
	{
		PX_Menu_Item *pSubItem=PX_LIST_NODETDATA(PX_Menu_Item,pNode);
		pSubItem->onCursor=PX_FALSE;
		pSubItem->Activated=PX_FALSE;
	}
	pMenu->OnFocus=PX_FALSE;
}


px_bool PX_Menu_CursorAction(PX_Menu *pMenu,PX_Menu_Item *pItem,px_float x,px_float y,PX_MENU_CURSORACTION action)
{
	px_list_node *pNode=PX_NULL;


	if (PX_isXYInRegion(x,y,(px_float)pItem->x,(px_float)pItem->y,(px_float)pItem->width,(px_float)pItem->height))
	{
		PX_MenuClearParent(pMenu,pItem);
		switch (action)
		{
		case PX_MENU_CURSORACTION_MOVE:
			{
				pItem->onCursor=PX_TRUE;
				if (pMenu->OnFocus&&pItem->Items.size!=0)
				{
					pItem->Activated=PX_TRUE;
					PX_MenuClearChild(pMenu,pItem);
				}
			}
			break;
		case PX_MENU_CURSORACTION_DOWN:
			{
				pItem->onCursor=PX_TRUE;
				pItem->Activated=PX_TRUE;

				if (pItem->func_callback)
				{
					pItem->func_callback(pItem->user_ptr);
				}
				if (pItem->Items.size==0)
				{
					PX_MenuClearState(pMenu);
				}
				else
				{
					PX_MenuClearChild(pMenu,pItem);
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
		PX_Menu_Item *pSubItem=PX_LIST_NODETDATA(PX_Menu_Item,pNode);
		if(PX_Menu_CursorAction(pMenu,pSubItem,x,y,action))
			return PX_TRUE;
	}
	return PX_FALSE;
}


px_void PX_MenuPostEvent_OnCursorDown(PX_Menu *pMenu,PX_Object_Event e)
{
	px_list_node *pNode=PX_NULL;
	px_float x=PX_Object_Event_GetCursorX(e);
	px_float y=PX_Object_Event_GetCursorY(e);

	if (!pMenu->OnFocus)
	{
		for (pNode=PX_ListNodeAt(&pMenu->root.Items,0);pNode;pNode=PX_ListNodeNext(pNode))
		{
			PX_Menu_Item *pSubItem=PX_LIST_NODETDATA(PX_Menu_Item,pNode);
			if (PX_isXYInRegion(x,y,(px_float)pSubItem->x,(px_float)pSubItem->y,(px_float)pSubItem->width,(px_float)pSubItem->height))
			{
				pMenu->OnFocus=PX_TRUE;
				pSubItem->Activated=PX_TRUE;
				PX_MenuClearChild(pMenu,pSubItem);
				return;
			}
		}
		pMenu->OnFocus=PX_FALSE;
		return;
	}
	else
	{
		if (!PX_Menu_CursorAction(pMenu,&pMenu->root,x,y,PX_MENU_CURSORACTION_DOWN))
		{
			PX_MenuClearState(pMenu);
		}
	}
}

px_void PX_MenuPostEvent_OnCursorMove(PX_Menu *pMenu,PX_Object_Event e)
{
	px_float x=PX_Object_Event_GetCursorX(e);
	px_float y=PX_Object_Event_GetCursorY(e);
	PX_Menu_CursorAction(pMenu,&pMenu->root,x,y,PX_MENU_CURSORACTION_MOVE);	
}

px_void PX_MenuPostEvent(PX_Menu *pMenu,PX_Object_Event e)
{
	switch(e.Event)
	{
	case PX_OBJECT_EVENT_CURSORMOVE:
		{
			PX_MenuPostEvent_OnCursorMove(pMenu,e);
		}
		break;
	case PX_OBJECT_EVENT_CURSORDOWN:
		{
			PX_MenuPostEvent_OnCursorDown(pMenu,e);
		}
		break;
	default:
		break;
	}

}
px_void PX_MenuUpdate(PX_Menu *pMenu,px_dword elpased)
{

}

static px_void PX_MenuRenderItem(px_surface *pSurface,PX_Menu *pMenu,PX_Menu_Item *pItem,px_bool broot,px_dword elpased)
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
		PX_FontModuleDrawText(pSurface,pMenu->fontmodule,pItem->x+pItem->width/2,pItem->y+pItem->height/2,PX_FONT_ALIGN_CENTER,pItem->Text,pMenu->fontColor);
		else
		{
			PX_FontModuleDrawText(pSurface,pMenu->fontmodule,pItem->x,pItem->y+pItem->height/2,PX_FONT_ALIGN_LEFTMID,pItem->Text,pMenu->fontColor);
			if (pItem->Items.size!=0)
			{
				PX_GeoDrawRect(pSurface,pItem->x+pItem->width-8,pItem->y+pItem->height/2-1,pItem->x+pItem->width-6,pItem->y+pItem->height/2+1,pMenu->fontColor);
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
				PX_Menu_Item *pSubItem=PX_LIST_NODETDATA(PX_Menu_Item,pNode);
				PX_MenuRenderItem(pSurface,pMenu,pSubItem,PX_FALSE,elpased);
			}
		}
	}
}

px_void PX_MenuRender(px_surface *pSurface,PX_Menu *pMenu,px_dword elpased)
{
	px_list_node *pNode=PX_NULL;

	for (pNode=PX_ListNodeAt(&pMenu->root.Items,0);pNode;pNode=PX_ListNodeNext(pNode))
	{
		PX_Menu_Item *pSubItem=PX_LIST_NODETDATA(PX_Menu_Item,pNode);
		PX_MenuRenderItem(pSurface,pMenu,pSubItem,PX_TRUE,elpased);
	}
	
}


static px_void PX_MenuItemFree(PX_Menu_Item *pItem)
{
	px_list_node *pNode=PX_NULL;

	for (pNode=PX_ListNodeAt(&pItem->Items,0);pNode;pNode=PX_ListNodeNext(pNode))
	{
		PX_Menu_Item *pSubItem=PX_LIST_NODETDATA(PX_Menu_Item,pNode);
		PX_MenuItemFree(pSubItem);
	}
	PX_ListFree(&pItem->Items);
}

px_void PX_MenuFree(PX_Menu *pMenu)
{
	PX_MenuItemFree(&pMenu->root);
}

PX_Menu_Item * PX_MenuAddItem(PX_Menu *pMenu,PX_Menu_Item *parent,const px_char Text[],PX_MenuExecuteFunc _callback,px_void *ptr)
{
	PX_Menu_Item item,*ret;
	PX_memset(&item,0,sizeof(PX_Menu_Item));
	PX_strcpy(item.Text,Text,sizeof(item.Text));
	item.func_callback=_callback;
	PX_ListInitialize(pMenu->mp,&item.Items);
	item.pParent=parent;
	item.user_ptr=ptr;
	ret=(PX_Menu_Item *)PX_ListPush(&parent->Items,&item,sizeof(item));
	if (ret!=PX_NULL)
	{
		PX_MenuSubMenuUpdate(pMenu);
	}
	return ret;
}

