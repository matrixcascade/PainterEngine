#include "PX_Object_MemoryView.h"

//////////////////////////////////////////////////////////////////////////
// 0x00000000  00 a --
// 16+64     16 16+8+8+16

static px_int PX_Object_MemoryViewGetCursorMapIndex(PX_Object *pObject,px_float cx,px_float cy)
{
	PX_Object_MemoryView *pdesc;
	px_int x,y,w,h;
	px_float inheritX,inheritY;
	px_int row;
	px_int columnoffset;
	px_int rel_address_offset;
	px_int xIndex,yIndex,Index;
	pdesc=PX_ObjectGetDesc(PX_Object_MemoryView,pObject);
	PX_ObjectGetInheritXY(pObject,&inheritX,&inheritY);
	x=(px_int)(pObject->x+inheritX);
	y=(px_int)(pObject->y+inheritY);
	w=(px_int)pObject->Width;
	h=(px_int)pObject->Height;

	row=(w-(16+64+16+8+16))/32;

	cx-=x;
	cy-=y;

	columnoffset=PX_Object_SliderBarGetValue(pdesc->scrollbar);
	rel_address_offset=row*columnoffset;

	if (cx<96+row*24)
	{
		xIndex=(px_int)((cx-96)/8-(cx-96)/24);
		if(xIndex<0)xIndex=0;
	}
	else if(cx<w-16)
	{
		xIndex=(px_int)((cx-96-row*24)/8)*2;
		if(xIndex>=row*2) xIndex=row*2-2;
	}
	else
	{
		return -1;
	}

	yIndex=(px_int)(cy/16);

	Index=yIndex*row*2+rel_address_offset*2+xIndex;
	if (Index<0)
	{
		Index=0;
	}

	if (Index>=pdesc->size*2)
	{
		Index=pdesc->size*2-1;
	}
	return Index;
}


px_void PX_Object_MemoryViewRender(px_surface *prendersurface,PX_Object *pObject,px_dword elapsed)
{
	px_int row,column,i,j;
	px_int rel_address_offset;
	px_int mst,med;
	PX_Object_MemoryView *pdesc;
	px_int x,y,w,h;
	px_float inheritX,inheritY;
	px_int columnoffset;
	px_int drawx,drawy;

	
	
	pdesc=PX_ObjectGetDesc(PX_Object_MemoryView,pObject);
	pdesc->cursor_elapsed+=elapsed;
	columnoffset=PX_Object_SliderBarGetValue(pdesc->scrollbar);

	PX_ObjectGetInheritXY(pObject,&inheritX,&inheritY);
	x=(px_int)(pObject->x+inheritX);
	y=(px_int)(pObject->y+inheritY);
	w=(px_int)pObject->Width;
	h=(px_int)pObject->Height;


	pdesc->scrollbar->x=w-16.f;
	pdesc->scrollbar->y=0;
	pdesc->scrollbar->Width=16.f;
	pdesc->scrollbar->Height=(px_float)h;

	row=(w-(16+64+16+8+16))/32;

	if (row<1)
	{
		return;
	}

	column=h/(16);
	if (column<1)
	{
		return;
	}

	mst=pdesc->mark_start_cursor;
	med=pdesc->mark_end_cursor;
	if (mst>med)
	{
		mst^=med;
		med=mst^med;
		mst^=med;
	}


	


	//////////////////////////////////////////////////////////////////////////
	//render background
	PX_GeoDrawRect(prendersurface,x,y,x+w,y+h,pdesc->backgroundcolor);

	/////////////////////////////////////////////////////////////////////////
	//render border
	PX_GeoDrawBorder(prendersurface, x, y, x + w, y + h, 1,pdesc->bordercolor);

	rel_address_offset=row*columnoffset;

	drawy=y;
	for (j=0;j<column;j++)
	{
		px_char _addrhex[9];
		px_char addrhex[9] = {0};
		px_dword addr;
		if (rel_address_offset>=pdesc->size)
		{
			break;
		}
		addr=rel_address_offset+pdesc->address_offset;
		
		
		drawx=x;
		//0x
		PX_FontDrawChar(prendersurface,drawx,drawy,'0',pdesc->fontcolor);
		drawx+=8;

		PX_FontDrawChar(prendersurface,drawx,drawy,'x',pdesc->fontcolor);
		drawx+=8;

		
		PX_itoa(addr, _addrhex,9,16);
		PX_memset(addrhex, '0', 8);
		PX_memcpy(addrhex+ 8-PX_strlen(_addrhex), _addrhex,PX_strlen(_addrhex));
		PX_FontDrawText(prendersurface,drawx,drawy,PX_ALIGN_LEFTTOP,addrhex,pdesc->fontcolor);
		drawx+=8*8;

		drawx+=16;

		for (i=0;i<row;i++)
		{
			px_char hex[3];
			px_byte b;

			if (rel_address_offset>=pdesc->size)
			{
				break;
			}
			b=((px_byte *)pdesc->pdata)[rel_address_offset];
			PX_BufferToHexString(&b,1,hex);
			PX_strupr(hex);

			if (rel_address_offset*2>=mst&&rel_address_offset*2<=med)
			{
				PX_GeoDrawRect(prendersurface,drawx,drawy,drawx+7,drawy+15,pdesc->selectcolor);
			}

			if ((rel_address_offset*2>=mst&&rel_address_offset*2<=med)||(rel_address_offset*2+1>=mst&&rel_address_offset*2+1<=med))
			{
				PX_GeoDrawRect(prendersurface,x+96+row*24+8*i,drawy,x+96+row*24+8*i+7,drawy+15,pdesc->selectcolor);
			}

			if (rel_address_offset*2==pdesc->cursor)
			{
				if ((pdesc->cursor_elapsed/200)&1)
				{
					PX_GeoDrawRect(prendersurface,drawx,drawy,drawx+7,drawy+15,pdesc->cursorcolor);
				}
			}
			if (rel_address_offset*2==pdesc->cursor||rel_address_offset*2+1==pdesc->cursor)
			{
				PX_GeoDrawRect(prendersurface, x + 96+row*24+8*i,drawy, x + 96+row*24+8*i+7,drawy+15,pdesc->selectcolor);
			}
			

			PX_FontDrawChar(prendersurface,drawx,drawy,hex[0],pdesc->fontcolor);
			if (b>=32&&b<=126)
			{
				PX_FontDrawChar(prendersurface, x + 96+row*24+8*i,drawy,b,pdesc->fontcolor);
			}

			drawx+=8;
			if (rel_address_offset*2+1>=mst&&rel_address_offset*2+1<=med)
			{
				PX_GeoDrawRect(prendersurface,drawx,drawy,drawx+7,drawy+15,pdesc->selectcolor);
			}

			if (rel_address_offset*2+1==pdesc->cursor)
			{
				if ((pdesc->cursor_elapsed/200)&1)
				{
					PX_GeoDrawRect(prendersurface,drawx,drawy,drawx+7,drawy+15,pdesc->cursorcolor);
				}
			}
			PX_FontDrawChar(prendersurface,drawx,drawy,hex[1],pdesc->fontcolor);
			drawx+=16;
			rel_address_offset++;
		}
		
		drawy+=16;

	}
	

}

px_void PX_Object_MemoryViewOnCursorClick(PX_Object *pObject,PX_Object_Event e,px_void *ptr)
{
	px_float x,y;
	PX_Object_MemoryView *pdesc;

	if (!PX_ObjectIsCursorInRegion(pObject,e))
	{
		return;
	}

	pdesc=PX_ObjectGetDesc(PX_Object_MemoryView,pObject);
	
	pdesc->mark_end_cursor=-1;
	pdesc->mark_start_cursor=-1;
	x=PX_Object_Event_GetCursorX(e);
	y=PX_Object_Event_GetCursorY(e);
	
	pdesc->cursor=PX_Object_MemoryViewGetCursorMapIndex(pObject,x,y);
}

px_void PX_Object_MemoryViewOnCursorDrag(PX_Object *pObject,PX_Object_Event e,px_void *ptr)
{
	px_float x,y;
	PX_Object_MemoryView *pdesc;

	if (!PX_ObjectIsCursorInRegion(pObject,e))
	{
		return;
	}

	pdesc=PX_ObjectGetDesc(PX_Object_MemoryView,pObject);
	x=PX_Object_Event_GetCursorX(e);
	y=PX_Object_Event_GetCursorY(e);

	pdesc->mark_end_cursor=PX_Object_MemoryViewGetCursorMapIndex(pObject,x,y);
	
}

px_void PX_Object_MemoryViewOnCursorDown(PX_Object *pObject,PX_Object_Event e,px_void *ptr)
{
	px_float x,y;
	PX_Object_MemoryView *pdesc;

	if (!PX_ObjectIsCursorInRegion(pObject,e))
	{
		return;
	}

	pdesc=PX_ObjectGetDesc(PX_Object_MemoryView,pObject);
	pdesc->mark_end_cursor=-1;
	pdesc->mark_start_cursor=-1;
	pdesc->cursor=-1;
	x=PX_Object_Event_GetCursorX(e);
	y=PX_Object_Event_GetCursorY(e);

	pdesc->mark_start_cursor=pdesc->mark_end_cursor=PX_Object_MemoryViewGetCursorMapIndex(pObject,x,y);
}

px_void PX_Object_MemoryViewOnKeyDown(PX_Object *pObject,PX_Object_Event e,px_void *ptr)
{
	PX_Object_MemoryView *pdesc=PX_ObjectGetDesc(PX_Object_MemoryView,pObject);
	PX_Object_Event ne;
	px_char key=PX_Object_Event_GetKeyDown(e);

	if ((key>='A'&&key<='F')||(key>='a'&&key<='f')||(key>='0'&&key<='9'))
	{
		if (pdesc->cursor>=0&&pdesc->cursor<pdesc->size*2)
		{
			px_int index=pdesc->cursor/2;
			px_uchar value=0;
			if ((key>='A'&&key<='F'))
			{
				value=key-'A'+10;
			}
			else if ((key>='a'&&key<='f'))
			{
				value=key-'a'+10;
			}
			else if ((key>='0'&&key<='9'))
			{
				value=key-'0';
			}

			if (pdesc->cursor&1)
			{
				((px_byte *)pdesc->pdata)[index]&=0xf0;
				((px_byte *)pdesc->pdata)[index]|=(value);
			}
			else
			{
				((px_byte *)pdesc->pdata)[index]&=0x0f;
				((px_byte *)pdesc->pdata)[index]|=(value<<4);
			}


			ne.Event=PX_OBJECT_EVENT_VALUECHANGED;
			ne.Param_uint[0]=index;
			ne.Param_uint[1]=((px_byte *)pdesc->pdata)[index];
			PX_ObjectExecuteEvent(pObject,ne);

			if (pdesc->cursor<pdesc->size*2-1)
			{
				pdesc->cursor++;
			}
		}
	}
	
}


PX_Object * PX_Object_MemoryViewCreate(px_memorypool *mp,PX_Object *pParent,px_int x,px_int y,px_int Width,px_int Height)
{
	PX_Object_MemoryView desc,*pdesc;
	PX_Object *pObject;
	PX_memset(&desc,0,sizeof(desc));
	desc.backgroundcolor=PX_OBJECT_UI_DEFAULT_BACKGROUNDCOLOR;
	desc.cursor=-1;
	desc.mark_end_cursor=-1;
	desc.mark_start_cursor=-1;
	desc.cursorcolor=PX_COLOR(64,255,255,255);
	desc.fontcolor=PX_OBJECT_UI_DEFAULT_FONTCOLOR;
	desc.selectcolor=PX_COLOR(64,255,255,255);
	desc.bordercolor = PX_OBJECT_UI_DEFAULT_BORDERCOLOR;
	pObject=PX_ObjectCreateEx(mp,pParent,(px_float)x,(px_float)y,0,(px_float)Width,(px_float)Height,0,PX_OBJECT_TYPE_MEMORYVIEW,PX_NULL,PX_Object_MemoryViewRender,PX_NULL,&desc,sizeof(desc));
	pdesc=PX_ObjectGetDesc(PX_Object_MemoryView,pObject);
	pdesc->scrollbar=PX_Object_SliderBarCreate(mp,pObject,0,0,10,10,PX_OBJECT_SLIDERBAR_TYPE_VERTICAL,PX_OBJECT_SLIDERBAR_STYLE_BOX);
	pdesc->scrollbar->Visible=PX_FALSE;

	PX_ObjectRegisterEvent(pObject,PX_OBJECT_EVENT_CURSORCLICK,PX_Object_MemoryViewOnCursorClick,PX_NULL);
	PX_ObjectRegisterEvent(pObject,PX_OBJECT_EVENT_CURSORDOWN,PX_Object_MemoryViewOnCursorDown,PX_NULL);
	PX_ObjectRegisterEvent(pObject,PX_OBJECT_EVENT_CURSORDRAG,PX_Object_MemoryViewOnCursorDrag,PX_NULL);
	PX_ObjectRegisterEvent(pObject,PX_OBJECT_EVENT_KEYDOWN,PX_Object_MemoryViewOnKeyDown,PX_NULL);
	return pObject;
}

PX_Object_MemoryView * PX_Object_GetMemoryView(PX_Object *pObject)
{
	if (pObject->Type!=PX_OBJECT_TYPE_MEMORYVIEW)
	{
		PX_ASSERT();
		return PX_NULL;
	}
	return (PX_Object_MemoryView * )pObject->pObject;
}

px_void PX_Object_MemoryViewSetFontColor(PX_Object *pObject,px_color Color)
{
	PX_Object_MemoryView *pdesc=PX_Object_GetMemoryView(pObject);
	if(pdesc)
	pdesc->fontcolor=Color;
}

px_void PX_Object_MemoryViewSetBackgroundColor(PX_Object *pObject,px_color Color)
{
	PX_Object_MemoryView *pdesc=PX_Object_GetMemoryView(pObject);
	if(pdesc)
		pdesc->backgroundcolor=Color;
}

px_void PX_Object_MemoryViewSetData(PX_Object *pObject,px_void *pdata,px_dword size)
{
	PX_Object_MemoryView *pdesc=PX_Object_GetMemoryView(pObject);
	if(pdesc)
	{
		px_int row,column,cc;
		pdesc->pdata=pdata;
		pdesc->size=size;
		pdesc->cursor=-1;
		pdesc->mark_start_cursor=-1;
		pdesc->mark_end_cursor=-1;

		//////////////////////////////////////////////////////////////////////////
		//scrollbar
		row=(px_int)(pObject->Width-(16+64+16+8+16))/32;
		column=(px_int)pObject->Height/(15+3);
		cc=(pdesc->size+row/2)/row;
		if (cc>column)
		{
			PX_Object_SliderBarSetRange(pdesc->scrollbar,0,cc-column);
			PX_Object_SliderBarSetValue(pdesc->scrollbar,0);
			pdesc->scrollbar->Visible=PX_TRUE;
		}
		else
		{
			pdesc->scrollbar->Visible=PX_FALSE;
			PX_Object_SliderBarSetValue(pdesc->scrollbar,0);
		}
			
	}
}

px_bool		PX_Object_MemoryViewReadData(PX_Object* pObject,px_dword offset, px_void* poutdata, px_dword size)
{
	PX_Object_MemoryView* pdesc = PX_Object_GetMemoryView(pObject);
	if (offset + size <= (px_dword)pdesc->size)
	{
		PX_memcpy(poutdata, (px_byte *)pdesc->pdata + offset, size);
	}
	return PX_FALSE;
}