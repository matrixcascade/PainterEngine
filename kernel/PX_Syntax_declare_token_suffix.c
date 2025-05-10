#include "PX_Syntax_declare_token_suffix.h"

PX_SYNTAX_FUNCTION(PX_Syntax_load_declare_array_0_suffix)
{
	px_abi* pabi;
	if (PX_Syntax_CheckLastAbiName(pSyntax,"declare_array"))
	{
		px_int d,c;
		pabi = PX_Syntax_GetAbiLast(pSyntax);
		if (!pabi)
		{
			PX_Syntax_Terminate(pSyntax, "Memory Error");
			return PX_FALSE;
		}
		d = *PX_AbiGet_int(pabi, "d");
		c = *PX_AbiGet_int(pabi, PX_itos(d-1,10).data);
		if (c==0)
		{
			PX_Syntax_Terminate(pSyntax, "Syntax Error:unexpected array count");
			return PX_FALSE;
		}
		if (!PX_AbiSet_int(pabi, "d", d + 1))
		{
			PX_AbiFree(pabi);
			PX_Syntax_Terminate(pSyntax, "Memory Error");
			return PX_FALSE;
		}
		if (!PX_AbiSet_int(pabi, PX_itos(d, 10).data, c))
		{
			PX_AbiFree(pabi);
			PX_Syntax_Terminate(pSyntax, "Memory Error");
			return PX_FALSE;
		}
		return PX_TRUE;
	}
	pabi = PX_Syntax_NewAbi(pSyntax, "declare_array", pSyntax->reg_lifetime);
	if (!pabi)
	{
		PX_Syntax_Terminate(pSyntax, "Memory Error");
		return PX_FALSE;
	}
	if (!PX_AbiSet_int(pabi, "d", 1))
	{
		PX_AbiFree(pabi);
		PX_Syntax_Terminate(pSyntax, "Memory Error");
		return PX_FALSE;
	}
	if (!PX_AbiSet_int(pabi, "0", 0))
	{
		PX_AbiFree(pabi);
		PX_Syntax_Terminate(pSyntax, "Memory Error");
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
		PX_Syntax_Terminate(pSyntax, "Syntax Error:unexpected array count");
		return PX_FALSE;
	}
	if (PX_Syntax_CheckLastAbiName(pSyntax, "declare_array"))
	{
		px_int d;
		pabi = PX_Syntax_GetAbiLast(pSyntax);
		d = *PX_AbiGet_int(pabi, "d");
		if (!PX_AbiSet_int(pabi, "d", d + 1))
		{
			PX_AbiFree(pabi);
			PX_Syntax_Terminate(pSyntax, "Memory Error");
			return PX_FALSE;
		}
		lastc = *PX_AbiGet_int(pabi, PX_itos(d - 1, 10).data);
		if (lastc==0)
		{
			PX_Syntax_Terminate(pSyntax, "Syntax Error:unexpected array count");
			return PX_FALSE;
		}
		if(!PX_AbiSet_int(pabi, PX_itos(d,10).data, c))
		{
			PX_AbiFree(pabi);
			PX_Syntax_Terminate(pSyntax, "Memory Error");
			return PX_FALSE;
		}
		return PX_TRUE;
	}
	pabi = PX_Syntax_NewAbi(pSyntax, "declare_array", pSyntax->reg_lifetime);
	if (!pabi)
	{
		PX_Syntax_Terminate(pSyntax, "Memory Error");
		return PX_FALSE;
	}
	if (!PX_AbiSet_int(pabi, "d", 1))
	{
		PX_AbiFree(pabi);
		PX_Syntax_Terminate(pSyntax, "Memory Error");
		return PX_FALSE;
	}
	if (!PX_AbiSet_int(pabi,"0", c))
	{
		PX_AbiFree(pabi);
		PX_Syntax_Terminate(pSyntax, "Memory Error");
		return PX_FALSE;
	}
	return PX_TRUE;
}

PX_SYNTAX_FUNCTION(PX_Syntax_load_declare_array_suffix_error)
{
	PX_Syntax_Terminate(pSyntax, "Syntax Error:unexpected declare_array");
	return PX_FALSE;
}

PX_SYNTAX_FUNCTION(PX_Syntax_load_declare_array_suffix_end)
{
	px_int* pd=0, d;
	px_int* pc=0, c;
	px_int count=1;
	px_abi* pabilast = PX_Syntax_GetAbiLast(pSyntax);
	PX_ASSERTIF(!PX_Syntax_CheckAbiName(pabilast, "declare_array"));
	pd = PX_AbiGet_int(pabilast, "d");
	PX_ASSERTIF(pd == 0);
	d = *pd;
	while (d--)
	{
		pc = PX_AbiGet_int(pabilast, PX_itos(d, 10).data);
		PX_ASSERTIF(pc == 0);
		c = *pc;
		count *= c;
	}
	if (!PX_AbiSet_int(pabilast,"count",count))
	{
		PX_Syntax_Terminate(pSyntax, "Memory Error");
		return PX_FALSE;
	}
	return PX_TRUE;
}

px_bool PX_Syntax_load_declare_token_suffix(PX_Syntax* pSyntax)
{
	if (!PX_Syntax_Parse_PEBNF(pSyntax, "declare_array = '[' ']'", PX_Syntax_load_declare_array_0_suffix))
		return PX_FALSE;

	if (!PX_Syntax_Parse_PEBNF(pSyntax, "declare_array = '[' const_int ", PX_Syntax_load_declare_array_suffix_exec))
		return PX_FALSE;

	if (!PX_Syntax_Parse_PEBNF(pSyntax, "declare_array = '[' const_int ']'", 0))
		return PX_FALSE;

	if (!PX_Syntax_Parse_PEBNF(pSyntax, "declare_array = '[' const_int *", PX_Syntax_load_declare_array_suffix_error))
		return PX_FALSE;

	if (!PX_Syntax_Parse_PEBNF(pSyntax, "declare_token_suffix = declare_array", 0))
		return PX_FALSE;

	if (!PX_Syntax_Parse_PEBNF(pSyntax, "declare_token_suffix = declare_array ...", 0))
		return PX_FALSE;

	if (!PX_Syntax_Parse_PEBNF(pSyntax, "declare_token_suffix = declare_array *", PX_Syntax_load_declare_array_suffix_end))
		return PX_FALSE;

	return PX_TRUE;

}
