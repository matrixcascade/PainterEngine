#include "PX_Object_Typer.h"


px_void PX_Object_TyperRender(px_surface* psurface, PX_Object* pObject, px_uint elapsed)
{
	px_int x_draw_oft, y_draw_oft, fsize;
	PX_Object_Typer* pDesc = (PX_Object_Typer*)pObject->pObjectDesc;
	px_float objx, objy, objHeight, objWidth;
	px_float inheritX, inheritY;
	px_float h = 0;
	px_int ip=0;
	px_dword time;

	PX_ObjectGetInheritXY(pObject, &inheritX, &inheritY);

	objx = (pObject->x + inheritX);
	objy = (pObject->y + inheritY);
	objWidth = pObject->Width;
	objHeight = pObject->Height;

	x_draw_oft = (px_int)objx;
	y_draw_oft = (px_int)objy;

	if (pDesc->reg_elapsed < 100000000)
	{
		pDesc->reg_elapsed += elapsed * pDesc->reg_speed;
	}

	time = (px_dword)pDesc->reg_elapsed;
	pDesc->reg_delay = 100;

	while (time>pDesc->reg_delay)
	{
		px_byte opcode;
		const px_char * Text;

		if (ip>=pDesc->payload.usedsize)
		{
			break;
		}
		
		opcode = pDesc->payload.buffer[ip];

		if (opcode==0x12)
		{
			ip++;
			if (ip >= pDesc->payload.usedsize)
			{
				break;
			}
			opcode = pDesc->payload.buffer[ip];
			if (opcode == PX_OBJECT_TYPER_OPCODE_SETCOLOR)
			{
				px_byte r, g, b, a;
				ip++;
				if (pDesc->payload.usedsize - ip < 4)
				{
					break;
				}
				a = pDesc->payload.buffer[ip++];
				r = pDesc->payload.buffer[ip++];
				g = pDesc->payload.buffer[ip++];
				b = pDesc->payload.buffer[ip++];
				pDesc->reg_color = PX_COLOR(a, r, g, b);
				continue;
			}
			else if (opcode == PX_OBJECT_TYPER_OPCODE_SETDELAY)
			{
				//4 byte littleendian
				ip++;
				if (pDesc->payload.usedsize - ip < 4)
				{
					break;
				}
				pDesc->reg_delay = *(px_dword*)(pDesc->payload.buffer + ip);
				ip += 4;
				continue;
			}
			else if (opcode == PX_OBJECT_TYPER_OPCODE_SETXSPACER)
			{
				//4 byte littleendian
				ip++;
				if (pDesc->payload.usedsize - ip < 4)
				{
					break;
				}
				pDesc->x_spacer = *(px_int32*)(pDesc->payload.buffer + ip);
				ip += 4;
				continue;
			}
			else if (opcode == PX_OBJECT_TYPER_OPCODE_SETYSPACER)
			{
				//4 byte littleendian
				ip++;
				if (pDesc->payload.usedsize - ip < 4)
				{
					break;
				}
				pDesc->y_spacer = *(px_int32*)(pDesc->payload.buffer + ip);
				ip += 4;
				continue;
			}
			else
			{
				break;
			}
		}
		
		Text = (const px_char *)(pDesc->payload.buffer + ip);
		time -= pDesc->reg_delay;
		fsize = 0;

		if (pDesc->fontModule)
		{
			px_dword code;
			px_int width, height;
			fsize = PX_FontModuleGetOneCharacterDesc(pDesc->fontModule, Text, &code, &width, &height);
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
				if (pDesc->y_spacer == 0)
				{
					y_draw_oft += pDesc->fontModule->max_Height + 2;
					h += pDesc->fontModule->max_Height + 2;
				}
				else
				{
					y_draw_oft += pDesc->y_spacer;
					h += pDesc->y_spacer;
				}
			}
			else
			{
				if (psurface)
					PX_FontModuleDrawCharacter(psurface, pDesc->fontModule, x_draw_oft, y_draw_oft, code, pDesc->reg_color);
				if(pDesc->x_spacer==0)
					x_draw_oft += width;
				else
					x_draw_oft += pDesc->x_spacer;
			}

			if (x_draw_oft > objx + objWidth - pDesc->fontModule->max_Width * 2)
			{
				x_draw_oft = (px_int)objx;

				if (pDesc->y_spacer == 0)
				{
					y_draw_oft += pDesc->fontModule->max_Height + 2;
					h += pDesc->fontModule->max_Height + 2;
				}
				else
				{
					y_draw_oft += pDesc->y_spacer;
					h += pDesc->y_spacer;
				}	
			}

		}
		else
		{
			fsize = 1;

			if (*Text== '\r')
			{
				//skip
			}
			else if (*Text == '\n')
			{
				x_draw_oft = (px_int)objx;
				if (pDesc->y_spacer == 0)
				{
					y_draw_oft += __PX_FONT_HEIGHT + 2;
					h += __PX_FONT_HEIGHT + 2;
				}
				else
				{
					y_draw_oft += pDesc->y_spacer;
					h += pDesc->y_spacer;
				}
			}
			else if (*Text)
			{
				if (psurface)
					PX_FontDrawChar(psurface, x_draw_oft, y_draw_oft, *Text, pDesc->reg_color);
				if(pDesc->x_spacer==0)
					x_draw_oft += __PX_FONT_ASCSIZE;
				else
					x_draw_oft += pDesc->x_spacer;
			}
			else
			{
				break;
			}

			if (x_draw_oft > objx + objWidth - PX_FontGetAscCharactorWidth() * 2)
			{
				x_draw_oft = (px_int)objx;
				if (pDesc->y_spacer == 0)
				{
					y_draw_oft += __PX_FONT_HEIGHT + 2;
					h += __PX_FONT_HEIGHT + 2;
				}
				else
				{
					y_draw_oft += pDesc->y_spacer;
					h += pDesc->y_spacer;
				}
			}
		}
		ip += fsize;
	}

	if (pDesc->fontModule)
	{
		if (pDesc->y_spacer == 0)
		{
			h += pDesc->fontModule->max_Height + 2;
		}
		else
		{
			h += pDesc->y_spacer;
		}
	}
	else
	{
		if (pDesc->y_spacer == 0)
		{
			h += __PX_FONT_HEIGHT + 2;
		}
		else
		{
			h += pDesc->y_spacer;
		}
	}
	pObject->Height = h;
}

px_void PX_Object_TyperFree(PX_Object* pObject)
{
	PX_Object_Typer * pDesc = PX_Object_GetTyper(pObject);
	if (!pDesc)
	{
		PX_ASSERT();
		return;
	}
	PX_MemoryFree(&pDesc->payload);
}


PX_Object_Typer* PX_Object_GetTyper(PX_Object* pObject)
{
	if (pObject->Type == PX_OBJECT_TYPE_TYPER)
		return (PX_Object_Typer*)pObject->pObjectDesc;
	else
		return PX_NULL;
}

PX_Object* PX_Object_TyperCreate(px_memorypool* mp, PX_Object* Parent, px_int x, px_int y, px_int limit_width, PX_FontModule* fm)
{
	PX_Object_Typer *pDesc;
	PX_Object *pObject;
	if (limit_width < 0)
	{
		PX_ASSERT();
		return PX_NULL;
	}
	pObject = PX_ObjectCreateEx(mp, Parent,  x*1.0f, y*1.0f,0, limit_width*1.0f, 0, 0, PX_OBJECT_TYPE_TYPER,PX_NULL, PX_Object_TyperRender, PX_Object_TyperFree,0, sizeof(PX_Object_Typer));
	if (pObject == PX_NULL)
	{
		return PX_NULL;
	}
	pDesc = (PX_Object_Typer *)pObject->pObjectDesc;
	pDesc->fontModule = fm;
	pDesc->reg_color = PX_COLOR_FONTCOLOR;
	pDesc->reg_speed = 1.0;
	pDesc->reg_delay = 100;
	PX_MemoryInitialize(mp, &pDesc->payload);
    return pObject;
}

px_bool PX_Object_TyperSetColor(PX_Object* pObject, px_byte a, px_byte r, px_byte g, px_byte b)
{
	PX_Object_Typer* pDesc = PX_Object_GetTyper(pObject);
	px_byte opcode;
	opcode = 0x12;
	if (!PX_MemoryCat(&pDesc->payload, &opcode, sizeof(px_byte)))return PX_FALSE;
	opcode = PX_OBJECT_TYPER_OPCODE_SETCOLOR;
	if (!PX_MemoryCat(&pDesc->payload, &opcode, sizeof(px_byte)))return PX_FALSE;
	if (!PX_MemoryCat(&pDesc->payload, &a, sizeof(px_byte)))return PX_FALSE;
	if (!PX_MemoryCat(&pDesc->payload, &r, sizeof(px_byte)))return PX_FALSE;
	if (!PX_MemoryCat(&pDesc->payload, &g, sizeof(px_byte)))return PX_FALSE;
	if (!PX_MemoryCat(&pDesc->payload, &b, sizeof(px_byte)))return PX_FALSE;
	return PX_TRUE;
}

px_bool PX_Object_TyperSetDelay(PX_Object* pObject, px_dword delay)
{
PX_Object_Typer* pDesc = PX_Object_GetTyper(pObject);
	px_byte opcode;
	opcode = 0x12;
	if (!PX_MemoryCat(&pDesc->payload, &opcode, sizeof(px_byte)))return PX_FALSE;
	opcode = PX_OBJECT_TYPER_OPCODE_SETDELAY;
	if (!PX_MemoryCat(&pDesc->payload, &opcode, sizeof(px_byte)))return PX_FALSE;
	if (!PX_MemoryCat(&pDesc->payload, &delay, sizeof(px_dword)))return PX_FALSE;
	return PX_TRUE;
}

px_bool PX_Object_TyperSetXSpace(PX_Object* pObject, px_int32 x)
{
	PX_Object_Typer* pDesc = PX_Object_GetTyper(pObject);
	px_byte opcode;
	opcode = 0x12;
	if (!PX_MemoryCat(&pDesc->payload, &opcode, sizeof(px_byte)))return PX_FALSE;
	opcode = PX_OBJECT_TYPER_OPCODE_SETXSPACER;
	if (!PX_MemoryCat(&pDesc->payload, &opcode, sizeof(px_byte)))return PX_FALSE;
	if (!PX_MemoryCat(&pDesc->payload, &x, sizeof(px_int32)))return PX_FALSE;
	return PX_TRUE;
}

px_bool PX_Object_TyperSetYSpace(PX_Object* pObject, px_int32 y)
{
	PX_Object_Typer* pDesc = PX_Object_GetTyper(pObject);
	px_byte opcode;
	opcode = 0x12;
	if (!PX_MemoryCat(&pDesc->payload, &opcode, sizeof(px_byte)))return PX_FALSE;
	opcode = PX_OBJECT_TYPER_OPCODE_SETYSPACER;
	if (!PX_MemoryCat(&pDesc->payload, &opcode, sizeof(px_byte)))return PX_FALSE;
	if (!PX_MemoryCat(&pDesc->payload, &y, sizeof(px_int32)))return PX_FALSE;
	return PX_TRUE;
}


px_bool PX_Object_TyperPrintText(PX_Object* pObject, const px_char* Text)
{
	PX_Object_Typer* pDesc = PX_Object_GetTyper(pObject);
	if (!PX_MemoryCat(&pDesc->payload, Text, PX_strlen(Text)))return PX_FALSE;
	return PX_TRUE;
}



px_void PX_Object_TyperPlay(PX_Object* pObject)
{
	PX_Object_Typer* pDesc = PX_Object_GetTyper(pObject);
	if (!pDesc) {
		PX_ASSERT();
		return;
	}
	pDesc->state=PX_OBJECT_TYPER_STATE_PLAY;
}

px_void PX_Object_TyperPause(PX_Object* pObject)
{
	PX_Object_Typer* pDesc = PX_Object_GetTyper(pObject);
	if (!pDesc) {
		PX_ASSERT();
		return;
	}
	pDesc->state = PX_OBJECT_TYPER_STATE_PAUSE;
}

px_void PX_Object_TyperReset(PX_Object* pObject)
{
	PX_Object_Typer* pDesc = PX_Object_GetTyper(pObject);
	if (!pDesc) {
		PX_ASSERT();
		return;
	}
	pDesc->reg_elapsed = 0;
}

px_void PX_Object_TyperSetSpeed(PX_Object* pObject, px_double reg_speed)
{
	PX_Object_Typer* pDesc = PX_Object_GetTyper(pObject);
	if (!pDesc) {
		PX_ASSERT();
		return;
	}
	pDesc->reg_speed = reg_speed;
}


px_void PX_Object_TyperClear(PX_Object* pObject)
{
	PX_Object_Typer* pDesc = PX_Object_GetTyper(pObject);
	if (!pDesc) {
		PX_ASSERT();
		return;
	}
	PX_MemoryClear(&pDesc->payload);
	pDesc->reg_elapsed = 0;
}

px_bool PX_Object_TyperPrintPayload(PX_Object* pObject, const px_char* payload)
{
	px_int strlen_size=PX_strlen(payload);
	px_int cursor=0;
	PX_Object_Typer* pDesc = PX_Object_GetTyper(pObject);

	if (strlen_size<=0)
	{
		return PX_FALSE;
	}

	while (cursor<strlen_size)
	{
		if (payload[cursor]=='(')
		{
			if (payload[cursor + 1] == '!')
			{
				px_char content[32] = { 0 };
				px_int offset=cursor+2;
				px_int untillen;
				untillen = PX_StringReadUntil(payload + offset, ' ', content, sizeof(content));
				if (untillen)
				{
					offset += untillen;
					PX_trim(content);
					if (PX_strequ(content, "color"))
					{
						untillen = PX_StringReadUntil(payload + offset, ')', content, sizeof(content));
						if (untillen)
						{
							offset += untillen;
							PX_trim(content);
							if (PX_strlen(content) == 8)
							{
								px_dword color;
								PX_HexStringToBuffer(content, (px_byte *)&color);
								if (!PX_Object_TyperSetColor(pObject, (px_byte)(color), (px_byte)(color >> 8), (px_byte)(color >> 16), (px_byte)(color>>24)))
									return PX_FALSE;
								cursor = offset;
								continue;
							}
						}
					}
					else if (PX_strequ(content, "delay"))
					{
						untillen = PX_StringReadUntil(payload  + offset, ')', content, sizeof(content));
						if (untillen)
						{
							offset += untillen;
							PX_trim(content);
							if (PX_strIsNumeric(content))
							{
								px_int value = PX_atoi(content);
								if (!PX_Object_TyperSetDelay(pObject, (px_dword)value))
									return PX_FALSE;
								cursor = offset;
								continue;
							}
						}
					}
					else if (PX_strequ(content, "xspace")|| PX_strequ(content, "spacex"))
					{
						untillen = PX_StringReadUntil(payload  + offset, ')', content, sizeof(content));
						if (untillen)
						{
							offset += untillen;
							PX_trim(content);
							if (PX_strIsNumeric(content))
							{
								px_int value = PX_atoi(content);
								if (!PX_Object_TyperSetXSpace(pObject, value))
									return PX_FALSE;
								cursor = offset;
								continue;
							}
						}
					}
					else if (PX_strequ(content, "yspace")|| PX_strequ(content, "spacey"))
					{
						untillen = PX_StringReadUntil(payload + cursor + offset, ')', content, sizeof(content));
						if (untillen)
						{
							offset += untillen;
							PX_trim(content);
							if (PX_strIsNumeric(content))
							{
								px_int value = PX_atoi(content);
								if (!PX_Object_TyperSetYSpace(pObject, value))
									return PX_FALSE;
								cursor = offset;
								continue;
							}
						}
					}
				}
				
				
			}
		}

		PX_MemoryCat(&pDesc->payload, &payload[cursor], 1);
		cursor++;
	}

	return PX_TRUE;
}
