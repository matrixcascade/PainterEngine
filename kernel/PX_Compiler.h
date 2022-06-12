#ifndef PX_COMPILER_H
#define PX_COMPILER_H

#include "PX_Script_Interpreter.h"
#include "PX_Script_ASMCompiler.h"
#include "PX_Script_VM.h"

#define PX_COMPILER_DEFAULT_STACK_SIZE 256
typedef struct
{
	px_memorypool *mp;
	PX_SCRIPT_LIBRARY lib;
	px_char error[1024];
}PX_Compiler;

px_bool PX_CompilerInitialize(px_memorypool *mp,PX_Compiler *compiler);
px_bool PX_CompilerAddSource(PX_Compiler *compiler,const px_char script[]);
px_bool PX_CompilerCompile(PX_Compiler *compiler,px_memory *bin,PX_VM_DebuggerMap *pdebugmap ,const px_char entryScript[]);
px_void PX_CompilerFree(PX_Compiler *compiler);
px_char* PX_CompilerError(PX_Compiler* compiler);
#endif
