#include "PX_Object_Label.h"

PX_OBJECT_RENDER_FUNCTION(PX_Object_LabelRender)
{
	px_int x,y,w,h;
	px_float inheritX,inheritY;
	PX_SurfaceLimitInfo limitInfo;
	PX_Object_Label *pLabel= PX_ObjectGetDesc(PX_Object_Label,pObject);
	if (pLabel==PX_NULL)
	{
		return;
	}

	if (!pObject->Visible)
	{
		return;
	}

	PX_ObjectGetInheritXY(pObject,&inheritX,&inheritY);
	x=(px_int)(pObject->x+inheritX);
	y=(px_int)(pObject->y+inheritY);
	w=(px_int)pObject->Width;
	h=(px_int)pObject->Height;

	PX_GeoDrawRect(psurface,x,y,x+w,y+h-1,pLabel->BackgroundColor);
	if (pLabel->bBorder)
	{
		switch(pLabel->style)
		{
		case PX_OBJECT_LABEL_STYLE_ROUNDRECT:
			PX_GeoDrawRoundRect(psurface,x,y,x+w,y+h-1,h/2-2.0f,1,pLabel->borderColor);
			break;
		default:
			PX_GeoDrawBorder(psurface,x,y,x+w,y+h-1,1,pLabel->borderColor);
		}
	}

	limitInfo=PX_SurfaceGetLimit(psurface);
	PX_SurfaceSetLimit(psurface,x,y,x+w-1,y+h-1);

	switch (pLabel->Align)
	{
	case PX_ALIGN_LEFTTOP:
		PX_FontModuleDrawText(psurface,pLabel->fontModule,x,y,PX_ALIGN_LEFTTOP,pLabel->Text,pLabel->TextColor);
		break;
	case PX_ALIGN_LEFTMID:
		PX_FontModuleDrawText(psurface,pLabel->fontModule,x,y+h/2,PX_ALIGN_LEFTMID,pLabel->Text,pLabel->TextColor);
		break;
	case PX_ALIGN_LEFTBOTTOM:
		PX_FontModuleDrawText(psurface,pLabel->fontModule,x,y+h,PX_ALIGN_LEFTBOTTOM,pLabel->Text,pLabel->TextColor);
		break;
	case PX_ALIGN_MIDTOP:
		PX_FontModuleDrawText(psurface,pLabel->fontModule,x+w/2,y,PX_ALIGN_MIDTOP,pLabel->Text,pLabel->TextColor);
		break;
	case PX_ALIGN_CENTER:
		PX_FontModuleDrawText(psurface,pLabel->fontModule,x+w/2,y+h/2,PX_ALIGN_CENTER,pLabel->Text,pLabel->TextColor);
		break;
	case PX_ALIGN_MIDBOTTOM:
		PX_FontModuleDrawText(psurface,pLabel->fontModule,x+w/2,y+h,PX_ALIGN_MIDBOTTOM,pLabel->Text,pLabel->TextColor);
		break;
	case PX_ALIGN_RIGHTTOP:
		PX_FontModuleDrawText(psurface,pLabel->fontModule,x+w,y,PX_ALIGN_RIGHTTOP,pLabel->Text,pLabel->TextColor);
		break;
	case PX_ALIGN_RIGHTMID:
		PX_FontModuleDrawText(psurface,pLabel->fontModule,x+w,y+h/2,PX_ALIGN_RIGHTMID,pLabel->Text,pLabel->TextColor);
		break;
	case PX_ALIGN_RIGHTBOTTOM:
		PX_FontModuleDrawText(psurface,pLabel->fontModule,x+w,y+h,PX_ALIGN_RIGHTBOTTOM,pLabel->Text,pLabel->TextColor);
		break;
	}
	PX_SurfaceSetLimitInfo(psurface,limitInfo);
}

PX_OBJECT_FREE_FUNCTION(PX_Object_LabelFree)
{
	PX_Object_Label *pLabel=PX_ObjectGetDesc(PX_Object_Label,pObject);
	if (pLabel!=PX_NULL)
	{
		MP_Free(pObject->mp, pLabel->Text);
	}
}
PX_Object* PX_Object_LabelAttachObject( PX_Object* pObject,px_int attachIndex, px_int x, px_int y, px_int Width, px_int Height, const px_char* Text, PX_FontModule* fm, px_color Color)
{
	px_memorypool* mp= pObject->mp;
	PX_Object_Label* pLabel;
	px_int TextLen;
	PX_ASSERTIF(pObject == PX_NULL);
	PX_ASSERTIF(attachIndex < 0 || attachIndex >= PX_COUNTOF(pObject->pObjectDesc));
	PX_ASSERTIF(pObject->pObjectDesc[attachIndex] != PX_NULL);
	pLabel = (PX_Object_Label*)PX_ObjectCreateDesc(pObject, attachIndex, PX_OBJECT_TYPE_LABEL, 0, PX_Object_LabelRender, PX_Object_LabelFree, 0, sizeof(PX_Object_Label));
	PX_ASSERTIF(pLabel == PX_NULL);

	TextLen = PX_strlen(Text);

	pLabel->Text = (px_char*)MP_Malloc(mp, TextLen + 1);
	if (pLabel->Text == PX_NULL)
	{
		return PX_NULL;
	}
	PX_strcpy(pLabel->Text, Text, TextLen + 1);
	pLabel->TextColor = Color;
	pLabel->BackgroundColor = PX_COLOR(0, 0, 0, 0);
	pLabel->Align = PX_ALIGN_LEFTMID;
	pLabel->bBorder = PX_FALSE;
	pLabel->borderColor = PX_COLOR(255, 0, 0, 0);
	pLabel->fontModule = fm;
	pLabel->style = PX_OBJECT_LABEL_STYLE_RECT;
	return pObject;
}

PX_Object* PX_Object_LabelCreate(px_memorypool *mp,PX_Object *Parent,px_int x,px_int y,px_int Width,px_int Height,const px_char *Text,PX_FontModule *fm,px_color Color)
{
	PX_Object *pObject;
	pObject=PX_ObjectCreate(mp,Parent,(px_float)x,(px_float)y,0,(px_float)Width,(px_float)Height,0);

	if (pObject==PX_NULL)
	{
		return PX_NULL;
	}
	if (!PX_Object_LabelAttachObject(pObject,0,x,y,Width,Height,Text,fm,Color))
	{
		PX_ObjectDelete(pObject);
		return PX_NULL;
	}
	return pObject;
}


PX_Object_Label  * PX_Object_GetLabel( PX_Object *pObject )
{
	return (PX_Object_Label*)PX_ObjectGetDescByType(pObject,PX_OBJECT_TYPE_LABEL);
}

px_char	  * PX_Object_LabelGetText( PX_Object * pObject)
{
	PX_Object_Label *pLabel=PX_Object_GetLabel(pObject);
	PX_ASSERTIF(pLabel==PX_NULL);
	return pLabel->Text;
}
px_void PX_Object_LabelSetText( PX_Object *pObject,const px_char *Text )
{
	PX_Object_Label *pLabel;
	px_int TextLen;
	if (pObject==PX_NULL||Text==PX_NULL)
	{
		return;
	}
	pLabel = PX_Object_GetLabel(pObject);
	PX_ASSERTIF(pLabel == PX_NULL);
	

	TextLen=PX_strlen(Text);
	pLabel= PX_Object_GetLabel(pObject);
	if(TextLen>PX_strlen(pLabel->Text))
	{
		MP_Free(pObject->mp,pLabel->Text);
		pLabel->Text=(px_char *)MP_Malloc(pObject->mp,TextLen+1);
		if (!pLabel->Text)
		{
			PX_ERROR("Text create out of memories");
		}
		PX_strcpy(pLabel->Text,Text,TextLen+1);
	}
	else
	{
		PX_strcpy(pLabel->Text,Text,TextLen+1);
	}
}


px_void PX_Object_LabelSetTextColor(PX_Object *pObject,px_color Color)
{
	PX_Object_Label * pLabel=PX_Object_GetLabel(pObject);
	PX_ASSERTIF(pLabel == PX_NULL);
	pLabel->TextColor = Color;
}


px_void PX_Object_LabelSetBackgroundColor( PX_Object *pObject,px_color Color )
{
	PX_Object_Label * pLabel=PX_Object_GetLabel(pObject);
	PX_ASSERTIF(pLabel == PX_NULL);
	pLabel->BackgroundColor = Color;
}
px_void PX_Object_LabelSetAlign( PX_Object *pObject,PX_ALIGN Align )
{
	PX_Object_Label* pLabel = PX_Object_GetLabel(pObject);
	PX_ASSERTIF(pLabel == PX_NULL);
	pLabel->Align=Align;
}



px_void PX_Object_LabelSetBorder(PX_Object *pObject,px_bool b)
{
	PX_Object_Label* pLabel = PX_Object_GetLabel(pObject);
	PX_ASSERTIF(pLabel == PX_NULL);
	pLabel->bBorder=b;
}

px_void PX_Object_LabelSetBorderColor(PX_Object *pObject,px_color color)
{
	PX_Object_Label* pLabel = PX_Object_GetLabel(pObject);
	PX_ASSERTIF(pLabel == PX_NULL);
	pLabel->borderColor=color;
}

px_void PX_Object_LabelSetStyle(PX_Object *pObject,PX_OBJECT_LABEL_STYLE style)
{
	PX_Object_Label* pLabel = PX_Object_GetLabel(pObject);
	PX_ASSERTIF(pLabel == PX_NULL);
	pLabel->style=style;
}

px_int PX_Object_LabelGetTextRenderWidth(PX_Object* pObject)
{
	PX_Object_Label* pLabel = PX_Object_GetLabel(pObject);
	px_int w, h;
	PX_ASSERTIF(pLabel == PX_NULL);
    PX_FontModuleTextGetRenderWidthHeight(pLabel->fontModule, pLabel->Text, &w, &h);
	return w;
}



PX_OBJECT_RENDER_FUNCTION(PX_Object_SliderTextOnRender)
{
	PX_Object_SliderText* pDesc = PX_ObjectGetDesc(PX_Object_SliderText, pObject);
	px_float x, y, w, h;
	PX_OBJECT_INHERIT_CODE(pObject, x, y, w, h);
	if (elapsed>pDesc->alive)
	{
		PX_ObjectDelayDelete(pObject);
		return;
	}
	PX_FontModuleDrawText(psurface, pDesc->fm, (px_int)x, (px_int)y, PX_ALIGN_LEFTMID, pDesc->text.buffer, pDesc->color);
}

PX_OBJECT_FREE_FUNCTION(PX_Object_SliderTextOnFree)
{
	PX_Object_SliderText* pDesc = PX_ObjectGetDesc(PX_Object_SliderText, pObject);
	PX_StringFree(&pDesc->text);
}

PX_Object* PX_Object_SliderTextAttachObject( PX_Object* pObject,px_int attachIndex, px_float x, px_float y, px_float vx, px_float vy, px_dword alive, const px_char* Text, PX_FontModule* fm, px_color Color)
{
	px_memorypool* mp=pObject->mp;
	PX_Object_SliderText Desc = { 0 },*pdesc;
	if (!PX_StringInitialize(mp, &Desc.text))return 0;
	if (!PX_StringCat(&Desc.text, Text))
	{
		PX_StringFree(&Desc.text);
		return 0;
	}
	Desc.fm = fm;
	Desc.color = Color;
	Desc.alive = alive;
	PX_ASSERTIF(pObject == PX_NULL);
	PX_ASSERTIF(attachIndex < 0 || attachIndex >= PX_COUNTOF(pObject->pObjectDesc));
	PX_ASSERTIF(pObject->pObjectDesc[attachIndex] != PX_NULL);
	pdesc = (PX_Object_SliderText*)PX_ObjectCreateDesc(pObject, attachIndex, PX_OBJECT_TYPE_SLIDERTEXT,0 , PX_Object_SliderTextOnRender, PX_Object_SliderTextOnFree, &Desc, sizeof(PX_Object_SliderText));
	PX_ASSERTIF(pdesc == PX_NULL);
	return pObject;
}

PX_Object* PX_Object_SliderTextCreate(px_memorypool* mp, PX_Object* Parent, px_float x, px_float y, px_float vx,px_float vy,px_dword alive, const px_char* Text, PX_FontModule* fm, px_color Color)
{
	PX_Object* pObject;
	px_int fh, fw;
	PX_FontModuleTextGetRenderWidthHeight(fm, Text, &fw, &fh);
	pObject = PX_ObjectCreate(mp, Parent,  x, y, 0,fw*1.0f, fh*1.0f, 0);
	if (!pObject)
	{
		return PX_NULL;
	}
	if (!PX_Object_SliderTextAttachObject(pObject, 0, x, y, vx, vy, alive, Text, fm, Color))
	{
		PX_ObjectDelete(pObject);
		return PX_NULL;
	}
	return pObject;
}

