#ifndef PX_EXECUTER_H
#define PX_EXECUTER_H

#include "PX_Compiler.h"
#include "PX_Object.h"
#include "PX_Resource.h"

typedef struct
{
	PX_Object* printer;
	PX_VM_DebuggerMap debugmap;
	PX_VM vm;
	PX_FontModule* fm;
	px_int runtick;
	PX_ResourceLibrary *preslib;
}PX_Object_Executer;

PX_Object* PX_Object_ExecuterCreatePayload(px_memorypool* mp, PX_Object* Parent, px_int x, px_int y, px_int width, px_int height, PX_FontModule* fm, const px_byte bin[], px_int binsize);
PX_Object*	PX_Object_ExecuterCreate(px_memorypool* mp, PX_Object* Parent, px_int x, px_int y, px_int width, px_int height, PX_FontModule* fm, const px_char shellcode[]);
px_void PX_Object_ExecuterSetResourceLibrary(PX_Object* pObject, PX_ResourceLibrary* preslib);
px_bool PX_Object_ExecuterRunScript(PX_Object* pObject, const px_char pshellstr[]);
px_void PX_Object_ExecuterSetBackgroundColor(PX_Object* pObject, px_color color);
px_void PX_Object_ExecuterSetFontColor(PX_Object* pObject, px_color color);
px_void PX_Object_ExecuterSetRunTick(PX_Object* pObject, px_int tick);
PX_VM_DebuggerMap* PX_Object_ExecuterGetDebugMap(PX_Object* pObject);
PX_VM* PX_Object_ExecuterGetVM(PX_Object* pObject);
#endif

