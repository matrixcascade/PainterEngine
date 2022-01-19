#ifndef PX_OBJECT_GROUND_H
#define PX_OBJECT_GROUND_H
#include "PainterEngine_Startup.h"
#include "PX_GameEvent.h"

#define PX_OBJECT_GROUND_TEXTURE_COUNT 5
#define PX_OBJECT_GROUND_BASE 128
#define PX_OBJECT_GROUND_Z 10

typedef enum 
{
	PX_Object_Ground_State_Ready,
	PX_Object_Ground_State_Run,
	PX_Object_Ground_State_Stop
}PX_Object_Ground_State;

typedef struct  
{
	PX_World* pWorld;
	PX_Runtime* pruntime;
	px_bool run;
	px_float xoffset;
	px_list texturelist;
	px_texture textures[PX_OBJECT_GROUND_TEXTURE_COUNT];

	PX_Object_Ground_State state;
}PX_Object_Ground;

PX_Object* PX_Object_GroundCreate(PX_World* pWorld, PX_Runtime* pruntime);

#endif
