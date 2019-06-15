#include "PX_Script_ASMCompiler.h"

static px_char PX_Script_ASMError[128];

static px_void PX_ScriptAsmError(px_lexer *lexer,px_char *err)
{
	lexer->Sources[lexer->SourceOffset]='\0';

	PX_Script_ASMError[0]='\0';
	if (lexer->SourceOffset>96)
	{
		px_strcpy(PX_Script_ASMError,lexer->Sources+lexer->SourceOffset-96,96);	
	}
	else
	{
		px_strcpy(PX_Script_ASMError,lexer->Sources,96);	
	}

	PX_LOG(err);
}

px_int PX_ScriptAsm_atoi(px_char *str)
{
	if (str[0]=='0'&&str[1]=='X')
	{
		return PX_htoi(str);
	}
	return PX_atoi(str);
}

px_bool PX_ScriptAsm_isInt(px_char token[])
{
	px_int i;
	if (token[0]=='0'&&(token[1]=='X'))
	{
		i=2;
		while (token[i])
		{
			if ((token[i]>='0'&&token[i]<='9')||(token[i]>='A'&&token[i]<='F'))
			{
				i++;
				continue;
			}
			else
			{
				return PX_FALSE;
			}

			if (i>10)
			{
				return PX_FALSE;
			}
		}
		return PX_TRUE;
	}
	return px_strIsInt(token);
}

static px_void PX_ScriptAsmRegistInstr(PX_SCRIPT_ASM_COMPILER *compiler,px_char mnemonic[__PX_SCRIPT_ASM_INSTR_MNEMONIC_NAME_LEN],px_char opcode,px_char paramcout,...)
{
	px_int i=0;
	PX_SCRIPT_ASM_GRAMMAR_INSTR instr;
	_px_va_list ap;
	px_strcpy(instr.mnemonic,mnemonic,sizeof(instr.mnemonic));
	instr.opcode=opcode;
	instr.paramCount=paramcout;
	_px_va_start(ap, paramcout);
	for (i=0;i<paramcout;i++)
	{
		instr.operandAccTypes[i]=_px_va_arg(ap,px_int);
	}
	_px_va_end(ap);
	PX_ListPush(&compiler->GrammarInstrTable,&instr,sizeof(instr));
}
static px_void PX_ScriptAsmStringConvert(px_string *str)
{
	px_int i,j;
	for (i=0;i<px_strlen(str->buffer);i++)
	{
		if (str->buffer[i]=='\\')
		{
			switch (str->buffer[i+1])
			{
			case '\\':
				for(j=i+1;str->buffer[j];j++)
					str->buffer[j]=str->buffer[j+1];
				str->buffer[j-1]=0;
				break;
			case 'r':
			case 'R':
				str->buffer[i]='\r';
				for(j=i+1;str->buffer[j];j++)
					str->buffer[j]=str->buffer[j+1];
				str->buffer[j-1]=0;
				break;
			case 'n':
			case 'N':
				str->buffer[i]='\n';
				for(j=i+1;str->buffer[j];j++)
					str->buffer[j]=str->buffer[j+1];
				str->buffer[j-1]=0;
				break;
			case 't':
			case 'T':
				str->buffer[i]='\t';
				for(j=i+1;str->buffer[j];j++)
					str->buffer[j]=str->buffer[j+1];
				str->buffer[j-1]=0;
				break;
			case '"':
				str->buffer[i]='"';
				for(j=i+1;str->buffer[j];j++)
					str->buffer[j]=str->buffer[j+1];
				str->buffer[j-1]=0;
				break;
			default:
				break;
			}
		}
	}
}


PX_LEXER_LEXEME_TYPE PX_ScriptAsmNexLexeme(px_lexer *lexer)
{
	PX_LEXER_LEXEME_TYPE type;
	while (PX_LEXER_LEXEME_TYPE_SPACER==(type=PX_LexerGetNextLexeme(lexer)));
	if (px_strlen(lexer->CurLexeme.buffer)>__PX_SCRIPT_ASM_MNEMONIC_NAME_LEN-1)
	{
		if(type!=PX_LEXER_LEXEME_TYPE_CONATINER)
		{
			PX_ScriptAsmError(lexer,"Script ASM Compiler:Token too long error");
		    return PX_LEXER_LEXEME_TYPE_ERR;
		}
	}
	return type;

}

px_bool PX_ScriptAsmIsValidToken(px_char *token)
{
	if (token[0]=='\0'||px_charIsNumeric(token[0]))
	{
		return PX_FALSE;
	}
	while (*token)
	{
		if ((*token>='A'&&*token<='Z')||*token=='_'||px_charIsNumeric(*token))
		{
			token++;
			continue;
		}
		return PX_FALSE;
	}
	return PX_TRUE;
}

px_int PX_ScriptAsmGetGlobalIndex(px_char * mne)
{
	px_char data[__PX_SCRIPT_ASM_MNEMONIC_NAME_LEN];
	px_strcpy(data,mne,sizeof(data));
	data[px_strlen(data)-1]='\0';
	return PX_ScriptAsm_atoi(data+sizeof("GLOBAL["));
}

px_int PX_ScriptAsmGetStackIndex(px_char * mne)
{
	px_char data[__PX_SCRIPT_ASM_MNEMONIC_NAME_LEN];
	px_strcpy(data,mne,sizeof(data));
	data[px_strlen(data)-1]='\0';
	return PX_ScriptAsm_atoi(data+sizeof("STACK["));
}

px_int PX_ScriptAsmTokenType(PX_SCRIPT_ASM_COMPILER *compiler,px_char *token)
{
	px_bool bfound;
	px_int matchIteration=256,i;
	while (--matchIteration)
	{
		bfound=PX_FALSE;
		for (i=0;i<compiler->assumeTable.size;i++)
		{
			if (px_strequ(((PX_SCRIPT_ASM_ASSUME_NODE *)(PX_ListAt(&compiler->assumeTable,i)->pdata))->name.buffer,token))
			{
				token=((PX_SCRIPT_ASM_ASSUME_NODE *)(PX_ListAt(&compiler->assumeTable,i)->pdata))->assume.buffer;
				bfound=PX_TRUE;
				break;
			}
		}
		if (bfound==PX_FALSE)
		{
			break;
		}
	}
	if (matchIteration==0)
	{
		PX_ScriptAsmError(&compiler->lexer,"ASSUME too much iteration");
		return PX_SCRIPT_ASM_OPERAND_ACC_TYPE_UNKNOW;
	}
	if (px_strequ(token,"SP"))
	{
		return PX_SCRIPT_ASM_OPERAND_ACC_TYPE_SP;
	}

	if (px_strequ(token,"IP"))
	{
		return PX_SCRIPT_ASM_OPERAND_ACC_TYPE_IP;
	}

	if (px_strequ(token,"BP"))
	{
		return PX_SCRIPT_ASM_OPERAND_ACC_TYPE_BP;
	}

	if (px_memequ(token,"GLOBAL[",sizeof("GLOBAL[")-1)&&token[px_strlen(token)-1]==']')
	{
		return PX_SCRIPT_ASM_OPERAND_ACC_TYPE_GLOBAL;
	}

	if (px_memequ(token,"LOCAL[",sizeof("LOCAL[")-1)&&token[px_strlen(token)-1]==']')
	{
		return PX_SCRIPT_ASM_OPERAND_ACC_TYPE_LOCAL;
	}

	if (token[0]=='R'&&px_strIsInt(token+1))
	{
		return PX_SCRIPT_ASM_OPERAND_ACC_TYPE_REG;
	}

	if (token[0]=='$'&&PX_ScriptAsmIsValidToken(token+1))
	{
		return PX_SCRIPT_ASM_OPERAND_ACC_TYPE_HOST;
	}

	if (token[0]=='\"'&&token[px_strlen(token)-1]=='\"')
	{
		return PX_SCRIPT_ASM_OPERAND_ACC_TYPE_STRING;
	}

	if (token[0]=='\''&&token[px_strlen(token)-1]=='\'')
	{
		return PX_SCRIPT_ASM_OPERAND_ACC_TYPE_INT;
	}

	if (token[0]=='@'&&token[px_strlen(token)-1]=='@')
	{
		return PX_SCRIPT_ASM_OPERAND_ACC_TYPE_MEMORY;
	}



	if (PX_ScriptAsm_isInt(token))
	{
		return PX_SCRIPT_ASM_OPERAND_ACC_TYPE_INT;
	}

	if (px_strIsFloat(token))
	{
		return PX_SCRIPT_ASM_OPERAND_ACC_TYPE_FLOAT;
	}

	for (i=0;i<compiler->LabelTable.size;i++)
	{
		if (px_strequ(((PX_SCRIPT_ASM_LABEL_NODE *)(PX_ListAt(&compiler->LabelTable,i)->pdata))->mnemonic,token))
		{
			return PX_SCRIPT_ASM_OPERAND_ACC_TYPE_LABEL;
		}
	}

	return PX_SCRIPT_ASM_OPERAND_ACC_TYPE_UNKNOW;
}


px_char PX_ScriptAsmTokenTypeToOpType(px_int TokenType)
{
	switch(TokenType)
	{
	case PX_SCRIPT_ASM_OPERAND_ACC_TYPE_INT:
		return PX_SCRIPT_ASM_OPTYPE_INT;
	case PX_SCRIPT_ASM_OPERAND_ACC_TYPE_FLOAT:
		return PX_SCRIPT_ASM_OPTYPE_FLOAT;
	case PX_SCRIPT_ASM_OPERAND_ACC_TYPE_REG:
		return PX_SCRIPT_ASM_OPTYPE_REG;
	case PX_SCRIPT_ASM_OPERAND_ACC_TYPE_LOCAL:
		{
			return PX_SCRIPT_ASM_OPTYPE_LOCAL;
		}
		break;
	case PX_SCRIPT_ASM_OPERAND_ACC_TYPE_GLOBAL:
		{
			return PX_SCRIPT_ASM_OPTYPE_GLOBAL;
		}
		break;
	case PX_SCRIPT_ASM_OPERAND_ACC_TYPE_STRING:
		return PX_SCRIPT_ASM_OPTYPE_STRING;
	case PX_SCRIPT_ASM_OPERAND_ACC_TYPE_LABEL:
		return PX_SCRIPT_ASM_OPTYPE_LABEL;
	case PX_SCRIPT_ASM_OPERAND_ACC_TYPE_HOST:
		return PX_SCRIPT_ASM_OPTYPE_HOST;
	case PX_SCRIPT_ASM_OPERAND_ACC_TYPE_MEMORY:
		return PX_SCRIPT_ASM_OPTYPE_MEMORY;
	case PX_SCRIPT_ASM_OPERAND_ACC_TYPE_BP:
		return PX_SCRIPT_ASM_OPTYPE_BP;
	case PX_SCRIPT_ASM_OPERAND_ACC_TYPE_SP:
		return PX_SCRIPT_ASM_OPTYPE_SP;
	case PX_SCRIPT_ASM_OPERAND_ACC_TYPE_IP:
		return PX_SCRIPT_ASM_OPTYPE_IP;
	default:
		return 0;
	}
}

static px_bool PX_ScriptASMMapAssume(PX_SCRIPT_ASM_COMPILER *compiler,px_string *mnenonic)
{
	px_bool bfound;
	px_int matchIteration=256,i;

	while (--matchIteration)
	{
		bfound=PX_FALSE;
		for (i=0;i<compiler->assumeTable.size;i++)
		{
			if (px_strequ(((PX_SCRIPT_ASM_ASSUME_NODE *)(PX_ListAt(&compiler->assumeTable,i)->pdata))->name.buffer,mnenonic->buffer))
			{
			
				PX_StringCopy(mnenonic,&(((PX_SCRIPT_ASM_ASSUME_NODE *)PX_ListAt(&compiler->assumeTable,i)->pdata)->assume));

				bfound=PX_TRUE;
				break;
			}
		}
		if (bfound==PX_FALSE)
		{
			break;
		}
	}
	if (matchIteration==0)
	{
		PX_ScriptAsmError(&compiler->lexer,"ASSUME too much iteration");
		return PX_FALSE;
	}
	return PX_TRUE;
}

static px_bool PX_ScriptAsmLocalType(PX_SCRIPT_ASM_COMPILER *compiler,px_char *Token,px_char *type,px_dword *index)
{
	px_string TempTokenString;
	PX_StringInit(compiler->mp,&TempTokenString);
	PX_StringCat(&TempTokenString,Token);

	PX_StringTrimLeft(&TempTokenString,px_strlen("LOCAL["));
	PX_StringTrimRight(&TempTokenString,1);
	
	PX_ScriptASMMapAssume(compiler,&TempTokenString);

	if (px_memequ(TempTokenString.buffer,"GLOBAL[",sizeof("GLOBAL[")-1)&&TempTokenString.buffer[px_strlen(TempTokenString.buffer)-1]==']')
	{
		PX_StringTrimLeft(&TempTokenString,px_strlen("GLOBAL["));
		PX_StringTrimRight(&TempTokenString,1);
		PX_ScriptASMMapAssume(compiler,&TempTokenString);
		if (!PX_ScriptAsm_isInt(TempTokenString.buffer))
		{
			PX_StringFree(&TempTokenString);
			return PX_FALSE;
		}
		*type=PX_SCRIPT_ASM_OPTYPE_LOCAL_GLOBALREF;
		*index=PX_ScriptAsm_atoi(TempTokenString.buffer);
		PX_StringFree(&TempTokenString);
		return PX_TRUE;
	}

	if (px_memequ(TempTokenString.buffer,"LOCAL[",sizeof("LOCAL[")-1)&&TempTokenString.buffer[px_strlen(TempTokenString.buffer)-1]==']')
	{
		PX_StringTrimLeft(&TempTokenString,px_strlen("LOCAL["));
		PX_StringTrimRight(&TempTokenString,1);
		PX_ScriptASMMapAssume(compiler,&TempTokenString);

		if (!PX_ScriptAsm_isInt(TempTokenString.buffer))
		{
			PX_StringFree(&TempTokenString);
			return PX_FALSE;
		}
		*type=PX_SCRIPT_ASM_OPTYPE_LOCAL_LOCALREF;
		*index=PX_ScriptAsm_atoi(TempTokenString.buffer);
		PX_StringFree(&TempTokenString);
		return PX_TRUE;
	}

	if (TempTokenString.buffer[0]=='R'&&px_strIsInt(TempTokenString.buffer+1))
	{
		*type=PX_SCRIPT_ASM_OPTYPE_LOCAL_REGREF;
		*index=PX_ScriptAsm_atoi(TempTokenString.buffer+1);
		PX_StringFree(&TempTokenString);
		return PX_TRUE;
	}

	if (PX_ScriptAsm_isInt(TempTokenString.buffer))
	{
		*type=PX_SCRIPT_ASM_OPTYPE_LOCAL_CONST;
		*index=PX_ScriptAsm_atoi(TempTokenString.buffer);
		PX_StringFree(&TempTokenString);
		return PX_TRUE;
	}
	PX_StringFree(&TempTokenString);
	return PX_FALSE;
}

static px_bool PX_ScriptAsmGlobalType(PX_SCRIPT_ASM_COMPILER *compiler,px_char *Token,px_char *type,px_dword *index)
{
	px_string TempTokenString;
	PX_StringInit(compiler->mp,&TempTokenString);

	PX_StringCat(&TempTokenString,Token);

	PX_StringTrimLeft(&TempTokenString,px_strlen("GLOBAL["));
	PX_StringTrimRight(&TempTokenString,1);

	PX_ScriptASMMapAssume(compiler,&TempTokenString);

	if (px_memequ(TempTokenString.buffer,"GLOBAL[",sizeof("GLOBAL[")-1)&&TempTokenString.buffer[px_strlen(TempTokenString.buffer)-1]==']')
	{
		PX_StringTrimLeft(&TempTokenString,px_strlen("GLOBAL["));
		PX_StringTrimRight(&TempTokenString,1);
		PX_ScriptASMMapAssume(compiler,&TempTokenString);
		if (!PX_ScriptAsm_isInt(TempTokenString.buffer))
		{
			PX_StringFree(&TempTokenString);
			return PX_FALSE;
		}

		*type=PX_SCRIPT_ASM_OPTYPE_GLOBAL_GLOBALREF;
		*index=PX_ScriptAsm_atoi(TempTokenString.buffer);
		PX_StringFree(&TempTokenString);
		return PX_TRUE;
	}

	if (px_memequ(TempTokenString.buffer,"LOCAL[",sizeof("LOCAL[")-1)&&TempTokenString.buffer[px_strlen(TempTokenString.buffer)-1]==']')
	{
		PX_StringTrimLeft(&TempTokenString,px_strlen("LOCAL["));
		PX_StringTrimRight(&TempTokenString,1);
		PX_ScriptASMMapAssume(compiler,&TempTokenString);
		if (!PX_ScriptAsm_isInt(TempTokenString.buffer))
		{
			PX_StringFree(&TempTokenString);
			return PX_FALSE;
		}
		*type=PX_SCRIPT_ASM_OPTYPE_GLOBAL_LOCALREF;
		*index=PX_ScriptAsm_atoi(TempTokenString.buffer);
		PX_StringFree(&TempTokenString);
		return PX_TRUE;
	}

	if (px_memequ(TempTokenString.buffer,"BP+",sizeof("BP+")-1)&&PX_ScriptAsm_isInt(TempTokenString.buffer+sizeof("BP+")-1))
	{
		PX_StringTrimLeft(&TempTokenString,px_strlen("BP+"));
		
		PX_ScriptASMMapAssume(compiler,&TempTokenString);
		if (!PX_ScriptAsm_isInt(TempTokenString.buffer))
		{
			PX_StringFree(&TempTokenString);
			return PX_FALSE;
		}
		*type=PX_SCRIPT_ASM_OPTYPE_GLOBAL_LOCALREF;
		*index=PX_ScriptAsm_atoi(TempTokenString.buffer);
		PX_StringFree(&TempTokenString);
		return PX_TRUE;
	}
	else if (px_strequ(TempTokenString.buffer,"BP"))
	{
		*type=PX_SCRIPT_ASM_OPTYPE_GLOBAL_LOCALREF;
		*index=0;
		PX_StringFree(&TempTokenString);
		return PX_TRUE;
	}


	if (px_memequ(TempTokenString.buffer,"SP+",sizeof("SP+")-1)&&PX_ScriptAsm_isInt(TempTokenString.buffer+sizeof("SP+")-1))
	{
		PX_StringTrimLeft(&TempTokenString,px_strlen("SP+"));
		PX_ScriptASMMapAssume(compiler,&TempTokenString);
		if (!PX_ScriptAsm_isInt(TempTokenString.buffer))
		{
			PX_StringFree(&TempTokenString);
			return PX_FALSE;
		}
		*type=PX_SCRIPT_ASM_OPTYPE_GLOBAL_SPREF;
		*index=PX_ScriptAsm_atoi(TempTokenString.buffer);
		PX_StringFree(&TempTokenString);
		return PX_TRUE;
	}
	else if (px_strequ(TempTokenString.buffer,"SP"))
	{
		*type=PX_SCRIPT_ASM_OPTYPE_GLOBAL_SPREF;
		*index=0;
		PX_StringFree(&TempTokenString);
		return PX_TRUE;
	}


	if (TempTokenString.buffer[0]=='R'&&px_strIsInt(TempTokenString.buffer+1))
	{
		*type=PX_SCRIPT_ASM_OPTYPE_GLOBAL_REGREF;
		*index=PX_ScriptAsm_atoi(TempTokenString.buffer+1);
		PX_StringFree(&TempTokenString);
		return PX_TRUE;
	}


	if (PX_ScriptAsm_isInt(TempTokenString.buffer))
	{
		*type=PX_SCRIPT_ASM_OPTYPE_GLOBAL_CONST;
		*index=PX_ScriptAsm_atoi(TempTokenString.buffer);
		PX_StringFree(&TempTokenString);
		return PX_TRUE;
	}
	PX_StringFree(&TempTokenString);
	return PX_FALSE;
}


px_void PX_ScriptAsmUpdateAssumeTable(PX_SCRIPT_ASM_COMPILER *compiler,px_char *name,px_char *assume)
{
	px_int i;
	PX_SCRIPT_ASM_ASSUME_NODE newNode;
	for (i=0;i<compiler->assumeTable.size;i++)
	{
		if (px_strequ(((PX_SCRIPT_ASM_ASSUME_NODE *)(PX_ListAt(&compiler->assumeTable,i)->pdata))->name.buffer,name))
		{
			PX_StringClear(&PX_LISTAT(PX_SCRIPT_ASM_ASSUME_NODE,&compiler->assumeTable,i)->assume);
			PX_StringCat(&PX_LISTAT(PX_SCRIPT_ASM_ASSUME_NODE,&compiler->assumeTable,i)->assume,assume);
			return;
		}
	}
	PX_StringInit(compiler->mp,&newNode.assume);
	PX_StringInit(compiler->mp,&newNode.name);
	PX_StringCat(&newNode.assume,assume);
	PX_StringCat(&newNode.name,name);

	PX_ListPush(&compiler->assumeTable,&newNode,sizeof(PX_SCRIPT_ASM_ASSUME_NODE));
}

px_bool PX_ScriptAsmAddSourceLabel(PX_SCRIPT_ASM_COMPILER *compiler,px_char *Name,px_bool bFunction)
{
	px_int i;
	PX_SCRIPT_ASM_LABEL_NODE label;
	for (i=0;i<compiler->LabelTable.size;i++)
	{
		if (px_strequ(PX_LISTAT(PX_SCRIPT_ASM_LABEL_NODE,&compiler->LabelTable,i)->mnemonic,Name))
		{
			PX_ScriptAsmError(&compiler->lexer,"Label name redefined.");
			return PX_FALSE;
		}
	}
	px_strcpy(label.mnemonic,Name,sizeof(label.mnemonic));
	label.bfunction=bFunction;
	PX_ListPush(&compiler->LabelTable,&label,sizeof(label));
	return PX_TRUE;
}

px_bool PX_ScriptAsmUpdateLabelAddr(PX_SCRIPT_ASM_COMPILER *compiler,px_char *Name,px_int addr)
{
	px_int i;
	for (i=0;i<compiler->LabelTable.size;i++)
	{
		if (px_strequ(((PX_SCRIPT_ASM_LABEL_NODE *)(PX_ListAt(&compiler->LabelTable,i)->pdata))->mnemonic,Name))
		{
			((PX_SCRIPT_ASM_LABEL_NODE *)(PX_ListAt(&compiler->LabelTable,i)->pdata))->binaddr=addr;
			return PX_TRUE;
		}
	}
	return PX_FALSE;
}

px_int PX_ScriptAsmGetLabelIndex(PX_SCRIPT_ASM_COMPILER *compiler,px_char *Name)
{
	px_int i;
	for (i=0;i<compiler->LabelTable.size;i++)
	{
		if (px_strequ(PX_LISTAT(PX_SCRIPT_ASM_LABEL_NODE,&compiler->LabelTable,i)->mnemonic,Name))
		{
			return i;
		}
	}
	return -1;
}

px_int PX_ScriptAsmGetFuncCount(PX_SCRIPT_ASM_COMPILER *compiler)
{
	px_int i;
	px_int count=0;
	for (i=0;i<compiler->LabelTable.size;i++)
	{
		if (PX_LISTAT(PX_SCRIPT_ASM_LABEL_NODE,&compiler->LabelTable,i)->bfunction)
		{
			count++;
		}
	}
	return count;
}

px_int PX_ScriptAsmStringSize(PX_SCRIPT_ASM_COMPILER *compiler)
{
	px_int i;
	px_int size=0;
	for (i=0;i<compiler->StringTable.size;i++)
	{
		size+= (px_strlen(((PX_SCRIPT_ASM_STRING_NODE *)(PX_ListAt(&compiler->StringTable,i)->pdata))->str.buffer))+1;
	}
	return size;
}

px_int PX_ScriptAsmMemorySize(PX_SCRIPT_ASM_COMPILER *compiler)
{
	px_int i;
	px_int size=0;
	for (i=0;i<compiler->memoriesTable.size;i++)
	{
		size+=PX_LISTAT(PX_SCRIPT_ASM_MEMORY_NODE,&compiler->memoriesTable,i)->mem.usedsize+4;
	}
	return size;
}

px_void PX_ScriptAsmAddStringConst(PX_SCRIPT_ASM_COMPILER *compiler,px_char *Str)
{
	px_int i;
	PX_SCRIPT_ASM_STRING_NODE stringNode,*pLastNode;
	for (i=0;i<compiler->StringTable.size;i++)
	{
		if (px_strequ(((PX_SCRIPT_ASM_STRING_NODE *)(PX_ListAt(&compiler->StringTable,i)->pdata))->str.buffer,Str))
		{
			return;
		}
	}
	PX_StringInit(compiler->mp,&stringNode.str);
	if(compiler->StringTable.size!=0)
	{
		pLastNode=(PX_SCRIPT_ASM_STRING_NODE *)(PX_ListAt(&compiler->StringTable,compiler->StringTable.size-1)->pdata);
		stringNode.addr=pLastNode->addr+px_strlen(pLastNode->str.buffer)+1;
	}
	else
	stringNode.addr=0;
	PX_StringCat(&stringNode.str,Str);
	PX_ListPush(&compiler->StringTable,&stringNode,sizeof(stringNode));
}

px_bool PX_ScriptAsmAddMemoryConst(PX_SCRIPT_ASM_COMPILER *compiler,px_char *StringMap)
{
	px_char hex[3];
	px_char _byte;
	PX_SCRIPT_ASM_MEMORY_NODE memoryNode,*pLastNode;
	if (px_strlen(StringMap)==0)
	{
		return PX_FALSE;
	}
	if (px_strlen(StringMap)&1)
	{
		PX_ScriptAsmError(&compiler->lexer,"Invalid binary stream map");
		return PX_FALSE;
	}

	PX_MemoryInit(compiler->mp,&memoryNode.mem);
	
	if(!PX_MemoryAlloc(&memoryNode.mem,px_strlen(StringMap)>>1))
		return PX_FALSE;

	while (*StringMap)
	{
		hex[0]=*StringMap;
		hex[1]=*(StringMap+1);
		hex[2]=0;

		_byte=(px_char)PX_htoi(hex);
		PX_MemoryCat(&memoryNode.mem,&_byte,1);
		StringMap+=2;
	}

	if(compiler->memoriesTable.size!=0)
	{
		pLastNode=PX_LISTAT(PX_SCRIPT_ASM_MEMORY_NODE,&compiler->memoriesTable,compiler->memoriesTable.size-1);
		memoryNode.addr=pLastNode->addr+pLastNode->mem.usedsize+sizeof(px_dword);
	}
	else
		memoryNode.addr=0;

	PX_ListPush(&compiler->memoriesTable,&memoryNode,sizeof(memoryNode));
	return PX_TRUE;
}

px_dword PX_ScriptAsmGetStringAddr(PX_SCRIPT_ASM_COMPILER *compiler,px_char *Str)
{
	px_int i;
	for (i=0;i<compiler->StringTable.size;i++)
	{
		if (px_strequ(((PX_SCRIPT_ASM_STRING_NODE *)(PX_ListAt(&compiler->StringTable,i)->pdata))->str.buffer,Str))
		{
			return ((PX_SCRIPT_ASM_STRING_NODE *)(PX_ListAt(&compiler->StringTable,i)->pdata))->addr;
		}
	}
	return 0;
}

px_dword PX_ScriptAsmGetMemoryAddr(PX_SCRIPT_ASM_COMPILER *compiler,px_char *StringMap)
{
	px_int i;
	px_char hex[3];
	px_char _byte;
	PX_SCRIPT_ASM_MEMORY_NODE memoryNode;

	if (px_strlen(StringMap)&1)
	{
		PX_ScriptAsmError(&compiler->lexer,"Invalid binary stream map");
		return PX_FALSE;
	}

	PX_MemoryInit(compiler->mp,&memoryNode.mem);
	PX_MemoryAlloc(&memoryNode.mem,px_strlen(StringMap)>>1);
	while (*StringMap)
	{
		hex[0]=*StringMap;
		hex[1]=*(StringMap+1);
		hex[2]=0;

		_byte=(px_char)PX_htoi(hex);
		PX_MemoryCat(&memoryNode.mem,&_byte,1);
		StringMap+=2;
	}


	for (i=0;i<compiler->memoriesTable.size;i++)
	{
		if (PX_LISTAT(PX_SCRIPT_ASM_MEMORY_NODE,&compiler->memoriesTable,i)->mem.buffer==PX_NULL)
		{
			continue;
		}

		if (px_memequ(PX_LISTAT(PX_SCRIPT_ASM_MEMORY_NODE,&compiler->memoriesTable,i)->mem.buffer,memoryNode.mem.buffer,memoryNode.mem.usedsize))
		{
			PX_MemoryFree(&memoryNode.mem);
			return PX_LISTAT(PX_SCRIPT_ASM_MEMORY_NODE,&compiler->memoriesTable,i)->addr;
		}
	}
	PX_MemoryFree(&memoryNode.mem);
	return 0;
}

px_void PX_ScriptAsmAddHost(PX_SCRIPT_ASM_COMPILER *compiler,px_char *Str)
{
	px_int i;
	PX_SCRIPT_ASM_HOST_NODE Host;
	for (i=0;i<compiler->HostTable.size;i++)
	{
		if (px_strequ(((PX_SCRIPT_ASM_HOST_NODE *)(PX_ListAt(&compiler->HostTable,i)->pdata))->name,Str))
		{
			return;
		}
	}
	px_strcpy(Host.name,Str,sizeof(Host.name));
	Host.map=PX_NULL;
	PX_ListPush(&compiler->HostTable,&Host,sizeof(Host));
}

px_int PX_ScriptAsmGetHostIndex(PX_SCRIPT_ASM_COMPILER *compiler,px_char *Str)
{
	px_int i;
	for (i=0;i<compiler->HostTable.size;i++)
	{
		if (px_strequ(((PX_SCRIPT_ASM_HOST_NODE *)(PX_ListAt(&compiler->HostTable,i)->pdata))->name,Str))
		{
			return i;
		}
	}
	return -1;
}
px_bool PX_ScriptAsmScan(PX_SCRIPT_ASM_COMPILER *compiler)
{
	PX_LEXER_STATE state=PX_LexerGetState(&compiler->lexer);
	PX_LEXER_LEXEME_TYPE type;
	PX_SCRIPT_ASM_GRAMMAR_INSTR *pinstr;
	px_string assumeString,assumeName;
	px_string includeStr;
	px_int i,j;
	px_bool hasMatched;
	px_char *pLexeme;

	PX_StringInit(compiler->mp,&assumeName);
	PX_StringInit(compiler->mp,&assumeString);

	while ((type=PX_ScriptAsmNexLexeme (&compiler->lexer))!=PX_LEXER_LEXEME_TYPE_END)
	{
		pLexeme=compiler->lexer.CurLexeme.buffer;
		switch(type)
		{
		case PX_LEXER_LEXEME_TYPE_SPACER:
		case PX_LEXER_LEXEME_TYPE_NEWLINE:
			continue;
			break;
		case PX_LEXER_LEXEME_TYPE_TOKEN:
			{
				//special 

				//ASSUME
				pLexeme=compiler->lexer.CurLexeme.buffer;
				if (px_strequ("ASSUME",pLexeme))
				{
					if ((type=PX_ScriptAsmNexLexeme (&compiler->lexer))!=PX_LEXER_LEXEME_TYPE_TOKEN)
					{
						PX_ScriptAsmError(&compiler->lexer,"ASSUME first token should be token!");
						goto _ERROR;
					}
					pLexeme=compiler->lexer.CurLexeme.buffer;
					if (px_strIsNumeric(pLexeme))
					{
						PX_ScriptAsmError(&compiler->lexer,"ASSUME first token should not be numeric!");
						goto _ERROR;
					}

					PX_StringClear(&assumeName);
					PX_StringCat(&assumeName,pLexeme);
					
					if ((type=PX_ScriptAsmNexLexeme (&compiler->lexer))!=PX_LEXER_LEXEME_TYPE_TOKEN)
					{
						PX_ScriptAsmError(&compiler->lexer,"ASSUME second token should be token!");
						goto _ERROR;
					}
					pLexeme=compiler->lexer.CurLexeme.buffer;
					PX_StringClear(&assumeString);
					PX_StringCat(&assumeString,pLexeme);

					if (px_strequ(assumeString.buffer,assumeName.buffer))
					{
						PX_ScriptAsmError(&compiler->lexer,"ASSUME token should not equal.");
						goto _ERROR;
					}

					if ((type=PX_ScriptAsmNexLexeme (&compiler->lexer))!=PX_LEXER_LEXEME_TYPE_NEWLINE&&type!=PX_LEXER_LEXEME_TYPE_END)
					{
						PX_ScriptAsmError(&compiler->lexer,"New line is expected but not found.");
						goto _ERROR;
					}
					pLexeme=compiler->lexer.CurLexeme.buffer;
					PX_ScriptAsmUpdateAssumeTable(compiler,assumeName.buffer,assumeString.buffer);
					continue;
				}

				//Function or Lable
				if (pLexeme[px_strlen(pLexeme)-1]==':')
				{
					pLexeme[px_strlen(pLexeme)-1]='\0';
					if (PX_ScriptAsmIsValidToken(pLexeme))
					{
						if (!PX_ScriptAsmAddSourceLabel(compiler,pLexeme,PX_FALSE))
						{
							PX_ScriptAsmError(&compiler->lexer,"Add label error.");
							goto _ERROR;
						}
					}
					continue;
				}


				if (px_strequ("EXPORT",pLexeme))
				{
					if ((type=PX_ScriptAsmNexLexeme (&compiler->lexer))!=PX_LEXER_LEXEME_TYPE_TOKEN)
					{
						PX_ScriptAsmError(&compiler->lexer,"Mnemonic \"Function\" not found!");
						goto _ERROR;
					}
					pLexeme=compiler->lexer.CurLexeme.buffer;

					if (!px_strequ(pLexeme,"FUNCTION"))
					{
						PX_ScriptAsmError(&compiler->lexer,"Mnemonic \"Function\" not found!");
						goto _ERROR;
					}

					if ((type=PX_ScriptAsmNexLexeme (&compiler->lexer))!=PX_LEXER_LEXEME_TYPE_TOKEN)
					{
						PX_ScriptAsmError(&compiler->lexer,"Function name expected but not found!");
						goto _ERROR;
					}

					pLexeme=compiler->lexer.CurLexeme.buffer;

					if (pLexeme[px_strlen(pLexeme)-1]!=':')
					{
						PX_ScriptAsmError(&compiler->lexer,"Function name colon expected but not found!");
						goto _ERROR;
					}
					pLexeme[px_strlen(pLexeme)-1]='\0';
					if (PX_ScriptAsmIsValidToken(pLexeme))
					{
						if (!PX_ScriptAsmAddSourceLabel(compiler,pLexeme,PX_TRUE))
						{
							PX_ScriptAsmError(&compiler->lexer,"Add label error.");
							goto _ERROR;
						}
					}

					continue;
				}

				if (px_strequ("FUNCTION",pLexeme))
				{
					if ((type=PX_ScriptAsmNexLexeme (&compiler->lexer))!=PX_LEXER_LEXEME_TYPE_TOKEN)
					{
						PX_ScriptAsmError(&compiler->lexer,"Function name expected but not found!");
						goto _ERROR;
					}
					pLexeme=compiler->lexer.CurLexeme.buffer;

					if (pLexeme[px_strlen(pLexeme)-1]!=':')
					{
						PX_ScriptAsmError(&compiler->lexer,"Function name colon expected but not found!");
						goto _ERROR;
					}
					pLexeme[px_strlen(pLexeme)-1]='\0';
					if (PX_ScriptAsmIsValidToken(pLexeme))
					{
						if (!PX_ScriptAsmAddSourceLabel(compiler,pLexeme,PX_FALSE))
						{
							PX_ScriptAsmError(&compiler->lexer,"Add label error.");
							goto _ERROR;
						}
					}

					continue;
				}

				//.Global
				if (px_strequ(".GLOBAL",pLexeme))
				{
					if ((type=PX_ScriptAsmNexLexeme (&compiler->lexer))!=PX_LEXER_LEXEME_TYPE_TOKEN)
					{
						PX_ScriptAsmError(&compiler->lexer,"Global size should be integer(const)!");
						goto _ERROR;
					}

					pLexeme=compiler->lexer.CurLexeme.buffer;
					if(!PX_ScriptAsm_isInt(pLexeme))
					{
						PX_ScriptAsmError(&compiler->lexer,"Global size should be integer(const)!");
						goto _ERROR;
					}
					compiler->globalsize=PX_ScriptAsm_atoi(pLexeme);
					if ((type=PX_ScriptAsmNexLexeme (&compiler->lexer))!=PX_LEXER_LEXEME_TYPE_NEWLINE&&type!=PX_LEXER_LEXEME_TYPE_END)
					{
						PX_ScriptAsmError(&compiler->lexer,"New line is expected but not found.");
						goto _ERROR;
					}
					continue;
				}

				//.STACK
				if (px_strequ(".STACK",pLexeme))
				{
					if ((type=PX_ScriptAsmNexLexeme (&compiler->lexer))!=PX_LEXER_LEXEME_TYPE_TOKEN)
					{
						PX_ScriptAsmError(&compiler->lexer,"Stack size should be integer(const)!");
						goto _ERROR;
					}

					pLexeme=compiler->lexer.CurLexeme.buffer;

					if(!PX_ScriptAsm_isInt(pLexeme))
					{
						PX_ScriptAsmError(&compiler->lexer,"Stack size should be integer(const)!");
						goto _ERROR;
					}
					compiler->stacksize=PX_ScriptAsm_atoi(pLexeme);
					if ((type=PX_ScriptAsmNexLexeme (&compiler->lexer))!=PX_LEXER_LEXEME_TYPE_NEWLINE&&type!=PX_LEXER_LEXEME_TYPE_END)
					{
						PX_ScriptAsmError(&compiler->lexer,"New line is expected but not found.");
						goto _ERROR;
					}
					continue;
				}

				//.THREAD
				if (px_strequ(".THREAD",pLexeme))
				{
					if ((type=PX_ScriptAsmNexLexeme (&compiler->lexer))!=PX_LEXER_LEXEME_TYPE_TOKEN)
					{
						PX_ScriptAsmError(&compiler->lexer,"thread count should be integer(const)!");
						goto _ERROR;
					}

					pLexeme=compiler->lexer.CurLexeme.buffer;

					if(!PX_ScriptAsm_isInt(pLexeme))
					{
						PX_ScriptAsmError(&compiler->lexer,"thread count should be integer(const)!");
						goto _ERROR;
					}
					compiler->threadcount=PX_ScriptAsm_atoi(pLexeme);

					if(compiler->threadcount<=0)
					{
						PX_ScriptAsmError(&compiler->lexer,"thread count should not less than 1!");
						goto _ERROR;
					}

					if ((type=PX_ScriptAsmNexLexeme (&compiler->lexer))!=PX_LEXER_LEXEME_TYPE_NEWLINE&&type!=PX_LEXER_LEXEME_TYPE_END)
					{
						PX_ScriptAsmError(&compiler->lexer,"New line is expected but not found.");
						goto _ERROR;
					}
					continue;
				}

				//normal
				hasMatched=PX_FALSE;
				state=PX_LexerGetState(&compiler->lexer);
				for (i=0;i<compiler->GrammarInstrTable.size;i++)
				{
					pinstr=PX_LISTAT(PX_SCRIPT_ASM_GRAMMAR_INSTR,&compiler->GrammarInstrTable,i);
					if (px_strequ(pinstr->mnemonic,pLexeme))
					{
						if (pinstr->paramCount==0)
						{
							if ((type=PX_ScriptAsmNexLexeme (&compiler->lexer))!=PX_LEXER_LEXEME_TYPE_NEWLINE&&type!=PX_LEXER_LEXEME_TYPE_END)
							{
								PX_LexerSetState(state);
								continue;
							}
							hasMatched=PX_TRUE;
							break;
						}

						for (j=0;j<pinstr->paramCount;j++)
						{
							type=PX_ScriptAsmNexLexeme (&compiler->lexer);
							pLexeme=compiler->lexer.CurLexeme.buffer;
							if (type!=PX_LEXER_LEXEME_TYPE_TOKEN&&type!=PX_LEXER_LEXEME_TYPE_CONATINER)
							{
								PX_LexerSetState(state);
								continue;
							}
							else
							{

								if (PX_ScriptAsmTokenType(compiler,compiler->lexer.CurLexeme.buffer)==PX_SCRIPT_ASM_OPERAND_ACC_TYPE_UNKNOW)
								{
									if (!PX_ScriptAsmIsValidToken(compiler->lexer.CurLexeme.buffer))
									{
									PX_ScriptAsmError(&compiler->lexer,"Unknow token type");
									goto _ERROR;
									}
								}

								if (PX_ScriptAsmTokenType(compiler,compiler->lexer.CurLexeme.buffer)==PX_SCRIPT_ASM_OPERAND_ACC_TYPE_STRING)
								{
									PX_StringInit(compiler->mp,&includeStr);
									PX_LexerGetIncludedString(&compiler->lexer,&includeStr);
									PX_ScriptAsmStringConvert(&includeStr);
									PX_ScriptAsmAddStringConst(compiler,includeStr.buffer);
									PX_StringFree(&includeStr);
								}

								if (PX_ScriptAsmTokenType(compiler,compiler->lexer.CurLexeme.buffer)==PX_SCRIPT_ASM_OPERAND_ACC_TYPE_MEMORY)
								{
									PX_StringInit(compiler->mp,&includeStr);
									PX_LexerGetIncludedString(&compiler->lexer,&includeStr);
									if(!PX_ScriptAsmAddMemoryConst(compiler,includeStr.buffer))
									{
										PX_ScriptAsmError(&compiler->lexer,"Invalid memories const!");
										goto _ERROR;
									}
									PX_StringFree(&includeStr);
								}

								if (PX_ScriptAsmTokenType(compiler,compiler->lexer.CurLexeme.buffer)==PX_SCRIPT_ASM_OPERAND_ACC_TYPE_HOST)
								{
									PX_ScriptAsmAddHost(compiler,compiler->lexer.CurLexeme.buffer);
								}

								if(j<pinstr->paramCount-1)
								{
									type=PX_ScriptAsmNexLexeme (&compiler->lexer);
									pLexeme=compiler->lexer.CurLexeme.buffer;
									if (type!=PX_LEXER_LEXEME_TYPE_DELIMITER)
									{
										PX_ScriptAsmError(&compiler->lexer,"Parameters error.");
										goto _ERROR;
									}
								}
							}
						}

						if ((type=PX_ScriptAsmNexLexeme (&compiler->lexer))!=PX_LEXER_LEXEME_TYPE_NEWLINE&&type!=PX_LEXER_LEXEME_TYPE_END)
						{
							PX_LexerSetState(state);
							continue;
						}
						hasMatched=PX_TRUE;
						break;
					}
				}
				if (!hasMatched)
				{
					PX_ScriptAsmError(&compiler->lexer,"Instr not match.");
					PX_LexerSetState(state);
					goto _ERROR;
				}

			}
			break;
		}
	}
	PX_StringFree(&assumeName);
	PX_StringFree(&assumeString);
	return PX_TRUE;
_ERROR:
	PX_StringFree(&assumeName);
	PX_StringFree(&assumeString);
	return PX_FALSE;

}




px_bool PX_ScriptAsmCc(PX_SCRIPT_ASM_COMPILER *compiler)
{
	px_string ParamMne;
	PX_LEXER_STATE state=PX_LexerGetState(&compiler->lexer);
	PX_LEXER_LEXEME_TYPE type;
	PX_SCRIPT_ASM_GRAMMAR_INSTR *pinstr;
	PX_SCRIPT_ASM_INSTR_BIN instrbin;
	px_string assumeString,assumeName;
	px_int i,j,TokenType,InstrBinOffset=0;
	px_float itf;
	px_bool hasMatched;
	px_char *pLexeme;
	px_string strInc;

	PX_StringInit(compiler->mp,&ParamMne);
	PX_StringInit(compiler->mp,&assumeName);
	PX_StringInit(compiler->mp,&assumeString);

	
	while ((type=PX_ScriptAsmNexLexeme (&compiler->lexer))!=PX_LEXER_LEXEME_TYPE_END)
	{
		pLexeme=compiler->lexer.CurLexeme.buffer;
		switch(type)
		{
		case PX_LEXER_LEXEME_TYPE_SPACER:
		case PX_LEXER_LEXEME_TYPE_NEWLINE:
			continue;
			break;
		case PX_LEXER_LEXEME_TYPE_TOKEN:
			{
				//special 

				//ASSUME
				pLexeme=compiler->lexer.CurLexeme.buffer;
				if (px_strequ("ASSUME",pLexeme))
				{
					if ((type=PX_ScriptAsmNexLexeme (&compiler->lexer))!=PX_LEXER_LEXEME_TYPE_TOKEN)
					{
						PX_ScriptAsmError(&compiler->lexer,"ASSUME first token should be token!");
						goto _ERROR;
					}

					if (px_strIsNumeric(pLexeme))
					{
						PX_ScriptAsmError(&compiler->lexer,"ASSUME first token should not be numeric!");
						goto _ERROR;
					}

					PX_StringFormat(&assumeName,pLexeme);

					if ((type=PX_ScriptAsmNexLexeme (&compiler->lexer))!=PX_LEXER_LEXEME_TYPE_TOKEN)
					{
						PX_ScriptAsmError(&compiler->lexer,"ASSUME second token should be token!");
						goto _ERROR;
					}
					pLexeme=compiler->lexer.CurLexeme.buffer;
					PX_StringFormat(&assumeString,pLexeme);

					if (px_strequ(assumeString.buffer,assumeName.buffer))
					{
						PX_ScriptAsmError(&compiler->lexer,"ASSUME token should not equal.");
						goto _ERROR;
					}

					if ((type=PX_ScriptAsmNexLexeme (&compiler->lexer))!=PX_LEXER_LEXEME_TYPE_NEWLINE&&type!=PX_LEXER_LEXEME_TYPE_END)
					{
						PX_ScriptAsmError(&compiler->lexer,"New line is expected but not found.");
						goto _ERROR;
					}
					pLexeme=compiler->lexer.CurLexeme.buffer;
					PX_ScriptAsmUpdateAssumeTable(compiler,assumeName.buffer,assumeString.buffer);
					continue;
				}

				//Function or Lable
				if (pLexeme[px_strlen(pLexeme)-1]==':')
				{
					pLexeme[px_strlen(pLexeme)-1]='\0';
					if (PX_ScriptAsmIsValidToken(pLexeme))
					{
						if (!PX_ScriptAsmUpdateLabelAddr(compiler,pLexeme,InstrBinOffset))
						{
							goto _ERROR;
						}
					}
					continue;
				}

				if (px_strequ("EXPORT",pLexeme))
				{
					PX_ScriptAsmNexLexeme (&compiler->lexer);
					PX_ScriptAsmNexLexeme (&compiler->lexer);
					pLexeme=compiler->lexer.CurLexeme.buffer;
					pLexeme[px_strlen(pLexeme)-1]='\0';
					if (PX_ScriptAsmIsValidToken(pLexeme))
					{
						if (!PX_ScriptAsmUpdateLabelAddr(compiler,pLexeme,InstrBinOffset))
						{
							goto _ERROR;
						}
					}
					continue;
				}

				if (px_strequ("FUNCTION",pLexeme))
				{
					PX_ScriptAsmNexLexeme (&compiler->lexer);
					pLexeme=compiler->lexer.CurLexeme.buffer;
					pLexeme[px_strlen(pLexeme)-1]='\0';
					if (PX_ScriptAsmIsValidToken(pLexeme))
					{
						if (!PX_ScriptAsmUpdateLabelAddr(compiler,pLexeme,InstrBinOffset))
						{
							goto _ERROR;
						}
					}
					continue;
				}

				//.Global
				if (px_strequ(".GLOBAL",pLexeme))
				{
					PX_ScriptAsmNexLexeme (&compiler->lexer);
					PX_ScriptAsmNexLexeme (&compiler->lexer);
					continue;
				}

				//.Stack
				if (px_strequ(".STACK",pLexeme))
				{
					PX_ScriptAsmNexLexeme (&compiler->lexer);
					PX_ScriptAsmNexLexeme (&compiler->lexer);
					continue;
				}

				//.Thread
				if (px_strequ(".THREAD",pLexeme))
				{
					PX_ScriptAsmNexLexeme (&compiler->lexer);
					PX_ScriptAsmNexLexeme (&compiler->lexer);
					continue;
				}

				//normal
				hasMatched=PX_FALSE;
				state=PX_LexerGetState(&compiler->lexer);
				for (i=0;i<compiler->GrammarInstrTable.size;i++)
				{
					px_memset(&instrbin,0,sizeof(instrbin));
					pinstr=(PX_SCRIPT_ASM_GRAMMAR_INSTR *)PX_ListAt(&compiler->GrammarInstrTable,i)->pdata;
					if (px_strequ(pinstr->mnemonic,pLexeme))
					{
						instrbin.opCode=pinstr->opcode;

						for (j=0;j<pinstr->paramCount;j++)
						{
							type=PX_ScriptAsmNexLexeme (&compiler->lexer);
							pLexeme=compiler->lexer.CurLexeme.buffer;
							PX_StringClear(&ParamMne);
							PX_StringCat(&ParamMne,pLexeme);

							PX_ScriptASMMapAssume(compiler,&ParamMne);

							if (type!=PX_LEXER_LEXEME_TYPE_TOKEN&&type!=PX_LEXER_LEXEME_TYPE_CONATINER)
							{
								PX_LexerSetState(state);
								continue;
							}
							else
							{
								if (((TokenType=PX_ScriptAsmTokenType(compiler,ParamMne.buffer))&pinstr->operandAccTypes[j])==0)
								{
								PX_LexerSetState(state);
								continue;
								}
								if (TokenType==PX_SCRIPT_ASM_OPERAND_ACC_TYPE_UNKNOW)
								{
									PX_ScriptAsmError(&compiler->lexer,"illegal Token Type");
									return PX_FALSE;
								}
								instrbin.optype[j]=PX_ScriptAsmTokenTypeToOpType(TokenType);
								switch(instrbin.optype[j])
								{
								case PX_SCRIPT_ASM_OPTYPE_BP:
									case PX_SCRIPT_ASM_OPTYPE_SP:
										case PX_SCRIPT_ASM_OPTYPE_IP:
											instrbin.param[j]=0;
											break;

								case PX_SCRIPT_ASM_OPTYPE_MEMORY:
									PX_StringInit(compiler->mp,&strInc);
									PX_StringCat(&strInc,ParamMne.buffer);
									PX_LexerGetIncludedString(&compiler->lexer,&strInc);
									instrbin.param[j]=PX_ScriptAsmGetMemoryAddr(compiler,strInc.buffer);
									PX_StringFree(&strInc);
									break;
								case PX_SCRIPT_ASM_OPTYPE_STRING:
									PX_StringInit(compiler->mp,&strInc);
									PX_StringCat(&strInc,ParamMne.buffer);
									PX_LexerGetIncludedString(&compiler->lexer,&strInc);
									PX_ScriptAsmStringConvert(&strInc);
									instrbin.param[j]=PX_ScriptAsmGetStringAddr(compiler,strInc.buffer);
									PX_StringFree(&strInc);
									break;
								case PX_SCRIPT_ASM_OPTYPE_LABEL:
									instrbin.param[j]=PX_ScriptAsmGetLabelIndex(compiler,ParamMne.buffer);
									break;
								case PX_SCRIPT_ASM_OPTYPE_FLOAT:
									itf=PX_atof(ParamMne.buffer);
									instrbin.param[j]=*((int *)&itf);
									break;
								case PX_SCRIPT_ASM_OPTYPE_INT:
									if(ParamMne.buffer[0]!='\'')
									instrbin.param[j]=PX_ScriptAsm_atoi(ParamMne.buffer);
									else
									instrbin.param[j]=ParamMne.buffer[1];
									break;
								case PX_SCRIPT_ASM_OPTYPE_REG:
									instrbin.param[j]=PX_ScriptAsm_atoi(ParamMne.buffer+1);
									break;
								case PX_SCRIPT_ASM_OPTYPE_LOCAL:
									if (!PX_ScriptAsmLocalType(compiler,ParamMne.buffer,&instrbin.optype[j],&instrbin.param[j]))
									{
										PX_ScriptAsmError(&compiler->lexer,"illegal local referenced");
										return PX_FALSE;
									}

									break;
								case PX_SCRIPT_ASM_OPTYPE_GLOBAL:
									if (!PX_ScriptAsmGlobalType(compiler,ParamMne.buffer,&instrbin.optype[j],&instrbin.param[j]))
									{
										PX_ScriptAsmError(&compiler->lexer,"illegal local referenced");
										return PX_FALSE;
									}
									break;
								case PX_SCRIPT_ASM_OPTYPE_HOST:
									instrbin.param[j]=PX_ScriptAsmGetHostIndex(compiler,ParamMne.buffer);
									break;
								default:
									{
										PX_ScriptAsmError(&compiler->lexer,"Instrutment not match.");
										PX_LexerSetState(state);
										return PX_FALSE;
									}
								}
							
							}
							if (j!=pinstr->paramCount-1)
							{
								PX_ScriptAsmNexLexeme (&compiler->lexer);
							}
						}

						if ((type=PX_ScriptAsmNexLexeme (&compiler->lexer))!=PX_LEXER_LEXEME_TYPE_NEWLINE&&type!=PX_LEXER_LEXEME_TYPE_END)
						{
							PX_LexerSetState(state);
							continue;
						}
						hasMatched=PX_TRUE;
						instrbin.addr=InstrBinOffset;
						instrbin.opcount=pinstr->paramCount;
						PX_VectorPushback(&compiler->StreamTable,&instrbin);
						InstrBinOffset+=(4+sizeof(px_dword)*pinstr->paramCount);
						break;
					}
				}
				if (!hasMatched)
				{
					PX_ScriptAsmError(&compiler->lexer,"Instr not match.");
					PX_LexerSetState(state);
					return PX_FALSE;
				}

			}
			break;
		}
	}
	PX_StringFree(&ParamMne);
	PX_StringFree(&assumeName);
	PX_StringFree(&assumeString);
	return PX_TRUE;
_ERROR:
	PX_StringFree(&ParamMne);
	PX_StringFree(&assumeName);
	PX_StringFree(&assumeString);
	return PX_FALSE;
}

px_void PX_ScriptAsmLink(PX_SCRIPT_ASM_COMPILER *compiler)
{
	px_int i,j;
	PX_SCRIPT_ASM_INSTR_BIN *pbin=(PX_SCRIPT_ASM_INSTR_BIN *)compiler->StreamTable.data;
	for (i=0;i<compiler->StreamTable.size;i++)
	{
		for (j=0;j<3;j++)
		{
			if (pbin[i].optype[j]==PX_SCRIPT_ASM_OPTYPE_LABEL)
			{
				pbin[i].param[j]=PX_LISTAT(PX_SCRIPT_ASM_LABEL_NODE,&compiler->LabelTable,pbin[i].param[j])->binaddr;
				pbin[i].optype[j]=PX_SCRIPT_ASM_OPTYPE_INT;
			}
		}
	}
}

px_bool PX_ScriptAsmCompile(px_memorypool *mp,px_char *asmcode,px_memory *binmemory)
{

	PX_SCRIPT_ASM_COMPILER compiler;
	PX_SCRIPT_ASM_HEADER header,*pheader;
	PX_SCRIPT_EXPORT_FUNCTION expFunc;
	PX_SCRIPT_ASM_HOST_NODE host;
	PX_SCRIPT_ASM_INSTR_BIN *pinstrs;
	PX_LEXER_STATE lexerstate;
	px_uint quotes;
	px_int i;
	px_char *pbuf;
	px_dword woffset;
	px_dword binsize,bufsize;

	//////////////////////////////////////////////////////////////////////////
	//test 

	compiler.mp=mp;

	PX_LexerInit(&compiler.lexer,mp);
	PX_LexerRegisterComment(&compiler.lexer,";","\n");
	PX_LexerRegisterComment(&compiler.lexer,";","\r");
	PX_LexerRegisterDelimiter(&compiler.lexer,',');
	PX_LexerRegisterSpacer(&compiler.lexer,' ');
	PX_LexerRegisterSpacer(&compiler.lexer,'\t');
	quotes=PX_LexerRegisterContainer(&compiler.lexer,"\"","\"");
	PX_LexerRegisterContainerTransfer(&compiler.lexer,quotes,'\\');
	PX_LexerRegisterContainer(&compiler.lexer,"\'","\'");
	PX_LexerRegisterContainer(&compiler.lexer,"@","@");

	PX_VectorInit(mp,&compiler.StreamTable,sizeof(PX_SCRIPT_ASM_INSTR_BIN),256);
	PX_ListInit(mp,&compiler.assumeTable);
	PX_ListInit(mp,&compiler.GrammarInstrTable);
	PX_ListInit(mp,&compiler.HostTable);
	PX_ListInit(mp,&compiler.LabelTable);
	PX_ListInit(mp,&compiler.StringTable);
	PX_ListInit(mp,&compiler.memoriesTable);
	
	compiler.threadcount=1;

	//binary format
	//opcode 1 byte
	//optype 3 byte
	//operand [0,3]4 byte


	PX_ScriptAsmRegistInstr(&compiler,"MOV",PX_SCRIPT_ASM_INSTR_OPCODE_MOV,2,
		PX_SCRIPT_ASM_OPERAND_ACC_TYPE_REG|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_LOCAL|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_GLOBAL|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_BP|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_SP|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_IP,
		PX_SCRIPT_ASM_OPERAND_ACC_TYPE_NUM|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_REG|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_LOCAL|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_GLOBAL|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_BP|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_SP|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_IP|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_MEMORY|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_STRING
		);

	PX_ScriptAsmRegistInstr(&compiler,"ADD",PX_SCRIPT_ASM_INSTR_OPCODE_ADD,2,
		PX_SCRIPT_ASM_OPERAND_ACC_TYPE_REG|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_LOCAL|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_GLOBAL|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_BP|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_SP|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_IP,
		PX_SCRIPT_ASM_OPERAND_ACC_TYPE_NUM|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_REG|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_LOCAL|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_GLOBAL|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_BP|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_SP|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_IP
		);

	PX_ScriptAsmRegistInstr(&compiler,"SUB",PX_SCRIPT_ASM_INSTR_OPCODE_SUB,2,
		PX_SCRIPT_ASM_OPERAND_ACC_TYPE_REG|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_LOCAL|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_GLOBAL|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_BP|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_SP|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_IP,
		PX_SCRIPT_ASM_OPERAND_ACC_TYPE_NUM|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_REG|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_LOCAL|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_GLOBAL
		);

	PX_ScriptAsmRegistInstr(&compiler,"DIV",PX_SCRIPT_ASM_INSTR_OPCODE_DIV,2,
		PX_SCRIPT_ASM_OPERAND_ACC_TYPE_REG|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_LOCAL|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_GLOBAL,
		PX_SCRIPT_ASM_OPERAND_ACC_TYPE_NUM|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_REG|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_LOCAL|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_GLOBAL
		);

	PX_ScriptAsmRegistInstr(&compiler,"MUL",PX_SCRIPT_ASM_INSTR_OPCODE_MUL,2,
		PX_SCRIPT_ASM_OPERAND_ACC_TYPE_REG|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_LOCAL|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_GLOBAL,
		PX_SCRIPT_ASM_OPERAND_ACC_TYPE_NUM|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_REG|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_LOCAL|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_GLOBAL
		);

	PX_ScriptAsmRegistInstr(&compiler,"MOD",PX_SCRIPT_ASM_INSTR_OPCODE_MOD,2,
		PX_SCRIPT_ASM_OPERAND_ACC_TYPE_REG|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_LOCAL|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_GLOBAL,
		PX_SCRIPT_ASM_OPERAND_ACC_TYPE_INT|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_REG|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_LOCAL|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_GLOBAL
		);

	PX_ScriptAsmRegistInstr(&compiler,"SHL",PX_SCRIPT_ASM_INSTR_OPCODE_SHL,2,
		PX_SCRIPT_ASM_OPERAND_ACC_TYPE_REG|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_LOCAL|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_GLOBAL,
		PX_SCRIPT_ASM_OPERAND_ACC_TYPE_INT|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_REG|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_LOCAL|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_GLOBAL
		);

	PX_ScriptAsmRegistInstr(&compiler,"SHR",PX_SCRIPT_ASM_INSTR_OPCODE_SHR,2,
		PX_SCRIPT_ASM_OPERAND_ACC_TYPE_REG|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_LOCAL|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_GLOBAL,
		PX_SCRIPT_ASM_OPERAND_ACC_TYPE_INT|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_REG|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_LOCAL|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_GLOBAL
		);

	PX_ScriptAsmRegistInstr(&compiler,"AND",PX_SCRIPT_ASM_INSTR_OPCODE_AND,2,
		PX_SCRIPT_ASM_OPERAND_ACC_TYPE_REG|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_LOCAL|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_GLOBAL,
		PX_SCRIPT_ASM_OPERAND_ACC_TYPE_NUM|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_REG|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_LOCAL|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_GLOBAL
		);

	PX_ScriptAsmRegistInstr(&compiler,"ANDL",PX_SCRIPT_ASM_INSTR_OPCODE_ANDL,2,
		PX_SCRIPT_ASM_OPERAND_ACC_TYPE_REG|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_LOCAL|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_GLOBAL,
		PX_SCRIPT_ASM_OPERAND_ACC_TYPE_NUM|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_REG|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_LOCAL|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_GLOBAL
		);

	PX_ScriptAsmRegistInstr(&compiler,"ORL",PX_SCRIPT_ASM_INSTR_OPCODE_ORL,2,
		PX_SCRIPT_ASM_OPERAND_ACC_TYPE_REG|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_LOCAL|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_GLOBAL,
		PX_SCRIPT_ASM_OPERAND_ACC_TYPE_NUM|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_REG|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_LOCAL|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_GLOBAL
		);

	PX_ScriptAsmRegistInstr(&compiler,"OR",PX_SCRIPT_ASM_INSTR_OPCODE_OR,2,
		PX_SCRIPT_ASM_OPERAND_ACC_TYPE_REG|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_LOCAL|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_GLOBAL,
		PX_SCRIPT_ASM_OPERAND_ACC_TYPE_NUM|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_REG|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_LOCAL|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_GLOBAL
		);

	PX_ScriptAsmRegistInstr(&compiler,"XOR",PX_SCRIPT_ASM_INSTR_OPCODE_XOR,2,
		PX_SCRIPT_ASM_OPERAND_ACC_TYPE_REG|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_LOCAL|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_GLOBAL,
		PX_SCRIPT_ASM_OPERAND_ACC_TYPE_NUM|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_REG|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_LOCAL|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_GLOBAL
		);

	PX_ScriptAsmRegistInstr(&compiler,"POW",PX_SCRIPT_ASM_INSTR_OPCODE_POW,2,
		PX_SCRIPT_ASM_OPERAND_ACC_TYPE_REG|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_LOCAL|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_GLOBAL,
		PX_SCRIPT_ASM_OPERAND_ACC_TYPE_NUM|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_REG|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_LOCAL|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_GLOBAL
		);
	PX_ScriptAsmRegistInstr(&compiler,"SIN",PX_SCRIPT_ASM_INSTR_OPCODE_SIN,2,
		PX_SCRIPT_ASM_OPERAND_ACC_TYPE_REG|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_LOCAL|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_GLOBAL,
		PX_SCRIPT_ASM_OPERAND_ACC_TYPE_NUM|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_REG|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_LOCAL|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_GLOBAL
		);

	PX_ScriptAsmRegistInstr(&compiler,"COS",PX_SCRIPT_ASM_INSTR_OPCODE_COS,2,
		PX_SCRIPT_ASM_OPERAND_ACC_TYPE_REG|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_LOCAL|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_GLOBAL,
		PX_SCRIPT_ASM_OPERAND_ACC_TYPE_NUM|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_REG|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_LOCAL|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_GLOBAL
		);

	PX_ScriptAsmRegistInstr(&compiler,"INT",PX_SCRIPT_ASM_INSTR_OPCODE_INT,1,
		PX_SCRIPT_ASM_OPERAND_ACC_TYPE_REG|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_LOCAL|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_GLOBAL
		);

	PX_ScriptAsmRegistInstr(&compiler,"FLT",PX_SCRIPT_ASM_INSTR_OPCODE_FLT,1,
		PX_SCRIPT_ASM_OPERAND_ACC_TYPE_REG|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_LOCAL|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_GLOBAL
		);


	PX_ScriptAsmRegistInstr(&compiler,"STRLEN",PX_SCRIPT_ASM_INSTR_OPCODE_STRLEN,2,
		PX_SCRIPT_ASM_OPERAND_ACC_TYPE_REG|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_LOCAL|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_GLOBAL,
		PX_SCRIPT_ASM_OPERAND_ACC_TYPE_REG|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_LOCAL|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_GLOBAL|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_STRING
		);

	PX_ScriptAsmRegistInstr(&compiler,"STRCAT",PX_SCRIPT_ASM_INSTR_OPCODE_STRCAT,2,
		PX_SCRIPT_ASM_OPERAND_ACC_TYPE_REG|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_LOCAL|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_GLOBAL,
		PX_SCRIPT_ASM_OPERAND_ACC_TYPE_REG|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_LOCAL|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_GLOBAL|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_STRING
		);

	PX_ScriptAsmRegistInstr(&compiler,"STRREP",PX_SCRIPT_ASM_INSTR_OPCODE_STRREP,3,
		PX_SCRIPT_ASM_OPERAND_ACC_TYPE_REG|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_LOCAL|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_GLOBAL,
		PX_SCRIPT_ASM_OPERAND_ACC_TYPE_REG|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_LOCAL|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_GLOBAL|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_STRING,
		PX_SCRIPT_ASM_OPERAND_ACC_TYPE_REG|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_LOCAL|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_GLOBAL|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_STRING
		);

	PX_ScriptAsmRegistInstr(&compiler,"STRCHR",PX_SCRIPT_ASM_INSTR_OPCODE_STRCHR,3,
		PX_SCRIPT_ASM_OPERAND_ACC_TYPE_REG|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_LOCAL|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_GLOBAL,
		PX_SCRIPT_ASM_OPERAND_ACC_TYPE_REG|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_LOCAL|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_GLOBAL|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_STRING,
		PX_SCRIPT_ASM_OPERAND_ACC_TYPE_REG|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_LOCAL|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_GLOBAL|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_INT
		);

	PX_ScriptAsmRegistInstr(&compiler,"STRTOI",PX_SCRIPT_ASM_INSTR_OPCODE_STRTOI,2,
		PX_SCRIPT_ASM_OPERAND_ACC_TYPE_REG|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_LOCAL|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_GLOBAL,
		PX_SCRIPT_ASM_OPERAND_ACC_TYPE_REG|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_LOCAL|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_GLOBAL|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_STRING
		);

	PX_ScriptAsmRegistInstr(&compiler,"STRTOF",PX_SCRIPT_ASM_INSTR_OPCODE_STRTOF,2,
		PX_SCRIPT_ASM_OPERAND_ACC_TYPE_REG|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_LOCAL|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_GLOBAL,
		PX_SCRIPT_ASM_OPERAND_ACC_TYPE_REG|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_LOCAL|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_GLOBAL|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_STRING
		);

	PX_ScriptAsmRegistInstr(&compiler,"STRFRI",PX_SCRIPT_ASM_INSTR_OPCODE_STRFRI,2,
		PX_SCRIPT_ASM_OPERAND_ACC_TYPE_REG|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_LOCAL|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_GLOBAL,
		PX_SCRIPT_ASM_OPERAND_ACC_TYPE_REG|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_LOCAL|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_GLOBAL|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_INT
		);

	PX_ScriptAsmRegistInstr(&compiler,"STRFRF",PX_SCRIPT_ASM_INSTR_OPCODE_STRFRF,2,
		PX_SCRIPT_ASM_OPERAND_ACC_TYPE_REG|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_LOCAL|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_GLOBAL,
		PX_SCRIPT_ASM_OPERAND_ACC_TYPE_REG|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_LOCAL|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_GLOBAL|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_FLOAT
		);

	PX_ScriptAsmRegistInstr(&compiler,"STRTMEM",PX_SCRIPT_ASM_INSTR_OPCODE_STRTMEM,1,
		PX_SCRIPT_ASM_OPERAND_ACC_TYPE_REG|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_LOCAL|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_GLOBAL
		);

	PX_ScriptAsmRegistInstr(&compiler,"STRSET",PX_SCRIPT_ASM_INSTR_OPCODE_STRSET,3,
		PX_SCRIPT_ASM_OPERAND_ACC_TYPE_REG|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_LOCAL|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_GLOBAL,
		PX_SCRIPT_ASM_OPERAND_ACC_TYPE_REG|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_LOCAL|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_GLOBAL|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_INT,
		PX_SCRIPT_ASM_OPERAND_ACC_TYPE_REG|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_LOCAL|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_GLOBAL|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_INT|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_STRING
		);

	PX_ScriptAsmRegistInstr(&compiler,"STRFIND",PX_SCRIPT_ASM_INSTR_OPCODE_STRFIND,3,
		PX_SCRIPT_ASM_OPERAND_ACC_TYPE_REG|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_LOCAL|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_GLOBAL,
		PX_SCRIPT_ASM_OPERAND_ACC_TYPE_REG|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_LOCAL|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_GLOBAL|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_STRING,
		PX_SCRIPT_ASM_OPERAND_ACC_TYPE_REG|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_LOCAL|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_GLOBAL|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_STRING
		);

	PX_ScriptAsmRegistInstr(&compiler,"ASC",PX_SCRIPT_ASM_INSTR_OPCODE_ASC,2,
		PX_SCRIPT_ASM_OPERAND_ACC_TYPE_REG|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_LOCAL|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_GLOBAL,
		PX_SCRIPT_ASM_OPERAND_ACC_TYPE_REG|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_LOCAL|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_GLOBAL|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_STRING
		);

	PX_ScriptAsmRegistInstr(&compiler,"MEMBYTE",PX_SCRIPT_ASM_INSTR_OPCODE_MEMBYTE,3,
		PX_SCRIPT_ASM_OPERAND_ACC_TYPE_REG|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_LOCAL|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_GLOBAL,
		PX_SCRIPT_ASM_OPERAND_ACC_TYPE_REG|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_GLOBAL|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_LOCAL|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_INT,
		PX_SCRIPT_ASM_OPERAND_ACC_TYPE_REG|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_LOCAL|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_GLOBAL|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_MEMORY
		);

	PX_ScriptAsmRegistInstr(&compiler,"MEMSET",PX_SCRIPT_ASM_INSTR_OPCODE_MEMSET,3,
		PX_SCRIPT_ASM_OPERAND_ACC_TYPE_REG|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_LOCAL|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_GLOBAL,
		PX_SCRIPT_ASM_OPERAND_ACC_TYPE_REG|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_GLOBAL|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_LOCAL|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_INT,
		PX_SCRIPT_ASM_OPERAND_ACC_TYPE_NUM|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_REG|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_LOCAL|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_GLOBAL
		);
	PX_ScriptAsmRegistInstr(&compiler,"MEMTRM",PX_SCRIPT_ASM_INSTR_OPCODE_MEMTRM,3,
		PX_SCRIPT_ASM_OPERAND_ACC_TYPE_REG|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_LOCAL|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_GLOBAL,
		PX_SCRIPT_ASM_OPERAND_ACC_TYPE_REG|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_GLOBAL|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_LOCAL|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_INT,
		PX_SCRIPT_ASM_OPERAND_ACC_TYPE_REG|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_LOCAL|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_GLOBAL|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_INT
		);

	PX_ScriptAsmRegistInstr(&compiler,"MEMFIND",PX_SCRIPT_ASM_INSTR_OPCODE_MEMFIND,3,
		PX_SCRIPT_ASM_OPERAND_ACC_TYPE_REG|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_LOCAL|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_GLOBAL,
		PX_SCRIPT_ASM_OPERAND_ACC_TYPE_REG|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_LOCAL|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_GLOBAL|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_MEMORY,
		PX_SCRIPT_ASM_OPERAND_ACC_TYPE_REG|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_LOCAL|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_GLOBAL|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_MEMORY
		);

	PX_ScriptAsmRegistInstr(&compiler,"MEMLEN",PX_SCRIPT_ASM_INSTR_OPCODE_MEMLEN,2,
		PX_SCRIPT_ASM_OPERAND_ACC_TYPE_REG|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_LOCAL|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_GLOBAL,
		PX_SCRIPT_ASM_OPERAND_ACC_TYPE_REG|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_LOCAL|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_GLOBAL|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_MEMORY
		);

	PX_ScriptAsmRegistInstr(&compiler,"MEMCAT",PX_SCRIPT_ASM_INSTR_OPCODE_MEMCAT,2,
		PX_SCRIPT_ASM_OPERAND_ACC_TYPE_REG|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_LOCAL|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_GLOBAL,
		PX_SCRIPT_ASM_OPERAND_ACC_TYPE_REG|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_LOCAL|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_GLOBAL|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_MEMORY
		);

	PX_ScriptAsmRegistInstr(&compiler,"MEMTSTR",PX_SCRIPT_ASM_INSTR_OPCODE_MEMTSTR,1,
		PX_SCRIPT_ASM_OPERAND_ACC_TYPE_REG|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_LOCAL|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_GLOBAL
		);

	PX_ScriptAsmRegistInstr(&compiler,"MEMTRM",PX_SCRIPT_ASM_INSTR_OPCODE_MEMTRM,3,
		PX_SCRIPT_ASM_OPERAND_ACC_TYPE_REG|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_LOCAL|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_GLOBAL|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_INT,
		PX_SCRIPT_ASM_OPERAND_ACC_TYPE_REG|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_LOCAL|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_GLOBAL|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_INT,
		PX_SCRIPT_ASM_OPERAND_ACC_TYPE_REG|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_LOCAL|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_GLOBAL|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_INT
		);

	PX_ScriptAsmRegistInstr(&compiler,"DATACPY",PX_SCRIPT_ASM_INSTR_OPCODE_DATACPY,3,
		PX_SCRIPT_ASM_OPERAND_ACC_TYPE_REG|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_LOCAL|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_GLOBAL|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_INT,
		PX_SCRIPT_ASM_OPERAND_ACC_TYPE_REG|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_LOCAL|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_GLOBAL|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_INT,
		PX_SCRIPT_ASM_OPERAND_ACC_TYPE_REG|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_LOCAL|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_GLOBAL|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_INT
		);

	PX_ScriptAsmRegistInstr(&compiler,"NEG",PX_SCRIPT_ASM_INSTR_OPCODE_NEG,1,
		PX_SCRIPT_ASM_OPERAND_ACC_TYPE_REG|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_LOCAL|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_GLOBAL
		);

	PX_ScriptAsmRegistInstr(&compiler,"INV",PX_SCRIPT_ASM_INSTR_OPCODE_INV,1,
		PX_SCRIPT_ASM_OPERAND_ACC_TYPE_REG|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_LOCAL|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_GLOBAL
		);

	PX_ScriptAsmRegistInstr(&compiler,"NOT",PX_SCRIPT_ASM_INSTR_OPCODE_NOT,1,
		PX_SCRIPT_ASM_OPERAND_ACC_TYPE_REG|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_LOCAL|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_GLOBAL
		);

	PX_ScriptAsmRegistInstr(&compiler,"JMP",PX_SCRIPT_ASM_INSTR_OPCODE_JMP,1,
		PX_SCRIPT_ASM_OPERAND_ACC_TYPE_REG|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_LOCAL|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_GLOBAL|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_INT|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_LABEL
		);

	PX_ScriptAsmRegistInstr(&compiler,"JE",PX_SCRIPT_ASM_INSTR_OPCODE_JE,3,
		PX_SCRIPT_ASM_OPERAND_ACC_TYPE_REG|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_LOCAL|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_GLOBAL|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_NUM|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_STRING,
		PX_SCRIPT_ASM_OPERAND_ACC_TYPE_REG|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_LOCAL|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_GLOBAL|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_NUM|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_STRING,
		PX_SCRIPT_ASM_OPERAND_ACC_TYPE_REG|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_LOCAL|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_GLOBAL|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_LABEL|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_INT
		);

	PX_ScriptAsmRegistInstr(&compiler,"JNE",PX_SCRIPT_ASM_INSTR_OPCODE_JNE,3,
		PX_SCRIPT_ASM_OPERAND_ACC_TYPE_REG|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_LOCAL|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_GLOBAL|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_NUM|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_STRING,
		PX_SCRIPT_ASM_OPERAND_ACC_TYPE_REG|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_LOCAL|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_GLOBAL|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_NUM|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_STRING,
		PX_SCRIPT_ASM_OPERAND_ACC_TYPE_REG|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_LOCAL|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_GLOBAL|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_LABEL|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_INT
		);

	PX_ScriptAsmRegistInstr(&compiler,"JLE",PX_SCRIPT_ASM_INSTR_OPCODE_JLE,3,
		PX_SCRIPT_ASM_OPERAND_ACC_TYPE_REG|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_LOCAL|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_GLOBAL|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_NUM|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_STRING,
		PX_SCRIPT_ASM_OPERAND_ACC_TYPE_REG|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_LOCAL|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_GLOBAL|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_NUM|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_STRING,
		PX_SCRIPT_ASM_OPERAND_ACC_TYPE_REG|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_LOCAL|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_GLOBAL|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_LABEL|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_INT
		);

	PX_ScriptAsmRegistInstr(&compiler,"JGE",PX_SCRIPT_ASM_INSTR_OPCODE_JGE,3,
		PX_SCRIPT_ASM_OPERAND_ACC_TYPE_REG|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_LOCAL|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_GLOBAL|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_NUM|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_STRING,
		PX_SCRIPT_ASM_OPERAND_ACC_TYPE_REG|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_LOCAL|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_GLOBAL|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_NUM|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_STRING,
		PX_SCRIPT_ASM_OPERAND_ACC_TYPE_REG|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_LOCAL|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_GLOBAL|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_LABEL|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_INT
		);

	PX_ScriptAsmRegistInstr(&compiler,"JL",PX_SCRIPT_ASM_INSTR_OPCODE_JL,3,
		PX_SCRIPT_ASM_OPERAND_ACC_TYPE_REG|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_LOCAL|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_GLOBAL|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_NUM|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_STRING,
		PX_SCRIPT_ASM_OPERAND_ACC_TYPE_REG|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_LOCAL|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_GLOBAL|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_NUM|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_STRING,
		PX_SCRIPT_ASM_OPERAND_ACC_TYPE_REG|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_LOCAL|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_GLOBAL|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_LABEL|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_INT
		);

	PX_ScriptAsmRegistInstr(&compiler,"JG",PX_SCRIPT_ASM_INSTR_OPCODE_JG,3,
		PX_SCRIPT_ASM_OPERAND_ACC_TYPE_REG|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_LOCAL|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_GLOBAL|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_NUM|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_STRING,
		PX_SCRIPT_ASM_OPERAND_ACC_TYPE_REG|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_LOCAL|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_GLOBAL|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_NUM|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_STRING,
		PX_SCRIPT_ASM_OPERAND_ACC_TYPE_REG|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_LOCAL|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_GLOBAL|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_LABEL|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_INT
		);

	PX_ScriptAsmRegistInstr(&compiler,"LGE",PX_SCRIPT_ASM_INSTR_OPCODE_LGE,3,
		PX_SCRIPT_ASM_OPERAND_ACC_TYPE_REG|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_LOCAL|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_GLOBAL|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_LABEL|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_INT,
		PX_SCRIPT_ASM_OPERAND_ACC_TYPE_REG|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_LOCAL|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_GLOBAL|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_NUM|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_STRING,
		PX_SCRIPT_ASM_OPERAND_ACC_TYPE_REG|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_LOCAL|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_GLOBAL|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_NUM|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_STRING
		);

	PX_ScriptAsmRegistInstr(&compiler,"LGNE",PX_SCRIPT_ASM_INSTR_OPCODE_LGNE,3,
		PX_SCRIPT_ASM_OPERAND_ACC_TYPE_REG|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_LOCAL|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_GLOBAL|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_LABEL|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_INT,
		PX_SCRIPT_ASM_OPERAND_ACC_TYPE_REG|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_LOCAL|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_GLOBAL|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_NUM|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_STRING,
		PX_SCRIPT_ASM_OPERAND_ACC_TYPE_REG|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_LOCAL|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_GLOBAL|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_NUM|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_STRING
		);


	PX_ScriptAsmRegistInstr(&compiler,"LGZ",PX_SCRIPT_ASM_INSTR_OPCODE_LGZ,1,
		PX_SCRIPT_ASM_OPERAND_ACC_TYPE_REG|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_LOCAL|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_GLOBAL
		);

	PX_ScriptAsmRegistInstr(&compiler,"LGGZ",PX_SCRIPT_ASM_INSTR_OPCODE_LGGZ,1,
		PX_SCRIPT_ASM_OPERAND_ACC_TYPE_REG|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_LOCAL|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_GLOBAL
		);

	PX_ScriptAsmRegistInstr(&compiler,"LGGEZ",PX_SCRIPT_ASM_INSTR_OPCODE_LGGEZ,1,
		PX_SCRIPT_ASM_OPERAND_ACC_TYPE_REG|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_LOCAL|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_GLOBAL
		);

	PX_ScriptAsmRegistInstr(&compiler,"LGLZ",PX_SCRIPT_ASM_INSTR_OPCODE_LGLZ,1,
		PX_SCRIPT_ASM_OPERAND_ACC_TYPE_REG|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_LOCAL|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_GLOBAL
		);

	PX_ScriptAsmRegistInstr(&compiler,"LGLEZ",PX_SCRIPT_ASM_INSTR_OPCODE_LGLEZ,1,
		PX_SCRIPT_ASM_OPERAND_ACC_TYPE_REG|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_LOCAL|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_GLOBAL
		);

	PX_ScriptAsmRegistInstr(&compiler,"CALL",PX_SCRIPT_ASM_INSTR_OPCODE_CALL,1,
		PX_SCRIPT_ASM_OPERAND_ACC_TYPE_REG|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_LOCAL|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_GLOBAL|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_LABEL|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_HOST
		);

	PX_ScriptAsmRegistInstr(&compiler,"PUSH",PX_SCRIPT_ASM_INSTR_OPCODE_PUSH,1,
		PX_SCRIPT_ASM_OPERAND_ACC_TYPE_REG|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_LOCAL|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_GLOBAL|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_NUM|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_LABEL|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_STRING|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_BP|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_SP
		);

	PX_ScriptAsmRegistInstr(&compiler,"POP",PX_SCRIPT_ASM_INSTR_OPCODE_POP,1,
		PX_SCRIPT_ASM_OPERAND_ACC_TYPE_REG|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_LOCAL|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_GLOBAL|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_BP|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_SP
		);

	PX_ScriptAsmRegistInstr(&compiler,"POPN",PX_SCRIPT_ASM_INSTR_OPCODE_POPN,1,
		PX_SCRIPT_ASM_OPERAND_ACC_TYPE_REG|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_LOCAL|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_GLOBAL|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_INT
		);

	PX_ScriptAsmRegistInstr(&compiler,"ADR",PX_SCRIPT_ASM_INSTR_OPCODE_ADR,2,
		PX_SCRIPT_ASM_OPERAND_ACC_TYPE_REG|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_LOCAL|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_GLOBAL,
		PX_SCRIPT_ASM_OPERAND_ACC_TYPE_LOCAL|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_GLOBAL
		);

	PX_ScriptAsmRegistInstr(&compiler,"RET",PX_SCRIPT_ASM_INSTR_OPCODE_RET,0);
	PX_ScriptAsmRegistInstr(&compiler,"NOP",PX_SCRIPT_ASM_INSTR_OPCODE_NOP,0);
	PX_ScriptAsmRegistInstr(&compiler,"BPX",PX_SCRIPT_ASM_INSTR_OPCODE_BPX,0);

	PX_ScriptAsmRegistInstr(&compiler,"WAIT",PX_SCRIPT_ASM_INSTR_OPCODE_WAIT,1,
		PX_SCRIPT_ASM_OPERAND_ACC_TYPE_REG|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_LOCAL|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_GLOBAL|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_INT
		);

	PX_ScriptAsmRegistInstr(&compiler,"SIGNAL",PX_SCRIPT_ASM_INSTR_OPCODE_SIGNAL,2,
		PX_SCRIPT_ASM_OPERAND_ACC_TYPE_REG|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_LOCAL|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_GLOBAL|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_INT,
		PX_SCRIPT_ASM_OPERAND_ACC_TYPE_REG|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_LOCAL|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_GLOBAL|PX_SCRIPT_ASM_OPERAND_ACC_TYPE_INT
		);

	if(!PX_LexerLoadSourceFromMemory(&compiler.lexer,asmcode))
	{
		goto __ERROR;
	}

	PX_LexerSetTokenCase(&compiler.lexer,PX_LEXER_LEXEME_CASE_UPPER);

	lexerstate=PX_LexerGetState(&compiler.lexer);
	if(!PX_ScriptAsmScan(&compiler))
		goto __ERROR;
	PX_LexerSetState(lexerstate);
	if(!PX_ScriptAsmCc(&compiler))
		goto __ERROR;

	PX_ScriptAsmLink(&compiler);

	//Header
	woffset=sizeof(PX_SCRIPT_ASM_HEADER);
	px_memset(&header,0,sizeof(header));
	header.magic=*((px_dword *)"PASM");
	header.stacksize=compiler.stacksize;
	header.globalsize=compiler.globalsize;
	header.threadcount=compiler.threadcount;
	header.oftfunc=woffset;
	header.funcCount=PX_ScriptAsmGetFuncCount(&compiler);
	woffset+=header.funcCount*sizeof(PX_SCRIPT_EXPORT_FUNCTION);
	header.ofthost=woffset;
	header.hostCount=compiler.HostTable.size;
	woffset+=header.hostCount*sizeof(PX_SCRIPT_ASM_HOST_NODE);
	header.oftString=woffset;
	header.stringSize=PX_ScriptAsmStringSize(&compiler);
	woffset+=header.stringSize;
	header.oftmem=woffset;
	header.memsize=PX_ScriptAsmMemorySize(&compiler);
	woffset+=header.memsize;
	header.oftbin=woffset;

	PX_MemoryCat(binmemory,&header,sizeof(header));
	for (i=0;i<compiler.LabelTable.size;i++)
	{
		if (PX_LISTAT(PX_SCRIPT_ASM_LABEL_NODE,&compiler.LabelTable,i)->bfunction)
		{
			px_memset(expFunc.name,0,sizeof(expFunc.name));
			px_strcpy(expFunc.name,PX_LISTAT(PX_SCRIPT_ASM_LABEL_NODE,&compiler.LabelTable,i)->mnemonic,sizeof(expFunc.name));
			expFunc.Addr=PX_LISTAT(PX_SCRIPT_ASM_LABEL_NODE,&compiler.LabelTable,i)->binaddr;
			PX_MemoryCat(binmemory,&expFunc,sizeof(expFunc));
		}
	}
	
	for (i=0;i<compiler.HostTable.size;i++)
	{
			px_memset(host.name,0,sizeof(host.name));
			px_strcpy(host.name,PX_LISTAT(PX_SCRIPT_ASM_HOST_NODE,&compiler.HostTable,i)->name,sizeof(host.name));
			host.map=PX_NULL;
			expFunc.Addr=0;
			PX_MemoryCat(binmemory,&host,sizeof(host));
	}

	for (i=0;i<compiler.StringTable.size;i++)
	{
		pbuf=PX_LISTAT(PX_SCRIPT_ASM_STRING_NODE,&compiler.StringTable,i)->str.buffer;
		PX_MemoryCat(binmemory,pbuf,px_strlen(pbuf)+1);
	}

	for (i=0;i<compiler.memoriesTable.size;i++)
	{
		bufsize=PX_LISTAT(PX_SCRIPT_ASM_MEMORY_NODE,&compiler.memoriesTable,i)->mem.usedsize;
		pbuf=(px_char *)PX_LISTAT(PX_SCRIPT_ASM_MEMORY_NODE,&compiler.memoriesTable,i)->mem.buffer;
		PX_MemoryCat(binmemory,&bufsize,sizeof(px_dword));
		PX_MemoryCat(binmemory,pbuf,bufsize);
	}

	
	binsize=0;
	for (i=0;i<compiler.StreamTable.size;i++)
	{
		pinstrs=PX_VECTORAT(PX_SCRIPT_ASM_INSTR_BIN,&compiler.StreamTable,i);

		PX_MemoryCat(binmemory,&pinstrs->opCode,1);
		PX_MemoryCat(binmemory,pinstrs->optype,3);
		PX_MemoryCat(binmemory,pinstrs->param,pinstrs->opcount*sizeof(px_dword));
		binsize+=pinstrs->opcount*sizeof(px_dword)+4;
	}
	pheader=(PX_SCRIPT_ASM_HEADER *)binmemory->buffer;
	pheader->binsize=binsize;

	pheader->CRC=PX_crc32(binmemory->buffer+8,binmemory->usedsize-8);

	//pTexpfunc=(PX_SCRIPT_EXPORT_FUNCTION *)(binmemory->buffer+header.oftfunc);

	PX_LexerFree(&compiler.lexer);
	//link
	PX_ListFree(&compiler.LabelTable);
	PX_ListFree(&compiler.HostTable);
	for (i=0;i<compiler.assumeTable.size;i++)
	{
		PX_StringFree(&PX_LISTAT(PX_SCRIPT_ASM_ASSUME_NODE,&compiler.assumeTable,i)->name);
		PX_StringFree(&PX_LISTAT(PX_SCRIPT_ASM_ASSUME_NODE,&compiler.assumeTable,i)->assume);
	}
	PX_ListFree(&compiler.assumeTable);
	for (i=0;i<compiler.StringTable.size;i++)
	{
		PX_StringFree(&PX_LISTAT(PX_SCRIPT_ASM_STRING_NODE,&compiler.StringTable,i)->str);
	}
	PX_ListFree(&compiler.StringTable);
	for (i=0;i<compiler.memoriesTable.size;i++)
	{
		PX_MemoryFree(&PX_LISTAT(PX_SCRIPT_ASM_MEMORY_NODE,&compiler.memoriesTable,i)->mem);
	}
	PX_ListFree(&compiler.memoriesTable);

	PX_ListFree(&compiler.GrammarInstrTable);
	PX_VectorFree(&compiler.StreamTable);
	
	return PX_TRUE;

	__ERROR:
	//free compiler
	    //lexer
	PX_LexerFree(&compiler.lexer);
	//link
	PX_ListFree(&compiler.LabelTable);
	PX_ListFree(&compiler.HostTable);
	for (i=0;i<compiler.assumeTable.size;i++)
	{
		PX_StringFree(&PX_LISTAT(PX_SCRIPT_ASM_ASSUME_NODE,&compiler.assumeTable,i)->name);
		PX_StringFree(&PX_LISTAT(PX_SCRIPT_ASM_ASSUME_NODE,&compiler.assumeTable,i)->assume);
	}
	PX_ListFree(&compiler.assumeTable);
	for (i=0;i<compiler.StringTable.size;i++)
	{
		PX_StringFree(&PX_LISTAT(PX_SCRIPT_ASM_STRING_NODE,&compiler.StringTable,i)->str);
	}
	PX_ListFree(&compiler.StringTable);
	for (i=0;i<compiler.memoriesTable.size;i++)
	{
		PX_MemoryFree(&PX_LISTAT(PX_SCRIPT_ASM_MEMORY_NODE,&compiler.memoriesTable,i)->mem);
	}
	PX_ListFree(&compiler.memoriesTable);

	PX_ListFree(&compiler.GrammarInstrTable);
	PX_VectorFree(&compiler.StreamTable);
	return PX_FALSE;

}

px_bool PX_ScriptAsmOptimization(px_string *asmcode)
{
	px_bool opt=PX_FALSE;
	
	
	do{
	opt=PX_FALSE;
	opt=opt|PX_StringTrimer_Solve(asmcode,"SUB SP,0\n","");
	opt=opt|PX_StringTrimer_Solve(asmcode,"SUB %1,0\n","");
	opt=opt|PX_StringTrimer_Solve(asmcode,"ADD %1,0\n","");
	opt=opt|PX_StringTrimer_Solve(asmcode,"MUL %1,1\n","");
	opt=opt|PX_StringTrimer_Solve(asmcode,"PUSH %1\nPOP %1\n","");
	opt=opt|PX_StringTrimer_Solve(asmcode,"RET\nRET\n","RET\n");
	opt=opt|PX_StringTrimer_Solve(asmcode,"PUSH %1\nPOP %2\n","MOV %2,%1\n");
	opt=opt|PX_StringTrimer_Solve(asmcode,"MOV R%1,%2\nMOV %3,R%1\n","MOV %3,%2\n");
	opt=opt|PX_StringTrimer_Solve(asmcode,"MOV R2,0\nFLT R2\n","MOV R2,0.0\n");
	opt=opt|PX_StringTrimer_Solve(asmcode,"PUSH R1\nMOV R2,%1\nPOP R1\n","MOV R2,%1\n");
	}while(opt);

	return PX_TRUE;
}

px_char * PX_ScriptASMCompilerError()
{
	return PX_Script_ASMError;
}
