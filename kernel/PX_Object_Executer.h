#ifndef PX_EXECUTER_H
#define PX_EXECUTER_H

#include "PX_Compiler.h"
#include "PX_Object.h"

typedef struct
{
	PX_Object* printer;
	PX_VM_DebuggerMap debugmap;
	PX_VM vm;
	PX_FontModule* fm;
}PX_Object_Executer;

PX_Object* PX_Object_ExecuterCreatePayload(px_memorypool* mp, PX_Object* Parent, px_int x, px_int y, px_int width, px_int height, PX_FontModule* fm, const px_byte bin[], px_int binsize);
PX_Object*	PX_Object_ExecuterCreate(px_memorypool* mp, PX_Object* Parent, px_int x, px_int y, px_int width, px_int height, PX_FontModule* fm, const px_char shellcode[]);
PX_VM_DebuggerMap* PX_Object_ExecuterGetDebugMap(PX_Object* pObject);
PX_VM* PX_Object_ExecuterGetVM(PX_Object* pObject);
#endif

