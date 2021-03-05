#include "PX_Object_List.h"
PX_Object_List  * PX_Object_GetList( PX_Object *Object )
{
	if(Object->Type==PX_OBJECT_TYPE_LIST)
		return (PX_Object_List *)Object->pObject;
	else
		return PX_NULL;
}

PX_Object_ListItem  * PX_Object_GetListItem( PX_Object *Object )
{
	if(Object->Type==PX_OBJECT_TYPE_LISTITEM)
		return (PX_Object_ListItem *)Object->pObject;
	else
		return PX_NULL;
}

px_void PX_Object_ListOnCursorMove(PX_Object *pObject,PX_Object_Event e,px_void *ptr)
{

	PX_Object_List *pList= PX_Object_GetList(pObject);
	px_float objx,objy,objWidth,objHeight;
	px_float inheritX,inheritY;


	PX_ObjectGetInheritXY(pObject,&inheritX,&inheritY);

	objx=(pObject->x+inheritX);
	objy=(pObject->y+inheritY);
	objWidth=pObject->Width;
	objHeight=pObject->Height;
	pList->currentCursorIndex=-1;
	if (!PX_ObjectIsCursorInRegion(pObject,e))
	{
		return;
	}

	if (pList->pData.size*pList->ItemHeight>pObject->Height)
	{
		if (PX_Object_Event_GetCursorX(e)>objx+objWidth-32)
		{
			return;
		}
	}


	pList->currentCursorIndex=(px_int)(PX_Object_Event_GetCursorY(e)+pList->offsety-objy)/pList->ItemHeight;
	if(pList->currentCursorIndex<0||pList->currentCursorIndex>=pList->pData.size)
	{
		pList->currentCursorIndex=-1;
	}

}

px_void PX_Object_ListOnCursorDown(PX_Object *pObject,PX_Object_Event e,px_void *ptr)
{
	PX_Object_List *pList= PX_Object_GetList(pObject);
	px_float objx,objy,objWidth,objHeight;
	px_float inheritX,inheritY;
	px_int currentSelectedIndex;
	PX_Object_Event ne;
	PX_ObjectGetInheritXY(pObject,&inheritX,&inheritY);

	objx=(pObject->x+inheritX);
	objy=(pObject->y+inheritY);
	objWidth=pObject->Width;
	objHeight=pObject->Height;

	if (!PX_ObjectIsCursorInRegion(pObject,e))
	{
		return;
	}
	if (pList->pData.size*pList->ItemHeight>pObject->Height)
	{
		if (PX_Object_Event_GetCursorX(e)>objx+objWidth-32)
		{
			return;
		}
	}

	currentSelectedIndex=(px_int)(PX_Object_Event_GetCursorY(e)+pList->offsety-objy)/pList->ItemHeight;
	if (pList->currentSelectedIndex!=currentSelectedIndex)
	{
		PX_memset(&ne,0,sizeof(ne));
		pList->currentSelectedIndex=currentSelectedIndex;
		if(pList->currentSelectedIndex<0||pList->currentSelectedIndex>=pList->pData.size)
		{
			pList->currentSelectedIndex=-1;
		}
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
	}
}

px_void PX_Object_ListOnWheel(PX_Object *pObject,PX_Object_Event e,px_void *ptr)
{
	PX_Object_List *pList= PX_Object_GetList(pObject);
	px_float objx,objy,objWidth,objHeight;
	px_float inheritX,inheritY;

	PX_ObjectGetInheritXY(pObject,&inheritX,&inheritY);

	objx=(pObject->x+inheritX);
	objy=(pObject->y+inheritY);
	objWidth=pObject->Width;
	objHeight=pObject->Height;

	if (!PX_ObjectIsCursorInRegion(pObject,e))
	{
		return;
	}
	pList->offsety-=(px_int)PX_Object_Event_GetCursorZ(e)/5;


	if (pList->offsety>(pList->ItemHeight)*pList->pData.size-objHeight)
	{
		pList->offsety=(pList->ItemHeight)*pList->pData.size-(px_int)objHeight;
	}

	if (pList->offsety<0)
	{
		pList->offsety=0;
	}

	PX_Object_SliderBarSetValue(pList->SliderBar,pList->offsety);
}

px_void PX_Object_ListRender(px_surface *psurface, PX_Object *pObject,px_uint elpased)
{
	PX_Object *pItemObject;
	px_int offsetX=0,drawXCenter=0;
	px_int i;
	px_int iy;
	PX_Object_List *pList= PX_Object_GetList(pObject);
	PX_Object_ListItem *pItem;
	px_float objx,objy,objWidth,objHeight;
	px_float inheritX,inheritY;

	PX_ObjectGetInheritXY(pObject,&inheritX,&inheritY);

	objx=(pObject->x+inheritX);
	objy=(pObject->y+inheritY);
	objWidth=pObject->Width;
	objHeight=pObject->Height;


	if (pList->ItemHeight*pList->pData.size>objHeight)
	{
		PX_Object_SliderBar *pSliderBar=PX_Object_GetSliderBar(pList->SliderBar);
		pList->SliderBar->Visible=PX_TRUE;
		pList->SliderBar->y=objy;
		pList->SliderBar->x=objx+objWidth-32;
		pSliderBar->SliderButtonLength=(px_int)(pList->SliderBar->Height*objHeight/(pList->ItemHeight*pList->pData.size));
		if(pSliderBar->SliderButtonLength<32) pSliderBar->SliderButtonLength=32;
		pSliderBar->Max=(pList->ItemHeight)*pList->pData.size-(px_int)objHeight+32;
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
		pItem=PX_Object_GetListItem(pItemObject);
		if (pList->offsety<0)
		{
			if(i<pList->pData.size)
			{
				pItem->pdata=*PX_VECTORAT(px_void *,&pList->pData,i);
				PX_ObjectUpdate(pItemObject,elpased);
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
				pItem->pdata=*PX_VECTORAT(px_void *,&pList->pData,i+pList->offsety/pList->ItemHeight);
				PX_ObjectUpdate(pItemObject,elpased);
			}
			else
			{
				pItem->pdata=PX_NULL;
			}
		}
		iy+=(px_int)pItemObject->Height;
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
				}
			}
			if (index==pList->currentSelectedIndex)
			{
				PX_GeoDrawRect(&pList->renderSurface,(px_int)pItemObject->x,(px_int)pItemObject->y,(px_int)(pItemObject->x+pItemObject->Width-1),(px_int)(pItemObject->y+pItemObject->Height-1),pList->SelectCursor);
			}
			PX_ObjectRender(&pList->renderSurface,pItemObject,elpased);
		}
	}
	PX_GeoDrawBorder(&pList->renderSurface,0,0,pList->renderSurface.width-1,pList->renderSurface.height-1,1,pList->BorderColor);
	PX_SurfaceCover(psurface,&pList->renderSurface,(px_int)objx,(px_int)objy,PX_TEXTURERENDER_REFPOINT_LEFTTOP);

}

px_void PX_Object_ListClear(PX_Object *pListObj)
{
	PX_Object_List  *pList=PX_Object_GetList(pListObj);
	if(pList)
		PX_VectorClear(&pList->pData);
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

px_int PX_Object_ListAdd(PX_Object *pListObj,px_void *ptr)
{
	PX_Object_List  *pList=PX_Object_GetList(pListObj);
	if(pList)
	{
		if(PX_VectorPushback(&pList->pData,&ptr))
			return pList->pData.size-1;
	}
	return -1;
}

px_void PX_Object_ListFree(PX_Object *pListObj)
{
	PX_Object_List *pList=PX_Object_GetList(pListObj);
	if (!pList)
	{
		PX_ASSERT();
		return;
	}
	PX_Object_ListClear(pListObj);
	PX_SurfaceFree(&pList->renderSurface);
	PX_VectorFree(&pList->Items);
}

px_void PX_Object_ListOnSliderValueChanged(PX_Object  *Object,PX_Object_Event e,px_void *ptr)
{
	px_int value=PX_Object_SliderBarGetValue(Object);
	PX_Object_List *pList=PX_Object_GetList((PX_Object *)ptr);
	pList->offsety=value;
}

px_void PX_Object_ListOnEvent(PX_Object  *Object,PX_Object_Event e,px_void *ptr)
{
	px_int i;
	PX_Object_List  *pList=PX_Object_GetList(Object);

	switch (e.Event)
	{
	case PX_OBJECT_EVENT_CURSORWHEEL:
		if (!PX_ObjectIsPointInRegion(Object,PX_Object_Event_GetCursorX(e),PX_Object_Event_GetCursorY(e)))
		{
			return;
		}
	case PX_OBJECT_EVENT_CURSORDRAG:
	case PX_OBJECT_EVENT_CURSORRDOWN:
	case PX_OBJECT_EVENT_CURSORDOWN:
	case PX_OBJECT_EVENT_CURSORUP:
	case PX_OBJECT_EVENT_CURSORRUP:
	case PX_OBJECT_EVENT_CURSORMOVE:
		PX_Object_Event_SetCursorX(&e,PX_Object_Event_GetCursorX(e)-Object->x);
		PX_Object_Event_SetCursorY(&e,PX_Object_Event_GetCursorY(e)-Object->y);
		break;
	}

	for (i=0;i<pList->Items.size;i++)
	{
		PX_Object *pItem=*PX_VECTORAT(PX_Object *,&pList->Items,i);
		PX_ObjectPostEvent(pItem,e);
	}
}

PX_Object * PX_Object_ListCreate(px_memorypool *mp, PX_Object *Parent,px_int x,px_int y,px_int Width,px_int Height,px_int ItemHeight,PX_Object_ListItemOnCreate _CreateFunc,px_void *userptr)
{
	px_int i;
	PX_Object_List List,*pList;
	PX_Object_ListItem Item;
	PX_Object *pListObject,*pItemObject;
	PX_memset(&List,0,sizeof(List));
	List.mp=mp;
	List.ItemWidth=Width-32;
	List.ItemHeight=ItemHeight;
	List.CreateFunctions=_CreateFunc;

	List.BorderColor=PX_OBJECT_UI_DEFAULT_BORDERCOLOR;
	List.BackgroundColor=PX_OBJECT_UI_DEFAULT_BACKGROUNDCOLOR;
	List.CursorColor=PX_OBJECT_UI_DEFAULT_CURSORCOLOR;
	List.SelectCursor=PX_OBJECT_UI_DEFAULT_PUSHCOLOR;

	List.offsetx=0;
	List.currentSelectedIndex=-1;
	List.offsety=0;
	List.showCursor=PX_TRUE;

	PX_VectorInitialize(mp,&List.Items,sizeof(PX_Object *),64);
	PX_VectorInitialize(mp,&List.pData,sizeof(px_void *),32);
	if(!PX_SurfaceCreate(mp,Width,Height,&List.renderSurface)) return PX_NULL;
	if(!(pListObject=PX_ObjectCreateEx(mp,Parent,(px_float)x,(px_float)y,0,(px_float)Width,(px_float)Height,0,PX_OBJECT_TYPE_LIST,PX_NULL,PX_Object_ListRender,PX_Object_ListFree,&List,sizeof(PX_Object_List)))) return PX_NULL;

	pList=PX_Object_GetList(pListObject);
	pList->SliderBar=PX_Object_SliderBarCreate(mp,pListObject,(px_int)pListObject->Width-32,0,32,(px_int)pListObject->Height,PX_OBJECT_SLIDERBAR_TYPE_VERTICAL,PX_OBJECT_SLIDERBAR_STYLE_BOX);

	for (i=0;i<Height/ItemHeight+2;i++)
	{
		pItemObject=PX_ObjectCreateEx(mp,PX_NULL,0,0,0,(px_float)pListObject->Width,(px_float)ItemHeight,0,PX_OBJECT_TYPE_LISTITEM,PX_NULL,PX_NULL,PX_NULL,&Item,sizeof(PX_Object_ListItem));
		if(!_CreateFunc(mp,pItemObject,userptr)) return PX_FALSE;
		if(!PX_VectorPushback(&pList->Items,&pItemObject))return PX_FALSE;
	}

	PX_ObjectRegisterEvent(pListObject,PX_OBJECT_EVENT_CURSORWHEEL,PX_Object_ListOnWheel,pListObject);
	PX_ObjectRegisterEvent(pListObject,PX_OBJECT_EVENT_CURSORDOWN,PX_Object_ListOnCursorDown,pListObject);
	PX_ObjectRegisterEvent(pListObject,PX_OBJECT_EVENT_CURSORMOVE,PX_Object_ListOnCursorMove,pListObject);
	PX_ObjectRegisterEvent(pList->SliderBar,PX_OBJECT_EVENT_VALUECHANGED,PX_Object_ListOnSliderValueChanged,pListObject);
	PX_ObjectRegisterEvent(pListObject,PX_OBJECT_EVENT_ANY,PX_Object_ListOnEvent,pListObject);
	return pListObject;
}

px_void PX_Object_ListSetBackgroundColor(PX_Object *pListObject,px_color color)
{
	PX_Object_List *pList=PX_Object_GetList(pListObject);
	if (pList)
	{
		pList->BackgroundColor=color;
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

px_void * PX_Object_ListGetItemData(PX_Object *pListObject,px_int index)
{
	PX_Object_List *pList=PX_Object_GetList(pListObject);
	if (index>0&&index<pList->pData.size)
	{
		return PX_VECTORAT(px_void *,&pList->pData,index);
	}
	return PX_NULL;
}

px_void PX_Object_ListRemoveItem(PX_Object *pListObject,px_int index)
{
	PX_Object_List *pList=PX_Object_GetList(pListObject);
	if (index>0&&index<pList->pData.size)
	{
		PX_VectorErase(&pList->pData,index);
	}
	return ;
}

