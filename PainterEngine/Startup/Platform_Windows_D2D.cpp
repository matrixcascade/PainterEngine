#include <d2d1.h>
#include <d2d1helper.h>
#include <WindowsX.h>
#pragma comment(lib,"d2d1.lib")

typedef struct
{
	UINT uMsg;
	LPARAM lparam;
	WPARAM wparam;
}WM_MESSAGE;

extern "C" BOOL PX_CreateWindow( int Width,int Height,const char *name,BOOL bfullScreen);
extern "C" HWND PX_GetWindowHwnd();
extern "C" VOID PX_SystemReadDeviceState();
extern "C" BOOL PX_SystemLoop();
extern "C" BOOL PX_SystemRender(void *raw,int width,int height);
extern "C" BOOL PX_SystemisAvtivated();
extern "C" BOOL PX_KeyboardDown(unsigned char X );
extern "C" char *PX_KeyboardString();
extern "C" char *PX_DragfileString();
extern "C" BOOL PX_MouseLButtonDown();
extern "C" BOOL PX_MouseRButtonDown();
extern "C" BOOL PX_MouseMButtonDown();
extern "C" POINT PX_MousePosition();
extern "C" BOOL PX_KeyDown(unsigned char key);
extern "C" BOOL PX_MouseWheel(int *x,int *y,int *delta);
extern "C" BOOL PX_GetWinMessage(WM_MESSAGE *Msg);
extern "C" char *PX_OpenFileDialog(const char Filter[]);
extern "C" char *PX_SaveFileDialog(const char Filter[],const char ext[]);
extern "C" char *PX_MultFileDialog(const char Filter[]);
extern "C" char * PX_GetFileName(const char filePath[]);


#define         WIN_MAX_INPUT_STRING_LEN   64
#define         WIN_MAX_INPUT_SPECKEY_LEN  0xff


#define         WIN_KEYDOWN_BACKSPACE    1
#define         WIN_KEYDOWN_RETURN	     2
#define         WIN_KEYDOWN_LEFT         3
#define         WIN_KEYDOWN_RIGHT        4
#define         WIN_KEYDOWN_UP           5
#define         WIN_KEYDOWN_DOWN         6


#define			WIN_MESSAGESTACK_SIZE 32

HWND					Win_Hwnd;
int						Win_Height;
int						Win_Width;
BOOL                    Win_bFullScreen;
BOOL                    Win_Activated;
WM_MESSAGE				Win_messageStack[WIN_MESSAGESTACK_SIZE]={0};
unsigned char		   DInput_KeyBoardState[256];
char				   DInput_AccepyFile[MAX_PATH]={0};
POINT				   DInput_MousePosition;
POINT                  DInput_MouseWheelPosition;
POINT                  DInput_MouseWheelDelta;

ID2D1Bitmap			   *D2D_pSurface;
ID2D1Factory*			D2D_pDirect2dFactory;
ID2D1HwndRenderTarget  *D2D_pRenderTarget;




static WNDCLASSEXA      Win_Wcx;

/////////////////////////////////////////////////////
LRESULT CALLBACK AppWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
/////////////////////////////////////////////////////


void DInput_Update()
{
	POINT point;
	GetCursorPos(&point);
	ScreenToClient(Win_Hwnd,&point);
	DInput_MousePosition=point;
}

BOOL PX_D3DReset(HWND hWnd,int Width,int Height,BOOL bfullScreen)
{
	return TRUE;
}


BOOL PX_CreateWindow( int Width,int Height,const char *name,BOOL bfullScreen)
{
	HRESULT hr;
	D2D1_SIZE_U size;
	RECT rc;

	int nScreenWidth, nScreenHeight;
	nScreenWidth = GetSystemMetrics(SM_CXSCREEN);
	nScreenHeight = GetSystemMetrics(SM_CYSCREEN);


	if (Win_Wcx.cbSize==0)
	{
		////////////////////////////////////////////////////////////////////////////
		// Initialize Window class struct
		///////////////////////////////////////////////////////////////////////////
		Win_Wcx.cbSize        = sizeof(WNDCLASSEX);
		Win_Wcx.style         = CS_CLASSDC;
		Win_Wcx.lpfnWndProc   = AppWindowProc;
		Win_Wcx.cbClsExtra    = 0;
		Win_Wcx.cbWndExtra    = 0;
		Win_Wcx.hInstance     = GetModuleHandle(NULL);
		Win_Wcx.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
		Win_Wcx.hCursor       = LoadCursor(NULL, IDC_ARROW);
		Win_Wcx.hbrBackground = (HBRUSH)(COLOR_WINDOW);
		Win_Wcx.lpszMenuName  = NULL;
		Win_Wcx.lpszClassName = "WindowCls";
		Win_Wcx.hIconSm       = LoadIcon(NULL, IDI_APPLICATION);


		///////////////////////////////////////////////////////////////////////////
		//Class Register
		///////////////////////////////////////////////////////////////////////////
		if(!RegisterClassExA(&Win_Wcx))
			return FALSE;
	}


	Win_Width=Width;
	Win_Height=Height;
	Win_bFullScreen=bfullScreen;

	////////////////////////////////////////////////////////////////////////////
	//Create window
	////////////////////////////////////////////////////////////////////////////
	Win_Hwnd = CreateWindowA("WindowCls", name,
		WS_OVERLAPPED|WS_SYSMENU,
		CW_USEDEFAULT, CW_USEDEFAULT,
		Width, Height,
		NULL, NULL, GetModuleHandle(NULL), NULL);

	

	if(!Win_Hwnd)
		return FALSE;

	

	////////////////////////////////////////////////////////////////////////////
	//Show window
	////////////////////////////////////////////////////////////////////////////
	ShowWindow(Win_Hwnd, SW_SHOWNORMAL);
	UpdateWindow(Win_Hwnd);
	//CoInitialize(NULL);
	DragAcceptFiles(Win_Hwnd,TRUE);
	//////////////////////////////////////////////////////////////////////////
	//
	
	if((Win_Hwnd) == NULL)
		return FALSE;

	hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED,&D2D_pDirect2dFactory);
	if (FAILED(hr))
	{
		return FALSE;
	}
	
	GetClientRect(Win_Hwnd, &rc);
	SetWindowPos(Win_Hwnd,0,0,0,Width+Width-(rc.right - rc.left),Height+Height-(rc.bottom - rc.top),SWP_NOMOVE);
	GetClientRect(Win_Hwnd, &rc);

	
	size.width=rc.right - rc.left;
	size.height=rc.bottom - rc.top;

	hr=D2D_pDirect2dFactory->CreateHwndRenderTarget(
		D2D1::RenderTargetProperties(),
		D2D1::HwndRenderTargetProperties(Win_Hwnd, size),
		&D2D_pRenderTarget
		);

	if (FAILED(hr))
	{
		return FALSE;
	}

	D2D1_BITMAP_PROPERTIES bitmapformat;
	bitmapformat.dpiX=0;
	bitmapformat.dpiY=0;
	bitmapformat.pixelFormat=D2D1::PixelFormat(DXGI_FORMAT_R8G8B8A8_UNORM,D2D1_ALPHA_MODE_IGNORE);

	size = D2D1::SizeU(
		Width,
		Height
		);
	hr=D2D_pRenderTarget->CreateBitmap(size,bitmapformat,&D2D_pSurface);


	if (FAILED(hr))
	{
		return FALSE;
	}
	return (SUCCEEDED(hr));

	return TRUE;
}
/////////////////////////////////////////////////////////////
char			Win_Str[WIN_MAX_INPUT_STRING_LEN]={0};
char			Win_SpecKey[WIN_MAX_INPUT_SPECKEY_LEN]={0};
int				Win_CurrentIndex=0;
#include "stdio.h"
LRESULT CALLBACK AppWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{

// 	if(uMsg==0x246)
// 	printf("%x\n",uMsg);
	WM_MESSAGE message;
	int i;

	switch(uMsg) 
	{
	case WM_MOUSEMOVE:
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONDOWN:
	case WM_RBUTTONUP:
		break;
	case WM_KILLFOCUS:
		{
			Win_Activated=FALSE;
		}
		break;
	case WM_SETFOCUS:
		{
			Win_Activated=TRUE;
		}
		break;
	case WM_KEYDOWN:
		{
			Win_SpecKey[wParam]=1;
		}
		break;
	case WM_MOUSEWHEEL:
		{
			DInput_MouseWheelPosition.x=GET_X_LPARAM(lParam); 
			DInput_MouseWheelPosition.y=GET_Y_LPARAM(lParam); 
			DInput_MouseWheelDelta.x=0;
			DInput_MouseWheelDelta.y=GET_WHEEL_DELTA_WPARAM(wParam);
		}
		break;
	case WM_CHAR:
		{
			if(Win_CurrentIndex<WIN_MAX_INPUT_STRING_LEN-1)
			{
				if(wParam>=32||wParam==8)
				{
				Win_Str[Win_CurrentIndex++]=(char)wParam;
				Win_Str[Win_CurrentIndex]='\0';
				}
			}
		}
		break;
	case WM_DROPFILES:
		{
			HDROP hdrop = (HDROP)wParam;
			DragQueryFileA(hdrop, 0, DInput_AccepyFile, MAX_PATH);
			DragFinish(hdrop);
		}
		break;
	case WM_CLOSE:
		{
			PostQuitMessage(0);
			exit(0);
		}
		break;
	case WM_DESTROY:
		{
			PostQuitMessage(0);
			return 0;
		}
		break;
	default: return DefWindowProc(hWnd,uMsg,wParam,lParam);
	}

	message.uMsg=uMsg;
	message.wparam=wParam;
	message.lparam=lParam;
	for (i=0;i<WIN_MESSAGESTACK_SIZE;i++)
	{
		if (Win_messageStack[i].uMsg==0)
		{
			Win_messageStack[i]=message;
			break;
		}
	}

	return TRUE;
}

BOOL PX_SystemLoop()
{
	MSG Msg;
	ZeroMemory(&Msg, sizeof(MSG));

	if(PeekMessage(&Msg, NULL, 0, 0, PM_REMOVE)) 
	{
		TranslateMessage(&Msg);
		DispatchMessage(&Msg);
	} 
	else
	{
		Sleep(10);
	}
	return TRUE;
}


BOOL PX_SystemRender(void *raw,int width,int height)
{
	D2D1_RECT_U size;
	size.left=0;
	size.right=width;
	size.top=0;
	size.bottom=height;

	D2D_pRenderTarget->BeginDraw();
	D2D_pRenderTarget->Clear();
	D2D_pSurface->CopyFromMemory(&size,raw,width*sizeof(DWORD));
	D2D_pRenderTarget->DrawBitmap(D2D_pSurface);
	D2D_pRenderTarget->EndDraw();
	
	return TRUE;
}

BOOL PX_SystemisAvtivated()
{
	return Win_Activated;
}

BOOL PX_KeyboardDown(unsigned char X)
{
	return (DInput_KeyBoardState[X]&0x80?TRUE:FALSE);
}

BOOL PX_MouseLButtonDown()
{
	return GetAsyncKeyState(VK_LBUTTON) & 0xFF00;
}

BOOL PX_MouseRButtonDown()
{
	return GetAsyncKeyState(VK_RBUTTON) & 0xFF00;
}

BOOL PX_MouseMButtonDown()
{
	return GetAsyncKeyState(VK_MBUTTON) & 0xFF00;
}

POINT PX_MousePosition()
{
	return DInput_MousePosition;
}

char * PX_KeyboardString()
{
	if(Win_CurrentIndex!=0)
	{
	Win_CurrentIndex=0;
	return Win_Str;
	}
	else
	{
		return 0;
	}
}

VOID PX_SystemReadDeviceState()
{
	DInput_Update();
}

char * PX_DragfileString()
{
	return DInput_AccepyFile;
}

HWND PX_GetWindowHwnd()
{
	return Win_Hwnd;
}

BOOL PX_KeyDown(unsigned char key)
{
	if (Win_SpecKey[key])
	{
		Win_SpecKey[key]=0;
		return TRUE;
	}
	return FALSE;
}

BOOL PX_MouseWheel(int *x,int *y,int *delta)
{
	if (DInput_MouseWheelDelta.y!=0)
	{
		*x=DInput_MouseWheelPosition.x;
		*y=DInput_MouseWheelPosition.y;
		*delta=DInput_MouseWheelDelta.y;
		DInput_MouseWheelDelta.y=0;
		return TRUE;
	}
	return FALSE;
}

BOOL PX_GetWinMessage(WM_MESSAGE *Msg)
{
	int i;
	for (i=0;i<WIN_MESSAGESTACK_SIZE;i++)
	{
		if (Win_messageStack[i].uMsg!=0)
		{
			*Msg=Win_messageStack[i];
			memset(&Win_messageStack[i],0,sizeof(WM_MESSAGE));
			return TRUE;
		}
	}
	return FALSE;
}


char *PX_OpenFileDialog(const char Filter[])
{
	OPENFILENAMEA ofn;
	static char szFile[MAX_PATH];
	ZeroMemory(&ofn,sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.lpstrFile = szFile;
	ofn.lpstrFile[0] = TEXT('\0');
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter = Filter;//TEXT("Mirror文件(.mirror)\0*.mirror");
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL;
	ofn.hwndOwner = Win_Hwnd;
	ofn.Flags = OFN_EXPLORER |OFN_PATHMUSTEXIST ;

	if (GetSaveFileNameA(&ofn))
	{
		return szFile;
	}
	return NULL;
}

char *PX_MultFileDialog(const char Filter[])
{
	OPENFILENAMEA ofn;
	static char szFile[MAX_PATH*64];
	ZeroMemory(&ofn,sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.lpstrFile = szFile;
	ofn.lpstrFile[0] = TEXT('\0');
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter = Filter;//TEXT("Mirror文件(.mirror)\0*.mirror");
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL;
	ofn.hwndOwner = Win_Hwnd;
	ofn.Flags = OFN_EXPLORER |OFN_PATHMUSTEXIST|OFN_ALLOWMULTISELECT;

	if (GetOpenFileNameA(&ofn))
	{
		if (szFile[strlen(szFile)+1]=='\0')
		{
			int oft=strlen(szFile)-1;
			while (oft>=0)
			{
				if (szFile[oft]=='\\'||szFile[oft]=='/')
				{
					szFile[oft]=0;
					break;
				}
				oft--;
			}
		}
		return szFile;
	}
	return NULL;
}

char *PX_SaveFileDialog(const char Filter[],const char ext[])
{
	OPENFILENAMEA ofn;
	static char szFile[MAX_PATH];
	ZeroMemory(&ofn,sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.lpstrFile = szFile;
	ofn.lpstrFile[0] = TEXT('\0');
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter = Filter;//TEXT("Mirror文件(.mirror)\0*.mirror");
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL;
	ofn.hwndOwner = Win_Hwnd;
	ofn.Flags = OFN_EXPLORER |OFN_PATHMUSTEXIST ;

	if (GetSaveFileNameA(&ofn))
	{
		if (ext)
		{
			char uprFile[MAX_PATH]={0};
			char uprExt[MAX_PATH]={0};
			strcpy_s(uprExt,ext);
			_strupr_s(uprExt);
			strcpy_s(uprFile,szFile);
			_strupr_s(uprFile);
			if (!strstr(uprFile,uprExt))
			{
				strcat_s(szFile,ext);
			}
		}
		return szFile;
	}
	return NULL;
}


char * PX_GetFileName(const char filePath[])
{
	int offset=strlen(filePath)-1;
	while (offset)
	{
		if (filePath[offset]=='/'||filePath[offset]=='\\')
		{
			offset++;
			break;
		}
		offset--;
	}
	return (char *)filePath+offset;
}