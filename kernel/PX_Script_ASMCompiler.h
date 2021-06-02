#ifndef PX_SCRIPT_ASM_COMPILER_H
#define PX_SCRIPT_ASM_COMPILER_H
#include "PX_ScriptCore.h"
px_bool PX_ScriptAsmCompile(px_memorypool *mp,px_char *asmcode,px_memory *binmemory);
px_bool PX_ScriptAsmOptimization(px_string *asmcode);
px_char * PX_ScriptASMCompilerError(void);
#endif
