#ifndef PX_OBJECT_TYPER_H
#define PX_OBJECT_TYPER_H
#include "PX_Object.h"

typedef enum
{
	PX_OBJECT_TYPER_OPCODE_SETCOLOR,
	PX_OBJECT_TYPER_OPCODE_SETDELAY,
	PX_OBJECT_TYPER_OPCODE_SETXSPACER,
	PX_OBJECT_TYPER_OPCODE_SETYSPACER,
}PX_OBJECT_TYPER_OPCODE;

typedef enum
{
	PX_OBJECT_TYPER_STATE_PAUSE,
	PX_OBJECT_TYPER_STATE_PLAY,
}PX_OBJECT_TYPER_STATE;

typedef struct 
{
	px_color reg_color;
	px_dword reg_delay;
}PX_Object_Typer_Header;

typedef struct
{
	
	PX_FontModule* fontModule;
	px_memory payload;
	px_color reg_color;
	px_int x_spacer;
	px_int y_spacer;
	px_dword reg_delay;
	px_double reg_elapsed;
	px_double reg_speed;
	PX_OBJECT_TYPER_STATE state;
}PX_Object_Typer;


PX_Object* PX_Object_TyperCreate(px_memorypool* mp, PX_Object* Parent, px_int x, px_int y, px_int limit_width, PX_FontModule* fm);
PX_Object_Typer* PX_Object_GetTyper(PX_Object* pObject);
px_void PX_Object_TyperPlay(PX_Object* pObject);
px_void PX_Object_TyperPause(PX_Object* pObject);
px_void PX_Object_TyperReset(PX_Object* pObject);
px_void PX_Object_TyperSetSpeed(PX_Object* pObject, px_double reg_speed);
px_bool PX_Object_TyperPrintPayload(PX_Object *pObject, const px_char * payload);
px_void PX_Object_TyperClear(PX_Object* pObject);
#endif

