#include "../modules/px_audio.h"

#include "SLES/OpenSLES.h"
#include "SLES/OpenSLES_Android.h"

//engine
SLObjectItf engineObject = PX_NULL;
SLEngineItf engineEngine = PX_NULL;

//mixer
SLObjectItf outputMixObject = PX_NULL;
SLEnvironmentalReverbItf outputMixEnvironmentalReverb = PX_NULL;
SLEnvironmentalReverbSettings reverbSettings = SL_I3DL2_ENVIRONMENT_PRESET_STONECORRIDOR;

//pcm
SLObjectItf pcmPlayerObject = PX_NULL;
SLPlayItf pcmPlayerPlay = PX_NULL;
SLVolumeItf pcmPlayerVolume = PX_NULL;

SLAndroidSimpleBufferQueueItf pcmBufferQueue;
#define SL_PCM_BLOCK_SIZE 2048
#define SL_PCM_BLOCK_COUNT 8
static px_byte SL_PCMBuffer[SL_PCM_BLOCK_SIZE*SL_PCM_BLOCK_COUNT];
static px_int  SL_PCMrBlock=0;
static PX_SoundPlay *SL_soundPlay=PX_NULL;


void pcmBufferCallBack(SLAndroidSimpleBufferQueueItf bf, void * context)
{
    px_int SL_PCMwBlock;

    if(!SL_soundPlay)
    {
        PX_memset(SL_PCMBuffer,0,sizeof(SL_PCMBuffer));
        (*pcmBufferQueue)->Enqueue(pcmBufferQueue, SL_PCMBuffer, SL_PCM_BLOCK_SIZE);
        return;
    }


    //write a block
    (*pcmBufferQueue)->Enqueue(pcmBufferQueue, SL_PCMBuffer+SL_PCMrBlock*SL_PCM_BLOCK_SIZE, SL_PCM_BLOCK_SIZE);

    //read a block
    SL_PCMwBlock=SL_PCMrBlock+SL_PCM_BLOCK_COUNT-1;

    if(SL_PCMwBlock>=SL_PCM_BLOCK_COUNT)
    {
        SL_PCMwBlock-=SL_PCM_BLOCK_COUNT;
    }
    PX_SoundPlayRead(SL_soundPlay,SL_PCMBuffer+SL_PCMwBlock*SL_PCM_BLOCK_SIZE,SL_PCM_BLOCK_SIZE);

    SL_PCMrBlock++;
    if(SL_PCMrBlock>=SL_PCM_BLOCK_COUNT)
        SL_PCMrBlock=0;
}

px_bool PX_AudioInitialize(PX_SoundPlay *soundPlay)
{
    SLresult result;
    SL_soundPlay=soundPlay;
    //engine create
    slCreateEngine(&engineObject, 0, 0, 0, 0, 0);
    (*engineObject)->Realize(engineObject, SL_BOOLEAN_FALSE);
    (*engineObject)->GetInterface(engineObject, SL_IID_ENGINE, &engineEngine);

    const SLInterfaceID mids[1] = {SL_IID_ENVIRONMENTALREVERB};
    const SLboolean mreq[1] = {SL_BOOLEAN_FALSE};
    result = (*engineEngine)->CreateOutputMix(engineEngine, &outputMixObject, 1, mids, mreq);
    if (SL_RESULT_SUCCESS != result)
        return  PX_FALSE;
    result = (*outputMixObject)->Realize(outputMixObject, SL_BOOLEAN_FALSE);
    if (SL_RESULT_SUCCESS != result)
        return  PX_FALSE;
    result = (*outputMixObject)->GetInterface(outputMixObject, SL_IID_ENVIRONMENTALREVERB, &outputMixEnvironmentalReverb);
    if (SL_RESULT_SUCCESS == result) {
        result = (*outputMixEnvironmentalReverb)->SetEnvironmentalReverbProperties(
                outputMixEnvironmentalReverb, &reverbSettings);
    } else
    {
        return  PX_FALSE;
    }
    SLDataLocator_OutputMix outputMix = {SL_DATALOCATOR_OUTPUTMIX, outputMixObject};


    SLDataLocator_AndroidSimpleBufferQueue android_queue={SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE,2};
    SLDataFormat_PCM pcm={
            SL_DATAFORMAT_PCM,
            2,
            SL_SAMPLINGRATE_44_1,//44100hz
            SL_PCMSAMPLEFORMAT_FIXED_16,//16bits
            SL_PCMSAMPLEFORMAT_FIXED_16,//16bits
            SL_SPEAKER_FRONT_LEFT | SL_SPEAKER_FRONT_RIGHT,
            SL_BYTEORDER_LITTLEENDIAN
    };

    SLDataSource slDataSource = {&android_queue, &pcm};
    SLDataSink audioSnk = {&outputMix, NULL};
    const SLInterfaceID ids[3] = {SL_IID_BUFFERQUEUE, SL_IID_EFFECTSEND, SL_IID_VOLUME};
    const SLboolean req[3] = {SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE};

    result = (*engineEngine)->CreateAudioPlayer(engineEngine, &pcmPlayerObject, &slDataSource, &audioSnk, 3, ids, req);
    (*pcmPlayerObject)->Realize(pcmPlayerObject, SL_BOOLEAN_FALSE);
    (*pcmPlayerObject)->GetInterface(pcmPlayerObject, SL_IID_PLAY, &pcmPlayerPlay);


    (*pcmPlayerObject)->GetInterface(pcmPlayerObject, SL_IID_BUFFERQUEUE, &pcmBufferQueue);
    (*pcmBufferQueue)->RegisterCallback(pcmBufferQueue, pcmBufferCallBack, NULL);
    (*pcmPlayerObject)->GetInterface(pcmPlayerObject, SL_IID_VOLUME, &pcmPlayerVolume);

    (*pcmPlayerPlay)->SetPlayState(pcmPlayerPlay, SL_PLAYSTATE_PLAYING);

    pcmBufferCallBack(pcmBufferQueue, NULL);
    return PX_TRUE;
}


void PX_AudioSetVolume(unsigned int volume)
{
    (*pcmPlayerVolume)->SetVolumeLevel(pcmPlayerVolume, volume);
}
