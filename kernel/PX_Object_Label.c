#include "PX_Object_Label.h"

px_void PX_Object_LabelRender(px_surface *psurface, PX_Object *pObject,px_uint elapsed)
{
	px_int x,y,w,h;
	px_float inheritX,inheritY;
	PX_SurfaceLimitInfo limitInfo;
	PX_Object_Label *pLabel=(PX_Object_Label *)pObject->pObject;
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

px_void PX_Object_LabelFree( PX_Object *pLabel )
{
	PX_Object_Label *Label=PX_Object_GetLabel(pLabel);
	if (pLabel!=PX_NULL)
	{
		MP_Free(pLabel->mp,Label->Text);
	}
}

PX_Object* PX_Object_LabelCreate(px_memorypool *mp,PX_Object *Parent,px_int x,px_int y,px_int Width,px_int Height,const px_char *Text,PX_FontModule *fm,px_color Color )
{
	px_int TextLen;
	PX_Object *pObject;
	PX_Object_Label *pLable=(PX_Object_Label *)MP_Malloc(mp,sizeof(PX_Object_Label));
	if (pLable==PX_NULL)
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
		MP_Free(mp,pLable);
		return PX_NULL;
	}
	pObject->pObject=pLable;
	pObject->Type=PX_OBJECT_TYPE_LABEL;
	pObject->ReceiveEvents=PX_FALSE;
	pObject->Func_ObjectFree=PX_Object_LabelFree;
	pObject->Func_ObjectRender=PX_Object_LabelRender;

	TextLen=PX_strlen(Text);

	pLable->Text=(px_char *)MP_Malloc(mp,TextLen+1);
	if (pLable->Text==PX_NULL)
	{
		MP_Free(pObject->mp,pObject);
		MP_Free(pObject->mp,pLable);
		return PX_NULL;
	}

	PX_strcpy(pLable->Text,Text,TextLen+1);

	pLable->TextColor=Color;
	pLable->BackgroundColor=PX_COLOR(0,0,0,0);
	pLable->Align=PX_ALIGN_LEFTMID;
	pLable->bBorder=PX_FALSE;
	pLable->borderColor=PX_COLOR(255,0,0,0);
	pLable->fontModule=fm;
	pLable->style=PX_OBJECT_LABEL_STYLE_RECT;
	return pObject;
}


PX_Object_Label  * PX_Object_GetLabel( PX_Object *Object )
{
	if(Object->Type==PX_OBJECT_TYPE_LABEL)
		return (PX_Object_Label *)Object->pObject;
	else
		return PX_NULL;
}

px_char	  * PX_Object_LabelGetText( PX_Object *Label )
{
	PX_Object_Label *pLabel=PX_Object_GetLabel(Label);
	if (pLabel!=PX_NULL)
	{
		return pLabel->Text;
	}
	return PX_NULL;
}
px_void PX_Object_LabelSetText( PX_Object *pObject,const px_char *Text )
{
	PX_Object_Label *pLabel;
	px_int TextLen;
	if (pObject==PX_NULL||Text==PX_NULL)
	{
		return;
	}
	if (pObject->Type!=PX_OBJECT_TYPE_LABEL)
	{
		PX_ASSERT();
		return;
	}

	TextLen=PX_strlen(Text);
	pLabel=(PX_Object_Label *)pObject->pObject;
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
	if (pLabel)
	{
		pLabel->TextColor=Color;
	}
}


px_void PX_Object_LabelSetBackgroundColor( PX_Object *pObject,px_color Color )
{
	PX_Object_Label * pLabel=PX_Object_GetLabel(pObject);
	if (pLabel)
	{
		pLabel->BackgroundColor=Color;
	}
}
px_void PX_Object_LabelSetAlign( PX_Object *pObject,PX_ALIGN Align )
{
	PX_Object_Label * pLabel=PX_Object_GetLabel(pObject);
	if (pLabel)
	{

		pLabel->Align=Align;
	}
}



px_void PX_Object_LabelSetBorder(PX_Object *pObject,px_bool b)
{
	PX_Object_Label * pLabel=PX_Object_GetLabel(pObject);
	if (pLabel)
	{
		pLabel->bBorder=b;
	}
}

px_void PX_Object_LabelSetBorderColor(PX_Object *pObject,px_color color)
{
	PX_Object_Label * pLabel=PX_Object_GetLabel(pObject);
	if (pLabel)
	{
		pLabel->borderColor=color;
	}
}

px_void PX_Object_LabelSetStyle(PX_Object *pObject,PX_OBJECT_LABEL_STYLE style)
{
	PX_Object_Label * pLabel=PX_Object_GetLabel(pObject);
	if (pLabel)
	{
		pLabel->style=style;
	}
}

px_int PX_Object_LabelGetTextRenderWidth(PX_Object* pObject)
{
	PX_Object_Label* pLabel = PX_Object_GetLabel(pObject);
	if (pLabel)
	{
		px_int w, h;
		PX_FontModuleTextGetRenderWidthHeight(pLabel->fontModule, pLabel->Text, &w, &h);
		return w;
	}
	return 0;
}
