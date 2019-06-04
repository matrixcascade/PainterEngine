#include "PX_Grammar.h"

static PX_LEXER_LEXEME_TYPE PX_GrammarLexerNext(PX_ASM_Grammar *grammar)
{
	PX_LEXER_LEXEME_TYPE type;
	while(PX_TRUE)
	{
		type=PX_LexerGetNextLexeme(grammar->lexer);
		if (type==PX_LEXER_LEXEME_TYPE_SPACER)
		{
			if (grammar->bSkipSpacer)
			{
				continue;
			}
			else
				break;;
		}
		if (type==PX_LEXER_LEXEME_TYPE_NEWLINE)
		{
			if (grammar->bSkipNewline)
			{
				continue;
			}
			else
				break;;
		}
	}
	return type;
}

