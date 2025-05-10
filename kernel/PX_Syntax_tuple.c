#include "PX_Syntax_tuple.h"

PX_SYNTAX_FUNCTION(PX_Syntax_Parse_tuple)
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

	if (PX_strequ(pname, "const_int")|| PX_strequ(pname, "const_float") || PX_strequ(pname, "const_string"))
	{
		if (!PX_Syntax_MergeLastAbi(pSyntax, "const_tuple"))
		{
			PX_Syntax_Terminate(pSyntax, "Memory Error");
			return PX_FALSE;
		}
		return PX_TRUE;
	}
	
	PX_ASSERT();
	return PX_FALSE;
}

px_bool PX_Syntax_load_tuple(PX_Syntax* pSyntax)
{

	if (!PX_Syntax_Parse_PEBNF(pSyntax, "const_tuple = const_int", PX_Syntax_Parse_tuple))
		return PX_FALSE;

	if (!PX_Syntax_Parse_PEBNF(pSyntax, "const_tuple = const_float", PX_Syntax_Parse_tuple))
		return PX_FALSE;

	if (!PX_Syntax_Parse_PEBNF(pSyntax, "const_tuple = const_string", PX_Syntax_Parse_tuple))
		return PX_FALSE;


	return PX_TRUE;
}