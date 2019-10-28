#include "PX_Partical.h"
/*
set Partical_Init_Info
{
int		     generateDuration;  +0
int			 maxCount; +1
float        forceX +2 ,forceY +3,forceZ +4;
float        resistanceK +5;
int          launchCount +6;
}
*/

static px_bool PX_Partical_Rand(PX_ScriptVM_Instance *ins)
{
	PX_ScriptVM_RET(ins,PX_ScriptVM_Variable_float(PX_rand()*1.0f/PX_RAND_MAX));
	return PX_TRUE;
}

px_int PX_ParticalVM_ConvertToInt(PX_SCRIPTVM_VARIABLE var)
{
	if (var.type==PX_SCRIPTVM_VARIABLE_TYPE_INT)
	{
		return var._int;
	}
	else if(var.type==PX_SCRIPTVM_VARIABLE_TYPE_FLOAT)
	{
		return (px_int)var._float;
	}
	else if(var.type==PX_SCRIPTVM_VARIABLE_TYPE_STRING)
	{
		return PX_atoi(var._string.buffer);
	}
	else
		return 0;
}

px_float PX_ParticalVM_ConvertToFloat(PX_SCRIPTVM_VARIABLE var)
{
	if (var.type==PX_SCRIPTVM_VARIABLE_TYPE_INT)
	{
		return (px_float)var._int;
	}
	else if(var.type==PX_SCRIPTVM_VARIABLE_TYPE_FLOAT)
	{
		return var._float;
	}
	else if(var.type==PX_SCRIPTVM_VARIABLE_TYPE_STRING)
	{
		return PX_atof(var._string.buffer);
	}
	else
		return 0;
}

px_bool PX_ParticalLauncherCreateEx(PX_Partical_Launcher *env,px_memorypool *mp,PX_ParticalLauncher_InitializeInfo Info)
{
	env->VM_Instance=PX_NULL;

	env->mp=mp;
	env->genIndex=0;
	env->texture=Info.tex;
	env->generateDuration=Info.generateDuration;
	env->elpased=0;
	env->maxCount=Info.maxCount;
	env->force=Info.force;
	env->resistanceK=Info.resistanceK;
	env->launchCount=Info.launchCount;
	env->Create_func=Info.Create_func;
	env->Update_func=Info.Update_func;
	env->launcherVelocity=PX_POINT(0,0,0);
	env->launcherPosition=PX_POINT(0,0,0);
	env->user=PX_NULL;
	env->velocity.x=0;
	env->velocity.y=0;
	env->velocity.z=0;
	env->direction.x=1;
	env->direction.y=0;
	env->direction.z=0;
	env->CreateParticalFuncIndex=-1;
	env->UpdateParitcalFuncIndex=-1;

	env->ParticalPool=(PX_Partical_Atom *)MP_Malloc(mp,sizeof(PX_Partical_Atom)*env->maxCount);
	PX_memset(env->ParticalPool,0,sizeof(PX_Partical_Atom)*env->maxCount);
	return PX_TRUE;
}

px_bool PX_ParticalLauncherCreate(PX_Partical_Launcher *env,px_memorypool *mp,px_texture *tex,PX_ScriptVM_Instance *pIns,px_char *Initfunc,px_char *_createfunc,px_char *_updatefunc)
{
	px_int vm_ptr;

	env->VM_Instance=pIns;
	env->mp=mp;
	env->genIndex=0;
	env->texture=tex;

	env->Create_func=PX_NULL;
	env->Update_func=PX_NULL;
	
	if (!PX_ScriptVM_InstanceRunFunction(env->VM_Instance,0,PX_NULL,Initfunc,PX_NULL,0))
	{
		PX_ScriptVM_InstanceFree(env->VM_Instance);
		return PX_FALSE;
	}

	if(PX_ScriptVM_REG_RETURN(env->VM_Instance).type!=PX_SCRIPTVM_VARIABLE_TYPE_INT)
	{
		PX_ScriptVM_InstanceFree(env->VM_Instance);
		return PX_FALSE;
	}
	vm_ptr=PX_ScriptVM_REG_RETURN(env->VM_Instance)._int;



	env->generateDuration=PX_ParticalVM_ConvertToInt(PX_ScriptVM_GLOBAL(env->VM_Instance,vm_ptr+0));

	env->elpased=0;

	env->maxCount=PX_ParticalVM_ConvertToInt(PX_ScriptVM_GLOBAL(env->VM_Instance,vm_ptr+1));

	env->force.x=PX_ParticalVM_ConvertToFloat(PX_ScriptVM_GLOBAL(env->VM_Instance,vm_ptr+2));

	env->force.y=PX_ParticalVM_ConvertToFloat(PX_ScriptVM_GLOBAL(env->VM_Instance,vm_ptr+3));

	env->force.z=PX_ParticalVM_ConvertToFloat(PX_ScriptVM_GLOBAL(env->VM_Instance,vm_ptr+4));

	env->resistanceK=PX_ParticalVM_ConvertToFloat(PX_ScriptVM_GLOBAL(env->VM_Instance,vm_ptr+5));

	env->launchCount=PX_ParticalVM_ConvertToInt(PX_ScriptVM_GLOBAL(env->VM_Instance,vm_ptr+6));

	env->velocity.x=0;
	env->velocity.y=0;
	env->velocity.z=0;

	env->direction.x=1;
	env->direction.y=0;
	env->direction.z=0;

	env->launcherPosition.x=0;
	env->launcherPosition.y=0;
	env->launcherPosition.z=0;

	env->launcherVelocity.x=0;
	env->launcherVelocity.y=0;
	env->launcherVelocity.z=0;

 	env->CreateParticalFuncIndex=PX_ScriptVM_GetFunctionIndex(env->VM_Instance,_createfunc);
 	env->UpdateParitcalFuncIndex=PX_ScriptVM_GetFunctionIndex(env->VM_Instance,_updatefunc);

	env->ParticalPool=(PX_Partical_Atom *)MP_Malloc(mp,sizeof(PX_Partical_Atom)*env->maxCount);
	PX_memset(env->ParticalPool,0,sizeof(PX_Partical_Atom)*env->maxCount);

	PX_ScriptVM_RegistryHostFunction(env->VM_Instance,"PARTICAL_RAND",PX_Partical_Rand);

	return PX_TRUE;
}

px_void PX_ParticalLauncherSetLauncherPosition(PX_Partical_Launcher *launcher,px_point position)
{
	launcher->launcherPosition=position;
}

/*
set PARTICAL_ATOM_INFO
{
float   size; +0
float   rotation; +1
float   mass;+2
float   alpha;+3
int     aliveTime;+4
float   x +5,y +6,z +7;
float   vx +8,vy +9,vz +10;
float   hdrR +11,hdrG +12,hdrB +13;
float   rotationSpeed +14;
float   alphaIncrement +15;
float   sizeIncrement +16;
};

void Partical_Atom_Create(PARTICAL_ATOM_INFO *atomInfo)
{

}

void Partical_Atom_Update(PARTICAL_ATOM_INFO *atomInfo)
{

}
*/
px_void PX_ParticalAtomUpdate(PX_Partical_Launcher *env,PX_Partical_Atom *pAtom,px_dword elpased)
{
	px_int updateTime;
	px_int atomTime;
	px_float vx,vy,vz,rX,rY,rZ;
	px_float resisForceX,resisForceY,resisForceZ;
	PX_SCRIPTVM_MEMORY_PTR memptr;
	PX_SCRIPTVM_VARIABLE var;
	if (elpased==0)
	{
		return;
	}
	if (pAtom->aliveTime)
	{
		if (pAtom->aliveTime<elpased)
		{
			pAtom->aliveTime=0;
			return;
		}
		else
		{
			pAtom->aliveTime-=elpased;
			pAtom->elpasedTime+=elpased;
		}

		updateTime=elpased;

		while (updateTime)
		{
			if (updateTime>PX_PARTICAL_ATOM_TIME)
			{
				atomTime=PX_PARTICAL_ATOM_TIME;
				updateTime-=PX_PARTICAL_ATOM_TIME;
			}
			else
			{
				atomTime=updateTime;
				updateTime=0;
			}

			pAtom->size+=pAtom->sizeIncrement*atomTime/1000;
			pAtom->alpha+=pAtom->alphaIncrement*atomTime/1000;
			pAtom->rotation+=pAtom->roatationSpeed*atomTime/1000;

			vx=pAtom->velocity.x*atomTime/1000+env->force.x/pAtom->mass*0.5f*atomTime*atomTime/1000000;
			vy=pAtom->velocity.y*atomTime/1000+env->force.y/pAtom->mass*0.5f*atomTime*atomTime/1000000;
			vz=pAtom->velocity.z*atomTime/1000+env->force.z/pAtom->mass*0.5f*atomTime*atomTime/1000000;

			resisForceX=(env->resistanceK*pAtom->velocity.x);
			resisForceY=(env->resistanceK*pAtom->velocity.y);
			resisForceZ=(env->resistanceK*pAtom->velocity.z);

			rX=resisForceX/pAtom->mass*0.5f*atomTime*atomTime/1000000;
			rY=resisForceY/pAtom->mass*0.5f*atomTime*atomTime/1000000;
			rZ=resisForceZ/pAtom->mass*0.5f*atomTime*atomTime/1000000;


			if (vx*vx+vy*vy+vz*vz<rX*rX+rY*rY+rZ*rZ)
			{
				pAtom->velocity.x=0;
				pAtom->velocity.y=0;
				pAtom->velocity.z=0;
			}
			else
			{
				pAtom->position.x+=vx-rX;
				pAtom->position.y+=vy-rY;
				pAtom->position.z+=vz-rZ;

				pAtom->velocity.x+=(env->force.x-resisForceX)/pAtom->mass*PX_PARTICAL_ATOM_TIME/1000;
				pAtom->velocity.y+=(env->force.y-resisForceY)/pAtom->mass*PX_PARTICAL_ATOM_TIME/1000;
				pAtom->velocity.z+=(env->force.z-resisForceZ)/pAtom->mass*PX_PARTICAL_ATOM_TIME/1000;
			}
		}

		/*
	set PARTICAL_ATOM_INFO
	{
		float   size; +0
			float   rotation; +1
			float   mass;+2
			float   alpha;+3
			int     aliveTime;+4
			float   x +5,y +6,z +7;
		float   vx +8,vy +9,vz +10;
		float hdrR;
		float hdrG;
		float hdrB;
		float RotationSpeed;
		float sizeIncrement;
		float alphaIncrement;
	};
	*/
		if (env->Update_func)
		{
			env->Update_func(env,pAtom);
		}
		else if(env->UpdateParitcalFuncIndex!=-1)
			{
				if(!PX_ScriptVM_LocalAlloc(env->VM_Instance,18,&memptr))
					return;

				PX_ScriptVM_GLOBAL(env->VM_Instance,memptr.ptr+0)=PX_ScriptVM_Variable_float(pAtom->size);
				PX_ScriptVM_GLOBAL(env->VM_Instance,memptr.ptr+1)=PX_ScriptVM_Variable_float(pAtom->rotation);
				PX_ScriptVM_GLOBAL(env->VM_Instance,memptr.ptr+2)=PX_ScriptVM_Variable_float(pAtom->mass);
				PX_ScriptVM_GLOBAL(env->VM_Instance,memptr.ptr+3)=PX_ScriptVM_Variable_float(pAtom->alpha);
				PX_ScriptVM_GLOBAL(env->VM_Instance,memptr.ptr+4)=PX_ScriptVM_Variable_int(pAtom->aliveTime);
				PX_ScriptVM_GLOBAL(env->VM_Instance,memptr.ptr+5)=PX_ScriptVM_Variable_int(pAtom->elpasedTime);
				PX_ScriptVM_GLOBAL(env->VM_Instance,memptr.ptr+6)=PX_ScriptVM_Variable_float(pAtom->position.x);
				PX_ScriptVM_GLOBAL(env->VM_Instance,memptr.ptr+7)=PX_ScriptVM_Variable_float(pAtom->position.y);
				PX_ScriptVM_GLOBAL(env->VM_Instance,memptr.ptr+8)=PX_ScriptVM_Variable_float(pAtom->position.z);
				PX_ScriptVM_GLOBAL(env->VM_Instance,memptr.ptr+9)=PX_ScriptVM_Variable_float(pAtom->velocity.x);
				PX_ScriptVM_GLOBAL(env->VM_Instance,memptr.ptr+10)=PX_ScriptVM_Variable_float(pAtom->velocity.y);
				PX_ScriptVM_GLOBAL(env->VM_Instance,memptr.ptr+11)=PX_ScriptVM_Variable_float(pAtom->velocity.z);
				PX_ScriptVM_GLOBAL(env->VM_Instance,memptr.ptr+12)=PX_ScriptVM_Variable_float(pAtom->hdrR);
				PX_ScriptVM_GLOBAL(env->VM_Instance,memptr.ptr+13)=PX_ScriptVM_Variable_float(pAtom->hdrG);
				PX_ScriptVM_GLOBAL(env->VM_Instance,memptr.ptr+14)=PX_ScriptVM_Variable_float(pAtom->hdrB);
				PX_ScriptVM_GLOBAL(env->VM_Instance,memptr.ptr+15)=PX_ScriptVM_Variable_float(pAtom->roatationSpeed);
				PX_ScriptVM_GLOBAL(env->VM_Instance,memptr.ptr+16)=PX_ScriptVM_Variable_float(pAtom->sizeIncrement);
				PX_ScriptVM_GLOBAL(env->VM_Instance,memptr.ptr+17)=PX_ScriptVM_Variable_float(pAtom->alphaIncrement);
				var=PX_ScriptVM_Variable_int(memptr.ptr);
			if(PX_ScriptVM_InstanceRunFunctionIndex(env->VM_Instance,0,PX_NULL,env->UpdateParitcalFuncIndex,&var,1))
			{
			  pAtom->size=PX_ParticalVM_ConvertToFloat(PX_ScriptVM_GLOBAL(env->VM_Instance,memptr.ptr+0));
			  pAtom->rotation=PX_ParticalVM_ConvertToFloat(PX_ScriptVM_GLOBAL(env->VM_Instance,memptr.ptr+1));
			  pAtom->mass=PX_ParticalVM_ConvertToFloat(PX_ScriptVM_GLOBAL(env->VM_Instance,memptr.ptr+2));
			  pAtom->alpha=PX_ParticalVM_ConvertToFloat(PX_ScriptVM_GLOBAL(env->VM_Instance,memptr.ptr+3));
			  pAtom->aliveTime=PX_ParticalVM_ConvertToInt(PX_ScriptVM_GLOBAL(env->VM_Instance,memptr.ptr+4));
			  pAtom->elpasedTime=PX_ParticalVM_ConvertToInt(PX_ScriptVM_GLOBAL(env->VM_Instance,memptr.ptr+5));
			  pAtom->position.x=PX_ParticalVM_ConvertToFloat(PX_ScriptVM_GLOBAL(env->VM_Instance,memptr.ptr+6));
			  pAtom->position.y=PX_ParticalVM_ConvertToFloat(PX_ScriptVM_GLOBAL(env->VM_Instance,memptr.ptr+7));
			  pAtom->position.z=PX_ParticalVM_ConvertToFloat(PX_ScriptVM_GLOBAL(env->VM_Instance,memptr.ptr+8));
			  pAtom->velocity.x=PX_ParticalVM_ConvertToFloat(PX_ScriptVM_GLOBAL(env->VM_Instance,memptr.ptr+9));
			  pAtom->velocity.y=PX_ParticalVM_ConvertToFloat(PX_ScriptVM_GLOBAL(env->VM_Instance,memptr.ptr+10));
			  pAtom->velocity.z=PX_ParticalVM_ConvertToFloat(PX_ScriptVM_GLOBAL(env->VM_Instance,memptr.ptr+11));
			  pAtom->hdrR=PX_ParticalVM_ConvertToFloat(PX_ScriptVM_GLOBAL(env->VM_Instance,memptr.ptr+12));
			  pAtom->hdrG=PX_ParticalVM_ConvertToFloat(PX_ScriptVM_GLOBAL(env->VM_Instance,memptr.ptr+13));
			  pAtom->hdrB=PX_ParticalVM_ConvertToFloat(PX_ScriptVM_GLOBAL(env->VM_Instance,memptr.ptr+14));
			  pAtom->roatationSpeed=PX_ParticalVM_ConvertToFloat(PX_ScriptVM_GLOBAL(env->VM_Instance,memptr.ptr+15));
			  pAtom->sizeIncrement=PX_ParticalVM_ConvertToFloat(PX_ScriptVM_GLOBAL(env->VM_Instance,memptr.ptr+16));
			  pAtom->alphaIncrement=PX_ParticalVM_ConvertToFloat(PX_ScriptVM_GLOBAL(env->VM_Instance,memptr.ptr+17));
			}

			PX_ScriptVM_LocalFree(env->VM_Instance,&memptr);
		}

		if (pAtom->size<0)
		{
			pAtom->size=0;
		}

		if (pAtom->hdrR<0)
		{
			pAtom->hdrR=0;
		}

		if (pAtom->hdrG<0)
		{
			pAtom->hdrG=0;
		}

		if (pAtom->hdrB<0)
		{
			pAtom->hdrB=0;
		}

		if (pAtom->alpha<0)
		{
			pAtom->alpha=0;
		}
	}
	

}

px_bool PX_ParticalLauncherUpdate(PX_Partical_Launcher *env,px_dword elpased)
{
	px_int i,j;
	px_uint redTime;
	px_int gencount=0;
	PX_Partical_Atom *pAtom;
	PX_SCRIPTVM_VARIABLE var;
	env->lefttopX=0;
	env->leftTopY=0;
	env->rightBottomX=0;
	env->rightBottomY=0;

	for (i=0;i<(px_int)env->maxCount;i++)
	{
		pAtom=&env->ParticalPool[i];
		PX_ParticalAtomUpdate(env,pAtom,elpased);
		if (pAtom->aliveTime!=0)
		{
			if (env->ParticalPool[i].position.x<env->lefttopX)
			{
				env->lefttopX=env->ParticalPool[i].position.x;
			}
			if (env->ParticalPool[i].position.x>env->rightBottomX)
			{
				env->rightBottomX=env->ParticalPool[i].position.x;
			}
			if (env->ParticalPool[i].position.y<env->leftTopY)
			{
				env->leftTopY=env->ParticalPool[i].position.y;
			}
			if (env->ParticalPool[i].position.y>env->rightBottomY)
			{
				env->rightBottomY=env->ParticalPool[i].position.y;
			}
		}
	}
	if(env->launchCount!=0)
	{
		if (env->generateDuration!=0)
		{
			if (env->elpased/env->generateDuration<((env->elpased+elpased)/env->generateDuration))
			{
				gencount=(env->elpased+elpased)/env->generateDuration-env->elpased/env->generateDuration;
				redTime=env->elpased+elpased-(env->elpased+env->generateDuration)/env->generateDuration*env->generateDuration;
			}
			else
				gencount=0;
		}
		else
		{
			gencount=env->maxCount;
			redTime=elpased;
		}
		

		for (i=0;i<gencount;i++)
		{
			if(env->launchCount!=0)
			{
				for (j=0;j<(px_int)env->maxCount;j++)
				{
					if (env->ParticalPool[j].aliveTime==0)
					{
						if (env->Create_func)
						{
							env->ParticalPool[j]=env->Create_func(env,env->genIndex);
						}
						else if (env->CreateParticalFuncIndex!=-1)
						{
							var=PX_ScriptVM_Variable_int(env->genIndex);
							if(PX_ScriptVM_InstanceRunFunctionIndex(env->VM_Instance,0,PX_NULL,env->CreateParticalFuncIndex,&var,1))
							{
							/*
							set PARTICAL_ATOM_INFO
							{
								    float   size; +0
									float   rotation; +1
									float   mass;+2
									float   alpha;+3
									int     aliveTime;+4
									int     elpased;+5
									float   x +6,y +7,z +8;
								    float   vx +9,vy +10,vz +11;
									float   hdrR +12,hdrG +13,hdrB +14;
							};
							*/	
							env->ParticalPool[j].size=PX_ScriptVM_RETURN_POINTER(env->VM_Instance,0)._float;
							env->ParticalPool[j].rotation=PX_ScriptVM_RETURN_POINTER(env->VM_Instance,1)._float;
							env->ParticalPool[j].mass=PX_ScriptVM_RETURN_POINTER(env->VM_Instance,2)._float;
							env->ParticalPool[j].alpha=PX_ScriptVM_RETURN_POINTER(env->VM_Instance,3)._float;
							env->ParticalPool[j].aliveTime=PX_ScriptVM_RETURN_POINTER(env->VM_Instance,4)._int;
							env->ParticalPool[j].elpasedTime=PX_ScriptVM_RETURN_POINTER(env->VM_Instance,5)._int;
							env->ParticalPool[j].position.x=PX_ScriptVM_RETURN_POINTER(env->VM_Instance,6)._float;
							env->ParticalPool[j].position.y=PX_ScriptVM_RETURN_POINTER(env->VM_Instance,7)._float;
							env->ParticalPool[j].position.z=PX_ScriptVM_RETURN_POINTER(env->VM_Instance,8)._float;
							env->ParticalPool[j].velocity.x=PX_ScriptVM_RETURN_POINTER(env->VM_Instance,9)._float+env->velocity.x;
							env->ParticalPool[j].velocity.y=PX_ScriptVM_RETURN_POINTER(env->VM_Instance,10)._float+env->velocity.y;
							env->ParticalPool[j].velocity.z=PX_ScriptVM_RETURN_POINTER(env->VM_Instance,11)._float+env->velocity.z;
							env->ParticalPool[j].hdrR=PX_ScriptVM_RETURN_POINTER(env->VM_Instance,12)._float;
							env->ParticalPool[j].hdrG=PX_ScriptVM_RETURN_POINTER(env->VM_Instance,13)._float;
							env->ParticalPool[j].hdrB=PX_ScriptVM_RETURN_POINTER(env->VM_Instance,14)._float;
							env->ParticalPool[j].roatationSpeed=PX_ScriptVM_RETURN_POINTER(env->VM_Instance,15)._float;
							env->ParticalPool[j].sizeIncrement=PX_ScriptVM_RETURN_POINTER(env->VM_Instance,16)._float;
							env->ParticalPool[j].alphaIncrement=PX_ScriptVM_RETURN_POINTER(env->VM_Instance,17)._float;
							}
						}
						//apply direction & launch postion
						env->ParticalPool[j].velocity=PX_PointReflectX(env->direction,env->ParticalPool[j].velocity);
						env->ParticalPool[j].velocity=PX_PointAdd(env->ParticalPool[j].velocity,env->launcherVelocity);
						env->ParticalPool[j].position=PX_PointReflectX(env->direction,env->ParticalPool[j].position);
						env->ParticalPool[j].position=PX_PointAdd(env->ParticalPool[j].position,env->launcherPosition);


						PX_ParticalAtomUpdate(env,&env->ParticalPool[j],redTime);

						if (env->ParticalPool[j].position.x<env->lefttopX)
						{
							env->lefttopX=env->ParticalPool[j].position.x;
						}
						if (env->ParticalPool[j].position.x>env->rightBottomX)
						{
							env->rightBottomX=env->ParticalPool[j].position.x;
						}
						if (env->ParticalPool[j].position.y<env->leftTopY)
						{
							env->leftTopY=env->ParticalPool[j].position.y;
						}
						if (env->ParticalPool[j].position.y>env->rightBottomY)
						{
							env->rightBottomY=env->ParticalPool[j].position.y;
						}
						if(redTime>env->generateDuration)
						{
							redTime-=env->generateDuration;
						}
						else
						{
							redTime=0;
						}

						env->genIndex++;
						if (env->launchCount>0)
						{
							env->launchCount--;
						}
						break;
					}
				}
			}
			
		}
	}
	env->elpased+=elpased;
	return PX_TRUE;
}

px_bool PX_ParticalIsInSurfaceRegion(px_point atomPoint,px_int atomWidth,px_int atomHeight,px_float scale,px_int surfaceWidth,px_int surfaceHeight)
{
	px_rect atomRect;
	px_rect surfaceRect;
	atomRect.x=atomPoint.x;
	atomRect.y=atomPoint.y;
	atomRect.width=atomWidth*scale;
	atomRect.height=atomHeight*scale;

	surfaceRect.x=0;
	surfaceRect.y=0;
	surfaceRect.height=(px_float)surfaceHeight;
	surfaceRect.width=(px_float)surfaceWidth;

	return PX_isRectCrossRect(atomRect,surfaceRect);
}

px_void PX_ParticalLauncherRender(px_surface *surface,PX_Partical_Launcher *env,px_point offset)
{
	px_int i;
	PX_TEXTURERENDER_BLEND blend;
	
	if (!PX_isRectCrossRect(PX_RECT(0,0,(px_float)surface->width,(px_float)surface->height),\
		PX_RECT(env->lefttopX+offset.x,env->leftTopY+offset.y,env->rightBottomX-env->lefttopX+1,env->rightBottomY-env->leftTopY+1)))
	{
		return;
	}

	if (env->texture==PX_NULL)
	{
		PX_ASSERT();
	}

	for (i=0;i<(px_int)env->maxCount;i++)
	{
		
		px_point pos=env->ParticalPool[i].position;
		pos=PX_PointAdd(pos,offset);

		if (env->ParticalPool[i].aliveTime!=0&&env->ParticalPool[i].size!=0)
		{
			if (env->ParticalPool[i].size!=1.0||env->ParticalPool[i].rotation!=0)
			{
				if (env->ParticalPool[i].size<0)
				{
					env->ParticalPool[i].size=0;
					continue;
				}
				if(env->ParticalPool[i].alpha==1.0&&env->ParticalPool[i].hdrR==1.0f&&env->ParticalPool[i].hdrG==1.0f&&env->ParticalPool[i].hdrB==1.0f)
					PX_TextureRenderEx(surface,env->texture,(px_int)(pos.x),(px_int)(pos.y),PX_TEXTURERENDER_REFPOINT_CENTER,PX_NULL,env->ParticalPool[i].size,env->ParticalPool[i].rotation);
				else
				{
					blend.alpha=env->ParticalPool[i].alpha;
					blend.hdr_R=env->ParticalPool[i].hdrR;
					blend.hdr_G=env->ParticalPool[i].hdrG;
					blend.hdr_B=env->ParticalPool[i].hdrB;
					PX_TextureRenderEx(surface,env->texture,(px_int)(pos.x),(px_int)(pos.y),PX_TEXTURERENDER_REFPOINT_CENTER,&blend,env->ParticalPool[i].size,env->ParticalPool[i].rotation);
				}
			}
			else
			{
				if(env->ParticalPool[i].alpha==1.0&&env->ParticalPool[i].hdrR==1.0f&&env->ParticalPool[i].hdrG==1.0f&&env->ParticalPool[i].hdrB==1.0f)
					PX_TextureRender(surface,env->texture,(px_int)(pos.x),(px_int)(pos.y),PX_TEXTURERENDER_REFPOINT_CENTER,PX_NULL);
				else
				{
					blend.alpha=env->ParticalPool[i].alpha;
					blend.hdr_R=env->ParticalPool[i].hdrR;
					blend.hdr_G=env->ParticalPool[i].hdrG;
					blend.hdr_B=env->ParticalPool[i].hdrB;
					PX_TextureRender(surface,env->texture,(px_int)(pos.x),(px_int)(pos.y),PX_TEXTURERENDER_REFPOINT_CENTER,&blend);
				}
			}
		}
	}

}

px_void PX_ParticalLauncherFree(PX_Partical_Launcher *env)
{
	MP_Free(env->mp,env->ParticalPool);
}

px_bool PX_ParticalLauncherSetCreateFuncIndex(PX_Partical_Launcher *launcher,px_char *func_Name)
{
	return launcher->CreateParticalFuncIndex=PX_ScriptVM_GetFunctionIndex(launcher->VM_Instance,func_Name)!=-1;
}

px_bool PX_ParticalLauncherSetUpdateFuncIndex(PX_Partical_Launcher *launcher,px_char *func_Name)
{
	return launcher->UpdateParitcalFuncIndex=PX_ScriptVM_GetFunctionIndex(launcher->VM_Instance,func_Name)!=-1;
}

