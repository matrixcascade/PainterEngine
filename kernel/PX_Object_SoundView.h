#ifndef PX_OBJECT_SOUNDVIEW_H
#define PX_OBJECT_SOUNDVIEW_H
#include "PX_Object.h"


typedef struct 
{
	PX_SoundPlay* pSoundPlay;
}PX_Object_SoundView;

PX_Object* PX_Object_SoundViewCreate(px_memorypool* mp, PX_Object* Parent, px_float x, px_float y, px_float width, px_float height, PX_SoundPlay* pSoundPlay);


#endif



