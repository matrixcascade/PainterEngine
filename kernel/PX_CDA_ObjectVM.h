#include "PX_CDA.h"

const px_char PX_CDA_Object_stdlib[] = "#name \"stdlib.h\"\n\
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
host int getportinteger(int port);\n\
host float getportfloat(int port);\n\
host string getportstring(int port);\n\
host memory getportmemory(int port);\n\
host void setportinteger(int port,int v);\n\
host void setportfloat(int port,float v);\n\
host void setportstring(int port,string v);\n\
host void setportmemory(int port,memory v);\n\
host int playanimation(string animationkey);\n\
 ";


px_bool PX_CDA_ObjectVM_Sleep(PX_VM* Ins, px_void* userptr)
{
	PX_CDA_Object* pDesc = PX_ObjectGetDescIndex(PX_CDA_Object, (PX_Object*)userptr,1);

	if (PX_VM_STACK(Ins, 0).type != PX_VM_VARIABLE_TYPE_INT)
	{
		PX_VM_RET(Ins, PX_Variable_int(0));
		return PX_FALSE;
	}
	pDesc->vm.pThread[Ins->T].sleep = PX_VM_STACK(Ins, 0)._int;
	PX_VM_RET(Ins, PX_Variable_int(0));
	return PX_TRUE;
}

px_bool PX_CDA_ObjectVM_Rand(PX_VM* Ins, px_void* userptr)
{
	PX_CDA_Object* pDesc = PX_ObjectGetDescIndex(PX_CDA_Object, (PX_Object*)userptr,1);

	pDesc->rand_seed = PX_randEx(pDesc->rand_seed);
	PX_VM_RET_float(Ins, (px_float)PX_randRange(0, 1));
	return PX_TRUE;
}

px_bool PX_CDA_ObjectVM_Sin(PX_VM* Ins, px_void* userptr)
{
	if (PX_VM_STACK(Ins, 0).type != PX_VM_VARIABLE_TYPE_FLOAT)
	{
		PX_VM_RET(Ins, PX_Variable_int(0));
		return PX_FALSE;
	}
	PX_VM_RET(Ins, PX_Variable_float((px_float)PX_sin_angle(PX_VM_STACK(Ins, 0)._float)));
	return PX_TRUE;
}

px_bool PX_CDA_ObjectVM_Cos(PX_VM* Ins, px_void* userptr)
{
	if (PX_VM_STACK(Ins, 0).type != PX_VM_VARIABLE_TYPE_FLOAT)
	{
		PX_VM_RET(Ins, PX_Variable_int(0));
		return PX_FALSE;
	}
	PX_VM_RET(Ins, PX_Variable_float((px_float)PX_cos_angle(PX_VM_STACK(Ins, 0)._float)));
	return PX_TRUE;
}

px_bool PX_CDA_ObjectVM_Sqrt(PX_VM* Ins, px_void* userptr)
{
	if (PX_VM_STACK(Ins, 0).type != PX_VM_VARIABLE_TYPE_FLOAT)
	{
		PX_VM_RET(Ins, PX_Variable_int(0));
		return PX_FALSE;
	}
	PX_VM_RET(Ins, PX_Variable_float((px_float)PX_sqrtd(PX_VM_STACK(Ins, 0)._float)));
	return PX_TRUE;
}

px_bool PX_CDA_ObjectVM_SetObjectVelocity(PX_VM* Ins, px_void* userptr)
{
	PX_Object* handler = (PX_Object*)userptr;
	px_float x, y, z;
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

px_bool PX_CDA_ObjectVM_SetObjectForce(PX_VM* Ins, px_void* userptr)
{
	PX_Object* handler = (PX_Object*)userptr;
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
	PX_ObjectSetForce(handler, x, y, z);
	return PX_TRUE;
}

px_bool PX_CDA_ObjectVM_SetObjectResistance(PX_VM* Ins, px_void* userptr)
{
	PX_Object* handler = (PX_Object*)userptr;
	px_float x;
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

	PX_ObjectSetResistance(handler, x);
	return PX_TRUE;
}

px_bool PX_CDA_ObjectVM_SetObjectName(PX_VM* Ins, px_void* userptr)
{
	PX_Object* handler = (PX_Object*)userptr;
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

px_bool PX_CDA_ObjectVM_GetObjectName(PX_VM* Ins, px_void* userptr)
{
	PX_Object* handler = (PX_Object*)userptr;
	if (PX_VM_STACK(Ins, 0).type != PX_VM_VARIABLE_TYPE_HANDLE)
	{
		PX_VM_RET(Ins, PX_Variable_int(0));
		return PX_FALSE;
	}
	handler = (PX_Object*)PX_VM_STACK(Ins, 0)._userptr;

	PX_VM_RET_String(Ins, handler->id);
	return PX_TRUE;
}

px_bool PX_CDA_ObjectVM_SetObjectImpact(PX_VM* Ins, px_void* userptr)
{
	PX_Object* handler = (PX_Object*)userptr;
	PX_CDA_Object* pDesc = PX_ObjectGetDescIndex(PX_CDA_Object, handler, 1);
	PX_CDA* pCDA = (PX_CDA*)pDesc->pCDA;
	px_int x;

	if (PX_VM_STACK(Ins, 0).type != PX_VM_VARIABLE_TYPE_INT)
	{
		PX_VM_RET(Ins, PX_Variable_int(0));
		return PX_FALSE;
	}
	x = PX_VM_STACK(Ins, 0)._int;

	return PX_TRUE;
}

px_bool PX_CDA_ObjectVM_SetObjectPosition(PX_VM* Ins, px_void* userptr)
{
	PX_Object* handler = (PX_Object*)userptr;
	PX_CDA_Object* pDesc = PX_ObjectGetDescIndex(PX_CDA_Object, handler, 1);
	PX_CDA* pCDA = (PX_CDA*)pDesc->pCDA;

	px_float x, y;

	if (PX_VM_STACK(Ins, 0).type != PX_VM_VARIABLE_TYPE_FLOAT)
	{
		PX_VM_RET(Ins, PX_Variable_int(0));
		return PX_FALSE;
	}
	x = PX_VM_STACK(Ins, 0)._float;


	if (PX_VM_STACK(Ins, 1).type != PX_VM_VARIABLE_TYPE_FLOAT)
	{
		PX_VM_RET(Ins, PX_Variable_int(0));
		return PX_FALSE;
	}
	y = PX_VM_STACK(Ins, 1)._float;


	pDesc->grid_x = x;
	pDesc->grid_y = x;

	return PX_TRUE;
}

px_bool PX_CDA_ObjectVM_GetObjectPosition(PX_VM* Ins, px_void* userptr)
{
	PX_Object* handler = (PX_Object*)userptr;
	PX_CDA_Object* pDesc = PX_ObjectGetDescIndex(PX_CDA_Object, handler, 1);
	PX_CDA* pCDA = (PX_CDA*)pDesc->pCDA;

	px_int x, y;


	if (PX_VM_STACK(Ins, 0).type != PX_VM_VARIABLE_TYPE_PTR)
	{
		PX_VM_RET(Ins, PX_Variable_int(0));
		return PX_TRUE;
	}
	x = PX_VM_STACK(Ins, 0)._int;


	if (PX_VM_STACK(Ins, 1).type != PX_VM_VARIABLE_TYPE_PTR)
	{
		PX_VM_RET(Ins, PX_Variable_int(0));
		return PX_TRUE;
	}
	y = PX_VM_STACK(Ins, 1)._int;

	if (PX_VM_GLOBAL(Ins, x).type != PX_VM_VARIABLE_TYPE_FLOAT || PX_VM_GLOBAL(Ins, y).type != PX_VM_VARIABLE_TYPE_FLOAT)
	{
		PX_VM_RET(Ins, PX_Variable_int(0));
		return PX_TRUE;
	}

	PX_VM_GLOBAL(Ins, x)._float = pDesc->grid_x;
	PX_VM_GLOBAL(Ins, y)._float = pDesc->grid_y;

	return PX_TRUE;
}

px_bool PX_CDA_ObjectVM_GetCDASize(PX_VM* Ins, px_void* userptr)
{
	PX_Object* handler = (PX_Object*)userptr;
	PX_CDA_Object* pDesc = PX_Object_GetCDAObject(handler);
	PX_CDA* pCDA = (PX_CDA*)pDesc->pCDA;

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

px_bool PX_CDA_ObjectVM_Abi_int(PX_VM* Ins, px_void* userptr)
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

px_bool PX_CDA_ObjectVM_Abi_float(PX_VM* Ins, px_void* userptr)
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

px_bool PX_CDA_ObjectVM_Abi_string(PX_VM* Ins, px_void* userptr)
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

px_bool PX_CDA_ObjectVM_Abi_point(PX_VM* Ins, px_void* userptr)
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

px_bool PX_CDA_ObjectVM_Abi_color(PX_VM* Ins, px_void* userptr)
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

px_bool PX_CDA_ObjectVM_CreateThread(PX_VM* Ins, px_void* userptr)
{
	PX_Object* handler = (PX_Object*)userptr;
	PX_CDA_Object* pDesc = PX_ObjectGetDescIndex(PX_CDA_Object, handler, 1);
	PX_CDA* pCDA = (PX_CDA*)pDesc->pCDA;


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

px_bool PX_CDA_ObjectVM_GetPortInteger(PX_VM* Ins, px_void* userptr)
{
	PX_Object* handler = (PX_Object*)userptr;
	PX_CDA_Object* pDesc = PX_ObjectGetDescIndex(PX_CDA_Object, handler, 1);
	PX_CDA* pCDA = (PX_CDA*)pDesc->pCDA;
	px_variable* pvar;
	if (PX_VM_STACK(Ins, 0).type != PX_VM_VARIABLE_TYPE_INT)
	{
		PX_VM_RET(Ins, PX_Variable_int(0));
		return PX_TRUE;
	}
	pvar=PX_CDA_ObjectGetPortVariable(handler, PX_VM_STACK(Ins, 0)._int);
	if (pvar&&pvar->type== PX_VM_VARIABLE_TYPE_INT)
	{
		PX_VM_RET(Ins, PX_Variable_int(pvar->_int));
	}
	else
	{
		PX_VM_RET(Ins, PX_Variable_int(0));
	}
	return PX_TRUE;
}

px_bool PX_CDA_ObjectVM_GetPortFloat(PX_VM* Ins, px_void* userptr)
{
	PX_Object* handler = (PX_Object*)userptr;
	PX_CDA_Object* pDesc = PX_ObjectGetDescIndex(PX_CDA_Object, handler,1);
	PX_CDA* pCDA = (PX_CDA*)pDesc->pCDA;
	px_variable* pvar;
	if (PX_VM_STACK(Ins, 0).type != PX_VM_VARIABLE_TYPE_INT)
	{
		PX_VM_RET_float(Ins, 0);
		return PX_TRUE;
	}
	pvar = PX_CDA_ObjectGetPortVariable(handler, PX_VM_STACK(Ins, 0)._int);
	if (pvar&&pvar->type == PX_VM_VARIABLE_TYPE_FLOAT)
	{
		PX_VM_RET_float(Ins, pvar->_float);
	}
	else
	{
		PX_VM_RET_float(Ins, 0);
	}
	return PX_TRUE;
}

px_bool PX_CDA_ObjectVM_GetPortString(PX_VM* Ins, px_void* userptr)
{
	PX_Object* handler = (PX_Object*)userptr;
	PX_CDA_Object* pDesc = PX_Object_GetCDAObject(handler);
	PX_CDA* pCDA = (PX_CDA*)pDesc->pCDA;
	px_variable* pvar;
	if (PX_VM_STACK(Ins, 0).type != PX_VM_VARIABLE_TYPE_INT)
	{
		PX_VM_RET_String(Ins, "");
		return PX_TRUE;
	}
	pvar = PX_CDA_ObjectGetPortVariable(handler, PX_VM_STACK(Ins, 0)._int);
	if (pvar&&pvar->type == PX_VM_VARIABLE_TYPE_STRING)
	{
		PX_VM_RET_String(Ins, pvar->_string.buffer);
	}
	else
	{
		PX_VM_RET_String(Ins, "");
	}
	return PX_TRUE;
}

px_bool PX_CDA_ObjectVM_GetPortMemory(PX_VM* Ins, px_void* userptr)
{
	PX_Object* handler = (PX_Object*)userptr;
	PX_CDA_Object* pDesc = PX_Object_GetCDAObject(handler);
	PX_CDA* pCDA = (PX_CDA*)pDesc->pCDA;
	px_variable* pvar;
	if (PX_VM_STACK(Ins, 0).type != PX_VM_VARIABLE_TYPE_INT)
	{
		PX_VM_RET_memory(Ins, PX_NULL, 0);
		return PX_TRUE;
	}
	pvar = PX_CDA_ObjectGetPortVariable(handler, PX_VM_STACK(Ins, 0)._int);
	if (pvar&&pvar->type == PX_VM_VARIABLE_TYPE_MEMORY)
	{
		PX_VM_RET_memory(Ins, pvar->_memory.buffer,pvar->_memory.usedsize);
	}
	else
	{
		PX_VM_RET_memory(Ins, PX_NULL, 0);
	}
	return PX_TRUE;
}

px_bool PX_CDA_ObjectVM_SetPortInteger(PX_VM* Ins, px_void* userptr)
{
	PX_Object* handler = (PX_Object*)userptr;
	px_int port;
	if (PX_VM_STACK(Ins, 0).type != PX_VM_VARIABLE_TYPE_INT)
	{
		PX_VM_RET(Ins, PX_Variable_int(0));
		return PX_TRUE;
	}
	port = PX_VM_STACK(Ins, 0)._int;
	if (PX_VM_STACK(Ins, 1).type != PX_VM_VARIABLE_TYPE_INT)
	{
		PX_VM_RET(Ins, PX_Variable_int(0));
		return PX_TRUE;
	}
	PX_CDA_ObjectSetPortVariable(handler, port, &PX_VM_STACK(Ins, 1));
	return PX_TRUE;
}

px_bool PX_CDA_ObjectVM_SetPortFloat(PX_VM* Ins, px_void* userptr)
{
	PX_Object* handler = (PX_Object*)userptr;
	px_int port;
	if (PX_VM_STACK(Ins, 0).type != PX_VM_VARIABLE_TYPE_INT)
	{
		PX_VM_RET(Ins, PX_Variable_int(0));
		return PX_TRUE;
	}
	port = PX_VM_STACK(Ins, 0)._int;
	if (PX_VM_STACK(Ins, 1).type != PX_VM_VARIABLE_TYPE_FLOAT)
	{
		PX_VM_RET(Ins, PX_Variable_int(0));
		return PX_TRUE;
	}
	PX_CDA_ObjectSetPortVariable(handler, port, &PX_VM_STACK(Ins, 1));
	return PX_TRUE;
}

px_bool PX_CDA_ObjectVM_SetPortString(PX_VM* Ins, px_void* userptr)
{
	PX_Object* handler = (PX_Object*)userptr;
	px_int port;
	if (PX_VM_STACK(Ins, 0).type != PX_VM_VARIABLE_TYPE_INT)
	{
		PX_VM_RET(Ins, PX_Variable_int(0));
		return PX_TRUE;
	}
	port = PX_VM_STACK(Ins, 0)._int;
	if (PX_VM_STACK(Ins, 1).type != PX_VM_VARIABLE_TYPE_STRING)
	{
		PX_VM_RET(Ins, PX_Variable_int(0));
		return PX_TRUE;
	}
	PX_CDA_ObjectSetPortVariable(handler, port, &PX_VM_STACK(Ins, 1));
	return PX_TRUE;
}

px_bool PX_CDA_ObjectVM_SetPortMemory(PX_VM* Ins, px_void* userptr)
{
	PX_Object* handler = (PX_Object*)userptr;
	px_int port;
	if (PX_VM_STACK(Ins, 0).type != PX_VM_VARIABLE_TYPE_INT)
	{
		PX_VM_RET(Ins, PX_Variable_int(0));
		return PX_TRUE;
	}
	port = PX_VM_STACK(Ins, 0)._int;
	if (PX_VM_STACK(Ins, 1).type != PX_VM_VARIABLE_TYPE_MEMORY)
	{
		PX_VM_RET(Ins, PX_Variable_int(0));
		return PX_TRUE;
	}
	PX_CDA_ObjectSetPortVariable(handler, port, &PX_VM_STACK(Ins, 1));
	return PX_TRUE;
}


px_bool PX_CDA_ObjectVM_PlayAnimation(PX_VM* Ins, px_void* userptr)
{
	PX_Object* handler = (PX_Object*)userptr;
	PX_CDA_Object* pDesc = PX_ObjectGetDescIndex(PX_CDA_Object, handler,1);
	PX_CDA* pCDA = (PX_CDA*)pDesc->pCDA;

	if (PX_VM_STACK(Ins, 0).type != PX_VM_VARIABLE_TYPE_STRING)
	{
		PX_VM_RET(Ins, PX_Variable_int(0));
		return PX_TRUE;
	}

	if (!PX_AnimationPlay(&pDesc->animation, PX_VM_STACK(Ins, 0)._string.buffer))
	{
		PX_VM_RET(Ins, PX_Variable_int(0));
	}
	else
	{
		PX_VM_RET(Ins, PX_Variable_int(1));
	}
	return PX_TRUE;
}

px_bool PX_CDA_ObjectCompileScript( const px_char script[],px_memory *pvmbin,PX_VM_DebuggerMap *pdebugmap)
{
	px_char compiler_cache_buffer[1024 * 256];//512kb for compiler cache
	px_memorypool compiler_cache = MP_Create(compiler_cache_buffer, sizeof(compiler_cache_buffer));
	PX_Compiler compiler;


	if (script && script[0])
	{
		if (!PX_CompilerInitialize(&compiler_cache, &compiler))return PX_FALSE;
		if (!PX_CompilerAddSource(&compiler, PX_CDA_Object_stdlib))return PX_FALSE;

		
		PX_CompilerAddSource(&compiler, script);

		if (!PX_CompilerCompile(&compiler, pvmbin, pdebugmap, "main"))
		{
			return PX_FALSE;
		}

		
		PX_CompilerFree(&compiler);
		
	}
	return PX_TRUE;
}
