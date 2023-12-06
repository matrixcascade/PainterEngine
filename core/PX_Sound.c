#include "PX_Sound.h"

px_bool PX_SoundPlayInitialize(px_memorypool *mp, PX_SoundPlay *pSoundPlay)
{
	PX_memset(pSoundPlay,0,sizeof(PX_SoundPlay));
    pSoundPlay->mp=mp;
    pSoundPlay->bLock=PX_FALSE;
    pSoundPlay->mix_mode=PX_SOUND_MIX_MODE_AVG;
    pSoundPlay->parallel=PX_SOUND_DEFAULT_PARALLEL;
    pSoundPlay->pause=PX_FALSE;
	pSoundPlay->current_amplitude = 0;
	PX_memset(pSoundPlay->Sounds, 0, sizeof(pSoundPlay->Sounds));
	return PX_TRUE;
}


px_void PX_SoundPlaySetUserRead(PX_SoundPlay *pSoundPlay,px_void (*userread)(px_void *userptr,px_byte *pBuffer,px_int readSize),px_void *ptr)
{
	pSoundPlay->userread=userread;
	pSoundPlay->userptr=ptr;
}

px_void PX_SoundPlayPause(PX_SoundPlay *pSoundPlay,px_bool pause)
{
	pSoundPlay->pause=pause;
}

px_void PX_SoundPlayPlayData(PX_SoundPlay* pSoundPlay, PX_SoundData* pdata)
{
	PX_SoundPlayAdd(pSoundPlay, PX_SoundCreate(pdata, 0));
}

px_void PX_SoundPlayRemove(PX_SoundPlay* pSoundPlay, px_int index)
{
	if (index>=0&&index<PX_COUNTOF(pSoundPlay->Sounds))
	{
		PX_memset(pSoundPlay->Sounds + index, 0, sizeof(pSoundPlay->Sounds[0]));
	}
}

px_int PX_SoundPlayAdd(PX_SoundPlay *pSound,PX_Sound sounddata)
{
	px_int i;
	while(pSound->bLock);
	pSound->bLock=PX_TRUE;

	for (i=0;i<PX_COUNTOF(pSound->Sounds);i++)
	{
		if (pSound->Sounds[i].datasize==0)
		{
			pSound->Sounds[i] = sounddata;
			pSound->bLock = PX_FALSE;
			return i;
		}
	}

	pSound->bLock = PX_FALSE;
	return -1;
}

px_void PX_SoundPlaySetMixMode(PX_SoundPlay* pSoundPlay, PX_SOUND_MIX_MODE mode)
{
	pSoundPlay->mix_mode=mode;
}


px_bool PX_SoundPlayRead(PX_SoundPlay *pSoundPlay,px_byte *pBuffer,px_int readSize)
{
	px_int i,j,ReadblockCount;
	PX_Sound *pSound;
	px_short *pSourcePCM16,*pTargetPCM16=(px_short *)pBuffer;
	px_int div;
	if (readSize%4)
	{
		return PX_FALSE;
	}
	ReadblockCount = readSize / 2 / 2;//2 channel 16bits
	
	if (ReadblockCount==0)
	{
		return PX_TRUE;
	}
	

	if (pSoundPlay->userread)
	{

		pSoundPlay->userread(pSoundPlay->userptr,pBuffer,readSize);
		for (i=0;i<readSize/4;i++)
		{
			pSoundPlay->amplitude_sum += PX_ABS(pTargetPCM16[i]);
			pSoundPlay->amplitude_sample++;
			if (pSoundPlay->amplitude_sample>2205)
			{
				pSoundPlay->current_amplitude = pSoundPlay->amplitude_sum / 2205.f/32768.f;
				pSoundPlay->amplitude_sum = 0;
				pSoundPlay->amplitude_sample = 0;
			}
		}

		return PX_TRUE;
	}

	while(pSoundPlay->bLock);
	pSoundPlay->bLock=PX_TRUE;

	PX_memset(pBuffer,0,readSize);

	if (pSoundPlay->pause)
	{
		pSoundPlay->bLock=PX_FALSE;
		pSoundPlay->current_amplitude = 0;
		pSoundPlay->amplitude_sum = 0;
		pSoundPlay->amplitude_sample = 0;
		return PX_TRUE;
	}

	div = 0;
	for (j = 0; j < PX_COUNTOF(pSoundPlay->Sounds); j++)
	{
		pSound = pSoundPlay->Sounds + j;
		if (!pSound->datasize || pSound->pause)
		{
			continue;
		}
		div++;
	}

	for (j=0;j<PX_COUNTOF(pSoundPlay->Sounds);j++)
	{
		pSound=pSoundPlay->Sounds+j;

		if (!pSound->datasize||pSound->pause)
		{
			continue;
		}

		pSourcePCM16=(px_short *)pSound->data->buffer;

		if (pSound->datasize != -1)
		{
			px_int resBlockCount;
			resBlockCount = pSound->data->channel==PX_SOUND_CHANNEL_ONE?pSound->datasize/2:pSound->datasize/4;
			if (ReadblockCount > resBlockCount)
			{
				ReadblockCount=resBlockCount;
				pSound->datasize = 0;
			}
			else
			{
				pSound->datasize -= ReadblockCount * 2 * (pSound->data->channel);
			}
		}

		for (i = 0; i < ReadblockCount; i++)
		{
			switch (pSoundPlay->mix_mode)
			{
			case PX_SOUND_MIX_MODE_PARALLEL:
				{
					pTargetPCM16[i * 2] += pSourcePCM16[pSound->roffset / 2] / pSoundPlay->parallel;
					if (pSound->data->channel == PX_SOUND_CHANNEL_DOUBLE)
					{
						pSound->roffset += 2;
						pSound->roffset %= pSound->data->size;
					}
					pTargetPCM16[i * 2 + 1] += pSourcePCM16[pSound->roffset / 2] / pSoundPlay->parallel;
					pSound->roffset += 2;
					pSound->roffset %= pSound->data->size;
				}
				break;
			case PX_SOUND_MIX_MODE_AVG:
				{
					pTargetPCM16[i * 2] += pSourcePCM16[pSound->roffset / 2] / div;
					if (pSound->data->channel == PX_SOUND_CHANNEL_DOUBLE)
					{
						pSound->roffset += 2;
						pSound->roffset %= pSound->data->size;
					}
					pTargetPCM16[i * 2 + 1] += pSourcePCM16[pSound->roffset / 2] / div;
					pSound->roffset += 2;
					pSound->roffset %= pSound->data->size;
				}
				break;
			}
		}
	}
		
	pSoundPlay->bLock=PX_FALSE;

	for (i = 0; i < readSize / 4; i++)
	{
		pSoundPlay->amplitude_sum += PX_ABS(pTargetPCM16[i]);
		pSoundPlay->amplitude_sample++;
		if (pSoundPlay->amplitude_sample > 2205)
		{
			pSoundPlay->current_amplitude = pSoundPlay->amplitude_sum / 2205.f / 32768.f;
			pSoundPlay->amplitude_sum = 0;
			pSoundPlay->amplitude_sample = 0;
		}
	}

	return PX_TRUE;
}


px_bool PX_SoundPlayReadCurrentPlayingData(PX_SoundPlay *pSoundPlay,px_int soundIndex,px_int channel,px_int16 *out,px_int count)
{
	PX_SoundData *pData;
	px_int offset;
	if (soundIndex>=PX_COUNTOF(pSoundPlay->Sounds)||soundIndex<0)
	{
		return PX_FALSE;
	}
	if (pSoundPlay->Sounds[soundIndex].data==PX_NULL)
	{
		return PX_FALSE;
	}
	if (channel<0||channel>=2)
	{
		return PX_FALSE;
	}

	pData=pSoundPlay->Sounds[soundIndex].data;
	offset=pSoundPlay->Sounds[soundIndex].roffset;
	switch (pData->channel)
	{
	case PX_SOUND_CHANNEL_ONE:
		{
			if (pData->size-offset<count*2)
			{
				return PX_FALSE;
			}
			PX_memcpy(out,pData->buffer+offset,count*2);
		}
		break;
	case PX_SOUND_CHANNEL_DOUBLE:
		{
			px_int i;
			px_int16 *pdptr;
			if (pData->size-offset<2*count*2)
			{
				return PX_FALSE;
			}
			pdptr=(px_int16 *)(pData->buffer+offset);
			for (i=0;i<count;i++)
			{
				out[i]=pdptr[i*2+channel];
			}
		}
		break;
	}
	return PX_TRUE;

}

px_void PX_SoundPlayFree(PX_SoundPlay *pSoundPlay)
{
	
}


px_void PX_SoundPlayClear(PX_SoundPlay *pSoundPlay)
{
	PX_memset(pSoundPlay->Sounds,0,sizeof(pSoundPlay->Sounds));
}

px_float PX_SoundPlayGetCurrentAmplitude(PX_SoundPlay* pSoundPlay)
{
	return pSoundPlay->current_amplitude;
}

px_int PX_SoundPlayGetDataCount(PX_SoundPlay *pSoundPlay)
{
	px_int count=0,j;
	if (pSoundPlay->userread)
	{
		return 1;
	}

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
	PX_Sound sound = {0};
	sound.data=data;
	sound.roffset=0;
	if(loop)
		sound.datasize = -1;
	else
		sound.datasize = data->size;


	sound.woffset = 0;
	return sound;
}

px_void PX_SoundPause(PX_SoundPlay* pSoundPlay, px_int index)
{
	if (index>=0&&index<PX_COUNTOF(pSoundPlay->Sounds))
	{
		pSoundPlay->Sounds[index].pause=PX_TRUE;
	}
}

px_void PX_SoundReset(PX_SoundPlay* pSoundPlay, px_int index)
{
	if (index >= 0 && index < PX_COUNTOF(pSoundPlay->Sounds))
	{
		pSoundPlay->Sounds[index].roffset = 0;
	}
}

px_void PX_SoundResume(PX_SoundPlay* pSoundPlay, px_int index)
{
	if (index >= 0 && index < PX_COUNTOF(pSoundPlay->Sounds))
	{
		pSoundPlay->Sounds[index].pause = PX_FALSE;
	}
}


PX_SoundData PX_SoundDataCreate(PX_SOUND_CHANNEL channel, px_byte* data, px_int datasize)
{
	PX_SoundData sdata;
	sdata.buffer = data;
	sdata.channel = channel;
	sdata.mp = PX_NULL;
	sdata.size = datasize;
	return sdata;
}

px_int PX_SoundDataReadSamples(PX_SoundData* sounddata, px_int byte_offset, px_int readcount, px_float unit_out[],px_int channel)
{
	if (sounddata->channel == PX_SOUND_CHANNEL_ONE)
	{
		if (byte_offset % 2)
		{
			return 0;
		}
	}

	if (sounddata->channel==PX_SOUND_CHANNEL_DOUBLE)
	{
		if (byte_offset % 4)
		{
			return 0;
		}
	}

	if (sounddata->channel == PX_SOUND_CHANNEL_ONE)
	{
		if (channel==1)
		{
			channel = 0;
		}
	}


	if (sounddata->channel==PX_SOUND_CHANNEL_ONE)
	{
		px_int readbyte=0;
		px_short* pPCM = (px_short*)sounddata->buffer;
		px_int ssize = sounddata->size / 2;
		px_int soffset = byte_offset/2;
		while (soffset<ssize)
		{
			unit_out[readbyte / 2] = pPCM[soffset]/32768.f;
			soffset++;
			readbyte += 2;
		}
		return readbyte;
	}
	else if (sounddata->channel == PX_SOUND_CHANNEL_DOUBLE)
	{
		px_int readbyte = 0;
		px_short* pPCM = (px_short*)sounddata->buffer;
		px_int ssize = sounddata->size / 2;
		px_int soffset = byte_offset / 2;
		while (soffset < ssize)
		{
			if (readcount==0)
			{
				break;
			}
			unit_out[readbyte / 4] = pPCM[soffset+ channel] / 32768.f;
			soffset+=2;
			readbyte += 4;
			readcount--;
		}
		return readbyte;
	}
	else
	{
		return 0;
	}
	return 0;
}


px_bool PX_SoundDataCopy(px_memorypool *mp,PX_SoundData *resSounddata,PX_SoundData *targetSounddata)
{
	targetSounddata->buffer=(px_byte *)MP_Malloc(mp,resSounddata->size);
	if(!targetSounddata->buffer)
	{
		return PX_FALSE;
	}
	targetSounddata->channel=resSounddata->channel;
	targetSounddata->mp=mp;
	targetSounddata->size=resSounddata->size;
	PX_memcpy(targetSounddata->buffer,resSounddata->buffer,resSounddata->size);
	return PX_TRUE;
}

px_bool PX_SoundDataCreateFromWavFileData(PX_SoundData *sounddata,px_memorypool *mp,px_byte *data,px_int datasize)
{
	if (PX_WaveVerify(data,datasize))
	{
		px_uint offset=0,pcmSize,woffset;
		
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
				goto _ERROR;
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
			goto _ERROR;
	}
	else
	{
		goto _ERROR;
	}
	sounddata->mp=mp;
	return PX_TRUE;
_ERROR:
	sounddata->mp=PX_NULL;
	sounddata->size=0;
	sounddata->buffer=PX_NULL;
	return PX_FALSE;
}

px_void PX_SoundDataFree(PX_SoundData *sounddata)
{
	if (sounddata->mp)
	{
		MP_Free(sounddata->mp,sounddata->buffer);
		sounddata->buffer=PX_NULL;
		sounddata->size=0;
		sounddata->mp=PX_NULL;
	}
	
}

px_bool PX_SoundCircularWrite(PX_Sound* pSound, px_byte* pBuffer, px_int writeSize)
{
	px_int len1, len2;

	if (writeSize>pSound->data->size)
	{
		return PX_FALSE;
	}

	len1 = pSound->data->size - pSound->woffset;
	len2 = writeSize - len1;
	
	if (writeSize>len1)
	{
		PX_memcpy(pSound->data->buffer + pSound->woffset, pBuffer, len1);
		writeSize -= len1;
		PX_memcpy(pSound->data->buffer, pBuffer + len1, writeSize);
	}
	else
	{
		PX_memcpy(pSound->data->buffer + pSound->woffset, pBuffer, writeSize);
	}

	pSound->woffset = (pSound->woffset + writeSize) % pSound->data->size;

	if (pSound->datasize != -1)
	{
		pSound->datasize += writeSize;
		if (pSound->datasize > (px_uint)pSound->data->size)
		{
			pSound->datasize = pSound->data->size;
		}
	}
		
	return PX_TRUE;
}

px_bool PX_SoundCircularWriteSample(PX_Sound* pSound, px_short sample)
{
	return PX_SoundCircularWrite(pSound, (px_byte*)&sample, 2);
}


