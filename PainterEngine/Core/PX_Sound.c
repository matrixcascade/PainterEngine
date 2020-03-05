#include "PX_Sound.h"

px_bool PX_SoundPlayInit(px_memorypool *mp,PX_SoundPlay *pSound)
{
	pSound->mp=mp;
	pSound->bLock=PX_FALSE;
	pSound->mix_mode=PX_SOUND_MIX_MODE_PARALLEL;
	pSound->parallel=PX_SOUND_DEFAULT_PARALLEL;
	pSound->pause=PX_FALSE;
	PX_memset(pSound->Sounds,0,sizeof(pSound->Sounds));
	return PX_TRUE;
}


px_void PX_SoundPlayPause(PX_SoundPlay *pSoundPlay,px_bool pause)
{
	pSoundPlay->pause=pause;
}

px_bool PX_SoundPlayAdd(PX_SoundPlay *pSound,PX_Sound sounddata)
{
	px_int i;
	px_int min_size=0,index;
	while(pSound->bLock);
	pSound->bLock=PX_TRUE;

	for (i=0;i<PX_COUNTOF(pSound->Sounds);i++)
	{
		if (pSound->Sounds[i].data==PX_NULL)
		{
			index=i;
			break;
		}
		else
		{
			if (min_size==0&&!pSound->Sounds[i].loop)
			{
				min_size=pSound->Sounds[i].data->size-pSound->Sounds[i].offset;
				index=i;
			}
			else
			{
				if (!pSound->Sounds[i].loop && pSound->Sounds[i].data->size-pSound->Sounds[i].offset<min_size)
				{
					min_size=pSound->Sounds[i].data->size-pSound->Sounds[i].offset;
					index=i;
				}
			}
		}
	}
	if (index<PX_COUNTOF(pSound->Sounds))
	{
		pSound->Sounds[index]=sounddata;
		pSound->bLock=PX_FALSE;
		return PX_TRUE;
	}
	else
	{
		pSound->bLock=PX_FALSE;
		return PX_FALSE;
	}
	
}


px_bool PX_SoundPlayRead(PX_SoundPlay *pSoundPlay,px_byte *pBuffer,px_int readSize)
{
	px_int i,j,CpySize,ReadblockSize,SourceblockLen;
	PX_Sound *pSound;
	px_short *pSourcePCM16,*pTargetPCM16=(px_short *)pBuffer;
	px_short div;
	if (readSize%4)
	{
		return PX_FALSE;
	}

	while(pSoundPlay->bLock);
	pSoundPlay->bLock=PX_TRUE;

	PX_memset(pBuffer,0,readSize);

	if (pSoundPlay->pause)
	{
		pSoundPlay->bLock=PX_FALSE;
		return PX_TRUE;
	}

	div=PX_COUNTOF(pSoundPlay->Sounds);

	for (j=0;j<PX_COUNTOF(pSoundPlay->Sounds);j++)
	{
		pSound=pSoundPlay->Sounds+j;
		if (!pSound->data)
		{
			continue;
		}
			
		pSourcePCM16=(px_short *)pSound->data->buffer;

		if (!pSound->loop)
		{
			px_int resvSize=pSound->data->size-pSound->offset;
			if (pSound->data->channel==PX_SOUND_CHANNEL_ONE)
			{
				if (resvSize*2>readSize)
				{
					CpySize=readSize;
				}
				else
				{
					CpySize=resvSize*2;
				}
			}
			else
			{
				if (resvSize>readSize)
				{
					CpySize=readSize;
				}
				else
				{
					CpySize=resvSize;
				}
			}
		}
		else
		{
			CpySize=readSize;
		}

		ReadblockSize=CpySize/2;
		SourceblockLen=pSound->data->size/2;
		
		if (pSound->data->channel==PX_SOUND_CHANNEL_ONE)
		{
			for (i=0;i<ReadblockSize;i+=2)
			{
				switch (pSoundPlay->mix_mode)
				{
				case PX_SOUND_MIX_MODE_PARALLEL:
					pTargetPCM16[i]+=pSourcePCM16[pSound->offset/2]/pSoundPlay->parallel;
					pTargetPCM16[i+1]+=pSourcePCM16[pSound->offset/2]/pSoundPlay->parallel;
					pSound->offset+=2;
					break;
				case PX_SOUND_MIX_MODE_AVG:
					pTargetPCM16[i]+=pSourcePCM16[pSound->offset/2]/div;
					pTargetPCM16[i+1]+=pSourcePCM16[pSound->offset/2+1]/div;
					pSound->offset+=2;
					break;
				}

				if (pSound->offset>=pSound->data->size)
				{
					if (pSound->loop)
					{
						pSound->offset=0;
					}
					else
					{
						break;
					}
				}
			}
		}
		else
		{
			for (i=0;i<ReadblockSize;i++)
			{
				switch (pSoundPlay->mix_mode)
				{
				case PX_SOUND_MIX_MODE_PARALLEL:
					pTargetPCM16[i]+=pSourcePCM16[pSound->offset/2]/pSoundPlay->parallel;
					pSound->offset+=2;
					break;
				case PX_SOUND_MIX_MODE_AVG:
					pTargetPCM16[i]+=pSourcePCM16[pSound->offset/2]/div;
					pSound->offset+=2;
					break;
				}
				if (pSound->offset>=pSound->data->size)
				{
					if (pSound->loop)
					{
						pSound->offset=0;
					}
					else
					{
						break;
					}
				}
			}
		}

		if (pSound->offset>=pSound->data->size)
		{
			if (!pSoundPlay->Sounds[j].loop)
			{
				pSoundPlay->Sounds[j].data=PX_NULL;
			}
			continue;
		}
	}
		
	pSoundPlay->bLock=PX_FALSE;
	return PX_TRUE;
}

px_void PX_SoundPlayFree(PX_SoundPlay *pSoundPlay)
{
	
}


px_void PX_SoundPlayClear(PX_SoundPlay *pSoundPlay)
{
	px_int count=0,j;
	for (j=0;j<PX_COUNTOF(pSoundPlay->Sounds);j++)
	{
		pSoundPlay->Sounds[j].data=PX_NULL;
		pSoundPlay->Sounds[j].offset=0;
	}
}

px_bool PX_SoundPlayGetDataCount(PX_SoundPlay *pSoundPlay)
{
	px_int count=0,j;
	for (j=0;j<PX_COUNTOF(pSoundPlay->Sounds);j++)
	{
		if (pSoundPlay->Sounds[j].data)
		{
			count++;
		}
	}
	return count;
}

PX_Sound PX_SoundCreate(PX_SoundData *data,px_bool loop)
{
	PX_Sound sound;
	sound.data=data;
	sound.offset=0;
	sound.loop=loop;
	return sound;
}

px_bool PX_SoundStaticDataCreate(PX_SoundData *sounddata,px_memorypool *mp,px_byte *data,px_int datasize)
{
	if (PX_WaveVerify(data,datasize))
	{
		px_uint offset=0,pcmSize,woffset;
		sounddata->mp=mp;
		pcmSize=PX_WaveGetPCMSize(data,datasize);

		if (pcmSize!=0)
		{
			PX_WAVE_DATA_BLOCK *pBlock;
			PX_WAVE_RIFF_HEADER *pHeader=(PX_WAVE_RIFF_HEADER *)data;
			PX_WAVE_FMT_BLOCK  *pfmt_block;
			sounddata->buffer=(px_byte *)MP_Malloc(mp,pcmSize);
			sounddata->size=pcmSize;
			sounddata->channel=PX_WaveGetChannel(data,pcmSize)==1?PX_SOUND_CHANNEL_ONE:PX_SOUND_CHANNEL_DOUBLE;
			if (!sounddata->buffer)
			{
				return PX_FALSE;
			}
			pfmt_block=(PX_WAVE_FMT_BLOCK  *)(data+sizeof(PX_WAVE_RIFF_HEADER));
			offset+=sizeof(PX_WAVE_RIFF_HEADER);
			offset+=8;
			offset+=pfmt_block->dwFmtSize;

			pcmSize=0;
			woffset=0;
			while (offset<(px_uint)datasize)
			{
				pBlock=(PX_WAVE_DATA_BLOCK*)(data+offset);
				if(!PX_memequ(pBlock->szDataID,"data",4))
				{
					offset+=pBlock->dwDataSize+sizeof(PX_WAVE_DATA_BLOCK);
					continue;
				}
				offset+=sizeof(PX_WAVE_DATA_BLOCK);
				PX_memcpy(sounddata->buffer+woffset,data+offset,pBlock->dwDataSize);
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
	return PX_TRUE;
}

px_void PX_SoundStaticDataFree(PX_SoundData *sounddata)
{
	if (sounddata->mp)
	{
		MP_Free(sounddata->mp,sounddata->buffer);
		sounddata->buffer=PX_NULL;
		sounddata->size=0;
		sounddata->mp=PX_NULL;
	}
	
}

