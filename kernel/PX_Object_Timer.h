#ifndef PX_OBJECT_TIMER_H
#define PX_OBJECT_TIMER_H

#include "PX_Object.h"

typedef enum
{
    PX_OBJECT_TIMER_MODE_STRICT = 0b00000001,
    PX_OBJECT_TIMER_MODE_DELETE = 0b00000010,
} PX_OBJECT_TIMER_MODE_BIT;

typedef struct
{
    px_byte Mode;
    px_bool IsPaused;
    px_int ExecCount;
    px_dword Interval;
    px_dword Elapsed;
} PX_Object_Timer;

PX_Object *PX_Object_TimerCreate(px_memorypool *mp, PX_Object *parent, px_int execCount, px_dword interval);
PX_Object_Timer *PX_Object_GetTimer(PX_Object *);

px_bool PX_Object_TimerPause(PX_Object *);
px_bool PX_Object_TimerResume(PX_Object *);
px_bool PX_Object_TimerClear(PX_Object *);

px_bool PX_Object_Timer_SetModeStrict(PX_Object *pObject);
px_bool PX_Object_Timer_ResetModeStrict(PX_Object *pObject);
px_bool PX_Object_Timer_SetModeDelete(PX_Object *pObject);
px_bool PX_Object_Timer_ResetModeDelete(PX_Object *pObject);

#endif