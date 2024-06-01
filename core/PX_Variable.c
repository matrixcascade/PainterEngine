#include "PX_Variable.h"

px_variable PX_Variable_int(px_int _int)
{
	px_variable var;
	var.type = PX_VM_VARIABLE_TYPE_INT;
	var._int = _int;
	return var;
}

px_variable PX_Variable_handle(px_void* _ptr)
{
	px_variable var;
	var.type = PX_VM_VARIABLE_TYPE_HANDLE;
	var._userptr = _ptr;
	return var;
}

px_variable PX_Variable_float(px_float _float)
{
	px_variable var;
	var.type = PX_VM_VARIABLE_TYPE_FLOAT;
	var._float = _float;
	return var;
}

px_variable PX_Variable_string(px_string _string)
{
	px_variable var;
	var.type = PX_VM_VARIABLE_TYPE_STRING;
	var._string = _string;
	return var;
}

px_variable PX_Variable_build_string(px_memorypool* mp, const px_char* buffer)
{
	px_variable var = { 0 };
	var.type = PX_VM_VARIABLE_TYPE_STRING;
	PX_StringInitialize(mp, &var._string);
	PX_StringCat(&var._string, buffer);
	return var;
}

px_variable PX_Variable_memory(px_byte* buffer, px_int _size)
{
	px_variable var;
	var.type = PX_VM_VARIABLE_TYPE_MEMORY;
	var._memory.allocsize = 0;
	var._memory.buffer = buffer;
	var._memory.usedsize = _size;
	return var;
}

px_variable PX_Variable_const_string(const px_char* buffer)
{
	px_variable var;
	var.type = PX_VM_VARIABLE_TYPE_STRING;
	var._string.buffer = (px_char*)buffer;
	var._string.bufferlen = 0;
	var._string.mp = PX_NULL;
	return var;
}

px_variable PX_Variable_const_memory(const px_byte* buffer, px_int _size)
{
	px_variable var;
	var.type = PX_VM_VARIABLE_TYPE_MEMORY;
	var._memory.allocsize = 0;
	var._memory.buffer = (px_byte*)buffer;
	var._memory.usedsize = _size;
	return var;
}

px_void PX_VariableFree(px_variable* var)
{
	if (var->type == PX_VM_VARIABLE_TYPE_MEMORY)
	{
		PX_MemoryFree(&var->_memory);
	}
	else if (var->type == PX_VM_VARIABLE_TYPE_STRING)
	{
		PX_StringFree(&var->_string);
	}
	var->type = PX_VM_VARIABLE_TYPE_INT;
	var->_int = 0;
}

