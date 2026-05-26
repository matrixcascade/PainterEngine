#include "PX_Syntax_eof.h"

PX_SYNTAX_FUNCTION(PX_Syntax_parse_is_eof)
{
	return PX_Syntax_IsEndOfSource(pSyntax);
}


px_bool PX_Syntax_load_eof(PX_Syntax* pSyntax)
{
	PX_Syntax_Parse_PEBNF(pSyntax, "is_eof = *",0, PX_Syntax_parse_is_eof, 0);
	PX_Syntax_Parse_PEBNF(pSyntax, "eof = *", 0, PX_Syntax_parse_is_eof, 0);
	return PX_TRUE;
}