#include "PX_Syntax_comment.h"

PX_SYNTAX_FUNCTION(PX_Syntax_comment)
{
	px_int begin_source_index, begin;
	px_int end_source_index, end;
	PX_SYNTAXLEXER_LEXEME_TYPE type = PX_Syntax_GetNextLexeme(pSyntax);
	if (type == PX_SYNTAXLEXER_LEXEME_TYPE_DELIMITER)
	{
		const px_char* plexeme = PX_Syntax_GetCurrentLexeme(pSyntax);
		if (plexeme[0] == '/')
		{
			px_char nextchar = PX_Syntax_PreviewNextChar(pSyntax);
			if (nextchar == '/')
			{
				// line comment //
				begin_source_index = PX_Syntax_GetCurrentLexemeBeginSourceIndex(pSyntax);
				begin = PX_Syntax_GetCurrentLexemeBegin(pSyntax);
				PX_Syntax_LexerForward(pSyntax,1); // consume the second '/'
				while (PX_TRUE)
				{
					nextchar = PX_Syntax_PreviewNextChar(pSyntax);
					if (nextchar == '\n' || nextchar == '\0')
					{
						break;
					}
					PX_Syntax_LexerForward(pSyntax,1);
				}
				end_source_index = PX_Syntax_GetCurrentSourceIndex(pSyntax);
				end = PX_Syntax_GetCurrentLexerOffset(pSyntax);
				PX_Syntax_NewMapToken(pSyntax, begin_source_index, begin, end_source_index, end, PX_COLOR(255, 128, 128, 128), 0);
				return PX_TRUE;
			}
			else if (nextchar == '*')
			{
				// block comment /* */
				begin_source_index = PX_Syntax_GetCurrentLexemeBeginSourceIndex(pSyntax);
				begin = PX_Syntax_GetCurrentLexemeBegin(pSyntax);
				PX_Syntax_LexerForward(pSyntax,1); // consume the '*'
				while (PX_TRUE)
				{
					nextchar = PX_Syntax_GetNextChar(pSyntax);
					if (nextchar == '\0')
					{
						PX_Syntax_Terminate(pSyntax, "ast:error:unterminated block comment, expected */");
						return PX_FALSE;
					}
					if (nextchar == '*')
					{
						nextchar = PX_Syntax_PreviewNextChar(pSyntax);
						if (nextchar == '/')
						{
							PX_Syntax_LexerForward(pSyntax,1); // consume the '/'
							break;
						}
					}
				}
				end_source_index = PX_Syntax_GetCurrentSourceIndex(pSyntax);
				end = PX_Syntax_GetCurrentLexerOffset(pSyntax);
				PX_Syntax_NewMapToken(pSyntax, begin_source_index, begin, end_source_index, end, PX_COLOR(255, 128, 128, 128), 0);
				return PX_TRUE;
			}
		}
	}
	return PX_FALSE;
}

px_bool PX_Syntax_load_comment(PX_Syntax* pSyntax)
{
	if (!PX_Syntax_Parse_PEBNF(pSyntax, "comment = *", 0, PX_Syntax_comment, 0))
		return PX_FALSE;

	return PX_TRUE;
}