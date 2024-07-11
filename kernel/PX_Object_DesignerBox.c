#include "PX_Object_DesignerBox.h"
#include "PX_Object_DesignerBox.h"

PX_OBJECT_RENDER_FUNCTION(PX_Object_DesignerBoxRender)
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

	PX_GeoDrawBorder(psurface, (px_int)objx, (px_int)objy, (px_int)(objx + objWidth), (px_int)(objy + objHeight), 1, renderColor);
	
	//////////////////////////////////////////////////////////////////////////
	//controller point
	renderColor=pDesc->borderColor;
	renderColor._argb.a/=2;

	PX_GeoDrawSolidCircle(psurface,(px_int)(objx),(px_int)(objy),5,renderColor);
	PX_GeoDrawSolidCircle(psurface,(px_int)(objx+objWidth),(px_int)(objy),5,renderColor);
	PX_GeoDrawSolidCircle(psurface,(px_int)(objx),(px_int)(objy+objHeight),5,renderColor);
	PX_GeoDrawSolidCircle(psurface,(px_int)(objx+objWidth),(px_int)(objy+objHeight),5,renderColor);

}

PX_OBJECT_EVENT_FUNCTION(PX_Object_DesignerBoxOnCursorDown)
{
	px_float x,y;
	px_float objx,objy,objWidth,objHeight;
	px_float inheritX,inheritY;
	PX_Object_DesignerBox *pDesc=(PX_Object_DesignerBox *)PX_ObjectGetDescByType(pObject,PX_OBJECT_TYPE_DESIGNERBOX);
	PX_ASSERTIF(!pDesc);
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



PX_OBJECT_EVENT_FUNCTION(PX_Object_DesignerBoxOnCursorDrag)
{
	px_float x,y;
	PX_Object_DesignerBox*pDesc= (PX_Object_DesignerBox*)PX_ObjectGetDescByType(pObject, PX_OBJECT_TYPE_DESIGNERBOX);
	
	px_float objx, objy, objWidth, objHeight;
	px_float inheritX, inheritY;
	PX_ASSERTIF(!pDesc);
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
PX_Object* PX_Object_DesignerBoxAttachObject(PX_Object* pObject,px_int attachIndex)
{
	PX_Object_DesignerBox *pDesc;


	PX_ASSERTIF(pObject == PX_NULL);
	PX_ASSERTIF(attachIndex < 0 || attachIndex >= PX_COUNTOF(pObject->pObjectDesc));
	PX_ASSERTIF(pObject->pObjectDesc[attachIndex] != PX_NULL);
	pDesc = (PX_Object_DesignerBox*)PX_ObjectCreateDesc(pObject, attachIndex, PX_OBJECT_TYPE_DESIGNERBOX, 0, PX_Object_DesignerBoxRender, 0, 0, sizeof(PX_Object_DesignerBox));
	PX_ASSERTIF(pDesc == PX_NULL);

	pDesc->borderColor = PX_OBJECT_UI_DEFAULT_FONTCOLOR;

	PX_ObjectRegisterEventEx(pObject, PX_OBJECT_EVENT_CURSORDOWN, PX_OBJECT_TYPE_DESIGNERBOX, PX_Object_DesignerBoxOnCursorDown, PX_NULL);
	PX_ObjectRegisterEventEx(pObject, PX_OBJECT_EVENT_CURSORDRAG, PX_OBJECT_TYPE_DESIGNERBOX, PX_Object_DesignerBoxOnCursorDrag, PX_NULL);
	return pObject;

}
PX_Object * PX_Object_DesignerBoxCreate(px_memorypool *mp,PX_Object *Parent,px_float x,px_float y,px_float widht,px_float height)
{
	PX_Object *pObject;
	pObject=PX_ObjectCreate(mp,Parent,x,y,0,widht,height,0);
	if(!pObject) 
	{
		return PX_NULL;
	}
	if(!PX_Object_DesignerBoxAttachObject(pObject,0))
	{
		PX_ObjectDelete(pObject);
		return PX_NULL;
	}
	return pObject;

}

PX_Object_DesignerBox* PX_Object_GetDesignerBox(PX_Object *pObject)
{
	PX_Object_DesignerBox* pDesc;
	pDesc = (PX_Object_DesignerBox*)PX_ObjectGetDescByType(pObject, PX_OBJECT_TYPE_DESIGNERBOX);
	PX_ASSERTIF(!pDesc);
	return pDesc;
}

px_void PX_Object_DesignerBoxReset(PX_Object* pObject, px_float x, px_float y, px_float width, px_float height)
{
	PX_Object_DesignerBox* pDesc = PX_Object_GetDesignerBox(pObject);
	PX_ASSERTIF(!pDesc);
	pObject->x = x;
	pObject->y = y;
	pObject->Width = width;
	pObject->Height = height;
}

