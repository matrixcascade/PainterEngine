#ifndef PX_OBJECT_MACHIN_H
#define PX_OBJECT_MACHIN_H
#include "PX_Object.h"
#include "PX_Machine.h"


typedef struct 
{
	px_memorypool* mp;
	PX_Machine machine;
}PX_Object_Machine;

PX_Object* PX_Object_MachineCreate(px_memorypool* mp, PX_Object* Parent);
px_bool PX_Object_MachineExecute(PX_Object* pObject, const px_byte* payload, px_int size,px_abi *pout);
#endif



