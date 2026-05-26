#ifndef PX_FSM_H
#define PX_FSM_H
#include "PX_Vector.h"

typedef struct _PX_FSM PX_FSM;

typedef struct
{
	px_uint Event;
	union
	{
		px_char Param_char[16];
		px_uint Param_uint[4];
		px_int Param_int[4];
		px_float Param_float[4];
		px_void* Param_ptr[4];
		px_bool Param_bool[4];
	};
}PX_FSM_Event;

#define PX_FSM_EVENT_FUNCTION(name) px_void name(PX_FSM *pfsm,PX_FSM_Event e,px_void * userptr)
typedef px_void(*pfsm_event_function)(PX_FSM*, PX_FSM_Event, px_void*);

#define PX_FSM_UPDATE_FUNCTION(name) px_void name(PX_FSM *pfsm,px_void * userptr,px_uint elapsed)
typedef px_void(*pfsm_update_function)(PX_FSM*, px_void*, px_uint);

typedef struct
{
	px_int state;
	struct _PX_FSM_EventAction* pEventActions;
	pfsm_update_function update_function;
	px_void* userptr;
}PX_FSM_State;

struct _PX_FSM
{
	px_memorypool* mp;
	px_int current_state_index;
	px_vector states;
};



typedef struct _PX_FSM_EventAction
{
	px_uint EventAction;
	pfsm_event_function EventActionFunc;
	px_void* user_ptr;
	struct _PX_FSM_EventAction* pNext;
	struct _PX_FSM_EventAction* pPre;
}PX_FSM_EventAction;

PX_FSM_Event PX_FSM_BUILD_EVENT(px_uint Event);
PX_FSM_Event PX_FSM_BUILD_EVENT_STRING(px_uint Event, const px_char* content);
PX_FSM_Event PX_FSM_BUILD_EVENT_INT(px_uint Event, px_int i);
PX_FSM_Event PX_FSM_BUILD_EVENT_DATA(px_uint Event, px_void* ptr, px_int size);

px_int PX_FSM_GetEvent_Int(PX_FSM_Event e);
const px_char* PX_FSM_GetEvent_String(PX_FSM_Event e);
px_void* PX_FSM_GetEvent_DataPtr(PX_FSM_Event e);
px_int PX_FSM_GetEvent_DataSize(PX_FSM_Event e);

px_bool PX_FSM_Initialize(px_memorypool* mp, PX_FSM* fsm);
px_void PX_FSM_Clear(PX_FSM* fsm);
px_void PX_FSM_Update(PX_FSM* fsm, px_uint elapsed);
px_void PX_FSM_Free(PX_FSM* fsm);
px_bool PX_FSM_NewState(PX_FSM* fsm, px_int state, pfsm_update_function update_function,px_void *userptr);
px_bool PX_FSM_RegisterEvent(PX_FSM* fsm, px_int state, px_uint Event, pfsm_event_function EventFunc, px_void* user_ptr);
px_void PX_FSM_ExecuteEvent(PX_FSM* fsm, PX_FSM_Event e);
px_bool PX_FSM_SetState(PX_FSM* fsm, px_int state);
px_int  PX_FSM_GetCurrentState(PX_FSM* fsm);
#endif // !PX_FSM_H
