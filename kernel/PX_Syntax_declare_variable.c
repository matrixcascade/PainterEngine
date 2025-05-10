#include "PX_Syntax_declare_variable.h"



PX_SYNTAX_FUNCTION(PX_Syntax_new_identifier_token)
{
	px_abi* pabi;
	if (!PX_Syntax_MergeLastAbi(pSyntax, "variable"))
	{
		PX_Syntax_Terminate(pSyntax, "Memory Error");
		return PX_FALSE;
	}
	pabi = PX_Syntax_GetAbiLast(pSyntax);
	if (!pabi)
	{
		PX_Syntax_Terminate(pSyntax, "Memory Error");
		return PX_FALSE;
	}
	if (!PX_AbiSet_int(pabi, "size", 0))
	{
		PX_Syntax_Terminate(pSyntax, "Memory Error");
		return PX_FALSE;
	}
	if (!PX_AbiSet_int(pabi, "offset", 0))
	{
		PX_Syntax_Terminate(pSyntax, "Memory Error");
		return PX_FALSE;
	}
	if (!PX_AbiSet_int(pabi, "count", 1))
	{
		PX_Syntax_Terminate(pSyntax, "Memory Error");
		return PX_FALSE;
	}
	return PX_TRUE;
}

PX_SYNTAX_FUNCTION(PX_Syntax_merge_suffix_to_identifier)
{
	px_abi* plastabi;
	px_int* p, count;
	plastabi = PX_Syntax_GetAbiLast(pSyntax);
	PX_ASSERTIFX(!PX_Syntax_CheckAbiName(plastabi, "declare_array"), "Error: last abi is not variable");
	p = PX_AbiGet_int(plastabi, "count");
	PX_ASSERTIFX(!p, "Error: count not found!");
	count = *p;
	if (!PX_Syntax_MergeLast2AbiToSecondLast(pSyntax))
	{
		PX_Syntax_Terminate(pSyntax, "Memory Error");
		return PX_FALSE;
	}
	plastabi = PX_Syntax_GetAbiLast(pSyntax);
	PX_ASSERTIFX(!PX_Syntax_CheckAbiName(plastabi, "variable"), "Error: last abi is not variable");
	if (!PX_AbiSet_int(plastabi, "count", count))
	{
		PX_Syntax_Terminate(pSyntax, "Memory Error");
		return PX_FALSE;
	}
	return PX_TRUE;
}


PX_SYNTAX_FUNCTION(PX_Syntax_update_variable_attributes)
{
	px_int* pi, typesize=0, pointer_level=0, alloc_offset=0,count=1;
	px_abi* pabi=PX_NULL;
	px_int index = PX_Syntax_FindAbiIndexFromBackward(pSyntax, "type");
	PX_ASSERTIFX(index == -1, "Error: type not found");
	pabi = PX_Syntax_GetAbiByIndex(pSyntax, index);
	pi = PX_AbiGet_int(pabi, "type_define.size");//get type_define size
	PX_ASSERTIFX(pi == 0, "Error: type size not found");
	typesize = *pi;
	if (typesize==0)
	{
		PX_Syntax_Terminate(pSyntax, "Error: uncompleted type");
		return PX_FALSE;
	}
	
	if (!PX_Syntax_CopyAbiToLast(pSyntax, index))
	{
		PX_Syntax_Terminate(pSyntax, "Memory Error");
		return PX_FALSE;
	}

	index = PX_Syntax_FindAbiIndexFromBackward(pSyntax, "declare_token_prefix");
	if (index!=-1)
	{
		typesize = 1;
		pabi = PX_Syntax_GetAbiByIndex(pSyntax, index);
		if (PX_AbiExist_string(pabi, "type", "pointer"))
		{
			pi = PX_AbiGet_int(pabi, "pointer_level");
			PX_ASSERTIFX(pi == 0, "Error: pointer level is not valid");
			pointer_level = *pi;
		}
		else
		{
			return PX_FALSE;
		}
		if (!PX_Syntax_CopyAbiToLast(pSyntax, index))
		{
			PX_Syntax_Terminate(pSyntax, "Memory Error");
			return PX_FALSE;
		}
	}

	pabi = PX_Syntax_GetAbiLast(pSyntax);
	pi = PX_AbiGet_int(pabi, "count");
	if (pi)
	{
		count = *pi;
	}
	index = PX_Syntax_FindAbiIndexFromBackward(pSyntax, "declare_prefix");
	if (index != -1)
	{
		pabi = PX_Syntax_GetAbiByIndex(pSyntax, index);
		if (PX_AbiExist_int(pabi,"lifetime",1)|| PX_AbiExist_string(pabi, "value", "static"))
		{
			pabi = PX_Syntax_GetAbiLast(pSyntax);
			if (!PX_AbiSet_int(pabi, "size", typesize * count))
			{
				PX_Syntax_Terminate(pSyntax, "Memory Error");
				return PX_FALSE;
			}
			alloc_offset = PX_Syntax_AllocGlobal(pSyntax, typesize*count);
			if (!PX_AbiSet_string(pabi, "scope", "global"))
			{
				PX_Syntax_Terminate(pSyntax, "Memory Error");
				return PX_FALSE;
			}
		}
		else
		{
			pabi = PX_Syntax_GetAbiLast(pSyntax);
			if (!PX_AbiSet_int(pabi, "size", typesize * count))
			{
				PX_Syntax_Terminate(pSyntax, "Memory Error");
				return PX_FALSE;
			}
			alloc_offset = PX_Syntax_AllocLocal(pSyntax, typesize * count);
			if (!PX_AbiSet_string(pabi, "scope", "local"))
			{
				PX_Syntax_Terminate(pSyntax, "Memory Error");
				return PX_FALSE;
			}
		}
	}
	else
	{
		pabi = PX_Syntax_GetAbiByIndex(pSyntax, index);
		if (PX_AbiExist_int(pabi, "lifetime", 1))
		{
			if (!PX_AbiSet_int(pabi, "size", typesize * count))
			{
				PX_Syntax_Terminate(pSyntax, "Memory Error");
				return PX_FALSE;
			}
			alloc_offset = PX_Syntax_AllocGlobal(pSyntax, typesize * count);
			if (!PX_AbiSet_string(pabi, "scope", "global"))
			{
				PX_Syntax_Terminate(pSyntax, "Memory Error");
				return PX_FALSE;
			}
		}
		else
		{
			if (!PX_AbiSet_int(pabi, "size", typesize * count))
			{
				PX_Syntax_Terminate(pSyntax, "Memory Error");
				return PX_FALSE;
			}
			alloc_offset = PX_Syntax_AllocLocal(pSyntax, typesize * count);
			if (!PX_AbiSet_string(pabi, "scope", "local"))
			{
				PX_Syntax_Terminate(pSyntax, "Memory Error");
				return PX_FALSE;
			}
		}
	}
	if (!PX_AbiSet_int(pabi, "offset", alloc_offset))
	{
		PX_Syntax_Terminate(pSyntax, "Memory Error");
		return PX_FALSE;
	}

	PX_Syntax_Message(pSyntax, "DECLARE:\"");
	PX_Syntax_Message(pSyntax, PX_AbiGet_string(pabi, "identifier.value"));
	PX_Syntax_Message(pSyntax, "\" scope:");
	PX_Syntax_Message(pSyntax, PX_AbiGet_string(pabi, "scope"));
	PX_Syntax_Message(pSyntax, " size:");
	PX_Syntax_Message(pSyntax, PX_itos(*PX_AbiGet_int(pabi, "size"), 10).data);
	PX_Syntax_Message(pSyntax, " offset:");
	PX_Syntax_Message(pSyntax, PX_itos(*PX_AbiGet_int(pabi, "offset"), 10).data);
	PX_Syntax_Message(pSyntax, " \n");

	return PX_TRUE;
}

PX_SYNTAX_FUNCTION(PX_Syntax_declare_variable_struct_type)
{
	px_abi* pabi = PX_Syntax_LastTypeDefine(pSyntax),merge_abi;
	px_abi* newabi;
	merge_abi = *pabi;
	newabi = PX_Syntax_NewAbi(pSyntax, "type", pSyntax->reg_lifetime);
	if (!newabi)
	{
		PX_Syntax_Terminate(pSyntax, "Memory Error");
		return PX_FALSE;
	}
	PX_ASSERTIFX(pabi == PX_NULL, "Error: type define not found");
	if (!PX_AbiSet_Abi(newabi, "type_define", &merge_abi))
	{
		PX_Syntax_Terminate(pSyntax, "Memory Error");
		return PX_FALSE;
	}
	return PX_TRUE;
}

PX_SYNTAX_FUNCTION(PX_Syntax_declare_variable_end)
{
	px_int index = PX_Syntax_FindAbiIndexFromBackward(pSyntax, "declare_prefix");
	if(index != -1)
		PX_Syntax_PopAbiIndex(pSyntax, index);
	index = PX_Syntax_FindAbiIndexFromBackward(pSyntax, "type");
	PX_ASSERTIFX(index == -1, "Error: type not found");
	PX_Syntax_PopAbiIndex(pSyntax, index);
	return PX_TRUE;
}


px_bool PX_Syntax_load_declare_variable(PX_Syntax* pSyntax)
{
	if (!PX_Syntax_Parse_PEBNF(pSyntax, "identifier_token =identifier", PX_Syntax_new_identifier_token))
		return PX_FALSE;

	if (!PX_Syntax_Parse_PEBNF(pSyntax, "identifier_token =identifier declare_token_suffix", PX_Syntax_merge_suffix_to_identifier))
		return PX_FALSE;

	if (!PX_Syntax_Parse_PEBNF(pSyntax, "identifier_token =identifier *", 0))
		return PX_FALSE;

	if (!PX_Syntax_Parse_PEBNF(pSyntax, "declare_token =declare_token_prefix identifier_token", PX_Syntax_update_variable_attributes))
		return PX_FALSE;

	if (!PX_Syntax_Parse_PEBNF(pSyntax, "declare_token =identifier_token", PX_Syntax_update_variable_attributes))
		return PX_FALSE;

	if (!PX_Syntax_Parse_PEBNF(pSyntax, "declare_list = declare_token ", 0))
		return PX_FALSE;

	if (!PX_Syntax_Parse_PEBNF(pSyntax, "declare_list = declare_token ',' ... ", 0))
		return PX_FALSE;

	if (!PX_Syntax_Parse_PEBNF(pSyntax, "declare_list = declare_token * ", 0))
		return PX_FALSE;

	if (!PX_Syntax_Parse_PEBNF(pSyntax, "type_declare = define_struct", PX_Syntax_declare_variable_struct_type))
		return PX_FALSE;

	if (!PX_Syntax_Parse_PEBNF(pSyntax, "type_declare = define_struct declare_list", 0))
		return PX_FALSE;

	if (!PX_Syntax_Parse_PEBNF(pSyntax, "type_declare = define_struct *", 0))
		return PX_FALSE;

	if (!PX_Syntax_Parse_PEBNF(pSyntax, "type_declare = type declare_list", 0))
		return PX_FALSE;

	if (!PX_Syntax_Parse_PEBNF(pSyntax, "declare_variable =declare_prefix type_declare ';'", PX_Syntax_declare_variable_end))
		return PX_FALSE;

	if (!PX_Syntax_Parse_PEBNF(pSyntax, "declare_variable =type_declare ';'", PX_Syntax_declare_variable_end))
		return PX_FALSE;
	return PX_TRUE;
}
