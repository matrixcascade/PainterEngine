#include "PX_Syntax_identifier.h"

PX_SYNTAX_FUNCTION(PX_Syntax_Parse_identifier_notkeyword)
{
	PX_Syntax_AstReturn(pSyntax, past,"identifier should not be keyword.");
	return PX_FALSE;
}

PX_SYNTAX_FUNCTION(PX_Syntax_Parse_identifier)
{
	px_abi* pnewabi;
	PX_SYNTAXLEXER_LEXEME_TYPE type = PX_Syntax_GetNextLexeme(pSyntax);
	const px_char* pstr = PX_Syntax_GetCurrentLexeme(pSyntax);
	if (type != PX_SYNTAXLEXER_LEXEME_TYPE_TOKEN)
	{
		return PX_FALSE;
	}
	if (!PX_Syntax_IsValidToken(pstr))
	{
		return PX_FALSE;
	}

	pnewabi = PX_Syntax_NewAbi(pSyntax, "identifier", pSyntax->reg_lifetime);
	if (!pnewabi)
	{
		PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_Parse_identifier Memory Error1");
		return PX_FALSE;
	}
	if (!PX_AbiSet_string(pnewabi, "value", pstr))
	{
		PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_Parse_identifier Memory Error2");
		return PX_FALSE;
	}


	if (!PX_AbiSet_int(pnewabi, "source_index", PX_Syntax_GetCurrentSourceIndex(pSyntax)))
	{
		PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_Parse_identifier Memory Error3");
		return PX_FALSE;
	}

	if (!PX_AbiSet_int(pnewabi, "begin", PX_Syntax_GetCurrentLexemeBegin(pSyntax)))
	{
		PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_Parse_identifier Memory Error3");
		return PX_FALSE;
	}

	if (!PX_AbiSet_int(pnewabi, "end", PX_Syntax_GetCurrentLexemeEnd(pSyntax)))
	{
		PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_Parse_identifier Memory Error4");
		return PX_FALSE;
	}



	return PX_TRUE;

}

px_bool PX_Syntax_load_identifier(PX_Syntax* pSyntax)
{
	PX_Syntax_Parse_PEBNF(pSyntax, "identifier= keyword",0, PX_Syntax_Parse_identifier_notkeyword, 0);
	PX_Syntax_Parse_PEBNF(pSyntax, "identifier= *",0, PX_Syntax_Parse_identifier, 0);
	return PX_TRUE;
}