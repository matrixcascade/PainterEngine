#include "PX_Syntax_const_string.h"

PX_SYNTAX_FUNCTION(PX_Syntax_Parse_const_string)
{
	px_abi* pnewabi;
	px_string str;
	px_char nextchar;
	px_int begin, end, begin_source_index, end_source_index;
	const px_char* pstr = PX_Syntax_GetCurrentLexeme(pSyntax);
	PX_SYNTAXLEXER_LEXEME_TYPE type = PX_Syntax_GetNextLexeme(pSyntax);
	begin_source_index = PX_Syntax_GetCurrentLexemeBeginSourceIndex(pSyntax);
	end_source_index = PX_Syntax_GetCurrentLexemeEndSourceIndex(pSyntax);
	if (type != PX_SYNTAXLEXER_LEXEME_TYPE_DELIMITER)
	{
		return PX_FALSE;
	}
	if (begin_source_index != end_source_index)
	{
		PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_Parse_const_string Memory Error1");
		return PX_FALSE;
	}
	pstr = PX_Syntax_GetCurrentLexeme(pSyntax);
	if (pstr[0] != '"')
	{
		return PX_FALSE;
	}
	begin = PX_Syntax_GetCurrentLexemeBegin(pSyntax);
	PX_StringInitialize(pSyntax->mp, &str);

	while (PX_TRUE)
	{
		nextchar = PX_Syntax_GetNextChar(pSyntax);
		if (nextchar == '\0' || nextchar == '\n')
		{
			PX_StringFree(&str);
			return PX_FALSE;
		}
		if (nextchar == '"')
		{
			end = PX_Syntax_GetCurrentLexemeEnd(pSyntax);
			break;
		}
		if (nextchar == '\\')
		{
			nextchar = PX_Syntax_GetNextChar(pSyntax);
			switch (nextchar)
			{
			case 'n':
				nextchar = '\n';
				break;
			case 't':
				nextchar = '\t';
				break;
			case 'r':
				nextchar = '\r';
				break;
			case '0':
				nextchar = '\0';
				break;
			case '\\':
				nextchar = '\\';
				break;
			case '"':
				nextchar = '"';
				break;
			case 'x':
			{
				px_char hex[3] = { 0 };
				hex[0] = PX_Syntax_GetNextChar(pSyntax);
				hex[1] = PX_Syntax_GetNextChar(pSyntax);
				if (hex[0] == '\0' || hex[1] == '\0')
				{
					PX_Syntax_Terminate(pSyntax, "ast:error:unexpected end of source");
					PX_StringFree(&str);
					return PX_FALSE;
				}
				PX_HexStringToBuffer(hex,(px_byte *) & nextchar);
			}
			default:
				PX_Syntax_Terminate(pSyntax, "ast:error:unexpected escape character");
				PX_StringFree(&str);
				return PX_FALSE;
			}
		}
		PX_StringCatChar(&str, nextchar);
	}

	pnewabi = PX_Syntax_NewAbi(pSyntax, "const_string", pSyntax->reg_lifetime);
	if (!pnewabi)
	{
		PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_Parse_const_string Memory Error1");
		PX_StringFree(&str);
		return PX_FALSE;
	}
	if (!PX_AbiSet_string(pnewabi, "value", str.buffer))
	{
		PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_Parse_const_string Memory Error2");
		PX_StringFree(&str);
		return PX_FALSE;
	}

	if (!PX_AbiSet_int(pnewabi, "source_index", begin_source_index))
	{
		PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_Parse_const_string Memory Error3");
		PX_StringFree(&str);
		return PX_FALSE;
	}

	if (!PX_AbiSet_int(pnewabi, "begin", begin))
	{
		PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_Parse_const_string Memory Error3");
		PX_StringFree(&str);
		return PX_FALSE;
	}

	if (!PX_AbiSet_int(pnewabi, "end", end))
	{
		PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_Parse_const_string Memory Error4");
		PX_StringFree(&str);
		return PX_FALSE;
	}

	PX_StringFree(&str);

if (!PX_Syntax_NewMapToken(pSyntax, begin_source_index, begin, end_source_index, end, PX_COLOR(255, 236, 166, 120), "array.ix.u.8"))
	{
		PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_Parse_const_string Memory Error5");
		return PX_FALSE;
	}

	return PX_TRUE;
}

px_bool PX_Syntax_load_const_string(PX_Syntax* pSyntax)
{
	PX_Syntax_Parse_PEBNF(pSyntax, "const_string = *",0, PX_Syntax_Parse_const_string, 0);
	return PX_TRUE;
}