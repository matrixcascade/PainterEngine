#include "PX_Object_Code.h"


PX_OBJECT_EVENT_FUNCTION(PX_Object_Code_HSliderBarValueChanged)
{
	PX_Object* pCodeObject = (PX_Object*)ptr;
	PX_Object_Code* pDesc = PX_ObjectGetDesc0(PX_Object_Code, pCodeObject);
	px_int value = PX_Object_SliderBarGetValue(pObject);
	pDesc->x_offset = value;
	pDesc->content_rerender_flag = PX_TRUE;
}

PX_OBJECT_EVENT_FUNCTION(PX_Object_Code_VSliderBarValueChanged)
{
	PX_Object* pCodeObject = (PX_Object*)ptr;
	PX_Object_Code* pDesc = PX_ObjectGetDesc0(PX_Object_Code, pCodeObject);
	px_int value = PX_Object_SliderBarGetValue(pObject);
	pDesc->row_offset = value;
	pDesc->content_rerender_flag = PX_TRUE;
}

PX_OBJECT_EVENT_FUNCTION(PX_Object_Code_OnCursorWheel)
{
	PX_Object_Code* pDesc = PX_ObjectGetDesc0(PX_Object_Code, pObject);
	px_float x, y, z;
	px_rect region = PX_ObjectGetRect(pObject);
	x = PX_Object_Event_GetCursorX(e);
	y = PX_Object_Event_GetCursorY(e);
	z = PX_Object_Event_GetCursorZ(e);
	pDesc->cursorx = (px_int)x;
	pDesc->cursory = (px_int)y;
	if (pDesc->content_panel_surface.width && pDesc->content_panel_surface.height)
	{
		if (x > region.x && x < region.x + pDesc->content_panel_surface.width &&
			y > region.y && y < region.y + pDesc->content_panel_surface.height)
		{
			PX_Object_SliderBarChangeValue(pDesc->content_vslider_bar, z > 0 ? -1 : 1);
		}
	}
}

PX_OBJECT_EVENT_FUNCTION(PX_Object_Code_OnCursorMove)
{
	PX_Object_Code* pDesc = PX_ObjectGetDesc0(PX_Object_Code, pObject);
	px_float x, y;
	px_rect region = PX_ObjectGetRect(pObject);
	x = PX_Object_Event_GetCursorX(e);
	y = PX_Object_Event_GetCursorY(e);
	pDesc->cursorx = (px_int)x;
	pDesc->cursory = (px_int)y;
	if (pDesc->content_panel_surface.width && pDesc->content_panel_surface.height)
	{
		if (x > region.x && y > region.y)
		{
			pDesc->content_rerender_flag = PX_TRUE;
		}
	}
}

PX_OBJECT_EVENT_FUNCTION(PX_Object_Code_OnCursorDown)
{
	PX_Object_Code* pDesc = PX_ObjectGetDesc0(PX_Object_Code, pObject);
	px_float x, y;
	px_rect region = PX_ObjectGetRect(pObject);
	x = PX_Object_Event_GetCursorX(e);
	y = PX_Object_Event_GetCursorY(e);
	pDesc->cursorx = (px_int)x;
	pDesc->cursory = (px_int)y;
	pDesc->content_rerender_flag = PX_TRUE;

	if (pDesc->content_panel_surface.width && pDesc->content_panel_surface.height)
	{
		if (pDesc->pSource)
		{
			px_int ref_x = pDesc->cursorx - (px_int)region.x;
			px_int ref_y = pDesc->cursory - (px_int)region.y;
			px_int line_pitch = PX_OBJECT_CODE_LINE_PITCH;
			if (ref_x >= 0 && ref_x <= PX_OBJECT_CODE_PREPANEL_WIDTH && ref_y >= 0 && ref_y < pDesc->content_panel_surface.height)
			{
				px_int yindex = ref_y / line_pitch;
				px_int abs_line = pDesc->row_offset + yindex;
				if (abs_line < pDesc->pSource->line_count)
				{
					PX_SyntaxLexer_Source* pSource = pDesc->pSource;
					if (pSource && abs_line >= 0 && abs_line < pSource->line_count)
					{
						PX_SyntaxLexer_LineMap* prow = &pSource->line_begin_cell_index_map[abs_line];
						prow->bdebugbreak = !prow->bdebugbreak;
						PX_ObjectExecuteEvent(pObject, PX_OBJECT_BUILD_EVENT_INT(PX_OBJECT_CODE_EVENT_TRIGGER_BREAKPOINT, abs_line));
					}
					
				}
			}
		}
	}
}


static px_void PX_Object_Code_RenderContent(PX_Object* pObject)
{
	PX_Object_Code* pDesc = PX_ObjectGetDesc0(PX_Object_Code, pObject);
	PX_SyntaxLexer_Source* pSource = pDesc->pSource;
	px_rect region = PX_ObjectGetRect(pObject);
	px_int render_x, render_y;
	px_int cursor_abs_x, cursor_abs_y;
	px_int current_render_line_index;

	if (!pSource)
		return;

	pDesc->cursor_abi_index = -1;
	cursor_abs_x = pDesc->cursorx - (px_int)region.x;
	cursor_abs_y = pDesc->cursory - (px_int)region.y;

	if (pDesc->content_panel_surface.mp == PX_NULL)
		return;

	render_y = 0;
	current_render_line_index = pDesc->row_offset;
	while (PX_TRUE)
	{
		px_int cell_index;
		PX_SyntaxLexer_Cell* pcell = PX_NULL;
		PX_SyntaxLexer_LineMap* prow = PX_NULL;

		if (current_render_line_index >= pSource->line_count)
			return;
		if (render_y > pDesc->content_panel_surface.height)
			return;
		prow = &pSource->line_begin_cell_index_map[current_render_line_index];
		cell_index = prow->begin_cell_index;
		if (cell_index < 0)
			return;

		render_x = -pDesc->x_offset + PX_OBJECT_CODE_PREPANEL_WIDTH;
		while (cell_index < pSource->cells_count)
		{
			px_int render_width;
			pcell = pSource->cells + cell_index;

			if (render_x > pDesc->content_panel_surface.width)
				break;

			if (pcell->unicode == '\0')
			{
				PX_ASSERTX("Error: unicode should not be null here");
				return;
			}
			else if (pcell->unicode == '\n')
				break;
			else if (pcell->unicode == '\t')
			{
				px_int abs_x = render_x + pDesc->x_offset - PX_OBJECT_CODE_PREPANEL_WIDTH;
				render_width = 32 - (abs_x & 31);
			}
			else if (pcell->unicode >= 255)
				render_width = PX_OBJECT_CODE_RENDER_CELL_CHAR_WIDTH;
			else
				render_width = PX_OBJECT_CODE_RENDER_CELL_ASC_WIDTH;

			if (render_x + render_width > PX_OBJECT_CODE_PREPANEL_WIDTH)
			{
				px_color clr = PX_COLOR(255, 255, 255, 255);
				if (pcell->abi_index != -1 && PX_VectorCheckIndex(&pSource->descriptor, pcell->abi_index))
				{
					px_abi* pabi = PX_VECTORAT(px_abi, &pSource->descriptor, pcell->abi_index);
					px_color* pcolor = PX_AbiGet_color(pabi, "color");
					if (pcolor) clr = *pcolor;

					if (PX_isIntPointXYInRect(cursor_abs_x, cursor_abs_y, render_x, render_y, render_width, PX_OBJECT_CODE_RENDER_CELL_HEIGHT))
						pDesc->cursor_abi_index = pcell->abi_index;
				}

				if (pDesc->fm)
					PX_FontModuleDrawCharacter(&pDesc->content_panel_surface, pDesc->fm, render_x, render_y, pcell->unicode, clr);
				else
					PX_FontModuleDrawCharacter(&pDesc->content_panel_surface, PX_NULL, render_x, render_y, pcell->unicode, clr);
			}
			render_x += render_width;
			if (cell_index + 1 >= pSource->cells_count)
				return;
			else if (pSource->cells[cell_index + 1].row_index != pSource->cells[cell_index].row_index)
				break;
			cell_index++;
		}

		render_y += PX_OBJECT_CODE_LINE_PITCH;
		current_render_line_index++;
	}
}

static px_void PX_Object_Code_RenderPrePanel(PX_Object* pObject)
{
	px_int reader_y_idx;
	PX_Object_Code* pDesc = PX_ObjectGetDesc0(PX_Object_Code, pObject);
	PX_SyntaxLexer_Source* pSource = pDesc->pSource;
	px_rect region = PX_ObjectGetRect(pObject);
	px_int preview_breakpoint_line_index = -1;
	px_int line_pitch = PX_OBJECT_CODE_LINE_PITCH;

	if (!pSource)
		return;
	if (pDesc->content_panel_surface.mp == PX_NULL)
		return;

	PX_SurfaceClear(&pDesc->content_panel_surface, 0, 0, PX_OBJECT_CODE_PREPANEL_WIDTH - 1, pDesc->content_panel_surface.height - 1, PX_COLOR(255, 30, 30, 30));
	reader_y_idx = pDesc->row_offset;

	do {
		px_int rel_x = pDesc->cursorx - (px_int)region.x;
		px_int rel_y = pDesc->cursory - (px_int)region.y;
		if (rel_x < 0 || rel_x > PX_OBJECT_CODE_PREPANEL_WIDTH) break;
		if (rel_y < 0 || rel_y > pDesc->content_panel_surface.height) break;
		preview_breakpoint_line_index = rel_y / line_pitch;
	} while (0);

	while (PX_TRUE)
	{
		px_int render_y_index = reader_y_idx - pDesc->row_offset;
		px_int abs_line = reader_y_idx;
		if (reader_y_idx >= pSource->line_count) break;
		if (render_y_index * line_pitch > pDesc->content_panel_surface.height) break;

		if (pSource->line_begin_cell_index_map[abs_line].bdebugbreak)
			PX_GeoDrawSolidCircle(&pDesc->content_panel_surface, 8, render_y_index * line_pitch + 8, 6, PX_COLOR_RED);
		else if (render_y_index == preview_breakpoint_line_index)
			PX_GeoDrawSolidCircle(&pDesc->content_panel_surface, 8, render_y_index * line_pitch + 8, 6, PX_COLOR(128, 255, 0, 0));

		if (pDesc->current_cursor_line == abs_line)
		{
			PX_GeoDrawRightTriangle(&pDesc->content_panel_surface, \
				4.f, render_y_index * line_pitch + 4.f, \
				12.f, render_y_index * line_pitch + 12.f, \
				PX_COLOR(255, 0, 255, 0));
		}

		PX_FontModuleDrawText(&pDesc->content_panel_surface, PX_NULL, 56, render_y_index * line_pitch, PX_ALIGN_RIGHTTOP, PX_itos(reader_y_idx, 10).data, PX_COLOR(255, 160, 200, 220));
		reader_y_idx++;
	}
}

static px_void PX_Object_Code_ReRenderContent(PX_Object* pObject)
{
	PX_Object_Code* pDesc = PX_ObjectGetDesc0(PX_Object_Code, pObject);
	PX_SyntaxLexer_Source* pSource = pDesc->pSource;
	px_int line_pitch = PX_OBJECT_CODE_LINE_PITCH;
	px_int line_width, button_width;

	if (!pSource)
		return;
	if (pDesc->content_panel_surface.mp == PX_NULL)
		return;

	PX_SurfaceClear(&pDesc->content_panel_surface, PX_OBJECT_CODE_PREPANEL_WIDTH, 0, pDesc->content_panel_surface.width - 1, pDesc->content_panel_surface.height - 1, PX_COLOR(255, 30, 30, 30));
	PX_Object_Code_RenderContent(pObject);
	PX_Object_Code_RenderPrePanel(pObject);

	PX_ObjectSetEnabled(pDesc->content_hslider_bar, PX_TRUE);
	line_width = pSource->max_line_char_width * PX_OBJECT_CODE_RENDER_CELL_CHAR_WIDTH;
	if (line_width < pDesc->content_panel_surface.width)
		line_width = pDesc->content_panel_surface.width;
	button_width = (px_int)(pDesc->content_hslider_bar->Width * pDesc->content_panel_surface.width / line_width / 2);
	button_width = PX_MAX(button_width, 20);
	PX_Object_SliderBarSetRange(pDesc->content_hslider_bar, 0, 2 * line_width);
	PX_Object_SliderBarSetSliderButtonLength(pDesc->content_hslider_bar, button_width);

	PX_Object_SliderBarSetRange(pDesc->content_vslider_bar, 0, pSource->line_count + pDesc->content_panel_surface.height / line_pitch / 2);
	PX_Object_SliderBarSetSliderButtonLength(pDesc->content_vslider_bar, (px_int)(pDesc->content_vslider_bar->Height * (pDesc->content_panel_surface.height / line_pitch) / (pSource->line_count + (pDesc->content_panel_surface.height / line_pitch))));
	PX_Object_SliderBarSetValue(pDesc->content_vslider_bar, pDesc->row_offset);
}


PX_OBJECT_RENDER_FUNCTION(PX_Object_Code_Render)
{
	px_rect region = PX_ObjectGetRect(pObject);
	PX_Object_Code* pDesc = PX_ObjectGetDesc(PX_Object_Code, pObject);
	px_int content_panel_width = (px_int)region.width - 20;
	px_int content_panel_height = (px_int)region.height - 20;

	if (content_panel_width < 100)
		content_panel_width = 100;
	if (content_panel_height < 100)
		content_panel_height = 100;

	if (pDesc->content_panel_surface.width != content_panel_width || pDesc->content_panel_surface.height != content_panel_height)
	{
		if (pDesc->content_panel_surface.mp)
			PX_TextureFree(&pDesc->content_panel_surface);
		if (!PX_TextureCreate(pDesc->mp, &pDesc->content_panel_surface, content_panel_width, content_panel_height))
		{
			return;
		}
		PX_SurfaceClear(&pDesc->content_panel_surface, 0, 0, pDesc->content_panel_surface.width - 1, pDesc->content_panel_surface.height - 1, PX_COLOR(255, 30, 30, 30));
		pDesc->current_panel_max_row = content_panel_height / PX_OBJECT_CODE_LINE_PITCH;
		pDesc->content_rerender_flag = PX_TRUE;
	}

	if (pDesc->content_rerender_flag)
	{
		if (pDesc->content_render_delay > elapsed)
		{
			pDesc->content_render_delay -= elapsed;
		}
		else
		{
			PX_SurfaceClear(&pDesc->content_panel_surface, 0, 0, pDesc->content_panel_surface.width - 1, pDesc->content_panel_surface.height - 1, PX_COLOR(255, 30, 30, 30));
			PX_Object_Code_ReRenderContent(pObject);
			pDesc->content_rerender_flag = PX_FALSE;
			pDesc->content_render_delay = 0;
		}
	}

	PX_TextureRender(psurface, &pDesc->content_panel_surface, (px_int)region.x, (px_int)region.y, PX_ALIGN_LEFTTOP, PX_NULL);

	pDesc->content_hslider_bar->x = 0;
	pDesc->content_hslider_bar->y = (px_float)content_panel_height;
	pDesc->content_hslider_bar->Width = (px_float)content_panel_width + 20.f;
	pDesc->content_hslider_bar->Height = 20;

	pDesc->content_vslider_bar->x = (px_float)content_panel_width;
	pDesc->content_vslider_bar->y = 0;
	pDesc->content_vslider_bar->Width = 20;
	pDesc->content_vslider_bar->Height = (px_float)content_panel_height;

	do
	{
		PX_SyntaxLexer_Source* pSource = pDesc->pSource;
		if (!pSource)
			break;
		if (PX_VectorCheckIndex(&pSource->descriptor, pDesc->cursor_abi_index))
		{
			px_abi* pabi = PX_VECTORAT(px_abi, &pSource->descriptor, pDesc->cursor_abi_index);
			if (pabi)
			{
				px_int render_width, render_height;
				px_char desc_content[256] = { 0 };
				const px_char* ptype = PX_AbiGet_string(pabi, "type");
				const px_char* info = PX_AbiGet_string(pabi, "info");
				const px_char* pregion = PX_AbiGet_string(pabi, "region");
				const px_int* poffset = PX_AbiGet_int(pabi, "offset");
				if (pregion)
				{
					PX_strcat(desc_content, "[");
					PX_strcat(desc_content, pregion);
					PX_strcat(desc_content, "]");
				}
				if (ptype) PX_strcat(desc_content, ptype);
				if (poffset)
				{
					PX_strcat(desc_content, " offset:");
					PX_strcat(desc_content, PX_itos(*poffset, 10).data);
				}
				if (info)
				{
					PX_strcat(desc_content, "\n");
					PX_strcat(desc_content, info);
				}
				PX_FontModuleTextGetRenderWidthHeight(pDesc->fm, desc_content, &render_width, &render_height);
				if (render_width && render_height)
				{
					px_int line_pitch = PX_OBJECT_CODE_LINE_PITCH;
					px_int render_y = (px_int)region.y + ((pDesc->cursory - (px_int)region.y + line_pitch - 1) / line_pitch * line_pitch);
					px_int render_x = pDesc->cursorx;
					render_width += 10;
					render_height += 10;
					if (render_x + render_width > psurface->width)
						render_x = psurface->width - render_width;
					PX_GeoDrawRect(psurface, render_x, render_y, render_x + render_width, render_y + render_height, PX_COLOR(192, 64, 64, 64));
					PX_GeoDrawBorder(psurface, render_x, render_y, render_x + render_width, render_y + render_height, 1, PX_COLOR_WHITE);
					PX_FontModuleDrawText(psurface, pDesc->fm, render_x + 5, render_y + 5, PX_ALIGN_LEFTTOP, desc_content, PX_COLOR_WHITE);
				}
			}
		}
	} while (0);

	if (pDesc->bordercolor._argb.a)
	{
		PX_GeoDrawBorder(psurface, (px_int)region.x, (px_int)region.y, (px_int)(region.x + region.width-21), (px_int)(region.y + region.height-21), 2, pDesc->bordercolor);
	}
}

PX_OBJECT_FREE_FUNCTION(PX_Object_Code_Free)
{
	PX_Object_Code* pDesc = PX_ObjectGetDesc0(PX_Object_Code, pObject);
	if (pDesc)
	{
		if (pDesc->content_panel_surface.mp)
		{
			PX_TextureFree(&pDesc->content_panel_surface);
		}
	}
}

PX_Object* PX_Object_Code_Create(px_memorypool* mp, PX_Object* Parent, px_int x, px_int y, px_int Width, px_int Height, PX_FontModule* fm)
{
	PX_Object* pObject;
	PX_Object_Code* pDesc;
	pObject = PX_ObjectCreateEx(mp, Parent, (px_float)x, (px_float)y, 0, (px_float)Width, (px_float)Height, 0, PX_OBJECT_TYPE_CODE, PX_NULL, PX_Object_Code_Render, PX_Object_Code_Free, 0, sizeof(PX_Object_Code));
	if (!pObject)
		return PX_NULL;
	pDesc = PX_ObjectGetDesc0(PX_Object_Code, pObject);
	if (!pDesc)
		return PX_NULL;
	pDesc->mp = mp;
	pDesc->fm = fm;
	pDesc->pSource = PX_NULL;
	pDesc->row_offset = 0;
	pDesc->x_offset = 0;
	pDesc->cursor_abi_index = -1;
	pDesc->current_cursor_line = -1;
	pDesc->content_rerender_flag = PX_FALSE;
	pDesc->content_render_delay = 0;
	pDesc->current_panel_max_row = 0;

	pDesc->content_hslider_bar = PX_Object_SliderBarCreate(mp, pObject, 0, 0, Width, 20, PX_OBJECT_SLIDERBAR_TYPE_HORIZONTAL, PX_OBJECT_SLIDERBAR_STYLE_BOX);
	PX_ObjectRegisterEvent(pDesc->content_hslider_bar, PX_OBJECT_EVENT_VALUECHANGED, PX_Object_Code_HSliderBarValueChanged, pObject);

	pDesc->content_vslider_bar = PX_Object_SliderBarCreate(mp, pObject, 0, 0, 20, Height, PX_OBJECT_SLIDERBAR_TYPE_VERTICAL, PX_OBJECT_SLIDERBAR_STYLE_BOX);
	PX_ObjectRegisterEvent(pDesc->content_vslider_bar, PX_OBJECT_EVENT_VALUECHANGED, PX_Object_Code_VSliderBarValueChanged, pObject);

	PX_ObjectRegisterEvent(pObject, PX_OBJECT_EVENT_CURSORMOVE, PX_Object_Code_OnCursorMove, PX_NULL);
	PX_ObjectRegisterEvent(pObject, PX_OBJECT_EVENT_CURSORDRAG, PX_Object_Code_OnCursorMove, PX_NULL);
	PX_ObjectRegisterEvent(pObject, PX_OBJECT_EVENT_CURSORDOWN, PX_Object_Code_OnCursorDown, PX_NULL);
	PX_ObjectRegisterEvent(pObject, PX_OBJECT_EVENT_CURSORWHEEL, PX_Object_Code_OnCursorWheel, PX_NULL);

	return pObject;
}

px_void PX_Object_Code_SetSource(PX_Object* pObject, PX_SyntaxLexer_Source* pSource)
{
	PX_Object_Code* pDesc = PX_ObjectGetDesc0(PX_Object_Code, pObject);
	if (!pDesc) return;
	pDesc->pSource = pSource;
	pDesc->row_offset = 0;
	pDesc->x_offset = 0;
	pDesc->content_rerender_flag = PX_TRUE;
}

px_void PX_Object_Code_SetCursorLine(PX_Object* pObject, px_int line)
{
	PX_Object_Code* pDesc = PX_ObjectGetDesc0(PX_Object_Code, pObject);
	if (!pDesc) return;
	if (pDesc->current_cursor_line != line)
	{
		pDesc->current_cursor_line = line;
		pDesc->content_rerender_flag = PX_TRUE;
	}
}

px_void PX_Object_Code_SetCursorLineAndView(PX_Object* pObject, px_int line)
{
	PX_Object_Code* pDesc = PX_ObjectGetDesc0(PX_Object_Code, pObject);
	if (!pDesc) return;
	if (pDesc->current_cursor_line != line)
	{
		pDesc->current_cursor_line = line;
		pDesc->content_rerender_flag = PX_TRUE;
	}
	if (line >= 0)
	{
		if (line < pDesc->row_offset || line >= pDesc->row_offset + pDesc->current_panel_max_row)
		{
			pDesc->row_offset = line - pDesc->current_panel_max_row / 2;
			if (pDesc->row_offset < 0) pDesc->row_offset = 0;
			pDesc->content_rerender_flag = PX_TRUE;
		}
	}
}

px_int PX_Object_Code_GetCursorAbiIndex(PX_Object* pObject)
{
	PX_Object_Code* pDesc = PX_ObjectGetDesc0(PX_Object_Code, pObject);
	if (!pDesc) return -1;
	return pDesc->cursor_abi_index;
}

px_int PX_Object_Code_GetCurrentCursorLine(PX_Object* pObject)
{
	PX_Object_Code* pDesc = PX_ObjectGetDesc0(PX_Object_Code, pObject);
	if (!pDesc) return -1;
	return pDesc->current_cursor_line;
}

px_int PX_Object_Code_GetScrollOffsetX(PX_Object* pObject)
{
	PX_Object_Code* pDesc = PX_ObjectGetDesc0(PX_Object_Code, pObject);
	if (!pDesc) return 0;
	return pDesc->x_offset;
}

px_int PX_Object_Code_GetScrollOffsetYRow(PX_Object* pObject)
{
	PX_Object_Code* pDesc = PX_ObjectGetDesc0(PX_Object_Code, pObject);
	if (!pDesc) return 0;
	return pDesc->row_offset;
}

px_void PX_Object_Code_SetScrollOffset(PX_Object* pObject, px_int row_offset, px_int x_offset)
{
	PX_Object_Code* pDesc = PX_ObjectGetDesc0(PX_Object_Code, pObject);
	if (!pDesc) return;
	pDesc->row_offset = row_offset;
	pDesc->x_offset = x_offset;
	pDesc->content_rerender_flag = PX_TRUE;
}

px_void PX_Object_Code_SetBorderColor(PX_Object* pObject, px_color color)
{
	PX_Object_Code* pDesc = PX_ObjectGetDesc0(PX_Object_Code, pObject);
	if (!pDesc) return;
	pDesc->bordercolor = color;
	
}
