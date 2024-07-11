#include "PX_CDA.h"

const px_char PX_CDA_stdlib[] = "#name \"stdlib.h\"\n\
host void sleep(int millionsecond);\n\
host int rand();\n\
host float sin(float x);\n\
host float cos(float x);\n\
host float sqrt(float x);\n\
host string getobjectname(handle object);\n\
host void setobjectname(handle object);\n\
host void abi_int(memory *mem,string name,int v);\n\
host void abi_float(memory *mem,string name,float v);\n\
host void abi_string(memory *mem,string name,string v);\n\
host void abi_point(memory *mem,string name,float x,float y,float z);\n\
host void abi_color(memory *mem,string name,float a,float r,float g,float b);\n\
host void getCDAsize(float *width,float *height);\n\
host void getobjectposition(handle object,float *x,float *y);\n\
host void setobjectposition(handle object, float x, float y); \n\
host void setobjectvelocity(handle object,float x,float y,float z);\n\
host void setobjectforce(handle object,float x,float y,float z);\n\
host void setobjectresistance(handle object,float ak);\n\
host void setobjecttype(handle object,int type);\n\
host void setobjectimpacttargettype(handle object,int impact_target_type);\n\
host int createthread(string callname);\n\
 ";


px_bool PX_CDA_VM_Sleep(PX_VM* Ins, px_void* userptr)
{
	PX_CDA* pCDA = (PX_CDA*)userptr;

	if (PX_VM_STACK(Ins, 0).type != PX_VM_VARIABLE_TYPE_INT)
	{
		PX_VM_RET(Ins, PX_Variable_int(0));
		return PX_FALSE;
	}
	pCDA->vm.pThread[Ins->T].sleep = PX_VM_STACK(Ins, 0)._int;
	PX_VM_RET(Ins, PX_Variable_int(0));
	return PX_TRUE;
}

px_bool PX_CDA_VM_Rand(PX_VM* Ins, px_void* userptr)
{
	PX_CDA* pCDA = (PX_CDA*)userptr;

	pCDA->rand_seed = PX_randEx(pCDA->rand_seed);
	PX_VM_RET_float(Ins, (px_float)PX_randRange(0, 1));
	return PX_TRUE;
}

px_bool PX_CDA_VM_Sin(PX_VM* Ins, px_void* userptr)
{
	if (PX_VM_STACK(Ins, 0).type != PX_VM_VARIABLE_TYPE_FLOAT)
	{
		PX_VM_RET(Ins, PX_Variable_int(0));
		return PX_FALSE;
	}
	PX_VM_RET(Ins, PX_Variable_float((px_float)PX_sin_angle(PX_VM_STACK(Ins, 0)._float)));
	return PX_TRUE;
}

px_bool PX_CDA_VM_Cos(PX_VM* Ins, px_void* userptr)
{
	if (PX_VM_STACK(Ins, 0).type != PX_VM_VARIABLE_TYPE_FLOAT)
	{
		PX_VM_RET(Ins, PX_Variable_int(0));
		return PX_FALSE;
	}
	PX_VM_RET(Ins, PX_Variable_float((px_float)PX_cos_angle(PX_VM_STACK(Ins, 0)._float)));
	return PX_TRUE;
}

px_bool PX_CDA_VM_Sqrt(PX_VM* Ins, px_void* userptr)
{
	if (PX_VM_STACK(Ins, 0).type != PX_VM_VARIABLE_TYPE_FLOAT)
	{
		PX_VM_RET(Ins, PX_Variable_int(0));
		return PX_FALSE;
	}
	PX_VM_RET(Ins, PX_Variable_float((px_float)PX_sqrtd(PX_VM_STACK(Ins, 0)._float)));
	return PX_TRUE;
}

px_bool PX_CDA_VM_SetObjectVelocity(PX_VM* Ins, px_void* userptr)
{
	PX_CDA* pCDA = (PX_CDA*)userptr;
	PX_Object* handler;
	px_float x, y, z;
	if (PX_VM_STACK(Ins, 0).type != PX_VM_VARIABLE_TYPE_HANDLE)
	{
		PX_VM_RET(Ins, PX_Variable_int(0));
		return PX_FALSE;
	}
	handler = (PX_Object*)PX_VM_STACK(Ins, 0)._userptr;

	if (PX_VM_STACK(Ins, 1).type != PX_VM_VARIABLE_TYPE_FLOAT)
	{
		PX_VM_RET(Ins, PX_Variable_int(0));
		return PX_FALSE;
	}
	x = PX_VM_STACK(Ins, 1)._float;
	if (PX_VM_STACK(Ins, 2).type != PX_VM_VARIABLE_TYPE_FLOAT)
	{
		PX_VM_RET(Ins, PX_Variable_int(0));
		return PX_FALSE;
	}
	y = PX_VM_STACK(Ins, 2)._float;
	if (PX_VM_STACK(Ins, 3).type != PX_VM_VARIABLE_TYPE_FLOAT)
	{
		PX_VM_RET(Ins, PX_Variable_int(0));
		return PX_FALSE;
	}
	z = PX_VM_STACK(Ins, 3)._float;
	PX_ObjectSetVelocity(handler, x, y, z);
	return PX_TRUE;
}


px_bool PX_CDA_VM_SetObjectName(PX_VM* Ins, px_void* userptr)
{
	PX_Object* handler;
	const px_char* x;
	if (PX_VM_STACK(Ins, 0).type != PX_VM_VARIABLE_TYPE_HANDLE)
	{
		PX_VM_RET(Ins, PX_Variable_int(0));
		return PX_FALSE;
	}
	handler = (PX_Object*)PX_VM_STACK(Ins, 0)._userptr;

	if (PX_VM_STACK(Ins, 1).type != PX_VM_VARIABLE_TYPE_STRING)
	{
		PX_VM_RET(Ins, PX_Variable_int(0));
		return PX_FALSE;
	}
	x = PX_VM_STACK(Ins, 1)._string.buffer;

	PX_ObjectSetId(handler, x);
	return PX_TRUE;
}

px_bool PX_CDA_VM_GetObjectName(PX_VM* Ins, px_void* userptr)
{
	PX_Object* handler;
	if (PX_VM_STACK(Ins, 0).type != PX_VM_VARIABLE_TYPE_HANDLE)
	{
		PX_VM_RET(Ins, PX_Variable_int(0));
		return PX_FALSE;
	}
	handler = (PX_Object*)PX_VM_STACK(Ins, 0)._userptr;

	PX_VM_RET_String(Ins, handler->id);
	return PX_TRUE;
}

px_bool PX_CDA_VM_SetObjectImpact(PX_VM* Ins, px_void* userptr)
{
	PX_Object* handler;
	px_int x;
	if (PX_VM_STACK(Ins, 0).type != PX_VM_VARIABLE_TYPE_HANDLE)
	{
		PX_VM_RET(Ins, PX_Variable_int(0));
		return PX_FALSE;
	}
	handler = (PX_Object*)PX_VM_STACK(Ins, 0)._userptr;

	if (PX_VM_STACK(Ins, 1).type != PX_VM_VARIABLE_TYPE_INT)
	{
		PX_VM_RET(Ins, PX_Variable_int(0));
		return PX_FALSE;
	}
	x = PX_VM_STACK(Ins, 1)._int;

	return PX_TRUE;
}

px_bool PX_CDA_VM_SetObjectPosition(PX_VM* Ins, px_void* userptr)
{
	PX_Object* handler;
	PX_CDA_Object *pDesc;
	px_float x, y;
	if (PX_VM_STACK(Ins, 0).type != PX_VM_VARIABLE_TYPE_HANDLE)
	{
		PX_VM_RET(Ins, PX_Variable_int(0));
		return PX_FALSE;
	}
	handler = (PX_Object*)PX_VM_STACK(Ins, 0)._userptr;

	if (PX_VM_STACK(Ins, 1).type != PX_VM_VARIABLE_TYPE_FLOAT)
	{
		PX_VM_RET(Ins, PX_Variable_int(0));
		return PX_FALSE;
	}
	x = PX_VM_STACK(Ins, 1)._float;


	if (PX_VM_STACK(Ins, 2).type != PX_VM_VARIABLE_TYPE_FLOAT)
	{
		PX_VM_RET(Ins, PX_Variable_int(0));
		return PX_FALSE;
	}
	y = PX_VM_STACK(Ins, 2)._float;

	pDesc = PX_Object_GetCDAObject(handler);
	pDesc->grid_x = x;
	pDesc->grid_y = x;

	return PX_TRUE;
}

px_bool PX_CDA_VM_GetObjectPosition(PX_VM* Ins, px_void* userptr)
{
	PX_Object* handler;
	PX_CDA_Object* pDesc;
	px_int x,y;
	if (PX_VM_STACK(Ins, 0).type != PX_VM_VARIABLE_TYPE_HANDLE)
	{
		PX_VM_RET(Ins, PX_Variable_int(0));
		return PX_FALSE;
	}
	handler = (PX_Object*)PX_VM_STACK(Ins, 0)._userptr;


	if (PX_VM_STACK(Ins, 1).type != PX_VM_VARIABLE_TYPE_PTR)
	{
		PX_VM_RET(Ins, PX_Variable_int(0));
		return PX_TRUE;
	}
	x = PX_VM_STACK(Ins, 1)._int;


	if (PX_VM_STACK(Ins, 2).type != PX_VM_VARIABLE_TYPE_PTR)
	{
		PX_VM_RET(Ins, PX_Variable_int(0));
		return PX_TRUE;
	}
	y = PX_VM_STACK(Ins, 2)._int;

	if (PX_VM_GLOBAL(Ins, x).type != PX_VM_VARIABLE_TYPE_FLOAT || PX_VM_GLOBAL(Ins, y).type != PX_VM_VARIABLE_TYPE_FLOAT)
	{
		PX_VM_RET(Ins, PX_Variable_int(0));
		return PX_TRUE;
	}
	pDesc = PX_Object_GetCDAObject(handler);

	PX_VM_GLOBAL(Ins, x)._float = pDesc->grid_x;
	PX_VM_GLOBAL(Ins, y)._float = pDesc->grid_y;

	return PX_TRUE;
}

px_bool PX_CDA_VM_GetCDASize(PX_VM* Ins, px_void* userptr)
{
	PX_CDA* pCDA = (PX_CDA*)userptr;

	px_int x, y;
	if (PX_VM_STACK(Ins, 0).type != PX_VM_VARIABLE_TYPE_PTR)
	{
		PX_VM_RET(Ins, PX_Variable_int(0));
		return PX_FALSE;
	}
	x = PX_VM_STACK(Ins, 0)._int;


	if (PX_VM_STACK(Ins, 1).type != PX_VM_VARIABLE_TYPE_PTR)
	{
		PX_VM_RET(Ins, PX_Variable_int(0));
		return PX_FALSE;
	}
	y = PX_VM_STACK(Ins, 1)._int;

	if (PX_VM_GLOBAL(Ins, x).type != PX_VM_VARIABLE_TYPE_INT || PX_VM_GLOBAL(Ins, y).type != PX_VM_VARIABLE_TYPE_INT)
	{
		PX_VM_RET(Ins, PX_Variable_int(0));
		return PX_FALSE;
	}
	PX_VM_GLOBAL(Ins, x)._int = pCDA->grid_x_count;
	PX_VM_GLOBAL(Ins, y)._int = pCDA->grid_y_count;

	return PX_TRUE;
}

px_bool PX_CDA_VM_Abi_int(PX_VM* Ins, px_void* userptr)
{
	px_int ptr;
	if (PX_VM_STACK(Ins, 0).type != PX_VM_VARIABLE_TYPE_PTR)
	{
		PX_VM_RET(Ins, PX_Variable_int(0));
		return PX_FALSE;
	}
	if (PX_VM_STACK(Ins, 1).type != PX_VM_VARIABLE_TYPE_STRING)
	{
		PX_VM_RET(Ins, PX_Variable_int(0));
		return PX_FALSE;
	}
	if (PX_VM_STACK(Ins, 2).type != PX_VM_VARIABLE_TYPE_INT)
	{
		PX_VM_RET(Ins, PX_Variable_int(0));
		return PX_FALSE;
	}
	ptr = PX_VM_STACK(Ins, 0)._int;
	if (PX_VM_GLOBAL(Ins, ptr).type != PX_VM_VARIABLE_TYPE_MEMORY)
	{
		return PX_FALSE;
	}
	PX_AbiMemoryWrite_int(&(PX_VM_GLOBAL(Ins, ptr)._memory), PX_VM_STACK(Ins, 1)._string.buffer, PX_VM_STACK(Ins, 2)._int);
	return PX_TRUE;
}

px_bool PX_CDA_VM_Abi_float(PX_VM* Ins, px_void* userptr)
{
	px_int ptr;
	if (PX_VM_STACK(Ins, 0).type != PX_VM_VARIABLE_TYPE_PTR)
	{
		PX_VM_RET(Ins, PX_Variable_int(0));
		return PX_FALSE;
	}
	if (PX_VM_STACK(Ins, 1).type != PX_VM_VARIABLE_TYPE_STRING)
	{
		PX_VM_RET(Ins, PX_Variable_int(0));
		return PX_FALSE;
	}
	if (PX_VM_STACK(Ins, 2).type != PX_VM_VARIABLE_TYPE_FLOAT)
	{
		PX_VM_RET(Ins, PX_Variable_int(0));
		return PX_FALSE;
	}
	ptr = PX_VM_STACK(Ins, 0)._int;
	if (PX_VM_GLOBAL(Ins, ptr).type != PX_VM_VARIABLE_TYPE_MEMORY)
	{
		return PX_FALSE;
	}

	PX_AbiMemoryWrite_float(&(PX_VM_GLOBAL(Ins, ptr)._memory), PX_VM_STACK(Ins, 1)._string.buffer, PX_VM_STACK(Ins, 2)._float);
	return PX_TRUE;
}

px_bool PX_CDA_VM_Abi_string(PX_VM* Ins, px_void* userptr)
{
	px_int ptr;
	if (PX_VM_STACK(Ins, 0).type != PX_VM_VARIABLE_TYPE_PTR)
	{
		PX_VM_RET(Ins, PX_Variable_int(0));
		return PX_FALSE;
	}
	if (PX_VM_STACK(Ins, 1).type != PX_VM_VARIABLE_TYPE_STRING)
	{
		PX_VM_RET(Ins, PX_Variable_int(0));
		return PX_FALSE;
	}
	if (PX_VM_STACK(Ins, 2).type != PX_VM_VARIABLE_TYPE_STRING)
	{
		PX_VM_RET(Ins, PX_Variable_int(0));
		return PX_FALSE;
	}
	ptr = PX_VM_STACK(Ins, 0)._int;
	if (PX_VM_GLOBAL(Ins, ptr).type != PX_VM_VARIABLE_TYPE_MEMORY)
	{
		return PX_FALSE;
	}
	PX_AbiMemoryWrite_string(&(PX_VM_GLOBAL(Ins, ptr)._memory), PX_VM_STACK(Ins, 1)._string.buffer, PX_VM_STACK(Ins, 2)._string.buffer);
	return PX_TRUE;
}

px_bool PX_CDA_VM_Abi_point(PX_VM* Ins, px_void* userptr)
{

	px_int ptr;
	px_float x, y, z;
	if (PX_VM_STACK(Ins, 0).type != PX_VM_VARIABLE_TYPE_PTR)
	{
		PX_VM_RET(Ins, PX_Variable_int(0));
		return PX_FALSE;
	}
	if (PX_VM_STACK(Ins, 1).type != PX_VM_VARIABLE_TYPE_STRING)
	{
		PX_VM_RET(Ins, PX_Variable_int(0));
		return PX_FALSE;
	}
	if (PX_VM_STACK(Ins, 2).type != PX_VM_VARIABLE_TYPE_FLOAT)
	{
		PX_VM_RET(Ins, PX_Variable_int(0));
		return PX_FALSE;
	}
	if (PX_VM_STACK(Ins, 3).type != PX_VM_VARIABLE_TYPE_FLOAT)
	{
		PX_VM_RET(Ins, PX_Variable_int(0));
		return PX_FALSE;
	}
	if (PX_VM_STACK(Ins, 4).type != PX_VM_VARIABLE_TYPE_FLOAT)
	{
		PX_VM_RET(Ins, PX_Variable_int(0));
		return PX_FALSE;
	}
	ptr = PX_VM_STACK(Ins, 0)._int;
	x = PX_VM_STACK(Ins, 2)._float;
	y = PX_VM_STACK(Ins, 3)._float;
	z = PX_VM_STACK(Ins, 4)._float;
	if (PX_VM_GLOBAL(Ins, ptr).type != PX_VM_VARIABLE_TYPE_MEMORY)
	{
		return PX_FALSE;
	}
	PX_AbiMemoryWrite_point(&(PX_VM_GLOBAL(Ins, ptr)._memory), PX_VM_STACK(Ins, 1)._string.buffer, PX_POINT(x, y, z));
	return PX_TRUE;
}

px_bool PX_CDA_VM_Abi_color(PX_VM* Ins, px_void* userptr)
{

	px_int ptr;
	px_float x, y, z, w;
	if (PX_VM_STACK(Ins, 0).type != PX_VM_VARIABLE_TYPE_PTR)
	{
		PX_VM_RET(Ins, PX_Variable_int(0));
		return PX_FALSE;
	}
	if (PX_VM_STACK(Ins, 1).type != PX_VM_VARIABLE_TYPE_STRING)
	{
		PX_VM_RET(Ins, PX_Variable_int(0));
		return PX_FALSE;
	}
	if (PX_VM_STACK(Ins, 2).type != PX_VM_VARIABLE_TYPE_FLOAT)
	{
		PX_VM_RET(Ins, PX_Variable_int(0));
		return PX_FALSE;
	}
	if (PX_VM_STACK(Ins, 3).type != PX_VM_VARIABLE_TYPE_FLOAT)
	{
		PX_VM_RET(Ins, PX_Variable_int(0));
		return PX_FALSE;
	}
	if (PX_VM_STACK(Ins, 4).type != PX_VM_VARIABLE_TYPE_FLOAT)
	{
		PX_VM_RET(Ins, PX_Variable_int(0));
		return PX_FALSE;
	}
	if (PX_VM_STACK(Ins, 5).type != PX_VM_VARIABLE_TYPE_FLOAT)
	{
		PX_VM_RET(Ins, PX_Variable_int(0));
		return PX_FALSE;
	}
	ptr = PX_VM_STACK(Ins, 0)._int;
	x = PX_VM_STACK(Ins, 2)._float;
	y = PX_VM_STACK(Ins, 3)._float;
	z = PX_VM_STACK(Ins, 4)._float;
	w = PX_VM_STACK(Ins, 5)._float;
	if (PX_VM_GLOBAL(Ins, ptr).type != PX_VM_VARIABLE_TYPE_MEMORY)
	{
		return PX_FALSE;
	}
	PX_AbiMemoryWrite_color(&(PX_VM_GLOBAL(Ins, ptr)._memory), PX_VM_STACK(Ins, 1)._string.buffer, PX_COLORF(x, y, z, w));
	return PX_TRUE;
}

px_bool PX_CDA_VM_CreateThread(PX_VM* Ins, px_void* userptr)
{
	PX_CDA* pDesc = (PX_CDA*)userptr;


	if (PX_VM_STACK(Ins, 0).type != PX_VM_VARIABLE_TYPE_STRING)
	{
		PX_VM_RET(Ins, PX_Variable_int(0));
		return PX_TRUE;
	}

	if (!PX_VMBeginThreadFunction(&pDesc->vm, PX_VMGetFreeThreadId(Ins), PX_VM_STACK(Ins, 0)._string.buffer, PX_NULL, 0))
	{
		PX_VM_RET(Ins, PX_Variable_int(0));
	}
	return PX_TRUE;
}

px_bool PX_CDA_LoadScript(PX_CDA* pCDA, const px_char script[])
{
	px_char compiler_cache_buffer[1024 * 512];//512kb for compiler cache
	px_memorypool compiler_cache = MP_Create(compiler_cache_buffer, sizeof(compiler_cache_buffer));
	PX_Compiler compiler;
	px_memory vmbin;

	if (script && script[0])
	{
		if (!PX_CompilerInitialize(&compiler_cache, &compiler))return PX_FALSE;
		if (!PX_CompilerAddSource(&compiler, PX_CDA_stdlib))return PX_FALSE;
		PX_MemoryInitialize(&compiler_cache, &vmbin);
		if (!PX_VMDebuggerMapInitialize(pCDA->mp, &pCDA->debugmap))return PX_FALSE;
		PX_CompilerAddSource(&compiler, script);

		if (!PX_CompilerCompile(&compiler, &vmbin, &pCDA->debugmap, "main"))
		{
			return PX_FALSE;
		}

		if (!PX_VMInitialize(&pCDA->vm, pCDA->mp, vmbin.buffer, vmbin.usedsize))
		{
			return PX_FALSE;
		}
		PX_CompilerFree(&compiler);
		PX_memset(&compiler, 0, sizeof(PX_Compiler));
		PX_MemoryFree(&vmbin);
		PX_memset(&vmbin, 0, sizeof(px_memory));
		PX_VMRegistHostFunction(&pCDA->vm, "sleep", PX_CDA_VM_Sleep, pCDA);//Sleep
		PX_VMRegistHostFunction(&pCDA->vm, "rand", PX_CDA_VM_Rand, pCDA);//Rand
		PX_VMRegistHostFunction(&pCDA->vm, "sin", PX_CDA_VM_Sin, pCDA);//Sin
		PX_VMRegistHostFunction(&pCDA->vm, "cos", PX_CDA_VM_Cos, pCDA);//Cos
		PX_VMRegistHostFunction(&pCDA->vm, "sqrt", PX_CDA_VM_Sqrt, pCDA);//sqrt
		PX_VMRegistHostFunction(&pCDA->vm, "getCDAsize", PX_CDA_VM_GetCDASize, pCDA);//getCDAsize
		PX_VMRegistHostFunction(&pCDA->vm, "getobjectname", PX_CDA_VM_GetObjectName, pCDA);//getobjectname
		PX_VMRegistHostFunction(&pCDA->vm, "setobjectname", PX_CDA_VM_SetObjectName, pCDA);//setobjectname
		PX_VMRegistHostFunction(&pCDA->vm, "setobjectposition", PX_CDA_VM_SetObjectPosition, pCDA);//setobjectvelocity
		PX_VMRegistHostFunction(&pCDA->vm, "getobjectposition", PX_CDA_VM_GetObjectPosition, pCDA);//setobjectvelocity
		PX_VMRegistHostFunction(&pCDA->vm, "setobjectvelocity", PX_CDA_VM_SetObjectVelocity, pCDA);//setobjectvelocity
		PX_VMRegistHostFunction(&pCDA->vm, "abi_int", PX_CDA_VM_Abi_int, pCDA);//abi_int
		PX_VMRegistHostFunction(&pCDA->vm, "abi_float", PX_CDA_VM_Abi_float, pCDA);//abi_float
		PX_VMRegistHostFunction(&pCDA->vm, "abi_string", PX_CDA_VM_Abi_string, pCDA);//abi_string
		PX_VMRegistHostFunction(&pCDA->vm, "abi_point", PX_CDA_VM_Abi_point, pCDA);//abi_point
		PX_VMRegistHostFunction(&pCDA->vm, "abi_color", PX_CDA_VM_Abi_color, pCDA);//abi_color
		PX_VMRegistHostFunction(&pCDA->vm, "createthread", PX_CDA_VM_CreateThread, pCDA);//createthread
		PX_VMBeginThreadFunction(&pCDA->vm, 0, "main", 0, 0);
	}
	return PX_TRUE;
}