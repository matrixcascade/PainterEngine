#include "PX_Object_Fox.h"

px_void PX_Object_FoxUpdate(PX_Object *pObject,px_dword elpased)
{
	PX_Object_Fox *pDesc=PX_ObjectGetDesc(PX_Object_Fox,pObject);
	px_float x,y;
	
	if (pDesc->state!=PX_Object_Fox_State_Run)
	{
		return;
	}

	x=pObject->x;
	y=pDesc->pRuntime->surface_height-PX_OBJECT_FOX_BASE-pObject->y;

	if (y>0)
	{
		pDesc->velocity.y-=PX_OBJECT_WORLD_G*elpased/1000.f;
	}

	x+=pDesc->velocity.x*elpased/1000.f;
	y+=pDesc->velocity.y*elpased/1000.f;

	pObject->x=x;
	pObject->y=pDesc->pRuntime->surface_height-PX_OBJECT_FOX_BASE-y;

	if (pObject->y>pDesc->pRuntime->surface_height-PX_OBJECT_FOX_BASE)
	{
		pObject->y=pDesc->pRuntime->surface_height-PX_OBJECT_FOX_BASE*1.0f;
		if (pDesc->velocity.y<0)
		{
			pDesc->velocity.y=0;
		}
	}
	else 
	{
		if (pObject->y == pDesc->pRuntime->surface_height - PX_OBJECT_FOX_BASE)
		{
			pDesc->jump_cooldown = 2;
		}
	}
}

px_void PX_Object_FoxRender(px_surface *psurface,PX_Object *pObject,px_dword elpased)
{
	PX_Object_Fox *pDesc=PX_ObjectGetDesc(PX_Object_Fox,pObject);
	px_float x,y;

	x=pObject->x;
	y=pDesc->pRuntime->surface_height-PX_OBJECT_FOX_BASE-pObject->y;

	PX_AnimationUpdate(&pDesc->animation, elpased);

	switch (pDesc->state)
	{
	case PX_Object_Fox_State_Ready:
		{
			px_dword id = PX_AnimationGetCurrentPlayAnimation(&pDesc->animation);
			if (id != pDesc->animation_walk_id)
			{
				PX_AnimationSetCurrentPlayAnimation(&pDesc->animation, pDesc->animation_walk_id);
			}
		}
		break;
	case PX_Object_Fox_State_Run:
		{
			if (y > 0)
			{
				if (pDesc->velocity.y > 0)
				{
					px_dword id = PX_AnimationGetCurrentPlayAnimation(&pDesc->animation);
					if (id != pDesc->animation_up_id)
					{
						PX_AnimationSetCurrentPlayAnimation(&pDesc->animation, pDesc->animation_up_id);
					}
				}
				else
				{
					px_dword id = PX_AnimationGetCurrentPlayAnimation(&pDesc->animation);
					if (id != pDesc->animation_down_id)
					{
						PX_AnimationSetCurrentPlayAnimation(&pDesc->animation, pDesc->animation_down_id);
					}
				}
			}
			else
			{
				px_dword id = PX_AnimationGetCurrentPlayAnimation(&pDesc->animation);
				if (id != pDesc->animation_run_id)
				{
					PX_AnimationSetCurrentPlayAnimation(&pDesc->animation, pDesc->animation_run_id);
				}
			}
		}
		break;
	case PX_Object_Fox_State_Stop:
		{
			px_dword id = PX_AnimationGetCurrentPlayAnimation(&pDesc->animation);
			if (id != pDesc->animation_impact_id)
			{
				PX_AnimationSetCurrentPlayAnimation(&pDesc->animation, pDesc->animation_impact_id);
			}
		}
		break;
	default:
		return;
	}

	PX_AnimationRender(psurface, &pDesc->animation, (px_int)pObject->x, (px_int)pObject->y, PX_ALIGN_CENTER, PX_NULL);
}

px_void PX_Object_FoxFree(PX_Object *pObject)
{
	PX_Object_Fox *pDesc=PX_ObjectGetDesc(PX_Object_Fox,pObject);
	PX_AnimationFree(&pDesc->animation);
}


px_void PX_Object_FoxOnCursorDown(PX_Object *pObject,PX_Object_Event e,px_void *ptr)
{
	PX_Object_Fox *pDesc=PX_ObjectGetDesc(PX_Object_Fox,pObject);

	if (pDesc->state!=PX_Object_Fox_State_Run)
	{
		return;
	}

	if (pDesc->jump_cooldown)
	{
		pDesc->velocity.y=PX_OBJECT_FOX_IMPLUSE;
		pDesc->jump_cooldown--;
	}
}

px_void PX_Object_FoxOnReady(PX_Object* pObject, PX_Object_Event e, px_void* ptr)
{
	PX_Object_Fox* pDesc = PX_ObjectGetDesc(PX_Object_Fox, pObject);
	pDesc->state = PX_Object_Fox_State_Ready;
	return;
}

px_void PX_Object_FoxOnRun(PX_Object* pObject, PX_Object_Event e, px_void* ptr)
{
	PX_Object_Fox* pDesc = PX_ObjectGetDesc(PX_Object_Fox, pObject);
	pDesc->state = PX_Object_Fox_State_Run;
	return;
}

px_void PX_Object_FoxOnStop(PX_Object* pObject, PX_Object_Event e, px_void* ptr)
{
	PX_Object_Fox* pDesc = PX_ObjectGetDesc(PX_Object_Fox, pObject);
	pDesc->state = PX_Object_Fox_State_Stop;
	return;
}

px_void PX_Object_FoxOnImpact(PX_Object* pObject, PX_Object_Event e, px_void* ptr)
{
	PX_Object_Fox* pDesc = PX_ObjectGetDesc(PX_Object_Fox, pObject);
	PX_ObjectExecuteEvent(pDesc->pschedular, PX_OBJECT_BUILD_EVENT(PX_OBJECT_FOX_EVENT_IMPACT));
	return;
}

PX_Object * PX_Object_FoxCreate(PX_Object *schedular,PX_World *pWorld,PX_Runtime *pRuntime)
{
	PX_Object_Fox fox;
	PX_AnimationLibrary *pLibrary;
	PX_Object *pObject;
	PX_memset(&fox, 0, sizeof(fox));
	fox.pschedular = schedular;
	fox.pRuntime=pRuntime;
	fox.velocity.x=0;
	fox.velocity.y=0;
	fox.pworld = pWorld;
	pLibrary=PX_ResourceLibraryGetAnimationLibrary(&pRuntime->ResourceLibrary,"fox");
	fox.animation_walk_id = PX_AnimationLibraryGetPlayAnimationIndexByName(pLibrary, "walk");
	fox.animation_run_id = PX_AnimationLibraryGetPlayAnimationIndexByName(pLibrary, "run");
	fox.animation_up_id = PX_AnimationLibraryGetPlayAnimationIndexByName(pLibrary, "jump_up");
	fox.animation_down_id = PX_AnimationLibraryGetPlayAnimationIndexByName(pLibrary, "jump_down");
	fox.animation_impact_id = PX_AnimationLibraryGetPlayAnimationIndexByName(pLibrary, "impact");
	fox.state = PX_Object_Fox_State_Ready;


	if(!pLibrary)return PX_FALSE;
	if(!PX_AnimationCreate(&fox.animation,pLibrary))return PX_FALSE;
	
	pObject=PX_ObjectCreateEx(&pRuntime->mp_game,PX_NULL, PX_OBJECT_FOX_X, pRuntime->surface_height - PX_OBJECT_FOX_BASE*1.0f, 0,64,24,0,PX_OBJECT_FOX_TYPE,PX_Object_FoxUpdate,PX_Object_FoxRender,PX_Object_FoxFree,&fox,sizeof(PX_Object_Fox));

	if (!pObject)
	{
		return PX_FALSE;
	}
	pObject->impact_object_type = 1;
	pObject->impact_target_type = 2;
	
	PX_ObjectRegisterEvent(pObject, PX_OBJECT_EVENT_CURSORDOWN, PX_Object_FoxOnCursorDown, PX_NULL);
	PX_ObjectRegisterEvent(pObject, PX_GAMEEVENT_READY, PX_Object_FoxOnReady, PX_NULL);
	PX_ObjectRegisterEvent(pObject, PX_GAMEEVENT_RUN, PX_Object_FoxOnRun, PX_NULL);
	PX_ObjectRegisterEvent(pObject, PX_GAMEEVENT_STOP, PX_Object_FoxOnStop, PX_NULL);
	PX_ObjectRegisterEvent(pObject, PX_OBJECT_EVENT_IMPACT, PX_Object_FoxOnImpact, PX_NULL);
	return pObject;
}
