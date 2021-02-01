#include "PX_Compiler.h"

px_bool PX_CompilerInitialize(px_memorypool *mp,PX_Compiler *compiler)
{
	compiler->mp=mp;
	if(!PX_ScriptCompilerInitialize(&compiler->lib,mp))
	{
		return PX_FALSE;
	}
	return PX_TRUE;
}

px_bool PX_CompilerAddSource(PX_Compiler *compiler,const px_char script[])
{
	if(!PX_ScriptCompilerLoad(&compiler->lib,script))
	{
		return PX_FALSE;
	}
	return PX_TRUE;

}

px_bool PX_CompilerCompile(PX_Compiler *compiler,px_memory *bin,const px_char entryScript[])
{
	
	px_string asmcodeString;
	
	PX_StringInitialize(compiler->mp,&asmcodeString);
	if(PX_ScriptCompilerCompile(&compiler->lib,entryScript,&asmcodeString,128))
	{
		PX_ScriptAsmOptimization(&asmcodeString);

		if(!PX_ScriptAsmCompile(compiler->mp,asmcodeString.buffer,bin))
		{
			return PX_FALSE;
		}
		PX_StringFree(&asmcodeString);
		return PX_TRUE;
	}

	PX_StringFree(&asmcodeString);

	return PX_FALSE;
}

px_void PX_CompilerFree(PX_Compiler *compiler)
{
	PX_ScriptCompilerFree(&compiler->lib);
}

