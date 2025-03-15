#ifndef PX_SYNTAX_H
#define PX_SYNTAX_H

#include "../core/PX_Core.h"
struct _PX_Syntax;
struct _PX_Syntax_bnfnode;
struct _PX_Syntax_ast;


typedef px_bool(*PX_Syntax_Function)(struct _PX_Syntax* pSyntax, struct _PX_Syntax_ast* past);
#define PX_SYNTAX_FUNCTION(name) px_bool name(struct _PX_Syntax* pSyntax,struct _PX_Syntax_ast* past)

typedef enum
{
	PX_SYNTAX_AST_TYPE_CONSTANT,
	PX_SYNTAX_AST_TYPE_LINKER,
	PX_SYNTAX_AST_TYPE_FUNCTION,
	PX_SYNTAX_AST_TYPE_RECURSION,
}PX_SYNTAX_AST_TYPE;

typedef struct _PX_Syntax_bnfnode
{
	PX_SYNTAX_AST_TYPE type;
	px_string constant;
	PX_Syntax_Function pfunction;
	struct _PX_Syntax_bnfnode* pothers;
	struct _PX_Syntax_bnfnode* pnext;
}PX_Syntax_bnfnode;

typedef struct
{
	px_string mnenonic;
	PX_Syntax_Function pfunction;
	PX_Syntax_bnfnode * pbnfnode;
}PX_Syntax_pebnf;

typedef struct
{
	px_string name;
	px_string source;
}PX_Syntax_Source;

typedef struct _PX_Syntax_ast
{
	PX_LEXER_STATE lexer_state;
	px_int call_abistack_index;
	PX_Syntax_bnfnode * pbnfnode;
}PX_Syntax_ast;

typedef struct _PX_Syntax
{
	px_memorypool* mp;
	px_int lifetime;
	px_string message,ast_message;
	px_vector pebnf;
	px_vector aststack;
	px_vector abistack;
	px_vector sources;
	px_vector reg_ast_opcode;
	px_vector reg_ast_lexer;
}PX_Syntax;
PX_Syntax_Source* PX_Syntax_GetSource(PX_Syntax* pSyntax, const px_char name[]);
px_bool PX_Syntax_AddLexer(PX_Syntax* pSyntax, const px_char filename[], const px_char source[]);
px_bool PX_Syntax_IsValidToken(const px_char token[]);
PX_Syntax_pebnf* PX_Syntax_GetPEBNF(PX_Syntax* pSyntax, const px_char mnemonic[]);
px_lexer* PX_Syntax_GetCurrentLexer(PX_Syntax* pSyntax);
px_bool PX_Syntax_PushLexer(PX_Syntax* pSyntax, px_lexer* plexer);
px_void PX_Syntax_PopLexer(PX_Syntax* pSyntax);
PX_Syntax_bnfnode* PX_Syntax_GetPebnfNode(PX_Syntax* pSyntax, const px_char mnemonic[]);
PX_Syntax_bnfnode* PX_Syntax_GetOtherNode(PX_Syntax_bnfnode* pbnfnode, PX_SYNTAX_AST_TYPE ast_type, const px_char mnemonic[]);
PX_LEXER_LEXEME_TYPE PX_SyntaxGetNextLexemeFilter(px_lexer* plexer);
const px_char* PX_SyntaxGetCurrentLexeme(PX_Syntax_ast* past);
PX_LEXER_LEXEME_TYPE PX_SyntaxGetNextAstFilter(PX_Syntax_ast* past);
px_bool PX_Syntax_Parse_NextPENNF(PX_Syntax* pSyntax, PX_Syntax_pebnf* ppebnf, PX_Syntax_bnfnode* pprevious, px_lexer* plexer, PX_Syntax_Function pfunction);
px_bool PX_Syntax_Parse_PEBNF(PX_Syntax* pSyntax, const px_char PEBNF[], PX_Syntax_Function pfunction);
px_bool PX_Syntax_PushOpcode(PX_Syntax* pSyntax, const px_char type[]);
px_abi* PX_Syntax_GetAbi(PX_Syntax* pSyntax, const px_char name[], px_int lifetime);
px_abi* PX_Syntax_GetAbiStack(PX_Syntax* pSyntax, px_int index);
px_bool PX_Syntax_PushAbiStack(PX_Syntax* pSyntax, px_abi* pabi);
px_void PX_Syntax_PopAbiStack(PX_Syntax* pSyntax);
px_abi* PX_Syntax_CreateNewIr(PX_Syntax* pSyntax);
px_abi* PX_Syntax_PushNewAbi(PX_Syntax* pSyntax, const px_char name[], px_int lifetime);
px_abi* PX_Syntax_GetAbiStackLast(PX_Syntax* pSyntax);
px_abi* PX_Syntax_GetAbiStackSecondLast(PX_Syntax* pSyntax);
px_bool PX_Syntax_AddSource(PX_Syntax* pSyntax, const px_char filename[], const px_char source[]);
px_bool PX_Syntax_Execute(PX_Syntax* pSyntax, const px_char sources[],const px_char pebnf[]);
px_void PX_Syntax_Terminate(PX_Syntax* pSyntax, PX_Syntax_ast *past,const px_char message[]);
px_void PX_Syntax_AstMessage(PX_Syntax* pSyntax, const px_char message[]);
px_int PX_Syntax_GetAbiStackCount(PX_Syntax* pSyntax);
px_char PX_Syntax_GetNextChar(PX_Syntax_ast* past);
px_bool PX_SyntaxInitialize(px_memorypool* mp, PX_Syntax* pSyntax);
px_void PX_SyntaxFree(PX_Syntax* pSyntax);
px_void PX_SyntaxClear(PX_Syntax* pSyntax);
#endif // !PX_SCRIPT_STNTACTIC_H

