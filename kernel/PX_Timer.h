#ifndef __PX_TIMER_H
#define __PX_TIMER_H

#include "PX_Object.h"

typedef px_void (*px_timer_callback)(px_void *user_ptr);

typedef struct
{
    px_int ExecCnt;
    px_int Elapsed;
    px_int interval;
    px_timer_callback callback;
    px_void *user_ptr;
} PX_Task;


PX_Task PX_Timer_SetTimeout(px_int delay, px_timer_callback callback, px_void *user_ptr);
PX_Task PX_Timer_SetInterval(px_int delay, px_timer_callback callback, px_void *user_ptr);
PX_Task PX_Timer_CreateTask(px_int execCount,px_int interval, px_timer_callback callback, px_void *user_ptr);

PX_Task *PX_Timer_UpdateTask(PX_Task *task, px_int elapsed);
PX_Task *PX_Timer_ClearTask(PX_Task *task);


#endif