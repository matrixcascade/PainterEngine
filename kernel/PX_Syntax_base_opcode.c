#include "PX_Syntax_base_opcode.h"
#include "PX_Syntax_base_operate.h"



static px_bool PX_Syntax_SolveExpression(PX_Syntax* pSyntax)
{
	while (PX_TRUE)
	{
		px_abi* plast_opcode_abi = PX_NULL, * psecondlast_opcode_abi = PX_NULL, type_define_abi = {0};
		px_int lastopcode_index = 0, secondlastopcode_index = 0, operate_count = 0;
		px_int plast_opcode_abi_index = -1, psecondlast_opcode_abi_index = -1;
		
		PX_Syntax_opcode* plast_define_opcode = PX_NULL, * psecondlast_define_opcode = PX_NULL;
		px_int second_last_opcount = 0;
		px_int i;
		for (i = pSyntax->reg_abi_stack.size - 1; i >= 0; i--)
		{
			px_abi* pabi = PX_VECTORAT(px_abi, &pSyntax->reg_abi_stack, i);
			if (PX_Syntax_CheckAbiName(pabi, "opcode"))
			{
				if (!plast_opcode_abi)
				{
					plast_opcode_abi = pabi;
					plast_opcode_abi_index = i;
				}
				else if (!psecondlast_opcode_abi)
				{
					psecondlast_opcode_abi = pabi;
					psecondlast_opcode_abi_index = i;
					break;
				}
			}
		}
		if (plast_opcode_abi == PX_NULL)
		{
			return PX_TRUE;
		}
		if (psecondlast_opcode_abi == PX_NULL)
		{
			return PX_TRUE;
		}

		lastopcode_index = PX_AbiGetValue_int(plast_opcode_abi, "index");
		secondlastopcode_index = PX_AbiGetValue_int(psecondlast_opcode_abi, "index");
		
		PX_ASSERTIFX(!PX_VectorCheckIndex(&pSyntax->reg_expr_opcode_stack, lastopcode_index), "invalid opcode index");
		PX_ASSERTIFX(!PX_VectorCheckIndex(&pSyntax->reg_expr_opcode_stack, secondlastopcode_index), "invalid opcode index");

		plast_define_opcode = PX_VECTORAT(PX_Syntax_opcode, &pSyntax->reg_expr_opcode_stack, lastopcode_index);
		psecondlast_define_opcode = PX_VECTORAT(PX_Syntax_opcode, &pSyntax->reg_expr_opcode_stack, secondlastopcode_index);

		if (plast_define_opcode->precedence > psecondlast_define_opcode->precedence)
		{
			if (psecondlast_define_opcode->type == PX_SYNTAX_OPCODE_TYPE_BEGIN)
			{
				if (plast_define_opcode->type == PX_SYNTAX_OPCODE_TYPE_END)
				{
					px_int last_oprand_abi_index = PX_Syntax_GetAbiIndexFromBackward(pSyntax, "oprand");
					//check pair
					if (plast_define_opcode->pair!=psecondlast_define_opcode->opcode[0])
					{
						PX_Syntax_Terminate(pSyntax, "ast:error:invalid expression,parentheses pair mismatch");
						return PX_FALSE;
					}
					//pop opcodes
					PX_Syntax_PopOperate2(pSyntax, psecondlast_opcode_abi_index, plast_opcode_abi_index);
					continue;
				}
				else
				{
					return PX_TRUE;
				}
			}

			second_last_opcount = \
				(psecondlast_define_opcode->type == PX_SYNTAX_OPCODE_TYPE_UNARY_PREFIX || psecondlast_define_opcode->type == PX_SYNTAX_OPCODE_TYPE_UNARY_SUFFIX) ? \
				1 : 2;
			if (second_last_opcount == 1)
			{
				px_int last_oprand_index = PX_Syntax_GetAbiIndexFromBackward(pSyntax, "oprand");
				px_abi* poprand;
				const px_char* oprand_type = PX_NULL;
				px_string fallback_type;
				if (last_oprand_index == -1)
				{
					PX_Syntax_Terminate(pSyntax, "ast:error:SolveExpression invalid expression,miss operand");
					return PX_FALSE;
				}
				poprand = PX_VECTORAT(px_abi, &pSyntax->reg_abi_stack, last_oprand_index);
				oprand_type = PX_AbiGet_string(poprand, "type");
				PX_StringInitialize(pSyntax->mp, &fallback_type);
				PX_StringSet(&fallback_type, oprand_type);
				while (PX_StringLen(&fallback_type))
				{
					type_define_abi = PX_Syntax_GetTypeDefineAbiReadOnly(pSyntax, fallback_type.buffer);
					operate_count = PX_AbiGet_PayloadMemberCount(&type_define_abi, "operates");
					for (i = 0; i < operate_count; i++)
					{
						px_char operate_payload[32] = { 0 };
						px_int opcode_index;
						PX_sprintf1(operate_payload, sizeof(operate_payload), "operates.[%1].opcode_index", PX_STRINGFORMAT_INT(i));
						opcode_index = PX_AbiGetValue_int(&type_define_abi, operate_payload);
						if (opcode_index == secondlastopcode_index)
						{
							PX_Syntax_Operate_Function pfunction;
							PX_sprintf1(operate_payload, sizeof(operate_payload), "operates.[%1].function", PX_STRINGFORMAT_INT(i));
							if (PX_AbiExist_Type(&type_define_abi, operate_payload, PX_ABI_TYPE_PTR))
							{
								pfunction = (PX_Syntax_Operate_Function)PX_AbiGetValue_ptr(&type_define_abi, operate_payload);
								if (pfunction)
								{
									if (!pfunction(pSyntax, last_oprand_index, -1, psecondlast_opcode_abi_index))
									{
										PX_StringFree(&fallback_type);
										PX_Syntax_Terminate(pSyntax, "fail to operate opcode");
										return PX_FALSE;
									}
									break;
								}
							}
						}
					}
					if (i != operate_count)
						break;
					//fallback to parent type: "ix.i.32" -> "ix.i" -> "ix"
					PX_StringTrimBackwardUntil(&fallback_type, '.');
				}
				if (PX_StringLen(&fallback_type)==0)
				{
					PX_StringFree(&fallback_type);
					PX_Syntax_Terminate(pSyntax, "ast:error:unsupport unary operate for type");
					return PX_FALSE;
				}
				PX_StringFree(&fallback_type);
			}
			else if (second_last_opcount == 2)
			{
				px_int last_oprand_abi_index = -1, second_last_oprand_abi_index = -1;
				px_abi* poprand1, * poprand2;
				const px_char* oprand1_type = PX_NULL, * oprand2_type = PX_NULL;
				px_int j = 0;
				px_string fallback_type;
				for (j = pSyntax->reg_abi_stack.size - 1; j >= 0; j--)
				{
					px_abi* pabi = PX_VECTORAT(px_abi, &pSyntax->reg_abi_stack, j);
					if (PX_Syntax_CheckAbiName(pabi, "oprand"))
					{
						if (last_oprand_abi_index == -1)
						{
							last_oprand_abi_index = j;
						}
						else
						{
							second_last_oprand_abi_index = j;
							break;
						}
					}
				}
				if (last_oprand_abi_index == -1 || second_last_oprand_abi_index == -1)
				{
					PX_Syntax_Terminate(pSyntax, "ast:error:invalid expression");
					return PX_FALSE;
				}
				poprand1 = PX_VECTORAT(px_abi, &pSyntax->reg_abi_stack, second_last_oprand_abi_index);
				poprand2 = PX_VECTORAT(px_abi, &pSyntax->reg_abi_stack, last_oprand_abi_index);
				oprand1_type = PX_AbiGet_string(poprand1, "type");
				oprand2_type = PX_AbiGet_string(poprand2, "type");

				PX_StringInitialize(pSyntax->mp, &fallback_type);
				PX_StringSet(&fallback_type, oprand1_type);
				while (PX_StringLen(&fallback_type))
				{
					type_define_abi = PX_Syntax_GetTypeDefineAbiReadOnly(pSyntax, fallback_type.buffer);
					if (!PX_AbiCheck(&type_define_abi))
					{
						PX_Syntax_Terminate(pSyntax, "ast:error:invalid type");
						return PX_FALSE;
					}
					operate_count = PX_AbiGet_PayloadMemberCount(&type_define_abi, "operates");
					for (i = 0; i < operate_count; i++)
					{
						px_char operate_payload[32] = { 0 };
						px_int opcode_index;
						PX_sprintf1(operate_payload, sizeof(operate_payload), "operates.[%1].opcode_index", PX_STRINGFORMAT_INT(i));
						opcode_index = PX_AbiGetValue_int(&type_define_abi, operate_payload);
						if (opcode_index == secondlastopcode_index)
						{
							const px_char* other_type_str;
							PX_sprintf1(operate_payload, sizeof(operate_payload), "operates.[%1].other_type", PX_STRINGFORMAT_INT(i));
							other_type_str = PX_AbiGetValue_string(&type_define_abi, operate_payload);
							if (PX_Syntax_TypeMatch(oprand2_type, other_type_str))
							{
								PX_Syntax_Operate_Function pfunction;
								PX_sprintf1(operate_payload, sizeof(operate_payload), "operates.[%1].function", PX_STRINGFORMAT_INT(i));
								if (PX_AbiExist_Type(&type_define_abi, operate_payload, PX_ABI_TYPE_PTR))
								{
									pfunction = (PX_Syntax_Operate_Function)PX_AbiGetValue_ptr(&type_define_abi, operate_payload);
									if (pfunction)
									{
										if (!pfunction(pSyntax,  second_last_oprand_abi_index, last_oprand_abi_index, psecondlast_opcode_abi_index))
										{
											PX_StringFree(&fallback_type);
											PX_Syntax_Terminate(pSyntax, "fail to operate opcode");
											return PX_FALSE;
										}
										break;
									}
								}
							}
						}
					}
					if (i < operate_count)
						break;
					//fallback to parent type: "ix.i.32" -> "ix.i" -> "ix"
					PX_StringTrimBackwardUntil(&fallback_type, '.');
				}
				if (PX_StringLen(&fallback_type)==0)
				{
					PX_StringFormat1(&fallback_type, "ast:error:unsupport binary operate for type '%1'", PX_STRINGFORMAT_STRING(oprand1_type));
					PX_Syntax_Terminate(pSyntax, fallback_type.buffer);
					PX_StringFree(&fallback_type);
					return PX_FALSE;
				}
				PX_StringFree(&fallback_type);
			}
			else
			{
				PX_Syntax_Terminate(pSyntax, "ast:error:unsupport opcount");
				return PX_FALSE;
			}
		}
		else
		{
			//continue
			return PX_TRUE;
		}
	}
	return PX_TRUE;
}




px_bool PX_Syntax_ExecuteOpcode(PX_Syntax* pSyntax, px_int define_opcode_index)
{
	px_abi* opcode_abi;

	if (!PX_VectorCheckIndex(&pSyntax->reg_expr_opcode_stack, define_opcode_index))
	{
		PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_ExecuteOpcode invalid opcode index");
		return PX_FALSE;
	}
	opcode_abi = PX_Syntax_NewAbi(pSyntax, "opcode", pSyntax->reg_lifetime);
	if (!opcode_abi)
	{
		PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_ExecuteOpcode out of memory1");
		return PX_FALSE;
	}
	if (!PX_AbiSet_int(opcode_abi, "index", define_opcode_index))
	{
		PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_ExecuteOpcode out of memory2");
		return PX_FALSE;
	}
	if (!PX_Syntax_SolveExpression(pSyntax))
	{
		return PX_FALSE;
	}
	return PX_TRUE;
}


px_void PX_Syntax_PopLastOpcodeAbi(PX_Syntax* pSyntax)
{
	px_int i;
	for (i = pSyntax->reg_abi_stack.size - 1; i >= 0; i--)
	{
		px_abi* pabi = PX_VECTORAT(px_abi, &pSyntax->reg_abi_stack, i);
		if (PX_Syntax_CheckAbiName(pabi, "opcode"))
		{
			PX_Syntax_PopAbiIndex(pSyntax, i);
			return;
		}
	}
	PX_ASSERTX("Error:pop last opcode failed");
}

px_void PX_Syntax_PopSecondLastOpcodeAbi(PX_Syntax* pSyntax)
{
	px_int found = 0;
	px_int i;
	for (i = pSyntax->reg_abi_stack.size - 1; i >= 0; i--)
	{
		px_abi* pabi = PX_VECTORAT(px_abi, &pSyntax->reg_abi_stack, i);
		if (PX_Syntax_CheckAbiName(pabi, "opcode"))
		{
			found++;
			if (found == 2)
			{
				PX_Syntax_PopAbiIndex(pSyntax, i);
				return;
			}
		}
	}
	PX_ASSERTX("Error:pop second last opcode failed");
}


static px_char PX_ScriptParseGetOpLevel(const px_char* op, px_bool binary)
{
	if (PX_strlen(op) == 1)
	{
		switch (*op)
		{
		case '(':
		case ')':
		case '[':
		case ']':
		case '.':
			return 1;
		case '!':
		case '~':
			return 2;
		case '+':
		case '-':
		{
			if (binary)
				return 4;
			else
				return 2;
		}
		break;
		case '>':
			return 6;
		case '<':
			return 6;
		case '*':
			if (!binary) return 2;
		case '/':
		case '%':
			return 3;
		case '&':
			if (!binary) return 2;
			return 8;
		case '^':
			return 9;
		case '|':
			return 10;

		case '=':
			return 14;
		case ',':
			return 15;

		}
	}
	else
	{
		if (PX_strequ(op, "->"))
		{
			return 1;
		}
		if (PX_strequ(op, ">="))
		{
			return 6;
		}
		if (PX_strequ(op, "<="))
		{
			return 6;
		}
		if (PX_strequ(op, "<<"))
		{
			return 5;
		}
		if (PX_strequ(op, ">>"))
		{
			return 5;
		}
		if (PX_strequ(op, "&&"))
		{
			return 11;
		}
		if (PX_strequ(op, "||"))
		{
			return 12;
		}
		if (PX_strequ(op, "=="))
		{
			return 7;
		}
		if (PX_strequ(op, "!="))
		{
			return 7;
		}
		if (PX_strequ(op, "++"))
		{
			return 2;
		}
		if (PX_strequ(op, "--"))
		{
			return 2;
		}
	}
	PX_ASSERT();
	return -1;
}



static px_bool PX_Syntax_is_opcode_unary(struct _PX_Syntax* pSyntax, struct _PX_Syntax_ast* past, px_void* userptr, px_bool prefix)
{
	px_int i;
	px_int source_index,begin,end;

	PX_SYNTAXLEXER_LEXEME_TYPE lexeme_type = PX_Syntax_GetNextLexeme(pSyntax);
	if (lexeme_type != PX_SYNTAXLEXER_LEXEME_TYPE_DELIMITER)
	{
		return PX_FALSE;
	}

	source_index = PX_Syntax_GetCurrentLexemeBeginSourceIndex(pSyntax);
	begin = PX_Syntax_GetCurrentLexemeBegin(pSyntax);
	end = begin;
	for (i = 0; i < pSyntax->reg_expr_opcode_stack.size; i++)
	{
		PX_Syntax_opcode* popcode = PX_VECTORAT(PX_Syntax_opcode, &pSyntax->reg_expr_opcode_stack, i);
		const px_char* pcurrent_lexeme = PX_Syntax_GetCurrentLexeme(pSyntax);
		if (prefix)
		{
			if (popcode->type != PX_SYNTAX_OPCODE_TYPE_UNARY_PREFIX)
			{
				continue;
			}
		}
		else
		{
			if (popcode->type != PX_SYNTAX_OPCODE_TYPE_UNARY_SUFFIX)
			{
				continue;
			}
		}

		if (popcode->opcode[0] == pcurrent_lexeme[0])
		{
			px_bool match = PX_TRUE;
			px_int forward = 1;
			PX_SYNTAXLEXER_STATE state = PX_Syntax_GetLexerState(pSyntax);
			while (popcode->opcode[forward] != '\0')
			{
				px_char forward_char = popcode->opcode[forward];
				px_char next_char = PX_Syntax_PreviewNextChar(pSyntax);
				if (next_char != forward_char)
				{
					PX_Syntax_SetLexerState(pSyntax, &state);
					match = PX_FALSE;
					break;
				}
				end = PX_Syntax_GetCurrentLexerOffset(pSyntax);
				PX_Syntax_LexerForward(pSyntax, 1);
				forward++;
			}
			if (!match)
			{
				continue;
			}

			if (!PX_Syntax_NewMapToken(pSyntax, source_index, begin, source_index, end, PX_COLOR(255, 128, 192, 255), "unary opcode"))
			{
				PX_Syntax_Terminate(pSyntax, "runrime:error:PX_Syntax_is_opcode_unary out of memory");
				return PX_FALSE;
			}

			if (!PX_Syntax_ExecuteOpcode(pSyntax, i))
				return PX_FALSE;
			else
				return PX_TRUE;
		}
	}
	return PX_FALSE;
}

PX_SYNTAX_FUNCTION(PX_Syntax_is_opcode_unary_prefix)
{
	return PX_Syntax_is_opcode_unary(pSyntax, past, userptr, PX_TRUE);
}

PX_SYNTAX_FUNCTION(PX_Syntax_is_opcode_unary_suffix)
{
	return PX_Syntax_is_opcode_unary(pSyntax, past, userptr, PX_FALSE);
}

PX_SYNTAX_FUNCTION(PX_Syntax_execute_binary_opcode)
{
	px_int i;
	px_int source_index,begin,end;
	
	PX_SYNTAXLEXER_LEXEME_TYPE lexeme_type = PX_Syntax_GetNextLexeme(pSyntax);
	if (lexeme_type != PX_SYNTAXLEXER_LEXEME_TYPE_DELIMITER)
	{
		return PX_FALSE;
	}

	source_index = PX_Syntax_GetCurrentLexemeBeginSourceIndex(pSyntax);
	begin = PX_Syntax_GetCurrentLexemeBegin(pSyntax);
	end = begin;
	for (i = 0; i < pSyntax->reg_expr_opcode_stack.size; i++)
	{
		PX_Syntax_opcode* popcode = PX_VECTORAT(PX_Syntax_opcode, &pSyntax->reg_expr_opcode_stack, i);
		const px_char* pcurrent_lexeme = PX_Syntax_GetCurrentLexeme(pSyntax);
		if (popcode->type != PX_SYNTAX_OPCODE_TYPE_BINARY)
		{
			continue;
		}

		if (popcode->opcode[0]== pcurrent_lexeme[0])
		{
			px_bool match = PX_TRUE;
			px_int forward = 1;
			PX_SYNTAXLEXER_STATE state = PX_Syntax_GetLexerState(pSyntax);
			while (popcode->opcode[forward] != '\0')
			{
				px_char forward_char = popcode->opcode[forward];
				px_char next_char = PX_Syntax_PreviewNextChar(pSyntax);
				if (next_char!= forward_char)
				{
					PX_Syntax_SetLexerState(pSyntax, &state);
					match = PX_FALSE;
					break;
				}
				end = PX_Syntax_GetCurrentLexerOffset(pSyntax);
				PX_Syntax_LexerForward(pSyntax, 1);
				forward++;
			}
			if (!match)
			{
				continue;
			}

			if (!PX_Syntax_NewMapToken(pSyntax, source_index, begin, source_index, end, PX_COLOR(255, 128, 192, 255), "opcode"))
			{
				PX_Syntax_Terminate(pSyntax, "runrime:error:PX_Syntax_is_binary_opcode out of memory1");
				return PX_FALSE;
			}
			switch (popcode->type)
			{
			case PX_SYNTAX_OPCODE_TYPE_BINARY:
				if (!PX_Syntax_NewMapToken(pSyntax, source_index, begin, source_index, end, PX_COLOR(255, 128, 128, 255), "binary_opcode"))
				{
					PX_Syntax_Terminate(pSyntax, "runrime:error:PX_Syntax_is_binary_opcode out of memory2");
					return PX_FALSE;
				}
				break;
			case PX_SYNTAX_OPCODE_TYPE_UNARY_PREFIX:
				if (!PX_Syntax_NewMapToken(pSyntax, source_index, begin, source_index, end, PX_COLOR(128, 255, 128, 255), "unary_prefix_opcode"))
				{
					PX_Syntax_Terminate(pSyntax, "runrime:error:PX_Syntax_is_binary_opcode out of memory3");
					return PX_FALSE;
				}
				break;
			case PX_SYNTAX_OPCODE_TYPE_UNARY_SUFFIX:
				if (!PX_Syntax_NewMapToken(pSyntax, source_index, begin, source_index, end, PX_COLOR(128, 128, 255, 255), "unary_suffix_opcode"))
				{
					PX_Syntax_Terminate(pSyntax, "runrime:error:PX_Syntax_is_binary_opcode out of memory4");
					return PX_FALSE;
				}
				break;
			default:
				PX_ASSERT();
				break;
			}

			if (!PX_Syntax_ExecuteOpcode(pSyntax, i))
				return PX_FALSE;
			else
				return PX_TRUE;
		}

	}
	return PX_FALSE;
}

px_bool PX_Syntax_begin_end_opcode(struct _PX_Syntax* pSyntax, struct _PX_Syntax_ast* past, px_void* userptr, px_char begin_end_str[],px_bool isBegin)
{
	px_int i, opcode_index = -1;
	px_int source_index, begin, end;

	PX_SYNTAXLEXER_LEXEME_TYPE lexeme_type = PX_Syntax_GetNextLexeme(pSyntax);
	if (lexeme_type != PX_SYNTAXLEXER_LEXEME_TYPE_DELIMITER)
	{
		return PX_FALSE;
	}

	source_index = PX_Syntax_GetCurrentLexemeBeginSourceIndex(pSyntax);
	begin = PX_Syntax_GetCurrentLexemeBegin(pSyntax);
	end = begin;

	for (i = 0; i < pSyntax->reg_expr_opcode_stack.size; i++)
	{
		PX_Syntax_opcode* popcode = PX_VECTORAT(PX_Syntax_opcode, &pSyntax->reg_expr_opcode_stack, i);
		const px_char* pcurrent_lexeme = PX_Syntax_GetCurrentLexeme(pSyntax);
		if (isBegin)
		{
			if (popcode->type != PX_SYNTAX_OPCODE_TYPE_BEGIN)
			{
				continue;
			}
		}
		else
		{
			if (popcode->type != PX_SYNTAX_OPCODE_TYPE_END)
			{
				continue;
			}
		}

		if (popcode->opcode[0] == pcurrent_lexeme[0])
		{
			px_bool match = PX_TRUE;
			px_int forward = 1;
			PX_SYNTAXLEXER_STATE state = PX_Syntax_GetLexerState(pSyntax);
			while (popcode->opcode[forward] != '\0')
			{
				px_char forward_char = popcode->opcode[forward];
				px_char next_char = PX_Syntax_PreviewNextChar(pSyntax);
				if (next_char != forward_char)
				{
					PX_Syntax_SetLexerState(pSyntax, &state);
					match = PX_FALSE;
					break;
				}
				end = PX_Syntax_GetCurrentLexerOffset(pSyntax);
				PX_Syntax_LexerForward(pSyntax, 1);
				forward++;
			}
			if (!match)
			{
				continue;
			}

			if (isBegin)
			{
				if (!PX_Syntax_NewMapToken(pSyntax, source_index, begin, source_index, end, PX_COLOR(255, 255, 192, 255), "begin_opcode"))
				{
					PX_Syntax_Terminate(pSyntax, "runrime:error:PX_Syntax_begin_end_opcode out of memory1");
					return PX_FALSE;
				}
			}
			else
			{
				if (!PX_Syntax_NewMapToken(pSyntax, source_index, begin, source_index, end, PX_COLOR(255, 255, 192, 255), "end_opcode"))
				{
					PX_Syntax_Terminate(pSyntax, "runrime:error:PX_Syntax_begin_end_opcode out of memory2");
					return PX_FALSE;
				}
			}

			if (!PX_Syntax_ExecuteOpcode(pSyntax, i))
				return PX_FALSE;
			else
				return PX_TRUE;
		}
	}
	return PX_FALSE;
}

PX_SYNTAX_FUNCTION(PX_Syntax_parenthese_begin_opcode)
{
	return PX_Syntax_begin_end_opcode(pSyntax, past, userptr, "(",PX_TRUE);
}

PX_SYNTAX_FUNCTION(PX_Syntax_parenthese_end_opcode)
{
	return PX_Syntax_begin_end_opcode(pSyntax, past, userptr, ")", PX_FALSE);
}

PX_SYNTAX_FUNCTION(PX_Syntax_bracket_begin_opcode)
{
	return PX_Syntax_begin_end_opcode(pSyntax, past, userptr, "[", PX_TRUE);
}

PX_SYNTAX_FUNCTION(PX_Syntax_bracket_end_opcode)
{
	return PX_Syntax_begin_end_opcode(pSyntax, past, userptr, "]", PX_FALSE);
}

px_bool PX_Syntax_load_base_opcode(PX_Syntax* pSyntax)
{
	px_int index;
	PX_Syntax_opcode* popcode;
	if (PX_Syntax_NewOpcodeDefine(pSyntax, "->", PX_SYNTAX_OPCODE_TYPE_BINARY, 1) == -1)
	{
		return PX_FALSE;
	}

	if (PX_Syntax_NewOpcodeDefine(pSyntax, "{", PX_SYNTAX_OPCODE_TYPE_BEGIN, 1) == -1)
	{
		return PX_FALSE;
	}
	if ((index=PX_Syntax_NewOpcodeDefine(pSyntax, "}", PX_SYNTAX_OPCODE_TYPE_END, 99)) == -1)
	{
		return PX_FALSE;
	}
	popcode = PX_VECTORAT(PX_Syntax_opcode, &pSyntax->reg_expr_opcode_stack, index);
	popcode->pair = '{';

	if (PX_Syntax_NewOpcodeDefine(pSyntax, "(", PX_SYNTAX_OPCODE_TYPE_BEGIN, 1) == -1)
	{
		return PX_FALSE;
	}
	if ((index = PX_Syntax_NewOpcodeDefine(pSyntax, ")", PX_SYNTAX_OPCODE_TYPE_END, 99)) == -1)
	{
		return PX_FALSE;
	}
	popcode = PX_VECTORAT(PX_Syntax_opcode, &pSyntax->reg_expr_opcode_stack, index);
	popcode->pair = '(';

	if (PX_Syntax_NewOpcodeDefine(pSyntax, "[", PX_SYNTAX_OPCODE_TYPE_BEGIN, 1) == -1)
	{
		return PX_FALSE;
	}
	if ((index = PX_Syntax_NewOpcodeDefine(pSyntax, "]", PX_SYNTAX_OPCODE_TYPE_END, 99)) == -1)
	{
		return PX_FALSE;
	}
	popcode = PX_VECTORAT(PX_Syntax_opcode, &pSyntax->reg_expr_opcode_stack, index);
	popcode->pair = '[';


	if (PX_Syntax_NewOpcodeDefine(pSyntax, ".", PX_SYNTAX_OPCODE_TYPE_BINARY, 1) == -1)
	{
		return PX_FALSE;
	}
	if (PX_Syntax_NewOpcodeDefine(pSyntax, "+", PX_SYNTAX_OPCODE_TYPE_UNARY_PREFIX, 2) == -1)
	{
		return PX_FALSE;
	}
	if (PX_Syntax_NewOpcodeDefine(pSyntax, "-", PX_SYNTAX_OPCODE_TYPE_UNARY_PREFIX, 2) == -1)
	{
		return PX_FALSE;
	}
	if (PX_Syntax_NewOpcodeDefine(pSyntax, "!", PX_SYNTAX_OPCODE_TYPE_UNARY_PREFIX, 2) == -1)
	{
		return PX_FALSE;
	}
	if (PX_Syntax_NewOpcodeDefine(pSyntax, "~", PX_SYNTAX_OPCODE_TYPE_UNARY_PREFIX, 2) == -1)
	{
		return PX_FALSE;
	}

	if (PX_Syntax_NewOpcodeDefine(pSyntax, "++", PX_SYNTAX_OPCODE_TYPE_UNARY_SUFFIX, 2) == -1)
	{
		return PX_FALSE;
	}
	if (PX_Syntax_NewOpcodeDefine(pSyntax, "--", PX_SYNTAX_OPCODE_TYPE_UNARY_SUFFIX, 2) == -1)
	{
		return PX_FALSE;
	}
	if (PX_Syntax_NewOpcodeDefine(pSyntax, "*", PX_SYNTAX_OPCODE_TYPE_BINARY, 3) == -1)
	{
		return PX_FALSE;
	}
	if (PX_Syntax_NewOpcodeDefine(pSyntax, "/", PX_SYNTAX_OPCODE_TYPE_BINARY, 3) == -1)
	{
		return PX_FALSE;
	}
	if (PX_Syntax_NewOpcodeDefine(pSyntax, "%", PX_SYNTAX_OPCODE_TYPE_BINARY, 3) == -1)
	{
		return PX_FALSE;
	}
	if (PX_Syntax_NewOpcodeDefine(pSyntax, "+", PX_SYNTAX_OPCODE_TYPE_BINARY, 4) == -1)
	{
		return PX_FALSE;
	}
	if (PX_Syntax_NewOpcodeDefine(pSyntax, "-", PX_SYNTAX_OPCODE_TYPE_BINARY, 4) == -1)
	{
		return PX_FALSE;
	}
	if (PX_Syntax_NewOpcodeDefine(pSyntax, "<<", PX_SYNTAX_OPCODE_TYPE_BINARY, 5) == -1)
	{
		return PX_FALSE;
	}
	if (PX_Syntax_NewOpcodeDefine(pSyntax, ">>", PX_SYNTAX_OPCODE_TYPE_BINARY, 5) == -1)
	{
		return PX_FALSE;
	}
	if (PX_Syntax_NewOpcodeDefine(pSyntax, ">", PX_SYNTAX_OPCODE_TYPE_BINARY, 6) == -1)
	{
		return PX_FALSE;
	}
	if (PX_Syntax_NewOpcodeDefine(pSyntax, "<", PX_SYNTAX_OPCODE_TYPE_BINARY, 6) == -1)
	{
		return PX_FALSE;
	}
	if (PX_Syntax_NewOpcodeDefine(pSyntax, "==", PX_SYNTAX_OPCODE_TYPE_BINARY, 7) == -1)
	{
		return PX_FALSE;
	}
	if (PX_Syntax_NewOpcodeDefine(pSyntax, "!=", PX_SYNTAX_OPCODE_TYPE_BINARY, 7) == -1)
	{
		return PX_FALSE;
	}
	if (PX_Syntax_NewOpcodeDefine(pSyntax, "&", PX_SYNTAX_OPCODE_TYPE_BINARY, 8) == -1)
	{
		return PX_FALSE;
	}
	if (PX_Syntax_NewOpcodeDefine(pSyntax, "^", PX_SYNTAX_OPCODE_TYPE_BINARY, 9) == -1)
	{
		return PX_FALSE;
	}
	if (PX_Syntax_NewOpcodeDefine(pSyntax, "|", PX_SYNTAX_OPCODE_TYPE_BINARY, 10) == -1)
	{
		return PX_FALSE;
	}
	if (PX_Syntax_NewOpcodeDefine(pSyntax, "&&", PX_SYNTAX_OPCODE_TYPE_BINARY, 11) == -1)
	{
		return PX_FALSE;
	}
	if (PX_Syntax_NewOpcodeDefine(pSyntax, "||", PX_SYNTAX_OPCODE_TYPE_BINARY, 12) == -1)
	{
		return PX_FALSE;
	}
	if (PX_Syntax_NewOpcodeDefine(pSyntax, "=", PX_SYNTAX_OPCODE_TYPE_BINARY, 14) == -1)
	{
		return PX_FALSE;
	}
	if (PX_Syntax_NewOpcodeDefine(pSyntax, ",", PX_SYNTAX_OPCODE_TYPE_BINARY, 15) == -1)
	{
		return PX_FALSE;
	}
	if (PX_Syntax_NewOpcodeDefine(pSyntax, ":", PX_SYNTAX_OPCODE_TYPE_BINARY, 16) == -1)
	{
		return PX_FALSE;
	}
	if (PX_Syntax_NewOpcodeDefine(pSyntax, "?", PX_SYNTAX_OPCODE_TYPE_BINARY, 17) == -1)
	{
		return PX_FALSE;
	}

	if (!PX_Syntax_Parse_PEBNF(pSyntax, "opcode_unary_prefix =  *", 0, PX_Syntax_is_opcode_unary_prefix, 0))
		return PX_FALSE;

	if (!PX_Syntax_Parse_PEBNF(pSyntax, "opcode_unary_suffix =  *", 0, PX_Syntax_is_opcode_unary_suffix, 0))
		return PX_FALSE;

	if (!PX_Syntax_Parse_PEBNF(pSyntax, "opcode_binary =  *", 0, PX_Syntax_execute_binary_opcode, 0))
		return PX_FALSE;

	if (!PX_Syntax_Parse_PEBNF(pSyntax, "expr_opcode_begin =  *", 0, PX_Syntax_parenthese_begin_opcode, 0))
		return PX_FALSE;

	if (!PX_Syntax_Parse_PEBNF(pSyntax, "expr_opcode_end =  *", 0, PX_Syntax_parenthese_end_opcode, 0))
		return PX_FALSE;

	if (!PX_Syntax_Parse_PEBNF(pSyntax, "expr_opcode_index_begin =  *", 0, PX_Syntax_bracket_begin_opcode, 0))
		return PX_FALSE;

	if (!PX_Syntax_Parse_PEBNF(pSyntax, "expr_opcode_index_end =  *", 0, PX_Syntax_bracket_end_opcode, 0))
		return PX_FALSE;

	return PX_TRUE;
}
