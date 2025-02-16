#include "PX_Syntax_statement.h"

PX_SYNTAX_FUNCTION(PX_Syntax_Parse_STATEMENT_OTHERWISE)
{
	PX_LexerSetState(past->lexer_state);
	if (PX_LexerIsEnd(past->lexer_state.plexer))
	{
		//end of file£¬pop lexer
		px_lexer* plexer = PX_Syntax_GetCurrentLexer(pSyntax);
		PX_StringFormat1(&pSyntax->message, "CompileDone:%1\n", PX_STRINGFORMAT_STRING(PX_LexerGetName(plexer)));
		PX_Syntax_PopLexer(pSyntax);
		return PX_TRUE;
	}
	else
	{
		PX_StringFormat2(&pSyntax->message, "%1:%2 Error:Unexpected end of file.\n", \
			PX_STRINGFORMAT_INT(PX_LexerGetCurrentLine(past->lexer_state.plexer)), \
			PX_STRINGFORMAT_INT(PX_LexerGetCurrentColumn(past->lexer_state.plexer)));
		PX_Syntax_PushOpcode(pSyntax, "terminate");
		return PX_FALSE;
	}

}


PX_SYNTAX_FUNCTION(PX_Syntax_Parse_STATEMENT)
{
	return PX_TRUE;
}

px_bool PX_Syntax_Load_statement(PX_Syntax* pSyntax)
{
	if (!PX_Syntax_Parse_PEBNF(pSyntax, "STATEMENT = BLOCK", PX_Syntax_Parse_STATEMENT))
		return PX_FALSE;

	if (!PX_Syntax_Parse_PEBNF(pSyntax, "STATEMENT = *", PX_Syntax_Parse_STATEMENT_OTHERWISE))


		if (!PX_Syntax_Parse_PEBNF(pSyntax, "STATEMENT = BLOCK STATEMENT", PX_Syntax_Parse_STATEMENT))
			return PX_FALSE;

	return PX_TRUE;
}

