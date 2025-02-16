#include "PX_Syntax_numeric.h"


PX_SYNTAX_FUNCTION(PX_Syntax_Parse_float)
{
	PX_LEXER_LEXEME_TYPE type;
	const px_char *plexeme;
	px_char build_number[32] = { 0 };
	px_int len,i;
	px_abi *pnewabi;
	px_char next_char_dot;
	type = PX_SyntaxGetNextAstFilter(past);
	if (type!=PX_LEXER_LEXEME_TYPE_TOKEN)
	{
		return PX_FALSE;
	}
	plexeme = PX_SyntaxGetCurrentLexeme(past);

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
		next_char = PX_LexerGetNextChar(past->lexer_state.plexer);
		if (next_char != '-')
		{
			return PX_FALSE;
		}
		
		type = PX_SyntaxGetNextAstFilter(past);
		if (type != PX_LEXER_LEXEME_TYPE_TOKEN)
		{
			return PX_FALSE;
		}
		plexeme = PX_SyntaxGetCurrentLexeme(past);
		if (!PX_strIsInteger(plexeme))
		{
			return PX_FALSE;
		}
		PX_strcat_s(build_number2, sizeof(build_number2), plexeme);
		if (PX_strlen(build_number2) == sizeof(build_number2) - 1)
		{
			//numeric too long
			PX_Syntax_Terminate(pSyntax, past,"Numeric too long");
			return PX_FALSE;
		}
		e = PX_atoi(build_number2);
		if (e == 0)
		{
			return PX_FALSE;
		}
		if (e > 12)
		{
			PX_Syntax_Terminate(pSyntax, past, "Numeric too long");
			return PX_FALSE;
		}
		if (PX_strlen(build_number) >= 12)
		{
			//numeric too long
			PX_Syntax_Terminate(pSyntax, past, "Numeric too long");
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
		if(!(pnewabi=PX_Syntax_NewAbi(pSyntax, "float",pSyntax->lifetime)))
		{
			PX_Syntax_Terminate(pSyntax, past, "Memory Error");
			return PX_FALSE;
		}
		if(!PX_AbiWrite_string(pnewabi,"value",build_number))
		{
			PX_Syntax_Terminate(pSyntax, past, "Memory Error");
			return PX_FALSE;
		}
		PX_Syntax_AstMessage(pSyntax, "float:");
		PX_Syntax_AstMessage(pSyntax, build_number);
		PX_Syntax_AstMessage(pSyntax, "\n");
		return PX_TRUE;
	}


	if (!PX_strIsInteger(plexeme))
	{
		return PX_FALSE;
	}
	PX_strcat_s(build_number, sizeof(build_number), plexeme);
	if (PX_strlen(build_number)==sizeof(build_number)-1)
	{
		PX_Syntax_Terminate(pSyntax, past, "Numeric too long");
		return PX_FALSE;
	}
	next_char_dot = PX_LexerGetNextChar(past->lexer_state.plexer);
	
	if (next_char_dot!= '.')
	{
		return PX_FALSE;
	}
	PX_strcat_s(build_number, sizeof(build_number), ".");
	if (PX_strlen(build_number) == sizeof(build_number) - 1)
	{
		PX_Syntax_Terminate(pSyntax, past, "Numeric too long");
		return PX_FALSE;
	}

	type = PX_SyntaxGetNextAstFilter(past);
	if (type != PX_LEXER_LEXEME_TYPE_TOKEN)
	{
		return PX_FALSE;
	}
	plexeme = PX_SyntaxGetCurrentLexeme(past);
	//xxx.f
	if (PX_strequ(plexeme,"f"))
	{
		PX_strcat_s(build_number, sizeof(build_number), "0");
		if (PX_strlen(build_number) == sizeof(build_number) - 1)
		{
			PX_Syntax_Terminate(pSyntax, past, "Numeric too long");
			return PX_FALSE;
		}
		if (!(pnewabi = PX_Syntax_NewAbi(pSyntax, "float", pSyntax->lifetime)))
		{
			PX_Syntax_Terminate(pSyntax, past, "Memory Error");
			return PX_FALSE;
		}
		if (!PX_AbiWrite_string(pnewabi, "value", build_number))
		{
			PX_Syntax_Terminate(pSyntax, past, "Memory Error");
			return PX_FALSE;
		}
		PX_Syntax_AstMessage(pSyntax, "float:");
		PX_Syntax_AstMessage(pSyntax, build_number);
		PX_Syntax_AstMessage(pSyntax, "\n");
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
			PX_Syntax_Terminate(pSyntax, past, "Numeric too long");
			return PX_FALSE;
		}
		if (!(pnewabi = PX_Syntax_NewAbi(pSyntax, "float", pSyntax->lifetime)))
		{
			PX_Syntax_Terminate(pSyntax, past, "Memory Error");
			return PX_FALSE;
		}
		if (!PX_AbiWrite_string(pnewabi, "value", build_number))
		{
			PX_Syntax_Terminate(pSyntax, past, "Memory Error");
			return PX_FALSE;
		}
		PX_Syntax_AstMessage(pSyntax, "float:");
		PX_Syntax_AstMessage(pSyntax, build_number);
		PX_Syntax_AstMessage(pSyntax, "\n");
		return PX_TRUE;
	}

	//xxx.xxx
	if (PX_strIsInteger(plexeme))
	{
		PX_strcat_s(build_number, sizeof(build_number), plexeme);
		if (PX_strlen(build_number) == sizeof(build_number) - 1)
		{
			PX_Syntax_Terminate(pSyntax, past, "Numeric too long");
			return PX_FALSE;
		}
		if (!(pnewabi = PX_Syntax_NewAbi(pSyntax, "float", pSyntax->lifetime)))
		{
			PX_Syntax_Terminate(pSyntax, past, "Memory Error");
			return PX_FALSE;
		}
		if (!PX_AbiWrite_string(pnewabi, "value", build_number))
		{
			PX_Syntax_Terminate(pSyntax, past, "Memory Error");
			return PX_FALSE;
		}
		PX_Syntax_AstMessage(pSyntax, "float:");
		PX_Syntax_AstMessage(pSyntax, build_number);
		PX_Syntax_AstMessage(pSyntax, "\n");
		return PX_TRUE;
	}
	return PX_FALSE;
}

PX_SYNTAX_FUNCTION(PX_Syntax_Parse_int)
{
	PX_LEXER_LEXEME_TYPE type;
	const px_char* plexeme;
	px_abi* pnewabi;
	px_int i,len;
	px_bool unsigned_flag = PX_FALSE;
	px_char build_number[32] = { 0 };
	type = PX_SyntaxGetNextAstFilter(past);
	if (type != PX_LEXER_LEXEME_TYPE_TOKEN)
	{
		return PX_FALSE;
	}
	plexeme = PX_SyntaxGetCurrentLexeme(past);

	//xxxx
	if (PX_strIsInteger(plexeme))
	{
		px_char next_char_is_dot;
		PX_strcat_s(build_number, sizeof(build_number), plexeme);
		next_char_is_dot = PX_LexerPreviewNextChar(past->lexer_state.plexer);
		if (next_char_is_dot == '.')
		{
			return PX_FALSE;
		}
		
		if (PX_strlen(build_number) == sizeof(build_number) - 1)
		{
			//numeric too long
			PX_Syntax_Terminate(pSyntax, past, "Numeric too long");
			return PX_FALSE;
		}
		if (!(pnewabi = PX_Syntax_NewAbi(pSyntax, "int", pSyntax->lifetime)))
		{
			PX_Syntax_Terminate(pSyntax, past, "Memory Error");
			return PX_FALSE;
		}
		if (!PX_AbiWrite_string(pnewabi, "value", build_number))
		{
			PX_Syntax_Terminate(pSyntax, past, "Memory Error");
			return PX_FALSE;
		}
		PX_Syntax_AstMessage(pSyntax, "int:");
		PX_Syntax_AstMessage(pSyntax, build_number);
		PX_Syntax_AstMessage(pSyntax, "\n");
		return PX_TRUE;
	}

	//xxxexxx
	len = PX_strlen(plexeme);
	for (i = 0; i < len; i++)
	{
		if (plexeme[i] == 'e')
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
				PX_Syntax_Terminate(pSyntax, past, "Numeric too long");
				return PX_FALSE;
			}
			if (PX_strlen(build_number1)>=12)
			{
				PX_Syntax_Terminate(pSyntax, past, "Numeric too long");
				return PX_FALSE;
			}

			PX_strcat_s(build_number, sizeof(build_number), build_number1);
			for ( j = 0; j < e; j++)
			{
				PX_strcat_s(build_number, sizeof(build_number), "0");
			}
			if (!(pnewabi = PX_Syntax_NewAbi(pSyntax, "int", pSyntax->lifetime)))
			{
				PX_Syntax_Terminate(pSyntax, past, "Memory Error");
				return PX_FALSE;
			}
			if (!PX_AbiWrite_string(pnewabi, "value", build_number))
			{
				PX_Syntax_Terminate(pSyntax, past, "Memory Error");
				return PX_FALSE;
			}
			if (!PX_AbiWrite_bool(pnewabi, "unsigned", PX_FALSE))
			{
				PX_Syntax_Terminate(pSyntax, past, "Memory Error");
				return PX_FALSE;
			}
			PX_Syntax_AstMessage(pSyntax, "int:");
			PX_Syntax_AstMessage(pSyntax, build_number);
			PX_Syntax_AstMessage(pSyntax, "\n");
			return PX_TRUE;

		}
	}


	for ( i = 0; i < len-1; i++)
	{
		if (!PX_charIsNumeric(plexeme[i]))
		{
			return PX_FALSE;
		}
	}
	//last char is u or l
	if (len>=2&&(plexeme[len - 1] == 'u'|| plexeme[len - 1] == 'l'|| plexeme[len - 1] == 'U' || plexeme[len - 1] == 'L'))
	{
		PX_strcat_s(build_number, sizeof(build_number), plexeme);
		if (PX_strlen(build_number) == sizeof(build_number) - 1)
		{
			PX_Syntax_Terminate(pSyntax, past, "Numeric too long");
			return PX_FALSE;
		}
		build_number[PX_strlen(build_number) - 1] = '\0';
		if (!(pnewabi = PX_Syntax_NewAbi(pSyntax, "int", pSyntax->lifetime)))
		{
			PX_Syntax_Terminate(pSyntax, past, "Memory Error");
			return PX_FALSE;
		}
		if (!PX_AbiWrite_string(pnewabi, "value", build_number))
		{
			PX_Syntax_Terminate(pSyntax, past, "Memory Error");
			return PX_FALSE;
		}
		if (plexeme[len - 1] == 'U' || plexeme[len - 1] == 'u')
		{
			if (!PX_AbiWrite_bool(pnewabi, "unsigned", PX_TRUE))
			{
				PX_Syntax_Terminate(pSyntax, past, "Memory Error");
				return PX_FALSE;
			}
		}
		else
		{
			if (!PX_AbiWrite_bool(pnewabi, "unsigned", PX_FALSE))
			{
				PX_Syntax_Terminate(pSyntax, past, "Memory Error");
				return PX_FALSE;
			}
		}
		
		PX_Syntax_AstMessage(pSyntax, "uint:");
		PX_Syntax_AstMessage(pSyntax, build_number);
		PX_Syntax_AstMessage(pSyntax, "\n");
		return PX_TRUE;
	}

	//0xxxxxxxxx
	if (plexeme[0] == '0'&& (plexeme[1] == 'x'|| plexeme[1] == 'X'))
	{
		px_dword value = PX_htoi(plexeme + 2);
		if (!(pnewabi = PX_Syntax_NewAbi(pSyntax, "int", pSyntax->lifetime)))
		{
			PX_Syntax_Terminate(pSyntax, past, "Memory Error");
			return PX_FALSE;
		}
		PX_itoa(value, build_number,sizeof(build_number),10);
		if (!PX_AbiWrite_string(pnewabi, "value", build_number))
		{
			PX_Syntax_Terminate(pSyntax, past, "Memory Error");
			return PX_FALSE;
		}
		if (!PX_AbiWrite_bool(pnewabi, "unsigned", PX_TRUE))
		{
			PX_Syntax_Terminate(pSyntax, past, "Memory Error");
			return PX_FALSE;
		}
		PX_Syntax_AstMessage(pSyntax, "uint:");
		PX_Syntax_AstMessage(pSyntax, build_number);
		PX_Syntax_AstMessage(pSyntax, "\n");
		return PX_TRUE;
	}
	return PX_FALSE;
}

PX_SYNTAX_FUNCTION(PX_Syntax_Parse_numeric)
{
	px_abi* plastabi;
	const px_char* pname;
	plastabi = PX_Syntax_GetAbiStack(pSyntax, -1);
	if (!plastabi)
	{
		PX_ASSERT();
		return PX_FALSE;
	}
	pname = PX_AbiGet_string(plastabi, "name");
	if (!pname)
	{
		PX_ASSERT();
		return PX_FALSE;
	}

	if (PX_strequ(pname, "int"))
	{
		if (!PX_AbiSet_string(plastabi, "name",  "numeric"))
		{
			return PX_FALSE;
		}
		if (!PX_AbiSet_string(plastabi, "type",  "int"))
		{
			return PX_FALSE;
		}
		return PX_TRUE;
	}
	if (PX_strequ(pname, "float"))
	{
		if (!PX_AbiSet_string(plastabi, "name",  "numeric"))
		{
			return PX_FALSE;
		}
		if (!PX_AbiSet_string(plastabi, "type", "float"))
		{
			return PX_FALSE;
		}
		return PX_TRUE;
	}
	PX_ASSERT();
	return PX_FALSE;
}

px_bool PX_Syntax_Load_numeric(PX_Syntax* pSyntax)
{
	if (!PX_Syntax_Parse_PEBNF(pSyntax, "const_float = *", PX_Syntax_Parse_float))
		return PX_FALSE;

	if (!PX_Syntax_Parse_PEBNF(pSyntax, "const_int = *", PX_Syntax_Parse_int))
		return PX_FALSE;

	if (!PX_Syntax_Parse_PEBNF(pSyntax, "numeric = const_int", PX_Syntax_Parse_numeric))
		return PX_FALSE;

	if (!PX_Syntax_Parse_PEBNF(pSyntax, "numeric = const_float", PX_Syntax_Parse_numeric))
		return PX_FALSE;

	return PX_TRUE;
}