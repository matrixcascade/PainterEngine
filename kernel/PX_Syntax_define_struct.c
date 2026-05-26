#include "PX_Syntax_define_struct.h"

static px_int unnamed_struct_count = 0;

PX_SYNTAX_FUNCTION(PX_Syntax_define_struct_identifier_to_struct_name)
{
	px_abi *pidentifier_abi;
	px_int source_index,begin,end;
	PX_ASSERTIFX(!PX_Syntax_CheckLastAbiName(pSyntax, "identifier"), "Error: struct name should be identifier");
	if (!PX_Syntax_RenameLastAbi(pSyntax, "struct_name"))
	{
		PX_Syntax_Terminate(pSyntax, "ast:error:unexpected struct name");
		return PX_FALSE;
	}
	pidentifier_abi = PX_Syntax_GetAbiLast(pSyntax);
	source_index = PX_AbiGetValue_int(pidentifier_abi, "source_index");
	begin = PX_AbiGetValue_int(pidentifier_abi, "begin");
	end = PX_AbiGetValue_int(pidentifier_abi, "end");
	if(!PX_Syntax_NewMapToken(pSyntax, source_index, begin,source_index, end, PX_Syntax_GetRandomColor(1232), "struct_name"))
	{
		PX_Syntax_Terminate(pSyntax, "ast:error:out of memory");
		return PX_FALSE;
	}
	return PX_TRUE;
}

PX_SYNTAX_FUNCTION(PX_Syntax_define_struct_enter_scope)
{
	px_int struct_name_index = PX_Syntax_GetAbiIndexFromBackward(pSyntax, "struct_name");
	if (struct_name_index == -1)
	{
		//create unnamed struct name
		px_abi *pnew_abi;
		const px_char *punnamed =PX_Syntax_AllocUnnamed(pSyntax);
		if (!punnamed)
		{
			PX_Syntax_Terminate(pSyntax, "ast:error:out of memory");
			return PX_FALSE;
		}
		pnew_abi= PX_Syntax_NewAbi(pSyntax, "struct_name", pSyntax->reg_lifetime);
		if (!pnew_abi)
		{
			PX_Syntax_Terminate(pSyntax, "ast:error:out of memory");
			return PX_FALSE;
		}
		if (!PX_AbiSet_string(pnew_abi, "value", punnamed))
		{
			PX_Syntax_Terminate(pSyntax, "ast:error:out of memory");
			return PX_FALSE;
		}
	}
	
	if (!PX_Syntax_EnterScope(pSyntax))
	{
		PX_Syntax_Terminate(pSyntax, "ast:error:enter struct scope failed");
		return PX_FALSE;
	}
	return PX_TRUE;
}

PX_SYNTAX_FUNCTION(PX_Syntax_define_struct_leave_scope)
{
	//'struct' [struct_name] '{'<---(scope) define_struct_list '}'
	px_int total_size = 0;
	px_int last_scope_index = PX_Syntax_GetAbiIndexFromBackward(pSyntax, "scope");
	px_int secondlast_scope_index = PX_Syntax_GetSecondAbiIndexFromBackward(pSyntax, "scope");
	const px_char* pstruct_name = PX_NULL;
	px_int struct_name_index = PX_Syntax_GetAbiIndexFromBackward(pSyntax, "struct_name");
	px_string struct_payload;
	px_abi *plastsecond_scope_abi;
	px_abi *plast_scope_abi;
	px_abi variables_abi;
	PX_StringInitialize(pSyntax->mp, &struct_payload);
	PX_ASSERTIFX(last_scope_index == -1, "Error: struct should be defined in a scope");
	PX_ASSERTIFX(secondlast_scope_index == -1, "Error: struct should be merge in second last scope");
	PX_ASSERTIFX(struct_name_index == -1, "Error: struct name not found");
	//get struct name
	pstruct_name = PX_AbiGetValue_string(PX_Syntax_GetAbiByIndex(pSyntax, struct_name_index), "value");

	plastsecond_scope_abi = PX_Syntax_GetAbiByIndex(pSyntax, secondlast_scope_index);
	plast_scope_abi = PX_Syntax_GetAbiByIndex(pSyntax, last_scope_index);

	//get total size from scope alloc
	total_size = PX_AbiGetValue_int(plast_scope_abi, "alloc");

	//new struct type_define abi in secondlast scope
	PX_StringFormat1(&struct_payload, "type_defines.struct.%1", PX_STRINGFORMAT_STRING(pstruct_name));
	if (PX_AbiExist_abi(plastsecond_scope_abi, struct_payload.buffer))
	{
		PX_StringFormat1(&struct_payload, "type_defines.struct.%1.size", PX_STRINGFORMAT_STRING(pstruct_name));
		if (PX_AbiGetValue_int(plastsecond_scope_abi, struct_payload.buffer) !=0)
		{
			PX_Syntax_Terminate(pSyntax, "ast:error:redefine struct");
			goto _ERROR;
		}
		//fill in size for forward declared struct
		if (!PX_AbiSet_int(plastsecond_scope_abi, struct_payload.buffer, total_size))
		{
			PX_Syntax_Terminate(pSyntax, "ast:error:PX_Syntax_define_struct_leave_scope out of memory 1");
			goto _ERROR;
		}
	}
	else
	{
		PX_StringFormat1(&struct_payload, "struct.%1", PX_STRINGFORMAT_STRING(pstruct_name));
		if(!PX_Syntax_NewTypeDefine(pSyntax, struct_payload.buffer, pstruct_name, total_size, pSyntax->reg_lifetime-1))
		{
			PX_Syntax_Terminate(pSyntax, "ast:error:PX_Syntax_define_struct_leave_scope out of memory 1");
			goto _ERROR;
		}
	}

	//merge scope variables to struct type_define members
	if (PX_AbiGet_AbiReadOnly(plast_scope_abi, &variables_abi, "variables"))
	{
		PX_StringFormat1(&struct_payload, "type_defines.struct.%1.members", PX_STRINGFORMAT_STRING(pstruct_name));
		if(!PX_AbiSet_Abi(plastsecond_scope_abi, struct_payload.buffer, &variables_abi))
		{
			PX_Syntax_Terminate(pSyntax, "ast:error:PX_Syntax_define_struct_leave_scope out of memory 2");
			goto _ERROR;
		}
	}
	//leave scope
	if (!PX_Syntax_LeaveScope(pSyntax))
	{
		PX_Syntax_Terminate(pSyntax, "ast:error:leave struct scope failed");
		goto _ERROR;
	}


	PX_StringFree(&struct_payload);
	return PX_TRUE;

_ERROR:
	PX_StringFree(&struct_payload);
	return PX_FALSE;
}

PX_SYNTAX_FUNCTION(PX_Syntax_define_struct_type)
{
	px_int struct_name_index = PX_Syntax_GetAbiIndexFromBackward(pSyntax, "struct_name");
	const px_char* pstruct_name = PX_NULL;
	px_string struct_payload;
	//check struct name

	do
	{
		px_abi* new_type = PX_Syntax_NewAbi(pSyntax, "type", pSyntax->reg_lifetime);
		if (!new_type)
		{
			PX_Syntax_Terminate(pSyntax, "ast:error:PX_Syntax_define_struct_leave_scope out of memory 3");
			return PX_FALSE;
		}
		pstruct_name = PX_AbiGetValue_string(PX_Syntax_GetAbiByIndex(pSyntax, struct_name_index), "value");
		PX_StringInitialize(pSyntax->mp, &struct_payload);
		if (!PX_StringFormat1(&struct_payload, "struct.%1", PX_STRINGFORMAT_STRING(pstruct_name)))
		{
			PX_Syntax_Terminate(pSyntax, "ast:error:PX_Syntax_define_struct_leave_scope out of memory 4");
			PX_StringFree(&struct_payload);
			return PX_FALSE;
		}
		if (!PX_AbiSet_string(new_type, "value", struct_payload.buffer))
		{
			PX_Syntax_Terminate(pSyntax, "ast:error:PX_Syntax_define_struct_leave_scope out of memory 5");
			PX_StringFree(&struct_payload);
			return PX_FALSE;
		}
		PX_StringFree(&struct_payload);
		PX_Syntax_PopAbiIndex(pSyntax, struct_name_index);
		return PX_TRUE;
	} while (0);
	
	
}

PX_SYNTAX_FUNCTION(PX_Syntax_define_struct_begin_color)
{
	px_int source_index = PX_Syntax_GetCurrentLexemeBeginSourceIndex(pSyntax);
	px_int begin = PX_Syntax_GetCurrentLexemeBegin(pSyntax);
	px_int end = PX_Syntax_GetCurrentLexemeEnd(pSyntax);
	if(!PX_Syntax_NewMapToken(pSyntax, source_index, begin, source_index, end, PX_Syntax_GetRandomColor(9991), 0))
	{
		PX_Syntax_Terminate(pSyntax, "ast:error:out of memory");
		return PX_FALSE;
	}
	return PX_TRUE;
}

PX_SYNTAX_FUNCTION(PX_Syntax_define_struct_error)
{
	PX_Syntax_Terminate(pSyntax, "ast:error:unexpected define_struct");
	return PX_FALSE;
}


px_bool PX_Syntax_load_define_struct(PX_Syntax* pSyntax)
{
	//'struct' [struct_name] '{'<---(scope) define_struct_list '}' 
	if (!PX_Syntax_Parse_PEBNF(pSyntax, "define_struct_list = declare_variable", 0,0, 0))
		return PX_FALSE;

	if (!PX_Syntax_Parse_PEBNF(pSyntax, "define_struct_list = declare_variable  ...",0, 0, 0))
		return PX_FALSE;

	if (!PX_Syntax_Parse_PEBNF(pSyntax, "define_struct_list = declare_variable *", 0,0, 0))
		return PX_FALSE;

	if (!PX_Syntax_Parse_PEBNF(pSyntax, "struct_name = identifier",0, PX_Syntax_define_struct_identifier_to_struct_name, 0))
		return PX_FALSE;

	if (!PX_Syntax_Parse_PEBNF(pSyntax, "struct_name = *",0, 0, 0))
		return PX_FALSE;

	if (!PX_Syntax_Parse_PEBNF(pSyntax, "define_struct",0, PX_Syntax_define_struct_type,0))
		return PX_FALSE;

	if (!PX_Syntax_Parse_PEBNF(pSyntax, "define_struct = 'struct'",0, PX_Syntax_define_struct_begin_color,0))
		return PX_FALSE;

	if (!PX_Syntax_Parse_PEBNF(pSyntax, "define_struct = 'struct' struct_name '{' ",0, PX_Syntax_define_struct_enter_scope, 0))
		return PX_FALSE;

	if (!PX_Syntax_Parse_PEBNF(pSyntax, "define_struct = 'struct' struct_name '{' define_struct_list '}' ", 0,PX_Syntax_define_struct_leave_scope, 0))
		return PX_FALSE;

	if (!PX_Syntax_Parse_PEBNF(pSyntax, "define_struct = 'struct' struct_name '{' *",0, PX_Syntax_define_struct_error, 0))
		return PX_FALSE;

	if (!PX_Syntax_Parse_PEBNF(pSyntax, "define_struct = 'struct' struct_name '{' define_struct_list *",0, PX_Syntax_define_struct_error, 0))
		return PX_FALSE;

	if (!PX_Syntax_Parse_PEBNF(pSyntax, "define_struct = 'struct' struct_name *",0, 0, 0))
		return PX_FALSE;


	return PX_TRUE;
}