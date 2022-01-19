#include "PX_Object_Wood.h"


px_void PX_Object_WoodUpdate(PX_Object* pObject, px_dword elpased)
{
	PX_Object_Wood* pdesc = PX_ObjectGetDesc(PX_Object_Wood, pObject);

	if (pdesc->state!=PX_Object_Wood_State_Run)
	{
		return;
	}

	pObject->x -= elpased / 2.0f;
	if (pObject->x<-pdesc->ptextures->width)
	{
		PX_WorldRemoveObject(pdesc->pWorld, pObject);
	}
}

px_void PX_Object_WoodRender(px_surface *psurface,PX_Object* pObject, px_dword elpased)
{
	PX_Object_Wood* pdesc = PX_ObjectGetDesc(PX_Object_Wood, pObject);
	PX_TextureRender(psurface, pdesc->ptextures, (px_int)pObject->x, (px_int)pObject->y, PX_ALIGN_CENTER, PX_NULL);
}

px_void PX_Object_WoodOnReady(PX_Object* pObject, PX_Object_Event e, px_void* ptr)
{
	PX_Object_Wood* pDesc = PX_ObjectGetDesc(PX_Object_Wood, pObject);
	pDesc->state = PX_Object_Wood_State_Ready;
	return;
}

px_void PX_Object_WoodOnRun(PX_Object* pObject, PX_Object_Event e, px_void* ptr)
{
	PX_Object_Wood* pDesc = PX_ObjectGetDesc(PX_Object_Wood, pObject);
	pDesc->state = PX_Object_Wood_State_Run;
	return;
}

px_void PX_Object_WoodOnStop(PX_Object* pObject, PX_Object_Event e, px_void* ptr)
{
	PX_Object_Wood* pDesc = PX_ObjectGetDesc(PX_Object_Wood, pObject);
	pDesc->state = PX_Object_Wood_State_Stop;
	return;
}


PX_Object* PX_Object_WoodCreate(PX_World* pWorld, PX_Runtime* pruntime)
{
	PX_Object_Wood desc;
	PX_Object* pObject;
	PX_memset(&desc, 0, sizeof(desc));
	desc.pWorld = pWorld;
	desc.pruntime = pruntime;
	switch (PX_rand()%2)
	{
	case 0:
		desc.ptextures = PX_ResourceLibraryGetTexture(&pruntime->ResourceLibrary, "wood1");
		break;
	case 1:
		desc.ptextures = PX_ResourceLibraryGetTexture(&pruntime->ResourceLibrary, "wood2");
		break;
	default:
		break;
	}
	
	desc.state = PX_Object_Wood_State_Run;
	if (!desc.ptextures)
	{
		return PX_FALSE;
	}
	pObject = PX_ObjectCreateEx(&pruntime->mp_game, PX_NULL, \
		pruntime->surface_width + desc.ptextures->width + 16.f, \
		pruntime->surface_height - PX_OBJECT_WOOD_BASE*1.0f, \
		PX_OBJECT_WOOD_Z, \
		desc.ptextures->width*1.0f-24, \
		desc.ptextures->height*1.0f-16, \
		0, \
		0, \
		PX_Object_WoodUpdate, \
		PX_Object_WoodRender, \
		PX_NULL, \
		&desc, \
		sizeof(desc)\
	);
	pObject->impact_object_type = 2;

	PX_ObjectRegisterEvent(pObject, PX_GAMEEVENT_READY, PX_Object_WoodOnReady, PX_NULL);
	PX_ObjectRegisterEvent(pObject, PX_GAMEEVENT_RUN, PX_Object_WoodOnRun, PX_NULL);
	PX_ObjectRegisterEvent(pObject, PX_GAMEEVENT_STOP, PX_Object_WoodOnStop, PX_NULL);

	return pObject;
}

