#ifndef PX_SYNTAX_H
#define PX_SYNTAX_H

#include "../core/PX_Core.h"
struct _PX_Syntax;
struct _PX_Syntax_bnfnode;
struct _PX_Syntax_ast;


typedef px_bool(*PX_Syntax_Function)(struct _PX_Syntax* pSyntax, struct _PX_Syntax_ast* past);
#define PX_SYNTAX_FUNCTION(name) px_bool name(struct _PX_Syntax* pSyntax,struct _PX_Syntax_ast* past)

typedef px_bool(*PX_Syntax_Operate_Function)(struct _PX_Syntax* pSyntax, px_abi *oprand1,px_abi *oprand2,const px_char opcode[]);
#define PX_SYNTAX_OPERATE_FUNCTION(name) px_bool name(struct _PX_Syntax* pSyntax, px_abi *oprand1,px_abi *oprand2,const px_char opcode[]);

typedef enum
{
	PX_SYNTAX_AST_TYPE_CONSTANT=0,
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

typedef struct
{
	px_lexer lexer;
	px_int   expand_lexer_index;
	px_int   expand_lexer_offset_begin;
	px_int   expand_lexer_offset_end;
}PX_Syntax_Lexer;

typedef struct _PX_Syntax_ast
{
	px_int call_lexer_offset;
	px_int call_lexer_index;
	px_int call_abistack_index;
	PX_Syntax_bnfnode * pbnfnode;
}PX_Syntax_ast;

typedef struct _PX_Syntax_expand
{
	px_int lifttime;
	px_int parameters_count;
	px_string token;
	px_string format;
}PX_Syntax_expand;

typedef struct _PX_Syntax_operate
{
	px_dword oprand_count;
	px_string oprand1_type;
	px_string oprand2_type;
	px_string oprand3_type;
	px_char opcode[8];
	PX_Syntax_Operate_Function pfunction;
}PX_Syntax_operate;

typedef enum
{
	PX_SYNTAX_OPCODE_TYPE_UNARY,
	PX_SYNTAX_OPCODE_TYPE_BINARY,
	PX_SYNTAX_OPCODE_TYPE_TERNARY,

}PX_SYNTAX_OPCODE_TYPE;

typedef struct _PX_Syntax_opcode
{
	px_dword precedence;
	PX_SYNTAX_OPCODE_TYPE type;
	px_char opcode[8];
}PX_Syntax_opcode;

typedef struct _PX_Syntax
{
	px_memorypool* mp;
	px_int reg_lifetime;
	px_string message,ast_message;
	px_vector pebnf;
	px_vector sources;
	px_vector opcodes;
	PX_Syntax_bnfnode bnfnode_return;
	px_vector reg_ast_stack;
	px_vector reg_abi_stack;
	px_vector reg_astopcode_stack;
	px_int    reg_lexer_index;
	px_vector reg_lexer_stack;
	px_vector reg_expand_stack;//abi
	px_vector  reg_oprand_stack;//abi
	px_vector  reg_opcode_stack;//PX_Syntax_opcode
	px_vector reg_operate_stack;//PX_Syntax_operate
	px_string reg_ir;
	px_int reg_unname_index;
	px_char reg_unname[32];
}PX_Syntax;

px_bool PX_Syntax_AddSource(PX_Syntax* pSyntax, const px_char filename[], const px_char source[]);
PX_Syntax_Source* PX_Syntax_GetSource(PX_Syntax* pSyntax, const px_char name[]);

px_bool PX_Syntax_IsValidToken(const px_char token[]);

//pebnf
PX_Syntax_pebnf* PX_Syntax_GetPEBNF(PX_Syntax* pSyntax, const px_char mnemonic[]);
PX_Syntax_bnfnode* PX_Syntax_GetPebnfNode(PX_Syntax* pSyntax, const px_char mnemonic[]);
PX_Syntax_bnfnode* PX_Syntax_GetOtherNode(PX_Syntax_bnfnode* pbnfnode, PX_SYNTAX_AST_TYPE ast_type, const px_char mnemonic[]);
px_bool PX_Syntax_Parse_NextPENNF(PX_Syntax* pSyntax, PX_Syntax_pebnf* ppebnf, PX_Syntax_bnfnode* pprevious, px_lexer* plexer, PX_Syntax_Function pfunction);
px_bool PX_Syntax_Parse_PEBNF(PX_Syntax* pSyntax, const px_char PEBNF[], PX_Syntax_Function pfunction);

//ast opcode
px_bool PX_Syntax_ExecuteAstOpcode(PX_Syntax* pSyntax, const px_char type[]);


//abi
px_abi* PX_Syntax_NewAbi(PX_Syntax* pSyntax, const px_char name[], px_int reg_lifetime);
px_abi* PX_Syntax_InsertAbi(PX_Syntax* pSyntax, const px_char name[], px_int reg_lifetime, px_int index);
px_abi* PX_Syntax_GetAbi(PX_Syntax* pSyntax, const px_char name[], px_int reg_lifetime);
px_abi* PX_Syntax_GetAbiByIndex(PX_Syntax* pSyntax, px_int index);
px_abi* PX_Syntax_FindAbiFromForward(PX_Syntax* pSyntax, const px_char name[]);
px_abi* PX_Syntax_FindAbiFromBackward(PX_Syntax* pSyntax, const px_char name[]);

px_void PX_Syntax_PopAbi(PX_Syntax* pSyntax);
px_void PX_Syntax_PopSecondAbi(PX_Syntax* pSyntax);
px_void PX_Syntax_PopAbiIndex(PX_Syntax* pSyntax, px_int index);
px_bool PX_Syntax_CopyAbiToLast(PX_Syntax* pSyntax, px_int index);
px_bool PX_Syntax_RenameLastAbi(PX_Syntax* pSyntax, const px_char name[]);
px_bool PX_Syntax_MergeLastToIndexWithName(PX_Syntax* pSyntax, px_int index, const px_char new_name[]);
px_bool PX_Syntax_MergeLastAbi(PX_Syntax* pSyntax, const px_char new_name[]);
px_bool PX_Syntax_MergeLast2AbiToSecondLast(PX_Syntax* pSyntax);
px_bool PX_Syntax_MergeLast2AbiToLast(PX_Syntax* pSyntax);
px_bool PX_Syntax_MergeLast2AbiWithNameToSecondLast(PX_Syntax* pSyntax, const px_char name[]);
px_bool PX_Syntax_MergeLast2AbiWithNameToLast(PX_Syntax* pSyntax, const px_char name[]);
px_bool PX_Syntax_MergeLast2AbiToNewAbi(PX_Syntax* pSyntax, const px_char newname[]);
px_abi* PX_Syntax_GetAbiLast(PX_Syntax* pSyntax);
px_abi* PX_Syntax_GetAbiSecondLast(PX_Syntax* pSyntax);
px_bool PX_Syntax_CheckAbiName(px_abi* pabi, const px_char name[]);
px_bool PX_Syntax_CheckLastAbiName(PX_Syntax* pSyntax, const px_char name[]);
px_bool PX_Syntax_CheckSecondLastAbiName(PX_Syntax* pSyntax, const px_char name[]);
px_int  PX_Syntax_FindAbiIndexFromForward(PX_Syntax* pSyntax, const px_char name[]);
px_int  PX_Syntax_FindAbiIndexFromBackward(PX_Syntax* pSyntax, const px_char name[]);

const px_char* PX_Syntax_GetAbiValue(px_abi* pabi);
px_int PX_Syntax_GetAbiCount(PX_Syntax* pSyntax);

//abi-type
px_abi* PX_Syntax_NewTypeDefine(PX_Syntax* pSyntax, const px_char name[], const px_char type[], px_int size, px_int reg_lifetime);
px_abi* PX_Syntax_LastTypeDefine(PX_Syntax* pSyntax);
px_int PX_Syntax_FindTypeDefine(PX_Syntax* pSyntax, const px_char name[], px_int reg_lifetime);

//local / global
px_bool PX_Syntax_EnterScope(PX_Syntax* pSyntax);
px_bool PX_Syntax_LeaveScope(PX_Syntax* pSyntax);
px_int  PX_Syntax_AllocLocal(PX_Syntax* pSyntax, px_int size);
px_int  PX_Syntax_AllocGlobal(PX_Syntax* pSyntax, px_int size);

//ast
px_bool PX_Syntax_NewAst(PX_Syntax* pSyntax, const px_char mnemonic[]);
px_void PX_Syntax_AstReturn(PX_Syntax* pSyntax, PX_Syntax_ast* past,const px_char message[]);

//expand
px_bool PX_Syntax_NewExpand(PX_Syntax* pSyntax, const px_char token[], const px_char format[],px_int paramcount, px_int reg_lifetime);

//lexer
px_lexer* PX_Syntax_GetCurrentLexer(PX_Syntax* pSyntax);
px_bool PX_Syntax_SetCurrentLexer(PX_Syntax* pSyntax, px_int index, px_int offset);
px_bool PX_Syntax_NewSyntaxExpandLexer(PX_Syntax* pSyntax, const px_char name[], const px_char source[], px_int begin, px_int end);
px_bool PX_Syntax_EntrySyntaxExpandLexer(PX_Syntax* pSyntax, px_int index);
const px_char* PX_Syntax_GetCurrentLexeme(PX_Syntax* pSyntax);
PX_LEXER_LEXEME_TYPE PX_Syntax_GetNextLexeme(PX_Syntax* pSyntax);
PX_LEXER_LEXEME_TYPE PX_Syntax_GetNextLexeme(PX_Syntax* pSyntax);
PX_LEXER_LEXEME_TYPE PX_Syntax_GetNextLexeme2(PX_Syntax* pSyntax);
PX_LEXER_LEXEME_TYPE PX_Syntax_GetNextLexeme3(PX_Syntax* pSyntax);
PX_LEXER_LEXEME_TYPE PX_Syntax_GetNextLexeme4(PX_Syntax* pSyntax);
const px_char* PX_Syntax_GetCurrentLexeme(PX_Syntax* pSyntax);
px_int PX_Syntax_GetCurrentLexerOffset(PX_Syntax* pSyntax);
px_char PX_Syntax_GetNextChar(PX_Syntax* pSyntax);
px_char PX_Syntax_PreviewNextChar(PX_Syntax* pSyntax);
px_bool PX_Syntax_NewEntrySource(PX_Syntax* pSyntax, const px_char name[]);

//opcode
px_bool PX_Syntax_NewOpcode(PX_Syntax* pSyntax, const px_char opcode[], PX_SYNTAX_OPCODE_TYPE type,px_dword precedence);
px_bool PX_Syntax_NewOperate1(PX_Syntax* pSyntax, const px_char type1[], const px_char opcode[], PX_Syntax_Operate_Function opfun);
px_bool PX_Syntax_NewOperate2(PX_Syntax* pSyntax, const px_char type1[], const px_char type2[], const px_char opcode[], PX_Syntax_Operate_Function opfun);
px_bool PX_Syntax_NewOperate3(PX_Syntax* pSyntax, const px_char type1[], const px_char type2[], const px_char type3[], const px_char opcode[], PX_Syntax_Operate_Function opfun);


//common
px_bool PX_Syntax_ExecuteSource(PX_Syntax* pSyntax, const px_char name[], const px_char pebnf[]);
px_bool PX_Syntax_Execute(PX_Syntax* pSyntax, const px_char pebnf[]);
px_bool PX_Syntax_LoadBaseType(PX_Syntax* pSyntax);
px_bool PX_Syntax_ExecuteTestBench(PX_Syntax* pSyntax, const px_char pebnf_expr[]);
px_void PX_Syntax_Terminate(PX_Syntax* pSyntax,const px_char message[]);
px_void PX_Syntax_Message(PX_Syntax* pSyntax, const px_char message[]);
px_bool PX_Syntax_Initialize(px_memorypool* mp, PX_Syntax* pSyntax);
px_void PX_Syntax_Free(PX_Syntax* pSyntax);
px_void PX_Syntax_Clear(PX_Syntax* pSyntax);
px_int  PX_Syntax_GetCurrentLexemeBegin(PX_Syntax* pSyntax);
px_int  PX_Syntax_GetCurrentLexemeEnd(PX_Syntax* pSyntax);
px_bool PX_Syntax_RemovePebnf(PX_Syntax* pSyntax, const px_char pebnf[]);
const px_char* PX_Syntax_AllocUnnamed(PX_Syntax* pSyntax);



#endif // !PX_SCRIPT_STNTACTIC_H

