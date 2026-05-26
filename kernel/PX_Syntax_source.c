#include "PX_Syntax_source.h"

PX_SYNTAX_FUNCTION(PX_Syntax_source_skip_newline_spacer)
{
	px_int counter = 0;
	while (PX_TRUE)
	{
		px_char ch = PX_Syntax_PreviewNextChar(pSyntax);
		if (ch==' '||ch=='\t'||ch=='\r'||ch=='\n')
		{
			PX_Syntax_GetNextChar(pSyntax);
			counter++;
			continue;
		}
		else
		{
			break;
		}
	}
	return counter!=0;
}

PX_SYNTAX_FUNCTION(PX_Syntax_source_error)
{
	PX_Syntax_Terminate(pSyntax, "ast:error:Unknow content");
	return PX_TRUE;
}


PX_SYNTAX_FUNCTION(PX_Syntax_source_init)
{
	if (pSyntax->reg_abi_stack.size==0)
	{

		if (!PX_Syntax_EnterScope(pSyntax))
		{
			PX_ASSERTX("Error:sources_enter scope failed.");
			PX_Syntax_Terminate(pSyntax, "Error:sources_enter scope failed.");
			return PX_FALSE;
		}
		if (!PX_Syntax_load_comment(pSyntax))
		{
			PX_Syntax_Terminate(pSyntax, "ast:error:source init PX_Syntax_load_comment failed.");
			return PX_FALSE;
		}

		if (!PX_Syntax_load_include(pSyntax))
		{
			PX_Syntax_Terminate(pSyntax, "ast:error:source init PX_Syntax_load_include failed.");
			return PX_FALSE;
		}

		if (!PX_Syntax_load_const_int(pSyntax))
		{
            PX_Syntax_Terminate(pSyntax, "ast:error:source init PX_Syntax_load_const_int failed.");
			return PX_FALSE;
		}
		if (!PX_Syntax_load_const_string(pSyntax))
		{
            PX_Syntax_Terminate(pSyntax, "ast:error:source init PX_Syntax_load_const_string failed.");
			return PX_FALSE;
		}
		if (!PX_Syntax_load_const_float(pSyntax))
		{
            PX_Syntax_Terminate(pSyntax, "ast:error:source init PX_Syntax_load_const_float failed.");
			return PX_FALSE;
		}
		if (!PX_Syntax_load_keyword(pSyntax))
		{
            PX_Syntax_Terminate(pSyntax, "ast:error:source init PX_Syntax_load_keyword failed.");
			return PX_FALSE;
		}
		if (!PX_Syntax_load_bcontainer(pSyntax))
		{
            PX_Syntax_Terminate(pSyntax, "ast:error:source init PX_Syntax_load_bcontainer failed.");
			return PX_FALSE;
		}
		if (!PX_Syntax_load_identifier(pSyntax))
		{
            PX_Syntax_Terminate(pSyntax, "ast:error:source init PX_Syntax_load_identifier failed.");
			return PX_FALSE;
		}
		if (!PX_Syntax_load_numeric(pSyntax))
		{
            PX_Syntax_Terminate(pSyntax, "ast:error:source init PX_Syntax_load_numeric failed.");
			return PX_FALSE;
		}
		if (!PX_Syntax_load_tuple(pSyntax))
		{
            PX_Syntax_Terminate(pSyntax, "ast:error:source init PX_Syntax_load_tuple failed.");
			return PX_FALSE;
		}
		if (!PX_Syntax_load_const_string_list(pSyntax))
		{
            PX_Syntax_Terminate(pSyntax, "ast:error:source init PX_Syntax_load_const_string_list failed.");
			return PX_FALSE;
		}

		if (!PX_Syntax_load_const_int_list(pSyntax))
		{
            PX_Syntax_Terminate(pSyntax, "ast:error:source init PX_Syntax_load_const_int_list failed.");
			return PX_FALSE;
		}

		if (!PX_Syntax_load_const_float_list(pSyntax))
		{
            PX_Syntax_Terminate(pSyntax, "ast:error:source init PX_Syntax_load_const_float_list failed.");
			return PX_FALSE;
		}

		if (!PX_Syntax_load_const_numeric_list(pSyntax))
		{
            PX_Syntax_Terminate(pSyntax, "ast:error:source init PX_Syntax_load_const_numeric_list failed.");
			return PX_FALSE;
		}

		if (!PX_Syntax_load_const_tuple_list(pSyntax))
		{
            PX_Syntax_Terminate(pSyntax, "ast:error:source init PX_Syntax_load_const_tuple_list failed.");
			return PX_FALSE;
		}

		if (!PX_Syntax_load_const_set(pSyntax))
		{
            PX_Syntax_Terminate(pSyntax, "ast:error:source init PX_Syntax_load_const_set failed.");
			return PX_FALSE;
		}

		if (!PX_Syntax_load_bcontainer(pSyntax))
		{
            PX_Syntax_Terminate(pSyntax, "ast:error:source init PX_Syntax_load_bcontainer failed.");
			return PX_FALSE;
		}

		if (!PX_Syntax_load_define(pSyntax))
		{
            PX_Syntax_Terminate(pSyntax, "ast:error:source init PX_Syntax_load_define failed.");
			return PX_FALSE;
		}

		if (!PX_Syntax_load_declare_prefix(pSyntax))
		{
            PX_Syntax_Terminate(pSyntax, "ast:error:source init PX_Syntax_load_declare_prefix failed.");
			return PX_FALSE;
		}

		if (!PX_Syntax_load_declare_token_prefix(pSyntax))
		{
            PX_Syntax_Terminate(pSyntax, "ast:error:source init PX_Syntax_load_declare_token_prefix failed.");
			return PX_FALSE;
		}

		if (!PX_Syntax_load_declare_token_suffix(pSyntax))
		{
            PX_Syntax_Terminate(pSyntax, "ast:error:source init PX_Syntax_load_declare_token_suffix failed.");
			return PX_FALSE;
		}

		if (!PX_Syntax_load_define_struct(pSyntax))
		{
            PX_Syntax_Terminate(pSyntax, "ast:error:source init PX_Syntax_load_define_struct failed.");
			return PX_FALSE;
		}

		if (!PX_Syntax_load_declare_variable(pSyntax))
		{
            PX_Syntax_Terminate(pSyntax, "ast:error:source init PX_Syntax_load_declare_variable failed.");
			return PX_FALSE;
		}

		if (!PX_Syntax_load_eof(pSyntax))
		{
            PX_Syntax_Terminate(pSyntax, "ast:error:source init PX_Syntax_load_eof failed.");
			return PX_FALSE;
		}

		if (!PX_Syntax_load_function(pSyntax))
		{
			PX_Syntax_Terminate(pSyntax, "ast:error:source init PX_Syntax_load_function failed.");
			return PX_FALSE;
		}

		if (!PX_Syntax_load_expr(pSyntax))
		{
			PX_Syntax_Terminate(pSyntax, "ast:error:source init PX_Syntax_load_expr failed.");
			return PX_FALSE;
		}

		if (!PX_Syntax_init_base_type(pSyntax))
		{
			PX_Syntax_Terminate(pSyntax, "ast:error:source init PX_Syntax_init_base_type failed.");
			return PX_FALSE;
		}

		return PX_TRUE;
	}

	return PX_FALSE;
}

PX_SYNTAX_FUNCTION(PX_Syntax_source_leave)
{

	return PX_TRUE;
}


px_bool PX_Syntax_load_sources(PX_Syntax* pSyntax)
{
	if (!PX_Syntax_Parse_PEBNF(pSyntax, "source_skip_newline_spacer= *", PX_Syntax_source_skip_newline_spacer, 0, 0))
	{
		return PX_FALSE;
	}
	
	if (!PX_Syntax_Parse_PEBNF(pSyntax, "source_init= *", PX_Syntax_source_init, 0, 0))
	{
		return PX_FALSE;
	}

	if (!PX_Syntax_Parse_PEBNF(pSyntax, "source= source_init", 0, 0, 0))
	{
		return PX_FALSE;
	}

	if (!PX_Syntax_Parse_PEBNF(pSyntax, "source= source_skip_newline_spacer", 0, 0, 0))
	{
		return PX_FALSE;
	}

	if (!PX_Syntax_Parse_PEBNF(pSyntax, "source= comment", 0, 0, 0))
	{
		return PX_FALSE;
	}

	if (!PX_Syntax_Parse_PEBNF(pSyntax, "source= include", 0, 0, 0))
	{
		return PX_FALSE;
	}

	if (!PX_Syntax_Parse_PEBNF(pSyntax, "source= define", 0, 0, 0))
	{
		return PX_FALSE;
	}

	if (!PX_Syntax_Parse_PEBNF(pSyntax, "source= declare_variable", 0, 0, 0))
	{
		return PX_FALSE;
	}

	if (!PX_Syntax_Parse_PEBNF(pSyntax, "source= define_function", 0, 0, 0))
	{
		return PX_FALSE;
	}

	if (!PX_Syntax_Parse_PEBNF(pSyntax, "source= expr_sentence", 0, 0, 0))
	{
		return PX_FALSE;
	}

	if (!PX_Syntax_Parse_PEBNF(pSyntax, "_ = source ...", 0, 0, 0))
	{
		return PX_FALSE;
	}

	if (!PX_Syntax_Parse_PEBNF(pSyntax, "_ = eof", 0, 0, 0))
	{
		return PX_FALSE;
	}

	if (!PX_Syntax_Parse_PEBNF(pSyntax, "_ = source *", 0, PX_Syntax_source_error, 0))
	{
		return PX_FALSE;
	}
    return PX_TRUE;
}
