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

typedef px_void (*px_soundplay_userread)(px_void *userptr,px_byte *pBuffer,px_int readSize);

typedef struct
{
	px_memorypool *mp;
	px_int	parallel;
	px_bool pause;
	volatile px_bool bLock;
	PX_SOUND_MIX_MODE mix_mode;
	px_soundplay_userread userread;
	px_void *userptr;
	px_float current_amplitude;
	px_int amplitude_sum;
	px_int amplitude_sample;
	PX_Sound Sounds[PX_SOUND_DEFAULT_PARALLEL];
}PX_SoundPlay;

px_bool PX_SoundPlayInitialize(px_memorypool *mp, PX_SoundPlay *pSoundPlay);
px_void PX_SoundPlaySetUserRead(PX_SoundPlay *pSoundPlay,px_void (*userread)(px_void *userptr,px_byte *pBuffer,px_int readSize),px_void *ptr);
px_void PX_SoundPlayPause(PX_SoundPlay *pSoundPlay,px_bool pause);
px_bool PX_SoundPlayAdd(PX_SoundPlay *pSoundPlay,PX_Sound sound);
px_bool PX_SoundPlayRead(PX_SoundPlay *pSoundPlay,px_byte *pBuffer,px_int readSize);
px_bool PX_SoundPlayReadCurrentPlayingData(PX_SoundPlay *pSoundPlay,px_int soundIndex,px_int channel,px_int16 *out,px_int count);
px_void PX_SoundPlayFree(PX_SoundPlay *pSoundPlay);
px_void PX_SoundPlayClear(PX_SoundPlay *pSoundPlay);
px_float PX_SoundPlayGetCurrentAmplitude(PX_SoundPlay* pSoundPlay);
px_int  PX_SoundPlayGetDataCount(PX_SoundPlay *pSoundPlay);
PX_Sound PX_SoundCreate(PX_SoundData *data,px_bool loop);
PX_SoundData PX_SoundDataCreate(PX_SOUND_CHANNEL channel, px_byte* data, px_int datasize);
px_bool PX_SoundStaticDataCopy(px_memorypool *mp,PX_SoundData *resSounddata,PX_SoundData *targetSounddata);
px_bool PX_SoundStaticDataCreate(PX_SoundData *sounddata,px_memorypool *mp,px_byte *data,px_int datasize);
px_void PX_SoundStaticDataFree(PX_SoundData *sounddata);
#endif
