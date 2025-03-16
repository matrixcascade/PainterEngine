#include "PX_Syntax.h"


px_bool PX_Syntax_IsValidToken(const px_char token[])
{
	if (token[0] == '\0' || PX_charIsNumeric(token[0]))
	{
		return PX_FALSE;
	}

	while (*token)
	{
		if ((*token >= 'A' && *token <= 'Z') || (*token >= 'a' && *token <= 'z') || *token == '_' || *token == ':' || PX_charIsNumeric(*token))
		{
			token++;
			continue;
		}
		return PX_FALSE;
	}
	return PX_TRUE;
}

PX_Syntax_pebnf* PX_Syntax_GetPEBNF(PX_Syntax* pSyntax, const px_char mnemonic[])
{
	px_int i;
	PX_Syntax_pebnf* ptype;
	for (i = 0; i < pSyntax->pebnf.size; i++)
	{
		ptype = PX_VECTORAT(PX_Syntax_pebnf, &pSyntax->pebnf, i);
		if (PX_strequ(ptype->mnenonic.buffer, mnemonic))
		{
			return ptype;
		}
	}
	return PX_NULL;
}

px_lexer* PX_Syntax_GetCurrentLexer(PX_Syntax* pSyntax)
{
	if (pSyntax->reg_ast_lexer.size)
	{
		return PX_VECTORLAST(px_lexer, &pSyntax->reg_ast_lexer);
	}
	PX_ASSERT();
	return PX_NULL;
}

px_bool PX_Syntax_PushLexer(PX_Syntax* pSyntax, px_lexer* plexer)
{
	return PX_VectorPushback(&pSyntax->reg_ast_lexer, plexer);
}

px_void PX_Syntax_PopLexer(PX_Syntax* pSyntax)
{
	if (pSyntax->reg_ast_lexer.size) {
		px_lexer* plexer = PX_VECTORLAST(px_lexer, &pSyntax->reg_ast_lexer);
		PX_LexerFree(plexer);
		PX_VectorPop(&pSyntax->reg_ast_lexer);
	}
	else
		PX_ASSERT();
}

PX_Syntax_bnfnode* PX_Syntax_GetPebnfNode(PX_Syntax* pSyntax, const px_char mnemonic[])
{
	PX_Syntax_pebnf* ppebnf = PX_Syntax_GetPEBNF(pSyntax, mnemonic);
	if (ppebnf)
	{
		return ppebnf->pbnfnode;
	}
	return PX_NULL;
}

PX_Syntax_bnfnode* PX_Syntax_GetOtherNode(PX_Syntax_bnfnode* pbnfnode, PX_SYNTAX_AST_TYPE ast_type, const px_char mnemonic[])
{
	while (pbnfnode)
	{
		if (pbnfnode->type == ast_type)
		{
			if (PX_strequ(pbnfnode->constant.buffer, mnemonic))
			{
				return pbnfnode;
			}
		}
		pbnfnode = pbnfnode->pothers;
	}
	return PX_NULL;
}

PX_LEXER_LEXEME_TYPE PX_SyntaxGetNextLexemeFilter(px_lexer* plexer)
{
	PX_LEXER_LEXEME_TYPE type;
	while (PX_TRUE)
	{
		PX_LEXER_STATE state;
		state = PX_LexerGetState(plexer);
		type = PX_LexerGetNextLexeme(plexer);
		switch (type)
		{
		case PX_LEXER_LEXEME_TYPE_NEWLINE:
		case PX_LEXER_LEXEME_TYPE_SPACER:
			continue;
		default:
			return type;
		}
	}
	return PX_LEXER_LEXEME_TYPE_ERR;
}

PX_LEXER_LEXEME_TYPE PX_SyntaxGetNextAstFilter(PX_Syntax_ast *past)
{
	return PX_SyntaxGetNextLexemeFilter(past->lexer_state.plexer);
}

const px_char* PX_SyntaxGetCurrentLexeme(PX_Syntax_ast* past)
{
	return PX_LexerGetLexeme(past->lexer_state.plexer);
}

PX_Syntax_pebnf* PX_Syntax_SetPEBNF(PX_Syntax* pSyntax, const px_char mnemonic[])
{
	PX_Syntax_pebnf* ppebnf = PX_Syntax_GetPEBNF(pSyntax, mnemonic);
	if (!ppebnf)
	{
		px_memorypool* mp = pSyntax->mp;
		PX_Syntax_pebnf pebnf = {0};

		if (!PX_StringInitialize(mp, &pebnf.mnenonic))
			return PX_NULL;
		if (!PX_StringCat(&pebnf.mnenonic, mnemonic))
			return PX_NULL;
		PX_VectorPushback(&pSyntax->pebnf, &pebnf);
		return PX_VECTORLAST(PX_Syntax_pebnf, &pSyntax->pebnf);
	}
	return ppebnf;

}

px_bool PX_Syntax_Parse_NextPENNF(PX_Syntax* pSyntax, PX_Syntax_pebnf* ppebnf, PX_Syntax_bnfnode* pprevious, px_lexer* plexer, PX_Syntax_Function pfunction)
{
	PX_LEXER_LEXEME_TYPE type;
	type = PX_SyntaxGetNextLexemeFilter(plexer);

	switch (type)
	{
	case PX_LEXER_LEXEME_TYPE_CONATINER:
	case PX_LEXER_LEXEME_TYPE_TOKEN:
	{
		PX_SYNTAX_AST_TYPE ast_type;
		const px_char* pstr;
		PX_Syntax_bnfnode* psearchast;
		if (type == PX_LEXER_LEXEME_TYPE_CONATINER)
		{
			ast_type = PX_SYNTAX_AST_TYPE_CONSTANT;
			PX_LexerGetIncludedString(plexer, &plexer->CurLexeme);
			pstr = PX_LexerGetLexeme(plexer);
		}
		else
		{
			pstr = PX_LexerGetLexeme(plexer);
			if (pstr[0] == '*' && pstr[1] == '\0')
			{
				ast_type = PX_SYNTAX_AST_TYPE_FUNCTION;
			}
			else if (PX_strequ(pstr,"..."))
			{
				ast_type = PX_SYNTAX_AST_TYPE_RECURSION;
			}
			else
			{
				ast_type = PX_SYNTAX_AST_TYPE_LINKER;
			}
		}

		if (pprevious)
		{
			psearchast = PX_Syntax_GetOtherNode(pprevious->pnext, ast_type, pstr);
		}
		else
		{
			psearchast = PX_Syntax_GetOtherNode(ppebnf->pbnfnode, ast_type, pstr);
		}

		if (!psearchast)
		{
			PX_Syntax_bnfnode* plast;
			PX_Syntax_bnfnode* pnewast = (PX_Syntax_bnfnode*)MP_Malloc(pSyntax->mp, sizeof(PX_Syntax_bnfnode));
			if (pnewast)
			{
				if (!PX_StringInitialize(pSyntax->mp, &pnewast->constant))
					return PX_NULL;
				pnewast->type = ast_type;
				if (ast_type== PX_SYNTAX_AST_TYPE_RECURSION)
				{
					if (!PX_StringCat(&pnewast->constant, ppebnf->mnenonic.buffer))
						return PX_NULL;
				}
				else
				{
					if (!PX_StringCat(&pnewast->constant, pstr))
						return PX_NULL;
				}
				
				pnewast->pfunction = PX_NULL;
				pnewast->pothers = PX_NULL;
				pnewast->pnext = PX_NULL;
			}
			else
			{
				PX_ASSERT();
				return PX_FALSE;
			}
			if (pprevious)
			{
				plast = pprevious->pnext;
				if (plast == PX_NULL)
				{
					pprevious->pnext = pnewast;
				}
				else
				{
					while (plast->pothers)
					{
						plast = plast->pothers;
					}
					plast->pothers = pnewast;
				}
			}
			else
			{
				plast = ppebnf->pbnfnode;
				if (plast == PX_NULL)
				{
					ppebnf->pbnfnode = pnewast;
				}
				else
				{
					while (plast->pothers)
					{
						plast = plast->pothers;
					}
					plast->pothers = pnewast;
				}
			}
			return PX_Syntax_Parse_NextPENNF(pSyntax, ppebnf, pnewast, plexer, pfunction);
		}
		else
		{
			return PX_Syntax_Parse_NextPENNF(pSyntax, ppebnf, psearchast, plexer, pfunction);
		}
	}
	break;
	case PX_LEXER_LEXEME_TYPE_END:
	{
		pprevious->pnext = PX_NULL;
		pprevious->pfunction = pfunction;
		return PX_TRUE;
	}
	break;
	default:
		PX_ASSERTIFX(PX_FALSE, "Error:PEBNF Error");
		return PX_FALSE;
	}
}

px_bool PX_Syntax_Parse_PEBNF(PX_Syntax* pSyntax, const px_char PEBNF[], PX_Syntax_Function pfunction)
{
	px_lexer lexer;
	PX_LEXER_LEXEME_TYPE type;
	PX_LexerInitialize(&lexer, pSyntax->mp);
	PX_LexerRegisterSpacer(&lexer, ' ');
	PX_LexerRegisterContainer(&lexer, "'", "'");
	PX_LexerRegisterDelimiter(&lexer, '=');
	PX_LexerLoadSourceWithPresort(&lexer, PEBNF);

	type = PX_SyntaxGetNextLexemeFilter(&lexer);
	if (type == PX_LEXER_LEXEME_TYPE_TOKEN)
	{
		const px_char* plexeme = PX_LexerGetLexeme(&lexer);
		PX_Syntax_pebnf* ppebnf = PX_Syntax_SetPEBNF(pSyntax, plexeme);
		if (!ppebnf)
		{
			PX_ASSERTIFX(!ppebnf, "Error:PEBNF malloc Error");
			goto _ERROR;
		}
		type = PX_SyntaxGetNextLexemeFilter(&lexer);
		if (type==PX_LEXER_LEXEME_TYPE_END)
		{
			ppebnf->pfunction = pfunction;
			PX_LexerFree(&lexer);
			return PX_TRUE;
		}

		PX_ASSERTIFX(type != PX_LEXER_LEXEME_TYPE_DELIMITER, "Error:PEBNF Error");
		PX_ASSERTIFX(!PX_strequ(PX_LexerGetLexeme(&lexer), "="), "Error:::= expected but not found");
		if (ppebnf)
		{
			if (PX_Syntax_Parse_NextPENNF(pSyntax, ppebnf, 0, &lexer, pfunction))
			{
				PX_LexerFree(&lexer);
				return PX_TRUE;
			}
			else
			{
				goto _ERROR;
			}
		}
		else
		{
			goto _ERROR;
		}
	}
	else
	{
		PX_ASSERTIFX(PX_FALSE, "Error:PEBNF Error");
		goto _ERROR;
	}

	PX_LexerFree(&lexer);
	return PX_TRUE;
_ERROR:
	PX_LexerFree(&lexer);
	return PX_FALSE;
}

px_bool PX_Syntax_PushOpcode(PX_Syntax* pSyntax, const px_char type[])
{
	px_abi newabi;
	PX_AbiCreateDynamicWriter(&newabi, pSyntax->mp);
	if (!PX_AbiSet_string(&newabi, "type", type))
	{
		PX_AbiFree(&newabi);
		return PX_FALSE;
	}
	if (!PX_VectorPushback(&pSyntax->reg_ast_opcode, &newabi))
	{
		PX_AbiFree(&newabi);
		return PX_FALSE;
	}
	return PX_TRUE;
}
px_int PX_Syntax_GetAbiStackCount(PX_Syntax* pSyntax)
{
	return pSyntax->abistack.size;
}

px_abi* PX_Syntax_GetAbiStack(PX_Syntax* pSyntax, px_int index)
{
	if (index >= 0)
	{
		PX_ASSERT();
		return PX_NULL;
	}
	if (index < -pSyntax->abistack.size)
	{
		PX_ASSERT();
		return PX_NULL;
	}
	return PX_VECTORAT(px_abi, &pSyntax->abistack, pSyntax->abistack.size + index);
}

px_abi* PX_Syntax_GetAbiStackLast(PX_Syntax* pSyntax)
{
	if (pSyntax->abistack.size == 0)
	{
		PX_ASSERT();
		return PX_NULL;
	}
	return PX_VECTORLAST(px_abi, &pSyntax->abistack);
}

px_abi* PX_Syntax_GetAbiStackSecondLast(PX_Syntax* pSyntax)
{
	if (pSyntax->abistack.size < 2)
	{
		PX_ASSERT();
		return PX_NULL;
	}
	return PX_VECTORAT(px_abi, &pSyntax->abistack, pSyntax->abistack.size - 2);
}

px_abi* PX_Syntax_GetAbi(PX_Syntax* pSyntax, const px_char name[], px_int lifetime)
{
	px_abi* pabi;
	px_int i;
	for (i = pSyntax->abistack.size; i >= 0; i--)
	{
		const px_char* pname;
		px_int* plifetime;
		pabi = PX_VECTORAT(px_abi, &pSyntax->abistack, i);
		pname = PX_AbiGet_string(pabi, "name");
		plifetime = PX_AbiGet_int(pabi, "lifetime");
		if (!pname || !plifetime)
		{
			return PX_NULL;
		}

		if (PX_strequ(pname, name))
		{
			if (*plifetime < lifetime)
			{
				return pabi;
			}
		}

	}
	return PX_NULL;
}

px_abi* PX_Syntax_PushNewAbi(PX_Syntax* pSyntax, const px_char name[], px_int lifetime)
{
	px_abi abi;
	PX_AbiCreateDynamicWriter(&abi, pSyntax->mp);

	if (!PX_AbiSet_string(&abi, "name", name))
	{
		PX_AbiFree(&abi);
		return PX_NULL;
	}

	if (!PX_AbiSet_int(&abi, "lifetime", lifetime))
	{
		PX_AbiFree(&abi);
		return PX_NULL;
	}


	if (PX_VectorPushback(&pSyntax->abistack, &abi))
	{
		return PX_VECTORLAST(px_abi, &pSyntax->abistack);
	}
	return PX_NULL;
}

px_abi* PX_Syntax_CreateNewIr(PX_Syntax* pSyntax)
{
	//create new ir
	px_abi newabi;
	PX_AbiCreateDynamicWriter(&newabi, pSyntax->mp);
	if (!PX_AbiSet_string(&newabi, "name", "ir"))
	{
		PX_AbiFree(&newabi);
		return PX_FALSE;
	}
	if (!PX_AbiSet_string(&newabi, "value", ""))
	{
		PX_AbiFree(&newabi);
		return PX_FALSE;
	}

	if (!PX_Syntax_PushAbiStack(pSyntax, &newabi))
	{
		PX_AbiFree(&newabi);
		return PX_FALSE;
	}
	return PX_VECTORLAST(px_abi, &pSyntax->abistack);
}


px_bool PX_Syntax_PushAbiStack(PX_Syntax* pSyntax, px_abi* pabi)
{
	return PX_VectorPushback(&pSyntax->abistack, pabi);
}

px_void PX_Syntax_PopAbiStack(PX_Syntax* pSyntax)
{
	px_abi* pabi;
	if (pSyntax->abistack.size == 0)
	{
		PX_ASSERT();
	}
	//free abi
	pabi = PX_VECTORLAST(px_abi, &pSyntax->abistack);
	PX_AbiFree(pabi);
	PX_VectorPop(&pSyntax->abistack);
}

px_void PX_Syntax_PopSecondAbiStack(PX_Syntax* pSyntax)
{
	px_abi* pabi;
	if (pSyntax->abistack.size < 2)
	{
		PX_ASSERT();
	}
	pabi = PX_VECTORAT(px_abi, &pSyntax->abistack, pSyntax->abistack.size - 2);
	PX_AbiFree(pabi);
	PX_VectorErase(&pSyntax->abistack, pSyntax->abistack.size - 2);
}


px_void PX_Syntax_PopAbiStackN(PX_Syntax* pSyntax, px_int N)
{
	px_int i;
	px_abi* pabi;
	if (pSyntax->abistack.size < N)
	{
		PX_ASSERT();
	}
	for (i = 0; i < N; i++)
	{
		pabi = PX_VECTORLAST(px_abi, &pSyntax->abistack);
		PX_AbiFree(pabi);
		PX_VectorPop(&pSyntax->abistack);
	}
}

px_bool PX_SyntaxInitialize(px_memorypool* mp, PX_Syntax* pSyntax)
{
	pSyntax->mp = mp;

	if (!PX_StringInitialize(mp, &pSyntax->message))
		return PX_FALSE;

	if (!PX_StringInitialize(mp, &pSyntax->ast_message))
		return PX_FALSE;

	PX_VectorInitialize(mp, &pSyntax->pebnf, sizeof(PX_Syntax_pebnf), 0);
	PX_VectorInitialize(mp, &pSyntax->sources, sizeof(PX_Syntax_Source), 0);
	PX_VectorInitialize(mp, &pSyntax->aststack, sizeof(PX_Syntax_ast), 0);
	PX_VectorInitialize(mp, &pSyntax->abistack, sizeof(px_abi), 0);
	PX_VectorInitialize(mp, &pSyntax->reg_ast_opcode, sizeof(px_abi), 0);
	PX_VectorInitialize(mp, &pSyntax->reg_ast_lexer, sizeof(px_lexer), 0);

	return PX_TRUE;
}

px_void PX_SyntaxClear(PX_Syntax* pSyntax)
{
	px_int i;
	PX_Syntax_Source* psource;
	px_abi* pabi;
	px_abi* popcode;
	px_lexer* plexer;
	for (i = 0; i < pSyntax->sources.size; i++)
	{
		psource = PX_VECTORAT(PX_Syntax_Source, &pSyntax->sources, i);
		PX_StringFree(&psource->name);
		PX_StringFree(&psource->source);
	}
	PX_VectorClear(&pSyntax->sources);
	PX_VectorClear(&pSyntax->aststack);
	for (i = 0; i < pSyntax->abistack.size; i++)
	{
		pabi = PX_VECTORAT(px_abi, &pSyntax->abistack, i);
		PX_AbiFree(pabi);
	}
	PX_VectorClear(&pSyntax->abistack);

	for (i = 0; i < pSyntax->reg_ast_opcode.size; i++)
	{
		popcode = PX_VECTORAT(px_abi, &pSyntax->reg_ast_opcode, i);
		PX_AbiFree(popcode);
	}
	PX_VectorClear(&pSyntax->reg_ast_opcode);

	for (i = 0; i < pSyntax->reg_ast_lexer.size; i++)
	{
		plexer = PX_VECTORAT(px_lexer, &pSyntax->reg_ast_lexer, i);
		PX_LexerFree(plexer);
	}



	PX_VectorClear(&pSyntax->reg_ast_lexer);
	PX_StringClear(&pSyntax->message);
	PX_StringClear(&pSyntax->ast_message);
}

static px_void PX_SyntaxFree_pebnfnode(PX_Syntax* pSyntax, PX_Syntax_bnfnode*pnode)
{
	if (pnode->pnext)
	{
		PX_SyntaxFree_pebnfnode(pSyntax, pnode->pnext);
	}
	if (pnode->pothers)
	{
		PX_SyntaxFree_pebnfnode(pSyntax, pnode->pothers);
	}
	PX_StringFree(&pnode->constant);
	MP_Free(pSyntax->mp, pnode);
}
px_void PX_SyntaxFree(PX_Syntax* pSyntax)
{
	px_int i;
	PX_Syntax_pebnf* ppebnf;
	PX_SyntaxClear(pSyntax);
	for (i = 0; i < pSyntax->pebnf.size; i++)
	{
		ppebnf = PX_VECTORAT(PX_Syntax_pebnf, &pSyntax->pebnf, i);
		PX_SyntaxFree_pebnfnode(pSyntax, ppebnf->pbnfnode);
		PX_StringFree(&ppebnf->mnenonic);
	}
	PX_VectorFree(&pSyntax->pebnf);
	PX_VectorFree(&pSyntax->sources);
	PX_VectorFree(&pSyntax->aststack);
	PX_VectorFree(&pSyntax->abistack);
	PX_VectorFree(&pSyntax->reg_ast_opcode);
	PX_VectorFree(&pSyntax->reg_ast_lexer);
	PX_StringFree(&pSyntax->message);
	PX_StringFree(&pSyntax->ast_message);
}


px_bool PX_Syntax_AddSource(PX_Syntax* pSyntax, const px_char name[], const px_char source[])
{
	PX_Syntax_Source sourcefile;
	if (!PX_StringInitialize(pSyntax->mp, &sourcefile.name))
		return PX_FALSE;
	if (!PX_StringCat(&sourcefile.name, name))
	{
		PX_StringFree(&sourcefile.name);
		return PX_FALSE;
	}
	if (!PX_StringInitialize(pSyntax->mp, &sourcefile.source))
	{
		PX_StringFree(&sourcefile.name);
		return PX_FALSE;
	}
	if (!PX_StringCat(&sourcefile.source, source))
	{
		PX_StringFree(&sourcefile.name);
		PX_StringFree(&sourcefile.source);
		return PX_FALSE;
	}
	if (!PX_VectorPushback(&pSyntax->sources, &sourcefile))
	{
		PX_StringFree(&sourcefile.name);
		PX_StringFree(&sourcefile.source);
		return PX_FALSE;
	}
	return PX_TRUE;
}

px_bool PX_Syntax_AddLexer(PX_Syntax* pSyntax, const px_char name[], const px_char source[])
{
	px_lexer sourcefile;


	PX_LexerInitialize(&sourcefile, pSyntax->mp);
	if (!PX_LexerSetName(&sourcefile, name))
		return PX_FALSE;
	PX_LexerRegisterDelimiter(&sourcefile, ',');
	PX_LexerRegisterDelimiter(&sourcefile, '.');
	PX_LexerRegisterDelimiter(&sourcefile, ';');
	PX_LexerRegisterDelimiter(&sourcefile, '+');
	PX_LexerRegisterDelimiter(&sourcefile, '-');
	PX_LexerRegisterDelimiter(&sourcefile, '*');
	PX_LexerRegisterDelimiter(&sourcefile, '/');
	PX_LexerRegisterDelimiter(&sourcefile, '%');
	PX_LexerRegisterDelimiter(&sourcefile, '&');
	PX_LexerRegisterDelimiter(&sourcefile, '^');
	PX_LexerRegisterDelimiter(&sourcefile, '~');
	PX_LexerRegisterDelimiter(&sourcefile, '(');
	PX_LexerRegisterDelimiter(&sourcefile, ')');
	PX_LexerRegisterDelimiter(&sourcefile, '!');
	PX_LexerRegisterDelimiter(&sourcefile, '=');
	PX_LexerRegisterDelimiter(&sourcefile, '>');
	PX_LexerRegisterDelimiter(&sourcefile, '<');
	PX_LexerRegisterDelimiter(&sourcefile, '{');
	PX_LexerRegisterDelimiter(&sourcefile, '}');
	PX_LexerRegisterDelimiter(&sourcefile, '[');
	PX_LexerRegisterDelimiter(&sourcefile, ']');
	PX_LexerRegisterDelimiter(&sourcefile, '\"');
	PX_LexerRegisterDiscard(&sourcefile, "\\\n");

	PX_LexerRegisterSpacer(&sourcefile, ' ');
	PX_LexerRegisterSpacer(&sourcefile, '\t');
	PX_LexerRegisterComment(&sourcefile, "//", "\n");
	PX_LexerRegisterComment(&sourcefile, "/*", "*/");
	if (PX_LexerLoadSource(&sourcefile, source))
	{
		return PX_VectorPushback(&pSyntax->reg_ast_lexer, &sourcefile);
	}
	return PX_FALSE;
}

PX_Syntax_Source* PX_Syntax_GetSource(PX_Syntax* pSyntax, const px_char name[])
{
	px_int i;
	PX_Syntax_Source* psource;
	for (i = 0; i < pSyntax->sources.size; i++)
	{
		psource = PX_VECTORAT(PX_Syntax_Source, &pSyntax->sources, i);
		if (PX_strequ(psource->name.buffer, name))
		{
			return psource;
		}
	}
	return PX_NULL;
}

PX_LEXER_LEXEME_TYPE PX_Syntax_GetNextLexemeFilter(px_lexer* plexer)
{
	PX_LEXER_LEXEME_TYPE type;
	while (PX_TRUE)
	{
		PX_LEXER_STATE state;
		state = PX_LexerGetState(plexer);
		type = PX_LexerGetNextLexeme(plexer);
		switch (type)
		{
		case PX_LEXER_LEXEME_TYPE_NEWLINE:
		case PX_LEXER_LEXEME_TYPE_SPACER:
		case PX_LEXER_LEXEME_TYPE_COMMENT:
			continue;
		default:
			return type;
		}
	}
	return PX_LEXER_LEXEME_TYPE_ERR;
}

static px_bool PX_Syntax_ExecuteNext(PX_Syntax* pSyntax, PX_Syntax_ast* pcurrent_ast)
{
	PX_Syntax_ast newast = { 0 };
	//push next
	if (pcurrent_ast->pbnfnode->pnext == PX_NULL)
	{
		PX_Syntax_AstMessage(pSyntax, "AST:next node is null,return true\n");
		if (PX_Syntax_PushOpcode(pSyntax, "return true"))
			return PX_TRUE;
		PX_Syntax_Terminate(pSyntax, pcurrent_ast, "out of memory");
		return PX_FALSE;
	}
	else
	{
		PX_Syntax_AstMessage(pSyntax, "AST:next node ");
		PX_Syntax_AstMessage(pSyntax, pcurrent_ast->pbnfnode->pnext->constant.buffer);
		PX_Syntax_AstMessage(pSyntax, "\n");
		newast.lexer_state = PX_LexerGetState(PX_Syntax_GetCurrentLexer(pSyntax));
		newast.pbnfnode = pcurrent_ast->pbnfnode->pnext;//try next
		newast.call_abistack_index = pSyntax->abistack.size;
		if (!PX_VectorPushback(&pSyntax->aststack, &newast))
		{
			PX_Syntax_Terminate(pSyntax, pcurrent_ast, "out of memory");
			return PX_FALSE;
		}

	}
	return PX_TRUE;
}

static px_bool PX_Syntax_ExecuteOthers(PX_Syntax* pSyntax, PX_Syntax_ast* pcurrent_ast)
{
	PX_Syntax_ast newast = { 0 };
	PX_Syntax_bnfnode* pbnfnode = pcurrent_ast->pbnfnode;
	//not matched,try others
	if (pbnfnode->pothers==PX_NULL)
	{
		PX_Syntax_AstMessage(pSyntax, "AST:others node is null,return false\n");

		if (PX_Syntax_PushOpcode(pSyntax, "return false"))
			return PX_TRUE;
		
		PX_Syntax_Terminate(pSyntax, pcurrent_ast, "out of memory");
		return PX_FALSE;
	}
	else
	{
		PX_Syntax_AstMessage(pSyntax, "AST:others node ");
		PX_Syntax_AstMessage(pSyntax, pbnfnode->pothers->constant.buffer);
		PX_Syntax_AstMessage(pSyntax, "\n");
		newast.lexer_state = pcurrent_ast->lexer_state;
		newast.pbnfnode = pbnfnode->pothers;
		newast.call_abistack_index = pSyntax->abistack.size;
		if (!PX_VectorPushback(&pSyntax->aststack, &newast))
		{
			PX_Syntax_Terminate(pSyntax, pcurrent_ast, "out of memory");
			return PX_FALSE;
		}
	}
	
	return PX_TRUE;
}

static px_bool PX_Syntax_ExecuteAst(PX_Syntax* pSyntax)
{
	PX_Syntax_ast current_ast = { 0 };
	PX_Syntax_bnfnode* pbnfnode = PX_NULL;

	if (pSyntax->aststack.size == 0)
	{
		//end of execute
		return PX_FALSE;
	}

	if (!PX_VectorLastTo(&pSyntax->aststack, &current_ast))
	{
		return PX_FALSE;
	}

	pbnfnode = current_ast.pbnfnode;
	PX_LexerSetState(current_ast.lexer_state);
	switch (pbnfnode->type)
	{
	case PX_SYNTAX_AST_TYPE_CONSTANT:
	{
		const px_char* pstr = pbnfnode->constant.buffer;
		PX_LEXER_LEXEME_TYPE type = PX_SyntaxGetNextAstFilter(&current_ast);
		const px_char* plexeme = PX_LexerGetLexeme(current_ast.lexer_state.plexer);
		PX_VectorPop(&pSyntax->aststack);
		if (PX_strequ(plexeme, pstr))
		{
			PX_Syntax_AstMessage(pSyntax, "AST:const matched ");
			PX_Syntax_AstMessage(pSyntax, current_ast.pbnfnode->constant.buffer);
			PX_Syntax_AstMessage(pSyntax, "\n");
			if (pbnfnode->pfunction)
			{
				if (pbnfnode->pfunction(pSyntax, &current_ast))
				{
					PX_Syntax_AstMessage(pSyntax, "AST:const function return true,ast next\n");
					return PX_Syntax_ExecuteNext(pSyntax, &current_ast);
				}
				else
				{
					PX_Syntax_AstMessage(pSyntax, "AST:const function return false,try others\n");
				}
			}
			else
			{
				PX_Syntax_AstMessage(pSyntax, "AST:const function is null,ast next\n");
				return PX_Syntax_ExecuteNext(pSyntax, &current_ast);
			}
		}
		return PX_Syntax_ExecuteOthers(pSyntax, &current_ast);
	}
	break;
	case PX_SYNTAX_AST_TYPE_FUNCTION:
	{
		PX_Syntax_AstMessage(pSyntax, "AST:function matched ");
		PX_Syntax_AstMessage(pSyntax, current_ast.pbnfnode->constant.buffer);
		PX_Syntax_AstMessage(pSyntax, "\n");
		PX_VectorPop(&pSyntax->aststack);
		if (pbnfnode->pfunction)
		{
			if (pbnfnode->pfunction(pSyntax, &current_ast))
			{
				PX_Syntax_AstMessage(pSyntax, "AST:function return true,ast next\n");
				return PX_Syntax_ExecuteNext(pSyntax, &current_ast);
			}
			else
			{
				PX_Syntax_AstMessage(pSyntax, "AST:function return false,try others\n");
			}
		}
		else
		{
			//type==PX_SYNTAX_AST_TYPE_FUNCTION but the function is not defined.
			PX_ASSERT();
			return PX_FALSE;
		}
		return PX_Syntax_ExecuteOthers(pSyntax, &current_ast);
	}
	break;
	case PX_SYNTAX_AST_TYPE_RECURSION:
	case PX_SYNTAX_AST_TYPE_LINKER:
	{
		//call linker
		PX_Syntax_ast newast;
		const px_char* pstr = pbnfnode->constant.buffer;
		PX_Syntax_pebnf* ppebnf = PX_Syntax_GetPEBNF(pSyntax, pstr);
		PX_Syntax_bnfnode* psearchbnf;
		if (!ppebnf)
		{
			PX_ASSERT();
			return PX_FALSE;
		}
		if (pbnfnode->type== PX_SYNTAX_AST_TYPE_LINKER &&ppebnf->pfunction)
		{
			if (!ppebnf->pfunction(pSyntax, &current_ast))
			{
				return PX_FALSE;
			}
		}

		psearchbnf = ppebnf->pbnfnode;
		PX_Syntax_AstMessage(pSyntax, "AST:linker matched ");
		PX_Syntax_AstMessage(pSyntax, current_ast.pbnfnode->constant.buffer);
		PX_Syntax_AstMessage(pSyntax, "-->");
		PX_Syntax_AstMessage(pSyntax, psearchbnf->constant.buffer);
		PX_Syntax_AstMessage(pSyntax, " abistack:");
		PX_Syntax_AstMessage(pSyntax, PX_itos(pSyntax->abistack.size,10).data);
		PX_Syntax_AstMessage(pSyntax, "\n");
		newast.lexer_state = PX_LexerGetState(current_ast.lexer_state.plexer);
		newast.pbnfnode = psearchbnf;
		newast.call_abistack_index = pSyntax->abistack.size;
		if (!PX_VectorPushback(&pSyntax->aststack, &newast))
		{
			return PX_FALSE;
		}
		return PX_TRUE;
	}
	break;
	default:
		PX_ASSERT();
		break;
	}
	return PX_FALSE;
}

px_void PX_Syntax_AstClearAbiStack(PX_Syntax* pSyntax, PX_Syntax_ast *past)
{
	PX_ASSERTIFX(past->call_abistack_index > pSyntax->abistack.size, "Stack Error");
	while (pSyntax->abistack.size > past->call_abistack_index)
	{
		PX_Syntax_PopAbiStack(pSyntax);
	}
}

px_bool PX_Syntax_Execute(PX_Syntax* pSyntax, const px_char name[], const px_char pebnf[])
{
	PX_Syntax_ast ast;
	PX_Syntax_bnfnode bnfnode = {0};
	PX_Syntax_Source* psource;
	PX_StringSetStatic(&bnfnode.constant, pebnf);
	bnfnode.type = PX_SYNTAX_AST_TYPE_LINKER;
	bnfnode.pfunction = PX_NULL;
	bnfnode.pnext = PX_NULL;
	bnfnode.pothers = PX_NULL;

	psource = PX_Syntax_GetSource(pSyntax, name);
	if (!psource)
	{
		PX_StringCatFormat1(&pSyntax->message, "Error:source file %1 not found.", PX_STRINGFORMAT_STRING(name));
		return PX_FALSE;
	}
	if (!PX_Syntax_AddLexer(pSyntax, name, psource->source.buffer))
		return PX_FALSE;

	ast.lexer_state.plexer = PX_Syntax_GetCurrentLexer(pSyntax);
	ast.lexer_state.offset = 0;
	ast.pbnfnode = &bnfnode;
	ast.call_abistack_index = pSyntax->abistack.size;

	if (!PX_VectorPushback(&pSyntax->aststack, &ast))
		return PX_FALSE;
	while (PX_TRUE)
	{
		if (!PX_Syntax_ExecuteAst(pSyntax))
		{
			break;
		}
		while (pSyntax->reg_ast_opcode.size)
		{
			px_abi* pabi = PX_VECTORLAST(px_abi, &pSyntax->reg_ast_opcode);
			const px_char* ptype = PX_AbiGet_string(pabi, "type");
			if (ptype)
			{
				if (PX_strequ(ptype, "terminate"))
				{
					PX_Syntax_AstMessage(pSyntax, "Error:compiler terminated.\n");
					PX_StringCat(&pSyntax->message, "Error:compiler terminated.");
					return PX_FALSE;
				}
				else if (PX_strequ(ptype, "pop"))
				{
					PX_Syntax_AstMessage(pSyntax, "AST:pop\n");
					PX_VectorPop(&pSyntax->aststack);
					PX_AbiFree(pabi);
					PX_VectorPop(&pSyntax->reg_ast_opcode);
				}
				else if (PX_strequ(ptype,"return true")|| PX_strequ(ptype, "return false"))
				{
					//return node
					PX_Syntax_ast current_ast = { 0 };

					if (pSyntax->aststack.size == 0)
					{
						if (PX_strequ(ptype, "return true"))
						{
							PX_AbiFree(pabi);
							PX_VectorPop(&pSyntax->reg_ast_opcode);
							return PX_TRUE;
						}
						else
						{
							PX_AbiFree(pabi);
							PX_VectorPop(&pSyntax->reg_ast_opcode);
							return PX_FALSE;
						}
					}
					PX_VectorPopTo(&pSyntax->aststack, &current_ast);
					if (current_ast.pbnfnode == PX_NULL || (current_ast.pbnfnode->type != PX_SYNTAX_AST_TYPE_LINKER && current_ast.pbnfnode->type != PX_SYNTAX_AST_TYPE_RECURSION))
					{
						PX_ASSERT();
						return PX_FALSE;
					}
					if (PX_strequ(ptype, "return true"))
					{
						PX_Syntax_AstMessage(pSyntax, "AST:");
						PX_Syntax_AstMessage(pSyntax, current_ast.pbnfnode->constant.buffer);
						PX_Syntax_AstMessage(pSyntax, " return true\n");
						PX_AbiFree(pabi);
						PX_VectorPop(&pSyntax->reg_ast_opcode);
						if (current_ast.pbnfnode->pfunction)
						{
							if (current_ast.pbnfnode->pfunction(pSyntax, &current_ast))
							{
								PX_Syntax_AstMessage(pSyntax, "AST:");
								PX_Syntax_AstMessage(pSyntax, current_ast.pbnfnode->constant.buffer);
								if(current_ast.pbnfnode->type== PX_SYNTAX_AST_TYPE_LINKER)
									PX_Syntax_AstMessage(pSyntax, " linker function return true,ast next\n");
								else
									PX_Syntax_AstMessage(pSyntax, " recursion function return true,ast next\n");
								if (!PX_Syntax_ExecuteNext(pSyntax, &current_ast))
									return PX_FALSE;
							}
							else
							{
								PX_Syntax_AstMessage(pSyntax, "AST:");
								PX_Syntax_AstMessage(pSyntax, current_ast.pbnfnode->constant.buffer);
								if (current_ast.pbnfnode->type == PX_SYNTAX_AST_TYPE_LINKER)
									PX_Syntax_AstMessage(pSyntax, " linker function return false,try others\n");
								else
									PX_Syntax_AstMessage(pSyntax, " recursion function return false,try others\n");
								PX_Syntax_AstClearAbiStack(pSyntax, &current_ast);
								if (!PX_Syntax_ExecuteOthers(pSyntax, &current_ast))
								{
									return PX_FALSE;
								}
							}
						}
						else
						{
							
							PX_Syntax_AstMessage(pSyntax, "AST:");
							PX_Syntax_AstMessage(pSyntax, current_ast.pbnfnode->constant.buffer);
							if (current_ast.pbnfnode->type == PX_SYNTAX_AST_TYPE_LINKER)
								PX_Syntax_AstMessage(pSyntax, " linker function is null,ast next\n");			
							else
								PX_Syntax_AstMessage(pSyntax, " recursion function is null,ast next\n");

							if (!PX_Syntax_ExecuteNext(pSyntax, &current_ast))
								return PX_FALSE;
						}
					}
					else
					{
						//false
						PX_AbiFree(pabi);
						PX_VectorPop(&pSyntax->reg_ast_opcode);
						PX_Syntax_AstClearAbiStack(pSyntax, &current_ast);
						if (!PX_Syntax_ExecuteOthers(pSyntax, &current_ast))
						{
							return PX_FALSE;
						}
					}
				}
			}
		}
	}
	return PX_FALSE;
}

px_void PX_Syntax_Terminate(PX_Syntax* pSyntax, PX_Syntax_ast *past, const px_char message[])
{
	//numeric too long
	PX_StringFormat3(&pSyntax->message, "%1:%2 %3\n", \
		PX_STRINGFORMAT_INT(PX_LexerGetCurrentLine(past->lexer_state.plexer)), \
		PX_STRINGFORMAT_INT(PX_LexerGetCurrentColumn(past->lexer_state.plexer)),\
		PX_STRINGFORMAT_STRING(message));
	//ternimate
	PX_Syntax_PushOpcode(pSyntax, "terminate");
}

px_void PX_Syntax_AstMessage(PX_Syntax* pSyntax, const px_char message[])
{
	printf("%s", message);
	if (!PX_StringCat(&pSyntax->ast_message, message))
	{
		PX_ASSERT();
		return;
	}
}
px_char PX_Syntax_GetNextChar(PX_Syntax_ast* past)
{
	while (PX_TRUE)
	{
		px_char ch=PX_LexerGetNextChar(past->lexer_state.plexer);
		if (ch=='\\'&&PX_LexerPreviewNextChar(past->lexer_state.plexer)=='\n')
		{
			PX_LexerGetNextChar(past->lexer_state.plexer);
			continue;
		}
		return ch;
	}
	 
}

px_char PX_Syntax_PreviewNextChar(PX_Syntax_ast* past)
{
	PX_LEXER_STATE state = PX_LexerGetState(past->lexer_state.plexer);
	while (PX_TRUE)
	{
		px_char ch = PX_LexerGetNextChar(past->lexer_state.plexer);
		if (ch == '\\' && PX_LexerPreviewNextChar(past->lexer_state.plexer) == '\n')
		{
			PX_LexerGetNextChar(past->lexer_state.plexer);
			continue;
		}
		PX_LexerSetState(state);
		return ch;
	}
}