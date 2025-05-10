#include "PX_Syntax_numeric.h"

PX_SYNTAX_FUNCTION(PX_Syntax_Parse_numeric)
{
	px_abi* plastabi;
	const px_char* pname;
	plastabi = PX_Syntax_GetAbiByIndex(pSyntax, -1);
	if (!plastabi)
	{
		PX_ASSERT();
		return PX_FALSE;
	}
	pname = PX_AbiGet_string(plastabi, "name");
	if (!pname)
	{
		PX_ASSERT();
		return PX_FALSE;
	}

	if (PX_strequ(pname, "const_int")|| PX_strequ(pname, "const_float"))
	{
		if (!PX_Syntax_MergeLastAbi(pSyntax, "const_numeric"))
		{
			PX_Syntax_Terminate(pSyntax, "Memory Error");
			return PX_FALSE;
		}
		return PX_TRUE;
	}
	
	PX_ASSERT();
	return PX_FALSE;
}

px_bool PX_Syntax_load_numeric(PX_Syntax* pSyntax)
{

	if (!PX_Syntax_Parse_PEBNF(pSyntax, "const_numeric = const_int", PX_Syntax_Parse_numeric))
		return PX_FALSE;

	if (!PX_Syntax_Parse_PEBNF(pSyntax, "const_numeric = const_float", PX_Syntax_Parse_numeric))
		return PX_FALSE;

	return PX_TRUE;
}