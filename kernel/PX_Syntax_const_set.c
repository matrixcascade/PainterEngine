#include "PX_Syntax_const_set.h"
PX_SYNTAX_FUNCTION(PX_Syntax_Parse_const_int_set_begin)
{
	px_abi* pnewabi = PX_Syntax_NewAbi(pSyntax, "const_int_set", pSyntax->reg_lifetime);
	if (!pnewabi)
	{
		PX_Syntax_Terminate(pSyntax, "Memory Error");
		return PX_FALSE;
	}

	if (!PX_AbiSet_int(pnewabi, "begin", PX_Syntax_GetCurrentLexemeBegin(pSyntax)))
	{
		PX_Syntax_Terminate(pSyntax, "Memory Error");
		return PX_FALSE;
	}
	return PX_TRUE;
}

PX_SYNTAX_FUNCTION(PX_Syntax_Parse_const_int_set)
{
	px_abi* plastabi = PX_Syntax_GetAbiLast(pSyntax);
	const px_char* pname;
	if(!plastabi)
	{
		return PX_FALSE;
	}
	pname = PX_AbiGet_string(plastabi, "name");
	if (!pname)
	{
		return PX_FALSE;
	}
	if (!PX_strequ(pname, "const_int_list"))
	{
		return PX_FALSE;
	}
	if (!PX_Syntax_MergeLast2AbiToSecondLast(pSyntax))
	{
		PX_Syntax_Terminate(pSyntax, "Memory Error");
		return PX_FALSE;
	}
	plastabi = PX_Syntax_GetAbiLast(pSyntax);
	if (!plastabi)
	{
		return PX_FALSE;
	}
	if (!PX_AbiSet_int(plastabi, "end", PX_Syntax_GetCurrentLexemeEnd(pSyntax)))
	{
		PX_Syntax_Terminate(pSyntax, "Memory Error");
		return PX_FALSE;
	}

	return PX_TRUE;
}

PX_SYNTAX_FUNCTION(PX_Syntax_Parse_const_float_set_begin)
{
	px_abi* pnewabi = PX_Syntax_NewAbi(pSyntax, "const_float_set", pSyntax->reg_lifetime);
	if (!pnewabi)
	{
		PX_Syntax_Terminate(pSyntax, "Memory Error");
		return PX_FALSE;
	}
	if (!PX_AbiSet_int(pnewabi, "begin", PX_Syntax_GetCurrentLexemeBegin(pSyntax)))
	{
		PX_Syntax_Terminate(pSyntax, "Memory Error");
		return PX_FALSE;
	}
	return PX_TRUE;
}

PX_SYNTAX_FUNCTION(PX_Syntax_Parse_const_float_set)
{
	px_abi* plastabi = PX_Syntax_GetAbiLast(pSyntax);
	const px_char* pname;
	if (!plastabi)
	{
		return PX_FALSE;
	}
	pname = PX_AbiGet_string(plastabi, "name");
	if (!pname)
	{
		return PX_FALSE;
	}
	if (!PX_strequ(pname, "const_float_list"))
	{
		return PX_FALSE;
	}
	if (!PX_Syntax_MergeLast2AbiToSecondLast(pSyntax))
	{
		PX_Syntax_Terminate(pSyntax, "Memory Error");
		return PX_FALSE;
	}
	plastabi = PX_Syntax_GetAbiLast(pSyntax);
	if (!plastabi)
	{
		return PX_FALSE;
	}
	if (!PX_AbiSet_int(plastabi, "end", PX_Syntax_GetCurrentLexemeEnd(pSyntax)))
	{
		PX_Syntax_Terminate(pSyntax, "Memory Error");
		return PX_FALSE;
	}
	return PX_TRUE;

}

PX_SYNTAX_FUNCTION(PX_Syntax_Parse_const_string_set_begin)
{
	px_abi* pnewabi = PX_Syntax_NewAbi(pSyntax, "const_string_set", pSyntax->reg_lifetime);
	if (!pnewabi)
	{
		PX_Syntax_Terminate(pSyntax, "Memory Error");
		return PX_FALSE;
	}
	if (!PX_AbiSet_int(pnewabi, "begin", PX_Syntax_GetCurrentLexemeBegin(pSyntax)))
	{
		PX_Syntax_Terminate(pSyntax, "Memory Error");
		return PX_FALSE;
	}
	return PX_TRUE;
}

PX_SYNTAX_FUNCTION(PX_Syntax_Parse_const_string_set)
{
	px_abi* plastabi = PX_Syntax_GetAbiLast(pSyntax);
	const px_char* pname;
	if (!plastabi)
	{
		return PX_FALSE;
	}
	pname = PX_AbiGet_string(plastabi, "name");
	if (!pname)
	{
		return PX_FALSE;
	}
	if (!PX_strequ(pname, "const_string_list"))
	{
		return PX_FALSE;
	}
	if (!PX_Syntax_MergeLast2AbiToSecondLast(pSyntax))
	{
		PX_Syntax_Terminate(pSyntax, "Memory Error");
		return PX_FALSE;
	}

	plastabi = PX_Syntax_GetAbiLast(pSyntax);
	if (!plastabi)
	{
		return PX_FALSE;
	}
	if (!PX_AbiSet_int(plastabi, "end", PX_Syntax_GetCurrentLexemeEnd(pSyntax)))
	{
		PX_Syntax_Terminate(pSyntax, "Memory Error");
		return PX_FALSE;
	}
	return PX_TRUE;
}

PX_SYNTAX_FUNCTION(PX_Syntax_Parse_const_tuple_set_begin)
{
	px_abi* pnewabi = PX_Syntax_NewAbi(pSyntax, "const_tuple_set", pSyntax->reg_lifetime);
	if (!pnewabi)
	{
		PX_Syntax_Terminate(pSyntax, "Memory Error");
		return PX_FALSE;
	}
	if (!PX_AbiSet_int(pnewabi, "begin", PX_Syntax_GetCurrentLexemeBegin(pSyntax)))
	{
		PX_Syntax_Terminate(pSyntax, "Memory Error");
		return PX_FALSE;
	}
	return PX_TRUE;
}

PX_SYNTAX_FUNCTION(PX_Syntax_Parse_const_tuple_set)
{
	px_abi* plastabi = PX_Syntax_GetAbiLast(pSyntax);
	const px_char* pname;
	if (!plastabi)
	{
		return PX_FALSE;
	}
	pname = PX_AbiGet_string(plastabi, "name");
	if (!pname)
	{
		return PX_FALSE;
	}
	if (!PX_strequ(pname, "const_tuple_list"))
	{
		return PX_FALSE;
	}
	if (!PX_Syntax_MergeLast2AbiToSecondLast(pSyntax))
	{
		PX_Syntax_Terminate(pSyntax, "Memory Error");
		return PX_FALSE;
	}
	plastabi = PX_Syntax_GetAbiLast(pSyntax);
	if (!plastabi)
	{
		return PX_FALSE;
	}
	if (!PX_AbiSet_int(plastabi, "end", PX_Syntax_GetCurrentLexemeEnd(pSyntax)))
	{
		PX_Syntax_Terminate(pSyntax, "Memory Error");
		return PX_FALSE;
	}
	return PX_TRUE;
}



px_bool PX_Syntax_load_const_set(PX_Syntax* pSyntax)
{
	PX_Syntax_Parse_PEBNF(pSyntax, "const_int_set = '{'", PX_Syntax_Parse_const_int_set_begin);
	PX_Syntax_Parse_PEBNF(pSyntax, "const_int_set = '{' const_int_list '}'", PX_Syntax_Parse_const_int_set);
	PX_Syntax_Parse_PEBNF(pSyntax, "const_float_set = '{'", PX_Syntax_Parse_const_float_set_begin);
	PX_Syntax_Parse_PEBNF(pSyntax, "const_float_set = '{' const_float_list '}'", PX_Syntax_Parse_const_float_set);
	PX_Syntax_Parse_PEBNF(pSyntax, "const_string_set = '{'", PX_Syntax_Parse_const_string_set_begin);
	PX_Syntax_Parse_PEBNF(pSyntax, "const_string_set = '{' const_string_list '}'", PX_Syntax_Parse_const_string_set);
	PX_Syntax_Parse_PEBNF(pSyntax, "const_tuple_set = '{'", PX_Syntax_Parse_const_tuple_set_begin);
	PX_Syntax_Parse_PEBNF(pSyntax, "const_tuple_set = '{' const_tuple_list '}'", PX_Syntax_Parse_const_tuple_set);

	return PX_TRUE;
}