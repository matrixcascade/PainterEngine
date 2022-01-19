#ifndef PX_OBJECT_WOOD_H
#define PX_OBJECT_WOOD_H
#include "PX_GameEvent.h"
#include "PainterEngine_Startup.h"

#define PX_OBJECT_WOOD_BASE 110
#define PX_OBJECT_WOOD_Z 8

typedef enum
{
	PX_Object_Wood_State_Ready,
	PX_Object_Wood_State_Run,
	PX_Object_Wood_State_Stop,
}PX_Object_Wood_State;

typedef struct
{
	PX_World* pWorld;
	PX_Runtime* pruntime;
	px_texture *ptextures;

	PX_Object_Wood_State state;
}PX_Object_Wood;

PX_Object* PX_Object_WoodCreate(PX_World* pWorld, PX_Runtime* pruntime);
#endif
