#ifndef PX_OBJECT_SYNTAX_H
#define PX_OBJECT_SYNTAX_H
//------------------------------------------------------------------------------
//area_tab buttons ... 24
//sliderbar 8
//------------------------------------------------------------------------------
//list     1/4 |bp  lines 64|  content panel 3/4(min 75px width)				|sliderbar 20
//             |     ...    |       |        |									|
//             |     ...    |       |        |									|
//             |     ...    |bp 8   | line 32|  content (min 35px width)		|
//             |     ...    |       |        |									|
//             |     ...    |.......											|
//             |     ...    |													|
//-------------|----------------------------------------------------------------|
//tree abi 1/4 |  slider bar 20
//             |----------------------------------------------------------------
//             |  condole panel 3/4 width 1/4 height
//             |
//             |  
//             |
//			   |
//-------------------------------------------------------------------------------

#include "PX_Object.h"
#include "PX_Syntax.h"


typedef enum
{
	PX_OBJECT_SYNTAX_STATE_IDLE = 0,
	PX_OBJECT_SYNTAX_STATE_STEP,
	PX_OBJECT_SYNTAX_STATE_PAUSE,
	PX_OBJECT_SYNTAX_STATE_RUN,
	PX_OBJECT_SYNTAX_STATE_ERROR
}PX_OBJECT_SYNTAX_STATE;

typedef struct
{
	px_int row_offset;
	px_int x_offset;
}PX_Object_Syntax_pagememory;

typedef struct
{
	px_memorypool* mp;
	PX_FontModule* fm;
	PX_OBJECT_SYNTAX_STATE state;
	PX_Syntax *psyntax;
	px_int  reg_target_syntax_message_offset;
	px_int  reg_last_break_row_index;
	px_int  current_view_source_index; // current view source index
	px_vector pages_memory; //PX_Object_Syntax_pagememory
	px_vector list_contents;//px_char *
	PX_Object* area_tab;
	px_vector  tab_buttons;
	PX_Object* tree_root_abi,*list_info;
	PX_Object* controller_panel;
	PX_Object* button_run, *button_pause, * button_step, * button_stop,* button_reset;
	px_texture texture_run, texture_pause, texture_step, texture_stop, texture_reset;
	PX_Object* source_code_viewer;
	PX_Object* printer;
	PX_Object* textviewer_out;
	px_dword struct_update_delay;
	px_int last_run_circles;
	px_int last_tab_syntax_source_count;
}PX_Object_Syntax;

PX_Object* PX_Object_Syntax_Create(px_memorypool* mp, PX_Object* Parent, px_int x, px_int y, px_int Width, px_int Height, PX_FontModule* fm);
px_void PX_Object_Syntax_SetSyntax(PX_Object* pObject, PX_Syntax* psyntax);
px_void PX_Object_Syntax_SetCurrentViewSourceIndex(PX_Object* pObject, px_int source_index);
#endif

