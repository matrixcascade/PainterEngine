#ifndef PX_OBJECT_BIRD_H
#define PX_OBJECT_BIRD_H
#include "PX_GameEvent.h"
#include "PainterEngine_Startup.h"

#define PX_OBJECT_BIRD_BASE 192
#define PX_OBJECT_BIRD_Z 8

typedef enum
{
	PX_Object_Bird_State_Ready,
	PX_Object_Bird_State_Run,
	PX_Object_Bird_State_Stop,
}PX_Object_Bird_State;

typedef struct
{
	PX_World* pWorld;
	PX_Runtime* pruntime;
	PX_Animation animation;

	PX_Object_Bird_State state;
}PX_Object_Bird;

PX_Object* PX_Object_BirdCreate(PX_World* pWorld, PX_Runtime* pruntime);
#endif
