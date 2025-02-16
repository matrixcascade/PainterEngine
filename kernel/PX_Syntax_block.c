#include "PX_Syntax_block.h"

PX_SYNTAX_FUNCTION(PX_Syntax_Parse_BLOCK_INCLUDE)
{
	//do noting
	return PX_TRUE;
}

px_bool PX_Syntax_Load_block(PX_Syntax* pSyntax)
{
	if (!PX_Syntax_Parse_PEBNF(pSyntax, "BLOCK = INCLUDE", PX_Syntax_Parse_BLOCK_INCLUDE))
		return PX_FALSE;

	if (!PX_Syntax_Parse_PEBNF(pSyntax, "BLOCK = INCLUDE", PX_Syntax_Parse_BLOCK_INCLUDE))
		return PX_FALSE;
	return PX_TRUE;
}