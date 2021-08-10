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


	pObject=PX_ObjectCreate(mp,Parent,(px_float)x,(px_float)y,0,(px_float)width,0,0);

	if (pObject==PX_NULL)
	{
		return PX_NULL;
	}

	if (!pObject)
	{
		MP_Free(mp,pAt);
		return PX_NULL;
	}

	pObject->pObject=pAt;
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
	PX_Object_AutoText *pAt=(PX_Object_AutoText *)pObject->pObject;
	const px_char *Text=pAt->text.buffer;
	px_float objx,objy,objHeight,objWidth;
	px_float inheritX,inheritY;

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

	x_draw_oft=(px_int)objx;
	y_draw_oft=(px_int)objy;

	cursor=0;

	while (PX_TRUE)
	{
		fsize=0;

		if (pAt->fontModule)
		{
			px_dword code;
			px_int width,height;
			fsize=PX_FontModuleGetCharacterDesc(pAt->fontModule,Text+cursor,&code,&width,&height);
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
				y_draw_oft+=pAt->fontModule->max_Height;
			}
			else
			{
				PX_FontModuleDrawCharacter(psurface,pAt->fontModule,x_draw_oft,y_draw_oft,code,pAt->TextColor);
				x_draw_oft+=width;
			}

			if (x_draw_oft>objx+objWidth-PX_FontGetAscCharactorWidth()*2)
			{
				x_draw_oft=(px_int)objx;
				y_draw_oft+=pAt->fontModule->max_Height;
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
				y_draw_oft+=__PX_FONT_HEIGHT;
			}
			else if(Text[cursor])
			{
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
				y_draw_oft+=__PX_FONT_HEIGHT;
			}
		}
		cursor+=fsize;
	}
}

px_void PX_Object_AutoTextFree(PX_Object *Obj)
{
	PX_Object_AutoText * pAt=PX_Object_GetAutoText(Obj);
	if (!pAt)
	{
		return;
	}
	PX_StringFree(&pAt->text);
}

px_int PX_Object_AutoTextGetHeight(PX_Object *pObject)
{
	px_int i;
	px_int w=0;
	px_int h=0;
	PX_Object_AutoText * pAt=PX_Object_GetAutoText(pObject);

	if (!pAt)
	{
		return 0;
	}

	for (i=0;i<PX_strlen(pAt->text.buffer);i++)
	{
		if (pAt->text.buffer[i]=='\n')
		{
			w=0;
			h+=PX_FontGetCharactorHeight();
			continue;
		}
		if (pAt->text.buffer[i]=='\r')
		{
			continue;
		}

		if (pAt->text.buffer[i]=='\t')
		{
			w+=PX_FontGetAscCharactorWidth();
			continue;
		}

		if (pAt->text.buffer[i]&0x80)
		{
			i++;
		}
		else
		{
			if(w+PX_FontGetAscCharactorWidth()>pObject->Width-PX_FontGetAscCharactorWidth()*2)
			{
				w=0;
				h+=PX_FontGetCharactorHeight();
				w+=PX_FontGetAscCharactorWidth();
			}
			else
			{
				w+=PX_FontGetAscCharactorWidth();
			}
		}
	}
	return h+PX_FontGetCharactorHeight();
}

PX_Object_AutoText * PX_Object_GetAutoText(PX_Object *Object)
{
	if(Object->Type==PX_OBJECT_TYPE_AUTOTEXT)
		return (PX_Object_AutoText *)Object->pObject;
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

px_void PX_Object_AutoTextSetText(PX_Object *Obj,const px_char *Text)
{
	PX_Object_AutoText * pAt=PX_Object_GetAutoText(Obj);
	if (!pAt)
	{
		return;
	}
	PX_StringClear(&pAt->text);
	PX_StringCat(&pAt->text,Text);
	Obj->Height=(px_float)PX_Object_AutoTextGetHeight(Obj);
}

