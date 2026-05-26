#include "PX_Object_Machine.h"

PX_OBJECT_UPDATE_FUNCTION(PX_Object_MachineUpdate)
{
	PX_Object_Machine* pMachineObject = PX_ObjectGetDesc(PX_Object_Machine, pObject);
	PX_Machine_Update(&pMachineObject->machine, 0xffff, elapsed);
}


PX_OBJECT_FREE_FUNCTION(PX_Object_MachineFree)
{
	PX_Object_Machine* pMachineObject = PX_ObjectGetDesc(PX_Object_Machine, pObject);
	PX_Machine_Free(&pMachineObject->machine);
}

PX_Object* PX_Object_MachineCreate(px_memorypool* mp, PX_Object* Parent)
{
	PX_Object* pObject = PX_ObjectCreateEx(mp, Parent, 0, 0, 0, 0, 0, 0,PX_OBJECT_TYPE_MACHINE, PX_Object_MachineUpdate, 0, PX_Object_MachineFree, PX_NULL, sizeof(PX_Object_Machine));
	PX_Object_Machine* pMachineObject;
	if (pObject == PX_NULL)
	{
		return PX_NULL;
	}
	pMachineObject = PX_ObjectGetDesc0(PX_Object_Machine,pObject);
	if (!PX_Machine_Initialize(mp, &pMachineObject->machine))
	{
		PX_ObjectDelete(pObject);
		return PX_NULL;
	}
	return pObject;
}

px_bool PX_Object_MachineExecute(PX_Object* pObject, const px_byte* payload, px_int size, px_abi* pout)
{
	PX_Object_Machine* pMachineObject = PX_ObjectGetDesc0(PX_Object_Machine, pObject);
	return PX_Machine_Execute(&pMachineObject->machine, payload, size, pout);
}
