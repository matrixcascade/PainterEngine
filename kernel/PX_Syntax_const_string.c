#include "PX_Syntax_const_string.h"

PX_SYNTAX_FUNCTION(PX_Syntax_Parse_const_string)
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
	if (pstr[0] != '"')
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
		if (nextchar == '"')
		{
			end = PX_Syntax_GetCurrentLexemeEnd(pSyntax);
			break;
		}
		if (nextchar == '\\')
		{
			nextchar = PX_Syntax_GetNextChar(pSyntax);
			switch (nextchar)
			{
			case 'n':
				nextchar = '\n';
				break;
			case 't':
				nextchar = '\t';
				break;
			case 'r':
				nextchar = '\r';
				break;
			case '0':
				nextchar = '\0';
				break;
			case '\\':
				nextchar = '\\';
				break;
			case '"':
				nextchar = '"';
				break;
			case 'x':
			{
				px_char hex[3] = { 0 };
				hex[0] = PX_Syntax_GetNextChar(pSyntax);
				hex[1] = PX_Syntax_GetNextChar(pSyntax);
				if (hex[0] == '\0' || hex[1] == '\0')
				{
					PX_Syntax_Terminate(pSyntax, "Syntax Error:unexpected end of source");
					PX_StringFree(&str);
					return PX_FALSE;
				}
				PX_HexStringToBuffer(hex,(px_byte *) & nextchar);
			}
			default:
				PX_Syntax_Terminate(pSyntax, "Syntax Error:unexpected escape character");
				PX_StringFree(&str);
				return PX_FALSE;
			}
		}
		PX_StringCatChar(&str, nextchar);
	}

	pnewabi = PX_Syntax_NewAbi(pSyntax, "const_string", pSyntax->reg_lifetime);
	if (!pnewabi)
	{
		PX_Syntax_Terminate(pSyntax, "Memory Error");
		PX_StringFree(&str);
		return PX_FALSE;
	}
	if (!PX_AbiSet_string(pnewabi, "value", str.buffer))
	{
		PX_Syntax_Terminate(pSyntax, "Memory Error");
		PX_StringFree(&str);
		return PX_FALSE;
	}

	if (!PX_AbiSet_int(pnewabi, "begin", begin))
	{
		PX_Syntax_Terminate(pSyntax, "Memory Error");
		PX_StringFree(&str);
		return PX_FALSE;
	}

	if (!PX_AbiSet_int(pnewabi, "end", end))
	{
		PX_Syntax_Terminate(pSyntax, "Memory Error");
		PX_StringFree(&str);
		return PX_FALSE;
	}

	PX_StringFree(&str);
	return PX_TRUE;
}

px_bool PX_Syntax_load_const_string(PX_Syntax* pSyntax)
{
	PX_Syntax_Parse_PEBNF(pSyntax, "const_string = *", PX_Syntax_Parse_const_string);
	return PX_TRUE;
}