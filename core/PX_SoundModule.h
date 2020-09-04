#ifndef PX_SOUNDMODULE
#define PX_SOUNDMODULE

#include "../core/PX_Core.h"

#define  PX_SOUNDMODULE_N 1024
#define  PX_SOUNDMODULE_FORMANT_COUNT 32
#define  PX_SOUNDMODULE_DEFALUT_SEARCH_HZ 90

typedef struct
{
	px_double A,W;
}PX_SoundModule_Formant;

typedef struct
{
	px_memorypool *mp;
	px_double lastPhase[PX_SOUNDMODULE_N/2];
	px_double remian[PX_SOUNDMODULE_N*2];
	px_double halfRebuild[PX_SOUNDMODULE_N/2];
	PX_SoundModule_Formant Formant[PX_SOUNDMODULE_FORMANT_COUNT];

	px_int  remainCount;
	px_int SampleRate;
	px_double searchHzW;
	px_double pitchShift;
	px_double ZCR_Low,ZCR_High;
	px_double amp_threshold;
	px_double amp_max;
	px_double Pitch_threshold_Low,Pitch_threshold_High;
}PX_SoundModule;


px_void PX_SoundModuleInitialize(px_memorypool *mp,PX_SoundModule *SoundModule,px_int SampleRate,px_double pitchShift,PX_SoundModule_Formant Formant[],px_int FormantCount);
px_int PX_SoundModuleFilter(PX_SoundModule *SoundModule,_IN px_double _in[],px_int count,_OUT px_double _out[]);
px_void PX_SoundModulePitchShift(PX_SoundModule *SoundModule,px_double pitchShift);
px_void PX_SoundModuleSetZCR(PX_SoundModule *SoundModule,px_double low,px_double high);

#endif
