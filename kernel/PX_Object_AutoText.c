#include "PX_Object_AutoText.h"

PX_Object * PX_Object_AutoTextCreate(px_memorypool *mp,PX_Object *Parent,px_int x,px_int y,px_int width,PX_FontModule *fm)
{
	PX_Object *pObject;

	PX_Object_AutoText *pAt=(PX_Object_AutoText *)MP_Malloc(mp,sizeof(PX_Object_AutoText));
	PX_StringInitialize(mp,&pAt->text);
	if (pAt==PX_NULL)
	{
		return PX_NULL;
	}

	if (width<=0)
	{
		return PX_NULL;
	}


	pObject=PX_ObjectCreate(mp,Parent,(px_float)x,(px_float)y,0,(px_float)width,16,0);

	if (pObject==PX_NULL)
	{
		return PX_NULL;
	}

	if (!pObject)
	{
		MP_Free(mp,pAt);
		return PX_NULL;
	}

	pObject->pObjectDesc=pAt;
	pObject->Type=PX_OBJECT_TYPE_AUTOTEXT;
	pObject->ReceiveEvents=PX_TRUE;
	pObject->Func_ObjectFree=PX_Object_AutoTextFree;
	pObject->Func_ObjectRender=PX_Object_AutoTextRender;

	pAt->TextColor=PX_OBJECT_UI_DEFAULT_FONTCOLOR;
	pAt->fontModule=fm;
	return pObject;
}

px_void PX_Object_AutoTextRender(px_surface *psurface, PX_Object *pObject,px_uint elapsed)
{
	px_int x_draw_oft,y_draw_oft,cursor,fsize;
	PX_Object_AutoText *pAt=(PX_Object_AutoText *)pObject->pObjectDesc;
	const px_char *Text=pAt->text.buffer;
	px_float objx,objy,objHeight,objWidth;
	px_float inheritX,inheritY;
	px_float h = 0;

	PX_ObjectGetInheritXY(pObject,&inheritX,&inheritY);

	objx=(pObject->x+inheritX);
	objy=(pObject->y+inheritY);
	objWidth=pObject->Width;
	objHeight=pObject->Height;

	if (pAt==PX_NULL)
	{
		return;
	}

	if (!pObject->Visible)
	{
		return;
	}

	x_draw_oft=(px_int)objx+5;
	y_draw_oft=(px_int)objy;

	cursor=0;

	while (PX_TRUE)
	{
		fsize=0;

		if (pAt->fontModule)
		{
			px_dword code;
			px_int width,height;
			fsize=PX_FontModuleGetOneCharacterDesc(pAt->fontModule,Text+cursor,&code,&width,&height);
			if (!fsize)
			{
				break;
			}
			if (code=='\r')
			{
				//skip
			}else if (code=='\n')
			{
				x_draw_oft=(px_int)objx;
				y_draw_oft+=pAt->fontModule->max_Height+2;
				h += pAt->fontModule->max_Height + 2;
			}
			else
			{
				if(psurface)
				PX_FontModuleDrawCharacter(psurface,pAt->fontModule,x_draw_oft,y_draw_oft,code,pAt->TextColor);
				x_draw_oft+=width;
			}

			if (x_draw_oft>objx+objWidth-pAt->fontModule->max_Width*2)
			{
				x_draw_oft=(px_int)objx;
				y_draw_oft+=pAt->fontModule->max_Height+2;
				h += pAt->fontModule->max_Height + 2;
			}

		}
		else
		{
			fsize=1;

			if (Text[cursor]=='\r')
			{
				//skip
			}else if (Text[cursor]=='\n')
			{
				x_draw_oft=(px_int)objx;
				y_draw_oft+=__PX_FONT_HEIGHT+2;
				h += __PX_FONT_HEIGHT + 2;
			}
			else if(Text[cursor])
			{
				if (psurface)
					PX_FontDrawChar(psurface,x_draw_oft,y_draw_oft,Text[cursor],pAt->TextColor);
				x_draw_oft+=__PX_FONT_ASCSIZE;
			}
			else
			{
				break;
			}

			if (x_draw_oft>objx+objWidth-PX_FontGetAscCharactorWidth()*2)
			{
				x_draw_oft=(px_int)objx;
				y_draw_oft+=__PX_FONT_HEIGHT+2;
				h += __PX_FONT_HEIGHT + 2;
			}
		}
		cursor+=fsize;
	}
	if (pAt->fontModule)
	{
		h += pAt->fontModule->max_Height + 2;
	}
	else
	{
		h += __PX_FONT_HEIGHT + 2;
	}
	pObject->Height = h;
	if(pObject->Height<16) pObject->Height = 16;
}

px_void PX_Object_AutoTextFree(PX_Object *pObject)
{
	PX_Object_AutoText * pAt=PX_Object_GetAutoText(pObject);
	if (!pAt)
	{
		PX_ASSERT();
		return;
	}
	PX_StringFree(&pAt->text);
}


PX_Object_AutoText * PX_Object_GetAutoText(PX_Object *pObject)
{
	if(pObject->Type==PX_OBJECT_TYPE_AUTOTEXT)
		return (PX_Object_AutoText *)pObject->pObjectDesc;
	else
		return PX_NULL;
}

px_void PX_Object_AutoTextSetTextColor(PX_Object *pObject,px_color Color)
{
	PX_Object_AutoText * pAt=PX_Object_GetAutoText(pObject);
	if (!pAt)
	{
		return;
	}
	pAt->TextColor=Color;
}

px_void PX_Object_AutoTextSetText(PX_Object *pObject,const px_char *Text)
{
	PX_Object_AutoText * pAt=PX_Object_GetAutoText(pObject);
	if (!pAt)
	{
		return;
	}
	PX_StringClear(&pAt->text);
	PX_StringCat(&pAt->text,Text);
	PX_Object_AutoTextRender(0, pObject, 0);

}

const px_char* PX_Object_AutoTextGetText(PX_Object* pObject)
{
	PX_Object_AutoText* pAt = PX_Object_GetAutoText(pObject);
	if (!pAt)
	{
		return 0;
	}
	return pAt->text.buffer;
}

//////////////////////////////////////////////////////////////////////////
//edit
//////////////////////////////////////////////////////////////////////////
PX_Object* PX_Designer_AutoTextCreate(px_memorypool* mp, PX_Object* pparent, px_float x, px_float y, px_float width, px_float height, px_void* ptr)
{
	PX_FontModule* fm = (PX_FontModule*)ptr;
	return PX_Object_AutoTextCreate(mp, pparent, (px_int)x, (px_int)y, 128, fm);
}

px_void PX_Designer_AutoTextSetText(PX_Object* pobject, const px_char text[])
{
	PX_Object_AutoTextSetText(pobject, text);
}

px_bool PX_Designer_AutoTextGetText(PX_Object* pobject, px_string* str)
{
	return PX_StringSet(str, PX_Object_AutoTextGetText(pobject));
}

PX_Designer_ObjectDesc PX_Object_AutoTextDesignerInstall()
{
	PX_Designer_ObjectDesc AutoText;
	px_int i = 0;
	PX_memset(&AutoText, 0, sizeof(AutoText));
	PX_strcat(AutoText.Name, "autotext");

	AutoText.createfunc = PX_Designer_AutoTextCreate;
	AutoText.type = PX_DESIGNER_OBJECT_TYPE_UI;

	PX_strcat(AutoText.properties[i].Name, "id");
	AutoText.properties[i].getstring = PX_Designer_GetID;
	AutoText.properties[i].setstring = PX_Designer_SetID;
	i++;

	PX_strcat(AutoText.properties[i].Name, "x");
	AutoText.properties[i].getfloat = PX_Designer_GetX;
	AutoText.properties[i].setfloat = PX_Designer_SetX;
	i++;

	PX_strcat(AutoText.properties[i].Name, "y");
	AutoText.properties[i].getfloat = PX_Designer_GetY;
	AutoText.properties[i].setfloat = PX_Designer_SetY;
	i++;

	PX_strcat(AutoText.properties[i].Name, "width");
	AutoText.properties[i].getfloat = PX_Designer_GetWidth;
	AutoText.properties[i].setfloat = PX_Designer_SetWidth;
	i++;

	PX_strcat(AutoText.properties[i].Name, "height");
	AutoText.properties[i].getfloat = PX_Designer_GetHeight;
	AutoText.properties[i].setfloat = PX_Designer_SetHeight;
	i++;

	PX_strcat(AutoText.properties[i].Name, "text");
	AutoText.properties[i].setstring = PX_Designer_AutoTextSetText;
	AutoText.properties[i].getstring = PX_Designer_AutoTextGetText;
	i++;
	return AutoText;

}
