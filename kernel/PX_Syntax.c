#include "PX_Syntax.h"
#include "PX_Syntax_base_type.h"

px_bool PX_Syntax_IsValidToken(const px_char token[])
{
	if (token[0] == '\0' || PX_charIsNumeric(token[0]))
	{
		return PX_FALSE;
	}

	while (*token)
	{
		if ((*token >= 'A' && *token <= 'Z') || (*token >= 'a' && *token <= 'z') || *token == '_' || *token == ':' || PX_charIsNumeric(*token)|| ((*token)&0x80))
		{
			token++;
			continue;
		}
		return PX_FALSE;
	}
	return PX_TRUE;
}

px_bool PX_Syntax_IsEndOfSource(PX_Syntax* pSyntax)
{
	return PX_SyntaxLexer_IsEnd(&pSyntax->reg_syntaxlexer);
}

px_int PX_Syntax_GetMaxLineCharWidthCount(PX_Syntax* pSyntax, px_int source_index)
{
	PX_SyntaxLexer_Source* psource = PX_Syntax_GetSourceByIndex(pSyntax, source_index);
	if (psource)
	{
		return psource->max_line_char_width;
	}
	return 0;
}

px_int PX_Syntax_GetLineCount(PX_Syntax* pSyntax, px_int source_index)
{
	PX_SyntaxLexer_Source* psource = PX_Syntax_GetSourceByIndex(pSyntax, source_index);
	if (psource)
	{
		return psource->line_count;
	}
	return 0;

}
PX_Syntax_pebnf* PX_Syntax_GetPebnfByIndex(PX_Syntax* pSyntax, px_int index)
{
	if (PX_VectorCheckIndex(&pSyntax->pebnf, index))
	{
		return PX_VECTORAT(PX_Syntax_pebnf, &pSyntax->pebnf, index);
	}
	return PX_NULL;
}

px_int PX_Syntax_GetPebnfIndexByMnemonic(PX_Syntax* pSyntax, const px_char mnemonic[])
{
	px_int i;
	PX_Syntax_pebnf* ptype;
	for (i = 0; i < pSyntax->pebnf.size; i++)
	{
		ptype = PX_VECTORAT(PX_Syntax_pebnf, &pSyntax->pebnf, i);
		if (PX_strequ(ptype->mnenonic.buffer, mnemonic))
		{
			return i;
		}
	}
	return -1;
}

PX_Syntax_pebnf* PX_Syntax_GetPebnf(PX_Syntax* pSyntax, const px_char mnemonic[])
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
	PX_Syntax_pebnf* ppebnf = PX_Syntax_GetPebnf(pSyntax, mnemonic);
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
	PX_Syntax_pebnf* ppebnf = PX_Syntax_GetPebnf(pSyntax, mnemonic);
	if (!ppebnf)
	{
		px_memorypool* mp = pSyntax->mp;
		PX_Syntax_pebnf pebnf = {0};

		if (!PX_StringInitialize(mp, &pebnf.mnenonic))
			return PX_NULL;
		if (!PX_StringAppend(&pebnf.mnenonic, mnemonic))
		{
			PX_StringFree(&pebnf.mnenonic);
			return PX_NULL;
		}
			
		PX_VectorPushback(&pSyntax->pebnf, &pebnf);
		return PX_VECTORLAST(PX_Syntax_pebnf, &pSyntax->pebnf);
	}
	return ppebnf;

}

px_bool PX_Syntax_Parse_NextPENNF(PX_Syntax* pSyntax, PX_Syntax_pebnf* ppebnf, PX_Syntax_bnfnode* pprevious, px_lexer* plexer, PX_Syntax_Function penterfunction, PX_Syntax_Function pleavefunction,px_void *userptr)
{
	PX_LEXER_LEXEME_TYPE type;
	while (PX_TRUE)
	{
		type = PX_LexerGetNextLexeme(plexer);
		if (type == PX_LEXER_LEXEME_TYPE_COMMENT)
			continue;
		if (type == PX_LEXER_LEXEME_TYPE_SPACER) 
			continue;
		break;
	}

	switch (type)
	{
	case PX_LEXER_LEXEME_TYPE_CONATINER:
	case PX_LEXER_LEXEME_TYPE_TOKEN:
	case PX_LEXER_LEXEME_TYPE_DELIMITER:
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
		else if (type == PX_LEXER_LEXEME_TYPE_DELIMITER)
		{
			if (PX_LexerGetSymbol(plexer)=='&')
			{
				ast_type = PX_SYNTAX_AST_TYPE_CONTINUOUS;
				pstr = "&";
			}
			else
			{
				return PX_FALSE;
			}
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
					return PX_FALSE;
				pnewast->type = ast_type;
				if (ast_type== PX_SYNTAX_AST_TYPE_RECURSION)
				{
					if (!PX_StringAppend(&pnewast->constant, ppebnf->mnenonic.buffer))
						return PX_FALSE;
				}
				else
				{
					if (!PX_StringAppend(&pnewast->constant, pstr))
						return PX_FALSE;
				}
				
				pnewast->penterfunction = PX_NULL;
				pnewast->pleavefunction = PX_NULL;
				pnewast->pothers = PX_NULL;
				pnewast->pnext = PX_NULL;
				pnewast->userptr = PX_NULL;
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
			return PX_Syntax_Parse_NextPENNF(pSyntax, ppebnf, pnewast, plexer, penterfunction, pleavefunction,userptr);
		}
		else
		{
			return PX_Syntax_Parse_NextPENNF(pSyntax, ppebnf, psearchast, plexer, penterfunction, pleavefunction, userptr);
		}
	}
	break;
	case PX_LEXER_LEXEME_TYPE_END:
	{
		pprevious->pnext = PX_NULL;
		if(!pprevious->penterfunction)
			pprevious->penterfunction = penterfunction;
		else if (pprevious->penterfunction != penterfunction)
			PX_ASSERTX("Error:PEBNF Error, enter function already exist");

		if (!pprevious->pleavefunction)
			pprevious->pleavefunction = pleavefunction;
		else if(pprevious->pleavefunction!= pleavefunction)
			PX_ASSERTX("Error:PEBNF Error, leave function already exist");

		if (!pprevious->userptr)
			 pprevious->userptr = userptr;
		else if(pprevious->userptr != userptr)
			PX_ASSERTX("Error:PEBNF Error, userptr already exist");
		return PX_TRUE;
	}
	break;
	default:
		PX_ASSERTX( "Error:PEBNF Error");
		return PX_FALSE;
	}
}

px_bool PX_Syntax_Parse_PEBNF(PX_Syntax* pSyntax, const px_char PEBNF[], PX_Syntax_Function penterfunction, PX_Syntax_Function pleavefunction,px_void *userptr)
{
	px_lexer lexer;
	PX_LEXER_LEXEME_TYPE type;
	PX_LexerInitialize(&lexer, pSyntax->mp);
	PX_LexerRegisterSpacer(&lexer, ' ');
	PX_LexerRegisterContainer(&lexer, "'", "'");
	PX_LexerRegisterDelimiter(&lexer, '=');
	PX_LexerRegisterDelimiter(&lexer, '&');
	
	PX_LexerLoadSourceWithPresort(&lexer, PEBNF);
	while (PX_TRUE)
	{
		type = PX_LexerGetNextLexeme(&lexer);
		if (type == PX_LEXER_LEXEME_TYPE_COMMENT)
			continue;
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
			if (type == PX_LEXER_LEXEME_TYPE_COMMENT)
				continue;
			if (type == PX_LEXER_LEXEME_TYPE_SPACER)
				continue;
			break;
		}
		if (type==PX_LEXER_LEXEME_TYPE_END)
		{
			ppebnf->penterfunction = penterfunction;
			ppebnf->pleavefunction = pleavefunction;
			ppebnf->userptr = userptr;
			PX_LexerFree(&lexer);
			return PX_TRUE;
		}

		PX_ASSERTIFX(type != PX_LEXER_LEXEME_TYPE_DELIMITER, "Error:PEBNF Error");
		PX_ASSERTIFX(!PX_strequ(PX_LexerGetLexeme(&lexer), "="), "Error: = expected but not found");
		if (ppebnf)
		{
			if (PX_Syntax_Parse_NextPENNF(pSyntax, ppebnf, 0, &lexer, penterfunction,pleavefunction,userptr))
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
	PX_ASSERTX("Error:PEBNF Error");
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
	if (!PX_VectorPushback(&pSyntax->reg_ast_instr_stack, &newabi))
	{
		PX_AbiFree(&newabi);
		return PX_FALSE;
	}
	return PX_TRUE;
}


px_int PX_Syntax_NewOpcodeDefine(PX_Syntax* pSyntax, const px_char opcode[], PX_SYNTAX_OPCODE_TYPE type, px_dword precedence)
{
	px_int i;
	PX_Syntax_opcode new_opcode;
	for (i = 0; i < pSyntax->reg_expr_opcode_stack.size; i++)
	{
		PX_Syntax_opcode* popcode = PX_VECTORAT(PX_Syntax_opcode, &pSyntax->reg_expr_opcode_stack, i);
		if (PX_strequ(popcode->opcode, opcode) && popcode->type == type)
		{
			return i;
		}

	}
	PX_memset(&new_opcode, 0, sizeof(new_opcode));
	new_opcode.type = type;
	new_opcode.precedence = precedence;
	PX_strcpy(new_opcode.opcode, opcode, sizeof(new_opcode.opcode));
	if (!PX_VectorPushback(&pSyntax->reg_expr_opcode_stack, &new_opcode))
	{
		PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_NewOpcodeAbi out of memory");
		return -1;
	}
	return pSyntax->reg_expr_opcode_stack.size - 1;
}



px_int PX_Syntax_GetOpcodeDefineIndex(PX_Syntax* pSyntax, const px_char opcode[], PX_SYNTAX_OPCODE_TYPE type)
{
	px_int i;
	for (i = pSyntax->reg_expr_opcode_stack.size - 1; i >= 0; i--)
	{
		PX_Syntax_opcode* popcode = PX_VECTORAT(PX_Syntax_opcode, &pSyntax->reg_expr_opcode_stack, i);
		if (PX_strequ(popcode->opcode, opcode) && popcode->type == type)
		{
			return i;
		}
	}
	return -1;
}

PX_Syntax_opcode* PX_Syntax_GetOpcodeDefine(PX_Syntax* pSyntax, px_int index)
{
	if (!PX_VectorCheckIndex(&pSyntax->reg_expr_opcode_stack, index))
		return PX_NULL;

	return PX_VECTORAT(PX_Syntax_opcode, &pSyntax->reg_expr_opcode_stack, index);
}

px_int PX_Syntax_GetAbiCount(PX_Syntax* pSyntax)
{
	return pSyntax->reg_abi_stack.size;
}

px_bool PX_Syntax_TypeMatch(const px_char source_type[], const px_char target_type[])
{
	//eg ix match ix,ix.i.8 match ix, ix.i.8 match ix.i.8, but ix match ix.i.8 is not match
	px_int i;
	for (i = 0; source_type[i] && target_type[i]; i++)
	{
		if (source_type[i] != target_type[i])
		{
			return PX_FALSE;
		}
	}
	if (target_type[i] == '\0' || target_type[i] == '.')
		return PX_TRUE;
	return PX_FALSE;
}

px_bool PX_Syntax_TypeMatch2(const px_char source_type[], const px_char target_type[], const px_char target_type2[])
{
	return PX_Syntax_TypeMatch(source_type, target_type) || PX_Syntax_TypeMatch(source_type, target_type2);
}

px_bool PX_Syntax_TypeMatch3(const px_char source_type[], const px_char target_type[], const px_char target_type2[], const px_char target_type3[])
{
	return PX_Syntax_TypeMatch(source_type, target_type) || PX_Syntax_TypeMatch(source_type, target_type2) || PX_Syntax_TypeMatch(source_type, target_type3);
}

px_bool PX_Syntax_NewTypeDefine(PX_Syntax* pSyntax, const px_char type[],const px_char mnemonic[],  const  px_int size, px_int reg_lifetime)
{
	px_abi* pscope=PX_NULL;
	px_int i, typelen = PX_strlen(type);
	px_char payload[256] = { 0 };
	//find scope abi
	for (i = pSyntax->reg_abi_stack.size - 1; i >= 0; i--)
	{
		px_abi* pabi = PX_VECTORAT(px_abi, &pSyntax->reg_abi_stack, i);
		if (PX_Syntax_CheckAbiName(pabi, "scope"))
		{
			px_int lifetime = PX_AbiGetValue_int(pabi, "lifetime");
			if (reg_lifetime>=lifetime)
			{
				pscope = pabi;
				break;
			}
		}
	}
	if (!pscope)
	{
		PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_NewTypeDefine No Scope Found");
		return PX_FALSE;
	}

	PX_sprintf1(payload, sizeof(payload), "type_defines.%1",PX_STRINGFORMAT_STRING(type));
	if (PX_AbiExist_abi(pscope, payload))
	{
		PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_NewTypeDefine Type Already Defined");
		return PX_FALSE;
	}

	if (!PX_AbiSet_Abi(pscope, payload, PX_NULL))
	{
		PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_NewTypeDefine Memory Error 0");
		return PX_FALSE;
	}

	PX_sprintf1(payload, sizeof(payload), "type_defines.%1.size", PX_STRINGFORMAT_STRING(type));
	if (!PX_AbiSet_int(pscope, payload, size))
	{
		PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_NewTypeDefine Memory Error 2");
		return PX_FALSE;
	}

	//build type_mnemonic
	PX_sprintf1(payload, sizeof(payload), "type_mnemonics.%1", PX_STRINGFORMAT_STRING(mnemonic));

	if (!PX_AbiSet_string(pscope, payload, type))
	{
		PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_NewTypeDefine Memory Error 3");
		return PX_FALSE;
	}
	return PX_TRUE;

}

px_bool PX_Syntax_GetTypeDefineByType(PX_Syntax* pSyntax, const px_char type[],px_abi *ptype_define_abi)
{
	px_abi* pabi;
	px_int i;
	for (i = pSyntax->reg_abi_stack.size - 1; i >= 0; i--)
	{
		pabi = PX_VECTORAT(px_abi, &pSyntax->reg_abi_stack, i);
		if (PX_Syntax_CheckAbiName(pabi, "scope"))
		{
			px_char payload[256] = { 0 };
			PX_sprintf1(payload, sizeof(payload), "type_defines.%1", PX_STRINGFORMAT_STRING(type));
			if (PX_AbiExist_abi(pabi, payload))
			{
				*ptype_define_abi=PX_AbiGetValue_abireadonly(pabi, payload);
				return PX_TRUE;
			}
		}
	}
	return PX_FALSE;
}

px_int PX_Syntax_GetTypeDefineTypeSize(PX_Syntax* pSyntax, const px_char type[])
{
	px_abi type_define_abi;
	if (PX_Syntax_GetTypeDefineByType(pSyntax, type, &type_define_abi))
	{
		if (PX_AbiExist_Type(&type_define_abi, "size", PX_ABI_TYPE_INT))
		{
			return PX_AbiGetValue_int(&type_define_abi, "size");
		}
		else
		{
			return 0;
		}
	}
	else
	{
		return 0;
	}
}

const px_char* PX_Syntax_FindTypeDefineTypeByMnemonic(PX_Syntax* pSyntax, const px_char mnemonic[])
{
	px_abi* pabi;
	px_int i;
	for (i = pSyntax->reg_abi_stack.size - 1; i >= 0; i--)
	{
		pabi = PX_VECTORAT(px_abi, &pSyntax->reg_abi_stack, i);
		if (PX_Syntax_CheckAbiName(pabi, "scope"))
		{
			px_char payload[256] = { 0 };
			PX_sprintf1(payload, sizeof(payload), "type_mnemonics.%1", PX_STRINGFORMAT_STRING(mnemonic));
			if (PX_AbiExist_Type(pabi, payload, PX_ABI_TYPE_STRING))
			{
				return PX_AbiGet_string(pabi, payload);
			}
		}
	}
	return PX_NULL;
}

px_bool PX_Syntax_GetTypeDefineByMnemonic(PX_Syntax* pSyntax, const px_char mnemonic[], px_abi* ptype_define_abi)
{
	px_abi* pabi;
	px_int i;
	for (i = pSyntax->reg_abi_stack.size - 1; i >= 0; i--)
	{
		pabi = PX_VECTORAT(px_abi, &pSyntax->reg_abi_stack, i);
		if (PX_Syntax_CheckAbiName(pabi, "scope"))
		{
			px_char payload[256] = { 0 };
			PX_sprintf1(payload, sizeof(payload), "type_mnemonics.%1", PX_STRINGFORMAT_STRING(mnemonic));
			if (PX_AbiExist_Type(pabi, payload,PX_ABI_TYPE_STRING))
			{
				const px_char* ptype_str = PX_AbiGet_string(pabi, payload);
				if (!ptype_str)
				{
					PX_ASSERTX("Error: type_mnemonics exist but not found");
					continue;
				}

				//rebuild payload to find type define abi
				PX_sprintf1(payload, sizeof(payload), "type_defines.%1", PX_STRINGFORMAT_STRING(ptype_str));
				//find abi
				if (PX_AbiExist_abi(pabi, payload))
				{
					*ptype_define_abi = PX_AbiGetValue_abireadonly(pabi, payload);
					return PX_TRUE;
				}
				else
				{
					PX_ASSERTX("Error: type_mnemonics exist but type define abi not found");
					return PX_FALSE;
				}
			}
		}
	}
	return PX_FALSE;
}

px_bool PX_Syntax_NewTypedef(PX_Syntax* pSyntax, const px_char type[], const px_char mnemonic[])
{
	px_int i;
	for (i = pSyntax->reg_abi_stack.size - 1; i >= 0; i--)
	{
		px_abi* pabi = PX_VECTORAT(px_abi, &pSyntax->reg_abi_stack, i);
		if (PX_Syntax_CheckAbiName(pabi, "scope"))
		{
			//find type define abi
			px_char payload[256] = { 0 };
			PX_sprintf1(payload, sizeof(payload), "type_defines.%1", PX_STRINGFORMAT_STRING(type));
			if (PX_AbiExist_abi(pabi, payload))
			{
				//new mnemonic
				PX_sprintf1(payload, sizeof(payload), "type_mnemonics.%1", PX_STRINGFORMAT_STRING(mnemonic));
				if (PX_AbiExist_Type(pabi,payload,PX_ABI_TYPE_STRING))
				{
					PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_NewTypedef Type Mnemonic Already Exist");
					return PX_FALSE;
				}
				return PX_AbiSet_string(pabi, payload, type);
			}
		}
	}
	PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_NewTypedef Type Not Found");
	return PX_FALSE;
}

px_abi* PX_Syntax_GetLastTypeDefineScopeAbi(PX_Syntax* pSyntax)
{
	px_abi* pabi;
	px_int i;
	for (i = pSyntax->reg_abi_stack.size - 1; i >= 0; i--)
	{
		pabi = PX_VECTORAT(px_abi, &pSyntax->reg_abi_stack, i);
		if (PX_Syntax_CheckAbiName(pabi, "scope"))
		{
			return pabi;
		}
	}
	return PX_NULL;
}

px_int PX_Syntax_GetTypeDefineScopeAbiIndex(PX_Syntax* pSyntax, const px_char type[])
{
	px_abi* pabi;
	px_int i;
	for (i = pSyntax->reg_abi_stack.size-1; i >= 0; i--)
	{
		pabi = PX_VECTORAT(px_abi, &pSyntax->reg_abi_stack, i);
		if (PX_Syntax_CheckAbiName(pabi, "scope"))
		{
			px_abi rabi;
			if (PX_AbiGet_AbiReadOnly(pabi,&rabi,"type_defines"))
			{
				if (PX_AbiExist_abi(&rabi,type))
				{
					return i;
				}
			}
		}
	}
	return -1;
}

px_abi PX_Syntax_GetTypeDefineAbiReadOnly(PX_Syntax* pSyntax, const px_char type[])
{
	px_abi* pabi;
	px_int i;
	px_abi rabi = { 0 };
	for (i = pSyntax->reg_abi_stack.size - 1; i >= 0; i--)
	{
		pabi = PX_VECTORAT(px_abi, &pSyntax->reg_abi_stack, i);
		if (PX_Syntax_CheckAbiName(pabi, "scope"))
		{
			
			if (PX_AbiGet_AbiReadOnly(pabi, &rabi, "type_defines"))
			{
				if (PX_AbiExist_abi(&rabi, type))
				{
					return PX_AbiGetValue_abireadonly(&rabi, type);
				}
			}
		}
	}
	return rabi;
}

px_int PX_Syntax_GetTypeSize(PX_Syntax* pSyntax, const px_char type[])
{
	px_abi rabi = PX_Syntax_GetTypeDefineAbiReadOnly(pSyntax, type);
	if (PX_AbiGet_Size(&rabi))
	{
		return PX_AbiGetValue_int(&rabi, "size");
	}
	return 0;
}

px_bool PX_Syntax_EnterScope(PX_Syntax* pSyntax)
{
	px_abi* pnewabi;
	pSyntax->reg_lifetime++;
	if (PX_NULL==( pnewabi=PX_Syntax_NewAbi(pSyntax, "scope", pSyntax->reg_lifetime)))
	{
		PX_ASSERTX("PX_Syntax_EnterScope memory error1");
		return PX_FALSE;
	}
	if (!PX_AbiSet_int(pnewabi, "alloc", 0))
	{
		PX_AbiFree(pnewabi);
		PX_ASSERTX("PX_Syntax_EnterScope memory error2");
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
		if(!plastabi) break;
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

px_abi* PX_Syntax_GetLastScopeAbi(PX_Syntax* pSyntax)
{
	px_abi* plastabi;
	px_int i;
	for (i = pSyntax->reg_abi_stack.size - 1; i >= 0; i--)
	{
		plastabi = PX_VECTORAT(px_abi, &pSyntax->reg_abi_stack, i);
		if (PX_Syntax_CheckAbiName(plastabi, "scope"))
		{
			return plastabi;
		}
	}
	PX_ASSERTX("no scope abi");
	return PX_NULL;
	
}

px_int  PX_Syntax_ScopeAllocLocal(PX_Syntax* pSyntax, px_int size)
{
	px_abi* pabi = PX_Syntax_GetAbiFromBackward(pSyntax, "scope");
	px_int* palloc,offset;
	if (!pabi)
	{
		PX_ASSERTX("no scope abi");
		return 0;
	}
	palloc = PX_AbiGet_int(pabi, "alloc");
	if (!palloc)
	{
		PX_ASSERTX("memory error");
		return 0;
	}
	offset = *palloc;
	if (!PX_AbiSet_int(pabi, "alloc", *palloc + size))
	{
		PX_ASSERTX("PX_Syntax_ScopeAllocLocal memory error");
		return 0;
	}
	return offset;
}

px_int  PX_Syntax_ScopeAllocGlobal(PX_Syntax* pSyntax, px_int size)
{

	px_abi* pabi = PX_Syntax_GetAbiFromForward(pSyntax, "scope");
	px_int* palloc, offset;
	if (!pabi)
	{
		PX_ASSERTX("no scope abi");
		return 0;
	}
	palloc = PX_AbiGet_int(pabi, "alloc");
	if (!palloc)
	{
		PX_ASSERTX("memory error");
		return 0;
	}
	offset = *palloc;
	if (!PX_AbiSet_int(pabi, "alloc", *palloc + size))
	{
		PX_ASSERTX("PX_Syntax_ScopeAllocGlobal memory error");
		return 0;
	}
	return offset;
}

px_int PX_Syntax_ScopeAlloc(PX_Syntax* pSyntax, px_int size)
{
	if (pSyntax->reg_lifetime == 0)
	{
		return PX_Syntax_ScopeAllocGlobal(pSyntax, size);
	}
	else
	{
		return PX_Syntax_ScopeAllocLocal(pSyntax, size);
	}
	
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
	for (i = pSyntax->reg_abi_stack.size - 1; i >= 0; i--)
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
	px_int insert_index= pSyntax->reg_abi_stack.size-1;
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

	while (insert_index >= 0)
	{
		px_abi* pabi = PX_VECTORAT(px_abi, &pSyntax->reg_abi_stack, insert_index);
		px_int lifetime = PX_AbiGetValue_int(pabi, "lifetime");

		if (lifetime <= reg_lifetime)
		{
			break;
		}
		insert_index--;
	}
	
	if (!PX_VectorInsertAfter(&pSyntax->reg_abi_stack, insert_index, &abi))
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

	if (PX_VectorInsertBefore(&pSyntax->reg_abi_stack, index, &abi))
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

px_void PX_Syntax_PopLastSecondAbi(PX_Syntax* pSyntax)
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
	PX_Syntax_PopLastSecondAbi(pSyntax);
	
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

px_bool PX_Syntax_MergeLastAbiToIndexWithName(PX_Syntax* pSyntax, px_int index,const px_char new_name[])
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
	if (!PX_AbiSet_Abi(plastabi, psecondlastname, psecondlastabi))
	{
		return PX_FALSE;
	}
	PX_Syntax_PopLastSecondAbi(pSyntax);

	return PX_TRUE;
}

px_bool PX_Syntax_MergeAbiIndexToLast(PX_Syntax* pSyntax, px_int index)
{
	px_abi* plastabi, * psecondlastabi;
	const px_char* plastname, * psecondlastname;
	plastabi = PX_Syntax_GetAbiLast(pSyntax);
	psecondlastabi = PX_Syntax_GetAbiByIndex(pSyntax, index);
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
	if (!PX_AbiSet_Abi(plastabi, psecondlastname, psecondlastabi))
	{
		return PX_FALSE;
	}
	PX_Syntax_PopAbiIndex(pSyntax, index);

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
	
	PX_Syntax_PopLastSecondAbi(pSyntax);
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
		PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_MergeLast2AbiToNewAbi Memory Error");
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

	PX_Syntax_PopLastSecondAbi(pSyntax);
	PX_Syntax_PopLastSecondAbi(pSyntax);
	return PX_TRUE;

}

px_bool PX_Syntax_MergeAbiIndexToAbiIndexWithName(PX_Syntax* pSyntax, px_int src_index, px_int dst_index, const px_char name[])
{
	px_abi* pabi1, * pabi2;
	const px_char* name1, * name2;
	pabi1 = PX_Syntax_GetAbiByIndex(pSyntax, src_index);
	pabi2 = PX_Syntax_GetAbiByIndex(pSyntax, dst_index);
	if (!pabi1 || !pabi2)
	{
		return PX_FALSE;
	}
	name1 = PX_AbiGet_string(pabi1, "name");
	name2 = PX_AbiGet_string(pabi2, "name");
	if (!name1 || !name2)
	{
		return PX_FALSE;
	}
	PX_Syntax_Message(pSyntax, "Merge ");
	PX_Syntax_Message(pSyntax, name1);
	PX_Syntax_Message(pSyntax, ",");
	PX_Syntax_Message(pSyntax, name2);
	PX_Syntax_Message(pSyntax, " to Index:");
	PX_Syntax_Message(pSyntax, PX_itos(dst_index, 10).data);
	PX_Syntax_Message(pSyntax, ":");
	PX_Syntax_Message(pSyntax, name);
	PX_Syntax_Message(pSyntax, "\n");
	if (!PX_AbiSet_Abi(pabi2, name, pabi1))
	{
		return PX_FALSE;
	}
	PX_Syntax_PopAbiIndex(pSyntax, src_index);
	return PX_TRUE;
}
px_bool PX_Syntax_MergeAbiIndexToAbiIndex(PX_Syntax* pSyntax, px_int src_index, px_int dst_index)
{
	px_abi* pabi1, * pabi2;
	const px_char* name1, * name2;
	pabi1 = PX_Syntax_GetAbiByIndex(pSyntax, src_index);
	pabi2 = PX_Syntax_GetAbiByIndex(pSyntax, dst_index);
	if (!pabi1 || !pabi2)
	{
		return PX_FALSE;
	}
	name1 = PX_AbiGet_string(pabi1, "name");
	name2 = PX_AbiGet_string(pabi2, "name");
	if (!name1 || !name2)
	{
		return PX_FALSE;
	}
	PX_Syntax_Message(pSyntax, "Merge ");
	PX_Syntax_Message(pSyntax, name1);
	PX_Syntax_Message(pSyntax, ",");
	PX_Syntax_Message(pSyntax, name2);
	PX_Syntax_Message(pSyntax, " to Index:");
	PX_Syntax_Message(pSyntax, PX_itos(dst_index, 10).data);
	PX_Syntax_Message(pSyntax, ":");
	PX_Syntax_Message(pSyntax, name2);
	PX_Syntax_Message(pSyntax, "\n");
	if (!PX_AbiSet_Abi(pabi2, name2, pabi1))
	{
		return PX_FALSE;
	}
	PX_Syntax_PopAbiIndex(pSyntax, src_index);
	return PX_TRUE;
}


px_bool PX_Syntax_Initialize(px_memorypool* mp, PX_Syntax* pSyntax)
{
	pSyntax->mp = mp;

	if(!PX_SyntaxLexer_Initialize(mp,&pSyntax->reg_syntaxlexer))
		return PX_FALSE;
	PX_SyntaxLexer_RegisterDelimiter(&pSyntax->reg_syntaxlexer, ',');
	PX_SyntaxLexer_RegisterDelimiter(&pSyntax->reg_syntaxlexer, '.');
	PX_SyntaxLexer_RegisterDelimiter(&pSyntax->reg_syntaxlexer, ';');
	PX_SyntaxLexer_RegisterDelimiter(&pSyntax->reg_syntaxlexer, ':');
	PX_SyntaxLexer_RegisterDelimiter(&pSyntax->reg_syntaxlexer, '+');
	PX_SyntaxLexer_RegisterDelimiter(&pSyntax->reg_syntaxlexer, '-');
	PX_SyntaxLexer_RegisterDelimiter(&pSyntax->reg_syntaxlexer, '*');
	PX_SyntaxLexer_RegisterDelimiter(&pSyntax->reg_syntaxlexer, '/');
	PX_SyntaxLexer_RegisterDelimiter(&pSyntax->reg_syntaxlexer, '%');
	PX_SyntaxLexer_RegisterDelimiter(&pSyntax->reg_syntaxlexer, '&');
	PX_SyntaxLexer_RegisterDelimiter(&pSyntax->reg_syntaxlexer, '^');
	PX_SyntaxLexer_RegisterDelimiter(&pSyntax->reg_syntaxlexer, '~');
	PX_SyntaxLexer_RegisterDelimiter(&pSyntax->reg_syntaxlexer, '(');
	PX_SyntaxLexer_RegisterDelimiter(&pSyntax->reg_syntaxlexer, ')');
	PX_SyntaxLexer_RegisterDelimiter(&pSyntax->reg_syntaxlexer, '!');
	PX_SyntaxLexer_RegisterDelimiter(&pSyntax->reg_syntaxlexer, '=');
	PX_SyntaxLexer_RegisterDelimiter(&pSyntax->reg_syntaxlexer, '>');
	PX_SyntaxLexer_RegisterDelimiter(&pSyntax->reg_syntaxlexer, '<');
	PX_SyntaxLexer_RegisterDelimiter(&pSyntax->reg_syntaxlexer, '{');
	PX_SyntaxLexer_RegisterDelimiter(&pSyntax->reg_syntaxlexer, '}');
	PX_SyntaxLexer_RegisterDelimiter(&pSyntax->reg_syntaxlexer, '[');
	PX_SyntaxLexer_RegisterDelimiter(&pSyntax->reg_syntaxlexer, ']');
	PX_SyntaxLexer_RegisterDelimiter(&pSyntax->reg_syntaxlexer, '#');
	PX_SyntaxLexer_RegisterDelimiter(&pSyntax->reg_syntaxlexer, '\"');
	PX_SyntaxLexer_RegisterDiscard(&pSyntax->reg_syntaxlexer, "\\\n");

	PX_SyntaxLexer_RegisterSpacer(&pSyntax->reg_syntaxlexer, ' ');
	PX_SyntaxLexer_RegisterSpacer(&pSyntax->reg_syntaxlexer, '\t');
	//PX_SyntaxLexer_RegisterComment(&pSyntax->reg_syntaxlexer, "//", "\n");
	//PX_SyntaxLexer_RegisterComment(&pSyntax->reg_syntaxlexer, "/*", "*/");

	if (!PX_StringInitialize(mp, &pSyntax->message))
		return PX_FALSE;

	PX_VectorInitialize(mp, &pSyntax->pebnf, sizeof(PX_Syntax_pebnf), 0);
	PX_VectorInitialize(mp, &pSyntax->reg_ast_stack, sizeof(PX_Syntax_ast), 0);
	PX_VectorInitialize(mp, &pSyntax->reg_abi_stack, sizeof(px_abi), 0);
	PX_VectorInitialize(mp, &pSyntax->reg_ast_instr_stack, sizeof(px_abi), 0);
	PX_VectorInitialize(mp, &pSyntax->reg_maptype_stack, sizeof(PX_Syntax_maptype), 0);
	PX_VectorInitialize(mp, &pSyntax->reg_expr_opcode_stack, sizeof(PX_Syntax_opcode), 32);


	pSyntax->reg_lifetime = 0;
	pSyntax->reg_lastsource_index = -1;
	pSyntax->reg_expr_begin_line = 0;
	pSyntax->reg_expr_source_index = -1;
	pSyntax->reg_write_expr_begin_line = -1;
	pSyntax->reg_write_expr_source_index = -1;

	PX_memset(&pSyntax->bnfnode_return, 0, sizeof(PX_Syntax_bnfnode));
	PX_StringInitialize(mp, &pSyntax->bnfnode_return.constant);

	return PX_TRUE;
}

px_bool PX_Syntax_MergeLast2AbiValue(PX_Syntax* pSyntax,const px_char last_abi_name[],const px_char value_name[],const px_char second_last_abi_name[],const px_char value_name2[])
{
	px_int last_abi_index = PX_Syntax_GetAbiIndexFromBackward(pSyntax, last_abi_name);
	px_abi* plast_ir_abi = PX_Syntax_GetAbiByIndex(pSyntax, last_abi_index);
	px_abi* psecondlast_ir_abi = PX_Syntax_GetAbiFromBackward(pSyntax, second_last_abi_name);
	const px_char* pvalue;
	if (!plast_ir_abi)
	{
		PX_ASSERTX("last abi not found");
		return PX_FALSE;
	}
	pvalue = PX_AbiGet_string(plast_ir_abi, value_name);
	if (!pvalue)
	{
		PX_ASSERTX("value not found");
		return PX_FALSE;
	}
	if (!psecondlast_ir_abi)
	{
		PX_ASSERTX("second last abi not found");
		return PX_FALSE;
	}
	else
	{
		if (!PX_AbiAppend_string(psecondlast_ir_abi, value_name2, pvalue))
		{
			PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_AppendString Memory Error2");
			return PX_FALSE;
		}
		PX_Syntax_PopAbiIndex(pSyntax, last_abi_index);
	}
	return PX_TRUE;
}
px_bool PX_Syntax_AppendData(PX_Syntax* pSyntax, const px_char name[], const char data_name[], const px_byte data[], px_int datasize)
{
	px_abi* pabi = PX_Syntax_GetAbiFromBackward(pSyntax, name);
	if (!pabi)
	{
		PX_ASSERTX("could not found abi");
		return PX_FALSE;
	}

	if (!PX_AbiExist_Type(pabi, data_name, PX_ABI_TYPE_DATA))
	{
		if (!PX_AbiSet_data(pabi, data_name, data, datasize))
		{
			return PX_FALSE;
		}
		return PX_TRUE;
	}
	if (!PX_AbiAppend_data(pabi, data_name, data, datasize))
	{
		PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_AppendData Memory Error2");
		return PX_FALSE;
	}
	return PX_TRUE;
}

px_bool PX_Syntax_DeleteData(PX_Syntax* pSyntax, const px_char name[], const char data_name[])
{
	px_abi* pabi = PX_Syntax_GetAbiFromBackward(pSyntax, name);
	if (!pabi)
	{
		PX_ASSERTX("could not found abi");
		return PX_FALSE;
	}

	if (PX_AbiExist_Type(pabi, data_name, PX_ABI_TYPE_DATA))
	{
		if (!PX_AbiDelete(pabi, data_name))
		{
			return PX_FALSE;
		}
		return PX_TRUE;
	}
	return PX_TRUE;
}



px_bool PX_Syntax_AppendString(PX_Syntax* pSyntax, const px_char name[],const char content_name[] ,const px_char content[])
{
	px_abi* pabi = PX_Syntax_GetAbiFromBackward(pSyntax, name);
	if (!pabi)
	{
		PX_ASSERTX("could not found abi");
		return PX_FALSE;
	}

	if (!PX_AbiGet_string(pabi, content_name))
	{
		if (PX_AbiSet_string(pabi,content_name,content))
		{
			return PX_TRUE;
		}
		else
		{
			return PX_FALSE;
		}
	}

	if (!PX_AbiAppend_string(pabi, content_name, content))
	{
		PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_AppendString Memory Error2");
		return PX_FALSE;
	}
	return PX_TRUE;
}


px_bool PX_Syntax_NewMap(PX_Syntax* pSyntax, px_int sourceindex,px_int begin, px_int end)
{
	px_int i,x;
	PX_SyntaxLexer_Source* psource = PX_SyntaxLexer_GetSourceByIndex(&pSyntax->reg_syntaxlexer, sourceindex);
	px_abi wabi;
	if (!psource)
	{
		PX_ASSERTX("could not found source");
		return PX_FALSE;
	}
	i = psource->descriptor.size;
	x = 0;
	while (i>0&&x<=8)
	{
		px_abi* pabi = PX_VECTORAT(px_abi, &psource->descriptor, i - 1);
		px_int source_index = PX_AbiGetValue_int(pabi, "source_index");
		px_int tbegin = PX_AbiGetValue_int(pabi, "begin");
		px_int tend = PX_AbiGetValue_int(pabi, "end");
		if (source_index== sourceindex&&begin == tbegin && end == tend)
		{
			psource->last_descriptor_index = i - 1;
			return PX_TRUE;
		}
		i--;
		x++;
	}
	PX_AbiCreate_DynamicWriter(&wabi, pSyntax->mp);
	if (!PX_AbiSet_int(&wabi, "source_index", sourceindex))
	{
		PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_ide_new Memory Error1");
		return PX_FALSE;
	}

	if (!PX_AbiSet_int(&wabi, "begin", begin))
	{
		PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_ide_new Memory Error1");
		return PX_FALSE;
	}
	if (!PX_AbiSet_int(&wabi, "end", end))
	{
		PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_ide_new Memory Error2");
		return PX_FALSE;
	}
	if (!PX_VectorPushback(&psource->descriptor, &wabi))
	{
		PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_ide_new Memory Error3");
		return PX_FALSE;
	}
	psource->last_descriptor_index = psource->descriptor.size - 1;

	for (i = begin; i <= end; i++)
	{
		px_int map_cell_index;
		if (i >= psource->source_length)
		{
			PX_ASSERTX("Assert Error: invalid map cell index");
			return PX_FALSE;
		}
		map_cell_index = psource->source_index_map_to_cell_index[i];
		if (map_cell_index < 0 || map_cell_index >= psource->cells_count)
		{
			PX_ASSERTX("Assert Error: invalid map cell index");
			return PX_FALSE;
		}
		psource->cells[map_cell_index].abi_index = psource->last_descriptor_index;
	}
	return PX_TRUE;
}
px_bool PX_Syntax_NewMapToken(PX_Syntax* pSyntax, px_int begin_sourceindex, px_int begin, px_int end_sourceindex, px_int end, px_color color, const px_char type[])
{
	PX_SyntaxLexer_Source* psource;
	px_abi* pabi;
	if (type==PX_NULL)
	{
		type = "";
	}
	if (begin_sourceindex!= end_sourceindex)
	{
		return PX_FALSE;
	}

	psource = PX_SyntaxLexer_GetSourceByIndex(&pSyntax->reg_syntaxlexer, begin_sourceindex);
	if (!psource)
	{
		PX_ASSERTX("could not found source");
		return PX_FALSE;
	}
	if (!PX_Syntax_NewMap(pSyntax, begin_sourceindex, begin, end))
		return PX_FALSE;

	pabi = PX_VECTORAT(px_abi, &psource->descriptor, psource->last_descriptor_index);
	if (!PX_AbiSet_color(pabi, "color", color))
	{
		PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_ide_color Memory Error");
		return PX_FALSE;
	}
	if (!PX_AbiSet_string(pabi, "type", type))
	{
		PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_ide_type Memory Error1");
		return PX_FALSE;
	}
	return PX_TRUE;
}

px_bool PX_Syntax_LastMapInfo(PX_Syntax* pSyntax, px_int sourceindex, const px_char info[])
{
	PX_SyntaxLexer_Source* psource;
	px_abi* pabi;
	psource = PX_SyntaxLexer_GetSourceByIndex(&pSyntax->reg_syntaxlexer, sourceindex);
	if (!psource)
	{
		PX_ASSERTX("could not found source");
		return PX_FALSE;
	}

	pabi = PX_VECTORAT(px_abi, &psource->descriptor, psource->last_descriptor_index);
	if (!PX_AbiSet_string(pabi, "info", info))
	{
		PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_LastMapInfo Memory Error");
		return PX_FALSE;
	}
	return PX_TRUE;
}

px_bool PX_Syntax_LastMapToMemory(PX_Syntax* pSyntax, px_int sourceindex,  PX_SYNTAX_OPRAND_FROM from, px_int offset)
{
	PX_SyntaxLexer_Source* psource = PX_SyntaxLexer_GetSourceByIndex(&pSyntax->reg_syntaxlexer, sourceindex);
	px_abi* pabi;
	if (!psource)
	{
		PX_ASSERTX("Syntax Error: invalid source index");
		return PX_FALSE;
	}
	pabi = PX_VECTORAT(px_abi, &psource->descriptor, psource->last_descriptor_index);

	if (!PX_AbiSet_int(pabi, "scope", (px_int)from))
	{
		PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_ide_map Memory Error1");
		return PX_FALSE;
	}
	if (!PX_AbiSet_int(pabi, "offset", offset))
	{
		PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_ide_map Memory Error2");
		return PX_FALSE;
	}
	return PX_TRUE;
}



px_bool PX_Syntax_NewMapTypeDefine(PX_Syntax* pSyntax,  const px_char basetype[], px_int size)
{
	PX_Syntax_maptype maptype = { 0 };
	if (!PX_StringInitialize(pSyntax->mp, &maptype.name))
		return PX_FALSE;
	if (!PX_VectorInitialize(pSyntax->mp, &maptype.members, sizeof(px_string), 0))
	{
		PX_StringFree(&maptype.name);
		return PX_FALSE;
	}

	if (!PX_StringSet(&maptype.name, basetype))
	{
		PX_StringFree(&maptype.name);
		PX_VectorFree(&maptype.members);
		return PX_FALSE;
	}

	if (!PX_VectorPushback(&pSyntax->reg_maptype_stack, &maptype))
	{
		PX_StringFree(&maptype.name);
		PX_VectorFree(&maptype.members);
		return PX_FALSE;
	}
	return PX_TRUE;
}

px_bool PX_Syntax_LastMapTypeMember(PX_Syntax* pSyntax,  const px_char basetype[], px_int count)
{
	PX_Syntax_maptype* plast;
	px_string membername;
	if (pSyntax->reg_maptype_stack.size==0)
	{
		PX_ASSERTX("PX_Syntax_LastMapTypeMember: no map type defined");
		return PX_FALSE;
	}
	plast = PX_VECTORAT(PX_Syntax_maptype, &pSyntax->reg_maptype_stack, pSyntax->reg_maptype_stack.size - 1);
	if (!PX_StringInitialize(pSyntax->mp, &membername))
	{
		return PX_FALSE;
	}
	if (!PX_StringSet(&membername, basetype))
	{
		return PX_FALSE;
	}
	if (!PX_VectorPushback(&plast->members, &membername))
	{
		return PX_FALSE;
	}
	return PX_TRUE;
}


px_bool PX_Syntax_NewIRInstruction3(PX_Syntax* pSyntax, const px_char opcode[], const px_char oprand1[], const px_char oprand2[], const px_char oprand3[])
{
	px_char content[64] = { 0 };
	if (pSyntax->reg_write_expr_begin_line!=pSyntax->reg_expr_begin_line|| pSyntax->reg_write_expr_source_index != pSyntax->reg_expr_source_index)
	{
		PX_sprintf2(content, sizeof(content), ";@loc %1 %2\n", PX_STRINGFORMAT_INT(pSyntax->reg_expr_source_index), PX_STRINGFORMAT_INT(pSyntax->reg_expr_begin_line));
		if (!PX_Syntax_AppendString(pSyntax,"expr","ir", content))return PX_FALSE;
		pSyntax->reg_write_expr_begin_line = pSyntax->reg_expr_begin_line;
		pSyntax->reg_write_expr_source_index = pSyntax->reg_expr_source_index;
	}
	content[0] = '\0';
	PX_strcat_s(content,sizeof(content), opcode);
	if (oprand1[0])
	{
		PX_strcat_s(content,sizeof(content), " ");
		PX_strcat_s(content,sizeof(content),oprand1);
	}
	if (oprand2[0])
	{
		PX_strcat_s(content,sizeof(content), ",");
		PX_strcat_s(content,sizeof(content), oprand2);
	}
	if (oprand3[0])
	{
		PX_strcat_s(content,sizeof(content), ",");
		PX_strcat_s(content,sizeof(content),oprand3);
	}
	PX_strcat_s(content,sizeof(content), "\n");
	if (!PX_Syntax_AppendString(pSyntax,"expr","ir", content))return PX_FALSE;
	return PX_TRUE;
}

px_bool PX_Syntax_NewIRInstruction0(PX_Syntax* pSyntax, const px_char opcode[])
{
	return PX_Syntax_NewIRInstruction3(pSyntax, opcode, "","","");
}
px_bool PX_Syntax_NewIRInstruction1(PX_Syntax* pSyntax, const px_char opcode[], const px_char oprand1[])
{
	return PX_Syntax_NewIRInstruction3(pSyntax, opcode, oprand1, "", "");
}
px_bool PX_Syntax_NewIRInstruction2(PX_Syntax* pSyntax, const px_char opcode[], const px_char oprand1[], const px_char oprand2[])
{
	return PX_Syntax_NewIRInstruction3(pSyntax, opcode, oprand1, oprand2, "");
}

px_void PX_Syntax_VariableToInstructionOperand(PX_Syntax* pSyntax, px_int abi_index, px_char content[16])
{
	const px_char* pscope;
	px_int offset;
	px_abi* pabi = PX_Syntax_GetAbiByIndex(pSyntax, abi_index);
	PX_ASSERTIFX(!pabi, "PX_Syntax_VariableToInstructionOperand: invalid abi index");
	PX_ASSERTIFX(PX_Syntax_CheckAbiName(pabi, "variable") == PX_FALSE, "PX_Syntax_VariableToInstructionOperand: abi is not variable");
	pscope = PX_AbiGet_string(pabi, "scope");
	offset = PX_AbiGetValue_int(pabi, "offset");
	if (PX_strequ(pscope, "global"))
	{
		PX_sprintf1(content, 16, "[%1]", PX_STRINGFORMAT_INT(offset));
	}
	else if (PX_strequ(pscope, "local"))
	{
		PX_sprintf1(content, 16, "[bp-%1]", PX_STRINGFORMAT_INT(offset));
	}
	else
	{
		PX_ASSERTX( "PX_Syntax_VariableToInstructionOperand: invalid region");
	}
}

px_void PX_Syntax_ResetSource(PX_Syntax* pSyntax)
{
	px_int i;
	px_abi* pabi;
	PX_VectorClear(&pSyntax->reg_ast_stack);
	for (i = 0; i < pSyntax->reg_abi_stack.size; i++)
	{
		pabi = PX_VECTORAT(px_abi, &pSyntax->reg_abi_stack, i);
		PX_AbiFree(pabi);
	}
	PX_VectorClear(&pSyntax->reg_abi_stack);

	PX_StringClear(&pSyntax->message);
	pSyntax->reg_lifetime = 0;
	pSyntax->reg_unname_index = 0;
	PX_memset(pSyntax->reg_unname, 0, sizeof(pSyntax->reg_unname));
	pSyntax->reg_lastsource_index = -1;
	PX_SyntaxLexer_Reset(&pSyntax->reg_syntaxlexer);
}


px_void PX_Syntax_ClearState(PX_Syntax* pSyntax)
{
	px_int i;
	px_abi* pabi;
	px_abi* popcode;

	PX_VectorClear(&pSyntax->reg_ast_stack);
	for (i = 0; i < pSyntax->reg_abi_stack.size; i++)
	{
		pabi = PX_VECTORAT(px_abi, &pSyntax->reg_abi_stack, i);
		PX_AbiFree(pabi);
	}
	PX_VectorClear(&pSyntax->reg_abi_stack);

	for (i = 0; i < pSyntax->reg_ast_instr_stack.size; i++)
	{
		popcode = PX_VECTORAT(px_abi, &pSyntax->reg_ast_instr_stack, i);
		PX_AbiFree(popcode);
	}
	PX_VectorClear(&pSyntax->reg_ast_instr_stack);
	PX_VectorClear(&pSyntax->reg_expr_opcode_stack);
	PX_SyntaxLexer_Reset(&pSyntax->reg_syntaxlexer);

	for (i = 0; i < pSyntax->reg_maptype_stack.size; i++)
	{
		px_int j;
		PX_Syntax_maptype* pmaptype = PX_VECTORAT(PX_Syntax_maptype, &pSyntax->reg_maptype_stack, i);
		for (j = 0; j < pmaptype->members.size; j++)
		{
			px_string* pstr = PX_VECTORAT(px_string, &pmaptype->members, j);
			PX_StringFree(pstr);
		}
		PX_StringFree(&pmaptype->name);
	}
	PX_VectorClear(&pSyntax->reg_maptype_stack);
	PX_StringClear(&pSyntax->message);
	pSyntax->reg_lifetime = 0;
	pSyntax->reg_unname_index = 0;
	PX_memset(pSyntax->reg_unname, 0, sizeof(pSyntax->reg_unname));
	pSyntax->reg_lastsource_index = -1;

}

px_void PX_Syntax_ClearSourceDescription(PX_Syntax* pSyntax)
{
	px_int i, j;
	for (i = 0; i < pSyntax->reg_syntaxlexer.sources.size; i++)
	{
		PX_SyntaxLexer_Source* psource = PX_VECTORAT(PX_SyntaxLexer_Source, &pSyntax->reg_syntaxlexer.sources, i);
		for (j = 0; j < psource->descriptor.size; j++)
		{
			px_abi* pabi = PX_VECTORAT(px_abi, &psource->descriptor, j);
			PX_AbiFree(pabi);
		}
		PX_VectorClear(&psource->descriptor);
		psource->last_descriptor_index = -1;
		for (j = 0; j < psource->cells_count; j++)
		{
			psource->cells[j].abi_index = -1;
		}
	}
}

px_void PX_Syntax_ClearSources(PX_Syntax* pSyntax)
{
	PX_SyntaxLexer_Clear(&pSyntax->reg_syntaxlexer);
}

px_void PX_Syntax_SetBreak(PX_Syntax* pSyntax, px_int id)
{
	pSyntax->break_execute_id = id;	
}

px_color PX_Syntax_GetRandomColor(px_uint32 seed)
{
	px_color color;
	seed = PX_rand_lcg(seed);
	color._argb.r = 128+(px_byte)(seed % 128);
	seed = PX_rand_lcg(seed);
	color._argb.g = 64+(px_byte)(seed % 192);
	seed = PX_rand_lcg(seed);
	color._argb.b = 64+(px_byte)(seed % 192);
	color._argb.a = 255;
	return color;
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
	PX_Syntax_ClearState(pSyntax);
	for (i = 0; i < pSyntax->pebnf.size; i++)
	{
		ppebnf = PX_VECTORAT(PX_Syntax_pebnf, &pSyntax->pebnf, i);
		if (ppebnf && ppebnf->pbnfnode)
		{
			PX_SyntaxFree_pebnfnode(pSyntax, ppebnf->pbnfnode);
		}
		PX_StringFree(&ppebnf->mnenonic);
	}

	PX_VectorFree(&pSyntax->reg_maptype_stack);
	PX_VectorFree(&pSyntax->pebnf);
	PX_VectorFree(&pSyntax->reg_ast_stack);
	PX_VectorFree(&pSyntax->reg_abi_stack);
	PX_VectorFree(&pSyntax->reg_ast_instr_stack);
	PX_VectorFree(&pSyntax->reg_expr_opcode_stack);
	PX_StringFree(&pSyntax->message);
	PX_StringFree(&pSyntax->bnfnode_return.constant);

	PX_SyntaxLexer_Free(&pSyntax->reg_syntaxlexer);
}

px_bool PX_Syntax_AddSource(PX_Syntax* pSyntax, const px_char name[], const px_char source[])
{
	return PX_SyntaxLexer_AddSource(&pSyntax->reg_syntaxlexer,name, source);
}

PX_SyntaxLexer_Source* PX_Syntax_GetSourceByIndex(PX_Syntax* pSyntax, px_int index)
{
	return PX_SyntaxLexer_GetSourceByIndex(&pSyntax->reg_syntaxlexer, index);
}

PX_SyntaxLexer_Source* PX_Syntax_GetSource(PX_Syntax* pSyntax, const px_char name[])
{
	return PX_SyntaxLexer_GetSourceByName(&pSyntax->reg_syntaxlexer, name);
}

px_int PX_Syntax_GetSourceCount(PX_Syntax* pSyntax)
{
	return PX_SyntaxLexer_GetSourceCount(&pSyntax->reg_syntaxlexer);
}

typedef enum
{
	PX_SYNTAXLEXER_FILTER_NONE = 0,
	PX_SYNTAXLEXER_FILTER_NEWLINE=1,
	PX_SYNTAXLEXER_FILTER_SPACER=2,
}PX_SYNTAXLEXER_FILTER;

static PX_SYNTAXLEXER_LEXEME_TYPE PX_Syntax_GetNextLexemeEx2(PX_Syntax* pSyntax,px_int filter)
{
	PX_SYNTAXLEXER_LEXEME_TYPE type;
	while (PX_TRUE)
	{
		type = PX_SyntaxLexer_GetNextLexeme(&pSyntax->reg_syntaxlexer);
		if (type == PX_SYNTAXLEXER_LEXEME_TYPE_COMMENT)
			continue;

		if ((filter & PX_SYNTAXLEXER_FILTER_NEWLINE) && type == PX_SYNTAXLEXER_LEXEME_TYPE_NEWLINE)
			continue;

		if ((filter & PX_SYNTAXLEXER_FILTER_SPACER) && type == PX_SYNTAXLEXER_LEXEME_TYPE_SPACER)
			continue;

		else if (type == PX_SYNTAXLEXER_LEXEME_TYPE_END)
		{
				return PX_SYNTAXLEXER_LEXEME_TYPE_END;
		}
		else
			return type;
	}
}

static PX_SYNTAXLEXER_LEXEME_TYPE PX_Syntax_GetNextLexemeEx(PX_Syntax* pSyntax, px_int filter)
{
	px_int last_entry = -1;
	const px_char _macro_expand_tag[64] = {0};
	while (PX_TRUE)
	{
		return PX_Syntax_GetNextLexemeEx2(pSyntax, filter);
	}

}

px_int PX_Syntax_GetCurrentLexemeBeginSourceIndex(PX_Syntax* pSyntax)
{
	return PX_SyntaxLexer_GetLexemeBeginSourceIndex(&pSyntax->reg_syntaxlexer);
}

px_int PX_Syntax_GetCurrentLexemeEndSourceIndex(PX_Syntax* pSyntax)
{
	return PX_SyntaxLexer_GetLexemeEndSourceIndex(&pSyntax->reg_syntaxlexer);
}

const px_char* PX_Syntax_GetCurrentLexerSourceContentPointer(PX_Syntax* pSyntax)
{
	return PX_SyntaxLexer_GetCurrentSourcePointer(&pSyntax->reg_syntaxlexer);
}

px_syntaxlexer* PX_Syntax_GetSyntaxLexer(PX_Syntax* pSyntax)
{
	return &pSyntax->reg_syntaxlexer;
}

px_int PX_Syntax_GetCurrentLexerOffset(PX_Syntax* pSyntax)
{
	return PX_SyntaxLexer_GetCurrentSourceOffset(&pSyntax->reg_syntaxlexer);
}

px_int PX_Syntax_GetCurrentLexemeBegin(PX_Syntax* pSyntax)
{
	return PX_SyntaxLexer_GetLexemeBegin(&pSyntax->reg_syntaxlexer);
}

px_int PX_Syntax_GetCurrentLexemeEnd(PX_Syntax* pSyntax)
{
	return PX_SyntaxLexer_GetLexemeEnd(&pSyntax->reg_syntaxlexer);
}

px_int PX_Syntax_GetCurrentLexemeLine(PX_Syntax* pSyntax)
{
	return PX_SyntaxLexer_GetLexemeLine(&pSyntax->reg_syntaxlexer);
}

px_int PX_Syntax_GetCurrentLexerIndex(PX_Syntax* pSyntax)
{
	return PX_SyntaxLexer_GetCurrentSourceIndex(&pSyntax->reg_syntaxlexer);
}

px_int PX_Syntax_GetCurrentLexerLine(PX_Syntax* pSyntax)
{
	return PX_SyntaxLexer_GetCurrentLine(&pSyntax->reg_syntaxlexer);
}

const px_char* PX_Syntax_GetCurrentLexerSourceName(PX_Syntax* pSyntax)
{
	return PX_SyntaxLexer_GetCurrentSourceName(&pSyntax->reg_syntaxlexer);
}

const px_char* PX_Syntax_GetCurrentLexeme(PX_Syntax* pSyntax)
{
	return PX_SyntaxLexer_GetCurrentLexeme(&pSyntax->reg_syntaxlexer);
}

const px_char* PX_Syntax_GetCurrentLexerSourcePointer(PX_Syntax* pSyntax)
{
	return PX_SyntaxLexer_GetCurrentSourcePointer(&pSyntax->reg_syntaxlexer);
}

const px_char* PX_Syntax_GetCurrentLexerEntrySourceName(PX_Syntax* pSyntax)
{
	return PX_SyntaxLexerGetEntrySourceName(&pSyntax->reg_syntaxlexer);
}

px_char PX_Syntax_PreviewNextChar(PX_Syntax* pSyntax)
{
	return PX_SyntaxLexer_PreviewNextChar(&pSyntax->reg_syntaxlexer);
}

px_char PX_Syntax_GetNextChar(PX_Syntax* pSyntax)
{
	return PX_SyntaxLexer_GetNextChar(&pSyntax->reg_syntaxlexer);
}

px_void PX_Syntax_LexerForward(PX_Syntax* pSyntax, px_int offset)
{
	PX_SyntaxLexer_Forward(&pSyntax->reg_syntaxlexer, offset);
}

PX_SYNTAXLEXER_LEXEME_TYPE PX_Syntax_GetNextLexeme(PX_Syntax* pSyntax)
{
	return PX_Syntax_GetNextLexemeEx(pSyntax, PX_SYNTAXLEXER_FILTER_NEWLINE| PX_SYNTAXLEXER_FILTER_SPACER);
}

PX_SYNTAXLEXER_LEXEME_TYPE PX_Syntax_GetNextLexeme2(PX_Syntax* pSyntax)
{
	return PX_Syntax_GetNextLexemeEx(pSyntax, PX_SYNTAXLEXER_FILTER_NEWLINE);
}

PX_SYNTAXLEXER_LEXEME_TYPE PX_Syntax_GetNextLexeme3(PX_Syntax* pSyntax)
{
	return PX_Syntax_GetNextLexemeEx(pSyntax, PX_SYNTAXLEXER_FILTER_SPACER);
}

PX_SYNTAXLEXER_LEXEME_TYPE PX_Syntax_GetNextLexeme4(PX_Syntax* pSyntax)
{
	return PX_Syntax_GetNextLexemeEx(pSyntax, PX_SYNTAXLEXER_FILTER_NONE);
}

PX_SYNTAXLEXER_LEXEME_TYPE PX_Syntax_GetLexemeType(PX_Syntax* pSyntax)
{
	return PX_SyntaxLexer_GetLexemeType(&pSyntax->reg_syntaxlexer);
}

PX_SYNTAXLEXER_STATE PX_Syntax_GetLexerState(PX_Syntax* pSyntax)
{
	return PX_SyntaxLexer_GetState(&pSyntax->reg_syntaxlexer);
}

PX_SyntaxLexer_Source* PX_Syntax_GetCurrentSource(PX_Syntax* pSyntax)
{
	return PX_SyntaxLexer_GetCurrentSource(&pSyntax->reg_syntaxlexer);
}

px_void PX_Syntax_SetLexerState(PX_Syntax* pSyntax, PX_SYNTAXLEXER_STATE *pstate)
{
	PX_SyntaxLexer_SetState(&pSyntax->reg_syntaxlexer, pstate);
}

static px_bool PX_Syntax_ExecuteNextAst(PX_Syntax* pSyntax, PX_Syntax_ast* pcurrent_ast)
{
	PX_Syntax_ast newast = { 0 };
	//push next
	if (!pcurrent_ast->pbnfnode||pcurrent_ast->pbnfnode->pnext == PX_NULL)
	{
		PX_Syntax_Message(pSyntax, "AST:next node is null,return true\n");
		if (PX_Syntax_ExecuteAstOpcode(pSyntax, "return true"))
			return PX_TRUE;
		PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_ExecuteNextAst out of memory1");
		return PX_FALSE;
	}
	else
	{
		PX_Syntax_Message(pSyntax, "AST:next node ");
		if(PX_strequ(pcurrent_ast->pbnfnode->pnext->constant.buffer,"\n"))
			PX_Syntax_Message(pSyntax, "newline");
		else
			PX_Syntax_Message(pSyntax, pcurrent_ast->pbnfnode->pnext->constant.buffer);
		PX_Syntax_Message(pSyntax, "\n");
		if (pcurrent_ast->pbnfnode->pnext->type==PX_SYNTAX_AST_TYPE_LINKER)
		{
			newast.pebnf_index = PX_Syntax_GetPebnfIndexByMnemonic(pSyntax, pcurrent_ast->pbnfnode->pnext->constant.buffer);
			PX_ASSERTIFX(newast.pebnf_index == -1,"Linker node should not be a null-pebnf node");
		}
		else
		{
			newast.pebnf_index = -1;
		}
		newast.pbnfnode = pcurrent_ast->pbnfnode->pnext;//try next
		PX_ASSERTIFX(newast.pbnfnode == PX_NULL, "pbnfnode should not be null");
		newast.call_abistack_count = pSyntax->reg_abi_stack.size;
		newast.syntaxlexer_state = PX_SyntaxLexer_GetState(&pSyntax->reg_syntaxlexer);
	
		if (!PX_VectorPushback(&pSyntax->reg_ast_stack, &newast))
		{
			PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_ExecuteNextAst out of memory2");
			return PX_FALSE;
		}
		return PX_TRUE;
	}
}

px_void PX_Syntax_AstClearAbiStack(PX_Syntax* pSyntax, PX_Syntax_ast* past)
{
	while (pSyntax->reg_abi_stack.size > past->call_abistack_count)
	{
		PX_Syntax_PopAbi(pSyntax);
	}
}

static px_bool PX_Syntax_ExecuteOthers(PX_Syntax* pSyntax, PX_Syntax_ast* pcurrent_ast)
{
	px_char content_message[128] = { 0 };
	PX_Syntax_ast newast = { 0 };
	PX_Syntax_bnfnode* pbnfnode = pcurrent_ast->pbnfnode;
	PX_Syntax_AstClearAbiStack(pSyntax, pcurrent_ast);
	//not matched,try others
	if (!pbnfnode||pbnfnode->pothers==PX_NULL)
	{
		PX_sprintf1(content_message, sizeof(content_message), "AST:others node is null,%1 return false\n", PX_STRINGFORMAT_STRING(pbnfnode->constant.buffer));
		PX_Syntax_Message(pSyntax, content_message);

		if (PX_Syntax_ExecuteAstOpcode(pSyntax, "return false"))
			return PX_TRUE;
		
		PX_Syntax_Terminate(pSyntax,  "runtime:error:PX_Syntax_ExecuteOthers out of memory");
		return PX_FALSE;
	}
	else
	{
		PX_Syntax_Message(pSyntax, "AST:others node ");
		PX_Syntax_Message(pSyntax, pbnfnode->pothers->constant.buffer);
		PX_Syntax_Message(pSyntax, "\n");
		if (pcurrent_ast->pbnfnode->pothers->type == PX_SYNTAX_AST_TYPE_LINKER)
		{
			newast.pebnf_index = PX_Syntax_GetPebnfIndexByMnemonic(pSyntax, pcurrent_ast->pbnfnode->pothers->constant.buffer);
			PX_ASSERTIFX(newast.pebnf_index == -1, "Error:Syntax Get PEBNF Error");
		}
		else
		{
			newast.pebnf_index = -1;
		}
		newast.syntaxlexer_state = pcurrent_ast->syntaxlexer_state;
		newast.pbnfnode = pbnfnode->pothers;
		PX_ASSERTIFX(newast.pbnfnode == PX_NULL, "pbnfnode should not be null");
		newast.call_abistack_count = pSyntax->reg_abi_stack.size;
		if (!PX_VectorPushback(&pSyntax->reg_ast_stack, &newast))
		{
			PX_Syntax_Terminate(pSyntax,  "runtime:error:PX_Syntax_ExecuteOthers out of memory1");
			return PX_FALSE;
		}
	}
	
	return PX_TRUE;
}

px_bool PX_Syntax_NewAst (PX_Syntax* pSyntax, const px_char mnemonic[])
{
	PX_Syntax_pebnf* ppebnf = PX_Syntax_GetPebnf(pSyntax, mnemonic);
	PX_Syntax_ast newast = { 0 };
	if (!ppebnf)
	{
		PX_ASSERT();
		return PX_FALSE;
	}
	newast.syntaxlexer_state = PX_SyntaxLexer_GetState(&pSyntax->reg_syntaxlexer);
	newast.pbnfnode = ppebnf->pbnfnode;
	PX_ASSERTIFX(newast.pbnfnode == PX_NULL, "pbnfnode should not be null");
	newast.call_abistack_count = pSyntax->reg_abi_stack.size;
	if (!PX_VectorPushback(&pSyntax->reg_ast_stack, &newast))
	{
		PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_NewAst out of memory");
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

px_int PX_Syntax_GetAbiIndexFromForward_LifeTime(PX_Syntax* pSyntax, const px_char name[],px_int lifetime)
{
	px_int i;
	px_abi* pabi;
	for (i =0 ; i < pSyntax->reg_abi_stack.size; i++)
	{
		px_int* plifetime;
		pabi = PX_VECTORAT(px_abi, &pSyntax->reg_abi_stack, i);
		plifetime = PX_AbiGet_int(pabi, "lifetime");
		if (plifetime&&PX_Syntax_CheckAbiName(pabi, name)&& *plifetime== lifetime)
		{
			return i;
		}
	}
	return -1;

}

px_int PX_Syntax_GetAbiIndexFromForward(PX_Syntax* pSyntax, const px_char name[])
{
	px_int i;
	px_abi* pabi;
	for (i = 0; i < pSyntax->reg_abi_stack.size; i++)
	{
		pabi = PX_VECTORAT(px_abi, &pSyntax->reg_abi_stack, i);
		if (PX_Syntax_CheckAbiName(pabi, name))
		{
			return i;
		}
	}
	return -1;

}

px_int PX_Syntax_GetAbiIndexFromBackward_LifeTime(PX_Syntax* pSyntax, const px_char name[],px_int lifetime)
{
	px_int i;
	px_abi* pabi;
	for (i = pSyntax->reg_abi_stack.size - 1; i >= 0; i--)
	{
		px_int *plifetime;
		pabi = PX_VECTORAT(px_abi, &pSyntax->reg_abi_stack, i);
		plifetime = PX_AbiGet_int(pabi, "lifetime");
		if (plifetime&&*plifetime==lifetime&&PX_Syntax_CheckAbiName(pabi, name))
		{
			return i;
		}
	}
	return -1;

}

px_int PX_Syntax_GetAbiIndexFromBackward(PX_Syntax* pSyntax, const px_char name[])
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

px_int PX_Syntax_GetSecondAbiIndexFromBackward(PX_Syntax* pSyntax, const px_char name[])
{
	px_int i;
	px_int count = 0;
	px_abi* pabi;
	for (i = pSyntax->reg_abi_stack.size - 1; i >= 0; i--)
	{
		pabi = PX_VECTORAT(px_abi, &pSyntax->reg_abi_stack, i);
		if (PX_Syntax_CheckAbiName(pabi, name))
		{
			if (count == 1)
			{
				return i;
			}
			count++;
		}
	}
	return -1;
	
}

px_abi * PX_Syntax_GetAbiFromBackward(PX_Syntax* pSyntax, const px_char name[])
{
	px_int index = PX_Syntax_GetAbiIndexFromBackward(pSyntax, name);
	if (index != -1)
	{
		return PX_VECTORAT(px_abi, &pSyntax->reg_abi_stack, index);
	}
	return PX_NULL;
}

px_abi* PX_Syntax_FindSecondAbiFromBackward(PX_Syntax* pSyntax, const px_char name[])
{
	px_int index = PX_Syntax_GetSecondAbiIndexFromBackward(pSyntax, name);
	if (index != -1)
	{
		return PX_VECTORAT(px_abi, &pSyntax->reg_abi_stack, index);
	}
	return PX_NULL;
}

px_int PX_Syntax_GetLastOpcodeAbiIndex(PX_Syntax* pSyntax)
{
	px_int i;
	for (i = pSyntax->reg_abi_stack.size - 1; i >= 0; i--)
	{
		px_abi* pabi = PX_VECTORAT(px_abi, &pSyntax->reg_abi_stack, i);
		if (PX_Syntax_CheckAbiName(pabi, "opcode"))
		{
			return i;
		}
	}
	return -1;
	
}

px_int PX_Syntax_GetSecondLastOpcodeAbiIndex(PX_Syntax* pSyntax)
{
	px_int i;
	px_int count = 0;
	for (i = pSyntax->reg_abi_stack.size - 1; i >= 0; i--)
	{
		px_abi* pabi = PX_VECTORAT(px_abi, &pSyntax->reg_abi_stack, i);
		if (PX_Syntax_CheckAbiName(pabi, "opcode"))
		{
			if (count == 1)
			{
				return i;
			}
			count++;
		}
	}
	return -1;
}


px_abi* PX_Syntax_GetAbiFromForward(PX_Syntax* pSyntax, const px_char name[])
{
	px_int index = PX_Syntax_GetAbiIndexFromForward(pSyntax, name);
	if (index != -1)
	{
		return PX_VECTORAT(px_abi, &pSyntax->reg_abi_stack, index);
	}
	return PX_NULL;
}

const px_char* PX_Syntax_GetAbiValue(px_abi* pabi)
{
	const px_char *p=PX_AbiGet_string(pabi, "value");
	PX_ASSERTIFX(!p, "Error:Abi value not found");
	return p;
}

static px_bool PX_Syntax_IsTerminated(PX_Syntax* pSyntax)
{
	px_int i;
	for (i = 0; i < pSyntax->reg_ast_instr_stack.size; i++)
	{
		px_abi *popcode = PX_VECTORAT(px_abi, &pSyntax->reg_ast_instr_stack, i);
		const px_char* ptype = PX_AbiGetValue_string(popcode, "type");
		if (PX_strequ(ptype, "terminate"))
			return PX_TRUE;
	}
	return PX_FALSE;
}

static px_bool PX_Syntax_ExecuteCurrentNode(PX_Syntax* pSyntax)
{
	PX_Syntax_ast current_ast = { 0 };
	PX_Syntax_bnfnode* pbnfnode = PX_NULL;

	if (pSyntax->reg_ast_stack.size == 0)
	{
		if (PX_Syntax_IsEndOfSource(pSyntax))
		{
			return PX_TRUE;
		}
		//end of execute
		return PX_FALSE;
	}

	if (!PX_VectorLastTo(&pSyntax->reg_ast_stack, &current_ast))
	{
		return PX_FALSE;
	}

	pbnfnode = current_ast.pbnfnode;

	PX_SyntaxLexer_SetState(&pSyntax->reg_syntaxlexer, &current_ast.syntaxlexer_state);

	PX_ASSERTIFX(!pbnfnode, "Error:Current AST node is null");

	if (pbnfnode->type == PX_SYNTAX_AST_TYPE_LINKER && current_ast.pebnf_index!=-1)
	{
		PX_Syntax_pebnf* ppebnf = PX_Syntax_GetPebnfByIndex(pSyntax, current_ast.pebnf_index);
		if (ppebnf->penterfunction)
		{
			PX_Syntax_Message(pSyntax, "call sources_enter ");
			if (!ppebnf->penterfunction(pSyntax, &current_ast, ppebnf->userptr))
			{
				if (PX_Syntax_IsTerminated(pSyntax))
					return PX_TRUE;
				px_char message_content[128] = { 0 };
				PX_sprintf1(message_content, sizeof(message_content), "%1 unction return false", PX_STRINGFORMAT_STRING(ppebnf->mnenonic.buffer));
				PX_Syntax_Message(pSyntax, message_content);
				return PX_Syntax_ExecuteOthers(pSyntax, &current_ast);
			}
			if (PX_Syntax_IsTerminated(pSyntax))
				return PX_TRUE;
		}
		
	}

	if (pbnfnode->penterfunction)
	{
		if (!pbnfnode->penterfunction(pSyntax, &current_ast,pbnfnode->userptr))
		{
			if (PX_Syntax_IsTerminated(pSyntax))
				return PX_TRUE;
			PX_VectorPop(&pSyntax->reg_ast_stack);
			return PX_Syntax_ExecuteOthers(pSyntax, &current_ast);
		}
		if (PX_Syntax_IsTerminated(pSyntax))
			return PX_TRUE;
	}
	
	switch (pbnfnode->type)
	{
	case PX_SYNTAX_AST_TYPE_CONSTANT:
	{
		const px_char* pstr = pbnfnode->constant.buffer;
		PX_SYNTAXLEXER_LEXEME_TYPE type;
		const px_char* plexeme;
		if (pstr[1]=='\0'&&(pstr[0] == '\r'|| pstr[0] == '\n'))
		{
			type = PX_Syntax_GetNextLexeme3(pSyntax);
			plexeme = PX_Syntax_GetCurrentLexeme(pSyntax);
			PX_Syntax_Message(pSyntax, "AST:compare constant \"");
			if (PX_strequ(pstr, "\n"))
			{
				PX_Syntax_Message(pSyntax, "newline");
			}
			else if (PX_strequ(pstr, "\r"))
			{
				PX_Syntax_Message(pSyntax, "carriage_return");
			}
			else
			{
				PX_Syntax_Message(pSyntax, pstr);
			}
			PX_Syntax_Message(pSyntax, "\" with ");

			if(PX_strequ(plexeme,"\n"))
				PX_Syntax_Message(pSyntax, "newline");
			else
				PX_Syntax_Message(pSyntax, plexeme);

			PX_Syntax_Message(pSyntax, "\n");
		}
		else
		{
			type = PX_Syntax_GetNextLexeme(pSyntax);
			plexeme = PX_Syntax_GetCurrentLexeme(pSyntax);
			PX_Syntax_Message(pSyntax, "AST:compare constant \"");
			if (PX_strequ(pstr, "\n"))
			{
				PX_Syntax_Message(pSyntax, "newline");
			}
			else if (PX_strequ(pstr, "\r"))
			{
				PX_Syntax_Message(pSyntax, "CarriageReturn");
			}
			else
			{
				PX_Syntax_Message(pSyntax, pstr);
			}
			PX_Syntax_Message(pSyntax, "\" with \"");
			PX_Syntax_Message(pSyntax, plexeme);
			PX_Syntax_Message(pSyntax, "\"\n");
		}
		PX_VectorPop(&pSyntax->reg_ast_stack);
		if (PX_strequ(plexeme, pstr))
		{
			if (pstr[1] == '\0' && (pstr[0] == '\r' || pstr[0] == '\n'))
			{
				PX_Syntax_Message(pSyntax, "AST:const matched NewLine\n");
			}
			else
			{
				PX_Syntax_Message(pSyntax, "AST:const matched ");
				PX_Syntax_Message(pSyntax, current_ast.pbnfnode->constant.buffer);
				PX_Syntax_Message(pSyntax, "\n");
			}
			PX_Syntax_Message(pSyntax, "AST: ");
			PX_Syntax_Message(pSyntax, pbnfnode->constant.buffer);
			if (pbnfnode->pleavefunction)
			{
				if (pbnfnode->pleavefunction(pSyntax, &current_ast, pbnfnode->userptr))
				{
					if (PX_Syntax_IsTerminated(pSyntax))
						return PX_TRUE;
					PX_Syntax_Message(pSyntax, " const function return true,ast next\n");
					return PX_Syntax_ExecuteNextAst(pSyntax, &current_ast);
				}
				else
				{
					if (PX_Syntax_IsTerminated(pSyntax))
						return PX_TRUE;
					PX_Syntax_Message(pSyntax, " const function return false,try others\n");
				}
			}
			else
			{
				PX_Syntax_Message(pSyntax, "AST:const function is null,ast next\n");
				return PX_Syntax_ExecuteNextAst(pSyntax, &current_ast);
			}
		}
		return PX_Syntax_ExecuteOthers(pSyntax, &current_ast);
	}
	break;
	case PX_SYNTAX_AST_TYPE_FUNCTION:
	{
		PX_VectorPop(&pSyntax->reg_ast_stack);
		if (pbnfnode->pleavefunction)
		{
			if (pbnfnode->pleavefunction(pSyntax, &current_ast, pbnfnode->userptr))
			{
				if (PX_Syntax_IsTerminated(pSyntax))
					return PX_TRUE;
				PX_Syntax_Message(pSyntax, "AST:function return true,ast next\n");
				return PX_Syntax_ExecuteNextAst(pSyntax, &current_ast);
			}
			else
			{
				if (PX_Syntax_IsTerminated(pSyntax))
					return PX_TRUE;
				PX_Syntax_Message(pSyntax, "AST:function return false,try others\n");
			}
		}
		else
		{
			PX_Syntax_Message(pSyntax, "AST:function is null,ast next\n");
			return PX_Syntax_ExecuteNextAst(pSyntax, &current_ast);
		}
		return PX_Syntax_ExecuteOthers(pSyntax, &current_ast);
	}
	break;
	case PX_SYNTAX_AST_TYPE_CONTINUOUS:
	{
		px_char ch;
		PX_VectorPop(&pSyntax->reg_ast_stack);
		ch = PX_Syntax_PreviewNextChar(pSyntax);
		if (ch==' '||ch=='\t'||ch=='\0'|| ch == '\n'|| ch == '\r')
		{
			PX_Syntax_Message(pSyntax, "AST:continuous not matched.\n");
			return PX_Syntax_ExecuteOthers(pSyntax, &current_ast);
		}
		PX_Syntax_Message(pSyntax, "AST:continuous matched\n");
		
		return PX_Syntax_ExecuteNextAst(pSyntax, &current_ast);
	}
	break;
	case PX_SYNTAX_AST_TYPE_RECURSION:
	case PX_SYNTAX_AST_TYPE_LINKER:
	{
		//call linker next
		PX_Syntax_ast newast = {0};
		const px_char* pstr = pbnfnode->constant.buffer;
		PX_Syntax_pebnf* ppebnf = PX_Syntax_GetPebnf(pSyntax, pstr);
		if (!ppebnf)
		{
			PX_ASSERTX("Error:linker not found");
			return PX_FALSE;
		}
		PX_Syntax_Message(pSyntax, "AST:call linker ");
		PX_Syntax_Message(pSyntax, current_ast.pbnfnode->constant.buffer);
		PX_Syntax_Message(pSyntax, " abistack:");
		PX_Syntax_Message(pSyntax, PX_itos(pSyntax->reg_abi_stack.size,10).data);
		PX_Syntax_Message(pSyntax, "\n");
		newast.syntaxlexer_state = PX_SyntaxLexer_GetState(&pSyntax->reg_syntaxlexer);
		newast.pbnfnode = ppebnf->pbnfnode;
		PX_ASSERTIFX(newast.pbnfnode == PX_NULL, "Error:linker pbnfnode not found");
		newast.call_abistack_count = pSyntax->reg_abi_stack.size;
		if (ppebnf->pbnfnode && ppebnf->pbnfnode->type == PX_SYNTAX_AST_TYPE_LINKER)
		{
			newast.pebnf_index = PX_Syntax_GetPebnfIndexByMnemonic(pSyntax, ppebnf->pbnfnode->constant.buffer);
			PX_ASSERTIFX(newast.pebnf_index == -1, "Error:linker not found");
		}
		else
		{
			newast.pebnf_index = -1;
		}
		
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


PX_SYNTAX_AST_RETURN PX_Syntax_ExecuteNext(PX_Syntax* pSyntax)
{

	if (!PX_Syntax_ExecuteCurrentNode(pSyntax))
	{
		return PX_SYNTAX_AST_RETURN_ERROR;
	}
	while (pSyntax->reg_ast_instr_stack.size)
	{
		px_abi* pabi = PX_VECTORLAST(px_abi, &pSyntax->reg_ast_instr_stack);
		const px_char* ptype = PX_AbiGet_string(pabi, "type");
		if (ptype)
		{
			if (PX_strequ(ptype, "terminate"))
			{
				PX_Syntax_Message(pSyntax, "AST:Compiler Terminated.\n");
				return PX_SYNTAX_AST_RETURN_ERROR;
			}
			else if (PX_strequ(ptype, "pop"))
			{
				PX_Syntax_Message(pSyntax, "AST:pop\n");
				PX_VectorPop(&pSyntax->reg_ast_stack);
				PX_AbiFree(pabi);
				PX_VectorPop(&pSyntax->reg_ast_instr_stack);
			}
			else if (PX_strequ(ptype, "continue"))
			{
				PX_Syntax_Message(pSyntax, "AST:continue\n");
			}
			else if (PX_strequ(ptype, "return true") || PX_strequ(ptype, "return false"))
			{
				//return node
				PX_Syntax_ast current_ast = { 0 };

				if (pSyntax->reg_ast_stack.size == 0)
				{
					PX_AbiFree(pabi);
					PX_VectorPop(&pSyntax->reg_ast_instr_stack);
					//PX_VectorPopTo(&pSyntax->reg_ast_stack, &current_ast);
					if (PX_strequ(ptype, "return true"))
					{
						PX_Syntax_pebnf* ppebnf = PX_Syntax_GetPebnfByIndex(pSyntax, current_ast.pebnf_index);
						if (ppebnf&&ppebnf->pleavefunction)
						{
							if (!ppebnf->pleavefunction(pSyntax, &current_ast, ppebnf->userptr))
							{
								return PX_SYNTAX_AST_RETURN_ERROR;
							}
							if (PX_Syntax_IsTerminated(pSyntax))
								return PX_SYNTAX_AST_RETURN_ERROR;
						}
						return PX_SYNTAX_AST_RETURN_END;
					}
					else
					{
						return PX_SYNTAX_AST_RETURN_ERROR;
					}
				}
				PX_VectorPopTo(&pSyntax->reg_ast_stack, &current_ast);
				if (current_ast.pbnfnode == PX_NULL || (current_ast.pbnfnode->type != PX_SYNTAX_AST_TYPE_LINKER && current_ast.pbnfnode->type != PX_SYNTAX_AST_TYPE_RECURSION))
				{
					PX_ASSERTX("Error:Return node not found");
					return PX_SYNTAX_AST_RETURN_ERROR;
				}
				if (PX_strequ(ptype, "return true"))
				{
					PX_Syntax_pebnf* ppebnf = PX_Syntax_GetPebnfByIndex(pSyntax, current_ast.pebnf_index);
					PX_Syntax_Message(pSyntax, "AST:");
					PX_Syntax_Message(pSyntax, current_ast.pbnfnode->constant.buffer);
					PX_Syntax_Message(pSyntax, " return true\n");
					PX_AbiFree(pabi);
					PX_VectorPop(&pSyntax->reg_ast_instr_stack);
					if (ppebnf)
					{
						if (ppebnf->pleavefunction)
						{
							if (!ppebnf->pleavefunction(pSyntax, &current_ast, ppebnf->userptr))
							{
								if (PX_Syntax_IsTerminated(pSyntax))
									return PX_SYNTAX_AST_RETURN_ERROR;
								if (!PX_Syntax_ExecuteOthers(pSyntax, &current_ast))
								{
									return PX_SYNTAX_AST_RETURN_ERROR;
								}
								continue;
							}
						}

					}


					if (current_ast.pbnfnode->pleavefunction)
					{
						if (current_ast.pbnfnode->pleavefunction(pSyntax, &current_ast, current_ast.pbnfnode->userptr))
						{
							if (PX_Syntax_IsTerminated(pSyntax))
								return PX_SYNTAX_AST_RETURN_ERROR;
							PX_Syntax_Message(pSyntax, "AST:");
							PX_Syntax_Message(pSyntax, current_ast.pbnfnode->constant.buffer);
							if (current_ast.pbnfnode->type == PX_SYNTAX_AST_TYPE_LINKER)
								PX_Syntax_Message(pSyntax, " linker function return true,ast next\n");
							else
								PX_Syntax_Message(pSyntax, " recursion function return true,ast next\n");
							if (!PX_Syntax_ExecuteNextAst(pSyntax, &current_ast))
								return PX_SYNTAX_AST_RETURN_ERROR;
						}
						else
						{
							if (PX_Syntax_IsTerminated(pSyntax))
								return PX_SYNTAX_AST_RETURN_ERROR;
							PX_Syntax_Message(pSyntax, "AST:");
							PX_Syntax_Message(pSyntax, current_ast.pbnfnode->constant.buffer);
							if (current_ast.pbnfnode->type == PX_SYNTAX_AST_TYPE_LINKER)
								PX_Syntax_Message(pSyntax, " linker function return false,try others\n");
							else
								PX_Syntax_Message(pSyntax, " recursion function return false,try others\n");
							
							if (!PX_Syntax_ExecuteOthers(pSyntax, &current_ast))
							{
								return PX_SYNTAX_AST_RETURN_ERROR;
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

						if (!PX_Syntax_ExecuteNextAst(pSyntax, &current_ast))
							return PX_SYNTAX_AST_RETURN_ERROR;
					}
				}
				else
				{
					//false
					PX_AbiFree(pabi);
					PX_VectorPop(&pSyntax->reg_ast_instr_stack);
					if (!PX_Syntax_ExecuteOthers(pSyntax, &current_ast))
					{
						return PX_SYNTAX_AST_RETURN_ERROR;
					}
				}
			}
		}
	}
	return PX_SYNTAX_AST_RETURN_CONTINUE;
}

px_int PX_Syntax_GetCurrentSourceIndex(PX_Syntax* pSyntax)
{
	return PX_SyntaxLexer_GetCurrentSourceIndex(&pSyntax->reg_syntaxlexer); 
}

px_int PX_Syntax_GetCurrentRow(PX_Syntax* pSyntax)
{
	return PX_SyntaxLexer_GetCurrentLine(&pSyntax->reg_syntaxlexer);
}

px_int PX_Syntax_GetCurrentLine(PX_Syntax* pSyntax)
{
	return PX_Syntax_GetCurrentRow(pSyntax);
}

px_bool PX_Syntax_IsExecuting(PX_Syntax* pSyntax)
{
	return PX_SyntaxLexer_IsEnd(&pSyntax->reg_syntaxlexer) ? PX_FALSE : PX_TRUE;
}



px_bool PX_Syntax_CallSource(PX_Syntax* pSyntax, const px_char name[], const px_char pebnf[])
{
	if (!PX_SyntaxLexer_CallSourceByName(&pSyntax->reg_syntaxlexer,name))
		return PX_FALSE;

	if (pebnf && pebnf[0])
	{
		PX_Syntax_ast init_ast = { 0 };
		//push ast
		init_ast.syntaxlexer_state = PX_Syntax_GetLexerState(pSyntax);
		init_ast.call_abistack_count = pSyntax->reg_abi_stack.size;
		init_ast.pbnfnode = PX_Syntax_GetPebnfNode(pSyntax, pebnf);
		init_ast.pebnf_index = PX_Syntax_GetPebnfIndexByMnemonic(pSyntax, pebnf);


		PX_ASSERTIFX(!init_ast.pbnfnode, "Error:pebnf node not found");
		PX_ASSERTIFX(init_ast.pebnf_index==-1, "Error:pebnf not found");

		if (!PX_VectorPushback(&pSyntax->reg_ast_stack, &init_ast))
		{
			return PX_FALSE;
		}
	}
	return PX_TRUE;
}
px_bool PX_Syntax_CallSourceIndex(PX_Syntax* pSyntax, px_int index, const px_char pebnf[])
{ 
	if (!PX_SyntaxLexer_CallSourceByIndex(&pSyntax->reg_syntaxlexer, index))
		return PX_FALSE;

	if (pebnf && pebnf[0])
	{
		PX_Syntax_ast init_ast = { 0 };
		//push ast
		init_ast.syntaxlexer_state = PX_Syntax_GetLexerState(pSyntax);
		init_ast.call_abistack_count = pSyntax->reg_abi_stack.size;
		init_ast.pbnfnode = PX_Syntax_GetPebnfNode(pSyntax, pebnf);
		init_ast.pebnf_index = PX_Syntax_GetPebnfIndexByMnemonic(pSyntax, pebnf);


		PX_ASSERTIFX(!init_ast.pbnfnode, "Error:pebnf node not found");
		PX_ASSERTIFX(init_ast.pebnf_index==-1, "Error:pebnf not found");

		if (!PX_VectorPushback(&pSyntax->reg_ast_stack, &init_ast))
		{
			PX_TERMINATE("Out of memory");
			return PX_FALSE;
		}
	}
	return PX_TRUE;
}

px_bool PX_Syntax_Execute(PX_Syntax* pSyntax, const px_char filename[], const px_char pebnf[])
{
	PX_Syntax_ClearState(pSyntax);
	if (!PX_Syntax_CallSource(pSyntax, filename, pebnf))
		return PX_FALSE;
	pSyntax->execute_id++;
	if (!PX_Syntax_EnterScope(pSyntax))//scope 1--->global scope
	{
		PX_Syntax_Message(pSyntax, "Error:sources_enter scope failed.");
		return PX_FALSE;
	}
	if (!PX_Syntax_init_base_type(pSyntax))
		return PX_FALSE;
	while (PX_TRUE)
	{
		PX_SYNTAX_AST_RETURN ast_return;
		pSyntax->execute_id++;
		//PX_Syntax_Message(pSyntax, "AST:execute ");
		//PX_Syntax_Message(pSyntax, PX_itos(pSyntax->execute_id, 10).data);
		//PX_Syntax_Message(pSyntax, "\n");
		if (pSyntax->execute_id== pSyntax->break_execute_id)
		{
			PX_DEBUG_BREAK();
		}

		ast_return = PX_Syntax_ExecuteNext(pSyntax);
		if (ast_return == PX_SYNTAX_AST_RETURN_ERROR)
		{
			return PX_FALSE;
		}
		else if (ast_return == PX_SYNTAX_AST_RETURN_END)
		{
			PX_ASSERTIFX(pSyntax->reg_ast_stack.size !=0, "Error:ast stack size not match");
			//if (!PX_Syntax_LeaveScope(pSyntax))
			//{
			//	PX_Syntax_Message(pSyntax, "Error:leave scope failed.");
			//	return PX_FALSE;
			//}
			return PX_TRUE;
		}
		else if (ast_return == PX_SYNTAX_AST_RETURN_CONTINUE)
		{
			continue;
		}
		else
		{
			PX_ASSERT();
			return PX_FALSE;
		}
	}
	return PX_FALSE;
}


px_void PX_Syntax_Terminate(PX_Syntax* pSyntax, const px_char message[])
{
	px_char build_content[1024] = { 0 };
	//numeric too long
	PX_sprintf3(build_content,sizeof(build_content), "Terminate Info:%1:%2 %3 \n", \
		PX_STRINGFORMAT_STRING(PX_Syntax_GetCurrentLexerSourceName(pSyntax)), \
		PX_STRINGFORMAT_INT(PX_Syntax_GetCurrentLexerLine(pSyntax)), \
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
		PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_AstReturn out of memory");
	}
}

px_void PX_Syntax_Message(PX_Syntax* pSyntax, const px_char message[])
{
	if (!PX_StringAppend(&pSyntax->message, message))
	{
		PX_ASSERT();
		return;
	}
}

px_string* PX_Syntax_GetMessage(PX_Syntax* pSyntax)
{
	return &pSyntax->message;
}

px_string* PX_Syntax_GetAstMessage(PX_Syntax* pSyntax)
{
	return &pSyntax->message;
}

