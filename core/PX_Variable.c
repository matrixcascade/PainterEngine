#include "PX_Variable.h"

px_variable PX_Variable_int(px_int _int)
{
	px_variable var;
	var.type = PX_VARIABLE_TYPE_INT;
	var._int = _int;
	return var;
}

px_variable PX_Variable_handle(px_void* _ptr)
{
	px_variable var;
	var.type = PX_VARIABLE_TYPE_HANDLE;
	var._userptr = _ptr;
	return var;
}

px_variable PX_Variable_ptr(px_void* _ptr)
{
	return PX_Variable_handle(_ptr);
}

px_variable PX_Variable_float(px_float _float)
{
	px_variable var;
	var.type = PX_VARIABLE_TYPE_FLOAT;
	var._float = _float;
	return var;
}

px_variable PX_Variable_string(px_string _string)
{
	px_variable var;
	var.type = PX_VARIABLE_TYPE_STRING;
	var._string = _string;
	return var;
}

px_variable PX_Variable_build_string(px_memorypool* mp, const px_char* buffer)
{
	px_variable var = { 0 };
	var.type = PX_VARIABLE_TYPE_STRING;
	PX_StringInitialize(mp, &var._string);
	PX_StringCat(&var._string, buffer);
	return var;
}

px_variable PX_Variable_memory(px_byte* buffer, px_int _size)
{
	px_variable var;
	var.type = PX_VARIABLE_TYPE_MEMORY;
	var._memory.allocsize = 0;
	var._memory.buffer = buffer;
	var._memory.usedsize = _size;
	return var;
}

px_variable PX_Variable_const_string(const px_char* buffer)
{
	px_variable var;
	var.type = PX_VARIABLE_TYPE_STRING;
	var._string.buffer = (px_char*)buffer;
	var._string.bufferlen = 0;
	var._string.mp = PX_NULL;
	return var;
}

px_variable PX_Variable_const_memory(const px_byte* buffer, px_int _size)
{
	px_variable var;
	var.type = PX_VARIABLE_TYPE_MEMORY;
	var._memory.allocsize = 0;
	var._memory.buffer = (px_byte*)buffer;
	var._memory.usedsize = _size;
	return var;
}

px_void PX_VariableFree(px_variable* var)
{
	if (var->type == PX_VARIABLE_TYPE_MEMORY)
	{
		PX_MemoryFree(&var->_memory);
	}
	else if (var->type == PX_VARIABLE_TYPE_STRING)
	{
		PX_StringFree(&var->_string);
	}
	var->type = PX_VARIABLE_TYPE_INT;
	var->_int = 0;
}

px_variable PX_VariableCopy(px_memorypool* mp, px_variable* pvar, px_bool* bOutOfMemory)
{
	px_variable cpyVar;
	cpyVar = *pvar;
	if (bOutOfMemory)
		*bOutOfMemory = PX_FALSE;

	if (pvar->type == PX_VARIABLE_TYPE_STRING)
	{
		if (!PX_StringInitialize(mp, &cpyVar._string))
		{
			cpyVar.type = PX_VARIABLE_TYPE_INT;
			if (bOutOfMemory)
				*bOutOfMemory = PX_TRUE;
		}
		else
		{
			if (!PX_StringCopy(&cpyVar._string, &pvar->_string))
			{
				cpyVar.type = PX_VARIABLE_TYPE_INT;
				if (bOutOfMemory)
					*bOutOfMemory = PX_TRUE;
			}
		}
	}
	else if (pvar->type == PX_VARIABLE_TYPE_MEMORY)
	{
		PX_MemoryInitialize(mp, &cpyVar._memory);
		if (!PX_MemoryCat(&cpyVar._memory, pvar->_memory.buffer, pvar->_memory.usedsize))
		{
			cpyVar.type = PX_VARIABLE_TYPE_INT;
			if (bOutOfMemory)
				*bOutOfMemory = PX_TRUE;
		}
	}
	return cpyVar;
}

px_bool PX_VariableSetInt(px_variable* var, px_int _int)
{
	PX_VariableFree(var);
	var->type = PX_VARIABLE_TYPE_INT;
	var->_int = _int;
	return PX_TRUE;
}
px_bool PX_VariableSetFloat(px_variable* var, px_float _float)
{
	PX_VariableFree(var);
	var->type = PX_VARIABLE_TYPE_FLOAT;
	var->_float = _float;
	return PX_TRUE;
}
px_bool PX_VariableSetString(px_memorypool* mp, px_variable* var, const px_char* string)
{
	PX_VariableFree(var);
	var->type = PX_VARIABLE_TYPE_STRING;
	PX_StringInitialize(mp, &var->_string);
	PX_StringCat(&var->_string, string);
	return PX_TRUE;

}
px_bool PX_VariableSetMemory(px_memorypool* mp, px_variable* var, px_byte* buffer, px_int _size)
{
	PX_VariableFree(var);
	var->type = PX_VARIABLE_TYPE_MEMORY;
	PX_MemoryInitialize(mp, &var->_memory);
	PX_MemoryCat(&var->_memory, buffer, _size);
	return PX_TRUE;
}
px_bool PX_VariableSetHandle(px_variable* var, px_void* _ptr)
{
	PX_VariableFree(var);
	var->type = PX_VARIABLE_TYPE_HANDLE;
	var->_userptr = _ptr;
	return PX_TRUE;
}
px_bool PX_VariableSetPtr(px_variable* var, px_void* _ptr)
{
	return PX_VariableSetHandle(var, _ptr);
}
