#include "PX_Syntax_typedef.h"
PX_SYNTAX_FUNCTION(PX_Syntax_Parse_typedef_begin)
{
	px_int begin = 0, end = 0, begin_source_index, end_source_index;
	begin = PX_Syntax_GetCurrentLexemeBegin(pSyntax);
	end = PX_Syntax_GetCurrentLexemeEnd(pSyntax);
	begin_source_index = PX_Syntax_GetCurrentLexemeBeginSourceIndex(pSyntax);
	end_source_index = PX_Syntax_GetCurrentLexemeEndSourceIndex(pSyntax);
	if (begin_source_index != end_source_index)
	{
		PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_Parse_typedef_begin Memory Error1");
		return PX_FALSE;
	}
	if (!PX_Syntax_NewMapToken(pSyntax, begin_source_index, begin, end_source_index, end, PX_COLOR(255, 255, 192, 64), "typedef begin"))
	{
		PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_Parse_typedef_begin Memory Error1");
		return PX_FALSE;
	}
	return PX_TRUE;
}
PX_SYNTAX_FUNCTION(PX_Syntax_Parse_typedef)
{
	px_abi* psecondlastabi, * plastabi;
	const px_char* ptype_source = PX_NULL;
	const px_char* ptype_target = PX_NULL;
	psecondlastabi = PX_Syntax_GetAbiSecondLast(pSyntax);
	PX_ASSERTIFX(!psecondlastabi || PX_Syntax_CheckAbiName(psecondlastabi, "type") == PX_FALSE, "Unknow Error");
	plastabi = PX_Syntax_GetAbiLast(pSyntax);
	PX_ASSERTIFX(!plastabi || PX_Syntax_CheckAbiName(plastabi, "identifier") == PX_FALSE, "Unknow Error");
	ptype_source = PX_AbiGet_string(psecondlastabi, "type_define.type");
	ptype_target = PX_AbiGet_string(plastabi, "value");
	if (!PX_Syntax_NewTypedef(pSyntax, ptype_source, ptype_target))
	{
		PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_Parse_typedef Memory Error1");
		return PX_FALSE;
	}
	return PX_TRUE;

}

PX_SYNTAX_FUNCTION(PX_Syntax_Parse_typedef_error)
{
	PX_Syntax_Terminate(pSyntax, "ast:error: typedef statement error");
	return PX_FALSE;
}


px_bool PX_Syntax_load_typedef(PX_Syntax* pSyntax)
{
	if (!PX_Syntax_Parse_PEBNF(pSyntax, "typedef = 'typedef' ", 0, PX_Syntax_Parse_typedef_begin,0))
		return PX_FALSE;

	if (!PX_Syntax_Parse_PEBNF(pSyntax, "typedef = 'typedef' type identifier ", 0, PX_Syntax_Parse_typedef,0))
		return PX_FALSE;

	return PX_TRUE;
}