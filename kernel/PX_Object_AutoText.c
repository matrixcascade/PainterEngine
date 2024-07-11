#include "PX_Object_AutoText.h"
PX_OBJECT_RENDER_FUNCTION(PX_Object_AutoTextRender)
{
	px_int x_draw_oft, y_draw_oft, cursor, fsize;
	PX_Object_AutoText* pAt = PX_ObjectGetDesc(PX_Object_AutoText, pObject);
	const px_char* Text = pAt->text.buffer;
	px_float objx, objy, objHeight, objWidth;
	px_float inheritX, inheritY;
	px_float h = 0;

	PX_ObjectGetInheritXY(pObject, &inheritX, &inheritY);

	objx = (pObject->x + inheritX);
	objy = (pObject->y + inheritY);
	objWidth = pObject->Width;
	objHeight = pObject->Height;

	if (pAt == PX_NULL)
	{
		return;
	}

	if (!pObject->Visible)
	{
		return;
	}

	x_draw_oft = (px_int)objx + 5;
	y_draw_oft = (px_int)objy;

	cursor = 0;

	while (PX_TRUE)
	{
		fsize = 0;

		if (pAt->fontModule)
		{
			px_dword code;
			px_int width, height;
			fsize = PX_FontModuleGetOneCharacterDesc(pAt->fontModule, Text + cursor, &code, &width, &height);
			if (!fsize)
			{
				break;
			}
			if (code == '\r')
			{
				//skip
			}
			else if (code == '\n')
			{
				x_draw_oft = (px_int)objx;
				y_draw_oft += pAt->fontModule->max_Height + 2;
				h += pAt->fontModule->max_Height + 2;
			}
			else
			{
				if (psurface)
					PX_FontModuleDrawCharacter(psurface, pAt->fontModule, x_draw_oft, y_draw_oft, code, pAt->TextColor);
				x_draw_oft += width;
			}

			if (x_draw_oft > objx + objWidth - pAt->fontModule->max_Width * 2)
			{
				x_draw_oft = (px_int)objx;
				y_draw_oft += pAt->fontModule->max_Height + 2;
				h += pAt->fontModule->max_Height + 2;
			}

		}
		else
		{
			fsize = 1;

			if (Text[cursor] == '\r')
			{
				//skip
			}
			else if (Text[cursor] == '\n')
			{
				x_draw_oft = (px_int)objx;
				y_draw_oft += __PX_FONT_HEIGHT + 2;
				h += __PX_FONT_HEIGHT + 2;
			}
			else if (Text[cursor])
			{
				if (psurface)
					PX_FontDrawChar(psurface, x_draw_oft, y_draw_oft, Text[cursor], pAt->TextColor);
				x_draw_oft += __PX_FONT_ASCSIZE;
			}
			else
			{
				break;
			}

			if (x_draw_oft > objx + objWidth - PX_FontGetAscCharactorWidth() * 2)
			{
				x_draw_oft = (px_int)objx;
				y_draw_oft += __PX_FONT_HEIGHT + 2;
				h += __PX_FONT_HEIGHT + 2;
			}
		}
		cursor += fsize;
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
	if (pObject->Height < 16) pObject->Height = 16;
}

PX_OBJECT_FREE_FUNCTION(PX_Object_AutoTextFree)
{
	PX_Object_AutoText* pAt = PX_ObjectGetDesc(PX_Object_AutoText, pObject);
	PX_StringFree(&pAt->text);
}


//attach object
PX_Object* PX_Object_AutoTextAttachObject(PX_Object* pObject, px_int attachIndex, px_int x, px_int y, px_int width, PX_FontModule* fm)
{
	px_memorypool* mp=pObject->mp;
	PX_Object_AutoText* pDesc;
	PX_ASSERTIF(pObject == PX_NULL);
	PX_ASSERTIF(attachIndex < 0 || attachIndex >= PX_COUNTOF(pObject->pObjectDesc));
	PX_ASSERTIF(pObject->pObjectDesc[attachIndex] != PX_NULL);
	pDesc = (PX_Object_AutoText*)PX_ObjectCreateDesc(pObject, attachIndex, PX_OBJECT_TYPE_AUTOTEXT, 0, PX_Object_AutoTextRender, PX_Object_AutoTextFree, 0, sizeof(PX_Object_AutoText));
	if (!PX_StringInitialize(mp, &pDesc->text))return PX_NULL;
	pDesc->TextColor = PX_OBJECT_UI_DEFAULT_FONTCOLOR;
	pDesc->fontModule = fm;
	return pObject;
}

PX_Object * PX_Object_AutoTextCreate(px_memorypool *mp,PX_Object *Parent,px_int x,px_int y,px_int width,PX_FontModule *fm)
{
	PX_Object *pObject;
	
	pObject=PX_ObjectCreate(mp,Parent,(px_float)x,(px_float)y,0,(px_float)width,16,0);
	if (pObject==PX_NULL)
	{
		return PX_NULL;
	}
	if(!PX_Object_AutoTextAttachObject(pObject,0,x,y,width,fm))
	{
		PX_ObjectDelete(pObject);
		return PX_NULL;
	}
	return pObject;
}


px_void PX_Object_AutoTextSetTextColor(PX_Object *pObject,px_color Color)
{
	PX_Object_AutoText * pAt= (PX_Object_AutoText*)PX_ObjectGetDescByType(pObject, PX_OBJECT_TYPE_AUTOTEXT);
	if (!pAt)
	{
		return;
	}
	pAt->TextColor=Color;
}

px_void PX_Object_AutoTextSetText(PX_Object *pObject,const px_char *Text)
{
	PX_Object_AutoText * pAt= (PX_Object_AutoText*)PX_ObjectGetDescByType(pObject, PX_OBJECT_TYPE_AUTOTEXT);
	if (!pAt)
	{
		return;
	}
	PX_StringClear(&pAt->text);
	PX_StringCat(&pAt->text,Text);
	PX_ObjectRender(0, pObject, 0);
}

const px_char* PX_Object_AutoTextGetText(PX_Object* pObject)
{
	PX_Object_AutoText* pAt = (PX_Object_AutoText*)PX_ObjectGetDescByType(pObject, PX_OBJECT_TYPE_AUTOTEXT);
	if (!pAt)
	{
		return 0;
	}
	return pAt->text.buffer;
}


