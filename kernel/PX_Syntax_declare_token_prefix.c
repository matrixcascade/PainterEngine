#include "PX_Syntax_declare_token_prefix.h"

PX_SYNTAX_FUNCTION(PX_Syntax_declare_token_prefix)
{
	PX_LEXER_LEXEME_TYPE type = PX_Syntax_GetNextLexeme(pSyntax);
	if (type== PX_LEXER_LEXEME_TYPE_DELIMITER)
	{
		if (*PX_Syntax_GetCurrentLexeme(pSyntax) == '*')
		{
			px_int ptr_level = 1;
			px_char nextchar;
			px_abi* newabi;
			while (PX_TRUE)
			{
				nextchar = PX_Syntax_PreviewNextChar(pSyntax);
				if (nextchar=='*')
				{
					PX_Syntax_GetNextChar(pSyntax);
					ptr_level++;
				}
				else
				{
					break;
				}
			}
			newabi= PX_Syntax_NewAbi(pSyntax, "declare_token_prefix", pSyntax->reg_lifetime);
			if (!newabi)
			{
				PX_Syntax_Terminate(pSyntax, "Memory Error");
				return PX_FALSE;
			}
			if (!PX_AbiSet_string(newabi, "type", "pointer"))
			{
				PX_AbiFree(newabi);
				PX_Syntax_Terminate(pSyntax, "Memory Error");
				return PX_FALSE;
			}
			if (!PX_AbiSet_int(newabi, "pointer_level", ptr_level))
			{
				PX_AbiFree(newabi);
				PX_Syntax_Terminate(pSyntax, "Memory Error");
				return PX_FALSE;
			}
			return PX_TRUE;
		}
	}
	return PX_FALSE;
}


px_bool PX_Syntax_load_declare_token_prefix(PX_Syntax* pSyntax)
{
	if (!PX_Syntax_Parse_PEBNF(pSyntax, "declare_token_prefix = *", PX_Syntax_declare_token_prefix))
		return PX_FALSE;

	return PX_TRUE;
}
