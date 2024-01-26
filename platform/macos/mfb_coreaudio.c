
#include <AudioToolbox/AudioToolbox.h>
#include <AudioUnit/AudioUnit.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>

#include "PainterEngine.h"

// ------------------------------------
#define kOutputBus 0
#define kInputBus 1
#define SAMPLE_RATE 44100

#define SOUND_BUFFER_SIZE (1764 * 16)

// ------------------------------------
static short* _pcm;
static int _pcmSize;
static int _pcmIndex = 0;

static AudioComponentInstance* _audioUnit = 0;
static pthread_mutex_t _playingMutex;
static pthread_cond_t _playingCond;

static PX_SoundPlay* Sound_soundplay;
static px_byte Sound_data[SOUND_BUFFER_SIZE];

// ------------------------------------
typedef struct {
    char RIFF[4];       // RIFF Header
    uint32_t chunkSize; // RIFF Chunk Size
    char WAVE[4];       // WAVE Header

    char fmt[4];            // FMT header
    uint32_t subchunk1Size; // Size of the fmt chunk
    uint16_t audioFormat;   // Audio format
    uint16_t numChannels;   // Number of channels
    uint32_t sampleRate;    // Sampling Frequency in Hz
    uint32_t bytesPerSec;   // bytes per second
    uint16_t blockAlign;    // 2=16-bit mono, 4=16-bit stereo
    uint16_t bitsPerSample; // Number of bits per sample
    char data[4];           // "data" string
    uint32_t dataSize;      // Sampled data length
} WAVHeader;

// ------------------------------------
short* loadWAVFile(const char* filename, WAVHeader* header, int* numSamples) {
    FILE* file = fopen(filename, "rb");
    if (!file) {
        perror("Error opening file");
        return NULL;
    }

    // Read header information
    fread(header, sizeof(WAVHeader), 1, file);

    // Skip non-data blocks
    while (strncmp(header->data, "data", 4) != 0) {
        // Skip the current block
        fseek(file, header->dataSize, SEEK_CUR);

        // Attempt to read next block header
        if (fread(header->data, sizeof(header->data) + sizeof(header->dataSize), 1, file) != 1) {
            fprintf(stderr, "Error reading file.\n");
            fclose(file);
            return NULL;
        }
    }

    // Check if the format is supported
    printf("audioFormat: %d, bitsPerSample: %d\n", header->audioFormat, header->bitsPerSample);
    if (header->audioFormat != 1 || header->bitsPerSample != 16) {
        fprintf(stderr, "Unsupported WAV format.\n");
        fclose(file);
        return NULL;
    }

    *numSamples = header->dataSize / sizeof(short);

    // Read audio data
    short* pcmData = (short*)malloc(header->dataSize);
    if (!pcmData) {
        fprintf(stderr, "Memory allocation error.\n");
        fclose(file);
        return NULL;
    }

    fread(pcmData, sizeof(short), *numSamples, file);
    fclose(file);

    return pcmData;
}

// ------------------------------------
void write_audio(void* dummy, short* pcm, int size) {
    _pcm = pcm;
    _pcmSize = size / sizeof(int);
    _pcmIndex = 0;

    // pthread_mutex_lock(&_playingMutex);
    // AudioOutputUnitStart(*_audioUnit);

    // block until finished playing
    // pthread_cond_wait(&_playingCond, &_playingMutex);
    // pthread_mutex_unlock(&_playingMutex);
}

// ------------------------------------
static OSStatus playbackCallback(void* inRefCon, AudioUnitRenderActionFlags* ioActionFlags, const AudioTimeStamp* inTimeStamp,
                                 UInt32 inBusNumber, UInt32 inNumberFrames, AudioBufferList* ioData) {
    int _index = 0;
    int totalNumberOfSamples = 0;
    px_dword dataByteSize = 0;

    for (UInt32 i = 0; i < ioData->mNumberBuffers; ++i) {
        dataByteSize = ioData->mBuffers[i].mDataByteSize;
        totalNumberOfSamples = dataByteSize / 4;

        if (Sound_soundplay && PX_SoundPlayGetDataCount(Sound_soundplay)) {
            PX_SoundPlayRead(Sound_soundplay, Sound_data, dataByteSize);
        } else {
            memset(Sound_data, 0, SOUND_BUFFER_SIZE);
        }

        int samplesLeft = totalNumberOfSamples - _index;
        int numSamples = dataByteSize / 4;

        if (samplesLeft > 0) {
            if (samplesLeft < numSamples) {
                memcpy(ioData->mBuffers[i].mData, &Sound_data[_index], samplesLeft * 4);
                _index += samplesLeft;
                memset(((char*)(ioData->mBuffers[i].mData)) + samplesLeft * 4, 0, (numSamples - samplesLeft) * 4);
            } else {
                memcpy(ioData->mBuffers[i].mData, &Sound_data[_index], numSamples * 4);
                _index += numSamples;
            }

        } else {
            memset(ioData->mBuffers[i].mData, 0, dataByteSize);

            // signal that pcm is finished playing
            // pthread_mutex_lock(&_playingMutex);
            // pthread_cond_signal(&_playingCond);
            // pthread_mutex_unlock(&_playingMutex);

            // stop the audio unit
            // AudioOutputUnitStop(*_audioUnit);
        }
    }

    return noErr;
}

// ------------------------------------
void close_audio(void* cookie) { AudioOutputUnitStop(*_audioUnit); }

// ------------------------------------
void mfb_audio_device_stop(void* s) {
    AudioUnitUninitialize(*_audioUnit);
    AudioComponentInstanceDispose(*_audioUnit);
    free(_audioUnit);
    _audioUnit = 0;
    pthread_mutex_destroy(&_playingMutex);
    pthread_cond_destroy(&_playingCond);

    AudioOutputUnitStop(*_audioUnit);
}

// ------------------------------------
int mfb_audio_device_start(char* dummy) {
    // skip if already created audio unit
    if (_audioUnit) return 0;

    pthread_mutex_init(&_playingMutex, NULL);
    pthread_cond_init(&_playingCond, NULL);

    _audioUnit = (AudioComponentInstance*)malloc(sizeof(AudioComponentInstance));
    OSStatus status;

    // Describe audio component
    AudioComponentDescription desc;
    desc.componentType = kAudioUnitType_Output;
    desc.componentSubType = kAudioUnitSubType_DefaultOutput;
    desc.componentFlags = 0;
    desc.componentFlagsMask = 0;
    desc.componentManufacturer = kAudioUnitManufacturer_Apple;

    // Get component
    AudioComponent inputComponent = AudioComponentFindNext(NULL, &desc);

    // Get audio units
    status = AudioComponentInstanceNew(inputComponent, _audioUnit);
    // checkStatus(status);

    UInt32 flag = 1;
    // Enable IO for playback
    status = AudioUnitSetProperty(*_audioUnit, kAudioOutputUnitProperty_EnableIO, kAudioUnitScope_Output, kOutputBus, &flag, sizeof(flag));
    // checkStatus(status);
    
    Float32 volume = 1.0;
    status = AudioUnitSetParameter(*_audioUnit, kHALOutputParam_Volume, kAudioUnitScope_Output, kOutputBus, volume, 0);
    // checkStatus(status);

    // Describe format
    AudioStreamBasicDescription audioFormat;
    audioFormat.mSampleRate = SAMPLE_RATE;
    audioFormat.mFormatID = kAudioFormatLinearPCM;
    audioFormat.mFormatFlags = kAudioFormatFlagIsSignedInteger | kAudioFormatFlagIsPacked;
    audioFormat.mFramesPerPacket = 1;
    audioFormat.mChannelsPerFrame = 2;
    audioFormat.mBitsPerChannel = 16;
    audioFormat.mBytesPerFrame = (audioFormat.mBitsPerChannel / 8) * audioFormat.mChannelsPerFrame;
    audioFormat.mBytesPerPacket = audioFormat.mBytesPerFrame * audioFormat.mFramesPerPacket;

    // Apply format
    status = AudioUnitSetProperty(*_audioUnit, kAudioUnitProperty_StreamFormat, kAudioUnitScope_Input, kOutputBus, &audioFormat,
                                  sizeof(audioFormat));
    // checkStatus(status);

    // Set output callback
    AURenderCallbackStruct callbackStruct;
    callbackStruct.inputProc = playbackCallback;
    callbackStruct.inputProcRefCon = NULL;
    status = AudioUnitSetProperty(*_audioUnit, kAudioUnitProperty_SetRenderCallback, kAudioUnitScope_Global, kOutputBus, &callbackStruct,
                                  sizeof(callbackStruct));

    // Initialize
    status = AudioUnitInitialize(*_audioUnit);

    AudioOutputUnitStart(*_audioUnit);
    return 1;
}

// ------------------------------------
px_int PX_AudioInitialize(PX_SoundPlay* soundPlay) {
    Sound_soundplay = soundPlay;
    return PX_TRUE;
}

// ------------------------------------
void PX_AudioSetVolume(unsigned int volume) {
    OSStatus status;

    float volumeLevel = volume / 100.0f;
    if (_audioUnit) {
        status = AudioUnitSetParameter(*_audioUnit, kHALOutputParam_Volume, kAudioUnitScope_Output, kOutputBus, volumeLevel, 0);
        // checkStatus(status);
    }
}

#if 0
int main() {
    if (mfb_audio_device_start(NULL) == 0) {
        printf("Audio device failed to start.\n");
        return 1;
    }

    WAVHeader header;
    int numSamples;
    short* pcmData = loadWAVFile("assets/bliss.wav", &header, &numSamples);
    if (!pcmData) {
        mfb_audio_device_stop(NULL);
        return 1;
    }

    // Play pcm data
    write_audio(NULL, pcmData, numSamples * sizeof(short));

    free(pcmData);
    mfb_audio_device_stop(NULL);

    return 0;
}
#endif
