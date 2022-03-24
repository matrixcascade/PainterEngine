#include "PX_Partical.h"



px_void PX_ParticalLauncherInitializeDefaultInfo(PX_ParticalLauncher_InitializeInfo* info)
{
	PX_memset(info, 0, sizeof(PX_ParticalLauncher_InitializeInfo));
	info->ak = 1;
	info->alive = -1;
	info->alpha = 1;
	info->atomsize = 1;
	info->hdrB = 1;
	info->hdrG = 1;
	info->hdrR = 1;
	info->launchCount = -1;
	info->maxCount = 100;
}

px_bool PX_ParticalLauncherInitialize(PX_ParticalLauncher *launcher,px_memorypool *mp,PX_ParticalLauncher_InitializeInfo Info)
{
	PX_memset(launcher,0,sizeof(PX_ParticalLauncher));
	launcher->LauncherInfo=Info;
	launcher->mp=mp;

	launcher->ParticalPool=(PX_Partical_Atom *)MP_Malloc(mp,sizeof(PX_Partical_Atom)*launcher->LauncherInfo.maxCount);
	PX_memset(launcher->ParticalPool,0,sizeof(PX_Partical_Atom)*launcher->LauncherInfo.maxCount);
	return PX_TRUE;
}


px_void PX_ParticalLauncherSetPosition(PX_ParticalLauncher *launcher,px_float x,px_float y,px_float z)
{
	launcher->LauncherInfo.position=PX_POINT(x,y,z);
}

px_void PX_ParticalAtomUpdate(PX_ParticalLauncher *env,PX_Partical_Atom *pAtom,px_dword elapsed)
{
	px_int updateTime;
	px_int atomTime;
	px_point ak;

	if (elapsed==0)
	{
		return;
	}
	if (pAtom->reg_alive)
	{
		if (pAtom->reg_alive<elapsed)
		{
			pAtom->reg_alive=0;
			return;
		}
		else
		{
			pAtom->reg_alive-=elapsed;
			pAtom->elapsedTime+=elapsed;
		}

		updateTime=elapsed;

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

			pAtom->reg_size+=pAtom->reg_sizeinc*atomTime/1000.f;
			pAtom->reg_alpha+=pAtom->reg_alphainc*atomTime/1000.f;
			pAtom->reg_currentrotation+=pAtom->reg_rotation*atomTime/1000.f;

			pAtom->velocity.x+=pAtom->reg_a.x*atomTime/1000.f;
			pAtom->velocity.y+=pAtom->reg_a.y*atomTime/1000.f;
			pAtom->velocity.z+=pAtom->reg_a.z*atomTime/1000.f;

			ak=PX_PointMul(pAtom->velocity,pAtom->reg_ak);
			ak = PX_PointMul(ak,atomTime/1000.f);
			pAtom->velocity.x-=ak.x;
			pAtom->velocity.y-=ak.y;
			pAtom->velocity.z-=ak.z;

			pAtom->position.x += pAtom->velocity.x * atomTime / 1000.f;
			pAtom->position.y += pAtom->velocity.y * atomTime / 1000.f;
			pAtom->position.z += pAtom->velocity.z * atomTime / 1000.f;
		}


		if (env->LauncherInfo.Update_func)
		{
			env->LauncherInfo.Update_func(env,pAtom);
		}
		

		if (pAtom->reg_size<0)
		{
			pAtom->reg_size=0;
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

		if (pAtom->reg_alpha<0)
		{
			pAtom->reg_alpha=0;
		}
	}
	

}

px_void PX_ParticalLauncherUpdate(PX_ParticalLauncher *launcher,px_dword elapsed)
{
	px_int i,j;
	px_int redTime=0;
	px_int gencount=0;
	PX_Partical_Atom *pAtom;

	launcher->lefttopX=0;
	launcher->leftTopY=0;
	launcher->rightBottomX=0;
	launcher->rightBottomY=0;

	for (i=0;i<(px_int)launcher->LauncherInfo.maxCount;i++)
	{
		pAtom=&launcher->ParticalPool[i];
		PX_ParticalAtomUpdate(launcher,pAtom,elapsed);
		if (pAtom->reg_alive!=0)
		{
			if (launcher->ParticalPool[i].position.x<launcher->lefttopX)
			{
				launcher->lefttopX=launcher->ParticalPool[i].position.x;
			}
			if (launcher->ParticalPool[i].position.x>launcher->rightBottomX)
			{
				launcher->rightBottomX=launcher->ParticalPool[i].position.x;
			}
			if (launcher->ParticalPool[i].position.y<launcher->leftTopY)
			{
				launcher->leftTopY=launcher->ParticalPool[i].position.y;
			}
			if (launcher->ParticalPool[i].position.y>launcher->rightBottomY)
			{
				launcher->rightBottomY=launcher->ParticalPool[i].position.y;
			}
		}
	}
	if(launcher->LauncherInfo.launchCount!=0)
	{
		if (launcher->LauncherInfo.generateDuration!=0)
		{
			if (launcher->elapsed/launcher->LauncherInfo.generateDuration<((launcher->elapsed+elapsed)/launcher->LauncherInfo.generateDuration))
			{
				gencount=(launcher->elapsed+elapsed)/launcher->LauncherInfo.generateDuration-launcher->elapsed/launcher->LauncherInfo.generateDuration;
				redTime=launcher->elapsed+elapsed-(launcher->elapsed+launcher->LauncherInfo.generateDuration)/launcher->LauncherInfo.generateDuration*launcher->LauncherInfo.generateDuration;
			}
			else
				gencount=0;
		}
		else
		{
			gencount=launcher->LauncherInfo.maxCount;
			redTime=elapsed;
		}
		

		for (i=0;i<gencount;i++)
		{
			if(launcher->LauncherInfo.launchCount!=0)
			{
				for (j=0;j<(px_int)launcher->LauncherInfo.maxCount;j++)
				{
					if (launcher->ParticalPool[j].reg_alive==0)
					{
						px_float dirAngle;
						px_float var;
						PX_memset(&launcher->ParticalPool[j],0,sizeof(launcher->ParticalPool[j]));
						launcher->lastgenIndex = j;
						//position
						launcher->ParticalPool[j].position = launcher->LauncherInfo.position;
						//velocity
						dirAngle = (px_float)PX_RadianToAngle(PX_atan2(launcher->LauncherInfo.direction.y, launcher->LauncherInfo.direction.x));
						dirAngle += (px_float)PX_randRange(-launcher->LauncherInfo.deviation_rangAngle, launcher->LauncherInfo.deviation_rangAngle);
						var = launcher->LauncherInfo.velocity;
						var += (px_float)PX_randRange(launcher->LauncherInfo.deviation_velocity_min, launcher->LauncherInfo.deviation_velocity_max);
						launcher->ParticalPool[j].velocity = PX_POINT(PX_cos_angle(dirAngle), PX_sin_angle(dirAngle), 0);
						launcher->ParticalPool[j].velocity = PX_PointMul(launcher->ParticalPool[j].velocity, var);
						//size
						var = launcher->LauncherInfo.atomsize;
						var += (px_float)PX_randRange(launcher->LauncherInfo.deviation_atomsize_min, launcher->LauncherInfo.deviation_atomsize_max);
						if (var < 0)var = 0;
						launcher->ParticalPool[j].reg_size = var;
						//rotation
						var = launcher->LauncherInfo.rotation;
						var += (px_float)PX_randRange(-launcher->LauncherInfo.deviation_rotation, launcher->LauncherInfo.deviation_rotation);
						launcher->ParticalPool[j].reg_rotation = var;
						//alpha
						var = launcher->LauncherInfo.alpha;
						var += (px_float)PX_randRange(-launcher->LauncherInfo.deviation_alpha, launcher->LauncherInfo.deviation_alpha);
						if (var < 0)var = 0;
						launcher->ParticalPool[j].reg_alpha = var;

						//hdrR
						var = launcher->LauncherInfo.hdrR;
						var += (px_float)PX_randRange(-launcher->LauncherInfo.deviation_hdrR, launcher->LauncherInfo.deviation_hdrR);
						if (var < 0)var = 0;
						launcher->ParticalPool[j].hdrR = var;

						//hdrG
						var = launcher->LauncherInfo.hdrG;
						var += (px_float)PX_randRange(-launcher->LauncherInfo.deviation_hdrG, launcher->LauncherInfo.deviation_hdrG);
						if (var < 0)var = 0;
						launcher->ParticalPool[j].hdrG = var;

						//hdrB
						var = launcher->LauncherInfo.hdrB;
						var += (px_float)PX_randRange(-launcher->LauncherInfo.deviation_hdrB, launcher->LauncherInfo.deviation_hdrB);
						if (var < 0)var = 0;
						launcher->ParticalPool[j].hdrB = var;

						//size inc
						launcher->ParticalPool[j].reg_sizeinc = launcher->LauncherInfo.sizeincrease;
						//alpha inc
						launcher->ParticalPool[j].reg_alphainc = launcher->LauncherInfo.alphaincrease;
						//a
						launcher->ParticalPool[j].reg_a = launcher->LauncherInfo.a;
						//ak
						launcher->ParticalPool[j].reg_ak = launcher->LauncherInfo.ak;
						//alive
						launcher->ParticalPool[j].reg_alive = launcher->LauncherInfo.alive;
						//texture
						launcher->ParticalPool[j].ptexture = launcher->LauncherInfo.tex;

						if (launcher->LauncherInfo.Create_func)
						{
							launcher->ParticalPool[j]=launcher->LauncherInfo.Create_func(launcher,launcher->genIndex);
						}

						PX_ParticalAtomUpdate(launcher,&launcher->ParticalPool[j],redTime);

						if (launcher->ParticalPool[j].position.x<launcher->lefttopX)
						{
							launcher->lefttopX=launcher->ParticalPool[j].position.x;
						}
						if (launcher->ParticalPool[j].position.x>launcher->rightBottomX)
						{
							launcher->rightBottomX=launcher->ParticalPool[j].position.x;
						}
						if (launcher->ParticalPool[j].position.y<launcher->leftTopY)
						{
							launcher->leftTopY=launcher->ParticalPool[j].position.y;
						}
						if (launcher->ParticalPool[j].position.y>launcher->rightBottomY)
						{
							launcher->rightBottomY=launcher->ParticalPool[j].position.y;
						}
						if(redTime>launcher->LauncherInfo.generateDuration)
						{
							redTime-=launcher->LauncherInfo.generateDuration;
						}
						else
						{
							redTime=0;
						}

						launcher->genIndex++;
						if (launcher->LauncherInfo.launchCount>0)
						{
							launcher->LauncherInfo.launchCount--;
						}
						break;
					}
				}
			}
			
		}
	}
	launcher->elapsed+=elapsed;
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

px_void PX_ParticalLauncherRender(px_surface *surface,PX_ParticalLauncher *launcher, px_dword elapsed)
{
	px_int m,i;
	PX_TEXTURERENDER_BLEND blend;
	PX_ParticalLauncherUpdate(launcher, elapsed);

	if (!PX_isRectCrossRect(PX_RECT(0,0,(px_float)surface->width,(px_float)surface->height),\
		PX_RECT(launcher->lefttopX+ launcher->LauncherInfo.position.x,launcher->leftTopY+ launcher->LauncherInfo.position.y,launcher->rightBottomX-launcher->lefttopX+1,launcher->rightBottomY-launcher->leftTopY+1)))
	{
		return;
	}

	i = launcher->lastgenIndex + 1;

	for (m =0; m <launcher->LauncherInfo.maxCount;m++,i++)
	{
		px_point pos;
		if (i>= launcher->LauncherInfo.maxCount)
		{
			i = 0;
		}

		pos=launcher->ParticalPool[i].position;

		if (!launcher->ParticalPool[i].ptexture)
		{
			continue;
		}

		if (launcher->ParticalPool[i].reg_alive!=0&&launcher->ParticalPool[i].reg_size!=0)
		{
			if (launcher->ParticalPool[i].reg_size!=1.0||launcher->ParticalPool[i].reg_currentrotation!=0)
			{
				if (launcher->ParticalPool[i].reg_size<0)
				{
					launcher->ParticalPool[i].reg_size=0;
					continue;
				}
				if(launcher->ParticalPool[i].reg_alpha==1.0&&launcher->ParticalPool[i].hdrR==1.0f&&launcher->ParticalPool[i].hdrG==1.0f&&launcher->ParticalPool[i].hdrB==1.0f)
					PX_TextureRenderEx(surface,launcher->ParticalPool[i].ptexture,(px_int)(pos.x),(px_int)(pos.y),PX_ALIGN_CENTER,PX_NULL,launcher->ParticalPool[i].reg_size,launcher->ParticalPool[i].reg_currentrotation);
				else
				{
					blend.alpha = launcher->ParticalPool[i].reg_alpha;
					blend.hdr_R=launcher->ParticalPool[i].hdrR;
					blend.hdr_G=launcher->ParticalPool[i].hdrG;
					blend.hdr_B=launcher->ParticalPool[i].hdrB;
					PX_TextureRenderEx(surface,launcher->ParticalPool[i].ptexture,(px_int)(pos.x),(px_int)(pos.y),PX_ALIGN_CENTER,&blend,launcher->ParticalPool[i].reg_size,launcher->ParticalPool[i].reg_currentrotation);
				}
			}
			else
			{
				if(launcher->ParticalPool[i].reg_alpha==1.0&&launcher->ParticalPool[i].hdrR==1.0f&&launcher->ParticalPool[i].hdrG==1.0f&&launcher->ParticalPool[i].hdrB==1.0f)
					PX_TextureRender(surface,launcher->ParticalPool[i].ptexture,(px_int)(pos.x),(px_int)(pos.y),PX_ALIGN_CENTER,PX_NULL);
				else
				{
					blend.alpha=launcher->ParticalPool[i].reg_alpha;
					blend.hdr_R=launcher->ParticalPool[i].hdrR;
					blend.hdr_G=launcher->ParticalPool[i].hdrG;
					blend.hdr_B=launcher->ParticalPool[i].hdrB;
					PX_TextureRender(surface,launcher->ParticalPool[i].ptexture,(px_int)(pos.x),(px_int)(pos.y),PX_ALIGN_CENTER,&blend);
				}
			}
		}
	}

}

px_void PX_ParticalLauncherFree(PX_ParticalLauncher *env)
{
	MP_Free(env->mp,env->ParticalPool);
}


px_void PX_ParticalLauncherSetDirection(PX_ParticalLauncher *launcher,px_point direction)
{
	launcher->LauncherInfo.direction=PX_PointNormalization(direction);
}

