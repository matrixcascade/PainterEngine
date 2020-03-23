#ifndef PLATFORM_WINDOWS_DSOUND_H
#define PLATFORM_WINDOWS_DSOUND_H
#include "Windows.h"
#include "../../core/PX_Sound.h"

BOOL PX_AudioInitialize(HWND hwnd,PX_SoundPlay *soundPlay,BOOL startThread);
void PX_AudioSetVolume(DWORD volume);
int  PX_AudioGetStandbyBufferSize();
int  PX_AudioWriteBuffer(void *pBuffer,size_t Size);


BOOL PX_AudioCaptureOpen(GUID dev_id,int channel);
DWORD PX_AudioCaptureRead(void *buffer,px_int buffersize);
DWORD PX_AudioCaptureReadEx(void *buffer,px_int buffersize,px_int align);
BOOL PX_AudioCaptureClose();
#endif