#include "PX_Animation.h"

px_bool PX_AnimationLibraryCreateFromMemory(px_memorypool *mp,px_animationlibrary *panimation,px_byte *_2dxBuffer,px_uint size)
{
	PX_2DX_Header _header;
	PX_TRaw_Header _trawheader;
	px_int i;
	px_byte *pbuffer;
	px_uint reservedSize=size;
	px_texture texture;
	PX_2DX_CODE_Header _codeheader;

	_header=*(PX_2DX_Header *)_2dxBuffer;
	if (!PX_memequ(&_header.magic,"2DX",sizeof(_header.magic)))
	{
		return PX_FALSE;
	}
	reservedSize-=sizeof(_header);
	pbuffer=_2dxBuffer+sizeof(_header);
	PX_VectorInit(mp,&panimation->animation,sizeof(PX_Animationlibrary_tagInfo),_header.animationCount);
	for (i=0;i<(px_int)_header.animationCount;i++)
	{
		PX_Animationlibrary_tagInfo tag;
		tag.ip=*(px_dword *)pbuffer;
		pbuffer+=sizeof(px_dword);
		PX_StringInit(mp,&tag.name);
		PX_StringCat(&tag.name,(px_char *)pbuffer);
		pbuffer+=PX_strlen((px_char *)pbuffer)+1;
		PX_VectorPushback(&panimation->animation,&tag);
	}


	PX_VectorInit(mp,&panimation->frames,sizeof(px_texture),_header.framecount);
	PX_MemoryInit(mp,&panimation->code);
	for (i=0;i<(px_int)_header.framecount;i++)
	{
		_trawheader=*(PX_TRaw_Header *)pbuffer;

		if (!PX_TextureCreateFromMemory(mp,pbuffer,reservedSize,&texture))
		{
			goto _ERROR;
		}
		PX_VectorPushback(&panimation->frames,&texture);
		pbuffer+=PX_TRawGetSize(&_trawheader);
		reservedSize-=PX_TRawGetSize(&_trawheader);
	}

	if (reservedSize<sizeof(_codeheader))
	{
		goto _ERROR;
	}

	_codeheader=*(PX_2DX_CODE_Header *)pbuffer;
	pbuffer+=sizeof(_codeheader);
	reservedSize-=sizeof(_codeheader);

	if (reservedSize<_codeheader.size)
	{
		goto _ERROR;
	}

	PX_MemoryCat(&panimation->code,pbuffer,_codeheader.size);

	return PX_TRUE;
	

_ERROR:
	for (i=0;i<panimation->frames.size;i++)
	{
		PX_TextureFree(PX_VECTORAT(px_texture,&panimation->frames,i));
	}
	PX_VectorFree(&panimation->frames);
	PX_MemoryFree(&panimation->code);
	return PX_FALSE;
}

px_void PX_AnimationLibraryFree(px_animationlibrary *panimation)
{
	px_int i;
	for (i=0;i<panimation->animation.size;i++)
	{
		PX_Animationlibrary_tagInfo *tagInfo=PX_VECTORAT(PX_Animationlibrary_tagInfo,&panimation->animation,i);
		PX_StringFree(&tagInfo->name);
	}
	PX_VectorFree(&panimation->animation);
	for (i=0;i<panimation->frames.size;i++)
	{
		PX_TextureFree(PX_VECTORAT(px_texture,&panimation->frames,i));
	}
	PX_VectorFree(&panimation->frames);
	PX_MemoryFree(&panimation->code);
}

px_void PX_AnimationUpdate(px_animation *panimation,px_uint elpased)
{
	PX_2DX_INSTR *pInstr;

	if (!panimation->linker)
	{
		return;
	}
	if (panimation->reg_reservedTime>=elpased)
	{
		panimation->reg_reservedTime-=elpased;
		return;
	}
	else
	{
		elpased-=panimation->reg_reservedTime;
	}

	while(elpased)
	{
		if (panimation->ip>panimation->linker->code.usedsize-sizeof(PX_2DX_INSTR))
		{
			PX_ERROR("2dx code error");
			return;
		}
		pInstr=(PX_2DX_INSTR *)(panimation->linker->code.buffer+panimation->ip);
		switch(pInstr->opcode)
		{
		case PX_2DX_OPCODE_END:
			return;
		case PX_2DX_OPCODE_FRAME:
			if(pInstr->param<panimation->linker->frames.size)
			panimation->reg_currentFrameIndex=pInstr->param;
			else
			{
				PX_ERROR("2dx code error");
				return;
			}
			panimation->ip+=sizeof(PX_2DX_INSTR);
			break;
		case PX_2DX_OPCODE_GOTO:

			if(panimation->reg_loopTimes!=0)
			{
				panimation->ip=pInstr->param;
				if(panimation->reg_loopTimes!=0xffff)
				{
				panimation->reg_loopTimes--;
				}
			}
			else
				panimation->ip+=sizeof(PX_2DX_INSTR);

			break;
		case PX_2DX_OPCODE_LOOP:
			panimation->reg_loopTimes=pInstr->param;
			panimation->ip+=sizeof(PX_2DX_INSTR);
			break;
		case PX_2DX_OPCODE_SLEEP:

			if (pInstr->param>elpased)
			{
				panimation->reg_reservedTime=pInstr->param-elpased;
				panimation->ip+=sizeof(PX_2DX_INSTR);
				return;
			}
			else
			{
				elpased-=pInstr->param;
				panimation->ip+=sizeof(PX_2DX_INSTR);
			}
			break;
		}
	}
}

px_void PX_AnimationRender(px_surface *psurface,px_animation *animation,px_point pos,PX_TEXTURERENDER_REFPOINT refPoint,PX_TEXTURERENDER_BLEND *blend)
{
	px_texture *pTexture;
	if (!animation->linker)
	{
		return;
	}
	if(animation->reg_currentFrameIndex>=0&&animation->reg_currentFrameIndex<animation->linker->frames.size)
	{
		pTexture=PX_VECTORAT(px_texture,&animation->linker->frames,animation->reg_currentFrameIndex);

		if (animation->scale!=1)
		{
			PX_TextureRenderEx_vector(psurface,pTexture,(px_int)pos.x,(px_int)pos.y,refPoint,blend,animation->scale,animation->direction);
		}
		else
		{
			if (animation->direction.x!=1)
			{
				PX_TextureRenderRotation_vector(psurface,pTexture,(px_int)pos.x,(px_int)pos.y,refPoint,blend,animation->direction);
			}
			else
			{
				PX_TextureRender(psurface,pTexture,(px_int)pos.x,(px_int)pos.y,refPoint,blend);
			}
		}
	}
}

px_bool PX_AnimationCreate(px_animation *animation,px_animationlibrary *linker)
{
	animation->elpased=0;
	animation->linker=linker;
	animation->reg_currentFrameIndex=-1;
	animation->reg_loopTimes=0;
	animation->reg_reservedTime=0;
	animation->ip=0;
	animation->scale=1;
	animation->direction=PX_POINT(1,0,0);
	return PX_TRUE;
}

px_bool PX_AnimationSetLibrary(px_animation *animation,px_animationlibrary *linker)
{
	PX_AnimationFree(animation);
	animation->elpased=0;
	animation->linker=linker;
	animation->reg_currentFrameIndex=-1;
	animation->reg_loopTimes=0;
	animation->reg_reservedTime=0;
	animation->ip=0;
	return PX_TRUE;
}

px_void PX_AnimationSetDirection(px_animation *animation,px_point direction)
{
	animation->direction=direction;
}

px_void PX_AnimationSetScale(px_animation *animation,px_float scale)
{
	animation->scale=scale;
}

px_void PX_AnimationFree(px_animation *animation)
{
	
}

px_void PX_AnimationReset(px_animation *animation)
{
	animation->elpased=0;
	animation->reg_currentFrameIndex=-1;
	animation->reg_loopTimes=0;
	animation->reg_reservedTime=0;
	animation->ip=0;
}

px_bool PX_AnimationLibrary_CreateEffect_JumpVertical(px_memorypool *mp,px_animationlibrary *panimation,px_texture *effectTexture)
{
	px_int i;
	px_int volume=effectTexture->width*effectTexture->height;
	px_int eff_height[6],eff_width[6];
	px_texture tex;

	PX_VectorInit(mp,&panimation->frames,sizeof(px_texture),6);
	PX_MemoryInit(mp,&panimation->code);
	PX_MemoryResize(&panimation->code,sizeof(PX_2DX_INSTR)*13);

	PX_AnimationLibraryAddInstr(panimation,PX_2DX_OPCODE_FRAME,0);
	PX_AnimationLibraryAddInstr(panimation,PX_2DX_OPCODE_SLEEP,50);
	PX_AnimationLibraryAddInstr(panimation,PX_2DX_OPCODE_FRAME,1);
	PX_AnimationLibraryAddInstr(panimation,PX_2DX_OPCODE_SLEEP,50);
	PX_AnimationLibraryAddInstr(panimation,PX_2DX_OPCODE_FRAME,2);
	PX_AnimationLibraryAddInstr(panimation,PX_2DX_OPCODE_SLEEP,50);
	PX_AnimationLibraryAddInstr(panimation,PX_2DX_OPCODE_FRAME,3);
	PX_AnimationLibraryAddInstr(panimation,PX_2DX_OPCODE_SLEEP,50);
	PX_AnimationLibraryAddInstr(panimation,PX_2DX_OPCODE_FRAME,4);
	PX_AnimationLibraryAddInstr(panimation,PX_2DX_OPCODE_SLEEP,50);
	PX_AnimationLibraryAddInstr(panimation,PX_2DX_OPCODE_FRAME,5);
	PX_AnimationLibraryAddInstr(panimation,PX_2DX_OPCODE_SLEEP,50);
	PX_AnimationLibraryAddInstr(panimation,PX_2DX_OPCODE_END,0);

	eff_height[0]=(px_int)(effectTexture->height*0.2);
	eff_height[1]=(px_int)(effectTexture->height*0.25);
	eff_height[2]=(px_int)(effectTexture->height*0.45);
	eff_height[3]=(px_int)(effectTexture->height*1.25);
	eff_height[4]=(px_int)(effectTexture->height*1.15);
	eff_height[5]=(px_int)(effectTexture->height*1);

	for(i=0;i<6;i++)
	{
		eff_width[i]=volume/eff_height[i];
		if (eff_width[i]>effectTexture->width*2)
		{
			eff_width[i]=(px_int)(eff_width[i]*0.4);
		}
		if(!PX_TextureCreateScale(mp,effectTexture,eff_width[i],eff_height[i],&tex))
		{
			PX_AnimationLibraryFree(panimation);
			return PX_FALSE;
		}
		PX_VectorPushback(&panimation->frames,&tex);
	}
	return PX_TRUE;
}

px_void PX_AnimationLibraryAddInstr(px_animationlibrary *panimationLib,PX_2DX_OPCODE opcode,px_word param)
{
	PX_2DX_INSTR instr;
	instr.opcode=opcode;
	instr.param=param;
	PX_MemoryCat(&panimationLib->code,&instr,sizeof(PX_2DX_INSTR));
}

px_bool PX_AnimationIsEnd(px_animation *panimation)
{
	if (!panimation->linker)
	{
		return PX_TRUE;
	}
	return (((PX_2DX_INSTR *)(panimation->linker->code.buffer+panimation->ip))->opcode==PX_2DX_OPCODE_END);
}


px_rect PX_AnimationGetSize(px_animation *panimation)
{
	px_rect rect;
	px_int i,w=0,h=0;
	px_texture *pTexture;
	if (!panimation->linker)
	{
		rect.x=0;
		rect.y=0;
		rect.height=0;
		rect.width=0;
		return rect;
	}
	for (i=0;i<panimation->linker->frames.size;i++)
	{
		pTexture=PX_VECTORAT(px_texture,&panimation->linker->frames,i);
		if (w<pTexture->width)
		{
			w=pTexture->width;
		}
		if (h<pTexture->height)
		{
			h=pTexture->height;
		}
	}
	rect.x=0;
	rect.y=0;
	rect.height=(px_float)h;
	rect.width=(px_float)w;
	return rect;
}

px_int PX_AnimationGetAnimationsCount(px_animation *animation)
{
	if(animation->linker)
	return animation->linker->frames.size;
	else
		return 0;
}

px_bool PX_AnimationSetCurrentPlayAnimation(px_animation *animation,px_int i)
{
	if(animation->linker)
	if (i<animation->linker->animation.size)
	{
		PX_Animationlibrary_tagInfo *tag=PX_VECTORAT(PX_Animationlibrary_tagInfo,&animation->linker->animation,i);
		animation->ip=tag->ip;
		return PX_TRUE;
	}
	return PX_FALSE;
}

px_bool PX_AnimationSetCurrentPlayAnimationByName(px_animation *animation,px_char *name)
{
	px_int i;
	if(animation->linker)
		for (i=0;i<animation->linker->animation.size;i++)
		{
			PX_Animationlibrary_tagInfo *tag=PX_VECTORAT(PX_Animationlibrary_tagInfo,&animation->linker->animation,i);
			if (PX_strequ(name,tag->name.buffer))
			{
				animation->ip=tag->ip;
				animation->reg_reservedTime=0;
				return PX_TRUE;
			}
		}
		return PX_FALSE;
}


px_int PX_AnimationGetPlayAnimationIndexByName(px_animation *animation,px_char *name)
{
	px_int i;
	if(animation->linker)
		for (i=0;i<animation->linker->animation.size;i++)
		{
			PX_Animationlibrary_tagInfo *tag=PX_VECTORAT(PX_Animationlibrary_tagInfo,&animation->linker->animation,i);
			if (PX_strequ(name,tag->name.buffer))
			{
				return i;
			}
		}
		return -1;
}
