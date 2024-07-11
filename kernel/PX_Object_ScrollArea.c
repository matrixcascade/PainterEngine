#include "PX_Object_ScrollArea.h"

PX_OBJECT_EVENT_FUNCTION(PX_Object_ScrollArea_EventDispatcher)
{
	PX_Object_ScrollArea *pSA=PX_Object_GetScrollArea(pObject);
	px_float objx,objy,objWidth,objHeight;
	px_float inheritX,inheritY;

	PX_ObjectGetInheritXY(pObject,&inheritX,&inheritY);

	objx=(pObject->x+inheritX);
	objy=(pObject->y+inheritY);
	objWidth=pObject->Width;
	objHeight=pObject->Height;

	if (!pSA)
	{
		return;
	}

	if (e.Event==PX_OBJECT_EVENT_EXECUTE||e.Event==PX_OBJECT_EVENT_CURSORDRAG||e.Event==PX_OBJECT_EVENT_CURSORDOWN||e.Event==PX_OBJECT_EVENT_CURSORUP||e.Event==PX_OBJECT_EVENT_CURSORMOVE)
	{

		if (!PX_ObjectIsPointInRegion(pObject,PX_Object_Event_GetCursorX(e),PX_Object_Event_GetCursorY(e)))
		{
			return;
		}
		PX_Object_Event_SetCursorX(&e,PX_Object_Event_GetCursorX(e)-objx);
		PX_Object_Event_SetCursorY(&e,PX_Object_Event_GetCursorY(e)-objy);
	}
	PX_ObjectPostEvent(pSA->root,e);
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
		if (!PX_SurfaceCreate(pObject->mp, (px_int)objWidth, (px_int)objHeight, &pSA->surface))
		{
			return;
		}
		PX_Object_ScrollAreaUpdateRange(pObject);
	}


	PX_SurfaceClear(&pSA->surface, 0, 0, (px_int)objWidth - 1, (px_int)objHeight - 1, pSA->BackgroundColor);

	//draw
	PX_ObjectRender(&pSA->surface, pSA->root, elapsed);
	PX_SurfaceRender(psurface, &pSA->surface, (px_int)objx, (px_int)objy, PX_ALIGN_LEFTTOP, PX_NULL);

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


	pSA->BackgroundColor = PX_COLOR(0, 0, 0, 0);
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
	px_float inheritX,inheritY;

	if (pObject==PX_NULL)
	{
		return;
	}
	if (pObject->Visible == PX_FALSE)
	{
		PX_Object_ScrollAreaGetRegion(pObject->pNextBrother, left, top, right, bottom);
		return;
	}

	PX_ObjectGetInheritXY(pObject,&inheritX,&inheritY);

	objx=(pObject->x+inheritX);
	objy=(pObject->y+inheritY);
	objWidth=pObject->Width;
	objHeight=pObject->Height;

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
	px_float inheritX,inheritY;
	px_float left=0,top=0,right=0,bottom=0;
	PX_Object_ScrollArea *pSA;


	pSA=PX_Object_GetScrollArea(pObject);
	pSA->root->x = 0;
	pSA->root->y = 0;

	PX_ObjectGetInheritXY(pObject,&inheritX,&inheritY);
	objx=(pObject->x+inheritX);
	objy=(pObject->y+inheritY);
	objWidth=pObject->Width;
	objHeight=pObject->Height;

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
			PX_Object_SliderBarSetRange(pSA->hscroll,(px_int)left,(px_int)(right-objWidth));
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
				PX_Object_SliderBarSetRange(pSA->vscroll, (px_int)top, (px_int)(bottom - objHeight+36));
			else
				PX_Object_SliderBarSetRange(pSA->vscroll, (px_int)top, (px_int)(bottom - objHeight));
			PX_Object_SliderBarSetValue(pSA->vscroll, (px_int)top);
		}
		else
		{
			pSA->vscroll->Visible=PX_FALSE;
			PX_Object_SliderBarSetValue(pSA->vscroll, 0);
		}
		
	} while (0);
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
