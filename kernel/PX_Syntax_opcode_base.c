#include "PX_Syntax_opcode_base.h"

static px_char PX_ScriptParseGetOpLevel(const px_char* op, px_bool binary)
{
	if (PX_strlen(op) == 1)
	{
		switch (*op)
		{
		case '(':
		case ')':
		case '[':
		case ']':
		case '.':
			return 1;
		case '!':
		case '~':
			return 2;
		case '+':
		case '-':
		{
			if (binary)
				return 4;
			else
				return 2;
		}
		break;
		case '>':
			return 6;
		case '<':
			return 6;
		case '*':
			if (!binary) return 2;
		case '/':
		case '%':
			return 3;
		case '&':
			if (!binary) return 2;
			return 8;
		case '^':
			return 9;
		case '|':
			return 10;

		case '=':
			return 14;
		case ',':
			return 15;

		}
	}
	else
	{
		if (PX_strequ(op, "->"))
		{
			return 1;
		}
		if (PX_strequ(op, ">="))
		{
			return 6;
		}
		if (PX_strequ(op, "<="))
		{
			return 6;
		}
		if (PX_strequ(op, "<<"))
		{
			return 5;
		}
		if (PX_strequ(op, ">>"))
		{
			return 5;
		}
		if (PX_strequ(op, "&&"))
		{
			return 11;
		}
		if (PX_strequ(op, "||"))
		{
			return 12;
		}
		if (PX_strequ(op, "=="))
		{
			return 7;
		}
		if (PX_strequ(op, "!="))
		{
			return 7;
		}
		if (PX_strequ(op, "++"))
		{
			return 2;
		}
		if (PX_strequ(op, "--"))
		{
			return 2;
		}
	}
	PX_ASSERT();
	return -1;
}
px_bool PX_Syntax_load_opcode_base(PX_Syntax* pSyntax)
{
	if (!PX_Syntax_NewOpcode(pSyntax, "->", PX_SYNTAX_OPCODE_TYPE_BINARY, 1))
	{
		return PX_FALSE;
	}
	if (!PX_Syntax_NewOpcode(pSyntax, "(", PX_SYNTAX_OPCODE_TYPE_BINARY, 1))
	{
		return PX_FALSE;
	}
	if (!PX_Syntax_NewOpcode(pSyntax, ")", PX_SYNTAX_OPCODE_TYPE_BINARY, 1))
	{
		return PX_FALSE;
	}
	if (!PX_Syntax_NewOpcode(pSyntax, "[", PX_SYNTAX_OPCODE_TYPE_BINARY, 1))
	{
		return PX_FALSE;
	}
	if (!PX_Syntax_NewOpcode(pSyntax, "]", PX_SYNTAX_OPCODE_TYPE_BINARY, 1))
	{
		return PX_FALSE;
	}
	if (!PX_Syntax_NewOpcode(pSyntax, ".", PX_SYNTAX_OPCODE_TYPE_BINARY, 1))
	{
		return PX_FALSE;
	}
	if (!PX_Syntax_NewOpcode(pSyntax, "!", PX_SYNTAX_OPCODE_TYPE_UNARY, 2))
	{
		return PX_FALSE;
	}
	if (!PX_Syntax_NewOpcode(pSyntax, "~", PX_SYNTAX_OPCODE_TYPE_UNARY, 2))
	{
		return PX_FALSE;
	}
	if (!PX_Syntax_NewOpcode(pSyntax, "->", PX_SYNTAX_OPCODE_TYPE_UNARY, 2))
	{
		return PX_FALSE;
	}
	if (!PX_Syntax_NewOpcode(pSyntax, "++", PX_SYNTAX_OPCODE_TYPE_UNARY, 2))
	{
		return PX_FALSE;
	}
	if (!PX_Syntax_NewOpcode(pSyntax, "--", PX_SYNTAX_OPCODE_TYPE_UNARY, 2))
	{
		return PX_FALSE;
	}
	if (!PX_Syntax_NewOpcode(pSyntax, "*", PX_SYNTAX_OPCODE_TYPE_BINARY, 3))
	{
		return PX_FALSE;
	}
	if (!PX_Syntax_NewOpcode(pSyntax, "/", PX_SYNTAX_OPCODE_TYPE_BINARY, 3))
	{
		return PX_FALSE;
	}
	if (!PX_Syntax_NewOpcode(pSyntax, "%", PX_SYNTAX_OPCODE_TYPE_BINARY, 3))
	{
		return PX_FALSE;
	}
	if (!PX_Syntax_NewOpcode(pSyntax, "+", PX_SYNTAX_OPCODE_TYPE_BINARY, 4))
	{
		return PX_FALSE;
	}
	if (!PX_Syntax_NewOpcode(pSyntax, "-", PX_SYNTAX_OPCODE_TYPE_BINARY, 4))
	{
		return PX_FALSE;
	}
	if (!PX_Syntax_NewOpcode(pSyntax, "<<", PX_SYNTAX_OPCODE_TYPE_BINARY, 5))
	{
		return PX_FALSE;
	}
	if (!PX_Syntax_NewOpcode(pSyntax, ">>", PX_SYNTAX_OPCODE_TYPE_BINARY, 5))
	{
		return PX_FALSE;
	}
	if (!PX_Syntax_NewOpcode(pSyntax, ">", PX_SYNTAX_OPCODE_TYPE_BINARY, 6))
	{
		return PX_FALSE;
	}
	if (!PX_Syntax_NewOpcode(pSyntax, "<", PX_SYNTAX_OPCODE_TYPE_BINARY, 6))
	{
		return PX_FALSE;
	}
	if (!PX_Syntax_NewOpcode(pSyntax, "==", PX_SYNTAX_OPCODE_TYPE_BINARY, 7))
	{
		return PX_FALSE;
	}
	if (!PX_Syntax_NewOpcode(pSyntax, "!=", PX_SYNTAX_OPCODE_TYPE_BINARY, 7))
	{
		return PX_FALSE;
	}
	if (!PX_Syntax_NewOpcode(pSyntax, "&", PX_SYNTAX_OPCODE_TYPE_BINARY, 8))
	{
		return PX_FALSE;
	}
	if (!PX_Syntax_NewOpcode(pSyntax, "^", PX_SYNTAX_OPCODE_TYPE_BINARY, 9))
	{
		return PX_FALSE;
	}
	if (!PX_Syntax_NewOpcode(pSyntax, "|", PX_SYNTAX_OPCODE_TYPE_BINARY, 10))
	{
		return PX_FALSE;
	}
	if (!PX_Syntax_NewOpcode(pSyntax, "&&", PX_SYNTAX_OPCODE_TYPE_BINARY, 11))
	{
		return PX_FALSE;
	}
	if (!PX_Syntax_NewOpcode(pSyntax, "||", PX_SYNTAX_OPCODE_TYPE_BINARY, 12))
	{
		return PX_FALSE;
	}
	if (!PX_Syntax_NewOpcode(pSyntax, "=", PX_SYNTAX_OPCODE_TYPE_BINARY, 14))
	{
		return PX_FALSE;
	}
	if (!PX_Syntax_NewOpcode(pSyntax, ",", PX_SYNTAX_OPCODE_TYPE_BINARY, 15))
	{
		return PX_FALSE;
	}
	if (!PX_Syntax_NewOpcode(pSyntax, ":", PX_SYNTAX_OPCODE_TYPE_BINARY, 16))
	{
		return PX_FALSE;
	}
	if (!PX_Syntax_NewOpcode(pSyntax, "?", PX_SYNTAX_OPCODE_TYPE_BINARY, 17))
	{
		return PX_FALSE;
	}
	return PX_TRUE;
}
