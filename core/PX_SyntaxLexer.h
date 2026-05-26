#ifndef __PX_SYNTAX_LEXER_H
#define __PX_SYNTAX_LEXER_H
#include "PX_String.h"
#include "PX_Vector.h"
#include "PX_Abi.h"
#define  PX_SYNTAX_LEXER_CA_COMMENT_MAX_COUNT    32
#define  PX_SYNTAX_LEXER_CA_SPACER_MAX_COUNT	 32
#define  PX_SYNTAX_LEXER_CA_DELIMITER_MAX_COUNT  32
#define  PX_SYNTAX_LEXER_CA_CONTAINER_MAX_COUNT  32
#define PX_SYNTAX_LEXER_CA_DISCARD_MAX_COUNT    32
#define PX_SYNTAX_LEXER_CALL_STACK_MAX_COUNT    32


typedef struct
{
	px_int   source_index; //the cell belongs to which source byte(offset)
	px_int   row_index; //the cell belongs to which row
	px_int   bytes_size; //the size of the cell in byte
	px_int   abi_index; //the abi index of the cell, -1 means no abi, abi is used for syntax analysis and code generation, it can be used to store variable, constant, function, type and so on
	px_dword unicode; //the unicode of the cell, it is used for syntax analysis and code generation, it can be used to store variable, constant, function, type and so on
}PX_SyntaxLexer_Cell;

typedef struct
{
	px_int  begin_cell_index;
	px_bool bdebugbreak;
}PX_SyntaxLexer_LineMap;

typedef struct
{
	px_char* name;
	px_char* source;
	px_int   source_length;
	

	px_int line_count;
	px_int max_line_char_width;
	px_int* source_index_map_to_line_index;
	

	px_int cells_count;
	PX_SyntaxLexer_Cell* cells;
	px_int* source_index_map_to_cell_index; // map source index to cell index

	PX_SyntaxLexer_LineMap* line_begin_cell_index_map;
	
	px_int last_descriptor_index;
	px_vector descriptor; // px_abi
}PX_SyntaxLexer_Source;

typedef struct
{
	px_int	source_index;
	px_int  read_offset;
}PX_SyntaxLexer_Call;

typedef enum __PX_SYNTAX_LEXER_SORT_STATUS
{
	PX_SYNTAX_LEXERSORT_STATUS_NORMAL=  0,
	PX_SYNTAX_LEXER_SORT_STATUS_SPACER=  1,
	PX_SYNTAX_LEXER_SORT_STATUS_COMMENT= 2,
	PX_SYNTAX_LEXER_SORT_STATUS_NEWLINE= 3,
}PX_SYNTAX_LEXER_SORT_STATUS;

typedef enum __PX_SYNTAX_LEXER_LEXEME_CASE
{
	PX_SYNTAX_LEXER_LEXEME_CASE_NORMAL,
	PX_SYNTAX_LEXER_LEXEME_CASE_LOWER,
	PX_SYNTAX_LEXER_LEXEME_CASE_UPPER,

}PX_SYNTAX_LEXER_LEXEME_CASE;


typedef struct PX_SYNTAX_LEXER_CA_Container 
{
	px_char ContainerBegin[8];
	px_char ContainerEnd[8];
	px_char transfer;
}PX_SYNTAX_LEXER_CA_Container;


typedef struct PX_SYNTAX_LEXER_CA_COMMENT 
{
	px_char CommentBegin[8];
	px_char CommentEnd[8];
}PX_SYNTAX_LEXER_CA_Comment;

typedef struct PX_SYNTAX_LEXER_CA_DISCARD
{
	px_char discard[8];
}PX_SYNTAX_LEXER_CA_Discard;



typedef enum __PX_SYNTAX_LEXER_LEXEME_TYPE
{
	PX_SYNTAXLEXER_LEXEME_TYPE_END       =0,
	PX_SYNTAXLEXER_LEXEME_TYPE_SPACER    =1,
	PX_SYNTAXLEXER_LEXEME_TYPE_DELIMITER =2,
	PX_SYNTAXLEXER_LEXEME_TYPE_CONATINER =3,
	PX_SYNTAXLEXER_LEXEME_TYPE_NEWLINE   =4,
	PX_SYNTAXLEXER_LEXEME_TYPE_TOKEN     =5,
	PX_SYNTAXLEXER_LEXEME_TYPE_COMMENT  = 6,
	PX_SYNTAXLEXER_LEXEME_TYPE_ERR		 = -1,
}PX_SYNTAXLEXER_LEXEME_TYPE;

typedef struct
{	
	px_memorypool* mp;
	px_int	     container_count;
	px_int       delimiter_count;
	px_int       discard_count;
	px_int	     comment_count;
	px_vector    sources;//PX_SyntaxLexer_Source
	px_int       entry_source_index;
	PX_SyntaxLexer_Call    call[PX_SYNTAX_LEXER_CALL_STACK_MAX_COUNT];
	px_int 	    callstack_count;

	px_int      lexeme_begin_source_index;
	px_int      lexeme_begin;
	px_int      lexeme_end_source_index;
	px_int      lexeme_end;
	px_int		spacerCount;
	px_char		symbol;

	px_uint      current_delimiter_type;
	px_uint      current_container_type;
	px_uint      current_comment_type;
	
	
	PX_SYNTAXLEXER_LEXEME_TYPE	current_lexeme_type;
	PX_SYNTAX_LEXER_SORT_STATUS sort_status;
	PX_SYNTAX_LEXER_LEXEME_CASE lexeme_token_case;
	px_string    current_lexeme;
	

	PX_SYNTAX_LEXER_CA_Comment   comment[PX_SYNTAX_LEXER_CA_COMMENT_MAX_COUNT];
	PX_SYNTAX_LEXER_CA_Container container[PX_SYNTAX_LEXER_CA_CONTAINER_MAX_COUNT];
	PX_SYNTAX_LEXER_CA_Discard   discard[PX_SYNTAX_LEXER_CA_DISCARD_MAX_COUNT];
	px_char	     spacer[PX_SYNTAX_LEXER_CA_SPACER_MAX_COUNT];
	px_char      delimiter[PX_SYNTAX_LEXER_CA_DELIMITER_MAX_COUNT];
}px_syntaxlexer;

typedef  struct 
{
	PX_SyntaxLexer_Call    call[PX_SYNTAX_LEXER_CALL_STACK_MAX_COUNT];
	px_int 	    callstack_count;
}PX_SYNTAXLEXER_STATE;


px_bool PX_SyntaxLexer_Initialize(px_memorypool *mp, px_syntaxlexer* plexer);
px_void PX_SyntaxLexer_Reset(px_syntaxlexer* plexer);
px_void PX_SyntaxLexer_Clear(px_syntaxlexer* plexer);
px_void PX_SyntaxLexer_Free(px_syntaxlexer* plexer);
px_bool PX_SyntaxLexer_AddSource(px_syntaxlexer* plexer, const px_char* name, const px_char* SourceText);
px_int PX_SyntaxLexer_GetSourceCount(px_syntaxlexer* plexer);

px_void PX_SyntaxLexer_GetIncludedString(px_syntaxlexer* plexer, px_string* str);
px_void PX_SyntaxLexer_SetTokenCase(px_syntaxlexer* plexer, PX_SYNTAX_LEXER_LEXEME_CASE _case);
px_void PX_SyntaxLexer_RegisterComment(px_syntaxlexer* plexer, const px_char Begin[], const px_char End[]);
px_void PX_SyntaxLexer_RegisterContainerTransfer(px_syntaxlexer* plexer, px_uint containerIndex, px_char transfer);
px_void PX_SyntaxLexer_RegisterSpacer(px_syntaxlexer* plexer, px_char spacer);
px_int PX_SyntaxLexer_RegisterContainer(px_syntaxlexer* plexer, const px_char Begin[], const px_char End[]);
px_int PX_SyntaxLexer_RegisterDelimiter(px_syntaxlexer* plexer, px_char delimiter);
px_int PX_SyntaxLexer_RegisterDiscard(px_syntaxlexer* plexer, const px_char *discard);
px_int PX_SyntaxLexer_GetDelimiterType(px_syntaxlexer* plexer, px_char delimiter);
px_int PX_SyntaxLexer_GetContainerType(px_syntaxlexer* plexer, px_char* pContainerText);
px_int PX_SyntaxLexer_GetCurrentContainerType(px_syntaxlexer* plexer);
px_int PX_SyntaxLexer_GetCurrentDelimiterType(px_syntaxlexer* plexer);
px_int  PX_SyntaxLexer_GetCurrentSourceOffset(px_syntaxlexer* plexer);
px_int PX_SyntaxLexer_GetCurrentLine(px_syntaxlexer* plexer);
px_bool PX_SyntaxLexer_ReadString(px_syntaxlexer* plexer, px_string* str, px_uint size);
px_bool PX_SyntaxLexer_SetCurrentOffset(px_syntaxlexer* plexer, px_int offset);
px_char	PX_SyntaxLexer_GetSymbol(px_syntaxlexer* plexer);
px_bool PX_SyntaxLexer_IsEnd(px_syntaxlexer* plexer);

px_bool PX_SyntaxLexer_CallRawSource(px_syntaxlexer* plexer, const px_char* name, const px_char* SourceText);
px_bool PX_SyntaxLexer_CallSourceByName(px_syntaxlexer* plexer, const px_char* name);
px_bool PX_SyntaxLexer_CallSourceByIndex(px_syntaxlexer* plexer, px_int index);
const px_char* PX_SyntaxLexerGetEntrySourceName(px_syntaxlexer* plexer);

px_void PX_SyntaxLexer_Forward(px_syntaxlexer* plexer,px_int offset);

px_char PX_SyntaxLexer_GetNextChar(px_syntaxlexer* plexer);
px_char PX_SyntaxLexer_PreviewNextChar(px_syntaxlexer* plexer);
px_int  PX_SyntaxLexer_GetLexemeBeginSourceIndex(px_syntaxlexer* plexer);
px_int  PX_SyntaxLexer_GetLexemeBegin(px_syntaxlexer* plexer);
px_int  PX_SyntaxLexer_GetLexemeEndSourceIndex(px_syntaxlexer* plexer);
px_int  PX_SyntaxLexer_GetLexemeEnd(px_syntaxlexer* plexer);
px_int PX_SyntaxLexer_GetLexemeLine(px_syntaxlexer* plexer);
px_int PX_SyntaxLexer_GetCurrentSourceIndex(px_syntaxlexer* plexer);
px_int PX_SyntaxLexer_GetSourceIndexByName(px_syntaxlexer* plexer, const px_char name[]);

PX_SyntaxLexer_Source* PX_SyntaxLexer_GetSourceByName(px_syntaxlexer* plexer, const px_char name[]);
PX_SyntaxLexer_Source* PX_SyntaxLexer_GetSourceByIndex(px_syntaxlexer* plexer, px_int index);
PX_SyntaxLexer_Source* PX_SyntaxLexer_GetCurrentSource(px_syntaxlexer* plexer);
PX_SyntaxLexer_Call* PX_SyntaxLexer_GetCurrentCall(px_syntaxlexer* plexer);

PX_SYNTAXLEXER_STATE PX_SyntaxLexer_GetState(px_syntaxlexer *plexer);
px_void PX_SyntaxLexer_SetState(px_syntaxlexer* plexer, PX_SYNTAXLEXER_STATE* psnapshot);

PX_SYNTAXLEXER_LEXEME_TYPE PX_SyntaxLexer_GetNextLexeme(px_syntaxlexer *plexer);
PX_SYNTAXLEXER_LEXEME_TYPE PX_SyntaxLexer_PreviewNextLexeme(px_syntaxlexer* plexer);
PX_SYNTAXLEXER_LEXEME_TYPE PX_SyntaxLexer_GetLexemeType(px_syntaxlexer* plexer);

const px_char* PX_SyntaxLexer_GetCurrentSourceName(px_syntaxlexer* plexer);
const px_char* PX_SyntaxLexer_GetLexeme(px_syntaxlexer* plexer);
const px_char* PX_SyntaxLexer_GetCurrentSourcePointer(px_syntaxlexer* plexer);
const px_char* PX_SyntaxLexer_GetCurrentLexeme(px_syntaxlexer* plexer);

#endif
