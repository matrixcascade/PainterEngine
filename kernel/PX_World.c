#include "PX_World.h"



px_bool PX_World_VM_Sleep(PX_VM* Ins, px_void* userptr)
{
	
	PX_World *pWorld=(PX_World *)userptr;

	if (PX_VM_STACK(Ins, 0).type != PX_VM_VARIABLE_TYPE_INT)
	{
		PX_VM_RET(Ins, PX_VM_Variable_int(0));
		return PX_FALSE;
	}
	pWorld->vm.pThread[Ins->T].sleep = PX_VM_STACK(Ins, 0)._int;
	PX_VM_RET(Ins, PX_VM_Variable_int(0));
	return PX_TRUE;
}

px_bool PX_World_VM_Rand(PX_VM* Ins, px_void* userptr)
{
	PX_World* pWorld = (PX_World*)userptr;
	pWorld->rand_seed = PX_randEx(pWorld->rand_seed);
	PX_VM_RET_float(Ins, (px_float)PX_randRange(0,1));
	return PX_TRUE;
}

px_bool PX_World_VM_Sin(PX_VM* Ins, px_void* userptr)
{
	if (PX_VM_STACK(Ins, 0).type != PX_VM_VARIABLE_TYPE_FLOAT)
	{
		PX_VM_RET(Ins, PX_VM_Variable_int(0));
		return PX_FALSE;
	}
	PX_VM_RET(Ins, PX_VM_Variable_float((px_float)PX_sin_angle(PX_VM_STACK(Ins, 0)._float)));
	return PX_TRUE;
}

px_bool PX_World_VM_Cos(PX_VM* Ins, px_void* userptr)
{
	if (PX_VM_STACK(Ins, 0).type != PX_VM_VARIABLE_TYPE_FLOAT)
	{
		PX_VM_RET(Ins, PX_VM_Variable_int(0));
		return PX_FALSE;
	}
	PX_VM_RET(Ins, PX_VM_Variable_float((px_float)PX_cos_angle(PX_VM_STACK(Ins, 0)._float)));
	return PX_TRUE;
}

px_bool PX_World_VM_Sqrt(PX_VM* Ins, px_void* userptr)
{
	if (PX_VM_STACK(Ins, 0).type != PX_VM_VARIABLE_TYPE_FLOAT)
	{
		PX_VM_RET(Ins, PX_VM_Variable_int(0));
		return PX_FALSE;
	}
	PX_VM_RET(Ins, PX_VM_Variable_float((px_float)PX_sqrtd(PX_VM_STACK(Ins, 0)._float)));
	return PX_TRUE;
}


px_bool PX_World_VM_SetObjectVelocity(PX_VM* Ins, px_void* userptr)
{
	PX_World* pWorld = (PX_World*)userptr;
	PX_Object *handler;
	px_float x, y, z;
	if (PX_VM_STACK(Ins, 0).type != PX_VM_VARIABLE_TYPE_HANDLE)
	{
		PX_VM_RET(Ins, PX_VM_Variable_int(0));
		return PX_FALSE;
	}
	handler = (PX_Object*)PX_VM_STACK(Ins, 0)._userptr;

	if (PX_VM_STACK(Ins, 1).type != PX_VM_VARIABLE_TYPE_FLOAT)
	{
		PX_VM_RET(Ins, PX_VM_Variable_int(0));
		return PX_FALSE;
	}
	x = PX_VM_STACK(Ins, 1)._float;
	if (PX_VM_STACK(Ins, 2).type != PX_VM_VARIABLE_TYPE_FLOAT)
	{
		PX_VM_RET(Ins, PX_VM_Variable_int(0));
		return PX_FALSE;
	}
	y = PX_VM_STACK(Ins, 2)._float;
	if (PX_VM_STACK(Ins, 3).type != PX_VM_VARIABLE_TYPE_FLOAT)
	{
		PX_VM_RET(Ins, PX_VM_Variable_int(0));
		return PX_FALSE;
	}
	z = PX_VM_STACK(Ins, 3)._float;
	PX_ObjectSetVelocity(handler, x, y, z);
	return PX_TRUE;
}

px_bool PX_World_VM_SetObjectForce(PX_VM* Ins, px_void* userptr)
{
	PX_World* pWorld = (PX_World*)userptr;
	PX_Object* handler;
	px_float x, y, z;
	if (PX_VM_STACK(Ins, 0).type != PX_VM_VARIABLE_TYPE_HANDLE)
	{
		PX_VM_RET(Ins, PX_VM_Variable_int(0));
		return PX_FALSE;
	}
	handler = (PX_Object*)PX_VM_STACK(Ins, 0)._userptr;

	if (PX_VM_STACK(Ins, 1).type != PX_VM_VARIABLE_TYPE_FLOAT)
	{
		PX_VM_RET(Ins, PX_VM_Variable_int(0));
		return PX_FALSE;
	}
	x = PX_VM_STACK(Ins, 1)._float;
	if (PX_VM_STACK(Ins, 2).type != PX_VM_VARIABLE_TYPE_FLOAT)
	{
		PX_VM_RET(Ins, PX_VM_Variable_int(0));
		return PX_FALSE;
	}
	y = PX_VM_STACK(Ins, 2)._float;
	if (PX_VM_STACK(Ins, 3).type != PX_VM_VARIABLE_TYPE_FLOAT)
	{
		PX_VM_RET(Ins, PX_VM_Variable_int(0));
		return PX_FALSE;
	}
	z = PX_VM_STACK(Ins, 3)._float;
	PX_ObjectSetForce(handler, x, y, z);
	return PX_TRUE;
}

px_bool PX_World_VM_SetObjectResistance(PX_VM* Ins, px_void* userptr)
{
	PX_World* pWorld = (PX_World*)userptr;
	PX_Object* handler;
	px_float x;
	if (PX_VM_STACK(Ins, 0).type != PX_VM_VARIABLE_TYPE_HANDLE)
	{
		PX_VM_RET(Ins, PX_VM_Variable_int(0));
		return PX_FALSE;
	}
	handler = (PX_Object*)PX_VM_STACK(Ins, 0)._userptr;

	if (PX_VM_STACK(Ins, 1).type != PX_VM_VARIABLE_TYPE_FLOAT)
	{
		PX_VM_RET(Ins, PX_VM_Variable_int(0));
		return PX_FALSE;
	}
	x = PX_VM_STACK(Ins, 1)._float;
	
	PX_ObjectSetResistance(handler, x);
	return PX_TRUE;
}


px_bool PX_World_VM_SetObjectType(PX_VM* Ins, px_void* userptr)
{
	PX_World* pWorld = (PX_World*)userptr;
	PX_Object* handler;
	px_int x;
	if (PX_VM_STACK(Ins, 0).type != PX_VM_VARIABLE_TYPE_HANDLE)
	{
		PX_VM_RET(Ins, PX_VM_Variable_int(0));
		return PX_FALSE;
	}
	handler = (PX_Object*)PX_VM_STACK(Ins, 0)._userptr;

	if (PX_VM_STACK(Ins, 1).type != PX_VM_VARIABLE_TYPE_INT)
	{
		PX_VM_RET(Ins, PX_VM_Variable_int(0));
		return PX_FALSE;
	}
	x = PX_VM_STACK(Ins, 1)._int;

	handler->impact_object_type = x;
	return PX_TRUE;
}

px_bool PX_World_VM_SetObjectName(PX_VM* Ins, px_void* userptr)
{
	PX_World* pWorld = (PX_World*)userptr;
	PX_Object* handler;
	const px_char * x;
	if (PX_VM_STACK(Ins, 0).type != PX_VM_VARIABLE_TYPE_HANDLE)
	{
		PX_VM_RET(Ins, PX_VM_Variable_int(0));
		return PX_FALSE;
	}
	handler = (PX_Object*)PX_VM_STACK(Ins, 0)._userptr;

	if (PX_VM_STACK(Ins, 1).type != PX_VM_VARIABLE_TYPE_STRING)
	{
		PX_VM_RET(Ins, PX_VM_Variable_int(0));
		return PX_FALSE;
	}
	x = PX_VM_STACK(Ins, 1)._string.buffer;

	PX_ObjectSetId(handler, x);
	return PX_TRUE;
}

px_bool PX_World_VM_GetObjectName(PX_VM* Ins, px_void* userptr)
{
	PX_World* pWorld = (PX_World*)userptr;
	PX_Object* handler;
	if (PX_VM_STACK(Ins, 0).type != PX_VM_VARIABLE_TYPE_HANDLE)
	{
		PX_VM_RET(Ins, PX_VM_Variable_int(0));
		return PX_FALSE;
	}
	handler = (PX_Object*)PX_VM_STACK(Ins, 0)._userptr;

	PX_VM_RET_String(Ins, handler->id);
	return PX_TRUE;
}


px_bool PX_World_VM_SetObjectImpactTargetType(PX_VM* Ins, px_void* userptr)
{
	PX_World* pWorld = (PX_World*)userptr;
	PX_Object* handler;
	px_int x;
	if (PX_VM_STACK(Ins, 0).type != PX_VM_VARIABLE_TYPE_HANDLE)
	{
		PX_VM_RET(Ins, PX_VM_Variable_int(0));
		return PX_FALSE;
	}
	handler = (PX_Object*)PX_VM_STACK(Ins, 0)._userptr;

	if (PX_VM_STACK(Ins, 1).type != PX_VM_VARIABLE_TYPE_INT)
	{
		PX_VM_RET(Ins, PX_VM_Variable_int(0));
		return PX_FALSE;
	}
	x = PX_VM_STACK(Ins, 1)._int;

	handler->impact_target_type = x;
	return PX_TRUE;
}

px_bool PX_World_VM_SetObjectPosition(PX_VM* Ins, px_void* userptr)
{
	PX_World* pWorld = (PX_World*)userptr;
	PX_Object* handler;
	px_float x, y;
	if (PX_VM_STACK(Ins, 0).type != PX_VM_VARIABLE_TYPE_HANDLE)
	{
		PX_VM_RET(Ins, PX_VM_Variable_int(0));
		return PX_FALSE;
	}
	handler = (PX_Object*)PX_VM_STACK(Ins, 0)._userptr;

	if (PX_VM_STACK(Ins, 1).type != PX_VM_VARIABLE_TYPE_FLOAT)
	{
		PX_VM_RET(Ins, PX_VM_Variable_int(0));
		return PX_FALSE;
	}
	x = PX_VM_STACK(Ins, 1)._float;


	if (PX_VM_STACK(Ins, 2).type != PX_VM_VARIABLE_TYPE_FLOAT)
	{
		PX_VM_RET(Ins, PX_VM_Variable_int(0));
		return PX_FALSE;
	}
	y = PX_VM_STACK(Ins, 2)._float;


   handler->x=x;
   handler->y = y;

	return PX_TRUE;
}

px_bool PX_World_VM_GetObjectPosition(PX_VM* Ins, px_void* userptr)
{
	PX_World* pWorld = (PX_World*)userptr;
	PX_Object* handler;
	px_int x,y;
	if (PX_VM_STACK(Ins, 0).type != PX_VM_VARIABLE_TYPE_HANDLE)
	{
		PX_VM_RET(Ins, PX_VM_Variable_int(0));
		return PX_TRUE;
	}
	handler = (PX_Object*)PX_VM_STACK(Ins, 0)._userptr;

	if (PX_VM_STACK(Ins, 1).type != PX_VM_VARIABLE_TYPE_PTR)
	{
		PX_VM_RET(Ins, PX_VM_Variable_int(0));
		return PX_TRUE;
	}
	x = PX_VM_STACK(Ins, 1)._int;


	if (PX_VM_STACK(Ins, 2).type != PX_VM_VARIABLE_TYPE_PTR)
	{
		PX_VM_RET(Ins, PX_VM_Variable_int(0));
		return PX_TRUE;
	}
	y = PX_VM_STACK(Ins, 2)._int;

	if (PX_VM_GLOBAL(Ins,x).type!=PX_VM_VARIABLE_TYPE_FLOAT|| PX_VM_GLOBAL(Ins, y).type != PX_VM_VARIABLE_TYPE_FLOAT)
	{
		PX_VM_RET(Ins, PX_VM_Variable_int(0));
		return PX_TRUE;
	}
	PX_VM_GLOBAL(Ins, y)._float = handler->x;
	PX_VM_GLOBAL(Ins, y)._float = handler->y;

	return PX_TRUE;
}

px_bool PX_World_VM_GetWorldSize(PX_VM* Ins, px_void* userptr)
{
	PX_World* pWorld = (PX_World*)userptr;
	px_int x, y;
	if (PX_VM_STACK(Ins, 0).type != PX_VM_VARIABLE_TYPE_PTR)
	{
		PX_VM_RET(Ins, PX_VM_Variable_int(0));
		return PX_FALSE;
	}
	x = PX_VM_STACK(Ins, 0)._int;


	if (PX_VM_STACK(Ins, 1).type != PX_VM_VARIABLE_TYPE_PTR)
	{
		PX_VM_RET(Ins, PX_VM_Variable_int(0));
		return PX_FALSE;
	}
	y = PX_VM_STACK(Ins, 1)._int;

	if (PX_VM_GLOBAL(Ins, x).type != PX_VM_VARIABLE_TYPE_FLOAT || PX_VM_GLOBAL(Ins, y).type != PX_VM_VARIABLE_TYPE_FLOAT)
	{
		PX_VM_RET(Ins, PX_VM_Variable_int(0));
		return PX_FALSE;
	}
	PX_VM_GLOBAL(Ins, x)._float = pWorld->world_width*1.f;
	PX_VM_GLOBAL(Ins, y)._float = pWorld->world_height*1.f;

	return PX_TRUE;
}

px_bool PX_World_VM_Abi_int(PX_VM* Ins, px_void* userptr)
{
	PX_World* pWorld = (PX_World*)userptr;
	px_int ptr;
	if (PX_VM_STACK(Ins, 0).type != PX_VM_VARIABLE_TYPE_PTR)
	{
		PX_VM_RET(Ins, PX_VM_Variable_int(0));
		return PX_FALSE;
	}
	if (PX_VM_STACK(Ins, 1).type != PX_VM_VARIABLE_TYPE_STRING)
	{
		PX_VM_RET(Ins, PX_VM_Variable_int(0));
		return PX_FALSE;
	}
	if (PX_VM_STACK(Ins, 2).type != PX_VM_VARIABLE_TYPE_INT)
	{
		PX_VM_RET(Ins, PX_VM_Variable_int(0));
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
px_bool PX_World_VM_Abi_float(PX_VM* Ins, px_void* userptr)
{
	PX_World* pWorld = (PX_World*)userptr;
	px_int ptr;
	if (PX_VM_STACK(Ins, 0).type != PX_VM_VARIABLE_TYPE_PTR)
	{
		PX_VM_RET(Ins, PX_VM_Variable_int(0));
		return PX_FALSE;
	}
	if (PX_VM_STACK(Ins, 1).type != PX_VM_VARIABLE_TYPE_STRING)
	{
		PX_VM_RET(Ins, PX_VM_Variable_int(0));
		return PX_FALSE;
	}
	if (PX_VM_STACK(Ins, 2).type != PX_VM_VARIABLE_TYPE_FLOAT)
	{
		PX_VM_RET(Ins, PX_VM_Variable_int(0));
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
px_bool PX_World_VM_Abi_string(PX_VM* Ins, px_void* userptr)
{
	PX_World* pWorld = (PX_World*)userptr;
	px_int ptr;
	if (PX_VM_STACK(Ins, 0).type != PX_VM_VARIABLE_TYPE_PTR)
	{
		PX_VM_RET(Ins, PX_VM_Variable_int(0));
		return PX_FALSE;
	}
	if (PX_VM_STACK(Ins, 1).type != PX_VM_VARIABLE_TYPE_STRING)
	{
		PX_VM_RET(Ins, PX_VM_Variable_int(0));
		return PX_FALSE;
	}
	if (PX_VM_STACK(Ins, 2).type != PX_VM_VARIABLE_TYPE_STRING)
	{
		PX_VM_RET(Ins, PX_VM_Variable_int(0));
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
px_bool PX_World_VM_Abi_point(PX_VM* Ins, px_void* userptr)
{
	PX_World* pWorld = (PX_World*)userptr;
	px_int ptr;
	px_float x, y, z;
	if (PX_VM_STACK(Ins, 0).type != PX_VM_VARIABLE_TYPE_PTR)
	{
		PX_VM_RET(Ins, PX_VM_Variable_int(0));
		return PX_FALSE;
	}
	if (PX_VM_STACK(Ins, 1).type != PX_VM_VARIABLE_TYPE_STRING)
	{
		PX_VM_RET(Ins, PX_VM_Variable_int(0));
		return PX_FALSE;
	}
	if (PX_VM_STACK(Ins, 2).type != PX_VM_VARIABLE_TYPE_FLOAT)
	{
		PX_VM_RET(Ins, PX_VM_Variable_int(0));
		return PX_FALSE;
	}
	if (PX_VM_STACK(Ins, 3).type != PX_VM_VARIABLE_TYPE_FLOAT)
	{
		PX_VM_RET(Ins, PX_VM_Variable_int(0));
		return PX_FALSE;
	}
	if (PX_VM_STACK(Ins, 4).type != PX_VM_VARIABLE_TYPE_FLOAT)
	{
		PX_VM_RET(Ins, PX_VM_Variable_int(0));
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
	PX_AbiMemoryWrite_point(&(PX_VM_GLOBAL(Ins, ptr)._memory), PX_VM_STACK(Ins, 1)._string.buffer, PX_POINT(x,y,z));
	return PX_TRUE;
}
px_bool PX_World_VM_Abi_color(PX_VM* Ins, px_void* userptr)
{
	PX_World* pWorld = (PX_World*)userptr;
	px_int ptr;
	px_float x, y, z,w;
	if (PX_VM_STACK(Ins, 0).type != PX_VM_VARIABLE_TYPE_PTR)
	{
		PX_VM_RET(Ins, PX_VM_Variable_int(0));
		return PX_FALSE;
	}
	if (PX_VM_STACK(Ins, 1).type != PX_VM_VARIABLE_TYPE_STRING)
	{
		PX_VM_RET(Ins, PX_VM_Variable_int(0));
		return PX_FALSE;
	}
	if (PX_VM_STACK(Ins, 2).type != PX_VM_VARIABLE_TYPE_FLOAT)
	{
		PX_VM_RET(Ins, PX_VM_Variable_int(0));
		return PX_FALSE;
	}
	if (PX_VM_STACK(Ins, 3).type != PX_VM_VARIABLE_TYPE_FLOAT)
	{
		PX_VM_RET(Ins, PX_VM_Variable_int(0));
		return PX_FALSE;
	}
	if (PX_VM_STACK(Ins, 4).type != PX_VM_VARIABLE_TYPE_FLOAT)
	{
		PX_VM_RET(Ins, PX_VM_Variable_int(0));
		return PX_FALSE;
	}
	if (PX_VM_STACK(Ins, 5).type != PX_VM_VARIABLE_TYPE_FLOAT)
	{
		PX_VM_RET(Ins, PX_VM_Variable_int(0));
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
	PX_AbiMemoryWrite_color(&(PX_VM_GLOBAL(Ins, ptr)._memory), PX_VM_STACK(Ins, 1)._string.buffer, PX_COLORF(x, y, z,w));
	return PX_TRUE;
}



px_bool PX_World_VM_CreateThread(PX_VM* Ins, px_void* userptr)
{	
	PX_World *pWorld=(PX_World *)userptr;

	if (PX_VM_STACK(Ins, 0).type != PX_VM_VARIABLE_TYPE_STRING)
	{
		PX_VM_RET(Ins, PX_VM_Variable_int(0));
		return PX_TRUE;
	}

	if (!PX_VMBeginThreadFunction(&pWorld->vm, PX_VMGetFreeThreadId(Ins), PX_VM_STACK(Ins, 0)._string.buffer, PX_NULL, 0))
	{
		PX_VM_RET(Ins, PX_VM_Variable_int(0));
	}
	return PX_TRUE;
}



px_bool PX_WorldInitialize(px_memorypool *mp,PX_World *pWorld,px_int world_width,px_int world_height,px_int surface_width,px_int surface_height,const px_char script[])
{
	px_void *ptr;
	const px_char stdlib[] = "#name \"stdlib.h\"\n\
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
host void getworldsize(float *width,float *height);\n\
host void getobjectposition(handle object,float *x,float *y);\n\
host void setobjectposition(handle object, float x, float y); \n\
host void setobjectvelocity(handle object,float x,float y,float z);\n\
host void setobjectforce(handle object,float x,float y,float z);\n\
host void setobjectresistance(handle object,float ak);\n\
host void setobjecttype(handle object,int type);\n\
host void setobjectimpacttargettype(handle object,int impact_target_type);\n\
host int createthread(string callname);\n\
 ";
	PX_memset(pWorld, 0, sizeof(PX_World));
	if(!PX_MapInitialize(mp,&pWorld->classes)) return PX_FALSE;
	if(!PX_VectorInitialize(mp,&pWorld->pObjects,sizeof(PX_WorldObject),256)) return PX_FALSE;
	if(!PX_VectorInitialize(mp,&pWorld->pNewObjects,sizeof(PX_Object *),128)) return PX_FALSE;
	pWorld->mp=mp;
	pWorld->world_height=world_height;
	pWorld->world_width=world_width;
	pWorld->surface_height=surface_height;
	pWorld->surface_width=surface_width;
	pWorld->auxiliaryline=PX_TRUE;
	pWorld->auxiliaryline_color=PX_COLOR(64,255,192,255);
	pWorld->camera_offset=PX_POINT(0,0,0);
	pWorld->aliveCount=0;
	pWorld->offsetx=0;
	pWorld->offsety=0;
	PX_WorldSetAuxiliaryXYSpacer(pWorld,32,32);
	if (PX_WORLD_CALC_SIZE)
	{
		ptr = MP_Malloc(pWorld->mp, PX_WORLD_CALC_SIZE);
		if (!ptr)
		{
			return PX_FALSE;
		}
		pWorld->mp_WorldCalc = MP_Create(ptr, PX_WORLD_CALC_SIZE);
	}
	PX_memset(pWorld->Impact_Test_array, 0, sizeof(pWorld->Impact_Test_array));
	if (script&&script[0])
	{
		if (!PX_CompilerInitialize(pWorld->mp, &pWorld->compiler))return PX_FALSE;
		if (!PX_CompilerAddSource(&pWorld->compiler, stdlib))return PX_FALSE;
		PX_MemoryInitialize(pWorld->mp, &pWorld->vmbin);
		if (!PX_VMDebuggerMapInitialize(pWorld->mp, &pWorld->debugmap))return PX_FALSE;
		PX_CompilerAddSource(&pWorld->compiler, script);

		if (!PX_CompilerCompile(&pWorld->compiler, &pWorld->vmbin, &pWorld->debugmap, "main"))
		{
			return PX_FALSE;
		}

		if (!PX_VMInitialize(&pWorld->vm, pWorld->mp, pWorld->vmbin.buffer, pWorld->vmbin.usedsize))
		{
			return PX_FALSE;
		}
		PX_CompilerFree(&pWorld->compiler);
		PX_memset(&pWorld->compiler,0,sizeof(PX_Compiler));
		PX_MemoryFree(&pWorld->vmbin);
		PX_memset(&pWorld->vmbin,0,sizeof(px_memory));
		PX_VMRegistHostFunction(&pWorld->vm, "sleep", PX_World_VM_Sleep, pWorld);
		PX_VMRegistHostFunction(&pWorld->vm, "sleep", PX_World_VM_Sleep, pWorld);//Sleep
		PX_VMRegistHostFunction(&pWorld->vm, "rand", PX_World_VM_Rand, pWorld);//Rand
		PX_VMRegistHostFunction(&pWorld->vm, "sin", PX_World_VM_Sin, pWorld);//Sin
		PX_VMRegistHostFunction(&pWorld->vm, "cos", PX_World_VM_Cos, pWorld);//Cos
		PX_VMRegistHostFunction(&pWorld->vm, "sqrt", PX_World_VM_Sqrt, pWorld);//sqrt
		PX_VMRegistHostFunction(&pWorld->vm, "getworldsize", PX_World_VM_GetWorldSize, pWorld);//getworldsize
		PX_VMRegistHostFunction(&pWorld->vm, "getobjectname", PX_World_VM_GetObjectName, pWorld);//getobjectname
		PX_VMRegistHostFunction(&pWorld->vm, "setobjectname", PX_World_VM_SetObjectName, pWorld);//setobjectname
		PX_VMRegistHostFunction(&pWorld->vm, "setobjectposition", PX_World_VM_SetObjectPosition, pWorld);//setobjectvelocity
		PX_VMRegistHostFunction(&pWorld->vm, "getobjectposition", PX_World_VM_GetObjectPosition, pWorld);//setobjectvelocity
		PX_VMRegistHostFunction(&pWorld->vm, "setobjectvelocity", PX_World_VM_SetObjectVelocity, pWorld);//setobjectvelocity
		PX_VMRegistHostFunction(&pWorld->vm, "setobjectforce", PX_World_VM_SetObjectForce, pWorld);//setobjectforce
		PX_VMRegistHostFunction(&pWorld->vm, "setobjectresistance", PX_World_VM_SetObjectResistance, pWorld);//setobjectresistance
		PX_VMRegistHostFunction(&pWorld->vm, "setobjecttype", PX_World_VM_SetObjectType, pWorld);//setobjecttype
		PX_VMRegistHostFunction(&pWorld->vm, "setobjectimpacttargettype", PX_World_VM_SetObjectImpactTargetType, pWorld);//setobjectimpacttargettype
		PX_VMRegistHostFunction(&pWorld->vm, "abi_int", PX_World_VM_Abi_int, pWorld);//abi_int
		PX_VMRegistHostFunction(&pWorld->vm, "abi_float", PX_World_VM_Abi_float, pWorld);//abi_float
		PX_VMRegistHostFunction(&pWorld->vm, "abi_string", PX_World_VM_Abi_string, pWorld);//abi_string
		PX_VMRegistHostFunction(&pWorld->vm, "abi_point", PX_World_VM_Abi_point, pWorld);//abi_point
		PX_VMRegistHostFunction(&pWorld->vm, "abi_color", PX_World_VM_Abi_color, pWorld);//abi_color
		PX_VMRegistHostFunction(&pWorld->vm, "createthread", PX_World_VM_CreateThread, pWorld);//createthread
		PX_VMBeginThreadFunction(&pWorld->vm, 0, "main", 0, 0);
	}
	

	return PX_TRUE;
}



px_void PX_WorldSetBackwardTexture(PX_World* pWorld, px_texture* ptexture)
{
	pWorld->pbackgroundtexture = ptexture;
}

px_void PX_WorldSetFrontwardTexture(PX_World* pWorld, px_texture* ptexture)
{
	pWorld->pfrontwardtexture = ptexture;
}

px_void PX_WorldSetAuxiliaryXYSpacer(PX_World *pw,px_int x,px_int y)
{
	pw->auxiliaryXSpacer=x;
	pw->auxiliaryYSpacer=y;
}

px_void PX_WorldEnableAuxiliaryLine(PX_World *pw,px_bool bline)
{
	pw->auxiliaryline=bline;
}


px_int PX_WorldGetCount(PX_World *World)
{
	return World->pObjects.size;
}

px_int PX_WorldGetAliveCount(PX_World* World)
{
	px_int i;
	px_int count=0;
	for (i = 0; i < World->pObjects.size; i++)
	{
		if (PX_WorldGetWorldObject(World, i)->pObject && !PX_WorldGetWorldObject(World, i)->DeleteMark)
			count++;
	}
	return count;
}

PX_WorldObject* PX_WorldGetWorldObject(PX_World* World, px_int index)
{
	PX_WorldObject* pwo;
	if (index < 0)
	{
		PX_ASSERT();
	}
	if (index >= World->pObjects.size)
	{
		return PX_NULL;
	}
	pwo = PX_VECTORAT(PX_WorldObject, &World->pObjects, index);
	return pwo;
}

PX_Object* PX_WorldGetObject(PX_World* World, px_int index)
{
	PX_WorldObject* pwo;
	if (index<0)
	{
		PX_ASSERT();
	}
	if (index>= World->pObjects.size)
	{
		return PX_NULL;
	}
	pwo=PX_VECTORAT(PX_WorldObject, &World->pObjects, index);
	if (pwo->DeleteMark)
	{
		return PX_NULL;
	}
	return pwo->pObject;
}

px_int PX_WorldAddObjectEx(PX_World *World,PX_Object *pObject)
{
	PX_WorldObject *pwo,wo;
	px_int i;
	wo.pObject=pObject;
	wo.DeleteMark=PX_FALSE;

	for (i=0;i<World->pObjects.size;i++)
	{
		pwo=PX_VECTORAT(PX_WorldObject,&World->pObjects,i);
		if (!pwo->pObject)
		{
			*pwo=wo;
			World->aliveCount++;
			return i;
		}
	}
	if (PX_VectorPushback(&World->pObjects,&wo))
	{
		World->aliveCount++;
		return World->pObjects.size-1;
	}
	return -1;
}

px_void PX_WorldRemoveObjectEx(PX_World *world,px_int i_index)
{
	PX_WorldObject *pwo=PX_VECTORAT(PX_WorldObject,&world->pObjects,i_index);
	if (!pwo)
	{
		return;
	}
	if (pwo->pObject)
	{
		pwo->DeleteMark=PX_FALSE;
		world->aliveCount--;
		PX_ObjectDelete(pwo->pObject);
		pwo->pObject=PX_NULL;
	}
}

px_void PX_WorldClear(PX_World* world)
{
	px_int i;
	for (i = 0; i < world->pObjects.size; i++)
	{
		PX_WorldRemoveObjectEx(world,i);
	}
	PX_VectorClear(&world->pObjects);
}

px_void PX_WorldBindSoundPlay(PX_World* pWorld, PX_SoundPlay* pSoundPlay)
{
	pWorld->psoundplay = pSoundPlay;
}

px_void PX_WorldBindResourceLibrary(PX_World* pWorld, PX_ResourceLibrary* pResourceLibrary)
{
	pWorld->presourceLibrary = pResourceLibrary;
}

px_void PX_WorldSoundPlay(PX_World* pWorld, const px_char key[], px_float x, px_float y)
{
	if (pWorld->presourceLibrary&&pWorld->psoundplay)
	{
		if (PX_isXYInRegion(x, y, pWorld->offsetx-100, pWorld->offsety-100, pWorld->surface_width+200.f, pWorld->surface_height+200.f))
		{
			PX_SoundPlayPlayData(pWorld->psoundplay, PX_ResourceLibraryGetSound(pWorld->presourceLibrary, key));
		}
	}
	
}

static px_void PX_WorldUpdateNewObjectList(PX_World* pworld)
{
	px_int k = 0,i,j;
	for (i = 0; i < pworld->pNewObjects.size; i++)
	{
		PX_WorldObject* pwo, wo;
		
		wo.pObject = *PX_VECTORAT(PX_Object*, &pworld->pNewObjects, i);
		wo.DeleteMark = PX_FALSE;

		for (j = k; j < pworld->pObjects.size; j++)
		{
			pwo = PX_VECTORAT(PX_WorldObject, &pworld->pObjects, j);
			if (!pwo->pObject)
			{
				wo.pObject->world_index = j;
				*pwo = wo;
				pworld->aliveCount++;
				k = j + 1;
				goto NEW_OBJECT_CONTINUE;
			}
		}

		if (PX_VectorPushback(&pworld->pObjects, &wo))
		{
			pworld->aliveCount++;
			wo.pObject->world_index = pworld->pObjects.size - 1;
			k = wo.pObject->world_index + 1;
		}
	NEW_OBJECT_CONTINUE:
		continue;
	}
	PX_VectorClear(&pworld->pNewObjects);
}

px_void PX_WorldUpdate( PX_World *pworld,px_uint elapsed )
{
	px_int updateCount;
	px_int i,b,j;
	px_float w_left=0,w_top=0,w_right=0,w_height=0;
	PX_Object_Event e;
	PX_WorldObject *pwo;
	
	px_int impact_count[sizeof(pwo->pObject->impact_object_type)*8]={0};
	px_memorypool *calcmp=&pworld->mp_WorldCalc;

	if (pworld==PX_NULL)
	{
		return;
	}

	MP_Reset(calcmp);
	//search map
	

	//Add NewObjects
	PX_WorldUpdateNewObjectList(pworld);

	//pObject Counts
	updateCount=pworld->pObjects.size;

	PX_MapInitialize(calcmp, &pworld->idSearchmap);
	for (i = 0; i < updateCount; i++)
	{
		pwo = PX_VECTORAT(PX_WorldObject, &pworld->pObjects, i);
		if (!pwo->pObject)
		{
			continue;
		}
		if (!pwo->pObject->id[0])
		{
			continue;
		}
		PX_MapPut(&pworld->idSearchmap, (const px_byte *)pwo->pObject->id,PX_strlen(pwo->pObject->id), pwo->pObject);

	}
	//////////////////////////////////////////////////////////////////////////
	//impact test
	//////////////////////////////////////////////////////////////////////////
	if (pworld->surface_height&&pworld->surface_width&&pworld->mp_WorldCalc.Size)
	{
		for (i = 0; i < updateCount; i++)
		{
			pwo = PX_VECTORAT(PX_WorldObject, &pworld->pObjects, i);
			if (!pwo->pObject)
			{
				continue;
			}
			if (pwo->pObject->impact_object_type)
			{
				j = 0;
				while (!(pwo->pObject->impact_object_type & (1 << j)))
					j++;

				impact_count[j]++;
			}
		}


		for (i = 0; i < sizeof(pwo->pObject->impact_object_type) * 8; i++)
		{
			if (impact_count[i])
			{
				PX_QuadtreeCreate(calcmp, &pworld->Impact_Test_array[i], 0, 0, (px_float)pworld->world_width, (px_float)pworld->world_height, impact_count[i], 2);
			}
			else
			{
				PX_QuadtreeCreate(calcmp, &pworld->Impact_Test_array[i], 0, 0, (px_float)pworld->world_width, (px_float)pworld->world_height, impact_count[i], 0);
			}
		}

		for (i = 0; i < updateCount; i++)
		{
			pwo = PX_VECTORAT(PX_WorldObject, &pworld->pObjects, i);
			if (!pwo->pObject)
			{
				continue;
			}
			if (pwo->pObject->impact_object_type)
			{
				for (b = 0; b < sizeof(pwo->pObject->impact_object_type) * 8; b++)
				{
					if ((pwo->pObject->impact_object_type & (1 << b)))
					{
						PX_Quadtree_UserData userData;
						userData.ptr = pwo;
						if ((px_float)pwo->pObject->diameter)
						{
							PX_QuadtreeAddNode(&pworld->Impact_Test_array[b], (px_float)pwo->pObject->x, (px_float)pwo->pObject->y, (px_float)pwo->pObject->diameter, (px_float)pwo->pObject->diameter, userData);
						}
						else
						{
							PX_QuadtreeAddNode(&pworld->Impact_Test_array[b], (px_float)pwo->pObject->x, (px_float)pwo->pObject->y, (px_float)pwo->pObject->Width, (px_float)pwo->pObject->Height, userData);
						}

					}
				}
			}
		}

		for (i = 0; i < updateCount; i++)
		{
			pwo = PX_VECTORAT(PX_WorldObject, &pworld->pObjects, i);
			if (!pwo->pObject)
			{
				continue;
			}

			if (pwo->pObject->impact_target_type)
			{
				for (b = 0; b < sizeof(pwo->pObject->impact_target_type) * 8; b++)
				{
					if ((pwo->pObject->impact_target_type & (1 << b)))
					{
						px_int im_i;
						PX_Quadtree_UserData userData;
						userData.ptr = pwo;
						PX_QuadtreeResetTest(&pworld->Impact_Test_array[b]);
						if (pwo->pObject->diameter)
						{
							PX_QuadtreeTestNode(&pworld->Impact_Test_array[b], (px_float)pwo->pObject->x, (px_float)pwo->pObject->y, (px_float)pwo->pObject->diameter, (px_float)pwo->pObject->diameter, userData);
						}
						else
						{
							PX_QuadtreeTestNode(&pworld->Impact_Test_array[b], (px_float)pwo->pObject->x, (px_float)pwo->pObject->y, (px_float)pwo->pObject->Width, (px_float)pwo->pObject->Height, userData);
						}

						for (im_i = 0; im_i < pworld->Impact_Test_array[b].Impacts.size; im_i++)
						{
							PX_Quadtree_UserData* puData = PX_VECTORAT(PX_Quadtree_UserData, &pworld->Impact_Test_array[b].Impacts, im_i);
							PX_Object* pObj1 = pwo->pObject;
							PX_WorldObject* pimpactWo = (PX_WorldObject*)puData->ptr;
							PX_Object* pObj2 = ((PX_WorldObject*)(puData->ptr))->pObject;

							if (pObj1->diameter && pObj2->diameter)
							{
								if (!PX_isCircleCrossCircle(PX_POINT(pObj1->x, pObj1->y, 0), pObj1->diameter / 2, PX_POINT(pObj2->x, pObj2->y, 0), pObj2->diameter / 2))
									continue;
							}
							else if (pObj1->diameter == 0 && pObj2->diameter)
							{
								if (!PX_isRectCrossCircle(PX_RECT(pObj1->x - pObj1->Width / 2, pObj1->y - pObj1->Height / 2, pObj1->Width, pObj1->Height), PX_POINT(pObj2->x, pObj2->y, 0), pObj2->diameter / 2))
								{
									continue;
								}
							}
							else if (pObj1->diameter && pObj2->diameter == 0)
							{
								if (!PX_isRectCrossCircle(PX_RECT(pObj2->x - pObj2->Width / 2, pObj2->y - pObj2->Height / 2, pObj2->Width, pObj2->Height), PX_POINT(pObj1->x, pObj1->y, 0), pObj1->diameter / 2))
								{
									continue;
								}
							}
							if (pimpactWo->DeleteMark != PX_TRUE)
							{
								e.Event = PX_OBJECT_EVENT_IMPACT;
								PX_Object_Event_SetImpactTargetObject(&e, pObj2);
								PX_ObjectPostEvent(pObj1, e);
							}
						}

					}
				}
			}
		}
	}

	//////////////////////////////////////////////////////////////////////////
	//Update Objects
	

	for (i=0;i<updateCount;i++)
	{
		pwo=PX_VECTORAT(PX_WorldObject,&pworld->pObjects,i);
		if (!pwo->pObject)
		{
			continue;
		}
		PX_ObjectUpdate(pwo->pObject,elapsed);
	}

	///////////////////////////////////////////////////////////////////////////
	if (pworld->vm.binsize)
	{
		PX_VMRun(&pworld->vm, -1, elapsed);
	}

	//////////////////////////////////////////////////////////////////////////
	//delete death objects
	for (i=0;i<updateCount;i++)
	{
		pwo=PX_VECTORAT(PX_WorldObject,&pworld->pObjects,i);
		if (pwo->pObject&&pwo->DeleteMark)
		{
			PX_WorldRemoveObjectEx(pworld,i);
		}
	}

}

px_void PX_WorldUpdateOffset(PX_World *pw)
{
	px_int surface_width,surface_height;
	px_int LeftTopX,LeftTopY;

	surface_width=pw->surface_width;
	surface_height=pw->surface_height;
	LeftTopX=(px_int)(pw->camera_offset.x-surface_width/2),LeftTopY=(px_int)(pw->camera_offset.y-surface_height/2);

	if (LeftTopX+surface_width>pw->world_width)
	{
		LeftTopX=pw->world_width-surface_width;
	}

	if (LeftTopY+surface_height>pw->world_height)
	{
		LeftTopY=pw->world_height-surface_height;
	}

	if (LeftTopX<0)
	{
		LeftTopX=0;
	}
	if (LeftTopY<0)
	{
		LeftTopY=0;
	}

	pw->offsetx=(px_float)LeftTopX;
	pw->offsety=(px_float)LeftTopY;
}

static px_void PX_WorldClildRender(PX_World *pworld,px_surface *pSurface, PX_Object *pObject,px_uint elapsed,px_float oftX,px_float oftY)
{
	if (pObject==PX_NULL)
	{
		return;
	}
	if (pObject->Visible==PX_FALSE)
	{
		return;
	}
	if (pObject->Func_ObjectRender!=0)
	{
		px_float x=pObject->x;
		px_float y=pObject->y;
		pObject->x-=oftX;
		pObject->y-=oftY;
		pObject->Func_ObjectRender(pSurface,pObject,elapsed);

		if (pworld->showImpactRegion)
		{
			if (pObject->diameter)
			{
				PX_GeoDrawSolidCircle(pSurface, (px_int)pObject->x, (px_int)pObject->y, (px_int)pObject->diameter / 2, PX_COLOR(32, 255, 0, 0));
			}
			else
			{
				PX_GeoDrawRect(pSurface, (px_int)(pObject->x- pObject->Width/2), (px_int)(pObject->y- pObject->Height/2), (px_int)(pObject->x + pObject->Width/2), (px_int)(pObject->y + pObject->Height/2), PX_COLOR(32, 255, 0, 0));
			}
		}


		pObject->x=x;
		pObject->y=y;
	}
	if (pObject->pNextBrother!=PX_NULL)
	{
		PX_WorldClildRender(pworld,pSurface,pObject->pNextBrother,elapsed,oftX,oftY);
	}
	if (pObject->pChilds!=PX_NULL)
	{
		PX_WorldClildRender(pworld,pSurface,pObject->pChilds,elapsed,oftX,oftY);
	}
}



px_void PX_WorldRender(px_surface *psurface,PX_World *pw,px_dword elapsed)
{
	px_int i,j;
	px_int surface_width,surface_height;
	PX_QuickSortAtom *ArrayIndex;
	px_memorypool *calcmp;
	px_int sx,sy;
	PX_WorldObject *pwo;

	if (pw==PX_NULL)
	{
		return;
	}

	calcmp=&pw->mp_WorldCalc;
	MP_Reset(calcmp);
	
	
	PX_WorldUpdateOffset(pw);

	surface_width=pw->surface_width;
	surface_height=pw->surface_height;

	if (pw->pbackgroundtexture)
	{
		px_float xfactor;
		px_float yfactor;
		px_int x;
		px_int y;
		if (pw->world_width != pw->surface_width)
		{
			xfactor = pw->offsetx * 1.0f / (pw->world_width - pw->surface_width);
		}
		else
		{
			xfactor = 0;
		}

		if (pw->world_height != pw->surface_height)
		{
			yfactor = pw->offsety * 1.0f / (pw->world_height - pw->surface_height);
		}
		else
		{
			yfactor = 0;
		}

		x = (px_int)((pw->pbackgroundtexture->width - pw->surface_width) * xfactor);
		y = (px_int)((pw->pbackgroundtexture->height - pw->surface_height) * yfactor);
		PX_TextureRender(psurface, pw->pbackgroundtexture, -x, -y, PX_ALIGN_LEFTTOP, 0);
	}

	if (pw->auxiliaryline&& pw->auxiliaryline_color._argb.a)
	{
		for (sy=pw->auxiliaryYSpacer-((px_int)pw->offsety%pw->auxiliaryYSpacer);sy<surface_height;sy+=pw->auxiliaryYSpacer)
		{
			PX_GeoDrawLine(psurface,0,sy,surface_width-1,sy,1,pw->auxiliaryline_color);
		}

		for (sx=pw->auxiliaryXSpacer-((px_int)pw->offsetx%pw->auxiliaryXSpacer);sx<surface_width;sx+=pw->auxiliaryXSpacer)
		{
			PX_GeoDrawLine(psurface,sx,0,sx,surface_height-1,1,pw->auxiliaryline_color);
		}

	}

	ArrayIndex=(PX_QuickSortAtom *)MP_Malloc(calcmp,pw->aliveCount*sizeof(PX_QuickSortAtom));
	if (pw->aliveCount&&!ArrayIndex)
	{
		PX_ERROR("out of memory");
	}
	if (ArrayIndex)
	{
		j = 0;

		for (i = 0; i < pw->pObjects.size; i++)
		{
			pwo = PX_VECTORAT(PX_WorldObject, &pw->pObjects, i);
			if (!pwo->pObject)
			{
				continue;
			}
			ArrayIndex[j].weight = pwo->pObject->z;
			ArrayIndex[j].pData = pwo->pObject;
			j++;
		}

		PX_Quicksort_MaxToMin(ArrayIndex, 0, pw->aliveCount - 1);

		for (i = 0; i < pw->aliveCount; i++)
		{
			PX_WorldClildRender(pw, psurface, (PX_Object*)ArrayIndex[i].pData, elapsed, pw->offsetx, pw->offsety);
		}
	}
	MP_Reset(calcmp);

	if (pw->pfrontwardtexture)
	{
		px_float xfactor;
		px_float yfactor;
		px_int x;
		px_int y;
		if (pw->world_width != pw->surface_width)
		{
			xfactor = pw->offsetx * 1.0f / (pw->world_width - pw->surface_width);
		}
		else
		{
			xfactor = 0;
		}

		if (pw->world_height != pw->surface_height)
		{
			yfactor = pw->offsety * 1.0f / (pw->world_height - pw->surface_height);
		}
		else
		{
			yfactor = 0;
		}

		x = (px_int)((pw->pfrontwardtexture->width - pw->surface_width) * xfactor);
		y = (px_int)((pw->pfrontwardtexture->height - pw->surface_height) * yfactor);
		PX_TextureRender(psurface, pw->pfrontwardtexture, -x, -y, PX_ALIGN_LEFTTOP, 0);
	}

}

px_void PX_WorldSetImpact(PX_Object *pObj,px_dword type,px_dword Intersect)
{
	pObj->impact_object_type=type;
	pObj->impact_target_type=Intersect;
}



px_bool PX_WorldAddObject(PX_World *World,PX_Object *pObject)
{
	return PX_VectorPushback(&World->pNewObjects,&pObject);
}




px_void PX_WorldSetAuxiliaryLineColor(PX_World *pw,px_color color)
{
	pw->auxiliaryline_color=color;
}



px_point PX_WorldObjectXYtoScreenXY(PX_World *pw,px_float x,px_float y)
{
	return PX_POINT(x-pw->offsetx,y-pw->offsety,0);
}

px_void PX_WorldPostEvent(PX_World* pw, PX_Object_Event e)
{
	PX_WorldObject* pwo;
	px_int i;
	for (i = 0; i < pw->pObjects.size; i++)
	{
		pwo = PX_VECTORAT(PX_WorldObject, &pw->pObjects, i);
		if (pwo->pObject)
		{
			PX_ObjectPostEvent(pwo->pObject, e);
		}
	}
}

px_bool PX_WorldRegisterClass(PX_World* pw, PX_WorldClass cls)
{
	PX_WorldClass *pNewClass=(PX_WorldClass *)MP_Malloc(pw->mp,sizeof(PX_WorldClass));
	if (pNewClass==PX_NULL)
	{
		return PX_FALSE;
	}
	*pNewClass=cls;
	if (PX_MapPut(&pw->classes,(const px_byte *)cls.name,PX_strlen(cls.name),pNewClass)!=PX_HASHMAP_RETURN_OK)
	{
		MP_Free(pw->mp,pNewClass);
		return PX_FALSE;
	}
	return PX_TRUE;
}

PX_Object* PX_WorldCreateClassObject(PX_World* pWorld, px_char name[], px_float x, px_float y, px_float z, px_float width, px_float height, px_float length, px_abi* abi)
{
	PX_WorldClass* pClass = (PX_WorldClass*)PX_MapGet(&pWorld->classes, (const px_byte*)name, PX_strlen(name));
	if (pClass)
	{
		PX_Object* pObject = pClass->func(pWorld, x, y, z, width, height, length, abi);
		if (pObject)
		{
			PX_WorldAddObject(pWorld, pObject);
			return pObject;
		}
	}
	return PX_NULL;

}

px_void PX_WorldFree(PX_World *pw)
{
	PX_WorldObject *pwo;
	px_int i;
    PX_RBNode *pNode;
	for (i=0;i<pw->pObjects.size;i++)
	{
		pwo=PX_VECTORAT(PX_WorldObject,&pw->pObjects,i);
		if (pwo->pObject)
		{
			PX_ObjectDelete(pwo->pObject);
		}
	}
	
	for (i = 0; i < pw->pNewObjects.size; i++)
	{
		PX_Object *pObject = *PX_VECTORAT(PX_Object *, &pw->pObjects, i);
		if (pObject)
		{
			PX_ObjectDelete(pObject);
		}
	}

	PX_VectorFree(&pw->pObjects);
	PX_VectorFree(&pw->pNewObjects);
	
	pNode = PX_MapFirst(&pw->classes);
	while (pNode)
	{
		MP_Free(pw->mp, pNode->_ptr);
		pNode->_ptr = PX_NULL;
		pNode = PX_MapNext(pNode);
	}
	PX_MapFree(&pw->classes);

	MP_Free(pw->mp,pw->mp_WorldCalc.StartAddr);
	if (pw->compiler.mp)
	{
		PX_CompilerFree(&pw->compiler);
	}
	if (pw->vmbin.mp)
	{
		PX_MemoryFree(&pw->vmbin);
	}
	if (pw->vm.mp)
	{
		PX_VMFree(&pw->vm);
	}
	if (pw->debugmap.mp)
	{
		PX_VMDebuggerMapFree(&pw->debugmap);
	}

}

px_void PX_WorldSetCamera(PX_World *World,px_point camera_center_point)
{
	World->camera_offset=camera_center_point;
	PX_WorldUpdateOffset(World);
}

px_void PX_WorldRemoveObject(PX_World *world,PX_Object *pObject)
{
	PX_WorldObject *pwo;
	if (pObject->world_index!=-1&&pObject->world_index<world->pObjects.size)
	{
		pwo=PX_VECTORAT(PX_WorldObject,&world->pObjects,pObject->world_index);
		if (pwo->pObject==pObject)
		{
			pwo->DeleteMark=PX_TRUE;
			return;
		}
		PX_ASSERT();
	}
}

px_void PX_WorldRemoveObjectByIndex(PX_World *world,px_int i_index)
{
	PX_WorldObject *pwo=PX_VECTORAT(PX_WorldObject,&world->pObjects,i_index);

#ifdef PX_DEBUG_MODE
	if (i_index>world->pObjects.size)
	{
		PX_ASSERT();
	}
#endif

	if (pwo->pObject)
	{
		pwo->DeleteMark=PX_TRUE;
	}
}

_LIMIT px_int PX_WorldSearchRegion(PX_World *world,px_float x,px_float y,px_float raduis,PX_Object *pObject[],px_int MaxSearchCount,px_dword impact_test_type)
{
	px_int b;
	px_int m=0;
	for (b=0;b<sizeof(impact_test_type)*8;b++)
	{
		if ((impact_test_type&(1<<b)))
		{
			px_int im_i;
			PX_Quadtree_UserData userData;
			userData.ptr=PX_NULL;
			PX_QuadtreeResetTest(&world->Impact_Test_array[b]);
			PX_QuadtreeTestNode(&world->Impact_Test_array[b],x,y,raduis*2,raduis*2,userData);

			for (im_i=0;im_i<world->Impact_Test_array[b].Impacts.size;im_i++)
			{
				PX_Quadtree_UserData *puData=PX_VECTORAT(PX_Quadtree_UserData,&world->Impact_Test_array[b].Impacts,im_i);
				PX_WorldObject *pimpactWo=(PX_WorldObject *)puData->ptr;
				PX_Object *pObj2=((PX_WorldObject *)(puData->ptr))->pObject;

				if (pObj2->diameter)
				{
					if(!PX_isCircleCrossCircle(PX_POINT(x,y,0),raduis,PX_POINT(pObj2->x,pObj2->y,0),pObj2->diameter/2))
						continue;
				}
				else
				{
					if (!PX_isRectCrossCircle(PX_RECT(pObj2->x-pObj2->Width/2,pObj2->y-pObj2->Height/2,pObj2->Width,pObj2->Height),PX_POINT(x,y,0),raduis))
					{
						continue;
					}
				}

				if (pimpactWo->DeleteMark!=PX_TRUE)
				{
					if (m<MaxSearchCount)
					{
						pObject[m]=pObj2;
						m++;
					}
					
				}
			}

		}
	}
	return m;
}

_LIMIT px_int PX_WorldSearch(PX_World* pw, PX_Object* pObject[], px_int MaxSearchCount, px_dword impact_object_type)
{
	px_int i;
	px_int count = 0;
	for (i = 0; i < pw->pObjects.size; i++)
	{
		PX_WorldObject *pwo = PX_VECTORAT(PX_WorldObject, &pw->pObjects, i);
		if (pwo&&pwo->pObject&&!pwo->DeleteMark&&pwo->pObject->impact_object_type== impact_object_type)
		{
			if (MaxSearchCount)
			{
				pObject[count] = pwo->pObject;
				MaxSearchCount--;
				count++;
			}
			else
			{
				break;
			}
		}
	}
	return count;
}

_LIMIT PX_Object* PX_WorldSearchObject(PX_World* pWorld, const px_char id[])
{
	return (PX_Object*)PX_MapGet(&pWorld->idSearchmap, (const px_byte *)id,PX_strlen(id));
}

px_void PX_WorldSetSize(PX_World* pWorld, px_int world_width, px_int world_height)
{
	pWorld->world_width = world_width;
	pWorld->world_height = world_height;
}
