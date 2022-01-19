#include "PX_Object_Ground.h"

px_void PX_Object_GroundRender(px_surface* psurface, PX_Object* pObject, px_dword elpased)
{
	px_int requireTextureIndex = 0;
	px_int renderOffset;
	PX_Object_Ground* pDesc = PX_ObjectGetDesc(PX_Object_Ground, pObject);
	PX_TEXTURERENDER_BLEND blend;
	blend.alpha = 0.2f;
	blend.hdr_R = 1;
	blend.hdr_G = 1;
	blend.hdr_B = 1;
	renderOffset = (px_int)pDesc->xoffset;

	while (renderOffset<pDesc->pruntime->surface_width)
	{
		px_texture* ptexture;
		if (requireTextureIndex>=pDesc->texturelist.size)
		{
			ptexture = &pDesc->textures[PX_rand() % PX_OBJECT_GROUND_TEXTURE_COUNT];
			PX_ListPush(&pDesc->texturelist, &ptexture, sizeof(px_texture*));
		}
		else
		{
			ptexture = *PX_LISTAT(px_texture*, &pDesc->texturelist, requireTextureIndex);
			
		}

		if (renderOffset+ptexture->width<0)
		{
			renderOffset += ptexture->width;
			pDesc->xoffset += ptexture->width;
			PX_ListErase(&pDesc->texturelist, 0);
			continue;
		}

		PX_TextureRender(psurface, ptexture, renderOffset, (px_int)(pDesc->pruntime->surface_height - PX_OBJECT_GROUND_BASE), PX_ALIGN_LEFTTOP, &blend);

		requireTextureIndex++;
		renderOffset += ptexture->width;
		
	}

	switch (pDesc->state)
	{
	case PX_Object_Ground_State_Ready:
	{
		pDesc->xoffset -= elpased / 4.0f;
	}
	break;
	case PX_Object_Ground_State_Run:
	{
		pDesc->xoffset -= elpased / 2.0f;
	}
	break;
	case PX_Object_Ground_State_Stop:
	{
		//nop
	}
	break;
	default:
		break;
	}
	
}

px_void PX_Object_GroundFree(PX_Object* pObject)
{
	px_int i;
	PX_Object_Ground* pDesc = PX_ObjectGetDesc(PX_Object_Ground, pObject);
	for (i = 0; i < PX_OBJECT_GROUND_TEXTURE_COUNT; i++)
	{
		PX_TextureFree(&pDesc->textures[i]);
	}
	PX_ListFree(&pDesc->texturelist);
}

px_void PX_Object_GroundOnReady(PX_Object* pObject, PX_Object_Event e, px_void* ptr)
{
	PX_Object_Ground* pDesc = PX_ObjectGetDesc(PX_Object_Ground, pObject);
	pDesc->state = PX_Object_Ground_State_Ready;
	return;
}

px_void PX_Object_GroundOnRun(PX_Object* pObject, PX_Object_Event e, px_void* ptr)
{
	PX_Object_Ground* pDesc = PX_ObjectGetDesc(PX_Object_Ground, pObject);
	pDesc->state = PX_Object_Ground_State_Run;
	return;
}

px_void PX_Object_GroundOnStop(PX_Object* pObject, PX_Object_Event e, px_void* ptr)
{
	PX_Object_Ground* pDesc = PX_ObjectGetDesc(PX_Object_Ground, pObject);
	pDesc->state = PX_Object_Ground_State_Stop;
	return;
}

PX_Object* PX_Object_GroundCreate(PX_World* pWorld, PX_Runtime* pruntime)
{
	px_int i;
	PX_Object* pObject;
	PX_Object_Ground desc;
	PX_memset(&desc, 0, sizeof(desc));
	desc.pruntime = pruntime;
	desc.pWorld = pWorld;
	desc.state = PX_Object_Ground_State_Ready;

	for (i=0;i< PX_OBJECT_GROUND_TEXTURE_COUNT;i++)
	{
		px_char path[260];
		PX_sprintf1(path, sizeof(path), "assets/ground%1.traw", PX_STRINGFORMAT_INT(i + 1));
		if (!PX_LoadTextureFromFile(&pruntime->mp_resources, &desc.textures[i], path))
			return PX_NULL;
	}
	PX_ListInitialize(&pruntime->mp_game, &desc.texturelist);
	pObject=PX_ObjectCreateEx(&pruntime->mp_game, PX_NULL, 0, 0, PX_OBJECT_GROUND_Z, 0, 0, 0, 0, 0, PX_Object_GroundRender, PX_Object_GroundFree, &desc, sizeof(desc));

	PX_ObjectRegisterEvent(pObject, PX_GAMEEVENT_READY, PX_Object_GroundOnReady, PX_NULL);
	PX_ObjectRegisterEvent(pObject, PX_GAMEEVENT_RUN, PX_Object_GroundOnRun, PX_NULL);
	PX_ObjectRegisterEvent(pObject, PX_GAMEEVENT_STOP, PX_Object_GroundOnStop, PX_NULL);

	return pObject;
}

