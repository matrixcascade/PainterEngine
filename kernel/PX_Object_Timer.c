#include "PX_Object_Timer.h"

PX_OBJECT_UPDATE_FUNCTION(Func_TimerUpadte)
{
    PX_Object_Timer *pTimer = PX_ObjectGetDesc(PX_Object_Timer, pObject);
    PX_Object_Event timeout = {0};

    if (pTimer->IsPaused)
    {
        return;
    }

    timeout.Event = PX_OBJECT_EVENT_TIMEOUT;
    pTimer->Elapsed += elapsed;

    while (pTimer->Elapsed >= pTimer->Interval && pTimer->ExecCount != 0)
    {
        if(pTimer->ExecCount>0)
            pTimer->ExecCount -= 1;
        pTimer->Elapsed -= pTimer->Interval;
        PX_ObjectExecuteEvent(pObject, timeout);
    }

    if (pTimer->ExecCount == 0 && (pTimer->delete_after_trigger))
    {
        PX_ObjectDelayDelete(pObject);
    }
}

PX_Object* PX_Object_TimerAttachObject( PX_Object* pObject,px_int attachIndex, px_int execCount, px_dword interval, px_bool DeleteAfterTrigger)
{
    px_memorypool* mp=pObject->mp;
    PX_Object_Timer timer,*pDesc;

    timer.delete_after_trigger = PX_FALSE;
    timer.IsPaused = PX_FALSE;
    timer.ExecCount = execCount;
    timer.Interval = interval;
    timer.Elapsed = 0;
    timer.delete_after_trigger = DeleteAfterTrigger;

    PX_ASSERTIF(pObject == PX_NULL);
    PX_ASSERTIF(attachIndex < 0 || attachIndex >= PX_COUNTOF(pObject->pObjectDesc));
    PX_ASSERTIF(pObject->pObjectDesc[attachIndex] != PX_NULL);
    pDesc = (PX_Object_Timer*)PX_ObjectCreateDesc(pObject, attachIndex, PX_OBJECT_TYPE_TIMER, Func_TimerUpadte, 0, 0, &timer, sizeof(PX_Object_Timer));
    PX_ASSERTIF(pDesc == PX_NULL);


    return pObject;
}


PX_Object *PX_Object_TimerCreate(px_memorypool *mp, PX_Object *parent, px_int execCount, px_dword interval,px_bool DeleteAfterTrigger)
{
    PX_Object *pObj;
    pObj = PX_ObjectCreate(mp, parent,0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
    if (pObj == PX_NULL)
    {
        return PX_NULL;
    }
    if(!PX_Object_TimerAttachObject(pObj, 0, execCount, interval, DeleteAfterTrigger))
	{
		PX_ObjectDelete(pObj);
		return PX_NULL;
	}
    return pObj;
}


PX_Object_Timer *PX_Object_GetTimer(PX_Object *pObject)
{
    PX_Object_Timer* pdesc;
    pdesc=(PX_Object_Timer*)PX_ObjectGetDescByType(pObject, PX_OBJECT_TYPE_TIMER);
    PX_ASSERTIF(pdesc==PX_NULL);
    return pdesc;
}

px_bool PX_Object_TimerPause(PX_Object *pObject)
{
    PX_Object_Timer *pTimer = PX_Object_GetTimer(pObject);
    pTimer->IsPaused = PX_TRUE;
    return PX_TRUE;
}

px_bool PX_Object_TimerResume(PX_Object *pObject)
{
    PX_Object_Timer *pTimer = PX_Object_GetTimer(pObject);
    pTimer->IsPaused = PX_FALSE;
    return PX_TRUE;
}
px_bool PX_Object_TimerClear(PX_Object *pObject)
{
    PX_Object_Timer *pTimer = PX_Object_GetTimer(pObject);
    pTimer->ExecCount = 0;
    return PX_TRUE;
}
