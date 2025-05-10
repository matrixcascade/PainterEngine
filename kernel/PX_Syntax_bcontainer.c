#include "PX_Syntax_bcontainer.h"

PX_SYNTAX_FUNCTION(PX_Syntax_Parse_bcontainer)
{
	px_abi* pnewabi;
	px_string str;
	px_char nextchar;
	px_int begin, end;
	const px_char* pstr = PX_Syntax_GetCurrentLexeme(pSyntax);
	PX_LEXER_LEXEME_TYPE type = PX_Syntax_GetNextLexeme(pSyntax);
	if (type != PX_LEXER_LEXEME_TYPE_DELIMITER)
	{
		return PX_FALSE;
	}
	pstr = PX_Syntax_GetCurrentLexeme(pSyntax);
	if (pstr[0] != '<')
	{
		return PX_FALSE;
	}
	begin = PX_Syntax_GetCurrentLexemeBegin(pSyntax);
	
	PX_StringInitialize(pSyntax->mp, &str);

	while (PX_TRUE)
	{
		nextchar = PX_Syntax_GetNextChar(pSyntax);
		if (nextchar == '\0' || nextchar == '\n')
		{
			PX_StringFree(&str);
			return PX_FALSE;
		}
		if (nextchar == '>')
		{
			end = PX_Syntax_GetCurrentLexemeEnd(pSyntax);
			break;
		}

		PX_StringCatChar(&str, nextchar);
	}

	pnewabi = PX_Syntax_NewAbi(pSyntax, "bcontainer", pSyntax->reg_lifetime);
	if (!pnewabi)
	{
		PX_StringFree(&str);
		return PX_FALSE;
	}
	if (!PX_AbiSet_string(pnewabi, "value", str.buffer))
	{
		PX_StringFree(&str);
		return PX_FALSE;
	}
	if (!PX_AbiSet_int(pnewabi, "begin", begin))
	{
		PX_StringFree(&str);
		return PX_FALSE;
	}
	if (!PX_AbiSet_int(pnewabi, "end", end))
	{
		PX_StringFree(&str);
		return PX_FALSE;
	}
	PX_StringFree(&str);
	return PX_TRUE;
}


px_bool PX_Syntax_load_bcontainer(PX_Syntax* pSyntax)
{
	PX_Syntax_Parse_PEBNF(pSyntax, "bcontainer = *", PX_Syntax_Parse_bcontainer);
	return PX_TRUE;
}