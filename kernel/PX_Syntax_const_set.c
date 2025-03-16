#include "PX_Syntax_const_set.h"


PX_SYNTAX_FUNCTION(PX_Syntax_Parse_const_int_set)
{
	
	return PX_TRUE;
}

PX_SYNTAX_FUNCTION(PX_Syntax_Parse_const_float_set)
{

	return PX_TRUE;
}

PX_SYNTAX_FUNCTION(PX_Syntax_Parse_const_string_set)
{

	return PX_TRUE;
}

PX_SYNTAX_FUNCTION(PX_Syntax_Parse_const_tuple_set)
{

	return PX_TRUE;
}



px_bool PX_Syntax_Load_const_set(PX_Syntax* pSyntax)
{
	PX_Syntax_Parse_PEBNF(pSyntax, "const_set = '{' const_int_list '}'", PX_Syntax_Parse_const_int_set);


	
	return PX_TRUE;
}