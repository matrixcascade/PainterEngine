#include "SOD_Game.h"


//mouse informations
POINT main_MouseLastPosition;
BOOL  main_MouseLDown=0;
HWAVEOUT main_hWaveOut;
HANDLE main_waveEvent;
#define MAIN_WAVEBUFFER_BLOCK_LEN 1764
#define MAIN_WAVEBUFFER_BLOCK_COUNT 8

SOD_Game game;
px_byte main_wavebuffer[MAIN_WAVEBUFFER_BLOCK_LEN*MAIN_WAVEBUFFER_BLOCK_COUNT]={0};

px_void SOD_GameStartUp()
{
	game.status=SOD_GAME_STATUS_STARTUP;
}

px_void SOD_GamePlay()
{
	game.status=SOD_GAME_STATUS_GAMING;
	SOD_PlayReset(&game.play);
}

px_void SOD_GameOver(px_dword pt,px_dword alive_time)
{
	game.status=SOD_GAME_STATUS_GAMEOVER;
	SOD_MessageAlert_OK(&game.GameOver,SOD_KEY_TEXT_GAMEOVER);

	game.last_game_pt=pt;
	game.last_game_time=alive_time;
}

px_void SOD_GameAbout()
{
	game.status=SOD_GAME_STATUS_ABOUT;
	SOD_MessageAlert_OK(&game.About,SOD_KEY_TEX_ABOUT);
}

DWORD  _stdcall DEMO_waveOutProc(LPVOID p)
{
	WAVEHDR waveheader;
	px_int QueueSize=0;
	px_int wIndex=0;

	while(1)
	{
		if (!PX_SoundPlayGetDataCount(&game.sodrt.sound))
		{
			QueueSize=0;
		}
		while (QueueSize<MAIN_WAVEBUFFER_BLOCK_COUNT)
		{
			px_byte *data=main_wavebuffer+wIndex*MAIN_WAVEBUFFER_BLOCK_LEN;
			PX_SoundPlayRead(&game.sodrt.sound,data,MAIN_WAVEBUFFER_BLOCK_LEN);
			waveheader.lpData=(LPSTR)data;
			waveheader.dwBufferLength=MAIN_WAVEBUFFER_BLOCK_LEN;
			waveheader.dwBytesRecorded=0;
			waveheader.dwUser=0;
			waveheader.dwFlags=WHDR_PREPARED;
			waveheader.dwLoops=0;
			waveheader.lpNext=PX_NULL;
			waveheader.reserved=PX_NULL;
			waveOutWrite (main_hWaveOut, &waveheader, sizeof (WAVEHDR)) ;
			QueueSize++;
			wIndex++;
			if (wIndex>=MAIN_WAVEBUFFER_BLOCK_COUNT)
			{
				wIndex=0;
			}
		}

		WaitForSingleObject(main_waveEvent,INFINITE);
		ResetEvent(main_waveEvent);
		if(QueueSize>0)
			QueueSize--;		
	}
}

DWORD WINAPI DEMO_RenderThreadFunc(LPVOID p)
{   
	px_int i;
	DWORD time,elpased;
	POINT mousePosition;
	PX_Object_Event e;
	char *keyBoardString,*dragFileString;
	//////////////////////////////////////////////////////////////////////////
	//CreateWindow


	time=timeGetTime();

	while(1)
	{

		PX_SystemReadDeviceState();

		elpased=timeGetTime()-time;
		time=timeGetTime();
		if(PX_SystemisAvtivated())
		{
			mousePosition=PX_MousePosition();
			if (main_MouseLastPosition.x!=mousePosition.x||main_MouseLastPosition.y!=mousePosition.y)
			{
				if (mousePosition.x>0&&mousePosition.x<SOD_RUNTIME_WINDOW_WIDTH&&mousePosition.y>0&&mousePosition.y<SOD_RUNTIME_WINDOW_HEIGHT)
				{
					if(PX_MouseLButtonDown())
						e.Event=PX_OBJECT_EVENT_CURSORDRAG;
					else
						e.Event=PX_OBJECT_EVENT_CURSORMOVE;

					if(PX_MouseLButtonDown())
						e.Event=PX_OBJECT_EVENT_CURSORDRAG;
					else
						e.Event=PX_OBJECT_EVENT_CURSORMOVE;

					e.Param_uint[0]=mousePosition.x;
					e.Param_uint[1]=mousePosition.y;
					main_MouseLastPosition=mousePosition;
					SOD_GamePostEvent(&game,e);
				}
			}


			if (PX_MouseLButtonDown())
			{
				if (!main_MouseLDown&&mousePosition.x>0&&mousePosition.x<SOD_RUNTIME_WINDOW_WIDTH&&mousePosition.y>0&&mousePosition.y<SOD_RUNTIME_WINDOW_HEIGHT)
				{
					e.Event=PX_OBJECT_EVENT_CURSORDOWN;
					e.Param_uint[0]=mousePosition.x;
					e.Param_uint[1]=mousePosition.y;
					SOD_GamePostEvent(&game,e);
				}
				main_MouseLDown=PX_TRUE;
			}
			else
			{
				if (main_MouseLDown&&mousePosition.x>0&&mousePosition.x<SOD_RUNTIME_WINDOW_WIDTH&&mousePosition.y>0&&mousePosition.y<SOD_RUNTIME_WINDOW_HEIGHT)
				{
					e.Event=PX_OBJECT_EVENT_CURSORUP;
					e.Param_uint[0]=mousePosition.x;
					e.Param_uint[1]=mousePosition.y;
					SOD_GamePostEvent(&game,e);
				}
				main_MouseLDown=PX_FALSE;
			}


			if (keyBoardString=PX_KeyboardString())
			{
				e.Event=PX_OBJECT_EVENT_STRING;
				e.param_ptr[0]=keyBoardString;
				SOD_GamePostEvent(&game,e);
			}

			for (i=0;i<0xff;i++)
			{
				if (PX_KeyDown(i))
				{
					e.Event=PX_OBJECT_EVENT_KEYDOWN;
					e.Param_uint[0]=i;
					SOD_GamePostEvent(&game,e);
				}
			}
		}

		if ((dragFileString=PX_DragfileString())[0])
		{
			e.Event=PX_OBJECT_EVENT_DRAGFILE;
			e.param_ptr[0]=dragFileString;
			SOD_GamePostEvent(&game,e);
			dragFileString[0]='\0';
		}

		SOD_GameUpdate(&game,elpased);
		SOD_GameRender(&game,elpased);
		PX_SystemRender(game.sodrt.runtime.RenderSurface.surfaceBuffer,SOD_RUNTIME_WINDOW_WIDTH,SOD_RUNTIME_WINDOW_HEIGHT);
		Sleep(0);
	}
	return 0;
}




int main()
//int WINAPI WinMain( __in HINSTANCE hInstance, __in_opt HINSTANCE hPrevInstance, __in LPSTR lpCmdLine, __in int nShowCmd )
{
	
	HANDLE hThread;
	DWORD  threadId;
	WAVEFORMATEX waveformat;


	waveformat.wFormatTag = WAVE_FORMAT_PCM;
	waveformat.nSamplesPerSec = 44100;//44100;
	waveformat.wBitsPerSample = 16;//
	waveformat.nChannels = 2;//
	waveformat.nBlockAlign = (waveformat.wBitsPerSample >> 3) * waveformat.nChannels;;
	waveformat.nAvgBytesPerSec = waveformat.nBlockAlign * waveformat.nSamplesPerSec;
	waveformat.cbSize = 0;

	PX_srand(477608346);

	main_waveEvent= CreateEvent(NULL,PX_FALSE,PX_FALSE,"PCM_DONE");

	if(FAILED(waveOutOpen(&main_hWaveOut,WAVE_MAPPER,&waveformat,(DWORD_PTR)main_waveEvent,(DWORD_PTR)PX_NULL,CALLBACK_EVENT)))
		 PX_FALSE;

	if(!SOD_GameInitialize(&game)) return PX_FALSE;

	if (!PX_CreateWindow(SOD_RUNTIME_WINDOW_WIDTH,SOD_RUNTIME_WINDOW_HEIGHT,"PainterEngine SOD",PX_FALSE))
	{
		return 0;
	}

	//////////////////////////////////////////////////////////////////////////
	//CreateThread
	hThread = CreateThread(NULL, 0, DEMO_waveOutProc, 0, 0, &threadId);
	hThread = CreateThread(NULL, 0, DEMO_RenderThreadFunc, 0, 0, &threadId);
	
	while(PX_SystemLoop()){};

	return 0;
}

