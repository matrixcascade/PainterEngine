#ifndef PX_OBJECT_RINGPROGRESS
#define PX_OBJECT_RINGPROGRESS

#include "PX_Object.h"


typedef struct  
{
	px_float max;
	px_float min;
	px_float current_value;
	px_float target_value;
	px_float Speed;
	px_color backgroundColor;
	px_color borderColor;
	px_color processColor;
	px_color fontColor;
	PX_FontModule* numericFM;
}PX_Object_RingProgress;


PX_Object* PX_Object_RingProgressCreate(px_memorypool* mp, PX_Object* Parent,px_float x,px_float y,px_float radius,PX_FontModule *fm);
PX_Object_RingProgress* PX_Object_GetRingProgress(PX_Object* pObject);
px_void PX_Object_RingProgressSetValue(PX_Object* pObject, px_float value);
px_void PX_Object_RingProgressSetMaxValue(PX_Object* pObject, px_float value);
px_void PX_Object_RingProgressSetCurrentValue(PX_Object* pObject, px_float value);
px_float PX_Object_RingProgressGetValue(PX_Object* pObject);
px_void PX_Object_RingProgressSetSpeed(PX_Object* pObject, px_float value);
#endif