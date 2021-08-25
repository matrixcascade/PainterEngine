#include "../modules/px_audio.h"
#include <SDL.h>
#include <string.h>
static PX_SoundPlay *SDL_SoundPlay;
static unsigned int SDL_volume=SDL_MIX_MAXVOLUME;
static void  pcm_play_callback(void *ptr,Uint8 *out,int require_len)
{ 
	if (SDL_SoundPlay)
	{
		PX_SoundPlayRead(SDL_SoundPlay,out,require_len);
	}
	else
	{
		memset(out,0,require_len);
	}
} 


px_bool PX_AudioInitialize(PX_SoundPlay *soundPlay)
{
	SDL_AudioSpec wanted_spec;
	SDL_SoundPlay=soundPlay;

	wanted_spec.freq = 44100; 
	wanted_spec.format = AUDIO_S16SYS; 
	wanted_spec.channels = 2; 
	wanted_spec.silence = 0; 
	wanted_spec.samples = 2048; 
	wanted_spec.callback = pcm_play_callback; 

	if (SDL_OpenAudio(&wanted_spec, NULL)<0){ 
		return PX_FALSE; 
	} 

	//Play
	SDL_PauseAudio(0);
	return PX_TRUE;
}

void PX_AudioSetVolume(unsigned int volume)
{
	//invalid function
	SDL_volume=volume;
}