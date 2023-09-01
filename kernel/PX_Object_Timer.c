#include "PX_Object_Timer.h"

px_void Func_TimerUpadte(PX_Object *pObject, px_uint elapsed)
{
    PX_Object_Timer *pTimer = pObject->pObject;
    PX_Object_Event timeout = {0};

    if (pTimer->IsPaused)
    {
        return;
    }

    timeout.Event = PX_OBJECT_EVENT_TIMEOUT;
    pTimer->Elapsed += elapsed;

    if (pTimer->Mode & PX_OBJECT_TIMER_MODE_STRICT)
    {
        while (pTimer->Elapsed >= pTimer->Interval && pTimer->ExecCount != 0)
        {
            pTimer->ExecCount -= 1;
            pTimer->Elapsed -= pTimer->Interval;
            PX_ObjectExecuteEvent(pObject, timeout);
        }
    }
    else
    {
        if (pTimer->Elapsed >= pTimer->Interval && pTimer->ExecCount != 0)
        {
            pTimer->ExecCount -= 1;
            pTimer->Elapsed %= pTimer->Interval;
            PX_ObjectExecuteEvent(pObject, timeout);
        }
    }

    if (pTimer->ExecCount == 0 && (pTimer->Mode & PX_OBJECT_TIMER_MODE_DELETE))
    {
        PX_ObjectDelayDelete(pObject);
    }
}

PX_Object *PX_Object_TimerCreate(px_memorypool *mp, PX_Object *parent, px_int execCount, px_dword interval)
{
    PX_Object_Timer timer;
    PX_Object *pObj;

    timer.Mode = 0b00000011;
    timer.IsPaused = PX_FALSE;
    timer.ExecCount = execCount;
    timer.Interval = interval;
    timer.Elapsed = 0;

    pObj = PX_ObjectCreateEx(mp, parent,
                             0.0f, 0.0f, 0.0f,
                             0.0f, 0.0f, 0.0f,
                             PX_OBJECT_TYPE_TIMER,
                             Func_TimerUpadte, PX_NULL, PX_NULL,
                             &timer, sizeof(timer));
    if (pObj == PX_NULL)
    {
        return PX_NULL;
    }
    return pObj;
}


PX_Object_Timer *PX_Object_GetTimer(PX_Object *pObject)
{
    if (pObject == PX_NULL || pObject->Type != PX_OBJECT_TYPE_TIMER)
    {
        return PX_NULL;
    }
    return (PX_Object_Timer *)(pObject->pObject);
}

px_bool PX_Object_TimerPause(PX_Object *pObject)
{
    PX_Object_Timer *pTimer = PX_Object_GetTimer(pObject);
    if (pTimer == PX_NULL)
    {
        return PX_FALSE;
    }
    pTimer->IsPaused = PX_TRUE;
    return PX_TRUE;
}

px_bool PX_Object_TimerResume(PX_Object *pObject)
{
    PX_Object_Timer *pTimer = PX_Object_GetTimer(pObject);
    if (pTimer == PX_NULL)
    {
        return PX_FALSE;
    }
    pTimer->IsPaused = PX_FALSE;
    return PX_TRUE;
}
px_bool PX_Object_TimerClear(PX_Object *pObject)
{
    PX_Object_Timer *pTimer = PX_Object_GetTimer(pObject);
    if (pTimer == PX_NULL)
    {
        return PX_FALSE;
    }
    pTimer->ExecCount = 0;
    return PX_TRUE;
}

px_bool PX_Object_Timer_SetModeStrict(PX_Object *pObject)
{
    PX_Object_Timer *pTimer = PX_Object_GetTimer(pObject);
    if (pTimer == PX_NULL)
    {
        return PX_FALSE;
    }
    pTimer->Mode |= PX_OBJECT_TIMER_MODE_STRICT;
    return PX_TRUE;
}

px_bool PX_Object_Timer_ResetModeStrict(PX_Object *pObject)
{
    PX_Object_Timer *pTimer = PX_Object_GetTimer(pObject);
    if (pTimer == PX_NULL)
    {
        return PX_FALSE;
    }
    pTimer->Mode &= ~PX_OBJECT_TIMER_MODE_STRICT;
    return PX_TRUE;
}

px_bool PX_Object_Timer_SetModeDelete(PX_Object *pObject)
{
    PX_Object_Timer *pTimer = PX_Object_GetTimer(pObject);
    if (pTimer == PX_NULL)
    {
        return PX_FALSE;
    }
    pTimer->Mode |= PX_OBJECT_TIMER_MODE_DELETE;
    return PX_TRUE;
}

px_bool PX_Object_Timer_ResetModeDelete(PX_Object *pObject)
{
    PX_Object_Timer *pTimer = PX_Object_GetTimer(pObject);
    if (pTimer == PX_NULL)
    {
        return PX_FALSE;
    }
    pTimer->Mode &= ~PX_OBJECT_TIMER_MODE_DELETE;
    return PX_TRUE;
}
