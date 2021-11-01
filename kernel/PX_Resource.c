#include "PX_Resource.h"

px_bool PX_ResourceLibraryInitialize(px_memorypool *mp,PX_ResourceLibrary *lib)
{
	lib->mp=mp;
	PX_MapInitialize(mp,&lib->map);
	PX_ListInitialize(mp,&lib->resources);
	return PX_TRUE;
}

px_bool PX_ResourceLibraryLoad(PX_ResourceLibrary *lib,PX_RESOURCE_TYPE type,px_byte *data,px_uint datasize,const px_char key[])
{
	PX_Resource res;
	res.Type=type;

	if (PX_ResourceLibraryGet(lib,key))
	{
		PX_ResourceLibraryDelete(lib,key);
	}

	switch (type)
	{
	case PX_RESOURCE_TYPE_NULL:
		break;
	case PX_RESOURCE_TYPE_TEXTURE:
		if(!PX_TextureCreateFromMemory(lib->mp,data,datasize,&res.texture))
			return PX_FALSE;
		break;
	case PX_RESOURCE_TYPE_SHAPE:
		if(!PX_ShapeCreateFromMemory(lib->mp,data,datasize,&res.shape))
			return PX_FALSE;
		break;
	case PX_RESOURCE_TYPE_SCRIPT:
		/*
		if (data[0]!='P'||data[1]!='A'||data[2]!='S'||data[3]!='M')
		{
			PX_SCRIPT_LIBRARY compilelib;
			px_string asmcodeString;
			px_memory bin;
			MP_Reset(mptemp);
			if(!PX_ScriptCompilerInit(&compilelib,mptemp))
			{
				return PX_FALSE;
			}

			if(!PX_ScriptCompilerLoad(&compilelib,(px_char *)data))
			{
				PX_ScriptCompilerFree(&compilelib);
				return PX_FALSE;
			}

			PX_VectorInitialize(mptemp,&asmcodeString);


			if(PX_ScriptCompilerCompile(&compilelib,"main",&asmcodeString,256))
			{
				PX_ScriptCompilerFree(&compilelib);

				PX_ScriptAsmOptimization(&asmcodeString);
				PX_MemoryInit(mptemp,&bin);
				
				if(!PX_ScriptAsmCompile(mptemp,asmcodeString.buffer,&bin))
				{
					PX_MemoryFree(&bin);
					PX_StringFree(&asmcodeString);
					PX_ScriptCompilerFree(&compilelib);
					return PX_FALSE;
				}

				if(!PX_ScriptVM_InstanceInit(&res.Script,lib->mp,bin.buffer,bin.usedsize))
					{
						PX_MemoryFree(&bin);
						PX_StringFree(&asmcodeString);
						PX_ScriptCompilerFree(&compilelib);
						return PX_FALSE;
					}

				PX_MemoryFree(&bin);
			}
			else
			{
				PX_StringFree(&asmcodeString);
				PX_ScriptCompilerFree(&compilelib);
				MP_Reset(mptemp);
				return PX_FALSE;
			}

			PX_StringFree(&asmcodeString);
			
			MP_Reset(mptemp);
		}
		else
		*/
		if(!PX_ScriptVM_InstanceInitialize(&res.Script,lib->mp,data,datasize))
			return PX_FALSE;
		break;
	case PX_RESOURCE_TYPE_ANIMATIONLIBRARY:
		if(!PX_AnimationLibraryCreateFromMemory(lib->mp,&res.animationlibrary,data,datasize))
			return PX_FALSE;
		break;
	case PX_RESOURCE_TYPE_SOUND:
		if (PX_WaveVerify(data,datasize))
		{
			px_uint offset=0,pcmSize,woffset;
			pcmSize=PX_WaveGetPCMSize(data,datasize);
			
			if (pcmSize!=0)
			{
				PX_WAVE_DATA_BLOCK *pBlock;
				PX_WAVE_RIFF_HEADER *pHeader=(PX_WAVE_RIFF_HEADER *)data;
				PX_WAVE_FMT_BLOCK  *pfmt_block;
				res.sound.mp=lib->mp;
				res.sound.buffer=(px_byte *)MP_Malloc(lib->mp,pcmSize);
				res.sound.size=pcmSize;
				res.sound.channel=PX_WaveGetChannel(data,pcmSize)==1?PX_SOUND_CHANNEL_ONE:PX_SOUND_CHANNEL_DOUBLE;
				if (!res.sound.buffer)
				{
					return PX_FALSE;
				}
				pfmt_block=(PX_WAVE_FMT_BLOCK  *)(data+sizeof(PX_WAVE_RIFF_HEADER));
				offset+=sizeof(PX_WAVE_RIFF_HEADER);
				offset+=8;
				offset+=pfmt_block->dwFmtSize;

				pcmSize=0;
				woffset=0;
				while (offset<datasize)
				{
					pBlock=(PX_WAVE_DATA_BLOCK*)(data+offset);
					if(!PX_memequ(pBlock->szDataID,"data",4))
					{
						offset+=pBlock->dwDataSize+sizeof(PX_WAVE_DATA_BLOCK);
						continue;
					}
					offset+=sizeof(PX_WAVE_DATA_BLOCK);
					PX_memcpy(res.sound.buffer+woffset,data+offset,pBlock->dwDataSize);
					offset+=pBlock->dwDataSize;
					woffset+=pBlock->dwDataSize;
				}
			}
			else
			return PX_FALSE;
		}
		else
		{
			return PX_FALSE;
		}
		break;
	case PX_RESOURCE_TYPE_DATA:
		PX_MemoryInitialize(lib->mp,&res.data);
		if (!PX_MemoryCat(&res.data,data,datasize))
		{
			PX_MemoryFree(&res.data);
			return PX_FALSE;
		}
		break;
	}
	
	PX_MapPut(&lib->map,key,PX_ListPush(&lib->resources,&res,sizeof(res)));
	return PX_TRUE;
}

px_void PX_ResourceLibraryFree(PX_ResourceLibrary *lib)
{
	PX_Resource *pres;
	px_list_node *pNode=lib->resources.head;
	while (pNode)
	{
		pres=(PX_Resource *)pNode->pdata;
		switch(pres->Type)
		{
		case PX_RESOURCE_TYPE_NULL:
			break;
		case PX_RESOURCE_TYPE_TEXTURE:
			PX_TextureFree(&pres->texture);
			break;
		case PX_RESOURCE_TYPE_SCRIPT:
			PX_ScriptVM_InstanceFree(&pres->Script);
			break;
		case PX_RESOURCE_TYPE_ANIMATIONLIBRARY:
			PX_AnimationLibraryFree(&pres->animationlibrary);
			break;
		case PX_RESOURCE_TYPE_DATA:
			PX_MemoryFree(&pres->data);
			break;
			case PX_RESOURCE_TYPE_SOUND:
				PX_SoundStaticDataFree(&pres->sound);
				break;
			case PX_RESOURCE_TYPE_SHAPE:
				PX_ShapeFree(&pres->shape);
				break;
		}
		pNode=pNode->pnext;
	}
	PX_ListFree(&lib->resources);
	PX_MapFree(&lib->map);
	
}

PX_Resource *  PX_ResourceLibraryGet(PX_ResourceLibrary *lib,const px_char key[])
{
	return (PX_Resource *)PX_MapGet(&lib->map,key);
}

px_bool PX_ResourceLibraryAddTexture(PX_ResourceLibrary *lib,const px_char key[],px_texture *pTexture)
{
	PX_Resource res;
	res.Type=PX_RESOURCE_TYPE_TEXTURE;
	if(!PX_TextureCopy(lib->mp,pTexture,&res.texture))return PX_FALSE;
	PX_MapPut(&lib->map,key,PX_ListPush(&lib->resources,&res,sizeof(res)));
	return PX_TRUE;
}

px_void PX_ResourceLibraryDelete(PX_ResourceLibrary *lib,const px_char key[])
{
	PX_Resource * pres,*pnodeRes;

	pres=(PX_Resource *)PX_MapGet(&lib->map,key);
	if (pres)
	{
		px_list_node *pNode=lib->resources.head;
		while (pNode)
		{
			pnodeRes=(PX_Resource *)pNode->pdata;
			if (pnodeRes==pres)
			{
				switch(pres->Type)
				{
				case PX_RESOURCE_TYPE_NULL:
					break;
				case PX_RESOURCE_TYPE_TEXTURE:
					PX_TextureFree(&pres->texture);
					break;
				case PX_RESOURCE_TYPE_SCRIPT:
					PX_ScriptVM_InstanceFree(&pres->Script);
					break;
				case PX_RESOURCE_TYPE_ANIMATIONLIBRARY:
					PX_AnimationLibraryFree(&pres->animationlibrary);
					break;
				case PX_RESOURCE_TYPE_DATA:
					PX_MemoryFree(&pres->data);
					break;
					case PX_RESOURCE_TYPE_SOUND:
						PX_SoundStaticDataFree(&pres->sound);
						break;
                    case PX_RESOURCE_TYPE_SHAPE:
                        PX_ShapeFree(&pres->shape);
                        break;
				}
				PX_ListPop(&lib->resources,pNode);
				PX_MapErase(&lib->map,key);
				return;
			}
			pNode=pNode->pnext;
		}
	}
}

px_texture * PX_ResourceLibraryGetTexture(PX_ResourceLibrary *lib,const px_char key[])
{
	PX_Resource *pres=PX_ResourceLibraryGet(lib,key);
	if (pres&&pres->Type==PX_RESOURCE_TYPE_TEXTURE)
	{
		return &pres->texture;
	}
	return PX_NULL;
}

px_shape * PX_ResourceLibraryGetShape(PX_ResourceLibrary *lib,const px_char key[])
{
	PX_Resource *pres=PX_ResourceLibraryGet(lib,key);
	if (pres&&pres->Type==PX_RESOURCE_TYPE_SHAPE)
	{
		return &pres->shape;
	}
	return PX_NULL;
}

PX_AnimationLibrary * PX_ResourceLibraryGetAnimationLibrary(PX_ResourceLibrary *lib,const px_char key[])
{
	PX_Resource *pres=PX_ResourceLibraryGet(lib,key);
	if (pres&&pres->Type==PX_RESOURCE_TYPE_ANIMATIONLIBRARY)
	{
		return &pres->animationlibrary;
	}
	return PX_NULL;
}

PX_ScriptVM_Instance * PX_ResourceLibraryGetScript(PX_ResourceLibrary *lib,const px_char key[])
{
	PX_Resource *pres=PX_ResourceLibraryGet(lib,key);
	if (pres&&pres->Type==PX_RESOURCE_TYPE_SCRIPT)
	{
		return &pres->Script;
	}
	return PX_NULL;
}

px_memory * PX_ResourceLibraryGetData(PX_ResourceLibrary *lib,const px_char key[])
{
	PX_Resource *pres=PX_ResourceLibraryGet(lib,key);
	if (pres&&pres->Type==PX_RESOURCE_TYPE_DATA)
	{
		return &pres->data;
	}
	return PX_NULL;
}

PX_SoundData * PX_ResourceLibraryGetSound(PX_ResourceLibrary *lib,const px_char key[])
{
	PX_Resource *pres=PX_ResourceLibraryGet(lib,key);
	if (pres&&pres->Type==PX_RESOURCE_TYPE_SOUND)
	{
		return &pres->sound;
	}
	return PX_NULL;
}

