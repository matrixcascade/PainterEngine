#include "PX_Object_List.h"
PX_Object_List  * PX_Object_GetList( PX_Object *pObject )
{
	return (PX_Object_List *)PX_ObjectGetDescByType(pObject, PX_OBJECT_TYPE_LIST);
}

PX_Object_ListItem  * PX_Object_GetListItem( PX_Object *pObject )
{
	return (PX_Object_ListItem*)PX_ObjectGetDescByType(pObject, PX_OBJECT_TYPE_LISTITEM);
}

PX_OBJECT_EVENT_FUNCTION(PX_Object_ListOnCursorMove)
{
	PX_Object_List *pList= PX_Object_GetList(pObject);
	px_float objx,objy,objWidth,objHeight;
	px_rect rect;
	rect = PX_ObjectGetRect(pObject);
	objx = rect.x;
	objy = rect.y;
	objWidth = rect.width;
	objHeight = rect.height;

	pList->currentCursorIndex=-1;
	if (!PX_ObjectIsCursorInRegion(pObject,e))
	{
		return;
	}

	if (pList->pArrayData)
	{
		if (pList->ArrayDataCount * pList->ItemHeight > pObject->Height)
		{
			if (PX_Object_Event_GetCursorX(e) > objx + objWidth - PX_OBJECT_LIST_SLIDERBAR_WIDTH)
			{
				return;
			}
		}
	}
	else
	{
		if (pList->pData.size * pList->ItemHeight > pObject->Height)
		{
			if (PX_Object_Event_GetCursorX(e) > objx + objWidth - PX_OBJECT_LIST_SLIDERBAR_WIDTH)
			{
				return;
			}
		}
	}
	
	pList->currentCursorIndex=(px_int)(PX_Object_Event_GetCursorY(e)+pList->offsety-objy)/pList->ItemHeight;

	if (pList->pArrayData)
	{
		if (pList->currentCursorIndex < 0 || pList->currentCursorIndex >= pList->ArrayDataCount)
		{
			pList->currentCursorIndex = -1;
		}
	}
	else
	{
		if (pList->currentCursorIndex < 0 || pList->currentCursorIndex >= pList->pData.size)
		{
			pList->currentCursorIndex = -1;
		}
	}
	
}

PX_OBJECT_EVENT_FUNCTION(PX_Object_ListOnCursorDown)
{
	PX_Object_List *pList= PX_Object_GetList(pObject);
	px_float objx,objy,objWidth,objHeight;
	px_rect rect;
	px_int currentSelectedIndex;
	PX_Object_Event ne;
	rect = PX_ObjectGetRect(pObject);
	objx = rect.x;
	objy = rect.y;
	objWidth = rect.width;
	objHeight = rect.height;


	if (!PX_ObjectIsCursorInRegion(pObject,e))
	{
		return;
	}

	if (pList->pArrayData)
	{
		if (pList->ArrayDataCount * pList->ItemHeight > pObject->Height)
		{
			if (PX_Object_Event_GetCursorX(e) > objx + objWidth - PX_OBJECT_LIST_SLIDERBAR_WIDTH)
			{
				return;
			}
		}
	}
	else
	{
		if (pList->pData.size * pList->ItemHeight > pObject->Height)
		{
			if (PX_Object_Event_GetCursorX(e) > objx + objWidth - PX_OBJECT_LIST_SLIDERBAR_WIDTH)
			{
				return;
			}
		}
	}

	currentSelectedIndex=(px_int)(PX_Object_Event_GetCursorY(e)+pList->offsety-objy)/pList->ItemHeight;
	if (pList->currentSelectedIndex!=currentSelectedIndex)
	{
		PX_memset(&ne,0,sizeof(ne));
		pList->currentSelectedIndex=currentSelectedIndex;
		if (pList->pArrayData)
		{
			if (pList->currentSelectedIndex < 0 || pList->currentSelectedIndex >= pList->ArrayDataCount)
			{
				pList->currentSelectedIndex = -1;
			}
		}
		else
		{
			if (pList->currentSelectedIndex < 0 || pList->currentSelectedIndex >= pList->pData.size)
			{
				pList->currentSelectedIndex = -1;
			}
		}
		pList->click_elapsed = 0;
		ne.Event=PX_OBJECT_EVENT_VALUECHANGED;
		PX_Object_Event_SetIndex(&ne,pList->currentSelectedIndex);
		PX_ObjectExecuteEvent(pObject,ne);
	}
	else
	{
		if (pList->doubleCursorCancel&&pList->currentSelectedIndex!=-1)
		{
			PX_memset(&e,0,sizeof(ne));
			pList->currentSelectedIndex=-1;
			ne.Event=PX_OBJECT_EVENT_VALUECHANGED;
			PX_Object_Event_SetIndex(&ne,pList->currentSelectedIndex);
			PX_ObjectExecuteEvent(pObject,ne);
		}
		else if(pList->currentSelectedIndex != -1&&pList->click_elapsed<260)
		{
			PX_memset(&e, 0, sizeof(ne));
			ne.Event = PX_OBJECT_EVENT_EXECUTE;
			PX_Object_Event_SetIndex(&ne, pList->currentSelectedIndex);
			PX_ObjectExecuteEvent(pObject, ne);
		}
		pList->click_elapsed = 0;
	}
}

PX_OBJECT_EVENT_FUNCTION(PX_Object_ListOnCursorRDown)
{
	PX_Object_List *pList= PX_Object_GetList(pObject);
	if (!PX_ObjectIsCursorInRegion(pObject,e))
	{
		return;
	}

	if (pList->currentSelectedIndex!=-1)
	{
		pList->currentSelectedIndex=-1;
		e.Event=PX_OBJECT_EVENT_VALUECHANGED;
		PX_Object_Event_SetIndex(&e,pList->currentSelectedIndex);
		PX_ObjectExecuteEvent(pObject,e);
	}
	
}

PX_OBJECT_EVENT_FUNCTION(PX_Object_ListOnWheel)
{
	PX_Object_List *pList= PX_Object_GetList(pObject);
	px_float objx,objy,objWidth,objHeight;
	px_rect rect;

	rect = PX_ObjectGetRect(pObject);
	objx = rect.x;
	objy = rect.y;
	objWidth = rect.width;
	objHeight = rect.height;


	if (!PX_ObjectIsCursorInRegion(pObject,e))
	{
		return;
	}
	pList->offsety-=(px_int)PX_Object_Event_GetCursorZ(e)/5;

	if (pList->pArrayData)
	{
		if (pList->offsety > (pList->ItemHeight) * pList->ArrayDataCount - objHeight)
		{
			pList->offsety = (pList->ItemHeight) * pList->ArrayDataCount - (px_int)objHeight;
		}
	}
	else
	{
		if (pList->offsety > (pList->ItemHeight) * pList->pData.size - objHeight)
		{
			pList->offsety = (pList->ItemHeight) * pList->pData.size - (px_int)objHeight;
		}
	}

	

	if (pList->offsety<0)
	{
		pList->offsety=0;
	}

	PX_Object_SliderBarSetValue(pList->SliderBar,pList->offsety);
}

px_void PX_Object_ListMoveToTop(PX_Object *pObject)
{
	PX_Object_List *pList= PX_Object_GetList(pObject);
	pList->offsety=0;
	PX_Object_SliderBarSetValue(pList->SliderBar,0);
}

px_void PX_Object_ListMoveToBottom(PX_Object* pObject)
{
	PX_Object_List* pList = PX_Object_GetList(pObject);
	if (pList->ItemHeight*pList->pData.size>pObject->Height)
	{
		pList->offsety = (px_int)(pList->ItemHeight * pList->pData.size - pObject->Height);
		PX_Object_SliderBarSetRange(pList->SliderBar, 0, (px_int)(pList->ItemHeight * (pList->pData.size) - pObject->Height));
		PX_Object_SliderBarSetValue(pList->SliderBar, (px_int)(pList->ItemHeight * pList->pData.size - pObject->Height));
	}
	else
	{
		pList->offsety = 0;
		PX_Object_SliderBarSetValue(pList->SliderBar, 0);
	}
	
}

PX_OBJECT_RENDER_FUNCTION(PX_Object_ListRender)
{
	PX_Object *pItemObject;
	px_int offsetX=0,drawXCenter=0;
	px_int i;
	px_int iy;
	PX_Object_List *pList=PX_ObjectGetDesc(PX_Object_List,pObject);
	PX_Object_ListItem *pItem;
	px_float objx,objy,objWidth,objHeight;
	px_rect rect;

	rect = PX_ObjectGetRect(pObject);
	objx = rect.x;
	objy = rect.y;
	objWidth = rect.width;
	objHeight = rect.height;


	pList->click_elapsed += elapsed;

	if (pList->Items.size!=(px_int)(objHeight/(pList->ItemHeight) + 2))
	{
		PX_Object_ListItem Item;
		for (i=0;i<pList->Items.size;i++)
		{
			PX_Object* pItemObject = *PX_VECTORAT(PX_Object*, &pList->Items, i);
			PX_ObjectDelete(pItemObject);
		}

		
		PX_VectorClear(&pList->Items);

		for (i = 0; i < (px_int)(objHeight / pList->ItemHeight + 2); i++)
		{
			Item.pList = pObject;
			Item.pdata = PX_NULL;
			pItemObject = PX_ObjectCreateEx(pList->mp, 0, 0, 0, 0, (px_float)pObject->Width, (px_float)pList->ItemHeight, 0, PX_OBJECT_TYPE_LISTITEM, PX_NULL, PX_NULL, PX_NULL, &Item, sizeof(PX_Object_ListItem));
			if (!pList->CreateFunctions(pList->mp, pItemObject,pList->userptr)) return;
			if (!PX_VectorPushback(&pList->Items, &pItemObject))return;
		}
	}


	if (pList->ItemHeight*pList->pData.size>objHeight)
	{
		PX_Object_SliderBar *pSliderBar=PX_Object_GetSliderBar(pList->SliderBar);
		pList->SliderBar->Visible=PX_TRUE;
		pList->SliderBar->y=0;
		pList->SliderBar->x=objWidth- PX_OBJECT_LIST_SLIDERBAR_WIDTH;
		pList->SliderBar->Height = objHeight;
		pSliderBar->SliderButtonLength=(px_int)(pList->SliderBar->Height*objHeight/(pList->ItemHeight*pList->pData.size));
		if(pSliderBar->SliderButtonLength<32) pSliderBar->SliderButtonLength=32;
		pSliderBar->Max=(pList->ItemHeight)*pList->pData.size-(px_int)objHeight;
		PX_Object_SliderBarSetBackgroundColor(pList->SliderBar,pList->BackgroundColor);
		PX_Object_SliderBarSetColor(pList->SliderBar,pList->BorderColor);
	}
	else
	{
		pList->SliderBar->Visible=PX_FALSE;
	}


	if (pList->offsety<0)
	{
		iy=-pList->offsety;
	}
	else
	{
		iy=-(pList->offsety%pList->ItemHeight);
	}

	for (i=0;i<pList->Items.size;i++)
	{

		pItemObject=*PX_VECTORAT(PX_Object *,&pList->Items,i);
		pItemObject->x=0;
		pItemObject->y=(px_float)iy;
		pItemObject->Width = pObject->Width;
		pItem=PX_Object_GetListItem(pItemObject);
		if (pList->offsety<0)
		{
			if(i<pList->pData.size)
			{
				pItem->pdata=*PX_VECTORAT(px_void *,&pList->pData,i);
				pItem->index = i;
				PX_ObjectUpdate(pItemObject,elapsed);
			}
			else
			{
				pItem->pdata=PX_NULL;
			}
		}
		else
		{
			if(i+pList->offsety/pList->ItemHeight<pList->pData.size)
			{
				px_int index = i + pList->offsety / pList->ItemHeight;
				pItem->pdata=*PX_VECTORAT(px_void *,&pList->pData,index);
				pItem->index = index;
				PX_ObjectUpdate(pItemObject,elapsed);
			}
			else
			{
				pItem->pdata=PX_NULL;
			}
		}
		iy+=(px_int)pItemObject->Height;
	}

	if ((px_int)pList->renderSurface.width!=(px_int)pObject->Width|| (px_int)pList->renderSurface.height != (px_int)pObject->Height)
	{
		PX_SurfaceFree(&pList->renderSurface);
		if (!PX_SurfaceCreate(pList->mp, (px_int)pObject->Width, (px_int)pObject->Height, &pList->renderSurface))
		{
			PX_ASSERT();
			return;
		}
	}

	PX_SurfaceClear(&pList->renderSurface,0,0,pList->renderSurface.width-1,pList->renderSurface.height-1,pList->BackgroundColor);
	for (i=0;i<pList->Items.size;i++)
	{
		px_int index=i+pList->offsety/pList->ItemHeight;
		pItemObject=*PX_VECTORAT(PX_Object *,&pList->Items,i);
		pItem=PX_Object_GetListItem(pItemObject);
		if (pItemObject->y+pItemObject->Height>0&&pItemObject->y<pObject->Height&&pItem->pdata)
		{

			if (pList->showCursor)
			{
				if (index==pList->currentCursorIndex&&index!=pList->currentSelectedIndex)
				{
					
					PX_GeoDrawRect(&pList->renderSurface,(px_int)pItemObject->x,(px_int)pItemObject->y,(px_int)(pItemObject->x+pItemObject->Width-1),(px_int)(pItemObject->y+pItemObject->Height-1),pList->CursorColor);
					PX_GeoDrawBorder(&pList->renderSurface, 0, (px_int)pItemObject->y, (px_int)pItemObject->Width - 1, (px_int)(pItemObject->y + pItemObject->Height - 1), 1, pList->BorderColor);
				}
			}
			if (index==pList->currentSelectedIndex)
			{
				PX_GeoDrawRect(&pList->renderSurface, (px_int)pItemObject->x, (px_int)pItemObject->y, (px_int)(pItemObject->x + pItemObject->Width - 1), (px_int)(pItemObject->y + pItemObject->Height - 1), pList->SelectColor);
				PX_GeoDrawBorder(&pList->renderSurface, 0, (px_int)pItemObject->y, (px_int)pItemObject->Width - 1, (px_int)(pItemObject->y + pItemObject->Height - 1), 1, pList->BorderColor);
				
			}
			PX_ObjectRender(&pList->renderSurface,pItemObject,elapsed);
		}
	}
	PX_GeoDrawBorder(&pList->renderSurface,0,0,pList->renderSurface.width-1,pList->renderSurface.height-1,1,pList->BorderColor);
	PX_TextureRender(psurface,&pList->renderSurface,(px_int)objx,(px_int)objy,PX_ALIGN_LEFTTOP,0);

}

px_void PX_Object_ListClear(PX_Object *pListObj)
{
	PX_Object_List  *pList=PX_Object_GetList(pListObj);
	if (pList)
	{
		PX_VectorClear(&pList->pData);
	}
}

px_void PX_Object_ListSetCurrentSelectIndex(PX_Object *pObject,px_int index)
{
	PX_Object_List  *pList=PX_Object_GetList(pObject);
	if(pList)
	{
		if (index>=0&&index<pList->Items.size)
		{
			pList->currentSelectedIndex=index;
		}
		else
		{
			pList->currentSelectedIndex=-1;
		}
	}
}

px_int PX_Object_ListGetCurrentSelectIndex(PX_Object *pObject)
{
	PX_Object_List  *pList=PX_Object_GetList(pObject);
	if(pList)
	{
		return pList->currentSelectedIndex;
	}
	return -1;
}

px_void * PX_Object_ListGetCurrentSelectData(PX_Object* pObject)
{
	PX_Object_List* pList = PX_Object_GetList(pObject);
	if (pList)
	{
		if (pList->currentSelectedIndex >= 0 && pList->currentSelectedIndex < pList->pData.size)
		{
			return *PX_VECTORAT(px_void *, &pList->pData, pList->currentSelectedIndex);
		}
	}
	return PX_NULL;
}

px_int PX_Object_ListAdd(PX_Object *pObject,px_void *ptr)
{
	PX_Object_List  *pList=PX_Object_GetList(pObject);
	if(pList)
	{
		if(PX_VectorPushback(&pList->pData,&ptr))
			return pList->pData.size-1;
	}
	PX_ASSERT();
	return -1;
}

PX_OBJECT_FREE_FUNCTION(PX_Object_ListFree)
{
	px_int i;
	PX_Object_List *pList=PX_Object_GetList(pObject);
	if (!pList)
	{
		PX_ASSERT();
		return;
	}
	
	PX_Object_ListClear(pObject);
	PX_SurfaceFree(&pList->renderSurface);
	for (i = 0; i < pList->Items.size; i++)
	{
		PX_Object* pItemObject = *PX_VECTORAT(PX_Object*, &pList->Items, i);
		PX_ObjectDelete(pItemObject);
	}
	PX_VectorFree(&pList->Items);
	PX_VectorFree(&pList->pData);
}

PX_OBJECT_EVENT_FUNCTION(PX_Object_ListOnSliderValueChanged)
{
	px_int value=PX_Object_SliderBarGetValue(pObject);
	PX_Object_List *pList=PX_Object_GetList((PX_Object *)ptr);
	pList->offsety=value;
}

PX_OBJECT_EVENT_FUNCTION(PX_Object_ListOnEvent)
{
	px_int i;
	PX_Object_List  *pList=PX_Object_GetList(pObject);

	switch (e.Event)
	{
	case PX_OBJECT_EVENT_CURSORWHEEL:
		if (!PX_ObjectIsPointInRegion(pObject,PX_Object_Event_GetCursorX(e),PX_Object_Event_GetCursorY(e)))
		{
			return;
		}
	case PX_OBJECT_EVENT_CURSORDRAG:
	case PX_OBJECT_EVENT_CURSORRDOWN:
	case PX_OBJECT_EVENT_CURSORDOWN:
	case PX_OBJECT_EVENT_CURSORUP:
	case PX_OBJECT_EVENT_CURSORRUP:
	case PX_OBJECT_EVENT_CURSORMOVE:
		if (!PX_ObjectIsPointInRegion(pObject,PX_Object_Event_GetCursorX(e),PX_Object_Event_GetCursorY(e)))
		{
			return;
		}
		PX_Object_Event_SetCursorX(&e,PX_Object_Event_GetCursorX(e)-pObject->x);
		PX_Object_Event_SetCursorY(&e,PX_Object_Event_GetCursorY(e)-pObject->y);
		break;
	case PX_OBJECT_EVENT_EXECUTE:
		return;
	}

	for (i=0;i<pList->Items.size;i++)
	{
		PX_Object *pItem=*PX_VECTORAT(PX_Object *,&pList->Items,i);
		PX_ObjectPostEvent(pItem,e);
	}
}

PX_Object* PX_Object_ListAttachObject(PX_Object* pObject,px_int attachIndex, px_int x, px_int y, px_int Width, px_int Height, px_int ItemHeight, PX_Object_ListItemOnCreate _CreateFunc, px_void* userptr)
{
	px_memorypool* mp=pObject->mp;
	PX_Object_List List = {0}, * pList;
	List.mp = mp;
	List.ItemWidth = Width - PX_OBJECT_LIST_SLIDERBAR_WIDTH;
	List.ItemHeight = ItemHeight;
	List.CreateFunctions = _CreateFunc;

	List.BorderColor = PX_OBJECT_UI_DEFAULT_BORDERCOLOR;
	List.BackgroundColor = PX_OBJECT_UI_DEFAULT_BACKGROUNDCOLOR;
	List.CursorColor = PX_OBJECT_UI_DEFAULT_CURSORCOLOR;
	List.SelectColor = PX_COLOR_FORCECOLOR;

	List.offsetx = 0;
	List.currentSelectedIndex = -1;
	List.currentCursorIndex = -1;
	List.offsety = 0;
	List.showCursor = PX_TRUE;
	List.userptr = userptr;

	PX_VectorInitialize(mp, &List.Items, sizeof(PX_Object*), 64);
	PX_VectorInitialize(mp, &List.pData, sizeof(px_void*), 32);
	if (!PX_SurfaceCreate(mp, Width, Height, &List.renderSurface)) return PX_NULL;

	PX_ASSERTIF(pObject == PX_NULL);
	PX_ASSERTIF(attachIndex < 0 || attachIndex >= PX_COUNTOF(pObject->pObjectDesc));
	PX_ASSERTIF(pObject->pObjectDesc[attachIndex] != PX_NULL);
	pList = (PX_Object_List*)PX_ObjectCreateDesc(pObject, attachIndex, PX_OBJECT_TYPE_LIST, 0, PX_Object_ListRender, PX_Object_ListFree, &List, sizeof(PX_Object_List));
	PX_ASSERTIF(pList == PX_NULL);

	pList->SliderBar = PX_Object_SliderBarCreate(mp, pObject, (px_int)pObject->Width - PX_OBJECT_LIST_SLIDERBAR_WIDTH, 0, PX_OBJECT_LIST_SLIDERBAR_WIDTH, (px_int)pObject->Height, PX_OBJECT_SLIDERBAR_TYPE_VERTICAL, PX_OBJECT_SLIDERBAR_STYLE_BOX);

	PX_ObjectRegisterEventEx(pObject, PX_OBJECT_EVENT_CURSORWHEEL, PX_OBJECT_TYPE_LIST,PX_Object_ListOnWheel, pObject);
	PX_ObjectRegisterEventEx(pObject, PX_OBJECT_EVENT_CURSORDOWN, PX_OBJECT_TYPE_LIST, PX_Object_ListOnCursorDown, pObject);
	PX_ObjectRegisterEventEx(pObject, PX_OBJECT_EVENT_CURSORRDOWN, PX_OBJECT_TYPE_LIST, PX_Object_ListOnCursorRDown, pObject);
	PX_ObjectRegisterEventEx(pObject, PX_OBJECT_EVENT_CURSORMOVE, PX_OBJECT_TYPE_LIST, PX_Object_ListOnCursorMove, pObject);
	PX_ObjectRegisterEventEx(pList->SliderBar, PX_OBJECT_EVENT_VALUECHANGED, PX_OBJECT_TYPE_LIST, PX_Object_ListOnSliderValueChanged, pObject);
	PX_ObjectRegisterEventEx(pObject, PX_OBJECT_EVENT_ANY, PX_OBJECT_TYPE_LIST, PX_Object_ListOnEvent, pObject);

	return pObject;
}

PX_Object * PX_Object_ListCreate(px_memorypool *mp, PX_Object *Parent,px_int x,px_int y,px_int Width,px_int Height,px_int ItemHeight,PX_Object_ListItemOnCreate _CreateFunc,px_void *userptr)
{
	
	PX_Object *pObject;
	pObject = PX_ObjectCreate(mp, Parent, (px_float)x, (px_float)y, 0, (px_float)Width, (px_float)Height, 0);
	if(!pObject)
	  return PX_NULL;
	if(!PX_Object_ListAttachObject(pObject,0,0,0,Width,Height,ItemHeight,_CreateFunc,userptr))
	{
		PX_ObjectDelete(pObject);
		return PX_NULL;
	}
	return pObject;
}

static PX_OBJECT_RENDER_FUNCTION(PX_Object_ListContentItemOnRender)
{
	PX_Object_ListItem* pListItem;
	px_float objx, objy, objWidth, objHeight;
	px_float inheritX, inheritY;
	pListItem = PX_ObjectGetDesc(PX_Object_ListItem,pObject);

	PX_ObjectGetInheritXY(pObject, &inheritX, &inheritY);
	objx = (pObject->x + inheritX);
	objy = (pObject->y + inheritY);
	objWidth = pObject->Width;
	objHeight = pObject->Height;

	PX_FontModuleDrawText(psurface, PX_Object_GetList(pListItem->pList)->fm, (px_int)objx + 3, (px_int)(objy + objHeight / 2), PX_ALIGN_LEFTMID, (const px_char*)pListItem->pdata, PX_OBJECT_UI_DEFAULT_FONTCOLOR);

}

px_bool PX_Designer_ListContentItemOnCreate(px_memorypool* mp, PX_Object* ItemObject, px_void* userptr)
{
	PX_ObjectSetRenderFunction(ItemObject, PX_Object_ListContentItemOnRender,0);
	return PX_TRUE;
}


PX_Object* PX_Object_ListContentCreate(px_memorypool* mp, PX_Object* Parent, px_int x, px_int y, px_int Width, px_int Height, PX_FontModule* fm)
{
	PX_Object* pObject;
	if (fm)
	{
		pObject=PX_Object_ListCreate(mp, Parent, x, y, Width, Height, fm->max_Height + 6, PX_Designer_ListContentItemOnCreate, PX_NULL);
		if (pObject)
		{
			PX_Object_List* pList = PX_Object_GetList(pObject);
			PX_ASSERTIF(pList);
			pList->fm = fm;
		}
	}
	else
	{
		pObject = PX_Object_ListCreate(mp, Parent, x, y, Width, Height, __PX_FONT_HEIGHT+6, PX_Designer_ListContentItemOnCreate,PX_NULL);
	}
	return pObject;
	
}

px_bool PX_Object_ListItemIsOnDisplayPresent(PX_Object* pObject, px_int index)
{

	PX_Object_List* pList = PX_Object_GetList(pObject);
	PX_Object* pItemObject = *PX_VECTORAT(PX_Object*, &pList->Items, index);
	PX_Object_ListItem* pItem = PX_Object_GetListItem(pItemObject);
	if (pItemObject->y + pItemObject->Height > 0 && pItemObject->y < pObject->Height && pItem->pdata)
	{
		return PX_TRUE;
	}
	return PX_FALSE;
}

px_int PX_Object_ListItemGetIndex(PX_Object* pObject)
{
	PX_Object_ListItem *pItem=PX_Object_GetListItem(pObject);
	PX_ASSERTIF(!pItem);
	return pItem->index;
}

px_void PX_Object_ListSetBackgroundColor(PX_Object *pListObject,px_color color)
{
	PX_Object_List *pList=PX_Object_GetList(pListObject);
	if (pList)
	{
		pList->BackgroundColor=color;
	}
}

px_void PX_Object_ListSetCursorColor(PX_Object* pListObject, px_color color)
{
	PX_Object_List* pList = PX_Object_GetList(pListObject);
	if (pList)
	{
		pList->CursorColor = color;
	}
}

px_void PX_Object_ListSetSelectColor(PX_Object* pListObject, px_color color)
{
	PX_Object_List* pList = PX_Object_GetList(pListObject);
	if (pList)
	{
		pList->SelectColor = color;
	}
}



px_void PX_Object_ListSetBorderColor(PX_Object *pListObject,px_color color)
{
	PX_Object_List *pList=PX_Object_GetList(pListObject);
	if (pList)
	{
		pList->BorderColor=color;
	}
}

px_void PX_Object_ListSetDoubleClickCancel(PX_Object *pListObject,px_bool b)
{
	PX_Object_List *pList=PX_Object_GetList(pListObject);
	if (pList)
	{
		pList->doubleCursorCancel=b;
	}
}

px_void PX_Object_ListViewFocus(PX_Object* pListObject, px_int index)
{
	PX_Object_List* pList = PX_Object_GetList(pListObject);
	if (!pList)
	{
		PX_ASSERT();
		return;
	}
	if (index>=0&&index<pList->pData.size)
	{
		px_int y = index  * pList->ItemHeight;
		if (y<pList->offsety||y+pList->ItemHeight>pList->offsety+pListObject->Height)
		{
			if (y + pList->ItemHeight <pListObject->Height)
			{
				pList->offsety = 0;
			}
			else if (y > pList->pData.size * pList->ItemHeight - pListObject->Height)
			{
				pList->offsety = (px_int)(pList->pData.size * pList->ItemHeight - pListObject->Height);
			}
			else
			{
				pList->offsety = (px_int)(y- pListObject->Height/2);
			}
			PX_Object_SliderBarSetValue(pList->SliderBar, pList->offsety);
		}
	}
}

px_void * PX_Object_ListGetItemData(PX_Object *pListObject,px_int index)
{
	PX_Object_List *pList=PX_Object_GetList(pListObject);
	if (index>=0&&index<pList->pData.size)
	{
		return *PX_VECTORAT(px_void *,&pList->pData,index);
	}
	return PX_NULL;
}

px_int PX_Object_ListGetItemCount(PX_Object* pListObject)
{
	PX_Object_List* pList = PX_Object_GetList(pListObject);
	PX_ASSERTIF(!pList);
	return pList->pData.size;
}

px_void* PX_Object_ListItemGetData(PX_Object* pItemObject)
{
	PX_Object_ListItem* pItem=PX_Object_GetListItem(pItemObject);
	PX_ASSERTIF(!pItem);
	return pItem->pdata;
}

px_void PX_Object_ListRemoveItem(PX_Object *pListObject,px_int index)
{
	PX_Object_List *pList=PX_Object_GetList(pListObject);
	PX_ASSERTIF(!pList);
	if (index>=0&&index<pList->pData.size)
	{
		PX_VectorErase(&pList->pData,index);
	}
	return ;
}


PX_OBJECT_RENDER_FUNCTION(PX_Object_ListArrayRender)
{
	PX_Object* pItemObject;
	px_int offsetX = 0, drawXCenter = 0;
	px_int i;
	px_int iy;
	PX_Object_List* pList = PX_Object_GetList(pObject);
	PX_Object_ListItem* pItem;
	px_float objx, objy, objWidth, objHeight;
	px_float inheritX, inheritY;

	PX_ObjectGetInheritXY(pObject, &inheritX, &inheritY);

	objx = (pObject->x + inheritX);
	objy = (pObject->y + inheritY);
	objWidth = pObject->Width;
	objHeight = pObject->Height;

	if (pList->Items.size != (px_int)(objHeight / (pList->ItemHeight) + 2))
	{
		PX_Object_ListItem Item;
		for (i = 0; i < pList->Items.size; i++)
		{
			PX_Object* pItemObject = *PX_VECTORAT(PX_Object*, &pList->Items, i);
			PX_ObjectDelete(pItemObject);
		}


		PX_VectorClear(&pList->Items);

		for (i = 0; i < (px_int)(objHeight / pList->ItemHeight + 2); i++)
		{
			Item.pList = pObject;
			Item.pdata = PX_NULL;
			pItemObject = PX_ObjectCreateEx(pList->mp, PX_NULL, 0, 0, 0, (px_float)pObject->Width, (px_float)pList->ItemHeight, 0, PX_OBJECT_TYPE_LISTITEM, PX_NULL, PX_NULL, PX_NULL, &Item, sizeof(PX_Object_ListItem));
			if (!pList->CreateFunctions(pList->mp, pItemObject, pList->userptr)) return;
			if (!PX_VectorPushback(&pList->Items, &pItemObject))return;
		}
	}


	if (pList->ItemHeight * pList->ArrayDataCount > objHeight)
	{
		PX_Object_SliderBar* pSliderBar = PX_Object_GetSliderBar(pList->SliderBar);
		pList->SliderBar->Visible = PX_TRUE;
		pList->SliderBar->y = 0;
		pList->SliderBar->x =  objWidth - PX_OBJECT_LIST_SLIDERBAR_WIDTH;
		pList->SliderBar->Height = objHeight;
		pSliderBar->SliderButtonLength = (px_int)(pList->SliderBar->Height * objHeight / (pList->ItemHeight * pList->ArrayDataCount));
		if (pSliderBar->SliderButtonLength < 32) pSliderBar->SliderButtonLength = 32;
		pSliderBar->Max = (pList->ItemHeight) * pList->ArrayDataCount - (px_int)objHeight;
		PX_Object_SliderBarSetBackgroundColor(pList->SliderBar, pList->BackgroundColor);
		PX_Object_SliderBarSetColor(pList->SliderBar, pList->BorderColor);
	}
	else
	{
		pList->SliderBar->Visible = PX_FALSE;
	}


	if (pList->offsety < 0)
	{
		iy = -pList->offsety;
	}
	else
	{
		iy = -(pList->offsety % pList->ItemHeight);
	}

	for (i = 0; i < pList->Items.size; i++)
	{

		pItemObject = *PX_VECTORAT(PX_Object*, &pList->Items, i);
		pItemObject->x = 0;
		pItemObject->y = (px_float)iy;
		pItemObject->Width = pObject->Width;
		pItem = PX_Object_GetListItem(pItemObject);
		if (pList->offsety < 0)
		{
			if (i < pList->ArrayDataCount)
			{
				pItem->pdata = pList->pArrayData;
				pItem->index = i;
				PX_ObjectUpdate(pItemObject, elapsed);
			}
			else
			{
				pItem->pdata = PX_NULL;
			}
		}
		else
		{
			if (i + pList->offsety / pList->ItemHeight < pList->ArrayDataCount)
			{
				px_int index = i + pList->offsety / pList->ItemHeight;
				pItem->pdata = pList->pArrayData;
				pItem->index = index;
				PX_ObjectUpdate(pItemObject, elapsed);
			}
			else
			{
				pItem->pdata = PX_NULL;
			}
		}
		iy += (px_int)pItemObject->Height;
	}

	if ((px_int)pList->renderSurface.width != (px_int)pObject->Width || (px_int)pList->renderSurface.height != (px_int)pObject->Height)
	{
		PX_SurfaceFree(&pList->renderSurface);
		if (!PX_SurfaceCreate(pList->mp, (px_int)pObject->Width, (px_int)pObject->Height, &pList->renderSurface))
		{
			PX_ASSERT();
			return;
		}
	}

	PX_SurfaceClear(&pList->renderSurface, 0, 0, pList->renderSurface.width - 1, pList->renderSurface.height - 1, pList->BackgroundColor);
	for (i = 0; i < pList->Items.size; i++)
	{
		px_int index = i + pList->offsety / pList->ItemHeight;
		pItemObject = *PX_VECTORAT(PX_Object*, &pList->Items, i);
		pItem = PX_Object_GetListItem(pItemObject);
		if (pItemObject->y + pItemObject->Height > 0 && pItemObject->y < pObject->Height && pItem->pdata)
		{

			if (pList->showCursor)
			{
				if (index == pList->currentCursorIndex && index != pList->currentSelectedIndex)
				{
					PX_GeoDrawRect(&pList->renderSurface, (px_int)pItemObject->x, (px_int)pItemObject->y, (px_int)(pItemObject->x + pItemObject->Width - 1), (px_int)(pItemObject->y + pItemObject->Height - 1), pList->CursorColor);
				}
			}
			if (index == pList->currentSelectedIndex)
			{
				PX_GeoDrawRect(&pList->renderSurface, (px_int)pItemObject->x, (px_int)pItemObject->y, (px_int)(pItemObject->x + pItemObject->Width - 1), (px_int)(pItemObject->y + pItemObject->Height - 1), pList->SelectColor);
			}
			PX_ObjectRender(&pList->renderSurface, pItemObject, elapsed);
		}
	}
	PX_GeoDrawBorder(&pList->renderSurface, 0, 0, pList->renderSurface.width - 1, pList->renderSurface.height - 1, 1, pList->BorderColor);
	PX_SurfaceCover(psurface, &pList->renderSurface, (px_int)objx, (px_int)objy, PX_ALIGN_LEFTTOP);

}

px_bool PX_Designer_ListArrayItemOnCreate(px_memorypool* mp, PX_Object* ItemObject, px_void* userptr)
{
	PX_Object_ListItem* pItem = PX_Object_GetListItem(ItemObject);
	PX_Object_List* pList =PX_Object_GetList(pItem->pList);

	PX_ObjectSetRenderFunction(ItemObject, pList->ArrayRender,0);
	return PX_TRUE;
}


PX_Object* PX_Object_ListArrayCreate(px_memorypool* mp, PX_Object* Parent, px_int x, px_int y, px_int Width, px_int Height, px_int ItemHeight, Function_ObjectRender render, px_void* userptr)
{
	PX_Object* pObject;

	pObject = PX_Object_ListCreate(mp, Parent, x, y, Width, Height, ItemHeight, PX_Designer_ListArrayItemOnCreate, PX_NULL);
	if (pObject)
	{
		PX_Object_List* pList = PX_Object_GetList(pObject);
		pList->ArrayRender = render;
		PX_ObjectSetRenderFunction(pObject, PX_Object_ListArrayRender, 0);
		return pObject;
	}
	return PX_NULL;
}

px_void PX_Object_ListArraySetData(PX_Object* pListObject, px_void* ArrayData, px_int ArrayDataCount)
{

	PX_Object_List* pList = PX_Object_GetList(pListObject);
	pList->ArrayDataCount = ArrayDataCount;
	pList->pArrayData = ArrayData;

}

px_void *PX_Object_ListArrayItemGetData(PX_Object* pListItemObject)
{

	PX_Object_ListItem* pItem = PX_Object_GetListItem( pListItemObject);
	return pItem->pdata;

}

