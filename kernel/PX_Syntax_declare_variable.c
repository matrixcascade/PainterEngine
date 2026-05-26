#include "PX_Syntax_declare_variable.h"

PX_SYNTAX_FUNCTION(PX_Syntax_new_variable_token)
{
	px_int declare_prefix_index = PX_Syntax_GetAbiIndexFromBackward(pSyntax, "declare_prefix");
	px_int type_index = PX_Syntax_GetAbiIndexFromBackward(pSyntax, "type");
	px_int declare_token_prefix_index = PX_Syntax_GetAbiIndexFromBackward(pSyntax, "declare_token_prefix");
	px_int identifier_index = PX_Syntax_GetAbiIndexFromBackward(pSyntax, "identifier");
	px_int declare_array_index = PX_Syntax_GetAbiIndexFromBackward(pSyntax, "declare_array");
	px_abi* pscopeabi = PX_NULL;
	const px_char* pdeclare_prefix_value = PX_NULL;
	const px_char* pidentifier_name = PX_NULL;

	px_int identifier_source_index = -1;
	px_int identifier_begin = -1, identifier_end = -1;
	px_string type_payload;
	px_string var_payload;

	const px_char* ptype_define_type;
	const px_char* final_scope = "local";
	px_int pointer_level = 0;
	px_int array_d=0, array_count=1;
	px_int final_lifetime = pSyntax->reg_lifetime;
	px_int final_type_size = 0, final_size=0, final_offset=0;
	//type_index identifier_index should exist, but declare_prefix_index and declare_token_prefix_index may not exist
	PX_ASSERTIFX(type_index == -1, "Error: type not found");
	PX_ASSERTIFX(identifier_index == -1, "Error: identifier not found");
	PX_StringInitialize(pSyntax->mp,&type_payload);
	PX_StringInitialize(pSyntax->mp,&var_payload);

	//get scope abi
	pscopeabi = PX_Syntax_GetLastScopeAbi(pSyntax);
	if (!pscopeabi)
	{
		PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_new_variable_token No Scope Found");
		goto _ERROR;
	}

	//get identifier name first
	do
	{
		px_abi* pidentifier_abi = PX_Syntax_GetAbiByIndex(pSyntax, identifier_index);
		pidentifier_name = PX_AbiGetValue_string(pidentifier_abi, "value");
		PX_ASSERTIFX(pidentifier_name == PX_NULL, "Error: identifier value not found");
		identifier_source_index = PX_AbiGetValue_int(pidentifier_abi, "source_index");
		identifier_begin = PX_AbiGetValue_int(pidentifier_abi, "begin");
		identifier_end = PX_AbiGetValue_int(pidentifier_abi, "end");
	} while (0);

	//set identifier into scope variables
	PX_StringFormat1(&var_payload, "variables.%1.identifier", PX_STRINGFORMAT_STRING(pidentifier_name));
	if (!PX_AbiSet_string(pscopeabi, var_payload.buffer, pidentifier_name))
	{
		PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_new_variable_token Memory Error1");
		goto _ERROR;
	}

	if (declare_array_index != -1)
	{
		px_abi* pdeclare_array_abi = PX_Syntax_GetAbiByIndex(pSyntax, declare_array_index);
		PX_StringFormat1(&var_payload, "variables.%1.array", PX_STRINGFORMAT_STRING(pidentifier_name));
		if (!PX_AbiSet_Abi(pscopeabi, var_payload.buffer, pdeclare_array_abi))
		{
			PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_new_variable_token Memory Error3");
			goto _ERROR;
		}
		array_d = PX_AbiGetValue_int(pdeclare_array_abi, "d");
		array_count = PX_AbiGetValue_int(pdeclare_array_abi, "count");
		PX_StringCat(&type_payload, "array.");
	}

	if (declare_token_prefix_index != -1)
	{
		px_abi* pdeclare_token_prefix_abi = PX_Syntax_GetAbiByIndex(pSyntax, declare_token_prefix_index);
		const px_char* pdeclare_token_prefix_value = PX_AbiGetValue_string(pdeclare_token_prefix_abi, "type");
		if (PX_strequ(pdeclare_token_prefix_value,"pointer"))
		{
			pointer_level = PX_AbiGetValue_int(pdeclare_token_prefix_abi, "pointer_level");
			PX_StringFormat1(&var_payload, "variables.%1.pointer_level", PX_STRINGFORMAT_STRING(pidentifier_name));
			if (!PX_AbiSet_int(pscopeabi, var_payload.buffer, pointer_level))
			{
				PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_new_variable_token Memory Error2");
				goto _ERROR;
			}
			PX_StringCat(&type_payload, "pointer.");
		}
		else if (PX_strequ(pdeclare_token_prefix_value, "reference"))
		{
			PX_StringCat(&type_payload, "reference.");
		}
		else
		{
			PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_new_variable_token unknown declare_token_prefix value");
			goto _ERROR;
		}
	}

	//set decoration
	if (declare_prefix_index != -1)
	{
		px_abi* pdeclare_prefix_abi = PX_Syntax_GetAbiByIndex(pSyntax, declare_prefix_index);
		px_int counter = 0;
		px_char deco_payload[64] = { 0 };
		while (PX_TRUE)
		{
			const px_char* pdeclare_prefix_value;
			PX_sprintf1(deco_payload, sizeof(deco_payload), "decoration.[%1]", PX_STRINGFORMAT_INT(counter));
			pdeclare_prefix_value = PX_AbiGet_string(pdeclare_prefix_abi, deco_payload);
			if (!pdeclare_prefix_value)
			{
				break;
			}
			counter++;
			if (PX_strequ(pdeclare_prefix_value, "static"))
			{
				final_lifetime = 1;//static lifetime
			}
			PX_StringFormat2(&var_payload, "variables.%1.%2", PX_STRINGFORMAT_STRING(pidentifier_name), PX_STRINGFORMAT_STRING(deco_payload));
			if (!PX_AbiSet_string(pscopeabi, var_payload.buffer, pdeclare_prefix_value))
			{
				PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_new_variable_token Memory Error2");
				goto _ERROR;
			}
		}
	}

	//set scope
	final_scope = final_lifetime <= 1 ? "global" : "local";
	PX_StringFormat1(&var_payload, "variables.%1.scope", PX_STRINGFORMAT_STRING(pidentifier_name));
	if (!PX_AbiSet_string(pscopeabi, var_payload.buffer, final_scope))
	{
		PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_new_variable_token Memory Error4");
		goto _ERROR;
	}

	//set type
	do{
		px_abi* ptype_abi = PX_Syntax_GetAbiByIndex(pSyntax, type_index);
		ptype_define_type = PX_AbiGetValue_string(ptype_abi, "value");
		PX_StringCat(&type_payload, ptype_define_type);
		PX_StringFormat1(&var_payload, "variables.%1.type", PX_STRINGFORMAT_STRING(pidentifier_name));
		if (!PX_AbiSet_string(pscopeabi, var_payload.buffer, type_payload.buffer))
		{
			PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_new_variable_token Memory Error5");
			goto _ERROR;
		}
	} while (0);

	//set type size
	do
	{
		//is pointer
		if (pointer_level!=0)
		{
			px_int pointer_type_size = PX_Syntax_GetTypeDefineTypeSize(pSyntax, "pointer");
			PX_ASSERTIFX(pointer_type_size == 0, "Error: pointer type size is not defined");
			final_type_size = pointer_type_size;
		}
		else
		{
			final_type_size = PX_Syntax_GetTypeDefineTypeSize(pSyntax, ptype_define_type);
		}

		PX_StringFormat1(&var_payload, "variables.%1.type_size", PX_STRINGFORMAT_STRING(pidentifier_name));
		if (!PX_AbiSet_int(pscopeabi, var_payload.buffer, final_type_size))
		{
			PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_new_variable_token Memory Error5");
			goto _ERROR;
		}
	} while (0);

	//set size
	do
	{
		if (array_d != 0)
		{
			final_size = final_type_size * array_count;
		}
		else
		{
			final_size = final_type_size;
		}

		PX_StringFormat1(&var_payload, "variables.%1.size", PX_STRINGFORMAT_STRING(pidentifier_name));
		if (!PX_AbiSet_int(pscopeabi, var_payload.buffer, final_size))
		{
			PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_new_variable_token Memory Error5");
			goto _ERROR;
		}

	} while (0);

	//set offset
	final_offset = PX_Syntax_ScopeAlloc(pSyntax, final_size);
	PX_StringFormat1(&var_payload, "variables.%1.offset", PX_STRINGFORMAT_STRING(pidentifier_name));
	if (!PX_AbiSet_int(pscopeabi, var_payload.buffer, final_offset))
	{
		PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_new_variable_token Memory Error5");
		goto _ERROR;
	}

	//new token
	do
	{
		px_string info;
		if (!PX_Syntax_NewMapToken(pSyntax, identifier_source_index, identifier_begin, identifier_source_index, identifier_end, PX_Syntax_GetRandomColor(1515), "variable"))
		{
			PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_new_variable_token Memory Error6");
			goto _ERROR;
		}
		//set token info
		
		PX_StringInitialize(pSyntax->mp, &info);
		//scope type size offset 
		PX_StringCat(&info, "scope:");
		PX_StringCat(&info, final_scope);
		PX_StringCat(&info, "\ntype:");
		PX_StringCat(&info, type_payload.buffer);
		PX_StringCat(&info, "\nsize:");
		PX_StringCatInt(&info, final_size);
		PX_StringCat(&info, "\noffset:");
		PX_StringCatInt(&info, final_offset);
		if (!PX_Syntax_LastMapInfo(pSyntax, identifier_source_index, info.buffer))
		{
			PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_new_variable_token Memory Error7");
			PX_StringFree(&info);
			goto _ERROR;
		}
		PX_StringFree(&info);
	} while (0);

	if (declare_array_index!=-1)
	{
		PX_Syntax_PopAbiIndex(pSyntax, declare_array_index);
	}

	PX_Syntax_PopAbiIndex(pSyntax, identifier_index);

	if (declare_token_prefix_index!=-1)
	{
		PX_Syntax_PopAbiIndex(pSyntax, declare_token_prefix_index);
	}
	
	PX_StringFree(&type_payload);
	PX_StringFree(&var_payload);
	return PX_TRUE;
_ERROR:
	PX_StringFree(&type_payload);
	PX_StringFree(&var_payload);
	return PX_FALSE;
}



PX_SYNTAX_FUNCTION(PX_Syntax_declare_variable_end)
{
	px_int index;

	while ((index = PX_Syntax_GetAbiIndexFromBackward(pSyntax, "declare_token_prefixs")) != -1)
	{
		PX_Syntax_PopAbiIndex(pSyntax, index);
	}

	while ((index = PX_Syntax_GetAbiIndexFromBackward(pSyntax, "type")) != -1)
	{
		PX_Syntax_PopAbiIndex(pSyntax, index);
	}

	while ((index = PX_Syntax_GetAbiIndexFromBackward(pSyntax, "declare_prefix")) != -1)
	{
		PX_Syntax_PopAbiIndex(pSyntax, index);
	}

	return PX_TRUE;
}

px_bool PX_Syntax_load_declare_variable(PX_Syntax* pSyntax)
{
	// [declare_prefix] type {[declare_token_prefixs] identifier [declare_array],...}
	if (!PX_Syntax_Parse_PEBNF(pSyntax, "identifier_token", 0, PX_Syntax_new_variable_token, 0))
		return PX_FALSE;

	if (!PX_Syntax_Parse_PEBNF(pSyntax, "identifier_token =identifier", 0,0, 0))
		return PX_FALSE;

	if (!PX_Syntax_Parse_PEBNF(pSyntax, "identifier_token =identifier declare_token_suffix", 0, 0, 0))
		return PX_FALSE;

	if (!PX_Syntax_Parse_PEBNF(pSyntax, "identifier_token =identifier *", 0, 0, 0))
		return PX_FALSE;

	if (!PX_Syntax_Parse_PEBNF(pSyntax, "variable",0, 0, 0))
		return PX_FALSE;

	if (!PX_Syntax_Parse_PEBNF(pSyntax, "variable =declare_token_prefix identifier_token",0, 0, 0))
		return PX_FALSE;

	if (!PX_Syntax_Parse_PEBNF(pSyntax, "variable =identifier_token",0, 0, 0))
		return PX_FALSE;

	if (!PX_Syntax_Parse_PEBNF(pSyntax, "declare_list = variable ",0, 0, 0))
		return PX_FALSE;

	if (!PX_Syntax_Parse_PEBNF(pSyntax, "declare_list = variable ',' ... ",0, 0, 0))
		return PX_FALSE;

	if (!PX_Syntax_Parse_PEBNF(pSyntax, "declare_list = variable * ",0, 0, 0))
		return PX_FALSE;

	if (!PX_Syntax_Parse_PEBNF(pSyntax, "type_declare = define_struct",0, 0, 0))
		return PX_FALSE;

    if (!PX_Syntax_Parse_PEBNF(pSyntax, "type_declare = define_struct declare_list",0, 0, 0))
		return PX_FALSE;

	if (!PX_Syntax_Parse_PEBNF(pSyntax, "type_declare = define_struct *",0, 0, 0))
		return PX_FALSE;

	if (!PX_Syntax_Parse_PEBNF(pSyntax, "type_set = declare_prefixs type", 0, 0, 0))
		return PX_FALSE;

	if (!PX_Syntax_Parse_PEBNF(pSyntax, "type_set = type", 0, 0, 0))
		return PX_FALSE;

	if (!PX_Syntax_Parse_PEBNF(pSyntax, "type_declare", 0, PX_Syntax_declare_variable_end, 0))
		return PX_FALSE;

	if (!PX_Syntax_Parse_PEBNF(pSyntax, "type_declare = type_set declare_list",0, 0, 0))
		return PX_FALSE;

	if (!PX_Syntax_Parse_PEBNF(pSyntax, "declare_variable =type_declare ';'",0, 0, 0))
		return PX_FALSE;

	return PX_TRUE;
}
