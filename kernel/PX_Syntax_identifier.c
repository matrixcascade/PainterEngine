#include "PX_Syntax_identifier.h"

PX_SYNTAX_FUNCTION(PX_Syntax_Parse_identifier_notkeyword)
{
	PX_StringCatFormat2(&pSyntax->message, "%1:%2 Error:Key Should.\n", \
		PX_STRINGFORMAT_INT(PX_LexerGetCurrentLine(past->lexer_state.plexer)), \
		PX_STRINGFORMAT_INT(PX_LexerGetCurrentColumn(past->lexer_state.plexer)));
	return PX_TRUE;
}

PX_SYNTAX_FUNCTION(PX_Syntax_Parse_identifier)
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
	if (!PX_Syntax_IsValidToken(pstr))
	{
		PX_StringCatFormat2(&pSyntax->message, "%1:%2 Error:Invalid identifier.\n", PX_STRINGFORMAT_INT(line), PX_STRINGFORMAT_INT(column));
		PX_Syntax_PushOpcode(pSyntax, "terminate");
		return PX_FALSE;

	}

	pnewabi = PX_Syntax_PushNewAbi(pSyntax, "identifier", pSyntax->lifetime);
	if (!pnewabi)
	{
		PX_StringCatFormat2(&pSyntax->message, "%1:%2 Error:Memory allocation failed.\n", PX_STRINGFORMAT_INT(line), PX_STRINGFORMAT_INT(column));
		return PX_FALSE;
	}
	if (!PX_AbiSet_string(pnewabi, "value", pstr))
	{
		PX_StringCatFormat2(&pSyntax->message, "%1:%2 Error:Memory allocation failed.\n", PX_STRINGFORMAT_INT(line), PX_STRINGFORMAT_INT(column));
		return PX_FALSE;
	}
	return PX_TRUE;

}

px_bool PX_Syntax_Load_identifier(PX_Syntax* pSyntax)
{
	PX_Syntax_Parse_PEBNF(pSyntax, "identifier= keyword", PX_Syntax_Parse_identifier_notkeyword);
	PX_Syntax_Parse_PEBNF(pSyntax, "identifier= *", PX_Syntax_Parse_identifier);
	return PX_TRUE;
}