#include "./PX_SyntaxLexer.h"

static px_char * PX_SyntaxLexerIsCommentStart(px_syntaxlexer *plexer,const px_char ch[]);
static px_char * PX_SyntaxLexerIsCommentEnd(px_syntaxlexer *plexer,const px_char startch[],const px_char ch[]);
static px_char* PX_SyntaxLexerIsContainerStart(px_syntaxlexer *plexer,const px_char ch[]);
static char*  PX_SyntaxLexerIsContainerEnd(px_syntaxlexer *plexer,const px_char startch[],const px_char ch[]);
static px_bool PX_SyntaxLexerIsContainerTransfer(px_syntaxlexer *plexer,const px_char startch[],px_char ch);

static px_int PX_SyntaxLexerIsSpacer(px_syntaxlexer *plexer,px_char chr);
static px_int PX_SyntaxLexerIsDelimiter(px_syntaxlexer *plexer,px_char chr);

static px_int PX_SyntaxLexerIsNewLine(px_char ch);


px_bool PX_SyntaxLexerIsNewLine(px_char ch)
{
	return ch=='\r'||ch=='\n';
}


px_char * PX_SyntaxLexerIsCommentStart(px_syntaxlexer *plexer,const px_char ch[])
{
	px_int i=0;
	for (i=0;i<plexer->comment_count;i++)
	{
		if(PX_memequ(ch,plexer->comment[i].CommentBegin,PX_strlen(plexer->comment[i].CommentBegin)))
			return plexer->comment[i].CommentBegin;
	}
	return PX_NULL;
}

px_char* PX_SyntaxLexerIsDiscard(px_syntaxlexer* plexer, const px_char ch[])
{
	px_int i = 0;
	for (i = 0; i < plexer->discard_count; i++)
	{
		if (PX_memequ(ch, plexer->discard[i].discard, PX_strlen(plexer->discard[i].discard)))
			return plexer->discard[i].discard;
	}
	return PX_NULL;
}

px_char * PX_SyntaxLexerIsCommentEnd(px_syntaxlexer *plexer,const px_char startch[],const px_char ch[])
{
	px_int i=0;
	for (i=0;i<plexer->comment_count;i++)
	{
		if(PX_memequ(startch,plexer->comment[i].CommentBegin,PX_strlen(plexer->comment[i].CommentBegin))&&PX_memequ(ch,plexer->comment[i].CommentEnd,PX_strlen(plexer->comment[i].CommentEnd)))
			{
				plexer->current_comment_type=i;
				return plexer->comment[i].CommentEnd;
		    }
	}
	return PX_NULL;
}

px_void PX_SyntaxLexer_RegisterSpacer(px_syntaxlexer *plexer,px_char spacer)
{
	px_int i;
	if (plexer->spacerCount<PX_SYNTAX_LEXER_CA_SPACER_MAX_COUNT)
	{
		for (i=0;i<plexer->spacerCount;i++)
		{
			if (plexer->spacer[i]==spacer)
			{
				return;
			}
		}
		plexer->spacer[plexer->spacerCount++]=spacer;
	}
}

px_bool PX_SyntaxLexerIsSpacer(px_syntaxlexer *plexer,px_char chr)
{
	px_int i;
	for (i=0;i<plexer->spacerCount;i++)
	{
		if (plexer->spacer[i]==chr)
		{
			return PX_TRUE;
		}
	}
	return PX_FALSE;
}

px_int PX_SyntaxLexer_RegisterDelimiter(px_syntaxlexer *plexer,px_char delimiter)
{
	px_int i;
	if (plexer->delimiter_count<PX_SYNTAX_LEXER_CA_DELIMITER_MAX_COUNT)
	{
		for (i=0;i<plexer->delimiter_count;i++)
		{
			if (plexer->delimiter[i]==delimiter)
			{
				return i;
			}
		}
		plexer->delimiter[plexer->delimiter_count++]=delimiter;
		return plexer->delimiter_count-1;
	}
	return -1;
}

px_int PX_SyntaxLexer_RegisterDiscard(px_syntaxlexer* plexer, const px_char *discard)
{
	px_int i;
	if (plexer->discard_count < PX_SYNTAX_LEXER_CA_DISCARD_MAX_COUNT)
	{
		for (i = 0; i < plexer->discard_count; i++)
		{
			if (PX_strequ(plexer->discard[i].discard, discard))
			{
				return i;
			}
		}
		PX_strcpy(plexer->discard[plexer->discard_count].discard, discard, sizeof(plexer->discard[plexer->discard_count]));
		plexer->discard_count++;
		return plexer->discard_count - 1;
	}
	return -1;
	
}

px_int PX_SyntaxLexerIsDelimiter(px_syntaxlexer *plexer,px_char chr)
{
	px_int i;
	for (i=0;i<plexer->delimiter_count;i++)
	{
		if (plexer->delimiter[i]==chr)
		{
			return PX_TRUE;
		}
	}
	return PX_FALSE;
}

px_int PX_SyntaxLexer_GetDelimiterType(px_syntaxlexer *plexer,px_char delimiter)
{
	px_int i;
	for (i=0;i<plexer->delimiter_count;i++)
	{
		if (plexer->delimiter[i]==delimiter)
		{
			return i;
		}
	}
	return -1;
}


px_int PX_SyntaxLexer_RegisterContainer(px_syntaxlexer *plexer,const px_char Begin[],const px_char End[])
{
	px_int i;
	if (plexer->container_count<PX_SYNTAX_LEXER_CA_CONTAINER_MAX_COUNT)
	{
		for (i=0;i<plexer->container_count;i++)
		{
			if (PX_strequ(plexer->container[i].ContainerBegin,Begin)&&PX_strequ(plexer->container[i].ContainerEnd,End))
			{
				return i;
			}
		}
		PX_strcpy(plexer->container[plexer->container_count].ContainerBegin,Begin,sizeof(plexer->container[plexer->container_count].ContainerBegin));
		PX_strcpy(plexer->container[plexer->container_count].ContainerEnd,End,sizeof(plexer->container[plexer->container_count].ContainerEnd));
		plexer->container_count++;
		return plexer->container_count-1;
	}
	return -1;
}


px_void PX_SyntaxLexer_RegisterContainerTransfer(px_syntaxlexer *plexer,px_uint containerIndex,px_char transfer)
{
	if(containerIndex<PX_SYNTAX_LEXER_CA_CONTAINER_MAX_COUNT)
	plexer->container[containerIndex].transfer=transfer;
}

px_int PX_SyntaxLexer_GetContainerType(px_syntaxlexer *plexer,px_char *pContainerText)
{
	return plexer->current_container_type;
}

px_char * PX_SyntaxLexerIsContainerStart(px_syntaxlexer *plexer,const px_char chr[])
{
	px_int i=0;
	for (i=0;i<plexer->container_count;i++)
	{
		if(PX_memequ(chr,plexer->container[i].ContainerBegin,PX_strlen(plexer->container[i].ContainerBegin)))
			return plexer->container[i].ContainerBegin;
	}
	return PX_NULL;
}

px_char * PX_SyntaxLexerIsContainerEnd(px_syntaxlexer *plexer,const px_char startch[],const px_char ch[])
{
	px_int i=0;
	for (i=0;i<plexer->container_count;i++)
	{
		if(PX_memequ(startch,plexer->container[i].ContainerBegin,PX_strlen(startch))&&PX_memequ(ch,plexer->container[i].ContainerEnd,PX_strlen(plexer->container[i].ContainerEnd)))
			{
				plexer->current_container_type=i;
				return plexer->container[i].ContainerEnd;
			}
	}
	return PX_NULL;
}


px_bool PX_SyntaxLexer_Initialize(px_memorypool* mp,px_syntaxlexer *plexer)
{
	PX_memset(plexer, 0, sizeof(px_syntaxlexer));
	plexer->comment_count=0;
	plexer->spacerCount=0;
	plexer->delimiter_count=0;
	plexer->container_count=0;
	plexer->mp=mp;
	plexer->lexeme_token_case=PX_SYNTAX_LEXER_LEXEME_CASE_NORMAL;
	PX_StringInitialize(mp,&plexer->current_lexeme);
	PX_memset(plexer->delimiter,0,sizeof(plexer->delimiter));
	PX_memset(plexer->spacer,0,sizeof(plexer->spacer));
	PX_memset(plexer->container,0,sizeof(plexer->container));
	PX_memset(plexer->comment,0,sizeof(plexer->comment));
	return PX_VectorInitialize(mp, &plexer->sources, sizeof(PX_SyntaxLexer_Source), 0);
}


px_bool PX_SyntaxLexer_IsEnd(px_syntaxlexer * plexer)
{
	if (plexer->callstack_count <= 0)
		return PX_TRUE;
		
	if (plexer->callstack_count == 1)
	{
		PX_SyntaxLexer_Source* psource;
		PX_SyntaxLexer_Call* pcall = &plexer->call[0];
		PX_ASSERTIFX(pcall->source_index >= plexer->sources.size, "Error: Call source not exist");
		psource = PX_VECTORAT(PX_SyntaxLexer_Source, &plexer->sources, pcall->source_index);
		PX_ASSERTIFX(psource == PX_NULL, "Error: Call source data crash");
		return psource->source[pcall->read_offset] == '\0';
	}
	return PX_FALSE;
}

px_int PX_SyntaxLexer_GetCurrentSourceOffset(px_syntaxlexer* plexer)
{
	if (plexer->callstack_count != 0)
	{
		return plexer->call[plexer->callstack_count - 1].read_offset;
	}
	return -1;
	
}


px_char PX_SyntaxLexerGetCurrentSourceChar(px_syntaxlexer* plexer)
{
	if (!PX_SyntaxLexer_IsEnd(plexer))
	{
		PX_SyntaxLexer_Call* pcall = PX_SyntaxLexer_GetCurrentCall(plexer);
		if (pcall)
		{
			PX_SyntaxLexer_Source* psource = PX_SyntaxLexer_GetCurrentSource(plexer);
			PX_ASSERTIFX(psource == PX_NULL, "Error: Current source is null");
			return psource->source[pcall->read_offset];
		}
		else
		{
			PX_ASSERTX("Error: Call stack crash");
			return '\0';
		}
	}
	else
	{
		return '\0';
	}
}

const px_char *PX_SyntaxLexerGetCurrentSourcePointer(px_syntaxlexer* plexer)
{
	PX_SyntaxLexer_Source* psource = PX_SyntaxLexer_GetCurrentSource(plexer);
	if (psource)
	{
		return psource->source + PX_SyntaxLexer_GetCurrentSourceOffset(plexer);
	}
	else
	{
		PX_ASSERTX("Error: Current source is null");
		return PX_NULL;
	}
}

px_void PX_SyntaxLexer_Forward(px_syntaxlexer* plexer, px_int inc)
{
	PX_SyntaxLexer_Source* psource=PX_NULL;
	PX_SyntaxLexer_Call* pcall=PX_NULL;

	PX_ASSERTIFX(inc <= 0, "Error: Invalid forward offset");

	if (plexer->callstack_count <= 0)
	{
		PX_ASSERTX("Error: Call stack crash");
		return;
	}

	while(PX_TRUE)
	{
		//update current source offset
		pcall = &plexer->call[plexer->callstack_count - 1];
		if (pcall->source_index >= plexer->sources.size)
		{
			PX_ASSERTX("Error: Call source not exist");
			return;
		}
		psource = PX_VECTORAT(PX_SyntaxLexer_Source, &plexer->sources, pcall->source_index);
		if (psource == PX_NULL)
		{
			PX_ASSERTX("Error: Call source data crash");
			return;
		}

		while (PX_TRUE)
		{
			if (psource->source[pcall->read_offset] == '\0')
			{
				if (plexer->callstack_count > 1)
				{
					plexer->callstack_count--;
					break;
				}
				else if (plexer->callstack_count == 1)
				{
					//end of all source
					return;
				}
			}
			if (!inc)
			{
				return;
			}
			pcall->read_offset++;
			inc--;
		}
	}
}


PX_SYNTAXLEXER_LEXEME_TYPE PX_SyntaxLexer_GetNextLexeme(px_syntaxlexer *plexer)
{
	px_char *chrst,*chred;
	px_uint i;
	PX_StringClear(&plexer->current_lexeme);
	plexer->symbol=0;
	while (PX_TRUE)
	{
		if (PX_SyntaxLexer_IsEnd(plexer))
		{
			//printf("<End>\n");
			plexer->lexeme_begin = PX_SyntaxLexer_GetCurrentSourceOffset(plexer);
			plexer->lexeme_end = PX_SyntaxLexer_GetCurrentSourceOffset(plexer);
			plexer->symbol = '\0';
			PX_StringCatChar(&plexer->current_lexeme, '\0');
			plexer->current_lexeme_type = PX_SYNTAXLEXER_LEXEME_TYPE_END;
			return PX_SYNTAXLEXER_LEXEME_TYPE_END;
		}

		//comment
		if ((chrst = PX_SyntaxLexerIsCommentStart(plexer, PX_SyntaxLexerGetCurrentSourcePointer(plexer))) != 0)
		{
			plexer->lexeme_begin = PX_SyntaxLexer_GetCurrentSourceOffset(plexer);
			plexer->lexeme_begin_source_index = PX_SyntaxLexer_GetCurrentSourceIndex(plexer);
			PX_StringCat(&plexer->current_lexeme, chrst);
			//plexer->SourceOffset += PX_strlen(chrst);
			PX_SyntaxLexer_Forward(plexer, PX_strlen(chrst));
			while (PX_TRUE)
			{
				px_char* pdiscard;
				if (PX_SyntaxLexer_IsEnd(plexer))
				{
					plexer->lexeme_end = PX_SyntaxLexer_GetCurrentSourceOffset(plexer)-1;
					plexer->lexeme_end_source_index = PX_SyntaxLexer_GetCurrentSourceIndex(plexer);
					return PX_SYNTAXLEXER_LEXEME_TYPE_COMMENT;
				}

				if ((pdiscard = PX_SyntaxLexerIsDiscard(plexer, PX_SyntaxLexerGetCurrentSourcePointer(plexer))) != 0)
				{
					//plexer->SourceOffset += PX_strlen(pdiscard);
					PX_SyntaxLexer_Forward(plexer, PX_strlen(pdiscard));
					//printf("<Discard> %s\n",chrst);
					continue;
				}

				if ((chred = PX_SyntaxLexerIsCommentEnd(plexer, chrst, PX_SyntaxLexerGetCurrentSourcePointer(plexer)))!=PX_NULL)
				{
					plexer->lexeme_end = PX_SyntaxLexer_GetCurrentSourceOffset(plexer)-1;
					plexer->lexeme_end_source_index = PX_SyntaxLexer_GetCurrentSourceIndex(plexer);
					break;
				}
				if (PX_SyntaxLexer_IsEnd(plexer))
				{
					plexer->lexeme_end = PX_SyntaxLexer_GetCurrentSourceOffset(plexer)-1;
					plexer->lexeme_end_source_index = PX_SyntaxLexer_GetCurrentSourceIndex(plexer);
					return PX_SYNTAXLEXER_LEXEME_TYPE_ERR;
				}
				PX_StringCatChar(&plexer->current_lexeme, PX_SyntaxLexerGetCurrentSourceChar(plexer));
				
				//plexer->SourceOffset++;
				PX_SyntaxLexer_Forward(plexer, 1);
			}
			PX_StringCat(&plexer->current_lexeme, chred);
			PX_SyntaxLexer_Forward(plexer, PX_strlen(chred));
			//printf("<Comment> %s\n",plexer->CurLexeme);
			plexer->current_lexeme_type = PX_SYNTAXLEXER_LEXEME_TYPE_COMMENT;
			return PX_SYNTAXLEXER_LEXEME_TYPE_COMMENT;
		}

		//container
		if ((chrst = PX_SyntaxLexerIsContainerStart(plexer, PX_SyntaxLexerGetCurrentSourcePointer(plexer))) != 0)
		{
			plexer->lexeme_begin = PX_SyntaxLexer_GetCurrentSourceOffset(plexer);
			plexer->lexeme_begin_source_index = PX_SyntaxLexer_GetCurrentSourceIndex(plexer);
			PX_StringCat(&plexer->current_lexeme, chrst);
			//plexer->SourceOffset += PX_strlen(chrst);
			PX_SyntaxLexer_Forward(plexer, PX_strlen(chrst));
			while (PX_TRUE)
			{
				px_char* pdiscard;
				if (PX_SyntaxLexer_IsEnd(plexer))
				{
					plexer->lexeme_end = PX_SyntaxLexer_GetCurrentSourceOffset(plexer)-1;
					plexer->lexeme_end_source_index = PX_SyntaxLexer_GetCurrentSourceIndex(plexer);
					return PX_SYNTAXLEXER_LEXEME_TYPE_ERR;
				}

				if ((pdiscard = PX_SyntaxLexerIsDiscard(plexer, PX_SyntaxLexerGetCurrentSourcePointer(plexer))) != 0)
				{
					PX_SyntaxLexer_Forward(plexer, PX_strlen(pdiscard));
					//printf("<Discard> %s\n",chrst);
					continue;
				}

				if (PX_NULL!=(chred = PX_SyntaxLexerIsContainerEnd(plexer, chrst, PX_SyntaxLexerGetCurrentSourcePointer(plexer))))
				{
					plexer->lexeme_end = PX_SyntaxLexer_GetCurrentSourceOffset(plexer);
					plexer->lexeme_end_source_index = PX_SyntaxLexer_GetCurrentSourceIndex(plexer);
					break;
				}
				
				if (PX_SyntaxLexerIsContainerTransfer(plexer, chrst, PX_SyntaxLexerGetCurrentSourceChar(plexer)) && PX_memequ(PX_SyntaxLexerGetCurrentSourcePointer(plexer)+1, chrst, PX_strlen(chrst)))
				{
					PX_StringCatChar(&plexer->current_lexeme, PX_SyntaxLexerGetCurrentSourceChar(plexer));
					PX_SyntaxLexer_Forward(plexer, 1);
					PX_StringCatChar(&plexer->current_lexeme, PX_SyntaxLexerGetCurrentSourceChar(plexer));
					PX_SyntaxLexer_Forward(plexer, 1);
				}
				else
				{
					PX_StringCatChar(&plexer->current_lexeme, PX_SyntaxLexerGetCurrentSourceChar(plexer));
					PX_SyntaxLexer_Forward(plexer, 1);
				}
			}
			PX_StringCat(&plexer->current_lexeme, chred);
			PX_SyntaxLexer_Forward(plexer, PX_strlen(chred));

			//printf("<Container> %s\n",plexer->CurLexeme);
			plexer->current_lexeme_type = PX_SYNTAXLEXER_LEXEME_TYPE_CONATINER;
			return PX_SYNTAXLEXER_LEXEME_TYPE_CONATINER;
		}


		if (PX_SyntaxLexerIsSpacer(plexer, PX_SyntaxLexerGetCurrentSourceChar(plexer)))
		{
			plexer->lexeme_begin = PX_SyntaxLexer_GetCurrentSourceOffset(plexer);
			plexer->lexeme_begin_source_index =PX_SyntaxLexer_GetCurrentSourceIndex(plexer);
			plexer->lexeme_end = PX_SyntaxLexer_GetCurrentSourceOffset(plexer);
			plexer->lexeme_end_source_index =PX_SyntaxLexer_GetCurrentSourceIndex(plexer);
			plexer->symbol = PX_SyntaxLexerGetCurrentSourceChar(plexer);
			PX_StringCatChar(&plexer->current_lexeme, PX_SyntaxLexerGetCurrentSourceChar(plexer));
			PX_SyntaxLexer_Forward(plexer, 1);
			//printf("<Spacer>\n");
			plexer->current_lexeme_type = PX_SYNTAXLEXER_LEXEME_TYPE_SPACER;
			return PX_SYNTAXLEXER_LEXEME_TYPE_SPACER;
		}

		if ( PX_SyntaxLexerIsNewLine(PX_SyntaxLexerGetCurrentSourceChar(plexer)))
		{
			plexer->lexeme_begin = PX_SyntaxLexer_GetCurrentSourceOffset(plexer);
			plexer->lexeme_begin_source_index =PX_SyntaxLexer_GetCurrentSourceIndex(plexer);
			plexer->lexeme_end = PX_SyntaxLexer_GetCurrentSourceOffset(plexer);
			plexer->lexeme_end_source_index =PX_SyntaxLexer_GetCurrentSourceIndex(plexer);
			plexer->symbol = PX_SyntaxLexerGetCurrentSourceChar(plexer);
			PX_StringCatChar(&plexer->current_lexeme, PX_SyntaxLexerGetCurrentSourceChar(plexer));
			PX_SyntaxLexer_Forward(plexer, 1);
			//printf("<New line>\n");
			plexer->current_lexeme_type = PX_SYNTAXLEXER_LEXEME_TYPE_NEWLINE;
			return PX_SYNTAXLEXER_LEXEME_TYPE_NEWLINE;
		}

		if (PX_SyntaxLexerIsDelimiter(plexer, PX_SyntaxLexerGetCurrentSourceChar(plexer)))
		{
			plexer->lexeme_begin = PX_SyntaxLexer_GetCurrentSourceOffset(plexer);
			plexer->lexeme_begin_source_index =PX_SyntaxLexer_GetCurrentSourceIndex(plexer);
			plexer->lexeme_end = PX_SyntaxLexer_GetCurrentSourceOffset(plexer);
			plexer->lexeme_end_source_index =PX_SyntaxLexer_GetCurrentSourceIndex(plexer);
			plexer->symbol = PX_SyntaxLexerGetCurrentSourceChar(plexer);
			for (i = 0; (px_int)i < plexer->delimiter_count; i++)
			{
				if (plexer->symbol == plexer->delimiter[i])
				{
					plexer->current_delimiter_type = i;
					break;
				}
			}

			PX_StringCatChar(&plexer->current_lexeme, PX_SyntaxLexerGetCurrentSourceChar(plexer));
			//printf("<Delimiter> %c\n",plexer->Sources[plexer->SourceOffset]);
			PX_SyntaxLexer_Forward(plexer, 1);
			plexer->current_lexeme_type = PX_SYNTAXLEXER_LEXEME_TYPE_DELIMITER;
			return PX_SYNTAXLEXER_LEXEME_TYPE_DELIMITER;
		}

		//token
		PX_StringClear(&plexer->current_lexeme);
		plexer->symbol = '\0';
		plexer->lexeme_begin = PX_SyntaxLexer_GetCurrentSourceOffset(plexer);
		plexer->lexeme_begin_source_index =PX_SyntaxLexer_GetCurrentSourceIndex(plexer);
		plexer->lexeme_end = PX_SyntaxLexer_GetCurrentSourceOffset(plexer);
		plexer->lexeme_end_source_index =PX_SyntaxLexer_GetCurrentSourceIndex(plexer);
		while (PX_TRUE)
		{
			px_char* pdiscard;
			if (PX_SyntaxLexer_IsEnd(plexer))
				break;

			if ( (pdiscard = PX_SyntaxLexerIsDiscard(plexer, PX_SyntaxLexerGetCurrentSourcePointer(plexer))) != 0)
			{
				//plexer->SourceOffset += PX_strlen(pdiscard);
				PX_SyntaxLexer_Forward(plexer, PX_strlen(pdiscard));
				continue;
			}

			if ( PX_SyntaxLexerIsDelimiter(plexer, PX_SyntaxLexerGetCurrentSourceChar(plexer)))
				break;
			if ( PX_SyntaxLexerIsSpacer(plexer, PX_SyntaxLexerGetCurrentSourceChar(plexer)))
				break;
			if ( PX_SyntaxLexerIsNewLine(PX_SyntaxLexerGetCurrentSourceChar(plexer)))
				break;
			if ( PX_SyntaxLexerIsContainerStart(plexer, PX_SyntaxLexerGetCurrentSourcePointer(plexer)))
				break;

			PX_StringCatChar(&plexer->current_lexeme, PX_SyntaxLexerGetCurrentSourceChar(plexer));
			plexer->lexeme_end = PX_SyntaxLexer_GetCurrentSourceOffset(plexer);
			plexer->lexeme_end_source_index =PX_SyntaxLexer_GetCurrentSourceIndex(plexer);
	
			PX_SyntaxLexer_Forward(plexer, 1);
		}
		//printf("<Token> %s\n",m_CurLexeme);
		if (plexer->lexeme_token_case == PX_SYNTAX_LEXER_LEXEME_CASE_UPPER)
		{
			PX_strupr(plexer->current_lexeme.buffer);
		}

		if (plexer->lexeme_token_case == PX_SYNTAX_LEXER_LEXEME_CASE_LOWER)
		{
			PX_strlwr(plexer->current_lexeme.buffer);
		}
		plexer->current_lexeme_type = PX_SYNTAXLEXER_LEXEME_TYPE_TOKEN;
		return PX_SYNTAXLEXER_LEXEME_TYPE_TOKEN;
	}
	
	return PX_SYNTAXLEXER_LEXEME_TYPE_ERR;
}

PX_SYNTAXLEXER_LEXEME_TYPE PX_SyntaxLexer_PreviewNextLexeme(px_syntaxlexer* plexer)
{
	PX_SYNTAXLEXER_STATE state = PX_SyntaxLexer_GetState(plexer);
	PX_SYNTAXLEXER_LEXEME_TYPE type = PX_SyntaxLexer_GetNextLexeme(plexer);
	PX_SyntaxLexer_SetState(plexer, &state);
	return type;
}

px_bool PX_SyntaxLexer_ReadString(px_syntaxlexer *plexer,px_string *str,px_uint size)
{
	PX_StringClear(str);
	while (size)
	{
		if (PX_SyntaxLexer_IsEnd(plexer))
		{
			return PX_FALSE;
		}
		PX_StringCatChar(str, PX_SyntaxLexerGetCurrentSourceChar(plexer));
		PX_SyntaxLexer_Forward(plexer, 1);
		size--;
	}
	return PX_TRUE;
}

px_void PX_SyntaxLexer_GetIncludedString(px_syntaxlexer *plexer,px_string *str)
{
	px_int left,right;
	if(str!=&plexer->current_lexeme)
	PX_StringCopy(str,&plexer->current_lexeme);
	
	if (plexer->current_lexeme_type==PX_SYNTAXLEXER_LEXEME_TYPE_CONATINER)
	{
		left=PX_strlen(plexer->container[plexer->current_container_type].ContainerBegin);
		right=PX_strlen(plexer->container[plexer->current_container_type].ContainerEnd);
		PX_StringTrimRight(str,right);
		PX_StringTrimLeft(str,left);
	}
}


const px_char* PX_SyntaxLexer_GetCurrentLexeme(px_syntaxlexer *plexer)
{
	return plexer->current_lexeme.buffer;
}

const px_char* PX_SyntaxLexer_GetCurrentSourcePointer(px_syntaxlexer* plexer)
{
	PX_SyntaxLexer_Source* psource = PX_SyntaxLexer_GetCurrentSource(plexer);
	if (psource)
	{
		return psource->source + PX_SyntaxLexer_GetCurrentSourceOffset(plexer);
	}
	else
	{
		PX_ASSERTX("Error: Current source is null");
		return PX_NULL;
	}
	
}

px_void PX_SyntaxLexer_RegisterComment(px_syntaxlexer *plexer,const px_char Begin[],const px_char End[] )
{
	px_int i;
	if (plexer->comment_count<PX_SYNTAX_LEXER_CA_COMMENT_MAX_COUNT)
	{
		for (i=0;i<plexer->comment_count;i++)
		{
			if (PX_strequ(plexer->comment[i].CommentBegin,Begin)&&PX_strequ(plexer->comment[i].CommentEnd,End))
			{
				return;
			}
		}
		PX_strcpy(plexer->comment[plexer->comment_count].CommentBegin,Begin,sizeof(plexer->comment[plexer->comment_count].CommentBegin));
		PX_strcpy(plexer->comment[plexer->comment_count].CommentEnd,End,sizeof(plexer->comment[plexer->comment_count].CommentEnd));
		plexer->comment_count++;
	}
}


px_int PX_SyntaxLexer_GetSourceCount(px_syntaxlexer* plexer)
{
	return plexer->sources.size;
}

px_int PX_SyntaxLexer_GetLineByOffset(px_syntaxlexer* plexer, px_int offset)
{
	PX_SyntaxLexer_Source* psource=PX_NULL;
	psource = PX_SyntaxLexer_GetCurrentSource(plexer);
	if (psource)
	{
		if (offset < psource->source_length)
		{
			return psource->source_index_map_to_line_index[offset];
		}
		else if (offset == psource->source_length)
		{
			return psource->line_count-1;
		}
		else
		{
			PX_ASSERTX("Error: PX_SyntaxLexer_GetLineByOffset offset out of range");
			return -1;
		}
	}
	return -1;
}

px_int PX_SyntaxLexer_GetCurrentLine(px_syntaxlexer* plexer)
{
	return PX_SyntaxLexer_GetLineByOffset(plexer, PX_SyntaxLexer_GetCurrentSourceOffset(plexer));
}

px_bool PX_SyntaxLexer_SetCurrentOffset(px_syntaxlexer* plexer, px_int offset)
{
	PX_SyntaxLexer_Source* psource = PX_NULL;
	psource = PX_SyntaxLexer_GetCurrentSource(plexer);
	if (psource)
	{
		if (offset < psource->source_length)
		{
			PX_SyntaxLexer_Call* pcall = PX_SyntaxLexer_GetCurrentCall(plexer);
			if (pcall)
			{
				pcall->read_offset = offset;
				return PX_TRUE;
			}
			else
			{
				PX_ASSERTX("Error: Call stack crash");
				return PX_FALSE;
			}
		}
		else
		{
			PX_ASSERTX("Error: PX_SyntaxLexer_SetCurrentOffset offset out of range");
			return PX_FALSE;
		}
	}

	return PX_FALSE;
}


px_void PX_SyntaxLexer_Free(px_syntaxlexer *plexer)
{
	PX_SyntaxLexer_Clear(plexer);
	PX_VectorFree(&plexer->sources);
	PX_StringFree(&plexer->current_lexeme);
}


px_int PX_SyntaxLexer_AddSource(px_syntaxlexer* plexer, const px_char* name, const px_char* __sourcetext)
{
	PX_SyntaxLexer_Source new_source = { 0 };
	px_int i,j, source_len,filter_len=0,  line_width_counter=0,curline=0;
	px_int source_index = 0, cells_count = 0, cell_index, row_index;

	source_index = PX_SyntaxLexer_GetSourceIndexByName(plexer, name);
	if (source_index!=-1)
	{
		return source_index;
	}

	//copy name
	if ((new_source.name = MP_Malloc(plexer->mp, PX_strlen(name) + 1)) == PX_NULL)
		goto _ERROR;

	PX_strcpy(new_source.name, name, PX_strlen(name) + 1);


	//remove '\r' from source code
	source_len = PX_strlen(__sourcetext);

	//calc source_length after filter '\r' and filter_len
	for (i = 0; i < source_len; i++)
	{
		if (__sourcetext[i] != '\r')
		{
			filter_len++;
		}
	}
	new_source.source_length = filter_len;

	//copy source code to new memory
	if ((new_source.source = (px_char*)MP_Malloc(plexer->mp, new_source.source_length+1)) == PX_NULL)
		goto _ERROR;

	new_source.source[new_source.source_length] = 0;//end of filter source

	j = 0;
	for (i = 0; i < source_len; i++)
	{
		if (__sourcetext[i] != '\r')
		{
			new_source.source[j++] = __sourcetext[i];
		}
	}

	//init source index map to line index

	new_source.line_count = 1;

	//line count/max line char width and set source index map to line index
	i = 0;
	while (i < new_source.source_length)
	{
		if (new_source.source[i] == '\n')
		{
			if (line_width_counter > new_source.max_line_char_width)
			{
				new_source.max_line_char_width = line_width_counter;
			}
			line_width_counter = 0;
			new_source.line_count++;
			cells_count++;
			i++;
		}
		else
		{
			px_int byte_size = PX_FontModuleGetCharacterRawCode(PX_FONTMODULE_CODEPAGE_UTF8, new_source.source + i, PX_NULL); 
			if (byte_size <= 0)
			{
				goto _ERROR;
			}
			line_width_counter += byte_size == 1 ? 1 : 2;
			cells_count++;
			i += byte_size;
			if (i > new_source.source_length)
			{
				//source code error, maybe invalid utf-8 code
				goto _ERROR;
			}
		}
	}

	//create source index map to line index
	if ((new_source.source_index_map_to_line_index = (px_int*)MP_Malloc(plexer->mp, sizeof(px_int) * filter_len)) == PX_NULL)
		goto _ERROR;

	for (i = 0; i < new_source.source_length; i++)
	{
		new_source.source_index_map_to_line_index[i] = curline;
		if (new_source.source[i] == '\n')
		{
			curline++;
		}
	}

	//cell count/cells/line begin cell index map/map to cell index
	new_source.cells_count = cells_count;
	new_source.cells = MP_Malloc(plexer->mp, sizeof(PX_SyntaxLexer_Cell) * cells_count);
	if (!new_source.cells)
	{
		goto _ERROR;
	}
	new_source.line_begin_cell_index_map = MP_Malloc(plexer->mp, sizeof(PX_SyntaxLexer_LineMap) * new_source.line_count);
	if (!new_source.line_begin_cell_index_map)
	{
		goto _ERROR;
	}
	PX_memset(new_source.line_begin_cell_index_map, 0, sizeof(PX_SyntaxLexer_LineMap) * new_source.line_count);

	new_source.source_index_map_to_cell_index = MP_Malloc(plexer->mp, sizeof(px_int) * new_source.source_length);
	if (!new_source.source_index_map_to_cell_index)
	{
		goto _ERROR;
	}

	//create cells

	//init cells abi
	for (cell_index = 0; cell_index < cells_count; cell_index++)
	{
		new_source.cells[cell_index].abi_index = -1;
	}

	//parse cells
	source_index = 0;
	cell_index = 0;
	row_index = 0;

	while (source_index<new_source.source_length)
	{
		px_dword rawcode;
		px_dword unicode;
		px_int bytes_size;
		px_int i;
	
		bytes_size = PX_FontModuleGetCharacterRawCode(PX_FONTMODULE_CODEPAGE_UTF8, new_source.source + source_index, &rawcode);
		if (bytes_size<=0)
		{
			goto _ERROR;
		}
		PX_FontUTF8RawToUnicode(rawcode, &unicode);
		new_source.cells[cell_index].source_index = source_index;
		new_source.cells[cell_index].bytes_size = bytes_size;
		new_source.cells[cell_index].unicode = unicode;
		new_source.cells[cell_index].row_index = row_index;
		for (i = 0; i < bytes_size; i++)
		{
			new_source.source_index_map_to_cell_index[source_index + i] = cell_index;
		}
		cell_index++;
		source_index += bytes_size;
		if (unicode == '\n')
		{
			row_index++;
			new_source.line_begin_cell_index_map[row_index].begin_cell_index = cell_index;
			continue;
		}

	}

	new_source.last_descriptor_index = -1;

	if (!PX_VectorInitialize(plexer->mp, &new_source.descriptor, sizeof(px_abi), 32))
	{
		goto _ERROR;
	}

	if (!PX_VectorPushback(&plexer->sources, &new_source))
	{
		goto _ERROR;
	}

	plexer->sort_status = PX_SYNTAX_LEXERSORT_STATUS_NORMAL;
	return plexer->sources.size-1;
_ERROR:
	if (new_source.name)
		MP_Free(plexer->mp, new_source.name);

	if (new_source.source)
		MP_Free(plexer->mp, new_source.source);

	if (new_source.source_index_map_to_line_index)
		MP_Free(plexer->mp, new_source.source_index_map_to_line_index);

	if (new_source.line_begin_cell_index_map)
		MP_Free(plexer->mp, new_source.line_begin_cell_index_map);

	if (new_source.cells)
		MP_Free(plexer->mp, new_source.cells);

	if (new_source.source_index_map_to_cell_index)
		MP_Free(plexer->mp, new_source.source_index_map_to_cell_index);

	return -1;
}

px_bool PX_SyntaxLexer_CallRawSource(px_syntaxlexer* plexer, const px_char* name, const px_char* SourceText)
{
	PX_SyntaxLexer_Call new_call = { 0 };
	px_int sourceindex;
	if (plexer->callstack_count>= PX_SYNTAX_LEXER_CALL_STACK_MAX_COUNT)
	{
		//call stack overflow
		return PX_FALSE;
	}

	sourceindex = PX_SyntaxLexer_AddSource(plexer, name, SourceText);
	if (sourceindex==-1)
	{
		return PX_FALSE;
	}
	new_call.read_offset = 0;
	new_call.source_index = sourceindex;
	plexer->call[plexer->callstack_count] = new_call;
	if (plexer->callstack_count==0)
	{
		plexer->entry_source_index = sourceindex;
	}
	plexer->callstack_count++;
	return PX_TRUE;


}

px_bool PX_SyntaxLexer_CallSourceByName(px_syntaxlexer* plexer, const px_char* name)
{
	PX_SyntaxLexer_Call new_call = { 0 };
	px_int  sourceindex;
	if (plexer->callstack_count >= PX_SYNTAX_LEXER_CALL_STACK_MAX_COUNT)
	{
		//call stack overflow
		return PX_FALSE;
	}
	sourceindex = PX_SyntaxLexer_GetSourceIndexByName(plexer, name);
	if (sourceindex == -1)
	{
		return PX_FALSE;
	}
	new_call.read_offset = 0;
	new_call.source_index = sourceindex;
	plexer->call[plexer->callstack_count] = new_call;
	if (plexer->callstack_count == 0)
	{
		plexer->entry_source_index = sourceindex;
	}
	plexer->callstack_count++;
	return PX_TRUE;
	
}

px_bool PX_SyntaxLexer_CallSourceByIndex(px_syntaxlexer* plexer, px_int index)
{
	PX_SyntaxLexer_Call new_call = { 0 };
	if (plexer->callstack_count >= PX_SYNTAX_LEXER_CALL_STACK_MAX_COUNT)
	{
		//call stack overflow
		return PX_FALSE;
	}
	if (index < 0 || index >= plexer->sources.size)
	{
		return PX_FALSE;
	}
	new_call.read_offset = 0;
	new_call.source_index = index;
	plexer->call[plexer->callstack_count] = new_call;
	if (plexer->callstack_count == 0)
	{
		plexer->entry_source_index = index;
	}
	plexer->callstack_count++;
	return PX_TRUE;
}

const px_char* PX_SyntaxLexer_GetCurrentSourceName(px_syntaxlexer* plexer)
{
	PX_SyntaxLexer_Source* psource = PX_SyntaxLexer_GetCurrentSource(plexer);
	if (psource)
	{
		return psource->name;
	}

	return "";
}

px_char PX_SyntaxLexer_GetSymbol(px_syntaxlexer *plexer)
{
	return plexer->symbol;
}

PX_SYNTAXLEXER_STATE PX_SyntaxLexer_GetState(px_syntaxlexer *plexer)
{
	PX_SYNTAXLEXER_STATE state = { 0 };
	PX_memcpy(state.call, plexer->call, sizeof(PX_SyntaxLexer_Call) * plexer->callstack_count);
	state.callstack_count = plexer->callstack_count;
	return state;

}

px_void PX_SyntaxLexer_SetState(px_syntaxlexer* plexer,PX_SYNTAXLEXER_STATE * psnapshot)
{
	PX_memcpy(plexer->call, psnapshot->call, sizeof(PX_SyntaxLexer_Call) * psnapshot->callstack_count);
	plexer->callstack_count = psnapshot->callstack_count;
}


px_char PX_SyntaxLexer_GetNextChar(px_syntaxlexer *plexer)
{
	px_char ch;
	PX_StringClear(&plexer->current_lexeme);
	ch = PX_SyntaxLexerGetCurrentSourceChar(plexer);
	plexer->lexeme_begin = PX_SyntaxLexer_GetCurrentSourceOffset(plexer);
	plexer->lexeme_begin_source_index =PX_SyntaxLexer_GetCurrentSourceIndex(plexer);
	plexer->lexeme_end = PX_SyntaxLexer_GetCurrentSourceOffset(plexer);
	plexer->lexeme_end_source_index =PX_SyntaxLexer_GetCurrentSourceIndex(plexer);
	PX_StringCatChar(&plexer->current_lexeme, ch);
	if(!PX_SyntaxLexer_IsEnd(plexer))
		PX_SyntaxLexer_Forward(plexer, 1);
	return ch;
}

px_char PX_SyntaxLexer_PreviewNextChar(px_syntaxlexer* plexer)
{
	return PX_SyntaxLexerGetCurrentSourceChar(plexer);
}

px_int  PX_SyntaxLexer_GetLexemeBeginSourceIndex(px_syntaxlexer* plexer)
{
	return plexer->lexeme_begin_source_index;
}

px_int  PX_SyntaxLexer_GetLexemeBegin(px_syntaxlexer* plexer)
{
	return plexer->lexeme_begin;
}

px_int  PX_SyntaxLexer_GetLexemeEndSourceIndex(px_syntaxlexer* plexer)
{
	return plexer->lexeme_end_source_index;
}

px_int  PX_SyntaxLexer_GetLexemeEnd(px_syntaxlexer* plexer)
{
	return plexer->lexeme_end;
}

px_int PX_SyntaxLexer_GetLexemeLine(px_syntaxlexer* plexer)
{
	PX_SyntaxLexer_Source* psource = PX_SyntaxLexer_GetSourceByIndex(plexer, plexer->lexeme_begin_source_index);
	if (psource)
	{
		return psource->source_index_map_to_line_index[plexer->lexeme_begin];
	}
	return -1;
}


px_void PX_SyntaxLexer_SetTokenCase(px_syntaxlexer *plexer,PX_SYNTAX_LEXER_LEXEME_CASE _case)
{
	plexer->lexeme_token_case=_case;
}

px_int PX_SyntaxLexer_GetCurrentContainerType(px_syntaxlexer *plexer)
{
	return plexer->current_container_type;
}

px_int PX_SyntaxLexer_GetCurrentDelimiterType(px_syntaxlexer *plexer)
{
	return plexer->current_delimiter_type;
}

const px_char* PX_SyntaxLexer_GetLexeme(px_syntaxlexer* plexer)
{
	return plexer->current_lexeme.buffer;
}

PX_SYNTAXLEXER_LEXEME_TYPE PX_SyntaxLexer_GetLexemeType(px_syntaxlexer* plexer)
{
	return plexer->current_lexeme_type;
}

px_void PX_SyntaxLexer_Reset(px_syntaxlexer* plexer)
{
	//reset call stack
	plexer->callstack_count = 0;
	PX_memset(plexer->call, 0, sizeof(PX_SyntaxLexer_Call) * PX_SYNTAX_LEXER_CALL_STACK_MAX_COUNT);

	PX_StringClear(&plexer->current_lexeme);
}

px_void PX_SyntaxLexer_Clear(px_syntaxlexer* plexer)
{
	px_int i;
	PX_SyntaxLexer_Reset(plexer);
	for (i = 0; i < plexer->sources.size; i++)
	{
		px_int j;
		PX_SyntaxLexer_Source* psource = (PX_SyntaxLexer_Source*)PX_VECTORAT(PX_SyntaxLexer_Source, &plexer->sources, i);
		if (psource->name)
			MP_Free(plexer->mp, psource->name);

		if (psource->source)
			MP_Free(plexer->mp, psource->source);

		if (psource->source_index_map_to_line_index)
			MP_Free(plexer->mp, psource->source_index_map_to_line_index);

		if (psource->line_begin_cell_index_map)
			MP_Free(plexer->mp, psource->line_begin_cell_index_map);

		if (psource->cells)
			MP_Free(plexer->mp, psource->cells);

		if (psource->source_index_map_to_cell_index)
			MP_Free(plexer->mp, psource->source_index_map_to_cell_index);

		for (j = 0; j < psource->descriptor.size; j++)
		{
			px_abi* pabi = (px_abi*)PX_VECTORAT(px_abi, &psource->descriptor, j);
			PX_AbiFree(pabi);
		}
		PX_VectorFree(&psource->descriptor);
	}
	PX_VectorClear(&plexer->sources);
	PX_StringClear(&plexer->current_lexeme);
	
}

px_int PX_SyntaxLexer_GetCurrentSourceIndex(px_syntaxlexer* plexer)
{
	if (plexer->callstack_count>0)
	{
		return plexer->call[plexer->callstack_count - 1].source_index;
	}
	else
	{
		return -1;
	}
}

px_int PX_SyntaxLexer_GetSourceIndexByName(px_syntaxlexer* plexer, const px_char name[])
{
	px_int i;
	for (i = 0; i < plexer->sources.size; i++)
	{
		PX_SyntaxLexer_Source* psource = (PX_SyntaxLexer_Source*)PX_VECTORAT(PX_SyntaxLexer_Source, &plexer->sources, i);
		if (PX_strequ(psource->name, name))
		{
			return i;
		}
	}
	return -1;
}

PX_SyntaxLexer_Source* PX_SyntaxLexer_GetSourceByName(px_syntaxlexer* plexer, const px_char name[])
{
	px_int i = PX_SyntaxLexer_GetSourceIndexByName(plexer, name);
	if (i != -1)
	{
		return (PX_SyntaxLexer_Source*)PX_VECTORAT(PX_SyntaxLexer_Source, &plexer->sources, i);
	}
	return PX_NULL;
}

PX_SyntaxLexer_Source* PX_SyntaxLexer_GetSourceByIndex(px_syntaxlexer* plexer, px_int index)
{
	if (index >= 0 && index < plexer->sources.size)
	{
		return (PX_SyntaxLexer_Source*)PX_VECTORAT(PX_SyntaxLexer_Source, &plexer->sources, index);
	}
	return PX_NULL;
}

PX_SyntaxLexer_Call* PX_SyntaxLexer_GetCurrentCall(px_syntaxlexer* plexer)
{
	if (plexer->callstack_count>0)
	{
		return plexer->call + plexer->callstack_count - 1;
	}
	return PX_NULL;
}

PX_SyntaxLexer_Source* PX_SyntaxLexer_GetCurrentSource(px_syntaxlexer* plexer)
{
	PX_SyntaxLexer_Call* pcall = PX_SyntaxLexer_GetCurrentCall(plexer);
	if (pcall)
	{
		return PX_SyntaxLexer_GetSourceByIndex(plexer, pcall->source_index);
	}
	return PX_NULL;
}

px_char PX_SyntaxLexerGetDelimiter(px_syntaxlexer* plexer)
{
	PX_ASSERTIF(!plexer->current_lexeme.buffer);
	return plexer->current_lexeme.buffer[0];
}

px_bool PX_SyntaxLexerIsContainerTransfer(px_syntaxlexer *plexer,const px_char startch[],px_char ch)
{
	px_int i=0;
	for (i=0;i<plexer->container_count;i++)
	{
		if(PX_memequ(startch,plexer->container[i].ContainerBegin,PX_strlen(startch))&&plexer->container[i].transfer&&plexer->container[i].transfer==ch)
		{
			return PX_TRUE;
		}
	}
	return PX_FALSE;
}

const px_char *PX_SyntaxLexerGetEntrySourceName(px_syntaxlexer* plexer)
{
	if (plexer->entry_source_index != -1)
	{
		PX_SyntaxLexer_Source* psource = PX_SyntaxLexer_GetSourceByIndex(plexer, plexer->entry_source_index);
		if (psource)
		{
			return psource->name;
		}
	}
	return "";
}