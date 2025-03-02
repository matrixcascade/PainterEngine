#include "./PX_Lexer.h"

static px_char * PX_LexerIsCommentStart(px_lexer *lexer,const px_char ch[]);
static px_char * PX_LexerIsCommentEnd(px_lexer *lexer,const px_char startch[],const px_char ch[]);
static px_char* PX_LexerIsContainerStart(px_lexer *lexer,const px_char ch[]);
static char*  PX_LexerIsContainerEnd(px_lexer *lexer,const px_char startch[],const px_char ch[]);
static px_bool PX_LexerIsContainerTransfer(px_lexer *lexer,const px_char startch[],px_char ch);

static px_int PX_LexerIsSpacer(px_lexer *lexer,px_char chr);
static px_int PX_LexerIsDelimiter(px_lexer *lexer,px_char chr);
static px_int PX_LexerFilterChar(px_lexer *lexer,px_char ch);
static px_int PX_LexerIsSourcsEnd(px_lexer *lexer);
static px_int PX_LexerIsNewLine(px_char ch);

//CA_Token *GetToken(px_lexer *lexer,pt_string Mnemonic);
//px_void RegisterToken(pt_string Mnemonic,px_uint Type);



//----------------------------------------------------------------------------------

//-------------------------------------------Lexer----------------------------------

px_bool PX_LexerIsCharNumeric(px_char chr)
{
	if (chr>=('0')&&chr<=('9'))
	{
		return PX_TRUE;
	}
	return PX_FALSE;
}

px_bool PX_LexerIsStringNumeric(px_string *pString)
{
	return PX_StringIsNumeric(pString);
}



// -------------------------------------------New Line-------------------------------
px_bool PX_LexerIsNewLine(px_char ch)
{
	return ch=='\r'||ch=='\n';
}

//-------------------------------------------Comment----------------------------------


px_char * PX_LexerIsCommentStart(px_lexer *lexer,const px_char ch[])
{
	px_int i=0;
	for (i=0;i<lexer->CommentCount;i++)
	{
		if(PX_memequ(ch,lexer->Comment[i].CommentBegin,PX_strlen(lexer->Comment[i].CommentBegin)))
			return lexer->Comment[i].CommentBegin;
	}
	return PX_NULL;
}

px_char* PX_LexerIsDiscard(px_lexer* lexer, const px_char ch[])
{
	px_int i = 0;
	for (i = 0; i < lexer->DiscardCount; i++)
	{
		if (PX_memequ(ch, lexer->Discard[i].discard, PX_strlen(lexer->Discard[i].discard)))
			return lexer->Discard[i].discard;
	}
	return PX_NULL;
}

px_char * PX_LexerIsCommentEnd(px_lexer *lexer,const px_char startch[],const px_char ch[])
{
	px_int i=0;
	for (i=0;i<lexer->CommentCount;i++)
	{
		if(PX_memequ(startch,lexer->Comment[i].CommentBegin,PX_strlen(lexer->Comment[i].CommentBegin))&&PX_memequ(ch,lexer->Comment[i].CommentEnd,PX_strlen(lexer->Comment[i].CommentEnd)))
			{
				lexer->CurrentCommentType=i;
				return lexer->Comment[i].CommentEnd;
		    }
	}
	return PX_NULL;
}


//--------------------------------------------Spacer-----------------------------------


px_void PX_LexerRegisterSpacer(px_lexer *lexer,px_char Spacer)
{
	px_int i;
	if (lexer->SpacerCount<PX_LEXER_CA_SPACER_MAX_COUNT)
	{
		for (i=0;i<lexer->SpacerCount;i++)
		{
			if (lexer->Spacer[i]==Spacer)
			{
				return;
			}
		}
		lexer->Spacer[lexer->SpacerCount++]=Spacer;
	}
}

px_bool PX_LexerIsSpacer(px_lexer *lexer,px_char chr)
{
	px_int i;
	for (i=0;i<lexer->SpacerCount;i++)
	{
		if (lexer->Spacer[i]==chr)
		{
			return PX_TRUE;
		}
	}
	return PX_FALSE;
}
//--------------------------------------------Delimiter--------------------------------


px_int PX_LexerRegisterDelimiter(px_lexer *lexer,px_char Delimiter)
{
	px_int i;
	if (lexer->DelimiterCount<PX_LEXER_CA_DELIMITER_MAX_COUNT)
	{
		for (i=0;i<lexer->DelimiterCount;i++)
		{
			if (lexer->Delimiter[i]==Delimiter)
			{
				return i;
			}
		}
		lexer->Delimiter[lexer->DelimiterCount++]=Delimiter;
		return lexer->DelimiterCount-1;
	}
	return -1;
}

px_int PX_LexerRegisterDiscard(px_lexer* lexer, px_char discard[8])
{
	px_int i;
	if (lexer->DiscardCount < PX_LEXER_CA_DISCARD_MAX_COUNT)
	{
		for (i = 0; i < lexer->DiscardCount; i++)
		{
			if (PX_strequ(lexer->Discard[i].discard, discard))
			{
				return i;
			}
		}
		PX_strcpy(lexer->Discard[lexer->DiscardCount].discard, discard, sizeof(lexer->Discard[lexer->DiscardCount]));
		lexer->DiscardCount++;
		return lexer->DiscardCount - 1;
	}
	return -1;
	
}


px_int PX_LexerIsDelimiter(px_lexer *lexer,px_char chr)
{
	px_int i;
	for (i=0;i<lexer->DelimiterCount;i++)
	{
		if (lexer->Delimiter[i]==chr)
		{
			return PX_TRUE;
		}
	}
	return PX_FALSE;
}

px_int PX_LexerGetDelimiterType(px_lexer *lexer,px_char Delimiter)
{
	px_int i;
	for (i=0;i<lexer->DelimiterCount;i++)
	{
		if (lexer->Delimiter[i]==Delimiter)
		{
			return i;
		}
	}
	return -1;
}

//--------------------------------------------Container---------------------------------



px_int PX_LexerRegisterContainer(px_lexer *lexer,const px_char Begin[],const px_char End[])
{
	px_int i;
	if (lexer->ContainerCount<PX_LEXER_CA_DELIMITER_MAX_COUNT)
	{
		for (i=0;i<lexer->ContainerCount;i++)
		{
			if (lexer->Container[i].ContainerBegin==Begin&&lexer->Container[i].ContainerEnd==End)
			{
				return i;
			}
		}
		PX_strcpy(lexer->Container[lexer->ContainerCount].ContainerBegin,Begin,sizeof(lexer->Container[lexer->ContainerCount].ContainerBegin));
		PX_strcpy(lexer->Container[lexer->ContainerCount].ContainerEnd,End,sizeof(lexer->Container[lexer->ContainerCount].ContainerEnd));
		lexer->ContainerCount++;
		return lexer->ContainerCount-1;
	}
	return -1;
}


px_void PX_LexerRegisterContainerTransfer(px_lexer *lexer,px_uint containerIndex,px_char transfer)
{
	if(containerIndex<PX_LEXER_CA_CONTAINER_MAX_COUNT)
	lexer->Container[containerIndex].transfer=transfer;
}

px_int PX_LexerGetContainerType(px_lexer *lexer,px_char *pContainerText)
{
	return lexer->CurrentContainerType;
}

px_char * PX_LexerIsContainerStart(px_lexer *lexer,const px_char chr[])
{
	px_int i=0;
	for (i=0;i<lexer->ContainerCount;i++)
	{
		if(PX_memequ(chr,lexer->Container[i].ContainerBegin,PX_strlen(lexer->Container[i].ContainerBegin)))
			return lexer->Container[i].ContainerBegin;
	}
	return PX_NULL;
}

px_char * PX_LexerIsContainerEnd(px_lexer *lexer,const px_char startch[],const px_char ch[])
{
	px_int i=0;
	for (i=0;i<lexer->ContainerCount;i++)
	{
		if(PX_memequ(startch,lexer->Container[i].ContainerBegin,PX_strlen(startch))&&PX_memequ(ch,lexer->Container[i].ContainerEnd,PX_strlen(lexer->Container[i].ContainerEnd)))
			{
				lexer->CurrentContainerType=i;
				return lexer->Container[i].ContainerEnd;
			}
	}
	return PX_NULL;
}


//--------------------------------------------TOKEN-------------------------------------
// 
// 
// CA_Token *Pt_Lexer::GetToken(pt_string Mnemonic)
// {
// 	for (px_uint i=0;i<lexer->Tokens.size();i++)
// 	{
// 		if (strcmp(Mnemonic,lexer->Tokens[i].Mnemonic)==0)
// 		{
// 			return &lexer->Tokens[i];
// 		}
// 	}
// 	return NULL;
// }
// 
// 
// px_void  Pt_Lexer::RegisterToken(pt_string Mnemonic,px_uint Type)
// {
// 
// 	if (GetToken(Mnemonic)!=NULL)
// 	{
// 		return;
// 	}
// 
// 	CA_Token Token;
// 	PT_STRING_COPY(Token.Mnemonic,sizeof(Token.Mnemonic),Mnemonic);
// 	Token.Type=Type;
// 
// 	lexer->Tokens.push_back(Token);
// 
// }
px_void PX_LexerInitialize(px_lexer *lexer,px_memorypool *mp)
{
	PX_memset(lexer, 0, sizeof(px_lexer));
	lexer->CommentCount=0;
	lexer->SpacerCount=0;
	lexer->DelimiterCount=0;
	lexer->ContainerCount=0;
	lexer->SourceOffset=0;
	lexer->Sources=PX_NULL;
	lexer->mp=mp;
	lexer->lexemeTokenCase=PX_LEXER_LEXEME_CASE_NORMAL;
	lexer->NumericMath=PX_FALSE;
	PX_StringInitialize(mp,&lexer->CurLexeme);
	PX_StringInitialize(mp,&lexer->LexerName);
	PX_VectorInitialize(lexer->mp, &lexer->symbolmap, sizeof(px_int), 8);
	PX_memset(lexer->Delimiter,0,sizeof(lexer->Delimiter));
	PX_memset(lexer->Spacer,0,sizeof(lexer->Spacer));
	PX_memset(lexer->Container,0,sizeof(lexer->Container));
	PX_memset(lexer->Comment,0,sizeof(lexer->Comment));

}

px_bool PX_LexerSetName(px_lexer* lexer, const px_char name[])
{
	PX_StringClear(&lexer->LexerName);
	return PX_StringCat(&lexer->LexerName, name);
}

const px_char* PX_LexerGetName(px_lexer* lexer)
{
	return lexer->LexerName.buffer;
}



px_int PX_LexerFilterChar(px_lexer *lexer,px_char ch)
{

	switch(lexer->SortStatus)
	{
	case PX_LEXERSORT_STATUS_NORMAL:
	{
		if (PX_LexerIsSpacer(lexer,ch))
		{
			lexer->SortStatus=PX_LEXER_SORT_STATUS_SPACER;
			return PX_TRUE;
		}
		if (PX_LexerIsNewLine(ch))
		{
			lexer->SortStatus=PX_LEXER_SORT_STATUS_NEWLINE;
			return PX_TRUE;
		}
		return PX_TRUE;
	}
	break;
	case PX_LEXER_SORT_STATUS_SPACER:
	{
		if (PX_LexerIsSpacer(lexer,ch))
		{
			return PX_FALSE;
		}
		if (PX_LexerIsNewLine(ch))
		{
			lexer->SortStatus=PX_LEXER_SORT_STATUS_NEWLINE;
			return PX_TRUE;
		}
		lexer->SortStatus=PX_LEXERSORT_STATUS_NORMAL;
		return PX_TRUE;
	}
		break;

	case PX_LEXER_SORT_STATUS_NEWLINE:
		if (PX_LexerIsSpacer(lexer,ch)||PX_LexerIsNewLine(ch))
		{
			return PX_FALSE;
		}
		lexer->SortStatus=PX_LEXERSORT_STATUS_NORMAL;
		return PX_TRUE;

		case PX_LEXER_SORT_STATUS_COMMENT:
			break;
	}
	return PX_TRUE;
}


px_bool PX_LexerSortTextMap(px_lexer *lexer,const px_char *SourceText,px_bool map)
{
	px_char *chrst,*chred;

	px_int sourceLines=1;
	px_int w_Offset = 0,r_Offset=0;

	px_int lastNewLineStamp = 0;
	px_int lastNewLineIndex = 0;
	px_int currentDstSourceLine=0;

	if (lexer->Sources)
	{
		PX_VectorClear(&lexer->symbolmap);
		MP_Free(lexer->mp,lexer->Sources);
		lexer->SourceOffset=0;
		lexer->SortStatus=PX_LEXERSORT_STATUS_NORMAL;
	}
	if ((lexer->Sources=(px_char *)MP_Malloc(lexer->mp,PX_strlen(SourceText)+1))==PX_NULL)
	{
		return PX_FALSE;
	}
	PX_memset(lexer->Sources,0,PX_strlen(SourceText)+1);
	lexer->SortStatus=PX_LEXER_SORT_STATUS_NEWLINE;


	
	while(SourceText[r_Offset])
	{
		//Comment trim
		if ((chrst=PX_LexerIsCommentStart(lexer,SourceText+r_Offset))!=0)
		{
			r_Offset+=PX_strlen(chrst);
			while (!(chred=PX_LexerIsCommentEnd(lexer,chrst,SourceText+ r_Offset)))
			{
				if (!SourceText[r_Offset])
				{
					return PX_FALSE;
				}

				r_Offset++;
			}
			//special end '\n'
			if (SourceText[r_Offset] == '\n')
			{
				r_Offset += PX_strlen(chred) - 1;
			}
			else
				r_Offset +=PX_strlen(chred);
			continue;
		}

		//container skip
		if ((chrst=PX_LexerIsContainerStart(lexer,SourceText+ r_Offset)) != 0)
		{
			
			do 
			{
				px_int i;
				for (i=0;i< PX_strlen(chrst);i++)
				{
					lexer->Sources[w_Offset] = chrst[i];
					///////////////////////////////////////////
					if (map && w_Offset > 0 && (lexer->Sources[w_Offset - 1] == '\n'))
					{
						for (; lastNewLineStamp < r_Offset; lastNewLineStamp++)
							if (SourceText[lastNewLineStamp] == '\n')
								lastNewLineIndex++;
						PX_VectorPushback(&lexer->symbolmap, &lastNewLineIndex);
						currentDstSourceLine++;
					}
					///////////////////////////////////////////
					w_Offset++;
					r_Offset++;
				}
			} while (0);

			while (!(chred=PX_LexerIsContainerEnd(lexer,chrst,SourceText+ r_Offset)))
			{
				if (SourceText[r_Offset] == '\0')
				{
					return PX_FALSE;
				}

				if(PX_LexerIsContainerTransfer(lexer,chrst,SourceText[r_Offset]) && PX_memequ(SourceText+r_Offset+1, chrst, PX_strlen(chrst)))
				{
					lexer->Sources[w_Offset]=(SourceText[r_Offset]);
					///////////////////////////////////////////
					if (map && w_Offset > 0 && (lexer->Sources[w_Offset - 1] == '\n'))
					{
						for (; lastNewLineStamp < r_Offset; lastNewLineStamp++)
							if (SourceText[lastNewLineStamp] == '\n')
								lastNewLineIndex++;
						PX_VectorPushback(&lexer->symbolmap, &lastNewLineIndex);
						currentDstSourceLine++;
					}
					///////////////////////////////////////////
					w_Offset++;
					r_Offset++;
					lexer->Sources[w_Offset]=(SourceText[r_Offset]);
					///////////////////////////////////////////
					if (map && w_Offset > 0 && (lexer->Sources[w_Offset - 1] == '\n'))
					{
						for (; lastNewLineStamp < r_Offset; lastNewLineStamp++)
							if (SourceText[lastNewLineStamp] == '\n')
								lastNewLineIndex++;
						PX_VectorPushback(&lexer->symbolmap, &lastNewLineIndex);
						currentDstSourceLine++;
					}
					///////////////////////////////////////////
					w_Offset++;
					r_Offset++;
				}
				else
				{
					lexer->Sources[w_Offset] = (SourceText[r_Offset]);
					///////////////////////////////////////////
					if (map && w_Offset > 0 && (lexer->Sources[w_Offset - 1] == '\n'))
					{
						for (; lastNewLineStamp < r_Offset; lastNewLineStamp++)
							if (SourceText[lastNewLineStamp] == '\n')
								lastNewLineIndex++;
						PX_VectorPushback(&lexer->symbolmap, &lastNewLineIndex);
						currentDstSourceLine++;
					}
					///////////////////////////////////////////
					if (lexer->Sources[w_Offset]=='\\') {
						w_Offset++;
						r_Offset++;
						lexer->Sources[w_Offset] = (SourceText[r_Offset]);
					}
					w_Offset++;
					r_Offset++;
				}
			}

			do
			{
				px_int i;
				for (i = 0; i < PX_strlen(chred); i++)
				{
					lexer->Sources[w_Offset] = chred[i];
					///////////////////////////////////////////
					if (map && w_Offset > 0 && (lexer->Sources[w_Offset - 1] == '\n'))
					{
						for (; lastNewLineStamp < r_Offset; lastNewLineStamp++)
							if ( SourceText[lastNewLineStamp] == '\n')
								lastNewLineIndex++;
						PX_VectorPushback(&lexer->symbolmap, &lastNewLineIndex);
						currentDstSourceLine++;
					}
					///////////////////////////////////////////
					w_Offset++;
					r_Offset++;
				}
				lexer->SortStatus = PX_LEXERSORT_STATUS_NORMAL;
			} while (0);
			continue;
		}


		if (PX_LexerFilterChar(lexer,SourceText[r_Offset]))
		{
			if(w_Offset>0)//forward trim
			{
				if (PX_LexerIsNewLine(SourceText[r_Offset]) && PX_LexerIsSpacer(lexer, lexer->Sources[w_Offset - 1]))
				{
					lexer->Sources[w_Offset-1] = (SourceText[r_Offset])=='\r'?'\n': (SourceText[r_Offset]);
				}
				else if (PX_LexerIsDelimiter(lexer, SourceText[r_Offset]) && PX_LexerIsSpacer(lexer, lexer->Sources[w_Offset - 1]))
				{
					lexer->Sources[w_Offset - 1] = (SourceText[r_Offset]);
				}
				else
				{
					lexer->Sources[w_Offset] = (SourceText[r_Offset]) == '\r' ? '\n' : (SourceText[r_Offset]);

					///////////////////////////////////////////
					if (map && w_Offset > 0 && ( lexer->Sources[w_Offset - 1] == '\n'))
					{
						for (; lastNewLineStamp < r_Offset; lastNewLineStamp++)
							if (SourceText[lastNewLineStamp] == '\n')
								lastNewLineIndex++;
						PX_VectorPushback(&lexer->symbolmap, &lastNewLineIndex);
						currentDstSourceLine++;
					}
					///////////////////////////////////////////

					w_Offset++;
				}
			}
			else
			{
				lexer->Sources[w_Offset] = (SourceText[r_Offset]) == '\r' ? '\n' : (SourceText[r_Offset]);

				///////////////////////////////////////////
				if (map)
				{
					for (; lastNewLineStamp < r_Offset; lastNewLineStamp++)
						if (SourceText[lastNewLineStamp] == '\n')
							lastNewLineIndex++;
					PX_VectorPushback(&lexer->symbolmap, &lastNewLineIndex);
					currentDstSourceLine++;
				}
				///////////////////////////////////////////

				w_Offset++;
			}
			
		}
		r_Offset++;
	}
	lexer->Sources[w_Offset]='\0';

	return PX_TRUE;
}

px_bool PX_LexerSortText(px_lexer* lexer, const px_char* SourceText)
{
	return PX_LexerSortTextMap(lexer, SourceText,PX_FALSE);
}


px_int PX_LexerIsSourcsEnd(px_lexer *lexer)
{
	return !lexer->Sources[lexer->SourceOffset];
}


PX_LEXER_LEXEME_TYPE PX_LexerGetNextLexeme(px_lexer *lexer)
{
	px_char *chrst,*chred;
	px_uint i,oft;
	px_bool match;
	PX_StringClear(&lexer->CurLexeme);
	lexer->Symbol=0;
	while (PX_TRUE)
	{
		if (PX_LexerIsSourcsEnd(lexer))
		{
			//printf("<End>\n");
			lexer->Symbol = '0';
			PX_StringCatChar(&lexer->CurLexeme, '\0');
			lexer->CurrentLexemeFlag = PX_LEXER_LEXEME_TYPE_END;
			return PX_LEXER_LEXEME_TYPE_END;
		}

		//comment
		if ((chrst = PX_LexerIsCommentStart(lexer, &lexer->Sources[lexer->SourceOffset])) != 0)
		{
			PX_StringCat(&lexer->CurLexeme, chrst);
			lexer->SourceOffset += PX_strlen(chrst);
			while (PX_TRUE)
			{
				px_char* pdiscard;
				if (PX_LexerIsSourcsEnd(lexer))
				{
					return PX_LEXER_LEXEME_TYPE_COMMENT;
				}

				if ((pdiscard = PX_LexerIsDiscard(lexer, &lexer->Sources[lexer->SourceOffset])) != 0)
				{
					lexer->SourceOffset += PX_strlen(pdiscard);
					//printf("<Discard> %s\n",chrst);
					continue;
				}

				if ((chred = PX_LexerIsCommentEnd(lexer, chrst, &lexer->Sources[lexer->SourceOffset]))!=PX_NULL)
				{
					break;
				}
				if (PX_LexerIsSourcsEnd(lexer))
				{
					return PX_LEXER_LEXEME_TYPE_ERR;
				}
				PX_StringCatChar(&lexer->CurLexeme, lexer->Sources[lexer->SourceOffset]);
				lexer->SourceOffset++;
			}
			PX_StringCat(&lexer->CurLexeme, chred);
			lexer->SourceOffset += PX_strlen(chred);
			//printf("<Comment> %s\n",lexer->CurLexeme);
			lexer->CurrentLexemeFlag = PX_LEXER_LEXEME_TYPE_COMMENT;
			return PX_LEXER_LEXEME_TYPE_COMMENT;
		}

		//container
		if ((chrst = PX_LexerIsContainerStart(lexer, (&lexer->Sources[lexer->SourceOffset]))) != 0)
		{
			PX_StringCat(&lexer->CurLexeme, chrst);
			lexer->SourceOffset += PX_strlen(chrst);
			while (PX_TRUE)
			{
				px_char* pdiscard;
				if (PX_LexerIsSourcsEnd(lexer))
				{
					return PX_LEXER_LEXEME_TYPE_ERR;
				}

				if ((pdiscard = PX_LexerIsDiscard(lexer, &lexer->Sources[lexer->SourceOffset])) != 0)
				{
					lexer->SourceOffset += PX_strlen(pdiscard);
					//printf("<Discard> %s\n",chrst);
					continue;
				}

				if (PX_NULL!=(chred = PX_LexerIsContainerEnd(lexer, chrst, &lexer->Sources[lexer->SourceOffset])))
				{
					break;
				}
				
				if (PX_LexerIsContainerTransfer(lexer, chrst, lexer->Sources[lexer->SourceOffset]) && PX_memequ(&lexer->Sources[lexer->SourceOffset + 1], chrst, PX_strlen(chrst)))
				{
					PX_StringCatChar(&lexer->CurLexeme, lexer->Sources[lexer->SourceOffset++]);
					PX_StringCatChar(&lexer->CurLexeme, lexer->Sources[lexer->SourceOffset++]);
				}
				else {
					if (lexer->Sources[lexer->SourceOffset]=='\\') {
						switch (lexer->Sources[lexer->SourceOffset+1]) {
							case '"': PX_StringCatChar(&lexer->CurLexeme,'\"'); lexer->Sources++;lexer->Sources++;break;
							case '\'': PX_StringCatChar(&lexer->CurLexeme,'\'');lexer->Sources++;lexer->Sources++;break;
							case '\\': PX_StringCatChar(&lexer->CurLexeme,'\\');lexer->Sources++;lexer->Sources++;break;
							case 'n': PX_StringCatChar(&lexer->CurLexeme,'\n'); lexer->Sources++;lexer->Sources++;break;
							case 'r': PX_StringCatChar(&lexer->CurLexeme,'\r'); lexer->Sources++;lexer->Sources++;break;
							case 't': PX_StringCatChar(&lexer->CurLexeme,'\t'); lexer->Sources++;lexer->Sources++;break;
							case 'b': PX_StringCatChar(&lexer->CurLexeme,'\b'); lexer->Sources++;lexer->Sources++;break;
						}
						// }
					}else {
						PX_StringCatChar(&lexer->CurLexeme,lexer->Sources[lexer->SourceOffset++]);
					}
				}
			}
			PX_StringCat(&lexer->CurLexeme, chred);
			lexer->SourceOffset += PX_strlen(chred);

			//printf("<Container> %s\n",lexer->CurLexeme);
			lexer->CurrentLexemeFlag = PX_LEXER_LEXEME_TYPE_CONATINER;
			return PX_LEXER_LEXEME_TYPE_CONATINER;
		}


		if (PX_LexerIsSpacer(lexer, lexer->Sources[lexer->SourceOffset]))
		{
			lexer->Symbol = lexer->Sources[lexer->SourceOffset];
			PX_StringCatChar(&lexer->CurLexeme, lexer->Sources[lexer->SourceOffset]);
			lexer->SourceOffset++;
			//printf("<Spacer>\n");
			lexer->CurrentLexemeFlag = PX_LEXER_LEXEME_TYPE_SPACER;
			return PX_LEXER_LEXEME_TYPE_SPACER;
		}

		if (PX_LexerIsNewLine(lexer->Sources[lexer->SourceOffset]))
		{
			lexer->Symbol = lexer->Sources[lexer->SourceOffset];
			PX_StringCatChar(&lexer->CurLexeme, lexer->Sources[lexer->SourceOffset]);
			lexer->SourceOffset++;
			//printf("<New line>\n");
			lexer->CurrentLexemeFlag = PX_LEXER_LEXEME_TYPE_NEWLINE;
			return PX_LEXER_LEXEME_TYPE_NEWLINE;
		}

		if (PX_LexerIsDelimiter(lexer, lexer->Sources[lexer->SourceOffset]))
		{
			lexer->Symbol = lexer->Sources[lexer->SourceOffset];
			for (i = 0; (px_int)i < lexer->DelimiterCount; i++)
			{
				if (lexer->Symbol == lexer->Delimiter[i])
				{
					lexer->CurrentDelimiterType = i;
					break;
				}
			}

			PX_StringCatChar(&lexer->CurLexeme, lexer->Sources[lexer->SourceOffset]);
			//printf("<Delimiter> %c\n",lexer->Sources[lexer->SourceOffset]);
			lexer->SourceOffset++;
			lexer->CurrentLexemeFlag = PX_LEXER_LEXEME_TYPE_DELIMITER;
			return PX_LEXER_LEXEME_TYPE_DELIMITER;
		}

		//token
		PX_StringClear(&lexer->CurLexeme);
		lexer->Symbol = '\0';
		while (PX_TRUE)
		{
			px_char* pdiscard;
			if (PX_LexerIsSourcsEnd(lexer))
				break;

			if ((pdiscard = PX_LexerIsDiscard(lexer, &lexer->Sources[lexer->SourceOffset])) != 0)
			{
				lexer->SourceOffset += PX_strlen(pdiscard);
				continue;
			}
			if (PX_LexerIsDelimiter(lexer, lexer->Sources[lexer->SourceOffset]))
				break;
			if (PX_LexerIsSpacer(lexer, lexer->Sources[lexer->SourceOffset]))
				break;
			if (PX_LexerIsNewLine(lexer->Sources[lexer->SourceOffset]))
				break;
			if (PX_LexerIsContainerStart(lexer, &lexer->Sources[lexer->SourceOffset]))
				break;

			PX_StringCatChar(&lexer->CurLexeme, lexer->Sources[lexer->SourceOffset++]);
			oft = lexer->SourceOffset;

			if (lexer->NumericMath && lexer->Sources[lexer->SourceOffset] == '.' && PX_LexerIsDelimiter(lexer, '.'))
			{
				match = PX_TRUE;
				if (PX_StringIsNumeric(&lexer->CurLexeme) && lexer->Sources[lexer->SourceOffset] == '.')
				{
					lexer->SourceOffset++;
					while (PX_TRUE)
					{
						if (PX_LexerIsSourcsEnd(lexer))
							break;

						if ((pdiscard = PX_LexerIsDiscard(lexer, &lexer->Sources[lexer->SourceOffset])) != 0)
						{
							lexer->SourceOffset += PX_strlen(pdiscard);
							continue;
						}
						if (PX_LexerIsDelimiter(lexer, lexer->Sources[lexer->SourceOffset]))
							break;
						if (PX_LexerIsSpacer(lexer, lexer->Sources[lexer->SourceOffset]))
							break;
						if (PX_LexerIsNewLine(lexer->Sources[lexer->SourceOffset]))
							break;
						if (PX_LexerIsContainerStart(lexer, &lexer->Sources[lexer->SourceOffset]))
							break;

						if (lexer->Sources[lexer->SourceOffset] >= '0' && lexer->Sources[lexer->SourceOffset] <= '9')
						{
							lexer->SourceOffset++;
							continue;
						}
						else
						{
							match = PX_FALSE;
							break;
						}
					}
				}
				else
					match = PX_FALSE;

				if (lexer->SourceOffset == oft + 1)
				{
					match = PX_FALSE;
				}

				lexer->SourceOffset = oft;
				if (match)
				{
					PX_StringCatChar(&lexer->CurLexeme, lexer->Sources[lexer->SourceOffset++]);
					while (PX_TRUE)
					{
						if (PX_LexerIsSourcsEnd(lexer))
							break;

						if ((pdiscard = PX_LexerIsDiscard(lexer, &lexer->Sources[lexer->SourceOffset])) != 0)
						{
							lexer->SourceOffset += PX_strlen(pdiscard);
							continue;
						}
						if (PX_LexerIsDelimiter(lexer, lexer->Sources[lexer->SourceOffset]))
							break;
						if (PX_LexerIsSpacer(lexer, lexer->Sources[lexer->SourceOffset]))
							break;
						if (PX_LexerIsNewLine(lexer->Sources[lexer->SourceOffset]))
							break;
						if (PX_LexerIsContainerStart(lexer, &lexer->Sources[lexer->SourceOffset]))
							break;
						PX_StringCatChar(&lexer->CurLexeme, lexer->Sources[lexer->SourceOffset++]);
					}
						
				}
			}
		}
		//printf("<Token> %s\n",m_CurLexeme);
		if (lexer->lexemeTokenCase == PX_LEXER_LEXEME_CASE_UPPER)
		{
			PX_strupr(lexer->CurLexeme.buffer);
		}

		if (lexer->lexemeTokenCase == PX_LEXER_LEXEME_CASE_LOWER)
		{
			PX_strlwr(lexer->CurLexeme.buffer);
		}
		lexer->CurrentLexemeFlag = PX_LEXER_LEXEME_TYPE_TOKEN;
		return PX_LEXER_LEXEME_TYPE_TOKEN;
	}
	
	return PX_LEXER_LEXEME_TYPE_ERR;
}

PX_LEXER_LEXEME_TYPE PX_LexerPreviewNextLexeme(px_lexer* lexer)
{
	PX_LEXER_STATE state;
	state = PX_LexerGetState(lexer);
	PX_LEXER_LEXEME_TYPE type = PX_LexerGetNextLexeme(lexer);
	PX_LexerSetState(state);
	return type;
}

px_bool PX_LexerReadString(px_lexer *lexer,px_string *str,px_uint size)
{
	PX_StringClear(str);
	while (size)
	{
		if (PX_LexerIsSourcsEnd(lexer))
		{
			return PX_FALSE;
		}
		PX_StringCatChar(str,lexer->Sources[lexer->SourceOffset++]);
		size--;
	}
	return PX_TRUE;
}

px_void PX_LexerGetIncludedString(px_lexer *lexer,px_string *str)
{
	px_int left,right;
	if(str!=&lexer->CurLexeme)
	PX_StringCopy(str,&lexer->CurLexeme);
	
	if (lexer->CurrentLexemeFlag==PX_LEXER_LEXEME_TYPE_CONATINER)
	{
		left=PX_strlen(lexer->Container[lexer->CurrentContainerType].ContainerBegin);
		right=PX_strlen(lexer->Container[lexer->CurrentContainerType].ContainerEnd);
		PX_StringTrimRight(str,right);
		PX_StringTrimLeft(str,left);
	}
}


PX_LEXER_LEXEME_TYPE PX_LexerGetCurrentLexeme(px_lexer *lexer)
{
	return lexer->CurrentLexemeFlag;
}

px_void PX_LexerRegisterComment(px_lexer *lexer,const px_char Begin[],const px_char End[] )
{
	px_int i;
	if (lexer->CommentCount<PX_LEXER_CA_COMMENT_MAX_COUNT)
	{
		for (i=0;i<lexer->CommentCount;i++)
		{
			if (lexer->Comment[i].CommentBegin==Begin&&lexer->Comment[i].CommentEnd==End)
			{
				return;
			}
		}
		PX_strcpy(lexer->Comment[lexer->CommentCount].CommentBegin,Begin,sizeof(lexer->Comment[lexer->CommentCount].CommentBegin));
		PX_strcpy(lexer->Comment[lexer->CommentCount].CommentEnd,End,sizeof(lexer->Comment[lexer->CommentCount].CommentEnd));
		lexer->CommentCount++;
	}
}


px_int PX_LexerLoadSourceWithPresort(px_lexer *lexer,const px_char *buffer)
{
		return PX_LexerSortText(lexer,buffer);
}

px_bool PX_LexerLoadSource(px_lexer* lexer, const px_char* SourceText)
{
	if (lexer->Sources)
	{
		PX_VectorClear(&lexer->symbolmap);
		MP_Free(lexer->mp, lexer->Sources);
		lexer->SourceOffset = 0;
		lexer->SortStatus = PX_LEXERSORT_STATUS_NORMAL;
	}
	if ((lexer->Sources = (px_char*)MP_Malloc(lexer->mp, PX_strlen(SourceText) + 1)) == PX_NULL)
	{
		return PX_FALSE;
	}
	PX_memcpy(lexer->Sources, SourceText, PX_strlen(SourceText) + 1);
	lexer->SourceOffset = 0;
	lexer->SortStatus = PX_LEXERSORT_STATUS_NORMAL;
	return PX_TRUE;
}


px_void PX_LexerFree(px_lexer *lexer)
{
	if(lexer->Sources)
	MP_Free(lexer->mp,lexer->Sources);

	PX_VectorFree(&lexer->symbolmap);

	PX_StringFree(&lexer->CurLexeme);
	PX_StringFree(&lexer->LexerName);
	lexer->Sources=PX_NULL;
}

px_bool PX_LexerIsEnd(px_lexer* lexer)
{
	return PX_LexerIsSourcsEnd(lexer);
}

px_int PX_LexerGetCurrentLine(px_lexer* lexer)
{
	px_int i;
	px_int line = 0;
	for (i = 0; i < lexer->SourceOffset; i++)
	{
		if (lexer->Sources[i] == '\n')
		{
			line++;
		}
	}
	return line;
	
}

px_int PX_LexerGetCurrentColumn(px_lexer* lexer)
{
	px_int i;
	px_int column = 0;
	for (i = 0; i < lexer->SourceOffset; i++)
	{
		if (lexer->Sources[i] == '\n')
		{
			column = 0;
		}
		else
		{
			column++;
		}
	}
	return column;
	
}

px_void  PX_LexerGetLexemeString(px_lexer *lexer,px_string *str)
{
	PX_StringCopy(str,&lexer->CurLexeme);
}

px_char PX_LexerGetSymbol(px_lexer *lexer)
{
	return lexer->Symbol;
}

PX_LEXER_STATE PX_LexerGetState(px_lexer *lexer)
{
	PX_LEXER_STATE state;
	state.plexer=lexer;
	state.offset=lexer->SourceOffset;
	return state;
}

px_void PX_LexerSetState(PX_LEXER_STATE state)
{
	state.plexer->SourceOffset=state.offset;
}

px_char PX_LexerGetNextChar(px_lexer *lexer)
{
	px_char ch;
	PX_StringClear(&lexer->CurLexeme);
	ch=lexer->Sources[lexer->SourceOffset];
	lexer->SourceOffset++;
	return ch;
}

px_char PX_LexerPreviewNextChar(px_lexer* lexer)
{
	px_char ch;
	PX_StringClear(&lexer->CurLexeme);
	ch = lexer->Sources[lexer->SourceOffset];
	return ch;
}

px_bool PX_LexerIsLememeIsNumeric(px_lexer *lexer)
{
	return PX_StringIsNumeric(&lexer->CurLexeme);
}

px_void PX_LexerSetTokenCase(px_lexer *lexer,PX_LEXER_LEXEME_CASE _case)
{
	lexer->lexemeTokenCase=_case;
}

px_bool PX_LexerSetSourcePointer(px_lexer *lexer,const px_char *buffer)
{
	lexer->Sources=(px_char *)buffer;
	return PX_TRUE;
}

px_int PX_LexerGetCurrentContainerType(px_lexer *lexer)
{
	return lexer->CurrentContainerType;
}

px_int PX_LexerGetCurrentDelimiterType(px_lexer *lexer)
{
	return lexer->CurrentDelimiterType;
}

px_void PX_LexerSetNumericMatch(px_lexer *lexer,px_bool b)
{
	lexer->NumericMath=b;
}

const px_char* PX_LexerGetLexeme(px_lexer* lexer)
{
	return lexer->CurLexeme.buffer;
}

px_void PX_LexerReset(px_lexer* lexer)
{
	lexer->SourceOffset = 0;
}

px_void PX_LexerSetOffset(px_lexer* lexer, px_int offset)
{
	if (offset < PX_strlen(lexer->Sources))
	{
		lexer->SourceOffset = offset;
	}
}

px_char PX_LexerGetDelimiter(px_lexer* lexer)
{
	PX_ASSERTIF(!lexer->CurLexeme.buffer);
	return lexer->CurLexeme.buffer[0];
}


px_bool PX_LexerIsContainerTransfer(px_lexer *lexer,const px_char startch[],px_char ch)
{
	px_int i=0;
	for (i=0;i<lexer->ContainerCount;i++)
	{
		if(PX_memequ(startch,lexer->Container[i].ContainerBegin,PX_strlen(startch))&&lexer->Container[i].transfer&&lexer->Container[i].transfer==ch)
		{
			return PX_TRUE;
		}
	}
	return PX_FALSE;
}

