#include "PX_Syntax_const_float.h"

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
		next_char = PX_Syntax_GetNextChar(past);
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
		if(!(pnewabi=PX_Syntax_PushNewAbi(pSyntax, "const_float",pSyntax->lifetime)))
		{
			PX_Syntax_Terminate(pSyntax, past, "Memory Error");
			return PX_FALSE;
		}
		if(!PX_AbiWrite_string(pnewabi,"value",build_number))
		{
			PX_Syntax_Terminate(pSyntax, past, "Memory Error");
			return PX_FALSE;
		}
		PX_Syntax_AstMessage(pSyntax, "const_float:");
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
	next_char_dot = PX_Syntax_GetNextChar(past);
	
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
		if (!(pnewabi = PX_Syntax_PushNewAbi(pSyntax, "const_float", pSyntax->lifetime)))
		{
			PX_Syntax_Terminate(pSyntax, past, "Memory Error");
			return PX_FALSE;
		}
		if (!PX_AbiWrite_string(pnewabi, "value", build_number))
		{
			PX_Syntax_Terminate(pSyntax, past, "Memory Error");
			return PX_FALSE;
		}
		PX_Syntax_AstMessage(pSyntax, "const_float:");
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
		if (!(pnewabi = PX_Syntax_PushNewAbi(pSyntax, "const_float", pSyntax->lifetime)))
		{
			PX_Syntax_Terminate(pSyntax, past, "Memory Error");
			return PX_FALSE;
		}
		if (!PX_AbiWrite_string(pnewabi, "value", build_number))
		{
			PX_Syntax_Terminate(pSyntax, past, "Memory Error");
			return PX_FALSE;
		}
		PX_Syntax_AstMessage(pSyntax, "const_float:");
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
		if (!(pnewabi = PX_Syntax_PushNewAbi(pSyntax, "const_float", pSyntax->lifetime)))
		{
			PX_Syntax_Terminate(pSyntax, past, "Memory Error");
			return PX_FALSE;
		}
		if (!PX_AbiWrite_string(pnewabi, "value", build_number))
		{
			PX_Syntax_Terminate(pSyntax, past, "Memory Error");
			return PX_FALSE;
		}
		PX_Syntax_AstMessage(pSyntax, "const_float:");
		PX_Syntax_AstMessage(pSyntax, build_number);
		PX_Syntax_AstMessage(pSyntax, "\n");
		return PX_TRUE;
	}
	return PX_FALSE;
}

px_bool PX_Syntax_Load_const_float(PX_Syntax* pSyntax)
{
	if (!PX_Syntax_Parse_PEBNF(pSyntax, "const_float = *", PX_Syntax_Parse_float))
		return PX_FALSE;

	return PX_TRUE;
}