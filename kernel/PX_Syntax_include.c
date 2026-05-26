#include "PX_Syntax_include.h"

PX_SYNTAX_FUNCTION(PX_Syntax_Parse_mark_include)
{
	px_int begin = 0, end = 0, begin_source_index, end_source_index;
	begin = PX_Syntax_GetCurrentLexemeBegin(pSyntax);
	end = PX_Syntax_GetCurrentLexemeEnd(pSyntax);

	begin_source_index = PX_Syntax_GetCurrentLexemeBeginSourceIndex(pSyntax);
	end_source_index = PX_Syntax_GetCurrentLexemeEndSourceIndex(pSyntax);
	if (begin_source_index != end_source_index)
	{
		PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_Parse_mark_include Memory Error1");
		return PX_FALSE;
	}
	if (!PX_Syntax_NewMapToken(pSyntax, begin_source_index,begin, end_source_index,end,PX_COLOR(255,128,128,255),0))
	{
		PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_Parse_mark_include Memory Error");
		return PX_FALSE;
	}
	return PX_TRUE;

}

PX_SYNTAX_FUNCTION(PX_Syntax_Parse_include)
{
	const px_char* pfilename;
	px_abi* plast = PX_Syntax_GetAbiLast(pSyntax);
	pfilename = PX_AbiGet_string(plast, "value");
	if(PX_Syntax_CallSource(pSyntax, pfilename,PX_NULL)==PX_FALSE)
	{
		PX_Syntax_Terminate(pSyntax, "ast:error: include file not found");
		return PX_FALSE;
	}
	PX_Syntax_PopAbi(pSyntax);
	return PX_TRUE;
}

PX_SYNTAX_FUNCTION(PX_Syntax_Parse_include_error)
{
	PX_Syntax_Terminate(pSyntax, "ast:error: include statement error");
	return PX_FALSE;
}


px_bool PX_Syntax_load_include(PX_Syntax* pSyntax)
{
	if (!PX_Syntax_Parse_PEBNF(pSyntax, "include = '#' 'include' ", 0, PX_Syntax_Parse_mark_include, 0))
		return PX_FALSE;

	if (!PX_Syntax_Parse_PEBNF(pSyntax, "include = '#' 'include' bcontainer '\n'",0, PX_Syntax_Parse_include, 0))
		return PX_FALSE;

	if (!PX_Syntax_Parse_PEBNF(pSyntax, "include = '#' 'include' const_string '\n'", 0, PX_Syntax_Parse_include, 0))
		return PX_FALSE;

	if (!PX_Syntax_Parse_PEBNF(pSyntax, "include = '#' 'include' * '\n'", 0, PX_Syntax_Parse_include_error, 0))
		return PX_FALSE;

	return PX_TRUE;
}