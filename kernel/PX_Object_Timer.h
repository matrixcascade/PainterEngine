#ifndef PX_OBJECT_TIMER_H
#define PX_OBJECT_TIMER_H

#include "PX_Object.h"


typedef struct
{
    px_bool time_trigger_once;
    px_bool delete_after_trigger;
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

px_void PX_Object_TimerDeleteAfterTrigger(PX_Object *pObject,px_bool b);
px_void PX_Object_TimerTimeOutTriggerOnce(PX_Object* pObject, px_bool b);


#endif