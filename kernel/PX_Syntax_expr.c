#include "PX_Syntax_expr.h"
#include "PX_Syntax_base_operate.h"

//static px_int expr_begin = -1;
//static px_int expr_end = -1;

static px_bool PX_Syntax_FindDeclareVariableAbiReadOnly(PX_Syntax* pSyntax, const px_char name[], px_abi* pout_abi)
{
	px_int i;
	for (i = pSyntax->reg_abi_stack.size - 1; i >= 0; i--)
	{
		px_abi* pabi = PX_VECTORAT(px_abi, &pSyntax->reg_abi_stack, i);
		if (PX_Syntax_CheckAbiName(pabi, "scope"))
		{
			px_abi variables_abi;
			if (PX_AbiGet_AbiReadOnly(pabi, &variables_abi, "variables"))
			{
				if (PX_AbiExist_abi(&variables_abi, name))
				{
					*pout_abi = PX_AbiGetValue_abireadonly(&variables_abi, name);
					return PX_TRUE;
				}
			}
		}
	}
	return PX_FALSE;
}

PX_SYNTAX_FUNCTION(PX_Syntax_expr_begin)
{
	//new abi
	px_int expr_begin = PX_Syntax_GetOpcodeDefineIndex(pSyntax, "(", PX_SYNTAX_OPCODE_TYPE_BEGIN);
	px_abi* pabi = PX_Syntax_NewAbi(pSyntax, "expr", pSyntax->reg_lifetime);
	PX_ASSERTIFX(expr_begin == -1, "expr_begin missing");
	if (!pabi)
	{
		PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_expr_begin NewAbi Memory Error");
		return PX_FALSE;
	}
	
	pSyntax->reg_expr_begin_line = PX_Syntax_GetCurrentLexerLine(pSyntax);
	pSyntax->reg_expr_source_index = PX_Syntax_GetCurrentLexerIndex(pSyntax);
	
	//new opcode
	if (!PX_Syntax_ExecuteOpcode(pSyntax, expr_begin))
	{
		return PX_FALSE;
	}

	return PX_TRUE;
}

PX_SYNTAX_FUNCTION(PX_Syntax_expr_end)
{
	//check opcode stack
	px_int expr_end = PX_Syntax_GetOpcodeDefineIndex(pSyntax, ")", PX_SYNTAX_OPCODE_TYPE_END);
	if (!PX_Syntax_ExecuteOpcode(pSyntax, expr_end))
	{
		return PX_FALSE;
	}

	if (PX_Syntax_GetAbiIndexFromForward(pSyntax,"opcode")!=-1)
	{
		PX_Syntax_Terminate(pSyntax, "ast:error:invalid expression");
		return PX_FALSE;
	}
	return PX_TRUE;
}

PX_SYNTAX_FUNCTION(PX_Syntax_expr_endline)
{
	px_int begin, end, begin_source_index, end_source_index;
	px_int last_oprand_index;
	begin=PX_Syntax_GetCurrentLexemeBegin(pSyntax);
	end = PX_Syntax_GetCurrentLexemeEnd(pSyntax);
	begin_source_index = PX_Syntax_GetCurrentLexemeBeginSourceIndex(pSyntax);
	end_source_index = PX_Syntax_GetCurrentLexemeEndSourceIndex(pSyntax);
	if (begin_source_index != end_source_index)
	{
		PX_Syntax_Terminate(pSyntax, "syntax:error:unsupport cross lexeme");
		return PX_FALSE;
	}
	if (!PX_Syntax_NewMapToken(pSyntax, begin_source_index, begin, end_source_index, end, PX_COLOR(255, 192, 255, 64), ""))
	{
		PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_expr_endline NewIRToken Memory Error");
		return PX_FALSE;
	}

	last_oprand_index = PX_Syntax_GetAbiIndexFromBackward(pSyntax, "oprand");
	if (last_oprand_index==-1)
	{
		PX_ASSERTX("oprand should be exist,and must be push r0");
		return PX_FALSE;
	}
	
	if(!PX_Syntax_MapOprandToRegister(pSyntax, last_oprand_index, 0))
	{
		PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_expr_endline PX_Syntax_MapOprandToRegister Memory Error");
		return PX_FALSE;
	}
	PX_Syntax_PopAbiIndex(pSyntax, last_oprand_index);

	

	//merge expr to scope ir
	if (!PX_Syntax_MergeLast2AbiValue(pSyntax, "expr", "ir", "scope", "ir"))
	{
		PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_expr_endline MergeLast2AbiValue Memory Error");
		return PX_FALSE;
	}

	return PX_TRUE;
}


PX_SYNTAX_FUNCTION(PX_Syntax_expr_parse_identifier_oprand)
{
	px_abi* plastabi;
	px_abi ref_variable_abi_readonly;
	const px_char* type;
	px_int begin, end,offset,size,type_size, source_index;
	const px_char* scope;
	const px_char* pidentifier_value;
	plastabi = PX_Syntax_GetAbiLast(pSyntax);
	pidentifier_value = PX_AbiGetValue_string(plastabi, "value");
	PX_ASSERTIFX(!plastabi || PX_Syntax_CheckAbiName(plastabi, "identifier") == PX_FALSE, "Unknow Error");
	//find variable from backward
	//is last oprand is struct and last opcode is '.' or '->'
	px_abi* plastoprand_abi = PX_Syntax_GetAbiFromBackward(pSyntax, "oprand");
	px_abi* plastopcode_abi = PX_Syntax_GetAbiFromBackward(pSyntax, "opcode");
	if (plastopcode_abi&& plastoprand_abi)
	{
		const px_char* lastoprandtype = PX_AbiGetValue_string(plastoprand_abi, "type");
		px_int index = PX_AbiGetValue_int(plastopcode_abi, "index");
		PX_Syntax_opcode* popcode_define;
		PX_ASSERTIFX(!PX_VECTOR_IN_RANGE(&pSyntax->reg_expr_opcode_stack, index), "Opcode index out of range");
		popcode_define = PX_Syntax_GetOpcodeDefine(pSyntax, index);
		PX_ASSERTIFX(!popcode_define, "missing opcode definition");
		if ((PX_Syntax_TypeMatch(lastoprandtype,"struct")&&(PX_strequ(popcode_define->opcode,"."))|| PX_strequ(popcode_define->opcode, "->"))\
		&& popcode_define->type ==PX_SYNTAX_OPCODE_TYPE_BINARY)
		{
			px_abi* poprand;
			poprand = PX_Syntax_PushOprand(pSyntax,"identifier", PX_strlen(pidentifier_value), PX_SYNTAX_OPRAND_FROM_CONST);
			if (!poprand)
			{
				PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_expr_parse_identifier_oprand PushOprand Memory Error");
				return PX_FALSE;
			}
			if (!PX_AbiSet_string(poprand, "value", pidentifier_value))
			{
				PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_expr_parse_identifier_oprand Set_string Memory Error");
				return PX_FALSE;
			}
			return PX_TRUE;
		}
	}
	source_index = PX_AbiGetValue_int(plastabi,"source_index");
	begin = PX_AbiGetValue_int(plastabi, "begin");
	end = PX_AbiGetValue_int(plastabi, "end");
	//identifier is variable name?
	if (PX_Syntax_FindDeclareVariableAbiReadOnly(pSyntax, pidentifier_value, &ref_variable_abi_readonly))
	{
		px_abi* poprand;
		PX_SYNTAX_OPRAND_FROM from;
		px_string content;
		//mandatory variable
		scope = PX_AbiGetValue_string(&ref_variable_abi_readonly, "scope");
		offset = PX_AbiGetValue_int(&ref_variable_abi_readonly, "offset");
		size = PX_AbiGetValue_int(&ref_variable_abi_readonly, "size");
		type = PX_AbiGetValue_string(&ref_variable_abi_readonly, "type");
		type_size = PX_AbiGetValue_int(&ref_variable_abi_readonly, "type_size");

		if (PX_strequ(scope, "global"))
			from=PX_SYNTAX_OPRAND_FROM_GLOBAL;
		else if(PX_strequ(scope, "local"))
			from=PX_SYNTAX_OPRAND_FROM_LOCAL;
		else
		{
			PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_expr_parse_identifier_oprand unknown scope");
			return PX_FALSE;
		}

		poprand = PX_Syntax_PushOprand(pSyntax, type, type_size, from);
		PX_ASSERTIFX(!poprand, "PushOprand Memory Error");
		do
		{
			px_abi array_abi;
			if (PX_AbiGet_AbiReadOnly(&ref_variable_abi_readonly, &array_abi, "array"))
			{
				if (!PX_AbiSet_Abi(poprand, "array", &array_abi))
				{
					PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_expr_parse_identifier_oprand Set_Abi array Memory Error");
					return PX_FALSE;
				}
			}
		}while(0);

		if (!PX_AbiSet_int(poprand, "offset", offset))
		{
			PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_expr_parse_identifier_oprand Set_int offset Memory Error");
			return PX_FALSE;
		}

		do
		{
			px_abi decoration_abi;
			if (PX_AbiGet_AbiReadOnly(&ref_variable_abi_readonly, &decoration_abi, "decoration"))
			{
				if (!PX_AbiSet_Abi(poprand, "decoration", &decoration_abi))
				{
					PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_expr_parse_identifier_oprand Set_Abi array Memory Error");
					return PX_FALSE;
				}
			}
		} while (0);

		if (!PX_Syntax_NewMapToken(pSyntax, source_index,begin, source_index, end, PX_COLOR(255, 255, 255, 64), "variable"))
		{
			PX_Syntax_Terminate(pSyntax, "runtime:error:New IR Token Memory Error");
			return PX_FALSE;
		}
		PX_StringInitialize(pSyntax->mp, &content);
		PX_StringFormat4(&content, "[%1] scope:%2 size:%3 offset:%4",\
			PX_STRINGFORMAT_STRING(type),\
			PX_STRINGFORMAT_STRING(scope),\
			 PX_STRINGFORMAT_INT(size),\
			PX_STRINGFORMAT_INT(offset));
		if (!PX_Syntax_LastMapInfo(pSyntax, source_index, content.buffer))
		{
			PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_expr_parse_identifier_oprand WriteIR Memory Error");
			PX_StringFree(&content);
			return PX_FALSE;
		}
		PX_StringFree(&content);

		if(!PX_Syntax_LastMapToMemory(pSyntax, source_index,from,offset))
		{
			PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_expr_parse_identifier_oprand NewIRMap Memory Error");
			return PX_FALSE;
		}

		PX_Syntax_PopLastSecondAbi(pSyntax);
	}
	else
	{
		//undeclared variable
		PX_Syntax_Terminate(pSyntax, "ast:error:undeclared variable");
		return PX_FALSE;
	}
	
	return PX_TRUE;
}

PX_SYNTAX_FUNCTION(PX_Syntax_expr_const_int_oprand)
{
	px_abi* pabi = PX_Syntax_GetAbiLast(pSyntax);
	px_abi* poprand;
	px_int begin, end, source_index;
	const px_char* pvalue;
	PX_ASSERTIFX(!pabi || PX_Syntax_CheckAbiName(pabi, "const_int") == PX_FALSE, "Unknow Error");
	//new abi oprand
	pvalue = PX_AbiGet_string(pabi, "value");
	PX_ASSERTIF(pvalue == PX_NULL);
	poprand = PX_Syntax_PushOprand(pSyntax, "ix.const",PX_Syntax_GetTypeSize(pSyntax,"ix.i.32"), PX_SYNTAX_OPRAND_FROM_CONST);
	if (!poprand)
	{
		PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_expr_const_int_oprand PushOprand Memory Error");
		return PX_FALSE;
	}
	if (!PX_AbiSet_string(poprand, "value", pvalue))
	{
		PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_expr_const_int_oprand Set_string Memory Error");
		return PX_FALSE;
	}
	source_index = PX_AbiGetValue_int(pabi, "source_index");
	begin = PX_AbiGetValue_int(pabi, "begin");
	end = PX_AbiGetValue_int(pabi, "end");
	
	if (!PX_Syntax_NewMapToken(pSyntax, source_index, begin, source_index, end, PX_COLOR(255, 255, 255, 64), "ix.const"))
	{
		PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_expr_const_int_oprand NewIRToken Memory Error");
		return PX_FALSE;
	}
	PX_Syntax_PopLastSecondAbi(pSyntax);
	return PX_TRUE;

}

PX_SYNTAX_FUNCTION(PX_Syntax_expr_const_float_oprand)
{
	px_abi* pabi = PX_Syntax_GetAbiLast(pSyntax);
	px_abi* poprand;
	px_int begin, end, source_index;
	PX_ASSERTIFX(!pabi || PX_Syntax_CheckAbiName(pabi, "const_float") == PX_FALSE, "Unknow Error");
	//new abi oprand
	poprand = PX_Syntax_PushOprand(pSyntax, "fx.const",  PX_Syntax_GetTypeSize(pSyntax,"fx.f.32"), PX_SYNTAX_OPRAND_FROM_CONST);
	if (!poprand)
	{
		PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_expr_const_float_oprand PushOprand Memory Error");
		return PX_FALSE;
	}
	if (!PX_AbiSet_string(poprand, "value", PX_AbiGet_string(pabi, "value")))
	{
		PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_expr_const_float_oprand PX_AbiSet_string Memory Error");
		return PX_FALSE;
	}
	
	begin = PX_AbiGetValue_int(pabi, "begin");
	end = PX_AbiGetValue_int(pabi, "end");
	source_index = PX_AbiGetValue_int(pabi, "source_index");
	
	if (!PX_Syntax_NewMapToken(pSyntax, source_index, begin, source_index, end, PX_COLOR(255, 255, 255, 64),"fx:const"))
	{
		PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_expr_const_float_oprand NewIRToken Memory Error");
		return PX_FALSE;
	}
	//pop
	PX_Syntax_PopLastSecondAbi(pSyntax);

	return PX_TRUE;
}

PX_SYNTAX_FUNCTION(PX_Syntax_expr_error)
{
	PX_Syntax_Terminate(pSyntax, "ast:error:invalid expression");
	return PX_FALSE;
}

px_bool PX_Syntax_load_expr(PX_Syntax* pSyntax)
{
	//[expression]

	if (!PX_Syntax_Parse_PEBNF(pSyntax, "expr_oprand = identifier", 0, PX_Syntax_expr_parse_identifier_oprand, 0))
		return PX_FALSE;

	if (!PX_Syntax_Parse_PEBNF(pSyntax, "expr_oprand = const_unsigned_float", 0, PX_Syntax_expr_const_float_oprand, 0))
		return PX_FALSE;

	if (!PX_Syntax_Parse_PEBNF(pSyntax, "expr_oprand = const_unsigned_int", 0, PX_Syntax_expr_const_int_oprand, 0))
		return PX_FALSE;

	if (!PX_Syntax_Parse_PEBNF(pSyntax, "expr_oprand = function_call", 0, 0, 0))
		return PX_FALSE;

	if (!PX_Syntax_Parse_PEBNF(pSyntax, "expr_operate1 = opcode_unary_prefix expr_oprand", 0, 0, 0))
		return PX_FALSE;

	if (!PX_Syntax_Parse_PEBNF(pSyntax, "expr_operate1 = expr_oprand opcode_unary_suffix", 0, 0, 0))
		return PX_FALSE;

	if (!PX_Syntax_Parse_PEBNF(pSyntax, "expr_operate1 = expr_oprand *", 0, 0, 0))
		return PX_FALSE;

	if (!PX_Syntax_Parse_PEBNF(pSyntax, "expr_operate2 = expr_operate1 opcode_binary ...", 0, 0, 0)) //a+b
		return PX_FALSE;

	if (!PX_Syntax_Parse_PEBNF(pSyntax, "expr_operate2 = expr_operate1 *", 0, 0, 0)) //a
		return PX_FALSE;

	if (!PX_Syntax_Parse_PEBNF(pSyntax, "expr_operate2 = expr_opcode_begin expr_operate2 expr_opcode_end opcode_binary ...", 0, 0, 0))
		return PX_FALSE;

	if (!PX_Syntax_Parse_PEBNF(pSyntax, "expr_operate2 = expr_opcode_begin expr_operate2 expr_opcode_end *", 0, 0, 0))
		return PX_FALSE;

	if (!PX_Syntax_Parse_PEBNF(pSyntax, "expr", PX_Syntax_expr_begin, PX_Syntax_expr_end, 0))
		return PX_FALSE;

	if (!PX_Syntax_Parse_PEBNF(pSyntax, "expr= expr_operate2 ", 0, 0, 0))
		return PX_FALSE;

	if (!PX_Syntax_Parse_PEBNF(pSyntax, "expr_sentence = expr ';'", 0, PX_Syntax_expr_endline, 0))
		return PX_FALSE;

	

	return PX_TRUE;
}



