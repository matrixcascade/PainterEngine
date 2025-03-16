#include "PX_Syntax_const_int.h"

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
		if (PX_strlen(plexeme) > 20)
		{
			return PX_FALSE;
		}
		PX_strcat_s(build_number, sizeof(build_number), plexeme);
		next_char_is_dot = PX_Syntax_PreviewNextChar(past);
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
		if (!(pnewabi = PX_Syntax_PushNewAbi(pSyntax, "const_int", pSyntax->lifetime)))
		{
			PX_Syntax_Terminate(pSyntax, past, "Memory Error");
			return PX_FALSE;
		}
		if (!PX_AbiWrite_string(pnewabi, "value", build_number))
		{
			PX_Syntax_Terminate(pSyntax, past, "Memory Error");
			return PX_FALSE;
		}
		PX_Syntax_AstMessage(pSyntax, "const_int:");
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
			if (!(pnewabi = PX_Syntax_PushNewAbi(pSyntax, "const_int", pSyntax->lifetime)))
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
			PX_Syntax_AstMessage(pSyntax, "const_int:");
			PX_Syntax_AstMessage(pSyntax, build_number);
			PX_Syntax_AstMessage(pSyntax, "\n");
			return PX_TRUE;

		}
	}

	//0xxxxxxxxx
	if (plexeme[0] == '0' && (plexeme[1] == 'x' || plexeme[1] == 'X'))
	{
		px_dword value;
		if (!(pnewabi = PX_Syntax_PushNewAbi(pSyntax, "int", pSyntax->lifetime)))
		{
			PX_Syntax_Terminate(pSyntax, past, "Memory Error");
			return PX_FALSE;
		}
		if (PX_strlen(plexeme+2)>8|| PX_strlen(plexeme + 2)==0)
		{
			PX_Syntax_Terminate(pSyntax, past, "illegal hex number");
			return PX_FALSE;
		}
		value = PX_htoi(plexeme + 2);
		PX_utoa(value, build_number, sizeof(build_number), 10);
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
		PX_Syntax_AstMessage(pSyntax, "const_uint:");
		PX_Syntax_AstMessage(pSyntax, build_number);
		PX_Syntax_AstMessage(pSyntax, "\n");
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
		PX_Syntax_Terminate(pSyntax, past, "Numeric too long");
		return PX_FALSE;
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
		if (!(pnewabi = PX_Syntax_PushNewAbi(pSyntax, "const_int", pSyntax->lifetime)))
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
		
		PX_Syntax_AstMessage(pSyntax, "const_uint:");
		PX_Syntax_AstMessage(pSyntax, build_number);
		PX_Syntax_AstMessage(pSyntax, "\n");
		return PX_TRUE;
	}


	return PX_FALSE;
}

px_bool PX_Syntax_Load_const_int(PX_Syntax* pSyntax)
{
	if (!PX_Syntax_Parse_PEBNF(pSyntax, "const_int = *", PX_Syntax_Parse_int))
		return PX_FALSE;

	return PX_TRUE;
}