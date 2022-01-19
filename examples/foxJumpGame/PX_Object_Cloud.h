#ifndef PX_OBJECT_CLOUD_H
#define PX_OBJECT_CLOUD_H
#include "PX_GameEvent.h"
#include "PainterEngine_Startup.h"

#define PX_OBJECT_CLOUD_BASE 150
#define PX_OBJECT_CLOUD_Z 9

typedef enum
{
	PX_Object_Cloud_State_Ready,
	PX_Object_Cloud_State_Run,
	PX_Object_Cloud_State_Stop,
}PX_Object_Cloud_State;

typedef struct
{
	PX_World* pWorld;
	PX_Runtime* pruntime;
	px_texture *ptextures;

	PX_Object_Cloud_State state;
}PX_Object_Cloud;

PX_Object* PX_Object_CloudCreate(PX_World* pWorld, PX_Runtime* pruntime);
#endif
