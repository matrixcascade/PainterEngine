#include "PX_Sound.h"

px_bool PX_SoundPlayInit(px_memorypool *mp,PX_SoundPlay *pSound)
{
	pSound->mp=mp;
	pSound->bLock=PX_FALSE;
	pSound->mix_mode=PX_SOUND_MIX_MODE_PARALLEL;
	pSound->parallel=PX_SOUND_DEFAULT_PARALLEL;
	return PX_VectorInit(mp,&pSound->Sounds,sizeof(PX_Sound),64);
}


px_bool PX_SoundPlayAdd(PX_SoundPlay *pSound,PX_Sound sounddata)
{
	px_bool ret;
	while(pSound->bLock);
	pSound->bLock=PX_TRUE;
	ret=PX_VectorPushback(&pSound->Sounds,&sounddata);
	pSound->bLock=PX_FALSE;
	return ret;
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
	px_memset(pBuffer,0,readSize);


	div=pSoundPlay->Sounds.size;
	for (j=0;j<pSoundPlay->Sounds.size;j++)
	{
		pSound=PX_VECTORAT(PX_Sound,&pSoundPlay->Sounds,j);
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
				PX_VectorErase(&pSoundPlay->Sounds,j);
				j--;
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
				PX_VectorErase(&pSoundPlay->Sounds,j);
				j--;
				continue;
			}
		}
	}
	pSoundPlay->bLock=PX_FALSE;
	return PX_TRUE;
}

px_void PX_SoundPlayFree(PX_SoundPlay *pSound)
{
	PX_VectorFree(&pSound->Sounds);
}

px_bool PX_SoundPlayGetDataCount(PX_SoundPlay *pSoundPlay)
{
	return pSoundPlay->Sounds.size;
}

PX_Sound PX_SoundCreate(PX_SoundData *data)
{
	PX_Sound sound;
	sound.data=data;
	sound.offset=0;
	return sound;
}

