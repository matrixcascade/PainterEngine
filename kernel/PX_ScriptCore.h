#ifndef __PX_SCRIPT_H
#define __PX_SCRIPT_H
#include "../core/PX_Core.h"
#include "PX_Lexer.h"

//special register IP,SP,BP,rRET

//register r0~r15 



//Pretreatment
//#include [name]

//special 
//ASSUME [name] [name]
//.GLOBAL [int]
//.STACK  [int]
//.THREAD [int]

//instructions


//mov  [reg,local,global],[num,string,reg,local,global]

//add  [reg,local,global],[num,reg,local,global]
//sub  [reg,local,global],[num,reg,local,global]
//div  [reg,local,global],[num,reg,local,global]
//mul  [reg,local,global],[num,reg,local,global]
//mod  [reg,local,global],[int,reg,local,global] 
//shl  [reg,local,global],[int,reg,local,global] 
//shr  [reg,local,global],[int,reg,local,global] 
//or   [reg,local,global],[num,reg,local,global]
//xor  [reg,local,global],[num,reg,local,global]
//pow  [reg,local,global],[num,reg,local,global]
//sin  [reg,local,global],[num,reg,local,global]
//cos  [reg,local,global],[num,reg,local,global]
//int  [reg,local,global]
//flt  [reg,local,global]

//strlen [reg,local,global],[reg,local,global,string]
//strcat [reg,local,global],[reg,local,global,string]
//strrep [reg,local,global],[reg,local,global,string],[reg,local,global,string]
//strchr [reg,local,global],[reg,local,global,string],[reg,local,global,int]
//strtoi [reg,local,global],[reg,local,global,string]
//strtof [reg,local,global],[reg,local,global,string]
//strfri [reg,local,global],[reg,local,global,int]
//strfrf [reg,local,global],[reg,local,global,float]
//strset [reg,local,global],[reg,local,global,int],[reg,local,global,string,int]
//strtmem [reg,local,global]
//asc [reg,local,global],[reg,local,global,string]


//membyte [reg,local,global],[reg,local,global,int],[reg,local,global,memory]
//memset [reg,local,global],[reg,local,global,int],[reg,local,global,memory]
//memcpy [reg,local,global],[reg,local,global,int],[reg,local,global,memory]
//memfind [reg,local,global],[reg,local,global,memory],[reg,local,global,memory]
//memlen [reg,local,global],[reg,local,global,memory]
//memcat [reg,local,global],[reg,local,global,memory]
//memtstr [reg,local,global]

//not  [reg,local,global]
//inv  [reg,local,global]

//jmp  [reg,num,local,global,label]
//je   [num,string,reg,local,global],[num,string,reg,local,global],[reg,int,local,global,label]
//jne  [num,string,reg,local,global],[num,string,reg,local,global],[reg,int,local,global,label]
//jl   [num,string,reg,local,global],[num,string,reg,local,global],[reg,int,local,global,label]
//jle  [num,string,reg,local,global],[num,string,reg,local,global],[reg,int,local,global,label]
//jg   [num,string,reg,local,global],[num,string,reg,local,global],[reg,int,local,global,label]
//jge  [num,string,reg,local,global],[num,string,reg,local,global],[reg,int,local,global,label]

//call [reg,int,local,global,label,host]

//push [num,reg,local,global]
//pop  [reg,local,global]

//ret

//binary format
//opcode 1 byte
//optype 3 byte
//operand [0,3] byte

#define PX_SCRIPT_MAX_FUNC_NAME_SIZE 32

enum PX_SCRIPT_ASM_INSTR_OPCODE
{
	PX_SCRIPT_ASM_INSTR_OPCODE_MOV=1,
	PX_SCRIPT_ASM_INSTR_OPCODE_ADD,
	PX_SCRIPT_ASM_INSTR_OPCODE_SUB,
	PX_SCRIPT_ASM_INSTR_OPCODE_DIV,
	PX_SCRIPT_ASM_INSTR_OPCODE_MUL,
	PX_SCRIPT_ASM_INSTR_OPCODE_MOD,
	PX_SCRIPT_ASM_INSTR_OPCODE_SHL,
	PX_SCRIPT_ASM_INSTR_OPCODE_SHR,
	PX_SCRIPT_ASM_INSTR_OPCODE_AND,
	PX_SCRIPT_ASM_INSTR_OPCODE_OR,
	PX_SCRIPT_ASM_INSTR_OPCODE_XOR,
	PX_SCRIPT_ASM_INSTR_OPCODE_ANDL,
	PX_SCRIPT_ASM_INSTR_OPCODE_ORL,
	PX_SCRIPT_ASM_INSTR_OPCODE_POW,
	PX_SCRIPT_ASM_INSTR_OPCODE_SIN,
	PX_SCRIPT_ASM_INSTR_OPCODE_COS,
	PX_SCRIPT_ASM_INSTR_OPCODE_INT,
	PX_SCRIPT_ASM_INSTR_OPCODE_FLT,
	PX_SCRIPT_ASM_INSTR_OPCODE_STRLEN,
	PX_SCRIPT_ASM_INSTR_OPCODE_STRCAT,
	PX_SCRIPT_ASM_INSTR_OPCODE_STRREP,
	PX_SCRIPT_ASM_INSTR_OPCODE_STRCHR,
	PX_SCRIPT_ASM_INSTR_OPCODE_STRTOI,
	PX_SCRIPT_ASM_INSTR_OPCODE_STRTOF,
	PX_SCRIPT_ASM_INSTR_OPCODE_STRFRI,
	PX_SCRIPT_ASM_INSTR_OPCODE_STRFRF,
	PX_SCRIPT_ASM_INSTR_OPCODE_STRSET,
	PX_SCRIPT_ASM_INSTR_OPCODE_STRFIND,
	PX_SCRIPT_ASM_INSTR_OPCODE_STRTMEM,
	PX_SCRIPT_ASM_INSTR_OPCODE_ASC,
	PX_SCRIPT_ASM_INSTR_OPCODE_MEMBYTE,
	PX_SCRIPT_ASM_INSTR_OPCODE_MEMSET,
	PX_SCRIPT_ASM_INSTR_OPCODE_MEMFIND,
	PX_SCRIPT_ASM_INSTR_OPCODE_MEMLEN,
	PX_SCRIPT_ASM_INSTR_OPCODE_MEMTRM,
	PX_SCRIPT_ASM_INSTR_OPCODE_MEMCAT,
	PX_SCRIPT_ASM_INSTR_OPCODE_MEMTSTR,
	PX_SCRIPT_ASM_INSTR_OPCODE_DATACPY,
	PX_SCRIPT_ASM_INSTR_OPCODE_INV,
	PX_SCRIPT_ASM_INSTR_OPCODE_NOT,
	PX_SCRIPT_ASM_INSTR_OPCODE_NEG,
	PX_SCRIPT_ASM_INSTR_OPCODE_JMP,
	PX_SCRIPT_ASM_INSTR_OPCODE_JE,
	PX_SCRIPT_ASM_INSTR_OPCODE_JNE,
	PX_SCRIPT_ASM_INSTR_OPCODE_JGE,
	PX_SCRIPT_ASM_INSTR_OPCODE_JLE,
	PX_SCRIPT_ASM_INSTR_OPCODE_JG,
	PX_SCRIPT_ASM_INSTR_OPCODE_JL,

	PX_SCRIPT_ASM_INSTR_OPCODE_LGE,
	PX_SCRIPT_ASM_INSTR_OPCODE_LGNE,
	PX_SCRIPT_ASM_INSTR_OPCODE_LGZ,
	PX_SCRIPT_ASM_INSTR_OPCODE_LGGZ,
	PX_SCRIPT_ASM_INSTR_OPCODE_LGGEZ,
	PX_SCRIPT_ASM_INSTR_OPCODE_LGLZ,
	PX_SCRIPT_ASM_INSTR_OPCODE_LGLEZ,

	PX_SCRIPT_ASM_INSTR_OPCODE_CALL,
	PX_SCRIPT_ASM_INSTR_OPCODE_PUSH,
	PX_SCRIPT_ASM_INSTR_OPCODE_POP,
	PX_SCRIPT_ASM_INSTR_OPCODE_POPN,
	PX_SCRIPT_ASM_INSTR_OPCODE_ADR,
	PX_SCRIPT_ASM_INSTR_OPCODE_RET,

	PX_SCRIPT_ASM_INSTR_OPCODE_NOP,
	PX_SCRIPT_ASM_INSTR_OPCODE_BPX,
	PX_SCRIPT_ASM_INSTR_OPCODE_WAIT,
	PX_SCRIPT_ASM_INSTR_OPCODE_SIGNAL,
};

enum PX_SCRIPT_ASM_OPTYPE
{
	PX_SCRIPT_ASM_OPTYPE_INT,
	PX_SCRIPT_ASM_OPTYPE_FLOAT,
	PX_SCRIPT_ASM_OPTYPE_REG,
	PX_SCRIPT_ASM_OPTYPE_LOCAL,//mnemonic
	PX_SCRIPT_ASM_OPTYPE_LOCAL_CONST,
	PX_SCRIPT_ASM_OPTYPE_LOCAL_REGREF,
	PX_SCRIPT_ASM_OPTYPE_LOCAL_GLOBALREF,
	PX_SCRIPT_ASM_OPTYPE_LOCAL_LOCALREF,
	PX_SCRIPT_ASM_OPTYPE_GLOBAL,//mnemonic
	PX_SCRIPT_ASM_OPTYPE_GLOBAL_CONST,
	PX_SCRIPT_ASM_OPTYPE_GLOBAL_REGREF,
	PX_SCRIPT_ASM_OPTYPE_GLOBAL_GLOBALREF,
	PX_SCRIPT_ASM_OPTYPE_GLOBAL_LOCALREF,
	PX_SCRIPT_ASM_OPTYPE_GLOBAL_SPREF,
	PX_SCRIPT_ASM_OPTYPE_STRING,
	PX_SCRIPT_ASM_OPTYPE_LABEL,//mnemonic
	PX_SCRIPT_ASM_OPTYPE_HOST,//mnemonic
	PX_SCRIPT_ASM_OPTYPE_MEMORY,
	PX_SCRIPT_ASM_OPTYPE_BP,
	PX_SCRIPT_ASM_OPTYPE_SP,
	PX_SCRIPT_ASM_OPTYPE_IP,
};
#define PX_SCRIPT_ASM_OPERAND_ACC_TYPE_UNKNOW		0
#define PX_SCRIPT_ASM_OPERAND_ACC_TYPE_INT			1
#define PX_SCRIPT_ASM_OPERAND_ACC_TYPE_FLOAT		2
#define PX_SCRIPT_ASM_OPERAND_ACC_TYPE_NUM			3
#define PX_SCRIPT_ASM_OPERAND_ACC_TYPE_REG			4
#define PX_SCRIPT_ASM_OPERAND_ACC_TYPE_LOCAL		8
#define PX_SCRIPT_ASM_OPERAND_ACC_TYPE_GLOBAL		16
#define PX_SCRIPT_ASM_OPERAND_ACC_TYPE_STRING		32
#define PX_SCRIPT_ASM_OPERAND_ACC_TYPE_LABEL		64
#define PX_SCRIPT_ASM_OPERAND_ACC_TYPE_HOST		    128
#define PX_SCRIPT_ASM_OPERAND_ACC_TYPE_MEMORY	    256
#define PX_SCRIPT_ASM_OPERAND_ACC_TYPE_IP			512
#define PX_SCRIPT_ASM_OPERAND_ACC_TYPE_SP			1024
#define PX_SCRIPT_ASM_OPERAND_ACC_TYPE_BP			2048

#define PX_SCRIPT_ASM_OPERAND_ACC_TYPE_TOKEN		0xffffffff


#define __PX_SCRIPT_ASM_MNEMONIC_NAME_LEN 48
#define __PX_SCRIPT_ASM_INSTR_MNEMONIC_NAME_LEN 16

typedef struct __PX_SCRIPT_EXPORT_FUNCTION
{
	px_char		name[__PX_SCRIPT_ASM_MNEMONIC_NAME_LEN];
	px_int		Addr;
}PX_SCRIPT_EXPORT_FUNCTION;

typedef struct __PX_SCRIPT_ASM_LABEL_NODE 
{
	px_char mnemonic[__PX_SCRIPT_ASM_MNEMONIC_NAME_LEN];
	px_bool bfunction;
	px_int  binaddr;
}PX_SCRIPT_ASM_LABEL_NODE;


typedef struct __PX_SCRIPT_ASM_ASSUME_NODE 
{
	px_string name;
	px_string assume;
	
}PX_SCRIPT_ASM_ASSUME_NODE;


typedef struct __PX_SCRIPT_ASM_STRING_NODE 
{
	px_string	str;
	px_int		addr;
}PX_SCRIPT_ASM_STRING_NODE;

typedef struct __PX_SCRIPT_ASM_MEMORY_NODE 
{
	px_memory	mem;
	px_int		addr;
}PX_SCRIPT_ASM_MEMORY_NODE;

typedef struct __PX_SCRIPT_ASM_HOST_NODE 
{
	px_char		name[__PX_SCRIPT_ASM_MNEMONIC_NAME_LEN];
	const px_void	*map;
	px_void *userptr;
}PX_SCRIPT_ASM_HOST_NODE;

typedef struct __PX_SCRIPT_ASM_GRAMMAR_INSTR
{
	px_char		mnemonic[__PX_SCRIPT_ASM_INSTR_MNEMONIC_NAME_LEN];
	px_char     opcode;
	px_char     paramCount;
	px_int      operandAccTypes[3]; 
}PX_SCRIPT_ASM_GRAMMAR_INSTR;

typedef struct __PX_SCRIPT_ASM_INSTR_BIN
{
	px_char opCode;
	px_char optype[3];
	px_dword  param[3];
	px_dword  addr;
	px_int    opcount;
}PX_SCRIPT_ASM_INSTR_BIN;


typedef struct __PX_SCRIPT_ASM_COMPILER
{
	px_lexer lexer;
	px_vector StreamTable;
	px_list LabelTable;
	px_list assumeTable;
	px_list StringTable;
	px_list memoriesTable;
	px_list HostTable;
	px_list GrammarInstrTable;
	px_int globalsize;
	px_int stacksize;
	px_int threadcount;
	px_memorypool *mp;

}PX_SCRIPT_ASM_COMPILER;


typedef struct __PX_SCRIPT_ASM_HEADER
{
	//////////////////////////////////////////////////////////////////////////
	//8 bytes check
	px_dword magic;
	px_dword CRC;
	//////////////////////////////////////////////////////////////////////////
	px_dword stacksize;
	px_dword globalsize;
	px_dword threadcount;
	px_dword oftbin;
	px_dword oftfunc;
	px_dword funcCount;
	px_dword ofthost;
	px_dword oftmem;
	px_dword memsize;
	px_dword hostCount;
	px_dword oftString;
	px_dword stringSize;
	px_dword binsize;
	px_dword reserved[6];
}PX_SCRIPT_ASM_HEADER;

typedef enum __PX_SCRIPT_VM_VARIABLE_TYPE
{
	PX_SCRIPTVM_VARIABLE_TYPE_INT,
	PX_SCRIPTVM_VARIABLE_TYPE_FLOAT,
	PX_SCRIPTVM_VARIABLE_TYPE_STRING,
	PX_SCRIPTVM_VARIABLE_TYPE_MEMORY,
}PX_SCRIPTVM_VARIABLE_TYPE;

#define PX_SCRIPTVM_VARIABLE_TYPE_PTR PX_SCRIPTVM_VARIABLE_TYPE_INT

typedef struct __PX_SCRIPT_VM_VARIABLE
{
	px_int type;
	union
	{
		px_char _byte;
		px_char _char;
		px_word _word;
		px_dword _dword;
		px_short _short;
		px_int _int;
		px_uint _uint;
		px_float _float;
		px_string _string;
		px_memory _memory;
	};
}PX_SCRIPTVM_VARIABLE;

typedef struct __PX_ASM_GRAMMAR
{
	px_lexer *lexer;
	px_memorypool *mp;
	px_bool   bSkipSpacer;
	px_bool   bSkipNewline;
	px_bool   bLowercase;
	px_vector V_SentenceMatcher;
}PX_ASM_Grammar;

#endif 

