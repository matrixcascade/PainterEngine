#ifndef PX_OBJECT_CODE_H
#define PX_OBJECT_CODE_H

#include "PX_Object.h"

#define PX_OBJECT_CODE_RENDER_CELL_ASC_WIDTH 8
#define PX_OBJECT_CODE_RENDER_CELL_CHAR_WIDTH 16
#define PX_OBJECT_CODE_RENDER_CELL_HEIGHT 16
#define PX_OBJECT_CODE_LINE_PITCH 18
#define PX_OBJECT_CODE_PREPANEL_WIDTH 80

#define PX_OBJECT_CODE_EVENT_TRIGGER_BREAKPOINT 0x3000

typedef struct
{
	px_memorypool* mp;
	PX_FontModule* fm;
	PX_SyntaxLexer_Source* pSource;
	px_int row_offset;
	px_int x_offset;
	px_texture content_panel_surface;
	px_bool content_rerender_flag;
	px_dword content_render_delay;
	PX_Object* content_hslider_bar;
	PX_Object* content_vslider_bar;
	px_int cursor_abi_index;
	px_int cursorx, cursory;
	px_int current_cursor_line;
	px_int current_panel_max_row;
	px_color bordercolor;
}PX_Object_Code;

PX_Object* PX_Object_Code_Create(px_memorypool* mp, PX_Object* Parent, px_int x, px_int y, px_int Width, px_int Height, PX_FontModule* fm);
px_void PX_Object_Code_SetSource(PX_Object* pObject, PX_SyntaxLexer_Source* pSource);
px_void PX_Object_Code_SetCursorLine(PX_Object* pObject, px_int line);
px_void PX_Object_Code_SetCursorLineAndView(PX_Object* pObject, px_int line);
px_int  PX_Object_Code_GetCursorAbiIndex(PX_Object* pObject);
px_int  PX_Object_Code_GetCurrentCursorLine(PX_Object* pObject);
px_int  PX_Object_Code_GetScrollOffsetX(PX_Object* pObject);
px_int  PX_Object_Code_GetScrollOffsetYRow(PX_Object* pObject);
px_void PX_Object_Code_SetScrollOffset(PX_Object* pObject, px_int row_offset, px_int x_offset);
px_void PX_Object_Code_SetBorderColor(PX_Object* pObject, px_color color);

#endif
