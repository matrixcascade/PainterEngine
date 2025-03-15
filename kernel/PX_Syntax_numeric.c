#include "PX_Syntax_numeric.h"

PX_SYNTAX_FUNCTION(PX_Syntax_Parse_numeric)
{
	px_abi* plastabi;
	const px_char* pname;
	plastabi = PX_Syntax_GetAbiStack(pSyntax, -1);
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

	if (PX_strequ(pname, "int"))
	{
		if (!PX_AbiSet_string(plastabi, "name",  "numeric"))
		{
			return PX_FALSE;
		}
		if (!PX_AbiSet_string(plastabi, "type",  "int"))
		{
			return PX_FALSE;
		}
		return PX_TRUE;
	}
	if (PX_strequ(pname, "float"))
	{
		if (!PX_AbiSet_string(plastabi, "name",  "numeric"))
		{
			return PX_FALSE;
		}
		if (!PX_AbiSet_string(plastabi, "type", "float"))
		{
			return PX_FALSE;
		}
		return PX_TRUE;
	}
	PX_ASSERT();
	return PX_FALSE;
}

px_bool PX_Syntax_Load_numeric(PX_Syntax* pSyntax)
{

	if (!PX_Syntax_Parse_PEBNF(pSyntax, "numeric = const_int", PX_Syntax_Parse_numeric))
		return PX_FALSE;

	if (!PX_Syntax_Parse_PEBNF(pSyntax, "numeric = const_float", PX_Syntax_Parse_numeric))
		return PX_FALSE;

	return PX_TRUE;
}