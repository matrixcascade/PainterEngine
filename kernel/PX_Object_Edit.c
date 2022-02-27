#include "PX_Object_Edit.h"


px_void PX_Object_EditGetCursorXY(PX_Object *pObject, px_int *cx, px_int *cy, px_int *height)
{
	px_int x = 0, y = 0, cursor = 0, fsize = 0;
	PX_Object_Edit *pEdit = (PX_Object_Edit *)pObject->pObject;
	const px_char *Text = pEdit->text.buffer;
	px_float objx, objy, objWidth, objHeight;
	px_float inheritX, inheritY;

	PX_ObjectGetInheritXY(pObject, &inheritX, &inheritY);

	objx = (pObject->x + inheritX);
	objy = (pObject->y + inheritY);
	objWidth = pObject->Width;
	objHeight = pObject->Height;


	if (pEdit == PX_NULL)
	{
		return;
	}

	x = pEdit->HorizontalOffset;
	y = pEdit->VerticalOffset;



	while (PX_TRUE)
	{
		fsize = 0;

		if (cursor == pEdit->cursor_index)
		{
			*cx = x;
			*cy = y;
			*height = pEdit->fontModule ? pEdit->fontModule->max_Height : __PX_FONT_HEIGHT;
			return;
		}

		if (pEdit->fontModule)
		{
			px_dword code;
			px_int width, height;
			fsize = PX_FontModuleGetCharacterDesc(pEdit->fontModule, Text + cursor, &code, &width, &height);
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
				x = 0;
				y += pEdit->fontModule->max_Height + pEdit->yFontSpacing;
			}
			else
			{
				x += width + pEdit->xFontSpacing;
			}

			if (pEdit->AutoNewline)
			{
				if (x > objWidth - pEdit->AutoNewLineSpacing)
				{
					x = 0;
					y += pEdit->fontModule->max_Height + pEdit->yFontSpacing;
				}
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
				x = pEdit->HorizontalOffset;
				y += __PX_FONT_HEIGHT + pEdit->yFontSpacing;
			}
			else if (Text[cursor])
			{
				x += __PX_FONT_ASCSIZE + pEdit->xFontSpacing;
			}
			else
			{
				break;
			}

			if (pEdit->AutoNewline)
			{
				if (x > objWidth - pEdit->AutoNewLineSpacing)
				{
					x = 0;
					y += __PX_FONT_HEIGHT + pEdit->yFontSpacing;
				}
			}
		}
		cursor += fsize;
	}

	*cx = x;
	*cy = y;
	*height = pEdit->fontModule ? pEdit->fontModule->max_Height : __PX_FONT_HEIGHT;
}

px_void PX_Object_EditUpdateCursorViewRegion(PX_Object *pObject)
{
	px_int cursorX, cursorY, cursorHeight;
	PX_Object_Edit *pEdit = (PX_Object_Edit *)pObject->pObject;
	px_float objWidth, objHeight;
	objWidth = pObject->Width;
	objHeight = pObject->Height;

	if (pEdit == PX_NULL)
	{
		return;
	}


	PX_Object_EditGetCursorXY(pObject, &cursorX, &cursorY, &cursorHeight);


	if (cursorX > objWidth - 8)
	{
		pEdit->XOffset = cursorX - (px_int)objWidth + 8;
	}
	else
	{
		pEdit->XOffset = 0;
	}

	if (cursorY + cursorHeight > objHeight - 2)
	{
		pEdit->YOffset = cursorY + cursorHeight - (px_int)objHeight + 2;
	}
	else
	{
		pEdit->YOffset = 0;
	}

}

px_void PX_Object_EditOnMouseMove(PX_Object *Object,PX_Object_Event e,px_void *user_ptr)
{
	PX_Object_Edit *pEdit=PX_Object_GetEdit(Object);
	px_float x,y;
	x=PX_Object_Event_GetCursorX(e);
	y=PX_Object_Event_GetCursorY(e);

	if (pEdit)
	{
		if(PX_ObjectIsPointInRegion(Object,(px_float)x,(px_float)y))
		{
			pEdit->state=PX_OBJECT_EDIT_STATE_ONCURSOR;
		}
		else
		{
			pEdit->state=PX_OBJECT_EDIT_STATE_NORMAL;
		}
	}
}
px_void PX_Object_EditUpdateCursorOnDown(PX_Object *pObject,px_int cx,px_int cy);
px_void PX_Object_EditOnMouseLButtonDown(PX_Object *Object,PX_Object_Event e,px_void *user_ptr)
{
	PX_Object_Edit *pEdit=PX_Object_GetEdit(Object);
	px_float x,y;
	px_float objx,objy;
	px_float inheritX,inheritY;

	PX_ObjectGetInheritXY(Object,&inheritX,&inheritY);

	objx=(Object->x+inheritX);
	objy=(Object->y+inheritY);

	x=PX_Object_Event_GetCursorX(e);
	y=PX_Object_Event_GetCursorY(e);

	if (pEdit)
	{
		if(PX_ObjectIsPointInRegion(Object,(px_float)x,(px_float)y))
		{
			PX_Object_EditSetFocus(Object,PX_TRUE);
			PX_Object_EditUpdateCursorOnDown(Object,(px_int)(x-objx),(px_int)(y-objy));
		}
		else
			PX_Object_EditSetFocus(Object,PX_FALSE);
	}
}

px_void PX_Object_EditOnKeyboardString(PX_Object *Object,PX_Object_Event e,px_void *user_ptr)
{
	PX_Object_Edit *pEdit=PX_Object_GetEdit(Object);

	if (pEdit->onFocus)
	{
		PX_Object_EditAddString(Object,PX_Object_Event_GetStringPtr(e));
	}
}

static px_void PX_Object_EditCheckCursor(PX_Object_Edit*pedit)
{
	if (pedit->cursor_index<0)
	{
		pedit->cursor_index=0;
	}
	if (pedit->cursor_index>PX_strlen(pedit->text.buffer))
	{
		pedit->cursor_index=PX_strlen(pedit->text.buffer);
	}
}

PX_Object* PX_Object_EditCreate(px_memorypool *mp, PX_Object *Parent,px_int x,px_int y,px_int Width,px_int Height,PX_FontModule *fontModule )
{
	PX_Object *pObject;
	PX_Object_Edit *pEdit=(PX_Object_Edit *)MP_Malloc(mp,sizeof(PX_Object_Edit));
	if (pEdit==PX_NULL)
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
		MP_Free(mp,pEdit);
		return PX_NULL;
	}

	if(!PX_SurfaceCreate(mp,Width,Height,&pEdit->EditSurface))
	{
		MP_Free(mp,pEdit);
		MP_Free(mp,pObject);
		return PX_NULL;
	}

	pObject->pObject=pEdit;
	pObject->Type=PX_OBJECT_TYPE_EDIT;
	pObject->ReceiveEvents=PX_TRUE;
	pObject->Func_ObjectFree=PX_Object_EditFree;
	pObject->Func_ObjectRender=PX_Object_EditRender;



	PX_StringInitialize(mp,&pEdit->text);


	pEdit->TextColor=PX_OBJECT_UI_DEFAULT_FONTCOLOR;

	pEdit->CursorColor=PX_COLOR_WHITE;
	pEdit->BorderColor=PX_OBJECT_UI_DEFAULT_BORDERCOLOR;
	pEdit->BackgroundColor=PX_OBJECT_UI_DEFAULT_BACKGROUNDCOLOR;
	pEdit->XOffset=0;
	pEdit->YOffset=0;
	pEdit->Border=PX_TRUE;
	pEdit->Password=PX_FALSE;
	pEdit->onFocus=PX_FALSE;
	pEdit->state=PX_OBJECT_EDIT_STATE_NORMAL;
	pEdit->AutoNewline=PX_FALSE;
	pEdit->xFontSpacing=0;
	pEdit->yFontSpacing=0;
	pEdit->HorizontalOffset=3;
	pEdit->VerticalOffset=3;
	pEdit->cursor_index=0;
	pEdit->max_length=-1;
	pEdit->fontModule=fontModule;
	pEdit->AutoNewLineSpacing=__PX_FONT_ASCSIZE+2;
	pEdit->style=PX_OBJECT_EDIT_STYLE_RECT;
	pEdit->multiLines=PX_FALSE;
	PX_ObjectRegisterEvent(pObject,PX_OBJECT_EVENT_CURSORMOVE,PX_Object_EditOnMouseMove,PX_NULL);
	PX_ObjectRegisterEvent(pObject,PX_OBJECT_EVENT_CURSORDOWN,PX_Object_EditOnMouseLButtonDown,PX_NULL);
	PX_ObjectRegisterEvent(pObject,PX_OBJECT_EVENT_STRING,PX_Object_EditOnKeyboardString,PX_NULL);

	return pObject;
}

PX_Object_Edit  * PX_Object_GetEdit( PX_Object *Object )
{
	if(Object->Type==PX_OBJECT_TYPE_EDIT)
		return (PX_Object_Edit *)Object->pObject;
	else
		return PX_NULL;
}

px_char	  * PX_Object_EditGetText( PX_Object *Object )
{
	PX_Object_Edit *pEdit=PX_Object_GetEdit(Object);
	if (pEdit!=PX_NULL)
	{
		return pEdit->text.buffer;
	}
	return PX_NULL;
}

px_void PX_Object_EditSetMaxTextLength(PX_Object *pObject,px_int max_length)
{
	PX_Object_Edit *pEdit=PX_Object_GetEdit(pObject);
	if (pEdit!=PX_NULL)
	{
		pEdit->max_length=max_length;
	}
}

px_void PX_Object_EditSetText( PX_Object *pObject,const px_char *Text )
{
	PX_Object_Edit *pEdit=PX_Object_GetEdit(pObject);
	if(pEdit)
	{
		PX_StringClear(&pEdit->text);
		PX_StringCat(&pEdit->text,Text);
		pEdit->cursor_index=PX_strlen(Text);
		PX_Object_EditUpdateCursorViewRegion(pObject);
	}
}

px_void PX_Object_EditAppendText(PX_Object *pObject,const px_char *Text)
{
	PX_Object_Edit *pEdit=PX_Object_GetEdit(pObject);
	if(pEdit)
	{
		PX_StringCat(&pEdit->text,Text);
		pEdit->cursor_index=PX_strlen(Text);
		PX_Object_EditUpdateCursorViewRegion(pObject);
	}
}

px_void PX_Object_EditSetFocus(PX_Object *pObject,px_bool OnFocus)
{
	PX_Object_Edit *pEdit=PX_Object_GetEdit(pObject);
	if(pEdit)
	{
		if (pEdit->onFocus!=OnFocus)
		{
			if (OnFocus)
			{
				PX_Object_Event e;
				pEdit->onFocus=OnFocus;
				e.Event=PX_OBJECT_EVENT_ONFOCUS;
				e.Param_bool[0]=OnFocus;
				PX_ObjectPostEvent(pObject,e);
			}
			else
			{
				PX_Object_Event e;
				pEdit->onFocus=OnFocus;
				e.Event=PX_OBJECT_EVENT_LOSTFOCUS;
				e.Param_bool[0]=OnFocus;
				PX_ObjectPostEvent(pObject,e);
			}
			
		}

	}
}

px_void PX_Object_EditSetPasswordStyle( PX_Object *pLabel,px_uchar Enabled )
{
	PX_Object_Edit *pEdit=PX_Object_GetEdit(pLabel);
	if (pEdit)
	{
		pEdit->Password=Enabled;
		pEdit->fontModule=PX_NULL;
	}
}
px_void PX_Object_EditSetBackgroundColor( PX_Object *pObject,px_color Color )
{
	PX_Object_Edit * pEdit=PX_Object_GetEdit(pObject);
	if (pEdit)
	{
		pEdit->BackgroundColor=Color;
	}
}

px_void PX_Object_EditSetBorderColor( PX_Object *pObject,px_color Color )
{
	PX_Object_Edit * pEdit=PX_Object_GetEdit(pObject);
	if (pEdit)
	{

		pEdit->BorderColor=Color;
	}
}

px_void PX_Object_EditSetCursorColor( PX_Object *pObject,px_color Color )
{
	PX_Object_Edit * pEdit=PX_Object_GetEdit(pObject);
	if (pEdit)
	{

		pEdit->CursorColor=Color;
	}
}

px_void PX_Object_EditSetTextColor(PX_Object *pObject,px_color Color)
{
	PX_Object_Edit * pEdit=PX_Object_GetEdit(pObject);
	if (pEdit)
	{
		pEdit->TextColor=Color;
	}
}

px_void PX_Object_EditSetLimit(PX_Object *pObject,const px_char *Limit)
{
	PX_Object_Edit * pEdit=PX_Object_GetEdit(pObject);
	if (pEdit)
	{
		PX_strcpy(pEdit->Limit,Limit,sizeof(pEdit->Limit));
	}
}

px_void PX_Object_EditSetStyle(PX_Object *pObject,PX_OBJECT_EDIT_STYLE style)
{
	PX_Object_Edit * pEdit=PX_Object_GetEdit(pObject);
	if (pEdit)
	{
		pEdit->style=style;
	}
}

px_void PX_Object_EditSetBorder( PX_Object *pObj,px_bool Border )
{
	PX_Object_Edit *pEdit=PX_Object_GetEdit(pObj);
	if (pEdit)
	{

		pEdit->Border=Border;
	}
}



px_void PX_Object_EditUpdateCursorOnDown(PX_Object *pObject,px_int cx,px_int cy)
{
	px_int x_draw_oft=0,y_draw_oft=0,x=0,y=0,cursor=0,fsize=0;
	PX_Object_Edit *pEdit=(PX_Object_Edit *)pObject->pObject;
	const px_char *Text=pEdit->text.buffer;
	px_float objx,objy,objWidth,objHeight;
	px_float inheritX,inheritY;

	PX_ObjectGetInheritXY(pObject,&inheritX,&inheritY);

	objx=(pObject->x+inheritX);
	objy=(pObject->y+inheritY);
	objWidth=pObject->Width;
	objHeight=pObject->Height;

	if (pEdit==PX_NULL)
	{
		return;
	}

	x=pEdit->HorizontalOffset;
	y=pEdit->VerticalOffset;



	while (PX_TRUE)
	{
		fsize=0;
		x_draw_oft=x-pEdit->XOffset;
		y_draw_oft=y-pEdit->YOffset;

		if (pEdit->fontModule)
		{
			px_dword code;
			px_int width,height;
			fsize=PX_FontModuleGetCharacterDesc(pEdit->fontModule,Text+cursor,&code,&width,&height);
			if (!fsize)
			{
				break;
			}
			if (code=='\r')
			{
				//skip
			}else if (code=='\n')
			{
				x=pEdit->HorizontalOffset;
				y+=pEdit->fontModule->max_Height+pEdit->yFontSpacing;
			}
			else
			{
				if (cx<=x_draw_oft+width/2+pEdit->xFontSpacing/2&&cy<=y_draw_oft+pEdit->fontModule->max_Height+pEdit->yFontSpacing/2)
				{
					pEdit->cursor_index=cursor;
					return;
				}
				else
				{
					pEdit->cursor_index=cursor+fsize;
				}

				x+=width+pEdit->xFontSpacing;
			}

			if (pEdit->AutoNewline)
			{
				if (x>objWidth-pEdit->AutoNewLineSpacing)
				{
					x=pEdit->HorizontalOffset;
					y+=pEdit->fontModule->max_Height+pEdit->yFontSpacing;
				}
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
				x=0;
				y+=__PX_FONT_HEIGHT+pEdit->yFontSpacing;
			}
			else if(Text[cursor])
			{
				if (cx<=x_draw_oft+__PX_FONT_ASCSIZE/2+pEdit->xFontSpacing/2&&cy<=y_draw_oft+__PX_FONT_HEIGHT+pEdit->yFontSpacing/2)
				{
					pEdit->cursor_index=cursor;
					return;
				}
				else
				{
					pEdit->cursor_index=cursor+fsize;
				}

				x+=__PX_FONT_ASCSIZE+pEdit->xFontSpacing;
			}
			else
			{
				break;
			}

			if (pEdit->AutoNewline)
			{
				if (x>objWidth-pEdit->AutoNewLineSpacing)
				{
					x=pEdit->HorizontalOffset;
					y+=__PX_FONT_HEIGHT+pEdit->yFontSpacing;
				}
			}
		}
		cursor+=fsize;
	}

}


px_void PX_Object_EditRender(px_surface *psurface, PX_Object *pObject,px_uint elapsed)
{
	px_int x_draw_oft,y_draw_oft,x,y,cursor,fsize;
	PX_Object_Edit *pEdit=(PX_Object_Edit *)pObject->pObject;
	const px_char *Text=pEdit->text.buffer;
	px_float objx,objy,objWidth,objHeight;
	px_float inheritX,inheritY;

	PX_ObjectGetInheritXY(pObject,&inheritX,&inheritY);

	objx=(pObject->x+inheritX);
	objy=(pObject->y+inheritY);
	objWidth=pObject->Width;
	objHeight=pObject->Height;

	if (pEdit==PX_NULL)
	{
		return;
	}

	if (!pObject->Visible)
	{
		return;
	}

	if (pEdit->EditSurface.width!=(px_int)objWidth||pEdit->EditSurface.height!=(px_int)objHeight)
	{
		PX_SurfaceFree(&pEdit->EditSurface);
		if(!PX_SurfaceCreate(pObject->mp,(px_int)objWidth,(px_int)objHeight,&pEdit->EditSurface))
		{
			return;
		}
	}


	if (pEdit->style==PX_OBJECT_EDIT_STYLE_RECT)
	{
		//clear
		PX_SurfaceClear(&pEdit->EditSurface,0,0,(px_int)objWidth-1,(px_int)objHeight-1,pEdit->BackgroundColor);
		if (pEdit->Border)
		{
			if(pEdit->state==PX_OBJECT_EDIT_STATE_NORMAL)
				PX_GeoDrawBorder(&pEdit->EditSurface,0,0,(px_int)objWidth-1,(px_int)objHeight-1,1,pEdit->BorderColor);
			else
				PX_GeoDrawBorder(&pEdit->EditSurface,0,0,(px_int)objWidth-1,(px_int)objHeight-1,1,pEdit->CursorColor);
		}
	}
	else if(pEdit->style==PX_OBJECT_EDIT_STYLE_ROUNDRECT)
	{
		PX_SurfaceClear(&pEdit->EditSurface,0,0,(px_int)objWidth-1,(px_int)objHeight-1,PX_COLOR(0,0,0,0));
		PX_GeoDrawSolidRoundRect(&pEdit->EditSurface,0,0,(px_int)objWidth-1,(px_int)objHeight-1,objHeight/2,pEdit->BackgroundColor);
		if (pEdit->Border)
		{
			if(pEdit->state==PX_OBJECT_EDIT_STATE_NORMAL)
				PX_GeoDrawRoundRect(&pEdit->EditSurface,0,0,(px_int)objWidth-1,(px_int)objHeight-1,objHeight/2,1,pEdit->BorderColor);
			else
				PX_GeoDrawRoundRect(&pEdit->EditSurface,0,0,(px_int)objWidth-1,(px_int)objHeight-1,objHeight/2,1,pEdit->CursorColor);
		}
	}



	x=pEdit->HorizontalOffset;
	y=pEdit->VerticalOffset;

	cursor=0;

	while (PX_TRUE)
	{
		fsize=0;
		x_draw_oft=x-pEdit->XOffset;
		y_draw_oft=y-pEdit->YOffset;

		//Draw Cursor
		if (pEdit->onFocus&&pEdit->cursor_index==cursor)
		{
			pEdit->elapsed+=elapsed;
			if ((pEdit->elapsed/200)&1)
			{
				if (pEdit->fontModule)
				{
					PX_GeoDrawRect(&pEdit->EditSurface,x_draw_oft+1,y_draw_oft,x_draw_oft,y_draw_oft+pEdit->fontModule->max_Height-2,pEdit->TextColor);
				}
				else
				{
					PX_GeoDrawRect(&pEdit->EditSurface,x_draw_oft+1,y_draw_oft,x_draw_oft,y_draw_oft+__PX_FONT_HEIGHT-1,pEdit->TextColor);
				}

			}
		}


		if (pEdit->fontModule)
		{
			px_dword code;
			px_int width,height;
			fsize=PX_FontModuleGetCharacterDesc(pEdit->fontModule,Text+cursor,&code,&width,&height);
			if (!fsize)
			{
				break;
			}
			if (code=='\r')
			{
				//skip
			}else if (code=='\n')
			{
				x=pEdit->HorizontalOffset;
				y+=pEdit->fontModule->max_Height+pEdit->yFontSpacing;
			}
			else
			{
				PX_FontModuleDrawCharacter(&pEdit->EditSurface,pEdit->fontModule,x_draw_oft,y_draw_oft,code,pEdit->TextColor);
				x+=width+pEdit->xFontSpacing;
			}

			if (pEdit->AutoNewline)
			{
				if (x>objWidth-pEdit->AutoNewLineSpacing)
				{
					x=0;
					y+=pEdit->fontModule->max_Height+pEdit->yFontSpacing;
				}
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
				x=pEdit->HorizontalOffset;
				y+=__PX_FONT_HEIGHT+pEdit->yFontSpacing;
			}
			else if(Text[cursor])
			{
				if (pEdit->Password)
				{
					PX_FontDrawChar(&pEdit->EditSurface,x_draw_oft,y_draw_oft,'*',pEdit->TextColor);
					x+=__PX_FONT_ASCSIZE;
				}
				else
				{
					PX_FontDrawChar(&pEdit->EditSurface,x_draw_oft,y_draw_oft,Text[cursor],pEdit->TextColor);
					x+=__PX_FONT_ASCSIZE;
				}

			}
			else
			{
				break;
			}

			if (pEdit->AutoNewline)
			{
				if (x>objWidth-pEdit->AutoNewLineSpacing)
				{
					x=pEdit->HorizontalOffset;
					y+=__PX_FONT_HEIGHT+pEdit->yFontSpacing;
				}
			}
		}
		cursor+=fsize;
	}

	PX_SurfaceRender(psurface,&pEdit->EditSurface,(px_int)objx,(px_int)objy,PX_ALIGN_LEFTTOP,PX_NULL);
}

px_void PX_Object_EditFree( PX_Object *pObject )
{
	PX_Object_Edit *pEdit=PX_Object_GetEdit(pObject);
	if (pObject!=PX_NULL)
	{
		PX_StringFree(&pEdit->text);
		PX_TextureFree(&pEdit->EditSurface);
	}
}

px_void PX_Object_EditAddString(PX_Object *pObject,px_char *Text)
{
	PX_Object_Edit *pEdit=PX_Object_GetEdit(pObject);
	PX_Object_Event e;
	if (pObject!=PX_NULL)
	{
		PX_Object_EditCheckCursor(pEdit);
		while (*Text)
		{

			if(*Text!=8)
			{
				if (!pEdit->multiLines&&Text[1]==0&&(*Text=='\n'||*Text=='\r'))
				{
					return;
				}

				if (pEdit->Limit[0])
				{
					px_bool bMatch=PX_FALSE;
					const px_char *limitFilter=pEdit->Limit;
					while (*limitFilter)
					{
						if (*limitFilter==*Text)
						{
							bMatch=PX_TRUE;
							break;
						}
						limitFilter++;
					}
					if (!bMatch)
					{
						Text++;
						continue;
					}
				}

				if (pEdit->max_length>0)
				{
					if (PX_StringLen(&pEdit->text)<pEdit->max_length)
					{
						px_char ch=*Text;
						if (ch=='\r')
						{
							ch='\n';
						}
						if (pEdit->inputmode == PX_OBJECT_EDIT_INPUT_MODE_LOWERCASE)
						{
							if (ch>='A'&& ch <= 'Z')
							{
								ch += 'a' - 'A';
							}
						}
						else if(pEdit->inputmode == PX_OBJECT_EDIT_INPUT_MODE_UPPERCASE)
						{
							if (ch >= 'a' && ch <= 'z')
							{
								ch += 'A' - 'a';
							}
						}
						PX_StringInsertChar(&pEdit->text,pEdit->cursor_index,ch);
						pEdit->cursor_index++;
					}
				}
				else
				{
					px_char ch=*Text;
					if (ch=='\r')
					{
						ch='\n';
					}
					if (pEdit->inputmode == PX_OBJECT_EDIT_INPUT_MODE_LOWERCASE)
					{
						if (ch >= 'A' && ch <= 'Z')
						{
							ch += 'a' - 'A';
						}
					}
					else if (pEdit->inputmode == PX_OBJECT_EDIT_INPUT_MODE_UPPERCASE)
					{
						if (ch >= 'a' && ch <= 'z')
						{
							ch += 'A' - 'a';
						}
					}
					PX_StringInsertChar(&pEdit->text,pEdit->cursor_index,ch);
					pEdit->cursor_index++;
				}

			}
			else
			{
				PX_Object_EditBackspace(pObject);
			}
			Text++;
		}
	}
	PX_Object_EditUpdateCursorViewRegion(pObject);
	e.Event=PX_OBJECT_EVENT_VALUECHANGED;
	e.Param_ptr[0]=0;
	PX_ObjectExecuteEvent(pObject,e);
}

px_void PX_Object_EditBackspace(PX_Object *pObject)
{
	PX_Object_Event e;
	PX_Object_Edit *pEdit=PX_Object_GetEdit(pObject);


	if (pObject!=PX_NULL&&pEdit->onFocus)
	{
		px_char *pText;
		pText=pEdit->text.buffer;

		if (!pEdit->fontModule)
		{
			if (pEdit->cursor_index)
			{
				PX_StringRemoveChar(&pEdit->text,pEdit->cursor_index-1);
				pEdit->cursor_index--;
			}

			return;
		}

		switch(pEdit->fontModule->codePage)
		{
		case PX_FONTMODULE_CODEPAGE_GBK:
			{
				px_int idx=0;
				px_int c=0;
				while (PX_TRUE)
				{

					if (idx==pEdit->cursor_index)
					{
						break;
					}

					if (pText[idx]==0)
					{
						break;
					}

					if (pText[idx]&0x80)
					{
						c=2;
						idx+=2;
					}
					else
					{
						idx++;
						c=1;
					}
				}

				while (c)
				{
					PX_StringRemoveChar(&pEdit->text,pEdit->cursor_index-1);
					pEdit->cursor_index--;
					c--;
				}

			}
			break;
		case PX_FONTMODULE_CODEPAGE_UTF8:
			{
				px_int i;
				for (i=0;i<6;i++)
				{
					if (pEdit->cursor_index==0)
					{
						break;
					}

					if ((pText[pEdit->cursor_index-1]&0x80)==0x00)
					{
						PX_StringRemoveChar(&pEdit->text,pEdit->cursor_index-1);
						pEdit->cursor_index--;
						break;
					}

					if ((pText[pEdit->cursor_index-1]&0xc0)==0x80)
					{
						PX_StringRemoveChar(&pEdit->text,pEdit->cursor_index-1);
						pEdit->cursor_index--;
						continue;
					}

					if ((pText[pEdit->cursor_index-1]&0xc0)==0xc0)
					{
						PX_StringRemoveChar(&pEdit->text,pEdit->cursor_index-1);
						pEdit->cursor_index--;
						break;
					}

				}
			}
			break;
		case PX_FONTMODULE_CODEPAGE_UTF16:
			{
				//not support
			}
			break;
		default:
			{
				//not support
			}
			break;
		}

		e.Event=PX_OBJECT_EVENT_VALUECHANGED;
		e.Param_ptr[0]=0;
		PX_ObjectExecuteEvent(pObject,e);
	}
}

px_void PX_Object_EditAutoNewLine(PX_Object *pObject,px_bool b,px_int AutoNewLineSpacing)
{
	PX_Object_Edit *pEdit=PX_Object_GetEdit(pObject);
	if (pObject!=PX_NULL)
	{
		pEdit->AutoNewline=b;
		pEdit->AutoNewLineSpacing=AutoNewLineSpacing;
	}
}

px_void PX_Object_EditSetOffset(PX_Object *pObject,px_int TopOffset,px_int LeftOffset)
{
	PX_Object_Edit *pEdit=PX_Object_GetEdit(pObject);
	if (pObject!=PX_NULL)
	{
		pEdit->VerticalOffset=TopOffset;
		pEdit->HorizontalOffset=LeftOffset;
	}
}

px_void PX_Object_EditSetXYOffset(PX_Object* pObject, px_int XOffset, px_int YOffset)
{
	PX_Object_Edit* pEdit = PX_Object_GetEdit(pObject);
	if (pEdit != PX_NULL)
	{
		pEdit->XOffset = XOffset;
		pEdit->YOffset = YOffset;
	}
}
px_void PX_Object_EditSetInputMode(PX_Object* pObject, PX_OBJECT_EDIT_INPUT_MODE mode)
{
	PX_Object_Edit* pEdit = PX_Object_GetEdit(pObject);
	if (pEdit != PX_NULL)
	{
		pEdit->inputmode = mode;
	}
}