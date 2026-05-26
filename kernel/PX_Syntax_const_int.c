#include "PX_Syntax_const_int.h"

PX_SYNTAX_FUNCTION(PX_Syntax_Parse_unsigned_int)
{
	PX_SYNTAXLEXER_LEXEME_TYPE type;
	const px_char* plexeme;
	px_abi* pnewabi;
	px_int i, len, begin, end, begin_source_index, end_source_index;
	px_bool unsigned_flag = PX_FALSE;
	px_char build_number[32] = { 0 };
	type = PX_Syntax_GetNextLexeme(pSyntax);
	if (type != PX_SYNTAXLEXER_LEXEME_TYPE_TOKEN)
	{
		return PX_FALSE;
	}
	plexeme = PX_Syntax_GetCurrentLexeme(pSyntax);
	begin_source_index = PX_Syntax_GetCurrentLexemeBeginSourceIndex(pSyntax);
	end_source_index = PX_Syntax_GetCurrentLexemeEndSourceIndex(pSyntax);
	begin = PX_Syntax_GetCurrentLexemeBegin(pSyntax);
	end = PX_Syntax_GetCurrentLexemeEnd(pSyntax);
	if (begin_source_index!= end_source_index)
	{
		PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_Parse_unsigned_int Memory Error1");
		return PX_FALSE;
	}

	//xxxx
	if (PX_strIsInteger(plexeme))
	{
		px_char next_char_is_dot;
		if (PX_strlen(plexeme) > 20)
		{
			return PX_FALSE;
		}
		PX_strcat_s(build_number, sizeof(build_number), plexeme);
		next_char_is_dot = PX_Syntax_PreviewNextChar(pSyntax);
		if (next_char_is_dot == '.')
		{
			return PX_FALSE;
		}
		
		if (PX_strlen(build_number) == sizeof(build_number) - 1)
		{
			//numeric too long
			PX_Syntax_Terminate(pSyntax, "ast:error:Numeric too long");
			return PX_FALSE;
		}
		if (!(pnewabi = PX_Syntax_NewAbi(pSyntax, "const_int", pSyntax->reg_lifetime)))
		{
			PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_Parse_int Memory Error1");
			return PX_FALSE;
		}
		if (!PX_AbiSet_string(pnewabi, "value", build_number))
		{
			PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_Parse_int Memory Error2");
			return PX_FALSE;
		}

		if (!PX_AbiSet_bool(pnewabi, "unsigned", PX_FALSE))
		{
			PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_Parse_int Memory Error3");
			return PX_FALSE;
		}

		if (!PX_AbiSet_int(pnewabi, "source_index", begin_source_index))
		{
			PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_Parse_int Memory Error4");
			return PX_FALSE;
		}

		if (!PX_AbiSet_int(pnewabi, "begin", begin))
		{
			PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_Parse_int Memory Error4");
			return PX_FALSE;
		}

		if (!PX_AbiSet_int(pnewabi, "end", end))
		{
			PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_Parse_int Memory Error5");
			return PX_FALSE;
		}

		PX_Syntax_NewMapToken(pSyntax, begin_source_index, begin, end_source_index, end, PX_COLOR(255, 255, 255, 255),"ix.const");

		PX_Syntax_Message(pSyntax, "const_int:");
		PX_Syntax_Message(pSyntax, build_number);
		PX_Syntax_Message(pSyntax, "\n");
		return PX_TRUE;
	}

	//xxxexxx
	len = PX_strlen(plexeme);
	for (i = 0; i < len; i++)
	{
		if (plexeme[i] == 'e'|| plexeme[i] == 'E')
		{
			px_int j, e;
			px_char build_number1[32] = { 0 };
			px_char build_number2[32] = { 0 };
			for (j = 0; j < i; j++)
			{
				build_number1[j] = plexeme[j];
			}
			for (j = i + 1; j < len; j++)
			{
				build_number2[j - i - 1] = plexeme[j];
			}
			if (!PX_strIsInteger(build_number1) || !PX_strIsInteger(build_number2))
			{
				return PX_FALSE;
			}
			e = PX_atoi(build_number2);
			if (e == 0)
			{
				return PX_FALSE;
			}
			if (e > 12)
			{
				PX_Syntax_Terminate(pSyntax, "ast:error:Numeric too long");
				return PX_FALSE;
			}
			if (PX_strlen(build_number1)>=12)
			{
				PX_Syntax_Terminate(pSyntax, "ast:error:Numeric too long");
				return PX_FALSE;
			}

			PX_strcat_s(build_number, sizeof(build_number), build_number1);
			for ( j = 0; j < e; j++)
			{
				PX_strcat_s(build_number, sizeof(build_number), "0");
			}

			if (!(pnewabi = PX_Syntax_NewAbi(pSyntax, "const_int", pSyntax->reg_lifetime)))
			{
				PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_Parse_int Memory Error6");
				return PX_FALSE;
			}
			if (!PX_AbiSet_string(pnewabi, "value", build_number))
			{
				PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_Parse_int Memory Error7");
				return PX_FALSE;
			}
			if (!PX_AbiSet_bool(pnewabi, "unsigned", PX_FALSE))
			{
				PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_Parse_int Memory Error8");
				return PX_FALSE;
			}

			if (!PX_AbiSet_int(pnewabi, "source_index", begin_source_index))
			{
				PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_Parse_unsigned_float Memory Error7");
				return PX_FALSE;
			}

			if (!PX_AbiSet_int(pnewabi, "begin", begin))
			{
				PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_Parse_int Memory Error9");
				return PX_FALSE;
			}

			if (!PX_AbiSet_int(pnewabi, "end", end))
			{
				PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_Parse_int Memory Error10");
				return PX_FALSE;
			}
			PX_Syntax_NewMapToken(pSyntax, begin_source_index, begin, end_source_index, end, PX_COLOR(255, 255, 255, 255), "ix.const");
			PX_Syntax_Message(pSyntax, "const_int:");
			PX_Syntax_Message(pSyntax, build_number);
			PX_Syntax_Message(pSyntax, "\n");
			return PX_TRUE;

		}
	}

	//0xxxxxxxxx
	if (plexeme[0] == '0' && (plexeme[1] == 'x' || plexeme[1] == 'X'))
	{
		px_dword value;
		if (!(pnewabi = PX_Syntax_NewAbi(pSyntax, "const_int", pSyntax->reg_lifetime)))
		{
			PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_Parse_int Memory Error11");
			return PX_FALSE;
		}
		if (PX_strlen(plexeme+2)>8|| PX_strlen(plexeme + 2)==0)
		{
			PX_Syntax_Terminate(pSyntax, "ast:error:illegal hex number");
			return PX_FALSE;
		}
		value = PX_htoi(plexeme + 2);
		PX_utoa(value, build_number, sizeof(build_number), 10);
		if (!PX_AbiSet_string(pnewabi, "value", build_number))
		{
			PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_Parse_int Memory Error12");
			return PX_FALSE;
		}
		if (!PX_AbiSet_bool(pnewabi, "unsigned", PX_TRUE))
		{
			PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_Parse_int Memory Error13");
			return PX_FALSE;
		}

		if (!PX_AbiSet_int(pnewabi, "source_index", begin_source_index))
		{
			PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_Parse_unsigned_float Memory Error7");
			return PX_FALSE;
		}

		if (!PX_AbiSet_int(pnewabi, "begin", begin))
		{
			PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_Parse_int Memory Error14");
			return PX_FALSE;
		}

		if (!PX_AbiSet_int(pnewabi, "end", end))
		{
			PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_Parse_int Memory Error15");
			return PX_FALSE;
		}

		PX_Syntax_NewMapToken(pSyntax, begin_source_index, begin, end_source_index, end, PX_COLOR(255, 255, 255, 255), "ix.const");

		PX_Syntax_Message(pSyntax, "const_uint:");
		PX_Syntax_Message(pSyntax, build_number);
		PX_Syntax_Message(pSyntax, "\n");
		return PX_TRUE;
	}


	//xxxxxxu/l/U/L
	for ( i = 0; i < len-1; i++)
	{
		if (!PX_charIsNumeric(plexeme[i]))
		{
			return PX_FALSE;
		}
	}
	if (PX_strlen(plexeme)>20)
	{
		PX_Syntax_Terminate(pSyntax, "ast:error:Numeric too long");
		return PX_FALSE;
	}
	//last char is u or l
	if (len>=2&&(plexeme[len - 1] == 'u'|| plexeme[len - 1] == 'l'|| plexeme[len - 1] == 'U' || plexeme[len - 1] == 'L'))
	{
		PX_strcat_s(build_number, sizeof(build_number), plexeme);
		if (PX_strlen(build_number) == sizeof(build_number) - 1)
		{
			PX_Syntax_Terminate(pSyntax, "ast:error:Numeric too long");
			return PX_FALSE;
		}
		build_number[PX_strlen(build_number) - 1] = '\0';
		if (!(pnewabi = PX_Syntax_NewAbi(pSyntax, "const_int", pSyntax->reg_lifetime)))
		{
			PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_Parse_int Memory Error16");
			return PX_FALSE;
		}
		if (!PX_AbiSet_string(pnewabi, "value", build_number))
		{
			PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_Parse_int Memory Error17");
			return PX_FALSE;
		}
		if (plexeme[len - 1] == 'U' || plexeme[len - 1] == 'u')
		{
			if (!PX_AbiSet_bool(pnewabi, "unsigned", PX_TRUE))
			{
				PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_Parse_int Memory Error18");
				return PX_FALSE;
			}
		}
		else
		{
			if (!PX_AbiSet_bool(pnewabi, "unsigned", PX_FALSE))
			{
				PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_Parse_int Memory Error19");
				return PX_FALSE;
			}
		}

		if (!PX_AbiSet_int(pnewabi, "source_index", begin_source_index))
		{
			PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_Parse_unsigned_float Memory Error7");
			return PX_FALSE;
		}

		if (!PX_AbiSet_int(pnewabi, "begin", begin))
		{
			PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_Parse_int Memory Error20");
			return PX_FALSE;
		}

		if (!PX_AbiSet_int(pnewabi, "end", end))
		{
			PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_Parse_int Memory Error21");
			return PX_FALSE;
		}

		PX_Syntax_NewMapToken(pSyntax, begin_source_index, begin, end_source_index, end, PX_COLOR(255, 255, 255, 255), "ix.const");
		
		PX_Syntax_Message(pSyntax, "const_uint:");
		PX_Syntax_Message(pSyntax, build_number);
		PX_Syntax_Message(pSyntax, "\n");
		return PX_TRUE;
	}


	return PX_FALSE;
}


PX_SYNTAX_FUNCTION(PX_Syntax_Parse_neg_int)
{
	px_abi* plastabi = PX_Syntax_GetAbiLast(pSyntax);
	if (!PX_AbiInsert_string(plastabi, "value", 0, "-"))
	{
		PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_Parse_neg_int Memory Error");
		return PX_FALSE;
	}
	return PX_TRUE;
}


px_bool PX_Syntax_load_const_int(PX_Syntax* pSyntax)
{

	if (!PX_Syntax_Parse_PEBNF(pSyntax, "const_unsigned_int = *", 0,PX_Syntax_Parse_unsigned_int, 0))
		return PX_FALSE;

	if (!PX_Syntax_Parse_PEBNF(pSyntax, "const_int = const_unsigned_int", 0, 0, 0))
		return PX_FALSE;

	if (!PX_Syntax_Parse_PEBNF(pSyntax, "const_int = '-' const_unsigned_int", 0, PX_Syntax_Parse_neg_int, 0))
		return PX_FALSE;

	return PX_TRUE;
}