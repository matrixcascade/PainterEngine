#include "PX_Syntax_include.h"
PX_SYNTAX_FUNCTION(PX_Syntax_Parse_INCLUDE)
{
	px_abi opcode;
	px_abi* pabi = PX_Syntax_GetAbiStack(pSyntax, -1);
	PX_AbiCreateDynamicWriter(&opcode, pSyntax->mp);
	if (pabi)
	{
		const px_char* pname = PX_AbiGet_string(pabi, "name");
		const px_char* pvalue = PX_AbiGet_string(pabi, "value");
		PX_Syntax_Source* psource;
		PX_ASSERTIFX(!pname, "Error:Abi name not found");
		PX_ASSERTIFX(!PX_strequ(pname, "string"), "Error:Abi should be string");
		psource = PX_Syntax_GetSource(pSyntax, pvalue);
		if (!psource)
		{
			if (!PX_Syntax_PushOpcode(pSyntax, "terminate"))
				return PX_FALSE;
			PX_StringCatFormat1(&pSyntax->message, "Error:Source %1 not found.\n", PX_STRINGFORMAT_STRING(pvalue));
			return PX_FALSE;
		}
		if (!PX_Syntax_AddLexer(pSyntax, pname, psource->source.buffer))
		{
			if (!PX_Syntax_PushOpcode(pSyntax, "terminate"))
				return PX_FALSE;
			PX_StringCatFormat1(&pSyntax->message, "Error:Include %1 out of memory.\n", PX_STRINGFORMAT_STRING(pvalue));

			return PX_FALSE;
		}
		PX_Syntax_PopAbiStack(pSyntax);
		return PX_TRUE;
	}
	PX_ASSERT();
	return PX_FALSE;
}

PX_SYNTAX_FUNCTION(PX_Syntax_Parse_INCLUDE_ERROR)
{

	PX_StringCatFormat2(&pSyntax->message, "%1:%2 Error:Incorrect include syntax.\n", \
		PX_STRINGFORMAT_INT(PX_LexerGetCurrentLine(past->lexer_state.plexer)), \
		PX_STRINGFORMAT_INT(PX_LexerGetCurrentColumn(past->lexer_state.plexer)));
	return PX_TRUE;
}

px_bool PX_Syntax_Load_include(PX_Syntax* pSyntax)
{
	PX_Syntax_Parse_PEBNF(pSyntax, "INCLUDE = '#include' STRING", PX_Syntax_Parse_INCLUDE);
	PX_Syntax_Parse_PEBNF(pSyntax, "INCLUDE = '#include' BCONTAINER", PX_Syntax_Parse_INCLUDE);
	PX_Syntax_Parse_PEBNF(pSyntax, "INCLUDE = '#include' *", PX_Syntax_Parse_INCLUDE_ERROR);
	return PX_TRUE;
}
