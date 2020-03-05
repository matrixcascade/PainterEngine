#ifndef PX_SOUND_H
#define PX_SOUND_H
//painter engine sound-mixer
//44100HZ
//16bit
//1/2-channel supported
#include "PX_Wave.h"
#include "PX_Vector.h"
#include "PX_Tuning.h"

#define PX_SOUND_DEFAULT_PARALLEL 8
typedef enum
{
	PX_SOUND_CHANNEL_ONE,
	PX_SOUND_CHANNEL_DOUBLE
}PX_SOUND_CHANNEL;

typedef enum
{
	PX_SOUND_MIX_MODE_AVG,
	PX_SOUND_MIX_MODE_PARALLEL,
}PX_SOUND_MIX_MODE;

typedef struct
{
	px_memorypool *mp;
	px_byte *buffer;
	px_int size;
	PX_SOUND_CHANNEL channel;
}PX_SoundData;

typedef struct
{
	px_int offset;
	px_bool loop;
	PX_SoundData *data;
}PX_Sound;

typedef struct
{
	px_memorypool *mp;
	px_int	parallel;
	px_bool pause;
	volatile px_bool bLock;
	PX_SOUND_MIX_MODE mix_mode;
	PX_Sound Sounds[PX_SOUND_DEFAULT_PARALLEL];
}PX_SoundPlay;

px_bool PX_SoundPlayInit(px_memorypool *mp,PX_SoundPlay *pSoundPlay);
px_void PX_SoundPlayPause(PX_SoundPlay *pSoundPlay,px_bool pause);
px_bool PX_SoundPlayAdd(PX_SoundPlay *pSoundPlay,PX_Sound sound);
px_bool PX_SoundPlayRead(PX_SoundPlay *pSoundPlay,px_byte *pBuffer,px_int readSize);
px_void PX_SoundPlayFree(PX_SoundPlay *pSoundPlay);
px_void PX_SoundPlayClear(PX_SoundPlay *pSoundPlay);
px_bool PX_SoundPlayGetDataCount(PX_SoundPlay *pSoundPlay);
PX_Sound PX_SoundCreate(PX_SoundData *data,px_bool loop);
px_bool PX_SoundStaticDataCreate(PX_SoundData *sounddata,px_memorypool *mp,px_byte *data,px_int datasize);
px_void PX_SoundStaticDataFree(PX_SoundData *sounddata);
#endif
