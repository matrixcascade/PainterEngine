#include "PX_Object_Bird.h"


px_void PX_Object_BirdUpdate(PX_Object* pObject, px_dword elpased)
{
	PX_Object_Bird* pdesc = PX_ObjectGetDesc(PX_Object_Bird, pObject);

	if (pdesc->state!=PX_Object_Bird_State_Run)
	{
		return;
	}

	pObject->x -= elpased / 1.2f;

	if (pObject->x<-128)
	{
		PX_WorldRemoveObject(pdesc->pWorld, pObject);
	}
}

px_void PX_Object_BirdRender(px_surface *psurface,PX_Object* pObject, px_dword elpased)
{
	
	PX_Object_Bird* pdesc = PX_ObjectGetDesc(PX_Object_Bird, pObject);
	PX_AnimationUpdate(&pdesc->animation, elpased);
	PX_AnimationRender(psurface, &pdesc->animation, (px_int)pObject->x, (px_int)pObject->y, PX_ALIGN_CENTER, PX_NULL);

}

px_void PX_Object_BirdOnReady(PX_Object* pObject, PX_Object_Event e, px_void* ptr)
{
	PX_Object_Bird* pDesc = PX_ObjectGetDesc(PX_Object_Bird, pObject);
	pDesc->state = PX_Object_Bird_State_Ready;
	return;
}

px_void PX_Object_BirdOnRun(PX_Object* pObject, PX_Object_Event e, px_void* ptr)
{
	PX_Object_Bird* pDesc = PX_ObjectGetDesc(PX_Object_Bird, pObject);
	pDesc->state = PX_Object_Bird_State_Run;
	return;
}

px_void PX_Object_BirdOnStop(PX_Object* pObject, PX_Object_Event e, px_void* ptr)
{
	PX_Object_Bird* pDesc = PX_ObjectGetDesc(PX_Object_Bird, pObject);
	pDesc->state = PX_Object_Bird_State_Stop;
	return;
}


PX_Object* PX_Object_BirdCreate(PX_World* pWorld, PX_Runtime* pruntime)
{
	PX_Object_Bird desc;
	PX_Object* pObject;
	PX_memset(&desc, 0, sizeof(desc));
	desc.pWorld = pWorld;
	desc.pruntime = pruntime;
	PX_AnimationCreate(&desc.animation, PX_ResourceLibraryGetAnimationLibrary(&pruntime->ResourceLibrary, "bird"));

	desc.state = PX_Object_Bird_State_Run;
	

	pObject = PX_ObjectCreateEx(&pruntime->mp_game, PX_NULL, \
		pruntime->surface_width + PX_AnimationGetSize(&desc.animation).width + 16.f, \
		(px_float)PX_randRange(80,pruntime->surface_height - PX_OBJECT_BIRD_BASE *1.0), \
		PX_OBJECT_BIRD_Z, \
		PX_AnimationGetSize(&desc.animation) .width*1.0f-8, \
		PX_AnimationGetSize(&desc.animation).height *1.0f-16, \
		0, \
		0, \
		PX_Object_BirdUpdate, \
		PX_Object_BirdRender, \
		PX_NULL, \
		&desc, \
		sizeof(desc)\
	);
	pObject->impact_object_type = 2;

	PX_ObjectRegisterEvent(pObject, PX_GAMEEVENT_READY, PX_Object_BirdOnReady, PX_NULL);
	PX_ObjectRegisterEvent(pObject, PX_GAMEEVENT_RUN, PX_Object_BirdOnRun, PX_NULL);
	PX_ObjectRegisterEvent(pObject, PX_GAMEEVENT_STOP, PX_Object_BirdOnStop, PX_NULL);

	return pObject;
}

