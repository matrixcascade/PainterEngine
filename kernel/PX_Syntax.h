#ifndef PX_SYNTAX_H
#define PX_SYNTAX_H
#include "../core/PX_Core.h"
struct _PX_Syntax;
struct _PX_Syntax_bnfnode;
struct _PX_Syntax_ast;


typedef px_bool(*PX_Syntax_Function)(struct _PX_Syntax* pSyntax, struct _PX_Syntax_ast* past,px_void *userptr);
#define PX_SYNTAX_FUNCTION(name) px_bool name(struct _PX_Syntax* pSyntax,struct _PX_Syntax_ast* past,px_void *userptr)

typedef px_bool(*PX_Syntax_Operate_Function)(struct _PX_Syntax* pSyntax, px_int oprand1_abi_index, px_int oprand2_abi_index,px_int opcode_abi_index);
#define PX_SYNTAX_OPERATE_FUNCTION(name) px_bool name(struct _PX_Syntax* pSyntax, px_int oprand1_abi_index, px_int oprand2_abi_index,px_int opcode_abi_index)

typedef px_bool(*PX_Syntax_Type_Load_Function)(struct _PX_Syntax* pSyntax, const px_char type[], px_int oprand_index, const px_int register_index);
#define PX_SYNTAX_TYPE_Load_FUNCTION(name) px_bool name(struct _PX_Syntax* pSyntax, const px_char type[], px_int oprand_index, const px_int register_index)

typedef px_bool(*PX_Syntax_Type_Store_Function)(struct _PX_Syntax* pSyntax, const px_char type[], px_int oprand_index, const px_int register_index);
#define PX_SYNTAX_TYPE_Store_FUNCTION(name) px_bool name(struct _PX_Syntax* pSyntax, const px_char type[], px_int oprand_index, const px_int register_index)

typedef enum
{
	PX_SYNTAX_AST_TYPE_CONSTANT=0,
	PX_SYNTAX_AST_TYPE_LINKER,
	PX_SYNTAX_AST_TYPE_FUNCTION,
	PX_SYNTAX_AST_TYPE_RECURSION,
	PX_SYNTAX_AST_TYPE_CONTINUOUS,
}PX_SYNTAX_AST_TYPE;

typedef enum
{
	PX_SYNTAX_OPRAND_FROM_GLOBAL=0,
	PX_SYNTAX_OPRAND_FROM_LOCAL,
	PX_SYNTAX_OPRAND_FROM_REGISTER,
	PX_SYNTAX_OPRAND_FROM_STACK,
	PX_SYNTAX_OPRAND_FROM_CONST,
}PX_SYNTAX_OPRAND_FROM;

typedef enum
{
	PX_SYNTAX_AST_RETURN_CONTINUE,
	PX_SYNTAX_AST_RETURN_ERROR,
	PX_SYNTAX_AST_RETURN_END,
}PX_SYNTAX_AST_RETURN;



typedef struct _PX_Syntax_bnfnode
{
	PX_SYNTAX_AST_TYPE type;
	px_string constant;
	PX_Syntax_Function penterfunction, pleavefunction;
	px_void* userptr;
	struct _PX_Syntax_bnfnode* pothers;
	struct _PX_Syntax_bnfnode* pnext;
}PX_Syntax_bnfnode;

typedef struct
{
	px_string mnenonic;
	PX_Syntax_Function penterfunction,pleavefunction;
	px_void* userptr;
	PX_Syntax_bnfnode * pbnfnode;
}PX_Syntax_pebnf;


typedef struct _PX_Syntax_ast
{
	px_int call_abistack_count;
	PX_Syntax_bnfnode * pbnfnode;
	px_int pebnf_index;
	//PX_Syntax_pebnf* ppebnf;
	PX_SYNTAXLEXER_STATE syntaxlexer_state;
}PX_Syntax_ast;

typedef struct
{
	px_string name;
	px_vector members;//px_string
}PX_Syntax_maptype;

typedef enum
{
	PX_SYNTAX_OPCODE_TYPE_UNARY_PREFIX,
	PX_SYNTAX_OPCODE_TYPE_UNARY_SUFFIX,
	PX_SYNTAX_OPCODE_TYPE_BINARY,
	PX_SYNTAX_OPCODE_TYPE_TERNARY,
	PX_SYNTAX_OPCODE_TYPE_BEGIN,
	PX_SYNTAX_OPCODE_TYPE_END,
}PX_SYNTAX_OPCODE_TYPE;

typedef struct
{
	PX_SYNTAX_OPCODE_TYPE type;
	px_char opcode[7];
	px_char pair;
	px_int precedence;
}PX_Syntax_opcode;

typedef enum
{
	PX_SYNTAX_MACHINE_OPCODE_TRAP=0,
    PX_SYNTAX_MACHINE_OPCODE_NOP,
	PX_SYNTAX_MACHINE_OPCODE_MOVR,
	PX_SYNTAX_MACHINE_OPCODE_MOVC,
	PX_SYNTAX_MACHINE_OPCODE_MOVF,
	PX_SYNTAX_MACHINE_OPCODE_MOVN,

    PX_SYNTAX_MACHINE_OPCODE_LOADU8,
	PX_SYNTAX_MACHINE_OPCODE_LOADU16,
	PX_SYNTAX_MACHINE_OPCODE_LOADU32,
	PX_SYNTAX_MACHINE_OPCODE_LOADI8,
	PX_SYNTAX_MACHINE_OPCODE_LOADI16,
	PX_SYNTAX_MACHINE_OPCODE_LOADI32,
	PX_SYNTAX_MACHINE_OPCODE_STORE8,
	PX_SYNTAX_MACHINE_OPCODE_STORE16,
	PX_SYNTAX_MACHINE_OPCODE_STORE32,

   PX_SYNTAX_MACHINE_OPCODE_F2I,
   PX_SYNTAX_MACHINE_OPCODE_F2U,
	PX_SYNTAX_MACHINE_OPCODE_I2F,
	PX_SYNTAX_MACHINE_OPCODE_U2F,
	PX_SYNTAX_MACHINE_OPCODE_FF2F,

  PX_SYNTAX_MACHINE_OPCODE_PUSH,
	PX_SYNTAX_MACHINE_OPCODE_PUSHAD,
	PX_SYNTAX_MACHINE_OPCODE_POP,
	PX_SYNTAX_MACHINE_OPCODE_POPAD,
	PX_SYNTAX_MACHINE_OPCODE_POPN,
	PX_SYNTAX_MACHINE_OPCODE_ADDSP,
	PX_SYNTAX_MACHINE_OPCODE_SUBSP,
	PX_SYNTAX_MACHINE_OPCODE_NEG,
	PX_SYNTAX_MACHINE_OPCODE_ADD,
	PX_SYNTAX_MACHINE_OPCODE_SUB,
	PX_SYNTAX_MACHINE_OPCODE_MUL,
	PX_SYNTAX_MACHINE_OPCODE_DIV,
	PX_SYNTAX_MACHINE_OPCODE_IDIV,
	PX_SYNTAX_MACHINE_OPCODE_MOD,
	PX_SYNTAX_MACHINE_OPCODE_FNEG,
	PX_SYNTAX_MACHINE_OPCODE_FADD,
	PX_SYNTAX_MACHINE_OPCODE_FSUB,
	PX_SYNTAX_MACHINE_OPCODE_FMUL,
	PX_SYNTAX_MACHINE_OPCODE_FDIV,
	PX_SYNTAX_MACHINE_OPCODE_FCMP,
	PX_SYNTAX_MACHINE_OPCODE_FCOMI,
	PX_SYNTAX_MACHINE_OPCODE_AND,
	PX_SYNTAX_MACHINE_OPCODE_OR,
	PX_SYNTAX_MACHINE_OPCODE_XOR,
	PX_SYNTAX_MACHINE_OPCODE_NOT,
	PX_SYNTAX_MACHINE_OPCODE_SHL,
	PX_SYNTAX_MACHINE_OPCODE_SHR,
	PX_SYNTAX_MACHINE_OPCODE_CMP,
	PX_SYNTAX_MACHINE_OPCODE_JMP,
	PX_SYNTAX_MACHINE_OPCODE_JE,
	PX_SYNTAX_MACHINE_OPCODE_JNE,
	PX_SYNTAX_MACHINE_OPCODE_JG,
	PX_SYNTAX_MACHINE_OPCODE_JL,
	PX_SYNTAX_MACHINE_OPCODE_JGE,
	PX_SYNTAX_MACHINE_OPCODE_JLE,
	PX_SYNTAX_MACHINE_OPCODE_CALL,

 PX_SYNTAX_MACHINE_OPCODE_CALLR,
	PX_SYNTAX_MACHINE_OPCODE_JMPR,

   PX_SYNTAX_MACHINE_OPCODE_RET,

    PX_SYNTAX_MACHINE_OPCODE_EE_ADD,
	PX_SYNTAX_MACHINE_OPCODE_EE_SUB,
	PX_SYNTAX_MACHINE_OPCODE_EE_MUL,
	PX_SYNTAX_MACHINE_OPCODE_EE_DIV,
	PX_SYNTAX_MACHINE_OPCODE_EE_IDIV,
	PX_SYNTAX_MACHINE_OPCODE_EE_FADD,
	PX_SYNTAX_MACHINE_OPCODE_EE_FSUB,
	PX_SYNTAX_MACHINE_OPCODE_EE_FMUL,
	PX_SYNTAX_MACHINE_OPCODE_EE_FDIV,
	PX_SYNTAX_MACHINE_OPCODE_EE_AND,
	PX_SYNTAX_MACHINE_OPCODE_EE_OR,
	PX_SYNTAX_MACHINE_OPCODE_EE_NOT,
	PX_SYNTAX_MACHINE_OPCODE_EE_POP,
	PX_SYNTAX_MACHINE_OPCODE_EE_PUSH,
	PX_SYNTAX_MACHINE_OPCODE_EE_SHL,
	PX_SYNTAX_MACHINE_OPCODE_EE_SHR,
	PX_SYNTAX_MACHINE_OPCODE_EE_XOR,
	PX_SYNTAX_MACHINE_OPCODE_EE_CALL,
	PX_SYNTAX_MACHINE_OPCODE_EE_MOD,
	PX_SYNTAX_MACHINE_OPCODE_EE_NEG,
	PX_SYNTAX_MACHINE_OPCODE_EE_CMP,
	PX_SYNTAX_MACHINE_OPCODE_EE_FNEG,
	PX_SYNTAX_MACHINE_OPCODE_EE_FCMP,
}PX_SYNTAX_MACHINE_OPCODE;


typedef struct _PX_Syntax
{
	px_memorypool* mp;
	px_int reg_lifetime;
	px_string message;
	px_vector pebnf;
	PX_Syntax_bnfnode bnfnode_return;
	px_vector reg_ast_stack;
	px_vector reg_abi_stack;
	px_vector reg_ast_instr_stack;
	px_vector reg_expr_opcode_stack;//px_string
	px_syntaxlexer reg_syntaxlexer;
	px_int    reg_expr_source_index;
	px_int    reg_expr_begin_line;
	px_int    reg_write_expr_source_index;
	px_int    reg_write_expr_begin_line;
	px_vector reg_maptype_stack;//PX_Syntax_MapType
	px_int    reg_lastsource_index;
	px_int	  reg_unname_index;
	px_char   reg_unname[32];
	px_int    execute_id, break_execute_id;
}PX_Syntax;

//Initialize/free/clear
px_bool PX_Syntax_Initialize(px_memorypool* mp, PX_Syntax* pSyntax);
px_void PX_Syntax_Free(PX_Syntax* pSyntax);
px_void PX_Syntax_ClearState(PX_Syntax* pSyntax);
px_void PX_Syntax_ClearSources(PX_Syntax* pSyntax);
px_void PX_Syntax_ClearSourceDescription(PX_Syntax* pSyntax);

px_void PX_Syntax_SetBreak(PX_Syntax* pSyntax, px_int id);

//random color
px_color PX_Syntax_GetRandomColor(px_uint32 seed);

//execute
PX_SYNTAX_AST_RETURN PX_Syntax_ExecuteNext(PX_Syntax* pSyntax);
px_int PX_Syntax_GetCurrentSourceIndex(PX_Syntax* pSyntax);
px_int PX_Syntax_GetCurrentRow(PX_Syntax* pSyntax);
px_int PX_Syntax_GetCurrentLine(PX_Syntax* pSyntax);
px_bool PX_Syntax_IsExecuting(PX_Syntax* pSyntax);

//source
px_bool PX_Syntax_AddSource(PX_Syntax* pSyntax, const px_char filename[], const px_char source[]);
PX_SyntaxLexer_Source* PX_Syntax_GetSource(PX_Syntax* pSyntax, const px_char name[]);
PX_SyntaxLexer_Source* PX_Syntax_GetSourceByIndex(PX_Syntax* pSyntax, px_int index);
px_int PX_Syntax_GetSourceCount(PX_Syntax* pSyntax);
px_bool PX_Syntax_IsValidToken(const px_char token[]);
px_bool PX_Syntax_IsEndOfSource(PX_Syntax* pSyntax);
px_int PX_Syntax_GetMaxLineCharWidthCount(PX_Syntax* pSyntax, px_int source_index);
px_int PX_Syntax_GetLineCount(PX_Syntax* pSyntax, px_int source_index);
//pebnf
PX_Syntax_pebnf* PX_Syntax_GetPebnfByIndex(PX_Syntax* pSyntax, px_int index);
PX_Syntax_pebnf* PX_Syntax_GetPebnf(PX_Syntax* pSyntax, const px_char mnemonic[]);
PX_Syntax_bnfnode* PX_Syntax_GetPebnfNode(PX_Syntax* pSyntax, const px_char mnemonic[]);
PX_Syntax_bnfnode* PX_Syntax_GetOtherNode(PX_Syntax_bnfnode* pbnfnode, PX_SYNTAX_AST_TYPE ast_type, const px_char mnemonic[]);
px_bool PX_Syntax_Parse_PEBNF(PX_Syntax* pSyntax, const px_char PEBNF[], PX_Syntax_Function penterfunction, PX_Syntax_Function pleavefunction, px_void* userptr);

//ast opcode
px_bool PX_Syntax_ExecuteAstOpcode(PX_Syntax* pSyntax, const px_char type[]);


//opcode
px_int PX_Syntax_NewOpcodeDefine(PX_Syntax* pSyntax, const px_char opcode[], PX_SYNTAX_OPCODE_TYPE type, px_dword precedence);
px_int PX_Syntax_GetOpcodeDefineIndex(PX_Syntax* pSyntax, const px_char opcode[], PX_SYNTAX_OPCODE_TYPE type);
PX_Syntax_opcode* PX_Syntax_GetOpcodeDefine(PX_Syntax* pSyntax, px_int index);

//abi
px_abi* PX_Syntax_NewAbi(PX_Syntax* pSyntax, const px_char name[], px_int reg_lifetime);

px_abi* PX_Syntax_InsertAbi(PX_Syntax* pSyntax, const px_char name[], px_int reg_lifetime, px_int index);
px_abi* PX_Syntax_GetAbi(PX_Syntax* pSyntax, const px_char name[], px_int reg_lifetime);
px_abi* PX_Syntax_GetAbiByIndex(PX_Syntax* pSyntax, px_int index);
px_abi* PX_Syntax_GetAbiFromForward(PX_Syntax* pSyntax, const px_char name[]);
px_abi* PX_Syntax_GetAbiFromBackward(PX_Syntax* pSyntax, const px_char name[]);
px_abi* PX_Syntax_FindSecondAbiFromBackward(PX_Syntax* pSyntax, const px_char name[]);


px_int  PX_Syntax_GetLastOpcodeAbiIndex(PX_Syntax* pSyntax);
px_int  PX_Syntax_GetSecondLastOpcodeAbiIndex(PX_Syntax* pSyntax);
px_void PX_Syntax_PopLastOpcodeAbi(PX_Syntax* pSyntax);
px_void PX_Syntax_PopSecondLastOpcodeAbi(PX_Syntax* pSyntax);
px_void PX_Syntax_PopOperate3(PX_Syntax* pSyntax, px_int abi1_index, px_int abi2_index, px_int abi3_index);
px_void PX_Syntax_PopOperate2(PX_Syntax* pSyntax, px_int abi1_index, px_int abi2_index);


px_void PX_Syntax_PopAbi(PX_Syntax* pSyntax);
px_void PX_Syntax_PopLastSecondAbi(PX_Syntax* pSyntax);
px_void PX_Syntax_PopAbiIndex(PX_Syntax* pSyntax, px_int index);
px_bool PX_Syntax_CopyAbiToLast(PX_Syntax* pSyntax, px_int index);
px_bool PX_Syntax_RenameLastAbi(PX_Syntax* pSyntax, const px_char name[]);
px_bool PX_Syntax_MergeLastAbiToIndexWithName(PX_Syntax* pSyntax, px_int index, const px_char new_name[]);
px_bool PX_Syntax_MergeLastAbi(PX_Syntax* pSyntax, const px_char new_name[]);
px_bool PX_Syntax_MergeLast2AbiToSecondLast(PX_Syntax* pSyntax);
px_bool PX_Syntax_MergeLast2AbiToLast(PX_Syntax* pSyntax);
px_bool PX_Syntax_MergeAbiIndexToLast(PX_Syntax* pSyntax, px_int index);
px_bool PX_Syntax_MergeLast2AbiWithNameToSecondLast(PX_Syntax* pSyntax, const px_char name[]);
px_bool PX_Syntax_MergeLast2AbiWithNameToLast(PX_Syntax* pSyntax, const px_char name[]);
px_bool PX_Syntax_MergeLast2AbiToNewAbi(PX_Syntax* pSyntax, const px_char newname[]);
px_bool PX_Syntax_MergeAbiIndexToAbiIndexWithName(PX_Syntax* pSyntax, px_int src_index, px_int dst_index, const px_char name[]);
px_bool PX_Syntax_MergeAbiIndexToAbiIndex(PX_Syntax* pSyntax, px_int src_index, px_int dst_index);

px_abi* PX_Syntax_GetAbiLast(PX_Syntax* pSyntax);
px_abi* PX_Syntax_GetAbiSecondLast(PX_Syntax* pSyntax);
px_bool PX_Syntax_CheckAbiName(px_abi* pabi, const px_char name[]);
px_bool PX_Syntax_CheckLastAbiName(PX_Syntax* pSyntax, const px_char name[]);
px_bool PX_Syntax_CheckSecondLastAbiName(PX_Syntax* pSyntax, const px_char name[]);
px_int PX_Syntax_GetAbiIndexFromForward_LifeTime(PX_Syntax* pSyntax, const px_char name[], px_int lifetime);
px_int  PX_Syntax_GetAbiIndexFromForward(PX_Syntax* pSyntax, const px_char name[]);
px_int PX_Syntax_GetAbiIndexFromBackward_LifeTime(PX_Syntax* pSyntax, const px_char name[], px_int lifetime);
px_int  PX_Syntax_GetAbiIndexFromBackward(PX_Syntax* pSyntax, const px_char name[]);
px_int  PX_Syntax_GetSecondAbiIndexFromBackward(PX_Syntax* pSyntax, const px_char name[]);
const px_char* PX_Syntax_GetAbiValue(px_abi* pabi);
px_int PX_Syntax_GetAbiCount(PX_Syntax* pSyntax);

//abi-type-typedef
px_bool PX_Syntax_TypeMatch(const px_char source_type[], const px_char target_type[]);
px_bool PX_Syntax_TypeMatch2(const px_char source_type[], const px_char target_type[], const px_char target_type2[]);
px_bool PX_Syntax_TypeMatch3(const px_char source_type[], const px_char target_type[], const px_char target_type2[], const px_char target_type3[]);
px_bool PX_Syntax_NewTypeDefine(PX_Syntax* pSyntax, const px_char type[], const px_char mnemonic[],  px_int size, px_int reg_lifetime);
px_bool PX_Syntax_NewTypedef(PX_Syntax* pSyntax, const px_char type[], const px_char mnemonic[]);
const px_char* PX_Syntax_FindTypeDefineTypeByMnemonic(PX_Syntax* pSyntax, const px_char mnemonic[]);
px_bool PX_Syntax_GetTypeDefineByMnemonic(PX_Syntax* pSyntax, const px_char mnemonic[], px_abi* ptype_define_abi);
px_bool PX_Syntax_GetTypeDefineByType(PX_Syntax* pSyntax, const px_char type[], px_abi* ptype_define_abi);
px_int  PX_Syntax_GetTypeDefineTypeSize(PX_Syntax* pSyntax, const px_char type[]);


px_abi* PX_Syntax_GetLastTypeDefineScopeAbi(PX_Syntax* pSyntax);
px_int PX_Syntax_GetTypeDefineScopeAbiIndex(PX_Syntax* pSyntax, const px_char type[]);
px_int PX_Syntax_GetTypeSize(PX_Syntax* pSyntax, const px_char type[]);
px_abi PX_Syntax_GetTypeDefineAbiReadOnly(PX_Syntax* pSyntax, const px_char type[]);

px_bool PX_Syntax_EnterScope(PX_Syntax* pSyntax);
px_bool PX_Syntax_LeaveScope(PX_Syntax* pSyntax);
px_abi* PX_Syntax_GetLastScopeAbi(PX_Syntax* pSyntax);
px_int  PX_Syntax_ScopeAllocLocal(PX_Syntax* pSyntax, px_int size);
px_int  PX_Syntax_ScopeAllocGlobal(PX_Syntax* pSyntax, px_int size);
px_int  PX_Syntax_ScopeAlloc(PX_Syntax* pSyntax, px_int size);
//ast
px_bool PX_Syntax_NewAst(PX_Syntax* pSyntax, const px_char mnemonic[]);
px_void PX_Syntax_AstReturn(PX_Syntax* pSyntax, PX_Syntax_ast* past,const px_char message[]);

//lexer
px_syntaxlexer* PX_Syntax_GetSyntaxLexer(PX_Syntax* pSyntax);
px_bool PX_Syntax_CallSource(PX_Syntax* pSyntax, const px_char name[], const px_char pebnf[]);
px_bool PX_Syntax_CallSourceIndex(PX_Syntax* pSyntax, px_int index, const px_char pebnf[]);
px_int PX_Syntax_GetCurrentLexerOffset(PX_Syntax* pSyntax);
px_int PX_Syntax_GetCurrentLexemeBeginSourceIndex(PX_Syntax* pSyntax);
px_int PX_Syntax_GetCurrentLexemeEndSourceIndex(PX_Syntax* pSyntax);
px_int PX_Syntax_GetCurrentLexemeBegin(PX_Syntax* pSyntax);
px_int PX_Syntax_GetCurrentLexemeEnd(PX_Syntax* pSyntax);
px_int PX_Syntax_GetCurrentLexemeLine(PX_Syntax* pSyntax);
px_int PX_Syntax_GetCurrentLexerIndex(PX_Syntax* pSyntax);
px_int PX_Syntax_GetCurrentLexerLine(PX_Syntax* pSyntax);
const px_char* PX_Syntax_GetCurrentLexerSourceName(PX_Syntax* pSyntax);
const px_char * PX_Syntax_GetCurrentLexeme(PX_Syntax* pSyntax);
const px_char* PX_Syntax_GetCurrentLexerSourcePointer(PX_Syntax* pSyntax);
const px_char* PX_Syntax_GetCurrentLexerEntrySourceName(PX_Syntax* pSyntax);
px_char PX_Syntax_PreviewNextChar(PX_Syntax* pSyntax);
px_char PX_Syntax_GetNextChar(PX_Syntax* pSyntax);
px_void PX_Syntax_LexerForward(PX_Syntax* pSyntax, px_int offset);
PX_SYNTAXLEXER_LEXEME_TYPE PX_Syntax_GetNextLexeme(PX_Syntax* pSyntax);
PX_SYNTAXLEXER_LEXEME_TYPE PX_Syntax_GetNextLexeme(PX_Syntax* pSyntax);
PX_SYNTAXLEXER_LEXEME_TYPE PX_Syntax_GetNextLexeme2(PX_Syntax* pSyntax);
PX_SYNTAXLEXER_LEXEME_TYPE PX_Syntax_GetNextLexeme3(PX_Syntax* pSyntax);
PX_SYNTAXLEXER_LEXEME_TYPE PX_Syntax_GetNextLexeme4(PX_Syntax* pSyntax);
PX_SYNTAXLEXER_LEXEME_TYPE PX_Syntax_GetLexemeType(PX_Syntax* pSyntax);
PX_SYNTAXLEXER_STATE PX_Syntax_GetLexerState(PX_Syntax* pSyntax);
PX_SyntaxLexer_Source* PX_Syntax_GetCurrentSource(PX_Syntax* pSyntax);
px_void PX_Syntax_SetLexerState(PX_Syntax* pSyntax, PX_SYNTAXLEXER_STATE* pstate);


//common
px_bool PX_Syntax_Execute(PX_Syntax* pSyntax,const px_char filename[], const px_char pebnf[]);

px_void PX_Syntax_Terminate(PX_Syntax* pSyntax,const px_char message[]);
px_void PX_Syntax_Message(PX_Syntax* pSyntax, const px_char message[]);
px_string* PX_Syntax_GetMessage(PX_Syntax* pSyntax);
px_string* PX_Syntax_GetAstMessage(PX_Syntax* pSyntax);
px_bool PX_Syntax_RemovePebnf(PX_Syntax* pSyntax, const px_char pebnf[]);
const px_char* PX_Syntax_AllocUnnamed(PX_Syntax* pSyntax);

//output
px_bool PX_Syntax_AppendString(PX_Syntax* pSyntax, const px_char name[], const char content_name[], const px_char content[]);
px_bool PX_Syntax_AppendData(PX_Syntax* pSyntax, const px_char name[], const char data_name[], const px_byte data[], px_int datasize);
px_bool PX_Syntax_DeleteData(PX_Syntax* pSyntax, const px_char name[], const char data_name[]);

px_bool PX_Syntax_MergeLast2AbiValue(PX_Syntax* pSyntax, const px_char last_abi_name[], const px_char value_name[], const px_char second_last_abi_name[], const px_char value_name2[]);

px_bool PX_Syntax_NewMap(PX_Syntax* pSyntax, px_int sourceindex, px_int begin, px_int end);
px_bool PX_Syntax_NewMapToken(PX_Syntax* pSyntax, px_int begin_sourceindex, px_int begin, px_int end_sourceindex, px_int end, px_color color, const px_char type[]);
px_bool PX_Syntax_LastMapInfo(PX_Syntax* pSyntax, px_int sourceindex, const px_char info[]);
px_bool PX_Syntax_LastMapToMemory(PX_Syntax* pSyntax, px_int sourceindex,PX_SYNTAX_OPRAND_FROM from, px_int offset);
px_bool PX_Syntax_NewMapTypeDefine(PX_Syntax* pSyntax,  const px_char type[], px_int size);
px_bool PX_Syntax_LastMapTypeMember(PX_Syntax* pSyntax,  const px_char type[], px_int count);

px_bool PX_Syntax_NewIRInstruction0(PX_Syntax* pSyntax, const px_char opcode[]);
px_bool PX_Syntax_NewIRInstruction1(PX_Syntax* pSyntax, const px_char opcode[], const px_char oprand1[]);
px_bool PX_Syntax_NewIRInstruction2(PX_Syntax* pSyntax, const px_char opcode[], const px_char oprand1[], const px_char oprand2[]);
px_bool PX_Syntax_NewIRInstruction3(PX_Syntax* pSyntax, const px_char opcode[], const px_char oprand1[], const px_char oprand2[], const px_char oprand3[]);
px_void PX_Syntax_VariableToInstructionOperand(PX_Syntax* pSyntax, px_int abi_index, px_char content[16]);


#endif // !PX_SCRIPT_STNTACTIC_H

