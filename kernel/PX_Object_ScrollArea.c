#include "PX_Object_ScrollArea.h"

PX_OBJECT_EVENT_FUNCTION(PX_Object_ScrollArea_EventDispatcher)
{
	PX_Object_ScrollArea *pSA=PX_Object_GetScrollArea(pObject);
	px_float objx,objy,objWidth,objHeight;
	px_rect rect;

	rect = PX_ObjectGetRect(pObject);
	objx = rect.x;
	objy = rect.y;
	objWidth = rect.width;
	objHeight = rect.height;

	if (!pSA)
	{
		return;
	}

	e=PX_Object_Event_CursorOffset(e, PX_POINT(-objx, -objy, 0));
	PX_ObjectPostEvent(pSA->root,e);
	if (pSA->root->OnFocus)
	{
		if (!pObject->OnFocus)
		{
			PX_ObjectSetFocus(pObject);
		}
	}
	else
	{
		if (pObject->OnFocus)
		{
			PX_ObjectClearFocus(pObject);
		}
	}

}

px_void  PX_Object_ScrollAreaLinkChild(PX_Object *parent,PX_Object *child)
{
	PX_ObjectAddChild(PX_Object_ScrollAreaGetIncludedObjects(parent),child);
	PX_Object_ScrollAreaUpdateRange(parent);
}

PX_OBJECT_EVENT_FUNCTION(PX_Object_ScrollAreaHSliderChanged)
{
	px_int value=PX_Object_SliderBarGetValue(pObject);
	PX_Object_ScrollArea *pSA=(PX_Object_ScrollArea *)ptr;
	pSA->root->x=-(px_float)value;
}

PX_OBJECT_EVENT_FUNCTION(PX_Object_ScrollAreaVSliderChanged)
{
	px_int value=PX_Object_SliderBarGetValue(pObject);
	PX_Object_ScrollArea *pSA=(PX_Object_ScrollArea *)ptr;
	pSA->root->y=-(px_float)value;
}

PX_OBJECT_EVENT_FUNCTION(PX_Object_ScrollAreaWheel)
{
	PX_Object_ScrollArea *pSA=PX_Object_GetScrollArea(pObject);
	px_int minz,maxz;
	if (!pSA->vscroll->Visible)
	{
		return;
	}
	if (pSA->root->OnFocus)
	{
		return;
	}
	PX_Object_SliderBarGetRange(pSA->vscroll,&minz,&maxz);

	if (PX_ObjectIsCursorInRegion(pObject, e))
	{
		px_float z = PX_Object_Event_GetCursorZ(e);
		pSA->root->y += z / 5;
		if (-pSA->root->y<minz)
		{
			pSA->root->y = -minz*1.f;
		}

		if (-pSA->root->y> maxz)
		{
			pSA->root->y = -maxz*1.f;
		}

		PX_Object_SliderBarSetValue(pSA->vscroll, -(px_int)pSA->root->y);
		return;
	}
}

PX_OBJECT_RENDER_FUNCTION(PX_Object_ScrollAreaRender)
{
	PX_Object_ScrollArea* pSA;
	px_float objx, objy, objWidth, objHeight;
	px_float inheritX, inheritY;


	PX_ObjectGetInheritXY(pObject, &inheritX, &inheritY);
	objx = (pObject->x + inheritX);
	objy = (pObject->y + inheritY);
	objWidth = pObject->Width;
	objHeight = pObject->Height;

	pSA = PX_Object_GetScrollArea(pObject);
	if (!pSA)
	{
		return;
	}


	if (pSA->surface.height != (px_int)objHeight || pSA->surface.width != (px_int)objWidth)
	{
		PX_SurfaceFree(&pSA->surface);
		if (objWidth==0)
		{
			objWidth = 1;
		}
		if (objHeight == 0)
		{
			objHeight = 1;
		}
		if (!PX_SurfaceCreate(pObject->mp, (px_int)objWidth, (px_int)objHeight, &pSA->surface))
		{
			return;
		}
		PX_Object_ScrollAreaUpdateRange(pObject);
	}


	PX_SurfaceClear(&pSA->surface, 0, 0, (px_int)objWidth - 1, (px_int)objHeight - 1, pSA->BackgroundColor);

	//draw
	PX_ObjectRender(&pSA->surface, pSA->root, elapsed);
	//Draw Border
	if (pSA->bBorder)
		PX_GeoDrawBorder(&pSA->surface, 0, 0, (px_int)objWidth - 1, (px_int)objHeight - 1, 1, pSA->borderColor);
	PX_SurfaceRender(psurface, &pSA->surface, (px_int)objx, (px_int)objy, PX_ALIGN_LEFTTOP, PX_NULL);
}

PX_OBJECT_FREE_FUNCTION(PX_Object_ScrollAreaFree)
{
	PX_Object_ScrollArea* pSA;
	pSA = PX_Object_GetScrollArea(pObject);
	PX_ObjectDelete(pSA->root);
	PX_SurfaceFree(&pSA->surface);
}


PX_Object* PX_Object_ScrollAreaAttachObject(PX_Object* pObject,px_int attachIndex, px_int x, px_int y, px_int Width, px_int Height)
{
	px_memorypool* mp = pObject->mp;

	PX_Object_ScrollArea* pSA;

	if (Height <= 24 || Width <= 24)
	{
		return PX_NULL;
	}

	PX_ASSERTIF(pObject == PX_NULL);
	PX_ASSERTIF(attachIndex < 0 || attachIndex >= PX_COUNTOF(pObject->pObjectDesc));
	PX_ASSERTIF(pObject->pObjectDesc[attachIndex] != PX_NULL);
	pSA = (PX_Object_ScrollArea*)PX_ObjectCreateDesc(pObject, attachIndex, PX_OBJECT_TYPE_SCROLLAREA, 0, PX_Object_ScrollAreaRender, PX_Object_ScrollAreaFree, 0, sizeof(PX_Object_ScrollArea));
	PX_ASSERTIF(pSA == PX_NULL);


	pSA->BackgroundColor = PX_COLOR_BACKGROUNDCOLOR;
	pSA->bBorder = PX_TRUE;
	pSA->borderColor = PX_COLOR(255, 0, 0, 0);
	//root
	pSA->root = PX_ObjectCreate(pObject->mp, 0, 0, 0, 0, 0, 0, 0);
	pSA->hscroll = PX_Object_SliderBarCreate(mp, pObject, 0, 0, 16, 16, PX_OBJECT_SLIDERBAR_TYPE_HORIZONTAL, PX_OBJECT_SLIDERBAR_STYLE_BOX);
	pSA->vscroll = PX_Object_SliderBarCreate(mp, pObject, 0, 0, 16, 16, PX_OBJECT_SLIDERBAR_TYPE_VERTICAL, PX_OBJECT_SLIDERBAR_STYLE_BOX);
	if (!PX_SurfaceCreate(mp, Width, Height, &pSA->surface))
	{
		return PX_NULL;
	}
	PX_ObjectRegisterEventEx(pObject, PX_OBJECT_EVENT_ANY, PX_OBJECT_TYPE_SCROLLAREA, PX_Object_ScrollArea_EventDispatcher, PX_NULL);
	PX_ObjectRegisterEventEx(pSA->hscroll, PX_OBJECT_EVENT_VALUECHANGED, PX_OBJECT_TYPE_SCROLLAREA, PX_Object_ScrollAreaHSliderChanged, pSA);
	PX_ObjectRegisterEventEx(pSA->vscroll, PX_OBJECT_EVENT_VALUECHANGED, PX_OBJECT_TYPE_SCROLLAREA, PX_Object_ScrollAreaVSliderChanged, pSA);
	pSA->scrollAction= PX_ObjectRegisterEventEx(pObject, PX_OBJECT_EVENT_CURSORWHEEL, PX_OBJECT_TYPE_SCROLLAREA, PX_Object_ScrollAreaWheel, PX_NULL);
	pSA->scrollAction->enable = PX_FALSE;
	pObject->Func_ObjectLinkChild = PX_Object_ScrollAreaLinkChild;
	PX_Object_ScrollAreaUpdateRange(pObject);
	return pObject;
}

PX_Object * PX_Object_ScrollAreaCreate(px_memorypool *mp,PX_Object *Parent,px_int x,px_int y,px_int Width,px_int Height)
{
	PX_Object *pObject;

	if (Height <= 24 || Width <= 24)
	{
		return PX_NULL;
	}

	pObject=PX_ObjectCreate(mp,Parent,(px_float)x,(px_float)y,0,(px_float)Width,(px_float)Height,0);
	if (pObject ==PX_NULL)
	{
		return PX_NULL;
	}

	if(!PX_Object_ScrollAreaAttachObject(pObject,0,x,y,Width,Height))
	{
		PX_ObjectDelete(pObject);
		return PX_NULL;
	}
	
	return pObject;
}

PX_Object * PX_Object_ScrollAreaGetIncludedObjects(PX_Object *pObj)
{
	PX_Object_ScrollArea *pSA;
	pSA=PX_Object_GetScrollArea(pObj);
	return pSA->root;
}

px_bool PX_Object_ScrollAreaSetBackgroundColor(PX_Object* pObj, px_color color)
{
	PX_Object_ScrollArea *pSA;
	pSA=PX_Object_GetScrollArea(pObj);
	pSA->BackgroundColor=color;
	return PX_TRUE;
}

px_void PX_Object_ScrollAreaScrollEnable(PX_Object* pObj, px_bool bEnable)
{
	PX_Object_ScrollArea* pSA;
	pSA = PX_Object_GetScrollArea(pObj);
	pSA->scrollAction->enable=bEnable;
}

//get background color
px_color PX_Object_ScrollAreaGetBackgroundColor(PX_Object* pObj)
{
	PX_Object_ScrollArea *pSA;
	pSA=PX_Object_GetScrollArea(pObj);
	return pSA->BackgroundColor;
}

px_void PX_Object_ScrollAreaMoveToBottom(PX_Object *pObject)
{
	px_float left=0,top=0,right=0,bottom=0;
	PX_Object_ScrollArea *psa=PX_Object_GetScrollArea(pObject);

		PX_Object_ScrollAreaGetRegion(psa->root,&left,&top,&right,&bottom);
		
		if (bottom-top>=pObject->Height)
		{
			psa->root->y=-(bottom-top-pObject->Height);
			PX_Object_SliderBarSetValue(psa->vscroll, -(px_int)psa->root->y);
		}
		else
		{
			psa->root->y=0;
		}
}

px_void PX_Object_ScrollAreaMoveToTop(PX_Object *pObject)
{
	px_float left=0,top=0,right=0,bottom=0;
	PX_Object_ScrollArea *psa=PX_Object_GetScrollArea(pObject);
	PX_Object_ScrollAreaGetRegion(psa->root,&left,&top,&right,&bottom);
	if(psa)
		psa->root->y=top;
}

px_void PX_Object_ScrollAreaGetRegion(PX_Object *pObject,px_float *left,px_float *top,px_float *right,px_float *bottom)
{
	px_float objx,objy,objWidth,objHeight;
	px_rect rect;

	if (pObject==PX_NULL)
	{
		return;
	}
	if (pObject->Visible == PX_FALSE)
	{
		PX_Object_ScrollAreaGetRegion(pObject->pNextBrother, left, top, right, bottom);
		return;
	}

	rect = PX_ObjectGetRect(pObject);
	objx = rect.x;
	objy = rect.y;
	objWidth = rect.width;
	objHeight = rect.height;

	if (objx<*left)
	{
		*left=objx;
	}


	if (objx+objWidth-1>*right)
	{
		*right=objx+objWidth;
	}

	if (objy<*top)
	{
		*top=objy;
	}

	if (objy+objHeight-1>*bottom)
	{
		*bottom=objy+objHeight;
	}

	PX_Object_ScrollAreaGetRegion(pObject->pNextBrother, left, top, right, bottom);
	PX_Object_ScrollAreaGetRegion(pObject->pChilds,left,top,right,bottom);
}

px_void PX_Object_ScrollAreaUpdateRange( PX_Object *pObject)
{
	px_float objx,objy,objWidth,objHeight;
	px_rect rect;
	px_float left=0,top=0,right=0,bottom=0;
	PX_Object_ScrollArea *pSA;


	pSA=PX_Object_GetScrollArea(pObject);
	pSA->root->x = 0;
	pSA->root->y = 0;

	rect = PX_ObjectGetRect(pObject);
	objx = rect.x;
	objy = rect.y;
	objWidth = rect.width;
	objHeight = rect.height;

	PX_Object_ScrollAreaGetRegion(pSA->root,&left,&top,&right,&bottom);

	do 
	{	
		px_float rWidth=right-left;
		pSA->hscroll->x=0;
		pSA->hscroll->y=objHeight-16;
		pSA->hscroll->Width=pObject->Width;
		pSA->hscroll->Height=16;
		if (rWidth>pObject->Width)
		{
			if (pObject->Width!=0)
			{
				px_float btnW=pObject->Width/rWidth*pObject->Width;
				pSA->hscroll->Visible=PX_TRUE;
				PX_Object_SliderBarSetSliderButtonLength(pSA->hscroll,(px_int)btnW);
			}
			PX_Object_SliderBarSetRange(pSA->hscroll,(px_int)left,(px_int)(right-objWidth+32));
			PX_Object_SliderBarSetValue(pSA->hscroll, (px_int)left);
		}
		else
		{
			pSA->hscroll->Visible=PX_FALSE;
			PX_Object_SliderBarSetValue(pSA->hscroll, 0);
		}
	} while (0);

	do 
	{	
		px_float rHeight=bottom-top;

		pSA->vscroll->x=objWidth-16;
		pSA->vscroll->y=0;
		pSA->vscroll->Width=16;
		pSA->vscroll->Height=objHeight;
		if (rHeight>pObject->Height)
		{
			if (pObject->Height!=0)
			{
				px_float btnH=pObject->Height/rHeight*pObject->Height;
				pSA->vscroll->Visible=PX_TRUE;
				PX_Object_SliderBarSetSliderButtonLength(pSA->vscroll,(px_int)btnH);
			}
			if(pSA->hscroll->Visible)
				PX_Object_SliderBarSetRange(pSA->vscroll, (px_int)top, (px_int)(bottom - objHeight+36+128));
			else
				PX_Object_SliderBarSetRange(pSA->vscroll, (px_int)top, (px_int)(bottom - objHeight+128));
			PX_Object_SliderBarSetValue(pSA->vscroll, (px_int)top);
		}
		else
		{
			pSA->vscroll->Visible=PX_FALSE;
			PX_Object_SliderBarSetValue(pSA->vscroll, 0);
		}
		
	} while (0);
}

px_void PX_Object_ScrollAreaVerticalReorder(PX_Object* pScrollAreaObject,px_int spacer)
{
	PX_Object_ScrollArea *pSA;
	PX_Object *pObject;
	px_float ofty = spacer*1.f;
	pSA=PX_Object_GetScrollArea(pScrollAreaObject);
	if (!pSA->root)
	{
		PX_ASSERT();
		return;
	}
	pObject=pSA->root->pChilds;
	while (pObject)
	{
		if (pObject->Visible)
		{
			pObject->y = ofty;
			ofty += pObject->Height + spacer;
		}
		pObject = pObject->pNextBrother;
	}
	PX_Object_ScrollAreaUpdateRange(pScrollAreaObject);
}

px_void PX_Object_ScrollAreaHorizontalReorder(PX_Object* pObject, px_int spacer)
{
	PX_Object_ScrollArea *pSA;
	PX_Object *pChild;
	px_float oftx = spacer*1.f;
	pSA=PX_Object_GetScrollArea(pObject);
	if (!pSA->root)
	{
		PX_ASSERT();
		return;
	}
	pChild=pSA->root->pChilds;
	while (pChild)
	{
		if (pChild->Visible)
		{
			pChild->x = oftx;
			oftx += pChild->Width + spacer;
		}
		pChild = pChild->pNextBrother;
	}
	PX_Object_ScrollAreaUpdateRange(pObject);
	
}


px_void PX_Object_ScrollAreaClear(PX_Object* pObj)
{
	PX_Object_ScrollArea *pSA;
	pSA=PX_Object_GetScrollArea(pObj);

	PX_ObjectDelete(pSA->root);
	pSA->root=PX_ObjectCreate(pObj->mp,PX_NULL,0,0,0,0,0,0);
}

px_void PX_Object_ScrollAreaSetBkColor(PX_Object *pObj,px_color bkColor)
{
	PX_Object_ScrollArea *pSA;
	pSA=PX_Object_GetScrollArea(pObj);
	pSA->BackgroundColor=bkColor;
}

px_void PX_Object_ScrollAreaSetBorder(PX_Object *pObj,px_bool Border)
{
	PX_Object_ScrollArea *pSA;
	pSA=PX_Object_GetScrollArea(pObj);
	pSA->bBorder=Border;
}

px_void PX_Object_ScrollAreaSetBorderColor(PX_Object *pObj,px_color borderColor)
{
	PX_Object_ScrollArea *pSA;
	pSA=PX_Object_GetScrollArea(pObj);
	pSA->borderColor=borderColor;
}

PX_Object_ScrollArea * PX_Object_GetScrollArea(PX_Object *pObject)
{
	PX_Object_ScrollArea*pdesc=(PX_Object_ScrollArea*)PX_ObjectGetDescByType(pObject,PX_OBJECT_TYPE_SCROLLAREA);
    PX_ASSERTIF(pdesc==PX_NULL);
	return pdesc;
}
