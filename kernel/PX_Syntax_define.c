#include "PX_Syntax_define.h"


PX_SYNTAX_FUNCTION(PX_Syntax_Parse_DEFINE_BEGIN)
{
	px_abi* pnewabi;
	px_int line, column;
	PX_LEXER_LEXEME_TYPE type = PX_SyntaxGetNextAstFilter(past);
	const px_char* pstr = PX_SyntaxGetCurrentLexeme(past);
	line = PX_LexerGetCurrentLine(past->lexer_state.plexer);
	column = PX_LexerGetCurrentColumn(past->lexer_state.plexer);
	if (type != PX_LEXER_LEXEME_TYPE_TOKEN)
	{
		return PX_FALSE;
	}
	if (!PX_strequ(pstr, "#define"))
	{
		return PX_FALSE;
	}
	pnewabi = PX_Syntax_NewAbi(pSyntax, "predefine", pSyntax->lifetime);
	if (!pnewabi)
	{
		PX_StringCatFormat2(&pSyntax->message, "%1:%2 Error:Memory allocation failed.\n", PX_STRINGFORMAT_INT(line), PX_STRINGFORMAT_INT(column));
		return PX_FALSE;
	}
	if (!PX_Syntax_PushAbiStack(pSyntax, pnewabi))
	{
		PX_AbiFree(pnewabi);
		return PX_FALSE;
	}
	pnewabi = PX_Syntax_GetAbiStack(pSyntax, -1);
	if (!pnewabi)
	{
		PX_ASSERT();
		return PX_FALSE;
	}
	if (!PX_AbiSet_int(pnewabi, "paramcount", 0))
	{
		PX_AbiFree(pnewabi);
		return PX_FALSE;
	}

	return PX_TRUE;
}

PX_SYNTAX_FUNCTION(PX_Syntax_Parse_DEFINE_NEWPARAMETER)
{
	px_abi* pabi;
	px_int* pparamcount;
	px_int line, column;
	px_char px[8] = { 0 };
	PX_LEXER_LEXEME_TYPE type = PX_SyntaxGetNextAstFilter(past);
	const px_char* pstr = PX_SyntaxGetCurrentLexeme(past);
	line = PX_LexerGetCurrentLine(past->lexer_state.plexer);
	column = PX_LexerGetCurrentColumn(past->lexer_state.plexer);
	if (type != PX_LEXER_LEXEME_TYPE_TOKEN)
	{
		return PX_FALSE;
	}
	pabi = PX_Syntax_GetAbiStack(pSyntax, -1);
	if (!pabi)
	{
		PX_ASSERT();
		return PX_FALSE;
	}
	if (!PX_strequ(PX_AbiGet_string(pabi, "name"), "define"))
	{
		PX_StringCatFormat2(&pSyntax->message, "%1:%2 Error:Incorrect predefine syntax.\n", PX_STRINGFORMAT_INT(line), PX_STRINGFORMAT_INT(column));
		PX_ASSERT();
		return PX_FALSE;
	}
	pparamcount = PX_AbiGet_int(pabi, "paramcount");
	if (!pparamcount)
	{
		//out of memory
		PX_ASSERT();
		return PX_FALSE;
	}
	PX_sprintf1(px, sizeof(px), "p%1", PX_STRINGFORMAT_INT(*pparamcount + 1));
	if (!PX_AbiSet_string(pabi, px, pstr))
	{
		PX_ASSERT();
		return PX_FALSE;
	}
	PX_AbiSet_int(pabi, "paramcount", *pparamcount + 1);
	return PX_TRUE;
}

PX_SYNTAX_FUNCTION(PX_Syntax_Parse_DEFINE_EXPAND)
{
	px_abi* pvalueabi = PX_Syntax_GetAbiStack(pSyntax, -1);
	px_string str;
	if (!PX_StringInitialize(pSyntax->mp, &str))
	{
		PX_ASSERT();
		PX_Syntax_PushOpcode(pSyntax, "terminate");
		PX_StringCatFormat2(&pSyntax->message, "%1:%2 Error:Memory allocation failed.\n", \
			PX_STRINGFORMAT_INT(PX_LexerGetCurrentLine(past->lexer_state.plexer)), \
			PX_STRINGFORMAT_INT(PX_LexerGetCurrentColumn(past->lexer_state.plexer)));
		return PX_FALSE;
	}
	while (PX_TRUE)
	{
		px_char ch = PX_LexerGetNextChar(past->lexer_state.plexer);
		if (ch == '\r')
		{
			ch = '\n';
		}
		if (ch == '\\')
		{
			PX_LEXER_STATE state = PX_LexerGetState(past->lexer_state.plexer);
			ch = PX_LexerGetNextChar(past->lexer_state.plexer);
			if (ch == '\n')
			{
				continue;
			}
			else
			{
				PX_LexerSetState(state);
			}
		}
		if (ch == '\n' || ch == '\0')
		{
			break;
		}
		if (!PX_StringCatChar(&str, ch))
		{
			PX_ASSERT();
			PX_StringFree(&str);
			PX_Syntax_PushOpcode(pSyntax, "terminate");
			PX_StringCatFormat2(&pSyntax->message, "%1:%2 Error:Memory allocation failed.\n", \
				PX_STRINGFORMAT_INT(PX_LexerGetCurrentLine(past->lexer_state.plexer)), \
				PX_STRINGFORMAT_INT(PX_LexerGetCurrentColumn(past->lexer_state.plexer)));
			return PX_FALSE;
		}
	}


	if (!PX_AbiSet_string(pvalueabi, "expand", str.buffer))
	{
		PX_ASSERT();
		PX_StringFree(&str);
		PX_Syntax_PushOpcode(pSyntax, "terminate");
		PX_StringCatFormat2(&pSyntax->message, "%1:%2 Error:Memory allocation failed.\n", \
			PX_STRINGFORMAT_INT(PX_LexerGetCurrentLine(past->lexer_state.plexer)), \
			PX_STRINGFORMAT_INT(PX_LexerGetCurrentColumn(past->lexer_state.plexer)));
		return PX_FALSE;
	}
	PX_StringFree(&str);
	return PX_TRUE;
}

PX_SYNTAX_FUNCTION(PX_Syntax_Parse_DEFINE_ERROR)
{
	PX_StringCatFormat2(&pSyntax->message, "%1:%2 Error:Incorrect predefine syntax.\n", \
		PX_STRINGFORMAT_INT(PX_LexerGetCurrentLine(past->lexer_state.plexer)), \
		PX_STRINGFORMAT_INT(PX_LexerGetCurrentColumn(past->lexer_state.plexer)));
	return PX_TRUE;
}

px_bool PX_Syntax_Load_define(PX_Syntax* pSyntax)
{
	PX_Syntax_Parse_PEBNF(pSyntax, "DEFINE_PARAMETER = $", PX_Syntax_Parse_DEFINE_NEWPARAMETER);
	PX_Syntax_Parse_PEBNF(pSyntax, "DEFINE_PARAMETERS = DEFINE_PARAMETER", PX_Syntax_Parse_DEFINE_NEWPARAMETER);
	PX_Syntax_Parse_PEBNF(pSyntax, "DEFINE_PARAMETERS = DEFINE_PARAMETER ',' DEFINE_PARAMETERS", PX_Syntax_Parse_DEFINE_NEWPARAMETER);
	PX_Syntax_Parse_PEBNF(pSyntax, "DEFINE_PARAMETERS = *", PX_Syntax_Parse_DEFINE_NEWPARAMETER);//none
	PX_Syntax_Parse_PEBNF(pSyntax, "DEFINE = '#define'", PX_Syntax_Parse_DEFINE_BEGIN);
	PX_Syntax_Parse_PEBNF(pSyntax, "DEFINE = '#define' '(' DEFINE_PARAMETERS ')' EXPAND", PX_Syntax_Parse_DEFINE_EXPAND);
	PX_Syntax_Parse_PEBNF(pSyntax, "DEFINE = '#define' DEFINE_PARAMETER  EXPAND", PX_Syntax_Parse_DEFINE_EXPAND);
	PX_Syntax_Parse_PEBNF(pSyntax, "DEFINE = '#define' *", PX_Syntax_Parse_DEFINE_ERROR);
	return PX_TRUE;
}
