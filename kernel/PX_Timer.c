#include "PX_Timer.h"

PX_Task PX_Timer_SetTimeout(px_int delay, px_timer_callback callback, px_void *user_ptr)
{
    return PX_Timer_CreateTask(1, delay, callback, user_ptr);
}

PX_Task PX_Timer_SetInterval(px_int delay, px_timer_callback callback, px_void *user_ptr)
{
    return PX_Timer_CreateTask(-1, delay, callback, user_ptr);
}

PX_Task PX_Timer_CreateTask(px_int execCount, px_int interval, px_timer_callback callback, px_void *user_ptr)
{
    PX_Task task = {execCount, 0, interval, callback, user_ptr};
    return task;
}

PX_Task *PX_Timer_UpdateTask(PX_Task *task, px_int elapsed)
{
    if (task == PX_NULL || task->ExecCnt == 0)
    {
        return task;
    }
    task->Elapsed += elapsed;
    while (task->ExecCnt != 0 && task->Elapsed > task->interval)
    {
        task->callback(task->user_ptr);
        task->Elapsed -= task->interval;
        task->ExecCnt--;
    }

    return task;
}

PX_Task *PX_Timer_ClearTask(PX_Task *task){
    task->ExecCnt = 0;
    return task;
}
