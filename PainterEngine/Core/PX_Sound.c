#include "PX_Sound.h"

px_bool PX_SoundPlayInit(px_memorypool *mp,PX_SoundPlay *pSound)
{
	pSound->mp=mp;
	pSound->bLock=PX_FALSE;
	pSound->mix_mode=PX_SOUND_MIX_MODE_PARALLEL;
	pSound->parallel=PX_SOUND_DEFAULT_PARALLEL;
	PX_memset(pSound->Sounds,0,sizeof(pSound->Sounds));
	return PX_TRUE;
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
	px_int i,j,resvSize,CpySize;
	PX_Sound *pSound;
	px_short *pdata,*ps16PCM=(px_short *)pBuffer;
	px_short div;
	if (readSize%4)
	{
		return PX_FALSE;
	}

	while(pSoundPlay->bLock);
	pSoundPlay->bLock=PX_TRUE;
	PX_memset(pBuffer,0,readSize);


	div=PX_COUNTOF(pSoundPlay->Sounds);

	for (j=0;j<PX_COUNTOF(pSoundPlay->Sounds);j++)
	{
		pSound=pSoundPlay->Sounds+j;
		if (!pSound->data)
		{
			continue;
		}

		if (pSound->data->channel==PX_SOUND_CHANNEL_ONE)
		{
			
			pdata=(px_short *)pSound->data->buffer;
			pdata+=(pSound->offset/2);

			resvSize=pSound->data->size-pSound->offset;

			if (resvSize*2>readSize)
			{
				CpySize=readSize;
			}
			else
			{
				CpySize=resvSize*2;
			}

			for (i=0;i<CpySize/2;i+=2)
			{
				switch (pSoundPlay->mix_mode)
				{
				case PX_SOUND_MIX_MODE_PARALLEL:
					if (j<pSoundPlay->parallel)
					{
						ps16PCM[i]+=pdata[i/2]/pSoundPlay->parallel;
						ps16PCM[i+1]+=pdata[i/2]/pSoundPlay->parallel;
					}
					break;
				case PX_SOUND_MIX_MODE_AVG:
					ps16PCM[i]+=pdata[i/2]/div;
					ps16PCM[i+1]+=pdata[i/2]/div;
					break;
				}
					
			}

			pSound->offset+=CpySize;

			if (pSound->offset>=pSound->data->size)
			{
				if (!pSoundPlay->Sounds[j].loop)
				{
					pSoundPlay->Sounds[j].data=PX_NULL;
				}
				pSoundPlay->Sounds[j].offset=0;
				continue;
			}

		}
		else
		{
			pdata=(px_short *)pSound->data->buffer;
			pdata+=(pSound->offset/2);

			resvSize=pSound->data->size-pSound->offset;

			if (resvSize>readSize)
			{
				CpySize=readSize;
			}
			else
			{
				CpySize=resvSize;
			}

			for (i=0;i<CpySize/2;i++)
			{
				switch (pSoundPlay->mix_mode)
				{
				case PX_SOUND_MIX_MODE_PARALLEL:
					if (j<pSoundPlay->parallel)
					{
						ps16PCM[i]+=pdata[i]/pSoundPlay->parallel;
					}
					break;
				case PX_SOUND_MIX_MODE_AVG:
					ps16PCM[i]+=pdata[i]/div;
					break;
				}

					
			}

			pSound->offset+=CpySize;

			if (pSound->offset>=pSound->data->size)
			{
				if (!pSoundPlay->Sounds[j].loop)
				{
					pSoundPlay->Sounds[j].data=PX_NULL;
				}
				pSoundPlay->Sounds[j].offset=0;
				continue;
			}
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

