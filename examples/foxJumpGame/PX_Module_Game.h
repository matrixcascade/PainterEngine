#ifndef PX_MODULE_GAME_H
#define PX_MODULE_GAME_H


#include "PainterEngine_Startup.h"
#include "PX_Object_Fox.h"
#include "PX_Object_Ground.h"
#include "PX_Object_Wood.h"
#include "PX_Object_Bird.h"
#include "PX_Object_Cloud.h"

#define PX_MODULE_GAME_UPDATE 10

typedef enum 
{
	PX_Module_Game_State_Ready,
	PX_Module_Game_State_Runing,
	PX_Module_Game_State_Stop,
}PX_Module_Game_State;

typedef struct
{
	PX_Runtime *pRuntime;
	PX_World world;

	px_dword wood_gen;
	px_dword bird_gen;
	px_dword cloud_gen;

	PX_Module_Game_State state;
	PX_Object* pSchedular;
	PX_Object* pFoxObject;
	PX_Object* pGroundObject;
}PX_Module_Game;

PX_Object *PX_Module_GameInstall(PX_Runtime *pruntime);
px_void PX_Module_GameUninstall(PX_Object* pModule);
#endif
