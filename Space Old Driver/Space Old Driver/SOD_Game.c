#include "SOD_Game.h"



px_bool SOD_GameInitialize(SOD_Game *game)
{
	if (!SOD_RuntimeInitialize(&game->sodrt))
	{
		return PX_FALSE;
	}

	if (!SOD_StartupInitialize(&game->sodrt,&game->startup))
	{
		return PX_FALSE;
	}

	if (!SOD_PlayInitialize(&game->sodrt,&game->play))
	{
		return PX_FALSE;
	}

	if (!SOD_MessageInitialize(&game->sodrt.runtime,&game->GameOver,SOD_RUNTIME_WINDOW_WIDTH,SOD_RUNTIME_WINDOW_HEIGHT))
	{
		return PX_FALSE;
	}

	if (!SOD_MessageInitialize(&game->sodrt.runtime,&game->About,SOD_RUNTIME_WINDOW_WIDTH,SOD_RUNTIME_WINDOW_HEIGHT))
	{
		return PX_FALSE;
	}
	game->last_game_time=0;
	game->last_game_pt=0;
	return PX_TRUE;
}

px_void SOD_GamePostEvent(SOD_Game *game,PX_Object_Event e)
{
	PX_ConsolePostEvent(&game->sodrt.Console,e);
	switch(game->status)
	{
	case SOD_GAME_STATUS_STARTUP:
		{
			SOD_StartupPostEvent(&game->startup,e);
		}
		break;
	case SOD_GAME_STATUS_GAMING:
		{
			SOD_PlayPostEvent(&game->play,e);
		}
		break;
	case SOD_GAME_STATUS_GAMEOVER:
		{
			SOD_MessagePostEvent(&game->GameOver,e);
		}
		break;
	case SOD_GAME_STATUS_ABOUT:
		{
			SOD_MessagePostEvent(&game->About,e);
		}
		break; 
	}
}
extern SOD_GameStartUp();
px_void SOD_GameUpdate(SOD_Game *game,px_dword elpased)
{
	PX_ConsoleUpdate(&game->sodrt.Console,elpased);
	switch (game->status)
	{
	case SOD_GAME_STATUS_STARTUP:
		{
		}
		break;
	case SOD_GAME_STATUS_GAMING:
		{
			SOD_PlayUpdate(&game->play,elpased);
		}
		break;
	case SOD_GAME_STATUS_GAMEOVER:
		{
			SOD_MessageUpdate(&game->GameOver,elpased);
			if (game->GameOver.mode==SOD_MESSAGE_MODE_CLOSE)
			{
				SOD_GameStartUp();
			}
		}
		break;
	case SOD_GAME_STATUS_ABOUT:
		{
			SOD_MessageUpdate(&game->About,elpased);
			if (game->About.mode==SOD_MESSAGE_MODE_CLOSE)
			{
				SOD_GameStartUp();
			}
		}
		break;
	}

}

px_void SOD_GameRender(SOD_Game *game,px_dword elpased)
{
	PX_SurfaceClear(&game->sodrt.runtime.RenderSurface,0,0,game->sodrt.runtime.RenderSurface.width-1,game->sodrt.runtime.RenderSurface.height-1,PX_COLOR(255,255,255,255));
	PX_ConsoleRender(&game->sodrt.Console,elpased);
	if (game->sodrt.Console.show)
	{
		return;
	}

	switch (game->status)
	{
	case SOD_GAME_STATUS_STARTUP:
		{
			SOD_StartupRender(&game->startup,elpased);
		}
		break;
	case SOD_GAME_STATUS_GAMING:
		{
			SOD_PlayRender(&game->play,elpased);
		}
		break;
	case SOD_GAME_STATUS_GAMEOVER:
		{
			px_int i;
			px_char Text[32];
			px_word wtext[32];
			//äÖÈ¾Message¿ò
			SOD_MessageRender(&game->GameOver,elpased);
			//äÖÈ¾»ý·Ö
			px_sprintf(Text,sizeof(Text),"%d",game->last_game_pt);
			i=0;
			while(Text[i]){wtext[i]=Text[i];i++;}
			wtext[i]=0;
			PX_FontModuleDrawText(&game->sodrt.runtime.RenderSurface,350,370,(px_uchar *)SOD_TEXT_SCORE,PX_COLOR(255,255,255,255),&game->sodrt.fontmodule);
			PX_FontModuleDrawText(&game->sodrt.runtime.RenderSurface,430,370,(px_uchar *)wtext,PX_COLOR(255,255,255,255),&game->sodrt.fontmodule);
		}
		break;
	case SOD_GAME_STATUS_ABOUT:
		{
			//äÖÈ¾Message¿ò
			SOD_MessageRender(&game->About,elpased);
		}
		break;
	}
}
