#include "PX_Syntax_declare_prefix.h"

PX_SYNTAX_FUNCTION(PX_Syntax_load_declare_prefix_exec)
{
	px_int begin, end;
	px_int begin_source_index, end_source_index;
	px_abi* pabi = PX_Syntax_NewAbi(pSyntax, "declare_prefix", pSyntax->reg_lifetime);
	if (!pabi)
	{
		PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_load_declare_prefix_exec Memory Error1");
		return PX_FALSE;
	}
	if (!PX_AbiSet_string(pabi, "value", PX_Syntax_GetCurrentLexeme(pSyntax)))
	{
		PX_AbiFree(pabi);
		PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_load_declare_prefix_exec Memory Error2");
		return PX_FALSE;
	}

	//symbol ir
	begin_source_index = PX_Syntax_GetCurrentLexemeBeginSourceIndex(pSyntax);
	end_source_index = PX_Syntax_GetCurrentLexemeEndSourceIndex(pSyntax);
	begin = PX_Syntax_GetCurrentLexemeBegin(pSyntax);
	end = PX_Syntax_GetCurrentLexemeEnd(pSyntax);
	if (begin_source_index != end_source_index)
	{
		PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_load_declare_prefix_exec Memory Error1");
		return PX_FALSE;
	}
	PX_Syntax_NewMapToken(pSyntax, begin_source_index, begin, end_source_index, end, PX_COLOR(255, 232, 212, 166), "declare_prefix");


	return PX_TRUE;
}


PX_SYNTAX_FUNCTION(PX_Syntax_load_declare_prefixs_exec)
{
	//merge declare_prefix to declare_prefixs
	px_abi* pabi = PX_Syntax_NewAbi(pSyntax, "declare_prefixs", pSyntax->reg_lifetime);
	px_int count = 0;
	if (!pabi)
	{
		PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_new_declare_prefixs_exec Memory Error1");
		return PX_FALSE;
	}
	

	while (PX_TRUE)
	{
		px_int declare_prefix_index = PX_Syntax_GetAbiIndexFromBackward(pSyntax, "declare_prefix");
		const px_char* pdeclare_prefix_value;
		px_char payload[8] = { 0 };
		if (declare_prefix_index==-1)
		{
			break;
		}
		pdeclare_prefix_value = PX_AbiGetValue_string(PX_Syntax_GetAbiByIndex(pSyntax, declare_prefix_index), "value");
		//set declare_prefixs value
		PX_sprintf1(payload, sizeof(payload), "[%1]", PX_STRINGFORMAT_INT(count));
		if (!PX_AbiSet_string(pabi, payload, pdeclare_prefix_value))
		{
			PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_new_declare_prefixs_exec Memory Error2");
			return PX_FALSE;
		}
		PX_Syntax_PopAbiIndex(pSyntax, declare_prefix_index);
	}
	return PX_TRUE;
}

px_bool PX_Syntax_load_declare_prefix(PX_Syntax* pSyntax)
{
	if (!PX_Syntax_Parse_PEBNF(pSyntax, "declare_prefix = 'static'",0, PX_Syntax_load_declare_prefix_exec, 0))
		return PX_FALSE;

	if (!PX_Syntax_Parse_PEBNF(pSyntax, "declare_prefix = 'const'",0, PX_Syntax_load_declare_prefix_exec, 0))
		return PX_FALSE;

	if (!PX_Syntax_Parse_PEBNF(pSyntax, "declare_prefix = 'unsigned'", 0, PX_Syntax_load_declare_prefix_exec, 0))
		return PX_FALSE;

	if (!PX_Syntax_Parse_PEBNF(pSyntax, "declare_prefixs = declare_prefix ", 0, 0, 0))
		return PX_FALSE;

	if (!PX_Syntax_Parse_PEBNF(pSyntax, "declare_prefixs = declare_prefix ...", 0, 0, 0))
		return PX_FALSE;

	if (!PX_Syntax_Parse_PEBNF(pSyntax, "declare_prefixs = declare_prefix *", 0, PX_Syntax_load_declare_prefixs_exec, 0))
		return PX_FALSE;

	return PX_TRUE;
}
