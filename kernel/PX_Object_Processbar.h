#ifndef PX_OBJECT_PROCESSBAR_H
#define PX_OBJECT_PROCESSBAR_H
#include "PX_Object.h"

typedef struct 
{
	px_int Value;
	px_int MAX;
	px_color ProgressTextColor;
	px_color Color;
	px_color BackgroundColor;
}PX_Object_ProcessBar;

PX_Object *	PX_Object_ProcessBarCreate(px_memorypool *mp,PX_Object *Parent,px_int x,px_int y,px_int Width,px_int Height);
PX_Object_ProcessBar *PX_Object_GetProcessBar(PX_Object *pObject);
px_void		PX_Object_ProcessBarSetColor(PX_Object *pProcessBar,px_color Color);
px_void		PX_Object_ProcessBarSetBackgroundColor(PX_Object *pProcessBar,px_color Color);
px_void		PX_Object_ProcessBarSetValue(PX_Object *pProcessBar,px_int Value);
px_void		PX_Object_ProcessBarSetMax(PX_Object *pProcessBar,px_int Max);
px_int		PX_Object_ProcessBarGetMax(PX_Object* pProcessBar);
px_int		PX_Object_ProcessBarGetValue(PX_Object* pProcessBar);
px_void		PX_Object_ProcessBarSetTextColor(PX_Object* pProcessBar, px_color Color);

typedef struct
{
	px_int Value;
	px_int Min;
	px_int Max;
	px_int linewidth;
	px_color Color;
	px_color BackgroundColor;
	PX_FontModule* fm;
}PX_Object_RingProcessBar;

PX_Object* PX_Object_RingProcessBarCreate(px_memorypool* mp, PX_Object* Parent, px_int x, px_int y, px_int r, PX_FontModule* fm);
PX_Object_RingProcessBar* PX_Object_GetRingProcessBar(PX_Object* pObject);
px_void		PX_Object_RingProcessBarSetColor(PX_Object* pRingProcessBar, px_color Color);
px_void		PX_Object_RingProcessBarSetBackgroundColor(PX_Object* pRingProcessBar, px_color Color);
px_void		PX_Object_RingProcessBarSetValue(PX_Object* pRingProcessBar, px_int Value);
px_void		PX_Object_RingProcessBarSetMax(PX_Object* pRingProcessBar, px_int Max);
px_int		PX_Object_RingProcessBarGetMax(PX_Object* pRingProcessBar);
px_int		PX_Object_RingProcessBarGetValue(PX_Object* pRingProcessBar);

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


PX_Object* PX_Object_RingProgressCreate(px_memorypool* mp, PX_Object* Parent, px_float x, px_float y, px_float radius, PX_FontModule* fm);
PX_Object_RingProgress* PX_Object_GetRingProgress(PX_Object* pObject);
px_void PX_Object_RingProgressSetValue(PX_Object* pObject, px_float value);
px_void PX_Object_RingProgressSetMaxValue(PX_Object* pObject, px_float value);
px_void PX_Object_RingProgressSetCurrentValue(PX_Object* pObject, px_float value);
px_float PX_Object_RingProgressGetValue(PX_Object* pObject);
px_void PX_Object_RingProgressSetSpeed(PX_Object* pObject, px_float value);
#endif

