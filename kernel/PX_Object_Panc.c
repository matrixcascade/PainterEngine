#include "PX_Object_Panc.h"
#include "PX_Object_Panc.h"

px_void PX_Object_PancRender(px_surface *pSurface,PX_Object *pObject,px_dword elapsed)
{
	PX_Object_Panc *pDesc=PX_ObjectGetDesc(PX_Object_Panc,pObject);
	px_int i;
	px_float step;
	px_color renderColor;
	
	px_float objx,objy,objWidth,objHeight;
	px_float inheritX,inheritY;

	PX_ObjectGetInheritXY(pObject,&inheritX,&inheritY);

	objx=(pObject->x+inheritX);
	objy=(pObject->y+inheritY);
	objWidth=pObject->Width;
	objHeight=pObject->Height;

	//////////////////////////////////////////////////////////////////////////
	//source
	//x

	renderColor=pDesc->borderColor;
	renderColor._argb.a/=6;

	step=pDesc->sourceX-objx;
	step=step/4;
	for (i=0;i<5;i++)
	{
		PX_GeoDrawLine(pSurface,(px_int)(objx+step*i), (px_int)objy,(px_int)(objx+step*i),(px_int)(objy+objHeight),1,renderColor);
	}

	step=objx+objWidth-pDesc->sourceX;
	step=step/4;
	for (i=0;i<5;i++)
	{
		PX_GeoDrawLine(pSurface,(px_int)(pDesc->sourceX+step*i), (px_int)objy,(px_int)(pDesc->sourceX+step*i),(px_int)(objy+objHeight),1,renderColor);
	}

	//y
	step=pDesc->sourceY-objy;
	step=step/4;
	for (i=0;i<5;i++)
	{
		PX_GeoDrawLine(pSurface,(px_int)objx,(px_int)(objy+step*i),(px_int)(objx+objWidth),(px_int)(objy+step*i),1,renderColor);
	}

	step=objy+objHeight-pDesc->sourceY;
	step=step/4;
	for (i=0;i<5;i++)
	{
		PX_GeoDrawLine(pSurface, (px_int)objx,(px_int)(pDesc->sourceY+step*i),(px_int)(objx+objWidth),(px_int)(pDesc->sourceY+step*i),1,renderColor);
	}


	//////////////////////////////////////////////////////////////////////////
	//current
	//x

	renderColor=pDesc->borderColor;

	step=pDesc->currentX-objx;
	step=step/4;
	for (i=0;i<5;i++)
	{
		PX_GeoDrawLine(pSurface,(px_int)(objx+step*i), (px_int)objy,(px_int)(objx+step*i),(px_int)(objy+objHeight),1,renderColor);
	}

	step=objx+objWidth-pDesc->currentX;
	step=step/4;
	for (i=0;i<5;i++)
	{
		PX_GeoDrawLine(pSurface,(px_int)(pDesc->currentX+step*i), (px_int)objy,(px_int)(pDesc->currentX+step*i),(px_int)(objy+objHeight),1,renderColor);
	}

	//y
	step=pDesc->currentY-objy;
	step=step/4;
	for (i=0;i<5;i++)
	{
		PX_GeoDrawLine(pSurface, (px_int)objx,(px_int)(objy+step*i),(px_int)(objx+objWidth),(px_int)(objy+step*i),1,renderColor);
	}

	step=objy+objHeight-pDesc->currentY;
	step=step/4;
	for (i=0;i<5;i++)
	{
		PX_GeoDrawLine(pSurface, (px_int)objx,(px_int)(pDesc->currentY+step*i),(px_int)(objx+objWidth),(px_int)(pDesc->currentY+step*i),1,renderColor);
	}

	//////////////////////////////////////////////////////////////////////////
	//controller point
	renderColor=pDesc->borderColor;
	renderColor._argb.a/=6;
	PX_GeoDrawSolidCircle(pSurface,(px_int)pDesc->currentX,(px_int)pDesc->currentY,8,renderColor);

	PX_GeoDrawSolidCircle(pSurface,(px_int)(objx),(px_int)(objy),5,renderColor);
	PX_GeoDrawSolidCircle(pSurface,(px_int)(objx+objWidth),(px_int)(objy),5,renderColor);
	PX_GeoDrawSolidCircle(pSurface,(px_int)(objx),(px_int)(objy+objHeight),5,renderColor);
	PX_GeoDrawSolidCircle(pSurface,(px_int)(objx+objWidth),(px_int)(objy+objHeight),5,renderColor);

	PX_GeoDrawBorder(pSurface,(px_int)objx,(px_int)objy,(px_int)(objx+objWidth),(px_int)(objy+objHeight),3,pDesc->borderColor);

}

px_void PX_Object_PancOnCursorDown(PX_Object *pObject,PX_Object_Event e,px_void *ptr)
{
	px_float x,y;
	px_float objx,objy,objWidth,objHeight;
	px_float inheritX,inheritY;
	PX_Object_Panc *pDesc=PX_ObjectGetDesc(PX_Object_Panc,pObject);
	PX_ObjectGetInheritXY(pObject,&inheritX,&inheritY);

	objx=(pObject->x+inheritX);
	objy=(pObject->y+inheritY);
	objWidth=pObject->Width;
	objHeight=pObject->Height;

	x=PX_Object_Event_GetCursorX(e);
	y=PX_Object_Event_GetCursorY(e);

	
	pDesc->bselect=PX_Object_Panc_bselect_none;
	if ((x-pDesc->currentX)*(x-pDesc->currentX)+(y-pDesc->currentY)*(y-pDesc->currentY)<64)
	{
		pDesc->bselect=PX_Object_Panc_bselect_center;
		return;
	}
	

	if ((x-objx)*(x-objx)+(y-objy)*(y-objy)<25)
	{
		pDesc->bselect=PX_Object_Panc_bselect_lefttop;
		return;
	}

	if ((x-objx-objWidth)*(x-objx-objWidth)+(y-objy)*(y-objy)<25)
	{
		pDesc->bselect=PX_Object_Panc_bselect_righttop;
		return;
	}

	if ((x-objx)*(x-objx)+(y-objy-objHeight)*(y-objy-objHeight)<25)
	{
		pDesc->bselect=PX_Object_Panc_bselect_leftbottom;
		return;
	}

	if ((x-objx-objWidth)*(x-objx-objWidth)+(y-objy-objHeight)*(y-objy-objHeight)<25)
	{
		pDesc->bselect=PX_Object_Panc_bselect_rightbottom;
		return;
	}
	
}

px_void PX_Object_PancOnCursorRDown(PX_Object* pObject, PX_Object_Event e, px_void* ptr)
{
	px_float x, y;
	PX_Object_Panc* pDesc = PX_ObjectGetDesc(PX_Object_Panc, pObject);
	px_float objx, objy, objWidth, objHeight;
	px_float inheritX, inheritY;
	PX_ObjectGetInheritXY(pObject, &inheritX, &inheritY);

	objx = (pObject->x + inheritX);
	objy = (pObject->y + inheritY);
	objWidth = pObject->Width;
	objHeight = pObject->Height;


	x = PX_Object_Event_GetCursorX(e);
	y = PX_Object_Event_GetCursorY(e);

	pDesc->sourceX = x;
	pDesc->sourceY = y;

	if (pDesc->sourceX < objx)
	{
		pDesc->sourceX = objx;
	}

	if (pDesc->sourceX > objx + objWidth)
	{
		pDesc->sourceX = objx + objWidth;
	}

	if (pDesc->sourceY < objy)
	{
		pDesc->sourceY = objy;
	}

	if (pDesc->sourceY > objy + objHeight)
	{
		pDesc->sourceY = objy + objHeight;
	}

	pDesc->currentX = pDesc->sourceX;
	pDesc->currentY = pDesc->sourceY;
}

px_void PX_Object_PancOnCursorDrag(PX_Object *pObject,PX_Object_Event e,px_void *ptr)
{
	px_float x,y;
	PX_Object_Panc *pDesc=PX_ObjectGetDesc(PX_Object_Panc,pObject);
	px_float objx, objy, objWidth, objHeight;
	px_float inheritX, inheritY;
	PX_ObjectGetInheritXY(pObject, &inheritX, &inheritY);

	objx = (pObject->x + inheritX);
	objy = (pObject->y + inheritY);
	objWidth = pObject->Width;
	objHeight = pObject->Height;

	
	x=PX_Object_Event_GetCursorX(e);
	y=PX_Object_Event_GetCursorY(e);

	switch (pDesc->bselect)
	{
	case PX_Object_Panc_bselect_center:
		{
			pDesc->currentX= x;
			pDesc->currentY =y;

			if (pDesc->currentX<objx)
			{
				pDesc->currentX = objx;
			}

			if (pDesc->currentX > objx+ objWidth)
			{
				pDesc->currentX = objx+ objWidth;
			}

			if (pDesc->currentY < objy)
			{
				pDesc->currentY = objy;
			}

			if (pDesc->currentY > objy + objHeight)
			{
				pDesc->currentY = objy + objHeight;
			}

			PX_ObjectExecuteEvent(pObject, PX_OBJECT_BUILD_EVENT(PX_OBJECT_EVENT_VALUECHANGED));
		}
		break;
	case PX_Object_Panc_bselect_lefttop:
		{
			px_float widthinc,heightinc;
			widthinc=objx-x;
			heightinc=objy-y;

			if (objWidth+widthinc<=10)
			{
				return;
			}

			if (objHeight+heightinc<=10)
			{
				return;
			}

			pObject->x=x-inheritX;
			pObject->y=y-inheritY;
			pObject->Width+=widthinc;
			pObject->Height+=heightinc;

			pDesc->sourceX=pObject->x+pObject->Width/2;
			pDesc->sourceY=pObject->y+pObject->Height/2;

			pDesc->currentX=pDesc->sourceX;
			pDesc->currentY=pDesc->sourceY;
			PX_ObjectExecuteEvent(pObject, PX_OBJECT_BUILD_EVENT(PX_OBJECT_EVENT_VALUECHANGED));
		}
		break;
	case PX_Object_Panc_bselect_righttop:
		{
			px_float widthinc,heightinc;
			widthinc=x-objx-objWidth;
			heightinc=objy-y;

			if (objWidth+widthinc<=10)
			{
				return;
			}

			if (objHeight+heightinc<=10)
			{
				return;
			}

			pObject->y=y-inheritY;

			pObject->Width+=widthinc;
			pObject->Height+=heightinc;

			pDesc->sourceX=pObject->x+pObject->Width/2;
			pDesc->sourceY=pObject->y+pObject->Height/2;

			pDesc->currentX=pDesc->sourceX;
			pDesc->currentY=pDesc->sourceY;
			PX_ObjectExecuteEvent(pObject, PX_OBJECT_BUILD_EVENT(PX_OBJECT_EVENT_VALUECHANGED));
		}
		break;
	case PX_Object_Panc_bselect_leftbottom:
		{
			px_float widthinc,heightinc;
			widthinc=objx-x;
			heightinc=y-objy-objHeight;

			if (pObject->Width+widthinc<=10)
			{
				return;
			}

			if (pObject->Height+heightinc<=10)
			{
				return;
			}

			pObject->x=x-inheritX;

			pObject->Width+=widthinc;
			pObject->Height+=heightinc;

			pDesc->sourceX=pObject->x+pObject->Width/2;
			pDesc->sourceY=pObject->y+pObject->Height/2;

			pDesc->currentX=pDesc->sourceX;
			pDesc->currentY=pDesc->sourceY;
			PX_ObjectExecuteEvent(pObject, PX_OBJECT_BUILD_EVENT(PX_OBJECT_EVENT_VALUECHANGED));
		}
		break;

	case PX_Object_Panc_bselect_rightbottom:
		{
			px_float widthinc,heightinc;
			widthinc=x-objx-objWidth;
			heightinc=y-objy-objHeight;

			if (pObject->Width+widthinc<=10)
			{
				return;
			}

			if (pObject->Height+heightinc<=10)
			{
				return;
			}

			pObject->Width+=widthinc;
			pObject->Height+=heightinc;

			pDesc->sourceX=pObject->x+pObject->Width/2;
			pDesc->sourceY=pObject->y+pObject->Height/2;

			pDesc->currentX=pDesc->sourceX;
			pDesc->currentY=pDesc->sourceY;
			PX_ObjectExecuteEvent(pObject, PX_OBJECT_BUILD_EVENT(PX_OBJECT_EVENT_VALUECHANGED));
		}
		break;
	default:
		break;
	}

	
	
}

PX_Object * PX_Object_PancCreate(px_memorypool *mp,PX_Object *Parent,px_float x,px_float y,px_float widht,px_float height)
{
	PX_Object_Panc panc;
	PX_Object *pObject;
	PX_memset(&panc, 0, sizeof(panc));
	panc.sourceX=x+widht/2;
	panc.sourceY=y+height/2;

	panc.currentX=x+widht/2;
	panc.currentY=y+height/2;

	panc.borderColor=PX_OBJECT_UI_DEFAULT_FONTCOLOR;

	pObject=PX_ObjectCreateEx(mp,Parent,x,y,0,widht,height,0,PX_OBJECT_TYPE_PANC,PX_NULL,PX_Object_PancRender,PX_NULL,&panc,sizeof(panc));
	PX_ObjectRegisterEvent(pObject, PX_OBJECT_EVENT_CURSORDOWN, PX_Object_PancOnCursorDown, PX_NULL);
	PX_ObjectRegisterEvent(pObject, PX_OBJECT_EVENT_CURSORDRAG, PX_Object_PancOnCursorDrag, PX_NULL);
	PX_ObjectRegisterEvent(pObject, PX_OBJECT_EVENT_CURSORRDOWN, PX_Object_PancOnCursorRDown, PX_NULL);
	return pObject;

}

PX_Object_Panc * PX_Object_GetPanc(PX_Object *pObject)
{
	return PX_ObjectGetDesc(PX_Object_Panc, pObject);
}

px_void PX_Object_PancReset(PX_Object* pObject, px_float x, px_float y, px_float width, px_float height)
{
	PX_Object_Panc* pDesc = PX_ObjectGetDesc(PX_Object_Panc, pObject);
	pObject->x = x;
	pObject->y = y;
	pObject->Width = width;
	pObject->Height = height;

	pDesc->sourceX = x + width / 2;
	pDesc->sourceY = y + height / 2;

	pDesc->currentX = x + width / 2;
	pDesc->currentY = y + height / 2;
}

PX_PancMatrix PX_Object_PancGetMatrix(PX_Object* pObject)
{
	PX_Object_Panc* pDesc = PX_ObjectGetDesc(PX_Object_Panc, pObject);
	PX_PancMatrix mat;
	mat.x = pObject->x;
	mat.y = pObject->y;
	mat.width = pObject->Width;
	mat.height = pObject->Height;
	mat.currentX = pDesc->currentX;
	mat.currentY = pDesc->currentY;
	mat.sourceX = pDesc->sourceX;
	mat.sourceY = pDesc->sourceY;
	return mat;
}

