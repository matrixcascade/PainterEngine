#include "PX_Syntax_keyword.h"

PX_SYNTAX_FUNCTION(PX_Syntax_Parse_keyword)
{
	const px_char* PX_Script_Keywords[] = { "if","else","switch","case","default",\
		"while","for","do","break","continue","return","goto","typedef",\
		"extern","static","auto","register","const","volatile","sizeof",\
		"enum","struct","union","void","char","short","int","long","float",\
		"double","signed","unsigned","_Bool","_Complex","_Imaginary","inline",\
		"restrict","_Alignas","_Alignof","_Atomic","_Generic","_Noreturn",\
		"_Static_assert","_Thread_local" };

	PX_LEXER_LEXEME_TYPE type = PX_SyntaxGetNextAstFilter(past);
	const px_char* pstr = PX_SyntaxGetCurrentLexeme(past);
	px_int i;
	if (type != PX_LEXER_LEXEME_TYPE_TOKEN)
	{
		return PX_FALSE;
	}
	for (i = 0; i < sizeof(PX_Script_Keywords) / sizeof(px_char*); i++)
	{
		if (PX_strequ(pstr, PX_Script_Keywords[i]))
		{
			return PX_TRUE;
		}
	}
	return PX_FALSE;
}

px_bool PX_Syntax_Load_keyword(PX_Syntax* pSyntax)
{
	PX_Syntax_Parse_PEBNF(pSyntax, "keyword= *", PX_Syntax_Parse_keyword);
	return PX_TRUE;
}

