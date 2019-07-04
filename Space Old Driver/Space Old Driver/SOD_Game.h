#ifndef SOD_GAME_H
#define SOD_GAME_H

#include "SOD_Runtime.h"
#include "SOD_Startup.h"
#include "SOD_Play.h"
#include "SOD_Message.h"

typedef enum
{
	SOD_GAME_STATUS_STARTUP,
	SOD_GAME_STATUS_GAMING,
	SOD_GAME_STATUS_GAMEOVER,
	SOD_GAME_STATUS_ABOUT,
	SOD_GAME_STATUS_REPLY,
}SOD_GAME_Status;

typedef struct
{
	SOD_GAME_Status status;
	SOD_Runtime sodrt;
	SOD_Startup startup;
	SOD_Play play;
	SOD_Message GameOver;
	SOD_Message About;
	px_dword last_game_time;
	px_dword last_game_pt;
}SOD_Game;

px_bool SOD_GameInitialize(SOD_Game *game);
px_void SOD_GamePostEvent(SOD_Game *game,PX_Object_Event e);
px_void SOD_GameUpdate(SOD_Game *game,px_dword elpased);
px_void SOD_GameRender(SOD_Game *game,px_dword elpased);
#endif
