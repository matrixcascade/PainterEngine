#include "PX_Module_Game.h"

px_void PX_Module_GameUpdateOnce(PX_Object* pModule)
{
	PX_Module_Game* pDesc = PX_ObjectGetDesc(PX_Module_Game, pModule);
	PX_WorldUpdate(&pDesc->world, PX_MODULE_GAME_UPDATE);

	if (pDesc->state==PX_Module_Game_State_Runing)
	{
		if (pDesc->wood_gen > PX_MODULE_GAME_UPDATE)
		{
			pDesc->wood_gen -= PX_MODULE_GAME_UPDATE;
		}
		else
		{
			pDesc->wood_gen = 2000 + PX_rand() % 1000;
			PX_WorldAddObject(&pDesc->world, PX_Object_WoodCreate(&pDesc->world, pDesc->pRuntime));
		}

		if (pDesc->bird_gen > PX_MODULE_GAME_UPDATE)
		{
			pDesc->bird_gen -= PX_MODULE_GAME_UPDATE;
		}
		else
		{
			pDesc->bird_gen = 1000 + PX_rand() % 1000;
			PX_WorldAddObject(&pDesc->world, PX_Object_BirdCreate(&pDesc->world, pDesc->pRuntime));
		}

		if (pDesc->cloud_gen > PX_MODULE_GAME_UPDATE)
		{
			pDesc->cloud_gen -= PX_MODULE_GAME_UPDATE;
		}
		else
		{
			pDesc->cloud_gen = 100 + PX_rand() % 5000;
			PX_WorldAddObject(&pDesc->world, PX_Object_CloudCreate(&pDesc->world, pDesc->pRuntime));
		}
	}
	

}

px_void PX_Module_GameUpdate(PX_Object* pModule, px_dword elpased)
{
	PX_Module_Game* pDesc = PX_ObjectGetDesc(PX_Module_Game, pModule);
	static px_dword reservedTime = 0;
	px_dword updateTime = elpased + reservedTime;

	while (updateTime>=PX_MODULE_GAME_UPDATE)
	{
		PX_Module_GameUpdateOnce(pModule);
		updateTime -= PX_MODULE_GAME_UPDATE;
	}
	reservedTime = updateTime;
}

px_void PX_Module_GameRender(px_surface *psurface,PX_Object* pModule, px_dword elpased)
{
	PX_Module_Game* pDesc = PX_ObjectGetDesc(PX_Module_Game, pModule);
	PX_WorldRender(psurface,&pDesc->world, elpased);
}

px_void PX_Module_GameFree(PX_Object* pModule)
{
	PX_Module_Game* pDesc = PX_ObjectGetDesc(PX_Module_Game, pModule);
	PX_WorldFree(&pDesc->world);
}

px_void PX_Module_GameStart(PX_Object* pModule)
{
	PX_Module_Game* pDesc = PX_ObjectGetDesc(PX_Module_Game, pModule);
	PX_WorldPostEvent(&pDesc->world, PX_OBJECT_BUILD_EVENT(PX_GAMEEVENT_RUN));
	pDesc->state = PX_Module_Game_State_Runing;
	
}

px_void PX_Module_GameStop(PX_Object* pModule)
{
	PX_Module_Game* pDesc = PX_ObjectGetDesc(PX_Module_Game, pModule);
	PX_WorldPostEvent(&pDesc->world, PX_OBJECT_BUILD_EVENT(PX_GAMEEVENT_STOP));
	pDesc->state = PX_Module_Game_State_Stop;

}

px_void PX_Module_GameEventDispatch(PX_Object* pModule, PX_Object_Event e, px_void* ptr)
{
	PX_Module_Game* pDesc = PX_ObjectGetDesc(PX_Module_Game, pModule);
	switch (pDesc->state)
	{
	case PX_Module_Game_State_Ready:
	{
		if (e.Event==PX_OBJECT_EVENT_CURSORDOWN)
		{
			PX_Module_GameStart(pModule);
		}
	}
	break;
	case PX_Module_Game_State_Runing:
	{
		PX_ObjectPostEvent(pDesc->pFoxObject, e);
	}
	break;
	case PX_Module_Game_State_Stop:
	{
	}
	break;
	default:
		break;
	}
}

px_void PX_Module_GameSchedular(PX_Object* pSchedular, PX_Object_Event e, px_void* ptr)
{
	PX_Module_Game* pDesc = PX_ObjectGetDesc(PX_Module_Game, (PX_Object *)ptr);
	if (e.Event==PX_OBJECT_FOX_EVENT_IMPACT)
	{
		PX_Module_GameStop((PX_Object*)ptr);
	}
}


PX_Object* PX_Module_GameInstall(PX_Runtime* pruntime)
{
	PX_Object* pObject;
	PX_Module_Game Game,*pGame;
	
	PX_memset(&Game, 0, sizeof(Game));
	Game.pRuntime = pruntime;
	//////////////////////////////////////////////////////////////////////////
	//load resources 
	if (!PX_LoadAnimationLibraryToResource(&pruntime->ResourceLibrary, "assets/fox.2dx", "fox"))
		return PX_FALSE;

	if (!PX_LoadAnimationLibraryToResource(&pruntime->ResourceLibrary, "assets/bird.2dx", "bird"))
		return PX_FALSE;

	if (!PX_LoadTextureToResource(&pruntime->ResourceLibrary, "assets/wood1.traw", "wood1"))
		return PX_FALSE;

	if (!PX_LoadTextureToResource(&pruntime->ResourceLibrary, "assets/wood2.traw", "wood2"))
		return PX_FALSE;


	if (!PX_LoadTextureToResource(&pruntime->ResourceLibrary, "assets/cloud1.traw", "cloud1"))
		return PX_FALSE;

	if (!PX_LoadTextureToResource(&pruntime->ResourceLibrary, "assets/cloud2.traw", "cloud2"))
		return PX_FALSE;

	if (!PX_LoadTextureToResource(&pruntime->ResourceLibrary, "assets/cloud3.traw", "cloud3"))
		return PX_FALSE;

	if (!PX_LoadTextureToResource(&pruntime->ResourceLibrary, "assets/cloud4.traw", "cloud4"))
		return PX_FALSE;

	//////////////////////////////////////////////////////////////////////////

	if (!PX_WorldInitialize(&pruntime->mp_game, \
		&Game.world, pruntime->surface_width, pruntime->surface_height, \
		pruntime->surface_width, pruntime->surface_height, \
		1024 * 1024))
		return PX_FALSE;

	//Game.world.showImpactRegion = PX_TRUE;
	Game.state = PX_Module_Game_State_Ready;

	PX_WorldSetAuxiliaryLineColor(&Game.world, PX_COLOR(0, 0, 0, 0));

	pObject = PX_ObjectCreateEx(&pruntime->mp_game, PX_NULL, 0, 0, 0, 0, 0, 0, 0, PX_Module_GameUpdate, PX_Module_GameRender, PX_Module_GameFree, &Game, sizeof(PX_Module_Game));
	pGame = PX_ObjectGetDesc(PX_Module_Game, pObject);
	
	PX_ObjectRegisterEvent(pObject, PX_OBJECT_EVENT_ANY, PX_Module_GameEventDispatch, PX_NULL);

	//////////////////////////////////////////////////////////////////////////
	/// Create Game Objects
	pGame->pSchedular = PX_ObjectCreate(& pruntime->mp_game, 0, 0, 0, 0, 0, 0, 0);
	PX_ObjectRegisterEvent(pGame->pSchedular, PX_OBJECT_EVENT_ANY, PX_Module_GameSchedular, pObject);

	pGame->pFoxObject = PX_Object_FoxCreate(pGame->pSchedular,&pGame->world, pruntime);
	if (!pGame->pFoxObject)return PX_FALSE;
	PX_WorldAddObject(&pGame->world, pGame->pFoxObject);

	pGame->pGroundObject = PX_Object_GroundCreate(&pGame->world, pruntime);
	if (!pGame->pGroundObject)return PX_FALSE;
	PX_WorldAddObject(&pGame->world, pGame->pGroundObject);

	return pObject;
}

px_void PX_Module_GameUninstall(PX_Object* pModule)
{
	PX_Module_Game* pDesc = PX_ObjectGetDesc(PX_Module_Game, pModule);
	PX_Runtime* pruntime = pDesc->pRuntime;
	PX_ObjectDelete(pModule);
	PX_RuntimeReset(pruntime);
}
