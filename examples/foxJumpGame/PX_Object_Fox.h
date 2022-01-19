#ifndef PX_OBJECT_FOX_H
#define PX_OBJECT_FOX_H
#include "PainterEngine_Startup.h"
#include "PX_GameEvent.h"
#define PX_OBJECT_FOX_X 128
#define PX_OBJECT_FOX_BASE 96
#define PX_OBJECT_FOX_IMPLUSE 600
#define PX_OBJECT_WORLD_G  1600
#define PX_OBJECT_FOX_TYPE 0x0001

#define PX_OBJECT_FOX_EVENT_IMPACT 0x0002000

typedef enum 
{
	PX_Object_Fox_State_Ready,
	PX_Object_Fox_State_Run,
	PX_Object_Fox_State_Stop,
}PX_Object_Fox_State;

typedef struct  
{
	PX_Object* pschedular;
	PX_Runtime *pRuntime;
	PX_World* pworld;
	px_vector2D velocity;
	PX_Animation animation;

	px_int  jump_cooldown;

	px_dword animation_walk_id;
	px_dword animation_run_id;
	px_dword animation_up_id;
	px_dword animation_down_id;
	px_dword animation_impact_id;

	PX_Object_Fox_State state;
}PX_Object_Fox;

PX_Object* PX_Object_FoxCreate(PX_Object* schedular, PX_World* pWorld, PX_Runtime* pRuntime);

#endif
