#include "PX_Object_Selectbar.h"

//////////////////////////////////////////////////////////////////////////
//
px_void PX_Object_SelectBarSliderOnChanged(PX_Object* pObject, PX_Object_Event e, px_void* ptr)
{
	PX_Object_SelectBar* pSelectbar = (PX_Object_SelectBar*)ptr;
	pSelectbar->currentDisplayOffsetIndex = PX_Object_SliderBarGetValue(pObject);
}

PX_Object_SelectBar *PX_Object_GetSelectBar(PX_Object *pSelecrBar)
{
	if (pSelecrBar->Type==PX_OBJECT_TYPE_SELECTBAR)
	{
		return (PX_Object_SelectBar *)pSelecrBar->pObject;
	}
	return PX_NULL;
}
static px_void PX_SelecrbarClearCurrentCursor(PX_Object_SelectBar *pSelectbar)
{
	px_int i;
	for (i=0;i<pSelectbar->Items.size;i++)
	{
		PX_VECTORAT(PX_Object_SelectBar_Item,&pSelectbar->Items,i)->onCursor=PX_FALSE;
	}
}
static px_void PX_SelectbarOnCursorMove(PX_Object *pObject,px_float x,px_float y)
{
	PX_Object_SelectBar *pSelectbar=PX_Object_GetSelectBar(pObject);
	px_float objx,objy,objWidth,objHeight;
	px_float inheritX,inheritY;

	PX_ObjectGetInheritXY(pObject,&inheritX,&inheritY);

	objx=(pObject->x+inheritX);
	objy=(pObject->y+inheritY);
	objWidth=pObject->Width;
	objHeight=pObject->Height;


	if(pSelectbar->activating)
	{
		px_int index;
		if (pSelectbar->Items.size>pSelectbar->maxDisplayCount)
		{
			if (x<objx||x>objx+objWidth-16)
			{
				return;
			}
		}
		else
		{
			if (x<objx||x>objx+objWidth)
			{
				return;
			}
		}

		index=(px_int)((y-objy-objHeight)/pSelectbar->ItemHeight);
		PX_SelecrbarClearCurrentCursor(pSelectbar);

		if (y<objy+objHeight||index<0||index>=pSelectbar->maxDisplayCount||index>=pSelectbar->Items.size)
		{
			return;
		}
		else
		{
			PX_VECTORAT(PX_Object_SelectBar_Item,&pSelectbar->Items,index+pSelectbar->currentDisplayOffsetIndex)->onCursor=PX_TRUE;
		}
	}
	else
	{
		if(PX_isPointInRect(PX_POINT(x,y,0),PX_RECT(objx,objy,objWidth,objHeight)))
		{
			pSelectbar->onCursor=PX_TRUE;
		}
		else
		{
			pSelectbar->onCursor=PX_FALSE;
		}
	}
}
static px_void PX_SelectbarOnCursorDown(PX_Object *pObject,px_float x,px_float y)
{
	PX_Object_SelectBar *pSelectbar=PX_Object_GetSelectBar(pObject);
	px_float objx,objy,objWidth,objHeight;
	px_float inheritX,inheritY;

	PX_ObjectGetInheritXY(pObject,&inheritX,&inheritY);

	objx=(pObject->x+inheritX);
	objy=(pObject->y+inheritY);
	objWidth=pObject->Width;
	objHeight=pObject->Height;

	if(pSelectbar->activating)
	{
		px_int index;
		if (pSelectbar->Items.size>pSelectbar->maxDisplayCount)
		{
			if (x<objx||x>objx+objWidth)
			{
				PX_SelecrbarClearCurrentCursor(pSelectbar);
				pSelectbar->onCursor=PX_FALSE;
				pSelectbar->activating=PX_FALSE;
				PX_ObjectClearFocus(pObject);
				return;
			}
			if (x>=objx+objWidth-16&&x<=objx+objWidth)
			{
				return;
			}
		}
		else
		{
			if (x<objx||x>objx+objWidth)
			{
				PX_SelecrbarClearCurrentCursor(pSelectbar);
				pSelectbar->activating=PX_FALSE;
				PX_ObjectClearFocus(pObject);
				pSelectbar->onCursor=PX_FALSE;
				return;
			}
		}

		index=(px_int)((y-objy-objHeight)/pSelectbar->ItemHeight);
		PX_SelecrbarClearCurrentCursor(pSelectbar);
		if (y<objy+objHeight||index<0||index>=pSelectbar->maxDisplayCount||index>=pSelectbar->Items.size)
		{
			pSelectbar->activating=PX_FALSE;
			PX_ObjectClearFocus(pObject);
			pSelectbar->onCursor=PX_FALSE;
			return;
		}
		else
		{
			pSelectbar->activating=PX_FALSE;
			PX_ObjectClearFocus(pObject);
			pSelectbar->selectIndex=index+pSelectbar->currentDisplayOffsetIndex;
			do 
			{
				PX_Object_Event e;
				e.Event=PX_OBJECT_EVENT_VALUECHANGED;
				PX_Object_Event_SetIndex(&e,index);
				PX_ObjectExecuteEvent(pObject,e);
			} while (0);
		}
		pSelectbar->onCursor=PX_FALSE;
	}
	else
	{
		if (pSelectbar->Items.size==0)
		{
			return;
		}

		if(PX_isPointInRect(PX_POINT(x,y,0),PX_RECT(objx,objy,objWidth,objHeight)))
		{
			pSelectbar->activating=PX_TRUE;
			PX_ObjectSetFocus(pObject);
		}
	}

}

static px_void PX_SelectbarOnCursorWheel(PX_Object *pObject,px_float z)
{
	PX_Object_SelectBar *pSelectbar=PX_Object_GetSelectBar(pObject);
	if (pSelectbar->activating)
	{
		if (pSelectbar->Items.size>pSelectbar->maxDisplayCount)
		{
			if (z<0)
			{
				PX_Object_SliderBarSetValue(pSelectbar->sliderBar,PX_Object_SliderBarGetValue(pSelectbar->sliderBar)+1);
			}
			else
			{
				PX_Object_SliderBarSetValue(pSelectbar->sliderBar,PX_Object_SliderBarGetValue(pSelectbar->sliderBar)-1);
			}
		}
	}
}
static px_void PX_SelectbarOnCursorEvent(PX_Object *pSelectBarObject,PX_Object_Event e,px_void *ptr)
{
	switch(e.Event)
	{
	case PX_OBJECT_EVENT_CURSORMOVE:
	case PX_OBJECT_EVENT_CURSORDRAG:
		{
			PX_SelectbarOnCursorMove(pSelectBarObject,PX_Object_Event_GetCursorX(e),PX_Object_Event_GetCursorY(e));
		}
		break;
	case PX_OBJECT_EVENT_CURSORDOWN:
		{
			PX_SelectbarOnCursorDown(pSelectBarObject,PX_Object_Event_GetCursorX(e),PX_Object_Event_GetCursorY(e));
		}
		break;
	case PX_OBJECT_EVENT_CURSORWHEEL:
		{
		PX_Object_SelectBar* pDesc = PX_ObjectGetDesc(PX_Object_SelectBar, pSelectBarObject);
			PX_SelectbarOnCursorWheel(pSelectBarObject,PX_Object_Event_GetCursorZ(e));
			PX_Object_SelectBarSliderOnChanged(pDesc->sliderBar, PX_OBJECT_BUILD_EVENT(PX_OBJECT_EVENT_VALUECHANGED), pDesc);
		}
		break;
	default:
		break;
	}
}
static px_void PX_SelectbarFree(PX_Object *pObject)
{
	PX_VectorFree(&PX_Object_GetSelectBar(pObject)->Items);
}
static px_void PX_SelectbarRender(px_surface *pRenderSurface,PX_Object *pObject,px_dword elpase)
{
	PX_Object_SelectBar *pSelectBar=PX_Object_GetSelectBar(pObject);
	px_float objx,objy,objWidth,objHeight;
	px_float inheritX,inheritY;

	PX_ObjectGetInheritXY(pObject,&inheritX,&inheritY);

	objx=(pObject->x+inheritX);
	objy=(pObject->y+inheritY);
	objWidth=pObject->Width;
	objHeight=pObject->Height;




	//border
	if (pSelectBar->style==PX_OBJECT_SELECTBAR_STYLE_RECT)
	{
		//background
		if (pSelectBar->onCursor)
		{
			PX_GeoDrawRect(pRenderSurface,(px_int)objx,(px_int)objy,(px_int)(objx+objWidth-1),(px_int)(objy+objHeight-1),pSelectBar->cursorColor);
		}
		else
		{
			PX_GeoDrawRect(pRenderSurface,(px_int)objx,(px_int)objy,(px_int)(objx+objWidth-1),(px_int)(objy+objHeight-1),pSelectBar->backgroundColor);
		}

		PX_GeoDrawBorder(pRenderSurface,(px_int)objx,(px_int)objy,(px_int)(objx+objWidth-1),(px_int)(objy+objHeight-1),1,pSelectBar->borderColor);

	}
	else if (pSelectBar->style==PX_OBJECT_SELECTBAR_STYLE_ROUNDRECT)
	{
		if (pSelectBar->onCursor)
		{
			PX_GeoDrawSolidRoundRect(pRenderSurface,(px_int)objx,(px_int)objy,(px_int)(objx+objWidth-1),(px_int)(objy+objHeight-1),objHeight/2,pSelectBar->cursorColor);
		}
		else
		{
			PX_GeoDrawSolidRoundRect(pRenderSurface,(px_int)objx,(px_int)objy,(px_int)(objx+objWidth-1),(px_int)(objy+objHeight-1),objHeight/2,pSelectBar->backgroundColor);
		}

		PX_GeoDrawRoundRect(pRenderSurface,(px_int)objx,(px_int)objy,(px_int)(objx+objWidth-1),(px_int)(objy+objHeight-1),objHeight/2,1,pSelectBar->borderColor);
	}

	//font
	do 
	{
		if (pSelectBar->selectIndex>=0&&pSelectBar->selectIndex<pSelectBar->Items.size)
		{
			PX_Object_SelectBar_Item *pItem=PX_VECTORAT(PX_Object_SelectBar_Item,&pSelectBar->Items,pSelectBar->selectIndex);
			PX_FontModuleDrawText(pRenderSurface,pSelectBar->fontmodule,(px_int)(objx+objHeight/2+1),(px_int)(objy+objHeight/2),PX_ALIGN_LEFTMID,pItem->Text,pSelectBar->fontColor);
		}
	} while (0);

	//triangle
	PX_GeoDrawTriangle(pRenderSurface,
		PX_POINT2D(objx+objWidth-16,objy+objHeight/2-3),
		PX_POINT2D(objx+objWidth-4,objy+objHeight/2-3),
		PX_POINT2D(objx+objWidth-10,objy+objHeight/2+3),
		pSelectBar->borderColor
		);

	if (pSelectBar->activating)
	{
		px_int i;
		px_int count=pSelectBar->maxDisplayCount>pSelectBar->Items.size?pSelectBar->Items.size:pSelectBar->maxDisplayCount;

		for (i=0;i<count;i++)
		{
			PX_Object_SelectBar_Item *pItem=PX_VECTORAT(PX_Object_SelectBar_Item,&pSelectBar->Items,i+pSelectBar->currentDisplayOffsetIndex);

			if (pItem->onCursor)
			{
				PX_GeoDrawRect(pRenderSurface,
					(px_int)(objx),
					(px_int)(objy+objHeight+i*pSelectBar->ItemHeight),
					(px_int)(objx+objWidth),
					(px_int)(objy+objHeight+(i+1)*pSelectBar->ItemHeight),
					pSelectBar->cursorColor
					);
			}
			else
			{
				PX_GeoDrawRect(pRenderSurface,
					(px_int)(objx),
					(px_int)(objy+objHeight+i*pSelectBar->ItemHeight),
					(px_int)(objx+objWidth),
					(px_int)(objy+objHeight+(i+1)*pSelectBar->ItemHeight),
					pSelectBar->backgroundColor
					);
			}

			PX_FontModuleDrawText(pRenderSurface,pSelectBar->fontmodule,(px_int)(objx+objHeight/2+1),
				(px_int)(objy+objHeight+(i*pSelectBar->ItemHeight)+pSelectBar->ItemHeight/2),
				PX_ALIGN_LEFTMID,
				pItem->Text,
				pSelectBar->fontColor
				);
		}
		PX_GeoDrawBorder(pRenderSurface,
			(px_int)objx,
			(px_int)(objy+objHeight),
			(px_int)(objx+objWidth-1),
			(px_int)(objy+objHeight+count*pSelectBar->ItemHeight),
			1,
			pSelectBar->borderColor
			);

		if (pSelectBar->Items.size>pSelectBar->maxDisplayCount)
		{
			pSelectBar->sliderBar->Visible=PX_TRUE;
			pSelectBar->sliderBar->x=objWidth-16;
			pSelectBar->sliderBar->y=objHeight;
			pSelectBar->sliderBar->Width=16;
			pSelectBar->sliderBar->Height=count*pSelectBar->ItemHeight*1.0f;
			PX_Object_SliderBarSetBackgroundColor(pSelectBar->sliderBar,pSelectBar->backgroundColor);
			PX_Object_SliderBarSetColor(pSelectBar->sliderBar,pSelectBar->borderColor);
			PX_Object_SliderBarSetRange(pSelectBar->sliderBar,0,pSelectBar->Items.size-pSelectBar->maxDisplayCount);
			PX_Object_SliderBarSetSliderButtonLength(pSelectBar->sliderBar,(px_int)(pSelectBar->sliderBar->Height*pSelectBar->maxDisplayCount/pSelectBar->Items.size));
		}
		else
		{
			pSelectBar->sliderBar->Visible=PX_FALSE;
		}
	}
	else
	{
		pSelectBar->sliderBar->Visible=PX_FALSE;
	}

}



PX_Object * PX_Object_SelectBarCreate(px_memorypool *mp,PX_Object *Parent,px_int x,int y,px_int width,px_int height,PX_FontModule *fontmodule)
{
	PX_Object *pObject;
	PX_Object_SelectBar *pSelectbar=(PX_Object_SelectBar *)MP_Malloc(mp,sizeof(PX_Object_SelectBar));

	if (pSelectbar==PX_NULL)
	{
		return PX_NULL;
	}

	PX_memset(pSelectbar,0,sizeof(PX_Object_SelectBar));
	pSelectbar->fontmodule=fontmodule;
	pSelectbar->mp=mp;
	pSelectbar->activating=PX_FALSE;
	pSelectbar->currentDisplayOffsetIndex=0;
	if (fontmodule)
	{
		pSelectbar->ItemHeight=fontmodule->max_Height;
	}
	else
	{
		pSelectbar->ItemHeight=__PX_FONT_HEIGHT;
	}

	pSelectbar->maxDisplayCount=16;
	pSelectbar->backgroundColor= PX_OBJECT_UI_DEFAULT_BACKGROUNDCOLOR;
	pSelectbar->cursorColor= PX_OBJECT_UI_DEFAULT_CURSORCOLOR;
	pSelectbar->fontColor=PX_OBJECT_UI_DEFAULT_FONTCOLOR;
	pSelectbar->borderColor= PX_OBJECT_UI_DEFAULT_BORDERCOLOR;
	pSelectbar->activating=PX_FALSE;
	pSelectbar->style=PX_OBJECT_SELECTBAR_STYLE_RECT;
	PX_VectorInitialize(mp,&pSelectbar->Items,sizeof(PX_Object_SelectBar_Item),16);


	pObject=PX_ObjectCreate(mp,Parent,(px_float)x,(px_float)y,0,(px_float)width,(px_float)height,0);
	if (pObject==PX_NULL)
	{
		MP_Free(pObject->mp,pSelectbar);
		return PX_NULL;
	}


	pObject->pObject=pSelectbar;
	pObject->Enabled=PX_TRUE;
	pObject->Visible=PX_TRUE;
	pObject->Type=PX_OBJECT_TYPE_SELECTBAR;
	pObject->Func_ObjectFree=PX_SelectbarFree;
	pObject->Func_ObjectRender=PX_SelectbarRender;
	pObject->OnLostFocusReleaseEvent=PX_TRUE;

	PX_ObjectRegisterEvent(pObject,PX_OBJECT_EVENT_CURSORDOWN,PX_SelectbarOnCursorEvent,PX_NULL);
	PX_ObjectRegisterEvent(pObject,PX_OBJECT_EVENT_CURSORMOVE,PX_SelectbarOnCursorEvent,PX_NULL);
	PX_ObjectRegisterEvent(pObject,PX_OBJECT_EVENT_CURSORDRAG,PX_SelectbarOnCursorEvent,PX_NULL);
	PX_ObjectRegisterEvent(pObject,PX_OBJECT_EVENT_CURSORWHEEL,PX_SelectbarOnCursorEvent,PX_NULL);

	pSelectbar->sliderBar=PX_Object_SliderBarCreate(mp,pObject,0,0,0,0,PX_OBJECT_SLIDERBAR_TYPE_VERTICAL,PX_OBJECT_SLIDERBAR_STYLE_BOX);

	if (!pSelectbar->sliderBar)
	{
		PX_ObjectDelete(pObject);
		return PX_NULL;
	}
	PX_ObjectRegisterEvent(pSelectbar->sliderBar,PX_OBJECT_EVENT_VALUECHANGED,PX_Object_SelectBarSliderOnChanged,pSelectbar);
	return pObject;
}

px_int PX_Object_SelectBarAddItem(PX_Object *pSelectBarObject,const px_char Text[])
{
	PX_Object_SelectBar *pSelectBar=PX_Object_GetSelectBar(pSelectBarObject);
	PX_Object_SelectBar_Item item;
	PX_strcpy(item.Text,Text,sizeof(item.Text));
	item.onCursor=PX_FALSE;
	if(PX_VectorPushback(&pSelectBar->Items,&item))
		return pSelectBar->Items.size-1;
	else
		return -1;
}

px_void PX_Object_SelectBarClear(PX_Object* pSelectBarObject)
{
	PX_Object_SelectBar* pSelectBar = PX_Object_GetSelectBar(pSelectBarObject);
	PX_VectorClear(&pSelectBar->Items);
}

px_void PX_Object_SelectBarRemoveItem(PX_Object *pObject,px_int index)
{
	PX_Object_SelectBar *pSelectBar=PX_Object_GetSelectBar(pObject);
	if (index>=0&&index<pSelectBar->Items.size)
	{
		PX_VectorErase(&pSelectBar->Items,index);
	}
}

px_int PX_Object_SelectBarGetItemIndexByText(PX_Object *pObject,const px_char Text[])
{
	PX_Object_SelectBar *pSelectBar=PX_Object_GetSelectBar(pObject);
	px_int i;
	for (i=0;i<pSelectBar->Items.size;i++)
	{
		PX_Object_SelectBar_Item *pItem=PX_VECTORAT(PX_Object_SelectBar_Item,&pSelectBar->Items,i);
		if (PX_strequ(Text,pItem->Text))
		{
			return i;
		}
	}
	return -1;
}

const px_char * PX_Object_SelectBarGetCurrentText(PX_Object *pObject)
{
	PX_Object_SelectBar *pSelectBar=PX_Object_GetSelectBar(pObject);
	if (pSelectBar)
	{
		return PX_VECTORAT(PX_Object_SelectBar_Item,&pSelectBar->Items,pSelectBar->selectIndex)->Text;
	}
	return "";
}


const px_char* PX_Object_SelectBarGetItemText(PX_Object* pObject,px_int index)
{
	PX_Object_SelectBar* pSelectBar = PX_Object_GetSelectBar(pObject);
	if (!PX_VectorCheckIndex(&pSelectBar->Items,index))
	{
		return "";
	}
	if (pSelectBar)
	{
		return PX_VECTORAT(PX_Object_SelectBar_Item, &pSelectBar->Items, index)->Text;
	}
	return "";
}

px_void PX_Object_SelectBarSetDisplayCount(PX_Object *pObject,px_int count)
{
	PX_Object_SelectBar *pSelectBar=PX_Object_GetSelectBar(pObject);
	if (pSelectBar)
	{
		pSelectBar->maxDisplayCount=count;
	}
}

px_int PX_Object_SelectBarGetCurrentIndex(PX_Object *pObject)
{
	PX_Object_SelectBar *pSelectBar=PX_Object_GetSelectBar(pObject);
	if (pSelectBar)
	{
		return pSelectBar->selectIndex;
	}
	return -1;
}

px_void PX_Object_SelectBarSetCurrentIndex(PX_Object *pObject,px_int index)
{
	PX_Object_SelectBar *pSelectBar=PX_Object_GetSelectBar(pObject);
	if (pSelectBar)
	{
		if (index>=0&&index<pSelectBar->Items.size)
		{
			pSelectBar->selectIndex=index;
		}
	}
}

px_void PX_Object_SelectBarSetStyle(PX_Object *pObject,PX_OBJECT_SELECTBAR_STYLE style)
{
	PX_Object_SelectBar *pSelectBar=PX_Object_GetSelectBar(pObject);
	if (pSelectBar)
	{
		pSelectBar->style=style;
	}
}

px_void PX_Object_SelectBarSetFontColor(PX_Object *pObject,px_color color)
{
	PX_Object_SelectBar *pSelectBar=PX_Object_GetSelectBar(pObject);
	if (pSelectBar)
	{
		pSelectBar->fontColor=color;
	}
}

px_void PX_Object_SelectBarSetCursorColor(PX_Object *pObject,px_color color)
{
	PX_Object_SelectBar *pSelectBar=PX_Object_GetSelectBar(pObject);
	if (pSelectBar)
	{
		pSelectBar->cursorColor=color;
	}
}

px_void PX_Object_SelectBarSetBorderColor(PX_Object *pObject,px_color color)
{
	PX_Object_SelectBar *pSelectBar=PX_Object_GetSelectBar(pObject);
	if (pSelectBar)
	{
		pSelectBar->borderColor=color;
	}
}

px_void PX_Object_SelectBarSetBackgroundColor(PX_Object *pObject,px_color color)
{
	PX_Object_SelectBar *pSelectBar=PX_Object_GetSelectBar(pObject);
	if (pSelectBar)
	{
		pSelectBar->backgroundColor=color;
	}
}

px_void PX_Object_SelectBarSetMaxDisplayCount(PX_Object* pObject, px_int i)
{
	PX_Object_SelectBar* pSelectBar = PX_Object_GetSelectBar(pObject);
	if (pSelectBar)
	{
		pSelectBar->maxDisplayCount = i;
	}
}

