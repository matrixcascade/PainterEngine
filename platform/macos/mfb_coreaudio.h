
#ifndef MFB_CORE_AUDIO_H
#define MFB_CORE_AUDIO_H

// ------------------------------------
void* mfb_audio_device_start(char* dummy);
void mfb_audio_device_stop(void* s);
int write_audio(void* cookie, short* incomingBuffer, int incomingSize);
int close_audio(void* cookie);

#endif
