#include "PX_Syntax_base_type.h"

PX_SYNTAX_FUNCTION(PX_Syntax_base_type_exec)
{
	px_int count = PX_Syntax_GetAbiCount(pSyntax);
	px_int i;
	PX_LEXER_LEXEME_TYPE type = PX_Syntax_GetNextLexeme(pSyntax);
	if (type!=PX_LEXER_LEXEME_TYPE_TOKEN)
	{
		return PX_FALSE;
	}
	for (i = 1; i <= count; i++)
	{
		px_abi merge_abi;
		px_abi* pabi = PX_Syntax_GetAbiByIndex(pSyntax, -i);
		if (PX_Syntax_CheckAbiName(pabi,"type_define"))
		{
			const px_char* ptype = PX_Syntax_GetCurrentLexeme(pSyntax);
			const px_char* pabitype = PX_AbiGet_string(pabi, "mnemonic");
			merge_abi = *pabi;
			if (PX_strequ(ptype, pabitype))
			{
				px_abi* newabi = PX_Syntax_NewAbi(pSyntax, "type", pSyntax->reg_lifetime);
				if (!newabi)
				{
					PX_Syntax_Terminate(pSyntax, "Memory Error");
					return PX_FALSE;
				}
				if (!PX_AbiSet_Abi(newabi,"type_define", &merge_abi))
				{
					PX_Syntax_Terminate(pSyntax, "Memory Error");
					return PX_FALSE;
				}
				return PX_TRUE;
			}
		}
	}
	return PX_FALSE;
}



px_bool PX_Syntax_load_base_type(PX_Syntax* pSyntax)
{
	if (!PX_Syntax_Parse_PEBNF(pSyntax, "type = *", PX_Syntax_base_type_exec))
		return PX_FALSE;

	return PX_TRUE;
}
