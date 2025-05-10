#include "PX_Syntax_tuple.h"

PX_SYNTAX_FUNCTION(PX_Syntax_Parse_define_new)
{
	px_abi* pnewabi;
	pnewabi = PX_Syntax_NewAbi(pSyntax, "define", pSyntax->reg_lifetime);
	if (!pnewabi)
	{
		PX_Syntax_Terminate(pSyntax, "Memory Error");
		return PX_FALSE;
	}
	return PX_TRUE;
}

PX_SYNTAX_FUNCTION(PX_Syntax_Parse_define_parameters_begin)
{
	px_abi* pnewabi;
	pnewabi = PX_Syntax_NewAbi(pSyntax, "define_parameters", pSyntax->reg_lifetime);
	if (!pnewabi)
	{
		PX_Syntax_Terminate(pSyntax, "Memory Error");
		return PX_FALSE;
	}
	if(!PX_AbiSet_int(pnewabi, "count", 0))
	{
		PX_Syntax_Terminate(pSyntax, "Memory Error");
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
	PX_Syntax_Terminate(pSyntax, "Syntax Error:unexpected define_parameters");
	return PX_FALSE;
}

px_bool PX_Syntax_load_define_parameters(PX_Syntax* pSyntax)
{
	if (!PX_Syntax_Parse_PEBNF(pSyntax, "define_parameters_list = identifier", PX_Syntax_Parse_define_parameters_new))
		return PX_FALSE;

	if (!PX_Syntax_Parse_PEBNF(pSyntax, "define_parameters_list = identifier ',' define_parameters_list",0))
		return PX_FALSE;

	if (!PX_Syntax_Parse_PEBNF(pSyntax, "define_parameters_list = identifier ',' *", PX_Syntax_Parse_define_parameters_error))
		return PX_FALSE;

	if (!PX_Syntax_Parse_PEBNF(pSyntax, "define_parameters_list = identifier *", PX_Syntax_Parse_define_parameters_end))
		return PX_FALSE;

	if (!PX_Syntax_Parse_PEBNF(pSyntax, "define_parameters = '('", PX_Syntax_Parse_define_parameters_begin))
		return PX_FALSE;

	if (!PX_Syntax_Parse_PEBNF(pSyntax, "define_parameters = '(' ')'", 0))
		return PX_FALSE;

	if (!PX_Syntax_Parse_PEBNF(pSyntax, "define_parameters = '(' define_parameters_list ')'", 0))
		return PX_FALSE;
	
	return PX_TRUE;
}

PX_SYNTAX_FUNCTION(PX_Syntax_Parse_define_identifier)
{
	px_abi* pnewabi;
	PX_LEXER_LEXEME_TYPE type = PX_Syntax_GetNextLexeme(pSyntax);
	if (type!=PX_LEXER_LEXEME_TYPE_TOKEN)
	{
		PX_Syntax_Terminate(pSyntax, "Syntax Error:unexpected define_identifier");
		return PX_FALSE;
	}
	else
	{
		px_char ch;
		pnewabi = PX_Syntax_NewAbi(pSyntax, "define_identifier", pSyntax->reg_lifetime);
		if (!pnewabi)
		{
			PX_Syntax_Terminate(pSyntax, "Memory Error");
			return PX_FALSE;
		}
		if (!PX_AbiSet_string(pnewabi,"value",PX_Syntax_GetCurrentLexeme(pSyntax)))
		{
			PX_Syntax_Terminate(pSyntax, "Memory Error");
			return PX_FALSE;
		}
		ch = PX_Syntax_PreviewNextChar(pSyntax);
		if (ch == '(')
		{
			if (!PX_Syntax_Execute(pSyntax, "define_parameters"))
			{
				PX_Syntax_Terminate(pSyntax, "Memory Error");
				return PX_FALSE;
			}
			if (!PX_Syntax_MergeLast2AbiToSecondLast(pSyntax))
			{
				return PX_FALSE;
			}
		}
	}

	return PX_TRUE;
}

PX_SYNTAX_FUNCTION(PX_Syntax_Parse_define_identifier_check)
{
	px_int i;
	px_abi* plastabi = PX_Syntax_GetAbiLast(pSyntax);
	px_abi* psencondlastabi = PX_Syntax_GetAbiSecondLast(pSyntax);
	const px_char * pcurrent_define_name;
	if (!plastabi)
	{
		return PX_FALSE;
	}
	
	if (!PX_Syntax_CheckAbiName(plastabi, "define_identifier"))
	{
		PX_Syntax_Terminate(pSyntax, "Syntax Error:unexpected define_identifier");
	}
	pcurrent_define_name = PX_AbiGet_string(plastabi, "value");
	for (i = 0; i < PX_Syntax_GetAbiCount(pSyntax); i++)
	{
		px_abi* pabi = PX_Syntax_GetAbiByIndex(pSyntax, i);
		if (PX_Syntax_CheckAbiName(pabi, "define"))
		{
			const px_char* pdefine_name = PX_AbiGet_string(pabi, "define_identifier.value");
			if (pdefine_name&&PX_strequ(pdefine_name, pcurrent_define_name))
			{
				PX_Syntax_Terminate(pSyntax, "Syntax Error:define_identifier already exist");
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

PX_SYNTAX_FUNCTION(PX_Syntax_Parse_define_content)
{
	PX_LEXER_LEXEME_TYPE type = PX_Syntax_GetNextLexeme4(pSyntax);
	px_string str;
	if (type == PX_LEXER_LEXEME_TYPE_END || type == PX_LEXER_LEXEME_TYPE_NEWLINE)
	{
		px_abi* plastabi = PX_Syntax_GetAbiLast(pSyntax);
		if (!PX_AbiSet_string(plastabi, "define_content", ""))
		{
			PX_Syntax_Terminate(pSyntax, "Memory Error");
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

		define_identifier = PX_AbiGet_string(plastdefineabi, "define_identifier.value");
		if (type != PX_LEXER_LEXEME_TYPE_SPACER)
		{
			PX_Syntax_Terminate(pSyntax, "Syntax Error:spacer expected");
			return PX_FALSE;
		}
		if (!PX_StringInitialize(pSyntax->mp, &str))
		{
			PX_Syntax_Terminate(pSyntax, "Memory Error");
			return PX_FALSE;
		}

		pcount = PX_AbiGet_int(plastdefineabi, "define_identifier.define_parameters.count");
		if (pcount)
			paramcount = *pcount;
		else
			paramcount = 0;

		while (PX_TRUE)
		{
			type = PX_Syntax_GetNextLexeme4(pSyntax);

			if (type == PX_LEXER_LEXEME_TYPE_END || type == PX_LEXER_LEXEME_TYPE_NEWLINE)
				break;

			if (type == PX_LEXER_LEXEME_TYPE_TOKEN)
			{
				px_int i;
				for (i = 0; i < paramcount; i++)
				{
					px_char payload[128] = { 0 };
					const px_char* pvalue;
					PX_sprintf1(payload, sizeof(payload), "define_identifier.define_parameters.identifier[%1].value", PX_STRINGFORMAT_INT(i));
					pvalue = PX_AbiGet_string(plastdefineabi, payload);
					if (!pvalue)
					{
						PX_ASSERTX("value not found");
						return PX_FALSE;
					}
					if (PX_strequ(pvalue, PX_Syntax_GetCurrentLexeme(pSyntax)))
					{
						if (!PX_StringCatChar(&str, '%') || !PX_StringCat(&str, PX_itos(i+1, 10).data))
						{
							PX_Syntax_Terminate(pSyntax, "Memory Error");
							return PX_FALSE;
						}
						break;
					}
				}
				if (paramcount==0||i== paramcount)
				{
					if (!PX_StringCat(&str, PX_Syntax_GetCurrentLexeme(pSyntax)))
					{
						PX_Syntax_Terminate(pSyntax, "Memory Error");
						return PX_FALSE;
					}
				}
			}
			else if (type == PX_LEXER_LEXEME_TYPE_DELIMITER)
			{
				if (PX_Syntax_GetCurrentLexeme(pSyntax), "#")
				{
					if (PX_Syntax_PreviewNextChar(pSyntax)=='#')
					{
						PX_Syntax_GetNextChar(pSyntax);
					}
					else
					{
						if (!PX_StringCat(&str, PX_Syntax_GetCurrentLexeme(pSyntax)))
						{
							PX_Syntax_Terminate(pSyntax, "Memory Error");
							return PX_FALSE;
						}
					}
				}
				else
				{
					if (!PX_StringCat(&str, PX_Syntax_GetCurrentLexeme(pSyntax)))
					{
						PX_Syntax_Terminate(pSyntax, "Memory Error");
						return PX_FALSE;
					}
				}
			}
			else
			{
				if (!PX_StringCat(&str, PX_Syntax_GetCurrentLexeme(pSyntax)))
				{
					PX_Syntax_Terminate(pSyntax, "Memory Error");
					return PX_FALSE;
				}
			}
			
		}
		if (!PX_Syntax_NewExpand(pSyntax, define_identifier, str.buffer, paramcount, pSyntax->reg_lifetime))
		{
			PX_Syntax_Terminate(pSyntax, "Memory Error");
			return PX_FALSE;
		}
		PX_StringFree(&str);
		PX_Syntax_PopAbi(pSyntax);
	}
	return PX_TRUE;
}

px_bool PX_Syntax_load_define(PX_Syntax* pSyntax)
{
	if(!PX_Syntax_load_define_parameters(pSyntax))
		return PX_FALSE;

	if (!PX_Syntax_Parse_PEBNF(pSyntax, "define = '#' 'define'", PX_Syntax_Parse_define_new))
		return PX_FALSE;

	if (!PX_Syntax_Parse_PEBNF(pSyntax, "define_identifier = *", PX_Syntax_Parse_define_identifier))
		return PX_FALSE;

	if (!PX_Syntax_Parse_PEBNF(pSyntax, "define = '#' 'define' define_identifier", PX_Syntax_Parse_define_identifier_check))
		return PX_FALSE;

	if (!PX_Syntax_Parse_PEBNF(pSyntax, "define_content = *", PX_Syntax_Parse_define_content))
		return PX_FALSE;

	if (!PX_Syntax_Parse_PEBNF(pSyntax, "define = '#' 'define' define_identifier define_content", 0))
		return PX_FALSE;

	return PX_TRUE;
}