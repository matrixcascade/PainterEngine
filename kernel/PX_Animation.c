#include "PX_Animation.h"

px_bool PX_AnimationLibraryCreateFromMemory(px_memorypool *mp,PX_AnimationLibrary *panimation,px_byte *_2dxBuffer,px_uint size)
{
	PX_2DX_Header _header;
	px_int i;
	px_byte *pbuffer;
	px_texture texture;
	PX_2DX_CODE_Header _codeheader;
	panimation->mp = mp;
	_header=*(PX_2DX_Header *)_2dxBuffer;
	if (!PX_memequ(&_header.magic,"2DX",sizeof(_header.magic)))
	{
		return PX_FALSE;
	}
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
		if (pbuffer>=_2dxBuffer+size)
		{
			goto _ERROR;
		}
	}


	PX_VectorInitialize(mp,&panimation->frames,sizeof(px_texture),_header.framecount);
	PX_MemoryInitialize(mp,&panimation->code);
	for (i=0;i<(px_int)_header.framecount;i++)
	{
		px_dword nbytes = PX_PngGetSize(pbuffer,(px_int)(_2dxBuffer + size - pbuffer));
		if (nbytes==0)
		{
			nbytes=PX_TRawGetSize((PX_TRaw_Header *)pbuffer);
		}
		if (nbytes==0)
		{
			goto _ERROR;
		}
		if (pbuffer >= _2dxBuffer + size)
		{
			goto _ERROR;
		}

		if (!PX_TextureCreateFromMemory(mp,pbuffer, nbytes,&texture))
		{
			goto _ERROR;
		}
		PX_VectorPushback(&panimation->frames,&texture);
		pbuffer+= nbytes;
		if (pbuffer >= _2dxBuffer + size)
		{
			goto _ERROR;
		}
	}

	_codeheader=*(PX_2DX_CODE_Header *)pbuffer;
	pbuffer+=sizeof(_codeheader);

	if (pbuffer >= _2dxBuffer + size)
	{
		goto _ERROR;
	}

	PX_MemoryCat(&panimation->code,pbuffer,_codeheader.size);

	return PX_TRUE;
	

_ERROR:
	for (i = 0; i < panimation->animation.size; i++)
	{
		PX_StringFree(&PX_VECTORAT(PX_Animationlibrary_tagInfo, &panimation->animation, i)->name);
	}
	PX_VectorFree(&panimation->animation);

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
			panimation->reg_priority = 0;
			return;
		case PX_2DX_OPCODE_FRAME:
			if (pInstr->param < panimation->linker->frames.size)
			{
				panimation->reg_currentFrameIndex = pInstr->param;
				panimation->reg_clipx = 0;
				panimation->reg_clipy = 0;
				panimation->reg_clipi = 0;
				panimation->reg_clipw = PX_AnimationLibraryGetFrameWidth(panimation->linker, panimation->reg_currentFrameIndex);
				panimation->reg_cliph = PX_AnimationLibraryGetFrameHeight(panimation->linker, panimation->reg_currentFrameIndex);
			}
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
		case PX_2DX_OPCODE_CLIPX:
			panimation->reg_clipx = pInstr->param;
			panimation->ip += sizeof(PX_2DX_INSTR);
			break;
		case PX_2DX_OPCODE_CLIPY:
			panimation->reg_clipy = pInstr->param;
			panimation->ip += sizeof(PX_2DX_INSTR);
			break;
		case PX_2DX_OPCODE_CLIPW:
			panimation->reg_clipw=pInstr->param;
			panimation->ip+=sizeof(PX_2DX_INSTR);
			break;
		case PX_2DX_OPCODE_CLIPH:
			panimation->reg_cliph=pInstr->param;
			panimation->ip+=sizeof(PX_2DX_INSTR);
			break;
		case PX_2DX_OPCODE_CLIPI:
			panimation->reg_clipi=pInstr->param;
			panimation->ip+=sizeof(PX_2DX_INSTR);
			break;
		case PX_2DX_OPCODE_CLIPINC:
			panimation->reg_clipi += pInstr->param;
			panimation->ip += sizeof(PX_2DX_INSTR);
			
			break;
		case PX_2DX_OPCODE_CLIPDEC:
			panimation->reg_clipi -= pInstr->param;
			panimation->ip += sizeof(PX_2DX_INSTR);
			break;
		}
	}
}

px_void PX_AnimationRenderMirror(px_surface *psurface,PX_Animation *animation,px_int x,px_int y,PX_ALIGN refPoint,PX_TEXTURERENDER_BLEND *blend,PX_TEXTURERENDER_MIRRROR_MODE mirror)
{
	px_texture *pTexture;
	if (!animation->linker)
	{
		return;
	}
	if(animation->reg_clipw&&animation->reg_cliph)
	if(animation->reg_currentFrameIndex>=0&&animation->reg_currentFrameIndex<animation->linker->frames.size)
	{
		px_int cxc;
		px_int clx, cly;
		pTexture=PX_VECTORAT(px_texture,&animation->linker->frames,animation->reg_currentFrameIndex);
		cxc = pTexture->width / animation->reg_clipw;
		clx = (animation->reg_clipi%cxc)*animation->reg_clipw+animation->reg_clipx;
		cly = (animation->reg_clipi/cxc)*animation->reg_cliph+animation->reg_clipy;
		PX_TextureRenderClipMirror(psurface, pTexture, x, y, clx,cly,animation->reg_clipw,animation->reg_cliph, refPoint, blend,mirror);
	}
}

px_void PX_AnimationRender(px_surface* psurface, PX_Animation* animation, px_int x, px_int y, PX_ALIGN refPoint, PX_TEXTURERENDER_BLEND* blend)
{
	PX_AnimationRenderMirror(psurface, animation , x, y, refPoint, blend, PX_TEXTURERENDER_MIRRROR_MODE_NONE);
}

px_void PX_AnimationRenderEx(px_surface *psurface,PX_Animation *animation,px_int x,px_int y,px_float scale,px_point2D direction,PX_ALIGN refPoint,PX_TEXTURERENDER_BLEND *blend)
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


px_void PX_AnimationRender_vector(px_surface *psurface,PX_Animation *animation,px_int x,px_int y,px_point2D direction,PX_ALIGN refPoint,PX_TEXTURERENDER_BLEND *blend)
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

px_int PX_AnimationGetFrameWidth(PX_Animation* animation)
{
	return animation->reg_clipw;
}

px_int PX_AnimationGetFrameHeight(PX_Animation* animation)
{
	return animation->reg_cliph;
}

px_int PX_AnimationLibraryGetFrameWidth(PX_AnimationLibrary *panimationLib,px_int frameIndex)
{
	if (frameIndex>=0&&frameIndex< panimationLib->frames.size)
	{
		return PX_VECTORAT(px_texture, &panimationLib->frames, frameIndex)->width;
	}
	return 0;
}

px_int PX_AnimationLibraryGetFrameHeight(PX_AnimationLibrary* panimationLib, px_int frameIndex)
{
	if (frameIndex >= 0 && frameIndex < panimationLib->frames.size)
	{
		return PX_VECTORAT(px_texture, &panimationLib->frames ,frameIndex)->height;
	}
	return 0;
}

px_bool PX_AnimationIsActivity(PX_Animation* animation)
{
	return animation->linker!=PX_NULL;
}
px_bool PX_AnimationCreate(PX_Animation *animation,PX_AnimationLibrary *linker)
{
	PX_memset(animation,0,sizeof(PX_Animation));
	if (linker==0)
	{
		return PX_FALSE;
	}
	animation->linker=linker;
	animation->reg_currentFrameIndex=-1;
	animation->reg_currentAnimation = -1;
	
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

#define PX_2DX_MNEMONIC_TEXTURE "TEXTURE"
#define PX_2DX_MNEMONIC_FRAME "FRAME"
#define PX_2DX_MNEMONIC_SLEEP "SLEEP"
#define PX_2DX_MNEMONIC_TAG "TAG"
#define PX_2DX_MNEMONIC_LOOP "LOOP"
#define PX_2DX_MNEMONIC_GOTO "GOTO"
#define PX_2DX_MNEMONIC_ANIMATION "ANIMATION"
#define PX_2DX_MNEMONIC_END "END"
#define PX_2DX_MNEMONIC_CLIPX "CLIPX"
#define PX_2DX_MNEMONIC_CLIPY "CLIPY"
#define PX_2DX_MNEMONIC_CLIPW "CLIPW"
#define PX_2DX_MNEMONIC_CLIPH "CLIPH"
#define PX_2DX_MNEMONIC_CLIPI "CLIPI"
#define PX_2DX_MNEMONIC_CLIPINC "CLIPINC"
#define PX_2DX_MNEMONIC_CLIPDEC "CLIPDEC"

PX_LEXER_LEXEME_TYPE PX_2dx_NextLexer(px_lexer* lexer)
{
	PX_LEXER_LEXEME_TYPE type;
	do
	{
		type = PX_LexerGetNextLexeme(lexer);
	} while (type == PX_LEXER_LEXEME_TYPE_SPACER);

	return type;
}

px_bool PX_AnimationShellCompile(px_memorypool* mp, const px_char script[], px_vector* taginfos, px_vector *texinfos, px_memory* out)
{
	px_int i;
	px_lexer     lexer;
	PX_LEXER_LEXEME_TYPE type;
	px_dword instrAddr = 0;
	px_int lastAnimationAddr = -1;

	PX_2dxMake_textureInfo textureInfo;
	PX_2dxMake_tagInfo taginfo;
	PX_2DX_INSTR instr;
	
	PX_LEXER_STATE state;

	PX_LexerInitialize(&lexer, mp);
	PX_LexerRegisterSpacer(&lexer, ' ');
	PX_LexerRegisterSpacer(&lexer, '\t');
	PX_LexerRegisterContainer(&lexer, "\"", "\"");

	PX_LexerSetNumericMatch(&lexer, PX_TRUE);
	PX_LexerSetTokenCase(&lexer, PX_LEXER_LEXEME_CASE_UPPER);

	if (!PX_LexerLoadSourceFromMemory(&lexer, script))
	{
		goto _RESERROR;
	}

	instrAddr = 0;
	state = PX_LexerGetState(&lexer);
	while (PX_TRUE)
	{
		type = PX_2dx_NextLexer(&lexer);

		if (type == PX_LEXER_LEXEME_TYPE_END)
		{
			break;;
		}


		if (type == PX_LEXER_LEXEME_TYPE_NEWLINE)
		{
			continue;
		}

		if (PX_strequ(lexer.CurLexeme.buffer, PX_2DX_MNEMONIC_TEXTURE))
		{
			type = PX_2dx_NextLexer(&lexer);
			if (type != PX_LEXER_LEXEME_TYPE_CONATINER)
			{
				goto _LEXER_ERROR;
			}
			
			PX_LexerGetIncludedString(&lexer, &lexer.CurLexeme);
			PX_strcpy(textureInfo.path, lexer.CurLexeme.buffer,sizeof(textureInfo.path));

			type = PX_2dx_NextLexer(&lexer);
			if (type != PX_LEXER_LEXEME_TYPE_TOKEN)
			{
				goto _LEXER_ERROR;
			}

			PX_strcpy(textureInfo.map, lexer.CurLexeme.buffer, sizeof(textureInfo.path));

			type = PX_2dx_NextLexer(&lexer);
			if (type != PX_LEXER_LEXEME_TYPE_NEWLINE)
			{
				goto _LEXER_ERROR;
			}

			PX_VectorPushback(texinfos, &textureInfo);
			continue;
		}

		if (PX_strequ(lexer.CurLexeme.buffer, PX_2DX_MNEMONIC_FRAME))
		{
			type = PX_2dx_NextLexer(&lexer);
			if (type != PX_LEXER_LEXEME_TYPE_TOKEN)
			{
				goto _LEXER_ERROR;
			}

			type = PX_2dx_NextLexer(&lexer);
			if (type != PX_LEXER_LEXEME_TYPE_NEWLINE)
			{
				goto _LEXER_ERROR;
			}

			instrAddr += sizeof(px_dword) * 1;

			continue;
		}

		if (PX_strequ(lexer.CurLexeme.buffer, PX_2DX_MNEMONIC_SLEEP))
		{
			type = PX_2dx_NextLexer(&lexer);
			if (type != PX_LEXER_LEXEME_TYPE_TOKEN)
			{
				goto _LEXER_ERROR;
			}
			if (!PX_strIsInt(lexer.CurLexeme.buffer))
			{
				goto _LEXER_ERROR;
			}

			type = PX_2dx_NextLexer(&lexer);
			if (type != PX_LEXER_LEXEME_TYPE_NEWLINE)
			{
				goto _LEXER_ERROR;
			}

			instrAddr += sizeof(px_dword) * 1;

			continue;
		}

		if (PX_strequ(lexer.CurLexeme.buffer, PX_2DX_MNEMONIC_LOOP))
		{
			type = PX_2dx_NextLexer(&lexer);
			if (type != PX_LEXER_LEXEME_TYPE_TOKEN)
			{
				goto _LEXER_ERROR;
			}
			if (!PX_strIsInt(lexer.CurLexeme.buffer))
			{
				goto _LEXER_ERROR;
			}

			type = PX_2dx_NextLexer(&lexer);
			if (type != PX_LEXER_LEXEME_TYPE_NEWLINE)
			{
				goto _LEXER_ERROR;
			}

			instrAddr += sizeof(px_dword) * 1;

			continue;
		}


		if (PX_strequ(lexer.CurLexeme.buffer, PX_2DX_MNEMONIC_TAG))
		{
			type = PX_2dx_NextLexer(&lexer);

			if (type != PX_LEXER_LEXEME_TYPE_TOKEN)
			{
				goto _LEXER_ERROR;
			}

			PX_strcpy(taginfo.tag, lexer.CurLexeme.buffer,sizeof(taginfo.tag));

			type = PX_2dx_NextLexer(&lexer);
			if (type != PX_LEXER_LEXEME_TYPE_NEWLINE)
			{
				goto _LEXER_ERROR;
			}
			taginfo.addr = instrAddr;

			PX_VectorPushback(taginfos, &taginfo);
			continue;
		}

		if (PX_strequ(lexer.CurLexeme.buffer, PX_2DX_MNEMONIC_GOTO))
		{
			type = PX_2dx_NextLexer(&lexer);
			if (type != PX_LEXER_LEXEME_TYPE_TOKEN)
			{
				goto _LEXER_ERROR;
			}

			type = PX_2dx_NextLexer(&lexer);
			if (type != PX_LEXER_LEXEME_TYPE_NEWLINE)
				goto _LEXER_ERROR;

			instrAddr += sizeof(px_dword);

			continue;
		}

		if (PX_strequ(lexer.CurLexeme.buffer, PX_2DX_MNEMONIC_ANIMATION))
		{
			type = PX_2dx_NextLexer(&lexer);

			if (type != PX_LEXER_LEXEME_TYPE_TOKEN)
			{
				goto _LEXER_ERROR;
			}

			PX_strcpy(taginfo.tag, lexer.CurLexeme.buffer,sizeof(taginfo.tag));

			type = PX_2dx_NextLexer(&lexer);
			if (type != PX_LEXER_LEXEME_TYPE_NEWLINE)
			{
				goto _LEXER_ERROR;
			}
			taginfo.addr = instrAddr;

			PX_VectorPushback(taginfos, &taginfo);
			continue;
		}

		if (PX_strequ(lexer.CurLexeme.buffer, PX_2DX_MNEMONIC_END))
		{
			type = PX_2dx_NextLexer(&lexer);

			if (type != PX_LEXER_LEXEME_TYPE_NEWLINE && type != PX_LEXER_LEXEME_TYPE_END)
			{
				goto _LEXER_ERROR;
			}
			instrAddr += sizeof(px_dword);
			continue;
		}
		
		if (PX_strequ(lexer.CurLexeme.buffer, PX_2DX_MNEMONIC_CLIPX)||\
			PX_strequ(lexer.CurLexeme.buffer, PX_2DX_MNEMONIC_CLIPY) ||\
			PX_strequ(lexer.CurLexeme.buffer, PX_2DX_MNEMONIC_CLIPW) ||\
			PX_strequ(lexer.CurLexeme.buffer, PX_2DX_MNEMONIC_CLIPH) ||\
			PX_strequ(lexer.CurLexeme.buffer, PX_2DX_MNEMONIC_CLIPI) || \
			PX_strequ(lexer.CurLexeme.buffer, PX_2DX_MNEMONIC_CLIPINC)||\
			PX_strequ(lexer.CurLexeme.buffer, PX_2DX_MNEMONIC_CLIPDEC) \
			)
		{
			type = PX_2dx_NextLexer(&lexer);
			if (type != PX_LEXER_LEXEME_TYPE_TOKEN)
			{
				goto _LEXER_ERROR;
			}

			type = PX_2dx_NextLexer(&lexer);
			if (type != PX_LEXER_LEXEME_TYPE_NEWLINE)
				goto _LEXER_ERROR;

			instrAddr += sizeof(px_dword);

			continue;
		}


		goto _LEXER_ERROR;
	}

	//compile
	PX_LexerSetState(state);

	while (PX_TRUE)
	{
		type = PX_2dx_NextLexer(&lexer);

		if (type == PX_LEXER_LEXEME_TYPE_END)
		{
			break;
		}

		if (type == PX_LEXER_LEXEME_TYPE_NEWLINE)
		{
			continue;
		}

		if (PX_strequ(lexer.CurLexeme.buffer, PX_2DX_MNEMONIC_TEXTURE))
		{
			PX_2dx_NextLexer(&lexer);
			PX_2dx_NextLexer(&lexer);
			PX_2dx_NextLexer(&lexer);
			continue;
		}

		if (PX_strequ(lexer.CurLexeme.buffer, PX_2DX_MNEMONIC_FRAME))
		{
			type = PX_2dx_NextLexer(&lexer);
			if (type != PX_LEXER_LEXEME_TYPE_TOKEN)
			{
				goto _LEXER_ERROR;
			}

			for (i = 0; i < texinfos->size; i++)
			{
				if (PX_strequ(PX_VECTORAT(PX_2dxMake_textureInfo, texinfos, i)->map, lexer.CurLexeme.buffer))
				{
					instr.opcode = PX_2DX_OPCODE_FRAME;
					instr.param = i;
					break;
				}
			}

			type = PX_2dx_NextLexer(&lexer);
			if (type != PX_LEXER_LEXEME_TYPE_NEWLINE)
			{
				goto _LEXER_ERROR;
			}

			PX_MemoryCat(out, &instr, sizeof(px_dword));
			continue;
		}

		if (PX_strequ(lexer.CurLexeme.buffer, PX_2DX_MNEMONIC_SLEEP))
		{
			type = PX_2dx_NextLexer(&lexer);
			if (type != PX_LEXER_LEXEME_TYPE_TOKEN)
			{
				goto _LEXER_ERROR;
			}
			if (!PX_strIsInt(lexer.CurLexeme.buffer))
			{
				goto _LEXER_ERROR;
			}

			instr.opcode = PX_2DX_OPCODE_SLEEP;

			if (PX_atoi(lexer.CurLexeme.buffer) > 0 && PX_atoi(lexer.CurLexeme.buffer) < 65535)
			{
				instr.param = PX_atoi(lexer.CurLexeme.buffer);
			}
			else
			{
				goto _LEXER_ERROR;
			}
			PX_MemoryCat(out, &instr, sizeof(px_dword));

			type = PX_2dx_NextLexer(&lexer);
			if (type != PX_LEXER_LEXEME_TYPE_NEWLINE)
			{
				goto _LEXER_ERROR;
			}

			continue;
		}

		if (PX_strequ(lexer.CurLexeme.buffer, PX_2DX_MNEMONIC_LOOP)||\
			PX_strequ(lexer.CurLexeme.buffer, PX_2DX_MNEMONIC_CLIPX) || \
			PX_strequ(lexer.CurLexeme.buffer, PX_2DX_MNEMONIC_CLIPY) || \
			PX_strequ(lexer.CurLexeme.buffer, PX_2DX_MNEMONIC_CLIPW) || \
			PX_strequ(lexer.CurLexeme.buffer, PX_2DX_MNEMONIC_CLIPH) || \
			PX_strequ(lexer.CurLexeme.buffer, PX_2DX_MNEMONIC_CLIPI) || \
			PX_strequ(lexer.CurLexeme.buffer, PX_2DX_MNEMONIC_CLIPINC) || \
			PX_strequ(lexer.CurLexeme.buffer, PX_2DX_MNEMONIC_CLIPDEC) \
			)
		{
			if (PX_strequ(lexer.CurLexeme.buffer, PX_2DX_MNEMONIC_LOOP))
				instr.opcode = PX_2DX_OPCODE_LOOP;
			else if (PX_strequ(lexer.CurLexeme.buffer, PX_2DX_MNEMONIC_CLIPX))
				instr.opcode = PX_2DX_OPCODE_CLIPX;
			else if (PX_strequ(lexer.CurLexeme.buffer, PX_2DX_MNEMONIC_CLIPY))
				instr.opcode = PX_2DX_OPCODE_CLIPY;
			else if (PX_strequ(lexer.CurLexeme.buffer, PX_2DX_MNEMONIC_CLIPW))
				instr.opcode = PX_2DX_OPCODE_CLIPW;
			else if (PX_strequ(lexer.CurLexeme.buffer, PX_2DX_MNEMONIC_CLIPH))
				instr.opcode = PX_2DX_OPCODE_CLIPH;
			else if (PX_strequ(lexer.CurLexeme.buffer, PX_2DX_MNEMONIC_CLIPI))
				instr.opcode = PX_2DX_OPCODE_CLIPI;
			else if (PX_strequ(lexer.CurLexeme.buffer, PX_2DX_MNEMONIC_CLIPINC))
				instr.opcode = PX_2DX_OPCODE_CLIPINC;
			else if (PX_strequ(lexer.CurLexeme.buffer, PX_2DX_MNEMONIC_CLIPDEC))
				instr.opcode = PX_2DX_OPCODE_CLIPDEC;

			type = PX_2dx_NextLexer(&lexer);
			if (type != PX_LEXER_LEXEME_TYPE_TOKEN)
			{
				goto _LEXER_ERROR;
			}
			if (!PX_strIsInt(lexer.CurLexeme.buffer))
			{
				goto _LEXER_ERROR;
			}

			

			if (PX_atoi(lexer.CurLexeme.buffer) >= 0 && PX_atoi(lexer.CurLexeme.buffer) <= 65535)
			{
				instr.param = PX_atoi(lexer.CurLexeme.buffer);
			}
			else if (PX_atoi(lexer.CurLexeme.buffer) == -1)
			{
				instr.param = 0xffff;
			}
			else
			{
				goto _LEXER_ERROR;
			}
			PX_MemoryCat(out, &instr, sizeof(px_dword));

			type = PX_2dx_NextLexer(&lexer);
			if (type != PX_LEXER_LEXEME_TYPE_NEWLINE)
			{
				goto _LEXER_ERROR;
			}

			continue;
		}


		if (PX_strequ(lexer.CurLexeme.buffer, PX_2DX_MNEMONIC_TAG))
		{
			PX_2dx_NextLexer(&lexer);
			PX_2dx_NextLexer(&lexer);
			continue;
		}

		if (PX_strequ(lexer.CurLexeme.buffer, PX_2DX_MNEMONIC_GOTO))
		{
			type = PX_2dx_NextLexer(&lexer);
			if (type != PX_LEXER_LEXEME_TYPE_TOKEN)
			{
				goto _LEXER_ERROR;
			}

			for (i = 0; i < taginfos->size; i++)
			{
				if (PX_strequ(PX_VECTORAT(PX_2dxMake_tagInfo, taginfos, i)->tag, lexer.CurLexeme.buffer))
				{
					instr.opcode = PX_2DX_OPCODE_GOTO;
					instr.param = PX_VECTORAT(PX_2dxMake_tagInfo, taginfos, i)->addr;
					break;
				}
			}

			type = PX_2dx_NextLexer(&lexer);

			if (type != PX_LEXER_LEXEME_TYPE_NEWLINE)
				goto _LEXER_ERROR;

			PX_MemoryCat(out, &instr, sizeof(px_dword));

			continue;
		}


		if (PX_strequ(lexer.CurLexeme.buffer, PX_2DX_MNEMONIC_ANIMATION))
		{
			if (lastAnimationAddr != -1)
			{
				goto _LEXER_ERROR;
			}

			type = PX_2dx_NextLexer(&lexer);
			if (type != PX_LEXER_LEXEME_TYPE_TOKEN)
			{
				goto _LEXER_ERROR;
			}

			for (i = 0; i < taginfos->size; i++)
			{
				if (PX_strequ(PX_VECTORAT(PX_2dxMake_tagInfo, taginfos, i)->tag, lexer.CurLexeme.buffer))
				{
					lastAnimationAddr = PX_VECTORAT(PX_2dxMake_tagInfo, taginfos, i)->addr;
					break;
				}
			}

			type = PX_2dx_NextLexer(&lexer);

			if (type != PX_LEXER_LEXEME_TYPE_NEWLINE)
				goto _LEXER_ERROR;

			continue;
		}

		if (PX_strequ(lexer.CurLexeme.buffer, PX_2DX_MNEMONIC_END))
		{
			if (lastAnimationAddr == -1)
			{
				goto _LEXER_ERROR;
			}

			type = PX_2dx_NextLexer(&lexer);

			if (type != PX_LEXER_LEXEME_TYPE_NEWLINE && type != PX_LEXER_LEXEME_TYPE_END)
			{
				goto _LEXER_ERROR;
			}
			instr.opcode = PX_2DX_OPCODE_END;
			instr.param = lastAnimationAddr;
			lastAnimationAddr = -1;

			PX_MemoryCat(out, &instr, sizeof(px_dword));

			continue;
		}

		goto _LEXER_ERROR;
	}

	if (lastAnimationAddr != -1)
	{
		goto _LEXER_ERROR;
	}

	PX_LexerFree(&lexer);
	return PX_TRUE;
_LEXER_ERROR:
	lexer.Sources[lexer.SourceOffset] = '\0';
_RESERROR:
	PX_VectorClear(texinfos);
	PX_VectorClear(taginfos);
	PX_LexerFree(&lexer);
	return PX_FALSE;
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

px_bool PX_AnimationGetRenderRange(PX_Animation* animation, px_recti *range)
{
	 px_texture * pTexture =PX_AnimationGetCurrentTexture(animation);
	 if (pTexture)
	 {
		 px_int cxc;
		 px_int clx, cly;
		 cxc = pTexture->width / animation->reg_clipw;
		 clx = (animation->reg_clipi % cxc) * animation->reg_clipw + animation->reg_clipx;
		 cly = (animation->reg_clipi / cxc) * animation->reg_cliph + animation->reg_clipy;

		return PX_TextureGetRenderRange(pTexture, clx, cly,animation->reg_clipw,animation->reg_cliph, range);
	 }
	 return PX_FALSE;
}

px_dword PX_AnimationGetCurrentPlayAnimation(PX_Animation *animation)
{
	return animation->reg_currentAnimation;
}

const px_char* PX_AnimationGetCurrentPlayAnimationName(PX_Animation* animation)
{
	if (animation->linker&& animation->reg_currentAnimation>=0&& animation->reg_currentAnimation< (px_dword)animation->linker->animation.size)
	{
		PX_Animationlibrary_tagInfo* tag = PX_VECTORAT(PX_Animationlibrary_tagInfo, &animation->linker->animation, animation->reg_currentAnimation);
		return tag->name.buffer;
	}
		
	return PX_NULL;
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

px_bool PX_AnimationPlay(PX_Animation* animation, const px_char* name)
{
	if (PX_AnimationSetCurrentPlayAnimationByName(animation, name))
	{
		animation->reg_priority = 0;
		return PX_TRUE;
	}
	return PX_FALSE;
}

px_bool PX_AnimationPlayWithPriority(PX_Animation* animation, const px_char* name,px_int Priority)
{
	if (Priority>=animation->reg_priority)
	{
		if (PX_AnimationSetCurrentPlayAnimationByName(animation, name))
		{
			animation->reg_priority = Priority;
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
