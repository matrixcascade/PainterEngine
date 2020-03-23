#include "PainterEngine_Application.h"

//mouse informations
POINT main_zoomPoint;
px_int main_ZoomRegion;
//////////////////////////////////////////////////////////////////////////
DWORD WINAPI DEMO_RenderThreadFunc(LPVOID p)
{   
	DWORD time,elpased;
	PX_Object_Event e;
	WM_MESSAGE msg;
	static POINT LastDownPoint;
	LastDownPoint.x=-1;
	LastDownPoint.y=-1;
/*	px_char *keyBoardString;*/
	//////////////////////////////////////////////////////////////////////////
	//CreateWindow

	time=timeGetTime();

	while(1)
	{

		PX_SystemReadDeviceState();

		elpased=timeGetTime()-time;
		time=timeGetTime();
		

		while (PX_GetWinMessage(&msg))
		{
			px_char text[2]={0};
			e.Event=PX_OBJECT_EVENT_ANY;
			switch(msg.uMsg)
			{
			case WM_MOUSEMOVE:
				{
					if (msg.wparam&MK_LBUTTON)
					{
						e.Event=PX_OBJECT_EVENT_CURSORDRAG;
					}
					else
					{
						e.Event=PX_OBJECT_EVENT_CURSORMOVE;
					}

					e.Param_uint[0]=((msg.lparam)&0xffff);
					e.Param_uint[1]=((msg.lparam>>16)&0xffff);
				}
				break;
			case WM_LBUTTONDOWN:
				{
					e.Event=PX_OBJECT_EVENT_CURSORDOWN;
					e.Param_uint[0]=((msg.lparam)&0xffff);
					e.Param_uint[1]=((msg.lparam>>16)&0xffff);
					LastDownPoint.x=((msg.lparam)&0xffff);
					LastDownPoint.y=((msg.lparam>>16)&0xffff);
				}
				break;
			case WM_LBUTTONUP:
				{
					if (((msg.lparam)&0xffff)==LastDownPoint.x&&((msg.lparam>>16)&0xffff)==LastDownPoint.y)
					{
						e.Event=PX_OBJECT_EVENT_CURSORCLICK;
						e.Param_uint[0]=((msg.lparam)&0xffff);
						e.Param_uint[1]=((msg.lparam>>16)&0xffff);
						LastDownPoint.x=-1;
						LastDownPoint.y=-1;
						PX_ApplicationPostEvent(&App,e);
						PX_ConsolePostEvent(&App.Instance.console,e);
					}
					e.Event=PX_OBJECT_EVENT_CURSORUP;
					e.Param_uint[0]=((msg.lparam)&0xffff);
					e.Param_uint[1]=((msg.lparam>>16)&0xffff);

				}
				break;
			case WM_RBUTTONDOWN:
				{
					e.Event=PX_OBJECT_EVENT_CURSORRDOWN;
					e.Param_uint[0]=((msg.lparam)&0xffff);
					e.Param_uint[1]=((msg.lparam>>16)&0xffff);
				}
				break;
			case WM_RBUTTONUP:
				{
					e.Event=PX_OBJECT_EVENT_CURSORRUP;
					e.Param_uint[0]=((msg.lparam)&0xffff);
					e.Param_uint[1]=((msg.lparam>>16)&0xffff);
				}
				break;

			case  WM_KEYDOWN:
				{
					e.Event=PX_OBJECT_EVENT_KEYDOWN;
					e.Param_uint[0]=msg.wparam;
				}
				break;
			case WM_MOUSEWHEEL:
				{
					RECT rect,crect;
					GetClientRect(PX_GetWindowHwnd(),&crect);
					GetWindowRect(PX_GetWindowHwnd(),&rect);
					e.Event=PX_OBJECT_EVENT_CURSORWHEEL;
					e.Param_int[0]=((msg.lparam)&0xffff)-rect.left;
					e.Param_int[1]=((msg.lparam>>16)&0xffff)-rect.top-((rect.bottom-rect.top)-crect.bottom)+2;
					e.Param_int[2]=(short)((msg.wparam>>16)&0xffff);
					
				}
				break;
			case WM_CHAR:
				{

					text[0]=(px_char)msg.wparam;
					e.Event=PX_OBJECT_EVENT_STRING;
					e.Param_ptr[0]=text;
				}
				break;
			case WM_GESTURE:
				{
					GESTUREINFO gi;  
					BOOL bHandled;
					BOOL bResult;
					ZeroMemory(&gi, sizeof(GESTUREINFO));

					gi.cbSize = sizeof(GESTUREINFO);

					bResult  = GetGestureInfo((HGESTUREINFO)msg.lparam, &gi);
					bHandled = FALSE;

					if (bResult){
						// now interpret the gesture
						switch (gi.dwID){
						case GID_ZOOM:
							// Code for zooming goes here    

							if (25>(main_zoomPoint.x-gi.ptsLocation.x)*(main_zoomPoint.x-gi.ptsLocation.x)+(main_zoomPoint.y-gi.ptsLocation.y)*(main_zoomPoint.y-gi.ptsLocation.y))
							{
								e.Event=PX_OBJECT_EVENT_SCALE;
								e.Param_int[0]=gi.ptsLocation.x;
								e.Param_int[1]=gi.ptsLocation.y;
								e.Param_int[2]=(px_int)gi.ullArguments-main_ZoomRegion;
							}

							main_zoomPoint.x=gi.ptsLocation.x;
							main_zoomPoint.y=gi.ptsLocation.y;
							main_ZoomRegion=(px_int)gi.ullArguments;
							
							bHandled = TRUE;
							break;
						case GID_PAN:
							// Code for panning goes here
							bHandled = TRUE;
							break;
						case GID_ROTATE:
							// Code for rotation goes here
							bHandled = TRUE;
							break;
						case GID_TWOFINGERTAP:
							// Code for two-finger tap goes here
							bHandled = TRUE;
							break;
						case GID_PRESSANDTAP:
							// Code for roll over goes here
							bHandled = TRUE;
							break;
						default:
							// A gesture was not recognized
							break;
						}
						CloseGestureInfoHandle((HGESTUREINFO)msg.lparam);
					}else{
						DWORD dwErr = GetLastError();
						if (dwErr > 0){
							//MessageBoxW(hWnd, L"Error!", L"Could not retrieve a GESTUREINFO structure.", MB_OK);
						}
					}
				}
				break;
			default:
				continue;
			}

			if(e.Event!=PX_OBJECT_EVENT_ANY)
			{
				PX_ApplicationPostEvent(&App,e);
				PX_ConsolePostEvent(&App.Instance.console,e);
			}
			
		}
		PX_ApplicationUpdate(&App,elpased);
		PX_ApplicationRender(&App,elpased);
		PX_SystemRender(App.Instance.runtime.RenderSurface.surfaceBuffer,PX_WINDOW_WIDTH,PX_WINDOW_HEIGHT);
		Sleep(0);
	}
	return 0;
}
//int main()
int WINAPI WinMain( __in HINSTANCE hInstance, __in_opt HINSTANCE hPrevInstance, __in LPSTR lpCmdLine, __in int nShowCmd )
{
	HANDLE hThread;
	DWORD  threadId;
	PX_srand(314159);
	if(!PX_InstanceInitialize(&App.Instance,PX_WINDOW_NAME,PX_WINDOW_WIDTH,PX_WINDOW_HEIGHT,PX_MEMORY_UI_SIZE,PX_MEMORY_RESOURCES_SIZE,PX_MEMORY_GAME_SIZE)) return 0;
	if(!PX_ApplicationInitialize(&App))return 0;
	//////////////////////////////////////////////////////////////////////////
	//CreateThread
	
	hThread = CreateThread(NULL, 0, DEMO_RenderThreadFunc, 0, 0, &threadId);
	PX_AudioInitialize(PX_GetWindowHwnd(),&App.Instance.soundplay,PX_FALSE);
	
	while(PX_SystemLoop()){};

	return 0;
}


