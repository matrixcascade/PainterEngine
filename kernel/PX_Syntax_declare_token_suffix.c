#include "PX_Syntax_declare_token_suffix.h"
PX_SYNTAX_FUNCTION(PX_Syntax_load_declare_array_begin)
{
	px_abi* pabi;
	if (!PX_Syntax_CheckLastAbiName(pSyntax, "declare_array"))
	{
		pabi = PX_Syntax_NewAbi(pSyntax, "declare_array", pSyntax->reg_lifetime);
		if (!pabi)
		{
			PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_load_declare_array_begin Memory Error1");
			return PX_FALSE;
		}
		if (!PX_AbiSet_int(pabi, "d", 1))
		{
			PX_AbiFree(pabi);
			PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_load_declare_array_begin Memory Error2");
			return PX_FALSE;
		}
		if (!PX_AbiSet_int(pabi, "1", 0))
		{
			PX_AbiFree(pabi);
			PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_load_declare_array_begin Memory Error3");
			return PX_FALSE;
		}
		if (!PX_AbiSet_int(pabi, "source_index", PX_Syntax_GetCurrentSourceIndex(pSyntax)))
		{
			PX_AbiFree(pabi);
			PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_load_declare_array_begin Memory Error4");
			return PX_FALSE;
		}
		if (!PX_AbiSet_int(pabi, "begin", PX_Syntax_GetCurrentLexemeBegin(pSyntax)))
		{
			PX_AbiFree(pabi);
			PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_load_declare_array_begin Memory Error4");
			return PX_FALSE;
		}
		
	}
	else
	{
		px_int d;
		pabi = PX_Syntax_GetAbiLast(pSyntax);
		if (!pabi)
		{
			PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_load_declare_array_begin Memory Error5");
			return PX_FALSE;
		}
		d = PX_AbiGetValue_int(pabi, "d");
		if (!PX_AbiSet_int(pabi, "d", d + 1))
		{
			PX_AbiFree(pabi);
			PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_load_declare_array_begin Memory Error6");
			return PX_FALSE;
		}
		if (!PX_AbiSet_int(pabi, PX_itos(d+1, 10).data, 0))
		{
			PX_AbiFree(pabi);
			PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_load_declare_array_begin Memory Error7");
			return PX_FALSE;
		}
	}
	return PX_TRUE;
}

PX_SYNTAX_FUNCTION(PX_Syntax_load_declare_array_end)
{
	px_abi* pabi;
	if (PX_Syntax_CheckLastAbiName(pSyntax, "declare_array"))
	{
		pabi = PX_Syntax_GetAbiLast(pSyntax);
		if (!pabi)
		{
			PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_load_declare_array_end Memory Error1");
			return PX_FALSE;
		}
		if (!PX_AbiSet_int(pabi, "end", PX_Syntax_GetCurrentLexemeEnd(pSyntax)))
		{
			PX_AbiFree(pabi);
			PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_load_declare_array_end Memory Error2");
			return PX_FALSE;
		}
		return PX_TRUE;
	}
	else
	{
		PX_ASSERTX("Syntax declare_array Unknow Error");
		return PX_FALSE;
	}
	return PX_TRUE;
	
}

PX_SYNTAX_FUNCTION(PX_Syntax_load_declare_array_suffix_exec)
{
	px_abi* pabi;
	px_int c,lastc;
	if (!PX_Syntax_CheckLastAbiName(pSyntax, "const_int"))
	{
		c = 0;
	}
	else
	{
		const px_char* pvalue = PX_AbiGet_string(PX_Syntax_GetAbiLast(pSyntax), "value");
		c = PX_atoi(pvalue);
		PX_Syntax_PopAbi(pSyntax);
	}
	
	if (c == 0)
	{
		PX_Syntax_Terminate(pSyntax, "ast:error:unexpected array count");
		return PX_FALSE;
	}
	if (PX_Syntax_CheckLastAbiName(pSyntax, "declare_array"))
	{
		px_int d;
		pabi = PX_Syntax_GetAbiLast(pSyntax);
		d = PX_AbiGetValue_int(pabi, "d");
		
		if (d>1)
		{
			lastc = PX_AbiGetValue_int(pabi, PX_itos(d - 1, 10).data);
			if (lastc == 0)
			{
				PX_Syntax_Terminate(pSyntax, "ast:error:Syntax Error:unexpected array count");
				return PX_FALSE;
			}
			
		}
		if(!PX_AbiSet_int(pabi, PX_itos(d,10).data, c))
		{
			PX_AbiFree(pabi);
			PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_load_declare_array_suffix_exec Memory Error");
			return PX_FALSE;
		}
		return PX_TRUE;
	}
	else
	{
		PX_ASSERTX("Syntax declare_array Unknow Error");

		return PX_FALSE;
	}
	return PX_TRUE;
}

PX_SYNTAX_FUNCTION(PX_Syntax_load_declare_array_suffix_error)
{
	PX_Syntax_Terminate(pSyntax, "ast:error:Syntax Error:unexpected declare_array");
	return PX_FALSE;
}

PX_SYNTAX_FUNCTION(PX_Syntax_load_declare_array_suffix_end)
{
	px_int* pd=0, d;
	px_int* pc=0, c;
	px_int  source_index,begin, end;
	px_int count=1;
	px_abi* pabilast = PX_Syntax_GetAbiLast(pSyntax);
	PX_ASSERTIF(!PX_Syntax_CheckAbiName(pabilast, "declare_array"));
	pd = PX_AbiGet_int(pabilast, "d");
	PX_ASSERTIF(pd == 0);
	d = *pd;
	while (d>0)
	{
		pc = PX_AbiGet_int(pabilast, PX_itos(d, 10).data);
		PX_ASSERTIF(pc == 0);
		c = *pc;
		count *= c;
		d--;
	}
	if (!PX_AbiSet_int(pabilast,"count",count))
	{
		PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_load_declare_array_suffix_end Memory Error1");
		return PX_FALSE;
	}
	
	begin = PX_AbiGetValue_int(pabilast, "begin");
	end = PX_AbiGetValue_int(pabilast, "end");
	source_index = PX_AbiGetValue_int(pabilast, "source_index");
	if (!PX_Syntax_NewMapToken(pSyntax, source_index,  begin, source_index, end,PX_COLOR(255,199,255,188), "array"))
	{
		PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_load_declare_array_suffix_end Memory Error2");
		return PX_FALSE;
	}
	return PX_TRUE;
}

px_bool PX_Syntax_load_declare_token_suffix(PX_Syntax* pSyntax)
{
	if (!PX_Syntax_Parse_PEBNF(pSyntax, "declare_array = '['", 0, PX_Syntax_load_declare_array_begin, 0))
		return PX_FALSE;

	if (!PX_Syntax_Parse_PEBNF(pSyntax, "declare_array = '[' ']'",0, PX_Syntax_load_declare_array_end, 0))
		return PX_FALSE;

	if (!PX_Syntax_Parse_PEBNF(pSyntax, "declare_array = '[' const_int ",0, PX_Syntax_load_declare_array_suffix_exec, 0))
		return PX_FALSE;

	if (!PX_Syntax_Parse_PEBNF(pSyntax, "declare_array = '[' const_int ']'",0, PX_Syntax_load_declare_array_end, 0))
		return PX_FALSE;

	if (!PX_Syntax_Parse_PEBNF(pSyntax, "declare_array = '[' const_int *",0, PX_Syntax_load_declare_array_suffix_error, 0))
		return PX_FALSE;

	if (!PX_Syntax_Parse_PEBNF(pSyntax, "declare_token_suffix = declare_array",0, 0, 0))
		return PX_FALSE;

	if (!PX_Syntax_Parse_PEBNF(pSyntax, "declare_token_suffix = declare_array ...",0, 0, 0))
		return PX_FALSE;

	if (!PX_Syntax_Parse_PEBNF(pSyntax, "declare_token_suffix = declare_array *",0, PX_Syntax_load_declare_array_suffix_end, 0))
		return PX_FALSE;

	return PX_TRUE;

}
