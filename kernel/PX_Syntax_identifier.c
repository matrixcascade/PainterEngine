#include "PX_Syntax_identifier.h"

PX_SYNTAX_FUNCTION(PX_Syntax_Parse_identifier_notkeyword)
{
	PX_Syntax_AstReturn(pSyntax, past,"identifier should not be keyword.");
	return PX_FALSE;
}

PX_SYNTAX_FUNCTION(PX_Syntax_Parse_identifier)
{
	px_abi* pnewabi;
	PX_LEXER_LEXEME_TYPE type = PX_Syntax_GetNextLexeme(pSyntax);
	const px_char* pstr = PX_Syntax_GetCurrentLexeme(pSyntax);
	if (type != PX_LEXER_LEXEME_TYPE_TOKEN)
	{
		return PX_FALSE;
	}
	if (!PX_Syntax_IsValidToken(pstr))
	{
		PX_Syntax_Terminate(pSyntax, "Syntax Error:invalid identifier");
		return PX_FALSE;

	}

	pnewabi = PX_Syntax_NewAbi(pSyntax, "identifier", pSyntax->reg_lifetime);
	if (!pnewabi)
	{
		PX_Syntax_Terminate(pSyntax, "Memory Error");
		return PX_FALSE;
	}
	if (!PX_AbiSet_string(pnewabi, "value", pstr))
	{
		PX_Syntax_Terminate(pSyntax, "Memory Error");
		return PX_FALSE;
	}

	if (!PX_AbiSet_int(pnewabi, "begin", PX_Syntax_GetCurrentLexemeBegin(pSyntax)))
	{
		PX_Syntax_Terminate(pSyntax, "Memory Error");
		return PX_FALSE;
	}

	if (!PX_AbiSet_int(pnewabi, "end", PX_Syntax_GetCurrentLexemeEnd(pSyntax)))
	{
		PX_Syntax_Terminate(pSyntax, "Memory Error");
		return PX_FALSE;
	}
	return PX_TRUE;

}

px_bool PX_Syntax_load_identifier(PX_Syntax* pSyntax)
{
	PX_Syntax_Parse_PEBNF(pSyntax, "identifier= keyword", PX_Syntax_Parse_identifier_notkeyword);
	PX_Syntax_Parse_PEBNF(pSyntax, "identifier= *", PX_Syntax_Parse_identifier);
	return PX_TRUE;
}