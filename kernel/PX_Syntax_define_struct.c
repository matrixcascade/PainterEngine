#include "PX_Syntax_define_struct.h"
PX_SYNTAX_FUNCTION(PX_Syntax_define_struct_begin)
{
	px_abi* pabi = PX_Syntax_NewAbi(pSyntax, "define_struct", pSyntax->reg_lifetime);
	if (!pabi)
	{
		PX_Syntax_Terminate(pSyntax, "Memory Error");
		return PX_FALSE;
	}

	if (!PX_AbiSet_int(pabi, "member_count", 0))
	{
		PX_AbiFree(pabi);
		PX_Syntax_Terminate(pSyntax, "Memory Error");
		return PX_FALSE;
	}

	if (!PX_AbiSet_int(pabi, "size", 0))
	{
		PX_AbiFree(pabi);
		PX_Syntax_Terminate(pSyntax, "Memory Error");
		return PX_FALSE;
	}
	return PX_TRUE;

}

PX_SYNTAX_FUNCTION(PX_Syntax_define_struct_name)
{
	if (!PX_Syntax_RenameLastAbi(pSyntax, "struct_name"))
		return PX_FALSE;
	return PX_TRUE;
}

PX_SYNTAX_FUNCTION(PX_Syntax_define_struct_unname)
{
	px_abi* pnewabi = PX_Syntax_NewAbi(pSyntax, "struct_name", pSyntax->reg_lifetime);
	if (!pnewabi)
	{
		PX_Syntax_Terminate(pSyntax, "Memory Error");
		return PX_FALSE;
	}
	if (!PX_AbiSet_string(pnewabi, "value", PX_Syntax_AllocUnnamed(pSyntax)))
	{
		PX_Syntax_Terminate(pSyntax, "Memory Error");
		return PX_FALSE;
	}
	return PX_TRUE;
}

PX_SYNTAX_FUNCTION(PX_Syntax_define_struct_merge_name)
{
	if (!PX_Syntax_MergeLast2AbiToSecondLast(pSyntax))
	{
		PX_Syntax_Terminate(pSyntax, "Memory Error");
		return PX_FALSE;
	}
	return PX_TRUE;
}

PX_SYNTAX_FUNCTION(PX_Syntax_define_struct_merge_unnamed)
{
	px_abi* plastabi;
	plastabi = PX_Syntax_GetAbiLast(pSyntax);
	if (!plastabi)
	{
		PX_Syntax_Terminate(pSyntax, "Memory Error");
		return PX_FALSE;
	}
	if (!PX_AbiSet_string(plastabi, "struct_name.value", PX_Syntax_AllocUnnamed(pSyntax)))
	{
		PX_Syntax_Terminate(pSyntax, "Memory Error");
		return PX_FALSE;
	}
	return PX_TRUE;
}

PX_SYNTAX_FUNCTION(PX_Syntax_define_struct_merge_declare_variable)
{
	px_int* pmember_count,*pvariable_size,*pstruct_size;
	px_abi* plastabi = PX_Syntax_GetAbiLast(pSyntax);
	px_int  define_struct_index = PX_Syntax_FindAbiIndexFromBackward(pSyntax, "define_struct");
	px_abi* psecondlastabi = PX_Syntax_GetAbiByIndex(pSyntax, define_struct_index);
	
	PX_ASSERTIFX(plastabi == PX_NULL, "Error: last abi not found");
	PX_ASSERTIFX(psecondlastabi == PX_NULL, "Error: second last abi not found");
	PX_ASSERTIFX(!PX_Syntax_CheckAbiName(plastabi, "variable"), "Error: last abi is not variable");
	PX_ASSERTIFX(!PX_Syntax_CheckAbiName(psecondlastabi, "define_struct"), "Error: second last abi is not define_struct");
	pmember_count = PX_AbiGet_int(psecondlastabi, "member_count");
	PX_ASSERTIFX(!pmember_count, "Error: member count not found");
	pvariable_size = PX_AbiGet_int(plastabi, "size");
	PX_ASSERTIFX(pvariable_size == PX_NULL, "Error: last abi not found");
	pstruct_size = PX_AbiGet_int(psecondlastabi, "size");
	PX_ASSERTIFX(pstruct_size == PX_NULL, "Error: second last abi not found");

	if (!PX_Syntax_MergeLastToIndexWithName(pSyntax, define_struct_index, PX_itos(*pmember_count, 10).data))
	{
		PX_Syntax_Terminate(pSyntax, "Memory Error");
		return PX_FALSE;
	}
	define_struct_index = PX_Syntax_FindAbiIndexFromBackward(pSyntax, "define_struct");
	psecondlastabi = PX_Syntax_GetAbiByIndex(pSyntax, define_struct_index);
	if (!PX_AbiSet_int(psecondlastabi, "member_count", (*pmember_count)+1))
	{
		PX_Syntax_Terminate(pSyntax, "Memory Error");
		return PX_FALSE;
	}
	if (!PX_AbiSet_int(psecondlastabi, "size",*pstruct_size+ *pvariable_size))
	{
		PX_Syntax_Terminate(pSyntax, "Memory Error");
		return PX_FALSE;
	}
	return PX_TRUE;

}

PX_SYNTAX_FUNCTION(PX_Syntax_define_struct_enter_scope)
{
	if (!PX_Syntax_EnterScope(pSyntax))
	{
		PX_Syntax_Terminate(pSyntax, "Enter struct scope error");
		return PX_FALSE;
	}
	return PX_TRUE;
}

PX_SYNTAX_FUNCTION(PX_Syntax_define_struct_leave_scope)
{
	px_abi* newabi;
	px_abi* plastabi;
	px_int* p, struct_size;
	const px_char* pstructname;
	if (!PX_Syntax_LeaveScope(pSyntax))
	{
		PX_Syntax_Terminate(pSyntax, "Leave struct scope error");
		return PX_FALSE;
	}
	plastabi = PX_Syntax_GetAbiLast(pSyntax);
	PX_ASSERTIFX(plastabi == PX_NULL, "Error: last abi not found");
	PX_ASSERTIFX(!PX_Syntax_CheckAbiName(plastabi, "define_struct"),"define_struct expected.");
	pstructname = PX_AbiGet_string(plastabi, "struct_name.value");
	PX_ASSERTIFX(pstructname == PX_NULL, "Error: struct name not found");
	p = PX_AbiGet_int(plastabi, "size");
	PX_ASSERTIFX(p == PX_NULL, "Error: struct size not found");
	struct_size = *p;
	if (PX_NULL== (newabi=PX_Syntax_NewTypeDefine(pSyntax, pstructname, "struct", struct_size, pSyntax->reg_lifetime)))
	{
		PX_Syntax_Terminate(pSyntax, "Error: type define already exist");
		return PX_FALSE;
	}
	if (!PX_Syntax_MergeLast2AbiToLast(pSyntax))
	{
		PX_Syntax_Terminate(pSyntax, "Memory Error");
		return PX_FALSE;
	}
	
	return PX_TRUE;
}

PX_SYNTAX_FUNCTION(PX_Syntax_define_uncomplete_struct)
{
	px_int index;
	px_abi* plastabi;
	const px_char* pstructname;
	plastabi = PX_Syntax_GetAbiLast(pSyntax);
	PX_ASSERTIFX(plastabi == PX_NULL, "Error: last abi not found");
	PX_ASSERTIFX(!PX_Syntax_CheckAbiName(plastabi, "define_struct"), "define_struct expected.");
	pstructname = PX_AbiGet_string(plastabi, "struct_name.value");
	PX_ASSERTIFX(pstructname == PX_NULL, "Error: struct name not found");

	index=PX_Syntax_FindTypeDefine(pSyntax, pstructname, pSyntax->reg_lifetime);
	if (index==-1)
	{
		px_abi* newabi;
		if (PX_NULL == (newabi = PX_Syntax_NewTypeDefine(pSyntax, pstructname, "struct", 0, pSyntax->reg_lifetime)))
		{
			PX_Syntax_Terminate(pSyntax, "Memory Error");
			return PX_FALSE;
		}
		if (!PX_Syntax_MergeLast2AbiToLast(pSyntax))
		{
			PX_Syntax_Terminate(pSyntax, "Memory Error");
			return PX_FALSE;
		}
	}
	else
	{
		px_int* p, struct_size;
		px_abi* newabi;
		px_abi* pstruct_abi = PX_Syntax_GetAbiByIndex(pSyntax, index);
		PX_ASSERTIFX(pstruct_abi == PX_NULL, "Error: struct abi not found");
		PX_ASSERTIFX(!PX_Syntax_CheckAbiName(pstruct_abi, "type_define"), "Error: struct abi not found");
		p = PX_AbiGet_int(pstruct_abi, "size");
		PX_ASSERTIFX(p == PX_NULL, "Error: struct size not found");
		struct_size = *p;
		if (PX_NULL == (newabi = PX_Syntax_NewTypeDefine(pSyntax, pstructname, "struct", struct_size, pSyntax->reg_lifetime)))
		{
			PX_Syntax_Terminate(pSyntax, "Error: type define already exist");
			return PX_FALSE;
		}
		if (!PX_Syntax_CopyAbiToLast(pSyntax, index))
		{
			PX_Syntax_Terminate(pSyntax, "Memory Error");
			return PX_FALSE;
		}
	}
	return PX_TRUE;
}

PX_SYNTAX_FUNCTION(PX_Syntax_define_struct_error)
{
	PX_Syntax_Terminate(pSyntax, "Syntax Error:unexpected define_struct");
	return PX_FALSE;
}


px_bool PX_Syntax_load_define_struct(PX_Syntax* pSyntax)
{
	if (!PX_Syntax_Parse_PEBNF(pSyntax, "define_struct_list = declare_variable", PX_Syntax_define_struct_merge_declare_variable))
		return PX_FALSE;

	if (!PX_Syntax_Parse_PEBNF(pSyntax, "define_struct_list = declare_variable  ...", 0))
		return PX_FALSE;

	if (!PX_Syntax_Parse_PEBNF(pSyntax, "define_struct_list = declare_variable *", 0))
		return PX_FALSE;

	if (!PX_Syntax_Parse_PEBNF(pSyntax, "define_struct = 'struct'", PX_Syntax_define_struct_begin))
		return PX_FALSE;

	if (!PX_Syntax_Parse_PEBNF(pSyntax, "struct_name = identifier", PX_Syntax_define_struct_name))
		return PX_FALSE;

	if (!PX_Syntax_Parse_PEBNF(pSyntax, "struct_name = *", PX_Syntax_define_struct_unname))
		return PX_FALSE;

	if (!PX_Syntax_Parse_PEBNF(pSyntax, "define_struct = 'struct' struct_name", PX_Syntax_define_struct_merge_name))
		return PX_FALSE;

	if (!PX_Syntax_Parse_PEBNF(pSyntax, "define_struct = 'struct' struct_name '{' ", PX_Syntax_define_struct_enter_scope))
		return PX_FALSE;

	if (!PX_Syntax_Parse_PEBNF(pSyntax, "define_struct = 'struct' struct_name '{' define_struct_list '}' ", PX_Syntax_define_struct_leave_scope))
		return PX_FALSE;

	if (!PX_Syntax_Parse_PEBNF(pSyntax, "define_struct = 'struct' struct_name '{' *", PX_Syntax_define_struct_error))
		return PX_FALSE;

	if (!PX_Syntax_Parse_PEBNF(pSyntax, "define_struct = 'struct' struct_name '{' define_struct_list *", PX_Syntax_define_struct_error))
		return PX_FALSE;

	if (!PX_Syntax_Parse_PEBNF(pSyntax, "define_struct = 'struct' struct_name *", PX_Syntax_define_uncomplete_struct))
		return PX_FALSE;




	return PX_TRUE;
}