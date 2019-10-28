#include "MatrixEffect_ui.h"


static px_surface PC_Ui_SurfaceChar[16][4];//ascii 3-25
static px_char PC_Ui_RandChar[16];
px_bool PC_Ui_SurfacebInitialized=PX_FALSE;

#define  PX_UI_MatrixConsole_CHARMAP(x,y) (effect->CharMap[y*effect->CharXCount+x])

px_void PX_Ui_MatrixEffectCharactersInitialize(PX_Ui_MatrixEffect *effect,px_surface *TargetSurface)
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

px_void PX_Ui_MatrixEffectCharactersDisplayMode1(PX_Ui_MatrixEffect *effect,px_surface *TargetSurface)
{
	int oft=-48,i;
	PX_Ui_MatrixEffectCharactersInitialize(effect,TargetSurface);
	
	for (i=0;i<effect->CharYCount;i++)
	{
     effect->CharAdvanceVertical[i]=oft+PX_rand()%24;
	}
	effect->PC_Ui_CharAdvanceType=PC_UI_ADVANCE_X_INC;
}

px_void PX_Ui_MatrixEffectCharactersDisplayMode2(PX_Ui_MatrixEffect *effect,px_surface *TargetSurface)
{
	int oft=-24,i;
	PX_Ui_MatrixEffectCharactersInitialize(effect,TargetSurface);

	for (i=0;i<effect->CharXCount;i++)
	{
		effect->CharAdvanceHorizontal[i]=oft+(PX_rand()%24);
	}
	effect->PC_Ui_CharAdvanceType=PC_UI_ADVANCE_Y_INC;
}


px_bool PX_Ui_MatrixEffectInitialize(px_memorypool *mp,PX_Ui_MatrixEffect *effect,px_int width,px_int height)
{
	int i,j;

	effect->mp=mp;

	effect->SWITCH_DURATION=20;
	effect->ALPHA_DEC_SPEED=500;
	effect->ATOM_OPRAND_TIME=25;
	effect->EXISTING_FLAG_TIMES=12;
	effect->NONE_FLAG_TIMES=6;
	effect->ADVANCE_TIME=50;
	effect->Done=PX_TRUE;
	effect->AdvanceElpased=0;

	if(!PC_Ui_SurfacebInitialized)
	{
		for (i=48;i<=57;i++)
		{
			PC_Ui_RandChar[i-48]=i;
		}
		PC_Ui_RandChar[10]='A';
		PC_Ui_RandChar[11]='B';
		PC_Ui_RandChar[12]='C';
		PC_Ui_RandChar[13]='D';
		PC_Ui_RandChar[14]='E';
		PC_Ui_RandChar[15]='F';

		for (i=0;i<16;i++)
		{
			for (j=0;j<4;j++)
			{
				if (!PX_SurfaceCreate(effect->mp,PC_UI_CHAR_WIDTH*2,PC_UI_CHAR_HEIGHT*2,&PC_Ui_SurfaceChar[i][j]))
				{
					goto _ERROR;
				}

			}
		}
		for (i=0;i<16;i++)
		{
			for (j=0;j<4;j++)
			{
				PX_FontDrawASCII(&PC_Ui_SurfaceChar[i][j],PC_UI_CHAR_WIDTH/2,PC_UI_CHAR_HEIGHT/2,PC_Ui_RandChar[i],PX_COLOR(255,0,240,64));
				PX_EffectShine(&PC_Ui_SurfaceChar[i][j],j+1,PX_COLOR(255,0,255,0),0.3f*j);
			}
		}
		PC_Ui_SurfacebInitialized=PX_TRUE;
	}
	


	if(!PX_SurfaceCreate(mp,width,height,&effect->SwitchTarget))
		goto _ERROR;

	

	//generate map
	effect->CharYCount=effect->SwitchTarget.height/PC_UI_CHAR_HEIGHT;
	effect->CharXCount=effect->SwitchTarget.width/PC_UI_CHAR_WIDTH;
	effect->CharMap=(PX_UI_MATRIXEFFECT_MAP *)MP_Malloc(mp,effect->CharYCount*effect->CharXCount*sizeof(PX_UI_MATRIXEFFECT_MAP));
	effect->CharAdvanceHorizontal=(px_int *)MP_Malloc(mp,(effect->CharXCount)*sizeof(px_int));
	effect->CharAdvanceVertical=(px_int *)MP_Malloc(mp,(effect->CharYCount)*sizeof(px_int));

	if (effect->CharMap==PX_NULL)
	{
		goto _ERROR;
	}

	return PX_TRUE;
_ERROR:
	return PX_FALSE;

}



px_bool PX_Ui_MatrixEffectCharactorsRender(PX_Ui_MatrixEffect *effect)
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
					PX_TEXTURERENDER_REFPOINT_CENTER,\
					PX_NULL\
					);
			}
			else
			{
				PX_SurfaceRender(&effect->SwitchTarget,\
					&PC_Ui_SurfaceChar[PX_UI_MatrixConsole_CHARMAP(x,y).c][7-PX_UI_MatrixConsole_CHARMAP(x,y).i],\
					x*PC_UI_CHAR_WIDTH+PC_UI_CHAR_WIDTH/2,\
					y*PC_UI_CHAR_HEIGHT+PC_UI_CHAR_HEIGHT/2,\
					PX_TEXTURERENDER_REFPOINT_CENTER,\
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
						PX_TEXTURERENDER_REFPOINT_CENTER,\
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

px_bool PX_Ui_MatrixEffectCharUpdate(PX_Ui_MatrixEffect *effect,px_uint elpased)
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

px_bool PX_Ui_MatrixEffectSwitch(PX_Ui_MatrixEffect *effect,px_surface *snapshot,px_int mode)
{
	switch (mode)
	{
	case 1:
		PX_Ui_MatrixEffectCharactersDisplayMode1(effect,snapshot);
		break;
	default:
	case 2:
		PX_Ui_MatrixEffectCharactersDisplayMode2(effect,snapshot);
	}
	return PX_TRUE;
}


px_bool PX_Ui_MatrixEffectUpdate(PX_Ui_MatrixEffect *effect,px_uint elpased)
{
	if(!effect->Done)
	{
	if(!PX_Ui_MatrixEffectCharUpdate(effect,elpased)) return PX_FALSE;
	
	if(!PX_Ui_MatrixEffectCharactorsRender(effect))return PX_FALSE;
	}
	return PX_TRUE;
}

px_void PX_Ui_MatrixEffectRender(PX_Ui_MatrixEffect *effect,px_surface *targetSurface)
{
	if(!effect->Done)
	PX_SurfaceRender(targetSurface,&effect->SwitchTarget,0,0,PX_TEXTURERENDER_REFPOINT_LEFTTOP,PX_NULL);
}

px_void PX_Ui_MatrixEffectFree(PX_Ui_MatrixEffect *effect)
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
