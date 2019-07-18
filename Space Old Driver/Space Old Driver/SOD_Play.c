#include "SOD_Play.h"

//////////////////////////////////////////////////////////////////////////
//ship
//////////////////////////////////////////////////////////////////////////

//static
px_float SOD_Ship_Speed=0;
px_float SOD_Ship_Force=0;

//
px_void SOD_GenInitializePositionAndVelocity(SOD_Play *play,px_point *Position,px_point *velocty,px_float mod_velocity)
{
	px_int genx,geny,vpx,vpy;
	px_point gen_v;
	switch(PX_rand()%4)
	{
	case 0:
		{
			geny=-SOD_STONE_GEN_SPACE;
			genx=-SOD_STONE_GEN_SPACE+(px_int)(PX_rand()%(play->world.world_width+2*SOD_STONE_GEN_SPACE));
			vpx=(px_int)(PX_rand()%play->world.world_width);
			vpy=0;
			gen_v=PX_PointUnit(PX_POINT((px_float)(vpx-genx),(px_float)(vpy-geny),0));
			gen_v=PX_PointMul(gen_v,mod_velocity);
		}
		break;
	case 3:
		{
			geny=play->world.world_height+SOD_STONE_GEN_SPACE;
			genx=-SOD_STONE_GEN_SPACE+(px_int)(PX_rand()%(play->world.world_width+2*SOD_STONE_GEN_SPACE));
			vpx=(px_int)(PX_rand()%play->world.world_width);
			vpy=play->world.world_height;

			gen_v=PX_PointUnit(PX_POINT((px_float)(vpx-genx),(px_float)(vpy-geny),0));
			gen_v=PX_PointMul(gen_v,mod_velocity);
		}
		break;
	case 1:
		{
			geny=-SOD_STONE_GEN_SPACE+(px_int)(PX_rand()%(play->world.world_height+2*SOD_STONE_GEN_SPACE));
			genx=-SOD_STONE_GEN_SPACE;
			vpx=0;
			vpy=(px_int)(PX_rand()%play->world.world_height);

			gen_v=PX_PointUnit(PX_POINT((px_float)(vpx-genx),(px_float)(vpy-geny),0));
			gen_v=PX_PointMul(gen_v,mod_velocity);
		}
		break;
	case 2:
		{
			geny=-SOD_STONE_GEN_SPACE+(px_int)(PX_rand()%(play->world.world_height+2*SOD_STONE_GEN_SPACE));
			genx=play->world.world_width+SOD_STONE_GEN_SPACE;
			vpx=play->world.world_width;
			vpy=(px_int)(PX_rand()%play->world.world_height);

			gen_v=PX_PointUnit(PX_POINT((px_float)(vpx-genx),(px_float)(vpy-geny),0));
			gen_v=PX_PointMul(gen_v,mod_velocity);
		}
		break;
	}
	*Position=PX_POINT((px_float)genx,(px_float)geny,0);
	*velocty=gen_v;
}
//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
//// 世界粒子系统
SOD_Play_Object_ParitcalLauncher *SOD_Object_GetParticalLauncher(PX_Object *pObject)
{
	return (SOD_Play_Object_ParitcalLauncher *)pObject->pObject;
}

px_void SOD_Object_ParticalLauncherFree(PX_Object *pObject)
{
	SOD_Play_Object_ParitcalLauncher *pFire=(SOD_Play_Object_ParitcalLauncher *)pObject->pObject;

	PX_ParticalLauncherFree(&pFire->launcher);
}

px_void SOD_Object_ParticalLauncherUpdate(PX_Object *pObject,px_dword elpased)
{
	SOD_Play *pPlay=(SOD_Play *)pObject->User_ptr;
	SOD_Play_Object_ParitcalLauncher *pFire=(SOD_Play_Object_ParitcalLauncher *)pObject->pObject;
	if (elpased>0)
	{
		if (elpased>pFire->alive)
		{
			pFire->alive=0;
		}
		else
		{
			pFire->alive-=elpased;
		}
	}
	PX_ParticalLauncherUpdate(&pFire->launcher,elpased);

	if (pFire->alive==0)
	{
		PX_WorldRemoveObject(&pPlay->world,pObject);
	}
}

px_void SOD_Object_ParticalLauncherRender(px_surface *psurface,PX_Object *pObject,px_dword elpased)
{
	SOD_Play_Object_ParitcalLauncher *pFire=(SOD_Play_Object_ParitcalLauncher *)pObject->pObject;
	PX_ParticalLauncherRender(psurface,&pFire->launcher,PX_POINT(pObject->x,pObject->y,pObject->z));
}

PX_Object* SOD_Object_ParticalLauncherCreate(SOD_Play *play,px_memorypool *mp,PX_ParticalLauncher_InitializeInfo init_info,px_dword alive)
{
	PX_Object *pObject=PX_NULL;
	px_texture *pTexture=init_info.tex;
	SOD_Play_Object_ParitcalLauncher *pLauncher;

	if (!pTexture)
	{
		PX_ASSERT();
		return PX_NULL;
	}

	pLauncher=(SOD_Play_Object_ParitcalLauncher *)MP_Malloc(&play->fire_mp,sizeof(SOD_Play_Object_ParitcalLauncher));

	if (pLauncher==PX_NULL)
	{
		PX_ASSERT();
		return PX_NULL;
	}

	pObject=(PX_Object *)PX_ObjectCreate(&play->fire_mp,PX_NULL,0,0,0,0,0,0);

	if (!pObject)
	{
		MP_Free(&play->fire_mp,pLauncher);
		PX_ASSERT();
		return PX_NULL;
	}
	pObject->x=0;
	pObject->y=0;
	pObject->z=SOD_PARTICAL_FIRE_Z;
	pObject->Width=1;
	pObject->Height=1;


	pObject->pObject=pLauncher;
	pObject->Type=SOD_OBJECT_TYPE_PARTICAL;
	pObject->ReceiveEvents=PX_FALSE;
	pObject->Func_ObjectFree=SOD_Object_ParticalLauncherFree;
	pObject->Func_ObjectUpdate=SOD_Object_ParticalLauncherUpdate;
	pObject->Func_ObjectRender=SOD_Object_ParticalLauncherRender;
	pObject->User_ptr=play;

	pLauncher->alive=alive;

	PX_ParticalLauncherCreateEx(&pLauncher->launcher,mp,init_info);
	pLauncher->launcher.user=pLauncher;
	return pObject;
}
//////////////////////////////////////////////////////////////////////////
PX_Partical_Atom SOD_ParticalLauncher_Explosion_CreatePartical(struct _PX_Partical_Launcher *launcher,px_int index)
{
	PX_Partical_Atom atom;
	px_float x_v,y_v;
	SOD_Play_Object_ParitcalLauncher *pL=(SOD_Play_Object_ParitcalLauncher *)launcher->user;
	
	x_v=-pL->range+(2*pL->range)*((PX_rand()%1000)/1000.f);
	y_v=PX_sqrt(pL->range*pL->range-x_v*x_v);
	y_v=-y_v+(2*y_v)*((PX_rand()%1000)/1000.f);

	atom.aliveTime=1000;
	atom.alpha=1;
	atom.alphaIncrement=-1.0f;
	atom.elpasedTime=0;
	atom.hdrB=1;
	atom.hdrG=1;
	atom.hdrR=1;
	atom.mass=1;
	atom.position=PX_POINT(x_v,y_v,0);
	atom.roatationSpeed=0;
	atom.rotation=0;
	atom.size=pL->range/100+(PX_rand()%1000)/4000.f;
	atom.sizeIncrement=0.1f;
	atom.velocity=PX_PointMul(PX_PointUnit(atom.position),300*((PX_rand()%1000)/1000.f));
	return atom;
}

px_void SOD_CreateExplosion(SOD_Play *pPlay,px_point position,px_float range)
{
	PX_ParticalLauncher_InitializeInfo partical_info;
	PX_Object *pObj;
	SOD_Play_Object_ParitcalLauncher *pLauncher;
	partical_info.Create_func=SOD_ParticalLauncher_Explosion_CreatePartical;
	partical_info.force=PX_POINT(0,0,0);
	partical_info.generateDuration=0;
	if (range<16)
	{
		partical_info.launchCount=(px_int)16;
		partical_info.maxCount=(px_int)16;
	}
	else
	{
		partical_info.launchCount=(px_int)range;
		partical_info.maxCount=(px_int)range;
	}
	
	partical_info.resistanceK=2.0f;
	partical_info.tex=PX_ResourceLibraryGetTexture(&pPlay->runtime->runtime.ResourceLibrary,SOD_KEY_TEX_EXPLODE_PARTICAL);
	partical_info.Update_func=PX_NULL;

	pObj=SOD_Object_ParticalLauncherCreate(pPlay,&pPlay->explode_mp,partical_info,1000);
	if (pObj)
	{
		pLauncher=SOD_Object_GetParticalLauncher(pObj);
		if (PX_WorldAddObject(&pPlay->world,pObj))
		{
			pLauncher->range=range;
			pLauncher->launcher.launcherPosition=position;
		}
	}

	switch(PX_rand()%3)
	{
	case 0:
		SOD_PlaySoundFromResource(pPlay->runtime,SOD_KEY_SND_EXPLODE01,PX_FALSE);
		break;
	case 1:
		SOD_PlaySoundFromResource(pPlay->runtime,SOD_KEY_SND_EXPLODE02,PX_FALSE);
		break;
	case 2:
		SOD_PlaySoundFromResource(pPlay->runtime,SOD_KEY_SND_EXPLODE02,PX_FALSE);
		break;
	}
}


PX_Partical_Atom SOD_ParticalLauncher_StarExplosion_CreatePartical(struct _PX_Partical_Launcher *launcher,px_int index)
{
	PX_Partical_Atom atom;
	px_float x_v,y_v;
	

	x_v=-16+(2*16)*((PX_rand()%1000)/1000.f);
	y_v=PX_sqrt(16*16-x_v*x_v);
	y_v=-y_v+(2*y_v)*((PX_rand()%1000)/1000.f);

	atom.aliveTime=1000;
	atom.alpha=0.6f;
	atom.alphaIncrement=-1.0f;
	atom.elpasedTime=0;
	atom.hdrB=0.5f+(PX_rand()%1000)/2000.f;
	atom.hdrG=0.5f+(PX_rand()%1000)/2000.f;
	atom.hdrR=0.5f+(PX_rand()%1000)/2000.f;
	atom.mass=1;
	atom.position=PX_POINT(x_v,y_v,0);
	atom.roatationSpeed=0;
	atom.rotation=0;
	atom.size=0.5f+(PX_rand()%1000)/2000.f;
	atom.sizeIncrement=0.1f;
	atom.velocity=PX_PointMul(PX_PointUnit(atom.position),300*((PX_rand()%1000)/1000.f));
	return atom;
}

px_void SOD_CreateStarExpolsion(SOD_Play *pPlay,px_point position)
{
	PX_ParticalLauncher_InitializeInfo partical_info;
	PX_Object *pObj;
	SOD_Play_Object_ParitcalLauncher *pLauncher;
	partical_info.Create_func=SOD_ParticalLauncher_StarExplosion_CreatePartical;
	partical_info.force=PX_POINT(0,0,0);
	partical_info.generateDuration=0;
	partical_info.launchCount=24;
	partical_info.maxCount=24;
	partical_info.resistanceK=2.0f;
	partical_info.tex=PX_ResourceLibraryGetTexture(&pPlay->runtime->runtime.ResourceLibrary,SOD_KEY_TEX_STAR);
	partical_info.Update_func=PX_NULL;

	pObj=SOD_Object_ParticalLauncherCreate(pPlay,&pPlay->explode_mp,partical_info,1500);
	if (pObj)
	{
		pLauncher=SOD_Object_GetParticalLauncher(pObj);
		if (PX_WorldAddObject(&pPlay->world,pObj))
		{
			pLauncher->launcher.launcherPosition=position;
		}
	}
}
//////////////////////////////////////////////////////////////////////////
//星星
SOD_Play_Object_Star *SOD_Object_GetStar(PX_Object *pObject)
{
	return (SOD_Play_Object_Star *)pObject->pObject;
}

px_void SOD_OnStarImpact(PX_Object *pObj,PX_Object_Event e,px_void *ptr)
{
	PX_Object_Event pe;
	SOD_Play_Object_Star *star=SOD_Object_GetStar(pObj);
	SOD_Play *pPlay=(SOD_Play *)pObj->User_ptr;

	switch(star->type)
	{
	case SOD_PLAY_STAR_TYPE_ATOM:
		pe.Event=SOD_OBJECT_EVENT_ATOM;
		pe.Param_int[0]=SOD_STAR_BUFFER_ATOM;
		break;
	case SOD_PLAY_STAR_TYPE_FORCE:
		pe.Event=SOD_OBJECT_EVENT_ADDFORCE;
		pe.Param_int[0]=SOD_STAR_BUFFER_FORCE;
		break;
	case SOD_PLAY_STAR_TYPE_HEAL:
		pe.Event=SOD_OBJECT_EVENT_HEALTH;
		pe.Param_int[0]=SOD_STAR_BUFFER_HEALTH;
		break;
	case SOD_PLAY_STAR_TYPE_SHIELD:
		pe.Event=SOD_OBJECT_EVENT_SHIELD;
		pe.Param_int[0]=0;
		break;
	case SOD_PLAY_STAR_TYPE_WEAPON:
		pe.Event=SOD_OBJECT_EVENT_ADDWEAPON;
		pe.Param_int[0]=SOD_STAR_BUFFER_WEAPON;
		break;
	}
	//发布消息
	PX_ObjectPostEvent((PX_Object *)e.param_ptr[0],pe);
	//播放音效
	SOD_PlaySoundFromResource(pPlay->runtime,SOD_KEY_SND_STARAPPLY,PX_FALSE);

	//移除对象
	PX_WorldRemoveObject(&pPlay->world,pObj);
	
	//星星效果
	SOD_CreateStarExpolsion(pPlay,PX_POINT(pObj->x,pObj->y,0));
	
}



px_void SOD_Object_StarFree(PX_Object *pObject)
{
	SOD_Play_Object_Star *pstar=SOD_Object_GetStar(pObject);
	PX_AnimationFree(&pstar->star_animation);
}

px_void SOD_Object_StarUpdate(PX_Object *pObject,px_dword elpased)
{
	SOD_Play *pPlay=(SOD_Play *)pObject->User_ptr;
	SOD_Play_Object_Star *pStar=SOD_Object_GetStar(pObject);

	PX_AnimationUpdate(&pStar->star_animation,elpased);

	pObject->x+=pStar->velocity.x*elpased/1000;
	pObject->y+=pStar->velocity.y*elpased/1000;

	//越界,删除
	if (pObject->x<0||pObject->x>pPlay->world.world_width||\
		pObject->y<0||pObject->y>pPlay->world.world_height\
		)
	{
		PX_WorldRemoveObject(&pPlay->world,pObject);

	}
}

px_void SOD_Object_StarRender(px_surface *psurface,PX_Object *pObject,px_dword elpased)
{
	SOD_Play_Object_Star *pStar=SOD_Object_GetStar(pObject);
	PX_AnimationRender(psurface,&pStar->star_animation,PX_POINT(pObject->x,pObject->y,0),PX_TEXTURERENDER_REFPOINT_CENTER,&pStar->blend);
}


PX_Object* SOD_Object_StarCreate(SOD_Play *play,px_float x,px_float y,px_float velocity)
{
	PX_Object *pObject;
	SOD_Play_Object_Star *pStar;
	px_float x_v,y_v;

	x_v=-10+(2*10)*((PX_rand()%1000)/1000.f);
	y_v=PX_sqrt(10*10-x_v*x_v);
	y_v=-y_v+(2*y_v)*((PX_rand()%1000)/1000.f);



	pStar=(SOD_Play_Object_Star *)MP_Malloc(&play->runtime->runtime.mp_game,sizeof(SOD_Play_Object_Star));

	if (pStar==PX_NULL)
	{
		PX_ASSERT();
		return PX_NULL;
	}

	pObject=(PX_Object *)PX_ObjectCreate(&play->runtime->runtime.mp_game,PX_NULL,0,0,0,0,0,0);

	if (!pObject)
	{
		MP_Free(&play->runtime->runtime.mp_game,pStar);
		PX_ASSERT();
		return PX_NULL;
	}

	pObject->x=x;
	pObject->y=y;
	pObject->z=SOD_STAR_Z;
	pObject->Width=48;
	pObject->diameter=48;
	pObject->Height=48;
	pObject->pObject=pStar;
	pObject->Type=SOD_OBJECT_TYPE_STAR;
	pObject->ReceiveEvents=PX_TRUE;
	pObject->Func_ObjectFree=SOD_Object_StarFree;
	pObject->Func_ObjectUpdate=SOD_Object_StarUpdate;
	pObject->Func_ObjectRender=SOD_Object_StarRender;
	pObject->User_ptr=play;
	pObject->impact_Object_type=SOD_IMPACTTEST_OBJECTTYPE_NEUTRALITY;
	pObject->impact_test_type=SOD_IMPACTTEST_OBJECTTYPE_USER;

	PX_AnimationCreate(&pStar->star_animation,PX_ResourceLibraryGetAnimationLibrary(&play->runtime->runtime.ResourceLibrary,SOD_KEY_ANIMATION_STAR));
	PX_AnimationSetCurrentPlayAnimationByName(&pStar->star_animation,SOD_ANIMATION_KEY_STAR_NORMAL);

	switch ((PX_rand()%5))
	{
	case 0:
		pStar->type=SOD_PLAY_STAR_TYPE_ATOM;
		pStar->blend.alpha=0.8f;
		pStar->blend.hdr_R=0.8f;
		pStar->blend.hdr_G=0.8f;
		pStar->blend.hdr_B=0.8f;
		break;
	case 1:
		pStar->type=SOD_PLAY_STAR_TYPE_FORCE;
		pStar->blend.alpha=0.6f;
		pStar->blend.hdr_R=1;
		pStar->blend.hdr_G=0.5f;
		pStar->blend.hdr_B=0.1f;
		break;
	case 2:
		pStar->type=SOD_PLAY_STAR_TYPE_HEAL;
		pStar->blend.alpha=0.6f;
		pStar->blend.hdr_R=0;
		pStar->blend.hdr_G=1;
		pStar->blend.hdr_B=0;
		break;
	case 3:
		pStar->type=SOD_PLAY_STAR_TYPE_SHIELD;
		pStar->blend.alpha=0.6f;
		pStar->blend.hdr_R=0;
		pStar->blend.hdr_G=0.85f;
		pStar->blend.hdr_B=1;
		break;
	case 4:
		pStar->type=SOD_PLAY_STAR_TYPE_WEAPON;
		pStar->blend.alpha=0.6f;
		pStar->blend.hdr_R=1.0f;
		pStar->blend.hdr_G=1.0f;
		pStar->blend.hdr_B=0;
		break;
	default:
		PX_ASSERT();
	}
	pStar->velocity=PX_PointMul(PX_PointUnit(PX_POINT(x_v,y_v,0)),velocity); 
	
	PX_ObjectRegisterEvent(pObject,PX_OBJECT_EVENT_IMPACT,SOD_OnStarImpact,PX_NULL);
	return pObject;
}

px_void SOD_CreateStarWithRandom(SOD_Play *play,px_float x,px_float y)
{
	if ((PX_rand()%100)<SOD_DROP_PERCENT)
	{
		PX_Object *pObj=SOD_Object_StarCreate(play,x,y,SOD_STAR_SPEED);
		if(!PX_WorldAddObject(&play->world,pObj))
			PX_ObjectDelete(pObj);
	}
}


//////////////////////////////////////////////////////////////////////////
//fire partical
PX_Partical_Atom SOD_ParticalLauncher_ShipFire_CreatePartical(struct _PX_Partical_Launcher *launcher,px_int index)
{
	PX_Partical_Atom atom;
	atom.aliveTime=300;
	atom.alpha=1;
	atom.alphaIncrement=-3.0f;
	atom.elpasedTime=0;
	atom.hdrB=1;
	atom.hdrG=1;
	atom.hdrR=1;
	atom.mass=1;
	atom.position=PX_POINT(0,0,0);
	atom.roatationSpeed=0;
	atom.rotation=0;
	atom.size=1;
	atom.sizeIncrement=0.0f;
	atom.velocity=PX_POINT((px_float)((px_int)(PX_rand()%100)+150),(px_float)(-55+(px_int)(PX_rand()%110)),0);
	return atom;
}


//////////////////////////////////////////////////////////////////////////
//Alien fire
//子弹碰撞直接删除
px_void SOD_OnAlienFireAtomDamage(PX_Object *pObject,PX_Object_Event e,px_void *ptr)
{
	SOD_Play *pPlay=(SOD_Play *)pObject->User_ptr;
	PX_WorldRemoveObject(&pPlay->world,pObject);
	SOD_CreateExplosion(pPlay,PX_POINT(pObject->x,pObject->y,0),1);
}

px_void SOD_OnAlienFireAtomImpact(PX_Object *pObj,PX_Object_Event e,px_void *ptr)
{
	PX_Object_Event pe;
	SOD_Play *pPlay=(SOD_Play *)pObj->User_ptr;
	PX_WorldRemoveObject(&pPlay->world,pObj);
	SOD_CreateExplosion(pPlay,PX_POINT(pObj->x,pObj->y,0),10);
	pe.Event=SOD_OBJECT_EVENT_DAMAGE;
	pe.Param_int[0]=SOD_ALIEN_BULLET_DAMAGE;
	PX_ObjectPostEvent((PX_Object *)e.param_ptr[0],pe);
}

SOD_Play_Object_AlienFireAtom *SOD_Object_GetAlienFireAtom(PX_Object *pObject)
{
	return (SOD_Play_Object_AlienFireAtom *)pObject->pObject;
}

px_void SOD_Object_AlienFireAtomFree(PX_Object *pObject)
{
	SOD_Play_Object_AlienFireAtom *pfa=SOD_Object_GetAlienFireAtom(pObject);
	PX_AnimationFree(&pfa->af_animation);
}

px_void SOD_Object_AlienFireAtomUpdate(PX_Object *pObject,px_dword elpased)
{
	SOD_Play *pPlay=(SOD_Play *)pObject->User_ptr;
	SOD_Play_Object_AlienFireAtom *pfa=SOD_Object_GetAlienFireAtom(pObject);

	PX_AnimationUpdate(&pfa->af_animation,elpased);

	pObject->x+=pfa->velocity.x*elpased/1000;
	pObject->y+=pfa->velocity.y*elpased/1000;

	//子弹越界,删除
	if (pObject->x<-SOD_STONE_GEN_SPACE-1||pObject->x>pPlay->world.world_width+SOD_STONE_GEN_SPACE+1||\
		pObject->y<-SOD_STONE_GEN_SPACE-1||pObject->y>pPlay->world.world_height+SOD_STONE_GEN_SPACE+1\
		)
	{
		PX_WorldRemoveObject(&pPlay->world,pObject);
		
	}
	else
	{
		//生存时间过,删除
		if (pfa->alive>elpased)
		{
			pfa->alive-=elpased;
		}
		else
		{
			PX_WorldRemoveObject(&pPlay->world,pObject);
			SOD_CreateExplosion(pPlay,PX_POINT(pObject->x,pObject->y,0),1);
		}
	}
}

px_void SOD_Object_AlienFireAtomRender(px_surface *psurface,PX_Object *pObject,px_dword elpased)
{
	SOD_Play_Object_AlienFireAtom *pfa=SOD_Object_GetAlienFireAtom(pObject);
	PX_AnimationRender(psurface,&pfa->af_animation,PX_POINT(pObject->x,pObject->y,0),PX_TEXTURERENDER_REFPOINT_CENTER,PX_NULL);
}


PX_Object* SOD_Object_AlienFireAtomCreate(SOD_Play *play,px_float x,px_float y,px_float velocity)
{
	PX_Object *pObject;
	SOD_Play_Object_AlienFireAtom *paatom;
	px_point shippt,atom_v;

	paatom=(SOD_Play_Object_AlienFireAtom *)MP_Malloc(&play->runtime->runtime.mp_game,sizeof(SOD_Play_Object_AlienFireAtom));

	if (paatom==PX_NULL)
	{
		PX_ASSERT();
		return PX_NULL;
	}

	pObject=(PX_Object *)PX_ObjectCreate(&play->runtime->runtime.mp_game,PX_NULL,0,0,0,0,0,0);

	if (!pObject)
	{
		MP_Free(&play->runtime->runtime.mp_game,paatom);
		PX_ASSERT();
		return PX_NULL;
	}

	pObject->x=x;
	pObject->y=y;
	pObject->z=SOD_ATOM_Z;
	pObject->Width=42;
	pObject->diameter=42;
	pObject->Height=42;
	pObject->pObject=paatom;
	pObject->Type=SOD_OBJECT_TYPE_ALIEN_ATOM;
	pObject->ReceiveEvents=PX_TRUE;
	pObject->Func_ObjectFree=SOD_Object_AlienFireAtomFree;
	pObject->Func_ObjectUpdate=SOD_Object_AlienFireAtomUpdate;
	pObject->Func_ObjectRender=SOD_Object_AlienFireAtomRender;
	pObject->User_ptr=play;
	pObject->impact_Object_type=SOD_IMPACTTEST_OBJECTTYPE_ENEMY;
	pObject->impact_test_type=SOD_IMPACTTEST_OBJECTTYPE_USER;

	PX_AnimationCreate(&paatom->af_animation,PX_ResourceLibraryGetAnimationLibrary(&play->runtime->runtime.ResourceLibrary,SOD_KEY_ANIMATION_AATOM));
	PX_AnimationSetCurrentPlayAnimationByName(&paatom->af_animation,SOD_ANIMATION_KEY_AATOM_NORMAL);

	shippt.x=play->shipObject->x;
	shippt.y=play->shipObject->y;
	shippt.z=0;
	atom_v=PX_PointSub(shippt,PX_POINT(x,y,0));

	paatom->velocity=PX_PointMul(PX_PointUnit(atom_v),velocity); 
	paatom->alive=SOD_ALIEN_ATOM_ALIVE_TIME;

	PX_ObjectRegisterEvent(pObject,PX_OBJECT_EVENT_IMPACT,SOD_OnAlienFireAtomImpact,PX_NULL);
	PX_ObjectRegisterEvent(pObject,SOD_OBJECT_EVENT_DAMAGE,SOD_OnAlienFireAtomDamage,PX_NULL);
	return pObject;
}


//////////////////////////////////////////////////////////////////////////
//alien

SOD_Play_Object_Alien *SOD_Object_GetAlien(PX_Object *pObject)
{
	return (SOD_Play_Object_Alien *)pObject->pObject;
}

px_void SOD_Object_Alien_OnDamage(PX_Object *pObject,PX_Object_Event e,px_void *ptr)
{
	SOD_Play_Object_Alien *Alien=SOD_Object_GetAlien(pObject);
	SOD_Play *pPlay=(SOD_Play *)pObject->User_ptr;
	if (Alien->life<=e.Param_int[0])
	{
		Alien->life=0;
		//destory alien
		PX_WorldRemoveObject(&pPlay->world,pObject);
		SOD_CreateExplosion(pPlay,PX_POINT(pObject->x,pObject->y,0),100);
		pPlay->score+=80;
		SOD_CreateStarWithRandom(pPlay,pObject->x,pObject->y);
	}
	else
	{
		if (Alien->status!=SOD_PLAY_ALIEN_STATUS_VERYANGRY)
		{
			Alien->status=SOD_PLAY_ALIEN_STATUS_VERYANGRY;
			PX_AnimationSetCurrentPlayAnimationByName(&Alien->alien_animation,SOD_ANIMATION_KEY_ALIEN_VERYANGRY);
		}
		
		Alien->life-=e.Param_int[0];
		Alien->beAttackElpased=0;
	}
}

px_void SOD_Object_Alien_OnImpact(PX_Object *pObject,PX_Object_Event e,px_void *ptr)
{
	SOD_Play_Object_Alien *Alien=SOD_Object_GetAlien(pObject);
	PX_Object *pTarget=(PX_Object *)e.param_ptr[0];
	SOD_Play *pPlay=(SOD_Play *)pObject->User_ptr;
	if (pTarget->Type==SOD_OBJECT_TYPE_SHIP)
	{
		PX_Object_Event e;
		PX_WorldRemoveObject(&pPlay->world,pObject);
		SOD_CreateExplosion(pPlay,PX_POINT(pObject->x,pObject->y,0),100);
		e.Event=SOD_OBJECT_EVENT_DAMAGE;
		e.Param_int[0]=SOD_ALIEN_IMPACT_DAMAGE;
		PX_ObjectPostEvent(pTarget,e);
	}
}



px_void SOD_Object_AlienFree(PX_Object *pObject)
{
	SOD_Play_Object_Alien *pAlien=SOD_Object_GetAlien(pObject);
	PX_AnimationFree(&pAlien->alien_animation);
}

px_void SOD_Object_AlienUpdate(PX_Object *pObject,px_dword elpased)
{
	SOD_Play *pPlay=(SOD_Play *)pObject->User_ptr;
	SOD_Play_Object_Alien *pAlien=SOD_Object_GetAlien(pObject);

	pAlien->atom_elpased+=elpased;
	//假如飞船还存活
	if (pPlay->shipObject->Visible)
	{
		if (pAlien->status==SOD_PLAY_ALIEN_STATUS_VERYANGRY)
		{
			pAlien->velocity=PX_PointSub(PX_POINT(pPlay->shipObject->x,pPlay->shipObject->y,0),PX_POINT(pObject->x,pObject->y,0));
			pAlien->velocity=PX_PointMul(PX_PointUnit(pAlien->velocity),SOD_ALIEN_SPEED);
		}

		if (SOD_ALIEN_SEARCH_DISTANCE>PX_PointDistance(PX_POINT(pObject->x,pObject->y,0),PX_POINT(pPlay->shipObject->x,pPlay->shipObject->y,0)))
		{
			//向飞船发射子弹
			if (pAlien->atom_count>0&&pAlien->atom_elpased>800)
			{
				PX_Object *pfa;
				pAlien->atom_count--;
				pAlien->atom_elpased=0;
				//fire
				pfa=SOD_Object_AlienFireAtomCreate(pPlay,pObject->x,pObject->y,SOD_ALIEN_ATOM_SPEED);
				if (pfa)
				{
					PX_WorldAddObject(&pPlay->world,pfa);
				}
				SOD_PlaySoundFromResource(pPlay->runtime,SOD_KEY_SND_ALIEN_FIRE,PX_FALSE);
			}
			if(pAlien->status==SOD_PLAY_ALIEN_STATUS_NORMAL)
			{
				PX_AnimationSetCurrentPlayAnimationByName(&pAlien->alien_animation,SOD_ANIMATION_KEY_ALIEN_ANGRY);
			}
			if (pAlien->status!=SOD_PLAY_ALIEN_STATUS_VERYANGRY)
			{
				pAlien->status=SOD_PLAY_ALIEN_STATUS_ANGRY;
			}
			
		}
		else
		{
			if(pAlien->status==SOD_PLAY_GHOST_STATUS_ANGRY)
			{
				PX_AnimationSetCurrentPlayAnimationByName(&pAlien->alien_animation,SOD_ANIMATION_KEY_ALIEN_NORMAL);
			}
			if (pAlien->status!=SOD_PLAY_ALIEN_STATUS_VERYANGRY)
			{
				pAlien->status=SOD_PLAY_ALIEN_STATUS_NORMAL;
			}
		}
	}

	pAlien->beAttackElpased+=elpased;
	pObject->x+=pAlien->velocity.x*elpased/1000;
	pObject->y+=pAlien->velocity.y*elpased/1000;


	//Alien越界,删除这个Ghost
	if (pObject->x<-SOD_STONE_GEN_SPACE-1||pObject->x>pPlay->world.world_width+SOD_STONE_GEN_SPACE+1||\
		pObject->y<-SOD_STONE_GEN_SPACE-1||pObject->y>pPlay->world.world_height+SOD_STONE_GEN_SPACE+1\
		)
	{
		PX_WorldRemoveObject(&pPlay->world,pObject);
	}
	//更新动画信息
	PX_AnimationUpdate(&pAlien->alien_animation,elpased);
}

px_void SOD_Object_AlienRender(px_surface *psurface,PX_Object *pObject,px_dword elpased)
{
	SOD_Play_Object_Alien *pAlien=SOD_Object_GetAlien(pObject);
	if (pAlien->beAttackElpased<50)
	{
		PX_TEXTURERENDER_BLEND blend;
		blend.alpha=1;
		blend.hdr_B=0;
		blend.hdr_G=0;
		blend.hdr_R=1;
		PX_AnimationRender(psurface,&pAlien->alien_animation,PX_POINT(pObject->x,pObject->y,0),PX_TEXTURERENDER_REFPOINT_CENTER,&blend);
	}
	else
		PX_AnimationRender(psurface,&pAlien->alien_animation,PX_POINT(pObject->x,pObject->y,0),PX_TEXTURERENDER_REFPOINT_CENTER,PX_NULL);


}

PX_Object* SOD_Object_AlienCreate(SOD_Play *play,px_int life,px_float velocity)
{
	PX_Object *pObject;
	SOD_Play_Object_Alien *pAlien;
	px_point genPostion,genVelocity;

	pAlien=(SOD_Play_Object_Alien *)MP_Malloc(&play->runtime->runtime.mp_game,sizeof(SOD_Play_Object_Alien));

	if (pAlien==PX_NULL)
	{
		PX_ASSERT();
		return PX_NULL;
	}

	pObject=(PX_Object *)PX_ObjectCreate(&play->runtime->runtime.mp_game,PX_NULL,0,0,0,0,0,0);

	if (!pObject)
	{
		MP_Free(&play->runtime->runtime.mp_game,pAlien);
		PX_ASSERT();
		return PX_NULL;
	}

	SOD_GenInitializePositionAndVelocity(play,&genPostion,&genVelocity,velocity);

	pObject->x=genPostion.x;
	pObject->y=genPostion.y;
	pObject->z=SOD_ALIEN_Z;
	pObject->Width=86;
	//pObject->diameter=84;
	pObject->Height=71;
	pObject->pObject=pAlien;
	pObject->Type=SOD_OBJECT_TYPE_ALIEN;
	pObject->ReceiveEvents=PX_TRUE;
	pObject->Func_ObjectFree=SOD_Object_AlienFree;
	pObject->Func_ObjectUpdate=SOD_Object_AlienUpdate;
	pObject->Func_ObjectRender=SOD_Object_AlienRender;
	pObject->User_ptr=play;
	pObject->impact_Object_type=SOD_IMPACTTEST_OBJECTTYPE_ENEMY;
	pObject->impact_test_type=SOD_IMPACTTEST_OBJECTTYPE_USER;

	pAlien->beAttackElpased=0;
	pAlien->life=life;
	PX_AnimationCreate(&pAlien->alien_animation,PX_ResourceLibraryGetAnimationLibrary(&play->runtime->runtime.ResourceLibrary,SOD_KEY_ANIMATION_ALIEN));
	PX_AnimationSetCurrentPlayAnimationByName(&pAlien->alien_animation,SOD_ANIMATION_KEY_ALIEN_NORMAL);
	pAlien->velocity=genVelocity;
	pAlien->status=SOD_PLAY_ALIEN_STATUS_NORMAL;
	pAlien->atom_count=5;
	pAlien->atom_elpased=0;

	PX_ObjectRegisterEvent(pObject,PX_OBJECT_EVENT_IMPACT,SOD_Object_Alien_OnImpact,PX_NULL);
	PX_ObjectRegisterEvent(pObject,SOD_OBJECT_EVENT_DAMAGE,SOD_Object_Alien_OnDamage,PX_NULL);
	return pObject;
}
//////////////////////////////////////////////////////////////////////////
//User fire
//子弹碰撞直接删除
px_void SOD_OnUserBulletDamage(PX_Object *pObject,PX_Object_Event e,px_void *ptr)
{
	SOD_Play *pPlay=(SOD_Play *)pObject->User_ptr;
	PX_WorldRemoveObject(&pPlay->world,pObject);//删除子弹
	SOD_CreateExplosion(pPlay,PX_POINT(pObject->x,pObject->y,0),1);//创建爆炸效果
}

//子弹碰撞
px_void SOD_OnUserBulletImpact(PX_Object *pObj,PX_Object_Event e,px_void *ptr)
{
	PX_Object_Event pe;
	SOD_Play *pPlay=(SOD_Play *)pObj->User_ptr;
	PX_WorldRemoveObject(&pPlay->world,pObj);//删除子弹
	SOD_CreateExplosion(pPlay,PX_POINT(pObj->x,pObj->y,0),10);//创建爆炸效果
	pe.Event=SOD_OBJECT_EVENT_DAMAGE;//damage消息
	pe.Param_int[0]=SOD_USER_BULLET_DAMAGE;//伤害量
	PX_ObjectPostEvent((PX_Object *)e.param_ptr[0],pe);//投递消息
}

SOD_Play_Object_UserBullet *SOD_Object_GetUserBullet(PX_Object *pObject)
{
	return (SOD_Play_Object_UserBullet *)pObject->pObject;
}

px_void SOD_Object_UserBulletFree(PX_Object *pObject)
{
	
}

px_void SOD_Object_UserBulletUpdate(PX_Object *pObject,px_dword elpased)
{
	SOD_Play *pPlay=(SOD_Play *)pObject->User_ptr;
	SOD_Play_Object_UserBullet *pbl=SOD_Object_GetUserBullet(pObject);

	pbl->rotation+=pbl->rotationSpeed*elpased/1000;//更新旋转角度

	pObject->x+=pbl->velocity.x*elpased/1000;//依据速度更新位置
	pObject->y+=pbl->velocity.y*elpased/1000;

	//子弹越界(超出地图范围),删除
	if (pObject->x<-SOD_STONE_GEN_SPACE-1||pObject->x>pPlay->world.world_width+SOD_STONE_GEN_SPACE+1||\
		pObject->y<-SOD_STONE_GEN_SPACE-1||pObject->y>pPlay->world.world_height+SOD_STONE_GEN_SPACE+1\
		)
	{
		PX_WorldRemoveObject(&pPlay->world,pObject);

	}
	else
	{
		//生存时间过,删除
		if (pbl->alive>elpased)
		{
			pbl->alive-=elpased;
		}
		else
		{
			//删除子弹
			PX_WorldRemoveObject(&pPlay->world,pObject);

			//创建爆炸效果
			SOD_CreateExplosion(pPlay,PX_POINT(pObject->x,pObject->y,0),1);
		}
	}
}

px_void SOD_Object_UserBulletRender(px_surface *psurface,PX_Object *pObject,px_dword elpased)
{
	SOD_Play_Object_UserBullet *pbl=SOD_Object_GetUserBullet(pObject);
	//渲染子弹
	PX_TextureRenderEx(psurface,pbl->tex,(px_int)pObject->x,(px_int)pObject->y,PX_TEXTURERENDER_REFPOINT_CENTER,PX_NULL,1.0f,pbl->rotation);
}


PX_Object* SOD_Object_UserBulletCreate(SOD_Play *play,px_float x,px_float y,px_float velocity)
{
	PX_Object *pObject;
	SOD_Play_Object_UserBullet *pbl;
	px_point shippt,atom_v;

	pbl=(SOD_Play_Object_UserBullet *)MP_Malloc(&play->runtime->runtime.mp_game,sizeof(SOD_Play_Object_UserBullet));

	if (pbl==PX_NULL)
	{
		PX_ASSERT();
		return PX_NULL;
	}

	pObject=(PX_Object *)PX_ObjectCreate(&play->runtime->runtime.mp_game,PX_NULL,0,0,0,0,0,0);

	if (!pObject)
	{
		MP_Free(&play->runtime->runtime.mp_game,pbl);
		PX_ASSERT();
		return PX_NULL;
	}

	pObject->x=x;
	pObject->y=y;
	pObject->z=SOD_ATOM_Z;
	pObject->Width=48;
	pObject->diameter=48;
	pObject->Height=48;
	pObject->pObject=pbl;
	pObject->Type=SOD_OBJECT_TYPE_USER_ATOM;
	pObject->ReceiveEvents=PX_TRUE;
	pObject->Func_ObjectFree=SOD_Object_UserBulletFree;//回调
	pObject->Func_ObjectUpdate=SOD_Object_UserBulletUpdate;
	pObject->Func_ObjectRender=SOD_Object_UserBulletRender;
	pObject->User_ptr=play;
	pObject->impact_Object_type=SOD_IMPACTTEST_OBJECTTYPE_ENEMY;//碰撞类型
	pObject->impact_test_type=SOD_IMPACTTEST_OBJECTTYPE_USER;//碰撞测试类型

	shippt.x=play->shipObject->x;
	shippt.y=play->shipObject->y;
	shippt.z=0;
	atom_v=PX_PointSub(shippt,PX_POINT(x,y,0));
	pbl->tex=PX_ResourceLibraryGetTexture(&play->runtime->runtime.ResourceLibrary,SOD_KEY_TEX_HUAJI);//绑定纹理
	pbl->velocity=PX_PointMul(PX_PointUnit(atom_v),velocity); 
	pbl->alive=SOD_USER_ATOM_ALIVE_TIME;//生存时间
	pbl->rotationSpeed=360;//初始角速度

	PX_ObjectRegisterEvent(pObject,PX_OBJECT_EVENT_IMPACT,SOD_OnUserBulletImpact,PX_NULL);//处理碰撞事件
	PX_ObjectRegisterEvent(pObject,SOD_OBJECT_EVENT_DAMAGE,SOD_OnUserBulletDamage,PX_NULL);//处理伤害事件
	return pObject;
}

//////////////////////////////////////////////////////////////////////////
//user Object

SOD_Play_Object_UserObject *SOD_Object_GetUserObject(PX_Object *pObject)
{
	return (SOD_Play_Object_UserObject *)pObject->pObject;
}

//处理用户对象伤害事件
px_void SOD_Object_UserObject_OnDamage(PX_Object *pObject,PX_Object_Event e,px_void *ptr)
{
	SOD_Play_Object_UserObject *UserObject=SOD_Object_GetUserObject(pObject);
	SOD_Play *pPlay=(SOD_Play *)pObject->User_ptr;
	if (UserObject->life<=e.Param_int[0])//血扣完了
	{
		UserObject->life=0;
		//删除对象
		PX_WorldRemoveObject(&pPlay->world,pObject);
		//创建爆炸效果
		SOD_CreateExplosion(pPlay,PX_POINT(pObject->x,pObject->y,0),192);
		pPlay->score+=100;
	}
	else
	{
		//扣除血量
		UserObject->life-=e.Param_int[0];
		UserObject->beAttackElpased=0;
	}
}


px_void SOD_Object_UserObjectFree(PX_Object *pObject)
{
	SOD_Play_Object_UserObject *puserObj=SOD_Object_GetUserObject(pObject);
	//释放动画集
	PX_AnimationFree(&puserObj->user_animation);
}

//对象更新
px_void SOD_Object_UserObjectUpdate(PX_Object *pObject,px_dword elpased)
{
	SOD_Play *pPlay=(SOD_Play *)pObject->User_ptr;
	SOD_Play_Object_UserObject *puo=SOD_Object_GetUserObject(pObject);
	px_point shippt,user_v;

	puo->atom_elpased+=elpased;
	//假如飞船还存活
	if (pPlay->shipObject->Visible)
	{
		if (SOD_ALIEN_SEARCH_DISTANCE>PX_PointDistance(PX_POINT(pObject->x,pObject->y,0),PX_POINT(pPlay->shipObject->x,pPlay->shipObject->y,0)))
		{
			//向飞船发射子弹
			if (puo->atom_elpased>1000)
			{
				PX_Object *pfa;
				puo->atom_elpased=0;
				//发射子弹
				pfa=SOD_Object_UserBulletCreate(pPlay,pObject->x,pObject->y,SOD_USER_ATOM_SPEED);
				if (pfa)
				{
					PX_WorldAddObject(&pPlay->world,pfa);
				}
				//播放发射子弹音效
				SOD_PlaySoundFromResource(pPlay->runtime,SOD_KEY_SND_ALIEN_FIRE,PX_FALSE);
			}
		}
		//修正速度,追踪ship
		shippt.x=pPlay->shipObject->x;
		shippt.y=pPlay->shipObject->y;
		shippt.z=0;
		user_v=PX_PointSub(shippt,PX_POINT(pObject->x,pObject->y,0));
		user_v=PX_PointMul(PX_PointUnit(user_v),SOD_USEROBJECT_SPEED);
		puo->velocity=user_v;
	}

	puo->beAttackElpased+=elpased;
	//依据速度更新当前位置
	pObject->x+=puo->velocity.x*elpased/1000;
	pObject->y+=puo->velocity.y*elpased/1000;

	if (pObject->x<-SOD_STONE_GEN_SPACE-1||pObject->x>pPlay->world.world_width+SOD_STONE_GEN_SPACE+1||\
		pObject->y<-SOD_STONE_GEN_SPACE-1||pObject->y>pPlay->world.world_height+SOD_STONE_GEN_SPACE+1\
		)
	{
		//删除对象
		PX_WorldRemoveObject(&pPlay->world,pObject);
	}
	//更新动画信息
	PX_AnimationUpdate(&puo->user_animation,elpased);
}

//渲染对象
px_void SOD_Object_UserObjectRender(px_surface *psurface,PX_Object *pObject,px_dword elpased)
{
	SOD_Play_Object_UserObject *pou=SOD_Object_GetUserObject(pObject);
	if (pou->beAttackElpased<50)
	{
		PX_TEXTURERENDER_BLEND blend;
		blend.alpha=1;
		blend.hdr_B=0;
		blend.hdr_G=0;
		blend.hdr_R=1;
		PX_AnimationRender(psurface,&pou->user_animation,PX_POINT(pObject->x,pObject->y,0),PX_TEXTURERENDER_REFPOINT_CENTER,&blend);
	}
	else
		PX_AnimationRender(psurface,&pou->user_animation,PX_POINT(pObject->x,pObject->y,0),PX_TEXTURERENDER_REFPOINT_CENTER,PX_NULL);

}

PX_Object* SOD_Object_UserObjectCreate(SOD_Play *play,px_int life)
{
	PX_Object *pObject;
	SOD_Play_Object_UserObject *pou;

	pou=(SOD_Play_Object_UserObject *)MP_Malloc(&play->runtime->runtime.mp_game,sizeof(SOD_Play_Object_UserObject));

	if (pou==PX_NULL)
	{
		PX_ASSERT();
		return PX_NULL;
	}

	pObject=(PX_Object *)PX_ObjectCreate(&play->runtime->runtime.mp_game,PX_NULL,0,0,0,0,0,0);

	if (!pObject)
	{
		MP_Free(&play->runtime->runtime.mp_game,pou);
		PX_ASSERT();
		return PX_NULL;
	}

	pObject->x=play->world.world_width/2.0f;
	pObject->y=play->world.world_height/2.0f;
	pObject->z=SOD_ALIEN_Z;
	pObject->Width=96;
	pObject->diameter=96;
	pObject->Height=96;
	pObject->pObject=pou;
	pObject->Type=SOD_OBJECT_TYPE_USEROBJECT;
	pObject->ReceiveEvents=PX_TRUE;
	pObject->Func_ObjectFree=SOD_Object_UserObjectFree;//回调事件
	pObject->Func_ObjectUpdate=SOD_Object_UserObjectUpdate;
	pObject->Func_ObjectRender=SOD_Object_UserObjectRender;
	pObject->User_ptr=play;
	pObject->impact_Object_type=SOD_IMPACTTEST_OBJECTTYPE_ENEMY;//碰撞类型
	pObject->impact_test_type=0;//碰撞测试类型

	pou->beAttackElpased=0;
	pou->life=life;
	//绑定动画集
	PX_AnimationCreate(&pou->user_animation,PX_ResourceLibraryGetAnimationLibrary(&play->runtime->runtime.ResourceLibrary,SOD_KEY_ANIMATION_USERDEF));
	PX_AnimationSetCurrentPlayAnimationByName(&pou->user_animation,SOD_ANIMATION_KEY_USERDEF_NORMAL);
	pou->velocity=PX_POINT(1,0,0);
	pou->atom_elpased=0;

	PX_ObjectRegisterEvent(pObject,SOD_OBJECT_EVENT_DAMAGE,SOD_Object_UserObject_OnDamage,PX_NULL);
	return pObject;
}

//////////////////////////////////////////////////////////////////////////
//ghost

SOD_Play_Object_Ghost *SOD_Object_GetGhost(PX_Object *pObject)
{
	return (SOD_Play_Object_Ghost *)pObject->pObject;
}

px_void SOD_Object_Ghost_OnDamage(PX_Object *pObject,PX_Object_Event e,px_void *ptr)
{
	SOD_Play_Object_Ghost *pgho=SOD_Object_GetGhost(pObject);
	SOD_Play *pPlay=(SOD_Play *)pObject->User_ptr;
	if (pgho->life<=e.Param_int[0])
	{
		pgho->life=0;
		//destory ghost
		PX_WorldRemoveObject(&pPlay->world,pObject);
		SOD_CreateExplosion(pPlay,PX_POINT(pObject->x,pObject->y,0),84);
		SOD_CreateStarWithRandom(pPlay,pObject->x,pObject->y);
		pPlay->score+=50;
	}
	else
	{
		pgho->life-=e.Param_int[0];
		pgho->beAttackElpased=0;
	}
}

px_void SOD_Object_Ghost_OnImpact(PX_Object *pObject,PX_Object_Event e,px_void *ptr)
{
	SOD_Play_Object_Ghost *pgho=SOD_Object_GetGhost(pObject);
	PX_Object *pTarget=(PX_Object *)e.param_ptr[0];
	SOD_Play *pPlay=(SOD_Play *)pObject->User_ptr;
	if (pTarget->Type==SOD_OBJECT_TYPE_SHIP)
	{
		PX_Object_Event e;
		PX_WorldRemoveObject(&pPlay->world,pObject);
		SOD_CreateExplosion(pPlay,PX_POINT(pObject->x,pObject->y,0),84);
		e.Event=SOD_OBJECT_EVENT_DAMAGE;
		e.Param_int[0]=SOD_GHOST_IMPACT_DAMAGE;
		PX_ObjectPostEvent(pTarget,e);
	}
}



px_void SOD_Object_GhostFree(PX_Object *pObject)
{
	SOD_Play_Object_Ghost *pgho=SOD_Object_GetGhost(pObject);
	PX_AnimationFree(&pgho->alien_animation);
}

px_void SOD_Object_GhostUpdate(PX_Object *pObject,px_dword elpased)
{
	SOD_Play *pPlay=(SOD_Play *)pObject->User_ptr;
	SOD_Play_Object_Ghost *pGho=SOD_Object_GetGhost(pObject);

	//假如飞船还存活
	if (pPlay->shipObject->Visible)
	{
		if (SOD_GHOST_SEARCH_DISTANCE>PX_PointDistance(PX_POINT(pObject->x,pObject->y,0),PX_POINT(pPlay->shipObject->x,pPlay->shipObject->y,0)))
		{
			pGho->velocity=PX_PointSub(PX_POINT(pPlay->shipObject->x,pPlay->shipObject->y,0),PX_POINT(pObject->x,pObject->y,0));
			pGho->velocity=PX_PointMul(PX_PointUnit(pGho->velocity),SOD_GHOST_SPEED);//追着飞船跑
			if(pGho->status==SOD_PLAY_GHOST_STATUS_NORMAL)
			{
				PX_AnimationSetCurrentPlayAnimationByName(&pGho->alien_animation,SOD_ANIMATION_KEY_GHOST_ANGRY);
			}
			pGho->status=SOD_PLAY_GHOST_STATUS_ANGRY;
		}
		else
		{
			if(pGho->status==SOD_PLAY_GHOST_STATUS_ANGRY)
			{
			    PX_AnimationSetCurrentPlayAnimationByName(&pGho->alien_animation,SOD_ANIMATION_KEY_GHOST_NORMAL);
			}
			pGho->status=SOD_PLAY_GHOST_STATUS_NORMAL;
		}
	}
	
	pGho->beAttackElpased+=elpased;
	pObject->x+=pGho->velocity.x*elpased/1000;
	pObject->y+=pGho->velocity.y*elpased/1000;


	//Ghost越界,删除这个Ghost
	if (pObject->x<-SOD_STONE_GEN_SPACE-1||pObject->x>pPlay->world.world_width+SOD_STONE_GEN_SPACE+1||\
		pObject->y<-SOD_STONE_GEN_SPACE-1||pObject->y>pPlay->world.world_height+SOD_STONE_GEN_SPACE+1\
		)
	{
		PX_WorldRemoveObject(&pPlay->world,pObject);
	}
	//更新动画信息
	PX_AnimationUpdate(&pGho->alien_animation,elpased);
}

px_void SOD_Object_GhostRender(px_surface *psurface,PX_Object *pObject,px_dword elpased)
{
	SOD_Play_Object_Ghost *pgho=SOD_Object_GetGhost(pObject);
	if (pgho->beAttackElpased<50)
	{
		PX_TEXTURERENDER_BLEND blend;
		blend.alpha=1;
		blend.hdr_B=0;
		blend.hdr_G=0;
		blend.hdr_R=1;
		PX_AnimationRender(psurface,&pgho->alien_animation,PX_POINT(pObject->x,pObject->y,0),PX_TEXTURERENDER_REFPOINT_CENTER,&blend);
	}
	else
		PX_AnimationRender(psurface,&pgho->alien_animation,PX_POINT(pObject->x,pObject->y,0),PX_TEXTURERENDER_REFPOINT_CENTER,PX_NULL);


}

PX_Object* SOD_Object_GhostCreate(SOD_Play *play,px_int life,px_float velocity)
{
	PX_Object *pObject;
	SOD_Play_Object_Ghost *pGho;
	px_point genPostion,genVelocity;

	pGho=(SOD_Play_Object_Ghost *)MP_Malloc(&play->runtime->runtime.mp_game,sizeof(SOD_Play_Object_Ghost));

	if (pGho==PX_NULL)
	{
		PX_ASSERT();
		return PX_NULL;
	}

	pObject=(PX_Object *)PX_ObjectCreate(&play->runtime->runtime.mp_game,PX_NULL,0,0,0,0,0,0);

	if (!pObject)
	{
		PX_ASSERT();
		MP_Free(&play->runtime->runtime.mp_game,pGho);
		return PX_NULL;
	}
	
	SOD_GenInitializePositionAndVelocity(play,&genPostion,&genVelocity,velocity);

	pObject->x=genPostion.x;
	pObject->y=genPostion.y;
	pObject->z=SOD_GHOST_Z;
	pObject->Width=84;
	pObject->diameter=84;
	pObject->Height=84;
	pObject->pObject=pGho;
	pObject->Type=SOD_OBJECT_TYPE_GHOST;
	pObject->ReceiveEvents=PX_TRUE;
	pObject->Func_ObjectFree=SOD_Object_GhostFree;
	pObject->Func_ObjectUpdate=SOD_Object_GhostUpdate;
	pObject->Func_ObjectRender=SOD_Object_GhostRender;
	pObject->User_ptr=play;
	pObject->impact_Object_type=SOD_IMPACTTEST_OBJECTTYPE_ENEMY;
	pObject->impact_test_type=SOD_IMPACTTEST_OBJECTTYPE_USER;

	pGho->beAttackElpased=0;
	pGho->life=life;
	PX_AnimationCreate(&pGho->alien_animation,PX_ResourceLibraryGetAnimationLibrary(&play->runtime->runtime.ResourceLibrary,SOD_KEY_ANIMATION_GHOST));
	PX_AnimationSetCurrentPlayAnimationByName(&pGho->alien_animation,SOD_ANIMATION_KEY_GHOST_NORMAL);
	pGho->velocity=genVelocity;
	pGho->status=SOD_PLAY_GHOST_STATUS_NORMAL;
	
	PX_ObjectRegisterEvent(pObject,PX_OBJECT_EVENT_IMPACT,SOD_Object_Ghost_OnImpact,PX_NULL);
	PX_ObjectRegisterEvent(pObject,SOD_OBJECT_EVENT_DAMAGE,SOD_Object_Ghost_OnDamage,PX_NULL);
	return pObject;
}
//////////////////////////////////////////////////////////////////////////
//stone

SOD_Play_Object_Stone *SOD_Object_GetStone(PX_Object *pObject)
{
	return (SOD_Play_Object_Stone *)pObject->pObject;
}

px_void SOD_Object_Stone_OnDamage(PX_Object *pObject,PX_Object_Event e,px_void *ptr)
{
	SOD_Play_Object_Stone *pst=SOD_Object_GetStone(pObject);
	SOD_Play *pPlay=(SOD_Play *)pObject->User_ptr;
	if (pst->life<=e.Param_int[0])
	{
		pst->life=0;
		//destory stone
		PX_WorldRemoveObject(&pPlay->world,pObject);
		SOD_CreateExplosion(pPlay,PX_POINT(pObject->x,pObject->y,0),pObject->diameter*pst->scale);
		pPlay->score+=100;
		SOD_CreateStarWithRandom(pPlay,pObject->x,pObject->y);
	}
	else
	{
		pst->life-=e.Param_int[0];
		pst->beAttackElpased=0;
	}
}

px_void SOD_Object_Stone_OnImpact(PX_Object *pObject,PX_Object_Event e,px_void *ptr)
{
	SOD_Play_Object_Stone *pst=SOD_Object_GetStone(pObject);
	PX_Object *pTarget=(PX_Object *)e.param_ptr[0];
	SOD_Play *pPlay=(SOD_Play *)pObject->User_ptr;
	if (pTarget->Type==SOD_OBJECT_TYPE_SHIP)
	{
		PX_Object_Event e;

		PX_WorldRemoveObject(&pPlay->world,pObject);
		SOD_CreateExplosion(pPlay,PX_POINT(pObject->x,pObject->y,0),pObject->diameter*pst->scale);
		e.Event=SOD_OBJECT_EVENT_DAMAGE;
		e.Param_int[0]=SOD_STONE_IMPACT_DAMAGE;
		PX_ObjectPostEvent(pTarget,e);
	}
}



px_void SOD_Object_StoneFree(PX_Object *pObject)
{

}

px_void SOD_Object_StoneUpdate(PX_Object *pObject,px_dword elpased)
{
	SOD_Play *pPlay=(SOD_Play *)pObject->User_ptr;

	SOD_Play_Object_Stone *pst=SOD_Object_GetStone(pObject);

	pst->beAttackElpased+=elpased;
	
	pst->rotation+=pst->rotationSpeed*elpased/1000;
	pObject->x+=pst->velocity.x*elpased/1000;
	pObject->y+=pst->velocity.y*elpased/1000;

	//陨石越界,删除这个陨石
	if (pObject->x<-SOD_STONE_GEN_SPACE-1||pObject->x>pPlay->world.world_width+SOD_STONE_GEN_SPACE+1||\
		pObject->y<-SOD_STONE_GEN_SPACE-1||pObject->y>pPlay->world.world_height+SOD_STONE_GEN_SPACE+1\
		)
	{
		PX_WorldRemoveObject(&pPlay->world,pObject);

	}

}

px_void SOD_Object_StoneRender(px_surface *psurface,PX_Object *pObject,px_dword elpased)
{
	SOD_Play_Object_Stone *pst=SOD_Object_GetStone(pObject);
	if (pst->beAttackElpased<50)
	{
		PX_TEXTURERENDER_BLEND blend;
		blend.alpha=1;
		blend.hdr_B=0;
		blend.hdr_G=0;
		blend.hdr_R=1;
		PX_TextureRenderEx(psurface,pst->pTexture,(px_int)pObject->x,(px_int)pObject->y,PX_TEXTURERENDER_REFPOINT_CENTER,&blend,pst->scale,pst->rotation);
	}
	else
	PX_TextureRenderEx(psurface,pst->pTexture,(px_int)pObject->x,(px_int)pObject->y,PX_TEXTURERENDER_REFPOINT_CENTER,PX_NULL,pst->scale,pst->rotation);



	if (pst->show_impact_region)
	{
		
		PX_GeoDrawSolidCircle(psurface,(px_int)pObject->x,(px_int)pObject->y,(px_int)(pObject->diameter/2),PX_COLOR(128,255,0,0));
	}
}

PX_Object* SOD_Object_StoneCreate(SOD_Play *play,px_int life,px_float velocity)
{
	PX_Object *pObject;
	px_texture *pTexture;
	SOD_Play_Object_Stone *pst;
	px_float scale;
	px_point genPostion,genVelocity;


	switch(PX_rand()%3)
	{
	case 0:
		pTexture=PX_ResourceLibraryGetTexture(&play->runtime->runtime.ResourceLibrary,SOD_KEY_TEX_STONE1);
		break;
	case 1:
		pTexture=PX_ResourceLibraryGetTexture(&play->runtime->runtime.ResourceLibrary,SOD_KEY_TEX_STONE2);
		break;
	case 2:
		pTexture=PX_ResourceLibraryGetTexture(&play->runtime->runtime.ResourceLibrary,SOD_KEY_TEX_STONE3);
		break;
	default:
		PX_ASSERT();
	}
	

	if (!pTexture)
	{
		PX_ASSERT();
		return PX_NULL;
	}

	pst=(SOD_Play_Object_Stone *)MP_Malloc(&play->runtime->runtime.mp_game,sizeof(SOD_Play_Object_Stone));

	if (pst==PX_NULL)
	{
		PX_ASSERT();
		return PX_NULL;
	}

	pObject=(PX_Object *)PX_ObjectCreate(&play->runtime->runtime.mp_game,PX_NULL,0,0,0,0,0,0);

	if (!pObject)
	{
		MP_Free(&play->runtime->runtime.mp_game,pst);
		PX_ASSERT();
		return PX_NULL;
	}
	//生成陨石的初始大小
	scale=0.5f+(px_int)(PX_rand()%1000)/2000.f;
	//生成陨石的初始位置和初始速度向量
	SOD_GenInitializePositionAndVelocity(play,&genPostion,&genVelocity,velocity);

	pObject->x=genPostion.x;
	pObject->y=genPostion.y;
	pObject->z=SOD_STONE_Z;
	pObject->Width=pTexture->width*scale;
	pObject->diameter=(pTexture->width*scale);
	pObject->Height=pTexture->height*scale;
	pObject->pObject=pst;
	pObject->Type=SOD_OBJECT_TYPE_STONE;
	pObject->ReceiveEvents=PX_TRUE;
	pObject->Func_ObjectFree=SOD_Object_StoneFree;
	pObject->Func_ObjectUpdate=SOD_Object_StoneUpdate;
	pObject->Func_ObjectRender=SOD_Object_StoneRender;
	pObject->User_ptr=play;
	pObject->impact_Object_type=SOD_IMPACTTEST_OBJECTTYPE_ENEMY;
	pObject->impact_test_type=SOD_IMPACTTEST_OBJECTTYPE_USER;

	pst->show_impact_region=PX_FALSE;
	pst->beAttackElpased=0;
	pst->life=life;
	pst->pTexture=pTexture;
	pst->rotation=0;
	pst->rotationSpeed=(px_float)(SOD_STONE_DEFALUE_MIN_ROTATION_SPEED+(PX_rand()%(SOD_STONE_DEFALUE_MAX_ROTATION_SPEED-SOD_STONE_DEFALUE_MIN_ROTATION_SPEED)));
	pst->scale=scale;
	pst->velocity=genVelocity;

	PX_ObjectRegisterEvent(pObject,PX_OBJECT_EVENT_IMPACT,SOD_Object_Stone_OnImpact,PX_NULL);
	PX_ObjectRegisterEvent(pObject,SOD_OBJECT_EVENT_DAMAGE,SOD_Object_Stone_OnDamage,PX_NULL);
	return pObject;
}

//////////////////////////////////////////////////////////////////////////
//ship fire atom
px_void SOD_OnFireAtomImpact(PX_Object *pObj,PX_Object_Event e,px_void *ptr)
{
	PX_Object_Event pe;
	SOD_Play *pPlay=(SOD_Play *)pObj->User_ptr;
	PX_WorldRemoveObject(&pPlay->world,pObj);
	SOD_CreateExplosion(pPlay,PX_POINT(pObj->x,pObj->y,0),10);
	pe.Event=SOD_OBJECT_EVENT_DAMAGE;
	pe.Param_int[0]=100;
	PX_ObjectPostEvent((PX_Object *)e.param_ptr[0],pe);
}

SOD_Play_Object_ShipFireAtom *SOD_Object_GetShipFireAtom(PX_Object *pObject)
{
	return (SOD_Play_Object_ShipFireAtom *)pObject->pObject;
}

px_void SOD_Object_ShipFireAtomFree(PX_Object *pObject)
{

}

px_void SOD_Object_ShipFireAtomUpdate(PX_Object *pObject,px_dword elpased)
{
	SOD_Play *pPlay=(SOD_Play *)pObject->User_ptr;
	SOD_Play_Object_ShipFireAtom *pfa=SOD_Object_GetShipFireAtom(pObject);
	pObject->x+=pfa->velocity.x*elpased/1000;
	pObject->y+=pfa->velocity.y*elpased/1000;
	if (pfa->alive>elpased)
	{
		pfa->alive-=elpased;
	}
	else
	{
		PX_WorldRemoveObject(&pPlay->world,pObject);
	}

}

px_void SOD_Object_ShipFireAtomRender(px_surface *psurface,PX_Object *pObject,px_dword elpased)
{
	SOD_Play_Object_ShipFireAtom *pfa=SOD_Object_GetShipFireAtom(pObject);
	PX_TextureRenderRotation_vector(psurface,pfa->tex,(px_int)pObject->x,(px_int)pObject->y,PX_TEXTURERENDER_REFPOINT_CENTER,PX_NULL,pfa->velocity);
	if (pfa->show_impact_region)
	{
		PX_GeoDrawSolidCircle(psurface,(px_int)pObject->x,(px_int)pObject->y,(px_int)pObject->diameter/2,PX_COLOR(128,255,0,0));
	}
}

PX_Object* SOD_Object_ShipFireAtomCreate(SOD_Play *play)
{
	PX_Object *pObject;
	px_texture *pTexture;
	SOD_Play_Object_ShipFireAtom *pfa;

	pTexture=PX_ResourceLibraryGetTexture(&play->runtime->runtime.ResourceLibrary,SOD_KEY_TEX_SHIP_ATOM);

	if (!pTexture)
	{
		PX_ASSERT();
		return PX_NULL;
	}

	pfa=(SOD_Play_Object_ShipFireAtom *)MP_Malloc(&play->runtime->runtime.mp_game,sizeof(SOD_Play_Object_ShipFireAtom));

	if (pfa==PX_NULL)
	{
		PX_ASSERT();
		return PX_NULL;
	}

	pObject=(PX_Object *)PX_ObjectCreate(&play->runtime->runtime.mp_game,PX_NULL,0,0,0,0,0,0);

	if (!pObject)
	{
		MP_Free(&play->runtime->runtime.mp_game,pfa);
		return PX_NULL;
	}

	pObject->x=0;
	pObject->y=0;
	pObject->z=SOD_ATOM_Z;
	pObject->Width=10;
	pObject->Height=10;
	pObject->diameter=10;


	pObject->pObject=pfa;
	pObject->Type=SOD_OBJECT_TYPE_SHIP_ATOM;
	pObject->ReceiveEvents=PX_TRUE;
	pObject->Func_ObjectFree=SOD_Object_ShipFireAtomFree;
	pObject->Func_ObjectUpdate=SOD_Object_ShipFireAtomUpdate;
	pObject->Func_ObjectRender=SOD_Object_ShipFireAtomRender;
	pObject->User_ptr=play;
	pObject->impact_test_type=SOD_IMPACTTEST_OBJECTTYPE_ENEMY;

	pfa->alive=SOD_FIRE_SHIP_ATOM_ALIVE_TIME;
	pfa->tex=pTexture;
	pfa->show_impact_region=PX_FALSE;

	PX_ObjectRegisterEvent(pObject,PX_OBJECT_EVENT_IMPACT,SOD_OnFireAtomImpact,play);

	return pObject;
}




//飞船开火执行函数
px_void SOD_ShipFire(SOD_Play *pPlay)
{
	PX_Object *pObj;
	SOD_Play_Object_Ship *pShip=SOD_Object_GetShip(pPlay->shipObject);
	SOD_Play_Object_ParitcalLauncher *pfire;
	SOD_Play_Object_ShipFireAtom *pAtom;
	PX_ParticalLauncher_InitializeInfo partical_info;
	partical_info.Create_func=SOD_ParticalLauncher_ShipFire_CreatePartical;
	partical_info.force=PX_POINT(0,0,0);
	partical_info.generateDuration=0;
	partical_info.launchCount=30;
	partical_info.maxCount=30;
	partical_info.resistanceK=0;
	partical_info.tex=PX_ResourceLibraryGetTexture(&pPlay->runtime->runtime.ResourceLibrary,SOD_KEY_PARTICAL_FIRE_ASH);
	partical_info.Update_func=PX_NULL;

	//创建飞船左边的火花粒子
	pObj=SOD_Object_ParticalLauncherCreate(pPlay,&pPlay->fire_mp,partical_info,SOD_FIRE_PARTICAL_ALIVE_TIME);
	
	if (pObj)
	{
		px_point pt;
		pfire=SOD_Object_GetParticalLauncher(pObj);
		if (PX_WorldAddObject(&pPlay->world,pObj))
		{
			pt=PX_PointReflectX(pShip->direction,pShip->cannon_point1);
			pt.x+=pPlay->shipObject->x;
			pt.y+=pPlay->shipObject->y;
			//设置粒子位置
			pfire->launcher.direction=pShip->direction;
			pfire->launcher.launcherVelocity=pShip->velocity;
			pfire->launcher.launcherPosition=pt;
		}
	}

	
	pObj=SOD_Object_ParticalLauncherCreate(pPlay,&pPlay->fire_mp,partical_info,SOD_FIRE_PARTICAL_ALIVE_TIME);
	if (pObj)
	{
		px_point pt;
		pfire=SOD_Object_GetParticalLauncher(pObj);
		if (PX_WorldAddObject(&pPlay->world,pObj))
		{
			pt=PX_PointReflectX(pShip->direction,pShip->cannon_point2);
			pt.x+=pPlay->shipObject->x;
			pt.y+=pPlay->shipObject->y;
			//设置粒子位置
			pfire->launcher.direction=pShip->direction;
			pfire->launcher.launcherVelocity=pShip->velocity;
			pfire->launcher.launcherPosition=pt;      
		}
	}


	//创建子弹
	
	pObj=SOD_Object_ShipFireAtomCreate(pPlay);
	if (pObj)
	{
		px_point pt;
		pAtom=SOD_Object_GetShipFireAtom(pObj);

		if (PX_WorldAddObject(&pPlay->world,pObj))
		{
			pt=PX_PointReflectX(pShip->direction,pShip->cannon_point1);
			pt.x+=pPlay->shipObject->x;
			pt.y+=pPlay->shipObject->y;
			//设置子弹初始位置
			pObj->x=pt.x;pObj->y=pt.y;
			pAtom->velocity=PX_PointMul(PX_PointUnit(pShip->direction),SOD_DEFAULT_ATOM_SPEED);
			pAtom->velocity=PX_PointAdd(pAtom->velocity,pShip->velocity);
		}
	}

	pObj=SOD_Object_ShipFireAtomCreate(pPlay);
	if (pObj)
	{
		px_point pt;
		pAtom=SOD_Object_GetShipFireAtom(pObj);

		if (PX_WorldAddObject(&pPlay->world,pObj))
		{
			pt=PX_PointReflectX(pShip->direction,pShip->cannon_point2);
			pt.x+=pPlay->shipObject->x;
			pt.y+=pPlay->shipObject->y;
			//设置子弹初始位置
			pObj->x=pt.x;pObj->y=pt.y;
			pAtom->velocity=PX_PointMul(PX_PointUnit(pShip->direction),SOD_DEFAULT_ATOM_SPEED);
			pAtom->velocity=PX_PointAdd(pAtom->velocity,pShip->velocity);
		}
	}

	//播放音效

	SOD_PlaySoundFromResource(pPlay->runtime,SOD_KEY_SOUND_SHIP_FIRE,PX_FALSE);
}

//
px_void SOD_Object_ShipOnDamage(PX_Object *pObject,PX_Object_Event e,px_void *ptr)
{
	SOD_Play *pPlay=(SOD_Play *)pObject->User_ptr;
	SOD_Play_Object_Ship *pShip=SOD_Object_GetShip(pObject);
	if (pShip->shield)//使用护盾抵消伤害
	{
		pShip->shield=PX_FALSE;
		return;
	}

	if (pShip->life>e.Param_int[0])
	{
		pShip->life-=e.Param_int[0];
	}
	else
	{
		pShip->life=0;
		pObject->Enabled=PX_FALSE;
		pObject->Visible=PX_FALSE;
		pObject->impact_Object_type=0;
		pObject->impact_test_type=0;
		pPlay->ship_powerpartical_l1->Visible=PX_FALSE;
		pPlay->ship_powerpartical_l2->Visible=PX_FALSE;
		pPlay->ship_powerpartical_main->Visible=PX_FALSE;
		pPlay->gameover_waiting=3000;
		SOD_CreateExplosion(pPlay,PX_POINT(pObject->x,pObject->y,0),84);
	}
}


//取得飞船对象数据
SOD_Play_Object_Ship *SOD_Object_GetShip(PX_Object *pObject)
{
	if (pObject->Type==SOD_OBJECT_TYPE_SHIP)
	{
		return (SOD_Play_Object_Ship *)pObject->pObject;
	}
	PX_ASSERT();
	return PX_NULL;
}

//释放飞船资源
px_void SOD_Object_ShipFree( PX_Object *pObject )
{
}

px_void SOD_Object_ShipUpdate(PX_Object *pObject,px_uint elpased)
{
	px_float f_distance;
	px_point power_direction;
	px_point blend_point;
	SOD_Play *play;
	SOD_Play_Object_Ship *pShip=SOD_Object_GetShip(pObject);

	if (!pShip)
	{
		PX_ASSERT();
		return;
	}

	play=(SOD_Play *)pObject->User_ptr;

	pShip->direction.x=play->control.mouse_x-(pObject->x-play->world.offsetx);
	pShip->direction.y=play->control.mouse_y-(pObject->y-play->world.offsety);
	pShip->direction.z=0;


	//sync direction
	//同步受力和方向
	f_distance=PX_PointMod(pShip->direction);
	if (f_distance>SOD_SHIP_FORCE_MAX_DISTANCE)
	{
		f_distance=SOD_SHIP_FORCE_MAX_DISTANCE;
	}
	pShip->force=PX_PointMul(PX_PointUnit(pShip->direction),f_distance/SOD_SHIP_FORCE_MAX_DISTANCE*pShip->max_force);

	
	//unit
	
	//依据受力更新速度,飞船的质量为1
	pShip->velocity.x+=pShip->force.x*elpased/1000;
	pShip->velocity.y+=pShip->force.y*elpased/1000;
	pShip->velocity.z=0;

	//判断速度是否大于最大速度
	if (PX_PointMod(pShip->velocity)>pShip->max_speed)
	{
		pShip->velocity=PX_PointMul(PX_PointUnit(pShip->velocity),pShip->max_speed);
	}

	//更新飞船的位置
	if (pObject->x+pShip->velocity.x*elpased/1000<0)
	{
		pShip->velocity.x/=-2;
	}
	else if (pObject->x+pShip->velocity.x*elpased/1000>play->world.world_width)
	{
		pShip->velocity.x/=-2;
	}
	else
	{
		pObject->x+=pShip->velocity.x*elpased/1000;
	}

	if (pObject->y+pShip->velocity.y*elpased/1000<0)
	{
		pShip->velocity.y/=-2;
	}
	else if (pObject->y+pShip->velocity.y*elpased/1000>play->world.world_height)
	{
		pShip->velocity.y/=-2;
	}
	else
	pObject->y+=pShip->velocity.y*elpased/1000;

	pObject->z=SOD_SHIP_Z;

	//设置飞船速度
	SOD_Ship_Speed=PX_PointMod(pShip->velocity);
	SOD_Ship_Force=PX_PointMod(pShip->force);
	//绑定喷射粒子系统
	power_direction=PX_PointInverse(pShip->direction);


	//主推动粒子系统设定
	blend_point=PX_PointReflectX(pShip->direction,pShip->power_main_point);
	blend_point.x+=pObject->x;
	blend_point.y+=pObject->y;
	PX_Object_GetPartical(play->ship_powerpartical_main)->launcher.direction=power_direction;
	PX_Object_GetPartical(play->ship_powerpartical_main)->launcher.launcherPosition=blend_point;
	PX_Object_GetPartical(play->ship_powerpartical_main)->launcher.launcherVelocity=pShip->velocity;
	PX_Object_GetPartical(play->ship_powerpartical_main)->launcher.generateDuration=(px_dword)(20.f-18.f*SOD_Ship_Force/pShip->max_force);

	//副推动粒子系统设定
	blend_point=PX_PointReflectX(pShip->direction,pShip->power_assist_L_point);
	blend_point.x+=pObject->x;
	blend_point.y+=pObject->y;
	PX_Object_GetPartical(play->ship_powerpartical_l1)->launcher.direction=power_direction;
	PX_Object_GetPartical(play->ship_powerpartical_l1)->launcher.launcherPosition=blend_point;
	PX_Object_GetPartical(play->ship_powerpartical_l1)->launcher.launcherVelocity=pShip->velocity;
	PX_Object_GetPartical(play->ship_powerpartical_l1)->launcher.generateDuration=(px_dword)(30.f-26.f*SOD_Ship_Force/pShip->max_force);
	//副推动粒子系统设定
	blend_point=PX_PointReflectX(pShip->direction,pShip->power_assist_R_point);
	blend_point.x+=pObject->x;
	blend_point.y+=pObject->y;
	PX_Object_GetPartical(play->ship_powerpartical_l2)->launcher.direction=power_direction;
	PX_Object_GetPartical(play->ship_powerpartical_l2)->launcher.launcherPosition=blend_point;
	PX_Object_GetPartical(play->ship_powerpartical_l2)->launcher.launcherVelocity=pShip->velocity;
	PX_Object_GetPartical(play->ship_powerpartical_l2)->launcher.generateDuration=(px_dword)(30.f-26.f*SOD_Ship_Force/pShip->max_force);

	//武器发射更新
	pShip->fire_elpased+=elpased;
	if (play->control.mouse_ldown)
	{
		//检查子弹库存
		if (pShip->atomcount>0)
		{
			if (pShip->fire_elpased>pShip->fire_duration)
			{
				pShip->atomcount--;
				pShip->fire_elpased=0;
				SOD_ShipFire(play);
			}
		}
		
	}
}
//渲染飞船对象
px_void SOD_Object_ShipRender(px_surface *psurface, PX_Object *pObject,px_uint elpased)
{
	SOD_Play *play;
	SOD_Play_Object_Ship *pShip=SOD_Object_GetShip(pObject);
	play=(SOD_Play *)pObject->User_ptr;

	//绘制辅助线
	if (pShip->showHelpLine)
	{
		px_float x0,y0,x1,y1;
		x0=pObject->x;
		y0=pObject->y;
		x1=x0+pShip->direction.x;
		y1=y0+pShip->direction.y;
		PX_GeoDrawLine(psurface,(px_int)x0,(px_int)y0,(px_int)x1,(px_int)y1,1,PX_COLOR(128,0,192,255));
	}

	//渲染飞船船体,旋转特定的方向
	if (PX_PointSquare(pShip->direction))
	{
		PX_TextureRenderRotation_vector(psurface,pShip->shipObject,(px_int)pObject->x,(px_int)pObject->y,PX_TEXTURERENDER_REFPOINT_CENTER,PX_NULL,pShip->direction);
	}
	else
	{
		PX_TextureRender(psurface,pShip->shipObject,(px_int)pObject->x,(px_int)pObject->y,PX_TEXTURERENDER_REFPOINT_CENTER,PX_NULL);
	}

	//渲染文本
	pShip->Buffer_Text_elpased+=elpased;
	if (pShip->Buffer_Text_elpased<3000)
	{
		px_uchar a;
		a=(px_uchar)(255*pShip->Buffer_Text_elpased/3000);
		if(pShip->Buffer_Text)
		PX_FontDrawText(psurface,(px_int)pObject->x-12,(px_int)pObject->y-48,pShip->Buffer_Text,PX_COLOR(255,0,0,255),PX_FONT_ALIGN_XLEFT);
	}

	//渲染混沌
	if (pShip->shield)
	{
		PX_TextureRender(psurface,pShip->shieldtex,(px_int)pObject->x,(px_int)pObject->y,PX_TEXTURERENDER_REFPOINT_CENTER,PX_NULL);
	}
	
}

px_void SOD_Object_OnHealth(PX_Object *pObj,PX_Object_Event e,px_void *ptr)
{
	SOD_Play *pPlay=(SOD_Play *)pObj->User_ptr;
	SOD_Play_Object_Ship *pShip=SOD_Object_GetShip(pObj);
	if (pShip->life==SOD_SHIP_DEFAULT_MAX_LIFE)
	{
		pShip->Buffer_Text=SOD_TEXT_ADD_SCORE;
		pShip->Buffer_Text_elpased=0;
		pPlay->score+=100;
	}
	else
	{
		if (pShip->life+e.Param_int[0]>SOD_SHIP_DEFAULT_MAX_LIFE)
		{
			pShip->life=SOD_SHIP_DEFAULT_MAX_LIFE;
		}
		else
		{
			pShip->life+=e.Param_int[0];
		}
		pShip->Buffer_Text=SOD_TEXT_HEALTH;
		pShip->Buffer_Text_elpased=0;
	}
}

px_void SOD_Object_OnAddAtom(PX_Object *pObj,PX_Object_Event e,px_void *ptr)
{
	SOD_Play *pPlay=(SOD_Play *)pObj->User_ptr;
	SOD_Play_Object_Ship *pShip=SOD_Object_GetShip(pObj);
	if (pShip->atomcount==SOD_SHIP_DEFAULT_MAX_ATOM)
	{
		pShip->Buffer_Text=SOD_TEXT_ADD_SCORE;
		pShip->Buffer_Text_elpased=0;
		pPlay->score+=100;
	}
	else
	{
		if (pShip->atomcount+e.Param_int[0]>SOD_SHIP_DEFAULT_MAX_ATOM)
		{
			pShip->atomcount=SOD_SHIP_DEFAULT_MAX_ATOM;
		}
		else
		{
			pShip->atomcount+=e.Param_int[0];
		}
		pShip->Buffer_Text=SOD_TEXT_ADD_ATOM;
		pShip->Buffer_Text_elpased=0;
	}
}

px_void SOD_Object_OnForceup(PX_Object *pObj,PX_Object_Event e,px_void *ptr)
{
	SOD_Play *pPlay=(SOD_Play *)pObj->User_ptr;
	SOD_Play_Object_Ship *pShip=SOD_Object_GetShip(pObj);
	if (pShip->max_force==SOD_SHIP_DEFAULT_MAX_UPDATE_FORCE)
	{
		pShip->Buffer_Text=SOD_TEXT_ADD_SCORE;
		pShip->Buffer_Text_elpased=0;
		pPlay->score+=100;
	}
	else
	{
		if (pShip->max_force+e.Param_int[0]>SOD_SHIP_DEFAULT_MAX_UPDATE_FORCE)
		{
			pShip->max_force=SOD_SHIP_DEFAULT_MAX_UPDATE_FORCE;
		}
		else
		{
			pShip->max_force+=e.Param_int[0];
		}
		pShip->Buffer_Text=SOD_TEXT_FORCE_UP;
		pShip->Buffer_Text_elpased=0;
	}
}

px_void SOD_Object_OnWeaponUp(PX_Object *pObj,PX_Object_Event e,px_void *ptr)
{
	SOD_Play *pPlay=(SOD_Play *)pObj->User_ptr;
	SOD_Play_Object_Ship *pShip=SOD_Object_GetShip(pObj);
	if (pShip->fire_duration==SOD_MIN_FIRE_DURATION)
	{
		pShip->Buffer_Text=SOD_TEXT_ADD_SCORE;
		pShip->Buffer_Text_elpased=0;
		pPlay->score+=100;
	}
	else
	{
		if (pShip->max_force-e.Param_int[0]<SOD_MIN_FIRE_DURATION)
		{
			pShip->max_force=SOD_MIN_FIRE_DURATION;
		}
		else
		{
			pShip->max_force-=e.Param_int[0];
		}
		pShip->Buffer_Text=SOD_TEXT_WEAPON_UP;
		pShip->Buffer_Text_elpased=0;
	}
}

px_void SOD_Object_OnShield(PX_Object *pObj,PX_Object_Event e,px_void *ptr)
{
	SOD_Play *pPlay=(SOD_Play *)pObj->User_ptr;
	SOD_Play_Object_Ship *pShip=SOD_Object_GetShip(pObj);
	if (pShip->shield)
	{
		pShip->Buffer_Text=SOD_TEXT_ADD_SCORE;
		pShip->Buffer_Text_elpased=0;
		pPlay->score+=100;
	}
	else
	{
		pShip->shield=PX_TRUE;
		pShip->Buffer_Text=SOD_TEXT_SHIELD;
		pShip->Buffer_Text_elpased=0;
	}
}

//创建飞船对象
PX_Object* SOD_Object_ShipCreate(SOD_Play *play)
{
	PX_Object *pObject;
	px_float x,y,width,height;
	px_texture *pTexture;
	SOD_Play_Object_Ship *pShip;

	x=(px_float)play->world.world_width/2;
	y=(px_float)play->world.world_height/2;
	pTexture=PX_ResourceLibraryGetTexture(&play->runtime->runtime.ResourceLibrary,SOD_KEY_TEX_SHIP);

	if (!pTexture)
	{
		PX_ASSERT();
		return PX_NULL;
	}

	width=(px_float)pTexture->width;
	height=(px_float)pTexture->height;

	pShip=(SOD_Play_Object_Ship *)MP_Malloc(&play->runtime->runtime.mp_game,sizeof(SOD_Play_Object_Ship));
	if (pShip==PX_NULL)
	{
		PX_ASSERT();
		return PX_NULL;
	}

	pObject=(PX_Object *)PX_ObjectCreate(&play->runtime->runtime.mp_game,PX_NULL,0,0,0,0,0,0);
	
	if (!pObject)
	{
		PX_ASSERT();
		MP_Free(&play->runtime->runtime.mp_game,pShip);
		return PX_NULL;
	}
	pObject->x=x;
	pObject->y=y;
	pObject->z=SOD_SHIP_Z;
	pObject->Width=width;
	pObject->Height=height;


	pObject->pObject=pShip;
	pObject->Type=SOD_OBJECT_TYPE_SHIP;
	pObject->ReceiveEvents=PX_TRUE;
	pObject->Func_ObjectFree=SOD_Object_ShipFree;
	pObject->Func_ObjectUpdate=SOD_Object_ShipUpdate;
	pObject->Func_ObjectRender=SOD_Object_ShipRender;
	pObject->diameter=48;
	pObject->User_ptr=play;
	pObject->impact_Object_type=SOD_IMPACTTEST_OBJECTTYPE_USER;


	pShip->max_life=SOD_SHIP_DEFAULT_MAX_LIFE;
	pShip->life=SOD_SHIP_DEFAULT_MAX_LIFE;
	pShip->force=PX_POINT(0,0,0);
	pShip->direction=PX_POINT(0,0,0);
	pShip->max_force=SOD_SHIP_DEFAULT_MAX_FORCE;
	pShip->max_speed=SOD_SHIP_DEFAULT_MAX_SPEED;
	pShip->shipObject=pTexture;
	pShip->velocity=PX_POINT(0,0,0);
	pShip->showHelpLine=PX_TRUE;
	pShip->fire_duration=SOD_SHIP_DEFAULT_FIRE_DURATION;
	pShip->fire_elpased=0;
	pShip->atomcount=SOD_SHIP_DEFAULT_MAX_ATOM;
	pShip->max_atomcount=SOD_SHIP_DEFAULT_MAX_ATOM;
	pShip->Buffer_Text=PX_NULL;
	pShip->Buffer_Text_elpased=0;
	pShip->shield=PX_FALSE;
	pShip->shieldtex=PX_ResourceLibraryGetTexture(&play->runtime->runtime.ResourceLibrary,SOD_KEY_TEX_SHIELD);
	pShip->power_main_point=PX_POINT(-30,0,0);
	pShip->power_assist_L_point=PX_POINT(-28,-12,0);
	pShip->power_assist_R_point=PX_POINT(-28,12,0);

	pShip->cannon_point1=PX_POINT(2,-12,0);
	pShip->cannon_point2=PX_POINT(2,12,0);

	PX_ObjectRegisterEvent(pObject,SOD_OBJECT_EVENT_DAMAGE,SOD_Object_ShipOnDamage,PX_NULL);
	PX_ObjectRegisterEvent(pObject,SOD_OBJECT_EVENT_HEALTH,SOD_Object_OnHealth,PX_NULL);
	PX_ObjectRegisterEvent(pObject,SOD_OBJECT_EVENT_ATOM,SOD_Object_OnAddAtom,PX_NULL);
	PX_ObjectRegisterEvent(pObject,SOD_OBJECT_EVENT_ADDWEAPON,SOD_Object_OnWeaponUp,PX_NULL);
	PX_ObjectRegisterEvent(pObject,SOD_OBJECT_EVENT_ADDFORCE,SOD_Object_OnForceup,PX_NULL);
	PX_ObjectRegisterEvent(pObject,SOD_OBJECT_EVENT_SHIELD,SOD_Object_OnShield,PX_NULL);
	return pObject;
}

//////////////////////////////////////////////////////////////////////////
px_bool SOD_PlayInitialize(SOD_Runtime *runtime,SOD_Play *pPlay)
{
	pPlay->fps_frame=0;
	pPlay->fps_elpased=0;
	pPlay->fps=0;
	pPlay->runtime=runtime;
	pPlay->shipObject=PX_NULL;
	pPlay->control.mouse_x=0;
	pPlay->control.mouse_y=0;
	pPlay->gametime=0;
	pPlay->score=0;
	pPlay->updatetime_reserved=0;

	pPlay->root=PX_ObjectCreate(&pPlay->runtime->runtime.mp_ui,0,0,0,0,0,0,0);
	pPlay->shipObject=PX_NULL;

	//加载用户自建动画
	if (!PX_LoadAnimationToResource(&runtime->runtime,SOD_PATH_ANIMATION_USERDEF,SOD_KEY_ANIMATION_USERDEF))
	{
		return PX_FALSE;
	}

	//加载用户自建纹理
	if (!PX_LoadTextureToResource(&runtime->runtime,SOD_PATH_HUAJI,SOD_KEY_TEX_HUAJI))
	{
		return PX_FALSE;
	}

	//加载护盾纹理
	if (!PX_LoadTextureToResource(&runtime->runtime,SOD_PATH_SHIELD,SOD_KEY_TEX_SHIELD))
	{
		return PX_FALSE;
	}

	//加载star的粒子
	if (!PX_LoadTextureToResource(&runtime->runtime,SOD_PATH_PSTAR,SOD_KEY_TEX_STAR))
	{
		return PX_FALSE;
	}

	//加载star的音效
	if (!PX_LoadSoundToResource(&runtime->runtime,SOD_PATH_STARAPPLY,SOD_KEY_SND_STARAPPLY))
	{
		return PX_FALSE;
	}


	//加载star的动画
	if (!PX_LoadAnimationToResource(&runtime->runtime,SOD_PATH_ANIMATION_STAR,SOD_KEY_ANIMATION_STAR))
	{
		return PX_FALSE;
	}

	//加载AAtom的音效
	if (!PX_LoadSoundToResource(&runtime->runtime,SOD_PATH_ALIEN_FIRE,SOD_KEY_SND_ALIEN_FIRE))
	{
		return PX_FALSE;
	}

	//加载AAtom的2dx动画
	if (!PX_LoadAnimationToResource(&runtime->runtime,SOD_PATH_ANIMATION_AATOM,SOD_KEY_ANIMATION_AATOM))
	{
		return PX_FALSE;
	}

	//加载Alien的2dx动画
	if (!PX_LoadAnimationToResource(&runtime->runtime,SOD_PATH_ANIMATION_ALIEN,SOD_KEY_ANIMATION_ALIEN))
	{
		return PX_FALSE;
	}

	//加载鬼魂的2dx动画
	if (!PX_LoadAnimationToResource(&runtime->runtime,SOD_PATH_ANIMATION_GHOST,SOD_KEY_ANIMATION_GHOST))
	{
		return PX_FALSE;
	}

	//加载BGM
	if (!PX_LoadSoundToResource(&runtime->runtime,SOD_PATH_BGM,SOD_KEY_SND_BGM))
	{
		return PX_FALSE;
	}

	


	//加载爆炸粒子
	if (!PX_LoadTextureToResource(&runtime->runtime,SOD_PATH_EXPLODE_PARTICAL,SOD_KEY_TEX_EXPLODE_PARTICAL))
	{
		return PX_FALSE;
	}

	//加载爆炸音效
	if (!PX_LoadSoundToResource(&runtime->runtime,SOD_PATH_SND_EXPLODE01,SOD_KEY_SND_EXPLODE01))
	{
		return PX_FALSE;
	}

	if (!PX_LoadSoundToResource(&runtime->runtime,SOD_PATH_SND_EXPLODE02,SOD_KEY_SND_EXPLODE02))
	{
		return PX_FALSE;
	}
	if (!PX_LoadSoundToResource(&runtime->runtime,SOD_PATH_SND_EXPLODE03,SOD_KEY_SND_EXPLODE03))
	{
		return PX_FALSE;
	}

	//加载石头纹理
	if (!PX_LoadTextureToResource(&runtime->runtime,SOD_PATH_STONE1,SOD_KEY_TEX_STONE1))
	{
		return PX_FALSE;
	}

	if (!PX_LoadTextureToResource(&runtime->runtime,SOD_PATH_STONE2,SOD_KEY_TEX_STONE2))
	{
		return PX_FALSE;
	}

	if (!PX_LoadTextureToResource(&runtime->runtime,SOD_PATH_STONE3,SOD_KEY_TEX_STONE3))
	{
		return PX_FALSE;
	}

	//加载子弹纹理
	if (!PX_LoadTextureToResource(&runtime->runtime,SOD_PATH_SHIP_FIRE_SHIP_ATOM,SOD_KEY_TEX_SHIP_ATOM))
	{
		return PX_FALSE;
	}

	//加载子弹发射音效
	if (!PX_LoadSoundToResource(&runtime->runtime,SOD_PATH_SHIP_FIRE,SOD_KEY_SOUND_SHIP_FIRE))
	{
		return PX_FALSE;
	}

	//加载鼠标图标
	if (!PX_LoadTextureFromFile(&runtime->runtime.mp_resources,&pPlay->cursor.tex,SOD_PATH_CURSOR))
	{
		return PX_FALSE;
	}

	//加载火焰粒子(飞船尾焰)
	if (!PX_LoadTextureToResource(&runtime->runtime,SOD_PATH_PARTICAL_FIRE_LARGE,SOD_KEY_PARTICAL_FIRE_LARGE))
	{
		return PX_FALSE;
	}

	//加载火焰粒子(爆炸及发射)
	if (!PX_LoadTextureToResource(&runtime->runtime,SOD_PATH_PARTICAL_FIRE_ASH,SOD_KEY_PARTICAL_FIRE_ASH))
	{
		return PX_FALSE;
	}


	//指针初始角度
	pPlay->cursor.rotation=0;

	//加载飞船纹理
	if(!PX_LoadTextureToResource(&runtime->runtime,SOD_PATH_SHIP,SOD_KEY_TEX_SHIP))
		return PX_FALSE;

	
	//创建世界
	return PX_TRUE;
}

px_void SOD_PlayRenderUI(SOD_Play *pPlay)
{
	px_int i;
	px_color color;
	px_float force;
	px_float life_pc;
	px_float atom_pc;
	px_char  text[32];
	px_word  utf16_text[32];
	SOD_Play_Object_Ship *pShip;

	pShip=SOD_Object_GetShip(pPlay->shipObject);
	//绘制鼠标标识
	PX_TextureRenderRotation(&pPlay->runtime->runtime.RenderSurface,&pPlay->cursor.tex,pPlay->control.mouse_x,pPlay->control.mouse_y,PX_TEXTURERENDER_REFPOINT_CENTER,PX_NULL,(px_int)pPlay->cursor.rotation);

	//渲染推动力环
	force=PX_PointMod(pShip->force);

	color._argb.a=255;
	color._argb.r=(px_uchar)(force/pShip->max_force*255);
	color._argb.g=255-color._argb.r;
	color._argb.b=96;
	
	PX_GeoDrawCircle(&pPlay->runtime->runtime.RenderSurface,32,32,24,8,PX_COLOR(255,0,0,0));
	PX_GeoDrawRing(&pPlay->runtime->runtime.RenderSurface,32,32,24,6,color,90,90+(px_int)(force/pShip->max_force*360));
	PX_itoa((px_int)force,text,sizeof(text),10);
	PX_FontDrawText(&pPlay->runtime->runtime.RenderSurface,29-__PX_FONT_ASCSIZE*(px_strlen(text)-1)/2-1,26,text,color,PX_FONT_ALIGN_XLEFT);

	//渲染生命条
	life_pc=pShip->life*1.0f/pShip->max_life;
	color._argb.a=255;
	color._argb.r=255-(px_uchar)(life_pc*255);
	color._argb.g=255-color._argb.r;
	color._argb.b=0;
	PX_GeoDrawBorder(&pPlay->runtime->runtime.RenderSurface,62,8,166,24,2,PX_COLOR(255,0,0,0));
	PX_GeoDrawRect(&pPlay->runtime->runtime.RenderSurface,64,10,(px_int)(life_pc*100)+64,22,color);
	

	//渲染子弹条
	atom_pc=pShip->atomcount*1.0f/pShip->max_atomcount;
	PX_GeoDrawBorder(&pPlay->runtime->runtime.RenderSurface,62,27,166,43,2,PX_COLOR(255,0,0,0));
	if (pShip->fire_elpased<50)
	{
		color=PX_COLOR(255,255,0,0);
	}
	else
	{
		color=PX_COLOR(255,128,32,255);
	}
	PX_GeoDrawRect(&pPlay->runtime->runtime.RenderSurface,64,29,(px_int)(atom_pc*100)+64,41,color);

	for (i=64;i<164;i+=3)
	{
		PX_GeoDrawLine(&pPlay->runtime->runtime.RenderSurface,i,27,i,43,1,PX_COLOR(255,0,0,0));
	}

	//分数
	PX_FontModuleDrawText(&pPlay->runtime->runtime.RenderSurface,300,30,(px_uchar *)SOD_TEXT_SCORE,PX_COLOR(255,0,0,0),&pPlay->runtime->fontmodule,PX_FONT_ALIGN_XLEFT);
	PX_itoa(pPlay->showScore,text,sizeof(text),10);
	for (i=0;i<px_strlen(text);i++)
	{
		utf16_text[i]=text[i];
	}
	utf16_text[i]=0;
	PX_FontModuleDrawText(&pPlay->runtime->runtime.RenderSurface,385,30,(px_uchar *)utf16_text,PX_COLOR(255,0,128,255),&pPlay->runtime->fontmodule,PX_FONT_ALIGN_XLEFT);

	//时间
	px_sprintf2(text,sizeof(text),SOD_TEXT_ALIVE_TIME,PX_STRINGFORMAT_INT(pPlay->gametime/1000),PX_STRINGFORMAT_INT((pPlay->gametime/10)%100));
	PX_FontDrawText(&pPlay->runtime->runtime.RenderSurface,325,40,text,PX_COLOR(255,255,32,64),PX_FONT_ALIGN_XLEFT);

	//fps
	px_sprintf1(text,sizeof(text),"fps:%1",PX_STRINGFORMAT_INT(pPlay->fps));
	if (pPlay->fps<25)
	{
		PX_FontDrawText(&pPlay->runtime->runtime.RenderSurface,75,45,text,PX_COLOR(255,255,0,0),PX_FONT_ALIGN_XLEFT);
	}
	else if(pPlay->fps<35)
	{
		PX_FontDrawText(&pPlay->runtime->runtime.RenderSurface,75,45,text,PX_COLOR(255,255,255,0),PX_FONT_ALIGN_XLEFT);
	}
	else
	{
		PX_FontDrawText(&pPlay->runtime->runtime.RenderSurface,75,45,text,PX_COLOR(255,37,119,0),PX_FONT_ALIGN_XLEFT);
	}
	

}

// 飞船主推动器尾部火焰粒子
PX_Partical_Atom SOD_ParticalLauncher_ShipPower_CreateAtom(struct _PX_Partical_Launcher *launcher,px_int index)
{
	PX_Partical_Atom atom;
	atom.aliveTime=500;
	atom.alpha=1.0f;
	atom.alphaIncrement=-2.f;
	atom.elpasedTime=0;
	atom.hdrB=1;
	atom.hdrG=1;
	atom.hdrR=1;
	atom.mass=1.0f;
	atom.position=PX_POINT(0,0,0);
	atom.roatationSpeed=0;
	atom.rotation=0;
	atom.size=0.1f;
	atom.sizeIncrement=10.f;
	atom.velocity=PX_POINT(SOD_Ship_Force*4+100,(px_float)(-30+PX_rand()%60),0);
	return atom;
}

// 飞船辅助推动器尾部火焰粒子
PX_Partical_Atom SOD_ParticalLauncher_ShipAssistPower_CreateAtom(struct _PX_Partical_Launcher *launcher,px_int index)
{
	PX_Partical_Atom atom;
	atom.aliveTime=300;
	atom.alpha=1.0f;
	atom.alphaIncrement=-3.f;
	atom.elpasedTime=0;
	atom.hdrB=0.2f;
	atom.hdrG=0.2f;
	atom.hdrR=2;
	atom.mass=1.0f;
	atom.position=PX_POINT(0,0,0);
	atom.roatationSpeed=0;
	atom.rotation=0;
	atom.size=0.1f;
	atom.sizeIncrement=5.f;
	atom.velocity=PX_POINT(SOD_Ship_Force*4+100,(px_float)(-10+PX_rand()%20),0);
	return atom;
}

px_void SOD_PlayReset(SOD_Play *pPlay)
{
	SOD_Play_Object_Ship *pShip;
	PX_ParticalLauncher_InitializeInfo partical_info;
	//重置游戏内存池
	MP_Reset(&pPlay->runtime->runtime.mp_game);

	//开火分配粒子空间
	pPlay->fire_mp=MP_Create(MP_Malloc(&pPlay->runtime->runtime.mp_game,SOD_PARTICAL_MP_FIRE),SOD_PARTICAL_MP_FIRE);

	//爆炸粒子分配内存池空间
	pPlay->explode_mp=MP_Create(MP_Malloc(&pPlay->runtime->runtime.mp_game,SOD_PARTICAL_MP_EXPLODE),SOD_PARTICAL_MP_EXPLODE);
	pPlay->showScore=0;
	pPlay->gametime=0;
	pPlay->timeProcess=0;
	pPlay->score=0;
	pPlay->updatetime_reserved=0;
	pPlay->gameover_waiting=0;
	pPlay->stone_genElpased=0;
	pPlay->ghost_genElpased=0;
	pPlay->alien_genElpased=0;
	pPlay->bossscore=1000;

	//初始化世界
	if(!PX_WorldInit(&pPlay->runtime->runtime.mp_game,&pPlay->world,SOD_WORLD_WIDTH,SOD_WORLD_HEIGHT,pPlay->runtime->runtime.RenderSurface.width,pPlay->runtime->runtime.RenderSurface.height))
		return;
	
	pPlay->world.auxiliaryXSpacer=84;
	pPlay->world.auxiliaryYSpacer=84;
	//pPlay->world.auxiliaryline_color=PX_COLOR(255,0,119,167);
	//创建飞船对象
	pPlay->shipObject=SOD_Object_ShipCreate(pPlay);
	pPlay->control.mouse_x=(px_int)pPlay->shipObject->x;
	pPlay->control.mouse_y=(px_int)pPlay->shipObject->y;

	pShip=SOD_Object_GetShip(pPlay->shipObject);
	//将飞船对象加入到世界中
	PX_WorldAddObject(&pPlay->world,pPlay->shipObject);

	
	//创建飞船的推动粒子系统
	partical_info.Create_func=SOD_ParticalLauncher_ShipPower_CreateAtom;
	partical_info.force=PX_POINT(0,0,0);
	partical_info.generateDuration=1000;
	partical_info.launchCount=-1;
	partical_info.maxCount=300;
	partical_info.resistanceK=0;
	partical_info.tex=PX_ResourceLibraryGetTexture(&pPlay->runtime->runtime.ResourceLibrary,SOD_KEY_PARTICAL_FIRE_LARGE);
	partical_info.Update_func=PX_NULL;
	pPlay->ship_powerpartical_main=PX_Object_ParticalCreateEx(&pPlay->runtime->runtime.mp_game,PX_NULL,0,0,0,partical_info);
	PX_WorldAddObject(&pPlay->world,pPlay->ship_powerpartical_main);

	//创建飞船的推动粒子系统
	partical_info.Create_func=SOD_ParticalLauncher_ShipAssistPower_CreateAtom;
	partical_info.force=PX_POINT(0,0,0);
	partical_info.generateDuration=1000;
	partical_info.launchCount=-1;
	partical_info.maxCount=100;
	partical_info.resistanceK=0;
	partical_info.tex=PX_ResourceLibraryGetTexture(&pPlay->runtime->runtime.ResourceLibrary,SOD_KEY_PARTICAL_FIRE_LARGE);
	partical_info.Update_func=PX_NULL;
	pPlay->ship_powerpartical_l1=PX_Object_ParticalCreateEx(&pPlay->runtime->runtime.mp_game,PX_NULL,0,0,0,partical_info);
	PX_WorldAddObject(&pPlay->world,pPlay->ship_powerpartical_l1);

	pPlay->ship_powerpartical_l2=PX_Object_ParticalCreateEx(&pPlay->runtime->runtime.mp_game,PX_NULL,0,0,0,partical_info);
	PX_WorldAddObject(&pPlay->world,pPlay->ship_powerpartical_l2);

	//插入BGM
	SOD_PlaySoundFromResource(pPlay->runtime,SOD_KEY_SND_BGM,PX_TRUE);
}

//鼠标移动时事件处理函数
px_void PX_Play_OnControlIORead(SOD_Play *pPlay)
{
	POINT mpt=PX_MousePosition();
	pPlay->control.mouse_x=mpt.x;
	pPlay->control.mouse_y=mpt.y;
	if (PX_MouseLButtonDown())
	{
		pPlay->control.mouse_ldown=PX_TRUE;
	}
	else
	{
		pPlay->control.mouse_ldown=PX_FALSE;
	}

	if (PX_MouseRButtonDown())
	{
		pPlay->control.mouse_rdown=PX_TRUE;
	}
	else
	{
		pPlay->control.mouse_rdown=PX_FALSE;
	}

}

//IO事件处理函数
px_void SOD_PlayPostEvent(SOD_Play *pPlay,PX_Object_Event e)
{
	//仅用于UI框架处理
	PX_ObjectPostEvent(pPlay->root,e);
}

extern px_void SOD_GameOver(px_dword pt,px_dword alive_time);
px_void SOD_PlayUpdate(SOD_Play *pPlay,px_dword elpased)
{
	px_memorypool calcmp;
	px_dword decTime=0;
	pPlay->updatetime_reserved+=elpased;
	//////////////////////////////////////////////////////////////////////////
	//读取设备IO信息
	PX_Play_OnControlIORead(pPlay);

	//////////////////////////////////////////////////////////////////////////
	//锁定地图摄像机
	if (pPlay->shipObject)
	{
		PX_WorldSetCamera(&pPlay->world,PX_POINT(pPlay->shipObject->x,pPlay->shipObject->y,0));
	}
	//////////////////////////////////////////////////////////////////////////
	//更新游戏时间
	if (pPlay->shipObject->Visible)
	{
		//更新时间分数
		pPlay->gametime+=elpased;

		//时间分数
		pPlay->timeProcess+=elpased;
		if (pPlay->timeProcess>1000)
		{
			if(SOD_Object_GetShip(pPlay->shipObject)->atomcount<SOD_Object_GetShip(pPlay->shipObject)->max_atomcount)
				SOD_Object_GetShip(pPlay->shipObject)->atomcount+=1;
			pPlay->score+=10;
			pPlay->timeProcess-=1000;
		}
	}

	if (pPlay->showScore!=pPlay->score)
	{
		px_int inc=(pPlay->score-pPlay->showScore)/10;
		if (PX_ABS(inc)<=1)
		{
			pPlay->showScore=pPlay->score;
		}
		else
			pPlay->showScore+=inc;
	}
	//Game over switcher
	if (pPlay->gameover_waiting)
	{
		if (pPlay->gameover_waiting<=elpased)
		{
			//清理音效数据
			PX_SoundPlayClear(&pPlay->runtime->sound);
			SOD_GameOver(pPlay->score,pPlay->gametime);
		}
		else
		{
			pPlay->gameover_waiting-=elpased;
		}
	}
	
	

	
	//更新指针旋转角度
	pPlay->cursor.rotation+=elpased*0.36f;


	//////////////////////////////////////////////////////////////////////////
	//随机生成陨石
	//每隔1500分减少100毫秒,最小SOD_MIN_STONE_GEN_DURATION
	decTime=pPlay->score/15;
	if (decTime>SOD_DEFAULT_STONE_GEN_DURATION-SOD_MIN_STONE_GEN_DURATION)
	{
		decTime=SOD_DEFAULT_STONE_GEN_DURATION-SOD_MIN_STONE_GEN_DURATION;
	}

	pPlay->stone_genElpased+=elpased;
	while (pPlay->stone_genElpased>=(SOD_DEFAULT_STONE_GEN_DURATION-decTime))
	{
		PX_Object *pObject;
		pPlay->stone_genElpased-=(SOD_DEFAULT_STONE_GEN_DURATION-decTime);
		pObject=SOD_Object_StoneCreate(pPlay,800,(px_float)(50+PX_rand()%100));
		PX_WorldAddObject(&pPlay->world,pObject);
	}

	//////////////////////////////////////////////////////////////////////////
	//随机生成鬼魂
	//每隔500分减少100毫秒
	if (pPlay->score>SOD_GHOST_GEN_SCORE)
	{
		decTime=(pPlay->score-SOD_GHOST_GEN_SCORE)/5;
		if (decTime>SOD_DEFAULT_GHOST_GEN_DURATION-SOD_MIN_GHOST_GEN_DURATION)
		{
			decTime=SOD_DEFAULT_GHOST_GEN_DURATION-SOD_MIN_GHOST_GEN_DURATION;
		}

		pPlay->ghost_genElpased+=elpased;
		while (pPlay->ghost_genElpased>=(SOD_DEFAULT_GHOST_GEN_DURATION-decTime))
		{
			PX_Object *pObject;
			pPlay->ghost_genElpased-=(SOD_DEFAULT_GHOST_GEN_DURATION-decTime);
			pObject=SOD_Object_GhostCreate(pPlay,600,(px_float)(50+PX_rand()%100));
			PX_WorldAddObject(&pPlay->world,pObject);
		}
	}
	


	//////////////////////////////////////////////////////////////////////////
	//随机生成外星人
	//每隔500分减少100毫秒
	if (pPlay->score>SOD_ALIEN_GEN_SCORE)
	{
		decTime=(pPlay->score-SOD_ALIEN_GEN_SCORE)/5;
		if (decTime>SOD_DEFAULT_ALIEN_GEN_DURATION-SOD_MIN_ALIEN_GEN_DURATION)
		{
			decTime=SOD_DEFAULT_ALIEN_GEN_DURATION-SOD_MIN_ALIEN_GEN_DURATION;
		}

		pPlay->alien_genElpased+=elpased;
		while (pPlay->alien_genElpased>=(SOD_DEFAULT_ALIEN_GEN_DURATION-decTime))
		{
			PX_Object *pObject;
			pPlay->alien_genElpased-=(SOD_DEFAULT_ALIEN_GEN_DURATION-decTime);
			pObject=SOD_Object_AlienCreate(pPlay,400,(px_float)(50+PX_rand()%100));
			PX_WorldAddObject(&pPlay->world,pObject);
		}
	}

	//////////////////////////////////////////////////////////////////////////
	//生成UserObject
	if (pPlay->score>pPlay->bossscore)
	{
		PX_Object *pObject=SOD_Object_UserObjectCreate(pPlay,pPlay->bossscore/6*4);
		pPlay->bossscore+=1000;
		PX_WorldAddObject(&pPlay->world,pObject);
	}
	


	//////////////////////////////////////////////////////////////////////////
	//更新世界对象管理器
	calcmp=PX_RuntimeCreateCalcMemoryPool(&pPlay->runtime->runtime);//用于物理计算的临时内存
	while (pPlay->updatetime_reserved>=SOD_WORLD_UPDATE_ATOM)//最小更新粒度时间
	{
		MP_Reset(&calcmp);
		PX_WorldUpdate(&pPlay->world,&calcmp,SOD_WORLD_UPDATE_ATOM);
		pPlay->updatetime_reserved-=SOD_WORLD_UPDATE_ATOM;
	}
	PX_RuntimeFreeCalcMemoryPool(&pPlay->runtime->runtime,calcmp);//释放计算内存
	
}

px_void SOD_PlayRender(SOD_Play *pPlay,px_dword elpased)
{
	
	px_memorypool calcmp;
	calcmp=PX_RuntimeCreateCalcMemoryPool(&pPlay->runtime->runtime);
	PX_WorldRender(&pPlay->runtime->runtime.RenderSurface,&calcmp,&pPlay->world,elpased);
	PX_RuntimeFreeCalcMemoryPool(&pPlay->runtime->runtime,calcmp);

	SOD_PlayRenderUI(pPlay);

	//fps
	pPlay->fps_frame++;
	pPlay->fps_elpased+=elpased;
	if (pPlay->fps_elpased>1000)
	{
		pPlay->fps=pPlay->fps_frame;
		pPlay->fps_frame=0;
		pPlay->fps_elpased=0;
	}
}

