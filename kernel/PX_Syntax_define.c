#include "PX_Syntax_define.h"

PX_SYNTAX_FUNCTION(PX_Syntax_Parse_define_new)
{
	px_abi* pnewabi;
	px_int begin, end, begin_source_index, end_source_index;
	pnewabi = PX_Syntax_NewAbi(pSyntax, "define", pSyntax->reg_lifetime);
	if (!pnewabi)
	{
		PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_Parse_define_new Memory Error");
		return PX_FALSE;
	}
	begin_source_index = PX_Syntax_GetCurrentLexemeBeginSourceIndex(pSyntax);
	end_source_index = PX_Syntax_GetCurrentLexemeEndSourceIndex(pSyntax);
	if (begin_source_index != end_source_index)
	{
		PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_Parse_define_new Memory Error1");
		return PX_FALSE;
	}
	begin = PX_Syntax_GetCurrentLexemeBegin(pSyntax);
	end = PX_Syntax_GetCurrentLexemeEnd(pSyntax);

	PX_Syntax_NewMapToken(pSyntax, begin_source_index, begin, end_source_index, end, PX_COLOR(255, 211, 122, 255), "define");

	return PX_TRUE;
}

PX_SYNTAX_FUNCTION(PX_Syntax_Parse_define_parameters_begin)
{
	px_abi* pnewabi;
	pnewabi = PX_Syntax_NewAbi(pSyntax, "define_parameters", pSyntax->reg_lifetime);
	if (!pnewabi)
	{
		PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_Parse_define_parameters_begin Memory Error1");
		return PX_FALSE;
	}
	if(!PX_AbiSet_int(pnewabi, "count", 0))
	{
		PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_Parse_define_parameters_begin Memory Error2");
		return PX_FALSE;
	}
	return PX_TRUE;

}

PX_SYNTAX_FUNCTION(PX_Syntax_Parse_define_parameters_new)
{
	px_abi* plastabi = PX_Syntax_GetAbiLast(pSyntax);
	px_abi* psecondlastabi = PX_Syntax_GetAbiSecondLast(pSyntax);
	const px_char* pname;
	px_int* pcount,count;
	px_char content[32] = {0};
	if (!plastabi||!psecondlastabi)
	{
		return PX_FALSE;
	}
	pname = PX_AbiGet_string(plastabi, "name");
	if (!pname||!PX_strequ(pname,"identifier"))
	{
		return PX_FALSE;
	}
	pcount = PX_AbiGet_int(psecondlastabi, "count");
	if (!pcount)
	{
		return PX_FALSE;
	}
	count = *pcount;
	PX_AbiSet_int(psecondlastabi, "count", count + 1);
	PX_sprintf1(content, sizeof(content), "identifier[%1]", PX_STRINGFORMAT_INT(count));

	if(!PX_Syntax_MergeLast2AbiWithNameToSecondLast(pSyntax, content))
	{
		PX_ASSERTX("Merge Error");
		return PX_FALSE;
	}
	return PX_TRUE;
}

PX_SYNTAX_FUNCTION(PX_Syntax_Parse_define_parameters_end)
{
	return PX_TRUE;
}

PX_SYNTAX_FUNCTION(PX_Syntax_Parse_define_parameters_error)
{
	PX_Syntax_Terminate(pSyntax, "ast:error:unexpected define_parameters");
	return PX_FALSE;
}

px_bool PX_Syntax_load_define_parameters(PX_Syntax* pSyntax)
{
	if (!PX_Syntax_Parse_PEBNF(pSyntax, "define_parameters_list = identifier",0, PX_Syntax_Parse_define_parameters_new, 0))
		return PX_FALSE;

	if (!PX_Syntax_Parse_PEBNF(pSyntax, "define_parameters_list = identifier ',' define_parameters_list",0,0, 0))
		return PX_FALSE;

	if (!PX_Syntax_Parse_PEBNF(pSyntax, "define_parameters_list = identifier ',' *",0, PX_Syntax_Parse_define_parameters_error, 0))
		return PX_FALSE;

	if (!PX_Syntax_Parse_PEBNF(pSyntax, "define_parameters_list = identifier *",0, PX_Syntax_Parse_define_parameters_end, 0))
		return PX_FALSE;

	if (!PX_Syntax_Parse_PEBNF(pSyntax, "define_parameters = '('",0, PX_Syntax_Parse_define_parameters_begin, 0))
		return PX_FALSE;

	if (!PX_Syntax_Parse_PEBNF(pSyntax, "define_parameters = '(' ')'",0, 0, 0))
		return PX_FALSE;

	if (!PX_Syntax_Parse_PEBNF(pSyntax, "define_parameters = '(' define_parameters_list ')'",0, 0, 0))
		return PX_FALSE;
	
	return PX_TRUE;
}

PX_SYNTAX_FUNCTION(PX_Syntax_Parse_define_identifier)
{
	px_abi* pnewabi;
	px_int begin, end, begin_source_index, end_source_index;
	PX_SYNTAXLEXER_LEXEME_TYPE type = PX_Syntax_GetNextLexeme(pSyntax);
	if (type!=PX_SYNTAXLEXER_LEXEME_TYPE_TOKEN)
	{
		PX_Syntax_Terminate(pSyntax, "ast:error:unexpected define_identifier");
		return PX_FALSE;
	}
	else
	{
		begin = PX_Syntax_GetCurrentLexemeBegin(pSyntax);
		end = PX_Syntax_GetCurrentLexemeEnd(pSyntax);
		begin_source_index = PX_Syntax_GetCurrentLexemeBeginSourceIndex(pSyntax);
		end_source_index = PX_Syntax_GetCurrentLexemeEndSourceIndex(pSyntax);
		if (begin_source_index != end_source_index)
		{
			PX_Syntax_Terminate(pSyntax, "runtime:error:unsupport cross lexeme");
			return PX_FALSE;
		}
		pnewabi = PX_Syntax_NewAbi(pSyntax, "define_identifier", pSyntax->reg_lifetime);
		if (!pnewabi)
		{
			PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_Parse_call_define_identifier Memory Error1");
			return PX_FALSE;
		}
		if (!PX_AbiSet_string(pnewabi,"value",PX_Syntax_GetCurrentLexeme(pSyntax)))
		{
			PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_Parse_call_define_identifier Memory Error2");
			return PX_FALSE;
		}

		PX_Syntax_NewMapToken(pSyntax, begin_source_index,begin, end_source_index, end, PX_COLOR(255, 32, 242, 132), "define_identifier");

	}

	return PX_TRUE;
}

PX_SYNTAX_FUNCTION(PX_Syntax_Parse_define_identifier_check)
{
	px_int i;
	px_abi* plastabi = PX_Syntax_GetAbiLast(pSyntax);
	const px_char * pcurrent_define_name;
	if (!plastabi)
	{
		return PX_FALSE;
	}
	
	if (!PX_Syntax_CheckAbiName(plastabi, "define_identifier"))
	{
		PX_Syntax_Terminate(pSyntax, "ast:error:unexpected define_identifier");
		return PX_FALSE;
	}
	pcurrent_define_name = PX_AbiGetValue_string(plastabi, "value");
	for (i = 0; i < PX_Syntax_GetAbiCount(pSyntax); i++)
	{
		px_abi* pabi = PX_Syntax_GetAbiByIndex(pSyntax, i);
		if (PX_Syntax_CheckAbiName(pabi, "define"))
		{
			const px_char* pdefine_name = PX_AbiGet_string(pabi, "define_identifier.value");
			if (pdefine_name&&PX_strequ(pdefine_name, pcurrent_define_name))
			{
				PX_Syntax_Terminate(pSyntax, "ast:error:define_identifier already exist");
				return PX_FALSE;
			}
		}
	}
	if (!PX_Syntax_MergeLast2AbiToSecondLast(pSyntax))
	{
		return PX_FALSE;
	}

	return PX_TRUE;
}

PX_SYNTAX_FUNCTION(PX_Syntax_Parse_define_parameters_merge)
{
	px_abi* plastabi = PX_Syntax_GetAbiLast(pSyntax);
	px_abi* psencondlastabi = PX_Syntax_GetAbiSecondLast(pSyntax);
	if (!plastabi)
	{
		return PX_FALSE;
	}

	if (!PX_Syntax_CheckAbiName(plastabi, "define_parameters"))
	{
		PX_Syntax_Terminate(pSyntax, "ast:error:unexpected define_identifier");
		return PX_FALSE;
	}

	if (!PX_Syntax_CheckAbiName(psencondlastabi, "define"))
	{
		PX_Syntax_Terminate(pSyntax, "ast:error:unexpected define_identifier");
		return PX_FALSE;
	}
	
	if (!PX_Syntax_MergeLast2AbiToSecondLast(pSyntax))
	{
		return PX_FALSE;
	}

	return PX_TRUE;
}

PX_SYNTAX_FUNCTION(PX_Syntax_Parse_define_content)
{
	PX_SYNTAXLEXER_LEXEME_TYPE type = PX_Syntax_GetNextLexeme4(pSyntax);
	px_string str;
	px_int begin=-1, end=-1;
	if (type == PX_SYNTAXLEXER_LEXEME_TYPE_END || type == PX_SYNTAXLEXER_LEXEME_TYPE_NEWLINE)
	{
		px_abi* plastabi = PX_Syntax_GetAbiLast(pSyntax);
		if (!PX_AbiSet_string(plastabi, "define_content", ""))
		{
			PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_Parse_define_content Memory Error1");
			return PX_FALSE;
		}
		return PX_TRUE;
	}
	else
	{
		px_int paramcount, * pcount;
		
		const px_char* define_identifier ;
		px_abi* plastdefineabi = PX_Syntax_GetAbiLast(pSyntax);
		if (!PX_Syntax_CheckAbiName(plastdefineabi,"define"))
		{
			PX_ASSERTX("define not found");
			return PX_FALSE;
		}

		define_identifier = PX_AbiGetValue_string(plastdefineabi, "define_identifier.value");
		if (type != PX_SYNTAXLEXER_LEXEME_TYPE_SPACER)
		{
			PX_Syntax_Terminate(pSyntax, "Syntax Error:spacer expected");
			return PX_FALSE;
		}
		if (!PX_StringInitialize(pSyntax->mp, &str))
		{
			PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_Parse_define_content Memory Error2");
			return PX_FALSE;
		}

		pcount = PX_AbiGet_int(plastdefineabi, "define_parameters.count");
		if (pcount)
			paramcount = *pcount;
		else
			paramcount = 0;

		while (PX_TRUE)
		{
			PX_SYNTAXLEXER_STATE state = PX_Syntax_GetLexerState(pSyntax);
			type = PX_Syntax_GetNextLexeme4(pSyntax);

			if (type == PX_SYNTAXLEXER_LEXEME_TYPE_END || type == PX_SYNTAXLEXER_LEXEME_TYPE_NEWLINE)
			{
				PX_Syntax_SetLexerState(pSyntax,&state);
				break;
			}
			if (begin==-1)
			{
				begin = PX_Syntax_GetCurrentLexemeBegin(pSyntax);
			}

			if (type == PX_SYNTAXLEXER_LEXEME_TYPE_TOKEN)
			{
				px_int i;
				for (i = 0; i < paramcount; i++)
				{
					px_char payload[128] = { 0 };
					const px_char* pvalue;
					PX_sprintf1(payload, sizeof(payload), "define_parameters.identifier[%1].value", PX_STRINGFORMAT_INT(i));
					pvalue = PX_AbiGetValue_string(plastdefineabi, payload);
					if (!pvalue)
					{
						PX_ASSERTX("value not found");
						return PX_FALSE;
					}
					if (PX_strequ(pvalue, PX_Syntax_GetCurrentLexeme(pSyntax)))
					{
						if (!PX_StringCatChar(&str, '%') || !PX_StringCat(&str, PX_itos(i+1, 10).data))
						{
							PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_Parse_define_content Memory Error3");
							return PX_FALSE;
						}
						break;
					}
				}
				if (paramcount==0||i== paramcount)
				{
					if (!PX_StringCat(&str, PX_Syntax_GetCurrentLexeme(pSyntax)))
					{
						PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_Parse_define_content Memory Error4");
						return PX_FALSE;
					}
				}
				end = PX_Syntax_GetCurrentLexemeEnd(pSyntax);
			}
			else if (type == PX_SYNTAXLEXER_LEXEME_TYPE_DELIMITER)
			{
				if (PX_strequ(PX_Syntax_GetCurrentLexeme(pSyntax), "#"))
				{
					if (PX_Syntax_PreviewNextChar(pSyntax)=='#')
					{
						PX_Syntax_GetNextChar(pSyntax);
					}
					else
					{
						if (!PX_StringCat(&str, PX_Syntax_GetCurrentLexeme(pSyntax)))
						{
							PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_Parse_define_content Memory Error5");
							return PX_FALSE;
						}
					}
				}
				else
				{
					if (!PX_StringCat(&str, PX_Syntax_GetCurrentLexeme(pSyntax)))
					{
						PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_Parse_define_content Memory Error6");
						return PX_FALSE;
					}
				}
			}
			else
			{
				if (!PX_StringCat(&str, PX_Syntax_GetCurrentLexeme(pSyntax)))
				{
					PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_Parse_define_content Memory Error7");
					return PX_FALSE;
				}
				end = PX_Syntax_GetCurrentLexemeEnd(pSyntax);
			}
		}
		
		if (str.buffer[0])
		{
			px_int begin_source_index = PX_Syntax_GetCurrentLexemeBeginSourceIndex(pSyntax);
			px_int end_source_index = PX_Syntax_GetCurrentLexemeEndSourceIndex(pSyntax);
			if (begin_source_index != end_source_index)
			{
				PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_Parse_define_content Memory Error1");
				return PX_FALSE;
			}
			PX_Syntax_NewMapToken(pSyntax, begin_source_index,begin, end_source_index, end, PX_COLOR(255, 162, 212, 166), "define_content");
		}

		if (!PX_AbiSet_string(plastdefineabi, "format", str.buffer))
		{
			PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_Parse_define_content Memory Error8");
			PX_StringFree(&str);
			return PX_FALSE;
		}

	
		PX_StringFree(&str);
	}
	return PX_TRUE;
}

PX_SYNTAX_FUNCTION(PX_Syntax_Parse_call_define_identifier)
{
	px_int i, begin_source_index, end_source_index;
	px_int begin, end;
	const px_char* pcurrent_define_name;
	PX_SYNTAXLEXER_STATE state = PX_Syntax_GetLexerState(pSyntax);
	//read next token
	PX_SYNTAXLEXER_LEXEME_TYPE type = PX_Syntax_GetNextLexeme(pSyntax);
	
	if (type!= PX_SYNTAXLEXER_LEXEME_TYPE_TOKEN)
	{
		PX_Syntax_SetLexerState(pSyntax, &state);
		return PX_FALSE;
	}
	pcurrent_define_name = PX_Syntax_GetCurrentLexeme(pSyntax);
	for (i = 0; i < PX_Syntax_GetAbiCount(pSyntax); i++)
	{
		px_abi* pabi = PX_Syntax_GetAbiByIndex(pSyntax, i);
		if (PX_Syntax_CheckAbiName(pabi, "define"))
		{
			const px_char* pdefine_name = PX_AbiGetValue_string(pabi, "define_identifier.value");
			if (pdefine_name && PX_strequ(pdefine_name, pcurrent_define_name))
			{
				px_char macro_id[32] = { 0 };
				begin_source_index = PX_Syntax_GetCurrentLexemeBeginSourceIndex(pSyntax);
				begin = PX_Syntax_GetCurrentLexemeBegin(pSyntax);
				end_source_index = PX_Syntax_GetCurrentLexemeEndSourceIndex(pSyntax);
				end = PX_Syntax_GetCurrentLexemeEnd(pSyntax);
				px_abi* newabi;
				if (begin_source_index== end_source_index)
				{
					if (!PX_Syntax_NewMapToken(pSyntax, begin_source_index, begin, end_source_index, end, PX_COLOR(255, 32, 242, 132), "call_define_identifier"))
					{
						return PX_FALSE;
					}
				}
				

				if (PX_NULL== (newabi=PX_Syntax_NewAbi(pSyntax,"call_define_identifier",pSyntax->reg_lifetime)))
				{
					return PX_FALSE;
				}

				if (!PX_AbiSet_string(newabi, "value", pcurrent_define_name))
				{
					PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_Parse_call_define_identifier Memory Error1");
					return PX_FALSE;
				}


				PX_sprintf3(macro_id, sizeof(macro_id), "macro_%1_%2_%3", PX_STRINGFORMAT_INT(begin_source_index), PX_STRINGFORMAT_INT(begin), PX_STRINGFORMAT_INT(end));
				
				if (!PX_AbiSet_string(newabi, "macro_id", macro_id))
				{
					PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_Parse_call_define_identifier Memory Error2");
					return PX_FALSE;
				}
				return PX_TRUE;
			}
		}
	}
	return PX_FALSE;
}

PX_SYNTAX_FUNCTION(PX_Syntax_Parse_call_define)
{
	px_abi* pdefine_identifier_abi = PX_Syntax_GetAbiFromBackward(pSyntax, "call_define_identifier");
	px_abi* pexpand_parameters = PX_Syntax_GetAbiFromBackward(pSyntax, "expand_parameters");
	px_abi* pdefine_abi=PX_NULL;
	const px_char* call_macro_id;
	px_int i,parameters_count;
	PX_ASSERTIFX(!pdefine_identifier_abi, "ast:error:call_define_identifier not found");

	for (i = 0; i < PX_Syntax_GetAbiCount(pSyntax); i++)
	{
		px_abi* pabi = PX_Syntax_GetAbiByIndex(pSyntax, i);
		if (PX_Syntax_CheckAbiName(pabi, "define"))
		{
			const px_char* pdefine_name = PX_AbiGetValue_string(pabi, "define_identifier.value");
			if (pdefine_name && PX_strequ(pdefine_name, PX_AbiGetValue_string(pdefine_identifier_abi, "value")))
			{
				PX_Syntax_Message(pSyntax, "call define:");
				PX_Syntax_Message(pSyntax, pdefine_name);
				PX_Syntax_Message(pSyntax, "\n");
				pdefine_abi = pabi;
				break;
			}
		}
	}

	if (!pdefine_abi)
	{
		PX_Syntax_Terminate(pSyntax, "ast:error:define not found");
		return PX_FALSE;
	}
	
	do
	{
		const px_char* pformat = PX_AbiGetValue_string(pdefine_abi, "format");
		if (pformat)
		{
			px_int expand_parameters_count=0;
			px_string expand_source;
			if (PX_AbiExist_Type(pdefine_abi, "define_parameters.count",PX_ABI_TYPE_INT))
			{
				parameters_count = PX_AbiGetValue_int(pdefine_abi, "define_parameters.count");
			}
			else
			{
				parameters_count = 0;
			}

			if (pexpand_parameters)
			{
				expand_parameters_count = PX_AbiGetValue_int(pexpand_parameters, "count");
			}
			
			call_macro_id = PX_AbiGetValue_string(pdefine_identifier_abi, "macro_id");
			if (parameters_count != expand_parameters_count)
			{
				PX_Syntax_Terminate(pSyntax, "ast:error:expand_parameters count error");
				return PX_FALSE;
			}

			if (!PX_StringInitialize(pSyntax->mp, &expand_source))
			{
				PX_Syntax_Terminate(pSyntax, "runtime:error:out of memory");
				return PX_FALSE;
			}

			switch (parameters_count)
			{
			case 0:
				PX_StringAppend(&expand_source, pformat);
				break;
			case 1:
				PX_StringFormat1(&expand_source, pformat, \
					PX_STRINGFORMAT_STRING(PX_AbiGetValue_string(pexpand_parameters, "expand_parameter[0].value"))\
				);
				break;
			case 2:
				PX_StringFormat2(&expand_source, pformat, \
					PX_STRINGFORMAT_STRING(PX_AbiGetValue_string(pexpand_parameters, "expand_parameter[0].value")), \
					PX_STRINGFORMAT_STRING(PX_AbiGetValue_string(pexpand_parameters, "expand_parameter[1].value"))\
				);
				break;
			case 3:
				PX_StringFormat3(&expand_source, pformat, \
					PX_STRINGFORMAT_STRING(PX_AbiGetValue_string(pexpand_parameters, "expand_parameter[0].value")), \
					PX_STRINGFORMAT_STRING(PX_AbiGetValue_string(pexpand_parameters, "expand_parameter[1].value")), \
					PX_STRINGFORMAT_STRING(PX_AbiGetValue_string(pexpand_parameters, "expand_parameter[2].value"))\
				);
				break;
			case 4:
				PX_StringFormat4(&expand_source, pformat, \
					PX_STRINGFORMAT_STRING(PX_AbiGetValue_string(pexpand_parameters, "expand_parameter[0].value")), \
					PX_STRINGFORMAT_STRING(PX_AbiGetValue_string(pexpand_parameters, "expand_parameter[1].value")), \
					PX_STRINGFORMAT_STRING(PX_AbiGetValue_string(pexpand_parameters, "expand_parameter[2].value")), \
					PX_STRINGFORMAT_STRING(PX_AbiGetValue_string(pexpand_parameters, "expand_parameter[3].value"))\
				);
				break;
			case 5:
				PX_StringFormat5(&expand_source, pformat, \
					PX_STRINGFORMAT_STRING(PX_AbiGetValue_string(pexpand_parameters, "expand_parameter[0].value")), \
					PX_STRINGFORMAT_STRING(PX_AbiGetValue_string(pexpand_parameters, "expand_parameter[1].value")), \
					PX_STRINGFORMAT_STRING(PX_AbiGetValue_string(pexpand_parameters, "expand_parameter[2].value")), \
					PX_STRINGFORMAT_STRING(PX_AbiGetValue_string(pexpand_parameters, "expand_parameter[3].value")), \
					PX_STRINGFORMAT_STRING(PX_AbiGetValue_string(pexpand_parameters, "expand_parameter[4].value"))\
				);
				break;
			case 6:
				PX_StringFormat6(&expand_source, pformat, \
					PX_STRINGFORMAT_STRING(PX_AbiGetValue_string(pexpand_parameters, "expand_parameter[0].value")), \
					PX_STRINGFORMAT_STRING(PX_AbiGetValue_string(pexpand_parameters, "expand_parameter[1].value")), \
					PX_STRINGFORMAT_STRING(PX_AbiGetValue_string(pexpand_parameters, "expand_parameter[2].value")), \
					PX_STRINGFORMAT_STRING(PX_AbiGetValue_string(pexpand_parameters, "expand_parameter[3].value")), \
					PX_STRINGFORMAT_STRING(PX_AbiGetValue_string(pexpand_parameters, "expand_parameter[4].value")), \
					PX_STRINGFORMAT_STRING(PX_AbiGetValue_string(pexpand_parameters, "expand_parameter[5].value"))\
				);
				break;
			case 7:
				PX_StringFormat7(&expand_source, pformat, \
					PX_STRINGFORMAT_STRING(PX_AbiGetValue_string(pexpand_parameters, "expand_parameter[0].value")), \
					PX_STRINGFORMAT_STRING(PX_AbiGetValue_string(pexpand_parameters, "expand_parameter[1].value")), \
					PX_STRINGFORMAT_STRING(PX_AbiGetValue_string(pexpand_parameters, "expand_parameter[2].value")), \
					PX_STRINGFORMAT_STRING(PX_AbiGetValue_string(pexpand_parameters, "expand_parameter[3].value")), \
					PX_STRINGFORMAT_STRING(PX_AbiGetValue_string(pexpand_parameters, "expand_parameter[4].value")), \
					PX_STRINGFORMAT_STRING(PX_AbiGetValue_string(pexpand_parameters, "expand_parameter[5].value")), \
					PX_STRINGFORMAT_STRING(PX_AbiGetValue_string(pexpand_parameters, "expand_parameter[6].value"))\
				);
				break;
			case 8:
				PX_StringFormat8(&expand_source, pformat, \
					PX_STRINGFORMAT_STRING(PX_AbiGetValue_string(pexpand_parameters, "expand_parameter[0].value")), \
					PX_STRINGFORMAT_STRING(PX_AbiGetValue_string(pexpand_parameters, "expand_parameter[1].value")), \
					PX_STRINGFORMAT_STRING(PX_AbiGetValue_string(pexpand_parameters, "expand_parameter[2].value")), \
					PX_STRINGFORMAT_STRING(PX_AbiGetValue_string(pexpand_parameters, "expand_parameter[3].value")), \
					PX_STRINGFORMAT_STRING(PX_AbiGetValue_string(pexpand_parameters, "expand_parameter[4].value")), \
					PX_STRINGFORMAT_STRING(PX_AbiGetValue_string(pexpand_parameters, "expand_parameter[5].value")), \
					PX_STRINGFORMAT_STRING(PX_AbiGetValue_string(pexpand_parameters, "expand_parameter[6].value")), \
					PX_STRINGFORMAT_STRING(PX_AbiGetValue_string(pexpand_parameters, "expand_parameter[7].value"))\
				);
				break;
			default:
				PX_StringFree(&expand_source);
				PX_Syntax_Terminate(pSyntax, "ast:error:expand_parameters count too large");
				return PX_FALSE;
			}

			if (!PX_Syntax_AddSource(pSyntax, call_macro_id, expand_source.buffer))
			{
				PX_StringFree(&expand_source);
				PX_Syntax_Terminate(pSyntax, "ast:error:expand add source error");
				return PX_FALSE;
			}

			if (!PX_Syntax_CallSource(pSyntax, call_macro_id,PX_NULL))
			{
				PX_StringFree(&expand_source);
				PX_Syntax_Terminate(pSyntax, "ast:error:expand lexer error");
				return PX_FALSE;
			}
			PX_StringFree(&expand_source);
		}
		else
		{
			PX_Syntax_Terminate(pSyntax, "ast:error:define format not found");
			return PX_FALSE;
		}
	} while (0);

	if(pexpand_parameters)
		PX_Syntax_PopAbi(pSyntax);

	if(pdefine_identifier_abi)
		PX_Syntax_PopAbi(pSyntax);

	return PX_TRUE;
}



PX_SYNTAX_FUNCTION(PX_Syntax_Parse_expand_parameter)
{
	px_string str;
	px_int bracket = 0;
	px_bool bstring = PX_FALSE;
	px_abi* newabi;
	if (!PX_StringInitialize(pSyntax->mp, &str))
	{
		PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_Parse_expand_parameter out of memory");
		return PX_FALSE;
	}
	while (PX_TRUE)
	{
		PX_SYNTAXLEXER_STATE lexerstate = PX_SyntaxLexer_GetState(&pSyntax->reg_syntaxlexer);
		px_char ch = PX_SyntaxLexer_GetNextChar(&pSyntax->reg_syntaxlexer);
		if (ch == '\0')
		{
			PX_Syntax_Terminate(pSyntax, "ast:error:unexpected end of file");
			PX_StringFree(&str);
			return PX_FALSE;
		}
		if (ch == ',' && bracket == 0 && !bstring)
		{
			PX_SyntaxLexer_SetState(&pSyntax->reg_syntaxlexer, &lexerstate);
			break;
		}
		else if (ch == '(')
		{
			bracket++;
		}
		else if (ch == '"')
		{
			bstring = !bstring;
		}
		else if (ch == ')')
		{
			if (bracket == 0)
			{
				PX_SyntaxLexer_SetState(&pSyntax->reg_syntaxlexer, &lexerstate);
				break;
			}
			bracket--;
		}

		if (!PX_StringCatChar(&str, ch))
		{
			PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_Parse_expand_parameter Memory Error1");
			PX_StringFree(&str);
			return PX_FALSE;
		}
	}
	newabi = PX_Syntax_NewAbi(pSyntax, "expand_parameter", pSyntax->reg_lifetime);
	if (!newabi)
	{
		PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_Parse_expand_parameter Memory Error2");
		PX_StringFree(&str);
		return PX_FALSE;
	}
	if (!PX_AbiSet_string(newabi, "value", str.buffer))
	{
		PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_Parse_expand_parameter Memory Error3");
		PX_StringFree(&str);
		return PX_FALSE;
	}
	PX_StringFree(&str);
	return PX_TRUE;
}

PX_SYNTAX_FUNCTION(PX_Syntax_Parse_expand_parameters_begin)
{
	px_abi* pnewabi;
	pnewabi = PX_Syntax_NewAbi(pSyntax, "expand_parameters", pSyntax->reg_lifetime);
	if (!pnewabi)
	{
		PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_Parse_expand_parameters_begin Memory Error1");
		return PX_FALSE;
	}
	if (!PX_AbiSet_int(pnewabi, "count", 0))
	{
		PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_Parse_expand_parameters_begin Memory Error2");
		return PX_FALSE;
	}
	return PX_TRUE;

}

PX_SYNTAX_FUNCTION(PX_Syntax_Parse_expand_parameters_merge)
{
	px_abi* plastabi = PX_Syntax_GetAbiLast(pSyntax);
	px_abi* psecondlastabi = PX_Syntax_GetAbiSecondLast(pSyntax);
	const px_char* pname;
	px_int* pcount, count;
	px_char content[32] = { 0 };
	if (!plastabi || !psecondlastabi)
	{
		return PX_FALSE;
	}
	pname = PX_AbiGet_string(plastabi, "name");
	if (!pname || !PX_strequ(pname, "expand_parameter"))
	{
		return PX_FALSE;
	}
	pcount = PX_AbiGet_int(psecondlastabi, "count");
	if (!pcount)
	{
		return PX_FALSE;
	}
	count = *pcount;
	PX_AbiSet_int(psecondlastabi, "count", count + 1);
	PX_sprintf1(content, sizeof(content), "expand_parameter[%1]", PX_STRINGFORMAT_INT(count));

	if (!PX_Syntax_MergeLast2AbiWithNameToSecondLast(pSyntax, content))
	{
		PX_ASSERTX("Merge Error");
		return PX_FALSE;
	}
	return PX_TRUE;
}

PX_SYNTAX_FUNCTION(PX_Syntax_Parse_expand_parameters_end)
{
	return PX_TRUE;
}

PX_SYNTAX_FUNCTION(PX_Syntax_Parse_expand_parameters_error)
{
	PX_Syntax_Terminate(pSyntax, "ast:error:unexpected expand_parameters");
	return PX_FALSE;
}

px_bool PX_Syntax_load_expand_parameters(PX_Syntax* pSyntax)
{
	if (!PX_Syntax_Parse_PEBNF(pSyntax, "expand_parameter = *", PX_Syntax_Parse_expand_parameter, 0, 0))
		return PX_FALSE;

	if (!PX_Syntax_Parse_PEBNF(pSyntax, "expand_parameters_list = expand_parameter", 0, PX_Syntax_Parse_expand_parameters_merge, 0))
		return PX_FALSE;

	if (!PX_Syntax_Parse_PEBNF(pSyntax, "expand_parameters_list = expand_parameter ',' expand_parameters_list", 0, 0, 0))
		return PX_FALSE;

	if (!PX_Syntax_Parse_PEBNF(pSyntax, "expand_parameters_list = expand_parameter ',' *", PX_Syntax_Parse_expand_parameters_error, 0, 0))
		return PX_FALSE;

	if (!PX_Syntax_Parse_PEBNF(pSyntax, "expand_parameters_list = expand_parameter *", PX_Syntax_Parse_expand_parameters_end, 0, 0))
		return PX_FALSE;

	if (!PX_Syntax_Parse_PEBNF(pSyntax, "expand_parameters = '('", 0, PX_Syntax_Parse_expand_parameters_begin, 0))
		return PX_FALSE;

	if (!PX_Syntax_Parse_PEBNF(pSyntax, "expand_parameters = '(' ')'", 0, 0, 0))
		return PX_FALSE;

	if (!PX_Syntax_Parse_PEBNF(pSyntax, "expand_parameters = '(' expand_parameters_list ')'", 0, 0, 0))
		return PX_FALSE;

	return PX_TRUE;
}

px_bool PX_Syntax_load_define(PX_Syntax* pSyntax)
{
	if (!PX_Syntax_load_expand_parameters(pSyntax))
		return PX_FALSE;

	if(!PX_Syntax_load_define_parameters(pSyntax))
		return PX_FALSE;

	if (!PX_Syntax_Parse_PEBNF(pSyntax, "define = '#' 'define'",0, PX_Syntax_Parse_define_new, 0))
		return PX_FALSE;

	if (!PX_Syntax_Parse_PEBNF(pSyntax, "define_identifier = *",0, PX_Syntax_Parse_define_identifier, 0))
		return PX_FALSE;

	if (!PX_Syntax_Parse_PEBNF(pSyntax, "define = '#' 'define' define_identifier",0, PX_Syntax_Parse_define_identifier_check, 0))
		return PX_FALSE;

	if (!PX_Syntax_Parse_PEBNF(pSyntax, "define = '#' 'define' define_identifier&define_parameters", 0, PX_Syntax_Parse_define_parameters_merge, 0))
		return PX_FALSE;

	if (!PX_Syntax_Parse_PEBNF(pSyntax, "define_content = *",0, PX_Syntax_Parse_define_content, 0))
		return PX_FALSE;

	if (!PX_Syntax_Parse_PEBNF(pSyntax, "define = '#' 'define' define_identifier&define_parameters define_content", 0, 0, 0))
		return PX_FALSE;

	if (!PX_Syntax_Parse_PEBNF(pSyntax, "define = '#' 'define' define_identifier define_content", 0, 0, 0))
		return PX_FALSE;

	if (!PX_Syntax_Parse_PEBNF(pSyntax, "call_define_identifier = *", 0, PX_Syntax_Parse_call_define_identifier, 0))
		return PX_FALSE;

	if (!PX_Syntax_Parse_PEBNF(pSyntax, "call_define = call_define_identifier&expand_parameters", 0, PX_Syntax_Parse_call_define, 0))
		return PX_FALSE;

	if (!PX_Syntax_Parse_PEBNF(pSyntax, "call_define = call_define_identifier *", 0, PX_Syntax_Parse_call_define, 0))
		return PX_FALSE;

	return PX_TRUE;
}