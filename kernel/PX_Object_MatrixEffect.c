#include "PX_Object_MatrixEffect.h"


static px_surface PC_Ui_SurfaceChar[16][4];//ascii 3-25
static px_char PC_Ui_RandChar[16];
px_bool PC_Ui_SurfacebInitialized=PX_FALSE;

#define  PX_UI_MatrixConsole_CHARMAP(x,y) (effect->CharMap[y*effect->CharXCount+x])

px_void PX_Object_MatrixEffectCharactersInitialize(PX_Object_MatrixEffect *effect,px_surface *TargetSurface)
{

	px_int x,y,tx,ty;
	px_bool bNone;
	
	for(y=0;y<effect->CharYCount;y++)
	{
		for (x=0;x<effect->CharXCount;x++)
		{
			bNone=PX_TRUE;
			for (ty=y*PC_UI_CHAR_HEIGHT;ty<y*PC_UI_CHAR_HEIGHT+PC_UI_CHAR_HEIGHT;ty++)
			{
				for (tx=x*PC_UI_CHAR_WIDTH;tx<x*PC_UI_CHAR_WIDTH+PC_UI_CHAR_WIDTH;tx++)
				{
					if (PX_SurfaceGetPixel(TargetSurface,tx,ty)._argb.a)
					{
						bNone=PX_FALSE;
						break;
					}
				}
				if(!bNone) break;
			}

			PX_UI_MatrixConsole_CHARMAP(x,y).a=255000;
			PX_UI_MatrixConsole_CHARMAP(x,y).activated=PX_FALSE;
			PX_UI_MatrixConsole_CHARMAP(x,y).c=PX_rand()%16;
			PX_UI_MatrixConsole_CHARMAP(x,y).elpased=0;
			PX_UI_MatrixConsole_CHARMAP(x,y).i=0;
			if (bNone)
			{
				PX_UI_MatrixConsole_CHARMAP(x,y).t=effect->NONE_FLAG_TIMES;
			}
			else
			{
				PX_UI_MatrixConsole_CHARMAP(x,y).t=effect->EXISTING_FLAG_TIMES;
			}

		}
	}
	effect->Done=PX_FALSE;
}

px_void PX_Object_MatrixEffectCharactersDisplayMode1(PX_Object_MatrixEffect *effect,px_surface *TargetSurface)
{
	px_int oft=-48,i;
	PX_Object_MatrixEffectCharactersInitialize(effect,TargetSurface);
	
	for (i=0;i<effect->CharYCount;i++)
	{
     effect->CharAdvanceVertical[i]=oft+PX_rand()%24;
	}
	effect->PC_Ui_CharAdvanceType=PC_UI_ADVANCE_X_INC;
}

px_void PX_Object_MatrixEffectCharactersDisplayMode2(PX_Object_MatrixEffect *effect,px_surface *TargetSurface)
{
	px_int oft=-24,i;
	PX_Object_MatrixEffectCharactersInitialize(effect,TargetSurface);

	for (i=0;i<effect->CharXCount;i++)
	{
		effect->CharAdvanceHorizontal[i]=oft+(PX_rand()%24);
	}
	effect->PC_Ui_CharAdvanceType=PC_UI_ADVANCE_Y_INC;
}



px_bool PX_Object_MatrixEffectCharactorsRender(PX_Object_MatrixEffect *effect)
{
	px_int x,y;
	PX_TEXTURERENDER_BLEND blend;
	PX_SurfaceClear(&effect->SwitchTarget,0,0,effect->SwitchTarget.width,effect->SwitchTarget.height,PX_COLOR(255,0,0,0));
	
	
	for(y=0;y<effect->CharYCount;y++)
	for (x=0;x<effect->CharXCount;x++)
	{
		if (PX_UI_MatrixConsole_CHARMAP(x,y).activated&&PX_UI_MatrixConsole_CHARMAP(x,y).t>0)
		{
			//draw
			if (PX_UI_MatrixConsole_CHARMAP(x,y).i<4)
			{
				PX_SurfaceRender(&effect->SwitchTarget,\
					&PC_Ui_SurfaceChar[PX_UI_MatrixConsole_CHARMAP(x,y).c][PX_UI_MatrixConsole_CHARMAP(x,y).i],\
					x*PC_UI_CHAR_WIDTH+PC_UI_CHAR_WIDTH/2,\
					y*PC_UI_CHAR_HEIGHT+PC_UI_CHAR_HEIGHT/2,\
					PX_ALIGN_CENTER,\
					PX_NULL\
					);
			}
			else
			{
				PX_SurfaceRender(&effect->SwitchTarget,\
					&PC_Ui_SurfaceChar[PX_UI_MatrixConsole_CHARMAP(x,y).c][7-PX_UI_MatrixConsole_CHARMAP(x,y).i],\
					x*PC_UI_CHAR_WIDTH+PC_UI_CHAR_WIDTH/2,\
					y*PC_UI_CHAR_HEIGHT+PC_UI_CHAR_HEIGHT/2,\
					PX_ALIGN_CENTER,\
					PX_NULL\
					);
			}
		}
		else
		{
			if (PX_UI_MatrixConsole_CHARMAP(x,y).activated)
			{
				if (PX_UI_MatrixConsole_CHARMAP(x,y).a)
				{
					blend.alpha=PX_UI_MatrixConsole_CHARMAP(x,y).a/1000/255.0f;
					blend.hdr_B=1;
					blend.hdr_G=1;
					blend.hdr_R=1;
					
					PX_SurfaceSetRect(&effect->SwitchTarget,\
						x*PC_UI_CHAR_WIDTH,\
						y*PC_UI_CHAR_HEIGHT,\
						x*PC_UI_CHAR_WIDTH+PC_UI_CHAR_WIDTH-1,\
						y*PC_UI_CHAR_HEIGHT+PC_UI_CHAR_HEIGHT-1,\
						PX_COLOR(0,0,0,0)
						);

					PX_SurfaceRender(&effect->SwitchTarget,\
						&PC_Ui_SurfaceChar[PX_UI_MatrixConsole_CHARMAP(x,y).c][0],\
						x*PC_UI_CHAR_WIDTH+PC_UI_CHAR_WIDTH/2,\
						y*PC_UI_CHAR_HEIGHT+PC_UI_CHAR_HEIGHT/2,\
						PX_ALIGN_CENTER,\
						&blend\
						);
				}
				else
				{
					PX_SurfaceSetRect(&effect->SwitchTarget,\
						x*PC_UI_CHAR_WIDTH,\
						y*PC_UI_CHAR_HEIGHT,\
						x*PC_UI_CHAR_WIDTH+PC_UI_CHAR_WIDTH-1,\
						y*PC_UI_CHAR_HEIGHT+PC_UI_CHAR_HEIGHT-1,\
						PX_COLOR(0,0,0,0)
						);
				}
			}
			
		}
	}
	return PX_TRUE;
}

px_bool PX_Object_MatrixEffectCharUpdate(PX_Object_MatrixEffect *effect,px_uint elpased)
{
	px_int x,y,i;
	px_uint atom_op_time;
	if (effect->Done)
	{
		return PX_TRUE;
	}
	
	
	while (elpased)
	{
		if (elpased>effect->ATOM_OPRAND_TIME)
		{
			atom_op_time=effect->ATOM_OPRAND_TIME;
			elpased-=effect->ATOM_OPRAND_TIME;
		}
		else
		{
			atom_op_time=elpased;
			elpased=0;
		}
		//////////////////////////////////////////////////////////////////////////
		//Direction update
		effect->Done=PX_TRUE;
		switch(effect->PC_Ui_CharAdvanceType)
		{
		case PC_UI_ADVANCE_X_INC:
			{
				for (i=0;i<effect->CharYCount;i++)
				{
					if (effect->CharAdvanceVertical[i]<effect->CharXCount)
					{
						effect->Done=PX_FALSE;
						break;
					}
				}
			}
			break;
		case PC_UI_ADVANCE_X_DEC:
			{
				for (i=0;i<effect->CharYCount;i++)
				{
					if (effect->CharAdvanceVertical[i]>0)
					{
						effect->Done=PX_FALSE;
						break;
					}
				}
			}
			break;
		case PC_UI_ADVANCE_Y_INC:
			{
				for (i=0;i<effect->CharXCount;i++)
				{
					if (effect->CharAdvanceHorizontal[i]<effect->CharYCount)
					{
						effect->Done=PX_FALSE;
						break;
					}
				}
			}
			break;
		default:
		case PC_UI_ADVANCE_Y_DEC:
			{
				for (i=0;i<effect->CharXCount;i++)
				{
					if (effect->CharAdvanceHorizontal[i]>0)
					{
						effect->Done=PX_FALSE;
						break;
					}
				}
			}
			break;
		}


		effect->AdvanceElpased+=atom_op_time;
		if (effect->AdvanceElpased>=effect->ADVANCE_TIME)
		{
			effect->AdvanceElpased-=effect->ADVANCE_TIME;
			switch(effect->PC_Ui_CharAdvanceType)
			{
			case PC_UI_ADVANCE_X_INC:
				{
					for (i=0;i<effect->CharYCount;i++)
					{
						effect->CharAdvanceVertical[i]++;
						if (effect->CharAdvanceVertical[i]<effect->CharXCount)
						{
							if(effect->CharAdvanceVertical[i]>=0)
							PX_UI_MatrixConsole_CHARMAP(effect->CharAdvanceVertical[i],i).activated=PX_TRUE;
							effect->Done=PX_FALSE;
						}
						else
						{
							effect->CharAdvanceVertical[i]=effect->CharXCount;
						}
					}
				}
				break;
			case PC_UI_ADVANCE_X_DEC:
				{
					for (i=0;i<effect->CharYCount;i++)
					{
						effect->CharAdvanceVertical[i]--;
						if (effect->CharAdvanceVertical[i]>=0)
						{
							if(effect->CharAdvanceVertical[i]<effect->CharYCount)
							PX_UI_MatrixConsole_CHARMAP(effect->CharAdvanceVertical[i],i).activated=PX_TRUE;
							effect->Done=PX_FALSE;
						}
						else
						{
							effect->CharAdvanceVertical[i]=0;
						}
					}
				}
				break;
			case PC_UI_ADVANCE_Y_INC:
				{
					for (i=0;i<effect->CharXCount;i++)
					{
						effect->CharAdvanceHorizontal[i]++;
						if (effect->CharAdvanceHorizontal[i]<effect->CharYCount)
						{
							if(effect->CharAdvanceHorizontal[i]>=0)
							PX_UI_MatrixConsole_CHARMAP(i,effect->CharAdvanceHorizontal[i]).activated=PX_TRUE;
							effect->Done=PX_FALSE;
						}
						else
						{
							effect->CharAdvanceHorizontal[i]=effect->CharYCount;
						}
					}
				}
				break;
			default:
			case PC_UI_ADVANCE_Y_DEC:
				{
					for (i=0;i<effect->CharXCount;i++)
					{
						effect->CharAdvanceHorizontal[i]--;
						if (effect->CharAdvanceHorizontal[i]>=0)
						{
							if(effect->CharAdvanceHorizontal[i]<effect->CharXCount)
							PX_UI_MatrixConsole_CHARMAP(i,effect->CharAdvanceHorizontal[i]).activated=PX_TRUE;
							effect->Done=PX_FALSE;
						}
						else
						{
							effect->CharAdvanceHorizontal[i]=0;
						}
					}
				}
				break;
			}
		}

		//////////////////////////////////////////////////////////////////////////
		//Characters update
		for(y=0;y<effect->CharYCount;y++)
		{
			for (x=0;x<effect->CharXCount;x++)
			{
				if (PX_UI_MatrixConsole_CHARMAP(x,y).activated)
				{
					if (PX_UI_MatrixConsole_CHARMAP(x,y).t>0)
					{
						effect->Done=PX_FALSE;
						if (PX_UI_MatrixConsole_CHARMAP(x,y).elpased+atom_op_time>=effect->SWITCH_DURATION)
						{
							//status switch
							PX_UI_MatrixConsole_CHARMAP(x,y).elpased+=atom_op_time-effect->SWITCH_DURATION;
							
							if (PX_UI_MatrixConsole_CHARMAP(x,y).i<7)
							{
								PX_UI_MatrixConsole_CHARMAP(x,y).i++;
							}
							else
							{
								PX_UI_MatrixConsole_CHARMAP(x,y).c=PX_rand()%16;
								PX_UI_MatrixConsole_CHARMAP(x,y).i=0;
								PX_UI_MatrixConsole_CHARMAP(x,y).t--;
							}
						}
						else
						{
							PX_UI_MatrixConsole_CHARMAP(x,y).elpased+=atom_op_time;
						}
					}
					else
					{
						if (PX_UI_MatrixConsole_CHARMAP(x,y).a>0)
						{
							effect->Done=PX_FALSE;
							if (PX_UI_MatrixConsole_CHARMAP(x,y).a>effect->ALPHA_DEC_SPEED*atom_op_time)
							{
								PX_UI_MatrixConsole_CHARMAP(x,y).a-=effect->ALPHA_DEC_SPEED*atom_op_time;
							}
							else
							{
								PX_UI_MatrixConsole_CHARMAP(x,y).a=0;
							}
						}
					}
					
				}
			}
		}
	}
	return PX_TRUE;
}

px_bool PX_Object_MatrixEffectSwitch(PX_Object_MatrixEffect *effect,px_surface *snapshot,px_int mode)
{
	switch (mode)
	{
	case 1:
		PX_Object_MatrixEffectCharactersDisplayMode1(effect,snapshot);
		break;
	default:
	case 2:
		PX_Object_MatrixEffectCharactersDisplayMode2(effect,snapshot);
	}
	return PX_TRUE;
}


px_bool PX_Object_MatrixEffectUpdateEx(PX_Object_MatrixEffect *effect,px_uint elpased)
{
	if(!effect->Done)
	{
	if(!PX_Object_MatrixEffectCharUpdate(effect,elpased)) return PX_FALSE;
	
	if(!PX_Object_MatrixEffectCharactorsRender(effect))return PX_FALSE;
	}
	return PX_TRUE;
}

px_void PX_Object_MatrixEffectRenderEx(PX_Object_MatrixEffect *effect,px_surface *targetSurface)
{
	if(!effect->Done)
		PX_SurfaceRender(targetSurface,&effect->SwitchTarget,0,0,PX_ALIGN_LEFTTOP,PX_NULL);
}

px_void PX_Object_MatrixEffectFreeEx(PX_Object_MatrixEffect *effect)
{
	px_int i,j;
	for (i=0;i<16;i++)
	{
		for (j=0;j<4;j++)
		{
			PX_SurfaceFree(&PC_Ui_SurfaceChar[i][j]);
		}
	}
	PX_SurfaceFree(&effect->SwitchTarget);
	MP_Free(effect->mp,effect->CharAdvanceHorizontal);
	MP_Free(effect->mp,effect->CharAdvanceVertical);
	MP_Free(effect->mp,effect->CharMap);
	PC_Ui_SurfacebInitialized=PX_FALSE;
}

PX_OBJECT_RENDER_FUNCTION(PX_Object_MatrixEffectRender)
{
	PX_Object_MatrixEffect *effect=(PX_Object_MatrixEffect *)pObject->pObjectDesc;
	PX_Object_MatrixEffectUpdateEx(effect,elapsed);
	PX_Object_MatrixEffectRenderEx(effect,psurface);
	if (effect->Done)
	{
		PX_ObjectDelayDelete(pObject);
	}
}

PX_OBJECT_FREE_FUNCTION(PX_Object_MatrixEffectFree)
{
	PX_Object_MatrixEffect* effect = (PX_Object_MatrixEffect*)pObject->pObjectDesc;
	PX_Object_MatrixEffectFreeEx(effect);
}

px_bool PX_Object_MatrixEffectCreate(px_memorypool* mp, PX_Object* parent, px_surface *pMapsurface, px_int Mode_0or1)
{
	px_int i, j;
	PX_Object_MatrixEffect* pDesc;
	PX_Object* pObject;

	pObject=PX_ObjectCreateEx(mp, parent, 0, 0, 0, pMapsurface->width*1.0f, pMapsurface->height*1.0f, 0, PX_OBJECT_TYPE_MATRIXEFFECT, 0, PX_Object_MatrixEffectRender, PX_NULL, PX_NULL, sizeof(PX_Object_MatrixEffect));
	pDesc=PX_ObjectGetDesc(PX_Object_MatrixEffect, pObject);

	pDesc->mp = mp;

	pDesc->SWITCH_DURATION = 20;
	pDesc->ALPHA_DEC_SPEED = 500;
	pDesc->ATOM_OPRAND_TIME = 25;
	pDesc->EXISTING_FLAG_TIMES = 12;
	pDesc->NONE_FLAG_TIMES = 6;
	pDesc->ADVANCE_TIME = 50;
	pDesc->Done = PX_TRUE;
	pDesc->AdvanceElpased = 0;

	if (!PC_Ui_SurfacebInitialized)
	{
		for (i = 48; i <= 57; i++)
		{
			PC_Ui_RandChar[i - 48] = i;
		}
		PC_Ui_RandChar[10] = 'A';
		PC_Ui_RandChar[11] = 'B';
		PC_Ui_RandChar[12] = 'C';
		PC_Ui_RandChar[13] = 'D';
		PC_Ui_RandChar[14] = 'E';
		PC_Ui_RandChar[15] = 'F';

		for (i = 0; i < 16; i++)
		{
			for (j = 0; j < 4; j++)
			{
				if (!PX_SurfaceCreate(pDesc->mp, PC_UI_CHAR_WIDTH * 2, PC_UI_CHAR_HEIGHT * 2, &PC_Ui_SurfaceChar[i][j]))
				{
					goto _ERROR;
				}

			}
		}
		for (i = 0; i < 16; i++)
		{
			for (j = 0; j < 4; j++)
			{
				PX_FontDrawChar(&PC_Ui_SurfaceChar[i][j], PC_UI_CHAR_WIDTH / 2, PC_UI_CHAR_HEIGHT / 2, PC_Ui_RandChar[i], PX_COLOR(255, 0, 240, 64));
				PX_EffectShine(&PC_Ui_SurfaceChar[i][j], j + 1, PX_COLOR(255, 0, 255, 0), 0.3f * j);
			}
		}
		PC_Ui_SurfacebInitialized = PX_TRUE;
	}


	if (!PX_SurfaceCreate(mp, pMapsurface->width, pMapsurface->height, &pDesc->SwitchTarget))
		goto _ERROR;



	//generate map
	pDesc->CharYCount = pDesc->SwitchTarget.height / PC_UI_CHAR_HEIGHT;
	pDesc->CharXCount = pDesc->SwitchTarget.width / PC_UI_CHAR_WIDTH;
	pDesc->CharMap = (PX_UI_MATRIXEFFECT_MAP*)MP_Malloc(mp, pDesc->CharYCount * pDesc->CharXCount * sizeof(PX_UI_MATRIXEFFECT_MAP));
	pDesc->CharAdvanceHorizontal = (px_int*)MP_Malloc(mp, (pDesc->CharXCount) * sizeof(px_int));
	pDesc->CharAdvanceVertical = (px_int*)MP_Malloc(mp, (pDesc->CharYCount) * sizeof(px_int));

	if (pDesc->CharMap == PX_NULL)
	{
		goto _ERROR;
	}
	PX_Object_MatrixEffectSwitch(pDesc, pMapsurface, Mode_0or1);
	return PX_TRUE;
_ERROR:
	return PX_FALSE;

}