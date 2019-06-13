#include "PX_Sound.h"

px_bool PX_SoundInit(px_memorypool *mp,PX_SoundPlay *pSound)
{
	pSound->mp=mp;
	pSound->bLock=PX_FALSE;
	return PX_VectorInit(mp,&pSound->Sounds,sizeof(PX_Sound),64);
}


px_bool PX_SoundAdd(PX_SoundPlay *pSound,PX_Sound sounddata)
{
	px_bool ret;
	while(pSound->bLock);
	pSound->bLock=PX_TRUE;
	ret=PX_VectorPushback(&pSound->Sounds,&sounddata);
	pSound->bLock=PX_FALSE;
	return ret;
}


px_bool PX_SoundRead(PX_SoundPlay *pSoundPlay,px_byte *pBuffer,px_int readSize)
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
					ps16PCM[i]+=pdata[i/2]/div;
					ps16PCM[i+1]+=pdata[i/2]/div;
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
					ps16PCM[i]+=pdata[i]/div;
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

px_void PX_SoundFree(PX_SoundPlay *pSound)
{
	PX_VectorFree(&pSound->Sounds);
}

px_bool PX_SoundGetDataCount(PX_SoundPlay *pSoundPlay)
{
	return pSoundPlay->Sounds.size;
}

