#ifndef PX_SOUND_H
#define PX_SOUND_H
//painter engine sound-mixer
//44100HZ
//16bit
//1/2-channel supported
#include "PX_Vector.h"
typedef enum
{
	PX_SOUND_CHANNEL_ONE,
	PX_SOUND_CHANNEL_DOUBLE
}PX_SOUND_CHANNEL;

typedef struct
{
	px_byte *buffer;
	px_int size;
	PX_SOUND_CHANNEL channel;
}PX_SoundData;

typedef struct
{
	px_int offset;
	PX_SoundData *data;
}PX_Sound;

typedef struct
{
	px_memorypool *mp;
    px_bool bLock;
	px_vector Sounds;
}PX_SoundPlay;

px_bool PX_SoundInit(px_memorypool *mp,PX_SoundPlay *pSoundPlay);
px_bool PX_SoundAdd(PX_SoundPlay *pSoundPlay,PX_Sound sounddata);
px_bool PX_SoundRead(PX_SoundPlay *pSoundPlay,px_byte *pBuffer,px_int readSize);
px_void PX_SoundFree(PX_SoundPlay *pSoundPlay);
px_bool PX_SoundGetDataCount(PX_SoundPlay *pSoundPlay);
#endif
