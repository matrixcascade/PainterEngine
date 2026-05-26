#include "PX_FSM.h"

static PX_FSM_State* PX_FSM_GetState(PX_FSM* fsm, px_int state)
{
	px_int i;
	for (i = 0; i < fsm->states.size; i++)
	{
		PX_FSM_State* pState = PX_VECTORAT(PX_FSM_State, &fsm->states, i);
		if (pState->state == state) return pState;
	}
	return PX_NULL;
}

static px_int PX_FSM_GetStateIndex(PX_FSM* fsm, px_int state)
{
	px_int i;
	for (i = 0; i < fsm->states.size; i++)
	{
		PX_FSM_State* pState = PX_VECTORAT(PX_FSM_State, &fsm->states, i);
		if (pState->state == state) return i;
	}
	return -1;
}

static px_void PX_FSM_FreeStateActions(PX_FSM* fsm, PX_FSM_State* pState)
{
	PX_FSM_EventAction* pCur = pState->pEventActions;
	while (pCur)
	{
		PX_FSM_EventAction* pNext = pCur->pNext;
		MP_Free(fsm->mp, pCur);
		pCur = pNext;
	}
	pState->pEventActions = PX_NULL;
}

PX_FSM_Event PX_FSM_BUILD_EVENT(px_uint Event)
{
	PX_FSM_Event e = { 0 };
	e.Event = Event;
	return e;
}
PX_FSM_Event PX_FSM_BUILD_EVENT_STRING(px_uint Event, const px_char* content)
{
	PX_FSM_Event e = { 0 };
	e.Event = Event;
	e.Param_ptr[0] = (px_void*)content;
	return e;
}
PX_FSM_Event PX_FSM_BUILD_EVENT_INT(px_uint Event, px_int i)
{
	PX_FSM_Event e = { 0 };
	e.Event = Event;
	e.Param_int[0] = i;
	return e;
}
PX_FSM_Event PX_FSM_BUILD_EVENT_DATA(px_uint Event, px_void* ptr, px_int size)
{
	PX_FSM_Event e = { 0 };
	e.Event = Event;
	e.Param_ptr[0] = ptr;
	PX_memcpy(&e.Param_ptr[1], (const px_void *)&size, sizeof(size));
	return e;
}

px_int PX_FSM_GetEvent_Int(PX_FSM_Event e)
{
	return e.Param_int[0];
}

const px_char* PX_FSM_GetEvent_String(PX_FSM_Event e)
{
	return (const px_char*)e.Param_ptr[0];
}

px_void* PX_FSM_GetEvent_DataPtr(PX_FSM_Event e)
{
	return e.Param_ptr[0];
}

px_int PX_FSM_GetEvent_DataSize(PX_FSM_Event e)
{
	px_int size;
	PX_memcpy(&size, e.Param_ptr[1], sizeof(size));
	return size;
}



px_bool PX_FSM_Initialize(px_memorypool* mp, PX_FSM* fsm)
{
    fsm->mp = mp;
	if (!PX_VectorInitialize(fsm->mp, &fsm->states, sizeof(PX_FSM_State), 4))
		return PX_FALSE;
	fsm->current_state_index = -1;
	return PX_TRUE;
}

px_void PX_FSM_Clear(PX_FSM* fsm)
{
	px_int i;
	for (i = 0; i < fsm->states.size; i++)
	{
		PX_FSM_State* pState = PX_VECTORAT(PX_FSM_State, &fsm->states, i);
		PX_FSM_FreeStateActions(fsm, pState);
	}
	PX_VectorClear(&fsm->states);
	fsm->current_state_index = -1;
}

px_void PX_FSM_Update(PX_FSM* fsm, px_uint elapsed)
{
	if(PX_VectorCheckIndex(&fsm->states, fsm->current_state_index))
	{
		PX_FSM_State* pState = PX_VECTORAT(PX_FSM_State, &fsm->states, fsm->current_state_index);
		if (pState && pState->update_function)
			pState->update_function(fsm,pState->userptr, elapsed);
	}
}

px_void PX_FSM_Free(PX_FSM* fsm)
{
	PX_FSM_Clear(fsm);
	PX_VectorFree(&fsm->states);
}

px_bool PX_FSM_NewState(PX_FSM* fsm, px_int state,pfsm_update_function update_function, px_void* userptr)
{
	PX_FSM_State newState;
	if (PX_FSM_GetState(fsm, state))
	{
		PX_ASSERTX("state already exist");
		return PX_FALSE;
	}
	newState.state = state;
	newState.pEventActions = PX_NULL;
	newState.update_function = update_function;
	newState.userptr = userptr;
	return PX_VectorPushback(&fsm->states, &newState);
}

px_bool PX_FSM_RegisterEvent(PX_FSM* fsm, px_int state, px_uint Event, pfsm_event_function EventFunc, px_void* user_ptr)
{
	PX_FSM_EventAction* pAction;
	PX_FSM_EventAction* pPoint;
	PX_FSM_State* pState = PX_FSM_GetState(fsm, state);
	if (!pState) return PX_FALSE;

	pAction = (PX_FSM_EventAction*)MP_Malloc(fsm->mp, sizeof(PX_FSM_EventAction));
	if (!pAction) return PX_FALSE;
	pAction->EventAction = Event;
	pAction->EventActionFunc = EventFunc;
	pAction->user_ptr = user_ptr;
	pAction->pNext = PX_NULL;
	pAction->pPre = PX_NULL;

	pPoint = pState->pEventActions;
	if (!pPoint)
	{
		pState->pEventActions = pAction;
		return PX_TRUE;
	}
	while (pPoint->pNext) pPoint = pPoint->pNext;
	pAction->pPre = pPoint;
	pPoint->pNext = pAction;
	return PX_TRUE;
}

px_void PX_FSM_ExecuteEvent(PX_FSM* fsm, PX_FSM_Event e)
{
	PX_FSM_EventAction* pAction;
	PX_FSM_State* pState;
	if (!PX_VectorCheckIndex(&fsm->states, fsm->current_state_index))
		return;
	pState = PX_VECTORAT(PX_FSM_State, &fsm->states, fsm->current_state_index);
	if (!pState)
		return;
	pAction = pState->pEventActions;
	while (pAction)
	{
		if (pAction->EventAction == 0 || pAction->EventAction == e.Event)
		{
			if (pAction->EventActionFunc)
				pAction->EventActionFunc(fsm, e, pAction->user_ptr);
		}
		pAction = pAction->pNext;
	}
}

px_bool PX_FSM_SetState(PX_FSM* fsm, px_int state)
{
	if (!PX_FSM_GetState(fsm, state))
	{
		PX_ASSERTX("state not exist");
		return PX_FALSE;
	}
	fsm->current_state_index = state;
	return PX_TRUE;
}

px_int PX_FSM_GetCurrentState(PX_FSM* fsm)
{
	PX_FSM_State* pState;
	if (!PX_VectorCheckIndex(&fsm->states, fsm->current_state_index))
		return -1;
	pState = PX_VECTORAT(PX_FSM_State, &fsm->states, fsm->current_state_index);
	if (!pState) return -1;
	return pState->state;
}
