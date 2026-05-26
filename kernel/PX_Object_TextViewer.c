#include "PX_Object_TextViewer.h"

px_void PX_Object_TextViewerReRender(PX_Object* pObject)
{
	px_int xoffset, yoffset;
	PX_Object_TextViewer* pDesc = PX_ObjectGetDesc0(PX_Object_TextViewer, pObject);
	xoffset = PX_Object_SliderBarGetValue(pDesc->hslider);
	yoffset = PX_Object_SliderBarGetValue(pDesc->vslider);
	PX_SurfaceClearAll(&pDesc->render_target, PX_COLOR(255, 255, 255, 255));
	PX_FontModuleDrawText(&pDesc->render_target, pDesc->fontModule, -xoffset, -yoffset, PX_ALIGN_LEFTTOP, pDesc->text.buffer, pDesc->TextColor);
}

PX_OBJECT_EVENT_FUNCTION(PX_Object_TextViewOnWheel)
{
	PX_Object_TextViewer* pdesc = PX_ObjectGetDesc0(PX_Object_TextViewer, (PX_Object *)ptr);
	px_float objx, objy, objWidth, objHeight;
	px_rect rect;

	rect = PX_ObjectGetRect((PX_Object*)ptr);
	objx = rect.x;
	objy = rect.y;
	objWidth = rect.width;
	objHeight = rect.height;

	if (!PX_ObjectIsCursorInRegion((PX_Object*)ptr, e))
	{
		return;
	}
	
	PX_Object_SliderBarSetValue(pdesc->vslider, PX_Object_SliderBarGetValue(pdesc->vslider) - (PX_Object_Event_GetCursorZ(e)>=0?8:-8));
}

PX_OBJECT_RENDER_FUNCTION(PX_Object_TextViewerRender)
{
	PX_Object_TextViewer* pDesc = PX_ObjectGetDesc(PX_Object_TextViewer, pObject);
	px_float objx, objy, objHeight, objWidth;
	px_rect rect = PX_ObjectGetRect(pObject);
	objx = rect.x;
	objy = rect.y;
	objWidth = rect.width;
	objHeight = rect.height;
	if (pObject->Width<32)
	{
		pObject->Width = 32;
	}
	if(pObject->Height<32)
	{
		pObject->Height = 32;
	}
	if (pDesc->render_target.width-20!=(px_int)objWidth|| pDesc->render_target.height - 20 != (px_int)objHeight)
	{
		if(PX_TextureIsValid(&pDesc->render_target))
			PX_TextureFree(&pDesc->render_target);
		if (!PX_TextureCreate(pObject->mp, &pDesc->render_target, (px_int)objWidth - 20, (px_int)objHeight - 20))
			return;
		PX_Object_TextViewerReRender(pObject);
	}

	pDesc->vslider->x =  objWidth - 20;
	pDesc->vslider->y = 0;
	pDesc->vslider->Height = objHeight - 20;

	pDesc->hslider->x = 0;
	pDesc->hslider->y = objHeight - 20;
	pDesc->hslider->Width = objWidth - 20;

	PX_TextureCover(psurface, &pDesc->render_target, (px_int)objx, (px_int)objy, PX_ALIGN_LEFTTOP);

	
}

PX_OBJECT_FREE_FUNCTION(PX_Object_TextViewerFree)
{
	PX_Object_TextViewer* pDesc = PX_ObjectGetDesc(PX_Object_TextViewer, pObject);
	PX_StringFree(&pDesc->text);
	PX_TextureFree(&pDesc->render_target);
}

PX_OBJECT_EVENT_FUNCTION(PX_Object_TextViewer_OnSliderChanged)
{
	PX_Object_TextViewerReRender((PX_Object*)ptr);
}


//attach object
PX_Object* PX_Object_TextViewerAttachObject(PX_Object* pObject, px_int attachIndex, px_int x, px_int y, px_int width, px_int height, PX_FontModule* fm)
{
	px_memorypool* mp=pObject->mp;
	PX_Object_TextViewer* pDesc;
	if (width<32)
	{
		width = 32;
	}
	if (height<32)
	{
		height = 32;
	}
	PX_ASSERTIF(pObject == PX_NULL);
	PX_ASSERTIF(attachIndex < 0 || attachIndex >= PX_COUNTOF(pObject->pObjectDesc));
	PX_ASSERTIF(pObject->pObjectDesc[attachIndex] != PX_NULL);
	pDesc = (PX_Object_TextViewer*)PX_ObjectCreateDesc(pObject, attachIndex, PX_OBJECT_TYPE_TEXTVIEWER, 0, PX_Object_TextViewerRender, PX_Object_TextViewerFree, 0, sizeof(PX_Object_TextViewer));
	if (!PX_StringInitialize(mp, &pDesc->text))return PX_NULL;
	if (!PX_TextureCreate(mp, &pDesc->render_target, width-20, height-20))
	{
		PX_StringFree(&pDesc->text);
		return PX_NULL;
	}
	pDesc->TextColor = PX_OBJECT_UI_DEFAULT_FONTCOLOR;
	pDesc->fontModule = fm;
	pDesc->vslider = PX_Object_SliderBarCreate(mp, pObject,width -20, 0, 20, height, PX_OBJECT_SLIDERBAR_TYPE_VERTICAL, PX_OBJECT_SLIDERBAR_STYLE_BOX);
	pDesc->hslider = PX_Object_SliderBarCreate(mp, pObject, 0, height - 20, width - 20, 20, PX_OBJECT_SLIDERBAR_TYPE_HORIZONTAL, PX_OBJECT_SLIDERBAR_STYLE_BOX);
	PX_Object_SliderBarSetRange(pDesc->vslider, 0, 0);
	PX_Object_SliderBarSetRange(pDesc->hslider, 0, 0);
	PX_ObjectRegisterEvent(pDesc->vslider, PX_OBJECT_EVENT_VALUECHANGED, PX_Object_TextViewer_OnSliderChanged, pObject);
	PX_ObjectRegisterEvent(pDesc->vslider, PX_OBJECT_EVENT_CURSORWHEEL, PX_Object_TextViewOnWheel, pObject);
	PX_ObjectRegisterEvent(pDesc->hslider, PX_OBJECT_EVENT_VALUECHANGED, PX_Object_TextViewer_OnSliderChanged, pObject);
	return pObject;
}

PX_Object* PX_Object_TextViewerCreate(px_memorypool* mp, PX_Object* Parent, px_int x, px_int y, px_int width, px_int height, PX_FontModule* fm)
{
	PX_Object *pObject;
	
	pObject=PX_ObjectCreate(mp,Parent,(px_float)x,(px_float)y,0,(px_float)width,16,0);
	if (pObject==PX_NULL)
	{
		return PX_NULL;
	}
	if (!PX_Object_TextViewerAttachObject(pObject, 0, x, y, width, height, fm))
	{
		PX_ObjectDelete(pObject);
		return PX_NULL;
	}
	return pObject;
}


px_void PX_Object_TextViewerSetTextColor(PX_Object *pObject,px_color Color)
{
	PX_Object_TextViewer * pDesc= (PX_Object_TextViewer*)PX_ObjectGetDescByType(pObject, PX_OBJECT_TYPE_TEXTVIEWER);
	if (!pDesc)
	{
		return;
	}
	pDesc->TextColor=Color;
}

px_void PX_Object_TextViewerSetText(PX_Object *pObject,const px_char *Text)
{
	px_int fwidth, fheight;
	px_rect objrect = PX_ObjectGetRect(pObject);
	PX_Object_TextViewer * pDesc= (PX_Object_TextViewer*)PX_ObjectGetDescByType(pObject, PX_OBJECT_TYPE_TEXTVIEWER);
	if (!pDesc)
	{
		return;
	}
	PX_StringClear(&pDesc->text);
	PX_StringCat(&pDesc->text,Text);
	PX_FontModuleTextGetRenderWidthHeight(pDesc->fontModule, pDesc->text.buffer, &fwidth, &fheight);
	if(fwidth <= objrect.width - 20)
	{
		PX_Object_SliderBarSetRange(pDesc->hslider, 0, 0);
	}
	else
	{
		PX_Object_SliderBarSetRange(pDesc->hslider, 0,(px_int)(fwidth-objrect.width + 30));
		PX_Object_SliderBarSetSliderButtonLength(pDesc->hslider, (px_int)(pDesc->hslider->Width * (objrect.width-20)/fwidth));
	}

	if (fheight <= objrect.height - 20)
	{
		PX_Object_SliderBarSetRange(pDesc->vslider, 0, 0);
	}
	else
	{
		PX_Object_SliderBarSetRange(pDesc->vslider, 0, (px_int)(fheight - objrect.height + 30));
		PX_Object_SliderBarSetSliderButtonLength(pDesc->vslider, (px_int)(pDesc->vslider->Width * (objrect.height-20) / fheight));
	}

}

const px_char* PX_Object_TextViewerGetText(PX_Object* pObject)
{
	PX_Object_TextViewer* pDesc = (PX_Object_TextViewer*)PX_ObjectGetDescByType(pObject, PX_OBJECT_TYPE_TEXTVIEWER);
	if (!pDesc)
	{
		return 0;
	}
	return pDesc->text.buffer;
}


