#include "PX_Syntax_const_list.h"

PX_SYNTAX_FUNCTION(PX_Syntax_Parse_const_set_begin)
{
	PX_Syntax_NewAbi(pSyntax, "set_begin",pSyntax->lifetime);
	return PX_TRUE;
}

PX_SYNTAX_FUNCTION(PX_Syntax_Parse_const_set_end)
{
	PX_Syntax_NewAbi(pSyntax, "set_end", pSyntax->lifetime);
	return PX_TRUE;
}


PX_SYNTAX_FUNCTION(PX_Syntax_Parse_const_int_list_error)
{
	PX_Syntax_Terminate(pSyntax, past, "expect int token");
	return PX_FALSE;
}

PX_SYNTAX_FUNCTION(PX_Syntax_Parse_const_numeric_list_error)
{
	PX_Syntax_Terminate(pSyntax, past, "expect numeric token");
	return PX_FALSE;
}

PX_SYNTAX_FUNCTION(PX_Syntax_Parse_const_string_list_error)
{
	PX_Syntax_Terminate(pSyntax, past, "expect string token");
	return PX_FALSE;
}

px_bool PX_Syntax_Load_const_list(PX_Syntax* pSyntax)
{
	if (!PX_Syntax_Parse_PEBNF(pSyntax, "numeric_list = numeric", 0))
		return PX_FALSE;

	if (!PX_Syntax_Parse_PEBNF(pSyntax, "numeric_list = numeric ',' numeric_list", 0))
		return PX_FALSE;

	if (!PX_Syntax_Parse_PEBNF(pSyntax, "numeric_list = numeric *", 0))
		return PX_FALSE;

	if (!PX_Syntax_Parse_PEBNF(pSyntax, "int_list = const_int", 0))
		return PX_FALSE;

	if (!PX_Syntax_Parse_PEBNF(pSyntax, "int_list = const_int ',' int_list", 0))
		return PX_FALSE;

	if (!PX_Syntax_Parse_PEBNF(pSyntax, "int_list = const_int ',' *", PX_Syntax_Parse_const_int_list_error))
		return PX_FALSE;

	if (!PX_Syntax_Parse_PEBNF(pSyntax, "int_list = const_int *", 0))
		return PX_FALSE;

	if (!PX_Syntax_Parse_PEBNF(pSyntax, "string_list = const_string", 0))
		return PX_FALSE;

	if (!PX_Syntax_Parse_PEBNF(pSyntax, "string_list = const_string ',' string_list", 0))
		return PX_FALSE;

	if (!PX_Syntax_Parse_PEBNF(pSyntax, "tuple_list = int_list", 0))
		return PX_FALSE;

	if (!PX_Syntax_Parse_PEBNF(pSyntax, "tuple_list = numeric_list", 0))
		return PX_FALSE;

	if (!PX_Syntax_Parse_PEBNF(pSyntax, "tuple_list = string_list", 0))
		return PX_FALSE;

	if (!PX_Syntax_Parse_PEBNF(pSyntax, "string_list = const_string ',' *", PX_Syntax_Parse_const_string_list_error))
		return PX_FALSE;

	if (!PX_Syntax_Parse_PEBNF(pSyntax, "string_list = const_string *", 0))
		return PX_FALSE;

	if (!PX_Syntax_Parse_PEBNF(pSyntax, "numeric_set = '{'", PX_Syntax_Parse_const_set_begin))
		return PX_FALSE;

	if (!PX_Syntax_Parse_PEBNF(pSyntax, "numeric_set = '{' numeric_list '}'", PX_Syntax_Parse_const_set_end))
		return PX_FALSE;

	if (!PX_Syntax_Parse_PEBNF(pSyntax, "int_set = '{'", PX_Syntax_Parse_const_set_begin))
		return PX_FALSE;

	if (!PX_Syntax_Parse_PEBNF(pSyntax, "int_set = '{' int_list '}'", PX_Syntax_Parse_const_set_end))
		return PX_FALSE;

	if (!PX_Syntax_Parse_PEBNF(pSyntax, "string_set = '{'", PX_Syntax_Parse_const_set_begin))
		return PX_FALSE;

	if (!PX_Syntax_Parse_PEBNF(pSyntax, "string_set = '{' string_list '}'", PX_Syntax_Parse_const_set_end))
		return PX_FALSE;

	if (!PX_Syntax_Parse_PEBNF(pSyntax, "tuple_set = '{'", PX_Syntax_Parse_const_set_begin))
		return PX_FALSE;

	if (!PX_Syntax_Parse_PEBNF(pSyntax, "tuple_set = '{' tuple_list '}'", PX_Syntax_Parse_const_set_end))
		return PX_FALSE;

	return PX_TRUE;
}
