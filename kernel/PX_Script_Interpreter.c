#include "PX_Script_Interpreter.h"

static px_char *PX_Script_Keywords[]={"IF","ELSE","SWITCH","CASE","WHILE","FOR","BREAK","RETURN","STRUCT","FUNCTION","EXPORT","HOST","INT","FLOAT","STRING","MEMORY","_BOOT","RETURN","_ASM"};
static px_char PX_Script_InterpreterError[256];

px_void PX_ScriptTranslatorError(px_lexer *lexer,px_char *info)
{
	if (lexer)
	{
		lexer->Sources[lexer->SourceOffset]='\0';
		PX_Script_InterpreterError[0]='\0';
		if (lexer->SourceOffset>200)
		{
			PX_strcpy(PX_Script_InterpreterError,lexer->Sources+lexer->SourceOffset-200,200);	
		}
		else
		{
			PX_strcpy(PX_Script_InterpreterError,lexer->Sources,200);	
		}
	}
	PX_LOG(info);
}

px_void PX_ScriptParserClearStack(PX_SCRIPT_Analysis *analysis)
{
	int i;
	PX_SCRIPT_VARIABLES *pvar;
	for (i=0;i<analysis->v_variablesStackTable.size;i++)
	{
		pvar=PX_VECTORAT(PX_SCRIPT_VARIABLES,&analysis->v_variablesStackTable,i);
		if(pvar->bInitialized)
			PX_StringFree(&pvar->GlobalInitializeValue);

		PX_StringFree(&pvar->Mnemonic);
	}
	PX_VectorClear(&analysis->v_variablesStackTable);
	analysis->currentAllocStackSize=0;
}

PX_LEXER_LEXEME_TYPE PX_ScriptTranslatorNextToken(px_lexer *lexer)
{
	PX_LEXER_LEXEME_TYPE type;
	while ((type= PX_LexerGetNextLexeme(lexer))==PX_LEXER_LEXEME_TYPE_SPACER);
	return type;
}
PX_LEXER_LEXEME_TYPE PX_ScriptTranslatorNextTokenSN(px_lexer *lexer)
{
	PX_LEXER_LEXEME_TYPE type;
	while ((type= PX_LexerGetNextLexeme(lexer))==PX_LEXER_LEXEME_TYPE_SPACER||type==PX_LEXER_LEXEME_TYPE_NEWLINE);

	return type;
}
px_bool PX_ScriptCompilerInitialize(PX_SCRIPT_LIBRARY *lib,px_memorypool *mp)
{
	lib->mp=mp;
	PX_VectorInitialize(mp,&lib->codeLibraries,sizeof(PX_SCRIPT_CODE),1);
	return PX_TRUE;
}
px_void PX_ScriptCompilerFree(PX_SCRIPT_LIBRARY *lib)
{
	int i;
	for (i=0;i<lib->codeLibraries.size;i++)
	{
		PX_StringFree(&PX_VECTORAT(PX_SCRIPT_CODE,&lib->codeLibraries,i)->name);
		PX_StringFree(&PX_VECTORAT(PX_SCRIPT_CODE,&lib->codeLibraries,i)->code);
	}
	PX_VectorFree(&lib->codeLibraries);
}
px_bool PX_ScriptCompilerLoad(PX_SCRIPT_LIBRARY *lib,const px_char *code)
{
	px_lexer lexer;
	PX_SCRIPT_CODE scode;
	int i;
	scode.bInclude=PX_FALSE;
	
	
	PX_LexerInitialize(&lexer,lib->mp);
	PX_LexerRegisterComment(&lexer,"//","\n");
	PX_LexerRegisterComment(&lexer,"/*","*/");
	PX_LexerRegisterDelimiter(&lexer,',');
	PX_LexerRegisterSpacer(&lexer,' ');
	PX_LexerRegisterSpacer(&lexer,'\t');
	PX_LexerRegisterContainer(&lexer,"\"","\"");
	PX_LexerSetTokenCase(&lexer,PX_LEXER_LEXEME_CASE_UPPER);

	if(!PX_LexerLoadSourceFromMemory(&lexer,code))
	{
		PX_LexerFree(&lexer);
		return PX_FALSE;
	}
	if(PX_LexerGetNextLexeme(&lexer)!=PX_LEXER_LEXEME_TYPE_TOKEN)
	{
		PX_LexerFree(&lexer);
		return PX_FALSE;
	}
	if (!PX_strequ(lexer.CurLexeme.buffer,"#NAME"))
	{
		PX_LexerFree(&lexer);
		return PX_FALSE;
	}

	if(PX_LexerGetNextLexeme(&lexer)!=PX_LEXER_LEXEME_TYPE_SPACER)
	{
		PX_LexerFree(&lexer);
		return PX_FALSE;
	}
	if(PX_LexerGetNextLexeme(&lexer)!=PX_LEXER_LEXEME_TYPE_CONATINER)
	{
		PX_LexerFree(&lexer);
		return PX_FALSE;
	}
	PX_StringInitialize(lib->mp,&scode.code);
	PX_StringInitialize(lib->mp,&scode.name);

	
	PX_StringCat(&scode.name,lexer.CurLexeme.buffer);
	PX_LexerGetIncludedString(&lexer,&scode.name);

	if(PX_LexerGetNextLexeme(&lexer)!=PX_LEXER_LEXEME_TYPE_NEWLINE)
	{
		PX_StringFree(&scode.name);
		 PX_StringFree(&scode.code);
		PX_LexerFree(&lexer);
		return PX_FALSE;
	}

	PX_StringCat(&scode.code,lexer.Sources+lexer.SourceOffset);
	PX_LexerFree(&lexer);	


	for (i=0;i<lib->codeLibraries.size;i++)
	{
		if (PX_strequ(scode.name.buffer,PX_VECTORAT(PX_SCRIPT_CODE,&lib->codeLibraries,i)->name.buffer))
		{
			 PX_StringFree(&scode.name);
			 PX_StringFree(&scode.code);
			 return PX_FALSE;
		}
	}

	PX_VectorPushback(&lib->codeLibraries,&scode);

	return PX_TRUE;
}
static px_bool PX_ScriptParseInclude(px_string *codes,PX_SCRIPT_LIBRARY *lib,const px_char *name)
{
	px_lexer lexer;
	PX_LEXER_STATE lexerState;
	px_int allocsize;
	px_int quotes,singleQuotes,i;
	px_bool bfound;
	px_char *exchangeBuffer;
	PX_LEXER_LEXEME_TYPE type;

	PX_LexerInitialize(&lexer,lib->mp);
	PX_LexerRegisterDelimiter(&lexer,',');
	PX_LexerRegisterDelimiter(&lexer,';');
	PX_LexerRegisterDelimiter(&lexer,'+');
	PX_LexerRegisterDelimiter(&lexer,'-');
	PX_LexerRegisterDelimiter(&lexer,'*');
	PX_LexerRegisterDelimiter(&lexer,'/');
	PX_LexerRegisterDelimiter(&lexer,'%');
	PX_LexerRegisterDelimiter(&lexer,'&');
	PX_LexerRegisterDelimiter(&lexer,'^');
	PX_LexerRegisterDelimiter(&lexer,'~');
	PX_LexerRegisterDelimiter(&lexer,'(');
	PX_LexerRegisterDelimiter(&lexer,')');
	PX_LexerRegisterDelimiter(&lexer,'!');
	PX_LexerRegisterDelimiter(&lexer,'=');
	PX_LexerRegisterDelimiter(&lexer,'>');
	PX_LexerRegisterDelimiter(&lexer,'<');
	PX_LexerRegisterDelimiter(&lexer,'{');
	PX_LexerRegisterDelimiter(&lexer,'}');
	PX_LexerRegisterDelimiter(&lexer,'[');
	PX_LexerRegisterDelimiter(&lexer,']');
	PX_LexerRegisterSpacer(&lexer,' ');
	PX_LexerRegisterSpacer(&lexer,'\t');
	quotes=PX_LexerRegisterContainer(&lexer,"\"","\"");
	PX_LexerRegisterContainerTransfer(&lexer,quotes,'\\');
	singleQuotes=PX_LexerRegisterContainer(&lexer,"\'","\'");
	PX_LexerSetTokenCase(&lexer,PX_LEXER_LEXEME_CASE_UPPER);


	if(!PX_LexerSetSourcePointer(&lexer,codes->buffer))
		goto _ERROR;

	while (PX_TRUE)
	{
		
		lexerState=PX_LexerGetState(&lexer);
		type=PX_ScriptTranslatorNextToken(&lexer);
		if (type==PX_LEXER_LEXEME_TYPE_ERR)
		{
			PX_ScriptTranslatorError(&lexer,"lexer error.");
			goto _ERROR;
		}
		if (type==PX_LEXER_LEXEME_TYPE_END)
		{
			break;
		}
		if (type==PX_LEXER_LEXEME_TYPE_TOKEN)
		{
			if (PX_strequ(lexer.CurLexeme.buffer,"#INCLUDE"))
			{
					if (PX_ScriptTranslatorNextToken(&lexer)==PX_LEXER_LEXEME_TYPE_CONATINER)
					{
						if (PX_LexerGetCurrentContainerType(&lexer)!=quotes)
						{
							PX_ScriptTranslatorError(&lexer,"syntactic error: include \"name\" expected but not found.");
							goto _ERROR;
						}
						PX_LexerGetIncludedString(&lexer,&lexer.CurLexeme);
						bfound=PX_FALSE;
						for (i=0;i<lib->codeLibraries.size;i++)
						{
							if (PX_strequ(lexer.CurLexeme.buffer,PX_VECTORAT(PX_SCRIPT_CODE,&lib->codeLibraries,i)->name.buffer))
							{
								if (PX_VECTORAT(PX_SCRIPT_CODE,&lib->codeLibraries,i)->bInclude!=PX_FALSE)
								{
									PX_ScriptTranslatorError(&lexer,"syntactic error: reduplicate included-name.");
									goto _ERROR;
								}
								PX_VECTORAT(PX_SCRIPT_CODE,&lib->codeLibraries,i)->bInclude=PX_TRUE;
								bfound=PX_TRUE;
								

								if ((type=PX_ScriptTranslatorNextToken(&lexer))!=PX_LEXER_LEXEME_TYPE_NEWLINE)
								{
									if (type==PX_LEXER_LEXEME_TYPE_END)
									{
										PX_ScriptTranslatorError(&lexer,"syntactic error:include on last line without newline");
										goto _ERROR;
									}
									else
									{
										PX_ScriptTranslatorError(&lexer,"syntactic error:new line expected but not found.");
										goto _ERROR;
									}
									
								}
								allocsize=lexerState.offset+PX_strlen(PX_VECTORAT(PX_SCRIPT_CODE,&lib->codeLibraries,i)->code.buffer)+PX_strlen(lexer.Sources+PX_LexerGetState(&lexer).offset-1)+1;
								exchangeBuffer=(px_char *)MP_Malloc(lib->mp,allocsize);

								if (exchangeBuffer==PX_NULL)
								{
									PX_ScriptTranslatorError(&lexer,"memory error.");
									goto _ERROR;
								}

								PX_memcpy(exchangeBuffer,codes->buffer,lexerState.offset);
								exchangeBuffer[lexerState.offset]=0;
								PX_strcat(exchangeBuffer,PX_VECTORAT(PX_SCRIPT_CODE,&lib->codeLibraries,i)->code.buffer);
								PX_strcat(exchangeBuffer,lexer.Sources+PX_LexerGetState(&lexer).offset-1);

								PX_StringClear(codes);
								PX_StringCat(codes,exchangeBuffer);

								PX_LexerSetSourcePointer(&lexer,codes->buffer);
								PX_LexerSetState(lexerState);

								MP_Free(lib->mp,exchangeBuffer);

								break;

							}
						}
						if (!bfound)
						{
							PX_ScriptTranslatorError(&lexer,"include error: source not found.");
							goto _ERROR;
						}
					}
					else
					{
						PX_ScriptTranslatorError(&lexer,"syntactic error:include-name expected but not found.");
						goto _ERROR;
					}
			}
		}
	}
	PX_LexerSetSourcePointer(&lexer,PX_NULL);
	PX_LexerFree(&lexer);
	return PX_TRUE;
_ERROR:
	PX_LexerFree(&lexer);
	PX_LexerSetSourcePointer(&lexer,PX_NULL);
	return PX_FALSE;
	
}
typedef struct  
{
	px_string name;
	px_string token;
}PX_SCRIPT_TRANSLATOR_DEFINE_ST;
static px_bool PX_ScriptParseDefine(px_string *codes,PX_SCRIPT_LIBRARY *lib,const px_char *name)
{
	px_lexer lexer;
	px_int startIndex;
	px_int quotes,singleQuotes,i;
	PX_LEXER_LEXEME_TYPE type;
	px_vector defines;
	PX_SCRIPT_TRANSLATOR_DEFINE_ST defst;

	PX_VectorInitialize(lib->mp,&defines,sizeof(PX_SCRIPT_TRANSLATOR_DEFINE_ST),32);

	PX_LexerInitialize(&lexer,lib->mp);
	PX_LexerRegisterDelimiter(&lexer,',');
	PX_LexerRegisterDelimiter(&lexer,';');
	PX_LexerRegisterDelimiter(&lexer,'+');
	PX_LexerRegisterDelimiter(&lexer,'-');
	PX_LexerRegisterDelimiter(&lexer,'*');
	PX_LexerRegisterDelimiter(&lexer,'/');
	PX_LexerRegisterDelimiter(&lexer,'%');
	PX_LexerRegisterDelimiter(&lexer,'&');
	PX_LexerRegisterDelimiter(&lexer,'^');
	PX_LexerRegisterDelimiter(&lexer,'~');
	PX_LexerRegisterDelimiter(&lexer,'(');
	PX_LexerRegisterDelimiter(&lexer,')');
	PX_LexerRegisterDelimiter(&lexer,'!');
	PX_LexerRegisterDelimiter(&lexer,'=');
	PX_LexerRegisterDelimiter(&lexer,'>');
	PX_LexerRegisterDelimiter(&lexer,'<');
	PX_LexerRegisterDelimiter(&lexer,'{');
	PX_LexerRegisterDelimiter(&lexer,'}');
	PX_LexerRegisterDelimiter(&lexer,'[');
	PX_LexerRegisterDelimiter(&lexer,']');
	PX_LexerRegisterSpacer(&lexer,' ');
	PX_LexerRegisterSpacer(&lexer,'\t');
	quotes=PX_LexerRegisterContainer(&lexer,"\"","\"");
	PX_LexerRegisterContainerTransfer(&lexer,quotes,'\\');
	singleQuotes=PX_LexerRegisterContainer(&lexer,"\'","\'");
	PX_LexerSetTokenCase(&lexer,PX_LEXER_LEXEME_CASE_UPPER);


	if(!PX_LexerSetSourcePointer(&lexer,codes->buffer))
		goto _ERROR;

	while (PX_TRUE)
	{
		type=PX_ScriptTranslatorNextToken(&lexer);
		if (type==PX_LEXER_LEXEME_TYPE_ERR)
		{
			PX_ScriptTranslatorError(&lexer,"lexer error.");
			goto _ERROR;
		}
		if (type==PX_LEXER_LEXEME_TYPE_END)
		{
			break;
		}
		if (type==PX_LEXER_LEXEME_TYPE_TOKEN)
		{
			if (PX_strequ(lexer.CurLexeme.buffer,"#DEFINE"))
			{
				startIndex=lexer.SourceOffset-7;
				if (PX_ScriptTranslatorNextToken(&lexer)!=PX_LEXER_LEXEME_TYPE_TOKEN)
				{
					PX_ScriptTranslatorError(&lexer,"syntactic error:include-name expected but not found.");
					goto _ERROR;
				}

				

				PX_StringInitialize(lib->mp,&defst.name);
				PX_StringCat(&defst.name,lexer.CurLexeme.buffer);
				
				if (PX_LexerGetNextLexeme(&lexer)!=PX_LEXER_LEXEME_TYPE_SPACER)
				{
					PX_ScriptTranslatorError(&lexer,"syntactic error:spacer expected but not found.");
					goto _ERROR;
				}

				PX_StringInitialize(lib->mp,&defst.token);

				if (lexer.Sources[lexer.SourceOffset]==' ')
				{
					lexer.SourceOffset++;
				}

				while (PX_TRUE)
				{
					if (lexer.Sources[lexer.SourceOffset]!='\n'&&lexer.Sources[lexer.SourceOffset]!='\0'&&lexer.Sources[lexer.SourceOffset]!='\r')
					{
						PX_StringCatChar(&defst.token,lexer.Sources[lexer.SourceOffset]);
						lexer.SourceOffset++;
					}
					else
						break;
				}
				PX_VectorPushback(&defines,&defst);

				PX_StringReplaceRange(codes,startIndex,lexer.SourceOffset,"");
				PX_LexerSetSourcePointer(&lexer,codes->buffer);
				lexer.SourceOffset=startIndex;
			}
		}
	}

	lexer.SourceOffset=0;

	while (PX_TRUE)
	{
		type=PX_ScriptTranslatorNextToken(&lexer);
		if (type==PX_LEXER_LEXEME_TYPE_ERR)
		{
			PX_ScriptTranslatorError(&lexer,"lexer error.");
			goto _ERROR;
		}
		if (type==PX_LEXER_LEXEME_TYPE_END)
		{
			break;
		}
		if (type==PX_LEXER_LEXEME_TYPE_TOKEN)
		{
			for (i=0;i<defines.size;i++)
			{
				if (PX_strequ(lexer.CurLexeme.buffer,PX_VECTORAT(PX_SCRIPT_TRANSLATOR_DEFINE_ST,&defines,i)->name.buffer))
				{
					startIndex=lexer.SourceOffset-PX_strlen(lexer.CurLexeme.buffer);
					PX_StringReplaceRange(codes,startIndex,lexer.SourceOffset-1,PX_VECTORAT(PX_SCRIPT_TRANSLATOR_DEFINE_ST,&defines,i)->token.buffer);
					PX_LexerSetSourcePointer(&lexer,codes->buffer);
					lexer.SourceOffset=startIndex+PX_strlen(PX_VECTORAT(PX_SCRIPT_TRANSLATOR_DEFINE_ST,&defines,i)->token.buffer);
					break;
				}
			}
		}
	}
	PX_LexerSetSourcePointer(&lexer,PX_NULL);
	PX_LexerFree(&lexer);
	for (i=0;i<defines.size;i++)
	{
		PX_StringFree(&PX_VECTORAT(PX_SCRIPT_TRANSLATOR_DEFINE_ST,&defines,i)->name);
		PX_StringFree(&PX_VECTORAT(PX_SCRIPT_TRANSLATOR_DEFINE_ST,&defines,i)->token);
	}
	PX_VectorFree(&defines);
	return PX_TRUE;
_ERROR:
	PX_LexerFree(&lexer);
	for (i=0;i<defines.size;i++)
	{
		PX_StringFree(&PX_VECTORAT(PX_SCRIPT_TRANSLATOR_DEFINE_ST,&defines,i)->name);
		PX_StringFree(&PX_VECTORAT(PX_SCRIPT_TRANSLATOR_DEFINE_ST,&defines,i)->token);
	}
	PX_LexerSetSourcePointer(&lexer,PX_NULL);
	PX_VectorFree(&defines);
	return PX_FALSE;
}
static px_bool PX_ScriptParsePretreatment(px_string *codes,PX_SCRIPT_LIBRARY *lib,const px_char *name)
{
	
	if(!PX_ScriptParseInclude(codes,lib,name))
		goto _ERROR;

 	if(!PX_ScriptParseDefine(codes,lib,name))
 		goto _ERROR;

	return PX_TRUE;
_ERROR:
	return PX_FALSE;
}
static px_bool PX_ScriptParseIsValidToken(px_char *token)
{
	px_int i;
	if (token[0]=='\0'||PX_charIsNumeric(token[0]))
	{
		return PX_FALSE;
	}

	for(i=0;i<sizeof(PX_Script_Keywords)/sizeof(px_char *);i++)
	if (PX_strequ(token,PX_Script_Keywords[i]))
	{
		return PX_FALSE;
	}

	while (*token)
	{
		if ((*token>='A'&&*token<='Z')||(*token>='a'&&*token<='z')||*token=='_'||*token==':'||PX_charIsNumeric(*token))
		{
			token++;
			continue;
		}
		return PX_FALSE;
	}
	return PX_TRUE;
}
static px_bool PX_ScriptParseCheckBrackets(px_char *pstr)
{
	px_int big=0,mid=0,lit=0;
	while (*pstr)
	{
		if (*pstr=='{')
		{
			big++;
		}

		if (*pstr=='}')
		{
			big--;
		}

		if (*pstr=='[')
		{
			mid++;
		}

		if (*pstr==']')
		{
			mid--;
		}

		if (*pstr=='(')
		{
			lit++;
		}

		if (*pstr==')')
		{
			lit--;
		}
		if (big<0||lit<0)
		{
			return PX_FALSE;
		}
		pstr++;
	}
	if (big||mid||lit)
	{
		return PX_FALSE;
	}
	return PX_TRUE;
}
static PX_SCRIPT_STRUCT *PX_ScriptParseGetStructByIndex(PX_SCRIPT_Analysis *analysis,px_int index)
{
	if (index>=analysis->v_struct.size||index<0)
	{
		return PX_NULL;
	}

	return PX_VECTORAT(PX_SCRIPT_STRUCT,&analysis->v_struct,index);
}
static px_char PX_ScriptParseGetOpLevel(px_char *op,px_bool binary)
{
	if (PX_strlen(op)==1)
	{
		switch(*op)
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
			if(!binary) return 2;
		case '/':
		case '%':
			return 3;
		case '&':
			if(!binary) return 2;
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
		if (PX_strequ(op,">="))
		{
			return 6;
		}
		if (PX_strequ(op,"<="))
		{
			return 6;
		}
		if (PX_strequ(op,"<<"))
		{
			return 5;
		}
		if (PX_strequ(op,">>"))
		{
			return 5;
		}
		if (PX_strequ(op,"&&"))
		{
			return 11;
		}
		if (PX_strequ(op,"||"))
		{
			return 12;
		}
		if (PX_strequ(op,"=="))
		{
			return 7;
		}
		if (PX_strequ(op,"!="))
		{
			return 7;
		}
		if (PX_strequ(op,"++"))
		{
			return 2;
		}
		if (PX_strequ(op,"--"))
		{
			return 2;
		}
	}
	return -1;
}
static px_bool PX_ScriptParse_AST_PushToken(PX_SCRIPT_Analysis *analysis,px_vector *op,px_vector *tk,PX_SCRIPT_EXPR_STREAM atom,PX_SCRIPT_STRUCT *_inset,PX_SCRIPT_STRUCT **_outset)
{
	px_int i;
	PX_SCRIPT_VARIABLES *pvar;
	PX_SCRIPT_AST_OPERAND operand;
	PX_SCRIPT_SETMEMBER *psetmem;

	//Struct offset
	if (_inset!=PX_NULL)
	{
		for (i=0;i<_inset->members.size;i++)
		{
			psetmem=PX_VECTORAT(PX_SCRIPT_SETMEMBER,&_inset->members,i);
			if (PX_strequ(psetmem->defvar.Mnemonic.buffer,atom.code.buffer))
			{
				*_outset=PX_NULL;
				switch(psetmem->defvar.type)
				{
					case PX_SCRIPT_PARSER_VAR_TYPE_INT:
						{
							operand.operandType=PX_SCRIPT_AST_OPERAND_TYPE_INT;
							operand._oft=psetmem->offset;
							operand.pSet=PX_NULL;
							PX_VectorPushback(tk,&operand);
							return PX_TRUE;
						}
						break;
					case PX_SCRIPT_PARSER_VAR_TYPE_FLOAT:
						{
							operand.operandType=PX_SCRIPT_AST_OPERAND_TYPE_FLOAT;
							operand._oft=psetmem->offset;
							operand.pSet=PX_NULL;
							PX_VectorPushback(tk,&operand);
							return PX_TRUE;
						}
						break;
					case PX_SCRIPT_PARSER_VAR_TYPE_STRING:
						{
							operand.operandType=PX_SCRIPT_AST_OPERAND_TYPE_STRING;
							operand._oft=psetmem->offset;
							operand.pSet=PX_NULL;
							PX_VectorPushback(tk,&operand);
							return PX_TRUE;
						}
						break;
					case PX_SCRIPT_PARSER_VAR_TYPE_MEMORY:
						{
							operand.operandType=PX_SCRIPT_AST_OPERAND_TYPE_MEMORY;
							operand._oft=psetmem->offset;
							operand.pSet=PX_NULL;
							PX_VectorPushback(tk,&operand);
							return PX_TRUE;
						}
						break;
					case PX_SCRIPT_PARSER_VAR_TYPE_STRUCT:
						{
							operand.operandType=PX_SCRIPT_AST_OPERAND_TYPE_STRUCT;
							operand._oft=psetmem->offset;
							operand.pSet=PX_ScriptParseGetStructByIndex(analysis,psetmem->defvar.setIndex);
							*_outset=operand.pSet;
							PX_VectorPushback(tk,&operand);
							return PX_TRUE;
						}
						break;
					case PX_SCRIPT_PARSER_VAR_TYPE_INT_ARRAY:
						{
							operand.operandType=PX_SCRIPT_AST_OPERAND_TYPE_INT_PTR_CONST;
							operand._oft=psetmem->offset;
							operand.pSet=PX_NULL;
							PX_VectorPushback(tk,&operand);
							return PX_TRUE;
						}
						break;
					case PX_SCRIPT_PARSER_VAR_TYPE_FLOAT_ARRAY:
						{
							operand.operandType=PX_SCRIPT_AST_OPERAND_TYPE_FLOAT_PTR_CONST;
							operand._oft=psetmem->offset;
							operand.pSet=PX_NULL;
							PX_VectorPushback(tk,&operand);
							return PX_TRUE;
						}
						break;
					case PX_SCRIPT_PARSER_VAR_TYPE_STRING_ARRAY:
						{
							operand.operandType=PX_SCRIPT_AST_OPERAND_TYPE_STRING_PTR_CONST;
							operand._oft=psetmem->offset;
							operand.pSet=PX_NULL;
							PX_VectorPushback(tk,&operand);
							return PX_TRUE;
						}
						break;
					case PX_SCRIPT_PARSER_VAR_TYPE_MEMORY_ARRAY:
						{
							operand.operandType=PX_SCRIPT_AST_OPERAND_TYPE_MEMORY_PTR_CONST;
							operand._oft=psetmem->offset;
							operand.pSet=PX_NULL;
							PX_VectorPushback(tk,&operand);
							return PX_TRUE;
						}
						break;
					case PX_SCRIPT_PARSER_VAR_TYPE_SET_ARRAY:
						{

							operand.operandType=PX_SCRIPT_AST_OPERAND_TYPE_SET_PTR_CONST;
							operand._oft=psetmem->offset;
							operand.pSet=PX_ScriptParseGetStructByIndex(analysis,psetmem->defvar.setIndex);
							*_outset=operand.pSet;
							PX_VectorPushback(tk,&operand);
							return PX_TRUE;
						}
						break;
					case PX_SCRIPT_PARSER_VAR_TYPE_INT_PTR:
						{
							operand.operandType=PX_SCRIPT_AST_OPERAND_TYPE_INT_PTR;
							operand._oft=psetmem->offset;
							operand.pSet=PX_NULL;
							PX_VectorPushback(tk,&operand);
							return PX_TRUE;
						}
						break;
					case PX_SCRIPT_PARSER_VAR_TYPE_FLOAT_PTR:
						{
							operand.operandType=PX_SCRIPT_AST_OPERAND_TYPE_FLOAT_PTR;
							operand._oft=psetmem->offset;
							operand.pSet=PX_NULL;
							PX_VectorPushback(tk,&operand);
							return PX_TRUE;
						}
						break;
					case PX_SCRIPT_PARSER_VAR_TYPE_STRING_PTR:
						{
							operand.operandType=PX_SCRIPT_AST_OPERAND_TYPE_STRING_PTR;
							operand._oft=psetmem->offset;
							operand.pSet=PX_NULL;
							PX_VectorPushback(tk,&operand);
							return PX_TRUE;
						}
						break;
					case PX_SCRIPT_PARSER_VAR_TYPE_MEMORY_PTR:
						{
							operand.operandType=PX_SCRIPT_AST_OPERAND_TYPE_MEMORY_PTR;
							operand._oft=psetmem->offset;
							operand.pSet=PX_NULL;
							PX_VectorPushback(tk,&operand);
							return PX_TRUE;
						}
						break;
					case PX_SCRIPT_PARSER_VAR_TYPE_STRUCT_PTR:
						{
							operand.operandType=PX_SCRIPT_AST_OPERAND_TYPE_MEMORY_PTR;
							operand._oft=psetmem->offset;
							operand.pSet=PX_ScriptParseGetStructByIndex(analysis,psetmem->defvar.setIndex);
							*_outset=operand.pSet;
							PX_VectorPushback(tk,&operand);
							return PX_TRUE;
						}
						break;
					default:
						break;
				}

			}
		}
		return PX_FALSE;
	}



	if (atom.opclass==PX_SCRIPT_TRANSLATOR_OP_CLASS_VAR)
	{

			for (i=0;i<analysis->v_variablesStackTable.size;i++)
			{
				pvar=PX_VECTORAT(PX_SCRIPT_VARIABLES,&analysis->v_variablesStackTable,i);
				if (PX_strequ(pvar->Mnemonic.buffer,atom.code.buffer))
				{
					operand.region=PX_SCRIPT_VARIABLE_REGION_LOCAL;
					goto _CONTINUE;
				}
			}

			for (i=0;i<analysis->v_variablesGlobalTable.size;i++)
			{
				pvar=PX_VECTORAT(PX_SCRIPT_VARIABLES,&analysis->v_variablesGlobalTable,i);
				if (PX_strequ(pvar->Mnemonic.buffer,atom.code.buffer))
				{
					operand.region=PX_SCRIPT_VARIABLE_REGION_GLOBAL;
					goto _CONTINUE;
				}
			}
			return PX_FALSE;
		
		_CONTINUE:
		switch (pvar->type)
		{
		case PX_SCRIPT_PARSER_VAR_TYPE_INT:
			{
				operand.operandType=PX_SCRIPT_AST_OPERAND_TYPE_INT;
				operand._oft=pvar->BeginIndex;
				operand.pSet=PX_NULL;
				PX_VectorPushback(tk,&operand);
			}
			break;
		case PX_SCRIPT_PARSER_VAR_TYPE_FLOAT:
			{
				operand.operandType=PX_SCRIPT_AST_OPERAND_TYPE_FLOAT;
				operand._oft=pvar->BeginIndex;
				operand.pSet=PX_NULL;
				PX_VectorPushback(tk,&operand);
			}
			break;
		case PX_SCRIPT_PARSER_VAR_TYPE_STRING:
			{
				operand.operandType=PX_SCRIPT_AST_OPERAND_TYPE_STRING;
				operand._oft=pvar->BeginIndex;
				operand.pSet=PX_NULL;
				PX_VectorPushback(tk,&operand);
			}
			break;
		case PX_SCRIPT_PARSER_VAR_TYPE_MEMORY:
			{
				operand.operandType=PX_SCRIPT_AST_OPERAND_TYPE_MEMORY;
				operand._oft=pvar->BeginIndex;
				operand.pSet=PX_NULL;
				PX_VectorPushback(tk,&operand);
			}
			break;
		case PX_SCRIPT_PARSER_VAR_TYPE_STRUCT:
			{
				*_outset=PX_ScriptParseGetStructByIndex(analysis,pvar->setIndex);
				operand.operandType=PX_SCRIPT_AST_OPERAND_TYPE_STRUCT;
				operand._oft=pvar->BeginIndex;
				operand.pSet=PX_ScriptParseGetStructByIndex(analysis,pvar->setIndex);
				PX_VectorPushback(tk,&operand);
			}
			break;

		case PX_SCRIPT_PARSER_VAR_TYPE_INT_ARRAY:
			{
				operand.operandType=PX_SCRIPT_AST_OPERAND_TYPE_INT_PTR_CONST;
				operand._oft=pvar->BeginIndex;
				operand.pSet=PX_NULL;
				PX_VectorPushback(tk,&operand);
			}
			break;
		case PX_SCRIPT_PARSER_VAR_TYPE_FLOAT_ARRAY:
			{
				operand.operandType=PX_SCRIPT_AST_OPERAND_TYPE_FLOAT_PTR_CONST;
				operand._oft=pvar->BeginIndex;
				operand.pSet=PX_NULL;
				PX_VectorPushback(tk,&operand);
			}
			break;
		case PX_SCRIPT_PARSER_VAR_TYPE_STRING_ARRAY:
			{
				operand.operandType=PX_SCRIPT_AST_OPERAND_TYPE_STRING_PTR_CONST;
				operand._oft=pvar->BeginIndex;
				operand.pSet=PX_NULL;
				PX_VectorPushback(tk,&operand);
			}
			break;
		case PX_SCRIPT_PARSER_VAR_TYPE_MEMORY_ARRAY:
			{
				operand.operandType=PX_SCRIPT_AST_OPERAND_TYPE_MEMORY_PTR_CONST;
				operand._oft=pvar->BeginIndex;
				operand.pSet=PX_NULL;
				PX_VectorPushback(tk,&operand);
			}
			break;
		case PX_SCRIPT_PARSER_VAR_TYPE_SET_ARRAY:
			{
				*_outset=PX_ScriptParseGetStructByIndex(analysis,pvar->setIndex);
				operand.operandType=PX_SCRIPT_AST_OPERAND_TYPE_SET_PTR_CONST;
				operand._oft=pvar->BeginIndex;
				operand.pSet=PX_ScriptParseGetStructByIndex(analysis,pvar->setIndex);
				PX_VectorPushback(tk,&operand);
			}
			break;
		case PX_SCRIPT_PARSER_VAR_TYPE_INT_PTR:
			{
				*_outset=PX_NULL;
				operand.operandType=PX_SCRIPT_AST_OPERAND_TYPE_INT_PTR;
				operand._oft=pvar->BeginIndex;
				operand.pSet=PX_NULL;
				PX_VectorPushback(tk,&operand);
			}
			break;
		case PX_SCRIPT_PARSER_VAR_TYPE_FLOAT_PTR:
			{
				*_outset=PX_NULL;
				operand.operandType=PX_SCRIPT_AST_OPERAND_TYPE_FLOAT_PTR;
				operand._oft=pvar->BeginIndex;
				operand.pSet=PX_NULL;
				PX_VectorPushback(tk,&operand);
			}
			break;
		case PX_SCRIPT_PARSER_VAR_TYPE_STRING_PTR:
			{
				*_outset=PX_NULL;
				operand.operandType=PX_SCRIPT_AST_OPERAND_TYPE_STRING_PTR;
				operand._oft=pvar->BeginIndex;
				operand.pSet=PX_NULL;
				PX_VectorPushback(tk,&operand);
			}
			break;
		case PX_SCRIPT_PARSER_VAR_TYPE_MEMORY_PTR:
			{
				*_outset=PX_NULL;
				operand.operandType=PX_SCRIPT_AST_OPERAND_TYPE_MEMORY_PTR;
				operand._oft=pvar->BeginIndex;
				operand.pSet=PX_NULL;
				PX_VectorPushback(tk,&operand);
			}
			break;
		case PX_SCRIPT_PARSER_VAR_TYPE_STRUCT_PTR:
			{
				*_outset=PX_ScriptParseGetStructByIndex(analysis,pvar->setIndex);
				operand.operandType=PX_SCRIPT_AST_OPERAND_TYPE_STRUCT_PTR;
				operand._oft=pvar->BeginIndex;
				operand.pSet=PX_ScriptParseGetStructByIndex(analysis,pvar->setIndex);
				PX_VectorPushback(tk,&operand);
			}
			break;
			default:
				return PX_FALSE;
		}
		
		return PX_TRUE;
	}
	else if(atom.opclass==PX_SCRIPT_TRANSLATOR_OP_CLASS_CONST)
	{
		//Hexadecimal
		if (atom.code.buffer[0]=='0'&&atom.code.buffer[1]=='X')
		{
			operand.region=PX_SCRIPT_VARIABLE_REGION_GLOBAL;
			operand.operandType=PX_SCRIPT_AST_OPERAND_TYPE_INT_CONST;
			operand._int=PX_htoi(atom.code.buffer+2);
			PX_VectorPushback(tk,&operand);
			return PX_TRUE;
		}
		else
		{
			if (!PX_strIsNumeric(atom.code.buffer))
			{
				return PX_FALSE;
			}
			operand.region=PX_SCRIPT_VARIABLE_REGION_GLOBAL;
			if (PX_strIsFloat(atom.code.buffer))
			{
				operand.operandType=PX_SCRIPT_AST_OPERAND_TYPE_FLOAT_CONST;
				operand._float=PX_atof(atom.code.buffer);
				PX_VectorPushback(tk,&operand);
				return PX_TRUE;
			}
			else
			{
				operand.operandType=PX_SCRIPT_AST_OPERAND_TYPE_INT_CONST;
				operand._int=PX_atoi(atom.code.buffer);
				PX_VectorPushback(tk,&operand);
				return PX_TRUE;
			}
		}
		
	}
	else if(atom.opclass==PX_SCRIPT_TRANSLATOR_OP_CLASS_CONTAINER)
	{
		operand.region=PX_SCRIPT_VARIABLE_REGION_GLOBAL;
		if(atom.code.buffer[0]=='"')
		{
			operand.operandType=PX_SCRIPT_AST_OPERAND_TYPE_STRING_CONST;
			operand._contaniner=atom.code.buffer;
		}
		else if(atom.code.buffer[0]=='\'')
		{
			if (atom.code.buffer[2]!='\'')
			{
				return PX_FALSE;
			}
			operand.operandType=PX_SCRIPT_AST_OPERAND_TYPE_INT_CONST;
			operand._int=atom.code.buffer[1];
		}
		else
		{
			operand.operandType=PX_SCRIPT_AST_OPERAND_TYPE_MEMORY_CONST;
			operand._contaniner=atom.code.buffer;
		}
		PX_VectorPushback(tk,&operand);
		return PX_TRUE;
	}
	else
	{
		return PX_FALSE;
	}
}
static px_bool PX_ScriptParseAST_PushOpcode(PX_SCRIPT_Analysis *analysis,px_vector *op,PX_SCRIPT_EXPR_STREAM atom)
{
	PX_SCRIPT_AST_OPCODE opc;
	switch(atom.type)
	{
	case PX_SCRIPT_TRANSLATOR_EXPRESSION_OP_ADD:
		 opc.operandType=PX_SCRIPT_AST_OPCODE_TYPE_ADD;
		 break;
	case PX_SCRIPT_TRANSLATOR_EXPRESSION_OP_INC:
		opc.operandType=PX_SCRIPT_AST_OPCODE_TYPE_INC;
		break;
	case PX_SCRIPT_TRANSLATOR_EXPRESSION_OP_POSITIVE:
		 opc.operandType=PX_SCRIPT_AST_OPCODE_TYPE_POSITIVE;
		 break;
	case PX_SCRIPT_TRANSLATOR_EXPRESSION_OP_SUB:
		 opc.operandType=PX_SCRIPT_AST_OPCODE_TYPE_SUB;
		 break;
	case PX_SCRIPT_TRANSLATOR_EXPRESSION_OP_DEC:
		opc.operandType=PX_SCRIPT_AST_OPCODE_TYPE_DEC;
		break;
	case PX_SCRIPT_TRANSLATOR_EXPRESSION_OP_NEGATIVE:
		opc.operandType=PX_SCRIPT_AST_OPCODE_TYPE_NEGATIVE;
		 break;
	case PX_SCRIPT_TRANSLATOR_EXPRESSION_OP_MUL:
		opc.operandType=PX_SCRIPT_AST_OPCODE_TYPE_MUL;
		break;
	case PX_SCRIPT_TRANSLATOR_EXPRESSION_OP_DIV:
		opc.operandType=PX_SCRIPT_AST_OPCODE_TYPE_DIV;
		break;
	case PX_SCRIPT_TRANSLATOR_EXPRESSION_OP_AND:
		opc.operandType=PX_SCRIPT_AST_OPCODE_TYPE_AND;
		break;		
	case PX_SCRIPT_TRANSLATOR_EXPRESSION_OP_OR:
		opc.operandType=PX_SCRIPT_AST_OPCODE_TYPE_OR;
		break;		
	case PX_SCRIPT_TRANSLATOR_EXPRESSION_OP_NOT:
		opc.operandType=PX_SCRIPT_AST_OPCODE_TYPE_NOT;
		break;				
	case PX_SCRIPT_TRANSLATOR_EXPRESSION_OP_XOR:
		opc.operandType=PX_SCRIPT_AST_OPCODE_TYPE_XOR;
		break;		
	case PX_SCRIPT_TRANSLATOR_EXPRESSION_OP_INV:
		opc.operandType=PX_SCRIPT_AST_OPCODE_TYPE_INV;
		break;	
	case PX_SCRIPT_TRANSLATOR_EXPRESSION_OP_MOD:
		opc.operandType=PX_SCRIPT_AST_OPCODE_TYPE_MOD;
		break;	
	case PX_SCRIPT_TRANSLATOR_EXPRESSION_OP_SHL:
		opc.operandType=PX_SCRIPT_AST_OPCODE_TYPE_SHL;
		break;
	case PX_SCRIPT_TRANSLATOR_EXPRESSION_OP_SHR:
		opc.operandType=PX_SCRIPT_AST_OPCODE_TYPE_SHR;
		break;
	case PX_SCRIPT_TRANSLATOR_EXPRESSION_OP_DOT:
		opc.operandType=PX_SCRIPT_AST_OPCODE_TYPE_DOT;
		break;
	case PX_SCRIPT_TRANSLATOR_EXPRESSION_OP_COMMA:
		opc.operandType=PX_SCRIPT_AST_OPCODE_TYPE_COMMA;
		break;
	case PX_SCRIPT_TRANSLATOR_EXPRESSION_OP_MBRACKETBEGIN:
		opc.operandType=PX_SCRIPT_AST_OPCODE_TYPE_IDX;
		break;
	case PX_SCRIPT_TRANSLATOR_EXPRESSION_OP_LBRACKETBEGIN:
		return PX_TRUE;
		break;
	case PX_SCRIPT_TRANSLATOR_EXPRESSION_OP_LARGE:
		opc.operandType=PX_SCRIPT_AST_OPCODE_TYPE_LARGE;
		break;
	case PX_SCRIPT_TRANSLATOR_EXPRESSION_OP_LARGEEUQ:
		opc.operandType=PX_SCRIPT_AST_OPCODE_TYPE_LARGEEQU;
		break;
	case PX_SCRIPT_TRANSLATOR_EXPRESSION_OP_LESS:
		opc.operandType=PX_SCRIPT_AST_OPCODE_TYPE_LESS;
		break;
	case PX_SCRIPT_TRANSLATOR_EXPRESSION_OP_LESSEQU:
		opc.operandType=PX_SCRIPT_AST_OPCODE_TYPE_LESSEQU;
		break;
	case PX_SCRIPT_TRANSLATOR_EXPRESSION_OP_UNEQU:
		opc.operandType=PX_SCRIPT_AST_OPCODE_TYPE_UNEQU;
		break;
	case PX_SCRIPT_TRANSLATOR_EXPRESSION_OP_EQU:
		opc.operandType=PX_SCRIPT_AST_OPCODE_TYPE_EQU;
		break;
	case PX_SCRIPT_TRANSLATOR_EXPRESSION_OP_LGEQU:
		opc.operandType=PX_SCRIPT_AST_OPCODE_TYPE_LGEQU;
		break;
	case PX_SCRIPT_TRANSLATOR_EXPRESSION_OP_LAND:
		opc.operandType=PX_SCRIPT_AST_OPCODE_TYPE_LAND;
		break;
	case PX_SCRIPT_TRANSLATOR_EXPRESSION_OP_LOR:
		opc.operandType=PX_SCRIPT_AST_OPCODE_TYPE_LOR;
		break;

	case PX_SCRIPT_TRANSLATOR_EXPRESSION_OP_PTR:
		opc.operandType=PX_SCRIPT_AST_OPCODE_TYPE_PTR;
		break;
	case PX_SCRIPT_TRANSLATOR_EXPRESSION_OP_ADR:
		opc.operandType=PX_SCRIPT_AST_OPCODE_TYPE_ADR;
		break;
	case PX_SCRIPT_TRANSLATOR_EXPRESSION_OP_OFT:
		opc.operandType=PX_SCRIPT_AST_OPCODE_TYPE_OFT;
		break;
	case PX_SCRIPT_TRANSLATOR_EXPRESSION_OP_INT:
		opc.operandType=PX_SCRIPT_AST_OPCODE_TYPE_INT;
		break;
	case PX_SCRIPT_TRANSLATOR_EXPRESSION_OP_FLOAT:
		opc.operandType=PX_SCRIPT_AST_OPCODE_TYPE_FLOAT;
		break;
	case PX_SCRIPT_TRANSLATOR_EXPRESSION_OP_STRING:
		opc.operandType=PX_SCRIPT_AST_OPCODE_TYPE_STRING;
		break;
	case PX_SCRIPT_TRANSLATOR_EXPRESSION_OP_MEMORY:
		opc.operandType=PX_SCRIPT_AST_OPCODE_TYPE_MEMORY;
		break;
	case PX_SCRIPT_TRANSLATOR_EXPRESSION_OP_STRLEN:
		opc.operandType=PX_SCRIPT_AST_OPCODE_TYPE_STRLEN;
		break;
	case PX_SCRIPT_TRANSLATOR_EXPRESSION_OP_MEMLEN:
		opc.operandType=PX_SCRIPT_AST_OPCODE_TYPE_MEMLEN;
		break;
	default:
		return PX_FALSE;
	}
	opc.level=atom.oplevel;
	PX_VectorPushback(op,&opc);
	return PX_TRUE;
}
static px_bool PX_ScriptParseIsOperandNumericType(PX_SCRIPT_AST_OPERAND ope)
{
	if (ope.operandType==PX_SCRIPT_AST_OPERAND_TYPE_INT||ope.operandType==PX_SCRIPT_AST_OPERAND_TYPE_INT_CONST)
	{
		return PX_TRUE;
	}
	if (ope.operandType==PX_SCRIPT_AST_OPERAND_TYPE_FLOAT||ope.operandType==PX_SCRIPT_AST_OPERAND_TYPE_FLOAT_CONST)
	{
		return PX_TRUE;
	}
	if (ope.operandType==PX_SCRIPT_AST_OPERAND_TYPE_STRING_IDX||ope.operandType==PX_SCRIPT_AST_OPERAND_TYPE_MEMORY_IDX)
	{
		return PX_TRUE;
	}
	return PX_FALSE;
}
static PX_SCRIPT_STRUCT *PX_ScriptParseGetStructInfo(PX_SCRIPT_Analysis *analysis,px_char *name)
{
	px_int i;
	PX_SCRIPT_STRUCT *pset;
	for (i=0;i<analysis->v_struct.size;i++)
	{
		pset=PX_VECTORAT(PX_SCRIPT_STRUCT,&analysis->v_struct,i);
		if (PX_strequ(name,pset->Name.buffer))
		{
			return pset;
		}
	}
	return PX_NULL;
}
static px_int PX_ScriptParseGetSetIndex(PX_SCRIPT_Analysis *analysis,px_char *name)
{
	px_int i;
	PX_SCRIPT_STRUCT *pset;
	for (i=0;i<analysis->v_struct.size;i++)
	{
		pset=PX_VECTORAT(PX_SCRIPT_STRUCT,&analysis->v_struct,i);
		if (PX_strequ(name,pset->Name.buffer))
		{
			return i;
		}
	}
	return -1;
}
static px_bool PX_ScriptParseIsCompareAbleOperand(PX_SCRIPT_AST_OPERAND operand1,PX_SCRIPT_AST_OPERAND operand2)
{
	if (!PX_ScriptParseIsOperandNumericType(operand1)||!PX_ScriptParseIsOperandNumericType(operand2))
	{
		switch(operand1.operandType)
		{
		case PX_SCRIPT_AST_OPERAND_TYPE_MEMORY_CONST:
		case PX_SCRIPT_AST_OPERAND_TYPE_MEMORY:
			{
				switch(operand2.operandType)
				{
				case PX_SCRIPT_AST_OPERAND_TYPE_MEMORY:
				case PX_SCRIPT_AST_OPERAND_TYPE_MEMORY_CONST:
					return PX_TRUE;
					break;
				default:
					return PX_FALSE;
				}
			}
			break;
		case PX_SCRIPT_AST_OPERAND_TYPE_STRING_CONST:
		case PX_SCRIPT_AST_OPERAND_TYPE_STRING:
			{
				switch(operand2.operandType)
				{
				case PX_SCRIPT_AST_OPERAND_TYPE_STRING:
				case PX_SCRIPT_AST_OPERAND_TYPE_STRING_CONST:
					return PX_TRUE;
					break;
				default:
					return PX_FALSE;
				}
			}
			break;
		case PX_SCRIPT_AST_OPERAND_TYPE_STRUCT:
			return PX_FALSE;
		default:
			if (operand1.operandType!=operand2.operandType)
			{
				return PX_FALSE;
			}
		}
	}
	return PX_TRUE;
}
static px_bool PX_ScriptParseAST_MapTokenToR2(PX_SCRIPT_Analysis *analysis,PX_SCRIPT_AST_OPERAND operand,px_string *out)
{
	px_string fmrString;

	switch(operand.operandType)
	{
	case PX_SCRIPT_AST_OPERAND_TYPE_VOID:
		PX_StringCat(out,"MOV R2,0\n");
		break;
		//////////////////////////////////////////////////////////////////////////
		//R2
	case PX_SCRIPT_AST_OPERAND_TYPE_FLOAT:
	case PX_SCRIPT_AST_OPERAND_TYPE_INT:
	case PX_SCRIPT_AST_OPERAND_TYPE_STRING: //string var
	case PX_SCRIPT_AST_OPERAND_TYPE_MEMORY://memory var
	case PX_SCRIPT_AST_OPERAND_TYPE_INT_PTR: //int var[x]
	case PX_SCRIPT_AST_OPERAND_TYPE_FLOAT_PTR://float var[x]
	case PX_SCRIPT_AST_OPERAND_TYPE_STRING_PTR://string var[x]
	case PX_SCRIPT_AST_OPERAND_TYPE_MEMORY_PTR://memory var[x]
	case PX_SCRIPT_AST_OPERAND_TYPE_STRUCT_PTR://struct xx var[x]
		{
			PX_StringInitialize(analysis->mp,&fmrString);

			if(operand.region==PX_SCRIPT_VARIABLE_REGION_LOCAL)
			{
				PX_StringFormat1(&fmrString,"MOV R2,LOCAL[%1]\n",PX_STRINGFORMAT_INT(operand._oft));
				PX_StringCat(out,fmrString.buffer);
			}
			else if(operand.region==PX_SCRIPT_VARIABLE_REGION_GLOBAL)
			{
				PX_StringFormat1(&fmrString,"MOV R2,GLOBAL[%1]\n",PX_STRINGFORMAT_INT(operand._oft));
				PX_StringCat(out,fmrString.buffer);
			}
			else if(operand.region==PX_SCRIPT_VARIABLE_REGION_POP)
			{
				PX_StringSet(&fmrString,"POP R2\n");
				PX_StringCat(out,fmrString.buffer);
				PX_StringSet(&fmrString,"MOV R2,GLOBAL[R2]\n");
				PX_StringCat(out,fmrString.buffer);
			}
			else
			{
				PX_StringFree(&fmrString);
				return PX_FALSE;
			}

			PX_StringFree(&fmrString);
		}
		break;

	case PX_SCRIPT_AST_OPERAND_TYPE_STRUCT:
		{
			PX_StringInitialize(analysis->mp,&fmrString);

			if(operand.region==PX_SCRIPT_VARIABLE_REGION_LOCAL)
			{
				PX_StringFormat1(&fmrString,"MOV R2,BP\nSUB R2,%1\n",PX_STRINGFORMAT_INT(operand._oft));
				PX_StringCat(out,fmrString.buffer);
			}
			else if(operand.region==PX_SCRIPT_VARIABLE_REGION_GLOBAL)
			{
				PX_StringFormat1(&fmrString,"MOV R2,%1\n",PX_STRINGFORMAT_INT(operand._oft));
				PX_StringCat(out,fmrString.buffer);
			}
			else if(operand.region==PX_SCRIPT_VARIABLE_REGION_POP)
			{
				PX_StringSet(&fmrString,"POP R2\n");
				PX_StringCat(out,fmrString.buffer);
			}
			else
			{
				PX_StringFree(&fmrString);
				return PX_FALSE;
			}

			PX_StringFree(&fmrString);
			return PX_TRUE;
		}
		break;

	case PX_SCRIPT_AST_OPERAND_TYPE_INT_PTR_CONST:
	case PX_SCRIPT_AST_OPERAND_TYPE_FLOAT_PTR_CONST:
	case PX_SCRIPT_AST_OPERAND_TYPE_STRING_PTR_CONST:
	case PX_SCRIPT_AST_OPERAND_TYPE_MEMORY_PTR_CONST:
	case PX_SCRIPT_AST_OPERAND_TYPE_SET_PTR_CONST:
		{
			PX_StringInitialize(analysis->mp,&fmrString);

			if(operand.region==PX_SCRIPT_VARIABLE_REGION_GLOBAL)
			{
				PX_StringFormat1(&fmrString,"MOV R2,%1\n",PX_STRINGFORMAT_INT(operand._oft));
				PX_StringCat(out,fmrString.buffer);
			}
			else if(operand.region==PX_SCRIPT_VARIABLE_REGION_LOCAL)
			{
				PX_StringFormat1(&fmrString,"MOV R2,BP\nSUB R2,%1\n",PX_STRINGFORMAT_INT(operand._oft));//
				PX_StringCat(out,fmrString.buffer);
			}
			else if(operand.region==PX_SCRIPT_VARIABLE_REGION_POP)
			{
				PX_StringSet(&fmrString,"POP R2\n");
				PX_StringCat(out,fmrString.buffer);
			}
			else
			{
				PX_StringFree(&fmrString);
				return PX_FALSE;
			}
			PX_StringFree(&fmrString);

		}
		break;

	case PX_SCRIPT_AST_OPERAND_TYPE_INT_CONST:
		{
			PX_StringInitialize(analysis->mp,&fmrString);

			if(operand.region==PX_SCRIPT_VARIABLE_REGION_GLOBAL)
			{
				PX_StringFormat1(&fmrString,"MOV R2,%1\n",PX_STRINGFORMAT_INT(operand._int));
				PX_StringCat(out,fmrString.buffer);
			}
			else if(operand.region==PX_SCRIPT_VARIABLE_REGION_POP)
			{
				PX_StringSet(&fmrString,"POP R2\n");
				PX_StringCat(out,fmrString.buffer);
			}
			else
			{
				PX_StringFree(&fmrString);
				return PX_FALSE;
			}

			PX_StringFree(&fmrString);
		}
		break;

	case PX_SCRIPT_AST_OPERAND_TYPE_FLOAT_CONST:
		{
			PX_StringInitialize(analysis->mp,&fmrString);

			if(operand.region==PX_SCRIPT_VARIABLE_REGION_GLOBAL)
			{
				PX_StringFormat1(&fmrString,"MOV R2,%1.7\n",PX_STRINGFORMAT_FLOAT(operand._float));
				PX_StringCat(out,fmrString.buffer);
			}
			else if(operand.region==PX_SCRIPT_VARIABLE_REGION_POP)
			{
				PX_StringSet(&fmrString,"POP R2\n");
				PX_StringCat(out,fmrString.buffer);
			}
			else
			{
				PX_StringFree(&fmrString);
				return PX_FALSE;
			}
			PX_StringFree(&fmrString);
		}
		break;

	case PX_SCRIPT_AST_OPERAND_TYPE_STRING_CONST:
	case PX_SCRIPT_AST_OPERAND_TYPE_MEMORY_CONST:
		{
			//////////////////////////////////////////////////////////////////////////

			PX_StringInitialize(analysis->mp,&fmrString);

			if(operand.region==PX_SCRIPT_VARIABLE_REGION_GLOBAL)
			{
				PX_StringFormat1(&fmrString,"MOV R2,%1\n",PX_STRINGFORMAT_STRING(operand._contaniner));
				PX_StringCat(out,fmrString.buffer);
			}
			else if(operand.region==PX_SCRIPT_VARIABLE_REGION_POP)
			{
				PX_StringSet(&fmrString,"POP R2\n");
				PX_StringCat(out,fmrString.buffer);
			}
			else
			{
				PX_StringFree(&fmrString);
				return PX_FALSE;
			}
			PX_StringFree(&fmrString);

			//Check operand
			//////////////////////////////////////////////////////////////////////////
		}
		break;

	case PX_SCRIPT_AST_OPERAND_TYPE_STRING_IDX:
		{
			switch(operand.region)
			{
			case PX_SCRIPT_VARIABLE_REGION_LOCAL:
				{
					PX_StringInitialize(analysis->mp,&fmrString);
					if(operand.bAtomPopIndex)
					{
						PX_StringCat(out,"POP R3\n");
						PX_StringFormat1(&fmrString,"STRCHR R2,LOCAL[%1],R3\n",PX_STRINGFORMAT_INT(operand._oft));
						PX_StringCat(out,fmrString.buffer);
					}
					else
					{
						PX_StringFormat2(&fmrString,"STRCHR R2,LOCAL[%1],%2\n",PX_STRINGFORMAT_INT(operand._oft),PX_STRINGFORMAT_INT(operand.bAtomPopIndex));
						PX_StringCat(out,fmrString.buffer);
					}
					PX_StringFree(&fmrString);
				}
				break;
			case PX_SCRIPT_VARIABLE_REGION_GLOBAL:
				{
					PX_StringInitialize(analysis->mp,&fmrString);
					if(operand.bAtomPopIndex)
					{
						PX_StringCat(out,"POP R3\n");
						PX_StringFormat1(&fmrString,"STRCHR R2,GLOBAL[%1],R3\n",PX_STRINGFORMAT_INT(operand._oft));
						PX_StringCat(out,fmrString.buffer);
					}
					else
					{
						PX_StringFormat2(&fmrString,"STRCHR R2,GLOBAL[%1],%2\n",PX_STRINGFORMAT_INT(operand._oft),PX_STRINGFORMAT_INT(operand.bAtomPopIndex));
						PX_StringCat(out,fmrString.buffer);
					}
					PX_StringFree(&fmrString);
				}
				break;
			case PX_SCRIPT_VARIABLE_REGION_POP:
				{
					PX_StringInitialize(analysis->mp,&fmrString);

					if(operand.bAtomPopIndex)
					{
						PX_StringCat(out,"POP R1\n");
						PX_StringCat(out,"POP R3\n");
						PX_StringSet(&fmrString,"STRCHR R2,GLOBAL[R1],R3\n");
						PX_StringCat(out,fmrString.buffer);
					}
					else
					{
						PX_StringCat(out,"POP R1\n");
						PX_StringFormat1(&fmrString,"STRCHR R2,STRSET GLOBAL[R1],%1\n",PX_STRINGFORMAT_INT(operand._oft));
						PX_StringCat(out,fmrString.buffer);
					}	
					PX_StringFree(&fmrString);
				}
				break;
			default:
				return PX_FALSE;
			}
		}
		break;

	case PX_SCRIPT_AST_OPERAND_TYPE_MEMORY_IDX:
		{

			switch(operand.region)
			{
			case PX_SCRIPT_VARIABLE_REGION_LOCAL:
				{
					PX_StringInitialize(analysis->mp,&fmrString);
					if(operand.bAtomPopIndex)
					{
						PX_StringCat(out,"POP R3\n");
						PX_StringFormat1(&fmrString,"MEMBYTE R2,LOCAL[%1],R3\n",PX_STRINGFORMAT_INT(operand._oft));
						PX_StringCat(out,fmrString.buffer);
					}
					else
					{
						PX_StringFormat2(&fmrString,"MEMBYTE R2,LOCAL[%1],%2\n",PX_STRINGFORMAT_INT(operand._oft),PX_STRINGFORMAT_INT(operand.bAtomPopIndex));
						PX_StringCat(out,fmrString.buffer);
					}
					PX_StringFree(&fmrString);
				}
				break;
			case PX_SCRIPT_VARIABLE_REGION_GLOBAL:
				{
					PX_StringInitialize(analysis->mp,&fmrString);
					if(operand.bAtomPopIndex)
					{
						PX_StringCat(out,"POP R3\n");
						PX_StringFormat1(&fmrString,"MEMBYTE R2,GLOBAL[%1],R3\n",PX_STRINGFORMAT_INT(operand._oft));
						PX_StringCat(out,fmrString.buffer);
					}
					else
					{
						PX_StringFormat2(&fmrString,"MEMBYTE R2,GLOBAL[%1],%2\n",PX_STRINGFORMAT_INT(operand._oft),PX_STRINGFORMAT_INT(operand.bAtomPopIndex));
						PX_StringCat(out,fmrString.buffer);
					}
					PX_StringFree(&fmrString);
				}
				break;
			case PX_SCRIPT_VARIABLE_REGION_POP:
				{
					PX_StringInitialize(analysis->mp,&fmrString);

					if(operand.bAtomPopIndex)
					{
						PX_StringCat(out,"POP R1\n");
						PX_StringCat(out,"POP R3\n");
						PX_StringSet(&fmrString,"MEMBYTE R2,GLOBAL[R1],R3\n");
						PX_StringCat(out,fmrString.buffer);
					}
					else
					{
						PX_StringCat(out,"POP R1\n");
						PX_StringFormat1(&fmrString,"MEMBYTE R2,STRSET GLOBAL[R1],%1\n",PX_STRINGFORMAT_INT(operand._oft));
						PX_StringCat(out,fmrString.buffer);
					}	
					PX_StringFree(&fmrString);
				}
				break;
			default:
				return PX_FALSE;
			}

		}
		break;

	default:
		return PX_FALSE;
	}
	return PX_TRUE;
}
static px_bool PX_ScriptParseAST_MapTokenToR1(PX_SCRIPT_Analysis *analysis,PX_SCRIPT_AST_OPERAND operand,px_string *out)
{
	px_string fmrString;

	switch(operand.operandType)
	{
	case PX_SCRIPT_AST_OPERAND_TYPE_VOID:
		PX_StringCat(out,"MOV R1,0\n");
		break;
		//////////////////////////////////////////////////////////////////////////
		//R2
	case PX_SCRIPT_AST_OPERAND_TYPE_FLOAT:
	case PX_SCRIPT_AST_OPERAND_TYPE_INT:
	case PX_SCRIPT_AST_OPERAND_TYPE_STRING: //string var
	case PX_SCRIPT_AST_OPERAND_TYPE_MEMORY://memory var
	case PX_SCRIPT_AST_OPERAND_TYPE_INT_PTR: //int var[x]
	case PX_SCRIPT_AST_OPERAND_TYPE_FLOAT_PTR://float var[x]
	case PX_SCRIPT_AST_OPERAND_TYPE_STRING_PTR://string var[x]
	case PX_SCRIPT_AST_OPERAND_TYPE_MEMORY_PTR://memory var[x]
	case PX_SCRIPT_AST_OPERAND_TYPE_STRUCT_PTR://struct xx var[x]
		{
			PX_StringInitialize(analysis->mp,&fmrString);

			if(operand.region==PX_SCRIPT_VARIABLE_REGION_LOCAL)
			{
				PX_StringFormat1(&fmrString,"MOV R1,LOCAL[%1]\n",PX_STRINGFORMAT_INT(operand._oft));
				PX_StringCat(out,fmrString.buffer);
			}
			else if(operand.region==PX_SCRIPT_VARIABLE_REGION_GLOBAL)
			{
				PX_StringFormat1(&fmrString,"MOV R1,GLOBAL[%1]\n",PX_STRINGFORMAT_INT(operand._oft));
				PX_StringCat(out,fmrString.buffer);
			}
			else if(operand.region==PX_SCRIPT_VARIABLE_REGION_POP)
			{
				PX_StringSet(&fmrString,"POP R1\n");
				PX_StringCat(out,fmrString.buffer);
				PX_StringSet(&fmrString,"MOV R1,GLOBAL[R1]\n");
				PX_StringCat(out,fmrString.buffer);
			}
			else
			{
				PX_StringFree(&fmrString);
				return PX_FALSE;
			}

			PX_StringFree(&fmrString);
		}
		break;

	case PX_SCRIPT_AST_OPERAND_TYPE_STRUCT:
		{
			PX_StringInitialize(analysis->mp,&fmrString);

			if(operand.region==PX_SCRIPT_VARIABLE_REGION_LOCAL)
			{
				PX_StringFormat1(&fmrString,"MOV R1,BP\nSUB R1,%1\n",PX_STRINGFORMAT_INT(operand._oft));
				PX_StringCat(out,fmrString.buffer);
			}
			else if(operand.region==PX_SCRIPT_VARIABLE_REGION_GLOBAL)
			{
				PX_StringFormat1(&fmrString,"MOV R1,%1\n",PX_STRINGFORMAT_INT(operand._oft));
				PX_StringCat(out,fmrString.buffer);
			}
			else if(operand.region==PX_SCRIPT_VARIABLE_REGION_POP)
			{
				PX_StringSet(&fmrString,"POP R1\n");
				PX_StringCat(out,fmrString.buffer);
			}
			else
			{
				PX_StringFree(&fmrString);
				return PX_FALSE;
			}

			PX_StringFree(&fmrString);
			return PX_TRUE;
		}
		break;

	case PX_SCRIPT_AST_OPERAND_TYPE_INT_PTR_CONST:
	case PX_SCRIPT_AST_OPERAND_TYPE_FLOAT_PTR_CONST:
	case PX_SCRIPT_AST_OPERAND_TYPE_STRING_PTR_CONST:
	case PX_SCRIPT_AST_OPERAND_TYPE_MEMORY_PTR_CONST:
	case PX_SCRIPT_AST_OPERAND_TYPE_SET_PTR_CONST:
		{
			PX_StringInitialize(analysis->mp,&fmrString);

			if(operand.region==PX_SCRIPT_VARIABLE_REGION_GLOBAL)
			{
				PX_StringFormat1(&fmrString,"MOV R1,%1\n",PX_STRINGFORMAT_INT(operand._oft));
				PX_StringCat(out,fmrString.buffer);
			}
			else if(operand.region==PX_SCRIPT_VARIABLE_REGION_LOCAL)
			{
				PX_StringFormat1(&fmrString,"MOV R1,BP\nSUB R1,%1\n",PX_STRINGFORMAT_INT(operand._oft));//
				PX_StringCat(out,fmrString.buffer);
			}
			else if(operand.region==PX_SCRIPT_VARIABLE_REGION_POP)
			{
				PX_StringSet(&fmrString,"POP R1\n");
				PX_StringCat(out,fmrString.buffer);
			}
			else
			{
				PX_StringFree(&fmrString);
				return PX_FALSE;
			}
			PX_StringFree(&fmrString);

		}
		break;

	case PX_SCRIPT_AST_OPERAND_TYPE_INT_CONST:
		{
			PX_StringInitialize(analysis->mp,&fmrString);

			if(operand.region==PX_SCRIPT_VARIABLE_REGION_GLOBAL)
			{
				PX_StringFormat1(&fmrString,"MOV R1,%1\n",PX_STRINGFORMAT_INT(operand._int));
				PX_StringCat(out,fmrString.buffer);
			}
			else if(operand.region==PX_SCRIPT_VARIABLE_REGION_POP)
			{
				PX_StringSet(&fmrString,"POP R1\n");
				PX_StringCat(out,fmrString.buffer);
			}
			else
			{
				PX_StringFree(&fmrString);
				return PX_FALSE;
			}

			PX_StringFree(&fmrString);
		}
		break;

	case PX_SCRIPT_AST_OPERAND_TYPE_FLOAT_CONST:
		{
			PX_StringInitialize(analysis->mp,&fmrString);

			if(operand.region==PX_SCRIPT_VARIABLE_REGION_GLOBAL)
			{
				PX_StringFormat1(&fmrString,"MOV R1,%1.7\n",PX_STRINGFORMAT_FLOAT(operand._float));
				PX_StringCat(out,fmrString.buffer);
			}
			else if(operand.region==PX_SCRIPT_VARIABLE_REGION_POP)
			{
				PX_StringSet(&fmrString,"POP R1\n");
				PX_StringCat(out,fmrString.buffer);
			}
			else
			{
				PX_StringFree(&fmrString);
				return PX_FALSE;
			}
			PX_StringFree(&fmrString);
		}
		break;

	case PX_SCRIPT_AST_OPERAND_TYPE_STRING_CONST:
	case PX_SCRIPT_AST_OPERAND_TYPE_MEMORY_CONST:
		{
			//////////////////////////////////////////////////////////////////////////

			PX_StringInitialize(analysis->mp,&fmrString);

			if(operand.region==PX_SCRIPT_VARIABLE_REGION_GLOBAL)
			{
				PX_StringFormat1(&fmrString,"MOV R1,%1\n",PX_STRINGFORMAT_STRING(operand._contaniner));
				PX_StringCat(out,fmrString.buffer);
			}
			else if(operand.region==PX_SCRIPT_VARIABLE_REGION_POP)
			{
				PX_StringSet(&fmrString,"POP R1\n");
				PX_StringCat(out,fmrString.buffer);
			}
			else
			{
				PX_StringFree(&fmrString);
				return PX_FALSE;
			}
			PX_StringFree(&fmrString);

			//Check operand
			//////////////////////////////////////////////////////////////////////////
		}
		break;

	case PX_SCRIPT_AST_OPERAND_TYPE_STRING_IDX:
		{
			switch(operand.region)
			{
			case PX_SCRIPT_VARIABLE_REGION_LOCAL:
				{
					PX_StringInitialize(analysis->mp,&fmrString);
					if(operand.bAtomPopIndex)
					{
						PX_StringCat(out,"POP R3\n");
						PX_StringFormat1(&fmrString,"STRCHR R1,LOCAL[%1],R3\n",PX_STRINGFORMAT_INT(operand._oft));
						PX_StringCat(out,fmrString.buffer);
					}
					else
					{
						PX_StringFormat2(&fmrString,"STRCHR R1,LOCAL[%1],%2\n",PX_STRINGFORMAT_INT(operand._oft),PX_STRINGFORMAT_INT(operand.bAtomPopIndex));
						PX_StringCat(out,fmrString.buffer);
					}
					PX_StringFree(&fmrString);
				}
				break;
			case PX_SCRIPT_VARIABLE_REGION_GLOBAL:
				{
					PX_StringInitialize(analysis->mp,&fmrString);
					if(operand.bAtomPopIndex)
					{
						PX_StringCat(out,"POP R3\n");
						PX_StringFormat1(&fmrString,"STRCHR R1,GLOBAL[%1],R3\n",PX_STRINGFORMAT_INT(operand._oft));
						PX_StringCat(out,fmrString.buffer);
					}
					else
					{
						PX_StringFormat2(&fmrString,"STRCHR R1,GLOBAL[%1],%2\n",PX_STRINGFORMAT_INT(operand._oft),PX_STRINGFORMAT_INT(operand.bAtomPopIndex));
						PX_StringCat(out,fmrString.buffer);
					}
					PX_StringFree(&fmrString);
				}
				break;
			case PX_SCRIPT_VARIABLE_REGION_POP:
				{
					PX_StringInitialize(analysis->mp,&fmrString);

					if(operand.bAtomPopIndex)
					{
						PX_StringCat(out,"POP R1\n");
						PX_StringCat(out,"POP R3\n");
						PX_StringSet(&fmrString,"STRCHR R1,GLOBAL[R1],R3\n");
						PX_StringCat(out,fmrString.buffer);
					}
					else
					{
						PX_StringCat(out,"POP R1\n");
						PX_StringFormat1(&fmrString,"STRCHR R1,STRSET GLOBAL[R1],%1\n",PX_STRINGFORMAT_INT(operand._oft));
						PX_StringCat(out,fmrString.buffer);
					}	
					PX_StringFree(&fmrString);
				}
				break;
			default:
				return PX_FALSE;
			}
		}
		break;

	case PX_SCRIPT_AST_OPERAND_TYPE_MEMORY_IDX:
		{

			switch(operand.region)
			{
			case PX_SCRIPT_VARIABLE_REGION_LOCAL:
				{
					PX_StringInitialize(analysis->mp,&fmrString);
					if(operand.bAtomPopIndex)
					{
						PX_StringCat(out,"POP R3\n");
						PX_StringFormat1(&fmrString,"MEMBYTE R1,LOCAL[%1],R3\n",PX_STRINGFORMAT_INT(operand._oft));
						PX_StringCat(out,fmrString.buffer);
					}
					else
					{
						PX_StringFormat2(&fmrString,"MEMBYTE R1,LOCAL[%1],%2\n",PX_STRINGFORMAT_INT(operand._oft),PX_STRINGFORMAT_INT(operand.bAtomPopIndex));
						PX_StringCat(out,fmrString.buffer);
					}
					PX_StringFree(&fmrString);
				}
				break;
			case PX_SCRIPT_VARIABLE_REGION_GLOBAL:
				{
					PX_StringInitialize(analysis->mp,&fmrString);
					if(operand.bAtomPopIndex)
					{
						PX_StringCat(out,"POP R3\n");
						PX_StringFormat1(&fmrString,"MEMBYTE R1,GLOBAL[%1],R3\n",PX_STRINGFORMAT_INT(operand._oft));
						PX_StringCat(out,fmrString.buffer);
					}
					else
					{
						PX_StringFormat2(&fmrString,"MEMBYTE R1,GLOBAL[%1],%2\n",PX_STRINGFORMAT_INT(operand._oft),PX_STRINGFORMAT_INT(operand.bAtomPopIndex));
						PX_StringCat(out,fmrString.buffer);
					}
					PX_StringFree(&fmrString);
				}
				break;
			case PX_SCRIPT_VARIABLE_REGION_POP:
				{
					PX_StringInitialize(analysis->mp,&fmrString);

					if(operand.bAtomPopIndex)
					{
						PX_StringCat(out,"POP R1\n");
						PX_StringCat(out,"POP R3\n");
						PX_StringSet(&fmrString,"MEMBYTE R1,GLOBAL[R1],R3\n");
						PX_StringCat(out,fmrString.buffer);
					}
					else
					{
						PX_StringCat(out,"POP R1\n");
						PX_StringFormat1(&fmrString,"MEMBYTE R1,STRSET GLOBAL[R1],%1\n",PX_STRINGFORMAT_INT(operand._oft));
						PX_StringCat(out,fmrString.buffer);
					}	
					PX_StringFree(&fmrString);
				}
				break;
			default:
				return PX_FALSE;
			}

		}
		break;

	default:
		return PX_FALSE;
	}

	return PX_TRUE;
}
static px_bool PX_ScriptParseLastInstr_DOT(PX_SCRIPT_Analysis *analysis,px_vector *op,px_vector *tk,px_string *out)
{
	PX_SCRIPT_AST_OPERAND operand1,operand2,*pTop;
	px_string fmrString;
	if (tk->size<2)
	{
		return PX_FALSE;
	}
	operand1=*((PX_SCRIPT_AST_OPERAND *)PX_VECTORAT(PX_SCRIPT_AST_OPERAND,tk,tk->size-2));
	operand2=*((PX_SCRIPT_AST_OPERAND *)PX_VECTORAT(PX_SCRIPT_AST_OPERAND,tk,tk->size-1));

	PX_VectorPop(op);

	if (operand1.operandType!=PX_SCRIPT_AST_OPERAND_TYPE_STRUCT)
	{
		return PX_FALSE;
	}

	if (operand1.region==PX_SCRIPT_VARIABLE_REGION_POP)
	{
		PX_StringInitialize(analysis->mp,&fmrString);
		PX_StringFormat1(&fmrString,"POP R1\nADD R1,%1\nPUSH R1\n",PX_STRINGFORMAT_INT(operand2._oft));
		PX_StringCat(out,fmrString.buffer);
		PX_StringFree(&fmrString);
		operand2._oft=0;
		operand2.region=operand1.region;
	}
	else if (operand1.region==PX_SCRIPT_VARIABLE_REGION_GLOBAL)
	{
		operand2._oft+=operand1._oft;
		operand2.region=operand1.region;
	}
	else if (operand1.region==PX_SCRIPT_VARIABLE_REGION_LOCAL)
	{
		operand2._oft=operand1._oft-operand2._oft;
		operand2.region=operand1.region;
	}
	else
	{
		return PX_FALSE;
	}

	
	PX_VectorPop(tk);

	pTop=(PX_SCRIPT_AST_OPERAND *)PX_VECTORAT(PX_SCRIPT_AST_OPERAND,tk,tk->size-1);
	*pTop=operand2;
	return PX_TRUE;

}
static px_bool PX_ScriptParseLastInstr_EQUAL(PX_SCRIPT_Analysis *analysis,px_vector *op,px_vector *tk,px_string *out)
{
	PX_SCRIPT_AST_OPERAND operandLeft,operandRight,*pTop;
	px_string fmrString;
	if (tk->size<2)
	{
		return PX_FALSE;
	}
	operandLeft=*((PX_SCRIPT_AST_OPERAND *)PX_VECTORAT(PX_SCRIPT_AST_OPERAND,tk,tk->size-2));
	operandRight=*((PX_SCRIPT_AST_OPERAND *)PX_VECTORAT(PX_SCRIPT_AST_OPERAND,tk,tk->size-1));
	
	PX_VectorPop(op);
	PX_VectorPop(tk);
	
	pTop=PX_VECTORLAST(PX_SCRIPT_AST_OPERAND,tk);


	switch(operandRight.operandType)
	{
		//////////////////////////////////////////////////////////////////////////
		//R2
	case PX_SCRIPT_AST_OPERAND_TYPE_INT:
		{
			if (!PX_ScriptParseAST_MapTokenToR2(analysis,operandRight,out))
			{
				return PX_FALSE;
			}

			//Check operand1
			switch (operandLeft.operandType)
			{
			case PX_SCRIPT_AST_OPERAND_TYPE_FLOAT:
			case PX_SCRIPT_AST_OPERAND_TYPE_INT:
				{
					if (operandLeft.operandType==PX_SCRIPT_AST_OPERAND_TYPE_INT)
					{
						pTop->operandType=PX_SCRIPT_AST_OPERAND_TYPE_INT_CONST;
						pTop->region=PX_SCRIPT_VARIABLE_REGION_POP;
						pTop->_oft=0;
					}
					else
					{
						pTop->operandType=PX_SCRIPT_AST_OPERAND_TYPE_FLOAT_CONST;
						pTop->region=PX_SCRIPT_VARIABLE_REGION_POP;
						pTop->_oft=0;
					    PX_StringCat(out,"FLT R2\n");

					}
					switch(operandLeft.region)
					{
					case PX_SCRIPT_VARIABLE_REGION_LOCAL:
						{
							PX_StringInitialize(analysis->mp,&fmrString);
							PX_StringFormat1(&fmrString,"MOV LOCAL[%1],R2\n",PX_STRINGFORMAT_INT(operandLeft._oft));
							PX_StringCat(out,fmrString.buffer);
							PX_StringFormat1(&fmrString,"PUSH LOCAL[%1]\n",PX_STRINGFORMAT_INT(operandLeft._oft));
							PX_StringCat(out,fmrString.buffer);
							PX_StringFree(&fmrString);
						}
						break;
					case PX_SCRIPT_VARIABLE_REGION_GLOBAL:
						{
							PX_StringInitialize(analysis->mp,&fmrString);
							PX_StringFormat1(&fmrString,"MOV GLOBAL[%1],R2\n",PX_STRINGFORMAT_INT(operandLeft._oft));
							PX_StringCat(out,fmrString.buffer);
							PX_StringFormat1(&fmrString,"PUSH GLOBAL[%1]\n",PX_STRINGFORMAT_INT(operandLeft._oft));
							PX_StringCat(out,fmrString.buffer);
							PX_StringFree(&fmrString);
						}
						break;
					case PX_SCRIPT_VARIABLE_REGION_POP:
						{
							PX_StringInitialize(analysis->mp,&fmrString);
							PX_StringCat(out,"POP R1\n");
							PX_StringSet(&fmrString,"MOV GLOBAL[R1],R2\n");
							PX_StringCat(out,fmrString.buffer);
							PX_StringSet(&fmrString,"PUSH GLOBAL[R1]\n");
							PX_StringCat(out,fmrString.buffer);
							PX_StringFree(&fmrString);
						}
						break;
						default:
							return PX_FALSE;
					}
				}
				break;

			case PX_SCRIPT_AST_OPERAND_TYPE_STRING_IDX:
				{
					pTop->operandType=PX_SCRIPT_AST_OPERAND_TYPE_INT_CONST;
					pTop->region=PX_SCRIPT_VARIABLE_REGION_POP;
					pTop->_oft=0;
					switch(operandLeft.region)
					{
					case PX_SCRIPT_VARIABLE_REGION_LOCAL:
						{
							PX_StringInitialize(analysis->mp,&fmrString);
							if(operandLeft.bAtomPopIndex)
							{
								PX_StringCat(out,"POP R3\n");
								PX_StringFormat1(&fmrString,"STRSET LOCAL[%1],R3,R2\n",PX_STRINGFORMAT_INT(operandLeft._oft));
								PX_StringCat(out,fmrString.buffer);
								PX_StringFormat1(&fmrString,"PUSH LOCAL[%1]\n",PX_STRINGFORMAT_INT(operandLeft._oft));
								PX_StringCat(out,fmrString.buffer);
							}
							else
							{
								PX_StringFormat2(&fmrString,"STRSET LOCAL[%1],%2,R2\n",PX_STRINGFORMAT_INT(operandLeft._oft),PX_STRINGFORMAT_INT(operandLeft.AtomIndex));
								PX_StringCat(out,fmrString.buffer);
								PX_StringFormat1(&fmrString,"PUSH LOCAL[%1]\n",PX_STRINGFORMAT_INT(operandLeft._oft));
								PX_StringCat(out,fmrString.buffer);
							}
							PX_StringFree(&fmrString);
						}
						break;
					case PX_SCRIPT_VARIABLE_REGION_GLOBAL:
						{
							PX_StringInitialize(analysis->mp,&fmrString);
							if(operandLeft.bAtomPopIndex)
							{
								PX_StringCat(out,"POP R3\n");
								PX_StringFormat1(&fmrString,"STRSET GLOBAL[%1],R3,R2\n",PX_STRINGFORMAT_INT(operandLeft._oft));
								PX_StringCat(out,fmrString.buffer);
								PX_StringFormat1(&fmrString,"PUSH GLOBAL[%1]\n",PX_STRINGFORMAT_INT(operandLeft._oft));
								PX_StringCat(out,fmrString.buffer);
							}
							else
							{
								PX_StringFormat2(&fmrString,"STRSET GLOBAL[%1],%2,R2\n",PX_STRINGFORMAT_INT(operandLeft._oft),PX_STRINGFORMAT_INT(operandLeft.AtomIndex));
								PX_StringCat(out,fmrString.buffer);
								PX_StringFormat1(&fmrString,"PUSH GLOBAL[%1]\n",PX_STRINGFORMAT_INT(operandLeft._oft));
								PX_StringCat(out,fmrString.buffer);
							}
							PX_StringFree(&fmrString);
						}
						break;
					case PX_SCRIPT_VARIABLE_REGION_POP:
						{
							PX_StringInitialize(analysis->mp,&fmrString);

							if(operandLeft.bAtomPopIndex)
							{
								PX_StringCat(out,"POP R1\n");
								PX_StringCat(out,"POP R3\n");
								PX_StringSet(&fmrString,"STRSET GLOBAL[R1],R3,R2\n");
								PX_StringCat(out,fmrString.buffer);
								PX_StringSet(&fmrString,"PUSH GLOBAL[R1]\n");
								PX_StringCat(out,fmrString.buffer);
							}
							else
							{
								PX_StringCat(out,"POP R1\n");
								PX_StringFormat1(&fmrString,"STRSET GLOBAL[R1],%1,R2\n",PX_STRINGFORMAT_INT(operandLeft._oft));
								PX_StringCat(out,fmrString.buffer);
								PX_StringSet(&fmrString,"PUSH GLOBAL[R1]\n");
								PX_StringCat(out,fmrString.buffer);
							}	
							PX_StringFree(&fmrString);
						}
						break;
						default:
							return PX_FALSE;
					}
				}
				break;

			case PX_SCRIPT_AST_OPERAND_TYPE_MEMORY_IDX:
				{
					pTop->operandType=PX_SCRIPT_AST_OPERAND_TYPE_INT_CONST;
					pTop->region=PX_SCRIPT_VARIABLE_REGION_POP;
					pTop->_oft=0;
					switch(operandLeft.region)
					{
					case PX_SCRIPT_VARIABLE_REGION_LOCAL:
						{
							PX_StringInitialize(analysis->mp,&fmrString);
							if(operandLeft.bAtomPopIndex)
							{
								PX_StringCat(out,"POP R3\n");
								PX_StringFormat1(&fmrString,"MEMSET LOCAL[%1],R3,R2\n",PX_STRINGFORMAT_INT(operandLeft._oft));
								PX_StringCat(out,fmrString.buffer);
								PX_StringFormat1(&fmrString,"PUSH LOCAL[%1]\n",PX_STRINGFORMAT_INT(operandLeft._oft));
								PX_StringCat(out,fmrString.buffer);
							}
							else
							{
								PX_StringFormat2(&fmrString,"MEMSET LOCAL[%1],%2,R2\n",PX_STRINGFORMAT_INT(operandLeft._oft),PX_STRINGFORMAT_INT(operandLeft.AtomIndex));
								PX_StringCat(out,fmrString.buffer);
								PX_StringFormat1(&fmrString,"PUSH LOCAL[%1]\n",PX_STRINGFORMAT_INT(operandLeft._oft));
								PX_StringCat(out,fmrString.buffer);
							}
							PX_StringFree(&fmrString);
						}
						break;
					case PX_SCRIPT_VARIABLE_REGION_GLOBAL:
						{
							PX_StringInitialize(analysis->mp,&fmrString);
							if(operandLeft.bAtomPopIndex)
							{
								PX_StringCat(out,"POP R3\n");
								PX_StringFormat1(&fmrString,"MEMSET GLOBAL[%1],R3,R2\n",PX_STRINGFORMAT_INT(operandLeft._oft));
								PX_StringCat(out,fmrString.buffer);
								PX_StringFormat1(&fmrString,"PUSH GLOBAL[%1]\n",PX_STRINGFORMAT_INT(operandLeft._oft));
								PX_StringCat(out,fmrString.buffer);
							}
							else
							{
								PX_StringFormat2(&fmrString,"MEMSET GLOBAL[%1],%2,R2\n",PX_STRINGFORMAT_INT(operandLeft._oft),PX_STRINGFORMAT_INT(operandLeft.AtomIndex));
								PX_StringCat(out,fmrString.buffer);
								PX_StringFormat1(&fmrString,"PUSH GLOBAL[%1]\n",PX_STRINGFORMAT_INT(operandLeft._oft));
								PX_StringCat(out,fmrString.buffer);
							}

							PX_StringFree(&fmrString);
						}
						break;
					case PX_SCRIPT_VARIABLE_REGION_POP:
						{
							PX_StringInitialize(analysis->mp,&fmrString);

							if(operandLeft.bAtomPopIndex)
							{
								PX_StringCat(out,"POP R1\n");
								PX_StringCat(out,"POP R3\n");
								PX_StringSet(&fmrString,"MEMSET GLOBAL[R1],R3,R2\n");
								PX_StringCat(out,fmrString.buffer);
								PX_StringSet(&fmrString,"PUSH GLOBAL[R1]\n");
								PX_StringCat(out,fmrString.buffer);
							}
							else
							{
								PX_StringCat(out,"POP R1\n");
								PX_StringFormat1(&fmrString,"MEMSET GLOBAL[R1],%1,R2\n",PX_STRINGFORMAT_INT(operandLeft._oft));
								PX_StringCat(out,fmrString.buffer);
								PX_StringSet(&fmrString,"PUSH GLOBAL[R1]\n");
								PX_StringCat(out,fmrString.buffer);
							}	
							PX_StringFree(&fmrString);
						}
						break;
						default:
							return PX_FALSE;
					}
				}
				break;
			default:
				return PX_FALSE;

			}
			
		}
		break;

	case PX_SCRIPT_AST_OPERAND_TYPE_FLOAT:
		{
			if(!PX_ScriptParseAST_MapTokenToR2(analysis,operandRight,out)) return PX_FALSE;

			//Check operand1
			switch (operandLeft.operandType)
			{
			case PX_SCRIPT_AST_OPERAND_TYPE_FLOAT:
			case PX_SCRIPT_AST_OPERAND_TYPE_INT:
				{
					if (operandLeft.operandType==PX_SCRIPT_AST_OPERAND_TYPE_FLOAT)
					{
						pTop->operandType=PX_SCRIPT_AST_OPERAND_TYPE_FLOAT_CONST;
						pTop->region=PX_SCRIPT_VARIABLE_REGION_POP;
						pTop->_oft=0;
					}
					else
					{
						pTop->operandType=PX_SCRIPT_AST_OPERAND_TYPE_INT_CONST;
						pTop->region=PX_SCRIPT_VARIABLE_REGION_POP;
						pTop->_oft=0;
					}

					if(operandLeft.operandType==PX_SCRIPT_AST_OPERAND_TYPE_INT)
						PX_StringCat(out,"INT R2\n");

					switch(operandLeft.region)
					{
					case PX_SCRIPT_VARIABLE_REGION_LOCAL:
						{
							PX_StringInitialize(analysis->mp,&fmrString);
							PX_StringFormat1(&fmrString,"MOV LOCAL[%1],R2\n",PX_STRINGFORMAT_INT(operandLeft._oft));
							PX_StringCat(out,fmrString.buffer);
							PX_StringFormat1(&fmrString,"PUSH LOCAL[%1]\n",PX_STRINGFORMAT_INT(operandLeft._oft));
							PX_StringCat(out,fmrString.buffer);
							PX_StringFree(&fmrString);
						}
						break;
					case PX_SCRIPT_VARIABLE_REGION_GLOBAL:
						{
							PX_StringInitialize(analysis->mp,&fmrString);
							PX_StringFormat1(&fmrString,"MOV GLOBAL[%1],R2\n",PX_STRINGFORMAT_INT(operandLeft._oft));
							PX_StringCat(out,fmrString.buffer);
							PX_StringFormat1(&fmrString,"PUSH GLOBAL[%1]\n",PX_STRINGFORMAT_INT(operandLeft._oft));
							PX_StringCat(out,fmrString.buffer);
							PX_StringFree(&fmrString);
						}
						break;
					case PX_SCRIPT_VARIABLE_REGION_POP:
						{
							PX_StringInitialize(analysis->mp,&fmrString);
							PX_StringCat(out,"POP R1\n");
							PX_StringSet(&fmrString,"MOV GLOBAL[R1],R2\n");
							PX_StringCat(out,fmrString.buffer);
							PX_StringSet(&fmrString,"PUSH GLOBAL[R1]\n");
							PX_StringCat(out,fmrString.buffer);
							PX_StringFree(&fmrString);
						}
						break;
						default:
							return PX_FALSE;
					}
				}
				break;
			default:
				return PX_FALSE;
			}
			
		}
		break;

	case PX_SCRIPT_AST_OPERAND_TYPE_STRING: //string var
	case PX_SCRIPT_AST_OPERAND_TYPE_MEMORY://memory var
	case PX_SCRIPT_AST_OPERAND_TYPE_INT_PTR: //int var[x]
	case PX_SCRIPT_AST_OPERAND_TYPE_FLOAT_PTR://float var[x]
	case PX_SCRIPT_AST_OPERAND_TYPE_STRING_PTR://string var[x]
	case PX_SCRIPT_AST_OPERAND_TYPE_MEMORY_PTR://memory var[x]
	case PX_SCRIPT_AST_OPERAND_TYPE_STRUCT_PTR://struct xx var[x]
		{
			if (operandLeft.operandType==PX_SCRIPT_AST_OPERAND_TYPE_STRING)
			{
				pTop->operandType=PX_SCRIPT_AST_OPERAND_TYPE_STRING_CONST;
			}
			else if (operandLeft.operandType==PX_SCRIPT_AST_OPERAND_TYPE_MEMORY)
			{
				pTop->operandType=PX_SCRIPT_AST_OPERAND_TYPE_MEMORY_CONST;
			}
			else if (operandLeft.operandType==PX_SCRIPT_AST_OPERAND_TYPE_INT_PTR)
			{
				pTop->operandType=PX_SCRIPT_AST_OPERAND_TYPE_INT_PTR_CONST;
			}
			else if (operandLeft.operandType==PX_SCRIPT_AST_OPERAND_TYPE_FLOAT_PTR)
			{
				pTop->operandType=PX_SCRIPT_AST_OPERAND_TYPE_FLOAT_PTR_CONST;
			}
			else if (operandLeft.operandType==PX_SCRIPT_AST_OPERAND_TYPE_STRING_PTR)
			{
				pTop->operandType=PX_SCRIPT_AST_OPERAND_TYPE_STRING_PTR_CONST;
			}
			else if (operandLeft.operandType==PX_SCRIPT_AST_OPERAND_TYPE_MEMORY_PTR)
			{
				pTop->operandType=PX_SCRIPT_AST_OPERAND_TYPE_MEMORY_PTR_CONST;
			}
			else
			{
				pTop->operandType=PX_SCRIPT_AST_OPERAND_TYPE_SET_PTR_CONST;
			}

			pTop->region=PX_SCRIPT_VARIABLE_REGION_POP;
			pTop->_oft=0;
			pTop->pSet=operandLeft.pSet;

			if (operandLeft.operandType!=operandRight.operandType)
			{
				return PX_FALSE;
			}

			PX_StringInitialize(analysis->mp,&fmrString);

			if(operandRight.region==PX_SCRIPT_VARIABLE_REGION_LOCAL)
			{
				PX_StringFormat1(&fmrString,"MOV R2,LOCAL[%1]\n",PX_STRINGFORMAT_INT(operandRight._oft));
				PX_StringCat(out,fmrString.buffer);
			}
			else if(operandRight.region==PX_SCRIPT_VARIABLE_REGION_GLOBAL)
			{
				PX_StringFormat1(&fmrString,"MOV R2,GLOBAL[%1]\n",PX_STRINGFORMAT_INT(operandRight._oft));
				PX_StringCat(out,fmrString.buffer);
			}
			else if(operandRight.region==PX_SCRIPT_VARIABLE_REGION_POP)
			{
				PX_StringSet(&fmrString,"POP R2\n");
				PX_StringCat(out,fmrString.buffer);
				PX_StringSet(&fmrString,"MOV R2,GLOBAL[R2]\n");
				PX_StringCat(out,fmrString.buffer);
			}
			else
			{
				return PX_FALSE;
			}

			PX_StringFree(&fmrString);

			switch(operandLeft.region)
			{
			case PX_SCRIPT_VARIABLE_REGION_LOCAL:
				{
					PX_StringInitialize(analysis->mp,&fmrString);
					PX_StringFormat1(&fmrString,"MOV LOCAL[%1],R2\n",PX_STRINGFORMAT_INT(operandLeft._oft));
					PX_StringCat(out,fmrString.buffer);
					PX_StringFormat1(&fmrString,"PUSH LOCAL[%1]\n",PX_STRINGFORMAT_INT(operandLeft._oft));
					PX_StringCat(out,fmrString.buffer);
					PX_StringFree(&fmrString);
				}
				break;
			case PX_SCRIPT_VARIABLE_REGION_GLOBAL:
				{
					PX_StringInitialize(analysis->mp,&fmrString);
					PX_StringFormat1(&fmrString,"MOV GLOBAL[%1],R2\n",PX_STRINGFORMAT_INT(operandLeft._oft));
					PX_StringCat(out,fmrString.buffer);
					PX_StringFormat1(&fmrString,"PUSH GLOBAL[%1]\n",PX_STRINGFORMAT_INT(operandLeft._oft));
					PX_StringCat(out,fmrString.buffer);
					PX_StringFree(&fmrString);
				}
				break;
			case PX_SCRIPT_VARIABLE_REGION_POP:
				{
					PX_StringInitialize(analysis->mp,&fmrString);
					PX_StringCat(out,"POP R1\n");
					PX_StringSet(&fmrString,"MOV GLOBAL[R1],R2\n");
					PX_StringCat(out,fmrString.buffer);
					PX_StringSet(&fmrString,"PUSH GLOBAL[R1]\n");
					PX_StringCat(out,fmrString.buffer);
					PX_StringFree(&fmrString);
				}
				break;
				default:
					PX_ScriptTranslatorError(&analysis->lexer,"Invalid left-value.");
					return PX_FALSE;
			}
			
		}
		break;

	case PX_SCRIPT_AST_OPERAND_TYPE_STRUCT:
		{
			if (operandLeft.operandType!=operandRight.operandType)
			{
				return PX_FALSE;
			}
			if (operandLeft.pSet!=operandRight.pSet)
			{
				return PX_FALSE;
			}
			PX_StringInitialize(analysis->mp,&fmrString);
			
			if (!PX_ScriptParseAST_MapTokenToR2(analysis,operandRight,out))
			{
				return PX_FALSE;
			}
			if (!PX_ScriptParseAST_MapTokenToR1(analysis,operandLeft,out))
			{
				return PX_FALSE;
			}


			PX_StringFormat1(&fmrString,"DATACPY R1,R2,%1\n",PX_STRINGFORMAT_INT(operandLeft.pSet->size));
			PX_StringCat(out,fmrString.buffer);
			PX_StringSet(&fmrString,"PUSH R1\n");
			PX_StringCat(out,fmrString.buffer);
			PX_StringFree(&fmrString);
			pTop->operandType=PX_SCRIPT_AST_OPERAND_TYPE_STRUCT;
			pTop->region=PX_SCRIPT_VARIABLE_REGION_POP;
			pTop->_oft=0;
			pTop->pSet=operandLeft.pSet;
		}
		break;

	case PX_SCRIPT_AST_OPERAND_TYPE_INT_PTR_CONST:
	case PX_SCRIPT_AST_OPERAND_TYPE_FLOAT_PTR_CONST:
	case PX_SCRIPT_AST_OPERAND_TYPE_STRING_PTR_CONST:
	case PX_SCRIPT_AST_OPERAND_TYPE_MEMORY_PTR_CONST:
	case PX_SCRIPT_AST_OPERAND_TYPE_SET_PTR_CONST:
		{
			if (operandRight.operandType==PX_SCRIPT_AST_OPERAND_TYPE_INT_PTR_CONST&&operandLeft.operandType!=PX_SCRIPT_AST_OPERAND_TYPE_INT_PTR)
			{
				return PX_FALSE;
			}
			if (operandRight.operandType==PX_SCRIPT_AST_OPERAND_TYPE_FLOAT_PTR_CONST&&operandLeft.operandType!=PX_SCRIPT_AST_OPERAND_TYPE_FLOAT_PTR)
			{
				return PX_FALSE;
			}
			if (operandRight.operandType==PX_SCRIPT_AST_OPERAND_TYPE_STRING_PTR_CONST&&operandLeft.operandType!=PX_SCRIPT_AST_OPERAND_TYPE_STRING_PTR)
			{
				return PX_FALSE;
			}
			if (operandRight.operandType==PX_SCRIPT_AST_OPERAND_TYPE_MEMORY_PTR_CONST&&operandLeft.operandType!=PX_SCRIPT_AST_OPERAND_TYPE_MEMORY_PTR)
			{
				return PX_FALSE;
			}
			if (operandRight.operandType==PX_SCRIPT_AST_OPERAND_TYPE_SET_PTR_CONST&&operandLeft.operandType!=PX_SCRIPT_AST_OPERAND_TYPE_STRUCT_PTR)
			{
				return PX_FALSE;
			}


			PX_StringInitialize(analysis->mp,&fmrString);

			if(operandRight.region==PX_SCRIPT_VARIABLE_REGION_GLOBAL)
			{
				PX_StringFormat1(&fmrString,"MOV R2,%1\n",PX_STRINGFORMAT_INT(operandRight._oft));
				PX_StringCat(out,fmrString.buffer);
			}
			else if(operandRight.region==PX_SCRIPT_VARIABLE_REGION_LOCAL)
			{
				PX_StringFormat1(&fmrString,"MOV R2,BP\nSUB R2,%1\n",PX_STRINGFORMAT_INT(operandRight._oft));//
				PX_StringCat(out,fmrString.buffer);
			}
			else if(operandRight.region==PX_SCRIPT_VARIABLE_REGION_POP)
			{
				PX_StringSet(&fmrString,"POP R2\n");
				PX_StringCat(out,fmrString.buffer);
			}
			else
			{
				PX_StringFree(&fmrString);
				return PX_FALSE;
			}

			switch(operandLeft.region)
			{
			case PX_SCRIPT_VARIABLE_REGION_LOCAL:
				{
					PX_StringFormat1(&fmrString,"MOV LOCAL[%1],R2\n",PX_STRINGFORMAT_INT(operandLeft._oft));
					PX_StringCat(out,fmrString.buffer);
					PX_StringFormat1(&fmrString,"PUSH LOCAL[%1]\n",PX_STRINGFORMAT_INT(operandLeft._oft));
					PX_StringCat(out,fmrString.buffer);
				}
				break;
			case PX_SCRIPT_VARIABLE_REGION_GLOBAL:
				{
					PX_StringFormat1(&fmrString,"MOV GLOBAL[%1],R2\n",PX_STRINGFORMAT_INT(operandLeft._oft));
					PX_StringCat(out,fmrString.buffer);
					PX_StringFormat1(&fmrString,"PUSH GLOBAL[%1]\n",PX_STRINGFORMAT_INT(operandLeft._oft));
					PX_StringCat(out,fmrString.buffer);
				}
				break;
			case PX_SCRIPT_VARIABLE_REGION_POP:
				{
					PX_StringCat(out,"POP R1\n");
					PX_StringSet(&fmrString,"MOV GLOBAL[R1],R2\n");
					PX_StringCat(out,fmrString.buffer);
					PX_StringSet(&fmrString,"PUSH GLOBAL[R1]\n");
					PX_StringCat(out,fmrString.buffer);
				}
				break;
				default:
				{
					PX_ScriptTranslatorError(&analysis->lexer,"Invalid left-value.");
					PX_StringFree(&fmrString);
					return PX_FALSE;
				}
			}
			PX_StringFree(&fmrString);
			pTop->operandType=operandRight.operandType;
			pTop->region=PX_SCRIPT_VARIABLE_REGION_POP;
			pTop->_oft=0;
			pTop->pSet=operandLeft.pSet;
		}
		break;

	case PX_SCRIPT_AST_OPERAND_TYPE_INT_CONST:
		{
			if (!PX_ScriptParseAST_MapTokenToR2(analysis,operandRight,out))
			{
				return PX_FALSE;
			}

			//Check operand1
			switch (operandLeft.operandType)
			{
			case PX_SCRIPT_AST_OPERAND_TYPE_FLOAT:
			case PX_SCRIPT_AST_OPERAND_TYPE_INT:
				{
					if (operandLeft.operandType==PX_SCRIPT_AST_OPERAND_TYPE_FLOAT)
					{
						pTop->operandType=PX_SCRIPT_AST_OPERAND_TYPE_FLOAT_CONST;
						pTop->region=PX_SCRIPT_VARIABLE_REGION_POP;
						pTop->_oft=0;
						PX_StringCat(out,"FLT R2\n");
					}
					else
					{
						pTop->operandType=PX_SCRIPT_AST_OPERAND_TYPE_INT_CONST;
						pTop->region=PX_SCRIPT_VARIABLE_REGION_POP;
						pTop->_oft=0;
					}

					switch(operandLeft.region)
					{
					case PX_SCRIPT_VARIABLE_REGION_LOCAL:
						{
							PX_StringInitialize(analysis->mp,&fmrString);
							PX_StringFormat1(&fmrString,"MOV LOCAL[%1],R2\n",PX_STRINGFORMAT_INT(operandLeft._oft));
							PX_StringCat(out,fmrString.buffer);
							PX_StringFormat1(&fmrString,"PUSH LOCAL[%1]\n",PX_STRINGFORMAT_INT(operandLeft._oft));
							PX_StringCat(out,fmrString.buffer);
							PX_StringFree(&fmrString);
						}
						break;
					case PX_SCRIPT_VARIABLE_REGION_GLOBAL:
						{
							PX_StringInitialize(analysis->mp,&fmrString);
							PX_StringFormat1(&fmrString,"MOV GLOBAL[%1],R2\n",PX_STRINGFORMAT_INT(operandLeft._oft));
							PX_StringCat(out,fmrString.buffer);
							PX_StringFormat1(&fmrString,"PUSH GLOBAL[%1]\n",PX_STRINGFORMAT_INT(operandLeft._oft));
							PX_StringCat(out,fmrString.buffer);
							PX_StringFree(&fmrString);
						}
						break;
					case PX_SCRIPT_VARIABLE_REGION_POP:
						{
							PX_StringInitialize(analysis->mp,&fmrString);
							PX_StringCat(out,"POP R1\n");
							PX_StringSet(&fmrString,"MOV GLOBAL[R1],R2\n");
							PX_StringCat(out,fmrString.buffer);
							PX_StringSet(&fmrString,"PUSH GLOBAL[R1]\n");
							PX_StringCat(out,fmrString.buffer);
							PX_StringFree(&fmrString);
						}
						break;
						default:
							return PX_FALSE;
					}
				}
				break;

			case PX_SCRIPT_AST_OPERAND_TYPE_STRING_IDX:
				{
					pTop->operandType=PX_SCRIPT_AST_OPERAND_TYPE_INT_CONST;
					pTop->region=PX_SCRIPT_VARIABLE_REGION_POP;
					pTop->_oft=0;
					switch(operandLeft.region)
					{
					case PX_SCRIPT_VARIABLE_REGION_LOCAL:
						{
							PX_StringInitialize(analysis->mp,&fmrString);
							if(operandLeft.bAtomPopIndex)
							{
								PX_StringCat(out,"POP R3\n");
								PX_StringFormat1(&fmrString,"STRSET LOCAL[%1],R3,R2\n",PX_STRINGFORMAT_INT(operandLeft._oft));
								PX_StringCat(out,fmrString.buffer);
								PX_StringFormat1(&fmrString,"PUSH LOCAL[%1]\n",PX_STRINGFORMAT_INT(operandLeft._oft));
								PX_StringCat(out,fmrString.buffer);
							}
							else
							{
								PX_StringFormat2(&fmrString,"STRSET LOCAL[%1],%2,R2\n",PX_STRINGFORMAT_INT(operandLeft._oft),PX_STRINGFORMAT_INT(operandLeft.AtomIndex));
								PX_StringCat(out,fmrString.buffer);
								PX_StringFormat1(&fmrString,"PUSH LOCAL[%1]\n",PX_STRINGFORMAT_INT(operandLeft._oft));
								PX_StringCat(out,fmrString.buffer);
							}
							PX_StringFree(&fmrString);
						}
						break;
					case PX_SCRIPT_VARIABLE_REGION_GLOBAL:
						{
							PX_StringInitialize(analysis->mp,&fmrString);
							if(operandLeft.bAtomPopIndex)
							{
								PX_StringCat(out,"POP R3\n");
								PX_StringFormat1(&fmrString,"STRSET GLOBAL[%1],R3,R2\n",PX_STRINGFORMAT_INT(operandLeft._oft));
								PX_StringCat(out,fmrString.buffer);
								PX_StringFormat1(&fmrString,"PUSH GLOBAL[%1]\n",PX_STRINGFORMAT_INT(operandLeft._oft));
								PX_StringCat(out,fmrString.buffer);
							}
							else
							{
								PX_StringFormat2(&fmrString,"STRSET GLOBAL[%1],%2,R2\n",PX_STRINGFORMAT_INT(operandLeft._oft),PX_STRINGFORMAT_INT(operandLeft.AtomIndex));
								PX_StringCat(out,fmrString.buffer);
								PX_StringFormat1(&fmrString,"PUSH GLOBAL[%1]\n",PX_STRINGFORMAT_INT(operandLeft._oft));
								PX_StringCat(out,fmrString.buffer);
							}
							PX_StringFree(&fmrString);
						}
						break;
					case PX_SCRIPT_VARIABLE_REGION_POP:
						{
							PX_StringInitialize(analysis->mp,&fmrString);

							if(operandLeft.bAtomPopIndex)
							{
								PX_StringCat(out,"POP R1\n");
								PX_StringCat(out,"POP R3\n");
								PX_StringSet(&fmrString,"STRSET GLOBAL[R1],R3,R2\n");
								PX_StringCat(out,fmrString.buffer);
								PX_StringSet(&fmrString,"PUSH GLOBAL[R1]\n");
								PX_StringCat(out,fmrString.buffer);
							}
							else
							{
								PX_StringCat(out,"POP R1\n");
								PX_StringFormat1(&fmrString,"STRSET GLOBAL[R1],%1,R2\n",PX_STRINGFORMAT_INT(operandLeft._oft));
								PX_StringCat(out,fmrString.buffer);
								PX_StringSet(&fmrString,"PUSH GLOBAL[R1]\n");
								PX_StringCat(out,fmrString.buffer);
							}	
							PX_StringFree(&fmrString);
						}
						break;
						default:
							return PX_FALSE;
					}
				}
				break;

			case PX_SCRIPT_AST_OPERAND_TYPE_MEMORY_IDX:
				{
					pTop->operandType=PX_SCRIPT_AST_OPERAND_TYPE_INT_CONST;
					pTop->region=PX_SCRIPT_VARIABLE_REGION_POP;
					pTop->_oft=0;
					switch(operandLeft.region)
					{
					case PX_SCRIPT_VARIABLE_REGION_LOCAL:
						{
							PX_StringInitialize(analysis->mp,&fmrString);
							if(operandLeft.bAtomPopIndex)
							{
								PX_StringCat(out,"POP R3\n");
								PX_StringFormat1(&fmrString,"MEMSET LOCAL[%1],R3,R2\n",PX_STRINGFORMAT_INT(operandLeft._oft));
								PX_StringCat(out,fmrString.buffer);
								PX_StringFormat1(&fmrString,"PUSH LOCAL[%1]\n",PX_STRINGFORMAT_INT(operandLeft._oft));
								PX_StringCat(out,fmrString.buffer);
							}
							else
							{
								PX_StringFormat2(&fmrString,"MEMSET LOCAL[%1],%2,R2\n",PX_STRINGFORMAT_INT(operandLeft._oft),PX_STRINGFORMAT_INT(operandLeft.AtomIndex));
								PX_StringCat(out,fmrString.buffer);
								PX_StringFormat1(&fmrString,"PUSH LOCAL[%1]\n",PX_STRINGFORMAT_INT(operandLeft._oft));
								PX_StringCat(out,fmrString.buffer);
							}
							PX_StringFree(&fmrString);
						}
						break;
					case PX_SCRIPT_VARIABLE_REGION_GLOBAL:
						{
							PX_StringInitialize(analysis->mp,&fmrString);
							if(operandLeft.bAtomPopIndex)
							{
								PX_StringCat(out,"POP R3\n");
								PX_StringFormat1(&fmrString,"MEMSET GLOBAL[%1],R3,R2\n",PX_STRINGFORMAT_INT(operandLeft._oft));
								PX_StringCat(out,fmrString.buffer);
								PX_StringFormat1(&fmrString,"PUSH GLOBAL[%1]\n",PX_STRINGFORMAT_INT(operandLeft._oft));
								PX_StringCat(out,fmrString.buffer);
							}
							else
							{
								PX_StringFormat2(&fmrString,"MEMSET GLOBAL[%1],%2,R2\n",PX_STRINGFORMAT_INT(operandLeft._oft),PX_STRINGFORMAT_INT(operandLeft.AtomIndex));
								PX_StringCat(out,fmrString.buffer);
								PX_StringFormat1(&fmrString,"PUSH GLOBAL[%1]\n",PX_STRINGFORMAT_INT(operandLeft._oft));
								PX_StringCat(out,fmrString.buffer);
							}
							PX_StringFree(&fmrString);
						}
						break;
					case PX_SCRIPT_VARIABLE_REGION_POP:
						{
							PX_StringInitialize(analysis->mp,&fmrString);

							if(operandLeft.bAtomPopIndex)
							{
								PX_StringCat(out,"POP R1\n");
								PX_StringCat(out,"POP R3\n");
								PX_StringSet(&fmrString,"MEMSET GLOBAL[R1],R3,R2\n");
								PX_StringCat(out,fmrString.buffer);
								PX_StringSet(&fmrString,"PUSH GLOBAL[R1]\n");
								PX_StringCat(out,fmrString.buffer);
							}
							else
							{
								PX_StringCat(out,"POP R1\n");
								PX_StringFormat1(&fmrString,"MEMSET GLOBAL[R1],%1,R2\n",PX_STRINGFORMAT_INT(operandLeft._oft));
								PX_StringCat(out,fmrString.buffer);
								PX_StringSet(&fmrString,"PUSH GLOBAL[R1]\n");
								PX_StringCat(out,fmrString.buffer);
							}	
							PX_StringFree(&fmrString);
						}
						break;
						default:
							return PX_FALSE;
					}
				}
				break;
			default:
				return PX_FALSE;

			}
		}
		break;

		case PX_SCRIPT_AST_OPERAND_TYPE_FLOAT_CONST:
			{
				if (!PX_ScriptParseAST_MapTokenToR2(analysis,operandRight,out))
				{
					return PX_FALSE;
				}

				//Check operand1
				switch (operandLeft.operandType)
				{
				case PX_SCRIPT_AST_OPERAND_TYPE_FLOAT:
				case PX_SCRIPT_AST_OPERAND_TYPE_INT:
					{
						if (operandLeft.operandType==PX_SCRIPT_AST_OPERAND_TYPE_FLOAT)
						{
							pTop->operandType=PX_SCRIPT_AST_OPERAND_TYPE_FLOAT_CONST;
							pTop->region=PX_SCRIPT_VARIABLE_REGION_POP;
							pTop->_oft=0;
						}
						else
						{
							pTop->operandType=PX_SCRIPT_AST_OPERAND_TYPE_INT_CONST;
							pTop->region=PX_SCRIPT_VARIABLE_REGION_POP;
							pTop->_oft=0;
						}

						if(operandLeft.operandType==PX_SCRIPT_AST_OPERAND_TYPE_INT)
							PX_StringCat(out,"INT R2\n");

						switch(operandLeft.region)
						{
						case PX_SCRIPT_VARIABLE_REGION_LOCAL:
							{
								PX_StringInitialize(analysis->mp,&fmrString);
								PX_StringFormat1(&fmrString,"MOV LOCAL[%1],R2\n",PX_STRINGFORMAT_INT(operandLeft._oft));
								PX_StringCat(out,fmrString.buffer);
								PX_StringFormat1(&fmrString,"PUSH LOCAL[%1]\n",PX_STRINGFORMAT_INT(operandLeft._oft));
								PX_StringCat(out,fmrString.buffer);
								PX_StringFree(&fmrString);
							}
							break;
						case PX_SCRIPT_VARIABLE_REGION_GLOBAL:
							{
								PX_StringInitialize(analysis->mp,&fmrString);
								PX_StringFormat1(&fmrString,"MOV GLOBAL[%1],R2\n",PX_STRINGFORMAT_INT(operandLeft._oft));
								PX_StringCat(out,fmrString.buffer);
								PX_StringFormat1(&fmrString,"PUSH GLOBAL[%1]\n",PX_STRINGFORMAT_INT(operandLeft._oft));
								PX_StringCat(out,fmrString.buffer);
								PX_StringFree(&fmrString);
							}
							break;
						case PX_SCRIPT_VARIABLE_REGION_POP:
							{
								PX_StringInitialize(analysis->mp,&fmrString);
								PX_StringCat(out,"POP R1\n");
								PX_StringSet(&fmrString,"MOV GLOBAL[R1],R2\n");
								PX_StringCat(out,fmrString.buffer);
								PX_StringSet(&fmrString,"PUSH GLOBAL[R1]\n");
								PX_StringCat(out,fmrString.buffer);
								PX_StringFree(&fmrString);
							}
							break;
						default:
							return PX_FALSE;
						}
					}
					break;
				default:
					return PX_FALSE;
				}
			}
			break;

		case PX_SCRIPT_AST_OPERAND_TYPE_STRING_CONST:
		case PX_SCRIPT_AST_OPERAND_TYPE_MEMORY_CONST:
			{
				pTop->operandType=operandRight.operandType;
				pTop->region=PX_SCRIPT_VARIABLE_REGION_POP;
				pTop->_oft=0;
				//////////////////////////////////////////////////////////////////////////
				if (operandRight.operandType==PX_SCRIPT_AST_OPERAND_TYPE_STRING_CONST&&operandLeft.operandType!=PX_SCRIPT_AST_OPERAND_TYPE_STRING)
				{
					return PX_FALSE;
				}

				if (operandRight.operandType==PX_SCRIPT_AST_OPERAND_TYPE_MEMORY&&operandLeft.operandType!=PX_SCRIPT_AST_OPERAND_TYPE_MEMORY)
				{
					return PX_FALSE;
				}

				PX_StringInitialize(analysis->mp,&fmrString);

				if(operandRight.region==PX_SCRIPT_VARIABLE_REGION_GLOBAL)
				{
					PX_StringFormat1(&fmrString,"MOV R2,%1\n",PX_STRINGFORMAT_STRING(operandRight._contaniner));
					PX_StringCat(out,fmrString.buffer);
				}
				else if(operandRight.region==PX_SCRIPT_VARIABLE_REGION_POP)
				{
					PX_StringSet(&fmrString,"POP R2\n");
					PX_StringCat(out,fmrString.buffer);
				}
				else
				{
					PX_StringFree(&fmrString);
					return PX_FALSE;
				}
				PX_StringFree(&fmrString);

				//Check operand1

				switch(operandLeft.region)
				{
				case PX_SCRIPT_VARIABLE_REGION_LOCAL:
					{
						PX_StringInitialize(analysis->mp,&fmrString);
						PX_StringFormat1(&fmrString,"MOV LOCAL[%1],R2\n",PX_STRINGFORMAT_INT(operandLeft._oft));
						PX_StringCat(out,fmrString.buffer);
						PX_StringFormat1(&fmrString,"PUSH LOCAL[%1]\n",PX_STRINGFORMAT_INT(operandLeft._oft));
						PX_StringCat(out,fmrString.buffer);
						PX_StringFree(&fmrString);
					}
					break;
				case PX_SCRIPT_VARIABLE_REGION_GLOBAL:
					{
						PX_StringInitialize(analysis->mp,&fmrString);
						PX_StringFormat1(&fmrString,"MOV GLOBAL[%1],R2\n",PX_STRINGFORMAT_INT(operandLeft._oft));
						PX_StringCat(out,fmrString.buffer);
						PX_StringFormat1(&fmrString,"PUSH GLOBAL[%1]\n",PX_STRINGFORMAT_INT(operandLeft._oft));
						PX_StringCat(out,fmrString.buffer);
						PX_StringFree(&fmrString);
					}
					break;
				case PX_SCRIPT_VARIABLE_REGION_POP:
					{
						PX_StringInitialize(analysis->mp,&fmrString);
						PX_StringCat(out,"POP R1\n");
						PX_StringSet(&fmrString,"MOV GLOBAL[R1],R2\n");
						PX_StringCat(out,fmrString.buffer);
						PX_StringSet(&fmrString,"PUSH GLOBAL[R1]\n");
						PX_StringCat(out,fmrString.buffer);
						PX_StringFree(&fmrString);
					}
					break;
				default:
					return PX_FALSE;
				}
				//////////////////////////////////////////////////////////////////////////
			}
			break;

		case PX_SCRIPT_AST_OPERAND_TYPE_STRING_IDX:
			{
				pTop->operandType=PX_SCRIPT_AST_OPERAND_TYPE_INT_CONST;
				pTop->region=PX_SCRIPT_VARIABLE_REGION_POP;
				pTop->_oft=0;
				if (operandLeft.operandType!=PX_SCRIPT_AST_OPERAND_TYPE_INT&&operandLeft.operandType!=PX_SCRIPT_AST_OPERAND_TYPE_STRING_IDX&&operandLeft.operandType!=PX_SCRIPT_AST_OPERAND_TYPE_MEMORY_IDX)
				{
					return PX_FALSE;
				}

				if (!PX_ScriptParseAST_MapTokenToR2(analysis,operandRight,out))
				{
					return PX_FALSE;
				}

				if (operandLeft.operandType==PX_SCRIPT_AST_OPERAND_TYPE_INT)
				{
					//////////////////////////////////////////////////////////////////////////
					switch(operandLeft.region)
					{
					case PX_SCRIPT_VARIABLE_REGION_LOCAL:
						{
							PX_StringInitialize(analysis->mp,&fmrString);
							PX_StringFormat1(&fmrString,"MOV LOCAL[%1],R2\n",PX_STRINGFORMAT_INT(operandLeft._oft));
							PX_StringCat(out,fmrString.buffer);
							PX_StringFormat1(&fmrString,"PUSH LOCAL[%1]\n",PX_STRINGFORMAT_INT(operandLeft._oft));
							PX_StringCat(out,fmrString.buffer);
							PX_StringFree(&fmrString);
						}
						break;
					case PX_SCRIPT_VARIABLE_REGION_GLOBAL:
						{
							PX_StringInitialize(analysis->mp,&fmrString);
							PX_StringFormat1(&fmrString,"MOV GLOBAL[%1],R2\n",PX_STRINGFORMAT_INT(operandLeft._oft));
							PX_StringCat(out,fmrString.buffer);
							PX_StringFormat1(&fmrString,"PUSH GLOBAL[%1]\n",PX_STRINGFORMAT_INT(operandLeft._oft));
							PX_StringCat(out,fmrString.buffer);
							PX_StringFree(&fmrString);
						}
						break;
					case PX_SCRIPT_VARIABLE_REGION_POP:
						{
							PX_StringInitialize(analysis->mp,&fmrString);
							PX_StringCat(out,"POP R1\n");
							PX_StringSet(&fmrString,"MOV GLOBAL[R1],R2\n");
							PX_StringCat(out,fmrString.buffer);
							PX_StringSet(&fmrString,"PUSH GLOBAL[R1]\n");
							PX_StringCat(out,fmrString.buffer);
							PX_StringFree(&fmrString);
						}
						break;
					default:
						return PX_FALSE;
					}
					//////////////////////////////////////////////////////////////////////////
				}
				else if(operandLeft.operandType==PX_SCRIPT_AST_OPERAND_TYPE_STRING_IDX)
				{
					switch(operandLeft.region)
					{
					case PX_SCRIPT_VARIABLE_REGION_LOCAL:
						{
							PX_StringInitialize(analysis->mp,&fmrString);
							if(operandLeft.bAtomPopIndex)
							{
								PX_StringCat(out,"POP R3\n");
								PX_StringFormat1(&fmrString,"STRSET LOCAL[%1],R3,R2\n",PX_STRINGFORMAT_INT(operandLeft._oft));
								PX_StringCat(out,fmrString.buffer);
								PX_StringFormat1(&fmrString,"PUSH LOCAL[%1]\n",PX_STRINGFORMAT_INT(operandLeft._oft));
								PX_StringCat(out,fmrString.buffer);
							}
							else
							{
								PX_StringFormat2(&fmrString,"STRSET LOCAL[%1],%2,R2\n",PX_STRINGFORMAT_INT(operandLeft._oft),PX_STRINGFORMAT_INT(operandLeft.bAtomPopIndex));
								PX_StringCat(out,fmrString.buffer);
								PX_StringFormat1(&fmrString,"PUSH LOCAL[%1]\n",PX_STRINGFORMAT_INT(operandLeft._oft));
								PX_StringCat(out,fmrString.buffer);
							}
							PX_StringFree(&fmrString);
						}
						break;
					case PX_SCRIPT_VARIABLE_REGION_GLOBAL:
						{
							PX_StringInitialize(analysis->mp,&fmrString);
							if(operandLeft.bAtomPopIndex)
							{
								PX_StringCat(out,"POP R3\n");
								PX_StringFormat1(&fmrString,"STRSET GLOBAL[%1],R3,R2\n",PX_STRINGFORMAT_INT(operandLeft._oft));
								PX_StringCat(out,fmrString.buffer);
								PX_StringFormat1(&fmrString,"PUSH GLOBAL[%1]\n",PX_STRINGFORMAT_INT(operandLeft._oft));
								PX_StringCat(out,fmrString.buffer);
							}
							else
							{
								PX_StringFormat2(&fmrString,"STRSET GLOBAL[%1],%2,R2\n",PX_STRINGFORMAT_INT(operandLeft._oft),PX_STRINGFORMAT_INT(operandLeft.bAtomPopIndex));
								PX_StringCat(out,fmrString.buffer);
								PX_StringFormat1(&fmrString,"PUSH GLOBAL[%1]\n",PX_STRINGFORMAT_INT(operandLeft._oft));
								PX_StringCat(out,fmrString.buffer);
							}
							PX_StringFree(&fmrString);
						}
						break;
					case PX_SCRIPT_VARIABLE_REGION_POP:
						{
							PX_StringInitialize(analysis->mp,&fmrString);

							if(operandLeft.bAtomPopIndex)
							{
								PX_StringCat(out,"POP R1\n");
								PX_StringCat(out,"POP R3\n");
								PX_StringSet(&fmrString,"STRSET GLOBAL[R1],R3,R2\n");
								PX_StringCat(out,fmrString.buffer);
								PX_StringSet(&fmrString,"PUSH GLOBAL[R1]\n");
								PX_StringCat(out,fmrString.buffer);
							}
							else
							{
								PX_StringCat(out,"POP R1\n");
								PX_StringFormat1(&fmrString,"STRSET GLOBAL[R1],%1,R2\n",PX_STRINGFORMAT_INT(operandLeft._oft));
								PX_StringCat(out,fmrString.buffer);
								PX_StringSet(&fmrString,"PUSH GLOBAL[R1]\n");
								PX_StringCat(out,fmrString.buffer);
							}	
							PX_StringFree(&fmrString);
						}
						break;
					default:
						return PX_FALSE;
					}
				}
				else
				{
					switch(operandLeft.region)
					{
					case PX_SCRIPT_VARIABLE_REGION_LOCAL:
						{
							PX_StringInitialize(analysis->mp,&fmrString);
							if(operandLeft.bAtomPopIndex)
							{
								PX_StringCat(out,"POP R3\n");
								PX_StringFormat1(&fmrString,"MEMSET LOCAL[%1],R3,R2\n",PX_STRINGFORMAT_INT(operandLeft._oft));
								PX_StringCat(out,fmrString.buffer);
								PX_StringFormat1(&fmrString,"PUSH LOCAL[%1]\n",PX_STRINGFORMAT_INT(operandLeft._oft));
								PX_StringCat(out,fmrString.buffer);
							}
							else
							{
								PX_StringFormat2(&fmrString,"MEMSET LOCAL[%1],%2,R2\n",PX_STRINGFORMAT_INT(operandLeft._oft),PX_STRINGFORMAT_INT(operandLeft.bAtomPopIndex));
								PX_StringCat(out,fmrString.buffer);
								PX_StringFormat1(&fmrString,"PUSH LOCAL[%1]\n",PX_STRINGFORMAT_INT(operandLeft._oft));
								PX_StringCat(out,fmrString.buffer);
							}
							PX_StringFree(&fmrString);
						}
						break;
					case PX_SCRIPT_VARIABLE_REGION_GLOBAL:
						{
							PX_StringInitialize(analysis->mp,&fmrString);
							if(operandLeft.bAtomPopIndex)
							{
								PX_StringCat(out,"POP R3\n");
								PX_StringFormat1(&fmrString,"MEMSET GLOBAL[%1],R3,R2\n",PX_STRINGFORMAT_INT(operandLeft._oft));
								PX_StringCat(out,fmrString.buffer);
								PX_StringFormat1(&fmrString,"PUSH GLOBAL[%1]\n",PX_STRINGFORMAT_INT(operandLeft._oft));
								PX_StringCat(out,fmrString.buffer);
							}
							else
							{
								PX_StringFormat2(&fmrString,"MEMSET GLOBAL[%1],%2,R2\n",PX_STRINGFORMAT_INT(operandLeft._oft),PX_STRINGFORMAT_INT(operandLeft.bAtomPopIndex));
								PX_StringCat(out,fmrString.buffer);
								PX_StringFormat1(&fmrString,"PUSH GLOBAL[%1]\n",PX_STRINGFORMAT_INT(operandLeft._oft));
								PX_StringCat(out,fmrString.buffer);
							}
							PX_StringFree(&fmrString);
						}
						break;
					case PX_SCRIPT_VARIABLE_REGION_POP:
						{
							PX_StringInitialize(analysis->mp,&fmrString);

							if(operandLeft.bAtomPopIndex)
							{
								PX_StringCat(out,"POP R1\n");
								PX_StringCat(out,"POP R3\n");
								PX_StringSet(&fmrString,"MEMSET GLOBAL[R1],R3,R2\n");
								PX_StringCat(out,fmrString.buffer);
								PX_StringSet(&fmrString,"PUSH GLOBAL[R1]\n");
								PX_StringCat(out,fmrString.buffer);
							}
							else
							{
								PX_StringCat(out,"POP R1\n");
								PX_StringFormat1(&fmrString,"MEMSET GLOBAL[R1],%1,R2\n",PX_STRINGFORMAT_INT(operandLeft._oft));
								PX_StringCat(out,fmrString.buffer);
								PX_StringSet(&fmrString,"PUSH GLOBAL[R1]\n");
								PX_StringCat(out,fmrString.buffer);
							}	
							PX_StringFree(&fmrString);
						}
						break;
					default:
						return PX_FALSE;
					}
				}
			}
			break;

			case PX_SCRIPT_AST_OPERAND_TYPE_MEMORY_IDX:
				{
					pTop->operandType=PX_SCRIPT_AST_OPERAND_TYPE_INT_CONST;
					pTop->region=PX_SCRIPT_VARIABLE_REGION_POP;
					pTop->_oft=0;
					if (operandLeft.operandType!=PX_SCRIPT_AST_OPERAND_TYPE_INT&&operandLeft.operandType!=PX_SCRIPT_AST_OPERAND_TYPE_MEMORY_IDX&&operandLeft.operandType!=PX_SCRIPT_AST_OPERAND_TYPE_STRING_IDX)
					{
						return PX_FALSE;
					}

					if (!PX_ScriptParseAST_MapTokenToR2(analysis,operandRight,out))
					{
						return PX_FALSE;
					}

					if (operandLeft.operandType==PX_SCRIPT_AST_OPERAND_TYPE_INT)
					{
						//////////////////////////////////////////////////////////////////////////
						switch(operandLeft.region)
						{
						case PX_SCRIPT_VARIABLE_REGION_LOCAL:
							{
								PX_StringInitialize(analysis->mp,&fmrString);
								PX_StringFormat1(&fmrString,"MOV LOCAL[%1],R2\n",PX_STRINGFORMAT_INT(operandLeft._oft));
								PX_StringCat(out,fmrString.buffer);
								PX_StringFormat1(&fmrString,"PUSH LOCAL[%1]\n",PX_STRINGFORMAT_INT(operandLeft._oft));
								PX_StringCat(out,fmrString.buffer);
								PX_StringFree(&fmrString);
							}
							break;
						case PX_SCRIPT_VARIABLE_REGION_GLOBAL:
							{
								PX_StringInitialize(analysis->mp,&fmrString);
								PX_StringFormat1(&fmrString,"MOV GLOBAL[%1],R2\n",PX_STRINGFORMAT_INT(operandLeft._oft));
								PX_StringCat(out,fmrString.buffer);
								PX_StringFormat1(&fmrString,"PUSH GLOBAL[%1]\n",PX_STRINGFORMAT_INT(operandLeft._oft));
								PX_StringCat(out,fmrString.buffer);
								PX_StringFree(&fmrString);
							}
							break;
						case PX_SCRIPT_VARIABLE_REGION_POP:
							{
								PX_StringInitialize(analysis->mp,&fmrString);
								PX_StringCat(out,"POP R1\n");
								PX_StringSet(&fmrString,"MOV GLOBAL[R1],R2\n");
								PX_StringCat(out,fmrString.buffer);
								PX_StringSet(&fmrString,"PUSH GLOBAL[R1]\n");
								PX_StringCat(out,fmrString.buffer);
								PX_StringFree(&fmrString);
							}
							break;
						default:
							return PX_FALSE;
						}
						//////////////////////////////////////////////////////////////////////////
					}
					else if(operandLeft.operandType==PX_SCRIPT_AST_OPERAND_TYPE_MEMORY_IDX)
					{
						switch(operandLeft.region)
						{
						case PX_SCRIPT_VARIABLE_REGION_LOCAL:
							{
								PX_StringInitialize(analysis->mp,&fmrString);
								if(operandLeft.bAtomPopIndex)
								{
									PX_StringCat(out,"POP R3\n");
									PX_StringFormat1(&fmrString,"MEMSET LOCAL[%1],R3,R2\n",PX_STRINGFORMAT_INT(operandLeft._oft));
									PX_StringCat(out,fmrString.buffer);
									PX_StringFormat1(&fmrString,"PUSH LOCAL[%1]\n",PX_STRINGFORMAT_INT(operandLeft._oft));
									PX_StringCat(out,fmrString.buffer);
								}
								else
								{
									PX_StringFormat2(&fmrString,"MEMSET LOCAL[%1],%2,R2\n",PX_STRINGFORMAT_INT(operandLeft._oft),PX_STRINGFORMAT_INT(operandLeft.bAtomPopIndex));
									PX_StringCat(out,fmrString.buffer);
									PX_StringFormat1(&fmrString,"PUSH LOCAL[%1]\n",PX_STRINGFORMAT_INT(operandLeft._oft));
									PX_StringCat(out,fmrString.buffer);
								}
								PX_StringFree(&fmrString);
							}
							break;
						case PX_SCRIPT_VARIABLE_REGION_GLOBAL:
							{
								PX_StringInitialize(analysis->mp,&fmrString);
								if(operandLeft.bAtomPopIndex)
								{
									PX_StringCat(out,"POP R3\n");
									PX_StringFormat1(&fmrString,"MEMSET GLOBAL[%1],R3,R2\n",PX_STRINGFORMAT_INT(operandLeft._oft));
									PX_StringCat(out,fmrString.buffer);
									PX_StringFormat1(&fmrString,"PUSH GLOBAL[%1]\n",PX_STRINGFORMAT_INT(operandLeft._oft));
									PX_StringCat(out,fmrString.buffer);
								}
								else
								{
									PX_StringFormat2(&fmrString,"MEMSET GLOBAL[%1],%2,R2\n",PX_STRINGFORMAT_INT(operandLeft._oft),PX_STRINGFORMAT_INT(operandLeft.bAtomPopIndex));
									PX_StringCat(out,fmrString.buffer);
									PX_StringFormat1(&fmrString,"PUSH GLOBAL[%1]\n",PX_STRINGFORMAT_INT(operandLeft._oft));
									PX_StringCat(out,fmrString.buffer);
								}
								PX_StringFree(&fmrString);
							}
							break;
						case PX_SCRIPT_VARIABLE_REGION_POP:
							{
								PX_StringInitialize(analysis->mp,&fmrString);

								if(operandLeft.bAtomPopIndex)
								{
									PX_StringCat(out,"POP R1\n");
									PX_StringCat(out,"POP R3\n");
									PX_StringSet(&fmrString,"MEMSET GLOBAL[R1],R3,R2\n");
									PX_StringCat(out,fmrString.buffer);
									PX_StringSet(&fmrString,"PUSH GLOBAL[R1]\n");
									PX_StringCat(out,fmrString.buffer);
								}
								else
								{
									PX_StringCat(out,"POP R1\n");
									PX_StringFormat1(&fmrString,"MEMSET GLOBAL[R1],%1,R2\n",PX_STRINGFORMAT_INT(operandLeft._oft));
									PX_StringCat(out,fmrString.buffer);
									PX_StringSet(&fmrString,"PUSH GLOBAL[R1]\n");
									PX_StringCat(out,fmrString.buffer);
								}	
								PX_StringFree(&fmrString);
							}
							break;
						default:
							return PX_FALSE;
						}
					}
					else
					{
						switch(operandLeft.region)
						{
						case PX_SCRIPT_VARIABLE_REGION_LOCAL:
							{
								PX_StringInitialize(analysis->mp,&fmrString);
								if(operandLeft.bAtomPopIndex)
								{
									PX_StringCat(out,"POP R3\n");
									PX_StringFormat1(&fmrString,"STRSET LOCAL[%1],R3,R2\n",PX_STRINGFORMAT_INT(operandLeft._oft));
									PX_StringCat(out,fmrString.buffer);
									PX_StringFormat1(&fmrString,"PUSH LOCAL[%1]\n",PX_STRINGFORMAT_INT(operandLeft._oft));
									PX_StringCat(out,fmrString.buffer);
								}
								else
								{
									PX_StringFormat2(&fmrString,"STRSET LOCAL[%1],%2,R2\n",PX_STRINGFORMAT_INT(operandLeft._oft),PX_STRINGFORMAT_INT(operandLeft.bAtomPopIndex));
									PX_StringCat(out,fmrString.buffer);
									PX_StringFormat1(&fmrString,"PUSH LOCAL[%1]\n",PX_STRINGFORMAT_INT(operandLeft._oft));
									PX_StringCat(out,fmrString.buffer);
								}
								PX_StringFree(&fmrString);
							}
							break;
						case PX_SCRIPT_VARIABLE_REGION_GLOBAL:
							{
								PX_StringInitialize(analysis->mp,&fmrString);
								if(operandLeft.bAtomPopIndex)
								{
									PX_StringCat(out,"POP R3\n");
									PX_StringFormat1(&fmrString,"STRSET GLOBAL[%1],R3,R2\n",PX_STRINGFORMAT_INT(operandLeft._oft));
									PX_StringCat(out,fmrString.buffer);
									PX_StringFormat1(&fmrString,"PUSH GLOBAL[%1]\n",PX_STRINGFORMAT_INT(operandLeft._oft));
									PX_StringCat(out,fmrString.buffer);
								}
								else
								{
									PX_StringFormat2(&fmrString,"STRSET GLOBAL[%1],%2,R2\n",PX_STRINGFORMAT_INT(operandLeft._oft),PX_STRINGFORMAT_INT(operandLeft.bAtomPopIndex));
									PX_StringCat(out,fmrString.buffer);
									PX_StringFormat1(&fmrString,"PUSH GLOBAL[%1]\n",PX_STRINGFORMAT_INT(operandLeft._oft));
									PX_StringCat(out,fmrString.buffer);
								}
								PX_StringFree(&fmrString);
							}
							break;
						case PX_SCRIPT_VARIABLE_REGION_POP:
							{
								PX_StringInitialize(analysis->mp,&fmrString);

								if(operandLeft.bAtomPopIndex)
								{
									PX_StringCat(out,"POP R1\n");
									PX_StringCat(out,"POP R3\n");
									PX_StringSet(&fmrString,"STRSET GLOBAL[R1],R3,R2\n");
									PX_StringCat(out,fmrString.buffer);
									PX_StringSet(&fmrString,"PUSH GLOBAL[R1]\n");
									PX_StringCat(out,fmrString.buffer);
								}
								else
								{
									PX_StringCat(out,"POP R1\n");
									PX_StringFormat1(&fmrString,"MEMSET GLOBAL[R1],%1,R2\n",PX_STRINGFORMAT_INT(operandLeft._oft));
									PX_StringCat(out,fmrString.buffer);
									PX_StringSet(&fmrString,"PUSH GLOBAL[R1]\n");
									PX_StringCat(out,fmrString.buffer);
								}	
								PX_StringFree(&fmrString);
							}
							break;
						default:
							return PX_FALSE;
						}
					}

				}
				break;

			default:
				return PX_FALSE;
	}

	return PX_TRUE;
}
static px_bool PX_ScriptParseLastInstr_IDX(PX_SCRIPT_Analysis *analysis,px_vector *op,px_vector *tk,px_string *out)
{
	PX_SCRIPT_AST_OPERAND operand1,operand2,resOperand;
	px_string fmrString;
	if (tk->size<2)
	{
		return PX_FALSE;
	}
	operand1=*((PX_SCRIPT_AST_OPERAND *)PX_VECTORAT(PX_SCRIPT_AST_OPERAND,tk,tk->size-2));
	operand2=*((PX_SCRIPT_AST_OPERAND *)PX_VECTORAT(PX_SCRIPT_AST_OPERAND,tk,tk->size-1));

	PX_VectorPop(op);
	PX_VectorPop(tk);
	PX_VectorPop(tk);

	PX_StringInitialize(analysis->mp,&fmrString);

	switch(operand2.operandType)
	{
	case PX_SCRIPT_AST_OPERAND_TYPE_INT_CONST:
	case PX_SCRIPT_AST_OPERAND_TYPE_INT:
	case PX_SCRIPT_AST_OPERAND_TYPE_STRING_IDX:
	case PX_SCRIPT_AST_OPERAND_TYPE_MEMORY_IDX:
		{
			if (!PX_ScriptParseAST_MapTokenToR2(analysis,operand2,out))
			{
				return PX_FALSE;
			}
		}
		break;
	default:
		{
			PX_StringFree(&fmrString);
			return PX_FALSE;
		}
	}

	switch (operand1.operandType)
	{
	case PX_SCRIPT_AST_OPERAND_TYPE_INT_PTR:
	case PX_SCRIPT_AST_OPERAND_TYPE_FLOAT_PTR:
	case PX_SCRIPT_AST_OPERAND_TYPE_MEMORY_PTR:
	case PX_SCRIPT_AST_OPERAND_TYPE_STRING_PTR:
	case PX_SCRIPT_AST_OPERAND_TYPE_STRUCT_PTR:
		{
			if (operand1.operandType==PX_SCRIPT_AST_OPERAND_TYPE_INT_PTR)
			{
				resOperand.operandType=PX_SCRIPT_AST_OPERAND_TYPE_INT;
				resOperand.pSet=PX_NULL;
			}
			if (operand1.operandType==PX_SCRIPT_AST_OPERAND_TYPE_FLOAT_PTR)
			{
				resOperand.operandType=PX_SCRIPT_AST_OPERAND_TYPE_FLOAT;
				resOperand.pSet=PX_NULL;
			}
			if (operand1.operandType==PX_SCRIPT_AST_OPERAND_TYPE_STRING_PTR)
			{
				resOperand.operandType=PX_SCRIPT_AST_OPERAND_TYPE_STRING;
				resOperand.pSet=PX_NULL;
			}
			if (operand1.operandType==PX_SCRIPT_AST_OPERAND_TYPE_MEMORY_PTR)
			{
				resOperand.operandType=PX_SCRIPT_AST_OPERAND_TYPE_MEMORY;
				resOperand.pSet=PX_NULL;
			}
			if (operand1.operandType==PX_SCRIPT_AST_OPERAND_TYPE_STRUCT_PTR)
			{
				resOperand.operandType=PX_SCRIPT_AST_OPERAND_TYPE_STRUCT;
				resOperand.pSet=operand1.pSet;
				PX_StringFormat1(&fmrString,"MUL R2,%1\n",PX_STRINGFORMAT_INT(operand1.pSet->size));
				PX_StringCat(out,fmrString.buffer);
			}

			if (operand1.region==PX_SCRIPT_VARIABLE_REGION_GLOBAL)
			{
				PX_StringFormat1(&fmrString,"ADD R2,GLOBAL[%1]\n",PX_STRINGFORMAT_INT(operand1._oft));
				PX_StringCat(out,fmrString.buffer);
			}
			else if(operand1.region==PX_SCRIPT_VARIABLE_REGION_LOCAL)
			{
				PX_StringFormat1(&fmrString,"ADD R2,LOCAL[%1]\n",PX_STRINGFORMAT_INT(operand1._oft));
				PX_StringCat(out,fmrString.buffer);
			}
			else if(operand1.region==PX_SCRIPT_VARIABLE_REGION_POP)
			{
				PX_StringCat(out,"POP R3\n");
				PX_StringSet(&fmrString,"ADD R2,GLOBAL[R3]\n");
				PX_StringCat(out,fmrString.buffer);
			}
			else
			{
				PX_StringFree(&fmrString);
				return PX_FALSE;
			}
		}
		break;

		case PX_SCRIPT_AST_OPERAND_TYPE_INT_PTR_CONST:
		case PX_SCRIPT_AST_OPERAND_TYPE_FLOAT_PTR_CONST:
		case PX_SCRIPT_AST_OPERAND_TYPE_STRING_PTR_CONST:
		case PX_SCRIPT_AST_OPERAND_TYPE_MEMORY_PTR_CONST:
		case PX_SCRIPT_AST_OPERAND_TYPE_SET_PTR_CONST:
			{
				if (operand1.operandType==PX_SCRIPT_AST_OPERAND_TYPE_INT_PTR_CONST)
				{
					resOperand.operandType=PX_SCRIPT_AST_OPERAND_TYPE_INT;
					resOperand.pSet=PX_NULL;
				}
				if (operand1.operandType==PX_SCRIPT_AST_OPERAND_TYPE_FLOAT_PTR_CONST)
				{
					resOperand.operandType=PX_SCRIPT_AST_OPERAND_TYPE_FLOAT;
					resOperand.pSet=PX_NULL;
				}
				if (operand1.operandType==PX_SCRIPT_AST_OPERAND_TYPE_STRING_PTR_CONST)
				{
					resOperand.operandType=PX_SCRIPT_AST_OPERAND_TYPE_STRING;
					resOperand.pSet=PX_NULL;
				}
				if (operand1.operandType==PX_SCRIPT_AST_OPERAND_TYPE_MEMORY_PTR_CONST)
				{
					resOperand.operandType=PX_SCRIPT_AST_OPERAND_TYPE_MEMORY;
					resOperand.pSet=PX_NULL;
				}
				if (operand1.operandType==PX_SCRIPT_AST_OPERAND_TYPE_SET_PTR_CONST)
				{
					resOperand.operandType=PX_SCRIPT_AST_OPERAND_TYPE_STRUCT;
					resOperand.pSet=operand1.pSet;
					PX_StringFormat1(&fmrString,"MUL R2,%1\n",PX_STRINGFORMAT_INT(operand1.pSet->size));
					PX_StringCat(out,fmrString.buffer);

				}

				if (operand1.region==PX_SCRIPT_VARIABLE_REGION_GLOBAL)
				{
					PX_StringFormat1(&fmrString,"ADD R2,%1\n",PX_STRINGFORMAT_INT(operand1._oft));
					PX_StringCat(out,fmrString.buffer);
				}
				else if(operand1.region==PX_SCRIPT_VARIABLE_REGION_LOCAL)
				{
					PX_StringFormat1(&fmrString,"SUB R2,%1\nADD R2,BP\n",PX_STRINGFORMAT_INT(operand1._oft));//
					PX_StringCat(out,fmrString.buffer);
				}
				else if(operand1.region==PX_SCRIPT_VARIABLE_REGION_POP)
				{
					PX_StringCat(out,"POP R3\n");
					PX_StringSet(&fmrString,"ADD R2,GLOBAL[R3]\n");
					PX_StringCat(out,fmrString.buffer);
				}
				else
				{
					PX_StringFree(&fmrString);
					return PX_FALSE;
				}

			}
			break;
		case PX_SCRIPT_AST_OPERAND_TYPE_STRING:
		case PX_SCRIPT_AST_OPERAND_TYPE_MEMORY:
			{
				resOperand.bAtomPopIndex=PX_TRUE;

				if(operand1.operandType==PX_SCRIPT_AST_OPERAND_TYPE_MEMORY)
				resOperand.operandType=PX_SCRIPT_AST_OPERAND_TYPE_MEMORY_IDX;
				else
				resOperand.operandType=PX_SCRIPT_AST_OPERAND_TYPE_STRING_IDX;

				if (operand1.region==PX_SCRIPT_VARIABLE_REGION_GLOBAL)
				{
					PX_StringCat(out,"PUSH R2\n");
					PX_StringFormat1(&fmrString,"MOV R2,%1\n",PX_STRINGFORMAT_INT(operand1._oft));
					PX_StringCat(out,fmrString.buffer);
				}
				else if(operand1.region==PX_SCRIPT_VARIABLE_REGION_LOCAL)
				{
					PX_StringCat(out,"PUSH R2\n");
					PX_StringFormat1(&fmrString,"MOV R2,BP\nSUB R2,%1\n",PX_STRINGFORMAT_INT(operand1._oft));//
					PX_StringCat(out,fmrString.buffer);
				}
				else if(operand1.region==PX_SCRIPT_VARIABLE_REGION_POP)
				{
					PX_StringCat(out,"MOV R1,R2\n");
					PX_StringCat(out,"POP R2\n");
					PX_StringCat(out,"PUSH R1\n");
				}
				else
				{
					PX_StringFree(&fmrString);
					return PX_FALSE;
				}
			}
			break;
		default:
		{
			PX_StringFree(&fmrString);
			return PX_FALSE;
		}
	}
	PX_StringCat(out,"PUSH R2\n");

	resOperand.region=PX_SCRIPT_VARIABLE_REGION_POP;
	resOperand._int=0;
	resOperand._oft=0;
	PX_VectorPushback(tk,&resOperand);

	PX_StringFree(&fmrString);
	return PX_TRUE;
}
static px_bool PX_ScriptParseLastInstr_OFT(PX_SCRIPT_Analysis *analysis,px_vector *op,px_vector *tk,px_string *out)
{
	PX_SCRIPT_AST_OPERAND operand1,operand2,*pTop;
	px_string fmrString;
	if (tk->size<2)
	{
		return PX_FALSE;
	}
	operand1=*((PX_SCRIPT_AST_OPERAND *)PX_VECTORAT(PX_SCRIPT_AST_OPERAND,tk,tk->size-2));
	operand2=*((PX_SCRIPT_AST_OPERAND *)PX_VECTORAT(PX_SCRIPT_AST_OPERAND,tk,tk->size-1));

	PX_VectorPop(op);
	PX_VectorPop(tk);

	if(!PX_ScriptParseAST_MapTokenToR1(analysis,operand1,out)) return PX_FALSE;
	
	PX_StringInitialize(analysis->mp,&fmrString);
	PX_StringFormat1(&fmrString,"ADD R1,%1\nPUSH R1\n",PX_STRINGFORMAT_INT(operand2._oft));
	PX_StringCat(out,fmrString.buffer);
	PX_StringFree(&fmrString);

	pTop=(PX_SCRIPT_AST_OPERAND *)PX_VECTORAT(PX_SCRIPT_AST_OPERAND,tk,tk->size-1);
	*pTop=operand2;
	pTop->region=PX_SCRIPT_VARIABLE_REGION_POP;
	pTop->_oft=0;
	return PX_TRUE;
}
static px_bool PX_ScriptParseLastInstr_PTR(PX_SCRIPT_Analysis *analysis,px_vector *op,px_vector *tk,px_string *out)
{
	PX_SCRIPT_AST_OPERAND operand1,resOperand;
	px_string fmrString;
	if (tk->size<1)
	{
		return PX_FALSE;
	}
	operand1=*((PX_SCRIPT_AST_OPERAND *)PX_VECTORAT(PX_SCRIPT_AST_OPERAND,tk,tk->size-1));

	PX_VectorPop(op);
	PX_VectorPop(tk);

	PX_StringInitialize(analysis->mp,&fmrString);

	if (operand1.operandType==PX_SCRIPT_AST_OPERAND_TYPE_INT_PTR)
	{
		resOperand.operandType=PX_SCRIPT_AST_OPERAND_TYPE_INT;
		resOperand.pSet=PX_NULL;
	}
	else if (operand1.operandType==PX_SCRIPT_AST_OPERAND_TYPE_FLOAT_PTR)
	{
		resOperand.operandType=PX_SCRIPT_AST_OPERAND_TYPE_FLOAT;
		resOperand.pSet=PX_NULL;
	}
	else if (operand1.operandType==PX_SCRIPT_AST_OPERAND_TYPE_STRING_PTR)
	{
		resOperand.operandType=PX_SCRIPT_AST_OPERAND_TYPE_STRING;
		resOperand.pSet=PX_NULL;
	}
	else if (operand1.operandType==PX_SCRIPT_AST_OPERAND_TYPE_MEMORY_PTR)
	{
		resOperand.operandType=PX_SCRIPT_AST_OPERAND_TYPE_MEMORY;
		resOperand.pSet=PX_NULL;
	}
	else if (operand1.operandType==PX_SCRIPT_AST_OPERAND_TYPE_STRUCT_PTR)
	{
		resOperand.operandType=PX_SCRIPT_AST_OPERAND_TYPE_STRUCT;
		resOperand.pSet=operand1.pSet;
	}

	else if (operand1.operandType==PX_SCRIPT_AST_OPERAND_TYPE_INT_PTR_CONST)
	{
		resOperand.operandType=PX_SCRIPT_AST_OPERAND_TYPE_INT;
		resOperand.pSet=PX_NULL;
	}
	else if (operand1.operandType==PX_SCRIPT_AST_OPERAND_TYPE_FLOAT_PTR_CONST)
	{
		resOperand.operandType=PX_SCRIPT_AST_OPERAND_TYPE_FLOAT;
		resOperand.pSet=PX_NULL;
	}
	else if (operand1.operandType==PX_SCRIPT_AST_OPERAND_TYPE_STRING_PTR_CONST)
	{
		resOperand.operandType=PX_SCRIPT_AST_OPERAND_TYPE_STRING;
		resOperand.pSet=PX_NULL;
	}
	else if (operand1.operandType==PX_SCRIPT_AST_OPERAND_TYPE_MEMORY_PTR_CONST)
	{
		resOperand.operandType=PX_SCRIPT_AST_OPERAND_TYPE_MEMORY;
		resOperand.pSet=PX_NULL;
	}
	else if (operand1.operandType==PX_SCRIPT_AST_OPERAND_TYPE_SET_PTR_CONST)
	{
		resOperand.operandType=PX_SCRIPT_AST_OPERAND_TYPE_STRUCT;
		resOperand.pSet=operand1.pSet;

	}
	else
	{
		return PX_FALSE;
	}

	if (!PX_ScriptParseAST_MapTokenToR1(analysis,operand1,out))
	{
		return PX_FALSE;
	}

	PX_StringCat(out,"PUSH R1\n");
	resOperand.region=PX_SCRIPT_VARIABLE_REGION_POP;
	resOperand._oft=0;
	resOperand._int=0;
	PX_VectorPushback(tk,&resOperand);

	PX_StringFree(&fmrString);
	return PX_TRUE;
}
static px_bool PX_ScriptParseLastInstr_ADR(PX_SCRIPT_Analysis *analysis,px_vector *op,px_vector *tk,px_string *out)
{
	PX_SCRIPT_AST_OPERAND operand1,*pTop;
	px_string fmrString;
	if (tk->size<1)
	{
		return PX_FALSE;
	}
	operand1=*((PX_SCRIPT_AST_OPERAND *)PX_VECTORAT(PX_SCRIPT_AST_OPERAND,tk,tk->size-1));

	PX_VectorPop(op);

	pTop=((PX_SCRIPT_AST_OPERAND *)PX_VECTORAT(PX_SCRIPT_AST_OPERAND,tk,tk->size-1));

	switch(operand1.operandType)
	{
	case PX_SCRIPT_AST_OPERAND_TYPE_INT:
		pTop->operandType=PX_SCRIPT_AST_OPERAND_TYPE_INT_PTR_CONST;
		break;
	case PX_SCRIPT_AST_OPERAND_TYPE_FLOAT:
		pTop->operandType=PX_SCRIPT_AST_OPERAND_TYPE_FLOAT_PTR_CONST;
		break;
	case PX_SCRIPT_AST_OPERAND_TYPE_STRING:
		pTop->operandType=PX_SCRIPT_AST_OPERAND_TYPE_STRING_PTR_CONST;
		break;
	case PX_SCRIPT_AST_OPERAND_TYPE_MEMORY:
		pTop->operandType=PX_SCRIPT_AST_OPERAND_TYPE_MEMORY_PTR_CONST;
		break;
	case PX_SCRIPT_AST_OPERAND_TYPE_STRUCT:
		pTop->operandType=PX_SCRIPT_AST_OPERAND_TYPE_SET_PTR_CONST;
		break;
	default:
		return PX_FALSE;
	}

	if(operand1.region==PX_SCRIPT_VARIABLE_REGION_GLOBAL)
	{
		pTop->_int=operand1._int;
		pTop->region=PX_SCRIPT_VARIABLE_REGION_GLOBAL;
		return PX_TRUE;
	}
	else if(operand1.region==PX_SCRIPT_VARIABLE_REGION_LOCAL)
	{
		PX_StringInitialize(analysis->mp,&fmrString);
		PX_StringFormat1(&fmrString,"MOV R1,BP\nSUB R1,%1\n",PX_STRINGFORMAT_INT(operand1._oft));//
		PX_StringCat(out,fmrString.buffer);
		PX_StringSet(&fmrString,"PUSH R1\n");
		PX_StringCat(out,fmrString.buffer);
		PX_StringFree(&fmrString);

		pTop->_int=operand1._int;
		pTop->region=PX_SCRIPT_VARIABLE_REGION_POP;
		pTop->_oft=0;
	}
	else if (operand1.region==PX_SCRIPT_VARIABLE_REGION_POP)
	{
		pTop->region=PX_SCRIPT_VARIABLE_REGION_POP;
		pTop->_oft=0;
	}
	else
	{
		return PX_FALSE;
	}

	return PX_TRUE;
}
static px_bool PX_ScriptParseLastInstr_ADD(PX_SCRIPT_Analysis *analysis,px_vector *op,px_vector *tk,px_string *out)
{
	PX_SCRIPT_AST_OPERAND operand1,operand2,*pTop;
	px_string fmrString;

	if (tk->size<2)
	{
		return PX_FALSE;
	}
	operand1=*((PX_SCRIPT_AST_OPERAND *)PX_VECTORAT(PX_SCRIPT_AST_OPERAND,tk,tk->size-2));
	operand2=*((PX_SCRIPT_AST_OPERAND *)PX_VECTORAT(PX_SCRIPT_AST_OPERAND,tk,tk->size-1));

	PX_VectorPop(op);
	PX_VectorPop(tk);
	if (operand1.operandType==PX_SCRIPT_AST_OPERAND_TYPE_INT_CONST&&operand1.region==PX_SCRIPT_VARIABLE_REGION_GLOBAL)
	{
		if (operand2.operandType==PX_SCRIPT_AST_OPERAND_TYPE_INT_CONST&&operand2.region==PX_SCRIPT_VARIABLE_REGION_GLOBAL)
		{
			pTop=PX_VECTORLAST(PX_SCRIPT_AST_OPERAND,tk);
			pTop->_int=operand1._int+operand2._int;
			return PX_TRUE;
		}

		if (operand2.operandType==PX_SCRIPT_AST_OPERAND_TYPE_FLOAT_CONST&&operand2.region==PX_SCRIPT_VARIABLE_REGION_GLOBAL)
		{
			pTop=PX_VECTORLAST(PX_SCRIPT_AST_OPERAND,tk);
			pTop->operandType=PX_SCRIPT_AST_OPERAND_TYPE_FLOAT_CONST;
			pTop->_float=operand1._int+operand2._float;
			return PX_TRUE;
		}
	}

	if (operand1.operandType==PX_SCRIPT_AST_OPERAND_TYPE_FLOAT_CONST&&operand1.region==PX_SCRIPT_VARIABLE_REGION_GLOBAL)
	{
		if (operand2.operandType==PX_SCRIPT_AST_OPERAND_TYPE_INT_CONST&&operand2.region==PX_SCRIPT_VARIABLE_REGION_GLOBAL)
		{
			pTop=PX_VECTORLAST(PX_SCRIPT_AST_OPERAND,tk);
			pTop->operandType=PX_SCRIPT_AST_OPERAND_TYPE_FLOAT_CONST;
			pTop->_float=operand1._float+operand2._int;
			return PX_TRUE;
		}

		if (operand2.operandType==PX_SCRIPT_AST_OPERAND_TYPE_FLOAT_CONST&&operand2.region==PX_SCRIPT_VARIABLE_REGION_GLOBAL)
		{
			pTop=PX_VECTORLAST(PX_SCRIPT_AST_OPERAND,tk);
			pTop->_float=operand1._float+operand2._float;
			return PX_TRUE;
		}
	}

	switch(operand1.operandType)
	{
	case PX_SCRIPT_AST_OPERAND_TYPE_INT:
	case PX_SCRIPT_AST_OPERAND_TYPE_INT_CONST:
	case PX_SCRIPT_AST_OPERAND_TYPE_STRING_IDX:
	case PX_SCRIPT_AST_OPERAND_TYPE_MEMORY_IDX:
		{
			switch(operand2.operandType)
			{
			case PX_SCRIPT_AST_OPERAND_TYPE_INT:
			case PX_SCRIPT_AST_OPERAND_TYPE_INT_CONST:
			case PX_SCRIPT_AST_OPERAND_TYPE_FLOAT:
			case PX_SCRIPT_AST_OPERAND_TYPE_FLOAT_CONST:
			case PX_SCRIPT_AST_OPERAND_TYPE_STRING_IDX:
			case PX_SCRIPT_AST_OPERAND_TYPE_MEMORY_IDX:
				{
					if(!PX_ScriptParseAST_MapTokenToR2(analysis,operand2,out)) return PX_FALSE;
					if(!PX_ScriptParseAST_MapTokenToR1(analysis,operand1,out)) return PX_FALSE;
					PX_StringCat(out,"ADD R1,R2\nPUSH R1\n");
					pTop=PX_VECTORLAST(PX_SCRIPT_AST_OPERAND,tk);
					if(operand2.operandType==PX_SCRIPT_AST_OPERAND_TYPE_FLOAT||operand2.operandType==PX_SCRIPT_AST_OPERAND_TYPE_FLOAT_CONST)
					pTop->operandType=PX_SCRIPT_AST_OPERAND_TYPE_FLOAT_CONST;
					else
					pTop->operandType=PX_SCRIPT_AST_OPERAND_TYPE_INT_CONST;

					pTop->region=PX_SCRIPT_VARIABLE_REGION_POP;
					pTop->_oft=0;
				}
				break;
			default:
				return PX_FALSE;
			}
		}
		break;

	case PX_SCRIPT_AST_OPERAND_TYPE_FLOAT_CONST:
	case PX_SCRIPT_AST_OPERAND_TYPE_FLOAT:
		{
			switch(operand2.operandType)
			{
			case PX_SCRIPT_AST_OPERAND_TYPE_INT:
			case PX_SCRIPT_AST_OPERAND_TYPE_INT_CONST:
			case PX_SCRIPT_AST_OPERAND_TYPE_STRING_IDX:
			case PX_SCRIPT_AST_OPERAND_TYPE_MEMORY_IDX:
			case PX_SCRIPT_AST_OPERAND_TYPE_FLOAT:
			case PX_SCRIPT_AST_OPERAND_TYPE_FLOAT_CONST:
				{
					if(!PX_ScriptParseAST_MapTokenToR2(analysis,operand2,out)) return PX_FALSE;
					if(!PX_ScriptParseAST_MapTokenToR1(analysis,operand1,out)) return PX_FALSE;
					PX_StringCat(out,"ADD R1,R2\nPUSH R1\n");
					pTop=PX_VECTORLAST(PX_SCRIPT_AST_OPERAND,tk);
					pTop->operandType=PX_SCRIPT_AST_OPERAND_TYPE_FLOAT_CONST;
					pTop->region=PX_SCRIPT_VARIABLE_REGION_POP;
					pTop->_oft=0;
				}
				break;
			default:
				return PX_FALSE;
			}
		}
		break;

	case PX_SCRIPT_AST_OPERAND_TYPE_STRING:
		{
			switch(operand2.operandType)
			{
			case PX_SCRIPT_AST_OPERAND_TYPE_STRING_CONST:
			case PX_SCRIPT_AST_OPERAND_TYPE_STRING:
			case PX_SCRIPT_AST_OPERAND_TYPE_INT:
			case PX_SCRIPT_AST_OPERAND_TYPE_INT_CONST:
			case PX_SCRIPT_AST_OPERAND_TYPE_MEMORY_IDX:
			case PX_SCRIPT_AST_OPERAND_TYPE_STRING_IDX:
				{
					if(!PX_ScriptParseAST_MapTokenToR2(analysis,operand2,out)) return PX_FALSE;
					if(!PX_ScriptParseAST_MapTokenToR1(analysis,operand1,out)) return PX_FALSE;
					PX_StringCat(out,"STRCAT R1,R2\nPUSH R1\n");
					pTop=PX_VECTORLAST(PX_SCRIPT_AST_OPERAND,tk);
					pTop->operandType=PX_SCRIPT_AST_OPERAND_TYPE_STRING_CONST;
					pTop->region=PX_SCRIPT_VARIABLE_REGION_POP;
					pTop->_oft=0;
				}
				break;
			default:
				return PX_FALSE;
			}
		}
		break;

	case PX_SCRIPT_AST_OPERAND_TYPE_MEMORY:
		{
			switch(operand2.operandType)
			{
			case PX_SCRIPT_AST_OPERAND_TYPE_MEMORY_CONST:
			case PX_SCRIPT_AST_OPERAND_TYPE_MEMORY:
			case PX_SCRIPT_AST_OPERAND_TYPE_INT:
			case PX_SCRIPT_AST_OPERAND_TYPE_INT_CONST:
			case PX_SCRIPT_AST_OPERAND_TYPE_MEMORY_IDX:
			case PX_SCRIPT_AST_OPERAND_TYPE_STRING_IDX:
				{
					if(!PX_ScriptParseAST_MapTokenToR2(analysis,operand2,out)) return PX_FALSE;
					if(!PX_ScriptParseAST_MapTokenToR1(analysis,operand1,out)) return PX_FALSE;
					PX_StringCat(out,"MEMCAT R1,R2\nPUSH R1\n");
					pTop=PX_VECTORLAST(PX_SCRIPT_AST_OPERAND,tk);
					pTop->operandType=PX_SCRIPT_AST_OPERAND_TYPE_MEMORY_CONST;
					pTop->region=PX_SCRIPT_VARIABLE_REGION_POP;
					pTop->_oft=0;
				}
				break;
			default:
				return PX_FALSE;
			}
		}
		break;


	case PX_SCRIPT_AST_OPERAND_TYPE_STRING_CONST:
		{
			switch(operand2.operandType)
			{
			case PX_SCRIPT_AST_OPERAND_TYPE_STRING:
			case PX_SCRIPT_AST_OPERAND_TYPE_STRING_CONST:
			case PX_SCRIPT_AST_OPERAND_TYPE_INT:
			case PX_SCRIPT_AST_OPERAND_TYPE_INT_CONST:
			case PX_SCRIPT_AST_OPERAND_TYPE_MEMORY_IDX:
			case PX_SCRIPT_AST_OPERAND_TYPE_STRING_IDX:
				{
					if(!PX_ScriptParseAST_MapTokenToR2(analysis,operand2,out)) return PX_FALSE;
					if(!PX_ScriptParseAST_MapTokenToR1(analysis,operand1,out)) return PX_FALSE;
					PX_StringCat(out,"STRCAT R1,R2\nPUSH R1\n");
					pTop=PX_VECTORLAST(PX_SCRIPT_AST_OPERAND,tk);
					pTop->operandType=operand1.operandType;
					pTop->region=PX_SCRIPT_VARIABLE_REGION_POP;
					pTop->_oft=0;
				}
				break;
			default:
				return PX_FALSE;
			}
		}
		break;

	case PX_SCRIPT_AST_OPERAND_TYPE_MEMORY_CONST:
		{
			switch(operand2.operandType)
			{
			case PX_SCRIPT_AST_OPERAND_TYPE_MEMORY:
			case PX_SCRIPT_AST_OPERAND_TYPE_MEMORY_CONST:
			case PX_SCRIPT_AST_OPERAND_TYPE_INT:
			case PX_SCRIPT_AST_OPERAND_TYPE_INT_CONST:
			case PX_SCRIPT_AST_OPERAND_TYPE_MEMORY_IDX:
			case PX_SCRIPT_AST_OPERAND_TYPE_STRING_IDX:
				{
					if(!PX_ScriptParseAST_MapTokenToR2(analysis,operand2,out)) return PX_FALSE;
					if(!PX_ScriptParseAST_MapTokenToR1(analysis,operand1,out)) return PX_FALSE;
					PX_StringCat(out,"MEMCAT R1,R2\nPUSH R1\n");
					pTop=PX_VECTORLAST(PX_SCRIPT_AST_OPERAND,tk);
					pTop->operandType=operand1.operandType;
					pTop->region=PX_SCRIPT_VARIABLE_REGION_POP;
					pTop->_oft=0;
				}
				break;
			default:
				return PX_FALSE;
			}
		}
		break;


	case PX_SCRIPT_AST_OPERAND_TYPE_INT_PTR:
	case PX_SCRIPT_AST_OPERAND_TYPE_INT_PTR_CONST:
		{
			switch(operand2.operandType)
			{
			case PX_SCRIPT_AST_OPERAND_TYPE_INT:
			case PX_SCRIPT_AST_OPERAND_TYPE_INT_CONST:
			case PX_SCRIPT_AST_OPERAND_TYPE_MEMORY_IDX:
			case PX_SCRIPT_AST_OPERAND_TYPE_STRING_IDX:
				{
					if(!PX_ScriptParseAST_MapTokenToR2(analysis,operand2,out)) return PX_FALSE;
					if(!PX_ScriptParseAST_MapTokenToR1(analysis,operand1,out)) return PX_FALSE;
					PX_StringInitialize(analysis->mp,&fmrString);
					PX_StringFormat1(&fmrString,"MUL R2,%1\n",PX_STRINGFORMAT_INT(operand1.pSet->size));
					PX_StringCat(out,fmrString.buffer);
					PX_StringFree(&fmrString);
					PX_StringCat(out,"ADD R1,R2\nPUSH R1\n");
					pTop=PX_VECTORLAST(PX_SCRIPT_AST_OPERAND,tk);
					pTop->operandType=PX_SCRIPT_AST_OPERAND_TYPE_INT_PTR_CONST;
					pTop->region=PX_SCRIPT_VARIABLE_REGION_POP;
					pTop->_oft=0;
				}
				break;
			default:
				return PX_FALSE;
			}
		}
		break;

	case PX_SCRIPT_AST_OPERAND_TYPE_FLOAT_PTR:
	case PX_SCRIPT_AST_OPERAND_TYPE_FLOAT_PTR_CONST:
		{
			switch(operand2.operandType)
			{
			case PX_SCRIPT_AST_OPERAND_TYPE_INT:
			case PX_SCRIPT_AST_OPERAND_TYPE_INT_CONST:
			case PX_SCRIPT_AST_OPERAND_TYPE_MEMORY_IDX:
			case PX_SCRIPT_AST_OPERAND_TYPE_STRING_IDX:
				{
					if(!PX_ScriptParseAST_MapTokenToR2(analysis,operand2,out)) return PX_FALSE;
					if(!PX_ScriptParseAST_MapTokenToR1(analysis,operand1,out)) return PX_FALSE;
					PX_StringCat(out,"ADD R1,R2\nPUSH R1\n");
					pTop=PX_VECTORLAST(PX_SCRIPT_AST_OPERAND,tk);
					pTop->operandType=PX_SCRIPT_AST_OPERAND_TYPE_FLOAT_PTR_CONST;
					pTop->region=PX_SCRIPT_VARIABLE_REGION_POP;
					pTop->_oft=0;
				}
				break;
			default:
				return PX_FALSE;
			}
		}
		break;

	case PX_SCRIPT_AST_OPERAND_TYPE_STRING_PTR:
	case PX_SCRIPT_AST_OPERAND_TYPE_STRING_PTR_CONST:
		{
			switch(operand2.operandType)
			{
			case PX_SCRIPT_AST_OPERAND_TYPE_INT:
			case PX_SCRIPT_AST_OPERAND_TYPE_INT_CONST:
			case PX_SCRIPT_AST_OPERAND_TYPE_MEMORY_IDX:
			case PX_SCRIPT_AST_OPERAND_TYPE_STRING_IDX:
				{
					if(!PX_ScriptParseAST_MapTokenToR2(analysis,operand2,out)) return PX_FALSE;
					if(!PX_ScriptParseAST_MapTokenToR1(analysis,operand1,out)) return PX_FALSE;
					PX_StringCat(out,"ADD R1,R2\nPUSH R1\n");
					pTop=PX_VECTORLAST(PX_SCRIPT_AST_OPERAND,tk);
					pTop->operandType=PX_SCRIPT_AST_OPERAND_TYPE_STRING_PTR_CONST;
					pTop->region=PX_SCRIPT_VARIABLE_REGION_POP;
					pTop->_oft=0;
				}
				break;
			default:
				return PX_FALSE;
			}
		}
		break;

	case PX_SCRIPT_AST_OPERAND_TYPE_MEMORY_PTR:
	case PX_SCRIPT_AST_OPERAND_TYPE_MEMORY_PTR_CONST:
		{
			switch(operand2.operandType)
			{
			case PX_SCRIPT_AST_OPERAND_TYPE_INT:
			case PX_SCRIPT_AST_OPERAND_TYPE_INT_CONST:
			case PX_SCRIPT_AST_OPERAND_TYPE_MEMORY_IDX:
			case PX_SCRIPT_AST_OPERAND_TYPE_STRING_IDX:
				{
					if(!PX_ScriptParseAST_MapTokenToR2(analysis,operand2,out)) return PX_FALSE;
					if(!PX_ScriptParseAST_MapTokenToR1(analysis,operand1,out)) return PX_FALSE;
					PX_StringCat(out,"ADD R1,R2\nPUSH R1\n");
					pTop=PX_VECTORLAST(PX_SCRIPT_AST_OPERAND,tk);
					pTop->operandType=PX_SCRIPT_AST_OPERAND_TYPE_MEMORY_PTR_CONST;
					pTop->region=PX_SCRIPT_VARIABLE_REGION_POP;
					pTop->_oft=0;
				}
				break;
			default:
				return PX_FALSE;
			}
		}
		break;

	case PX_SCRIPT_AST_OPERAND_TYPE_STRUCT_PTR:
	case PX_SCRIPT_AST_OPERAND_TYPE_SET_PTR_CONST:
		{
			switch(operand2.operandType)
			{
			case PX_SCRIPT_AST_OPERAND_TYPE_INT:
			case PX_SCRIPT_AST_OPERAND_TYPE_INT_CONST:
			case PX_SCRIPT_AST_OPERAND_TYPE_MEMORY_IDX:
			case PX_SCRIPT_AST_OPERAND_TYPE_STRING_IDX:
				{
					if(!PX_ScriptParseAST_MapTokenToR2(analysis,operand2,out)) return PX_FALSE;
					if(!PX_ScriptParseAST_MapTokenToR1(analysis,operand1,out)) return PX_FALSE;
					
					PX_StringCat(out,"ADD R1,R2\nPUSH R1\n");
					pTop=PX_VECTORLAST(PX_SCRIPT_AST_OPERAND,tk);
					pTop->operandType=PX_SCRIPT_AST_OPERAND_TYPE_SET_PTR_CONST;
					pTop->region=PX_SCRIPT_VARIABLE_REGION_POP;
					pTop->_oft=0;
				}
				break;
			default:
				return PX_FALSE;
			}
		}
		break;

	default:
		return PX_FALSE;
	}
	
	return PX_TRUE;

}
static px_bool PX_ScriptParseLastInstr_INC(PX_SCRIPT_Analysis *analysis,px_vector *op,px_vector *tk,px_string *out)
{
	PX_SCRIPT_AST_OPERAND operand1;
	px_string fmrString;

	if (tk->size<1)
	{
		return PX_FALSE;
	}
	operand1=*((PX_SCRIPT_AST_OPERAND *)PX_VECTORAT(PX_SCRIPT_AST_OPERAND,tk,tk->size-1));

	PX_VectorPop(op);
	if (operand1.operandType!=PX_SCRIPT_AST_OPERAND_TYPE_INT&&operand1.operandType!=PX_SCRIPT_AST_OPERAND_TYPE_INT_PTR&&operand1.operandType!=PX_SCRIPT_AST_OPERAND_TYPE_FLOAT_PTR&&operand1.operandType!=PX_SCRIPT_AST_OPERAND_TYPE_STRING_PTR&&operand1.operandType!=PX_SCRIPT_AST_OPERAND_TYPE_MEMORY_PTR&&operand1.operandType!=PX_SCRIPT_AST_OPERAND_TYPE_STRUCT_PTR)
	{
		return PX_FALSE;
	}
	switch(operand1.region)
	{
	case PX_SCRIPT_VARIABLE_REGION_GLOBAL:
		{
			PX_StringInitialize(analysis->mp,&fmrString);
			PX_StringFormat1(&fmrString,"ADD GLOBAL[%1],1\n",PX_STRINGFORMAT_INT(operand1._oft));
			PX_StringCat(out,fmrString.buffer);
			PX_StringFree(&fmrString);
		}
		break;
	case PX_SCRIPT_VARIABLE_REGION_LOCAL:
		{
			PX_StringInitialize(analysis->mp,&fmrString);
			PX_StringFormat1(&fmrString,"ADD LOCAL[%1],1\n",PX_STRINGFORMAT_INT(operand1._oft));
			PX_StringCat(out,fmrString.buffer);
			PX_StringFree(&fmrString);
		}
		break;
	case PX_SCRIPT_VARIABLE_REGION_POP:
		{
			PX_StringInitialize(analysis->mp,&fmrString);
			PX_StringCat(out,"POP R1\n");
			PX_StringSet(&fmrString,"ADD GLOBAL[R1],1\n");
			PX_StringCat(out,fmrString.buffer);
			PX_StringCat(out,"PUSH R1\n");
			PX_StringFree(&fmrString);
		}
		break;
	default:
		return PX_FALSE;
	}
	return PX_TRUE;
}
static px_bool PX_ScriptParseLastInstr_POSITIVE(PX_SCRIPT_Analysis *analysis,px_vector *op,px_vector *tk,px_string *out)
{
	PX_SCRIPT_AST_OPERAND operand1;

	if (tk->size<1)
	{
		return PX_FALSE;
	}
	operand1=*((PX_SCRIPT_AST_OPERAND *)PX_VECTORAT(PX_SCRIPT_AST_OPERAND,tk,tk->size-1));

	switch(operand1.operandType)
	{
	case PX_SCRIPT_AST_OPERAND_TYPE_INT:
	case PX_SCRIPT_AST_OPERAND_TYPE_INT_CONST:
	case PX_SCRIPT_AST_OPERAND_TYPE_FLOAT:
	case PX_SCRIPT_AST_OPERAND_TYPE_FLOAT_CONST:
	case PX_SCRIPT_AST_OPERAND_TYPE_STRING_IDX:
	case PX_SCRIPT_AST_OPERAND_TYPE_MEMORY_IDX:
		break;
	default:
		return PX_FALSE;
	}
	PX_VectorPop(op);
	return PX_TRUE;
}
static px_bool PX_ScriptParseLastInstr_SUB(PX_SCRIPT_Analysis *analysis,px_vector *op,px_vector *tk,px_string *out)
{
	PX_SCRIPT_AST_OPERAND operand1,operand2,*pTop;

	if (tk->size<2)
	{
		return PX_FALSE;
	}
	operand1=*((PX_SCRIPT_AST_OPERAND *)PX_VECTORAT(PX_SCRIPT_AST_OPERAND,tk,tk->size-2));
	operand2=*((PX_SCRIPT_AST_OPERAND *)PX_VECTORAT(PX_SCRIPT_AST_OPERAND,tk,tk->size-1));

	PX_VectorPop(op);
	PX_VectorPop(tk);
	if (operand1.operandType==PX_SCRIPT_AST_OPERAND_TYPE_INT_CONST&&operand1.region==PX_SCRIPT_VARIABLE_REGION_GLOBAL)
	{
		if (operand2.operandType==PX_SCRIPT_AST_OPERAND_TYPE_INT_CONST&&operand2.region==PX_SCRIPT_VARIABLE_REGION_GLOBAL)
		{
			pTop=PX_VECTORLAST(PX_SCRIPT_AST_OPERAND,tk);
			pTop->_int=operand1._int-operand2._int;
			return PX_TRUE;
		}

		if (operand2.operandType==PX_SCRIPT_AST_OPERAND_TYPE_FLOAT_CONST&&operand2.region==PX_SCRIPT_VARIABLE_REGION_GLOBAL)
		{
			pTop=PX_VECTORLAST(PX_SCRIPT_AST_OPERAND,tk);
			pTop->operandType=PX_SCRIPT_AST_OPERAND_TYPE_FLOAT_CONST;
			pTop->_float=operand1._int-operand2._float;
			return PX_TRUE;
		}
	}

	if (operand1.operandType==PX_SCRIPT_AST_OPERAND_TYPE_FLOAT_CONST&&operand1.region==PX_SCRIPT_VARIABLE_REGION_GLOBAL)
	{
		if (operand2.operandType==PX_SCRIPT_AST_OPERAND_TYPE_INT_CONST&&operand2.region==PX_SCRIPT_VARIABLE_REGION_GLOBAL)
		{
			pTop=PX_VECTORLAST(PX_SCRIPT_AST_OPERAND,tk);
			pTop->operandType=PX_SCRIPT_AST_OPERAND_TYPE_FLOAT_CONST;
			pTop->_float=operand1._float-operand2._int;
			return PX_TRUE;
		}

		if (operand2.operandType==PX_SCRIPT_AST_OPERAND_TYPE_FLOAT_CONST&&operand2.region==PX_SCRIPT_VARIABLE_REGION_GLOBAL)
		{
			pTop=PX_VECTORLAST(PX_SCRIPT_AST_OPERAND,tk);
			pTop->_float=operand1._float-operand2._float;
			return PX_TRUE;
		}
	}

	switch(operand1.operandType)
	{
	case PX_SCRIPT_AST_OPERAND_TYPE_INT:
	case PX_SCRIPT_AST_OPERAND_TYPE_INT_CONST:
	case PX_SCRIPT_AST_OPERAND_TYPE_STRING_IDX:
	case PX_SCRIPT_AST_OPERAND_TYPE_MEMORY_IDX:
		{
			switch(operand2.operandType)
			{
			case PX_SCRIPT_AST_OPERAND_TYPE_INT:
			case PX_SCRIPT_AST_OPERAND_TYPE_INT_CONST:
			case PX_SCRIPT_AST_OPERAND_TYPE_FLOAT:
			case PX_SCRIPT_AST_OPERAND_TYPE_FLOAT_CONST:
			case PX_SCRIPT_AST_OPERAND_TYPE_STRING_IDX:
			case PX_SCRIPT_AST_OPERAND_TYPE_MEMORY_IDX:
				{
					if(!PX_ScriptParseAST_MapTokenToR2(analysis,operand2,out)) return PX_FALSE;
					if(!PX_ScriptParseAST_MapTokenToR1(analysis,operand1,out)) return PX_FALSE;
					PX_StringCat(out,"SUB R1,R2\nPUSH R1\n");
					pTop=PX_VECTORLAST(PX_SCRIPT_AST_OPERAND,tk);
					if(operand2.operandType==PX_SCRIPT_AST_OPERAND_TYPE_FLOAT||operand2.operandType==PX_SCRIPT_AST_OPERAND_TYPE_FLOAT_CONST)
						pTop->operandType=PX_SCRIPT_AST_OPERAND_TYPE_FLOAT_CONST;
					else
						pTop->operandType=PX_SCRIPT_AST_OPERAND_TYPE_INT_CONST;

					pTop->region=PX_SCRIPT_VARIABLE_REGION_POP;
					pTop->_oft=0;
				}
				break;
			default:
				return PX_FALSE;
			}
		}
		break;

	case PX_SCRIPT_AST_OPERAND_TYPE_FLOAT_CONST:
	case PX_SCRIPT_AST_OPERAND_TYPE_FLOAT:
		{
			switch(operand2.operandType)
			{
			case PX_SCRIPT_AST_OPERAND_TYPE_INT:
			case PX_SCRIPT_AST_OPERAND_TYPE_INT_CONST:
			case PX_SCRIPT_AST_OPERAND_TYPE_STRING_IDX:
			case PX_SCRIPT_AST_OPERAND_TYPE_MEMORY_IDX:
			case PX_SCRIPT_AST_OPERAND_TYPE_FLOAT:
			case PX_SCRIPT_AST_OPERAND_TYPE_FLOAT_CONST:
				{
					if(!PX_ScriptParseAST_MapTokenToR2(analysis,operand2,out)) return PX_FALSE;
					if(!PX_ScriptParseAST_MapTokenToR1(analysis,operand1,out)) return PX_FALSE;
					PX_StringCat(out,"SUB R1,R2\nPUSH R1\n");
					pTop=PX_VECTORLAST(PX_SCRIPT_AST_OPERAND,tk);
					pTop->operandType=PX_SCRIPT_AST_OPERAND_TYPE_FLOAT_CONST;
					pTop->region=PX_SCRIPT_VARIABLE_REGION_POP;
					pTop->_oft=0;
				}
				break;
			default:
				return PX_FALSE;
			}
		}
		break;



	case PX_SCRIPT_AST_OPERAND_TYPE_INT_PTR:
	case PX_SCRIPT_AST_OPERAND_TYPE_INT_PTR_CONST:
		{
			switch(operand2.operandType)
			{
			case PX_SCRIPT_AST_OPERAND_TYPE_INT:
			case PX_SCRIPT_AST_OPERAND_TYPE_INT_CONST:
			case PX_SCRIPT_AST_OPERAND_TYPE_MEMORY_IDX:
			case PX_SCRIPT_AST_OPERAND_TYPE_STRING_IDX:
				{
					if(!PX_ScriptParseAST_MapTokenToR2(analysis,operand2,out)) return PX_FALSE;
					if(!PX_ScriptParseAST_MapTokenToR1(analysis,operand1,out)) return PX_FALSE;
					PX_StringCat(out,"SUB R1,R2\nPUSH R1\n");
					pTop=PX_VECTORLAST(PX_SCRIPT_AST_OPERAND,tk);
					pTop->operandType=PX_SCRIPT_AST_OPERAND_TYPE_INT_PTR_CONST;
					pTop->region=PX_SCRIPT_VARIABLE_REGION_POP;
					pTop->_oft=0;
				}
				break;
			default:
				return PX_FALSE;
			}
		}
		break;

	case PX_SCRIPT_AST_OPERAND_TYPE_FLOAT_PTR:
	case PX_SCRIPT_AST_OPERAND_TYPE_FLOAT_PTR_CONST:
		{
			switch(operand2.operandType)
			{
			case PX_SCRIPT_AST_OPERAND_TYPE_INT:
			case PX_SCRIPT_AST_OPERAND_TYPE_INT_CONST:
			case PX_SCRIPT_AST_OPERAND_TYPE_MEMORY_IDX:
			case PX_SCRIPT_AST_OPERAND_TYPE_STRING_IDX:
				{
					if(!PX_ScriptParseAST_MapTokenToR2(analysis,operand2,out)) return PX_FALSE;
					if(!PX_ScriptParseAST_MapTokenToR1(analysis,operand1,out)) return PX_FALSE;
					PX_StringCat(out,"SUB R1,R2\nPUSH R1\n");
					pTop=PX_VECTORLAST(PX_SCRIPT_AST_OPERAND,tk);
					pTop->operandType=PX_SCRIPT_AST_OPERAND_TYPE_FLOAT_PTR_CONST;
					pTop->region=PX_SCRIPT_VARIABLE_REGION_POP;
					pTop->_oft=0;
				}
				break;
			default:
				return PX_FALSE;
			}
		}
		break;

	case PX_SCRIPT_AST_OPERAND_TYPE_STRING_PTR:
	case PX_SCRIPT_AST_OPERAND_TYPE_STRING_PTR_CONST:
		{
			switch(operand2.operandType)
			{
			case PX_SCRIPT_AST_OPERAND_TYPE_INT:
			case PX_SCRIPT_AST_OPERAND_TYPE_INT_CONST:
			case PX_SCRIPT_AST_OPERAND_TYPE_MEMORY_IDX:
			case PX_SCRIPT_AST_OPERAND_TYPE_STRING_IDX:
				{
					if(!PX_ScriptParseAST_MapTokenToR2(analysis,operand2,out)) return PX_FALSE;
					if(!PX_ScriptParseAST_MapTokenToR1(analysis,operand1,out)) return PX_FALSE;
					PX_StringCat(out,"SUB R1,R2\nPUSH R1\n");
					pTop=PX_VECTORLAST(PX_SCRIPT_AST_OPERAND,tk);
					pTop->operandType=PX_SCRIPT_AST_OPERAND_TYPE_STRING_PTR_CONST;
					pTop->region=PX_SCRIPT_VARIABLE_REGION_POP;
					pTop->_oft=0;
				}
				break;
			default:
				return PX_FALSE;
			}
		}
		break;

	case PX_SCRIPT_AST_OPERAND_TYPE_MEMORY_PTR:
	case PX_SCRIPT_AST_OPERAND_TYPE_MEMORY_PTR_CONST:
		{
			switch(operand2.operandType)
			{
			case PX_SCRIPT_AST_OPERAND_TYPE_INT:
			case PX_SCRIPT_AST_OPERAND_TYPE_INT_CONST:
			case PX_SCRIPT_AST_OPERAND_TYPE_MEMORY_IDX:
			case PX_SCRIPT_AST_OPERAND_TYPE_STRING_IDX:
				{
					if(!PX_ScriptParseAST_MapTokenToR2(analysis,operand2,out)) return PX_FALSE;
					if(!PX_ScriptParseAST_MapTokenToR1(analysis,operand1,out)) return PX_FALSE;
					PX_StringCat(out,"SUB R1,R2\nPUSH R1\n");
					pTop=PX_VECTORLAST(PX_SCRIPT_AST_OPERAND,tk);
					pTop->operandType=PX_SCRIPT_AST_OPERAND_TYPE_MEMORY_PTR_CONST;
					pTop->region=PX_SCRIPT_VARIABLE_REGION_POP;
					pTop->_oft=0;
				}
				break;
			default:
				return PX_FALSE;
			}
		}
		break;

	case PX_SCRIPT_AST_OPERAND_TYPE_STRUCT_PTR:
	case PX_SCRIPT_AST_OPERAND_TYPE_SET_PTR_CONST:
		{
			switch(operand2.operandType)
			{
			case PX_SCRIPT_AST_OPERAND_TYPE_INT:
			case PX_SCRIPT_AST_OPERAND_TYPE_INT_CONST:
			case PX_SCRIPT_AST_OPERAND_TYPE_MEMORY_IDX:
			case PX_SCRIPT_AST_OPERAND_TYPE_STRING_IDX:
				{
					if(!PX_ScriptParseAST_MapTokenToR2(analysis,operand2,out)) return PX_FALSE;
					if(!PX_ScriptParseAST_MapTokenToR1(analysis,operand1,out)) return PX_FALSE;
					PX_StringCat(out,"SUB R1,R2\nPUSH R1\n");
					pTop=PX_VECTORLAST(PX_SCRIPT_AST_OPERAND,tk);
					pTop->operandType=PX_SCRIPT_AST_OPERAND_TYPE_SET_PTR_CONST;
					pTop->region=PX_SCRIPT_VARIABLE_REGION_POP;
					pTop->_oft=0;
				}
				break;
			default:
				return PX_FALSE;
			}
		}
		break;

	default:
		return PX_FALSE;
	}

	return PX_TRUE;

}
static px_bool PX_ScriptParseLastInstr_DEC(PX_SCRIPT_Analysis *analysis,px_vector *op,px_vector *tk,px_string *out)
{
	PX_SCRIPT_AST_OPERAND operand1;
	px_string fmrString;

	if (tk->size<1)
	{
		return PX_FALSE;
	}
	operand1=*((PX_SCRIPT_AST_OPERAND *)PX_VECTORAT(PX_SCRIPT_AST_OPERAND,tk,tk->size-1));

	PX_VectorPop(op);

	if (operand1.operandType!=PX_SCRIPT_AST_OPERAND_TYPE_INT&&operand1.operandType!=PX_SCRIPT_AST_OPERAND_TYPE_INT_PTR&&operand1.operandType!=PX_SCRIPT_AST_OPERAND_TYPE_FLOAT_PTR&&operand1.operandType!=PX_SCRIPT_AST_OPERAND_TYPE_STRING_PTR&&operand1.operandType!=PX_SCRIPT_AST_OPERAND_TYPE_MEMORY_PTR&&operand1.operandType!=PX_SCRIPT_AST_OPERAND_TYPE_STRUCT_PTR)
	{
		return PX_FALSE;
	}
	switch(operand1.region)
	{
	case PX_SCRIPT_VARIABLE_REGION_GLOBAL:
		{
			PX_StringInitialize(analysis->mp,&fmrString);
			PX_StringFormat1(&fmrString,"SUB GLOBAL[%1],1\n",PX_STRINGFORMAT_INT(operand1._oft));
			PX_StringCat(out,fmrString.buffer);
			PX_StringFree(&fmrString);
		}
		break;
	case PX_SCRIPT_VARIABLE_REGION_LOCAL:
		{
			PX_StringInitialize(analysis->mp,&fmrString);
			PX_StringFormat1(&fmrString,"SUB LOCAL[%1],1\n",PX_STRINGFORMAT_INT(operand1._oft));
			PX_StringCat(out,fmrString.buffer);
			PX_StringFree(&fmrString);
		}
		break;
	case PX_SCRIPT_VARIABLE_REGION_POP:
		{
			PX_StringInitialize(analysis->mp,&fmrString);
			PX_StringCat(out,"POP R1\n");
			PX_StringSet(&fmrString,"SUB GLOBAL[R1],1\n");
			PX_StringCat(out,fmrString.buffer);
			PX_StringCat(out,"PUSH R1\n");
			PX_StringFree(&fmrString);
		}
		break;
	default:
		return PX_FALSE;
	}
	return PX_TRUE;
}
static px_bool PX_ScriptParseLastInstr_NEGATIVE(PX_SCRIPT_Analysis *analysis,px_vector *op,px_vector *tk,px_string *out)
{
	PX_SCRIPT_AST_OPERAND operand1,*pTop;

	if (tk->size<1)
	{
		return PX_FALSE;
	}
	operand1=*((PX_SCRIPT_AST_OPERAND *)PX_VECTORAT(PX_SCRIPT_AST_OPERAND,tk,tk->size-1));
	pTop=PX_VECTORLAST(PX_SCRIPT_AST_OPERAND,tk);
	PX_VectorPop(op);
	if (operand1.operandType==PX_SCRIPT_AST_OPERAND_TYPE_INT_CONST&&operand1.region==PX_SCRIPT_VARIABLE_REGION_GLOBAL)
	{
			pTop=PX_VECTORLAST(PX_SCRIPT_AST_OPERAND,tk);
			pTop->_int=-operand1._int;
			return PX_TRUE;
	}

	if (operand1.operandType==PX_SCRIPT_AST_OPERAND_TYPE_FLOAT_CONST&&operand1.region==PX_SCRIPT_VARIABLE_REGION_GLOBAL)
	{
			pTop=PX_VECTORLAST(PX_SCRIPT_AST_OPERAND,tk);
			pTop->_float=-operand1._float;
			return PX_TRUE;
	}


	switch(operand1.operandType)
	{
	case PX_SCRIPT_AST_OPERAND_TYPE_INT:
	case PX_SCRIPT_AST_OPERAND_TYPE_INT_CONST:
	case PX_SCRIPT_AST_OPERAND_TYPE_STRING_IDX:
	case PX_SCRIPT_AST_OPERAND_TYPE_MEMORY_IDX:
		pTop->operandType=PX_SCRIPT_AST_OPERAND_TYPE_INT_CONST;
		break;
	case PX_SCRIPT_AST_OPERAND_TYPE_FLOAT:
	case PX_SCRIPT_AST_OPERAND_TYPE_FLOAT_CONST:
		pTop->operandType=PX_SCRIPT_AST_OPERAND_TYPE_FLOAT_CONST;
		break;
	default:
		return PX_FALSE;
	}
	pTop->region=PX_SCRIPT_VARIABLE_REGION_POP;
	pTop->_oft=0;
	pTop->pSet=PX_NULL;
	pTop->_int=0;
	if(!PX_ScriptParseAST_MapTokenToR1(analysis,operand1,out)) return PX_FALSE;
	PX_StringCat(out,"NEG R1\n");
	PX_StringCat(out,"PUSH R1\n");
	return PX_TRUE;
}
static px_bool PX_ScriptParseLastInstr_MUL(PX_SCRIPT_Analysis *analysis,px_vector *op,px_vector *tk,px_string *out)
{
	PX_SCRIPT_AST_OPERAND operand1,operand2,*pTop;

	if (tk->size<2)
	{
		return PX_FALSE;
	}
	operand1=*((PX_SCRIPT_AST_OPERAND *)PX_VECTORAT(PX_SCRIPT_AST_OPERAND,tk,tk->size-2));
	operand2=*((PX_SCRIPT_AST_OPERAND *)PX_VECTORAT(PX_SCRIPT_AST_OPERAND,tk,tk->size-1));

	PX_VectorPop(op);
	PX_VectorPop(tk);
	if (operand1.operandType==PX_SCRIPT_AST_OPERAND_TYPE_INT_CONST&&operand1.region==PX_SCRIPT_VARIABLE_REGION_GLOBAL)
	{
		if (operand2.operandType==PX_SCRIPT_AST_OPERAND_TYPE_INT_CONST&&operand2.region==PX_SCRIPT_VARIABLE_REGION_GLOBAL)
		{
			pTop=PX_VECTORLAST(PX_SCRIPT_AST_OPERAND,tk);
			pTop->region=PX_SCRIPT_VARIABLE_REGION_GLOBAL;
			pTop->_int=operand1._int*operand2._int;
			return PX_TRUE;
		}

		if (operand2.operandType==PX_SCRIPT_AST_OPERAND_TYPE_FLOAT_CONST&&operand2.region==PX_SCRIPT_VARIABLE_REGION_GLOBAL)
		{
			pTop=PX_VECTORLAST(PX_SCRIPT_AST_OPERAND,tk);
			pTop->operandType=PX_SCRIPT_AST_OPERAND_TYPE_FLOAT_CONST;
			pTop->_float=operand1._int*operand2._float;
			return PX_TRUE;
		}
	}

	if (operand1.operandType==PX_SCRIPT_AST_OPERAND_TYPE_FLOAT_CONST&&operand1.region==PX_SCRIPT_VARIABLE_REGION_GLOBAL)
	{
		if (operand2.operandType==PX_SCRIPT_AST_OPERAND_TYPE_INT_CONST&&operand2.region==PX_SCRIPT_VARIABLE_REGION_GLOBAL)
		{
			pTop=PX_VECTORLAST(PX_SCRIPT_AST_OPERAND,tk);
			pTop->operandType=PX_SCRIPT_AST_OPERAND_TYPE_FLOAT_CONST;
			pTop->_float=operand1._float*operand2._int;
			return PX_TRUE;
		}

		if (operand2.operandType==PX_SCRIPT_AST_OPERAND_TYPE_FLOAT_CONST&&operand2.region==PX_SCRIPT_VARIABLE_REGION_GLOBAL)
		{
			pTop=PX_VECTORLAST(PX_SCRIPT_AST_OPERAND,tk);
			pTop->_float=operand1._float*operand2._float;
			return PX_TRUE;
		}
	}

	switch(operand1.operandType)
	{
	case PX_SCRIPT_AST_OPERAND_TYPE_INT:
	case PX_SCRIPT_AST_OPERAND_TYPE_INT_CONST:
	case PX_SCRIPT_AST_OPERAND_TYPE_STRING_IDX:
	case PX_SCRIPT_AST_OPERAND_TYPE_MEMORY_IDX:
		{
			switch(operand2.operandType)
			{
			case PX_SCRIPT_AST_OPERAND_TYPE_INT:
			case PX_SCRIPT_AST_OPERAND_TYPE_INT_CONST:
			case PX_SCRIPT_AST_OPERAND_TYPE_FLOAT:
			case PX_SCRIPT_AST_OPERAND_TYPE_FLOAT_CONST:
			case PX_SCRIPT_AST_OPERAND_TYPE_STRING_IDX:
			case PX_SCRIPT_AST_OPERAND_TYPE_MEMORY_IDX:
				{
					if(!PX_ScriptParseAST_MapTokenToR2(analysis,operand2,out)) return PX_FALSE;
					if(!PX_ScriptParseAST_MapTokenToR1(analysis,operand1,out)) return PX_FALSE;
					PX_StringCat(out,"MUL R1,R2\nPUSH R1\n");
					pTop=PX_VECTORLAST(PX_SCRIPT_AST_OPERAND,tk);
					if(operand2.operandType==PX_SCRIPT_AST_OPERAND_TYPE_FLOAT||operand2.operandType==PX_SCRIPT_AST_OPERAND_TYPE_FLOAT_CONST)
						pTop->operandType=PX_SCRIPT_AST_OPERAND_TYPE_FLOAT_CONST;
					else
						pTop->operandType=PX_SCRIPT_AST_OPERAND_TYPE_INT_CONST;

					pTop->region=PX_SCRIPT_VARIABLE_REGION_POP;
					pTop->_oft=0;
				}
				break;
			default:
				return PX_FALSE;
			}
		}
		break;

	case PX_SCRIPT_AST_OPERAND_TYPE_FLOAT_CONST:
	case PX_SCRIPT_AST_OPERAND_TYPE_FLOAT:
		{
			switch(operand2.operandType)
			{
			case PX_SCRIPT_AST_OPERAND_TYPE_INT:
			case PX_SCRIPT_AST_OPERAND_TYPE_INT_CONST:
			case PX_SCRIPT_AST_OPERAND_TYPE_FLOAT:
			case PX_SCRIPT_AST_OPERAND_TYPE_FLOAT_CONST:
			case PX_SCRIPT_AST_OPERAND_TYPE_STRING_IDX:
			case PX_SCRIPT_AST_OPERAND_TYPE_MEMORY_IDX:
				{
					if(!PX_ScriptParseAST_MapTokenToR2(analysis,operand2,out)) return PX_FALSE;
					if(!PX_ScriptParseAST_MapTokenToR1(analysis,operand1,out)) return PX_FALSE;
					PX_StringCat(out,"MUL R1,R2\nPUSH R1\n");
					pTop=PX_VECTORLAST(PX_SCRIPT_AST_OPERAND,tk);
					pTop->operandType=PX_SCRIPT_AST_OPERAND_TYPE_FLOAT_CONST;
					pTop->region=PX_SCRIPT_VARIABLE_REGION_POP;
					pTop->_oft=0;
				}
				break;
			default:
				return PX_FALSE;
			}
		}
		break;

	default:
		return PX_FALSE;
	}

	return PX_TRUE;

}
static px_bool PX_ScriptParseLastInstr_DIV(PX_SCRIPT_Analysis *analysis,px_vector *op,px_vector *tk,px_string *out)
{
	PX_SCRIPT_AST_OPERAND operand1,operand2,*pTop;

	if (tk->size<2)
	{
		return PX_FALSE;
	}
	operand1=*((PX_SCRIPT_AST_OPERAND *)PX_VECTORAT(PX_SCRIPT_AST_OPERAND,tk,tk->size-2));
	operand2=*((PX_SCRIPT_AST_OPERAND *)PX_VECTORAT(PX_SCRIPT_AST_OPERAND,tk,tk->size-1));

	PX_VectorPop(op);
	PX_VectorPop(tk);
	if (operand1.operandType==PX_SCRIPT_AST_OPERAND_TYPE_INT_CONST&&operand1.region==PX_SCRIPT_VARIABLE_REGION_GLOBAL)
	{
		if (operand2._int==0)
		{
			PX_ScriptTranslatorError(&analysis->lexer,"Divide by zero error.");
			return PX_FALSE;
		}

		if (operand2.operandType==PX_SCRIPT_AST_OPERAND_TYPE_INT_CONST&&operand2.region==PX_SCRIPT_VARIABLE_REGION_GLOBAL)
		{
			pTop=PX_VECTORLAST(PX_SCRIPT_AST_OPERAND,tk);
			pTop->_int=operand1._int/operand2._int;
			return PX_TRUE;
		}

		if (operand2.operandType==PX_SCRIPT_AST_OPERAND_TYPE_FLOAT_CONST&&operand2.region==PX_SCRIPT_VARIABLE_REGION_GLOBAL)
		{
			pTop=PX_VECTORLAST(PX_SCRIPT_AST_OPERAND,tk);
			pTop->operandType=PX_SCRIPT_AST_OPERAND_TYPE_FLOAT_CONST;
			pTop->_float=operand1._int/operand2._float;
			return PX_TRUE;
		}
	}

	if (operand1.operandType==PX_SCRIPT_AST_OPERAND_TYPE_FLOAT_CONST&&operand1.region==PX_SCRIPT_VARIABLE_REGION_GLOBAL)
	{
		if (operand2._int==0)
		{
			PX_ScriptTranslatorError(&analysis->lexer,"Divide by zero error.");
			return PX_FALSE;
		}
		if (operand2.operandType==PX_SCRIPT_AST_OPERAND_TYPE_INT_CONST&&operand2.region==PX_SCRIPT_VARIABLE_REGION_GLOBAL)
		{
			pTop=PX_VECTORLAST(PX_SCRIPT_AST_OPERAND,tk);
			pTop->operandType=PX_SCRIPT_AST_OPERAND_TYPE_FLOAT_CONST;
			pTop->_float=operand1._float/operand2._int;
			return PX_TRUE;
		}

		if (operand2.operandType==PX_SCRIPT_AST_OPERAND_TYPE_FLOAT_CONST&&operand2.region==PX_SCRIPT_VARIABLE_REGION_GLOBAL)
		{
			pTop=PX_VECTORLAST(PX_SCRIPT_AST_OPERAND,tk);
			pTop->_float=operand1._float/operand2._float;
			return PX_TRUE;
		}
	}

	switch(operand1.operandType)
	{
	case PX_SCRIPT_AST_OPERAND_TYPE_INT:
	case PX_SCRIPT_AST_OPERAND_TYPE_INT_CONST:
	case PX_SCRIPT_AST_OPERAND_TYPE_STRING_IDX:
	case PX_SCRIPT_AST_OPERAND_TYPE_MEMORY_IDX:
		{
			switch(operand2.operandType)
			{
			case PX_SCRIPT_AST_OPERAND_TYPE_INT:
			case PX_SCRIPT_AST_OPERAND_TYPE_INT_CONST:
			case PX_SCRIPT_AST_OPERAND_TYPE_FLOAT:
			case PX_SCRIPT_AST_OPERAND_TYPE_FLOAT_CONST:
			case PX_SCRIPT_AST_OPERAND_TYPE_STRING_IDX:
			case PX_SCRIPT_AST_OPERAND_TYPE_MEMORY_IDX:
				{
					if(!PX_ScriptParseAST_MapTokenToR2(analysis,operand2,out)) return PX_FALSE;
					if(!PX_ScriptParseAST_MapTokenToR1(analysis,operand1,out)) return PX_FALSE;
					PX_StringCat(out,"DIV R1,R2\nPUSH R1\n");
					pTop=PX_VECTORLAST(PX_SCRIPT_AST_OPERAND,tk);
					if(operand2.operandType==PX_SCRIPT_AST_OPERAND_TYPE_FLOAT||operand2.operandType==PX_SCRIPT_AST_OPERAND_TYPE_FLOAT_CONST)
						pTop->operandType=PX_SCRIPT_AST_OPERAND_TYPE_FLOAT_CONST;
					else
						pTop->operandType=PX_SCRIPT_AST_OPERAND_TYPE_INT_CONST;

					pTop->region=PX_SCRIPT_VARIABLE_REGION_POP;
					pTop->_oft=0;
				}
				break;
			default:
				return PX_FALSE;
			}
		}
		break;

	case PX_SCRIPT_AST_OPERAND_TYPE_FLOAT_CONST:
	case PX_SCRIPT_AST_OPERAND_TYPE_FLOAT:
		{
			switch(operand2.operandType)
			{
			case PX_SCRIPT_AST_OPERAND_TYPE_INT:
			case PX_SCRIPT_AST_OPERAND_TYPE_INT_CONST:
			case PX_SCRIPT_AST_OPERAND_TYPE_FLOAT:
			case PX_SCRIPT_AST_OPERAND_TYPE_FLOAT_CONST:
			case PX_SCRIPT_AST_OPERAND_TYPE_STRING_IDX:
			case PX_SCRIPT_AST_OPERAND_TYPE_MEMORY_IDX:
				{
					if(!PX_ScriptParseAST_MapTokenToR2(analysis,operand2,out)) return PX_FALSE;
					if(!PX_ScriptParseAST_MapTokenToR1(analysis,operand1,out)) return PX_FALSE;
					PX_StringCat(out,"DIV R1,R2\nPUSH R1\n");
					pTop=PX_VECTORLAST(PX_SCRIPT_AST_OPERAND,tk);
					pTop->operandType=PX_SCRIPT_AST_OPERAND_TYPE_FLOAT_CONST;
					pTop->region=PX_SCRIPT_VARIABLE_REGION_POP;
					pTop->_oft=0;
				}
				break;
			default:
				return PX_FALSE;
			}
		}
		break;

	default:
		return PX_FALSE;
	}

	return PX_TRUE;

}
static px_bool PX_ScriptParseLastInstr_AND(PX_SCRIPT_Analysis *analysis,px_vector *op,px_vector *tk,px_string *out)
{
	PX_SCRIPT_AST_OPERAND operand1,operand2,*pTop;

	if (tk->size<2)
	{
		return PX_FALSE;
	}
	operand1=*((PX_SCRIPT_AST_OPERAND *)PX_VECTORAT(PX_SCRIPT_AST_OPERAND,tk,tk->size-2));
	operand2=*((PX_SCRIPT_AST_OPERAND *)PX_VECTORAT(PX_SCRIPT_AST_OPERAND,tk,tk->size-1));

	PX_VectorPop(op);
	PX_VectorPop(tk);
	if (operand1.operandType==PX_SCRIPT_AST_OPERAND_TYPE_INT_CONST&&operand1.region==PX_SCRIPT_VARIABLE_REGION_GLOBAL)
	{
		if (operand2.operandType==PX_SCRIPT_AST_OPERAND_TYPE_INT_CONST&&operand2.region==PX_SCRIPT_VARIABLE_REGION_GLOBAL)
		{
			pTop=PX_VECTORLAST(PX_SCRIPT_AST_OPERAND,tk);
			pTop->_int=operand1._int&operand2._int;
			return PX_TRUE;
		}
	}


	switch(operand1.operandType)
	{
	case PX_SCRIPT_AST_OPERAND_TYPE_INT:
	case PX_SCRIPT_AST_OPERAND_TYPE_INT_CONST:
	case PX_SCRIPT_AST_OPERAND_TYPE_STRING_IDX:
	case PX_SCRIPT_AST_OPERAND_TYPE_MEMORY_IDX:
		{
			switch(operand2.operandType)
			{
			case PX_SCRIPT_AST_OPERAND_TYPE_INT:
			case PX_SCRIPT_AST_OPERAND_TYPE_INT_CONST:
			case PX_SCRIPT_AST_OPERAND_TYPE_STRING_IDX:
			case PX_SCRIPT_AST_OPERAND_TYPE_MEMORY_IDX:
				{
					if(!PX_ScriptParseAST_MapTokenToR2(analysis,operand2,out)) return PX_FALSE;
					if(!PX_ScriptParseAST_MapTokenToR1(analysis,operand1,out)) return PX_FALSE;
					PX_StringCat(out,"AND R1,R2\nPUSH R1\n");
					pTop=PX_VECTORLAST(PX_SCRIPT_AST_OPERAND,tk);
					pTop->operandType=PX_SCRIPT_AST_OPERAND_TYPE_INT_CONST;
					pTop->region=PX_SCRIPT_VARIABLE_REGION_POP;
					pTop->_oft=0;
				}
				break;
			default:
				return PX_FALSE;
			}
		}
		break;

	default:
		return PX_FALSE;
	}

	return PX_TRUE;

}
static px_bool PX_ScriptParseLastInstr_OR(PX_SCRIPT_Analysis *analysis,px_vector *op,px_vector *tk,px_string *out)
{
	PX_SCRIPT_AST_OPERAND operand1,operand2,*pTop;

	if (tk->size<2)
	{
		return PX_FALSE;
	}
	operand1=*((PX_SCRIPT_AST_OPERAND *)PX_VECTORAT(PX_SCRIPT_AST_OPERAND,tk,tk->size-2));
	operand2=*((PX_SCRIPT_AST_OPERAND *)PX_VECTORAT(PX_SCRIPT_AST_OPERAND,tk,tk->size-1));

	PX_VectorPop(op);
	PX_VectorPop(tk);
	if (operand1.operandType==PX_SCRIPT_AST_OPERAND_TYPE_INT_CONST&&operand1.region==PX_SCRIPT_VARIABLE_REGION_GLOBAL)
	{
		if (operand2.operandType==PX_SCRIPT_AST_OPERAND_TYPE_INT_CONST&&operand2.region==PX_SCRIPT_VARIABLE_REGION_GLOBAL)
		{
			pTop=PX_VECTORLAST(PX_SCRIPT_AST_OPERAND,tk);
			pTop->_int=operand1._int|operand2._int;
			return PX_TRUE;
		}
	}


	switch(operand1.operandType)
	{
	case PX_SCRIPT_AST_OPERAND_TYPE_INT:
	case PX_SCRIPT_AST_OPERAND_TYPE_INT_CONST:
	case PX_SCRIPT_AST_OPERAND_TYPE_STRING_IDX:
	case PX_SCRIPT_AST_OPERAND_TYPE_MEMORY_IDX:
		{
			switch(operand2.operandType)
			{
			case PX_SCRIPT_AST_OPERAND_TYPE_INT:
			case PX_SCRIPT_AST_OPERAND_TYPE_INT_CONST:
			case PX_SCRIPT_AST_OPERAND_TYPE_STRING_IDX:
			case PX_SCRIPT_AST_OPERAND_TYPE_MEMORY_IDX:
				{
					if(!PX_ScriptParseAST_MapTokenToR2(analysis,operand2,out)) return PX_FALSE;
					if(!PX_ScriptParseAST_MapTokenToR1(analysis,operand1,out)) return PX_FALSE;
					PX_StringCat(out,"OR R1,R2\nPUSH R1\n");
					pTop=PX_VECTORLAST(PX_SCRIPT_AST_OPERAND,tk);
					pTop->operandType=PX_SCRIPT_AST_OPERAND_TYPE_INT_CONST;
					pTop->region=PX_SCRIPT_VARIABLE_REGION_POP;
					pTop->_oft=0;
				}
				break;
			default:
				return PX_FALSE;
			}
		}
		break;

	default:
		return PX_FALSE;
	}

	return PX_TRUE;

}
static px_bool PX_ScriptParseLastInstr_NOT(PX_SCRIPT_Analysis *analysis,px_vector *op,px_vector *tk,px_string *out)
{
	PX_SCRIPT_AST_OPERAND operand1,*pTop;

	if (tk->size<1)
	{
		return PX_FALSE;
	}
	operand1=*((PX_SCRIPT_AST_OPERAND *)PX_VECTORAT(PX_SCRIPT_AST_OPERAND,tk,tk->size-1));
	pTop=PX_VECTORLAST(PX_SCRIPT_AST_OPERAND,tk);
	PX_VectorPop(op);
	if (operand1.operandType==PX_SCRIPT_AST_OPERAND_TYPE_INT_CONST&&operand1.region==PX_SCRIPT_VARIABLE_REGION_GLOBAL)
	{
		pTop=PX_VECTORLAST(PX_SCRIPT_AST_OPERAND,tk);
		pTop->_int=(operand1._int==0);
		return PX_TRUE;
	}

	switch(operand1.operandType)
	{
	case PX_SCRIPT_AST_OPERAND_TYPE_INT:
	case PX_SCRIPT_AST_OPERAND_TYPE_INT_CONST:
	case PX_SCRIPT_AST_OPERAND_TYPE_STRING_IDX:
	case PX_SCRIPT_AST_OPERAND_TYPE_MEMORY_IDX:
		pTop->operandType=PX_SCRIPT_AST_OPERAND_TYPE_INT_CONST;
		break;
	default:
		return PX_FALSE;
	}
	pTop->region=PX_SCRIPT_VARIABLE_REGION_POP;
	pTop->_oft=0;
	pTop->pSet=PX_NULL;
	pTop->_int=0;
	if(!PX_ScriptParseAST_MapTokenToR1(analysis,operand1,out)) return PX_FALSE;
	PX_StringCat(out,"NOT R1\n");
	PX_StringCat(out,"PUSH R1\n");
	return PX_TRUE;
}
static px_bool PX_ScriptParseLastInstr_XOR(PX_SCRIPT_Analysis *analysis,px_vector *op,px_vector *tk,px_string *out)
{
	PX_SCRIPT_AST_OPERAND operand1,operand2,*pTop;

	if (tk->size<2)
	{
		return PX_FALSE;
	}
	operand1=*((PX_SCRIPT_AST_OPERAND *)PX_VECTORAT(PX_SCRIPT_AST_OPERAND,tk,tk->size-2));
	operand2=*((PX_SCRIPT_AST_OPERAND *)PX_VECTORAT(PX_SCRIPT_AST_OPERAND,tk,tk->size-1));

	PX_VectorPop(op);
	PX_VectorPop(tk);
	if (operand1.operandType==PX_SCRIPT_AST_OPERAND_TYPE_INT_CONST&&operand1.region==PX_SCRIPT_VARIABLE_REGION_GLOBAL)
	{
		if (operand2.operandType==PX_SCRIPT_AST_OPERAND_TYPE_INT_CONST&&operand2.region==PX_SCRIPT_VARIABLE_REGION_GLOBAL)
		{
			pTop=PX_VECTORLAST(PX_SCRIPT_AST_OPERAND,tk);
			pTop->_int=operand1._int^operand2._int;
			return PX_TRUE;
		}
	}


	switch(operand1.operandType)
	{
	case PX_SCRIPT_AST_OPERAND_TYPE_INT:
	case PX_SCRIPT_AST_OPERAND_TYPE_INT_CONST:
	case PX_SCRIPT_AST_OPERAND_TYPE_STRING_IDX:
	case PX_SCRIPT_AST_OPERAND_TYPE_MEMORY_IDX:
		{
			switch(operand2.operandType)
			{
			case PX_SCRIPT_AST_OPERAND_TYPE_INT:
			case PX_SCRIPT_AST_OPERAND_TYPE_INT_CONST:
			case PX_SCRIPT_AST_OPERAND_TYPE_STRING_IDX:
			case PX_SCRIPT_AST_OPERAND_TYPE_MEMORY_IDX:
				{
					if(!PX_ScriptParseAST_MapTokenToR2(analysis,operand2,out)) return PX_FALSE;
					if(!PX_ScriptParseAST_MapTokenToR1(analysis,operand1,out)) return PX_FALSE;
					PX_StringCat(out,"XOR R1,R2\nPUSH R1\n");
					pTop=PX_VECTORLAST(PX_SCRIPT_AST_OPERAND,tk);
					pTop->operandType=PX_SCRIPT_AST_OPERAND_TYPE_INT_CONST;
					pTop->region=PX_SCRIPT_VARIABLE_REGION_POP;
					pTop->_oft=0;
				}
				break;
			default:
				return PX_FALSE;
			}
		}
		break;

	default:
		return PX_FALSE;
	}

	return PX_TRUE;

}
static px_bool PX_ScriptParseLastInstr_INV(PX_SCRIPT_Analysis *analysis,px_vector *op,px_vector *tk,px_string *out)
{
	PX_SCRIPT_AST_OPERAND operand1,*pTop;

	if (tk->size<1)
	{
		return PX_FALSE;
	}
	operand1=*((PX_SCRIPT_AST_OPERAND *)PX_VECTORAT(PX_SCRIPT_AST_OPERAND,tk,tk->size-1));
	pTop=PX_VECTORLAST(PX_SCRIPT_AST_OPERAND,tk);
	PX_VectorPop(op);
	if (operand1.operandType==PX_SCRIPT_AST_OPERAND_TYPE_INT_CONST&&operand1.region==PX_SCRIPT_VARIABLE_REGION_GLOBAL)
	{
		pTop=PX_VECTORLAST(PX_SCRIPT_AST_OPERAND,tk);
		pTop->_int=(~operand1._int);
		return PX_TRUE;
	}

	switch(operand1.operandType)
	{
	case PX_SCRIPT_AST_OPERAND_TYPE_INT:
	case PX_SCRIPT_AST_OPERAND_TYPE_INT_CONST:
	case PX_SCRIPT_AST_OPERAND_TYPE_STRING_IDX:
	case PX_SCRIPT_AST_OPERAND_TYPE_MEMORY_IDX:
		pTop->operandType=PX_SCRIPT_AST_OPERAND_TYPE_INT_CONST;
		break;
	default:
		return PX_FALSE;
	}
	pTop->region=PX_SCRIPT_VARIABLE_REGION_POP;
	pTop->_oft=0;
	pTop->pSet=PX_NULL;
	pTop->_int=0;
	if(!PX_ScriptParseAST_MapTokenToR1(analysis,operand1,out)) return PX_FALSE;
	PX_StringCat(out,"INV R1\n");
	PX_StringCat(out,"PUSH R1\n");
	return PX_TRUE;
}
static px_bool PX_ScriptParseLastInstr_MOD(PX_SCRIPT_Analysis *analysis,px_vector *op,px_vector *tk,px_string *out)
{
	PX_SCRIPT_AST_OPERAND operand1,operand2,*pTop;

	if (tk->size<2)
	{
		return PX_FALSE;
	}
	operand1=*((PX_SCRIPT_AST_OPERAND *)PX_VECTORAT(PX_SCRIPT_AST_OPERAND,tk,tk->size-2));
	operand2=*((PX_SCRIPT_AST_OPERAND *)PX_VECTORAT(PX_SCRIPT_AST_OPERAND,tk,tk->size-1));

	PX_VectorPop(op);
	PX_VectorPop(tk);
	if (operand1.operandType==PX_SCRIPT_AST_OPERAND_TYPE_INT_CONST&&operand1.region==PX_SCRIPT_VARIABLE_REGION_GLOBAL)
	{
		if (operand2._int==0)
		{
			PX_ScriptTranslatorError(&analysis->lexer,"mod by zero error.");
			return PX_FALSE;
		}

		if (operand2.operandType==PX_SCRIPT_AST_OPERAND_TYPE_INT_CONST&&operand2.region==PX_SCRIPT_VARIABLE_REGION_GLOBAL)
		{
			pTop=PX_VECTORLAST(PX_SCRIPT_AST_OPERAND,tk);
			pTop->_int=operand1._int%operand2._int;
			return PX_TRUE;
		}
	}

	switch(operand1.operandType)
	{
	case PX_SCRIPT_AST_OPERAND_TYPE_INT:
	case PX_SCRIPT_AST_OPERAND_TYPE_INT_CONST:
	case PX_SCRIPT_AST_OPERAND_TYPE_STRING_IDX:
	case PX_SCRIPT_AST_OPERAND_TYPE_MEMORY_IDX:
		{
			switch(operand2.operandType)
			{
			case PX_SCRIPT_AST_OPERAND_TYPE_INT:
			case PX_SCRIPT_AST_OPERAND_TYPE_INT_CONST:
			case PX_SCRIPT_AST_OPERAND_TYPE_STRING_IDX:
			case PX_SCRIPT_AST_OPERAND_TYPE_MEMORY_IDX:
				{
					if(!PX_ScriptParseAST_MapTokenToR2(analysis,operand2,out)) return PX_FALSE;
					if(!PX_ScriptParseAST_MapTokenToR1(analysis,operand1,out)) return PX_FALSE;
					PX_StringCat(out,"MOD R1,R2\nPUSH R1\n");
					pTop=PX_VECTORLAST(PX_SCRIPT_AST_OPERAND,tk);
					pTop->operandType=PX_SCRIPT_AST_OPERAND_TYPE_INT_CONST;
					pTop->region=PX_SCRIPT_VARIABLE_REGION_POP;
					pTop->_oft=0;
				}
				break;
			default:
				return PX_FALSE;
			}
		}
		break;

	default:
		return PX_FALSE;
	}

	return PX_TRUE;

}
static px_bool PX_ScriptParseLastInstr_SHL(PX_SCRIPT_Analysis *analysis,px_vector *op,px_vector *tk,px_string *out)
{
	PX_SCRIPT_AST_OPERAND operand1,operand2,*pTop;

	if (tk->size<2)
	{
		return PX_FALSE;
	}
	operand1=*((PX_SCRIPT_AST_OPERAND *)PX_VECTORAT(PX_SCRIPT_AST_OPERAND,tk,tk->size-2));
	operand2=*((PX_SCRIPT_AST_OPERAND *)PX_VECTORAT(PX_SCRIPT_AST_OPERAND,tk,tk->size-1));

	PX_VectorPop(op);
	PX_VectorPop(tk);
	if (operand1.operandType==PX_SCRIPT_AST_OPERAND_TYPE_INT_CONST&&operand1.region==PX_SCRIPT_VARIABLE_REGION_GLOBAL)
	{
		if (operand2.operandType==PX_SCRIPT_AST_OPERAND_TYPE_INT_CONST&&operand2.region==PX_SCRIPT_VARIABLE_REGION_GLOBAL)
		{
			
			pTop=PX_VECTORLAST(PX_SCRIPT_AST_OPERAND,tk);
			pTop->_int=operand1._int<<operand2._int;
			return PX_TRUE;
		}
	}


	switch(operand1.operandType)
	{
	case PX_SCRIPT_AST_OPERAND_TYPE_INT:
	case PX_SCRIPT_AST_OPERAND_TYPE_INT_CONST:
	case PX_SCRIPT_AST_OPERAND_TYPE_STRING_IDX:
	case PX_SCRIPT_AST_OPERAND_TYPE_MEMORY_IDX:
		{
			switch(operand2.operandType)
			{
			case PX_SCRIPT_AST_OPERAND_TYPE_INT:
			case PX_SCRIPT_AST_OPERAND_TYPE_INT_CONST:
			case PX_SCRIPT_AST_OPERAND_TYPE_STRING_IDX:
			case PX_SCRIPT_AST_OPERAND_TYPE_MEMORY_IDX:
				{
					if(!PX_ScriptParseAST_MapTokenToR2(analysis,operand2,out)) return PX_FALSE;
					if(!PX_ScriptParseAST_MapTokenToR1(analysis,operand1,out)) return PX_FALSE;
					PX_StringCat(out,"SHL R1,R2\nPUSH R1\n");
					pTop=PX_VECTORLAST(PX_SCRIPT_AST_OPERAND,tk);
					pTop->operandType=PX_SCRIPT_AST_OPERAND_TYPE_INT_CONST;
					pTop->region=PX_SCRIPT_VARIABLE_REGION_POP;
					pTop->_oft=0;
				}
				break;
			default:
				return PX_FALSE;
			}
		}
		break;

	default:
		return PX_FALSE;
	}

	return PX_TRUE;

}
static px_bool PX_ScriptParseLastInstr_SHR(PX_SCRIPT_Analysis *analysis,px_vector *op,px_vector *tk,px_string *out)
{
	PX_SCRIPT_AST_OPERAND operand1,operand2,*pTop;

	if (tk->size<2)
	{
		return PX_FALSE;
	}
	operand1=*((PX_SCRIPT_AST_OPERAND *)PX_VECTORAT(PX_SCRIPT_AST_OPERAND,tk,tk->size-2));
	operand2=*((PX_SCRIPT_AST_OPERAND *)PX_VECTORAT(PX_SCRIPT_AST_OPERAND,tk,tk->size-1));

	PX_VectorPop(op);
	PX_VectorPop(tk);
	if (operand1.operandType==PX_SCRIPT_AST_OPERAND_TYPE_INT_CONST&&operand1.region==PX_SCRIPT_VARIABLE_REGION_GLOBAL)
	{
		if (operand2.operandType==PX_SCRIPT_AST_OPERAND_TYPE_INT_CONST&&operand2.region==PX_SCRIPT_VARIABLE_REGION_GLOBAL)
		{
			
			pTop=PX_VECTORLAST(PX_SCRIPT_AST_OPERAND,tk);
			pTop->_int=operand1._int>>operand2._int;
			return PX_TRUE;
		}
	}


	switch(operand1.operandType)
	{
	case PX_SCRIPT_AST_OPERAND_TYPE_INT:
	case PX_SCRIPT_AST_OPERAND_TYPE_INT_CONST:
	case PX_SCRIPT_AST_OPERAND_TYPE_STRING_IDX:
	case PX_SCRIPT_AST_OPERAND_TYPE_MEMORY_IDX:
		{
			switch(operand2.operandType)
			{
			case PX_SCRIPT_AST_OPERAND_TYPE_INT:
			case PX_SCRIPT_AST_OPERAND_TYPE_INT_CONST:
			case PX_SCRIPT_AST_OPERAND_TYPE_STRING_IDX:
			case PX_SCRIPT_AST_OPERAND_TYPE_MEMORY_IDX:
				{
					if(!PX_ScriptParseAST_MapTokenToR2(analysis,operand2,out)) return PX_FALSE;
					if(!PX_ScriptParseAST_MapTokenToR1(analysis,operand1,out)) return PX_FALSE;
					PX_StringCat(out,"SHR R1,R2\nPUSH R1\n");
					pTop=PX_VECTORLAST(PX_SCRIPT_AST_OPERAND,tk);
					pTop->operandType=PX_SCRIPT_AST_OPERAND_TYPE_INT_CONST;
					pTop->region=PX_SCRIPT_VARIABLE_REGION_POP;
					pTop->_oft=0;
				}
				break;
			default:
				return PX_FALSE;
			}
		}
		break;

	default:
		return PX_FALSE;
	}

	return PX_TRUE;

}
static px_bool PX_ScriptParseLastInstr_INT(PX_SCRIPT_Analysis *analysis,px_vector *op,px_vector *tk,px_string *out)
{
	PX_SCRIPT_AST_OPERAND operand1,*pTop;

	if (tk->size<1)
	{
		return PX_FALSE;
	}
	operand1=*((PX_SCRIPT_AST_OPERAND *)PX_VECTORAT(PX_SCRIPT_AST_OPERAND,tk,tk->size-1));
	pTop=PX_VECTORLAST(PX_SCRIPT_AST_OPERAND,tk);
	PX_VectorPop(op);
	pTop->region=PX_SCRIPT_VARIABLE_REGION_POP;
	pTop->_oft=0;
	pTop->pSet=PX_NULL;
	pTop->_int=0;
	if(!PX_ScriptParseAST_MapTokenToR1(analysis,operand1,out)) return PX_FALSE;
	pTop->operandType=PX_SCRIPT_AST_OPERAND_TYPE_INT_CONST;

	switch(operand1.operandType)
	{
	case PX_SCRIPT_AST_OPERAND_TYPE_INT:
	case PX_SCRIPT_AST_OPERAND_TYPE_INT_CONST:
	case PX_SCRIPT_AST_OPERAND_TYPE_STRING_IDX:
	case PX_SCRIPT_AST_OPERAND_TYPE_MEMORY_IDX:
	case PX_SCRIPT_AST_OPERAND_TYPE_FLOAT:
	case PX_SCRIPT_AST_OPERAND_TYPE_FLOAT_CONST:
		PX_StringCat(out,"INT R1\n");
		break;
	case PX_SCRIPT_AST_OPERAND_TYPE_STRING:
	case PX_SCRIPT_AST_OPERAND_TYPE_STRING_CONST:
		PX_StringCat(out,"STRTOI R1,R1\n");
		break;
	default:
		return PX_FALSE;
	}
	PX_StringCat(out,"PUSH R1\n");
	return PX_TRUE;
}
static px_bool PX_ScriptParseLastInstr_FLOAT(PX_SCRIPT_Analysis *analysis,px_vector *op,px_vector *tk,px_string *out)
{
	PX_SCRIPT_AST_OPERAND operand1,*pTop;

	if (tk->size<1)
	{
		return PX_FALSE;
	}
	operand1=*((PX_SCRIPT_AST_OPERAND *)PX_VECTORAT(PX_SCRIPT_AST_OPERAND,tk,tk->size-1));
	pTop=PX_VECTORLAST(PX_SCRIPT_AST_OPERAND,tk);
	PX_VectorPop(op);
	pTop->region=PX_SCRIPT_VARIABLE_REGION_POP;
	pTop->_oft=0;
	pTop->pSet=PX_NULL;
	pTop->_int=0;
	if(!PX_ScriptParseAST_MapTokenToR1(analysis,operand1,out)) return PX_FALSE;
	pTop->operandType=PX_SCRIPT_AST_OPERAND_TYPE_FLOAT_CONST;

	switch(operand1.operandType)
	{
	case PX_SCRIPT_AST_OPERAND_TYPE_INT:
	case PX_SCRIPT_AST_OPERAND_TYPE_INT_CONST:
	case PX_SCRIPT_AST_OPERAND_TYPE_STRING_IDX:
	case PX_SCRIPT_AST_OPERAND_TYPE_MEMORY_IDX:
	case PX_SCRIPT_AST_OPERAND_TYPE_FLOAT:
	case PX_SCRIPT_AST_OPERAND_TYPE_FLOAT_CONST:
		PX_StringCat(out,"FLT R1\n");
		break;
	case PX_SCRIPT_AST_OPERAND_TYPE_STRING:
	case PX_SCRIPT_AST_OPERAND_TYPE_STRING_CONST:
		PX_StringCat(out,"STRTOF R1,R1\n");
		break;
	default:
		return PX_FALSE;
	}
	PX_StringCat(out,"PUSH R1\n");
	return PX_TRUE;
}
static px_bool PX_ScriptParseLastInstr_STRING(PX_SCRIPT_Analysis *analysis,px_vector *op,px_vector *tk,px_string *out)
{
	PX_SCRIPT_AST_OPERAND operand1,*pTop;

	if (tk->size<1)
	{
		return PX_FALSE;
	}
	operand1=*((PX_SCRIPT_AST_OPERAND *)PX_VECTORAT(PX_SCRIPT_AST_OPERAND,tk,tk->size-1));
	pTop=PX_VECTORLAST(PX_SCRIPT_AST_OPERAND,tk);
	PX_VectorPop(op);
	pTop->region=PX_SCRIPT_VARIABLE_REGION_POP;
	pTop->_oft=0;
	pTop->pSet=PX_NULL;
	pTop->_int=0;
	if(!PX_ScriptParseAST_MapTokenToR1(analysis,operand1,out)) return PX_FALSE;
	pTop->operandType=PX_SCRIPT_AST_OPERAND_TYPE_STRING_CONST;

	switch(operand1.operandType)
	{
	case PX_SCRIPT_AST_OPERAND_TYPE_INT:
	case PX_SCRIPT_AST_OPERAND_TYPE_INT_CONST:
	case PX_SCRIPT_AST_OPERAND_TYPE_STRING_IDX:
	case PX_SCRIPT_AST_OPERAND_TYPE_MEMORY_IDX:
		PX_StringCat(out,"STRFRI R1,R1\n");
		break;
	case PX_SCRIPT_AST_OPERAND_TYPE_FLOAT:
	case PX_SCRIPT_AST_OPERAND_TYPE_FLOAT_CONST:
		PX_StringCat(out,"STRFRF R1,R1\n");
		break;
	case PX_SCRIPT_AST_OPERAND_TYPE_STRING:
	case PX_SCRIPT_AST_OPERAND_TYPE_STRING_CONST:
		break;
	case PX_SCRIPT_AST_OPERAND_TYPE_MEMORY:
	case PX_SCRIPT_AST_OPERAND_TYPE_MEMORY_CONST:
		PX_StringCat(out,"MEMTSTR R1\n");
		break;
	default:
		return PX_FALSE;
	}
	PX_StringCat(out,"PUSH R1\n");
	return PX_TRUE;
}
static px_bool PX_ScriptParseLastInstr_MEMORY(PX_SCRIPT_Analysis *analysis,px_vector *op,px_vector *tk,px_string *out)
{
	PX_SCRIPT_AST_OPERAND operand1,*pTop;

	if (tk->size<1)
	{
		return PX_FALSE;
	}
	operand1=*((PX_SCRIPT_AST_OPERAND *)PX_VECTORAT(PX_SCRIPT_AST_OPERAND,tk,tk->size-1));
	pTop=PX_VECTORLAST(PX_SCRIPT_AST_OPERAND,tk);
	PX_VectorPop(op);
	pTop->region=PX_SCRIPT_VARIABLE_REGION_POP;
	pTop->_oft=0;
	pTop->pSet=PX_NULL;
	pTop->_int=0;
	if(!PX_ScriptParseAST_MapTokenToR1(analysis,operand1,out)) return PX_FALSE;
	pTop->operandType=PX_SCRIPT_AST_OPERAND_TYPE_MEMORY_CONST;

	switch(operand1.operandType)
	{
	case PX_SCRIPT_AST_OPERAND_TYPE_STRING:
	case PX_SCRIPT_AST_OPERAND_TYPE_STRING_CONST:
		PX_StringCat(out,"STRTMEM R1\n");
		break;
	default:
		return PX_FALSE;
	}
	PX_StringCat(out,"PUSH R1\n");
	return PX_TRUE;
}
static px_bool PX_ScriptParseLastInstr_STRLEN(PX_SCRIPT_Analysis *analysis,px_vector *op,px_vector *tk,px_string *out)
{
	PX_SCRIPT_AST_OPERAND operand1,*pTop;

	if (tk->size<1)
	{
		return PX_FALSE;
	}
	operand1=*((PX_SCRIPT_AST_OPERAND *)PX_VECTORAT(PX_SCRIPT_AST_OPERAND,tk,tk->size-1));
	pTop=PX_VECTORLAST(PX_SCRIPT_AST_OPERAND,tk);
	PX_VectorPop(op);
	pTop->region=PX_SCRIPT_VARIABLE_REGION_POP;
	pTop->_oft=0;
	pTop->pSet=PX_NULL;
	pTop->_int=0;
	if(!PX_ScriptParseAST_MapTokenToR2(analysis,operand1,out)) return PX_FALSE;
	pTop->operandType=PX_SCRIPT_AST_OPERAND_TYPE_INT_CONST;

	switch(operand1.operandType)
	{
	case PX_SCRIPT_AST_OPERAND_TYPE_STRING:
	case PX_SCRIPT_AST_OPERAND_TYPE_STRING_CONST:
		PX_StringCat(out,"STRLEN R1,R2\n");
		break;
	default:
		return PX_FALSE;
	}
	PX_StringCat(out,"PUSH R1\n");
	return PX_TRUE;
}
static px_bool PX_ScriptParseLastInstr_MEMLEN(PX_SCRIPT_Analysis *analysis,px_vector *op,px_vector *tk,px_string *out)
{
	PX_SCRIPT_AST_OPERAND operand1,*pTop;

	if (tk->size<1)
	{
		return PX_FALSE;
	}
	operand1=*((PX_SCRIPT_AST_OPERAND *)PX_VECTORAT(PX_SCRIPT_AST_OPERAND,tk,tk->size-1));
	pTop=PX_VECTORLAST(PX_SCRIPT_AST_OPERAND,tk);
	PX_VectorPop(op);
	pTop->region=PX_SCRIPT_VARIABLE_REGION_POP;
	pTop->_oft=0;
	pTop->pSet=PX_NULL;
	pTop->_int=0;
	if(!PX_ScriptParseAST_MapTokenToR2(analysis,operand1,out)) return PX_FALSE;
	pTop->operandType=PX_SCRIPT_AST_OPERAND_TYPE_INT_CONST;

	switch(operand1.operandType)
	{
	case PX_SCRIPT_AST_OPERAND_TYPE_MEMORY:
	case PX_SCRIPT_AST_OPERAND_TYPE_MEMORY_CONST:
		PX_StringCat(out,"MEMLEN R1,R2\n");
		break;
	default:
		return PX_FALSE;
	}
	PX_StringCat(out,"PUSH R1\n");
	return PX_TRUE;
}

static px_bool PX_ScriptParseLastInstr_LARGE(PX_SCRIPT_Analysis *analysis,px_vector *op,px_vector *tk,px_string *out)
{
	PX_SCRIPT_AST_OPERAND operand1,operand2,*pTop;

	if (tk->size<2)
	{
		return PX_FALSE;
	}
	operand1=*((PX_SCRIPT_AST_OPERAND *)PX_VECTORAT(PX_SCRIPT_AST_OPERAND,tk,tk->size-2));
	operand2=*((PX_SCRIPT_AST_OPERAND *)PX_VECTORAT(PX_SCRIPT_AST_OPERAND,tk,tk->size-1));

	PX_VectorPop(op);
	PX_VectorPop(tk);
	pTop=PX_VECTORLAST(PX_SCRIPT_AST_OPERAND,tk);
	pTop->operandType=PX_SCRIPT_AST_OPERAND_TYPE_INT_CONST;
	pTop->region=PX_SCRIPT_VARIABLE_REGION_POP;
	pTop->_oft=0;
	if (!PX_ScriptParseIsOperandNumericType(operand1)||!PX_ScriptParseIsOperandNumericType(operand2))
	{
		return PX_FALSE;
	}
	if(!PX_ScriptParseAST_MapTokenToR2(analysis,operand2,out)) return PX_FALSE;
	if(!PX_ScriptParseAST_MapTokenToR1(analysis,operand1,out)) return PX_FALSE;

	PX_StringCat(out,"SUB R1,R2\n");
	PX_StringCat(out,"LGGZ R1\n");
	PX_StringCat(out,"PUSH R1\n");
	return PX_TRUE;
}
static px_bool PX_ScriptParseLastInstr_LARGEEQU(PX_SCRIPT_Analysis *analysis,px_vector *op,px_vector *tk,px_string *out)
{
	PX_SCRIPT_AST_OPERAND operand1,operand2,*pTop;

	if (tk->size<2)
	{
		return PX_FALSE;
	}
	operand1=*((PX_SCRIPT_AST_OPERAND *)PX_VECTORAT(PX_SCRIPT_AST_OPERAND,tk,tk->size-2));
	operand2=*((PX_SCRIPT_AST_OPERAND *)PX_VECTORAT(PX_SCRIPT_AST_OPERAND,tk,tk->size-1));

	PX_VectorPop(op);
	PX_VectorPop(tk);
	pTop=PX_VECTORLAST(PX_SCRIPT_AST_OPERAND,tk);
	pTop->operandType=PX_SCRIPT_AST_OPERAND_TYPE_INT_CONST;
	pTop->region=PX_SCRIPT_VARIABLE_REGION_POP;
	pTop->_oft=0;
	if (!PX_ScriptParseIsOperandNumericType(operand1)||!PX_ScriptParseIsOperandNumericType(operand2))
	{
		return PX_FALSE;
	}
	if(!PX_ScriptParseAST_MapTokenToR2(analysis,operand2,out)) return PX_FALSE;
	if(!PX_ScriptParseAST_MapTokenToR1(analysis,operand1,out)) return PX_FALSE;

	PX_StringCat(out,"SUB R1,R2\n");
	PX_StringCat(out,"LGGEZ R1\n");
	PX_StringCat(out,"PUSH R1\n");
	return PX_TRUE;
}
static px_bool PX_ScriptParseLastInstr_LESS(PX_SCRIPT_Analysis *analysis,px_vector *op,px_vector *tk,px_string *out)
{
	PX_SCRIPT_AST_OPERAND operand1,operand2,*pTop;

	if (tk->size<2)
	{
		return PX_FALSE;
	}
	operand1=*((PX_SCRIPT_AST_OPERAND *)PX_VECTORAT(PX_SCRIPT_AST_OPERAND,tk,tk->size-2));
	operand2=*((PX_SCRIPT_AST_OPERAND *)PX_VECTORAT(PX_SCRIPT_AST_OPERAND,tk,tk->size-1));

	PX_VectorPop(op);
	PX_VectorPop(tk);
	pTop=PX_VECTORLAST(PX_SCRIPT_AST_OPERAND,tk);
	pTop->operandType=PX_SCRIPT_AST_OPERAND_TYPE_INT_CONST;
	pTop->region=PX_SCRIPT_VARIABLE_REGION_POP;
	pTop->_oft=0;
	if (!PX_ScriptParseIsOperandNumericType(operand1)||!PX_ScriptParseIsOperandNumericType(operand2))
	{
		return PX_FALSE;
	}
	if(!PX_ScriptParseAST_MapTokenToR2(analysis,operand2,out)) return PX_FALSE;
	if(!PX_ScriptParseAST_MapTokenToR1(analysis,operand1,out)) return PX_FALSE;

	PX_StringCat(out,"SUB R1,R2\n");
	PX_StringCat(out,"LGLZ R1\n");
	PX_StringCat(out,"PUSH R1\n");
	return PX_TRUE;
}
static px_bool PX_ScriptParseLastInstr_LAND(PX_SCRIPT_Analysis *analysis,px_vector *op,px_vector *tk,px_string *out)
{
	PX_SCRIPT_AST_OPERAND operand1,operand2,*pTop;

	if (tk->size<2)
	{
		return PX_FALSE;
	}
	operand1=*((PX_SCRIPT_AST_OPERAND *)PX_VECTORAT(PX_SCRIPT_AST_OPERAND,tk,tk->size-2));
	operand2=*((PX_SCRIPT_AST_OPERAND *)PX_VECTORAT(PX_SCRIPT_AST_OPERAND,tk,tk->size-1));

	PX_VectorPop(op);
	PX_VectorPop(tk);
	pTop=PX_VECTORLAST(PX_SCRIPT_AST_OPERAND,tk);
	pTop->operandType=PX_SCRIPT_AST_OPERAND_TYPE_INT_CONST;
	pTop->region=PX_SCRIPT_VARIABLE_REGION_POP;
	pTop->_oft=0;
	if (!PX_ScriptParseIsOperandNumericType(operand1)||!PX_ScriptParseIsOperandNumericType(operand2))
	{
		return PX_FALSE;
	}
	if(!PX_ScriptParseAST_MapTokenToR2(analysis,operand2,out)) return PX_FALSE;
	if(!PX_ScriptParseAST_MapTokenToR1(analysis,operand1,out)) return PX_FALSE;

	PX_StringCat(out,"ANDL R1,R2\n");
	PX_StringCat(out,"PUSH R1\n");
	return PX_TRUE;
}
static px_bool PX_ScriptParseLastInstr_LOR(PX_SCRIPT_Analysis *analysis,px_vector *op,px_vector *tk,px_string *out)
{
	PX_SCRIPT_AST_OPERAND operand1,operand2,*pTop;

	if (tk->size<2)
	{
		return PX_FALSE;
	}
	operand1=*((PX_SCRIPT_AST_OPERAND *)PX_VECTORAT(PX_SCRIPT_AST_OPERAND,tk,tk->size-2));
	operand2=*((PX_SCRIPT_AST_OPERAND *)PX_VECTORAT(PX_SCRIPT_AST_OPERAND,tk,tk->size-1));

	PX_VectorPop(op);
	PX_VectorPop(tk);
	pTop=PX_VECTORLAST(PX_SCRIPT_AST_OPERAND,tk);
	pTop->operandType=PX_SCRIPT_AST_OPERAND_TYPE_INT_CONST;
	pTop->region=PX_SCRIPT_VARIABLE_REGION_POP;
	pTop->_oft=0;
	if (!PX_ScriptParseIsOperandNumericType(operand1)||!PX_ScriptParseIsOperandNumericType(operand2))
	{
		return PX_FALSE;
	}
	if(!PX_ScriptParseAST_MapTokenToR2(analysis,operand2,out)) return PX_FALSE;
	if(!PX_ScriptParseAST_MapTokenToR1(analysis,operand1,out)) return PX_FALSE;

	PX_StringCat(out,"ORL R1,R2\n");
	PX_StringCat(out,"PUSH R1\n");
	return PX_TRUE;
}
static px_bool PX_ScriptParseLastInstr_LESSEQU(PX_SCRIPT_Analysis *analysis,px_vector *op,px_vector *tk,px_string *out)
{
	PX_SCRIPT_AST_OPERAND operand1,operand2,*pTop;

	if (tk->size<2)
	{
		return PX_FALSE;
	}
	operand1=*((PX_SCRIPT_AST_OPERAND *)PX_VECTORAT(PX_SCRIPT_AST_OPERAND,tk,tk->size-2));
	operand2=*((PX_SCRIPT_AST_OPERAND *)PX_VECTORAT(PX_SCRIPT_AST_OPERAND,tk,tk->size-1));

	PX_VectorPop(op);
	PX_VectorPop(tk);
	pTop=PX_VECTORLAST(PX_SCRIPT_AST_OPERAND,tk);
	pTop->operandType=PX_SCRIPT_AST_OPERAND_TYPE_INT_CONST;
	pTop->region=PX_SCRIPT_VARIABLE_REGION_POP;
	pTop->_oft=0;
	if (!PX_ScriptParseIsOperandNumericType(operand1)||!PX_ScriptParseIsOperandNumericType(operand2))
	{
		return PX_FALSE;
	}
	if(!PX_ScriptParseAST_MapTokenToR2(analysis,operand2,out)) return PX_FALSE;
	if(!PX_ScriptParseAST_MapTokenToR1(analysis,operand1,out)) return PX_FALSE;

	PX_StringCat(out,"SUB R1,R2\n");
	PX_StringCat(out,"LGLEZ R1\n");
	PX_StringCat(out,"PUSH R1\n");
	return PX_TRUE;
}
static px_bool PX_ScriptParseLastInstr_LGEQU(PX_SCRIPT_Analysis *analysis,px_vector *op,px_vector *tk,px_string *out)
{
	PX_SCRIPT_AST_OPERAND operand1,operand2,*pTop;

	if (tk->size<2)
	{
		return PX_FALSE;
	}
	operand1=*((PX_SCRIPT_AST_OPERAND *)PX_VECTORAT(PX_SCRIPT_AST_OPERAND,tk,tk->size-2));
	operand2=*((PX_SCRIPT_AST_OPERAND *)PX_VECTORAT(PX_SCRIPT_AST_OPERAND,tk,tk->size-1));

	PX_VectorPop(op);
	PX_VectorPop(tk);
	pTop=PX_VECTORLAST(PX_SCRIPT_AST_OPERAND,tk);
	pTop->operandType=PX_SCRIPT_AST_OPERAND_TYPE_INT_CONST;
	pTop->region=PX_SCRIPT_VARIABLE_REGION_POP;
	pTop->_oft=0;
	if (!PX_ScriptParseIsOperandNumericType(operand1)||!PX_ScriptParseIsOperandNumericType(operand2))
	{
		switch(operand1.operandType)
		{
		case PX_SCRIPT_AST_OPERAND_TYPE_MEMORY_CONST:
		case PX_SCRIPT_AST_OPERAND_TYPE_MEMORY:
			{
				switch(operand2.operandType)
				{
				case PX_SCRIPT_AST_OPERAND_TYPE_MEMORY:
				case PX_SCRIPT_AST_OPERAND_TYPE_MEMORY_CONST:
					break;
				default:
					PX_ScriptTranslatorError(&analysis->lexer,"Matched-type difference.");
					return PX_FALSE;
				}
			}
			break;
		case PX_SCRIPT_AST_OPERAND_TYPE_STRING_CONST:
		case PX_SCRIPT_AST_OPERAND_TYPE_STRING:
			{
				switch(operand2.operandType)
				{
				case PX_SCRIPT_AST_OPERAND_TYPE_STRING:
				case PX_SCRIPT_AST_OPERAND_TYPE_STRING_CONST:
					break;
				default:
					PX_ScriptTranslatorError(&analysis->lexer,"Matched-type difference.");
					return PX_FALSE;
				}
			}
			break;
		case PX_SCRIPT_AST_OPERAND_TYPE_STRUCT:
			PX_ScriptTranslatorError(&analysis->lexer,"Struct-type could not be compared.");
			return PX_FALSE;
		default:
			if (operand1.operandType!=operand2.operandType)
			{
				PX_ScriptTranslatorError(&analysis->lexer,"Matched-type difference.");
				return PX_FALSE;
			}
		}
	}
	if(!PX_ScriptParseAST_MapTokenToR2(analysis,operand2,out)) return PX_FALSE;
	if(!PX_ScriptParseAST_MapTokenToR1(analysis,operand1,out)) return PX_FALSE;
	PX_StringCat(out,"LGE R1,R1,R2\n");
	PX_StringCat(out,"PUSH R1\n");
	return PX_TRUE;
}
static px_bool PX_ScriptParseLastInstr_UNEQU(PX_SCRIPT_Analysis *analysis,px_vector *op,px_vector *tk,px_string *out)
{
	PX_SCRIPT_AST_OPERAND operand1,operand2,*pTop;

	if (tk->size<2)
	{
		return PX_FALSE;
	}
	operand1=*((PX_SCRIPT_AST_OPERAND *)PX_VECTORAT(PX_SCRIPT_AST_OPERAND,tk,tk->size-2));
	operand2=*((PX_SCRIPT_AST_OPERAND *)PX_VECTORAT(PX_SCRIPT_AST_OPERAND,tk,tk->size-1));

	PX_VectorPop(op);
	PX_VectorPop(tk);
	pTop=PX_VECTORLAST(PX_SCRIPT_AST_OPERAND,tk);
	pTop->operandType=PX_SCRIPT_AST_OPERAND_TYPE_INT_CONST;
	pTop->region=PX_SCRIPT_VARIABLE_REGION_POP;
	pTop->_oft=0;
	if (!PX_ScriptParseIsOperandNumericType(operand1)||!PX_ScriptParseIsOperandNumericType(operand2))
	{
		switch(operand1.operandType)
		{
		case PX_SCRIPT_AST_OPERAND_TYPE_MEMORY_CONST:
		case PX_SCRIPT_AST_OPERAND_TYPE_MEMORY:
			{
				switch(operand2.operandType)
				{
					case PX_SCRIPT_AST_OPERAND_TYPE_MEMORY:
					case PX_SCRIPT_AST_OPERAND_TYPE_MEMORY_CONST:
						break;
					default:
						PX_ScriptTranslatorError(&analysis->lexer,"Matched-type difference.");
						return PX_FALSE;
				}
			}
			break;
		case PX_SCRIPT_AST_OPERAND_TYPE_STRING_CONST:
		case PX_SCRIPT_AST_OPERAND_TYPE_STRING:
			{
				switch(operand2.operandType)
				{
				case PX_SCRIPT_AST_OPERAND_TYPE_STRING:
				case PX_SCRIPT_AST_OPERAND_TYPE_STRING_CONST:
					break;
				default:
					PX_ScriptTranslatorError(&analysis->lexer,"Matched-type difference.");
					return PX_FALSE;
				}
			}
			break;
		case PX_SCRIPT_AST_OPERAND_TYPE_STRUCT:
			PX_ScriptTranslatorError(&analysis->lexer,"Struct-type could not be compared.");
			return PX_FALSE;
		default:
			if (operand1.operandType!=operand2.operandType)
			{
				PX_ScriptTranslatorError(&analysis->lexer,"Matched-type difference.");
				return PX_FALSE;
			}
		}
	}
	if(!PX_ScriptParseAST_MapTokenToR2(analysis,operand2,out)) return PX_FALSE;
	if(!PX_ScriptParseAST_MapTokenToR1(analysis,operand1,out)) return PX_FALSE;
	PX_StringCat(out,"LGNE R1,R1,R2\n");
	PX_StringCat(out,"PUSH R1\n");
	return PX_TRUE;
}
static px_bool PX_ScriptParseLastInstr(PX_SCRIPT_Analysis *analysis,px_vector *op,px_vector *tk,px_string *out)
{
	
	PX_SCRIPT_AST_OPCODE opcode;
	opcode=*((PX_SCRIPT_AST_OPCODE *)PX_VECTORAT(PX_SCRIPT_AST_OPCODE,op,op->size-1));
	
	if (tk->size==1&&op->size==0)//(const)
	{
		return PX_TRUE;
	}

	switch(opcode.operandType)
	{
	case PX_SCRIPT_AST_OPCODE_TYPE_EQU:
		{
			if(!PX_ScriptParseLastInstr_EQUAL(analysis,op,tk,out)) 
			{
				PX_ScriptTranslatorError(&analysis->lexer,"'=' not match error.");
				return PX_FALSE;
			}
		}
		break;
	case PX_SCRIPT_AST_OPCODE_TYPE_DOT:
		{
			if(!PX_ScriptParseLastInstr_DOT(analysis,op,tk,out)){
				PX_ScriptTranslatorError(&analysis->lexer,"'.' not match error.");
				return PX_FALSE;
			}
		}
		break;
	case PX_SCRIPT_AST_OPCODE_TYPE_IDX:
		{
			if(!PX_ScriptParseLastInstr_IDX(analysis,op,tk,out)){
				PX_ScriptTranslatorError(&analysis->lexer,"'.' not match error.");
				return PX_FALSE;
			}
		}
		break;
	case PX_SCRIPT_AST_OPCODE_TYPE_OFT:
		{
			if(!PX_ScriptParseLastInstr_OFT(analysis,op,tk,out)) {
				PX_ScriptTranslatorError(&analysis->lexer,"'[]' not match error.");
				return PX_FALSE;
			}
		}
		break;
	case PX_SCRIPT_AST_OPCODE_TYPE_PTR:
		{
			if(!PX_ScriptParseLastInstr_PTR(analysis,op,tk,out)) {
				PX_ScriptTranslatorError(&analysis->lexer,"'->' not match error.");
				return PX_FALSE;
			}
		}
		break;
	case PX_SCRIPT_AST_OPCODE_TYPE_ADR:
		{
			if(!PX_ScriptParseLastInstr_ADR(analysis,op,tk,out)) {
				PX_ScriptTranslatorError(&analysis->lexer,"'&' not match error.");
				return PX_FALSE;
			}
		}
		break;
	case PX_SCRIPT_AST_OPCODE_TYPE_ADD:
		{
			if(!PX_ScriptParseLastInstr_ADD(analysis,op,tk,out)) {
				PX_ScriptTranslatorError(&analysis->lexer,"'+' not match error.");
				return PX_FALSE;
			}
		}
		break;
	case PX_SCRIPT_AST_OPCODE_TYPE_INC:
		{
			if(!PX_ScriptParseLastInstr_INC(analysis,op,tk,out)) {
				PX_ScriptTranslatorError(&analysis->lexer,"'++' not match error.");
				return PX_FALSE;
			}
		}
		break;
	case PX_SCRIPT_AST_OPCODE_TYPE_POSITIVE:
		{
			if(!PX_ScriptParseLastInstr_POSITIVE(analysis,op,tk,out)) {
				PX_ScriptTranslatorError(&analysis->lexer,"'+' not match error.");
				return PX_FALSE;
			}
		}
		break;
	case PX_SCRIPT_AST_OPCODE_TYPE_SUB:
		{
			if(!PX_ScriptParseLastInstr_SUB(analysis,op,tk,out)) {
				PX_ScriptTranslatorError(&analysis->lexer,"'-' not match error.");
				return PX_FALSE;
			}
		}
		break;
	case PX_SCRIPT_AST_OPCODE_TYPE_DEC:
		{
			if(!PX_ScriptParseLastInstr_DEC(analysis,op,tk,out)) {
				PX_ScriptTranslatorError(&analysis->lexer,"'--' not match error.");
				return PX_FALSE;
			}
		}
		break;
	case PX_SCRIPT_AST_OPCODE_TYPE_NEGATIVE:
		{
			if(!PX_ScriptParseLastInstr_NEGATIVE(analysis,op,tk,out)) {
				PX_ScriptTranslatorError(&analysis->lexer,"'-' not match error.");
				return PX_FALSE;
			}
		}
		break;
	case PX_SCRIPT_AST_OPCODE_TYPE_MUL:
		{
			if(!PX_ScriptParseLastInstr_MUL(analysis,op,tk,out)) {
				PX_ScriptTranslatorError(&analysis->lexer,"'*' not match error.");
				return PX_FALSE;
			}
		}
		break;
	case PX_SCRIPT_AST_OPCODE_TYPE_DIV:
		{
			if(!PX_ScriptParseLastInstr_DIV(analysis,op,tk,out)) {
				PX_ScriptTranslatorError(&analysis->lexer,"'/' not match error.");
				return PX_FALSE;
			}
		}
		break;
	case PX_SCRIPT_AST_OPCODE_TYPE_AND:
		{
			if(!PX_ScriptParseLastInstr_AND(analysis,op,tk,out)) {
				PX_ScriptTranslatorError(&analysis->lexer,"'&' not match error.");
				return PX_FALSE;
			}
		}
		break;
	case PX_SCRIPT_AST_OPCODE_TYPE_OR:
		{
			if(!PX_ScriptParseLastInstr_OR(analysis,op,tk,out)) {
				PX_ScriptTranslatorError(&analysis->lexer,"'|' not match error.");
				return PX_FALSE;
			}
		}
		break;
	case PX_SCRIPT_AST_OPCODE_TYPE_NOT:
		{
			if(!PX_ScriptParseLastInstr_NOT(analysis,op,tk,out)) {
				PX_ScriptTranslatorError(&analysis->lexer,"'!' not match error.");
				return PX_FALSE;
			}
		}
		break;
	case PX_SCRIPT_AST_OPCODE_TYPE_XOR:
		{
			if(!PX_ScriptParseLastInstr_XOR(analysis,op,tk,out)) {
				PX_ScriptTranslatorError(&analysis->lexer,"'^' not match error.");
				return PX_FALSE;
			}
		}
		break;
	case PX_SCRIPT_AST_OPCODE_TYPE_INV:
		{
			if(!PX_ScriptParseLastInstr_INV(analysis,op,tk,out)) {
				PX_ScriptTranslatorError(&analysis->lexer,"'~' not match error.");
				return PX_FALSE;
			}
		}
		break;
	case PX_SCRIPT_AST_OPCODE_TYPE_MOD:
		{
			if(!PX_ScriptParseLastInstr_MOD(analysis,op,tk,out)) {
				PX_ScriptTranslatorError(&analysis->lexer,"'%' not match error.");
				return PX_FALSE;
			}
		}
		break;
	case PX_SCRIPT_AST_OPCODE_TYPE_SHL:
		{
			if(!PX_ScriptParseLastInstr_SHL(analysis,op,tk,out)) {
				PX_ScriptTranslatorError(&analysis->lexer,"'<<' not match error.");
				return PX_FALSE;
			}
		}
		break;
	case PX_SCRIPT_AST_OPCODE_TYPE_SHR:
		{
			if(!PX_ScriptParseLastInstr_SHR(analysis,op,tk,out)) {
				PX_ScriptTranslatorError(&analysis->lexer,"'>>' not match error.");
				return PX_FALSE;
			}
		}
		break;
	case PX_SCRIPT_AST_OPCODE_TYPE_LARGE:
		{
			if(!PX_ScriptParseLastInstr_LARGE(analysis,op,tk,out)) {
				PX_ScriptTranslatorError(&analysis->lexer,"'>' not match error.");
				return PX_FALSE;
			}
		}
		break;
	case PX_SCRIPT_AST_OPCODE_TYPE_LARGEEQU:
		{
			if(!PX_ScriptParseLastInstr_LARGEEQU(analysis,op,tk,out)) {
				PX_ScriptTranslatorError(&analysis->lexer,"'>=' not match error.");
				return PX_FALSE;
			}
		}
		break;
	case PX_SCRIPT_AST_OPCODE_TYPE_LESS:
		{
			if(!PX_ScriptParseLastInstr_LESS(analysis,op,tk,out)) {
				PX_ScriptTranslatorError(&analysis->lexer,"'<' not match error.");
				return PX_FALSE;
			}
		}
		break;
	case PX_SCRIPT_AST_OPCODE_TYPE_LESSEQU:
		{
			if(!PX_ScriptParseLastInstr_LESSEQU(analysis,op,tk,out)) {
				PX_ScriptTranslatorError(&analysis->lexer,"'<=' not match error.");
				return PX_FALSE;
			}
		}
		break;
	case PX_SCRIPT_AST_OPCODE_TYPE_LGEQU:
		{
			if(!PX_ScriptParseLastInstr_LGEQU(analysis,op,tk,out)) {
				PX_ScriptTranslatorError(&analysis->lexer,"'>=' not match error.");
				return PX_FALSE;
			}
		}
		break;
	case PX_SCRIPT_AST_OPCODE_TYPE_UNEQU:
		{
			if(!PX_ScriptParseLastInstr_UNEQU(analysis,op,tk,out)) {
				PX_ScriptTranslatorError(&analysis->lexer,"'!=' not match error.");
				return PX_FALSE;
			}
		}
		break;
	case PX_SCRIPT_AST_OPCODE_TYPE_LAND:
		{
			if(!PX_ScriptParseLastInstr_LAND(analysis,op,tk,out)) {
				PX_ScriptTranslatorError(&analysis->lexer,"'&&' not match error.");
				return PX_FALSE;
			}
		}
		break;
	case PX_SCRIPT_AST_OPCODE_TYPE_LOR:
		{
			if(!PX_ScriptParseLastInstr_LOR(analysis,op,tk,out)) {
				PX_ScriptTranslatorError(&analysis->lexer,"'||' not match error.");
				return PX_FALSE;
			}
		}
		break;
	case PX_SCRIPT_AST_OPCODE_TYPE_INT:
		{
			if(!PX_ScriptParseLastInstr_INT(analysis,op,tk,out)) {
				PX_ScriptTranslatorError(&analysis->lexer,"'int' not match error.");
				return PX_FALSE;
			}
		}
		break;
	case PX_SCRIPT_AST_OPCODE_TYPE_FLOAT:
		{
			if(!PX_ScriptParseLastInstr_FLOAT(analysis,op,tk,out)) {
				PX_ScriptTranslatorError(&analysis->lexer,"'float' not match error.");
				return PX_FALSE;
			}
		}
		break;
	case PX_SCRIPT_AST_OPCODE_TYPE_STRING:
		{
			if(!PX_ScriptParseLastInstr_STRING(analysis,op,tk,out)) {
				PX_ScriptTranslatorError(&analysis->lexer,"'string' not match error.");
				return PX_FALSE;
			}
		}
		break;
	case PX_SCRIPT_AST_OPCODE_TYPE_MEMORY:
		{
			if(!PX_ScriptParseLastInstr_MEMORY(analysis,op,tk,out)) {
				PX_ScriptTranslatorError(&analysis->lexer,"'memory' not match error.");
				return PX_FALSE;
			}
		}
		break;
	case PX_SCRIPT_AST_OPCODE_TYPE_STRLEN:
		{
			if(!PX_ScriptParseLastInstr_STRLEN(analysis,op,tk,out)) {
				PX_ScriptTranslatorError(&analysis->lexer,"'strlen' not match error.");
				return PX_FALSE;
			}
		}
		break;
	case PX_SCRIPT_AST_OPCODE_TYPE_MEMLEN:
		{
			if(!PX_ScriptParseLastInstr_MEMLEN(analysis,op,tk,out)) {
				PX_ScriptTranslatorError(&analysis->lexer,"'memlen' not match error.");
				return PX_FALSE;
			}
		}
		break;
	default:
		return PX_FALSE;
	}
	return PX_TRUE;
}
static px_bool PX_ScriptParseExpressionStream(PX_SCRIPT_Analysis *analysis,px_vector *stream,px_int *offset,PX_SCRIPT_AST_OPERAND *pretOperand,PX_SCRIPT_TRANSLATOR_EXPRESSION_TYPE expectedEndl,px_string *out)
{
	px_vector vOp;
	px_vector vTk;
	px_vector *op=&vOp;
	px_vector *tk=&vTk;
	PX_SCRIPT_AST_OPERAND retOperand;
	PX_SCRIPT_AST_OPCODE *plastop=PX_NULL;
	PX_SCRIPT_STRUCT *pcurrentSet=PX_NULL;
	px_int level=PX_SCRIPT_OPCODE_LEVEL_BOTTOM;
	PX_SCRIPT_EXPR_STREAM *pVec=(PX_SCRIPT_EXPR_STREAM *)(stream->data);
	PX_SCRIPT_FUNCTION *pfunc=PX_NULL;
	px_int i,LBracketBalance,MBracketBalance,paramBeginIndex[16],paramcount=0;
	px_string fmrString;
	PX_VectorInitialize(analysis->mp,op,sizeof(PX_SCRIPT_AST_OPCODE),16);
	PX_VectorInitialize(analysis->mp,tk,sizeof(PX_SCRIPT_AST_OPERAND),16);

	

	while(*offset<stream->size)
	{
		if (pVec[*offset].type==expectedEndl)
		{
			while(op->size)
			{
				if(!PX_ScriptParseLastInstr(analysis,op,tk,out))
				{
					goto _ERROR;
				}
			}
			*pretOperand=*(PX_VECTORAT(PX_SCRIPT_AST_OPERAND,tk,0));
			
			break;
		}
		switch(pVec[*offset].type)
		{
		case PX_SCRIPT_TRANSLATOR_EXPRESSION_TOKEN:
		{
			if(!PX_ScriptParse_AST_PushToken(analysis,op,tk,pVec[*offset],pcurrentSet,&pcurrentSet)) 
			{
				PX_ScriptTranslatorError(&analysis->lexer,"Function error.");
				goto _ERROR;
			}
		}
		break;
		case PX_SCRIPT_TRANSLATOR_EXPRESSION_OP_FUNCTION:
		{
			//call func
			for (i=0;i<analysis->v_functions.size;i++)
			{
				pfunc=PX_VECTORAT(PX_SCRIPT_FUNCTION,&analysis->v_functions,i);
				if (PX_strequ(pfunc->name,pVec[*offset].code.buffer))
				{
					break;
				}
			}
			if (i==analysis->v_functions.size)
			{
				PX_ScriptTranslatorError(&analysis->lexer,"Function name undefined.");
				goto _ERROR;
			}

			//(
			(*offset)++;
			if (pVec[*offset].type!=PX_SCRIPT_TRANSLATOR_EXPRESSION_OP_LBRACKETBEGIN)
			{
				PX_ScriptTranslatorError(&analysis->lexer,"'(' expected but not found.");
				goto _ERROR;
			}
			(*offset)++;
			LBracketBalance=1;
			paramcount=0;
			paramBeginIndex[paramcount]=*offset;

			if(pVec[*offset].type==PX_SCRIPT_TRANSLATOR_EXPRESSION_OP_LBRACKETEND)
			{
				paramBeginIndex[paramcount]++;
			}
			else
			while (PX_TRUE)
			{
				if (!LBracketBalance&&pVec[*offset].type==PX_SCRIPT_TRANSLATOR_EXPRESSION_OP_LBRACKETEND)
				{
					break;
				}
				MBracketBalance=0;
				while (PX_TRUE)
				{
					if (*offset==stream->size)
					{
						PX_ScriptTranslatorError(&analysis->lexer,"Function parse error.");
						goto _ERROR;
					}
					switch(pVec[*offset].type)
					{
					case PX_SCRIPT_TRANSLATOR_EXPRESSION_OP_LBRACKETBEGIN:
						LBracketBalance++;
						break;
					case PX_SCRIPT_TRANSLATOR_EXPRESSION_OP_LBRACKETEND:
						LBracketBalance--;
						if (LBracketBalance==0)
						{
							goto _EXPR_OUT;
						}
						break;
					case PX_SCRIPT_TRANSLATOR_EXPRESSION_OP_MBRACKETBEGIN:
						MBracketBalance++;
						break;
					case PX_SCRIPT_TRANSLATOR_EXPRESSION_OP_MBRACKETEND:
						MBracketBalance--;
						break;
					case PX_SCRIPT_TRANSLATOR_EXPRESSION_OP_COMMA:
						if (LBracketBalance==1&&MBracketBalance==0)
						{
							goto _EXPR_OUT;
						}
						break;
					default:
							break;
					}
					(*offset)++;
				}
_EXPR_OUT:
				if (pVec[*offset].type==PX_SCRIPT_TRANSLATOR_EXPRESSION_OP_LBRACKETEND)
				{
					(*offset)++;
					paramcount++;
					paramBeginIndex[paramcount]=*offset;
					break;
				}
				else
				{
					(*offset)++;
					paramcount++;
					paramBeginIndex[paramcount]=*offset;
				}
				
			}
			if (paramcount!=pfunc->parametersCount)
			{
				PX_ScriptTranslatorError(&analysis->lexer,"Function parameter-count error.");
				goto _ERROR;
			}


			for (i=paramcount-1;i>=0;i--)
			//for (i=0;i<paramcount;i++)
			{
				if (i==paramcount-1)
				{
					if (!PX_ScriptParseExpressionStream(analysis,stream,&paramBeginIndex[paramcount-1],&retOperand,PX_SCRIPT_TRANSLATOR_EXPRESSION_OP_LBRACKETEND,out))
					{
						PX_ScriptTranslatorError(&analysis->lexer,"Function left-bracket-end missing.");
						goto _ERROR;
					}
				}
				else
				{
					if (!PX_ScriptParseExpressionStream(analysis,stream,&paramBeginIndex[i],&retOperand,PX_SCRIPT_TRANSLATOR_EXPRESSION_OP_COMMA,out))
					{
						PX_ScriptTranslatorError(&analysis->lexer,"Function comma missing.");
						goto _ERROR;
					}
				}
				
				if (retOperand.operandType==PX_SCRIPT_AST_OPERAND_TYPE_STRUCT||retOperand.operandType==PX_SCRIPT_AST_OPERAND_TYPE_STRUCT_PTR||retOperand.operandType==PX_SCRIPT_AST_OPERAND_TYPE_SET_PTR_CONST)
				{
					pcurrentSet=retOperand.pSet;
				}

				switch (retOperand.operandType)
				{
				case PX_SCRIPT_AST_OPERAND_TYPE_INT:
				case PX_SCRIPT_AST_OPERAND_TYPE_INT_CONST:
				case PX_SCRIPT_AST_OPERAND_TYPE_STRING_IDX:
				case PX_SCRIPT_AST_OPERAND_TYPE_MEMORY_IDX:
// 					if (pfunc->parameters[i].type!=PX_SCRIPT_PARSER_VAR_TYPE_INT)
// 					{
// 						PX_ScriptTranslatorError(&analysis->lexer,"Parameter not matched.");
// 						goto _ERROR;
// 					}
// 					break;
				case PX_SCRIPT_AST_OPERAND_TYPE_FLOAT:
				case PX_SCRIPT_AST_OPERAND_TYPE_FLOAT_CONST:
					if (pfunc->parameters[i].type!=PX_SCRIPT_PARSER_VAR_TYPE_FLOAT&&pfunc->parameters[i].type!=PX_SCRIPT_PARSER_VAR_TYPE_INT)
					{
						PX_ScriptTranslatorError(&analysis->lexer,"Parameter not matched.");
						goto _ERROR;
					}
					break;
				case PX_SCRIPT_AST_OPERAND_TYPE_STRING:
				case PX_SCRIPT_AST_OPERAND_TYPE_STRING_CONST:
					if (pfunc->parameters[i].type!=PX_SCRIPT_PARSER_VAR_TYPE_STRING)
					{
						PX_ScriptTranslatorError(&analysis->lexer,"Parameter not matched.");
						goto _ERROR;
					}
					break;
				case PX_SCRIPT_AST_OPERAND_TYPE_MEMORY:
				case PX_SCRIPT_AST_OPERAND_TYPE_MEMORY_CONST:
					if (pfunc->parameters[i].type!=PX_SCRIPT_PARSER_VAR_TYPE_MEMORY)
					{
						PX_ScriptTranslatorError(&analysis->lexer,"Parameter not matched.");
						goto _ERROR;
					}
					break;
				case PX_SCRIPT_AST_OPERAND_TYPE_INT_PTR:
				case PX_SCRIPT_AST_OPERAND_TYPE_INT_PTR_CONST:
					if (pfunc->parameters[i].type!=PX_SCRIPT_PARSER_VAR_TYPE_INT_ARRAY&&pfunc->parameters[i].type!=PX_SCRIPT_PARSER_VAR_TYPE_INT_PTR)
					{
						PX_ScriptTranslatorError(&analysis->lexer,"Parameter not matched.");
						goto _ERROR;
					}
					break;
				case PX_SCRIPT_AST_OPERAND_TYPE_FLOAT_PTR:
				case PX_SCRIPT_AST_OPERAND_TYPE_FLOAT_PTR_CONST:
					if (pfunc->parameters[i].type!=PX_SCRIPT_PARSER_VAR_TYPE_FLOAT_ARRAY&&pfunc->parameters[i].type!=PX_SCRIPT_PARSER_VAR_TYPE_FLOAT_PTR)
					{
						PX_ScriptTranslatorError(&analysis->lexer,"Parameter not matched.");
						goto _ERROR;
					}
					break;
				case PX_SCRIPT_AST_OPERAND_TYPE_STRING_PTR:
				case PX_SCRIPT_AST_OPERAND_TYPE_STRING_PTR_CONST:
					if (pfunc->parameters[i].type!=PX_SCRIPT_PARSER_VAR_TYPE_STRING_ARRAY&&pfunc->parameters[i].type!=PX_SCRIPT_PARSER_VAR_TYPE_STRING_PTR)
					{
						PX_ScriptTranslatorError(&analysis->lexer,"Parameter not matched.");
						goto _ERROR;
					}
					break;
				case PX_SCRIPT_AST_OPERAND_TYPE_MEMORY_PTR:
				case PX_SCRIPT_AST_OPERAND_TYPE_MEMORY_PTR_CONST:
					if (pfunc->parameters[i].type!=PX_SCRIPT_PARSER_VAR_TYPE_MEMORY_ARRAY&&pfunc->parameters[i].type!=PX_SCRIPT_PARSER_VAR_TYPE_MEMORY_PTR)
					{
						PX_ScriptTranslatorError(&analysis->lexer,"Parameter not matched.");
						goto _ERROR;
					}
					break;
				case PX_SCRIPT_AST_OPERAND_TYPE_STRUCT_PTR:
				case PX_SCRIPT_AST_OPERAND_TYPE_SET_PTR_CONST:
					if (pfunc->parameters[i].type!=PX_SCRIPT_PARSER_VAR_TYPE_STRUCT_PTR||PX_ScriptParseGetStructByIndex(analysis,pfunc->parameters[i].setIndex)!=retOperand.pSet)
					{
						PX_ScriptTranslatorError(&analysis->lexer,"Parameter not matched.");
						goto _ERROR;
					}
					break;
				default:
					PX_ScriptTranslatorError(&analysis->lexer,"unexpected error.");
					goto _ERROR;
				}
				if (!PX_ScriptParseAST_MapTokenToR1(analysis,retOperand,out))
				{
					goto _ERROR;
				}

				if (retOperand.operandType==PX_SCRIPT_AST_OPERAND_TYPE_FLOAT||retOperand.operandType==PX_SCRIPT_AST_OPERAND_TYPE_FLOAT_CONST)
				{
					if (pfunc->parameters[i].type==PX_SCRIPT_PARSER_VAR_TYPE_INT)
					{
						PX_StringCat(out,"INT R1\n");
					}
				}

				if (retOperand.operandType==PX_SCRIPT_AST_OPERAND_TYPE_INT||retOperand.operandType==PX_SCRIPT_AST_OPERAND_TYPE_INT_CONST\
					||retOperand.operandType==PX_SCRIPT_AST_OPERAND_TYPE_STRING_IDX||retOperand.operandType==PX_SCRIPT_AST_OPERAND_TYPE_MEMORY_IDX)
				{
					if (pfunc->parameters[i].type==PX_SCRIPT_PARSER_VAR_TYPE_FLOAT)
					{
						PX_StringCat(out,"FLT R1\n");
					}
				}

				PX_StringCat(out,"PUSH R1\n");

			}

			//call
			switch(pfunc->type)
			{
			case PX_SCRIPT_TRANSLATOR_FUNCTION_TYPE_EXPORT:
			case PX_SCRIPT_TRANSLATOR_FUNCTION_TYPE_CUSTOM:
				PX_StringInitialize(analysis->mp,&fmrString);
				PX_StringFormat1(&fmrString,"CALL %1\n",PX_STRINGFORMAT_STRING(pfunc->name));
				PX_StringCat(out,fmrString.buffer);

				if(pfunc->parametersCount)
				{
					PX_StringFormat1(&fmrString,"POPN %1\n",PX_STRINGFORMAT_INT(pfunc->parametersSize));
					PX_StringCat(out,fmrString.buffer);
				}
				

				PX_StringFree(&fmrString);
				break;
			case PX_SCRIPT_TRANSLATOR_FUNCTION_TYPE_HOST:
				PX_StringInitialize(analysis->mp,&fmrString);
				PX_StringFormat1(&fmrString,"CALL $%1\n",PX_STRINGFORMAT_STRING(pfunc->name));
				PX_StringCat(out,fmrString.buffer);
				if(pfunc->parametersCount)
				{
					PX_StringFormat1(&fmrString,"POPN %1\n",PX_STRINGFORMAT_INT(pfunc->parametersSize));
					PX_StringCat(out,fmrString.buffer);
				}

				PX_StringFree(&fmrString);
				break;
			}
			
			retOperand.region=PX_SCRIPT_VARIABLE_REGION_POP;
			retOperand._oft=0;

			switch (pfunc->retType)
			{
			case PX_SCRIPT_PARSER_VAR_TYPE_INT:
				retOperand.operandType=PX_SCRIPT_AST_OPERAND_TYPE_INT_CONST;
				break;
			case PX_SCRIPT_PARSER_VAR_TYPE_FLOAT:
				retOperand.operandType=PX_SCRIPT_AST_OPERAND_TYPE_FLOAT_CONST;
				break;
			case PX_SCRIPT_PARSER_VAR_TYPE_STRING:
				retOperand.operandType=PX_SCRIPT_AST_OPERAND_TYPE_STRING_CONST;
				break;
			case PX_SCRIPT_PARSER_VAR_TYPE_MEMORY:
				retOperand.operandType=PX_SCRIPT_AST_OPERAND_TYPE_MEMORY_CONST;
				break;
			case PX_SCRIPT_PARSER_VAR_TYPE_INT_PTR:
				retOperand.operandType=PX_SCRIPT_AST_OPERAND_TYPE_INT_PTR_CONST;
				break;
			case PX_SCRIPT_PARSER_VAR_TYPE_FLOAT_PTR:
				retOperand.operandType=PX_SCRIPT_AST_OPERAND_TYPE_FLOAT_PTR_CONST;
				break;
			case PX_SCRIPT_PARSER_VAR_TYPE_STRING_PTR:
				retOperand.operandType=PX_SCRIPT_AST_OPERAND_TYPE_STRING_PTR_CONST;
				break;
			case PX_SCRIPT_PARSER_VAR_TYPE_MEMORY_PTR:
				retOperand.operandType=PX_SCRIPT_AST_OPERAND_TYPE_MEMORY_PTR_CONST;
				break;
			case PX_SCRIPT_PARSER_VAR_TYPE_STRUCT_PTR:
				retOperand.operandType=PX_SCRIPT_AST_OPERAND_TYPE_SET_PTR_CONST;
				retOperand.pSet=PX_ScriptParseGetStructByIndex(analysis,pfunc->retSetIndex);
				break;
			case PX_SCRIPT_PARSER_VAR_TYPE_VOID:
				retOperand.operandType=PX_SCRIPT_AST_OPERAND_TYPE_INT_CONST;
				retOperand._int=0;
				break;
			default:
				goto _ERROR;
			}
			


			PX_StringCat(out,"PUSH R1\n");
			PX_VectorPushback(tk,&retOperand);

			

			(*offset)=paramBeginIndex[paramcount]-1;


		}
		break;
		case PX_SCRIPT_TRANSLATOR_EXPRESSION_OP_LBRACKETBEGIN:
			{
				(*offset)++;

				if(!PX_ScriptParseExpressionStream(analysis,stream,offset,&retOperand,PX_SCRIPT_TRANSLATOR_EXPRESSION_OP_LBRACKETEND,out))
				goto _ERROR;

				PX_VectorPushback(tk,&retOperand);
				if (retOperand.operandType==PX_SCRIPT_AST_OPERAND_TYPE_STRUCT||retOperand.operandType==PX_SCRIPT_AST_OPERAND_TYPE_STRUCT_PTR||retOperand.operandType==PX_SCRIPT_AST_OPERAND_TYPE_SET_PTR_CONST)
				{
					pcurrentSet=retOperand.pSet;
				}
			}
			break;
		case PX_SCRIPT_TRANSLATOR_EXPRESSION_OP_MBRACKETBEGIN:
		{

			if (level>pVec[*offset].oplevel)
			{
				level=pVec[*offset].oplevel;
				if(!PX_ScriptParseAST_PushOpcode(analysis,op,pVec[*offset])) goto _ERROR;
			}
			else
			{
				level=pVec[*offset].oplevel;
				do{
					if(op->size>0)
					{
						if(!PX_ScriptParseLastInstr(analysis,op,tk,out)) goto _ERROR;
						if(op->size)
							plastop=PX_VECTORAT(PX_SCRIPT_AST_OPCODE,op,op->size-1);
					}
					else
						break;
				}while(op->size&&plastop->level<=level);
				if(!PX_ScriptParseAST_PushOpcode(analysis,op,pVec[*offset])) goto _ERROR;
			}

			(*offset)++;
			if(!PX_ScriptParseExpressionStream(analysis,stream,offset,&retOperand,PX_SCRIPT_TRANSLATOR_EXPRESSION_OP_MBRACKETEND,out))
				goto _ERROR;

			PX_VectorPushback(tk,&retOperand);

			if (retOperand.operandType==PX_SCRIPT_AST_OPERAND_TYPE_STRUCT||retOperand.operandType==PX_SCRIPT_AST_OPERAND_TYPE_STRUCT_PTR||retOperand.operandType==PX_SCRIPT_AST_OPERAND_TYPE_SET_PTR_CONST)
			{
				pcurrentSet=retOperand.pSet;
			}
		}
		break;
		case PX_SCRIPT_TRANSLATOR_EXPRESSION_OP_COMMA:
			{
				while(op->size)
				{
					if(!PX_ScriptParseLastInstr(analysis,op,tk,out))
						goto _ERROR;
				}
				*pretOperand=*(PX_VECTORAT(PX_SCRIPT_AST_OPERAND,tk,0));
				if(!PX_ScriptParseAST_MapTokenToR1(analysis,*pretOperand,out)) goto _ERROR;

				if (tk->size!=1)
				{
					goto _ERROR;
				}

				level=PX_SCRIPT_OPCODE_LEVEL_BOTTOM;
				PX_VectorPop(tk);
				break;
			}
			break;
		case PX_SCRIPT_TRANSLATOR_EXPRESSION_OP_ADD:
		case PX_SCRIPT_TRANSLATOR_EXPRESSION_OP_INC:
		case PX_SCRIPT_TRANSLATOR_EXPRESSION_OP_POSITIVE:
		case PX_SCRIPT_TRANSLATOR_EXPRESSION_OP_SUB:
		case PX_SCRIPT_TRANSLATOR_EXPRESSION_OP_DEC:
		case PX_SCRIPT_TRANSLATOR_EXPRESSION_OP_NEGATIVE:
		case PX_SCRIPT_TRANSLATOR_EXPRESSION_OP_MUL:
		case PX_SCRIPT_TRANSLATOR_EXPRESSION_OP_DIV:
		case PX_SCRIPT_TRANSLATOR_EXPRESSION_OP_AND:
		case PX_SCRIPT_TRANSLATOR_EXPRESSION_OP_OR:	
		case PX_SCRIPT_TRANSLATOR_EXPRESSION_OP_NOT:			
		case PX_SCRIPT_TRANSLATOR_EXPRESSION_OP_XOR:		
		case PX_SCRIPT_TRANSLATOR_EXPRESSION_OP_INV:
		case PX_SCRIPT_TRANSLATOR_EXPRESSION_OP_MOD:
		case PX_SCRIPT_TRANSLATOR_EXPRESSION_OP_SHL:
		case PX_SCRIPT_TRANSLATOR_EXPRESSION_OP_SHR:
		case PX_SCRIPT_TRANSLATOR_EXPRESSION_OP_DOT:
		case PX_SCRIPT_TRANSLATOR_EXPRESSION_OP_LARGE:
		case PX_SCRIPT_TRANSLATOR_EXPRESSION_OP_LARGEEUQ:
		case PX_SCRIPT_TRANSLATOR_EXPRESSION_OP_LESS:
		case PX_SCRIPT_TRANSLATOR_EXPRESSION_OP_LESSEQU:
		case PX_SCRIPT_TRANSLATOR_EXPRESSION_OP_UNEQU:
		case PX_SCRIPT_TRANSLATOR_EXPRESSION_OP_EQU:
		case PX_SCRIPT_TRANSLATOR_EXPRESSION_OP_LGEQU:
		case PX_SCRIPT_TRANSLATOR_EXPRESSION_OP_LAND:
		case PX_SCRIPT_TRANSLATOR_EXPRESSION_OP_LOR:
		case PX_SCRIPT_TRANSLATOR_EXPRESSION_OP_ADR:
		case PX_SCRIPT_TRANSLATOR_EXPRESSION_OP_OFT:
		case PX_SCRIPT_TRANSLATOR_EXPRESSION_OP_PTR:
		case PX_SCRIPT_TRANSLATOR_EXPRESSION_OP_INT:
		case PX_SCRIPT_TRANSLATOR_EXPRESSION_OP_FLOAT:
		case PX_SCRIPT_TRANSLATOR_EXPRESSION_OP_STRING:
		case PX_SCRIPT_TRANSLATOR_EXPRESSION_OP_MEMORY:
		case PX_SCRIPT_TRANSLATOR_EXPRESSION_OP_MEMLEN:
		case PX_SCRIPT_TRANSLATOR_EXPRESSION_OP_STRLEN:
			{
				if (pVec[*offset].type==PX_SCRIPT_TRANSLATOR_EXPRESSION_OP_DOT||pVec[*offset].type==PX_SCRIPT_TRANSLATOR_EXPRESSION_OP_OFT)
				{
					if (pcurrentSet==PX_NULL)
					{
						goto _ERROR;
					}
				}
				else
				{
					pcurrentSet=PX_NULL;
				}
				if (level>pVec[*offset].oplevel)
				{
					level=pVec[*offset].oplevel;
					if(!PX_ScriptParseAST_PushOpcode(analysis,op,pVec[*offset]))  goto _ERROR;;
				}
				else
				{
					level=pVec[*offset].oplevel;
					do{
						if(op->size>0)
						{
							if(!PX_ScriptParseLastInstr(analysis,op,tk,out)) goto _ERROR;;
							if(op->size)
							plastop=PX_VECTORAT(PX_SCRIPT_AST_OPCODE,op,op->size-1);
						}
						else
						break;
					}while(op->size&&plastop->level<=level);
					if(!PX_ScriptParseAST_PushOpcode(analysis,op,pVec[*offset])) goto _ERROR;;
				}
			}
			break;
		default:
		{
			goto _ERROR;
		}
		}
		(*offset)++;
	}

	PX_VectorFree(op);
	PX_VectorFree(tk);
	return PX_TRUE;
_ERROR:
	PX_VectorFree(op);
	PX_VectorFree(tk);
	return PX_FALSE;
}
#define PX_SCRIPT_EXPRESSION_ACCEPT_SINGLE 1
#define PX_SCRIPT_EXPRESSION_ACCEPT_BINARY 2
#define PX_SCRIPT_EXPRESSION_ACCEPT_TOKEN  4
#define PX_SCRIPT_EXPRESSION_ACCEPT_LBRACKET_START 8
#define PX_SCRIPT_EXPRESSION_ACCEPT_LBRACKET_END 16
#define PX_SCRIPT_EXPRESSION_ACCEPT_MBRACKET_START 32
#define PX_SCRIPT_EXPRESSION_ACCEPT_MBRACKET_END 16
#define PX_SCRIPT_EXPRESSION_ACCEPT_BRACKET_START (PX_SCRIPT_EXPRESSION_ACCEPT_LBRACKET_START|PX_SCRIPT_EXPRESSION_ACCEPT_MBRACKET_START)
#define PX_SCRIPT_EXPRESSION_ACCEPT_BRACKET_END (PX_SCRIPT_EXPRESSION_ACCEPT_LBRACKET_END|PX_SCRIPT_EXPRESSION_ACCEPT_MBRACKET_END)
static px_bool PX_ScriptParseExpression(PX_SCRIPT_Analysis *analysis,px_char *expr,px_string *out,PX_SCRIPT_AST_OPERAND *retOperand)
{
	px_int i;
	px_uint quotes;
	px_vector stream,opStack,tkStack;
	px_lexer lexer;
	PX_LEXER_STATE state;
	PX_LEXER_LEXEME_TYPE type;
	px_int streamOft=1; 
	px_int accept_type;
	PX_SCRIPT_AST_OPERAND operand;

	px_bool ret=PX_TRUE;

	PX_SCRIPT_EXPR_STREAM Op;

	if (PX_strlen(expr)==0)
	{
		retOperand->operandType=PX_SCRIPT_AST_OPERAND_TYPE_VOID;
		retOperand->region=PX_SCRIPT_VARIABLE_REGION_GLOBAL;
		return PX_TRUE;
	}

	PX_StringClear(out);


	PX_LexerInitialize(&lexer,analysis->mp);
	PX_LexerRegisterDelimiter(&lexer,',');
	PX_LexerRegisterDelimiter(&lexer,';');
	PX_LexerRegisterDelimiter(&lexer,'+');
	PX_LexerRegisterDelimiter(&lexer,'-');
	PX_LexerRegisterDelimiter(&lexer,'*');
	PX_LexerRegisterDelimiter(&lexer,'/');
	PX_LexerRegisterDelimiter(&lexer,'%');
	PX_LexerRegisterDelimiter(&lexer,'&');
	PX_LexerRegisterDelimiter(&lexer,'|');
	PX_LexerRegisterDelimiter(&lexer,'^');
	PX_LexerRegisterDelimiter(&lexer,'~');
	PX_LexerRegisterDelimiter(&lexer,'(');
	PX_LexerRegisterDelimiter(&lexer,')');
	PX_LexerRegisterDelimiter(&lexer,'!');
	PX_LexerRegisterDelimiter(&lexer,'=');
	PX_LexerRegisterDelimiter(&lexer,'>');
	PX_LexerRegisterDelimiter(&lexer,'<');
	PX_LexerRegisterDelimiter(&lexer,'{');
	PX_LexerRegisterDelimiter(&lexer,'}');
	PX_LexerRegisterDelimiter(&lexer,'[');
	PX_LexerRegisterDelimiter(&lexer,']');
	PX_LexerRegisterDelimiter(&lexer,'.');
	PX_LexerRegisterSpacer(&lexer,' ');
	PX_LexerRegisterSpacer(&lexer,'\t');
	quotes=PX_LexerRegisterContainer(&lexer,"\"","\"");
	PX_LexerRegisterContainerTransfer(&lexer,quotes,'\\');
	PX_LexerRegisterContainer(&lexer,"\'","\'");
	PX_LexerRegisterContainer(&lexer,"@","@");
	PX_LexerSetTokenCase(&lexer,PX_LEXER_LEXEME_CASE_UPPER);

	PX_LexerSetNumericMatch(&lexer,PX_TRUE);


	if(!PX_LexerLoadSourceFromMemory(&lexer,expr))
		return PX_FALSE;

	PX_VectorInitialize(analysis->mp,&stream,sizeof(PX_SCRIPT_EXPR_STREAM),32);
	PX_VectorInitialize(analysis->mp,&opStack,sizeof(PX_SCRIPT_AST_OPCODE),32);
	PX_VectorInitialize(analysis->mp,&tkStack,sizeof(PX_SCRIPT_AST_OPERAND),32);

	//
	accept_type=PX_SCRIPT_EXPRESSION_ACCEPT_SINGLE|PX_SCRIPT_EXPRESSION_ACCEPT_TOKEN|PX_SCRIPT_EXPRESSION_ACCEPT_BRACKET_START;

	Op.type=PX_SCRIPT_TRANSLATOR_EXPRESSION_OP_LBRACKETBEGIN;
	Op.opclass=PX_SCRIPT_TRANSLATOR_OP_CLASS_CONTAINER;
	Op.oplevel=0;
	PX_StringInitialize(analysis->mp,&Op.code);
	PX_StringCat(&Op.code,lexer.CurLexeme.buffer);
	PX_VectorPushback(&stream,&Op);

	while (PX_TRUE)
	{
		type=PX_ScriptTranslatorNextToken(&lexer);
		if (type==PX_LEXER_LEXEME_TYPE_END)
		{
			break;
		}

		if (type==PX_LEXER_LEXEME_TYPE_TOKEN)
		{
			//////////////////////////////////////////////////////////////////////////
			//key word
			if (PX_strequ(lexer.CurLexeme.buffer,"STRLEN"))
			{
				if ((accept_type&PX_SCRIPT_EXPRESSION_ACCEPT_SINGLE)==0)
				{
					ret=PX_FALSE;
					goto _CLEAR;
				}
				else
				{
					accept_type=PX_SCRIPT_EXPRESSION_ACCEPT_LBRACKET_START;
				}
				Op.type=PX_SCRIPT_TRANSLATOR_EXPRESSION_OP_STRLEN;
				Op.opclass=PX_SCRIPT_TRANSLATOR_OP_CLASS_SINGLE;
				Op.oplevel=1;
				PX_StringInitialize(analysis->mp,&Op.code);
				PX_StringCat(&Op.code,lexer.CurLexeme.buffer);
				PX_VectorPushback(&stream,&Op);
			}
			else if (PX_strequ(lexer.CurLexeme.buffer,"MEMLEN"))
			{
				if ((accept_type&PX_SCRIPT_EXPRESSION_ACCEPT_SINGLE)==0)
				{
					ret=PX_FALSE;
					goto _CLEAR;
				}
				else
				{
					accept_type=PX_SCRIPT_EXPRESSION_ACCEPT_LBRACKET_START;
				}

				Op.type=PX_SCRIPT_TRANSLATOR_EXPRESSION_OP_MEMLEN;
				Op.opclass=PX_SCRIPT_TRANSLATOR_OP_CLASS_SINGLE;
				Op.oplevel=1;
				PX_StringInitialize(analysis->mp,&Op.code);
				PX_StringCat(&Op.code,lexer.CurLexeme.buffer);
				PX_VectorPushback(&stream,&Op);
			}
			else if (PX_strequ(lexer.CurLexeme.buffer,"INT"))
			{
				if ((accept_type&PX_SCRIPT_EXPRESSION_ACCEPT_SINGLE)==0)
				{
					ret=PX_FALSE;
					goto _CLEAR;
				}
				else
				{
					accept_type=PX_SCRIPT_EXPRESSION_ACCEPT_LBRACKET_START;
				}

				Op.type=PX_SCRIPT_TRANSLATOR_EXPRESSION_OP_INT;
				Op.opclass=PX_SCRIPT_TRANSLATOR_OP_CLASS_SINGLE;
				Op.oplevel=1;
				PX_StringInitialize(analysis->mp,&Op.code);
				PX_StringCat(&Op.code,lexer.CurLexeme.buffer);
				PX_VectorPushback(&stream,&Op);
			}
			else if (PX_strequ(lexer.CurLexeme.buffer,"FLOAT"))
			{
				if ((accept_type&PX_SCRIPT_EXPRESSION_ACCEPT_SINGLE)==0)
				{
					ret=PX_FALSE;
					goto _CLEAR;
				}
				else
				{
					accept_type=PX_SCRIPT_EXPRESSION_ACCEPT_LBRACKET_START;
				}

				Op.type=PX_SCRIPT_TRANSLATOR_EXPRESSION_OP_FLOAT;
				Op.opclass=PX_SCRIPT_TRANSLATOR_OP_CLASS_SINGLE;
				Op.oplevel=1;
				PX_StringInitialize(analysis->mp,&Op.code);
				PX_StringCat(&Op.code,lexer.CurLexeme.buffer);
				PX_VectorPushback(&stream,&Op);
			}
			else if (PX_strequ(lexer.CurLexeme.buffer,"STRING"))
			{
				if ((accept_type&PX_SCRIPT_EXPRESSION_ACCEPT_SINGLE)==0)
				{
					ret=PX_FALSE;
					goto _CLEAR;
				}
				else
				{
					accept_type=PX_SCRIPT_EXPRESSION_ACCEPT_LBRACKET_START;
				}
				Op.type=PX_SCRIPT_TRANSLATOR_EXPRESSION_OP_STRING;
				Op.opclass=PX_SCRIPT_TRANSLATOR_OP_CLASS_SINGLE;
				Op.oplevel=1;
				PX_StringInitialize(analysis->mp,&Op.code);
				PX_StringCat(&Op.code,lexer.CurLexeme.buffer);
				PX_VectorPushback(&stream,&Op);
			}
			else if (PX_strequ(lexer.CurLexeme.buffer,"MEMORY"))
			{
				if ((accept_type&PX_SCRIPT_EXPRESSION_ACCEPT_SINGLE)==0)
				{
					ret=PX_FALSE;
					goto _CLEAR;
				}
				else
				{
					accept_type=PX_SCRIPT_EXPRESSION_ACCEPT_LBRACKET_START;
				}
				Op.type=PX_SCRIPT_TRANSLATOR_EXPRESSION_OP_MEMORY;
				Op.opclass=PX_SCRIPT_TRANSLATOR_OP_CLASS_SINGLE;
				Op.oplevel=1;
				PX_StringInitialize(analysis->mp,&Op.code);
				PX_StringCat(&Op.code,lexer.CurLexeme.buffer);
				PX_VectorPushback(&stream,&Op);
			}
			else if (PX_ScriptParseIsValidToken(lexer.CurLexeme.buffer))
			{
				if ((accept_type&PX_SCRIPT_EXPRESSION_ACCEPT_TOKEN)==0)
				{
					ret=PX_FALSE;
					goto _CLEAR;
				}

				PX_StringInitialize(analysis->mp,&Op.code);
				PX_StringCat(&Op.code,lexer.CurLexeme.buffer);

				state=PX_LexerGetState(&lexer);
				type=PX_ScriptTranslatorNextToken(&lexer);
				if (type==PX_LEXER_LEXEME_TYPE_DELIMITER&&lexer.Symbol=='(')
				{
					Op.type=PX_SCRIPT_TRANSLATOR_EXPRESSION_OP_FUNCTION;
					Op.opclass=PX_SCRIPT_TRANSLATOR_OP_CLASS_FUNC;
					Op.oplevel=0;
					accept_type=PX_SCRIPT_EXPRESSION_ACCEPT_LBRACKET_START;
				}
				else
				{
					Op.type=PX_SCRIPT_TRANSLATOR_EXPRESSION_TOKEN;
					Op.opclass=PX_SCRIPT_TRANSLATOR_OP_CLASS_VAR;
					Op.oplevel=0;
					accept_type=PX_SCRIPT_EXPRESSION_ACCEPT_BINARY|PX_SCRIPT_EXPRESSION_ACCEPT_MBRACKET_START|PX_SCRIPT_EXPRESSION_ACCEPT_BRACKET_END;
				}
				PX_VectorPushback(&stream,&Op);

				PX_LexerSetState(state);

			}
			else if(PX_strIsNumeric(lexer.CurLexeme.buffer))
			{
				accept_type=PX_SCRIPT_EXPRESSION_ACCEPT_BINARY|PX_SCRIPT_EXPRESSION_ACCEPT_BRACKET_END;
				Op.type=PX_SCRIPT_TRANSLATOR_EXPRESSION_TOKEN;
				Op.opclass=PX_SCRIPT_TRANSLATOR_OP_CLASS_CONST;
				Op.oplevel=0;
				PX_StringInitialize(analysis->mp,&Op.code);
				PX_StringCat(&Op.code,lexer.CurLexeme.buffer);
				PX_VectorPushback(&stream,&Op);
			}
			else if(lexer.CurLexeme.buffer[0]=='0'&&lexer.CurLexeme.buffer[1]=='X')
			{
				for (i=2;lexer.CurLexeme.buffer[i];i++)
				{
					if (lexer.CurLexeme.buffer[i]>='0'&&lexer.CurLexeme.buffer[i]<='F')
					{
						continue;
					}
					else
					{
						ret=PX_FALSE;
						PX_ScriptTranslatorError(&analysis->lexer,"Illegal hexadecimal.");
						goto _CLEAR;
					}
				}
				accept_type=PX_SCRIPT_EXPRESSION_ACCEPT_BINARY|PX_SCRIPT_EXPRESSION_ACCEPT_BRACKET_END;
				Op.type=PX_SCRIPT_TRANSLATOR_EXPRESSION_TOKEN;
				Op.opclass=PX_SCRIPT_TRANSLATOR_OP_CLASS_CONST;
				Op.oplevel=0;
				PX_StringInitialize(analysis->mp,&Op.code);
				PX_StringCat(&Op.code,lexer.CurLexeme.buffer);
				PX_VectorPushback(&stream,&Op);
			}
			else
			{
				ret=PX_FALSE;
				PX_ScriptTranslatorError(&analysis->lexer,"Unexpected opcode.");
				goto _CLEAR;
			}
		}
		else if (type==PX_LEXER_LEXEME_TYPE_CONATINER)
		{
			if (lexer.CurrentContainerType==quotes)
			{
				PX_StringReplace(&lexer.CurLexeme,"\n","\\n");
				PX_StringReplace(&lexer.CurLexeme,"\r","\\r");
			}
			Op.type=PX_SCRIPT_TRANSLATOR_EXPRESSION_TOKEN;
			Op.opclass=PX_SCRIPT_TRANSLATOR_OP_CLASS_CONTAINER;
			Op.oplevel=0;
			PX_StringInitialize(analysis->mp,&Op.code);
			PX_StringCat(&Op.code,lexer.CurLexeme.buffer);
			PX_VectorPushback(&stream,&Op);
			accept_type=PX_SCRIPT_EXPRESSION_ACCEPT_BINARY|PX_SCRIPT_EXPRESSION_ACCEPT_BRACKET_END;
		}
		else if (type==PX_LEXER_LEXEME_TYPE_DELIMITER)
		{
			switch(lexer.Symbol)
			{
			case '+':
				{
					if ((accept_type&PX_SCRIPT_EXPRESSION_ACCEPT_BINARY)==0&&(accept_type&PX_SCRIPT_EXPRESSION_ACCEPT_SINGLE)==0)
					{
						ret=PX_FALSE;
						goto _CLEAR;
					}
					state=PX_LexerGetState(&lexer);

					if(PX_LexerGetNextLexeme(&lexer)==PX_LEXER_LEXEME_TYPE_DELIMITER&&lexer.Symbol=='+')
					{
						if (accept_type&PX_SCRIPT_EXPRESSION_ACCEPT_BINARY)
						{
							Op.type=PX_SCRIPT_TRANSLATOR_EXPRESSION_OP_INC;
							Op.opclass=PX_SCRIPT_TRANSLATOR_OP_CLASS_SINGLE;
							Op.oplevel=PX_ScriptParseGetOpLevel("++",PX_FALSE);
							PX_StringInitialize(analysis->mp,&Op.code);
							PX_StringCat(&Op.code,"++");
							PX_VectorPushback(&stream,&Op);
							accept_type=PX_SCRIPT_EXPRESSION_ACCEPT_BINARY|PX_SCRIPT_EXPRESSION_ACCEPT_BRACKET_END;
						}
						else
						{
							ret=PX_FALSE;
							goto _CLEAR;
						}
					}
					else
					{
						PX_LexerSetState(state);

						if (accept_type&PX_SCRIPT_EXPRESSION_ACCEPT_BINARY)
						{
							Op.type=PX_SCRIPT_TRANSLATOR_EXPRESSION_OP_ADD;
							Op.opclass=PX_SCRIPT_TRANSLATOR_OP_CLASS_BINARY;
							Op.oplevel=PX_ScriptParseGetOpLevel("+",PX_TRUE);
							PX_StringInitialize(analysis->mp,&Op.code);
							PX_StringCat(&Op.code,"+");
							PX_VectorPushback(&stream,&Op);
							accept_type=PX_SCRIPT_EXPRESSION_ACCEPT_TOKEN|PX_SCRIPT_EXPRESSION_ACCEPT_LBRACKET_START|PX_SCRIPT_EXPRESSION_ACCEPT_SINGLE;
						}
						else
						{
							Op.type=PX_SCRIPT_TRANSLATOR_EXPRESSION_OP_POSITIVE;
							Op.opclass=PX_SCRIPT_TRANSLATOR_OP_CLASS_SINGLE;
							Op.oplevel=PX_ScriptParseGetOpLevel("+",PX_FALSE);
							PX_StringInitialize(analysis->mp,&Op.code);
							PX_StringCat(&Op.code,"+");
							PX_VectorPushback(&stream,&Op);
							accept_type=PX_SCRIPT_EXPRESSION_ACCEPT_TOKEN|PX_SCRIPT_EXPRESSION_ACCEPT_LBRACKET_START;
						}
					}

					
				}
			break;

			case  '-':
				{
					if ((accept_type&PX_SCRIPT_EXPRESSION_ACCEPT_BINARY)==0&&(accept_type&PX_SCRIPT_EXPRESSION_ACCEPT_SINGLE)==0)
					{
						ret=PX_FALSE;
						goto _CLEAR;
					}

					state=PX_LexerGetState(&lexer);
					type=PX_LexerGetNextLexeme(&lexer);
					if(type==PX_LEXER_LEXEME_TYPE_DELIMITER&&lexer.Symbol=='>')
					{
						if (accept_type&PX_SCRIPT_EXPRESSION_ACCEPT_BINARY)
						{
							Op.type=PX_SCRIPT_TRANSLATOR_EXPRESSION_OP_OFT;
							Op.opclass=PX_SCRIPT_TRANSLATOR_OP_CLASS_BINARY;
							Op.oplevel=PX_ScriptParseGetOpLevel("->",PX_TRUE);
							PX_StringInitialize(analysis->mp,&Op.code);
							PX_StringCat(&Op.code,"->");
							PX_VectorPushback(&stream,&Op);
							accept_type=PX_SCRIPT_EXPRESSION_ACCEPT_TOKEN;
						}
						else
						{
							ret=PX_FALSE;
							goto _CLEAR;
						}
					}
					else if(type==PX_LEXER_LEXEME_TYPE_DELIMITER&&lexer.Symbol=='-')
					{
						if (accept_type&PX_SCRIPT_EXPRESSION_ACCEPT_BINARY)
						{
							Op.type=PX_SCRIPT_TRANSLATOR_EXPRESSION_OP_DEC;
							Op.opclass=PX_SCRIPT_TRANSLATOR_OP_CLASS_SINGLE;
							Op.oplevel=PX_ScriptParseGetOpLevel("--",PX_FALSE);
							PX_StringInitialize(analysis->mp,&Op.code);
							PX_StringCat(&Op.code,"--");
							PX_VectorPushback(&stream,&Op);
							accept_type=PX_SCRIPT_EXPRESSION_ACCEPT_BINARY|PX_SCRIPT_EXPRESSION_ACCEPT_BRACKET_END;
						}
						else
						{
							ret=PX_FALSE;
							goto _CLEAR;
						}
					}
					else
					{
						PX_LexerSetState(state);
						if (accept_type&PX_SCRIPT_EXPRESSION_ACCEPT_BINARY)
						{
							Op.type=PX_SCRIPT_TRANSLATOR_EXPRESSION_OP_SUB;
							Op.opclass=PX_SCRIPT_TRANSLATOR_OP_CLASS_BINARY;
							Op.oplevel=PX_ScriptParseGetOpLevel("-",PX_TRUE);
							PX_StringInitialize(analysis->mp,&Op.code);
							PX_StringCat(&Op.code,"-");
							PX_VectorPushback(&stream,&Op);
							accept_type=PX_SCRIPT_EXPRESSION_ACCEPT_TOKEN|PX_SCRIPT_EXPRESSION_ACCEPT_LBRACKET_START|PX_SCRIPT_EXPRESSION_ACCEPT_SINGLE;
						}
						else
						{
							Op.type=PX_SCRIPT_TRANSLATOR_EXPRESSION_OP_NEGATIVE;
							Op.opclass=PX_SCRIPT_TRANSLATOR_OP_CLASS_SINGLE;
							Op.oplevel=PX_ScriptParseGetOpLevel("-",PX_FALSE);
							PX_StringInitialize(analysis->mp,&Op.code);
							PX_StringCat(&Op.code,"-");
							PX_VectorPushback(&stream,&Op);
							accept_type=PX_SCRIPT_EXPRESSION_ACCEPT_TOKEN|PX_SCRIPT_EXPRESSION_ACCEPT_LBRACKET_START;
						}
					}
					
				}
				break;

			case '*':
				{
					if ((accept_type&PX_SCRIPT_EXPRESSION_ACCEPT_BINARY)==0&&(accept_type&PX_SCRIPT_EXPRESSION_ACCEPT_SINGLE)==0)
					{
						ret=PX_FALSE;
						goto _CLEAR;
					}

					if (accept_type&PX_SCRIPT_EXPRESSION_ACCEPT_BINARY)
					{
						Op.type=PX_SCRIPT_TRANSLATOR_EXPRESSION_OP_MUL;
						Op.opclass=PX_SCRIPT_TRANSLATOR_OP_CLASS_BINARY;
						Op.oplevel=PX_ScriptParseGetOpLevel("*",PX_TRUE);
						PX_StringInitialize(analysis->mp,&Op.code);
						PX_StringCat(&Op.code,"*");
						PX_VectorPushback(&stream,&Op);
						accept_type=PX_SCRIPT_EXPRESSION_ACCEPT_TOKEN|PX_SCRIPT_EXPRESSION_ACCEPT_LBRACKET_START|PX_SCRIPT_EXPRESSION_ACCEPT_SINGLE;
					}
					else
					{
						Op.type=PX_SCRIPT_TRANSLATOR_EXPRESSION_OP_PTR;
						Op.opclass=PX_SCRIPT_TRANSLATOR_OP_CLASS_SINGLE;
						Op.oplevel=PX_ScriptParseGetOpLevel("*",PX_FALSE);
						PX_StringInitialize(analysis->mp,&Op.code);
						PX_StringCat(&Op.code,"*");
						PX_VectorPushback(&stream,&Op);
						accept_type=PX_SCRIPT_EXPRESSION_ACCEPT_TOKEN|PX_SCRIPT_EXPRESSION_ACCEPT_LBRACKET_START;
					}

				}
				break;

			case '/':
				{
					
					if ((accept_type&PX_SCRIPT_EXPRESSION_ACCEPT_BINARY)==0)
					{
						ret=PX_FALSE;
						PX_ScriptTranslatorError(&analysis->lexer,"Unexpected opcode.");
						goto _CLEAR;
					}
					Op.type=PX_SCRIPT_TRANSLATOR_EXPRESSION_OP_DIV;
					Op.opclass=PX_SCRIPT_TRANSLATOR_OP_CLASS_BINARY;
					Op.oplevel=PX_ScriptParseGetOpLevel("/",PX_TRUE);
					PX_StringInitialize(analysis->mp,&Op.code);
					PX_StringCat(&Op.code,"/");
					PX_VectorPushback(&stream,&Op);
					accept_type=PX_SCRIPT_EXPRESSION_ACCEPT_TOKEN|PX_SCRIPT_EXPRESSION_ACCEPT_LBRACKET_START|PX_SCRIPT_EXPRESSION_ACCEPT_SINGLE;
				}
				break;

			case  '[':
				{
					if ((accept_type&PX_SCRIPT_EXPRESSION_ACCEPT_MBRACKET_START)==0)
					{
						ret=PX_FALSE;
						PX_ScriptTranslatorError(&analysis->lexer,"Unexpected opcode.");
						goto _CLEAR;
					}
					Op.type=PX_SCRIPT_TRANSLATOR_EXPRESSION_OP_MBRACKETBEGIN;
					Op.opclass=PX_SCRIPT_TRANSLATOR_OP_CLASS_CONTAINER;
					Op.oplevel=PX_ScriptParseGetOpLevel("[",PX_FALSE);
					PX_StringInitialize(analysis->mp,&Op.code);
					PX_StringCat(&Op.code,"[");
					PX_VectorPushback(&stream,&Op);
					accept_type=PX_SCRIPT_EXPRESSION_ACCEPT_TOKEN|PX_SCRIPT_EXPRESSION_ACCEPT_BRACKET_START|PX_SCRIPT_EXPRESSION_ACCEPT_SINGLE;
				}
				break;

			case  ']':
				{
					if ((accept_type&PX_SCRIPT_EXPRESSION_ACCEPT_MBRACKET_END)==0)
					{
						ret=PX_FALSE;
						PX_ScriptTranslatorError(&analysis->lexer,"Unexpected opcode.");
						goto _CLEAR;
					}
					Op.type=PX_SCRIPT_TRANSLATOR_EXPRESSION_OP_MBRACKETEND;
					Op.opclass=PX_SCRIPT_TRANSLATOR_OP_CLASS_CONTAINER;
					Op.oplevel=PX_ScriptParseGetOpLevel("]",PX_FALSE);
					PX_StringInitialize(analysis->mp,&Op.code);
					PX_StringCat(&Op.code,"]");
					PX_VectorPushback(&stream,&Op);
					accept_type=PX_SCRIPT_EXPRESSION_ACCEPT_BINARY|PX_SCRIPT_EXPRESSION_ACCEPT_BRACKET_END;
				}
				break;

			case  '(':
				{
					if ((accept_type&PX_SCRIPT_EXPRESSION_ACCEPT_LBRACKET_START)==0)
					{
						ret=PX_FALSE;
						PX_ScriptTranslatorError(&analysis->lexer,"Unexpected opcode.");
						goto _CLEAR;
					}

					Op.type=PX_SCRIPT_TRANSLATOR_EXPRESSION_OP_LBRACKETBEGIN;
					Op.opclass=PX_SCRIPT_TRANSLATOR_OP_CLASS_CONTAINER;
					Op.oplevel=PX_ScriptParseGetOpLevel("(",PX_FALSE);
					PX_StringInitialize(analysis->mp,&Op.code);
					PX_StringCat(&Op.code,"(");
					PX_VectorPushback(&stream,&Op);
					accept_type=PX_SCRIPT_EXPRESSION_ACCEPT_TOKEN|PX_SCRIPT_EXPRESSION_ACCEPT_LBRACKET_START|PX_SCRIPT_EXPRESSION_ACCEPT_LBRACKET_END|PX_SCRIPT_EXPRESSION_ACCEPT_SINGLE;
				}
				break;

			case ')':
				{
					if ((accept_type&PX_SCRIPT_EXPRESSION_ACCEPT_LBRACKET_END)==0)
					{
						ret=PX_FALSE;
						PX_ScriptTranslatorError(&analysis->lexer,"Unexpected opcode.");
						goto _CLEAR;
					}
					Op.type=PX_SCRIPT_TRANSLATOR_EXPRESSION_OP_LBRACKETEND;
					Op.opclass=PX_SCRIPT_TRANSLATOR_OP_CLASS_CONTAINER;
					Op.oplevel=PX_ScriptParseGetOpLevel(")",PX_FALSE);
					PX_StringInitialize(analysis->mp,&Op.code);
					PX_StringCat(&Op.code,")");
					PX_VectorPushback(&stream,&Op);
					accept_type=PX_SCRIPT_EXPRESSION_ACCEPT_BINARY|PX_SCRIPT_EXPRESSION_ACCEPT_BRACKET_END|PX_SCRIPT_EXPRESSION_ACCEPT_MBRACKET_START;
				}
				break;

			case '.':
				{
					if ((accept_type&PX_SCRIPT_EXPRESSION_ACCEPT_BINARY)==0)
					{
						ret=PX_FALSE;
						PX_ScriptTranslatorError(&analysis->lexer,"Unexpected opcode.");
						goto _CLEAR;
					}
					Op.type=PX_SCRIPT_TRANSLATOR_EXPRESSION_OP_DOT;
					Op.opclass=PX_SCRIPT_TRANSLATOR_OP_CLASS_SINGLE;
					Op.oplevel=PX_ScriptParseGetOpLevel(lexer.CurLexeme.buffer,PX_FALSE);
					PX_StringInitialize(analysis->mp,&Op.code);
					PX_StringCat(&Op.code,".");
					PX_VectorPushback(&stream,&Op);
					accept_type=PX_SCRIPT_EXPRESSION_ACCEPT_TOKEN;
				}
				break;

			case '!':
				{
					state=PX_LexerGetState(&lexer);
					type=PX_LexerGetNextLexeme(&lexer);
					if(type!=PX_LEXER_LEXEME_TYPE_DELIMITER)
					{
						PX_LexerSetState(state);

						if ((accept_type&PX_SCRIPT_EXPRESSION_ACCEPT_SINGLE)==0)
						{
							ret=PX_FALSE;
							PX_ScriptTranslatorError(&analysis->lexer,"Unexpected opcode.");
							goto _CLEAR;
						}
						Op.type=PX_SCRIPT_TRANSLATOR_EXPRESSION_OP_NOT;
						Op.opclass=PX_SCRIPT_TRANSLATOR_OP_CLASS_SINGLE;
						Op.oplevel=PX_ScriptParseGetOpLevel("!",PX_FALSE);
						PX_StringInitialize(analysis->mp,&Op.code);
						PX_StringCat(&Op.code,"!");
						PX_VectorPushback(&stream,&Op);
						accept_type=PX_SCRIPT_EXPRESSION_ACCEPT_LBRACKET_START|PX_SCRIPT_EXPRESSION_ACCEPT_TOKEN;
					}
					else if (lexer.Symbol=='=')
					{
						if ((accept_type&PX_SCRIPT_EXPRESSION_ACCEPT_BINARY)==0)
						{
							ret=PX_FALSE;
							PX_ScriptTranslatorError(&analysis->lexer,"Unexpected opcode.");
							goto _CLEAR;
						}
						Op.type=PX_SCRIPT_TRANSLATOR_EXPRESSION_OP_UNEQU;
						Op.opclass=PX_SCRIPT_TRANSLATOR_OP_CLASS_BINARY;
						Op.oplevel=PX_ScriptParseGetOpLevel("!=",PX_TRUE);
						PX_StringInitialize(analysis->mp,&Op.code);
						PX_StringCat(&Op.code,"!=");
						PX_VectorPushback(&stream,&Op);
						accept_type=PX_SCRIPT_EXPRESSION_ACCEPT_LBRACKET_START|PX_SCRIPT_EXPRESSION_ACCEPT_TOKEN|PX_SCRIPT_EXPRESSION_ACCEPT_SINGLE;
					}
					else if (lexer.Symbol=='(')
					{
						PX_LexerSetState(state);

						if ((accept_type&PX_SCRIPT_EXPRESSION_ACCEPT_SINGLE)==0)
						{
							ret=PX_FALSE;
							PX_ScriptTranslatorError(&analysis->lexer,"Unexpected opcode.");
							goto _CLEAR;
						}
						Op.type=PX_SCRIPT_TRANSLATOR_EXPRESSION_OP_NOT;
						Op.opclass=PX_SCRIPT_TRANSLATOR_OP_CLASS_SINGLE;
						Op.oplevel=PX_ScriptParseGetOpLevel("!",PX_FALSE);
						PX_StringInitialize(analysis->mp,&Op.code);
						PX_StringCat(&Op.code,"!");
						PX_VectorPushback(&stream,&Op);
						accept_type=PX_SCRIPT_EXPRESSION_ACCEPT_LBRACKET_START|PX_SCRIPT_EXPRESSION_ACCEPT_TOKEN;
					}
					else
					{
						ret=PX_FALSE;
						PX_ScriptTranslatorError(&analysis->lexer,"Unexpected opcode.");
						goto _CLEAR;
					}

				}
				break;

			case '~':
				{
					if ((accept_type&PX_SCRIPT_EXPRESSION_ACCEPT_SINGLE)==0)
					{
						ret=PX_FALSE;
						PX_ScriptTranslatorError(&analysis->lexer,"Unexpected opcode.");
						goto _CLEAR;
					}
					Op.type=PX_SCRIPT_TRANSLATOR_EXPRESSION_OP_INV;
					Op.opclass=PX_SCRIPT_TRANSLATOR_OP_CLASS_SINGLE;
					Op.oplevel=PX_ScriptParseGetOpLevel("~",PX_FALSE);
					PX_StringInitialize(analysis->mp,&Op.code);
					PX_StringCat(&Op.code,"~");
					PX_VectorPushback(&stream,&Op);
					accept_type=PX_SCRIPT_EXPRESSION_ACCEPT_LBRACKET_START|PX_SCRIPT_EXPRESSION_ACCEPT_TOKEN;
				}
				break;

			case '^':
				{
					if ((accept_type&PX_SCRIPT_EXPRESSION_ACCEPT_BINARY)==0)
					{
						ret=PX_FALSE;
						PX_ScriptTranslatorError(&analysis->lexer,"Unexpected opcode.");
						goto _CLEAR;
					}
					Op.type=PX_SCRIPT_TRANSLATOR_EXPRESSION_OP_XOR;
					Op.opclass=PX_SCRIPT_TRANSLATOR_OP_CLASS_BINARY;
					Op.oplevel=PX_ScriptParseGetOpLevel("^",PX_TRUE);
					PX_StringInitialize(analysis->mp,&Op.code);
					PX_StringCat(&Op.code,"^");
					PX_VectorPushback(&stream,&Op);
					accept_type=PX_SCRIPT_EXPRESSION_ACCEPT_LBRACKET_START|PX_SCRIPT_EXPRESSION_ACCEPT_TOKEN|PX_SCRIPT_EXPRESSION_ACCEPT_SINGLE;
				}
				break;

			case  '%':
				{
					if ((accept_type&PX_SCRIPT_EXPRESSION_ACCEPT_BINARY)==0)
					{
						ret=PX_FALSE;
						PX_ScriptTranslatorError(&analysis->lexer,"Unexpected opcode.");
						goto _CLEAR;
					}
					Op.type=PX_SCRIPT_TRANSLATOR_EXPRESSION_OP_MOD;
					Op.opclass=PX_SCRIPT_TRANSLATOR_OP_CLASS_SINGLE;
					Op.oplevel=PX_ScriptParseGetOpLevel(lexer.CurLexeme.buffer,PX_TRUE);
					PX_StringInitialize(analysis->mp,&Op.code);
					PX_StringCat(&Op.code,"%");
					PX_VectorPushback(&stream,&Op);
					accept_type=PX_SCRIPT_EXPRESSION_ACCEPT_LBRACKET_START|PX_SCRIPT_EXPRESSION_ACCEPT_TOKEN|PX_SCRIPT_EXPRESSION_ACCEPT_SINGLE;
				}
				break;


			case '<':
				{
					if ((accept_type&PX_SCRIPT_EXPRESSION_ACCEPT_BINARY)==0)
					{
						ret=PX_FALSE;
						PX_ScriptTranslatorError(&analysis->lexer,"Unexpected opcode.");
						goto _CLEAR;
					}

					state=PX_LexerGetState(&lexer);

					if(PX_LexerGetNextLexeme(&lexer)!=PX_LEXER_LEXEME_TYPE_DELIMITER)
					{
						PX_LexerSetState(state);

						Op.type=PX_SCRIPT_TRANSLATOR_EXPRESSION_OP_LESS;
						Op.opclass=PX_SCRIPT_TRANSLATOR_OP_CLASS_BINARY;
						Op.oplevel=PX_ScriptParseGetOpLevel("<",PX_TRUE);
						PX_StringInitialize(analysis->mp,&Op.code);
						PX_StringCat(&Op.code,"<");
						PX_VectorPushback(&stream,&Op);
						accept_type=PX_SCRIPT_EXPRESSION_ACCEPT_LBRACKET_START|PX_SCRIPT_EXPRESSION_ACCEPT_TOKEN|PX_SCRIPT_EXPRESSION_ACCEPT_SINGLE;
					}
					else if (lexer.Symbol=='<')
					{
						Op.type=PX_SCRIPT_TRANSLATOR_EXPRESSION_OP_SHL;
						Op.opclass=PX_SCRIPT_TRANSLATOR_OP_CLASS_BINARY;
						Op.oplevel=PX_ScriptParseGetOpLevel("<<",PX_TRUE);
						PX_StringInitialize(analysis->mp,&Op.code);
						PX_StringCat(&Op.code,"<<");
						PX_VectorPushback(&stream,&Op);
						accept_type=PX_SCRIPT_EXPRESSION_ACCEPT_LBRACKET_START|PX_SCRIPT_EXPRESSION_ACCEPT_TOKEN|PX_SCRIPT_EXPRESSION_ACCEPT_SINGLE;
					}
					else if (lexer.Symbol=='=')
					{
						Op.type=PX_SCRIPT_TRANSLATOR_EXPRESSION_OP_LESSEQU;
						Op.opclass=PX_SCRIPT_TRANSLATOR_OP_CLASS_BINARY;
						Op.oplevel=PX_ScriptParseGetOpLevel("<=",PX_TRUE);
						PX_StringInitialize(analysis->mp,&Op.code);
						PX_StringCat(&Op.code,"<=");
						PX_VectorPushback(&stream,&Op);
						accept_type=PX_SCRIPT_EXPRESSION_ACCEPT_LBRACKET_START|PX_SCRIPT_EXPRESSION_ACCEPT_TOKEN|PX_SCRIPT_EXPRESSION_ACCEPT_SINGLE;
					}
					else
					{
						ret=PX_FALSE;
						PX_ScriptTranslatorError(&analysis->lexer,"Unexpected opcode.");
						goto _CLEAR;
					}

				}
				break;

			case '>':
				{
					if ((accept_type&PX_SCRIPT_EXPRESSION_ACCEPT_BINARY)==0)
					{
						ret=PX_FALSE;
						PX_ScriptTranslatorError(&analysis->lexer,"Unexpected opcode.");
						goto _CLEAR;
					}

					state=PX_LexerGetState(&lexer);

					if(PX_LexerGetNextLexeme(&lexer)!=PX_LEXER_LEXEME_TYPE_DELIMITER)
					{
						PX_LexerSetState(state);

						Op.type=PX_SCRIPT_TRANSLATOR_EXPRESSION_OP_LARGE;
						Op.opclass=PX_SCRIPT_TRANSLATOR_OP_CLASS_BINARY;
						Op.oplevel=PX_ScriptParseGetOpLevel(">",PX_TRUE);
						PX_StringInitialize(analysis->mp,&Op.code);
						PX_StringCat(&Op.code,">");
						PX_VectorPushback(&stream,&Op);
						accept_type=PX_SCRIPT_EXPRESSION_ACCEPT_LBRACKET_START|PX_SCRIPT_EXPRESSION_ACCEPT_TOKEN|PX_SCRIPT_EXPRESSION_ACCEPT_SINGLE;
					}
					else if (lexer.Symbol=='>')
					{
						Op.type=PX_SCRIPT_TRANSLATOR_EXPRESSION_OP_SHR;
						Op.opclass=PX_SCRIPT_TRANSLATOR_OP_CLASS_BINARY;
						Op.oplevel=PX_ScriptParseGetOpLevel(">>",PX_TRUE);
						PX_StringInitialize(analysis->mp,&Op.code);
						PX_StringCat(&Op.code,">>");
						PX_VectorPushback(&stream,&Op);
						accept_type=PX_SCRIPT_EXPRESSION_ACCEPT_LBRACKET_START|PX_SCRIPT_EXPRESSION_ACCEPT_TOKEN|PX_SCRIPT_EXPRESSION_ACCEPT_SINGLE;
					}
					else if (lexer.Symbol=='=')
					{
						Op.type=PX_SCRIPT_TRANSLATOR_EXPRESSION_OP_LARGEEUQ;
						Op.opclass=PX_SCRIPT_TRANSLATOR_OP_CLASS_BINARY;
						Op.oplevel=PX_ScriptParseGetOpLevel(">=",PX_TRUE);
						PX_StringInitialize(analysis->mp,&Op.code);
						PX_StringCat(&Op.code,">=");
						PX_VectorPushback(&stream,&Op);
						accept_type=PX_SCRIPT_EXPRESSION_ACCEPT_LBRACKET_START|PX_SCRIPT_EXPRESSION_ACCEPT_TOKEN|PX_SCRIPT_EXPRESSION_ACCEPT_SINGLE;
					}
					else
					{
						ret=PX_FALSE;
						PX_ScriptTranslatorError(&analysis->lexer,"Unexpected opcode.");
						goto _CLEAR;
					}

				}
				break;

			case '=':
				{
					if ((accept_type&PX_SCRIPT_EXPRESSION_ACCEPT_BINARY)==0)
					{
						ret=PX_FALSE;
						PX_ScriptTranslatorError(&analysis->lexer,"Unexpected opcode.");
						goto _CLEAR;
					}

					state=PX_LexerGetState(&lexer);

					if(PX_LexerGetNextLexeme(&lexer)==PX_LEXER_LEXEME_TYPE_DELIMITER&&lexer.Symbol=='=')
					{
						Op.type=PX_SCRIPT_TRANSLATOR_EXPRESSION_OP_LGEQU;
						Op.opclass=PX_SCRIPT_TRANSLATOR_OP_CLASS_BINARY;
						Op.oplevel=PX_ScriptParseGetOpLevel("==",PX_TRUE);
						PX_StringInitialize(analysis->mp,&Op.code);
						PX_StringCat(&Op.code,"==");
						PX_VectorPushback(&stream,&Op);
						accept_type=PX_SCRIPT_EXPRESSION_ACCEPT_LBRACKET_START|PX_SCRIPT_EXPRESSION_ACCEPT_TOKEN|PX_SCRIPT_EXPRESSION_ACCEPT_SINGLE;
					}
					else
					{
						PX_LexerSetState(state);
						Op.type=PX_SCRIPT_TRANSLATOR_EXPRESSION_OP_EQU;
						Op.opclass=PX_SCRIPT_TRANSLATOR_OP_CLASS_BINARY;
						Op.oplevel=PX_ScriptParseGetOpLevel("=",PX_TRUE);
						PX_StringInitialize(analysis->mp,&Op.code);
						PX_StringCat(&Op.code,"=");
						PX_VectorPushback(&stream,&Op);
						accept_type=PX_SCRIPT_EXPRESSION_ACCEPT_LBRACKET_START|PX_SCRIPT_EXPRESSION_ACCEPT_TOKEN|PX_SCRIPT_EXPRESSION_ACCEPT_SINGLE;
						continue;
					}
				}
				break;


			case '&':
				{
					if ((accept_type&PX_SCRIPT_EXPRESSION_ACCEPT_BINARY)==0&&(accept_type&PX_SCRIPT_EXPRESSION_ACCEPT_SINGLE)==0)
					{
						ret=PX_FALSE;
						goto _CLEAR;
					}

					if (accept_type&PX_SCRIPT_EXPRESSION_ACCEPT_BINARY)
					{
						state=PX_LexerGetState(&lexer);

						if(PX_LexerGetNextLexeme(&lexer)!=PX_LEXER_LEXEME_TYPE_DELIMITER)
						{
							if ((accept_type&PX_SCRIPT_EXPRESSION_ACCEPT_BINARY)==0)
							{
								ret=PX_FALSE;
								goto _CLEAR;
							}

							PX_LexerSetState(state);

							Op.type=PX_SCRIPT_TRANSLATOR_EXPRESSION_OP_AND;
							Op.opclass=PX_SCRIPT_TRANSLATOR_OP_CLASS_BINARY;
							Op.oplevel=PX_ScriptParseGetOpLevel("&",PX_TRUE);
							PX_StringInitialize(analysis->mp,&Op.code);
							PX_StringCat(&Op.code,"&");
							PX_VectorPushback(&stream,&Op);
							accept_type=PX_SCRIPT_EXPRESSION_ACCEPT_LBRACKET_START|PX_SCRIPT_EXPRESSION_ACCEPT_TOKEN|PX_SCRIPT_EXPRESSION_ACCEPT_SINGLE;
						}
						else if (lexer.Symbol=='&')
						{
							if ((accept_type&PX_SCRIPT_EXPRESSION_ACCEPT_BINARY)==0)
							{
								ret=PX_FALSE;
								goto _CLEAR;
							}
							Op.type=PX_SCRIPT_TRANSLATOR_EXPRESSION_OP_LAND;
							Op.opclass=PX_SCRIPT_TRANSLATOR_OP_CLASS_BINARY;
							Op.oplevel=PX_ScriptParseGetOpLevel("&&",PX_TRUE);
							PX_StringInitialize(analysis->mp,&Op.code);
							PX_StringCat(&Op.code,"&&");
							PX_VectorPushback(&stream,&Op);
							accept_type=PX_SCRIPT_EXPRESSION_ACCEPT_LBRACKET_START|PX_SCRIPT_EXPRESSION_ACCEPT_TOKEN|PX_SCRIPT_EXPRESSION_ACCEPT_SINGLE;
						}
						else if (lexer.Symbol=='(')
						{
							if ((accept_type&PX_SCRIPT_EXPRESSION_ACCEPT_BINARY)==0)
							{
								ret=PX_FALSE;
								goto _CLEAR;
							}

							PX_LexerSetState(state);

							Op.type=PX_SCRIPT_TRANSLATOR_EXPRESSION_OP_AND;
							Op.opclass=PX_SCRIPT_TRANSLATOR_OP_CLASS_BINARY;
							Op.oplevel=PX_ScriptParseGetOpLevel("&",PX_TRUE);
							PX_StringInitialize(analysis->mp,&Op.code);
							PX_StringCat(&Op.code,"&");
							PX_VectorPushback(&stream,&Op);
							accept_type=PX_SCRIPT_EXPRESSION_ACCEPT_LBRACKET_START|PX_SCRIPT_EXPRESSION_ACCEPT_TOKEN|PX_SCRIPT_EXPRESSION_ACCEPT_SINGLE;
						}
						else
						{
							ret=PX_FALSE;
							PX_ScriptTranslatorError(&analysis->lexer,"Unexpected opcode.");
							goto _CLEAR;
						}
					}
					else
					{
						Op.type=PX_SCRIPT_TRANSLATOR_EXPRESSION_OP_ADR;
						Op.opclass=PX_SCRIPT_TRANSLATOR_OP_CLASS_SINGLE;
						Op.oplevel=PX_ScriptParseGetOpLevel("&",PX_FALSE);
						PX_StringInitialize(analysis->mp,&Op.code);
						PX_StringCat(&Op.code,"&");
						PX_VectorPushback(&stream,&Op);
						accept_type=PX_SCRIPT_EXPRESSION_ACCEPT_TOKEN;
					}


				}
				break;

			case  '|':
				{
					if ((accept_type&PX_SCRIPT_EXPRESSION_ACCEPT_BINARY)==0)
					{
						ret=PX_FALSE;
						PX_ScriptTranslatorError(&analysis->lexer,"Unexpected opcode.");
						goto _CLEAR;
					}

					state=PX_LexerGetState(&lexer);

					if(PX_LexerGetNextLexeme(&lexer)!=PX_LEXER_LEXEME_TYPE_DELIMITER)
					{
						PX_LexerSetState(state);

						Op.type=PX_SCRIPT_TRANSLATOR_EXPRESSION_OP_OR;
						Op.opclass=PX_SCRIPT_TRANSLATOR_OP_CLASS_BINARY;
						Op.oplevel=PX_ScriptParseGetOpLevel("|",PX_TRUE);
						PX_StringInitialize(analysis->mp,&Op.code);
						PX_StringCat(&Op.code,"|");
						PX_VectorPushback(&stream,&Op);
						accept_type=PX_SCRIPT_EXPRESSION_ACCEPT_LBRACKET_START|PX_SCRIPT_EXPRESSION_ACCEPT_TOKEN|PX_SCRIPT_EXPRESSION_ACCEPT_SINGLE;
					}
					else if (lexer.Symbol=='|')
					{
						Op.type=PX_SCRIPT_TRANSLATOR_EXPRESSION_OP_LAND;
						Op.opclass=PX_SCRIPT_TRANSLATOR_OP_CLASS_BINARY;
						Op.oplevel=PX_ScriptParseGetOpLevel("||",PX_TRUE);
						PX_StringInitialize(analysis->mp,&Op.code);
						PX_StringCat(&Op.code,"||");
						PX_VectorPushback(&stream,&Op);
						accept_type=PX_SCRIPT_EXPRESSION_ACCEPT_LBRACKET_START|PX_SCRIPT_EXPRESSION_ACCEPT_TOKEN|PX_SCRIPT_EXPRESSION_ACCEPT_SINGLE;
					}
					else if (lexer.Symbol=='(')
					{
						PX_LexerSetState(state);
						Op.type=PX_SCRIPT_TRANSLATOR_EXPRESSION_OP_OR;
						Op.opclass=PX_SCRIPT_TRANSLATOR_OP_CLASS_BINARY;
						Op.oplevel=PX_ScriptParseGetOpLevel("|",PX_TRUE);
						PX_StringInitialize(analysis->mp,&Op.code);
						PX_StringCat(&Op.code,"|");
						PX_VectorPushback(&stream,&Op);
						accept_type=PX_SCRIPT_EXPRESSION_ACCEPT_LBRACKET_START|PX_SCRIPT_EXPRESSION_ACCEPT_TOKEN|PX_SCRIPT_EXPRESSION_ACCEPT_SINGLE;
					}
					else
					{
						ret=PX_FALSE;
						PX_ScriptTranslatorError(&analysis->lexer,"Unexpected opcode.");
						goto _CLEAR;
					}

				}
				break;

			case ',':
				{
					if ((accept_type&PX_SCRIPT_EXPRESSION_ACCEPT_BINARY)==0)
					{
						ret=PX_FALSE;
						PX_ScriptTranslatorError(&analysis->lexer,"Unexpected opcode.");
						goto _CLEAR;
					}

						Op.type=PX_SCRIPT_TRANSLATOR_EXPRESSION_OP_COMMA;
						Op.opclass=PX_SCRIPT_TRANSLATOR_OP_CLASS_BINARY;
						Op.oplevel=PX_ScriptParseGetOpLevel(",",PX_TRUE);
						PX_StringInitialize(analysis->mp,&Op.code);
						PX_StringCat(&Op.code,",");
						PX_VectorPushback(&stream,&Op);
						accept_type=PX_SCRIPT_EXPRESSION_ACCEPT_LBRACKET_START|PX_SCRIPT_EXPRESSION_ACCEPT_TOKEN|PX_SCRIPT_EXPRESSION_ACCEPT_SINGLE;
				}
				break;
			}
		}
	}

	Op.type=PX_SCRIPT_TRANSLATOR_EXPRESSION_OP_LBRACKETEND;
	Op.opclass=PX_SCRIPT_TRANSLATOR_OP_CLASS_CONTAINER;
	Op.oplevel=0;
	PX_StringInitialize(analysis->mp,&Op.code);
	PX_StringCat(&Op.code,lexer.CurLexeme.buffer);
	PX_VectorPushback(&stream,&Op);
	PX_StringCat(out,";--------");

	for(i=0;i<PX_strlen(expr);i++)
	{
		if(expr[i]=='\r'||expr[i]=='\n')
			expr[i]=' ';
	}
	PX_StringCat(out,expr);
	PX_StringCat(out,"\n");
	if (!PX_ScriptParseExpressionStream(analysis,&stream,&streamOft,&operand,PX_SCRIPT_TRANSLATOR_EXPRESSION_OP_LBRACKETEND,out))
	{
		ret=PX_FALSE;
	}

	if (!PX_ScriptParseAST_MapTokenToR1(analysis,operand,out))
	{
		ret=PX_FALSE;
		goto _CLEAR;
	}
	
	*retOperand=operand;
_CLEAR:

	for (i=0;i<stream.size;i++)
	{
		PX_StringFree(&PX_VECTORAT(PX_SCRIPT_EXPR_STREAM,&stream,i)->code);
	}
	PX_VectorFree(&stream);

	for (i=0;i<opStack.size;i++)
	{
		PX_StringFree(&PX_VECTORAT(PX_SCRIPT_EXPR_STREAM,&opStack,i)->code);
	}
	PX_VectorFree(&opStack);

	for (i=0;i<tkStack.size;i++)
	{
		PX_StringFree(PX_VECTORAT(px_string,&tkStack,i));
	}
	PX_VectorFree(&tkStack);

	PX_LexerFree(&lexer);
	return ret;
}
static px_bool PX_ScriptParseVar(PX_SCRIPT_Analysis *analysis)
{
	PX_LEXER_LEXEME_TYPE type;
	PX_SCRIPT_VARIABLES variable,*pvar;
	PX_SCRIPT_PARSER_VAR_TYPE resType;
	px_int i;

	

	type=PX_ScriptTranslatorNextToken(&analysis->lexer);
	if (type!=PX_LEXER_LEXEME_TYPE_TOKEN)
	{
		return PX_FALSE;
	}

	if (PX_strequ(analysis->lexer.CurLexeme.buffer,PX_SCRIPT_TRANSLATOR_KEYWORD_VAR_INT))
	{
		resType=PX_SCRIPT_PARSER_VAR_TYPE_INT;
	}
	else if (PX_strequ(analysis->lexer.CurLexeme.buffer,PX_SCRIPT_TRANSLATOR_KEYWORD_VAR_FLOAT))
	{
		resType=PX_SCRIPT_PARSER_VAR_TYPE_FLOAT;
	}
	else if (PX_strequ(analysis->lexer.CurLexeme.buffer,PX_SCRIPT_TRANSLATOR_KEYWORD_VAR_STRING))
	{
		resType=PX_SCRIPT_PARSER_VAR_TYPE_STRING;
	}
	else if (PX_strequ(analysis->lexer.CurLexeme.buffer,PX_SCRIPT_TRANSLATOR_KEYWORD_VAR_MEMORY))
	{
		resType=PX_SCRIPT_PARSER_VAR_TYPE_MEMORY;
	}
	else
	{
		goto _ERROR;
	}

	while(PX_TRUE)
	{
		PX_memset(&variable,0,sizeof(variable));
		variable.bParam=PX_FALSE;
		variable.bInitialized=PX_FALSE;
		variable.BeginIndex=-1;
		variable.size=-1;
		variable.type=resType;
		variable.layer=analysis->v_astStructure.size;

		type=PX_ScriptTranslatorNextToken(&analysis->lexer);

		if (type==PX_LEXER_LEXEME_TYPE_DELIMITER&&analysis->lexer.Symbol=='*')
		{
			switch(variable.type)
			{
			case PX_SCRIPT_PARSER_VAR_TYPE_INT:
				{
					variable.type=PX_SCRIPT_PARSER_VAR_TYPE_INT_PTR;
				}
				break;
			case PX_SCRIPT_PARSER_VAR_TYPE_FLOAT:
				{
					variable.type=PX_SCRIPT_PARSER_VAR_TYPE_FLOAT_PTR;
				}
				break;
			case PX_SCRIPT_PARSER_VAR_TYPE_STRING:
				{
					variable.type=PX_SCRIPT_PARSER_VAR_TYPE_STRING_PTR;
				}
				break;
			case PX_SCRIPT_PARSER_VAR_TYPE_MEMORY:
				{
					variable.type=PX_SCRIPT_PARSER_VAR_TYPE_MEMORY_PTR;
				}
				break;
			default:
				goto _ERROR;
			}

			type=PX_ScriptTranslatorNextToken(&analysis->lexer);
		}

		if (type!=PX_LEXER_LEXEME_TYPE_TOKEN)
		{
			return PX_FALSE;
		}

		if (!PX_ScriptParseIsValidToken(analysis->lexer.CurLexeme.buffer))
		{
			PX_ScriptTranslatorError(&analysis->lexer,"Invalid var token");
			goto _ERROR;
		}

		PX_StringInitialize(analysis->mp,&variable.Mnemonic);
		PX_StringCat(&variable.Mnemonic,analysis->lexer.CurLexeme.buffer);

		type=PX_ScriptTranslatorNextToken(&analysis->lexer);

		if (type!=PX_LEXER_LEXEME_TYPE_DELIMITER)
		{
			PX_ScriptTranslatorError(&analysis->lexer,"Invalid var token");
			PX_StringFree(&variable.Mnemonic);
			goto _ERROR;
		}


		if (analysis->lexer.Symbol=='[')
		{

			switch(variable.type)
			{
			case PX_SCRIPT_PARSER_VAR_TYPE_INT:
				{
					variable.type=PX_SCRIPT_PARSER_VAR_TYPE_INT_ARRAY;
				}
				break;
			case PX_SCRIPT_PARSER_VAR_TYPE_FLOAT:
				{
					variable.type=PX_SCRIPT_PARSER_VAR_TYPE_FLOAT_ARRAY;
				}
				break;
			case PX_SCRIPT_PARSER_VAR_TYPE_STRING:
				{
					variable.type=PX_SCRIPT_PARSER_VAR_TYPE_STRING_ARRAY;
				}
				break;
			case PX_SCRIPT_PARSER_VAR_TYPE_MEMORY:
				{
					variable.type=PX_SCRIPT_PARSER_VAR_TYPE_MEMORY_ARRAY;
				}
				break;

			default:
				goto _ERROR;
			}

			if(PX_ScriptTranslatorNextToken(&analysis->lexer)!=PX_LEXER_LEXEME_TYPE_TOKEN)
			{
				PX_ScriptTranslatorError(&analysis->lexer,"Invalid var token");
				PX_StringFree(&variable.Mnemonic);
				goto _ERROR;
			}

			if(!PX_strIsNumeric(analysis->lexer.CurLexeme.buffer))
			{
				PX_ScriptTranslatorError(&analysis->lexer,"Invalid var token");
			}

			variable.size=PX_atoi(analysis->lexer.CurLexeme.buffer);

			if (variable.size==0)
			{
				PX_ScriptTranslatorError(&analysis->lexer,"Invalid array");
				PX_StringFree(&variable.Mnemonic);
				goto _ERROR;
			}

			type=PX_ScriptTranslatorNextToken(&analysis->lexer);

			if (type!=PX_LEXER_LEXEME_TYPE_DELIMITER||analysis->lexer.Symbol!=']')
			{
				PX_ScriptTranslatorError(&analysis->lexer,"Invalid var token");
				PX_StringFree(&variable.Mnemonic);
				goto _ERROR;
			}

			type=PX_ScriptTranslatorNextToken(&analysis->lexer);
		}
		else if(analysis->lexer.Symbol=='=')
		{
			variable.size=1;
			variable.bInitialized=PX_TRUE;
			PX_StringInitialize(analysis->mp,&variable.GlobalInitializeValue);
			while(PX_TRUE)
			{
				type=PX_ScriptTranslatorNextToken(&analysis->lexer);
				if (type==PX_LEXER_LEXEME_TYPE_DELIMITER&&analysis->lexer.Symbol==';')
				{
					break;
				}
				if (type==PX_LEXER_LEXEME_TYPE_DELIMITER&&analysis->lexer.Symbol==',')
				{
					break;
				}
				if (type==PX_LEXER_LEXEME_TYPE_END)
				{
					PX_ScriptTranslatorError(&analysis->lexer,"Error Expression.");
					PX_StringFree(&variable.Mnemonic);
					PX_StringFree(&variable.GlobalInitializeValue);
					goto _ERROR;
				}	
				PX_StringCat(&variable.GlobalInitializeValue,analysis->lexer.CurLexeme.buffer);
			}

			if (PX_strlen(variable.GlobalInitializeValue.buffer)==0)
			{
				PX_ScriptTranslatorError(&analysis->lexer,"Error Expression.");
				PX_StringFree(&variable.Mnemonic);
				PX_StringFree(&variable.GlobalInitializeValue);
				goto _ERROR;
			}

		}
		else
		{
			variable.size=1;
		}
		

		if (analysis->functionInside)
		{

			for (i=0;i<analysis->v_variablesStackTable.size;i++)
			{
				if (PX_strequ(variable.Mnemonic.buffer,PX_VECTORAT(PX_SCRIPT_VARIABLES,&analysis->v_variablesStackTable,i)->Mnemonic.buffer))
				{
					PX_ScriptTranslatorError(&analysis->lexer,"variable redefined.");
					PX_StringFree(&variable.Mnemonic);
					if(variable.bInitialized)
						PX_StringFree(&variable.GlobalInitializeValue);
					goto _ERROR;
				}
			}

			pvar=PX_VECTORAT(PX_SCRIPT_VARIABLES,&analysis->v_variablesStackTable,analysis->v_variablesStackTable.size-1);
			variable.BeginIndex=pvar->BeginIndex+variable.size;

			if (variable.BeginIndex+variable.size-1>analysis->currentAllocStackSize)
			{
				analysis->currentAllocStackSize=variable.BeginIndex+variable.size-1;
			}

			PX_VectorPushback(&analysis->v_variablesStackTable,&variable);

			//////////////////////////////////////////////////////////////////////////
			if(variable.bInitialized)
			{
				px_string code;
				px_string exprgen;
				PX_SCRIPT_AST_OPERAND retOperand;
				PX_StringInitialize(analysis->mp,&code);
				PX_StringInitialize(analysis->mp,&exprgen);
				PX_StringFormat2(&code,"%1=%2",PX_STRINGFORMAT_STRING(variable.Mnemonic.buffer),PX_STRINGFORMAT_STRING(variable.GlobalInitializeValue.buffer));
				if (!PX_ScriptParseExpression(analysis,code.buffer,&exprgen,&retOperand))
				{
					PX_StringFree(&code);
					PX_StringFree(&exprgen);
					return PX_FALSE;
				}
				PX_StringCat(&analysis->code,exprgen.buffer);
				PX_StringFree(&exprgen);
				PX_StringFree(&code);

			}
			else
			{
				if (variable.type==PX_SCRIPT_PARSER_VAR_TYPE_STRING)
				{
					px_string code;
					px_string exprgen;
					PX_SCRIPT_AST_OPERAND retOperand;
					PX_StringInitialize(analysis->mp,&code);
					PX_StringInitialize(analysis->mp,&exprgen);
					PX_StringFormat1(&code,"%1=\"\"",PX_STRINGFORMAT_STRING(variable.Mnemonic.buffer));
					if (!PX_ScriptParseExpression(analysis,code.buffer,&exprgen,&retOperand))
					{
						PX_StringFree(&code);
						PX_StringFree(&exprgen);
						return PX_FALSE;
					}
					PX_StringCat(&analysis->code,exprgen.buffer);
					PX_StringFree(&exprgen);
					PX_StringFree(&code);
				}

				if (variable.type==PX_SCRIPT_PARSER_VAR_TYPE_MEMORY)
				{
					px_string code;
					px_string exprgen;
					PX_SCRIPT_AST_OPERAND retOperand;
					PX_StringInitialize(analysis->mp,&code);
					PX_StringInitialize(analysis->mp,&exprgen);
					PX_StringFormat1(&code,"%1=@@",PX_STRINGFORMAT_STRING(variable.Mnemonic.buffer));
					if (!PX_ScriptParseExpression(analysis,code.buffer,&exprgen,&retOperand))
					{
						PX_StringFree(&code);
						PX_StringFree(&exprgen);
						return PX_FALSE;
					}
					PX_StringCat(&analysis->code,exprgen.buffer);
					PX_StringFree(&exprgen);
					PX_StringFree(&code);
				}

				if (variable.type==PX_SCRIPT_PARSER_VAR_TYPE_STRUCT)
				{
					px_string code;
					PX_SCRIPT_STRUCT *pset=PX_ScriptParseGetStructByIndex(analysis,variable.setIndex);
					PX_StringInitialize(analysis->mp,&code);
					if (pset)
					{
						px_int j;
						for (j=0;j<pset->members.size;j++)
						{
							pvar=PX_VECTORAT(PX_SCRIPT_VARIABLES,&pset->members,j);

							if (!pvar->bParam&&pvar->type==PX_SCRIPT_PARSER_VAR_TYPE_STRING)
							{
								PX_StringFormat1(&code,"MOV LOCAL[%1],\"\"\n",PX_STRINGFORMAT_INT(pvar->BeginIndex));
								PX_StringCat(&analysis->code,code.buffer);
							}
							if (!pvar->bParam&&pvar->type==PX_SCRIPT_PARSER_VAR_TYPE_MEMORY)
							{
								PX_StringFormat1(&code,"MOV LOCAL[%1],@@\n",PX_STRINGFORMAT_INT(pvar->BeginIndex));
								PX_StringCat(&analysis->code,code.buffer);
							}
						}
					}
					PX_StringFree(&code);
				}
			};
			//////////////////////////////////////////////////////////////////////////
		}
		else
		{
			for (i=0;i<analysis->v_variablesGlobalTable.size;i++)
			{
				if (PX_strequ(variable.Mnemonic.buffer,PX_VECTORAT(PX_SCRIPT_VARIABLES,&analysis->v_variablesGlobalTable,i)->Mnemonic.buffer))
				{
					PX_ScriptTranslatorError(&analysis->lexer,"variable redefined.");
					PX_StringFree(&variable.Mnemonic);
					if(variable.bInitialized)
						PX_StringFree(&variable.GlobalInitializeValue);
					goto _ERROR;
				}
			}

			if (analysis->v_variablesGlobalTable.size==0)
			{
				variable.BeginIndex=0;
			}
			else
			{
				pvar=PX_VECTORAT(PX_SCRIPT_VARIABLES,&analysis->v_variablesGlobalTable,analysis->v_variablesGlobalTable.size-1);
				variable.BeginIndex=pvar->BeginIndex+pvar->size;
			}


			PX_VectorPushback(&analysis->v_variablesGlobalTable,&variable);
		}

		if(analysis->lexer.Symbol==',')
		{
			continue;
		}
		else if(analysis->lexer.Symbol==';')
		{
			break;
		}
		else
		{
			PX_ScriptTranslatorError(&analysis->lexer,"Invalid var token");
			PX_StringFree(&variable.Mnemonic);
			goto _ERROR;
		}
	}
	
	return PX_TRUE;
_ERROR:
	return PX_FALSE;
}
static px_bool PX_ScriptParseStruct(PX_SCRIPT_Analysis *analysis)
{
	PX_LEXER_LEXEME_TYPE type;
	PX_SCRIPT_VARIABLES variable,*pvar;
	PX_SCRIPT_STRUCT *pset;
	PX_LEXER_STATE state;
	px_int i;
	variable.bParam=PX_FALSE;
	variable.bInitialized=PX_FALSE;
	variable.BeginIndex=-1;
	variable.size=-1;
	variable.type=PX_SCRIPT_PARSER_VAR_TYPE_STRUCT;


	type=PX_ScriptTranslatorNextToken(&analysis->lexer);

	if (type!=PX_LEXER_LEXEME_TYPE_TOKEN)
	{
		return PX_FALSE;
	}

	if (!PX_ScriptParseIsValidToken(analysis->lexer.CurLexeme.buffer))
	{
		PX_ScriptTranslatorError(&analysis->lexer,"Invalid struct name");
		goto _ERROR;
	}

	if (!(pset=PX_ScriptParseGetStructInfo(analysis,analysis->lexer.CurLexeme.buffer)))
	{
		PX_ScriptTranslatorError(&analysis->lexer,"Invalid struct type");
		goto _ERROR;
	}

	variable.setIndex=PX_ScriptParseGetSetIndex(analysis,analysis->lexer.CurLexeme.buffer);
	variable.layer=analysis->v_astStructure.size;

	while(PX_TRUE)
	{
		type=PX_ScriptTranslatorNextToken(&analysis->lexer);
		
		if (type==PX_LEXER_LEXEME_TYPE_DELIMITER&&analysis->lexer.Symbol=='*')
		{
			variable.size=1;
			variable.type=PX_SCRIPT_PARSER_VAR_TYPE_STRUCT_PTR;
			type=PX_ScriptTranslatorNextToken(&analysis->lexer);
		}

		if (type!=PX_LEXER_LEXEME_TYPE_TOKEN)
		{
			return PX_FALSE;
		}

		if (!PX_ScriptParseIsValidToken(analysis->lexer.CurLexeme.buffer))
		{
			PX_ScriptTranslatorError(&analysis->lexer,"Invalid struct name");
			goto _ERROR;
		}

		PX_StringInitialize(analysis->mp,&variable.Mnemonic);
		PX_StringCat(&variable.Mnemonic,analysis->lexer.CurLexeme.buffer);


		state=PX_LexerGetState(&analysis->lexer);

		type=PX_ScriptTranslatorNextToken(&analysis->lexer);

		if (type!=PX_LEXER_LEXEME_TYPE_DELIMITER)
		{
			PX_ScriptTranslatorError(&analysis->lexer,"Invalid var token");
			PX_StringFree(&variable.Mnemonic);
			goto _ERROR;
		}

		
		if (analysis->lexer.Symbol=='[')
		{
			if (variable.type==PX_SCRIPT_PARSER_VAR_TYPE_STRUCT_PTR)
			{
				PX_ScriptTranslatorError(&analysis->lexer,"Array of struct pointers is not support.");
				PX_StringFree(&variable.Mnemonic);
				goto _ERROR;
			}

			variable.type=PX_SCRIPT_PARSER_VAR_TYPE_SET_ARRAY;

			if(PX_ScriptTranslatorNextToken(&analysis->lexer)!=PX_LEXER_LEXEME_TYPE_TOKEN)
			{
				PX_ScriptTranslatorError(&analysis->lexer,"Invalid var token");
				PX_StringFree(&variable.Mnemonic);
				goto _ERROR;
			}

			if(!PX_strIsNumeric(analysis->lexer.CurLexeme.buffer))
			{
				PX_ScriptTranslatorError(&analysis->lexer,"Invalid var token");
			}


			if(variable.type==PX_SCRIPT_PARSER_VAR_TYPE_STRUCT_PTR)
				variable.size=PX_atoi(analysis->lexer.CurLexeme.buffer);
			else
				variable.size=PX_atoi(analysis->lexer.CurLexeme.buffer)*pset->size;

			if (variable.size==0)
			{
				PX_ScriptTranslatorError(&analysis->lexer,"Invalid array");
				PX_StringFree(&variable.Mnemonic);
				goto _ERROR;
			}
			type=PX_ScriptTranslatorNextToken(&analysis->lexer);

			if (type!=PX_LEXER_LEXEME_TYPE_DELIMITER||analysis->lexer.Symbol!=']')
			{
				PX_ScriptTranslatorError(&analysis->lexer,"Invalid var token");
				PX_StringFree(&variable.Mnemonic);
				goto _ERROR;
			}

			type=PX_ScriptTranslatorNextToken(&analysis->lexer);
			
		}
		else
		{
			if(variable.type==PX_SCRIPT_PARSER_VAR_TYPE_STRUCT_PTR)
			variable.size=1;
			else
			variable.size=pset->size;
		}


		if (analysis->functionInside)
		{
			PX_SCRIPT_VARIABLES *pvar;
			for (i=0;i<analysis->v_variablesStackTable.size;i++)
			{
				if (PX_strequ(variable.Mnemonic.buffer,PX_VECTORAT(PX_SCRIPT_VARIABLES,&analysis->v_variablesStackTable,i)->Mnemonic.buffer))
				{
					PX_ScriptTranslatorError(&analysis->lexer,"variable redefined.");
					PX_StringFree(&variable.Mnemonic);
					if(variable.bInitialized)
						PX_StringFree(&variable.GlobalInitializeValue);
					goto _ERROR;
				}
			}
			
			pvar=PX_VECTORLAST(PX_SCRIPT_VARIABLES,&analysis->v_variablesStackTable);
			variable.BeginIndex=pvar->BeginIndex+variable.size;
			
			if (variable.BeginIndex+variable.size-1>analysis->currentAllocStackSize)
			{
				analysis->currentAllocStackSize=variable.BeginIndex+variable.size-1;
			}

			PX_VectorPushback(&analysis->v_variablesStackTable,&variable);
		}
		else
		{
			for (i=0;i<analysis->v_variablesGlobalTable.size;i++)
			{
				if (PX_strequ(variable.Mnemonic.buffer,PX_VECTORAT(PX_SCRIPT_VARIABLES,&analysis->v_variablesGlobalTable,i)->Mnemonic.buffer))
				{
					PX_ScriptTranslatorError(&analysis->lexer,"variable redefined.");
					PX_StringFree(&variable.Mnemonic);
					if(variable.bInitialized)
						PX_StringFree(&variable.GlobalInitializeValue);
					goto _ERROR;
				}
			}

			if (analysis->v_variablesGlobalTable.size==0)
			{
				variable.BeginIndex=0;
			}
			else
			{
				pvar=PX_VECTORAT(PX_SCRIPT_VARIABLES,&analysis->v_variablesGlobalTable,analysis->v_variablesGlobalTable.size-1);
				variable.BeginIndex=pvar->BeginIndex+pvar->size;
			}

			
			PX_VectorPushback(&analysis->v_variablesGlobalTable,&variable);
		}


		if (type!=PX_LEXER_LEXEME_TYPE_DELIMITER)
		{
			PX_ScriptTranslatorError(&analysis->lexer,"\";\" is expected but not found.");
			PX_StringFree(&variable.Mnemonic);
			goto _ERROR;
		}

		if(analysis->lexer.Symbol==',')
		{
			continue;
		}
		else if(analysis->lexer.Symbol==';')
		{
			break;
		}
		else
		{
			PX_ScriptTranslatorError(&analysis->lexer,"\";\" is expected but not found.");
			PX_StringFree(&variable.Mnemonic);
			goto _ERROR;
		}
	}

	return PX_TRUE;
_ERROR:
	return PX_FALSE;
}
px_bool PX_ScriptParseStructDefine(PX_SCRIPT_Analysis *analysis)
{
	PX_SCRIPT_SETMEMBER member,*pmember;
	PX_SCRIPT_STRUCT vSet,*pSet;
	PX_LEXER_LEXEME_TYPE type;
	px_int i;

	member.defvar.BeginIndex=-1;

	vSet.size=0;

	if ((PX_ScriptTranslatorNextToken(&analysis->lexer))!=PX_LEXER_LEXEME_TYPE_TOKEN)
	{
		return PX_FALSE;
	}

	if (!PX_ScriptParseIsValidToken(analysis->lexer.CurLexeme.buffer))
	{
		return PX_FALSE;
	}
	
	PX_StringInitialize(analysis->mp,&vSet.Name);
	PX_StringCat(&vSet.Name,analysis->lexer.CurLexeme.buffer);
	PX_VectorInitialize(analysis->mp,&vSet.members,sizeof(PX_SCRIPT_SETMEMBER),1);

	if ((PX_ScriptTranslatorNextTokenSN(&analysis->lexer))!=PX_LEXER_LEXEME_TYPE_DELIMITER)
	{
		goto _ERROR;
	}
	if (analysis->lexer.Symbol!='{')
	{
		goto _ERROR;
	}

	while(PX_TRUE)
	{
		type=PX_ScriptTranslatorNextToken(&analysis->lexer);

		if (type==PX_LEXER_LEXEME_TYPE_NEWLINE)
		{
			continue;
		}

		if (type==PX_LEXER_LEXEME_TYPE_DELIMITER&&analysis->lexer.Symbol=='}')
		{
			break;
		}

		if (type!=PX_LEXER_LEXEME_TYPE_TOKEN)
		{
			goto _ERROR;
		}

		if((pSet=PX_ScriptParseGetStructInfo(analysis,analysis->lexer.CurLexeme.buffer))!=PX_NULL)
		{
				member.defvar.setIndex=PX_ScriptParseGetSetIndex(analysis,analysis->lexer.CurLexeme.buffer);

				//////////////////////////////////////////////////////////////////////////
				while (PX_TRUE)
				{
					type=PX_ScriptTranslatorNextToken(&analysis->lexer);

					if (type==PX_LEXER_LEXEME_TYPE_DELIMITER&&analysis->lexer.Symbol=='*')
					{
						member.defvar.type=PX_SCRIPT_PARSER_VAR_TYPE_STRUCT_PTR;
						type=PX_ScriptTranslatorNextToken(&analysis->lexer);
					}


					if (type!=PX_LEXER_LEXEME_TYPE_TOKEN)
					{
						goto _ERROR;
					}

					if (!PX_ScriptParseIsValidToken(analysis->lexer.CurLexeme.buffer))
					{
						PX_ScriptTranslatorError(&analysis->lexer,"Invalid var token");
						goto _ERROR;
					}

					PX_StringInitialize(analysis->mp,&member.defvar.Mnemonic);
					PX_StringCat(&member.defvar.Mnemonic,analysis->lexer.CurLexeme.buffer);
					
					type=PX_ScriptTranslatorNextToken(&analysis->lexer);

					if (type!=PX_LEXER_LEXEME_TYPE_DELIMITER)
					{
						PX_ScriptTranslatorError(&analysis->lexer,"Invalid var token");
						PX_StringFree(&member.defvar.Mnemonic);
						goto _ERROR;
					}


					if (analysis->lexer.Symbol=='[')
					{
						if (member.defvar.type==PX_SCRIPT_PARSER_VAR_TYPE_STRUCT_PTR)
						{
							PX_ScriptTranslatorError(&analysis->lexer,"Array of struct is not support.");
							PX_StringFree(&member.defvar.Mnemonic);
							goto _ERROR;
						}

						member.defvar.type=PX_SCRIPT_PARSER_VAR_TYPE_SET_ARRAY;

						if(PX_ScriptTranslatorNextToken(&analysis->lexer)!=PX_LEXER_LEXEME_TYPE_TOKEN)
						{
							PX_ScriptTranslatorError(&analysis->lexer,"Invalid var token");
							PX_StringFree(&member.defvar.Mnemonic);
							goto _ERROR;
						}

						if(!PX_strIsNumeric(analysis->lexer.CurLexeme.buffer))
						{
							PX_ScriptTranslatorError(&analysis->lexer,"Invalid var token");
							PX_StringFree(&member.defvar.Mnemonic);
							goto _ERROR;
						}

						member.offset=vSet.size;
						vSet.size+=PX_atoi(analysis->lexer.CurLexeme.buffer)*pSet->size;

						if (vSet.size==0)
						{
							PX_ScriptTranslatorError(&analysis->lexer,"Invalid array");
							PX_StringFree(&member.defvar.Mnemonic);
							goto _ERROR;
						}
						type=PX_ScriptTranslatorNextToken(&analysis->lexer);

						if (type!=PX_LEXER_LEXEME_TYPE_DELIMITER||analysis->lexer.Symbol!=']')
						{
							PX_ScriptTranslatorError(&analysis->lexer,"Invalid var token");
							PX_StringFree(&member.defvar.Mnemonic);
							goto _ERROR;
						}


						type=PX_ScriptTranslatorNextToken(&analysis->lexer);


						if (type!=PX_LEXER_LEXEME_TYPE_DELIMITER)
						{
							PX_ScriptTranslatorError(&analysis->lexer,"\";\" is expected but not found.");
							PX_StringFree(&member.defvar.Mnemonic);
							goto _ERROR;
						}

						if(analysis->lexer.Symbol==',')
						{
							PX_VectorPushback(&vSet.members,&member);
							continue;
						}
						else if(analysis->lexer.Symbol==';')
						{
							PX_VectorPushback(&vSet.members,&member);
							break;
						}
						else
						{
							PX_ScriptTranslatorError(&analysis->lexer,"\";\" is expected but not found.");
							PX_StringFree(&member.defvar.Mnemonic);
							goto _ERROR;
						}
						break;
					}
					else if(analysis->lexer.Symbol==';')
					{
						member.offset=vSet.size;
						member.defvar.type=PX_SCRIPT_PARSER_VAR_TYPE_STRUCT;
						vSet.size+=pSet->size;
						PX_VectorPushback(&vSet.members,&member);
						break;
					}
					else if(analysis->lexer.Symbol==',')
					{
						member.offset=vSet.size;
						member.defvar.type=PX_SCRIPT_PARSER_VAR_TYPE_STRUCT;
						vSet.size+=pSet->size;
						PX_VectorPushback(&vSet.members,&member);
						continue;
					}
					else
					{
						PX_ScriptTranslatorError(&analysis->lexer,"Invalid var token");
						PX_StringFree(&member.defvar.Mnemonic);
						goto _ERROR;
					}
				}
				//////////////////////////////////////////////////////////////////////////
			
		}
		else
		{
			if (type!=PX_LEXER_LEXEME_TYPE_TOKEN)
			{
				return PX_FALSE;
			}

			if (PX_strequ(analysis->lexer.CurLexeme.buffer,PX_SCRIPT_TRANSLATOR_KEYWORD_VAR_INT))
			{
				member.defvar.type=PX_SCRIPT_PARSER_VAR_TYPE_INT;
			}
			else if (PX_strequ(analysis->lexer.CurLexeme.buffer,PX_SCRIPT_TRANSLATOR_KEYWORD_VAR_FLOAT))
			{
				member.defvar.type=PX_SCRIPT_PARSER_VAR_TYPE_FLOAT;
			}
			else if (PX_strequ(analysis->lexer.CurLexeme.buffer,PX_SCRIPT_TRANSLATOR_KEYWORD_VAR_STRING))
			{
				member.defvar.type=PX_SCRIPT_PARSER_VAR_TYPE_STRING;
			}
			else if (PX_strequ(analysis->lexer.CurLexeme.buffer,PX_SCRIPT_TRANSLATOR_KEYWORD_VAR_MEMORY))
			{
				member.defvar.type=PX_SCRIPT_PARSER_VAR_TYPE_MEMORY;
			}
			else
			{
				goto _ERROR;
			}

			//////////////////////////////////////////////////////////////////////////
			while (PX_TRUE)
			{
				type=PX_ScriptTranslatorNextToken(&analysis->lexer);

				if (type==PX_LEXER_LEXEME_TYPE_DELIMITER&&analysis->lexer.Symbol=='*')
				{
					switch(member.defvar.type)
					{
					case PX_SCRIPT_PARSER_VAR_TYPE_INT:
						{
							member.defvar.type=PX_SCRIPT_PARSER_VAR_TYPE_INT_PTR;
						}
						break;
					case PX_SCRIPT_PARSER_VAR_TYPE_FLOAT:
						{
							member.defvar.type=PX_SCRIPT_PARSER_VAR_TYPE_FLOAT_PTR;
						}
						break;
					case PX_SCRIPT_PARSER_VAR_TYPE_STRING:
						{
							member.defvar.type=PX_SCRIPT_PARSER_VAR_TYPE_STRING_PTR;
						}
						break;
					case PX_SCRIPT_PARSER_VAR_TYPE_MEMORY:
						{
							member.defvar.type=PX_SCRIPT_PARSER_VAR_TYPE_MEMORY_PTR;
						}
						break;
					default:
						goto _ERROR;
					}

					type=PX_ScriptTranslatorNextToken(&analysis->lexer);
				}

				if (type!=PX_LEXER_LEXEME_TYPE_TOKEN)
				{
				goto _ERROR;
				}

				if (!PX_ScriptParseIsValidToken(analysis->lexer.CurLexeme.buffer))
				{
				PX_ScriptTranslatorError(&analysis->lexer,"Invalid var token");
				goto _ERROR;
				}

				PX_StringInitialize(analysis->mp,&member.defvar.Mnemonic);
				PX_StringCat(&member.defvar.Mnemonic,analysis->lexer.CurLexeme.buffer);
				member.defvar.bInitialized=PX_FALSE;
			
				type=PX_ScriptTranslatorNextToken(&analysis->lexer);

				if (type!=PX_LEXER_LEXEME_TYPE_DELIMITER)
				{
				PX_ScriptTranslatorError(&analysis->lexer,"Invalid var token");
				PX_StringFree(&member.defvar.Mnemonic);
				goto _ERROR;
				}


				if (analysis->lexer.Symbol=='[')
				{
					switch(member.defvar.type)
					{
					case PX_SCRIPT_PARSER_VAR_TYPE_INT:
						{
							member.defvar.type=PX_SCRIPT_PARSER_VAR_TYPE_INT_ARRAY;
						}
						break;
					case PX_SCRIPT_PARSER_VAR_TYPE_FLOAT:
						{
							member.defvar.type=PX_SCRIPT_PARSER_VAR_TYPE_FLOAT_ARRAY;
						}
						break;
					case PX_SCRIPT_PARSER_VAR_TYPE_STRING:
						{
							member.defvar.type=PX_SCRIPT_PARSER_VAR_TYPE_STRING_ARRAY;
						}
						break;
					case PX_SCRIPT_PARSER_VAR_TYPE_MEMORY:
						{
							member.defvar.type=PX_SCRIPT_PARSER_VAR_TYPE_MEMORY_ARRAY;
						}
						break;

					default:
						goto _ERROR;
					}

				if(PX_ScriptTranslatorNextToken(&analysis->lexer)!=PX_LEXER_LEXEME_TYPE_TOKEN)
				{
					PX_ScriptTranslatorError(&analysis->lexer,"Invalid var token");
					PX_StringFree(&member.defvar.Mnemonic);
					goto _ERROR;
				}

				if(!PX_strIsNumeric(analysis->lexer.CurLexeme.buffer))
				{
					PX_ScriptTranslatorError(&analysis->lexer,"Invalid var token");
					PX_StringFree(&member.defvar.Mnemonic);
					goto _ERROR;
				}

				member.offset=vSet.size;
				member.defvar.size=PX_atoi(analysis->lexer.CurLexeme.buffer);
				vSet.size+=PX_atoi(analysis->lexer.CurLexeme.buffer);

				if (member.defvar.size==0)
				{
					PX_ScriptTranslatorError(&analysis->lexer,"Invalid array");
					PX_StringFree(&member.defvar.Mnemonic);
					goto _ERROR;
				}
				type=PX_ScriptTranslatorNextToken(&analysis->lexer);

				if (type!=PX_LEXER_LEXEME_TYPE_DELIMITER||analysis->lexer.Symbol!=']')
				{
					PX_ScriptTranslatorError(&analysis->lexer,"Invalid var token");
					PX_StringFree(&member.defvar.Mnemonic);
					goto _ERROR;
				}


				type=PX_ScriptTranslatorNextToken(&analysis->lexer);

				if(analysis->lexer.Symbol=='=')
				{
					PX_ScriptTranslatorError(&analysis->lexer,"Initialize of array is not support.");
					PX_StringFree(&member.defvar.Mnemonic);
					goto _ERROR;
				}

				if (type!=PX_LEXER_LEXEME_TYPE_DELIMITER)
				{
					PX_ScriptTranslatorError(&analysis->lexer,"\";\" is expected but not found.");
					PX_StringFree(&member.defvar.Mnemonic);
					goto _ERROR;
				}

				if(analysis->lexer.Symbol==',')
				{
					PX_VectorPushback(&vSet.members,&member);
					continue;
				}
				else if(analysis->lexer.Symbol==';')
				{
					PX_VectorPushback(&vSet.members,&member);
					break;
				}
				else
				{
					PX_ScriptTranslatorError(&analysis->lexer,"\";\" is expected but not found.");
					PX_StringFree(&member.defvar.Mnemonic);
					PX_VectorPushback(&vSet.members,&member);
					goto _ERROR;
				}
				break;
				}
				else if(analysis->lexer.Symbol==';')
				{
				member.offset=vSet.size;
				member.defvar.size=1;
				member.defvar.setIndex=-1;
				vSet.size+=1;
			
				PX_VectorPushback(&vSet.members,&member);
				break;
				}
				else if(analysis->lexer.Symbol==',')
				{
				member.offset=vSet.size;
				member.defvar.size=1;
				member.defvar.setIndex=-1;
				vSet.size+=1;
				PX_VectorPushback(&vSet.members,&member);
				continue;
				}
				else
				{
				PX_ScriptTranslatorError(&analysis->lexer,"Invalid var token");
				PX_StringFree(&member.defvar.Mnemonic);
				goto _ERROR;
				}
			}
			//////////////////////////////////////////////////////////////////////////
		}
		
	}
	if (vSet.size==0)
	{
		PX_ScriptTranslatorError(&analysis->lexer,"Zero size of struct");
		goto _ERROR;
	}
	PX_VectorPushback(&analysis->v_struct,&vSet);
	return PX_TRUE;
_ERROR:
	PX_StringFree(&vSet.Name);
	PX_VectorFree(&vSet.members);
	for (i=0;i<vSet.members.size;i++)
	{
		pmember=PX_VECTORAT(PX_SCRIPT_SETMEMBER,&vSet.members,i);
		PX_StringFree(&pmember->defvar.Mnemonic);
		if (pmember->defvar.bInitialized)
		{
			PX_StringFree(&pmember->defvar.GlobalInitializeValue);
		}
	}
	PX_VectorFree(&vSet.members);
	return PX_FALSE;
}
px_bool PX_ScriptParseFunctionDefined(PX_SCRIPT_Analysis *analysis,PX_SCRIPT_TRANSLATOR_FUNCTION_TYPE functype)
{
	PX_SCRIPT_FUNCTION func,*pfunc=PX_NULL;
	PX_SCRIPT_VARIABLES fvar;
	px_int count=0,i=0,j=0;
	PX_LEXER_LEXEME_TYPE type;
	fvar.bParam=PX_TRUE;
	func.retSetIndex=-1;
	type=PX_ScriptTranslatorNextToken(&analysis->lexer);
	if (type!=PX_LEXER_LEXEME_TYPE_TOKEN)
	{
		return PX_FALSE;
	}

	if (PX_strequ(analysis->lexer.CurLexeme.buffer,PX_SCRIPT_TRANSLATOR_KEYWORD_VAR_INT))
	{
		func.retType=PX_SCRIPT_PARSER_VAR_TYPE_INT;
	}
	else if (PX_strequ(analysis->lexer.CurLexeme.buffer,PX_SCRIPT_TRANSLATOR_KEYWORD_VAR_FLOAT))
	{
		func.retType=PX_SCRIPT_PARSER_VAR_TYPE_FLOAT;
	}
	else if (PX_strequ(analysis->lexer.CurLexeme.buffer,PX_SCRIPT_TRANSLATOR_KEYWORD_VAR_STRING))
	{
		func.retType=PX_SCRIPT_PARSER_VAR_TYPE_STRING;
	}
	else if (PX_strequ(analysis->lexer.CurLexeme.buffer,PX_SCRIPT_TRANSLATOR_KEYWORD_VAR_MEMORY))
	{
		func.retType=PX_SCRIPT_PARSER_VAR_TYPE_MEMORY;
	}
	else if (PX_strequ(analysis->lexer.CurLexeme.buffer,PX_SCRIPT_TRANSLATOR_KEYWORD_VAR_VOID))
	{
		func.retType=PX_SCRIPT_PARSER_VAR_TYPE_VOID;
	}
	else if ((func.retSetIndex=PX_ScriptParseGetSetIndex(analysis,analysis->lexer.CurLexeme.buffer))!=-1)
	{
		func.retType=PX_SCRIPT_PARSER_VAR_TYPE_STRUCT;
	}
	else
	{
		goto _ERROR;
	}

	//function Name
	PX_ScriptParserClearStack(analysis);
	func.parametersSize=0;
	
	type=PX_ScriptTranslatorNextToken(&analysis->lexer);

	if (type==PX_LEXER_LEXEME_TYPE_DELIMITER&&analysis->lexer.Symbol=='*')
	{
		switch(func.retType)
		{
			case PX_SCRIPT_PARSER_VAR_TYPE_INT:
				func.retType=PX_SCRIPT_PARSER_VAR_TYPE_INT_PTR;
				break;
			case PX_SCRIPT_PARSER_VAR_TYPE_FLOAT:
				func.retType=PX_SCRIPT_PARSER_VAR_TYPE_FLOAT_PTR;
				break;
			case PX_SCRIPT_PARSER_VAR_TYPE_STRING:
				func.retType=PX_SCRIPT_PARSER_VAR_TYPE_STRING_PTR;
				break;
			case PX_SCRIPT_PARSER_VAR_TYPE_MEMORY:
				func.retType=PX_SCRIPT_PARSER_VAR_TYPE_MEMORY_PTR;
				break;
			case PX_SCRIPT_PARSER_VAR_TYPE_STRUCT:
				func.retType=PX_SCRIPT_PARSER_VAR_TYPE_STRUCT_PTR;
				break;
			default:
				goto _ERROR;
		}
		type=PX_ScriptTranslatorNextToken(&analysis->lexer);
	}
	if (func.retType==PX_SCRIPT_PARSER_VAR_TYPE_STRUCT)
	{
		PX_ScriptTranslatorError(&analysis->lexer,"Could not return a struct-type");
		goto _ERROR;
	}
	if (type!=PX_LEXER_LEXEME_TYPE_TOKEN)
	{
		return PX_FALSE;
	}
	if (!PX_ScriptParseIsValidToken(analysis->lexer.CurLexeme.buffer))
	{
		return PX_FALSE;
	}

	if (PX_StringLen(&analysis->lexer.CurLexeme)>PX_SCRIPT_FUNCTION_NAME_MAX_LEN)
	{
		return PX_FALSE;
	}
	PX_strcpy(func.name,analysis->lexer.CurLexeme.buffer,PX_SCRIPT_FUNCTION_NAME_MAX_LEN);
	if ((PX_ScriptTranslatorNextToken(&analysis->lexer))!=PX_LEXER_LEXEME_TYPE_DELIMITER||analysis->lexer.Symbol!='(')
	{
		goto _ERROR;
	}

	while (PX_TRUE)
	{

		type=PX_ScriptTranslatorNextToken(&analysis->lexer);
		if (type==PX_LEXER_LEXEME_TYPE_DELIMITER&&analysis->lexer.Symbol==')')
		{
			break;
		}
		if (type!=PX_LEXER_LEXEME_TYPE_TOKEN)
		{
			goto _ERROR;
		}
		fvar.layer=analysis->v_astStructure.size;
		fvar.size=1;
		//////////////////////////////////////////////////////////////////////////
		if (PX_strequ(analysis->lexer.CurLexeme.buffer,PX_SCRIPT_TRANSLATOR_KEYWORD_VAR_INT))
		{
			fvar.type=PX_SCRIPT_PARSER_VAR_TYPE_INT;
		}
		else if (PX_strequ(analysis->lexer.CurLexeme.buffer,PX_SCRIPT_TRANSLATOR_KEYWORD_VAR_FLOAT))
		{
			fvar.type=PX_SCRIPT_PARSER_VAR_TYPE_FLOAT;
		}
		else if (PX_strequ(analysis->lexer.CurLexeme.buffer,PX_SCRIPT_TRANSLATOR_KEYWORD_VAR_STRING))
		{
			fvar.type=PX_SCRIPT_PARSER_VAR_TYPE_STRING;
		}
		else if (PX_strequ(analysis->lexer.CurLexeme.buffer,PX_SCRIPT_TRANSLATOR_KEYWORD_VAR_MEMORY))
		{
			fvar.type=PX_SCRIPT_PARSER_VAR_TYPE_MEMORY;
		}
		else if (PX_ScriptParseGetStructInfo(analysis,analysis->lexer.CurLexeme.buffer))
		{
			fvar.type=PX_SCRIPT_PARSER_VAR_TYPE_STRUCT;

			if ((fvar.setIndex=PX_ScriptParseGetSetIndex(analysis,analysis->lexer.CurLexeme.buffer))==-1)
			{
				goto _ERROR;
			}
		}
		else
		{
			goto _ERROR;
		}
		
		//////////////////////////////////////////////////////////////////////////
		
		type=PX_ScriptTranslatorNextToken(&analysis->lexer);
		
		if (type==PX_LEXER_LEXEME_TYPE_DELIMITER&&analysis->lexer.Symbol=='*')
		{
			switch(fvar.type)
			{
			case PX_SCRIPT_PARSER_VAR_TYPE_INT:
				fvar.type=PX_SCRIPT_PARSER_VAR_TYPE_INT_PTR;
				break;
			case PX_SCRIPT_PARSER_VAR_TYPE_FLOAT:
				fvar.type=PX_SCRIPT_PARSER_VAR_TYPE_FLOAT_PTR;
				break;
			case PX_SCRIPT_PARSER_VAR_TYPE_STRING:
				fvar.type=PX_SCRIPT_PARSER_VAR_TYPE_STRING_PTR;
				break;
			case PX_SCRIPT_PARSER_VAR_TYPE_MEMORY:
				fvar.type=PX_SCRIPT_PARSER_VAR_TYPE_MEMORY_PTR;
				break;
			case PX_SCRIPT_PARSER_VAR_TYPE_STRUCT:
				fvar.type=PX_SCRIPT_PARSER_VAR_TYPE_STRUCT_PTR;
				break;
			default:
				goto _ERROR;
			}
			type=PX_ScriptTranslatorNextToken(&analysis->lexer);
		}
		
		if (fvar.type==PX_SCRIPT_PARSER_VAR_TYPE_STRUCT)
		{
			PX_ScriptTranslatorError(&analysis->lexer,"Could not use a struct for parameter.consider use a struct pointer.");
			goto _ERROR;
		}

		if (type!=PX_LEXER_LEXEME_TYPE_TOKEN)
		{
			goto _ERROR;
		}
		if (!PX_ScriptParseIsValidToken(analysis->lexer.CurLexeme.buffer))
		{
			goto _ERROR;
		}

		fvar.bInitialized=PX_FALSE;
		if (analysis->v_variablesStackTable.size==0)
		{
			fvar.BeginIndex=-2;
		}
		else
		{
			PX_SCRIPT_VARIABLES *plastvar=PX_VECTORLAST(PX_SCRIPT_VARIABLES,&analysis->v_variablesStackTable);
			fvar.BeginIndex=plastvar->BeginIndex-plastvar->size;
		}
		

		PX_StringInitialize(analysis->mp,&fvar.Mnemonic);
		PX_StringCat(&fvar.Mnemonic,analysis->lexer.CurLexeme.buffer);
	
		
		if (count>=PX_SCRIPT_FUNCTION_MAX_PARAM)
		{
			goto _ERROR;
		}

		func.parameters[count]=fvar;
		PX_VectorPushback(&analysis->v_variablesStackTable,&fvar);
		count++;
		
		if ((PX_ScriptTranslatorNextToken(&analysis->lexer))!=PX_LEXER_LEXEME_TYPE_DELIMITER||analysis->lexer.Symbol!=',')
		{
			if (analysis->lexer.Symbol==')')
			{
				break;
			}
			goto _ERROR;
		}
	}

	do 
	{
		PX_SCRIPT_VARIABLES _ret_bp;
		PX_memset(&_ret_bp,0,sizeof(_ret_bp));
		_ret_bp.BeginIndex=0;
		_ret_bp.bInitialized=PX_FALSE;
		_ret_bp.bParam=PX_TRUE;
		PX_StringInitialize(analysis->mp,&_ret_bp.Mnemonic);
		_ret_bp.size=2;
		_ret_bp.type=PX_SCRIPT_PARSER_VAR_TYPE_INT_ARRAY;
		PX_VectorPushback(&analysis->v_variablesStackTable,&_ret_bp);
	} while (0);


	func.parametersCount=count;
	func.type=functype;
	
	for (i=0;i<analysis->v_functions.size;i++)
	{
		pfunc=PX_VECTORAT(PX_SCRIPT_FUNCTION,&analysis->v_functions,i);
		if (PX_strequ(pfunc->name,func.name))
		{
			if (pfunc->parametersCount!=func.parametersCount)
			{
				PX_ScriptTranslatorError(&analysis->lexer,"Function redefined error.");
				goto _ERROR;
			}
			for (j=0;j<func.parametersCount;j++)
			{
				if (func.parameters[j].type!=pfunc->parameters[j].type)
				{
					PX_ScriptTranslatorError(&analysis->lexer,"Function parameters are not compatible");
					goto _ERROR;
				}
			}
			analysis->currentFunc=*pfunc;
			return PX_TRUE;
		}
	}
	for (i=0;i<func.parametersCount;i++)
	{
		func.parametersSize+=func.parameters[i].size;
	}
	analysis->currentFunc=func;
	PX_VectorPushback(&analysis->v_functions,&func);
	return PX_TRUE;
_ERROR:
	return PX_FALSE;
}
px_bool PX_ScriptParseFunctionGuiderCode(PX_SCRIPT_Analysis *analysis)
{
	int stacksize;
	px_string code;

	px_string guiderCode;

	if(!PX_StringInitialize(analysis->mp,&guiderCode))return PX_FALSE;

	stacksize=analysis->currentAllocStackSize;
	
	if (analysis->currentFunc.type==PX_SCRIPT_TRANSLATOR_FUNCTION_TYPE_EXPORT)
	{
		PX_StringCat(&guiderCode,"EXPORT ");
	}

	PX_StringCat(&guiderCode,"FUNCTION ");
	PX_StringCat(&guiderCode,analysis->currentFunc.name);
	PX_StringCat(&guiderCode,":\n");

	//generate codes
	PX_StringInitialize(analysis->mp,&code);

	PX_StringCat(&guiderCode,"PUSH BP\nMOV BP,SP\n");

	if(stacksize!=0)
	{
	PX_StringFormat1(&code,"SUB SP,%1\n",PX_STRINGFORMAT_INT(stacksize));
	PX_StringCat(&guiderCode,code.buffer);
	}
	
	PX_StringFree(&code);
	

	if (analysis->functionguider!=-1)
	{
		if(!PX_StringInsert(&analysis->code,analysis->functionguider,guiderCode.buffer))return PX_FALSE;
	}
	else
	{
		PX_ASSERT();
		PX_StringFree(&guiderCode);
		return PX_FALSE;
	}
	
	analysis->functionguider=-1;
	PX_StringFree(&guiderCode);
	return PX_TRUE;
}
px_bool PX_ScriptParseBootCode(PX_SCRIPT_Analysis *analysis)
{
	px_int i;
	PX_SCRIPT_VARIABLES *pvar;
	px_string code;
	px_string exprgen;
	PX_SCRIPT_AST_OPERAND retOperand;
	for (i=0;i<analysis->v_variablesGlobalTable.size;i++)
	{
		pvar=PX_VECTORAT(PX_SCRIPT_VARIABLES,&analysis->v_variablesGlobalTable,i);
		if(pvar->bInitialized)
		{
			PX_StringInitialize(analysis->mp,&code);
			PX_StringInitialize(analysis->mp,&exprgen);
			PX_StringFormat2(&code,"%1=%2\n",PX_STRINGFORMAT_STRING(pvar->Mnemonic.buffer),PX_STRINGFORMAT_STRING(pvar->GlobalInitializeValue.buffer));
			if (!PX_ScriptParseExpression(analysis,code.buffer,&exprgen,&retOperand))
			{
				PX_StringFree(&code);
				PX_StringFree(&exprgen);
				return PX_FALSE;
			}
			PX_StringCat(&analysis->bootCode,exprgen.buffer);
			PX_StringFree(&exprgen);
			PX_StringFree(&code);
		}
	}
	return PX_TRUE;
}
px_bool PX_ScriptParseFunctionReturn(PX_SCRIPT_Analysis *analysis)
{
	px_string code;
	int stacksize=0;
	PX_StringInitialize(analysis->mp,&code);

	stacksize=analysis->currentAllocStackSize;
	
	PX_StringCat(&analysis->code,"_");
	PX_StringCat(&analysis->code,analysis->currentFunc.name);
	PX_StringCat(&analysis->code,"_RET:\n");

	if(stacksize!=0)
	{
		PX_StringFormat1(&code,"POPN %1\n",PX_STRINGFORMAT_INT(stacksize));
		PX_StringCat(&analysis->code,code.buffer);
	}
	PX_StringCat(&analysis->code,"POP BP\nRET\n");
	PX_StringFree(&code);
	return PX_TRUE;
}
px_bool PX_ScriptParsePopAstStructure(PX_SCRIPT_Analysis *analysis)
{
	PX_SCRIPT_AST_STRUCTURE *plast;
	if (analysis->v_astStructure.size==0)
	{
		return PX_FALSE;
	}
	plast=PX_VECTORLAST(PX_SCRIPT_AST_STRUCTURE,&analysis->v_astStructure);
	PX_VectorPop(&analysis->v_astStructure);
	return PX_TRUE;
}
px_bool PX_ScriptParseLastCodeblockEnd(PX_SCRIPT_Analysis *analysis);

px_bool PX_ScriptParseLastBlockEnd(PX_SCRIPT_Analysis *analysis)
{
	PX_SCRIPT_AST_STRUCTURE astStruct;

	astStruct=*PX_VECTORLAST(PX_SCRIPT_AST_STRUCTURE,&analysis->v_astStructure);
	switch(astStruct.type)
	{
	case PX_SCRIPT_AST_STRUCTURE_TYPE_IF:
		{
			if (astStruct._if.oneline_expr)
			{
				return PX_ScriptParseLastCodeblockEnd(analysis);
			}
		}
		break;
	case PX_SCRIPT_AST_STRUCTURE_TYPE_ELSE:
		{
			if (astStruct._else.oneline_expr)
			{
				return PX_ScriptParseLastCodeblockEnd(analysis);
			}
		}
		break;
	case PX_SCRIPT_AST_STRUCTURE_TYPE_FOR:
		{
			if (astStruct._for.oneline_expr)
			{
				return PX_ScriptParseLastCodeblockEnd(analysis);
			}
		}
		break;
	case PX_SCRIPT_AST_STRUCTURE_TYPE_WHILE:
		{
			if (astStruct._while.oneline_expr)
			{
				return PX_ScriptParseLastCodeblockEnd(analysis);
			}
		}
		break;
	case PX_SCRIPT_AST_STRUCTURE_TYPE_SWITCH:
		{
			if (astStruct._compare.oneline_expr)
			{
				return PX_ScriptParseLastCodeblockEnd(analysis);
			}
		}
		break;
	case PX_SCRIPT_AST_STRUCTURE_TYPE_CASE:
		{
			if (astStruct._with.oneline_expr)
			{
				return PX_ScriptParseLastCodeblockEnd(analysis);
			}
		}
		break;
	case PX_SCRIPT_AST_STRUCTURE_TYPE_FUNCTION:
		break;
	default:
			return PX_FALSE;
	}
	return PX_TRUE;
}
px_bool PX_ScriptParseLastCodeblockEnd(PX_SCRIPT_Analysis *analysis)
{
	PX_LEXER_STATE state;
	PX_LEXER_LEXEME_TYPE type;
	PX_SCRIPT_AST_STRUCTURE buildastStruct;
	PX_SCRIPT_AST_STRUCTURE astStruct;
	px_string fmrString;
	px_int layer=analysis->v_astStructure.size;

	if (analysis->v_astStructure.size==0)
	{
		return PX_FALSE;
	}

	astStruct=*PX_VECTORLAST(PX_SCRIPT_AST_STRUCTURE,&analysis->v_astStructure);

	if(!PX_ScriptParsePopAstStructure(analysis))return PX_FALSE;

	switch(astStruct.type)
	{
	case PX_SCRIPT_AST_STRUCTURE_TYPE_FUNCTION:
		{
			//////////////////////////////////////////////////////////////////////////
			//Function Guider
			//////////////////////////////////////////////////////////////////////////
			if(!PX_ScriptParseFunctionGuiderCode(analysis)) return PX_FALSE;


			//////////////////////////////////////////////////////////////////////////
			//Function Clear
			if(!PX_ScriptParseFunctionReturn(analysis)) return PX_FALSE;
			
			analysis->functionInside=PX_FALSE;
		}
		break;
	case PX_SCRIPT_AST_STRUCTURE_TYPE_IF:
		{
			//
			//if()
			//{
			//}<---here
			
			//else
			state=PX_LexerGetState(&analysis->lexer);
			type=PX_ScriptTranslatorNextTokenSN(&analysis->lexer);
			if (type==PX_LEXER_LEXEME_TYPE_TOKEN&&PX_strequ(analysis->lexer.CurLexeme.buffer,PX_SCRIPT_TRANSLATOR_KEYWORD_ELSE))
			{
				//generate code
				PX_StringInitialize(analysis->mp,&fmrString);
				PX_StringFormat1(&fmrString,"JMP _ELSE_%1\n",PX_STRINGFORMAT_INT(astStruct._if.elseflag));
				PX_StringCat(&analysis->code,fmrString.buffer);
				PX_StringFree(&fmrString);

				buildastStruct.type=PX_SCRIPT_AST_STRUCTURE_TYPE_ELSE;
				buildastStruct._else.elseflag=astStruct._if.elseflag;

				state=PX_LexerGetState(&analysis->lexer);
				type=PX_ScriptTranslatorNextTokenSN(&analysis->lexer);
				if (type==PX_LEXER_LEXEME_TYPE_DELIMITER&&analysis->lexer.Symbol=='{')
				{
					buildastStruct._else.oneline_expr=PX_FALSE;
				}
				else
				{
					PX_LexerSetState(state);
					buildastStruct._else.oneline_expr=PX_TRUE;
				}
				PX_VectorPushback(&analysis->v_astStructure,&buildastStruct);

				PX_StringInitialize(analysis->mp,&fmrString);
				PX_StringFormat1(&fmrString,"_IF_%1:\n",PX_STRINGFORMAT_INT(astStruct._if.ifflag));
				PX_StringCat(&analysis->code,fmrString.buffer);
				PX_StringFree(&fmrString);
			}
			else
			{
				PX_LexerSetState(state);

				PX_StringInitialize(analysis->mp,&fmrString);
				PX_StringFormat1(&fmrString,"_IF_%1:\n",PX_STRINGFORMAT_INT(astStruct._if.ifflag));
				PX_StringCat(&analysis->code,fmrString.buffer);
				PX_StringFree(&fmrString);

				if (!PX_ScriptParseLastBlockEnd(analysis))
				{
					return PX_FALSE;
				}
				
			}

		}
		break;
	case PX_SCRIPT_AST_STRUCTURE_TYPE_ELSE:
		{
			//
			PX_StringInitialize(analysis->mp,&fmrString);
			PX_StringFormat1(&fmrString,"_ELSE_%1:\n",PX_STRINGFORMAT_INT(astStruct._else.elseflag));
			PX_StringCat(&analysis->code,fmrString.buffer);
			PX_StringFree(&fmrString);

			if (!PX_ScriptParseLastBlockEnd(analysis))
			{
				return PX_FALSE;
			}
		}
		break;
	case PX_SCRIPT_AST_STRUCTURE_TYPE_WHILE:
		{
			PX_StringInitialize(analysis->mp,&fmrString);
			PX_StringFormat1(&fmrString,"JMP _WHILE_%1\n",PX_STRINGFORMAT_INT(astStruct._while.loopflag));
			PX_StringCat(&analysis->code,fmrString.buffer);
			PX_StringFormat1(&fmrString,"_WHILE_%1:\n",PX_STRINGFORMAT_INT(astStruct._while.endflag));
			PX_StringCat(&analysis->code,fmrString.buffer);
			PX_StringFree(&fmrString);

			if (!PX_ScriptParseLastBlockEnd(analysis))
			{
				return PX_FALSE;
			}
		}
		break;
	case PX_SCRIPT_AST_STRUCTURE_TYPE_FOR:
		{
			PX_StringInitialize(analysis->mp,&fmrString);
			PX_StringFormat1(&fmrString,"JMP _FOR_%1\n",PX_STRINGFORMAT_INT(astStruct._for.additionFlag));
			PX_StringCat(&analysis->code,fmrString.buffer);
			//endflag
			PX_StringFormat1(&fmrString,"_FOR_%1:\n",PX_STRINGFORMAT_INT(astStruct._for.endFlag));
			PX_StringCat(&analysis->code,fmrString.buffer);
			PX_StringFree(&fmrString);

			if (!PX_ScriptParseLastBlockEnd(analysis))
			{
				return PX_FALSE;
			}
		}
		break;

	case PX_SCRIPT_AST_STRUCTURE_TYPE_SWITCH:
		{
			PX_StringInitialize(analysis->mp,&fmrString);
			//endflag
			PX_StringFormat1(&fmrString,"_COMPARE_%1:\n",PX_STRINGFORMAT_INT(astStruct._compare.endFlag));
			PX_StringCat(&analysis->code,fmrString.buffer);
			//pop 
			PX_StringSet(&fmrString,"POP R1\n");
			PX_StringCat(&analysis->code,fmrString.buffer);
			
			PX_StringFree(&fmrString);

			if (!PX_ScriptParseLastBlockEnd(analysis))
			{
				return PX_FALSE;
			}
		}
		break;

	case PX_SCRIPT_AST_STRUCTURE_TYPE_CASE:
		{
			PX_StringInitialize(analysis->mp,&fmrString);
			//endflag
			PX_StringFormat1(&fmrString,"_WITH_%1:\n",PX_STRINGFORMAT_INT(astStruct._compare.endFlag));
			PX_StringCat(&analysis->code,fmrString.buffer);

			PX_StringFree(&fmrString);

			if (!PX_ScriptParseLastBlockEnd(analysis))
			{
				return PX_FALSE;
			}
		}
		break;
	default:
		PX_ERROR("Assert Error:FSM Error");
	}

	//clear stack
	while (analysis->v_variablesStackTable.size)
	{
		PX_SCRIPT_VARIABLES *pvar=PX_VECTORLAST(PX_SCRIPT_VARIABLES,&analysis->v_variablesStackTable);
		if (pvar->layer>=layer)
		{
			if(pvar->bInitialized)
				PX_StringFree(&pvar->GlobalInitializeValue);

			PX_StringFree(&pvar->Mnemonic);

			PX_VectorPop(&analysis->v_variablesStackTable);
		}
		else
		{
			break;
		}
	}


	return PX_TRUE;
}
px_bool PX_ScriptParseIfLastAST(PX_SCRIPT_Analysis *analysis)
{
	PX_SCRIPT_AST_STRUCTURE astStruct;
	

	if(analysis->v_astStructure.size)
	{
		astStruct=*PX_VECTORLAST(PX_SCRIPT_AST_STRUCTURE,&analysis->v_astStructure);
		switch(astStruct.type)
		{
		case PX_SCRIPT_AST_STRUCTURE_TYPE_IF:
		{
			if (astStruct._if.oneline_expr)
			{
				return PX_ScriptParseLastCodeblockEnd(analysis);
			}
		}
		break;
		case PX_SCRIPT_AST_STRUCTURE_TYPE_ELSE:
		{
			if (astStruct._else.oneline_expr)
			{
				return PX_ScriptParseLastCodeblockEnd(analysis);
			}
		}
		break;
		case PX_SCRIPT_AST_STRUCTURE_TYPE_WHILE:
			{
				if (astStruct._while.oneline_expr)
				{
					return PX_ScriptParseLastCodeblockEnd(analysis);
				}
			}
			break;
		case PX_SCRIPT_AST_STRUCTURE_TYPE_FOR:
			{
				if (astStruct._for.oneline_expr)
				{
					return PX_ScriptParseLastCodeblockEnd(analysis);
				}
			}
			break;
		case PX_SCRIPT_AST_STRUCTURE_TYPE_SWITCH:
			{
				if (astStruct._compare.oneline_expr)
				{
					PX_ScriptTranslatorError(&analysis->lexer,"expression in useless compared.");
					return PX_FALSE;
				}
			}
			break;
		case PX_SCRIPT_AST_STRUCTURE_TYPE_CASE:
			{
				if (astStruct._with.oneline_expr)
				{
					return PX_ScriptParseLastCodeblockEnd(analysis);
				}
			}
			break;
			default:
				break;
		}
	}
	else
		return PX_FALSE;

	return PX_TRUE;
}
px_bool PX_ScriptParseGetExpression(PX_SCRIPT_Analysis *analysis,px_string *expr,px_char end)
{
	int mBracket=0,lBracket=0;
	PX_LEXER_LEXEME_TYPE type;

	PX_StringClear(expr);

	while (PX_TRUE)
	{
		type=PX_ScriptTranslatorNextToken(&analysis->lexer);

		if (type==PX_LEXER_LEXEME_TYPE_NEWLINE)
		{
			continue;
		}
		if (type==PX_LEXER_LEXEME_TYPE_END)
		{
			return PX_FALSE;
		}
		if (type==PX_LEXER_LEXEME_TYPE_DELIMITER&&analysis->lexer.Symbol==end&&!mBracket&&!lBracket)
		{
			break;
		}

		if (analysis->lexer.Symbol=='(')
		{
			lBracket++;
		}
		if (analysis->lexer.Symbol==')')
		{
			lBracket--;
			if(lBracket<0)
				return PX_FALSE;
		}
		if (analysis->lexer.Symbol=='[')
		{
			mBracket++;
		}
		if (analysis->lexer.Symbol==']')
		{
			mBracket--;
			if(mBracket<0)
				return PX_FALSE;
		}

		if (type==PX_LEXER_LEXEME_TYPE_SPACER)
		{
			continue;
		}

		PX_StringCat(expr,analysis->lexer.CurLexeme.buffer);
	}
	return PX_TRUE;
}
px_bool PX_ScriptCompilerCompile(PX_SCRIPT_LIBRARY *lib,const px_char *name,px_string *ASM,px_int LocalStackSize)
{
	PX_SCRIPT_SETMEMBER *psetmem;
	PX_SCRIPT_STRUCT *pset;
	px_string codes;
	PX_LEXER_STATE state;
	px_int i,j,globalSize;
	px_uint quotes;
	PX_LEXER_LEXEME_TYPE type;
	PX_SCRIPT_Analysis analysis;
	PX_SCRIPT_AST_STRUCTURE buildAstStruct;
	PX_SCRIPT_AST_STRUCTURE astStruct;
	px_string expression,fmrString,condCodes;
	px_string expCode;
	px_bool hasLastReturn=PX_FALSE;
	PX_SCRIPT_AST_OPERAND retOperand;
	int mBracket=0,lBracket=0;
	PX_SCRIPT_VARIABLES *pvar;
	px_int thread=1;
	px_bool caseEnd;
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	PX_memset(&astStruct, 0, sizeof(&astStruct));
	PX_StringInitialize(lib->mp,&codes);

	for (i=0;i<lib->codeLibraries.size;i++)
	{
		if (PX_strequ(name,PX_VECTORAT(PX_SCRIPT_CODE,&lib->codeLibraries,i)->name.buffer))
		{
			PX_VECTORAT(PX_SCRIPT_CODE,&lib->codeLibraries,i)->bInclude=PX_TRUE;
			PX_StringCopy(&codes,&PX_VECTORAT(PX_SCRIPT_CODE,&lib->codeLibraries,i)->code);
			break;
		}
	}
	
	if (i==lib->codeLibraries.size)
	{
		PX_ScriptTranslatorError(PX_NULL,"Code of name was not existed");
		return PX_FALSE;
	}

	if(!PX_ScriptParsePretreatment(&codes,lib,name))
		return PX_FALSE;

	PX_memset(&analysis,0,sizeof(analysis));

	analysis.functionInside=PX_FALSE;
	analysis.functionReturn=PX_FALSE;
	analysis._jFlag=0;

	PX_LexerInitialize(&analysis.lexer,lib->mp);
	PX_LexerRegisterDelimiter(&analysis.lexer,',');
	PX_LexerRegisterDelimiter(&analysis.lexer,';');
	PX_LexerRegisterDelimiter(&analysis.lexer,'+');
	PX_LexerRegisterDelimiter(&analysis.lexer,'-');
	PX_LexerRegisterDelimiter(&analysis.lexer,'*');
	PX_LexerRegisterDelimiter(&analysis.lexer,'/');
	PX_LexerRegisterDelimiter(&analysis.lexer,'%');
	PX_LexerRegisterDelimiter(&analysis.lexer,'&');
	PX_LexerRegisterDelimiter(&analysis.lexer,'^');
	PX_LexerRegisterDelimiter(&analysis.lexer,'~');
	PX_LexerRegisterDelimiter(&analysis.lexer,'(');
	PX_LexerRegisterDelimiter(&analysis.lexer,')');
	PX_LexerRegisterDelimiter(&analysis.lexer,'!');
	PX_LexerRegisterDelimiter(&analysis.lexer,'=');
	PX_LexerRegisterDelimiter(&analysis.lexer,'>');
	PX_LexerRegisterDelimiter(&analysis.lexer,'<');
	PX_LexerRegisterDelimiter(&analysis.lexer,'{');
	PX_LexerRegisterDelimiter(&analysis.lexer,'}');
	PX_LexerRegisterDelimiter(&analysis.lexer,'[');
	PX_LexerRegisterDelimiter(&analysis.lexer,']');
	PX_LexerRegisterSpacer(&analysis.lexer,' ');
	PX_LexerRegisterSpacer(&analysis.lexer,'\t');
	quotes=PX_LexerRegisterContainer(&analysis.lexer,"\"","\"");
	PX_LexerRegisterContainerTransfer(&analysis.lexer,quotes,'\\');
	PX_LexerRegisterContainer(&analysis.lexer,"\'","\'");
	PX_LexerRegisterContainer(&analysis.lexer,"@","@");
	PX_LexerSetTokenCase(&analysis.lexer,PX_LEXER_LEXEME_CASE_UPPER);


	PX_LexerLoadSourceFromMemory(&analysis.lexer,codes.buffer);
	PX_StringFree(&codes);

	PX_VectorInitialize(lib->mp,&analysis.v_variablesGlobalTable,sizeof(PX_SCRIPT_VARIABLES),1);
	PX_VectorInitialize(lib->mp,&analysis.v_variablesStackTable,sizeof(PX_SCRIPT_VARIABLES),1);
	PX_VectorInitialize(lib->mp,&analysis.v_struct,sizeof(PX_SCRIPT_STRUCT),1);
	PX_VectorInitialize(lib->mp,&analysis.v_functions,sizeof(PX_SCRIPT_FUNCTION),1);
	PX_VectorInitialize(lib->mp,&analysis.v_astStructure,sizeof(PX_SCRIPT_AST_STRUCTURE),1);

	analysis.mp=lib->mp;
	PX_StringInitialize(analysis.mp,&analysis.bootCode);
	PX_StringInitialize(analysis.mp,&analysis.code);


	//////////////////////////////////////////////////////////////////////////
	//Handle codes to stream


	while (PX_TRUE)
	{
		state=PX_LexerGetState(&analysis.lexer);
		if ((type=PX_ScriptTranslatorNextToken(&analysis.lexer))==PX_LEXER_LEXEME_TYPE_ERR)
		{
			goto _ERROR;
		}
		if (type==PX_LEXER_LEXEME_TYPE_END)
		{
			break;
		}

		if (type==PX_LEXER_LEXEME_TYPE_NEWLINE)
		{
			continue;
		}		
		//////////////////////////////////////////////////////////////////////////
		///Runtime
		//////////////////////////////////////////////////////////////////////////
		if (PX_strequ(analysis.lexer.CurLexeme.buffer,PX_SCRIPT_TRANSLATOR_KEYWORD_RUNTIME))
		{
			type=PX_ScriptTranslatorNextToken(&analysis.lexer);
			if (type!=PX_LEXER_LEXEME_TYPE_TOKEN)
			{
				goto _ERROR;
			}
			if (PX_strequ(analysis.lexer.CurLexeme.buffer,PX_SCRIPT_TRANSLATOR_KEYWORD_STACK))
			{
				type=PX_ScriptTranslatorNextToken(&analysis.lexer);
				if (type!=PX_LEXER_LEXEME_TYPE_TOKEN)
				{
					goto _ERROR;
				}
				if (!PX_strIsInt(analysis.lexer.CurLexeme.buffer))
				{
					goto _ERROR;
				}
				LocalStackSize=PX_atoi(analysis.lexer.CurLexeme.buffer);
				type=PX_ScriptTranslatorNextToken(&analysis.lexer);
				if (type!=PX_LEXER_LEXEME_TYPE_NEWLINE)
				{
					goto _ERROR;
				}
			}
			if (PX_strequ(analysis.lexer.CurLexeme.buffer,PX_SCRIPT_TRANSLATOR_KEYWORD_THREAD))
			{
				type=PX_ScriptTranslatorNextToken(&analysis.lexer);
				if (type!=PX_LEXER_LEXEME_TYPE_TOKEN)
				{
					goto _ERROR;
				}
				if (!PX_strIsInt(analysis.lexer.CurLexeme.buffer))
				{
					goto _ERROR;
				}
				thread=PX_atoi(analysis.lexer.CurLexeme.buffer);
				type=PX_ScriptTranslatorNextToken(&analysis.lexer);
				if (type!=PX_LEXER_LEXEME_TYPE_NEWLINE)
				{
					goto _ERROR;
				}
			}

			continue;
		}

		//////////////////////////////////////////////////////////////////////////
		///VAR
		//////////////////////////////////////////////////////////////////////////
		if ((pset=PX_ScriptParseGetStructInfo(&analysis,analysis.lexer.CurLexeme.buffer))!=PX_NULL||\
			PX_strequ(analysis.lexer.CurLexeme.buffer,PX_SCRIPT_TRANSLATOR_KEYWORD_VAR_INT)||\
			PX_strequ(analysis.lexer.CurLexeme.buffer,PX_SCRIPT_TRANSLATOR_KEYWORD_VAR_FLOAT)||\
			PX_strequ(analysis.lexer.CurLexeme.buffer,PX_SCRIPT_TRANSLATOR_KEYWORD_VAR_STRING)||\
			PX_strequ(analysis.lexer.CurLexeme.buffer,PX_SCRIPT_TRANSLATOR_KEYWORD_VAR_VOID)||\
			PX_strequ(analysis.lexer.CurLexeme.buffer,PX_SCRIPT_TRANSLATOR_KEYWORD_VAR_MEMORY)
			)
		{	
			
			type=PX_ScriptTranslatorNextToken(&analysis.lexer);

			if (type==PX_LEXER_LEXEME_TYPE_DELIMITER&&analysis.lexer.Symbol=='*')
			{
				type=PX_ScriptTranslatorNextToken(&analysis.lexer);
			}

			if (type!=PX_LEXER_LEXEME_TYPE_TOKEN)
			{
				goto _ERROR;
			}
			if (!PX_ScriptParseIsValidToken(analysis.lexer.CurLexeme.buffer))
			{
				goto _ERROR;
			}
			type=PX_ScriptTranslatorNextToken(&analysis.lexer);
			if (type==PX_LEXER_LEXEME_TYPE_DELIMITER&&analysis.lexer.Symbol=='(')
			{
				//custom function
				PX_LexerSetState(state);
				//////////////////////////////////////////////////////////////////////////
				if (analysis.functionInside)
				{
					goto _ERROR;
				}

				if(!PX_ScriptParseFunctionDefined(&analysis,PX_SCRIPT_TRANSLATOR_FUNCTION_TYPE_CUSTOM))
					goto _ERROR;

				type=PX_ScriptTranslatorNextTokenSN(&analysis.lexer);

				if (type==PX_LEXER_LEXEME_TYPE_DELIMITER&&analysis.lexer.Symbol=='{')
				{
					analysis.functionInside=PX_TRUE;
					buildAstStruct.type=PX_SCRIPT_AST_STRUCTURE_TYPE_FUNCTION;
					PX_strcpy(buildAstStruct._func.name,analysis.currentFunc.name,sizeof(analysis.currentFunc.name));
					PX_VectorPushback(&analysis.v_astStructure,&buildAstStruct);
					analysis.functionguider=PX_StringLen(&analysis.code);
					continue;
				}
				else if (analysis.lexer.Symbol==';')
				{
					continue;
				}
				else
				{
					goto _ERROR;
				}
				//////////////////////////////////////////////////////////////////////////
			}
			else
			{
				
				if (pset)
				{	
					//////////////////////////////////////////////////////////////////////////
					//STRUCT VARIALBE
					//////////////////////////////////////////////////////////////////////////
					//if (isBeginExpression)
					//{
					//	goto _ERROR;
					//}

					PX_LexerSetState(state);
					if(!PX_ScriptParseStruct(&analysis)) goto _ERROR;
				}
				else
				{
					//var
					PX_LexerSetState(state);
// 
// 					if (isBeginExpression)
// 					{
// 						PX_ScriptTranslatorError(&analysis.lexer,"Define variable before expression");
// 						goto _ERROR;
// 					}
					if(!PX_ScriptParseVar(&analysis)) goto _ERROR;
				}
				continue;
			}
			
		}
		//////////////////////////////////////////////////////////////////////////


		//////////////////////////////////////////////////////////////////////////
		///STRUCT DEFINE
		//////////////////////////////////////////////////////////////////////////
		if (PX_strequ(analysis.lexer.CurLexeme.buffer,PX_SCRIPT_TRANSLATOR_KEYWORD_VAR_STRUCT))
		{	
			if (analysis.functionInside)
			{
				goto _ERROR;
			}

			state=PX_LexerGetState(&analysis.lexer);
			if (PX_ScriptTranslatorNextToken(&analysis.lexer)!=PX_LEXER_LEXEME_TYPE_TOKEN)
			{
				goto _ERROR;
			}

			if ((type=PX_ScriptTranslatorNextTokenSN(&analysis.lexer))==PX_LEXER_LEXEME_TYPE_DELIMITER&&analysis.lexer.Symbol=='{')
			{
				PX_LexerSetState(state);
				{if(!PX_ScriptParseStructDefine(&analysis)) goto _ERROR;}
			}
			else
			{
				goto _ERROR;
			}
			continue;
		}
		//////////////////////////////////////////////////////////////////////////


		//////////////////////////////////////////////////////////////////////////
		// Host function define
		if (PX_strequ(analysis.lexer.CurLexeme.buffer,PX_SCRIPT_TRANSLATOR_KEYWORD_HOST))
		{
			if (analysis.functionInside)
			{
				goto _ERROR;
			}
			state=PX_LexerGetState(&analysis.lexer);
			if ((type=PX_ScriptTranslatorNextToken(&analysis.lexer))!=PX_LEXER_LEXEME_TYPE_TOKEN)
			{
				goto _ERROR;
			}
			if (PX_strequ(analysis.lexer.CurLexeme.buffer,PX_SCRIPT_TRANSLATOR_KEYWORD_VAR_INT)||\
				PX_strequ(analysis.lexer.CurLexeme.buffer,PX_SCRIPT_TRANSLATOR_KEYWORD_VAR_FLOAT)||\
				PX_strequ(analysis.lexer.CurLexeme.buffer,PX_SCRIPT_TRANSLATOR_KEYWORD_VAR_STRING)||\
				PX_strequ(analysis.lexer.CurLexeme.buffer,PX_SCRIPT_TRANSLATOR_KEYWORD_VAR_MEMORY)||\
				PX_strequ(analysis.lexer.CurLexeme.buffer,PX_SCRIPT_TRANSLATOR_KEYWORD_VAR_VOID)||\
				PX_ScriptParseGetStructInfo(&analysis,analysis.lexer.CurLexeme.buffer)!=PX_NULL
				)
			{
				PX_LexerSetState(state);
				if(!PX_ScriptParseFunctionDefined(&analysis,PX_SCRIPT_TRANSLATOR_FUNCTION_TYPE_HOST))
					goto _ERROR;
			}
			else
			{
				goto _ERROR;
			}
			
			type=PX_ScriptTranslatorNextTokenSN(&analysis.lexer);

			if (type==PX_LEXER_LEXEME_TYPE_DELIMITER&&analysis.lexer.Symbol=='{')
			{
				PX_ScriptTranslatorError(&analysis.lexer,"Could not implement host function.");
				goto _ERROR;
			}
			else if (analysis.lexer.Symbol==';')
			{
				continue;
			}
			else
			{
				goto _ERROR;
			}

		}
		//////////////////////////////////////////////////////////////////////////

		//////////////////////////////////////////////////////////////////////////
		// Export function define
		if (PX_strequ(analysis.lexer.CurLexeme.buffer,PX_SCRIPT_TRANSLATOR_KEYWORD_EXPORT))
		{
			state=PX_LexerGetState(&analysis.lexer);

			if (analysis.functionInside)
			{
				goto _ERROR;
			}
			if ((type=PX_ScriptTranslatorNextToken(&analysis.lexer))!=PX_LEXER_LEXEME_TYPE_TOKEN)
			{
				goto _ERROR;
			}

			if (PX_strequ(analysis.lexer.CurLexeme.buffer,PX_SCRIPT_TRANSLATOR_KEYWORD_VAR_INT)||\
				PX_strequ(analysis.lexer.CurLexeme.buffer,PX_SCRIPT_TRANSLATOR_KEYWORD_VAR_FLOAT)||\
				PX_strequ(analysis.lexer.CurLexeme.buffer,PX_SCRIPT_TRANSLATOR_KEYWORD_VAR_STRING)||\
				PX_strequ(analysis.lexer.CurLexeme.buffer,PX_SCRIPT_TRANSLATOR_KEYWORD_VAR_MEMORY)||\
				PX_strequ(analysis.lexer.CurLexeme.buffer,PX_SCRIPT_TRANSLATOR_KEYWORD_VAR_VOID)||\
				PX_ScriptParseGetStructInfo(&analysis,analysis.lexer.CurLexeme.buffer)!=PX_NULL
				)
			{
				PX_LexerSetState(state);
				if(!PX_ScriptParseFunctionDefined(&analysis,PX_SCRIPT_TRANSLATOR_FUNCTION_TYPE_EXPORT))
					goto _ERROR;
			}
			else
			{
				goto _ERROR;
			}

			type=PX_ScriptTranslatorNextTokenSN(&analysis.lexer);

			if (type==PX_LEXER_LEXEME_TYPE_DELIMITER&&analysis.lexer.Symbol=='{')
			{
				analysis.functionInside=PX_TRUE;
				buildAstStruct.type=PX_SCRIPT_AST_STRUCTURE_TYPE_FUNCTION;
				PX_strcpy(buildAstStruct._func.name,analysis.currentFunc.name,sizeof(analysis.currentFunc.name));
				PX_VectorPushback(&analysis.v_astStructure,&buildAstStruct);
				analysis.functionguider=PX_StringLen(&analysis.code);
				continue;
			}
			else if (analysis.lexer.Symbol==';')
			{
				continue;
			}
			else
			{
				goto _ERROR;
			}
		}
		//////////////////////////////////////////////////////////////////////////



		//////////////////////////////////////////////////////////////////////////
		//IF
		//////////////////////////////////////////////////////////////////////////
		if (PX_strequ(analysis.lexer.CurLexeme.buffer,PX_SCRIPT_TRANSLATOR_KEYWORD_IF))
		{
			if (!analysis.functionInside)
			{
				goto _ERROR;
			}
			type=PX_ScriptTranslatorNextTokenSN(&analysis.lexer);

			if (type!=PX_LEXER_LEXEME_TYPE_DELIMITER||analysis.lexer.Symbol!='(')
			{
				goto _ERROR;
			}

			//condition expression
			PX_StringInitialize(analysis.mp,&expression);
			PX_StringInitialize(analysis.mp,&expCode);

			if(!PX_ScriptParseGetExpression(&analysis,&expression,')'))
			{
				PX_StringFree(&expression);
				PX_StringFree(&expCode);
				goto _ERROR;
			}
			PX_StringCat(&analysis.code,";--------IF (CONDITION) \n");
			if(!PX_ScriptParseExpression(&analysis,expression.buffer,&expCode,&retOperand))
			{
				PX_StringFree(&expression);
				PX_StringFree(&expCode);
				goto _ERROR;
			}
			PX_StringCat(&analysis.code,expCode.buffer);
			PX_StringFree(&expression);
			PX_StringFree(&expCode);
			//////////////////////////////////////////////////////////////////////////

			if (!PX_ScriptParseIsOperandNumericType(retOperand))
			{
				goto _ERROR;
			}


			buildAstStruct.type=PX_SCRIPT_AST_STRUCTURE_TYPE_IF;
			buildAstStruct._if.ifflag=analysis._jFlag++;
			buildAstStruct._if.elseflag=analysis._jFlag++;

			state=PX_LexerGetState(&analysis.lexer);
			type=PX_ScriptTranslatorNextTokenSN(&analysis.lexer);
			if (type==PX_LEXER_LEXEME_TYPE_DELIMITER&&analysis.lexer.Symbol=='{')
			{
				buildAstStruct._if.oneline_expr=PX_FALSE;
			}
			else
			{
				PX_LexerSetState(state);
				buildAstStruct._if.oneline_expr=PX_TRUE;
			}
			PX_VectorPushback(&analysis.v_astStructure,&buildAstStruct);

			PX_StringInitialize(analysis.mp,&expression);
			PX_StringFormat1(&expression,"JE R1,0,_IF_%1\n",PX_STRINGFORMAT_INT(buildAstStruct._if.ifflag));
			PX_StringCat(&analysis.code,expression.buffer);
			PX_StringFree(&expression);
			continue;
		}

		//////////////////////////////////////////////////////////////////////////
		//WHILE
		//////////////////////////////////////////////////////////////////////////

		if (PX_strequ(analysis.lexer.CurLexeme.buffer,PX_SCRIPT_TRANSLATOR_KEYWORD_WHILE))
		{
			if (!analysis.functionInside)
			{
				goto _ERROR;
			}
			type=PX_ScriptTranslatorNextTokenSN(&analysis.lexer);

			if (type!=PX_LEXER_LEXEME_TYPE_DELIMITER||analysis.lexer.Symbol!='(')
			{
				goto _ERROR;
			}

			//condition expression
			PX_StringInitialize(analysis.mp,&expression);
			PX_StringInitialize(analysis.mp,&expCode);

			if(!PX_ScriptParseGetExpression(&analysis,&expression,')'))
			{
				PX_StringFree(&expression);
				PX_StringFree(&expCode);
				goto _ERROR;
			}
			PX_StringCat(&analysis.code,";--------WHILE (CONDITION) \n");
			buildAstStruct.type=PX_SCRIPT_AST_STRUCTURE_TYPE_WHILE;
			buildAstStruct._while.loopflag=analysis._jFlag++;
			buildAstStruct._while.endflag=analysis._jFlag++;

			PX_StringInitialize(analysis.mp,&fmrString);
			PX_StringFormat1(&fmrString,"_WHILE_%1:\n",PX_STRINGFORMAT_INT(buildAstStruct._while.loopflag));
			PX_StringCat(&analysis.code,fmrString.buffer);
			PX_StringFree(&fmrString);

			if(!PX_ScriptParseExpression(&analysis,expression.buffer,&expCode,&retOperand))
			{
				PX_StringFree(&expression);
				PX_StringFree(&expCode);
				goto _ERROR;
			}
			PX_StringCat(&analysis.code,expCode.buffer);
			PX_StringFree(&expression);
			PX_StringFree(&expCode);
			//////////////////////////////////////////////////////////////////////////

			if (!PX_ScriptParseIsOperandNumericType(retOperand))
			{
				goto _ERROR;
			}


			

			state=PX_LexerGetState(&analysis.lexer);
			type=PX_ScriptTranslatorNextTokenSN(&analysis.lexer);
			if (type==PX_LEXER_LEXEME_TYPE_DELIMITER&&analysis.lexer.Symbol=='{')
			{
				buildAstStruct._while.oneline_expr=PX_FALSE;
			}
			else
			{
				PX_LexerSetState(state);
				buildAstStruct._while.oneline_expr=PX_TRUE;
			}
			PX_VectorPushback(&analysis.v_astStructure,&buildAstStruct);

			PX_StringInitialize(analysis.mp,&expression);
			PX_StringFormat1(&expression,"JE R1,0,_WHILE_%1\n",PX_STRINGFORMAT_INT(buildAstStruct._while.endflag));
			PX_StringCat(&analysis.code,expression.buffer);
			PX_StringFree(&expression);
			continue;
		}
		//////////////////////////////////////////////////////////////////////////
		//_ASM
		//////////////////////////////////////////////////////////////////////////
		
		if (PX_strequ(analysis.lexer.CurLexeme.buffer,PX_SCRIPT_TRANSLATOR_KEYWORD_ASM))
		{
			if (!analysis.functionInside)
			{
				goto _ERROR;
			}

			type=PX_ScriptTranslatorNextTokenSN(&analysis.lexer);
			if (type!=PX_LEXER_LEXEME_TYPE_DELIMITER||analysis.lexer.Symbol!='{')
			{
				goto _ERROR;
			}

			PX_StringInitialize(analysis.mp,&codes);
			PX_StringInitialize(analysis.mp,&fmrString);
			while (PX_TRUE)
			{
				type=PX_LexerGetNextLexeme(&analysis.lexer);
				if (type==PX_LEXER_LEXEME_TYPE_DELIMITER&&analysis.lexer.Symbol=='}')
				{
					PX_StringCat(&codes,"\n");
					break;
				}

				for (i=0;i<analysis.v_variablesStackTable.size;i++)
				{
					pvar=PX_VECTORAT(PX_SCRIPT_VARIABLES,&analysis.v_variablesStackTable,i);
					if(pvar->type==PX_SCRIPT_PARSER_VAR_TYPE_INT||pvar->type==PX_SCRIPT_PARSER_VAR_TYPE_FLOAT||pvar->type==PX_SCRIPT_PARSER_VAR_TYPE_STRING||pvar->type==PX_SCRIPT_PARSER_VAR_TYPE_MEMORY)
					if (PX_strequ(pvar->Mnemonic.buffer,analysis.lexer.CurLexeme.buffer))
					{
						PX_StringFormat1(&fmrString,"LOCAL[%1]",PX_STRINGFORMAT_INT(pvar->BeginIndex));
						PX_StringCat(&codes,fmrString.buffer);
						goto _CONTINUE;
					}
				}

				for (i=0;i<analysis.v_variablesGlobalTable.size;i++)
				{
					pvar=PX_VECTORAT(PX_SCRIPT_VARIABLES,&analysis.v_variablesGlobalTable,i);
					if(pvar->type==PX_SCRIPT_PARSER_VAR_TYPE_INT||pvar->type==PX_SCRIPT_PARSER_VAR_TYPE_FLOAT||pvar->type==PX_SCRIPT_PARSER_VAR_TYPE_STRING||pvar->type==PX_SCRIPT_PARSER_VAR_TYPE_MEMORY)
					if (PX_strequ(pvar->Mnemonic.buffer,analysis.lexer.CurLexeme.buffer))
					{
						PX_StringFormat1(&fmrString,"GLOBAL[%1]",PX_STRINGFORMAT_INT(pvar->BeginIndex));
						PX_StringCat(&codes,fmrString.buffer);
						goto _CONTINUE;
					}
				}

				PX_StringCat(&codes,analysis.lexer.CurLexeme.buffer);
_CONTINUE:
				PX_StringClear(&fmrString);
			}
			PX_StringCat(&analysis.code,codes.buffer);
			PX_StringFree(&codes);
			PX_StringFree(&fmrString);
			continue;
		}


		//////////////////////////////////////////////////////////////////////////
		//FOR
		//////////////////////////////////////////////////////////////////////////

		if (PX_strequ(analysis.lexer.CurLexeme.buffer,PX_SCRIPT_TRANSLATOR_KEYWORD_FOR))
		{
			if (!analysis.functionInside)
			{
				goto _ERROR;
			}
			type=PX_ScriptTranslatorNextTokenSN(&analysis.lexer);

			if (type!=PX_LEXER_LEXEME_TYPE_DELIMITER||analysis.lexer.Symbol!='(')
			{
				goto _ERROR;
			}

			buildAstStruct.type=PX_SCRIPT_AST_STRUCTURE_TYPE_FOR;
			buildAstStruct._for.additionFlag=analysis._jFlag++;
			buildAstStruct._for.conditionFlag=analysis._jFlag++;
			buildAstStruct._for.endFlag=analysis._jFlag++;

			//condition expression
			PX_StringCat(&analysis.code,";--------FOR (INIT;CONDITION;ADDITION) \n");

			PX_StringInitialize(analysis.mp,&expression);
			PX_StringInitialize(analysis.mp,&expCode);
			PX_StringInitialize(analysis.mp,&fmrString);
			PX_StringInitialize(analysis.mp,&condCodes);
			//INIT code
			if(!PX_ScriptParseGetExpression(&analysis,&expression,';'))
			{
				PX_StringFree(&expression);
				PX_StringFree(&expCode);
				PX_StringFree(&fmrString);
				PX_StringFree(&condCodes);
				goto _ERROR;
			}

			if(!PX_ScriptParseExpression(&analysis,expression.buffer,&expCode,&retOperand))
			{
				PX_StringFree(&expression);
				PX_StringFree(&expCode);
				PX_StringFree(&fmrString);
				PX_StringFree(&condCodes);
				goto _ERROR;
			}
			PX_StringCat(&analysis.code,expCode.buffer);

			//Jump to condition
			
			PX_StringFormat1(&fmrString,"JMP _FOR_%1\n",PX_STRINGFORMAT_INT(buildAstStruct._for.conditionFlag));
			PX_StringCat(&analysis.code,fmrString.buffer);


			//Condition
			if(!PX_ScriptParseGetExpression(&analysis,&expression,';'))
			{
				PX_StringFree(&expression);
				PX_StringFree(&expCode);
				PX_StringFree(&fmrString);
				PX_StringFree(&condCodes);
				goto _ERROR;
			}

			if(!PX_ScriptParseExpression(&analysis,expression.buffer,&expCode,&retOperand))
			{
				PX_StringFree(&expression);
				PX_StringFree(&expCode);
				PX_StringFree(&fmrString);
				PX_StringFree(&condCodes);
				goto _ERROR;
			}
			PX_StringCat(&condCodes,expCode.buffer);


			//Addition
			//Addition flag
			PX_StringFormat1(&fmrString,"_FOR_%1:\n",PX_STRINGFORMAT_INT(buildAstStruct._for.additionFlag));
			PX_StringCat(&analysis.code,fmrString.buffer);

			if(!PX_ScriptParseGetExpression(&analysis,&expression,')'))
			{
				PX_StringFree(&expression);
				PX_StringFree(&expCode);
				PX_StringFree(&fmrString);
				PX_StringFree(&condCodes);
				goto _ERROR;
			}

			if(!PX_ScriptParseExpression(&analysis,expression.buffer,&expCode,&retOperand))
			{
				PX_StringFree(&expression);
				PX_StringFree(&expCode);
				PX_StringFree(&fmrString);
				PX_StringFree(&condCodes);
				goto _ERROR;
			}
			PX_StringCat(&analysis.code,expCode.buffer);

			//condition
			//condition flag
			PX_StringFormat1(&fmrString,"_FOR_%1:\n",PX_STRINGFORMAT_INT(buildAstStruct._for.conditionFlag));
			PX_StringCat(&analysis.code,fmrString.buffer);
			PX_StringCat(&analysis.code,condCodes.buffer);

			//jump to end
			PX_StringFormat1(&fmrString,"JE R1,0,_FOR_%1\n",PX_STRINGFORMAT_INT(buildAstStruct._for.endFlag));
			PX_StringCat(&analysis.code,fmrString.buffer);

			PX_StringFree(&expression);
			PX_StringFree(&expCode);
			PX_StringFree(&fmrString);
			PX_StringFree(&condCodes);

		
			//////////////////////////////////////////////////////////////////////////

			if (!PX_ScriptParseIsOperandNumericType(retOperand))
			{
				goto _ERROR;
			}


			state=PX_LexerGetState(&analysis.lexer);
			type=PX_ScriptTranslatorNextTokenSN(&analysis.lexer);
			if (type==PX_LEXER_LEXEME_TYPE_DELIMITER&&analysis.lexer.Symbol=='{')
			{
				buildAstStruct._for.oneline_expr=PX_FALSE;
			}
			else
			{
				PX_LexerSetState(state);
				buildAstStruct._for.oneline_expr=PX_TRUE;
			}
			PX_VectorPushback(&analysis.v_astStructure,&buildAstStruct);
			continue;
		}


		//////////////////////////////////////////////////////////////////////////
		//SWITCH
		//////////////////////////////////////////////////////////////////////////
		if (PX_strequ(analysis.lexer.CurLexeme.buffer,PX_SCRIPT_TRANSLATOR_KEYWORD_SWITCH))
		{
			if (!analysis.functionInside)
			{
				goto _ERROR;
			}

			buildAstStruct.type=PX_SCRIPT_AST_STRUCTURE_TYPE_SWITCH;
			buildAstStruct._compare.endFlag=analysis._jFlag++;

			type=PX_ScriptTranslatorNextTokenSN(&analysis.lexer);

			if (type!=PX_LEXER_LEXEME_TYPE_DELIMITER||analysis.lexer.Symbol!='(')
			{
				goto _ERROR;
			}

			PX_StringCat(&analysis.code,";--------SWITCH (ORIGINAL) \n");

			//condition expression
			PX_StringInitialize(analysis.mp,&expression);
			PX_StringInitialize(analysis.mp,&expCode);

			if(!PX_ScriptParseGetExpression(&analysis,&expression,')'))
			{
				PX_StringFree(&expression);
				PX_StringFree(&expCode);
				goto _ERROR;
			}
			
			if(!PX_ScriptParseExpression(&analysis,expression.buffer,&expCode,&retOperand))
			{
				PX_StringFree(&expression);
				PX_StringFree(&expCode);
				goto _ERROR;
			}
			
			buildAstStruct._compare.OriginOperand=retOperand;

			PX_StringCat(&analysis.code,expCode.buffer);
			PX_StringCat(&analysis.code,"PUSH R1\n");
			PX_StringFree(&expression);
			PX_StringFree(&expCode);
			//////////////////////////////////////////////////////////////////////////

			state=PX_LexerGetState(&analysis.lexer);
			type=PX_ScriptTranslatorNextTokenSN(&analysis.lexer);
			if (type==PX_LEXER_LEXEME_TYPE_DELIMITER&&analysis.lexer.Symbol=='{')
			{
				buildAstStruct._compare.oneline_expr=PX_FALSE;
			}
			else
			{
				PX_LexerSetState(state);
				buildAstStruct._compare.oneline_expr=PX_TRUE;
			}
		
			PX_VectorPushback(&analysis.v_astStructure,&buildAstStruct);
			continue;
		}



		//////////////////////////////////////////////////////////////////////////
		//CASE
		//////////////////////////////////////////////////////////////////////////
		if (PX_strequ(analysis.lexer.CurLexeme.buffer,PX_SCRIPT_TRANSLATOR_KEYWORD_CASE))
		{
			if (!analysis.functionInside)
			{
				goto _ERROR;
			}
			caseEnd=PX_FALSE;
			for (i=analysis.v_astStructure.size-1;i>=0;i--)
			{
				astStruct=*PX_VECTORAT(PX_SCRIPT_AST_STRUCTURE,&analysis.v_astStructure,i);
				if (astStruct.type==PX_SCRIPT_AST_STRUCTURE_TYPE_SWITCH)
				{
					break;
				}
			}
			if (i<=0) //0 is function AST structure
			{
				goto _ERROR;
			}


			type=PX_ScriptTranslatorNextTokenSN(&analysis.lexer);

			if (type!=PX_LEXER_LEXEME_TYPE_DELIMITER||analysis.lexer.Symbol!='(')
			{
				goto _ERROR;
			}

			buildAstStruct.type=PX_SCRIPT_AST_STRUCTURE_TYPE_CASE;
			buildAstStruct._with.endFlag=analysis._jFlag++;

			PX_StringCat(&analysis.code,";--------WITH (CONDITION) \n");
			
			//condition expression
			PX_StringInitialize(analysis.mp,&expression);
			PX_StringInitialize(analysis.mp,&expCode);

			PX_StringCat(&analysis.code,"MOV R3,0\n");
			PX_StringCat(&analysis.code,"MOV R2,GLOBAL[SP]\n");

			while(PX_TRUE)
			{

// 				if(!PX_ScriptParseGetExpression(&analysis,&expression,';'))
// 				{
// 					PX_StringFree(&expression);
// 					PX_StringFree(&expCode);
// 					goto _ERROR;
// 				}
				//////////////////////////////////////////////////////////////////////////
				PX_StringClear(&expression);
				mBracket=0,lBracket=0;

				while (PX_TRUE)
				{
					type=PX_ScriptTranslatorNextToken(&analysis.lexer);

					if (type==PX_LEXER_LEXEME_TYPE_NEWLINE)
					{
						PX_StringFree(&expression);
						PX_StringFree(&expCode);
						goto _ERROR;
					}
					if (type==PX_LEXER_LEXEME_TYPE_END)
					{
						PX_StringFree(&expression);
						PX_StringFree(&expCode);
						goto _ERROR;
					}
					if (type==PX_LEXER_LEXEME_TYPE_DELIMITER&&analysis.lexer.Symbol==';'&&!mBracket&&!lBracket)
					{
						break;
					}

					if (type==PX_LEXER_LEXEME_TYPE_DELIMITER&&analysis.lexer.Symbol==')'&&!mBracket&&!lBracket)
					{
						caseEnd=PX_TRUE;
						break;
					}

					if (analysis.lexer.Symbol=='(')
					{
						lBracket++;
					}
					if (analysis.lexer.Symbol==')')
					{
						lBracket--;
						if(lBracket<0)
							return PX_FALSE;
					}
					if (analysis.lexer.Symbol=='[')
					{
						mBracket++;
					}
					if (analysis.lexer.Symbol==']')
					{
						mBracket--;
						if(mBracket<0)
							return PX_FALSE;
					}

					if (type==PX_LEXER_LEXEME_TYPE_SPACER)
					{
						continue;
					}

					PX_StringCat(&expression,analysis.lexer.CurLexeme.buffer);
				}
				//////////////////////////////////////////////////////////////////////////

				if(!PX_ScriptParseExpression(&analysis,expression.buffer,&expCode,&retOperand))
				{
					PX_StringFree(&expression);
					PX_StringFree(&expCode);
					goto _ERROR;
				}

				if (!PX_ScriptParseIsCompareAbleOperand(retOperand,astStruct._compare.OriginOperand))
				{
					PX_StringFree(&expression);
					PX_StringFree(&expCode);
					PX_ScriptTranslatorError(&analysis.lexer,"Invalid operand-type.");
					goto _ERROR;
				}

				PX_StringCat(&analysis.code,expCode.buffer);			
			    PX_StringCat(&analysis.code,"LGE R1,R1,R2\n");
				PX_StringCat(&analysis.code,"OR R3,R1\n");
				state=PX_LexerGetState(&analysis.lexer);

				if (caseEnd)
				{
					break;
				}
				
			}
			PX_StringFormat1(&expCode,"JE R3,0,_WITH_%1\n",PX_STRINGFORMAT_INT(buildAstStruct._with.endFlag));
			PX_StringCat(&analysis.code,expCode.buffer);

			PX_StringFree(&expression);
			PX_StringFree(&expCode);

			//////////////////////////////////////////////////////////////////////////
			

			state=PX_LexerGetState(&analysis.lexer);
			type=PX_ScriptTranslatorNextTokenSN(&analysis.lexer);
			if (type==PX_LEXER_LEXEME_TYPE_DELIMITER&&analysis.lexer.Symbol=='{')
			{
				buildAstStruct._with.oneline_expr=PX_FALSE;
			}
			else
			{
				PX_LexerSetState(state);
				buildAstStruct._with.oneline_expr=PX_TRUE;
			}

			PX_VectorPushback(&analysis.v_astStructure,&buildAstStruct);
			continue;
		}
		///////////////////////////////////////////////////////////////////////////
		//break
		//////////////////////////////////////////////////////////////////////////
		
		if (PX_strequ(analysis.lexer.CurLexeme.buffer,PX_SCRIPT_TRANSLATOR_KEYWORD_BREAK))
		{
			type=PX_ScriptTranslatorNextToken(&analysis.lexer);
			
			if (type!=PX_LEXER_LEXEME_TYPE_DELIMITER||analysis.lexer.Symbol!=';')
			{
				goto _ERROR;
			}
			for (i=analysis.v_astStructure.size-1;i>=0;i--)
			{
				astStruct=*PX_VECTORAT(PX_SCRIPT_AST_STRUCTURE,&analysis.v_astStructure,i);
				switch(astStruct.type)
				{
				case PX_SCRIPT_AST_STRUCTURE_TYPE_WHILE:
					{
						PX_StringInitialize(analysis.mp,&fmrString);
						PX_StringFormat1(&fmrString,"JMP _WHILE_%1\n",PX_STRINGFORMAT_INT(astStruct._while.endflag));
						PX_StringCat(&analysis.code,fmrString.buffer);
						PX_StringFree(&fmrString);
						goto _BREAKOUT;
					}
					break;
				case PX_SCRIPT_AST_STRUCTURE_TYPE_FOR:
					{
						PX_StringInitialize(analysis.mp,&fmrString);
						PX_StringFormat1(&fmrString,"JMP _FOR_%1\n",PX_STRINGFORMAT_INT(astStruct._for.endFlag));
						PX_StringCat(&analysis.code,fmrString.buffer);
						PX_StringFree(&fmrString);
						goto _BREAKOUT;
					}
					break;
				case PX_SCRIPT_AST_STRUCTURE_TYPE_SWITCH:
					{
						PX_StringInitialize(analysis.mp,&fmrString);
						PX_StringFormat1(&fmrString,"JMP _COMPARE_%1\n",PX_STRINGFORMAT_INT(astStruct._compare.endFlag));
						PX_StringCat(&analysis.code,fmrString.buffer);
						PX_StringFree(&fmrString);
						goto _BREAKOUT;
					}
				    break;
					default:
						continue;
				}
			}
			_BREAKOUT:
			if (i<0)
			{
				PX_ScriptTranslatorError(&analysis.lexer,"Could not matched  break to block.");
				goto _ERROR;
			}
			if(!PX_ScriptParseIfLastAST(&analysis)) goto _ERROR;
			continue;
		}


		///////////////////////////////////////////////////////////////////////////
		//continue
		//////////////////////////////////////////////////////////////////////////

		if (PX_strequ(analysis.lexer.CurLexeme.buffer,PX_SCRIPT_TRANSLATOR_KEYWORD_CONTINUE))
		{
			type=PX_ScriptTranslatorNextToken(&analysis.lexer);

			if (type!=PX_LEXER_LEXEME_TYPE_DELIMITER||analysis.lexer.Symbol!=';')
			{
				goto _ERROR;
			}
			for (i=analysis.v_astStructure.size-1;i>=0;i--)
			{
				astStruct=*PX_VECTORAT(PX_SCRIPT_AST_STRUCTURE,&analysis.v_astStructure,i);
				switch(astStruct.type)
				{
				case PX_SCRIPT_AST_STRUCTURE_TYPE_WHILE:
					{
						PX_StringInitialize(analysis.mp,&fmrString);
						PX_StringFormat1(&fmrString,"JMP _WHILE_%1\n",PX_STRINGFORMAT_INT(astStruct._while.loopflag));
						PX_StringCat(&analysis.code,fmrString.buffer);
						PX_StringFree(&fmrString);
						goto _CONTINUEOUT;
					}
					break;
				case PX_SCRIPT_AST_STRUCTURE_TYPE_FOR:
					{
						PX_StringInitialize(analysis.mp,&fmrString);
						PX_StringFormat1(&fmrString,"JMP _FOR_%1\n",PX_STRINGFORMAT_INT(astStruct._for.additionFlag));
						PX_StringCat(&analysis.code,fmrString.buffer);
						PX_StringFree(&fmrString);
						goto _CONTINUEOUT;
					}
					break;
					default:
						continue;
				}
			}
_CONTINUEOUT:
			if (i<0)
			{
				PX_ScriptTranslatorError(&analysis.lexer,"Could not matched  break to block.");
				goto _ERROR;
			}
			if(!PX_ScriptParseIfLastAST(&analysis)) goto _ERROR;
			continue;
		}

		///////////////////////////////////////////////////////////////////////////
		//Return
		//////////////////////////////////////////////////////////////////////////
		if (PX_strequ(analysis.lexer.CurLexeme.buffer,PX_SCRIPT_TRANSLATOR_KEYWORD_RETURN))
		{
			if (!analysis.functionInside)
			{
				goto _ERROR;
			}
			//custom expression
			PX_StringInitialize(analysis.mp,&expression);
			PX_StringInitialize(analysis.mp,&expCode);

			if(!PX_ScriptParseGetExpression(&analysis,&expression,';'))
			{
				PX_StringFree(&expression);
				PX_StringFree(&expCode);
				goto _ERROR;
			}
			PX_StringCat(&analysis.code,";RETURN (EXPRESSION)\n");
			if(!PX_ScriptParseExpression(&analysis,expression.buffer,&expCode,&retOperand))
			{
				PX_StringFree(&expression);
				PX_StringFree(&expCode);
				goto _ERROR;
			}
			
			
			PX_StringCat(&analysis.code,expCode.buffer);
			PX_StringFree(&expression);
			PX_StringFree(&expCode);

			switch (analysis.currentFunc.retType)
			{
			case PX_SCRIPT_PARSER_VAR_TYPE_VOID:
				if (retOperand.operandType!=PX_SCRIPT_AST_OPERAND_TYPE_VOID)
				{
					PX_ScriptTranslatorError(&analysis.lexer,"Return-type not matched.");
					goto _ERROR;
				}
				break;
			case PX_SCRIPT_PARSER_VAR_TYPE_INT:
				if (retOperand.operandType!=PX_SCRIPT_AST_OPERAND_TYPE_INT&&retOperand.operandType!=PX_SCRIPT_AST_OPERAND_TYPE_INT_CONST&&\
					retOperand.operandType!=PX_SCRIPT_AST_OPERAND_TYPE_FLOAT&&retOperand.operandType!=PX_SCRIPT_AST_OPERAND_TYPE_FLOAT_CONST&&\
					retOperand.operandType!=PX_SCRIPT_AST_OPERAND_TYPE_STRING_IDX&&retOperand.operandType!=PX_SCRIPT_AST_OPERAND_TYPE_MEMORY_IDX)
				{
					PX_ScriptTranslatorError(&analysis.lexer,"Return-type not matched.");
						goto _ERROR;
				}
				if (retOperand.operandType==PX_SCRIPT_AST_OPERAND_TYPE_FLOAT||retOperand.operandType==PX_SCRIPT_AST_OPERAND_TYPE_FLOAT_CONST)
				{
						PX_StringCat(&analysis.code,"INT R1\n");
				}
				break;
			case PX_SCRIPT_PARSER_VAR_TYPE_FLOAT:
				if (retOperand.operandType!=PX_SCRIPT_AST_OPERAND_TYPE_INT&&retOperand.operandType!=PX_SCRIPT_AST_OPERAND_TYPE_INT_CONST&&\
					retOperand.operandType!=PX_SCRIPT_AST_OPERAND_TYPE_FLOAT&&retOperand.operandType!=PX_SCRIPT_AST_OPERAND_TYPE_FLOAT_CONST&&\
					retOperand.operandType!=PX_SCRIPT_AST_OPERAND_TYPE_STRING_IDX&&retOperand.operandType!=PX_SCRIPT_AST_OPERAND_TYPE_MEMORY_IDX)
				{
					PX_ScriptTranslatorError(&analysis.lexer,"Return-type not matched.");
						goto _ERROR;
				}
				if (retOperand.operandType==PX_SCRIPT_AST_OPERAND_TYPE_INT||retOperand.operandType==PX_SCRIPT_AST_OPERAND_TYPE_INT_CONST)
				{
					PX_StringCat(&analysis.code,"FLT R1\n");
				}
				break;
			case PX_SCRIPT_PARSER_VAR_TYPE_STRING:
				if (retOperand.operandType!=PX_SCRIPT_AST_OPERAND_TYPE_STRING&&retOperand.operandType!=PX_SCRIPT_AST_OPERAND_TYPE_STRING_CONST)
				{
					PX_ScriptTranslatorError(&analysis.lexer,"Return-type not matched.");
						goto _ERROR;
				}
				break;
			case PX_SCRIPT_PARSER_VAR_TYPE_MEMORY:
				if (retOperand.operandType!=PX_SCRIPT_AST_OPERAND_TYPE_MEMORY&&retOperand.operandType!=PX_SCRIPT_AST_OPERAND_TYPE_MEMORY_CONST)
				{
					PX_ScriptTranslatorError(&analysis.lexer,"Return-type not matched.");
						goto _ERROR;
				}
				break;
			case PX_SCRIPT_PARSER_VAR_TYPE_INT_PTR:
				if (retOperand.operandType!=PX_SCRIPT_AST_OPERAND_TYPE_INT_PTR&&retOperand.operandType!=PX_SCRIPT_AST_OPERAND_TYPE_INT_PTR_CONST)
				{
					PX_ScriptTranslatorError(&analysis.lexer,"Return-type not matched.");
						goto _ERROR;
				}
				break;
			case PX_SCRIPT_PARSER_VAR_TYPE_FLOAT_PTR:
				if (retOperand.operandType!=PX_SCRIPT_AST_OPERAND_TYPE_FLOAT_PTR&&retOperand.operandType!=PX_SCRIPT_AST_OPERAND_TYPE_FLOAT_PTR_CONST)
				{
					PX_ScriptTranslatorError(&analysis.lexer,"Return-type not matched.");
						goto _ERROR;
				}
				break;
			case PX_SCRIPT_PARSER_VAR_TYPE_STRING_PTR:
				if (retOperand.operandType!=PX_SCRIPT_AST_OPERAND_TYPE_STRING_PTR&&retOperand.operandType!=PX_SCRIPT_AST_OPERAND_TYPE_STRING_PTR_CONST)
				{
					PX_ScriptTranslatorError(&analysis.lexer,"Return-type not matched.");
						goto _ERROR;
				}
				break;
			case PX_SCRIPT_PARSER_VAR_TYPE_MEMORY_PTR:
				if (retOperand.operandType!=PX_SCRIPT_AST_OPERAND_TYPE_MEMORY_PTR&&retOperand.operandType!=PX_SCRIPT_AST_OPERAND_TYPE_MEMORY_PTR_CONST)
				{
					PX_ScriptTranslatorError(&analysis.lexer,"Return-type not matched.");
						goto _ERROR;
				}
				break;
			case PX_SCRIPT_PARSER_VAR_TYPE_STRUCT_PTR:
				if (retOperand.operandType!=PX_SCRIPT_AST_OPERAND_TYPE_STRUCT_PTR&&retOperand.operandType!=PX_SCRIPT_AST_OPERAND_TYPE_SET_PTR_CONST)
				{
					PX_ScriptTranslatorError(&analysis.lexer,"Return-type not matched.");
						goto _ERROR;
				}
				if (retOperand.pSet!=PX_ScriptParseGetStructByIndex(&analysis,analysis.currentFunc.retSetIndex))
				{
					PX_ScriptTranslatorError(&analysis.lexer,"Return-type not matched.");
						goto _ERROR;
				}
				break;
				default:
					goto _ERROR;
			}

			PX_StringInitialize(analysis.mp,&fmrString);
			PX_StringFormat1(&fmrString,"JMP _%1_RET\n",PX_STRINGFORMAT_STRING(analysis.currentFunc.name));
			PX_StringCat(&analysis.code,fmrString.buffer);
			PX_StringFree(&fmrString);


			if(!PX_ScriptParseIfLastAST(&analysis)) goto _ERROR;
			continue;
		}

	
		//Expression parse
		
		if(analysis.functionInside)
		{

			if (type==PX_LEXER_LEXEME_TYPE_DELIMITER&&analysis.lexer.Symbol=='}')
			{
				if(!PX_ScriptParseLastCodeblockEnd(&analysis)) goto _ERROR;
			}
			else
			{
				//custom expression
				PX_LexerSetState(state);

				PX_StringInitialize(analysis.mp,&expression);
				PX_StringInitialize(analysis.mp,&expCode);

				if(!PX_ScriptParseGetExpression(&analysis,&expression,';'))
				{
					PX_StringFree(&expression);
					PX_StringFree(&expCode);
					goto _ERROR;
				}

				if(!PX_ScriptParseExpression(&analysis,expression.buffer,&expCode,&retOperand))
				{
					PX_StringFree(&expression);
					PX_StringFree(&expCode);
					goto _ERROR;
				}
				
				PX_StringCat(&analysis.code,expCode.buffer);
				
				PX_StringFree(&expression);
				PX_StringFree(&expCode);

				if(!PX_ScriptParseIfLastAST(&analysis)) goto _ERROR;
			}
			continue;
		}
		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		//

		PX_ScriptTranslatorError(&analysis.lexer,"Invalid expression.");
		goto _ERROR;

	}
	//////////////////////////////////////////////////////////////////////////
	//Output
	if (!PX_ScriptParseBootCode(&analysis))
	{
		goto _ERROR;
	}
	//.Global .Local
	globalSize=0;
	for (i=0;i<analysis.v_variablesGlobalTable.size;i++)
	{
		globalSize+=PX_VECTORAT(PX_SCRIPT_VARIABLES,&analysis.v_variablesGlobalTable,i)->size;
	}

	PX_StringInitialize(analysis.mp,&fmrString);
	PX_StringFormat3(&fmrString,".GLOBAL %1\n.STACK %2\n.THREAD %3\n",PX_STRINGFORMAT_INT(globalSize),PX_STRINGFORMAT_INT(LocalStackSize),PX_STRINGFORMAT_INT(thread));
	PX_StringCat(ASM,fmrString.buffer);
	PX_StringFree(&fmrString);

	PX_StringCat(ASM,"EXPORT FUNCTION _BOOT:\n");

	PX_StringInitialize(analysis.mp,&fmrString);
	for (i=0;i<analysis.v_variablesGlobalTable.size;i++)
	{
		pvar=PX_VECTORAT(PX_SCRIPT_VARIABLES,&analysis.v_variablesGlobalTable,i);
		if (!pvar->bParam&&pvar->type==PX_SCRIPT_PARSER_VAR_TYPE_STRING)
		{
			PX_StringFormat1(&fmrString,"MOV GLOBAL[%1],\"\"\n",PX_STRINGFORMAT_INT(pvar->BeginIndex));
			PX_StringCat(ASM,fmrString.buffer);
		}
		if (!pvar->bParam&&pvar->type==PX_SCRIPT_PARSER_VAR_TYPE_MEMORY)
		{
			PX_StringFormat1(&fmrString,"MOV GLOBAL[%1],@@\n",PX_STRINGFORMAT_INT(pvar->BeginIndex));
			PX_StringCat(ASM,fmrString.buffer);
		}
	}
	PX_StringFree(&fmrString);

	PX_StringCat(ASM,analysis.bootCode.buffer);
	PX_StringCat(ASM,"RET\n");

	PX_StringCat(ASM,analysis.code.buffer);

	if (analysis.v_astStructure.size)
	{
		while(analysis.v_astStructure.size)
		{
			PX_ScriptParsePopAstStructure(&analysis);
		}
		goto _ERROR;
	}

	for (i=0;i<analysis.v_variablesGlobalTable.size;i++)
	{
		PX_StringFree(&PX_VECTORAT(PX_SCRIPT_VARIABLES,&analysis.v_variablesGlobalTable,i)->Mnemonic);
		if (PX_VECTORAT(PX_SCRIPT_VARIABLES,&analysis.v_variablesGlobalTable,i)->bInitialized)
		{
			PX_StringFree(&PX_VECTORAT(PX_SCRIPT_VARIABLES,&analysis.v_variablesGlobalTable,i)->GlobalInitializeValue);
		}
	}

	for (i=0;i<analysis.v_variablesStackTable.size;i++)
	{
		PX_SCRIPT_VARIABLES *pvar=PX_VECTORAT(PX_SCRIPT_VARIABLES,&analysis.v_variablesStackTable,i);
		PX_StringFree(&pvar->Mnemonic);
		if (pvar->bInitialized)
		{
			PX_StringFree(&pvar->GlobalInitializeValue);
		}
	}

	//struct
	for (i=0;i<analysis.v_struct.size;i++)
	{
		pset=PX_VECTORAT(PX_SCRIPT_STRUCT,&analysis.v_struct,i);
		for (j=0;j<pset->members.size;j++)
		{
			psetmem=PX_VECTORAT(PX_SCRIPT_SETMEMBER,&pset->members,j);
			PX_StringFree(&psetmem->defvar.Mnemonic);
			if (psetmem->defvar.bInitialized)
			{
				PX_StringFree(&psetmem->defvar.GlobalInitializeValue);
			}
		}
		PX_StringFree(&pset->Name);
		PX_VectorFree(&pset->members);
	}

	//ast structure
	

	PX_VectorFree(&analysis.v_astStructure);
	PX_VectorFree(&analysis.v_variablesGlobalTable);
	PX_VectorFree(&analysis.v_variablesStackTable);
	PX_VectorFree(&analysis.v_struct);
	PX_VectorFree(&analysis.v_functions);

	PX_StringFree(&analysis.bootCode);
	PX_StringFree(&analysis.code);
	PX_LexerFree(&analysis.lexer);
	return PX_TRUE;
_ERROR:
	PX_ScriptTranslatorError(&analysis.lexer,"ERROR:Compile terminated.");

	if (analysis.v_astStructure.size)
	{
		while(analysis.v_astStructure.size)
		{
			PX_ScriptParsePopAstStructure(&analysis);
		}
		goto _ERROR;
	}

	for (i=0;i<analysis.v_variablesGlobalTable.size;i++)
	{
		PX_StringFree(&PX_VECTORAT(PX_SCRIPT_VARIABLES,&analysis.v_variablesGlobalTable,i)->Mnemonic);
		if (PX_VECTORAT(PX_SCRIPT_VARIABLES,&analysis.v_variablesGlobalTable,i)->bInitialized)
		{
			PX_StringFree(&PX_VECTORAT(PX_SCRIPT_VARIABLES,&analysis.v_variablesGlobalTable,i)->GlobalInitializeValue);
		}
	}

	for (i=0;i<analysis.v_variablesStackTable.size;i++)
	{
		PX_StringFree(&PX_VECTORAT(PX_SCRIPT_VARIABLES,&analysis.v_variablesStackTable,i)->Mnemonic);
		if (PX_VECTORAT(PX_SCRIPT_VARIABLES,&analysis.v_variablesStackTable,i)->bInitialized)
		{
			PX_StringFree(&PX_VECTORAT(PX_SCRIPT_VARIABLES,&analysis.v_variablesStackTable,i)->GlobalInitializeValue);
		}
	}

	//struct
	for (i=0;i<analysis.v_struct.size;i++)
	{
		pset=PX_VECTORAT(PX_SCRIPT_STRUCT,&analysis.v_struct,i);
		for (j=0;j<pset->members.size;j++)
		{
			psetmem=PX_VECTORAT(PX_SCRIPT_SETMEMBER,&pset->members,j);
			PX_StringFree(&psetmem->defvar.Mnemonic);
			if (psetmem->defvar.bInitialized)
			{
				PX_StringFree(&psetmem->defvar.GlobalInitializeValue);
			}
		}
		PX_StringFree(&pset->Name);
		PX_VectorFree(&pset->members);
	}

	//ast structure


	PX_VectorFree(&analysis.v_astStructure);
	PX_VectorFree(&analysis.v_variablesGlobalTable);
	PX_VectorFree(&analysis.v_variablesStackTable);
	PX_VectorFree(&analysis.v_struct);
	PX_VectorFree(&analysis.v_functions);

	PX_StringFree(&analysis.bootCode);
	PX_StringFree(&analysis.code);
	PX_LexerFree(&analysis.lexer);


	return PX_FALSE;
}

px_char * PX_ScriptCompilerError(void)
{
	return PX_Script_InterpreterError;
}

px_bool PX_ScriptInterpreterExpression(PX_SCRIPT_Analysis *analysis,px_char *expr,px_string *out,PX_SCRIPT_AST_OPERAND *retOperand)
{
	return PX_ScriptParseExpression(analysis,expr,out,retOperand);
}



