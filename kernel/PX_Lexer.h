#ifndef __PX_LEXER_H
#define __PX_LEXER_H
#include "../core/PX_Core.h"

#define  PX_LEXER_CA_COMMENT_MAX_COUNT    32
#define  PX_LEXER_CA_SPACER_MAX_COUNT	 32
#define  PX_LEXER_CA_DELIMITER_MAX_COUNT  32
#define  PX_LEXER_CA_CONTAINER_MAX_COUNT  32

typedef enum __PX_LEXER_SORT_STATUS
{
	PX_LEXERSORT_STATUS_NORMAL=  0,
	PX_LEXER_SORT_STATUS_SPACER=  1,
	PX_LEXER_SORT_STATUS_COMMENT= 2,
	PX_LEXER_SORT_STATUS_NEWLINE= 3,
}PX_LEXER_SORT_STATUS;

typedef enum __PX_LEXER_LEXEME_CASE
{
	PX_LEXER_LEXEME_CASE_NORMAL,
	PX_LEXER_LEXEME_CASE_LOWER,
	PX_LEXER_LEXEME_CASE_UPPER,

}PX_LEXER_LEXEME_CASE;


typedef struct PX_LEXER_CA_Container 
{
	px_char ContainerBegin[8];
	px_char ContainerEnd[8];
	px_char transfer;
}PX_LEXER_CA_Container;


typedef struct PX_LEXER_CA_COMMENT 
{
	px_char CommentBegin[8];
	px_char CommentEnd[8];
}PX_LEXER_CA_Comment;


typedef enum __PX_LEXER_LEXEME_TYPE
{
	PX_LEXER_LEXEME_TYPE_END       =0,
	PX_LEXER_LEXEME_TYPE_SPACER    =1,
	PX_LEXER_LEXEME_TYPE_DELIMITER =2,
	PX_LEXER_LEXEME_TYPE_CONATINER =3,
	PX_LEXER_LEXEME_TYPE_NEWLINE   =4,
	PX_LEXER_LEXEME_TYPE_TOKEN     =5,
	PX_LEXER_LEXEME_TYPE_ERR		 = -1,
}PX_LEXER_LEXEME_TYPE;

typedef struct __Pt_Lexer
{	
	px_int	     ContainerCount;
	px_int       DelimiterCount;
	px_int	     CommentCount;
	px_char		*Sources;
	px_char      SortComment;
	px_ulong    SourceOffset;
	px_int		SpacerCount;
	px_char		Symbol;
	px_bool     NumericMath;
	px_uint      CurrentDelimiterType;
	px_uint      CurrentContainerType;
	px_uint      CurrentCommentType;
	
	PX_LEXER_LEXEME_TYPE	CurrentLexemeFlag;
	PX_LEXER_SORT_STATUS SortStatus;
	PX_LEXER_LEXEME_CASE lexemeTokenCase;

	px_string    CurLexeme;
	px_memorypool *mp;

	PX_LEXER_CA_Comment   Comment[PX_LEXER_CA_COMMENT_MAX_COUNT];
	PX_LEXER_CA_Container Container[PX_LEXER_CA_CONTAINER_MAX_COUNT];
	px_char	     Spacer[PX_LEXER_CA_SPACER_MAX_COUNT];
	px_char      Delimiter[PX_LEXER_CA_DELIMITER_MAX_COUNT];

}px_lexer;

typedef  struct __PX_LEXER_START
{
	px_int offset;
	px_lexer *lexer;
}PX_LEXER_STATE;

px_void PX_LexerInitialize(px_lexer *lexer,px_memorypool *mp);
px_void PX_LexerRegisterComment(px_lexer *lexer,const px_char Begin[],const px_char End[]);
px_int PX_LexerRegisterContainer(px_lexer *lexer,const px_char Begin[],const px_char End[]);
px_void PX_LexerRegisterContainerTransfer(px_lexer *lexer,px_uint containerIndex,px_char transfer);

px_void PX_LexerRegisterSpacer(px_lexer *lexer,px_char Spacer);
px_int PX_LexerRegisterDelimiter(px_lexer *lexer,px_char Delimiter);
px_int PX_LexerGetDelimiterType(px_lexer *lexer,px_char Delimiter);
px_int PX_LexerGetContainerType(px_lexer *lexer,px_char *pContainerText);
px_int PX_LexerGetCurrentContainerType(px_lexer *lexer);
px_int PX_LexerGetCurrentDelimiterType(px_lexer *lexer);
px_void PX_LexerFree(px_lexer *lexer);
px_bool PX_LexerSortText(px_lexer *lexer,const px_char *SourceText);
px_bool  PX_LexerLoadSourceFromMemory(px_lexer *lexer,const px_char *buffer);
px_bool  PX_LexerSetSourcePointer(px_lexer *lexer,const px_char *buffer);
px_bool PX_LexerReadString(px_lexer *lexer,px_string *str,px_uint size);
px_char	PX_LexerGetSymbol(px_lexer *lexer);
px_void PX_LexerGetLexemeString(px_lexer *lexer,px_string *str);
px_void PX_LexerGetIncludedString(px_lexer *lexer,px_string *str);
px_void PX_LexerSetTokenCase(px_lexer *lexer,PX_LEXER_LEXEME_CASE _case);
px_void PX_LexerSetNumericMatch(px_lexer *lexer,px_bool b);

PX_LEXER_STATE PX_LexerGetState(px_lexer *lexer);
px_void PX_LexerSetState(PX_LEXER_STATE state);
px_bool PX_LexerIsLememeIsNumeric(px_lexer *lexer);
px_char PX_LexerGetNextChar(px_lexer *lexer);
PX_LEXER_LEXEME_TYPE PX_LexerGetNextLexeme(px_lexer *lexer);
PX_LEXER_LEXEME_TYPE PX_LexerGetCurrentLexeme(px_lexer *lexer);
#endif
