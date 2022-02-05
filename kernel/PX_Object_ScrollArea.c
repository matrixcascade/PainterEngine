#include "PX_Object_ScrollArea.h"

px_void PX_Object_ScrollArea_EventDispatcher(PX_Object *Object,PX_Object_Event e,px_void *user_ptr)
{
	PX_Object_ScrollArea *pSA=PX_Object_GetScrollArea(Object);
	px_float objx,objy,objWidth,objHeight;
	px_float inheritX,inheritY;

	PX_ObjectGetInheritXY(Object,&inheritX,&inheritY);

	objx=(Object->x+inheritX);
	objy=(Object->y+inheritY);
	objWidth=Object->Width;
	objHeight=Object->Height;

	if (!pSA)
	{
		return;
	}

	if (e.Event==PX_OBJECT_EVENT_EXECUTE||e.Event==PX_OBJECT_EVENT_CURSORDRAG||e.Event==PX_OBJECT_EVENT_CURSORDOWN||e.Event==PX_OBJECT_EVENT_CURSORUP||e.Event==PX_OBJECT_EVENT_CURSORMOVE)
	{

		if (!PX_ObjectIsPointInRegion(Object,PX_Object_Event_GetCursorX(e),PX_Object_Event_GetCursorY(e)))
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
	PX_ObjectAddClild(PX_Object_ScrollAreaGetIncludedObjects(parent),child);
	PX_Object_ScrollAreaUpdateRange(parent);
}

px_void PX_Object_ScrollAreaHSliderChanged(PX_Object *pObject,PX_Object_Event e,px_void *ptr)
{
	px_int value=PX_Object_SliderBarGetValue(pObject);
	PX_Object_ScrollArea *pSA=(PX_Object_ScrollArea *)ptr;
	pSA->root->x=-(px_float)value;
}

px_void PX_Object_ScrollAreaVSliderChanged(PX_Object *pObject,PX_Object_Event e,px_void *ptr)
{
	px_int value=PX_Object_SliderBarGetValue(pObject);
	PX_Object_ScrollArea *pSA=(PX_Object_ScrollArea *)ptr;
	pSA->root->y=-(px_float)value;
}

PX_Object * PX_Object_ScrollAreaCreate(px_memorypool *mp,PX_Object *Parent,px_int x,px_int y,px_int Width,px_int Height)
{
	PX_Object *pObject;
	PX_Object_ScrollArea *pSA=(PX_Object_ScrollArea *)MP_Malloc(mp,sizeof(PX_Object_ScrollArea));

	if (pSA==PX_NULL)
	{
		return PX_NULL;
	}
	PX_memset(pSA,0,sizeof(PX_Object_ScrollArea));

	if (Height<=24||Width<=24)
	{
		return PX_NULL;
	}

	pObject=PX_ObjectCreate(mp,Parent,(px_float)x,(px_float)y,0,(px_float)Width,(px_float)Height,0);

	if (pObject==PX_NULL)
	{
		return PX_NULL;
	}

	if (!pObject)
	{
		MP_Free(mp,pSA);
		return PX_NULL;
	}

	pObject->pObject=pSA;
	pObject->Type=PX_OBJECT_TYPE_SCROLLAREA;
	pObject->ReceiveEvents=PX_TRUE;
	pObject->Func_ObjectFree=PX_Object_ScrollAreaFree;
	pObject->Func_ObjectRender=PX_Object_ScrollAreaRender;

	pSA->BackgroundColor=PX_COLOR(0,0,0,0);
	pSA->bBorder=PX_TRUE;
	pSA->borderColor=PX_COLOR(255,0,0,0);
	//root
	pSA->root=PX_ObjectCreate(pObject->mp,0,0,0,0,0,0,0);
	pSA->hscroll=PX_Object_SliderBarCreate(mp,pObject,0,0,16,16,PX_OBJECT_SLIDERBAR_TYPE_HORIZONTAL,PX_OBJECT_SLIDERBAR_STYLE_BOX);
	pSA->vscroll=PX_Object_SliderBarCreate(mp,pObject,0,0,16,16,PX_OBJECT_SLIDERBAR_TYPE_VERTICAL,PX_OBJECT_SLIDERBAR_STYLE_BOX);
	if(!PX_SurfaceCreate(mp,Width,Height,&pSA->surface))
	{
		MP_Free(mp,pSA);
		MP_Free(mp,pObject);
		return PX_NULL;
	}
	PX_ObjectRegisterEvent(pObject,PX_OBJECT_EVENT_ANY,PX_Object_ScrollArea_EventDispatcher,PX_NULL);
	PX_ObjectRegisterEvent(pSA->hscroll,PX_OBJECT_EVENT_VALUECHANGED,PX_Object_ScrollAreaHSliderChanged,pSA);
	PX_ObjectRegisterEvent(pSA->vscroll,PX_OBJECT_EVENT_VALUECHANGED,PX_Object_ScrollAreaVSliderChanged,pSA);

	pObject->Func_ObjectLinkChild=PX_Object_ScrollAreaLinkChild;
	PX_Object_ScrollAreaUpdateRange(pObject);
	return pObject;
}

PX_Object * PX_Object_ScrollAreaGetIncludedObjects(PX_Object *pObj)
{
	PX_Object_ScrollArea *pSA;
	pSA=PX_Object_GetScrollArea(pObj);
	if (pSA)
	{
		return pSA->root;
	}
	return PX_NULL;
}

px_void PX_Object_ScrollAreaMoveToBottom(PX_Object *pObject)
{
	px_float left=0,top=0,right=0,bottom=0;
	PX_Object_ScrollArea *psa=PX_Object_GetScrollArea(pObject);
	if(psa)
	{
		PX_Object_ScrollAreaGetRegion(psa->root,&left,&top,&right,&bottom);
		if (bottom-top>=pObject->Height)
		{
			psa->root->y=-(bottom-top-pObject->Height);
		}
		else
		{
			psa->root->y=0;
		}
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

	if (pObject==PX_NULL||pObject->Visible==PX_FALSE)
	{
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

	PX_Object_ScrollAreaGetRegion(pObject->pNextBrother,left,top,right,bottom);
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
		pSA->hscroll->y=objHeight;
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

		pSA->vscroll->x=objWidth;
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

px_void PX_Object_ScrollAreaRender(px_surface *psurface, PX_Object *pObject,px_uint elapsed)
{
	PX_Object_ScrollArea *pSA;
	px_float objx,objy,objWidth,objHeight;
	px_float inheritX,inheritY;


	PX_ObjectGetInheritXY(pObject,&inheritX,&inheritY);
	objx=(pObject->x+inheritX);
	objy=(pObject->y+inheritY);
	objWidth=pObject->Width;
	objHeight=pObject->Height;

	pSA=PX_Object_GetScrollArea(pObject);
	if (!pSA)
	{
		return;
	}


	if (pSA->surface.height!=(px_int)objHeight||pSA->surface.width!=(px_int)objWidth)
	{
		PX_SurfaceFree(&pSA->surface);
		if(!PX_SurfaceCreate(pObject->mp,(px_int)objWidth,(px_int)objHeight,&pSA->surface))
		{
			return;
		}
		PX_Object_ScrollAreaUpdateRange(pObject);
	}


	PX_SurfaceClear(&pSA->surface,0,0,(px_int)objWidth-1,(px_int)objHeight-1,pSA->BackgroundColor);

	//draw
	PX_ObjectRender(&pSA->surface,pSA->root,elapsed);
	PX_SurfaceRender(psurface,&pSA->surface,(px_int)objx,(px_int)objy,PX_ALIGN_LEFTTOP,PX_NULL);

	//Draw Border
	if(pSA->bBorder)
		PX_GeoDrawBorder(&pSA->surface,0,0,(px_int)objWidth-1,(px_int)objHeight-1,1,pSA->borderColor);
	PX_SurfaceRender(psurface,&pSA->surface,(px_int)objx,(px_int)objy,PX_ALIGN_LEFTTOP,PX_NULL);
}

px_void PX_Object_ScrollAreaFree(PX_Object *pObj)
{
	PX_Object_ScrollArea *pSA;
	pSA=PX_Object_GetScrollArea(pObj);
	if (pSA)
	{
		PX_ObjectDelete(pSA->root);
		PX_SurfaceFree(&pSA->surface);
	}
}

px_void PX_Object_ScrollAreaSetBkColor(PX_Object *pObj,px_color bkColor)
{
	PX_Object_ScrollArea *pSA;
	pSA=PX_Object_GetScrollArea(pObj);
	if (pSA)
	{
		pSA->BackgroundColor=bkColor;
	}
}

px_void PX_Object_ScrollAreaSetBorder(PX_Object *pObj,px_bool Border)
{
	PX_Object_ScrollArea *pSA;
	pSA=PX_Object_GetScrollArea(pObj);
	if (pSA)
	{
		pSA->bBorder=Border;
	}
}

px_void PX_Object_ScrollAreaSetBorderColor(PX_Object *pObj,px_color borderColor)
{
	PX_Object_ScrollArea *pSA;
	pSA=PX_Object_GetScrollArea(pObj);
	if (pSA)
	{
		pSA->borderColor=borderColor;
	}
}

PX_Object_ScrollArea * PX_Object_GetScrollArea(PX_Object *Object)
{
	if(Object->Type==PX_OBJECT_TYPE_SCROLLAREA)
		return (PX_Object_ScrollArea *)Object->pObject;
	else
		return PX_NULL;
}
