#include "PX_Syntax_const_float.h"

PX_SYNTAX_FUNCTION(PX_Syntax_Parse_unsigned_float)
{
	PX_SYNTAXLEXER_LEXEME_TYPE type;
	const px_char *plexeme;
	px_char build_number[32] = { 0 };
	px_int len,i;
	px_abi *pnewabi;
	px_char next_char_dot;
	px_int begin, end;
	px_int begin_source_index, end_source_index;
	type = PX_Syntax_GetNextLexeme(pSyntax);
	if (type!=PX_SYNTAXLEXER_LEXEME_TYPE_TOKEN)
	{
		return PX_FALSE;
	}
	plexeme = PX_Syntax_GetCurrentLexeme(pSyntax);
	begin = PX_Syntax_GetCurrentLexemeBegin(pSyntax);
	begin_source_index = PX_Syntax_GetCurrentLexemeBeginSourceIndex(pSyntax);
	end_source_index = PX_Syntax_GetCurrentLexemeEndSourceIndex(pSyntax);
	if (begin_source_index != end_source_index)
	{
		PX_Syntax_Terminate(pSyntax, "runtime:error:cross source define is not allowed");
		return PX_FALSE;
	}
	///xxxe-xxx
	len = PX_strlen(plexeme);
	if (len>1&&plexeme[len-1]=='e')
	{
		px_char build_number1[32] = { 0 };
		px_char build_number2[32] = { 0 };
		px_int e,dot_pos;
		px_char next_char;
		for (i = 0; i < len - 1; i++)
		{
			build_number1[i] = plexeme[i];
		}
		next_char = PX_Syntax_GetNextChar(pSyntax);
		if (next_char != '-')
		{
			return PX_FALSE;
		}
		
		type = PX_Syntax_GetNextLexeme(pSyntax);
		if (type != PX_SYNTAXLEXER_LEXEME_TYPE_TOKEN)
		{
			return PX_FALSE;
		}
		plexeme = PX_Syntax_GetCurrentLexeme(pSyntax);
		if (!PX_strIsInteger(plexeme))
		{
			return PX_FALSE;
		}
		PX_strcat_s(build_number2, sizeof(build_number2), plexeme);
		if (PX_strlen(build_number2) == sizeof(build_number2) - 1)
		{
			//numeric too long
			PX_Syntax_Terminate(pSyntax,"ast:error:Numeric too long");
			return PX_FALSE;
		}
		end = PX_Syntax_GetCurrentLexemeEnd(pSyntax);
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
		if (PX_strlen(build_number) >= 12)
		{
			//numeric too long
			PX_Syntax_Terminate(pSyntax, "ast:error:Numeric too long");
			return PX_FALSE;
		}
		dot_pos = PX_strlen(build_number1) - e;
		if (dot_pos<0)
		{
			while (dot_pos < 0)
			{
				PX_strcat_s(build_number, sizeof(build_number), "0");
				dot_pos++;
			}
			PX_strcat_s(build_number, sizeof(build_number), ".");
			PX_strcat_s(build_number, sizeof(build_number), build_number1);
		}
		else
		{
			px_int j=0;
			for (i = 0; i < PX_strlen(build_number1); i++)
			{
				if (i == dot_pos)
				{
					PX_strcat_s(build_number, sizeof(build_number), ".");
					j++;
				}
				build_number[j] = build_number1[i];
				j++;
			}
		}

		if(!(pnewabi=PX_Syntax_NewAbi(pSyntax, "const_float",pSyntax->reg_lifetime)))
		{
			PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_Parse_unsigned_float Memory Error1");
			return PX_FALSE;
		}

		if(!PX_AbiSet_string(pnewabi,"value",build_number))
		{
			PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_Parse_unsigned_float Memory Error2");
			return PX_FALSE;
		}

		if (!PX_AbiSet_int(pnewabi, "source_index", begin_source_index))
		{
			PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_Parse_unsigned_float Memory Error4");
			return PX_FALSE;
		}

		if (!PX_AbiSet_int(pnewabi, "begin", begin))
		{
			PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_Parse_unsigned_float Memory Error5");
			return PX_FALSE;
		}
		if (!PX_AbiSet_int(pnewabi, "end", end))
		{
			PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_Parse_unsigned_float Memory Error6");
			return PX_FALSE;
		}
		
		PX_Syntax_NewMapToken(pSyntax, begin_source_index, begin, end_source_index, end, PX_COLOR(255, 255, 255, 255), "fx:const");
		
		PX_Syntax_Message(pSyntax, "const_float:");
		PX_Syntax_Message(pSyntax, build_number);
		PX_Syntax_Message(pSyntax, "\n");
		return PX_TRUE;
	}


	if (!PX_strIsInteger(plexeme))
	{
		return PX_FALSE;
	}
	PX_strcat_s(build_number, sizeof(build_number), plexeme);
	if (PX_strlen(build_number)==sizeof(build_number)-1)
	{
		PX_Syntax_Terminate(pSyntax, "ast:error:Numeric too long");
		return PX_FALSE;
	}
	next_char_dot = PX_Syntax_GetNextChar(pSyntax);
	
	if (next_char_dot!= '.')
	{
		return PX_FALSE;
	}
	PX_strcat_s(build_number, sizeof(build_number), ".");
	if (PX_strlen(build_number) == sizeof(build_number) - 1)
	{
		PX_Syntax_Terminate(pSyntax, "ast:error:Numeric too long");
		return PX_FALSE;
	}

	type = PX_Syntax_GetNextLexeme(pSyntax);
	if (type != PX_SYNTAXLEXER_LEXEME_TYPE_TOKEN)
	{
		return PX_FALSE;
	}
	plexeme = PX_Syntax_GetCurrentLexeme(pSyntax);
	//xxx.f
	if (PX_strequ(plexeme,"f"))
	{
		PX_strcat_s(build_number, sizeof(build_number), "0");
		if (PX_strlen(build_number) == sizeof(build_number) - 1)
		{
			PX_Syntax_Terminate(pSyntax, "ast:error:Numeric too long");
			return PX_FALSE;
		}
		end = PX_Syntax_GetCurrentLexemeEnd(pSyntax);
		if (!(pnewabi = PX_Syntax_NewAbi(pSyntax, "const_float", pSyntax->reg_lifetime)))
		{
			PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_Parse_unsigned_float Memory Error5");
			return PX_FALSE;
		}
		if (!PX_AbiSet_string(pnewabi, "value", build_number))
		{
			PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_Parse_unsigned_float Memory Error6");
			return PX_FALSE;
		}

		if (!PX_AbiSet_int(pnewabi, "source_index", begin_source_index))
		{
			PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_Parse_unsigned_float Memory Error7");
			return PX_FALSE;
		}

		if (!PX_AbiSet_int(pnewabi, "begin", begin))
		{
			PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_Parse_unsigned_float Memory Error7");
			return PX_FALSE;
		}

		if (!PX_AbiSet_int(pnewabi, "end", end))
		{
			PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_Parse_unsigned_float Memory Error8");
			return PX_FALSE;
		}

		if (!PX_Syntax_NewMapToken(pSyntax, begin_source_index,begin, end_source_index,end,PX_COLOR_WHITE,"fx:const"))
		{
			PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_Parse_unsigned_float Memory Error9");
			return PX_FALSE;
		}

		PX_Syntax_Message(pSyntax, "const_float:");
		PX_Syntax_Message(pSyntax, build_number);
		PX_Syntax_Message(pSyntax, "\n");
		return PX_TRUE;
	}

	//xxx.xxxf
	len = PX_strlen(plexeme);
	if (len > 1 && plexeme[len-1] == 'f')
	{
		px_char build_number1[32] = { 0 };
		PX_strcat_s(build_number1, sizeof(build_number1), plexeme);
		build_number1[len - 1] = 0;
		if (!PX_strIsInteger(build_number1))
		{
			return PX_FALSE;
		}
		PX_strcat_s(build_number, sizeof(build_number), build_number1);

		if (PX_strlen(build_number) == sizeof(build_number) - 1)
		{
			PX_Syntax_Terminate(pSyntax, "ast:error:Numeric too long");
			return PX_FALSE;
		}
		end = PX_Syntax_GetCurrentLexemeEnd(pSyntax);
		if (!(pnewabi = PX_Syntax_NewAbi(pSyntax, "const_float", pSyntax->reg_lifetime)))
		{
			PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_Parse_unsigned_float Memory Error10");
			return PX_FALSE;
		}
		if (!PX_AbiSet_string(pnewabi, "value", build_number))
		{
			PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_Parse_unsigned_float Memory Error11");
			return PX_FALSE;
		}

		if (!PX_AbiSet_int(pnewabi, "source_index", begin_source_index))
		{
			PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_Parse_unsigned_float Memory Error7");
			return PX_FALSE;
		}

		if (!PX_AbiSet_int(pnewabi, "begin", begin))
		{
			PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_Parse_unsigned_float Memory Error12");
			return PX_FALSE;
		}

		if (!PX_AbiSet_int(pnewabi, "end", end))
		{
			PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_Parse_unsigned_float Memory Error13");
			return PX_FALSE;
		}

		if (!PX_Syntax_NewMapToken(pSyntax, begin_source_index, begin, end_source_index, end, PX_COLOR_WHITE, "fx:const"))
		{
			PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_Parse_unsigned_float Memory Error14");
			return PX_FALSE;
		}

		PX_Syntax_Message(pSyntax, "const_float:");
		PX_Syntax_Message(pSyntax, build_number);
		PX_Syntax_Message(pSyntax, "\n");
		return PX_TRUE;
	}

	//xxx.xxx
	if (PX_strIsInteger(plexeme))
	{
		PX_strcat_s(build_number, sizeof(build_number), plexeme);
		if (PX_strlen(build_number) == sizeof(build_number) - 1)
		{
			PX_Syntax_Terminate(pSyntax, "ast:error:Numeric too long");
			return PX_FALSE;
		}
		end = PX_Syntax_GetCurrentLexemeEnd(pSyntax);
		if (!(pnewabi = PX_Syntax_NewAbi(pSyntax, "const_float", pSyntax->reg_lifetime)))
		{
			PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_Parse_unsigned_float Memory Error15");
			return PX_FALSE;
		}
		if (!PX_AbiSet_string(pnewabi, "value", build_number))
		{
			PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_Parse_unsigned_float Memory Error16");
			return PX_FALSE;
		}

		if (!PX_AbiSet_int(pnewabi, "source_index", begin_source_index))
		{
			PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_Parse_unsigned_float Memory Error17");
			return PX_FALSE;
		}

		if (!PX_AbiSet_int(pnewabi, "begin", begin))
		{
			PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_Parse_unsigned_float Memory Error18");
			return PX_FALSE;
		}

		if (!PX_AbiSet_int(pnewabi, "end", end))
		{
			PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_Parse_unsigned_float Memory Error19");
			return PX_FALSE;
		}

		if (!PX_Syntax_NewMapToken(pSyntax, begin_source_index, begin, end_source_index, end, PX_COLOR_WHITE, "fx:const"))
		{
			PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_Parse_unsigned_float Memory Error20");
			return PX_FALSE;
		}

		PX_Syntax_Message(pSyntax, "const_float:");
		PX_Syntax_Message(pSyntax, build_number);
		PX_Syntax_Message(pSyntax, "\n");
		return PX_TRUE;
	}
	return PX_FALSE;
}

PX_SYNTAX_FUNCTION(PX_Syntax_Parse_neg_float)
{
	px_abi* plastabi = PX_Syntax_GetAbiLast(pSyntax);
	if (!PX_AbiInsert_string(plastabi,"value",0,"-"))
	{
		PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_Parse_neg_float Memory Error");
		return PX_FALSE;
	}
	return PX_TRUE;
}


px_bool PX_Syntax_load_const_float(PX_Syntax* pSyntax)
{
	if (!PX_Syntax_Parse_PEBNF(pSyntax, "const_unsigned_float = *",0, PX_Syntax_Parse_unsigned_float, 0))
		return PX_FALSE;

	if (!PX_Syntax_Parse_PEBNF(pSyntax, "const_float = const_unsigned_float", 0, 0, 0))
		return PX_FALSE;

	if (!PX_Syntax_Parse_PEBNF(pSyntax, "const_float = '-' const_unsigned_float", 0, PX_Syntax_Parse_neg_float, 0))
		return PX_FALSE;

	return PX_TRUE;
}