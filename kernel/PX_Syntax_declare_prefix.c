#include "PX_Syntax_declare_prefix.h"

PX_SYNTAX_FUNCTION(PX_Syntax_load_declare_prefix_exec)
{
	px_abi* pabi = PX_Syntax_NewAbi(pSyntax, "declare_prefix", pSyntax->reg_lifetime);
	if (!pabi)
	{
		PX_Syntax_Terminate(pSyntax, "Memory Error");
		return PX_FALSE;
	}
	if (!PX_AbiSet_string(pabi, "value", PX_Syntax_GetCurrentLexeme(pSyntax)))
	{
		PX_AbiFree(pabi);
		PX_Syntax_Terminate(pSyntax, "Memory Error");
		return PX_FALSE;
	}

	return PX_TRUE;
}


px_bool PX_Syntax_load_declare_prefix(PX_Syntax* pSyntax)
{
	if (!PX_Syntax_Parse_PEBNF(pSyntax, "declare_prefix = 'static'", PX_Syntax_load_declare_prefix_exec))
		return PX_FALSE;

	if (!PX_Syntax_Parse_PEBNF(pSyntax, "declare_prefix = 'const'", PX_Syntax_load_declare_prefix_exec))
		return PX_FALSE;

	return PX_TRUE;
}
