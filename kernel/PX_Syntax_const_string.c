#include "PX_Syntax_const_string.h"

PX_SYNTAX_FUNCTION(PX_Syntax_Parse_const_string)
{
	px_abi* pnewabi;
	px_string str;
	px_char nextchar;
	px_int line, column;
	const px_char* pstr = PX_SyntaxGetCurrentLexeme(past);
	PX_LEXER_LEXEME_TYPE type = PX_SyntaxGetNextAstFilter(past);
	line = PX_LexerGetCurrentLine(past->lexer_state.plexer);
	column = PX_LexerGetCurrentColumn(past->lexer_state.plexer);
	if (type != PX_LEXER_LEXEME_TYPE_DELIMITER)
	{
		return PX_FALSE;
	}
	pstr = PX_SyntaxGetCurrentLexeme(past);
	if (pstr[0] != '"')
	{
		PX_StringCatFormat2(&pSyntax->message, "%1:%2 Error:Incorrect string syntax.\n", PX_STRINGFORMAT_INT(line), PX_STRINGFORMAT_INT(column));
		return PX_FALSE;
	}
	PX_StringInitialize(pSyntax->mp, &str);

	while (PX_TRUE)
	{
		nextchar = PX_Syntax_GetNextChar(past);
		if (nextchar == '\0' || nextchar == '\n')
		{
			PX_StringCatFormat2(&pSyntax->message, "%1:%2 Error:Unexpected end of string.\n", PX_STRINGFORMAT_INT(line), PX_STRINGFORMAT_INT(column));
			PX_StringFree(&str);
			return PX_FALSE;
		}
		if (nextchar == '"')
		{
			break;
		}
		if (nextchar == '\\')
		{
			nextchar = PX_Syntax_GetNextChar(past);
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
			default:
				line = PX_LexerGetCurrentLine(past->lexer_state.plexer);
				column = PX_LexerGetCurrentColumn(past->lexer_state.plexer);
				PX_StringCatFormat2(&pSyntax->message, "%1:%2 Error:Unknown escape character.\n", PX_STRINGFORMAT_INT(line), PX_STRINGFORMAT_INT(column));
				PX_StringFree(&str);
				return PX_FALSE;
			}
		}
		PX_StringCatChar(&str, nextchar);
	}

	pnewabi = PX_Syntax_PushNewAbi(pSyntax, "const_string", pSyntax->lifetime);
	if (!pnewabi)
	{
		PX_StringCatFormat2(&pSyntax->message, "%1:%2 Error:Memory allocation failed.\n", PX_STRINGFORMAT_INT(line), PX_STRINGFORMAT_INT(column));
		PX_StringFree(&str);
		return PX_FALSE;
	}
	if (!PX_AbiSet_string(pnewabi, "value", str.buffer))
	{
		PX_StringCatFormat2(&pSyntax->message, "%1:%2 Error:Memory allocation failed.\n", PX_STRINGFORMAT_INT(line), PX_STRINGFORMAT_INT(column));
		PX_StringFree(&str);
		return PX_FALSE;
	}
	PX_StringFree(&str);
	return PX_TRUE;
}

px_bool PX_Syntax_Load_const_string(PX_Syntax* pSyntax)
{
	PX_Syntax_Parse_PEBNF(pSyntax, "const_string = *", PX_Syntax_Parse_const_string);
	return PX_TRUE;
}