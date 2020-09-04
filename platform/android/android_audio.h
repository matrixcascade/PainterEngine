
#ifndef PLATFORM_ANDROID_SOUND_H
#define PLATFORM_ANDROID_SOUND_H
#include "SLES/OpenSLES.h"
#include "SLES/OpenSLES_Android.h"

#include "../../core/PX_Sound.h"

px_bool PX_AudioInitialize(PX_SoundPlay *soundPlay);
void PX_AudioSetVolume(px_dword volume);

#endif