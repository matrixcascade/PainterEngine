#include "PX_Object_Cloud.h"


px_void PX_Object_CloudUpdate(PX_Object* pObject, px_dword elpased)
{
	PX_Object_Cloud* pdesc = PX_ObjectGetDesc(PX_Object_Cloud, pObject);

	if (pdesc->state!=PX_Object_Cloud_State_Run)
	{
		return;
	}

	pObject->x -= elpased / 8.0f;
	if (pObject->x<-pdesc->ptextures->width)
	{
		PX_WorldRemoveObject(pdesc->pWorld, pObject);
	}
}

px_void PX_Object_CloudRender(px_surface *psurface,PX_Object* pObject, px_dword elpased)
{
	PX_Object_Cloud* pdesc = PX_ObjectGetDesc(PX_Object_Cloud, pObject);
	PX_TEXTURERENDER_BLEND blend;
	blend.alpha = 0.25f;
	blend.hdr_R = 1;
	blend.hdr_G = 1;
	blend.hdr_B = 1;
	PX_TextureRender(psurface, pdesc->ptextures, (px_int)pObject->x, (px_int)pObject->y, PX_ALIGN_CENTER, &blend);
}

px_void PX_Object_CloudOnReady(PX_Object* pObject, PX_Object_Event e, px_void* ptr)
{
	PX_Object_Cloud* pDesc = PX_ObjectGetDesc(PX_Object_Cloud, pObject);
	pDesc->state = PX_Object_Cloud_State_Ready;
	return;
}

px_void PX_Object_CloudOnRun(PX_Object* pObject, PX_Object_Event e, px_void* ptr)
{
	PX_Object_Cloud* pDesc = PX_ObjectGetDesc(PX_Object_Cloud, pObject);
	pDesc->state = PX_Object_Cloud_State_Run;
	return;
}

px_void PX_Object_CloudOnStop(PX_Object* pObject, PX_Object_Event e, px_void* ptr)
{
	PX_Object_Cloud* pDesc = PX_ObjectGetDesc(PX_Object_Cloud, pObject);
	pDesc->state = PX_Object_Cloud_State_Stop;
	return;
}


PX_Object* PX_Object_CloudCreate(PX_World* pWorld, PX_Runtime* pruntime)
{
	PX_Object_Cloud desc;
	PX_Object* pObject;
	PX_memset(&desc, 0, sizeof(desc));
	desc.pWorld = pWorld;
	desc.pruntime = pruntime;
	switch (PX_rand()%4)
	{
	default:
	case 0:
		desc.ptextures = PX_ResourceLibraryGetTexture(&pruntime->ResourceLibrary, "cloud1");
		break;
	case 1:
		desc.ptextures = PX_ResourceLibraryGetTexture(&pruntime->ResourceLibrary, "cloud2");
		break;
	case 2:
		desc.ptextures = PX_ResourceLibraryGetTexture(&pruntime->ResourceLibrary, "cloud3");
		break;
	case 3:
		desc.ptextures = PX_ResourceLibraryGetTexture(&pruntime->ResourceLibrary, "cloud4");
		break;
	}
	
	desc.state = PX_Object_Cloud_State_Run;
	if (!desc.ptextures)
	{
		return PX_FALSE;
	}
	pObject = PX_ObjectCreateEx(&pruntime->mp_game, PX_NULL, \
		pruntime->surface_width + desc.ptextures->width + 16.f, \
		(px_float)PX_randRange(0,pruntime->surface_height - PX_OBJECT_CLOUD_BASE*1.0f), \
		PX_OBJECT_CLOUD_Z, \
		desc.ptextures->width*1.0f-24, \
		desc.ptextures->height*1.0f-16, \
		0, \
		0, \
		PX_Object_CloudUpdate, \
		PX_Object_CloudRender, \
		PX_NULL, \
		&desc, \
		sizeof(desc)\
	);

	PX_ObjectRegisterEvent(pObject, PX_GAMEEVENT_READY, PX_Object_CloudOnReady, PX_NULL);
	PX_ObjectRegisterEvent(pObject, PX_GAMEEVENT_RUN, PX_Object_CloudOnRun, PX_NULL);
	PX_ObjectRegisterEvent(pObject, PX_GAMEEVENT_STOP, PX_Object_CloudOnStop, PX_NULL);

	return pObject;
}

