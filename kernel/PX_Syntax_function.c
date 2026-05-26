#include "PX_Syntax_function.h"
#include "PX_Syntax_base_operate.h"
PX_SYNTAX_FUNCTION(PX_Syntax_begin_define_function)
{
	px_abi* pabi;
	if ((pabi=PX_Syntax_NewAbi(pSyntax,"define_function",pSyntax->reg_lifetime)) ==PX_NULL)
	{
		PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_begin_define_function Memory Error1");
		return PX_FALSE;
	}
	if (!PX_Syntax_MergeLast2AbiWithNameToLast(pSyntax, "function_return_type"))
	{
		PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_begin_define_function Memory Error2");
		return PX_FALSE;
	}
	if (!PX_AbiSet_int(pabi, "param_count", 0))
	{
		PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_begin_define_function Memory Error3");
		return PX_FALSE;
	}
	return PX_TRUE;
}

PX_SYNTAX_FUNCTION(PX_Syntax_define_function_name)
{
	px_abi* pabi = PX_Syntax_GetAbiLast(pSyntax);
	if (!pabi)
	{
		return PX_FALSE;
	}
	if (!PX_Syntax_CheckAbiName(pabi,"identifier"))
	{
		return PX_FALSE;
	}

	if (!PX_Syntax_MergeLast2AbiWithNameToSecondLast(pSyntax, "function_name"))
	{
		PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_define_function_name Memory Error");
		return PX_FALSE;
	}
	return PX_TRUE;
}

PX_SYNTAX_FUNCTION(PX_Syntax_define_function_enter_param)
{
	px_int begin = PX_Syntax_GetCurrentLexemeBegin(pSyntax);
	px_int end = PX_Syntax_GetCurrentLexemeEnd(pSyntax);
	px_int begin_source_index = PX_Syntax_GetCurrentLexemeBeginSourceIndex(pSyntax);
	px_int end_source_index = PX_Syntax_GetCurrentLexemeEndSourceIndex(pSyntax);

	if (begin_source_index != end_source_index)
	{
		PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_define_function_enter_param Memory Error1");
		return PX_FALSE;
	}
	if (!PX_Syntax_EnterScope(pSyntax))
	{
		PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_define_function_enter_param Memory Error1");
		return PX_FALSE;
	}
	if(!PX_Syntax_NewMapToken(pSyntax, begin_source_index,begin, end_source_index, end, PX_COLOR(255, 255, 155, 64), "params_begin"))
	{
		PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_define_function_enter_param Memory Error2");
		return PX_FALSE;
	}
	return PX_TRUE;
}


PX_SYNTAX_FUNCTION(PX_Syntax_define_function_leave_param)
{
	px_abi* pfunctionabi;
	px_int param_count=0;

	px_int begin = PX_Syntax_GetCurrentLexemeBegin(pSyntax);
	px_int end = PX_Syntax_GetCurrentLexemeEnd(pSyntax);
	px_int scope_index = PX_Syntax_GetAbiIndexFromBackward_LifeTime(pSyntax, "scope", pSyntax->reg_lifetime);
	px_int begin_source_index = PX_Syntax_GetCurrentLexemeBeginSourceIndex(pSyntax);
	px_int end_source_index = PX_Syntax_GetCurrentLexemeEndSourceIndex(pSyntax);
	if (begin_source_index != end_source_index)
	{
		PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_define_function_leave_param Memory Error1");
		return PX_FALSE;
	}
	if (!PX_Syntax_NewMapToken(pSyntax, begin_source_index, begin, end_source_index, end, PX_COLOR(255, 255, 155, 64), "params_end"))
	{
		PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_define_function_leave_param Memory Error1");
		return PX_FALSE;
	}
	pfunctionabi = PX_Syntax_GetAbiByIndex(pSyntax, scope_index-1);
	scope_index++;
	while (scope_index < pSyntax->reg_abi_stack.size)
	{
		px_abi* pnextabi = PX_Syntax_GetAbiByIndex(pSyntax, scope_index);
		if (PX_Syntax_CheckAbiName(pnextabi, "variable"))
		{
			if (!PX_AbiSet_Abi(pfunctionabi, PX_itos(scope_index, 10).data, pnextabi))
			{
				PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_define_function_leave_param Memory Error2");
				return PX_FALSE;
			}
			param_count++;
		}
		scope_index++;
	}

	PX_AbiSet_int(pfunctionabi, "param_count", param_count);

	if (!PX_Syntax_LeaveScope(pSyntax))
	{
		PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_define_function_leave_param Memory Error3");
		return PX_FALSE;
	}
	return PX_TRUE;
}

PX_SYNTAX_FUNCTION(PX_Syntax_define_function_declare_end)
{
	px_abi* pabi = PX_Syntax_GetAbiFromBackward(pSyntax, "define_function");
	px_int begin = PX_Syntax_GetCurrentLexemeBegin(pSyntax);
	px_int end = PX_Syntax_GetCurrentLexemeEnd(pSyntax);
	px_char content[64] = { 0 };
	px_int begin_source_index = PX_Syntax_GetCurrentLexemeBeginSourceIndex(pSyntax);
	px_int end_source_index = PX_Syntax_GetCurrentLexemeEndSourceIndex(pSyntax);
	if (begin_source_index != end_source_index)
	{
		PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_define_function_declare_end Memory Error1");
		return PX_FALSE;
	}
	if (!PX_Syntax_NewMapToken(pSyntax, begin_source_index, begin, end_source_index, end, PX_COLOR(255, 255, 64, 64), "function_declare_end"))
	{
		PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_define_function_declare_end Memory Error1");
		return PX_FALSE;
	}

	begin = PX_AbiGet_int(pabi, "function_name.begin") ? PX_AbiGetValue_int(pabi, "function_name.begin") : 0;
	end = PX_AbiGet_int(pabi, "function_name.end") ? PX_AbiGetValue_int(pabi, "function_name.end") : 0;

	if (!PX_Syntax_NewMapToken(pSyntax, begin_source_index, begin, end_source_index, end, PX_COLOR(255, 199, 230, 166), "define_function"))
	{
		PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_define_function_declare_end Memory Error2");
		return PX_FALSE;
	}
	PX_sprintf1(content, sizeof(content), ";@info \"param_count:%1\"\n",
		PX_STRINGFORMAT_INT(PX_AbiGetValue_int(pabi, "param_count"))
	);
	if (!PX_Syntax_AppendString(pSyntax,"scope","ir", content))
	{
		PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_define_function_declare_end Memory Error3");
		return PX_FALSE;
	}



	return PX_TRUE;
}

PX_SYNTAX_FUNCTION(PX_Syntax_call_function_name)
{
	px_int i;
	px_abi* plast = PX_Syntax_GetAbiLast(pSyntax);
	if (!plast)
	{
		return PX_FALSE;
	}
	if (!PX_Syntax_CheckAbiName(plast, "identifier"))
	{
		return PX_FALSE;
	}
	//search function define
	for (i = 0; i < pSyntax->reg_abi_stack.size; i++)
	{
		px_abi* pabi = PX_Syntax_GetAbiByIndex(pSyntax, i);
		if (PX_Syntax_CheckAbiName(pabi, "define_function"))
		{
			if (PX_AbiGet_string(pabi, "function_name") && PX_AbiGet_string(plast, "value") &&
				PX_strequ(PX_AbiGet_string(pabi, "function_name"), PX_AbiGet_string(plast, "value")))
			{
				//found
				if (!PX_AbiSet_Abi(plast, "define_function", pabi))
				{
					PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_call_function_name Memory Error1");
					return PX_FALSE;
				}
				if (!PX_Syntax_RenameLastAbi(pSyntax, "call_function"))
				{
					PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_call_function_name Memory Error2");
					return PX_FALSE;
				}
				if(!PX_Syntax_CopyAbiToLast(pSyntax,i))
				{
					PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_call_function_name Memory Error3");
					return PX_FALSE;
				}
				if (!PX_AbiSet_int(plast,"check_param_count",0))
				{
					PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_call_function_name Memory Error4");
					return PX_FALSE;
				}
				return PX_TRUE;
			}
		}
	}
	return PX_FALSE;
}

PX_SYNTAX_FUNCTION(PX_Syntax_function_param_check)
{
	px_abi* plastoprand = PX_Syntax_GetAbiFromBackward(pSyntax, "oprand");
	px_abi* pcallfunction = PX_Syntax_GetAbiFromBackward(pSyntax, "call_function");
	const px_char* param_type;
	const px_char* oprand_type;
	px_int i;
	px_int param_pointer_level;

	px_char payload[128] = { 0 };
	px_int param_count;

	if (!plastoprand || !pcallfunction)
	{
		PX_Syntax_Terminate(pSyntax, "Unknow Error");
		return PX_FALSE;
	}
	
	param_count = PX_AbiGetValue_int(pcallfunction, "check_param_count");
	//get param type
	PX_sprintf1(payload, sizeof(payload), "define_function.%1.type_define.type", PX_STRINGFORMAT_INT(param_count));
	param_type = PX_AbiGet_string(pcallfunction, "define_function");

	oprand_type = PX_AbiGet_string(plastoprand, "type.type_define.type");

	//get param pointer_level
	PX_sprintf1(payload, sizeof(payload), "define_function.%1.pointer_level", PX_STRINGFORMAT_INT(param_count));
	param_pointer_level = PX_AbiGetValue_int(pcallfunction, payload);

	for ( i = 0; i < param_count; i++)
	{
		px_abi parem_variable; 
		const px_char* pvar_type;
		const px_char* pparam_type;
		px_abi* oprand = PX_Syntax_GetOprandStack(pSyntax, param_count);
		if (oprand==PX_NULL)
		{
			PX_Syntax_Terminate(pSyntax, "ast:error:Function param count not matched");
			return PX_FALSE;
		}
		if (!PX_AbiGet_AbiReadOnly(pcallfunction,&parem_variable, PX_itos(i, 10).data))
		{
			PX_Syntax_Terminate(pSyntax, "ast:error:Unknow Function Check error");
			return PX_FALSE;
		}
		pvar_type = PX_AbiGet_string(oprand, "type");
		pparam_type = PX_AbiGet_string(&parem_variable, "type.type_define.type");
		if (!pvar_type || !pparam_type)
		{
			PX_Syntax_Terminate(pSyntax, "ast:error:Unknow Function Check error");
			return PX_FALSE;
		}
		if (!PX_Syntax_TypeMatch(pvar_type, pparam_type))
		{
			PX_Syntax_Terminate(pSyntax, "ast:error:Function param type not matched");
			return PX_FALSE;
		}
	}

	param_count++;
	if (!PX_AbiSet_int(pcallfunction, "check_param_count", param_count))
	{
		PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_function_param_check Memory Error");
		return PX_FALSE;
	}
	
	return PX_TRUE;

}

px_bool PX_Syntax_load_function(PX_Syntax* pSyntax)
{
	if (!PX_Syntax_Parse_PEBNF(pSyntax, "define_function = type", 0, PX_Syntax_begin_define_function, 0))
		return PX_FALSE;

	if (!PX_Syntax_Parse_PEBNF(pSyntax, "define_function = type identifier", 0, PX_Syntax_define_function_name, 0))
		return PX_FALSE;

	if (!PX_Syntax_Parse_PEBNF(pSyntax, "define_function = type identifier '('", 0, PX_Syntax_define_function_enter_param, 0))
		return PX_FALSE;

	if (!PX_Syntax_Parse_PEBNF(pSyntax, "define_function_param = type variable", 0, 0, 0))
		return PX_FALSE;

	if (!PX_Syntax_Parse_PEBNF(pSyntax, "define_function_params_list = 'void'", 0, 0, 0))
		return PX_FALSE;

	if (!PX_Syntax_Parse_PEBNF(pSyntax, "define_function_params_list = define_function_param", 0, 0, 0))
		return PX_FALSE;

	if (!PX_Syntax_Parse_PEBNF(pSyntax, "define_function_params_list = define_function_param ',' ...", 0, 0, 0))
		return PX_FALSE;

	if (!PX_Syntax_Parse_PEBNF(pSyntax, "define_function_params_list = define_function_param *", 0, 0, 0))
		return PX_FALSE;

	if (!PX_Syntax_Parse_PEBNF(pSyntax, "define_function = type identifier '(' define_function_params_list ')'", 0, PX_Syntax_define_function_leave_param, 0))
		return PX_FALSE;

	if (!PX_Syntax_Parse_PEBNF(pSyntax, "define_function = type identifier '(' define_function_params_list ')' ';'", 0, PX_Syntax_define_function_declare_end, 0))
		return PX_FALSE;

	if (!PX_Syntax_Parse_PEBNF(pSyntax, "define_function = type identifier '(' ')'", 0, PX_Syntax_define_function_leave_param, 0))
		return PX_FALSE;

	if (!PX_Syntax_Parse_PEBNF(pSyntax, "define_function = type identifier '(' ')' ';'", 0, PX_Syntax_define_function_declare_end, 0))
		return PX_FALSE;

	if (!PX_Syntax_Parse_PEBNF(pSyntax, "call_function_name = identifier", 0, PX_Syntax_call_function_name, 0))
		return PX_FALSE;

	if (!PX_Syntax_Parse_PEBNF(pSyntax, "function_param_list = expr", 0, PX_Syntax_function_param_check, 0)) //push param
		return PX_FALSE;

	if (!PX_Syntax_Parse_PEBNF(pSyntax, "function_param_list = expr ',' ...", 0, 0, 0)) //push param
		return PX_FALSE;

	if (!PX_Syntax_Parse_PEBNF(pSyntax, "function_param_list = expr *", 0, 0, 0))
		return PX_FALSE;

	if (!PX_Syntax_Parse_PEBNF(pSyntax, "function_call = call_function_name '(' ')' ';'", 0, 0, 0))
		return PX_FALSE;

	return PX_TRUE;
}