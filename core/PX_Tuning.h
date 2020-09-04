#ifndef PX_TUNING
#define PX_TUNING

#include "../core/PX_Core.h"

#define  PX_TUNING_MAX_EXECUTE_SIZE 8192
#define  PX_TUNING_DEFAULT_FRAME_SIZE 1024
#define  PX_TUNING_MAX_FRAME_SIZE 2048


typedef struct
{
	px_memorypool *mp;
	px_double previous_frame[PX_TUNING_MAX_FRAME_SIZE/2*3];
	px_double half_previous_out[PX_TUNING_MAX_FRAME_SIZE/2];
	px_double filter[PX_TUNING_MAX_FRAME_SIZE];
	px_double window[PX_TUNING_MAX_FRAME_SIZE];
	px_double fix[PX_TUNING_MAX_FRAME_SIZE];
	px_double remian[PX_TUNING_MAX_EXECUTE_SIZE];
	px_int SampleRate;
	px_int  remainCount;
	px_double pitchShift,timeScale;
	px_double previousInterpolationValue;
	px_double previousInterpolationOffset;
	px_double spectrumInterpolationOffset;
	px_double spectrumInterpolationFactor;
	px_double interpolationFactor;
	px_double ZCR_Low,ZCR_High;
	px_double amp_threshold;
	px_double lastphase[PX_TUNING_MAX_FRAME_SIZE];
	px_int N;
}PX_Tuning;

typedef enum
{
	PX_TUNING_WINDOW_SIZE_8,
	PX_TUNING_WINDOW_SIZE_128,
	PX_TUNING_WINDOW_SIZE_256,
	PX_TUNING_WINDOW_SIZE_512,
	PX_TUNING_WINDOW_SIZE_1024,
	PX_TUNING_WINDOW_SIZE_2048,
}PX_TUNING_WINDOW_SIZE;

px_void PX_TuningInitialize(px_memorypool *mp,PX_Tuning *tuning,px_int SampleRate,px_double pitchShift,px_double timeShift,px_double window[],px_double filter[],px_double fix[],PX_TUNING_WINDOW_SIZE windowsize);
px_int PX_TuningFilter(PX_Tuning *tuning,_IN px_double frame_unit[],px_int Size,_OUT px_double out_unit[]);
px_void PX_TuningTimeScale(px_double timescale,px_double in[],px_int count,px_memory *out);
px_void PX_TuningSetPitchShift(PX_Tuning *tuning,px_double pitchShift);
px_void PX_TuningSetTimeScale(PX_Tuning *tuning,px_double TimeScale);
px_void PX_TuningSetFilter(PX_Tuning *tuning,px_double filter[]);
px_void PX_TuningSetFix(PX_Tuning *tuning,px_double fix[]);
px_void PX_TuningSetZCR(PX_Tuning *tuning,px_double low,px_double high);
#endif
