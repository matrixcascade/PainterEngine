#include "PX_Animation.h"

px_bool PX_AnimationLibraryCreateFromMemory(px_memorypool *mp,PX_AnimationLibrary *panimation,px_byte *_2dxBuffer,px_uint size)
{
	PX_2DX_Header _header;
	PX_TRaw_Header _trawheader;
	px_int i,j;
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
	PX_VectorInitialize(mp,&panimation->animation,sizeof(PX_Animationlibrary_tagInfo),_header.animationCount);
	for (i=0;i<(px_int)_header.animationCount;i++)
	{
		PX_Animationlibrary_tagInfo tag;
		tag.ip=*(px_dword *)pbuffer;
		pbuffer+=sizeof(px_dword);
		PX_StringInitialize(mp,&tag.name);
		PX_StringCat(&tag.name,(px_char *)pbuffer);
		pbuffer+=PX_strlen((px_char *)pbuffer)+1;
		PX_VectorPushback(&panimation->animation,&tag);
	}


	PX_VectorInitialize(mp,&panimation->frames,sizeof(px_texture),_header.framecount);
	PX_MemoryInitialize(mp,&panimation->code);
	for (i=0;i<(px_int)_header.framecount;i++)
	{
		for(j=0;j<sizeof(PX_TRaw_Header);j++)
		{
			*(((px_byte *)&_trawheader)+j)=*(pbuffer+j);
		}

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

px_void PX_AnimationLibraryFree(PX_AnimationLibrary *panimation)
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

px_void PX_AnimationUpdate(PX_Animation *panimation,px_uint elapsed)
{
	PX_2DX_INSTR *pInstr;

	if (!panimation->linker)
	{
		return;
	}
	if (panimation->reg_reservedTime>=elapsed)
	{
		panimation->reg_reservedTime-=elapsed;
		return;
	}
	else
	{
		elapsed-=panimation->reg_reservedTime;
	}

	while(elapsed)
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

			if (pInstr->param>elapsed)
			{
				panimation->reg_reservedTime=pInstr->param-elapsed;
				panimation->ip+=sizeof(PX_2DX_INSTR);
				return;
			}
			else
			{
				elapsed-=pInstr->param;
				panimation->ip+=sizeof(PX_2DX_INSTR);
			}
			break;
		}
	}
}

px_void PX_AnimationRender(px_surface *psurface,PX_Animation *animation,px_int x,px_int y,PX_ALIGN refPoint,PX_TEXTURERENDER_BLEND *blend)
{
	px_texture *pTexture;
	if (!animation->linker)
	{
		return;
	}
	if(animation->reg_currentFrameIndex>=0&&animation->reg_currentFrameIndex<animation->linker->frames.size)
	{
		pTexture=PX_VECTORAT(px_texture,&animation->linker->frames,animation->reg_currentFrameIndex);
		PX_TextureRender(psurface,pTexture,x,y,refPoint,blend);
	}
}

px_void PX_AnimationRenderEx(px_surface *psurface,PX_Animation *animation,px_int x,px_int y,px_float scale,px_point direction,PX_ALIGN refPoint,PX_TEXTURERENDER_BLEND *blend)
{
	px_texture *pTexture;
	if (!animation->linker)
	{
		return;
	}
	if(animation->reg_currentFrameIndex>=0&&animation->reg_currentFrameIndex<animation->linker->frames.size)
	{
		pTexture=PX_VECTORAT(px_texture,&animation->linker->frames,animation->reg_currentFrameIndex);

		if (scale!=1)
		{
			PX_TextureRenderEx_vector(psurface,pTexture,x,y,refPoint,blend,scale,direction);
		}
		else
		{
			PX_TextureRenderRotation_vector(psurface,pTexture,x,y,refPoint,blend,direction);
		}
	}
}

px_void PX_AnimationRender_scale(px_surface *psurface,PX_Animation *animation,px_int x,px_int y,px_float scale,PX_ALIGN refPoint,PX_TEXTURERENDER_BLEND *blend)
{
	px_texture *pTexture;
	if (!animation->linker)
	{
		return;
	}
	if(animation->reg_currentFrameIndex>=0&&animation->reg_currentFrameIndex<animation->linker->frames.size)
	{
		pTexture=PX_VECTORAT(px_texture,&animation->linker->frames,animation->reg_currentFrameIndex);
		PX_TextureRenderEx(psurface,pTexture,x,y,refPoint,blend,scale,0);
	}
}


px_void PX_AnimationRender_vector(px_surface *psurface,PX_Animation *animation,px_int x,px_int y,px_point direction,PX_ALIGN refPoint,PX_TEXTURERENDER_BLEND *blend)
{
	px_texture *pTexture;
	if (!animation->linker)
	{
		return;
	}
	if(animation->reg_currentFrameIndex>=0&&animation->reg_currentFrameIndex<animation->linker->frames.size)
	{
		pTexture=PX_VECTORAT(px_texture,&animation->linker->frames,animation->reg_currentFrameIndex);
		PX_TextureRenderRotation_vector(psurface,pTexture,x,y,refPoint,blend,direction);
	}
}


px_int PX_AnimationLibraryGetFrameWidth(PX_AnimationLibrary *panimationLib,px_int frameIndex)
{
	return PX_VECTORAT(px_texture,&panimationLib->frames,frameIndex)->width;
}


px_int PX_AnimationLibraryGetFrameHeight(PX_AnimationLibrary *panimationLib,px_int frameIndex)
{
	return PX_VECTORAT(px_texture,&panimationLib->frames,frameIndex)->height;
}

px_bool PX_AnimationCreate(PX_Animation *animation,PX_AnimationLibrary *linker)
{
	animation->elapsed=0;
	animation->linker=linker;
	animation->reg_currentFrameIndex=-1;
	animation->reg_loopTimes=0;
	animation->reg_reservedTime=0;
	animation->reg_currentAnimation = -1;
	animation->ip=0;

	return PX_TRUE;
}

px_bool PX_AnimationSetLibrary(PX_Animation *animation,PX_AnimationLibrary *linker)
{
	PX_AnimationFree(animation);
	animation->elapsed=0;
	animation->linker=linker;
	animation->reg_currentFrameIndex=-1;
	animation->reg_loopTimes=0;
	animation->reg_reservedTime=0;
	animation->ip=0;
	return PX_TRUE;
}



px_void PX_AnimationFree(PX_Animation *animation)
{
	
}

px_void PX_AnimationReset(PX_Animation *animation)
{
	animation->elapsed=0;
	animation->reg_currentFrameIndex=-1;
	animation->reg_loopTimes=0;
	animation->reg_reservedTime=0;
	animation->ip=0;
}

px_bool PX_AnimationLibrary_CreateEffect_JumpVertical(px_memorypool *mp,PX_AnimationLibrary *panimation,px_texture *effectTexture)
{
	px_int i;
	px_int volume=effectTexture->width*effectTexture->height;
	px_int eff_height[6],eff_width[6];
	px_texture tex;

	PX_VectorInitialize(mp,&panimation->frames,sizeof(px_texture),6);
	PX_MemoryInitialize(mp,&panimation->code);
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

px_void PX_AnimationLibraryAddInstr(PX_AnimationLibrary *panimationLib,PX_2DX_OPCODE opcode,px_word param)
{
	PX_2DX_INSTR instr;
	instr.opcode=opcode;
	instr.param=param;
	PX_MemoryCat(&panimationLib->code,&instr,sizeof(PX_2DX_INSTR));
}

px_bool PX_AnimationIsEnd(PX_Animation *panimation)
{
	if (!panimation->linker)
	{
		return PX_TRUE;
	}
	return (((PX_2DX_INSTR *)(panimation->linker->code.buffer+panimation->ip))->opcode==PX_2DX_OPCODE_END);
}


px_rect PX_AnimationGetSize(PX_Animation *panimation)
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


px_texture * PX_AnimationGetCurrentTexture(PX_Animation *panimation)
{
	return PX_VECTORAT(px_texture,&panimation->linker->frames,panimation->reg_currentFrameIndex);
}

px_void PX_AnimationRenderRotation(px_surface *psurface,PX_Animation *animation,px_point position,px_int angle,PX_ALIGN refPoint,PX_TEXTURERENDER_BLEND *blend)
{
	px_texture *pTexture;
	if (!animation->linker)
	{
		return;
	}
	if(animation->reg_currentFrameIndex>=0&&animation->reg_currentFrameIndex<animation->linker->frames.size)
	{
		pTexture=PX_VECTORAT(px_texture,&animation->linker->frames,animation->reg_currentFrameIndex);
		PX_TextureRenderRotation(psurface,pTexture,(px_int)position.x,(px_int)position.y,refPoint,blend,angle);
	}
}

px_int PX_AnimationGetAnimationsCount(PX_Animation *animation)
{
	if(animation->linker)
	return animation->linker->frames.size;
	else
		return 0;
}

px_dword PX_AnimationGetCurrentPlayAnimation(PX_Animation *animation)
{
	return animation->reg_currentAnimation;
}

px_bool PX_AnimationSetCurrentPlayAnimation(PX_Animation *animation,px_int i)
{
	if(animation->linker)
	if (i>=0&&i<animation->linker->animation.size)
	{
		PX_Animationlibrary_tagInfo *tag=PX_VECTORAT(PX_Animationlibrary_tagInfo,&animation->linker->animation,i);
		animation->ip=tag->ip;
		animation->reg_currentAnimation=i;
		return PX_TRUE;
	}
	return PX_FALSE;
}

px_bool PX_AnimationSetCurrentPlayAnimationByName(PX_Animation *animation,const px_char *name)
{
	px_int i;
	if(animation->linker)
		for (i=0;i<animation->linker->animation.size;i++)
		{
			PX_Animationlibrary_tagInfo *tag=PX_VECTORAT(PX_Animationlibrary_tagInfo,&animation->linker->animation,i);
			if (PX_strequ2(name,tag->name.buffer))
			{
				animation->ip=tag->ip;
				animation->reg_reservedTime=0;
				animation->reg_currentAnimation= i;
				return PX_TRUE;
			}
		}
		return PX_FALSE;
}

px_int PX_AnimationLibraryGetPlayAnimationIndexByName(PX_AnimationLibrary* pLib, const px_char* name)
{
	px_int i;
	if (pLib)
		for (i = 0; i < pLib->animation.size; i++)
		{
			PX_Animationlibrary_tagInfo* tag = PX_VECTORAT(PX_Animationlibrary_tagInfo, &pLib->animation, i);
			if (PX_strequ2(name, tag->name.buffer))
			{
				return i;
			}
		}
	return -1;
}


px_int PX_AnimationGetPlayAnimationIndexByName(PX_Animation *animation,const px_char *name)
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
