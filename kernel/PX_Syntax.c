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
	while (PX_TRUE)
	{
		type = PX_LexerGetNextLexeme(plexer);
		if (type == PX_LEXER_LEXEME_TYPE_SPACER) 
			continue;
		break;
	}

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
		PX_ASSERTX( "Error:PEBNF Error");
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
	while (PX_TRUE)
	{
		type = PX_LexerGetNextLexeme(&lexer);
		if (type == PX_LEXER_LEXEME_TYPE_SPACER)
			continue;
		break;
	}
	
	if (type == PX_LEXER_LEXEME_TYPE_TOKEN)
	{
		const px_char* plexeme = PX_LexerGetLexeme(&lexer);
		PX_Syntax_pebnf* ppebnf = PX_Syntax_SetPEBNF(pSyntax, plexeme);
		if (!ppebnf)
		{
			PX_ASSERTIFX(!ppebnf, "Error:PEBNF malloc Error");
			goto _ERROR;
		}
		while (PX_TRUE)
		{
			type = PX_LexerGetNextLexeme(&lexer);
			if (type == PX_LEXER_LEXEME_TYPE_SPACER)
				continue;
			break;
		}
		if (type==PX_LEXER_LEXEME_TYPE_END)
		{
			ppebnf->pfunction = pfunction;
			PX_LexerFree(&lexer);
			return PX_TRUE;
		}

		PX_ASSERTIFX(type != PX_LEXER_LEXEME_TYPE_DELIMITER, "Error:PEBNF Error");
		PX_ASSERTIFX(!PX_strequ(PX_LexerGetLexeme(&lexer), "="), "Error: = expected but not found");
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

px_bool PX_Syntax_ExecuteAstOpcode(PX_Syntax* pSyntax, const px_char type[])
{
	px_abi newabi;
	PX_AbiCreate_DynamicWriter(&newabi, pSyntax->mp);
	if (!PX_AbiSet_string(&newabi, "type", type))
	{
		PX_AbiFree(&newabi);
		return PX_FALSE;
	}
	if (!PX_VectorPushback(&pSyntax->reg_astopcode_stack, &newabi))
	{
		PX_AbiFree(&newabi);
		return PX_FALSE;
	}
	return PX_TRUE;
}
px_int PX_Syntax_GetAbiCount(PX_Syntax* pSyntax)
{
	return pSyntax->reg_abi_stack.size;
}

px_abi* PX_Syntax_NewTypeDefine(PX_Syntax* pSyntax,const px_char name[],const px_char type[], px_int size, px_int reg_lifetime)
{
	px_abi* pabi;
	px_int i;
	for (i = pSyntax->reg_abi_stack.size - 1; i >= 0; i--)
	{
		const px_char* pname;
		px_int* plifetime;
		pabi = PX_VECTORAT(px_abi, &pSyntax->reg_abi_stack, i);
		if (PX_Syntax_CheckAbiName(pabi, "type_define"))
		{
			pname = PX_AbiGet_string(pabi, "mnemonic");
			plifetime = PX_AbiGet_int(pabi, "lifetime");
			if (!pname || !plifetime)
			{
				PX_ASSERTX("Error: type_define not found");
				continue;
			}
			if (PX_strequ(pname, name))
			{
				if (*plifetime == reg_lifetime)
				{
					return PX_NULL;
				}
			}
		}
	}
	pabi=PX_Syntax_NewAbi(pSyntax, "type_define", reg_lifetime);
	if (!pabi)
	{
		PX_Syntax_Terminate(pSyntax, "Memory Error");
		return PX_FALSE;
	}
	if (!PX_AbiSet_int(pabi, "size", size))
	{
		PX_AbiFree(pabi);
		PX_Syntax_Terminate(pSyntax, "Memory Error");
		return PX_FALSE;
	}
	if (!PX_AbiSet_string(pabi, "mnemonic", name))
	{
		PX_AbiFree(pabi);
		PX_Syntax_Terminate(pSyntax, "Memory Error");
		return PX_FALSE;
	}
	if (!PX_AbiSet_string(pabi, "type", type))
	{
		PX_AbiFree(pabi);
		PX_Syntax_Terminate(pSyntax, "Memory Error");
		return PX_FALSE;
	}
	return pabi;
}

px_abi* PX_Syntax_LastTypeDefine(PX_Syntax* pSyntax)
{
	px_abi* pabi;
	px_int i;
	for (i = pSyntax->reg_abi_stack.size; i >= 0; i--)
	{
		pabi = PX_VECTORAT(px_abi, &pSyntax->reg_abi_stack, i);
		if (PX_Syntax_CheckAbiName(pabi, "type_define"))
		{
			return pabi;
		}
	}
	return PX_NULL;
}

px_int PX_Syntax_FindTypeDefine(PX_Syntax* pSyntax, const px_char name[], px_int reg_lifetime)
{
	px_abi* pabi;
	px_int i;
	for (i = pSyntax->reg_abi_stack.size-1; i >= 0; i--)
	{
		const px_char* pname;
		px_int* plifetime;
		pabi = PX_VECTORAT(px_abi, &pSyntax->reg_abi_stack, i);
		if (PX_Syntax_CheckAbiName(pabi, "type_define"))
		{
			pname = PX_AbiGet_string(pabi, "mnemonic");
			plifetime = PX_AbiGet_int(pabi, "lifetime");
			if (!pname || !plifetime)
			{
				PX_ASSERTX("Error: type_define not found");
				continue;
			}
			if (PX_strequ(pname, name))
			{
				if (*plifetime <= reg_lifetime)
				{
					return i;
				}
			}
		}
	}
	return -1;
}


px_bool PX_Syntax_EnterScope(PX_Syntax* pSyntax)
{
	px_abi* pnewabi;
	pSyntax->reg_lifetime++;
	if (PX_NULL==( pnewabi=PX_Syntax_NewAbi(pSyntax, "scope", pSyntax->reg_lifetime)))
	{
		PX_ASSERTX("memory error");
		return PX_FALSE;
	}
	if (!PX_AbiSet_int(pnewabi, "alloc", 0))
	{
		PX_AbiFree(pnewabi);
		PX_ASSERTX("memory error");
		return PX_FALSE;
	}
	return PX_TRUE;
	
}

px_bool PX_Syntax_LeaveScope(PX_Syntax* pSyntax)
{
	px_abi* plastabi;
	px_int *p,lifetime;
	PX_ASSERTIF(pSyntax->reg_lifetime <= 0);
	while (PX_TRUE)
	{
		plastabi = PX_Syntax_GetAbiLast(pSyntax);
		PX_ASSERTIF(!plastabi);
		p = PX_AbiGet_int(plastabi, "lifetime");
		PX_ASSERTIF(!p);
		lifetime = *p;
		if (lifetime == pSyntax->reg_lifetime)
			PX_Syntax_PopAbi(pSyntax);
		else
			break;
	}
	
	pSyntax->reg_lifetime--;
	return PX_TRUE;
}

px_int  PX_Syntax_AllocLocal(PX_Syntax* pSyntax, px_int size)
{
	px_abi* pabi = PX_Syntax_FindAbiFromBackward(pSyntax, "scope");
	px_int* palloc,offset;
	if (!pabi)
	{
		PX_ASSERTX("no scope abi");
		return 0;
	}
	palloc = PX_AbiGet_int(pabi, "alloc");
	if (!palloc)
	{
		PX_ASSERTX("no alloc abi");
		return 0;
	}
	offset = *palloc;
	if (!PX_AbiSet_int(pabi, "alloc", *palloc + size))
	{
		PX_ASSERTX("memory error");
		return 0;
	}
	return offset;
}


px_int  PX_Syntax_AllocGlobal(PX_Syntax* pSyntax, px_int size)
{

	px_abi* pabi = PX_Syntax_FindAbiFromForward(pSyntax, "scope");
	px_int* palloc, offset;
	if (!pabi)
	{
		PX_ASSERTX("no scope abi");
		return 0;
	}
	palloc = PX_AbiGet_int(pabi, "alloc");
	if (!palloc)
	{
		PX_ASSERTX("no alloc abi");
		return 0;
	}
	offset = *palloc;
	if (!PX_AbiSet_int(pabi, "alloc", *palloc + size))
	{
		PX_ASSERTX("memory error");
		return 0;
	}
	return offset;
}


px_abi* PX_Syntax_GetAbiByIndex(PX_Syntax* pSyntax, px_int index)
{
	if (index >= pSyntax->reg_abi_stack.size)
	{
		PX_ASSERT();
		return PX_NULL;
	}
	if (index < -pSyntax->reg_abi_stack.size)
	{
		PX_ASSERT();
		return PX_NULL;
	}
	if(index>=0)
		return PX_VECTORAT(px_abi, &pSyntax->reg_abi_stack, index);
	else
		return PX_VECTORAT(px_abi, &pSyntax->reg_abi_stack, pSyntax->reg_abi_stack.size + index);
	
}

px_abi* PX_Syntax_GetAbiLast(PX_Syntax* pSyntax)
{
	if (pSyntax->reg_abi_stack.size == 0)
	{
		return PX_NULL;
	}
	return PX_VECTORLAST(px_abi, &pSyntax->reg_abi_stack);
}

px_abi* PX_Syntax_GetAbiSecondLast(PX_Syntax* pSyntax)
{
	if (pSyntax->reg_abi_stack.size < 2)
	{
		return PX_NULL;
	}
	return PX_VECTORAT(px_abi, &pSyntax->reg_abi_stack, pSyntax->reg_abi_stack.size - 2);
}

px_abi* PX_Syntax_GetAbi(PX_Syntax* pSyntax, const px_char name[], px_int reg_lifetime)
{
	px_abi* pabi;
	px_int i;
	for (i = pSyntax->reg_abi_stack.size; i >= 0; i--)
	{
		const px_char* pname;
		px_int* plifetime;
		pabi = PX_VECTORAT(px_abi, &pSyntax->reg_abi_stack, i);
		pname = PX_AbiGet_string(pabi, "name");
		plifetime = PX_AbiGet_int(pabi, "lifetime");
		if (!pname || !plifetime)
		{
			return PX_NULL;
		}

		if (PX_strequ(pname, name))
		{
			if (*plifetime < reg_lifetime)
			{
				return pabi;
			}
		}

	}
	return PX_NULL;
}



px_abi* PX_Syntax_NewAbi(PX_Syntax* pSyntax, const px_char name[], px_int reg_lifetime)
{
	px_abi abi;
	PX_AbiCreate_DynamicWriter(&abi, pSyntax->mp);

	if (!PX_AbiSet_string(&abi, "name", name))
	{
		PX_AbiFree(&abi);
		return PX_NULL;
	}

	if (!PX_AbiSet_int(&abi, "lifetime", reg_lifetime))
	{
		PX_AbiFree(&abi);
		return PX_NULL;
	}


	if (!PX_VectorPushback(&pSyntax->reg_abi_stack, &abi))
	{
		PX_AbiFree(&abi);
		return PX_NULL;
	}
	
	PX_Syntax_Message(pSyntax, "NEW ABI:");
	PX_Syntax_Message(pSyntax, PX_itos(pSyntax->reg_abi_stack.size - 1, 10).data);
	PX_Syntax_Message(pSyntax, ":\"");
	PX_Syntax_Message(pSyntax, name);
	PX_Syntax_Message(pSyntax, "\" ");
	PX_Syntax_Message(pSyntax, "lifetime:");
	PX_Syntax_Message(pSyntax, PX_itos(reg_lifetime,10).data);
	PX_Syntax_Message(pSyntax, "\n");
	return PX_VECTORLAST(px_abi, &pSyntax->reg_abi_stack);
}

px_abi* PX_Syntax_InsertAbi(PX_Syntax* pSyntax, const px_char name[], px_int reg_lifetime,px_int index)
{
	px_abi abi;
	PX_AbiCreate_DynamicWriter(&abi, pSyntax->mp);

	if (!PX_AbiSet_string(&abi, "name", name))
	{
		PX_AbiFree(&abi);
		return PX_NULL;
	}

	if (!PX_AbiSet_int(&abi, "lifetime", reg_lifetime))
	{
		PX_AbiFree(&abi);
		return PX_NULL;
	}

	if (PX_VectorInsert(&pSyntax->reg_abi_stack, index, &abi))
	{
		return PX_VECTORLAST(px_abi, &pSyntax->reg_abi_stack);
	}
	return PX_NULL;
}



px_void PX_Syntax_PopAbi(PX_Syntax* pSyntax)
{
	px_abi* pabi;
	if (pSyntax->reg_abi_stack.size == 0)
	{
		PX_ASSERT();
	}
	//free abi
	pabi = PX_VECTORLAST(px_abi, &pSyntax->reg_abi_stack);
	PX_AbiFree(pabi);
	PX_VectorPop(&pSyntax->reg_abi_stack);
}

px_void PX_Syntax_PopSecondAbi(PX_Syntax* pSyntax)
{
	px_abi* pabi;
	if (pSyntax->reg_abi_stack.size < 2)
	{
		PX_ASSERT();
	}
	pabi = PX_VECTORAT(px_abi, &pSyntax->reg_abi_stack, pSyntax->reg_abi_stack.size - 2);
	PX_AbiFree(pabi);
	PX_VectorErase(&pSyntax->reg_abi_stack, pSyntax->reg_abi_stack.size - 2);
}

px_void PX_Syntax_PopAbiIndex(PX_Syntax* pSyntax,px_int index)
{
	px_abi* pabi;
	if (index >= pSyntax->reg_abi_stack.size)
	{
		PX_ASSERT();
	}
	if (index < -pSyntax->reg_abi_stack.size)
	{
		PX_ASSERT();
	}
	if (index >= 0)
	{
		pabi = PX_VECTORAT(px_abi, &pSyntax->reg_abi_stack, index);
		PX_AbiFree(pabi);
		PX_VectorErase(&pSyntax->reg_abi_stack, index);
	}
	else
	{
		pabi = PX_VECTORAT(px_abi, &pSyntax->reg_abi_stack, pSyntax->reg_abi_stack.size + index);
		PX_AbiFree(pabi);
		PX_VectorErase(&pSyntax->reg_abi_stack, pSyntax->reg_abi_stack.size + index);
	}
}

px_bool PX_Syntax_MergeLastAbi(PX_Syntax* pSyntax, const px_char new_name[])
{
	px_abi* ppnewabi = PX_Syntax_NewAbi(pSyntax, new_name, pSyntax->reg_lifetime);
	px_abi* poldlastabi = PX_Syntax_GetAbiSecondLast(pSyntax);
	const px_char* pname;
	if (!ppnewabi || !poldlastabi)
	{
		return PX_FALSE;
	}
	pname = PX_AbiGet_string(poldlastabi, "name");
	if (!pname)
	{
		return PX_FALSE;
	}
	PX_Syntax_Message(pSyntax, "Merge ");
	PX_Syntax_Message(pSyntax, pname);
	PX_Syntax_Message(pSyntax, " to ");
	PX_Syntax_Message(pSyntax, new_name);
	PX_Syntax_Message(pSyntax, "\n");
	if (!PX_AbiSet_Abi(ppnewabi, pname, poldlastabi))
	{
		return PX_FALSE;
	}
	PX_Syntax_PopSecondAbi(pSyntax);
	
	return PX_TRUE;
}


px_bool PX_Syntax_MergeLast2AbiToSecondLast(PX_Syntax* pSyntax)
{
	px_abi* plastabi, * psecondlastabi;
	const px_char* plastname, * psecondlastname;
	plastabi = PX_Syntax_GetAbiLast(pSyntax);
	psecondlastabi = PX_Syntax_GetAbiSecondLast(pSyntax);
	if (!plastabi || !psecondlastabi)
	{
		return PX_FALSE;
	}
	plastname = PX_AbiGet_string(plastabi, "name");
	psecondlastname = PX_AbiGet_string(psecondlastabi, "name");
	if (!plastname || !psecondlastname)
	{
		return PX_FALSE;
	}
	PX_Syntax_Message(pSyntax, "Merge ");
	PX_Syntax_Message(pSyntax, plastname);
	PX_Syntax_Message(pSyntax, ",");
	PX_Syntax_Message(pSyntax, psecondlastname);
	PX_Syntax_Message(pSyntax, " to ");
	PX_Syntax_Message(pSyntax, psecondlastname);
	PX_Syntax_Message(pSyntax, "\n");
	if (!PX_AbiSet_Abi(psecondlastabi, plastname, plastabi))
	{
		return PX_FALSE;
	}
	PX_Syntax_PopAbi(pSyntax);
	
	return PX_TRUE;
}
px_bool PX_Syntax_MergeLastToIndexWithName(PX_Syntax* pSyntax, px_int index,const px_char new_name[])
{
	px_abi* plastabi, * ptargetabi;
	const px_char* plastname, * pindexname;
	plastabi = PX_Syntax_GetAbiLast(pSyntax);
	ptargetabi = PX_Syntax_GetAbiByIndex(pSyntax, index);
	if (!plastabi || !ptargetabi)
	{
		return PX_FALSE;
	}
	plastname = PX_AbiGet_string(plastabi, "name");
	pindexname = PX_AbiGet_string(ptargetabi, "name");
	if (!plastname || !pindexname)
	{
		return PX_FALSE;
	}
	PX_Syntax_Message(pSyntax, "Merge ");
	PX_Syntax_Message(pSyntax, plastname);
	PX_Syntax_Message(pSyntax, ",");
	PX_Syntax_Message(pSyntax, pindexname);
	PX_Syntax_Message(pSyntax, " to Index:");
	PX_Syntax_Message(pSyntax, PX_itos(index, 10).data);
	PX_Syntax_Message(pSyntax, ":");
	PX_Syntax_Message(pSyntax, new_name);
	PX_Syntax_Message(pSyntax, "\n");
	if (!PX_AbiSet_Abi(ptargetabi, new_name, plastabi))
	{
		return PX_FALSE;
	}
	PX_Syntax_PopAbi(pSyntax);
	return PX_TRUE;
}

px_bool PX_Syntax_CopyAbiToLast(PX_Syntax* pSyntax, px_int index)
{
	px_abi* plastabi, * ptargetabi;
	const px_char* plastname, * pindexname;
	plastabi = PX_Syntax_GetAbiLast(pSyntax);
	ptargetabi = PX_Syntax_GetAbiByIndex(pSyntax, index);
	if (!plastabi || !ptargetabi)
	{
		return PX_FALSE;
	}
	plastname = PX_AbiGet_string(plastabi, "name");
	pindexname = PX_AbiGet_string(ptargetabi, "name");
	if (!plastname || !pindexname)
	{
		return PX_FALSE;
	}
	PX_Syntax_Message(pSyntax, "Copy ");
	PX_Syntax_Message(pSyntax, plastname);
	PX_Syntax_Message(pSyntax, ",");
	PX_Syntax_Message(pSyntax, pindexname);
	PX_Syntax_Message(pSyntax, " to ");
	PX_Syntax_Message(pSyntax, plastname);
	PX_Syntax_Message(pSyntax, "\n");
	if (!PX_AbiSet_Abi(plastabi, pindexname, ptargetabi))
	{
		return PX_FALSE;
	}
	return PX_TRUE;
}

px_bool PX_Syntax_RenameLastAbi(PX_Syntax* pSyntax, const px_char name[])
{
	px_abi* plastabi;
	const px_char* plastname;
	plastabi = PX_Syntax_GetAbiLast(pSyntax);
	if (!plastabi)
	{
		return PX_FALSE;
	}
	plastname = PX_AbiGet_string(plastabi, "name");
	if (!plastname)
	{
		return PX_FALSE;
	}
	PX_Syntax_Message(pSyntax, "Rename ");
	PX_Syntax_Message(pSyntax, plastname);
	PX_Syntax_Message(pSyntax, " to ");
	PX_Syntax_Message(pSyntax, name);
	PX_Syntax_Message(pSyntax, "\n");
	if (!PX_AbiSet_string(plastabi, "name", name))
	{
		return PX_FALSE;
	}
	return PX_TRUE;
	
}

px_bool PX_Syntax_MergeLast2AbiToLast(PX_Syntax* pSyntax)
{
	px_abi* plastabi, * psecondlastabi;
	const px_char* plastname, * psecondlastname;
	plastabi = PX_Syntax_GetAbiLast(pSyntax);
	psecondlastabi = PX_Syntax_GetAbiSecondLast(pSyntax);
	if (!plastabi || !psecondlastabi)
	{
		return PX_FALSE;
	}
	plastname = PX_AbiGet_string(plastabi, "name");
	psecondlastname = PX_AbiGet_string(psecondlastabi, "name");
	if (!plastname || !psecondlastname)
	{
		return PX_FALSE;
	}
	PX_Syntax_Message(pSyntax, "Merge ");
	PX_Syntax_Message(pSyntax, plastname);
	PX_Syntax_Message(pSyntax, ",");
	PX_Syntax_Message(pSyntax, psecondlastname);
	PX_Syntax_Message(pSyntax, " to ");
	PX_Syntax_Message(pSyntax, plastname);
	PX_Syntax_Message(pSyntax, "\n");
	if (!PX_AbiSet_Abi(plastabi, plastname, psecondlastabi))
	{
		return PX_FALSE;
	}
	PX_Syntax_PopSecondAbi(pSyntax);

	return PX_TRUE;
}

px_bool PX_Syntax_MergeLast2AbiWithNameToSecondLast(PX_Syntax* pSyntax,const px_char newname[])
{
	px_abi* plastabi, * psecondlastabi;
	const px_char* plastname, * psecondlastname;
	plastabi = PX_Syntax_GetAbiLast(pSyntax);
	psecondlastabi = PX_Syntax_GetAbiSecondLast(pSyntax);
	if (!plastabi || !psecondlastabi)
	{
		return PX_FALSE;
	}
	plastname = PX_AbiGet_string(plastabi, "name");
	psecondlastname = PX_AbiGet_string(psecondlastabi, "name");
	if (!plastname || !psecondlastname)
	{
		return PX_FALSE;
	}
	PX_Syntax_Message(pSyntax, "Merge ");
	PX_Syntax_Message(pSyntax, plastname);
	PX_Syntax_Message(pSyntax, ",");
	PX_Syntax_Message(pSyntax, psecondlastname);
	PX_Syntax_Message(pSyntax, " to ");
	PX_Syntax_Message(pSyntax, psecondlastname);
	PX_Syntax_Message(pSyntax, " with name ");
	PX_Syntax_Message(pSyntax, newname);
	PX_Syntax_Message(pSyntax, "\n");
	if (!PX_AbiSet_Abi(psecondlastabi, newname, plastabi))
	{
		return PX_FALSE;
	}
	PX_Syntax_PopAbi(pSyntax);
	
	return PX_TRUE;
}
px_bool PX_Syntax_MergeLast2AbiWithNameToLast(PX_Syntax* pSyntax, const px_char newname[])
{
	px_abi* plastabi, * psecondlastabi;
	const px_char* plastname, * psecondlastname;
	plastabi = PX_Syntax_GetAbiLast(pSyntax);
	psecondlastabi = PX_Syntax_GetAbiSecondLast(pSyntax);
	if (!plastabi || !psecondlastabi)
	{
		return PX_FALSE;
	}
	plastname = PX_AbiGet_string(plastabi, "name");
	psecondlastname = PX_AbiGet_string(psecondlastabi, "name");
	if (!plastname || !psecondlastname)
	{
		return PX_FALSE;
	}
	PX_Syntax_Message(pSyntax, "Merge ");
	PX_Syntax_Message(pSyntax, plastname);
	PX_Syntax_Message(pSyntax, ",");
	PX_Syntax_Message(pSyntax, psecondlastname);
	PX_Syntax_Message(pSyntax, " to ");
	PX_Syntax_Message(pSyntax, newname);
	PX_Syntax_Message(pSyntax, "\n");
	if (!PX_AbiSet_Abi(plastabi, newname, psecondlastabi))
	{
		return PX_FALSE;
	}
	
	PX_Syntax_PopSecondAbi(pSyntax);
	return PX_TRUE;
}

px_bool PX_Syntax_MergeLast2AbiToNewAbi(PX_Syntax* pSyntax, const px_char newname[])
{
	px_abi* plastabi, lastabi,* psecondlastabi, secondlastabi,* pnewabi;
	const px_char* plastname, * psecondlastname;
	plastabi = PX_Syntax_GetAbiLast(pSyntax);
	psecondlastabi = PX_Syntax_GetAbiSecondLast(pSyntax);
	if (!plastabi || !psecondlastabi)
	{
		return PX_FALSE;
	}

	lastabi = *plastabi;
	secondlastabi = *psecondlastabi;

	plastname = PX_AbiGet_string(plastabi, "name");
	psecondlastname = PX_AbiGet_string(psecondlastabi, "name");
	if (!plastname || !psecondlastname)
	{
		return PX_FALSE;
	}
	PX_Syntax_Message(pSyntax, "Merge ");
	PX_Syntax_Message(pSyntax, plastname);
	PX_Syntax_Message(pSyntax, ",");
	PX_Syntax_Message(pSyntax, psecondlastname);
	PX_Syntax_Message(pSyntax, " to ");
	PX_Syntax_Message(pSyntax, newname);
	PX_Syntax_Message(pSyntax, "\n");
	pnewabi = PX_Syntax_NewAbi(pSyntax, newname, pSyntax->reg_lifetime);
	if (!pnewabi)
	{
		PX_Syntax_Terminate(pSyntax, "Memory Error");
		return PX_FALSE;
	}
	if (!PX_AbiSet_Abi(pnewabi, psecondlastname, &secondlastabi))
	{
		return PX_FALSE;
	}
	if (!PX_AbiSet_Abi(pnewabi, plastname, &lastabi))
	{
		return PX_FALSE;
	}

	PX_Syntax_PopSecondAbi(pSyntax);
	PX_Syntax_PopSecondAbi(pSyntax);
	return PX_TRUE;

}
px_void PX_Syntax_PopAbiStackN(PX_Syntax* pSyntax, px_int N)
{
	px_int i;
	px_abi* pabi;
	if (pSyntax->reg_abi_stack.size < N)
	{
		PX_ASSERT();
	}
	for (i = 0; i < N; i++)
	{
		pabi = PX_VECTORLAST(px_abi, &pSyntax->reg_abi_stack);
		PX_AbiFree(pabi);
		PX_VectorPop(&pSyntax->reg_abi_stack);
	}
}

PX_SYNTAX_FUNCTION(PX_Syntax_Parse_expand_parameter)
{
	px_string str;
	px_int bracket = 0;
	px_bool bstring = PX_FALSE;
	px_abi* newabi;
	if (!PX_StringInitialize(pSyntax->mp, &str))
	{
		PX_Syntax_Terminate(pSyntax, "out of memory");
		return PX_FALSE;
	}
	while (PX_TRUE)
	{
		px_char ch = PX_Syntax_GetNextChar(pSyntax);
		if (ch=='0')
		{
			PX_Syntax_Terminate(pSyntax, "Syntax Error:unexpected end of file");
			PX_StringFree(&str);
			return PX_FALSE;
		}
		if (ch==','&&bracket==0&&!bstring)
		{
			PX_LexerSetOffset(PX_Syntax_GetCurrentLexer(pSyntax), PX_Syntax_GetCurrentLexer(pSyntax)->SourceOffset - 1);
			break;
		}
		else if (ch=='(')
		{
			bracket++;
		}
		else if (ch == '"')
		{
			bstring = !bstring;
		}
		else if (ch==')')
		{
			if (bracket==0)
			{
				PX_LexerSetOffset(PX_Syntax_GetCurrentLexer(pSyntax), PX_Syntax_GetCurrentLexer(pSyntax)->SourceOffset - 1);
				break;
			}
			bracket--;
		}
		if (!PX_StringCatChar(&str, ch))
		{
			PX_Syntax_Terminate(pSyntax, "Memory Error");
			PX_StringFree(&str);
			return PX_FALSE;
		}
	}
	newabi = PX_Syntax_NewAbi(pSyntax, "expand_parameter", pSyntax->reg_lifetime);
	if (!newabi)
	{
		PX_Syntax_Terminate(pSyntax, "Memory Error");
		PX_StringFree(&str);
		return PX_FALSE;
	}
	if (!PX_AbiSet_string(newabi, "value", str.buffer))
	{
		PX_Syntax_Terminate(pSyntax, "Memory Error");
		PX_StringFree(&str);
		return PX_FALSE;
	}
	PX_StringFree(&str);
	return PX_TRUE;
}

PX_SYNTAX_FUNCTION(PX_Syntax_Parse_expand_parameters_begin)
{
	px_abi* pnewabi;
	pnewabi = PX_Syntax_NewAbi(pSyntax, "expand_parameters", pSyntax->reg_lifetime);
	if (!pnewabi)
	{
		PX_Syntax_Terminate(pSyntax,  "Memory Error");
		return PX_FALSE;
	}
	if (!PX_AbiSet_int(pnewabi, "count", 0))
	{
		PX_Syntax_Terminate(pSyntax, "Memory Error");
		return PX_FALSE;
	}
	return PX_TRUE;

}

PX_SYNTAX_FUNCTION(PX_Syntax_Parse_expand_parameters_new)
{
	px_abi* plastabi = PX_Syntax_GetAbiLast(pSyntax);
	px_abi* psecondlastabi = PX_Syntax_GetAbiSecondLast(pSyntax);
	const px_char* pname;
	px_int* pcount, count;
	px_char content[32] = { 0 };
	if (!plastabi || !psecondlastabi)
	{
		return PX_FALSE;
	}
	pname = PX_AbiGet_string(plastabi, "name");
	if (!pname || !PX_strequ(pname, "expand_parameter"))
	{
		return PX_FALSE;
	}
	pcount = PX_AbiGet_int(psecondlastabi, "count");
	if (!pcount)
	{
		return PX_FALSE;
	}
	count = *pcount;
	PX_AbiSet_int(psecondlastabi, "count", count + 1);
	PX_sprintf1(content, sizeof(content), "expand_parameter[%1]", PX_STRINGFORMAT_INT(count));

	if (!PX_Syntax_MergeLast2AbiWithNameToSecondLast(pSyntax, content))
	{
		PX_ASSERTX("Merge Error");
		return PX_FALSE;
	}
	return PX_TRUE;
}

PX_SYNTAX_FUNCTION(PX_Syntax_Parse_expand_parameters_end)
{
	return PX_TRUE;
}

PX_SYNTAX_FUNCTION(PX_Syntax_Parse_expand_parameters_error)
{
	PX_Syntax_Terminate(pSyntax, "Syntax Error:unexpected expand_parameters");
	return PX_FALSE;
}

px_bool PX_Syntax_load_expand_parameters(PX_Syntax* pSyntax)
{
	if (!PX_Syntax_Parse_PEBNF(pSyntax, "expand_parameter = *", PX_Syntax_Parse_expand_parameter))
		return PX_FALSE;

	if (!PX_Syntax_Parse_PEBNF(pSyntax, "expand_parameters_list = expand_parameter", PX_Syntax_Parse_expand_parameters_new))
		return PX_FALSE;

	if (!PX_Syntax_Parse_PEBNF(pSyntax, "expand_parameters_list = expand_parameter ',' expand_parameters_list", 0))
		return PX_FALSE;

	if (!PX_Syntax_Parse_PEBNF(pSyntax, "expand_parameters_list = expand_parameter ',' *", PX_Syntax_Parse_expand_parameters_error))
		return PX_FALSE;

	if (!PX_Syntax_Parse_PEBNF(pSyntax, "expand_parameters_list = expand_parameter *", PX_Syntax_Parse_expand_parameters_end))
		return PX_FALSE;

	if (!PX_Syntax_Parse_PEBNF(pSyntax, "expand_parameters = '('", PX_Syntax_Parse_expand_parameters_begin))
		return PX_FALSE;

	if (!PX_Syntax_Parse_PEBNF(pSyntax, "expand_parameters = '(' ')'", 0))
		return PX_FALSE;

	if (!PX_Syntax_Parse_PEBNF(pSyntax, "expand_parameters = '(' expand_parameters_list ')'", 0))
		return PX_FALSE;

	return PX_TRUE;
}


px_bool PX_Syntax_Initialize(px_memorypool* mp, PX_Syntax* pSyntax)
{
	pSyntax->mp = mp;

	if (!PX_StringInitialize(mp, &pSyntax->message))
		return PX_FALSE;

	if (!PX_StringInitialize(mp, &pSyntax->ast_message))
		return PX_FALSE;

	PX_VectorInitialize(mp, &pSyntax->pebnf, sizeof(PX_Syntax_pebnf), 0);
	PX_VectorInitialize(mp, &pSyntax->sources, sizeof(PX_Syntax_Source), 0);
	PX_VectorInitialize(mp, &pSyntax->reg_ast_stack, sizeof(PX_Syntax_ast), 0);
	PX_VectorInitialize(mp, &pSyntax->reg_abi_stack, sizeof(px_abi), 0);
	PX_VectorInitialize(mp, &pSyntax->reg_astopcode_stack, sizeof(px_abi), 0);
	PX_VectorInitialize(mp, &pSyntax->reg_lexer_stack, sizeof(PX_Syntax_Lexer), 0);
	PX_VectorInitialize(mp, &pSyntax->reg_expand_stack, sizeof(PX_Syntax_expand), 0);
	PX_VectorInitialize(mp, &pSyntax->reg_oprand_stack, sizeof(px_abi), 0);
	PX_VectorInitialize(mp, &pSyntax->reg_opcode_stack, sizeof(PX_Syntax_opcode), 0);
	PX_VectorInitialize(mp, &pSyntax->reg_operate_stack, sizeof(PX_Syntax_operate), 0);
	PX_StringInitialize(mp, &pSyntax->reg_ir);


	pSyntax->reg_lifetime = 0;
	pSyntax->reg_lexer_index = -1;
	PX_memset(&pSyntax->bnfnode_return, 0, sizeof(PX_Syntax_bnfnode));
	PX_StringInitialize(mp, &pSyntax->bnfnode_return.constant);
	if (!PX_Syntax_load_expand_parameters(pSyntax))
		return PX_FALSE;

	return PX_TRUE;
}

px_void PX_Syntax_Clear(PX_Syntax* pSyntax)
{
	px_int i;
	PX_Syntax_Source* psource;
	px_abi* pabi;
	px_abi* popcode;
	PX_Syntax_Lexer* plexer;
	for (i = 0; i < pSyntax->sources.size; i++)
	{
		psource = PX_VECTORAT(PX_Syntax_Source, &pSyntax->sources, i);
		PX_StringFree(&psource->name);
		PX_StringFree(&psource->source);
	}
	PX_VectorClear(&pSyntax->sources);
	PX_VectorClear(&pSyntax->reg_ast_stack);
	for (i = 0; i < pSyntax->reg_abi_stack.size; i++)
	{
		pabi = PX_VECTORAT(px_abi, &pSyntax->reg_abi_stack, i);
		PX_AbiFree(pabi);
	}
	PX_VectorClear(&pSyntax->reg_abi_stack);

	for (i = 0; i < pSyntax->reg_astopcode_stack.size; i++)
	{
		popcode = PX_VECTORAT(px_abi, &pSyntax->reg_astopcode_stack, i);
		PX_AbiFree(popcode);
	}
	PX_VectorClear(&pSyntax->reg_astopcode_stack);

	for (i = 0; i < pSyntax->reg_lexer_stack.size; i++)
	{
		plexer = PX_VECTORAT(PX_Syntax_Lexer, &pSyntax->reg_lexer_stack, i);
		PX_LexerFree(&plexer->lexer);
	}

	for (i = 0; i < pSyntax->reg_expand_stack.size; i++)
	{
		PX_Syntax_expand* pexpand = PX_VECTORAT(PX_Syntax_expand, &pSyntax->reg_expand_stack, i);
		PX_StringFree(&pexpand->format);
		PX_StringFree(&pexpand->token);
	}

	for (i = 0; i < pSyntax->reg_oprand_stack.size; i++)
	{
		px_abi* poprand = PX_VECTORAT(px_abi, &pSyntax->reg_oprand_stack, i);
		PX_AbiFree(poprand);
	}

	PX_VectorClear(&pSyntax->reg_expand_stack);
	PX_VectorClear(&pSyntax->reg_lexer_stack);
	PX_VectorClear(&pSyntax->reg_oprand_stack);
	PX_StringClear(&pSyntax->message);
	PX_StringClear(&pSyntax->ast_message);
	PX_StringClear(&pSyntax->reg_ir);
	pSyntax->reg_lifetime = 0;
	pSyntax->reg_unname_index = 0;
	PX_memset(pSyntax->reg_unname, 0, sizeof(pSyntax->reg_unname));

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
px_void PX_Syntax_Free(PX_Syntax* pSyntax)
{
	px_int i;
	PX_Syntax_pebnf* ppebnf;
	PX_Syntax_Clear(pSyntax);
	for (i = 0; i < pSyntax->pebnf.size; i++)
	{
		ppebnf = PX_VECTORAT(PX_Syntax_pebnf, &pSyntax->pebnf, i);
		PX_SyntaxFree_pebnfnode(pSyntax, ppebnf->pbnfnode);
		PX_StringFree(&ppebnf->mnenonic);
	}
	for (i = 0; i < pSyntax->reg_operate_stack.size; i++)
	{
		PX_Syntax_operate* poperate = PX_VECTORAT(PX_Syntax_operate, &pSyntax->reg_operate_stack, i);
		PX_StringFree(&poperate->oprand1_type);
		PX_StringFree(&poperate->oprand2_type);
		PX_StringFree(&poperate->oprand3_type);

	}

	PX_VectorFree(&pSyntax->pebnf);
	PX_VectorFree(&pSyntax->sources);
	PX_VectorFree(&pSyntax->reg_ast_stack);
	PX_VectorFree(&pSyntax->reg_abi_stack);
	PX_VectorFree(&pSyntax->reg_astopcode_stack);
	PX_VectorFree(&pSyntax->reg_lexer_stack);
	PX_VectorFree(&pSyntax->reg_expand_stack);
	PX_VectorFree(&pSyntax->reg_opcode_stack);
	PX_StringFree(&pSyntax->message);
	PX_StringFree(&pSyntax->ast_message);
	PX_StringFree(&pSyntax->bnfnode_return.constant);
	PX_StringFree(&pSyntax->reg_ir);

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

typedef enum
{
	PX_SYNTAX_LEXER_FILTER_NONE = 0,
	PX_SYNTAX_LEXER_FILTER_NEWLINE=1,
	PX_SYNTAX_LEXER_FILTER_SPACER=2,
}PX_SYNTAX_LEXER_FILTER;


static PX_LEXER_LEXEME_TYPE PX_Syntax_GetNextLexemeEx2(PX_Syntax* pSyntax,px_int filter)
{
	while (PX_TRUE)
	{
		px_int i, offset, ilexerIndex;
		PX_Syntax_Lexer* pCurrentSyntaxLexer;
		if (pSyntax->reg_lexer_index < pSyntax->reg_lexer_stack.size)
			pCurrentSyntaxLexer = PX_VECTORAT(PX_Syntax_Lexer, &pSyntax->reg_lexer_stack, pSyntax->reg_lexer_index);
		else
		{
			PX_ASSERT();
			return PX_LEXER_LEXEME_TYPE_ERR;
		}
		offset = PX_LexerGetOffset(&pCurrentSyntaxLexer->lexer);
		ilexerIndex = pSyntax->reg_lexer_index;
		for (i = 0; i < pSyntax->reg_lexer_stack.size; i++)
		{
			PX_Syntax_Lexer* pSyntaxLexer = PX_VECTORAT(PX_Syntax_Lexer, &pSyntax->reg_lexer_stack, i);
			if (offset == pSyntaxLexer->expand_lexer_offset_end && ilexerIndex == pSyntaxLexer->expand_lexer_index)
			{
				PX_LexerSetOffset(&pCurrentSyntaxLexer->lexer, pSyntaxLexer->expand_lexer_offset_end);
				PX_LexerReset(&pSyntaxLexer->lexer);
				pSyntax->reg_lexer_index = i;
				break;
			}
		}

		if (i== pSyntax->reg_lexer_stack.size)
		{
			PX_LEXER_LEXEME_TYPE type;
			while (PX_TRUE)
			{
				type = PX_LexerGetNextLexeme(&pCurrentSyntaxLexer->lexer);
				if ((filter& PX_SYNTAX_LEXER_FILTER_NEWLINE)&&type == PX_LEXER_LEXEME_TYPE_NEWLINE )
					continue;

				if ((filter & PX_SYNTAX_LEXER_FILTER_SPACER) && type == PX_LEXER_LEXEME_TYPE_SPACER)
					continue;

				else if (type == PX_LEXER_LEXEME_TYPE_END)
				{
					PX_Syntax_Message(pSyntax, "AST:End of lexer index from ");
					PX_Syntax_Message(pSyntax, PX_itos(pSyntax->reg_lexer_index,10).data);
					PX_Syntax_Message(pSyntax, " to ");
					PX_Syntax_Message(pSyntax, PX_itos(pCurrentSyntaxLexer->expand_lexer_index, 10).data);
					PX_Syntax_Message(pSyntax, "\n");

					pSyntax->reg_lexer_index = pCurrentSyntaxLexer->expand_lexer_index;
					if (pSyntax->reg_lexer_index == -1)
					{
						return PX_LEXER_LEXEME_TYPE_END;
					}
					pCurrentSyntaxLexer = PX_VECTORAT(PX_Syntax_Lexer, &pSyntax->reg_lexer_stack, pSyntax->reg_lexer_index);
				}
				else
					return type;
			}
		}
	}
	return PX_LEXER_LEXEME_TYPE_ERR;
}
static PX_LEXER_LEXEME_TYPE PX_Syntax_GetNextLexemeEx(PX_Syntax* pSyntax, px_int filter)
{
	px_int last_entry = -1;
	px_int i; 
	PX_LEXER_LEXEME_TYPE type;
	while (PX_TRUE)
	{
		type = PX_Syntax_GetNextLexemeEx2(pSyntax, filter);
		if (type== PX_LEXER_LEXEME_TYPE_END|| type== PX_LEXER_LEXEME_TYPE_ERR)
		{
			return type;
		}
		//search exist expand lexer
		for ( i = 0; i < pSyntax->reg_lexer_stack.size; i++)
		{
			PX_Syntax_Lexer* pSyntaxLexer = PX_VECTORAT(PX_Syntax_Lexer, &pSyntax->reg_lexer_stack, i);
			if (pSyntaxLexer->expand_lexer_index == pSyntax->reg_lexer_index)
			{
				if (PX_Syntax_GetCurrentLexemeBegin(pSyntax) == pSyntaxLexer->expand_lexer_offset_begin)
				{
					PX_Syntax_EntrySyntaxExpandLexer(pSyntax, i);
					if (last_entry==-1)
					{
						last_entry = i;
					}
					else if(last_entry==i)
					{
						PX_Syntax_Terminate(pSyntax, "Error: circular recursive macro definition");
						return PX_LEXER_LEXEME_TYPE_ERR;
					}
					continue;
				}
			
			}

		}

		//expand new lexer
		if (type==PX_LEXER_LEXEME_TYPE_TOKEN)
		{
			const px_char* plexer = PX_Syntax_GetCurrentLexeme(pSyntax);
			px_int i,begin,end;
			PX_Syntax_expand* pexpand;
			for (i = 0; i < pSyntax->reg_expand_stack.size; i++)
			{
				pexpand = PX_VECTORAT(PX_Syntax_expand, &pSyntax->reg_expand_stack, i);
				if (PX_strequ(pexpand->token.buffer, plexer))
				{
					px_int* pparameters_count;
					px_string expand_source;
					px_abi* plastabi;
					px_char nextchar;
					begin = PX_Syntax_GetCurrentLexemeBegin(pSyntax);
					nextchar = PX_Syntax_PreviewNextChar(pSyntax);
					if (nextchar=='(')
					{
						if (!PX_Syntax_Execute(pSyntax, "expand_parameters"))
						{
							PX_Syntax_Terminate(pSyntax, "Error:expand_parameters execute error");
							return PX_LEXER_LEXEME_TYPE_ERR;
						}
						plastabi = PX_Syntax_GetAbiLast(pSyntax);
						if (!plastabi || !PX_Syntax_CheckAbiName(plastabi, "expand_parameters"))
						{
							PX_Syntax_Terminate(pSyntax, "Error:expand_parameters error");
							return PX_LEXER_LEXEME_TYPE_ERR;
						}
						pparameters_count = PX_AbiGet_int(PX_Syntax_GetAbiLast(pSyntax), "count");
						PX_ASSERTIFX(!pparameters_count, "Error:expand_parameters count error");
						if (*pparameters_count != pexpand->parameters_count)
						{
							PX_Syntax_Terminate(pSyntax, "Error:expand_parameters count error");
							return PX_LEXER_LEXEME_TYPE_ERR;
						}
					}
					else
					{
						if (pexpand->parameters_count != 0)
						{
							PX_Syntax_Terminate(pSyntax, "Error:expand_parameters count error");
							return PX_LEXER_LEXEME_TYPE_ERR;
						}
					}
					end = PX_Syntax_GetCurrentLexemeEnd(pSyntax);
					
					if(!PX_StringInitialize(pSyntax->mp, &expand_source))
					{
						PX_Syntax_Terminate(pSyntax, "Error:out of memory");
						return PX_LEXER_LEXEME_TYPE_ERR;
					}
					
					switch (pexpand->parameters_count)
					{
					case 0:
						PX_StringCat(&expand_source, pexpand->format.buffer);
						break;
					case 1:
						PX_StringFormat1(&expand_source, pexpand->format.buffer, \
							PX_STRINGFORMAT_STRING(PX_AbiGet_string(PX_Syntax_GetAbiLast(pSyntax), "expand_parameter[0].value"))\
						);
						PX_Syntax_PopAbi(pSyntax);
						break;
					case 2:
						PX_StringFormat2(&expand_source, pexpand->format.buffer, \
							PX_STRINGFORMAT_STRING(PX_AbiGet_string(PX_Syntax_GetAbiLast(pSyntax), "expand_parameter[0].value")), \
							PX_STRINGFORMAT_STRING(PX_AbiGet_string(PX_Syntax_GetAbiLast(pSyntax), "expand_parameter[1].value"))\
						);
						PX_Syntax_PopAbi(pSyntax);
						break;
					case 3:
						PX_StringFormat3(&expand_source, pexpand->format.buffer, \
							PX_STRINGFORMAT_STRING(PX_AbiGet_string(PX_Syntax_GetAbiLast(pSyntax), "expand_parameter[0].value")), \
							PX_STRINGFORMAT_STRING(PX_AbiGet_string(PX_Syntax_GetAbiLast(pSyntax), "expand_parameter[1].value")), \
							PX_STRINGFORMAT_STRING(PX_AbiGet_string(PX_Syntax_GetAbiLast(pSyntax), "expand_parameter[2].value"))\
						);
						PX_Syntax_PopAbi(pSyntax);
						break;
					case 4:
						PX_StringFormat4(&expand_source, pexpand->format.buffer, \
							PX_STRINGFORMAT_STRING(PX_AbiGet_string(PX_Syntax_GetAbiLast(pSyntax), "expand_parameter[0].value")), \
							PX_STRINGFORMAT_STRING(PX_AbiGet_string(PX_Syntax_GetAbiLast(pSyntax), "expand_parameter[1].value")), \
							PX_STRINGFORMAT_STRING(PX_AbiGet_string(PX_Syntax_GetAbiLast(pSyntax), "expand_parameter[2].value")), \
							PX_STRINGFORMAT_STRING(PX_AbiGet_string(PX_Syntax_GetAbiLast(pSyntax), "expand_parameter[3].value"))\
						);
						PX_Syntax_PopAbi(pSyntax);
						break;
					case 5:
						PX_StringFormat5(&expand_source, pexpand->format.buffer, \
							PX_STRINGFORMAT_STRING(PX_AbiGet_string(PX_Syntax_GetAbiLast(pSyntax), "expand_parameter[0].value")), \
							PX_STRINGFORMAT_STRING(PX_AbiGet_string(PX_Syntax_GetAbiLast(pSyntax), "expand_parameter[1].value")), \
							PX_STRINGFORMAT_STRING(PX_AbiGet_string(PX_Syntax_GetAbiLast(pSyntax), "expand_parameter[2].value")), \
							PX_STRINGFORMAT_STRING(PX_AbiGet_string(PX_Syntax_GetAbiLast(pSyntax), "expand_parameter[3].value")), \
							PX_STRINGFORMAT_STRING(PX_AbiGet_string(PX_Syntax_GetAbiLast(pSyntax), "expand_parameter[4].value"))\
						);
						PX_Syntax_PopAbi(pSyntax);
						break;
					case 6:
						PX_StringFormat6(&expand_source, pexpand->format.buffer, \
							PX_STRINGFORMAT_STRING(PX_AbiGet_string(PX_Syntax_GetAbiLast(pSyntax), "expand_parameter[0].value")), \
							PX_STRINGFORMAT_STRING(PX_AbiGet_string(PX_Syntax_GetAbiLast(pSyntax), "expand_parameter[1].value")), \
							PX_STRINGFORMAT_STRING(PX_AbiGet_string(PX_Syntax_GetAbiLast(pSyntax), "expand_parameter[2].value")), \
							PX_STRINGFORMAT_STRING(PX_AbiGet_string(PX_Syntax_GetAbiLast(pSyntax), "expand_parameter[3].value")), \
							PX_STRINGFORMAT_STRING(PX_AbiGet_string(PX_Syntax_GetAbiLast(pSyntax), "expand_parameter[4].value")), \
							PX_STRINGFORMAT_STRING(PX_AbiGet_string(PX_Syntax_GetAbiLast(pSyntax), "expand_parameter[5].value"))\
						);
						PX_Syntax_PopAbi(pSyntax);
						break;
					case 7:
						PX_StringFormat7(&expand_source, pexpand->format.buffer, \
							PX_STRINGFORMAT_STRING(PX_AbiGet_string(PX_Syntax_GetAbiLast(pSyntax), "expand_parameter[0].value")), \
							PX_STRINGFORMAT_STRING(PX_AbiGet_string(PX_Syntax_GetAbiLast(pSyntax), "expand_parameter[1].value")), \
							PX_STRINGFORMAT_STRING(PX_AbiGet_string(PX_Syntax_GetAbiLast(pSyntax), "expand_parameter[2].value")), \
							PX_STRINGFORMAT_STRING(PX_AbiGet_string(PX_Syntax_GetAbiLast(pSyntax), "expand_parameter[3].value")), \
							PX_STRINGFORMAT_STRING(PX_AbiGet_string(PX_Syntax_GetAbiLast(pSyntax), "expand_parameter[4].value")), \
							PX_STRINGFORMAT_STRING(PX_AbiGet_string(PX_Syntax_GetAbiLast(pSyntax), "expand_parameter[5].value")), \
							PX_STRINGFORMAT_STRING(PX_AbiGet_string(PX_Syntax_GetAbiLast(pSyntax), "expand_parameter[6].value"))\
						);
						PX_Syntax_PopAbi(pSyntax);
						break;
					case 8:
						PX_StringFormat8(&expand_source, pexpand->format.buffer, \
							PX_STRINGFORMAT_STRING(PX_AbiGet_string(PX_Syntax_GetAbiLast(pSyntax), "expand_parameter[0].value")), \
							PX_STRINGFORMAT_STRING(PX_AbiGet_string(PX_Syntax_GetAbiLast(pSyntax), "expand_parameter[1].value")), \
							PX_STRINGFORMAT_STRING(PX_AbiGet_string(PX_Syntax_GetAbiLast(pSyntax), "expand_parameter[2].value")), \
							PX_STRINGFORMAT_STRING(PX_AbiGet_string(PX_Syntax_GetAbiLast(pSyntax), "expand_parameter[3].value")), \
							PX_STRINGFORMAT_STRING(PX_AbiGet_string(PX_Syntax_GetAbiLast(pSyntax), "expand_parameter[4].value")), \
							PX_STRINGFORMAT_STRING(PX_AbiGet_string(PX_Syntax_GetAbiLast(pSyntax), "expand_parameter[5].value")), \
							PX_STRINGFORMAT_STRING(PX_AbiGet_string(PX_Syntax_GetAbiLast(pSyntax), "expand_parameter[6].value")), \
							PX_STRINGFORMAT_STRING(PX_AbiGet_string(PX_Syntax_GetAbiLast(pSyntax), "expand_parameter[7].value"))\
						);
						PX_Syntax_PopAbi(pSyntax);
						break;
					default:
						PX_StringFree(&expand_source);
						PX_Syntax_Terminate(pSyntax, "Error:expand_parameters count too large");
						break;
					}
					
					if(!PX_Syntax_NewSyntaxExpandLexer(pSyntax, pexpand->token.buffer, expand_source.buffer, begin, end))
					{
						PX_StringFree(&expand_source);
						PX_Syntax_Terminate(pSyntax, "Error:expand lexer error");
						return PX_LEXER_LEXEME_TYPE_ERR;
					}
					PX_StringFree(&expand_source);
					break;
				}

			}
			if (i!=pSyntax->reg_expand_stack.size)
			{
				continue;
			}
		}

		return type;
	}

}

PX_LEXER_LEXEME_TYPE PX_Syntax_GetNextLexeme(PX_Syntax* pSyntax)
{
	return PX_Syntax_GetNextLexemeEx(pSyntax, PX_SYNTAX_LEXER_FILTER_NEWLINE| PX_SYNTAX_LEXER_FILTER_SPACER);
}

PX_LEXER_LEXEME_TYPE PX_Syntax_GetNextLexeme2(PX_Syntax* pSyntax)
{
	return PX_Syntax_GetNextLexemeEx(pSyntax, PX_SYNTAX_LEXER_FILTER_NEWLINE);
}

PX_LEXER_LEXEME_TYPE PX_Syntax_GetNextLexeme3(PX_Syntax* pSyntax)
{
	return PX_Syntax_GetNextLexemeEx(pSyntax, PX_SYNTAX_LEXER_FILTER_SPACER);
}

PX_LEXER_LEXEME_TYPE PX_Syntax_GetNextLexeme4(PX_Syntax* pSyntax)
{
	return PX_Syntax_GetNextLexemeEx(pSyntax, PX_SYNTAX_LEXER_FILTER_NONE);
}

px_bool PX_Syntax_SetCurrentLexer(PX_Syntax* pSyntax, px_int index,px_int offset)
{
	if (index < pSyntax->reg_lexer_stack.size)
	{
		pSyntax->reg_lexer_index = index;
		PX_LexerSetOffset(&PX_VECTORAT(PX_Syntax_Lexer, &pSyntax->reg_lexer_stack, index)->lexer, offset);
		return PX_TRUE;
	}
	PX_ASSERT();
	return PX_FALSE;
}

px_bool PX_Syntax_NewSyntaxExpandLexer(PX_Syntax* pSyntax, const px_char name[], const px_char source[], px_int begin, px_int end)
{
	PX_Syntax_Lexer SyntaxLexer = { 0 };
	PX_LexerInitialize(&SyntaxLexer.lexer, pSyntax->mp);
	SyntaxLexer.expand_lexer_index = pSyntax->reg_lexer_index;
	SyntaxLexer.expand_lexer_offset_begin = begin;
	SyntaxLexer.expand_lexer_offset_end = end;

	if (!PX_LexerSetName(&SyntaxLexer.lexer, name))
		return PX_FALSE;
	PX_LexerRegisterDelimiter(&SyntaxLexer.lexer, ',');
	PX_LexerRegisterDelimiter(&SyntaxLexer.lexer, '.');
	PX_LexerRegisterDelimiter(&SyntaxLexer.lexer, ';');
	PX_LexerRegisterDelimiter(&SyntaxLexer.lexer, '+');
	PX_LexerRegisterDelimiter(&SyntaxLexer.lexer, '-');
	PX_LexerRegisterDelimiter(&SyntaxLexer.lexer, '*');
	PX_LexerRegisterDelimiter(&SyntaxLexer.lexer, '/');
	PX_LexerRegisterDelimiter(&SyntaxLexer.lexer, '%');
	PX_LexerRegisterDelimiter(&SyntaxLexer.lexer, '&');
	PX_LexerRegisterDelimiter(&SyntaxLexer.lexer, '^');
	PX_LexerRegisterDelimiter(&SyntaxLexer.lexer, '~');
	PX_LexerRegisterDelimiter(&SyntaxLexer.lexer, '(');
	PX_LexerRegisterDelimiter(&SyntaxLexer.lexer, ')');
	PX_LexerRegisterDelimiter(&SyntaxLexer.lexer, '!');
	PX_LexerRegisterDelimiter(&SyntaxLexer.lexer, '=');
	PX_LexerRegisterDelimiter(&SyntaxLexer.lexer, '>');
	PX_LexerRegisterDelimiter(&SyntaxLexer.lexer, '<');
	PX_LexerRegisterDelimiter(&SyntaxLexer.lexer, '{');
	PX_LexerRegisterDelimiter(&SyntaxLexer.lexer, '}');
	PX_LexerRegisterDelimiter(&SyntaxLexer.lexer, '[');
	PX_LexerRegisterDelimiter(&SyntaxLexer.lexer, ']');
	PX_LexerRegisterDelimiter(&SyntaxLexer.lexer, '#');
	PX_LexerRegisterDelimiter(&SyntaxLexer.lexer, '\"');
	PX_LexerRegisterDiscard(&SyntaxLexer.lexer, "\\\n");

	PX_LexerRegisterSpacer(&SyntaxLexer.lexer, ' ');
	PX_LexerRegisterSpacer(&SyntaxLexer.lexer, '\t');
	PX_LexerRegisterComment(&SyntaxLexer.lexer, "//", "\n");
	PX_LexerRegisterComment(&SyntaxLexer.lexer, "/*", "*/");
	if (PX_LexerLoadSource(&SyntaxLexer.lexer, source))
	{
		if (!PX_VectorPushback(&pSyntax->reg_lexer_stack, &SyntaxLexer))
		{
			PX_LexerFree(&SyntaxLexer.lexer);
			PX_Syntax_Terminate(pSyntax, "out of memory");
			return PX_FALSE;
		}
		pSyntax->reg_lexer_index = pSyntax->reg_lexer_stack.size - 1;
		return PX_TRUE;
	}
	return PX_FALSE;
}
px_bool PX_Syntax_EntrySyntaxExpandLexer(PX_Syntax* pSyntax, px_int index)
{
	PX_Syntax_Lexer* pSyntaxLexer;
	PX_ASSERTIFX(index < 0 || index >= pSyntax->reg_lexer_stack.size, "Error:Syntax Entery Lexer Index Error");
	pSyntaxLexer = PX_VECTORAT(PX_Syntax_Lexer, &pSyntax->reg_lexer_stack, index);
	pSyntax->reg_lexer_index = index;
	PX_LexerReset(&pSyntaxLexer->lexer);
	return PX_TRUE;
}

px_lexer* PX_Syntax_GetCurrentLexer(PX_Syntax* pSyntax)
{
	PX_Syntax_Lexer* pCurrentSyntaxLexer;
	if (pSyntax->reg_lexer_index >=0&&pSyntax->reg_lexer_index < pSyntax->reg_lexer_stack.size)
		pCurrentSyntaxLexer = PX_VECTORAT(PX_Syntax_Lexer, &pSyntax->reg_lexer_stack, pSyntax->reg_lexer_index);
	else
	{
		return PX_NULL;
	}
	return &pCurrentSyntaxLexer->lexer;
}

const px_char* PX_Syntax_GetCurrentLexeme(PX_Syntax* pSyntax)
{
	px_lexer* plexer = PX_Syntax_GetCurrentLexer(pSyntax);
	if (plexer)
		return PX_LexerGetLexeme(plexer);
	else
		return "";
}

px_int PX_Syntax_GetCurrentLexerOffset(PX_Syntax* pSyntax)
{
	px_lexer* plexer = PX_Syntax_GetCurrentLexer(pSyntax);
	return PX_LexerGetOffset(plexer);
}

px_int PX_Syntax_GetCurrentLexerIndex(PX_Syntax* pSyntax)
{
	return pSyntax->reg_lexer_index;
}


static px_bool PX_Syntax_ExecuteNext(PX_Syntax* pSyntax, PX_Syntax_ast* pcurrent_ast)
{
	PX_Syntax_ast newast = { 0 };
	//push next
	if (!pcurrent_ast->pbnfnode||pcurrent_ast->pbnfnode->pnext == PX_NULL)
	{
		PX_Syntax_Message(pSyntax, "AST:next node is null,return true\n");
		if (PX_Syntax_ExecuteAstOpcode(pSyntax, "return true"))
			return PX_TRUE;
		PX_Syntax_Terminate(pSyntax, "out of memory");
		return PX_FALSE;
	}
	else
	{
		PX_Syntax_Message(pSyntax, "AST:next node ");
		PX_Syntax_Message(pSyntax, pcurrent_ast->pbnfnode->pnext->constant.buffer);
		PX_Syntax_Message(pSyntax, "\n");
		
		newast.pbnfnode = pcurrent_ast->pbnfnode->pnext;//try next
		newast.call_lexer_index = pSyntax->reg_lexer_index;
		newast.call_abistack_index = pSyntax->reg_abi_stack.size;
		newast.call_lexer_offset = PX_Syntax_GetCurrentLexerOffset(pSyntax);
	
		if (!PX_VectorPushback(&pSyntax->reg_ast_stack, &newast))
		{
			PX_Syntax_Terminate(pSyntax, "out of memory");
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
	if (!pbnfnode||pbnfnode->pothers==PX_NULL)
	{
		PX_Syntax_Message(pSyntax, "AST:others node is null,return false\n");

		if (PX_Syntax_ExecuteAstOpcode(pSyntax, "return false"))
			return PX_TRUE;
		
		PX_Syntax_Terminate(pSyntax,  "out of memory");
		return PX_FALSE;
	}
	else
	{
		PX_Syntax_Message(pSyntax, "AST:others node ");
		PX_Syntax_Message(pSyntax, pbnfnode->pothers->constant.buffer);
		PX_Syntax_Message(pSyntax, "\n");
		newast.call_lexer_index = pcurrent_ast->call_lexer_index;
		newast.call_lexer_offset = pcurrent_ast->call_lexer_offset;
		newast.pbnfnode = pbnfnode->pothers;
		newast.call_abistack_index = pSyntax->reg_abi_stack.size;
		if (!PX_VectorPushback(&pSyntax->reg_ast_stack, &newast))
		{
			PX_Syntax_Terminate(pSyntax,  "out of memory");
			return PX_FALSE;
		}
	}
	
	return PX_TRUE;
}

px_bool PX_Syntax_NewAst (PX_Syntax* pSyntax, const px_char mnemonic[])
{
	PX_Syntax_pebnf* ppebnf = PX_Syntax_GetPEBNF(pSyntax, mnemonic);
	PX_Syntax_ast newast = { 0 };
	if (!ppebnf)
	{
		PX_ASSERT();
		return PX_FALSE;
	}
	newast.call_lexer_index = PX_Syntax_GetCurrentLexerIndex(pSyntax);
	newast.call_lexer_offset = PX_Syntax_GetCurrentLexerOffset(pSyntax);
	newast.pbnfnode = ppebnf->pbnfnode;
	newast.call_abistack_index = pSyntax->reg_abi_stack.size;
	if (!PX_VectorPushback(&pSyntax->reg_ast_stack, &newast))
	{
		PX_Syntax_Terminate(pSyntax, "out of memory");
		return PX_FALSE;
	}
	return PX_TRUE;
}

px_bool PX_Syntax_NewExpand(PX_Syntax* pSyntax, const px_char token[], const px_char format[], px_int paramcount, px_int reg_lifetime)
{
	PX_Syntax_expand expand;
	if(!PX_StringInitialize(pSyntax->mp, &expand.token))
		return PX_FALSE;
	if (!PX_StringCat(&expand.token, token))
	{
		PX_StringFree(&expand.token);
		return PX_FALSE;
	}
	if (!PX_StringInitialize(pSyntax->mp, &expand.format))
	{
		PX_StringFree(&expand.token);
		return PX_FALSE;
	}
	if (!PX_StringCat(&expand.format, format))
	{
		PX_StringFree(&expand.token);
		PX_StringFree(&expand.format);
		return PX_FALSE;
	}
	expand.parameters_count = paramcount;

	if (!PX_VectorPushback(&pSyntax->reg_expand_stack, &expand))
	{
		PX_StringFree(&expand.token);
		PX_StringFree(&expand.format);
		return PX_FALSE;
	}
	return PX_TRUE;
	
}


px_bool PX_Syntax_CheckAbiName(px_abi* pabi, const px_char name[])
{
	if (pabi)
	{
		const px_char* pname = PX_AbiGet_string(pabi, "name");
		if (!pname)
		{
			return PX_FALSE;
		}
		return PX_strequ(pname, name);
	}
	return PX_FALSE;
}

px_bool PX_Syntax_CheckLastAbiName(PX_Syntax* pSyntax, const px_char name[])
{
	px_abi* pabi = PX_Syntax_GetAbiLast(pSyntax);
	if (pabi)
	{
		return PX_Syntax_CheckAbiName(pabi, name);
	}
	return PX_FALSE;

}
px_bool PX_Syntax_CheckSecondLastAbiName(PX_Syntax* pSyntax, const px_char name[])
{
	px_abi* pabi = PX_Syntax_GetAbiSecondLast(pSyntax);
	if (pabi)
	{
		return PX_Syntax_CheckAbiName(pabi, name);
	}
	return PX_FALSE;

}

px_int PX_Syntax_FindAbiIndexFromForward(PX_Syntax* pSyntax, const px_char name[])
{
	px_int i;
	px_abi* pabi;
	for (i =0 ; i < pSyntax->reg_abi_stack.size; i++)
	{
		pabi = PX_VECTORAT(px_abi, &pSyntax->reg_abi_stack, i);
		if (PX_Syntax_CheckAbiName(pabi, name))
		{
			return i;
		}
	}
	return -1;

}

px_int PX_Syntax_FindAbiIndexFromBackward(PX_Syntax* pSyntax, const px_char name[])
{
	px_int i;
	px_abi* pabi;
	for (i = pSyntax->reg_abi_stack.size - 1; i >= 0; i--)
	{
		pabi = PX_VECTORAT(px_abi, &pSyntax->reg_abi_stack, i);
		if (PX_Syntax_CheckAbiName(pabi, name))
		{
			return i;
		}
	}
	return -1;
	
}

px_abi * PX_Syntax_FindAbiFromBackward(PX_Syntax* pSyntax, const px_char name[])
{
	px_int index = PX_Syntax_FindAbiIndexFromBackward(pSyntax, name);
	if (index != -1)
	{
		return PX_VECTORAT(px_abi, &pSyntax->reg_abi_stack, index);
	}
	return PX_NULL;
}

px_abi* PX_Syntax_FindAbiFromForward(PX_Syntax* pSyntax, const px_char name[])
{
	px_int index = PX_Syntax_FindAbiIndexFromForward(pSyntax, name);
	if (index != -1)
	{
		return PX_VECTORAT(px_abi, &pSyntax->reg_abi_stack, index);
	}
	return PX_NULL;
}

const px_char* PX_Syntax_GetAbiValue(px_abi* pabi)
{
	const px_char *p=PX_AbiGet_string(pabi, "value");
	PX_ASSERTIFX(p, "Error:Abi value not found");
	return p;
}


static px_bool PX_Syntax_ExecuteAst(PX_Syntax* pSyntax)
{
	PX_Syntax_ast current_ast = { 0 };
	PX_Syntax_bnfnode* pbnfnode = PX_NULL;

	if (pSyntax->reg_ast_stack.size == 0)
	{
		//end of execute
		return PX_FALSE;
	}

	if (!PX_VectorLastTo(&pSyntax->reg_ast_stack, &current_ast))
	{
		return PX_FALSE;
	}

	pbnfnode = current_ast.pbnfnode;

	PX_Syntax_SetCurrentLexer(pSyntax, current_ast.call_lexer_index, current_ast.call_lexer_offset);
	
	switch (pbnfnode->type)
	{
	case PX_SYNTAX_AST_TYPE_CONSTANT:
	{
		const px_char* pstr = pbnfnode->constant.buffer;
		PX_LEXER_LEXEME_TYPE type = PX_Syntax_GetNextLexeme(pSyntax);
		const px_char* plexeme = PX_Syntax_GetCurrentLexeme(pSyntax);
		PX_VectorPop(&pSyntax->reg_ast_stack);
		if (PX_strequ(plexeme, pstr))
		{
			PX_Syntax_Message(pSyntax, "AST:const matched ");
			PX_Syntax_Message(pSyntax, current_ast.pbnfnode->constant.buffer);
			PX_Syntax_Message(pSyntax, "\n");
			if (pbnfnode->pfunction)
			{
				if (pbnfnode->pfunction(pSyntax, &current_ast))
				{
					PX_Syntax_Message(pSyntax, "AST:const function return true,ast next\n");
					return PX_Syntax_ExecuteNext(pSyntax, &current_ast);
				}
				else
				{
					PX_Syntax_Message(pSyntax, "AST:const function return false,try others\n");
				}
			}
			else
			{
				PX_Syntax_Message(pSyntax, "AST:const function is null,ast next\n");
				return PX_Syntax_ExecuteNext(pSyntax, &current_ast);
			}
		}
		return PX_Syntax_ExecuteOthers(pSyntax, &current_ast);
	}
	break;
	case PX_SYNTAX_AST_TYPE_FUNCTION:
	{
		PX_Syntax_Message(pSyntax, "AST:function matched ");
		PX_Syntax_Message(pSyntax, current_ast.pbnfnode->constant.buffer);
		PX_Syntax_Message(pSyntax, "\n");
		PX_VectorPop(&pSyntax->reg_ast_stack);
		if (pbnfnode->pfunction)
		{
			if (pbnfnode->pfunction(pSyntax, &current_ast))
			{
				PX_Syntax_Message(pSyntax, "AST:function return true,ast next\n");
				return PX_Syntax_ExecuteNext(pSyntax, &current_ast);
			}
			else
			{
				PX_Syntax_Message(pSyntax, "AST:function return false,try others\n");
			}
		}
		else
		{
			PX_Syntax_Message(pSyntax, "AST:function is null,ast next\n");
			return PX_Syntax_ExecuteNext(pSyntax, &current_ast);
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
			PX_ASSERTX("Error:linker not found");
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
		PX_Syntax_Message(pSyntax, "AST:linker matched ");
		PX_Syntax_Message(pSyntax, current_ast.pbnfnode->constant.buffer);
		PX_Syntax_Message(pSyntax, "-->");
		PX_Syntax_Message(pSyntax, psearchbnf->constant.buffer);
		PX_Syntax_Message(pSyntax, " abistack:");
		PX_Syntax_Message(pSyntax, PX_itos(pSyntax->reg_abi_stack.size,10).data);
		PX_Syntax_Message(pSyntax, "\n");
		newast.call_lexer_index = PX_Syntax_GetCurrentLexerIndex(pSyntax);
		newast.call_lexer_offset = PX_Syntax_GetCurrentLexerOffset(pSyntax);
		newast.pbnfnode = psearchbnf;
		newast.call_abistack_index = pSyntax->reg_abi_stack.size;
		if (!PX_VectorPushback(&pSyntax->reg_ast_stack, &newast))
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
	PX_ASSERTIFX(past->call_abistack_index > pSyntax->reg_abi_stack.size, "Stack Error");
	while (pSyntax->reg_abi_stack.size > past->call_abistack_index)
	{
		PX_Syntax_PopAbi(pSyntax);
	}
}



px_bool PX_Syntax_NewEntrySource(PX_Syntax* pSyntax, const px_char name[])
{
	PX_Syntax_Source* psource;
	psource = PX_Syntax_GetSource(pSyntax, name);
	if (!psource)
	{
		PX_StringCatFormat1(&pSyntax->message, "Error:source file %1 not found.", PX_STRINGFORMAT_STRING(name));
		return PX_FALSE;
	}
	if (!PX_Syntax_NewSyntaxExpandLexer(pSyntax, name, psource->source.buffer,-1,-1))
		return PX_FALSE;

	PX_Syntax_SetCurrentLexer(pSyntax, pSyntax->reg_lexer_stack.size - 1, 0);

	return PX_TRUE;
}

px_bool PX_Syntax_NewOperate3(PX_Syntax* pSyntax, const px_char type1[], const px_char type2[], const px_char type3[], const px_char opcode[], PX_Syntax_Operate_Function opfun)
{
	PX_Syntax_operate operate = { 0 };
	operate.oprand_count = (type1[0] != 0) + (type2[0] != 0) + (type3[0] != 0);
	PX_StringInitialize(pSyntax->mp, &operate.oprand1_type);
	PX_StringInitialize(pSyntax->mp, &operate.oprand2_type);
	PX_StringInitialize(pSyntax->mp, &operate.oprand3_type);
	if (!PX_StringCat(&operate.oprand1_type, type1))
	{
		PX_StringFree(&operate.oprand1_type);
		PX_StringFree(&operate.oprand2_type);
		PX_StringFree(&operate.oprand3_type);
		return PX_FALSE;
	}
	if (!PX_StringCat(&operate.oprand2_type, type2))
	{
		PX_StringFree(&operate.oprand1_type);
		PX_StringFree(&operate.oprand2_type);
		PX_StringFree(&operate.oprand3_type);
		return PX_FALSE;
	}
	if (!PX_StringCat(&operate.oprand3_type, type3))
	{
		PX_StringFree(&operate.oprand1_type);
		PX_StringFree(&operate.oprand2_type);
		PX_StringFree(&operate.oprand3_type);
		return PX_FALSE;
	}
	PX_strcpy(operate.opcode, opcode, sizeof(operate.opcode));
	operate.pfunction = opfun;
	if (!PX_VectorPushback(&pSyntax->reg_operate_stack, &operate))
	{
		return PX_FALSE;
	}
	PX_VectorReorder_MinToMax(&pSyntax->reg_operate_stack, PX_STRUCT_OFFSET(PX_Syntax_opcode, precedence), PX_QUICKSORT_REORDER_TYPE_U32);

	return PX_TRUE;
}

px_bool PX_Syntax_NewOperate2(PX_Syntax* pSyntax, const px_char type1[], const px_char type2[], const px_char opcode[], PX_Syntax_Operate_Function opfun)
{
	return PX_Syntax_NewOperate3(pSyntax, type1, type2, "", opcode, opfun);
}

px_bool PX_Syntax_NewOperate1(PX_Syntax* pSyntax, const px_char type1[], const px_char opcode[], PX_Syntax_Operate_Function opfun)
{
	return PX_Syntax_NewOperate3(pSyntax, type1, "", "", opcode, opfun);
}


px_bool PX_Syntax_NewOpcode(PX_Syntax* pSyntax, const px_char opcode[], PX_SYNTAX_OPCODE_TYPE type, px_dword precedence)
{
	PX_Syntax_opcode opcode1 = { 0 };
	opcode1.precedence = precedence;
	opcode1.type = type;
	PX_strcpy(opcode1.opcode, opcode, sizeof(opcode1.opcode));
	if (!PX_VectorPushback(&pSyntax->reg_opcode_stack, &opcode1))
	{
		return PX_FALSE;
	}
	return PX_TRUE;
}


px_bool PX_Syntax_ExecuteOperate(PX_Syntax* pSyntax)
{
	return PX_TRUE;
}


px_bool PX_Syntax_RemovePebnf(PX_Syntax* pSyntax, const px_char pebnf[])
{
	px_int i;
	for (i = 0; i < pSyntax->pebnf.size; i++)
	{
		PX_Syntax_pebnf* ppebnf = PX_VECTORAT(PX_Syntax_pebnf, &pSyntax->pebnf, i);
		if (PX_strequ(ppebnf->mnenonic.buffer, pebnf))
		{
			PX_StringFree(&ppebnf->mnenonic);
			PX_SyntaxFree_pebnfnode(pSyntax, ppebnf->pbnfnode);
			PX_VectorErase(&pSyntax->pebnf, i);
			return PX_TRUE;
		}
	}
	return PX_FALSE;
}
const px_char* PX_Syntax_AllocUnnamed(PX_Syntax* pSyntax)
{
	PX_strset(pSyntax->reg_unname, "_unnamed_");
	PX_strcat(pSyntax->reg_unname, PX_itos(pSyntax->reg_unname_index, 10).data);
	pSyntax->reg_unname_index++;
	return pSyntax->reg_unname;
}
px_bool PX_Syntax_Execute(PX_Syntax* pSyntax,  const px_char pebnf[])
{
	PX_Syntax_ast ast;
	PX_Syntax_bnfnode bnfnode = {0};

	px_int stack_mark_index = pSyntax->reg_ast_stack.size;
	
	PX_StringSetStatic(&bnfnode.constant, pebnf);
	bnfnode.type = PX_SYNTAX_AST_TYPE_LINKER;
	bnfnode.pfunction = PX_NULL;
	bnfnode.pnext = PX_NULL;
	bnfnode.pothers = PX_NULL;

	ast.call_lexer_index = pSyntax->reg_lexer_index;
	ast.call_lexer_offset = PX_Syntax_GetCurrentLexerOffset(pSyntax);
	ast.pbnfnode = &bnfnode;
	ast.call_abistack_index = pSyntax->reg_abi_stack.size;

	if (!PX_VectorPushback(&pSyntax->reg_ast_stack, &ast))
		return PX_FALSE;

	while (PX_TRUE)
	{
		if (!PX_Syntax_ExecuteAst(pSyntax))
		{
			break;
		}
		while (pSyntax->reg_astopcode_stack.size)
		{
			px_abi* pabi = PX_VECTORLAST(px_abi, &pSyntax->reg_astopcode_stack);
			const px_char* ptype = PX_AbiGet_string(pabi, "type");
			if (ptype)
			{
				if (PX_strequ(ptype, "terminate"))
				{
					PX_Syntax_Message(pSyntax, "Error:compiler terminated.\n");
					PX_StringCat(&pSyntax->message, "Error:compiler terminated.");
					return PX_FALSE;
				}
				else if (PX_strequ(ptype, "pop"))
				{
					PX_Syntax_Message(pSyntax, "AST:pop\n");
					PX_VectorPop(&pSyntax->reg_ast_stack);
					PX_AbiFree(pabi);
					PX_VectorPop(&pSyntax->reg_astopcode_stack);
				}
				else if (PX_strequ(ptype, "continue"))
				{
					PX_Syntax_Message(pSyntax, "AST:continue\n");
				}
				else if (PX_strequ(ptype,"return true")|| PX_strequ(ptype, "return false"))
				{
					//return node
					PX_Syntax_ast current_ast = { 0 };

					if (pSyntax->reg_ast_stack.size == stack_mark_index)
					{
						if (PX_strequ(ptype, "return true"))
						{
							PX_AbiFree(pabi);
							PX_VectorPop(&pSyntax->reg_astopcode_stack);
							return PX_TRUE;
						}
						else
						{
							PX_AbiFree(pabi);
							PX_VectorPop(&pSyntax->reg_astopcode_stack);
							return PX_FALSE;
						}
					}
					PX_VectorPopTo(&pSyntax->reg_ast_stack, &current_ast);
					if (current_ast.pbnfnode == PX_NULL || (current_ast.pbnfnode->type != PX_SYNTAX_AST_TYPE_LINKER && current_ast.pbnfnode->type != PX_SYNTAX_AST_TYPE_RECURSION))
					{
						PX_ASSERT();
						return PX_FALSE;
					}
					if (PX_strequ(ptype, "return true"))
					{
						PX_Syntax_Message(pSyntax, "AST:");
						PX_Syntax_Message(pSyntax, current_ast.pbnfnode->constant.buffer);
						PX_Syntax_Message(pSyntax, " return true\n");
						PX_AbiFree(pabi);
						PX_VectorPop(&pSyntax->reg_astopcode_stack);
						if (current_ast.pbnfnode->pfunction)
						{
							if (current_ast.pbnfnode->pfunction(pSyntax, &current_ast))
							{
								PX_Syntax_Message(pSyntax, "AST:");
								PX_Syntax_Message(pSyntax, current_ast.pbnfnode->constant.buffer);
								if(current_ast.pbnfnode->type== PX_SYNTAX_AST_TYPE_LINKER)
									PX_Syntax_Message(pSyntax, " linker function return true,ast next\n");
								else
									PX_Syntax_Message(pSyntax, " recursion function return true,ast next\n");
								if (!PX_Syntax_ExecuteNext(pSyntax, &current_ast))
									return PX_FALSE;
							}
							else
							{
								PX_Syntax_Message(pSyntax, "AST:");
								PX_Syntax_Message(pSyntax, current_ast.pbnfnode->constant.buffer);
								if (current_ast.pbnfnode->type == PX_SYNTAX_AST_TYPE_LINKER)
									PX_Syntax_Message(pSyntax, " linker function return false,try others\n");
								else
									PX_Syntax_Message(pSyntax, " recursion function return false,try others\n");
								PX_Syntax_AstClearAbiStack(pSyntax, &current_ast);
								if (!PX_Syntax_ExecuteOthers(pSyntax, &current_ast))
								{
									return PX_FALSE;
								}
							}
						}
						else
						{
							
							PX_Syntax_Message(pSyntax, "AST:");
							PX_Syntax_Message(pSyntax, current_ast.pbnfnode->constant.buffer);
							if (current_ast.pbnfnode->type == PX_SYNTAX_AST_TYPE_LINKER)
								PX_Syntax_Message(pSyntax, " linker function is null,ast next\n");			
							else
								PX_Syntax_Message(pSyntax, " recursion function is null,ast next\n");

							if (!PX_Syntax_ExecuteNext(pSyntax, &current_ast))
								return PX_FALSE;
						}
					}
					else
					{
						//false
						PX_AbiFree(pabi);
						PX_VectorPop(&pSyntax->reg_astopcode_stack);
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

px_bool PX_Syntax_LoadBaseType(PX_Syntax* pSyntax)
{
	if (!PX_Syntax_NewTypeDefine(pSyntax, "u8", "base", 1, 0)) goto _ERROR;
	if (!PX_Syntax_NewTypeDefine(pSyntax, "u16", "base", 1, 0)) goto _ERROR;
	if (!PX_Syntax_NewTypeDefine(pSyntax, "u32", "base", 1, 0)) goto _ERROR;
	if (!PX_Syntax_NewTypeDefine(pSyntax, "u64", "base", 1, 0)) goto _ERROR;

	if (!PX_Syntax_NewTypeDefine(pSyntax, "i8", "base", 1, 0)) goto _ERROR;
	if (!PX_Syntax_NewTypeDefine(pSyntax, "i16", "base", 1, 0)) goto _ERROR;
	if (!PX_Syntax_NewTypeDefine(pSyntax, "i32", "base", 1, 0)) goto _ERROR;
	if (!PX_Syntax_NewTypeDefine(pSyntax, "i64", "base", 1, 0)) goto _ERROR;

	if (!PX_Syntax_NewTypeDefine(pSyntax, "f8", "base", 1, 0)) goto _ERROR;
	if (!PX_Syntax_NewTypeDefine(pSyntax, "f16", "base", 1, 0)) goto _ERROR;
	if (!PX_Syntax_NewTypeDefine(pSyntax, "f32", "base", 1, 0)) goto _ERROR;
	if (!PX_Syntax_NewTypeDefine(pSyntax, "f64", "base", 1, 0)) goto _ERROR;

	if (!PX_Syntax_NewTypeDefine(pSyntax, "bool", "base", 1, 0)) goto _ERROR;
	if (!PX_Syntax_NewTypeDefine(pSyntax, "void", "base", 1, 0)) goto _ERROR;
	if (!PX_Syntax_NewTypeDefine(pSyntax, "int", "base", 1, 0)) goto _ERROR;
	if (!PX_Syntax_NewTypeDefine(pSyntax, "float", "base", 1, 0)) goto _ERROR;
	if (!PX_Syntax_NewTypeDefine(pSyntax, "string", "base", 1, 0)) goto _ERROR;
	if (!PX_Syntax_NewTypeDefine(pSyntax, "memory", "base", 1, 0)) goto _ERROR;
	return PX_TRUE;
_ERROR:
	return PX_FALSE;
}

px_bool PX_Syntax_ExecuteTestBench(PX_Syntax* pSyntax, const px_char pebnf_expr[])
{
	px_char expr[128] = { 0 };

	PX_strcat(expr, "testbench =");
	PX_strcat(expr, pebnf_expr);
	if (PX_Syntax_Parse_PEBNF(pSyntax, expr, 0))
	{
		px_bool ret= PX_Syntax_Execute(pSyntax, "testbench");
		PX_Syntax_RemovePebnf(pSyntax,"testbench");
		return ret;

	}
	else
		return PX_FALSE;
}

px_bool PX_Syntax_ExecuteSource(PX_Syntax* pSyntax, const px_char name[], const px_char pebnf[])
{
	pSyntax->reg_lexer_index = -1;
	if (!PX_Syntax_LoadBaseType(pSyntax))
	{
		return PX_FALSE;
	}

	if (!PX_Syntax_EnterScope(pSyntax))//scope 1--->global scope
	{
		PX_StringCat(&pSyntax->message, "Error:enter scope failed.");
		return PX_FALSE;
	}
	if (!PX_Syntax_NewEntrySource(pSyntax, name))
	{
		return PX_FALSE;
	}
	if (!PX_Syntax_Execute(pSyntax, pebnf))
	{
		return PX_FALSE;
	}
	if (!PX_Syntax_LeaveScope(pSyntax))
	{
		PX_StringCat(&pSyntax->message, "Error:leave scope failed.");
		return PX_FALSE;
	}
	return PX_TRUE;
}

px_void PX_Syntax_Terminate(PX_Syntax* pSyntax, const px_char message[])
{
	px_char build_content[1024] = { 0 };
	//numeric too long
	PX_sprintf4(build_content,sizeof(build_content), "%1:%2:%3 %4\n", \
		PX_STRINGFORMAT_STRING(PX_LexerGetName(PX_Syntax_GetCurrentLexer(pSyntax))), \
		PX_STRINGFORMAT_INT(PX_LexerGetCurrentLine(PX_Syntax_GetCurrentLexer(pSyntax))), \
		PX_STRINGFORMAT_INT(PX_LexerGetCurrentColumn(PX_Syntax_GetCurrentLexer(pSyntax))),\
		PX_STRINGFORMAT_STRING(message));
	PX_Syntax_Message(pSyntax, build_content);
	//ternimate
	PX_Syntax_ExecuteAstOpcode(pSyntax, "terminate");
}

px_void PX_Syntax_AstReturn(PX_Syntax* pSyntax,PX_Syntax_ast *past,const px_char message[])
{
	past->pbnfnode = &pSyntax->bnfnode_return;
	if(!PX_StringSet(&past->pbnfnode->constant, message))
	{
		PX_Syntax_Terminate(pSyntax, "out of memory");
	}
}

px_void PX_Syntax_Message(PX_Syntax* pSyntax, const px_char message[])
{
	//printf("%s", message);
	if (!PX_StringCat(&pSyntax->ast_message, message))
	{
		PX_ASSERT();
		return;
	}
}
px_char PX_Syntax_GetNextChar(PX_Syntax* pSyntax)
{
	while (PX_TRUE)
	{
		px_char ch = PX_LexerGetNextChar(PX_Syntax_GetCurrentLexer(pSyntax));
		if (ch=='\\'&&PX_LexerPreviewNextChar(PX_Syntax_GetCurrentLexer(pSyntax))=='\n')
		{
			PX_LexerGetNextChar(PX_Syntax_GetCurrentLexer(pSyntax));
			continue;
		}
		else if (ch == '\0')
		{
			PX_Syntax_Lexer* pcurrentSyntaxLexer = PX_VECTORAT(PX_Syntax_Lexer, &pSyntax->reg_lexer_stack, pSyntax->reg_lexer_index);
			if (pcurrentSyntaxLexer->expand_lexer_index != -1 && pcurrentSyntaxLexer->expand_lexer_offset_end != -1)
			{
				PX_Syntax_SetCurrentLexer(pSyntax, pcurrentSyntaxLexer->expand_lexer_index, pcurrentSyntaxLexer->expand_lexer_offset_end);
				continue;
			}
			return 0;
		}
		return ch;
	}
	 
}

px_char PX_Syntax_PreviewNextChar(PX_Syntax* pSyntax)
{
	px_lexer* pcurrentlexer = PX_Syntax_GetCurrentLexer(pSyntax);
	PX_ASSERTIF(pcurrentlexer == PX_NULL);
	return PX_LexerPreviewNextChar(pcurrentlexer);
}



px_int  PX_Syntax_GetCurrentLexemeBegin(PX_Syntax* pSyntax)
{
	px_lexer* plexer = PX_Syntax_GetCurrentLexer(pSyntax);
	if (plexer)
		return PX_LexerGetLexemeBegin(plexer);
	else
		return -1;
}

px_int  PX_Syntax_GetCurrentLexemeEnd(PX_Syntax* pSyntax)
{
	px_lexer* plexer = PX_Syntax_GetCurrentLexer(pSyntax);
	if (plexer)
		return  PX_LexerGetLexemeEnd(plexer);
	else
		return -1;
}