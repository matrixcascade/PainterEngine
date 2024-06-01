#ifndef PX_VARIBALE_H
#define PX_VARIBALE_H

#include "PX_Memory.h"
#include "PX_String.h"

typedef enum __PX_SCRIPT_VM_VARIABLE_TYPE
{
	PX_VM_VARIABLE_TYPE_INT = 1,
	PX_VM_VARIABLE_TYPE_FLOAT = 2,
	PX_VM_VARIABLE_TYPE_STRING = 4,
	PX_VM_VARIABLE_TYPE_MEMORY = 8,
	PX_VM_VARIABLE_TYPE_HANDLE = 16,
}PX_VM_VARIABLE_TYPE;

#define PX_VM_VARIABLE_TYPE_PTR PX_VM_VARIABLE_TYPE_INT
#define PX_VM_VARIABLE_TYPE_NUMERIC (PX_VM_VARIABLE_TYPE_INT|PX_VM_VARIABLE_TYPE_FLOAT)

typedef struct __PX_VM_VARIABLE
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
		px_void* _userptr;
		px_string _string;
		px_memory _memory;
	};
}px_variable;

px_variable PX_Variable_int(px_int _int);
px_void PX_VariableFree(px_variable* var);
px_variable PX_Variable_handle(px_void* _ptr);
px_variable PX_Variable_float(px_float _float);
px_variable PX_Variable_string(px_string _ref_string);
px_variable PX_Variable_build_string(px_memorypool* mp, const px_char* buffer);
px_variable PX_Variable_memory(px_byte* buffer, px_int _size);
px_variable PX_Variable_const_string(const px_char* buffer);
px_variable PX_Variable_const_memory(const px_byte* buffer, px_int _size);
px_variable PX_VariableCopy(px_memorypool* mp, px_variable *pvar, px_bool* bOutofMemory);

#endif // !PX_VARIBALE_H
