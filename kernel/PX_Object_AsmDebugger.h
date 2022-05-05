#ifndef PX_OBJECT_ASMDEBUGGER_H
#define PX_OBJECT_ASMDEBUGGER_H
#include "PX_Object.h"

#define PX_OBJECT_ASMDEBUGGER_VARMONITOR_MAX_COUNT 64

typedef struct 
{
	px_bool brun;
	px_bool bbreak;
	px_string source;
}PX_Object_AsmDebugger_Line;

typedef struct
{
	px_char name[64];
	px_int line;
	PX_VM_VARIABLE* pvar;
}PX_Object_AsmDebugger_VarMonitor;

typedef struct  
{
	PX_Object* list_source;
	PX_Object* list_monitor;
	PX_Object* label_name,*autotext_data;
	PX_Object* label_register;
	PX_Object* button_break, * button_step, * button_pause, * button_run,* button_reset;
	
	PX_FontModule* fm;
	PX_VM_DebuggerMap* map;
	PX_VM* vm;

	px_vector lines;
	px_vector monitor;
	px_int lastip;
}PX_Object_AsmDebugger;


px_void PX_Object_AsmDebuggerDetach(PX_Object* pObject);
px_bool PX_Object_AsmDebuggerAttach(PX_Object* pObject, PX_VM_DebuggerMap* map, PX_VM* vm);
PX_Object* PX_Object_AsmDebuggerCreate(px_memorypool* mp, PX_Object* Parent, px_int x, px_int y, px_int width, px_int height, PX_FontModule* fm);
#endif