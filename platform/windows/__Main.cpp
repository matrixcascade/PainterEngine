


extern "C"
{
#include "PainterEngine_Template.h"
};
#include "resource.h"
#include "../../../PainterEngine/platform/windows/Platform_Windows_D2D.h"


#pragma comment(lib,"User32.lib")
#pragma comment(lib,"Shell32.lib")
#pragma comment(lib,"Ole32.lib")

px_surface *main_pSurface;
//mouse informations
POINT main_MouseLastPosition;
BOOL  main_MouseLDown=0;


PT_Template game_template;

DWORD WINAPI ThreadFunc(LPVOID p)
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
		Sleep(1);
		PX_SystemReadDeviceState();

		elpased=timeGetTime()-time;
		time=timeGetTime();
		if(PX_SystemisAvtivated())
		{
			e.user=PX_NULL;
			mousePosition=PX_MousePosition();
			if (main_MouseLastPosition.x!=mousePosition.x||main_MouseLastPosition.y!=mousePosition.y)
			{
				if (mousePosition.x>0&&mousePosition.x<PE_WINDOW_WIDTH&&mousePosition.y>0&&mousePosition.y<PE_WINDOW_HEIGHT)
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
					PT_Event(&game_template,e);
				}
			}


			if (PX_MouseLButtonDown())
			{
				if (!main_MouseLDown&&mousePosition.x>0&&mousePosition.x<PE_WINDOW_WIDTH&&mousePosition.y>0&&mousePosition.y<PE_WINDOW_HEIGHT)
				{
					e.Event=PX_OBJECT_EVENT_CURSORDOWN;
					e.Param_uint[0]=mousePosition.x;
					e.Param_uint[1]=mousePosition.y;
					PT_Event(&game_template,e);
				}
				main_MouseLDown=PX_TRUE;
			}
			else
			{
				if (main_MouseLDown&&mousePosition.x>0&&mousePosition.x<PE_WINDOW_WIDTH&&mousePosition.y>0&&mousePosition.y<PE_WINDOW_HEIGHT)
				{
					e.Event=PX_OBJECT_EVENT_CURSORUP;
					e.Param_uint[0]=mousePosition.x;
					e.Param_uint[1]=mousePosition.y;
					PT_Event(&game_template,e);
				}
				main_MouseLDown=PX_FALSE;
			}


			if (keyBoardString=PX_KeyboardString())
			{
				e.Event=PX_OBJECT_EVENT_STRING;
				e.user=keyBoardString;
				PT_Event(&game_template,e);
			}

			for (i=0;i<0xff;i++)
			{
				if (PX_KeyDown(i))
				{
					e.Event=PX_OBJECT_EVENT_KEYDOWN;
					e.Param_uint[0]=i;
					PT_Event(&game_template,e);
				}
			}
		}
		
		if ((dragFileString=PX_DragfileString())[0])
		{
			e.Event=PX_OBJECT_EVENT_DRAGFILE;
			e.user=dragFileString;
			PT_Event(&game_template,e);
			dragFileString[0]='\0';
		}

		PT_Update(&game_template,elpased);
		PT_Render(&game_template,elpased);
		PX_SystemRender(game_template.game_template.runtime.PE_RenderSurface.surfaceBuffer,PE_WINDOW_WIDTH,PE_WINDOW_HEIGHT);
	}
	return 0;
}
int main()
//int WINAPI WinMain( __in HINSTANCE hInstance, __in_opt HINSTANCE hPrevInstance, __in LPSTR lpCmdLine, __in int nShowCmd )
{
	
	HANDLE hThread;
	DWORD  threadId;

	PX_srand(timeGetTime());


	if(!PT_Initialize(&game_template)) return PX_FALSE;


	if (!PX_CreateWindow(PE_WINDOW_WIDTH,PE_WINDOW_HEIGHT,"PainterEngine",PX_FALSE))
	{
		return 0;
	}

	HICON hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICON1));
	if (NULL != hIcon)
	{
		SendMessage(PX_GetWindowHwnd(), WM_SETICON, ICON_BIG, (LPARAM)hIcon);
		SendMessage(PX_GetWindowHwnd(), WM_SETICON, ICON_SMALL, (LPARAM)hIcon);
		SendMessage(PX_GetWindowHwnd(), WM_SETICON, ICON_SMALL2, (LPARAM)hIcon);
	}
	


	//////////////////////////////////////////////////////////////////////////
	//CreateThread
	hThread = CreateThread(NULL, 0, ThreadFunc, 0, 0, &threadId);

	while(PX_SystemLoop()){};

	return 0;
}

