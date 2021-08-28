#include "PX_Object_DesignerBox.h"
#include "PX_Object_DesignerBox.h"

px_void PX_Object_DesignerBoxRender(px_surface *pSurface,PX_Object *pObject,px_dword elapsed)
{
	PX_Object_DesignerBox*pDesc=PX_ObjectGetDesc(PX_Object_DesignerBox,pObject);
	px_color renderColor;
	
	px_float objx,objy,objWidth,objHeight;
	px_float inheritX,inheritY;

	PX_ObjectGetInheritXY(pObject,&inheritX,&inheritY);

	objx=(pObject->x+inheritX);
	objy=(pObject->y+inheritY);
	objWidth=pObject->Width;
	objHeight=pObject->Height;

	renderColor=pDesc->borderColor;
	renderColor._argb.a/=2;

	PX_GeoDrawBorder(pSurface, (px_int)objx, (px_int)objy, (px_int)(objx + objWidth), (px_int)(objy + objHeight), 1, renderColor);
	
	//////////////////////////////////////////////////////////////////////////
	//controller point
	renderColor=pDesc->borderColor;
	renderColor._argb.a/=2;

	PX_GeoDrawSolidCircle(pSurface,(px_int)(objx),(px_int)(objy),5,renderColor);
	PX_GeoDrawSolidCircle(pSurface,(px_int)(objx+objWidth),(px_int)(objy),5,renderColor);
	PX_GeoDrawSolidCircle(pSurface,(px_int)(objx),(px_int)(objy+objHeight),5,renderColor);
	PX_GeoDrawSolidCircle(pSurface,(px_int)(objx+objWidth),(px_int)(objy+objHeight),5,renderColor);

}

px_void PX_Object_DesignerBoxOnCursorDown(PX_Object *pObject,PX_Object_Event e,px_void *ptr)
{
	px_float x,y;
	px_float objx,objy,objWidth,objHeight;
	px_float inheritX,inheritY;
	PX_Object_DesignerBox *pDesc=PX_ObjectGetDesc(PX_Object_DesignerBox,pObject);
	PX_ObjectGetInheritXY(pObject,&inheritX,&inheritY);

	objx=(pObject->x+inheritX);
	objy=(pObject->y+inheritY);
	objWidth=pObject->Width;
	objHeight=pObject->Height;

	x=PX_Object_Event_GetCursorX(e);
	y=PX_Object_Event_GetCursorY(e);

	
	pDesc->bselect=PX_Object_DesignerBox_bselect_none;

	if ((x-objx)*(x-objx)+(y-objy)*(y-objy)<25)
	{
		pDesc->bselect=PX_Object_DesignerBox_bselect_lefttop;
		return;
	}

	if ((x-objx-objWidth)*(x-objx-objWidth)+(y-objy)*(y-objy)<25)
	{
		pDesc->bselect=PX_Object_DesignerBox_bselect_righttop;
		return;
	}

	if ((x-objx)*(x-objx)+(y-objy-objHeight)*(y-objy-objHeight)<25)
	{
		pDesc->bselect=PX_Object_DesignerBox_bselect_leftbottom;
		return;
	}

	if ((x-objx-objWidth)*(x-objx-objWidth)+(y-objy-objHeight)*(y-objy-objHeight)<25)
	{
		pDesc->bselect=PX_Object_DesignerBox_bselect_rightbottom;
		return;
	}

	if (PX_ObjectIsCursorInRegion(pObject,e))
	{
		pDesc->bselect=PX_Object_DesignerBox_bselect_center;
		pDesc->lastx = x;
		pDesc->lasty = y;
		return;
	}
	


	
}



px_void PX_Object_DesignerBoxOnCursorDrag(PX_Object *pObject,PX_Object_Event e,px_void *ptr)
{
	px_float x,y;
	PX_Object_DesignerBox*pDesc=PX_ObjectGetDesc(PX_Object_DesignerBox,pObject);
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
	case PX_Object_DesignerBox_bselect_center:
		{

			pObject->x+=x-pDesc->lastx;
			pObject->y+=y-pDesc->lasty;
			pDesc->lastx= x;
			pDesc->lasty =y;
			PX_ObjectExecuteEvent(pObject, PX_OBJECT_BUILD_EVENT(PX_OBJECT_EVENT_VALUECHANGED));
		}
		break;
	case PX_Object_DesignerBox_bselect_lefttop:
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
			PX_ObjectExecuteEvent(pObject, PX_OBJECT_BUILD_EVENT(PX_OBJECT_EVENT_VALUECHANGED));
		}
		break;
	case PX_Object_DesignerBox_bselect_righttop:
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
			PX_ObjectExecuteEvent(pObject, PX_OBJECT_BUILD_EVENT(PX_OBJECT_EVENT_VALUECHANGED));
		}
		break;
	case PX_Object_DesignerBox_bselect_leftbottom:
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
			PX_ObjectExecuteEvent(pObject, PX_OBJECT_BUILD_EVENT(PX_OBJECT_EVENT_VALUECHANGED));
		}
		break;

	case PX_Object_DesignerBox_bselect_rightbottom:
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
			PX_ObjectExecuteEvent(pObject, PX_OBJECT_BUILD_EVENT(PX_OBJECT_EVENT_VALUECHANGED));
		}
		break;
	default:
		break;
	}

}

PX_Object * PX_Object_DesignerBoxCreate(px_memorypool *mp,PX_Object *Parent,px_float x,px_float y,px_float widht,px_float height)
{
	PX_Object_DesignerBox desc;
	PX_Object *pObject;
	PX_memset(&desc, 0, sizeof(desc));
	

	desc.borderColor=PX_OBJECT_UI_DEFAULT_FONTCOLOR;

	pObject=PX_ObjectCreateEx(mp,Parent,x,y,0,widht,height,0,PX_OBJECT_TYPE_DESIGNERBOX,PX_NULL,PX_Object_DesignerBoxRender,PX_NULL,&desc,sizeof(desc));
	PX_ObjectRegisterEvent(pObject, PX_OBJECT_EVENT_CURSORDOWN, PX_Object_DesignerBoxOnCursorDown, PX_NULL);
	PX_ObjectRegisterEvent(pObject, PX_OBJECT_EVENT_CURSORDRAG, PX_Object_DesignerBoxOnCursorDrag, PX_NULL);
	return pObject;

}

PX_Object_DesignerBox* PX_Object_GetDesignerBox(PX_Object *pObject)
{
	return PX_ObjectGetDesc(PX_Object_DesignerBox, pObject);
}

px_void PX_Object_DesignerBoxReset(PX_Object* pObject, px_float x, px_float y, px_float width, px_float height)
{
	PX_Object_DesignerBox* pDesc = PX_ObjectGetDesc(PX_Object_DesignerBox, pObject);
	pObject->x = x;
	pObject->y = y;
	pObject->Width = width;
	pObject->Height = height;
}

