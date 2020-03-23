
#include "Platform_Windows_D3D.h"


#define         WIN_MAX_INPUT_STRING_LEN   64
#define         WIN_MAX_INPUT_SPECKEY_LEN  0xff


#define         WIN_KEYDOWN_BACKSPACE    1
#define         WIN_KEYDOWN_RETURN	      2
#define         WIN_KEYDOWN_LEFT         3
#define         WIN_KEYDOWN_RIGHT        4
#define         WIN_KEYDOWN_UP           5
#define         WIN_KEYDOWN_DOWN         6

typedef enum
{
	DINPUT_IO_DEVICE_TYPE_KEYBOARD ,
	DINPUT_IO_DEVICE_TYPE_MOUSE	   ,
	DINPUT_IO_DEVICE_TYPE_JOY	   ,
}DINPUT_IO_DEVICE_TYPE;


typedef struct 
{
	IDirectInputDevice8 *pDIDevice;
	GUID guid;
}DINPUT_INPUT_DEVICE;

HWND					Win_Hwnd;
int						Win_Height;
int						Win_Width;
BOOL                    Win_bFullScreen;
BOOL                    Win_Activated;
IDirect3D9				*DirectX_pDirect3D9;
IDirect3DDevice9		*DirectX_pD3DDevice;
ID3DXSprite				*DirectX_pD3DSprite;
D3DPRESENT_PARAMETERS	DirectX_d3dpp;
D3DDISPLAYMODE			DirectX_DisplayMode;
unsigned int			DirectX_Width;
unsigned int			DirectX_Height;
char					DirectX_BPP;
BOOL					DirectX_HAREWARE;
BOOL					DirectX_FullScreen;
IDirect3DTexture9	   *DirectX_RenderTexture;

IDirectInput8		   *DInput_pDirectInput;
unsigned char		   DInput_KeyBoardState[256];
char				   DInput_AccepyFile[MAX_PATH]={0};
DIMOUSESTATE		   DInput_MouseState;
DIJOYSTATE			   DInput_JoyState;
POINT				   DInput_MousePosition;
int                    DInput_JoyDeviceIndex=0,DInput_KeyboardDeviceIndex=0,DInput_MouseDeviceIndex=0;
DINPUT_INPUT_DEVICE    DInput_JoyDevice[8],DInput_KeyboardDevice[8],DInput_MouseDevice[8];


static WNDCLASSEX      Win_Wcx;

/////////////////////////////////////////////////////
LRESULT CALLBACK AppWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
/////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////
//				Get BPP(Color bit) from D3DFORMAT            	///
///////////////////////////////////////////////////////////////////
//************************************
// Method:    GetFormatBPP
// FullName:  GetFormatBPP
// Access:    public 
// Returns:   CHAR
// Qualifier:
// Parameter: D3DFORMAT Format
//************************************
static int GetFormatBPP( D3DFORMAT Format )
{
	switch(Format) {

	case D3DFMT_A8R8G8B8:
	case D3DFMT_X8R8G8B8:
		return 32;
		break;


	case D3DFMT_R8G8B8:
		return 24;
		break;


	case D3DFMT_R5G6B5:
	case D3DFMT_X1R5G5B5:
	case D3DFMT_A1R5G5B5:
	case D3DFMT_A4R4G4B4:
		return 16;
		break;


	case D3DFMT_A8P8:
	case D3DFMT_P8:
		return 8;
		break;

	default:
		return 0;
	}
}


static BOOL CheckFormat( D3DFORMAT Format, BOOL Windowed, BOOL HAL )
{

	if(FAILED(IDirect3D9_CheckDeviceType(DirectX_pDirect3D9,D3DADAPTER_DEFAULT, 
		(HAL == TRUE) ? D3DDEVTYPE_HAL : D3DDEVTYPE_REF,
		Format, Format, Windowed)))
		return FALSE;

	return TRUE;
}

BOOL DINPUT_CreateDevice(HWND hWnd,REFGUID guid,LPDIRECTINPUTDEVICE8 *ppInputDevice,DINPUT_IO_DEVICE_TYPE nType)
{
	DIPROPRANGE dipr;

	if(FAILED(IDirectInput8_CreateDevice(DInput_pDirectInput,guid,ppInputDevice,NULL)))
	{
		return FALSE;
	}

	switch(nType)
	{
	case DINPUT_IO_DEVICE_TYPE_KEYBOARD:
		if(FAILED(IDirectInputDevice8_SetDataFormat((*ppInputDevice),&c_dfDIKeyboard)))
		{
			IDirectInputDevice8_Release((*ppInputDevice));
			(*ppInputDevice)=NULL;
			return FALSE;
		}
		break;
	case DINPUT_IO_DEVICE_TYPE_MOUSE:
		if(FAILED(IDirectInputDevice8_SetDataFormat((*ppInputDevice),&c_dfDIMouse)))
		{
			IDirectInputDevice8_Release((*ppInputDevice));
			(*ppInputDevice)=NULL;
			return FALSE;
		}
		break;
	case DINPUT_IO_DEVICE_TYPE_JOY:
		if(FAILED(IDirectInputDevice8_SetDataFormat((*ppInputDevice),&c_dfDIJoystick)))
		{
			IDirectInputDevice8_Release((*ppInputDevice));
			(*ppInputDevice)=NULL;
			return FALSE;
		}
		break;
	default:
		return FALSE;
	}

	if(FAILED(IDirectInputDevice8_SetCooperativeLevel((*ppInputDevice),hWnd,DISCL_BACKGROUND|DISCL_NONEXCLUSIVE)))
	{
		IDirectInputDevice8_Release((*ppInputDevice));
		(*ppInputDevice)=NULL;
		return FALSE;
	}

	if (nType==DINPUT_IO_DEVICE_TYPE_JOY)
	{
		ZeroMemory(&dipr,sizeof DIPROPRANGE);
		dipr.diph.dwSize=sizeof(dipr);
		dipr.diph.dwHeaderSize=sizeof (dipr);
		dipr.diph.dwObj=DIJOFS_X;
		dipr.diph.dwHow=DIPH_BYOFFSET;

		dipr.lMin=-1024;
		dipr.lMax=1024;

		if (FAILED(IDirectInputDevice8_SetProperty((*ppInputDevice),DIPROP_RANGE,&dipr.diph)))
		{
			IDirectInputDevice8_Release((*ppInputDevice));
			(*ppInputDevice)=NULL;
			return FALSE;
		}
	}
	if (FAILED(IDirectInputDevice8_Acquire((*ppInputDevice))))
	{
		IDirectInputDevice8_Release((*ppInputDevice));
		(*ppInputDevice)=NULL;
		return FALSE;
	}

	return TRUE;
}

BOOL CALLBACK DInput_RecvJoyDevice(LPCDIDEVICEINSTANCE pDevicsInst,LPVOID pvRef)
{
	if(DInput_JoyDeviceIndex>=8)
	{
		return TRUE;
	}

	DInput_JoyDevice[DInput_JoyDeviceIndex].guid=pDevicsInst->guidInstance;
	if(DINPUT_CreateDevice(Win_Hwnd,DInput_JoyDevice[DInput_JoyDeviceIndex].guid,&DInput_JoyDevice[DInput_JoyDeviceIndex].pDIDevice,DINPUT_IO_DEVICE_TYPE_JOY))
	{
		DInput_JoyDeviceIndex++;
	}

	return DIENUM_CONTINUE;
}

BOOL CALLBACK DInput_RecvMouseDevice(LPCDIDEVICEINSTANCE pDevicsInst,LPVOID pvRef)
{
	if(DInput_MouseDeviceIndex>=8)
	{
		return TRUE;
	}

	DInput_MouseDevice[DInput_MouseDeviceIndex].guid=pDevicsInst->guidInstance;
	if(DINPUT_CreateDevice(Win_Hwnd,DInput_MouseDevice[DInput_MouseDeviceIndex].guid,&DInput_MouseDevice[DInput_MouseDeviceIndex].pDIDevice,DINPUT_IO_DEVICE_TYPE_MOUSE))
	{
		DInput_MouseDeviceIndex++;
	}

	return DIENUM_CONTINUE;
}

BOOL CALLBACK DInput_RecvKeyboardDevice(LPCDIDEVICEINSTANCE pDevicsInst,LPVOID pvRef)
{
	if(DInput_KeyboardDeviceIndex>=8)
	{
		return TRUE;
	}

	DInput_KeyboardDevice[DInput_KeyboardDeviceIndex].guid=pDevicsInst->guidInstance;
	if(DINPUT_CreateDevice(Win_Hwnd,DInput_KeyboardDevice[DInput_KeyboardDeviceIndex].guid,&DInput_KeyboardDevice[DInput_KeyboardDeviceIndex].pDIDevice,DINPUT_IO_DEVICE_TYPE_KEYBOARD))
	{
		DInput_KeyboardDeviceIndex++;
	}

	return DIENUM_CONTINUE;
}

BOOL DInput_ReadDevice(IDirectInputDevice8 *pDevice,void *DataBuffer,long BufferSize)
{
	HRESULT hr;
	if (pDevice==NULL)
	{
		return FALSE;
	}
	while(TRUE)
	{
		IDirectInputDevice8_Poll(pDevice);

		if (SUCCEEDED(hr=IDirectInputDevice8_GetDeviceState(pDevice,BufferSize,(LPVOID)DataBuffer)))
		{
			break;
		}

		if (hr!=DIERR_INPUTLOST&&hr!=DIERR_NOTACQUIRED)
		{
			return FALSE;
		}

		if (FAILED(IDirectInputDevice8_Acquire(pDevice)))
		{
			return FALSE;
		}
	}

	return TRUE;
}

void DInput_UpdateDevice()
{
	DInput_JoyDeviceIndex=0,DInput_KeyboardDeviceIndex=0,DInput_MouseDeviceIndex=0;
	IDirectInput8_EnumDevices(DInput_pDirectInput,DI8DEVTYPE_JOYSTICK,DInput_RecvJoyDevice,NULL,DIEDFL_ATTACHEDONLY);
	IDirectInput8_EnumDevices(DInput_pDirectInput,DI8DEVTYPE_MOUSE,DInput_RecvMouseDevice,NULL,DIEDFL_ATTACHEDONLY);
	IDirectInput8_EnumDevices(DInput_pDirectInput,DI8DEVTYPE_KEYBOARD,DInput_RecvKeyboardDevice,NULL,DIEDFL_ATTACHEDONLY);
}


void DInput_Update()
{
	BOOL KeyBoardReaded=FALSE;
	BOOL MouseReaded=FALSE;
	BOOL JoyReaded=FALSE;
	int i;
	POINT point;

	for(i=0;i<DInput_KeyboardDeviceIndex;i++)
	{
		if (!KeyBoardReaded)
		{
			if(!DInput_ReadDevice(DInput_KeyboardDevice[i].pDIDevice,DInput_KeyBoardState,256))
			{
				DInput_UpdateDevice();
				return;
			}
			else
			{
				KeyBoardReaded=TRUE;
			}
		}
	}

	for(i=0;i<DInput_MouseDeviceIndex;i++)
	{
		if (!MouseReaded)
		{
			if (!DInput_ReadDevice(DInput_MouseDevice[i].pDIDevice,&DInput_MouseState,sizeof DIMOUSESTATE))
			{
				DInput_UpdateDevice();
				return;
			}
			else
			{
				MouseReaded=TRUE;
			}
		}
	}
	
	for(i=0;i<DInput_JoyDeviceIndex;i++)
	{
		if (!JoyReaded)
		{
			if (!DInput_ReadDevice(DInput_JoyDevice[i].pDIDevice,&DInput_JoyState,sizeof DIJOYSTATE))
			{
				DInput_UpdateDevice();
				return;
			}
			else
			{
				JoyReaded=TRUE;
			}
		}
	}

	GetCursorPos(&point);
	ScreenToClient(Win_Hwnd,&point);
	DInput_MousePosition=point;
}

BOOL PX_D3DReset(HWND hWnd,int Width,int Height,BOOL bfullScreen)
{
	D3DFORMAT             Format, AltFormat;
	RECT                  WndRect, ClientRect; 
	long                  WndWidth, WndHeight;
	HRESULT				  hr;
	//Create Direct3D Device
	if((DirectX_pDirect3D9 = Direct3DCreate9(D3D_SDK_VERSION)) == NULL)
		return FALSE;

	//If failed then return
	if(DirectX_pDirect3D9 == NULL)
		return FALSE;


	if(FAILED(IDirect3D9_GetAdapterDisplayMode(DirectX_pDirect3D9,D3DADAPTER_DEFAULT, &DirectX_DisplayMode)))
		return FALSE;


	//If Width is not set
	if(!Width) 
	{
		//Set up width as screen width if FullScreen flag is mark;
		if(bfullScreen == TRUE) 
		{
			DirectX_Width = DirectX_DisplayMode.Width;
		} 
		else 
		{
			//if not ,Set up width as window width
			GetWindowRect(hWnd, &ClientRect);
			DirectX_Width = ClientRect.right-ClientRect.left;
		}
	} 
	else 
	{
		DirectX_Width = Width;
	}

	//just like width
	if(!Height) 
	{
		if(bfullScreen == TRUE) 
		{
			DirectX_Height = DirectX_DisplayMode.Height;
		} 
		else 
		{
			GetWindowRect(hWnd, &ClientRect);
			DirectX_Height = ClientRect.bottom-ClientRect.top;
		}
	} 
	else 
	{
		DirectX_Height = Height;
	}

	if(bfullScreen == FALSE) 
	{
		if(!(DirectX_BPP = GetFormatBPP(DirectX_DisplayMode.Format)))
			return FALSE;
	}

	if(bfullScreen == FALSE) 
	{
		GetWindowRect(hWnd, &WndRect);
		GetClientRect(hWnd, &ClientRect);

		WndWidth  = (WndRect.right  - (ClientRect.right  - DirectX_Width))  - WndRect.left;
		WndHeight = (WndRect.bottom - (ClientRect.bottom - DirectX_Height)) - WndRect.top;

		MoveWindow(hWnd, WndRect.left, WndRect.top, WndWidth, WndHeight, TRUE);
	}

	ZeroMemory(&DirectX_d3dpp, sizeof(D3DPRESENT_PARAMETERS));

	DirectX_HAREWARE = FALSE;

	if((DirectX_FullScreen = bfullScreen) == FALSE) 
	{
		DirectX_d3dpp.Windowed         = TRUE;
		DirectX_d3dpp.SwapEffect       = D3DSWAPEFFECT_DISCARD;
		DirectX_d3dpp.BackBufferFormat = DirectX_DisplayMode.Format;
		//Ensuring support for hardware acceleration
		if(CheckFormat(DirectX_DisplayMode.Format, TRUE, TRUE) == TRUE) 
		{
			DirectX_HAREWARE = TRUE;
		} 
		else 
		{
			//if hardware is not support,ensure software acceleration is support
			if(CheckFormat(DirectX_DisplayMode.Format, TRUE, FALSE) == FALSE)
				return FALSE;
		}
	} 
	else 
	{
		//Full screen mode
		DirectX_d3dpp.Windowed   = FALSE;
		DirectX_d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;

		DirectX_d3dpp.BackBufferWidth  = DirectX_Width;
		DirectX_d3dpp.BackBufferHeight = DirectX_Height;
		DirectX_d3dpp.FullScreen_RefreshRateInHz      = D3DPRESENT_RATE_DEFAULT;
		DirectX_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_ONE; 

		if(DirectX_BPP == 32) 
		{
			Format    = D3DFMT_X8R8G8B8;
			AltFormat = D3DFMT_X8R8G8B8;
		}
		if(DirectX_BPP == 24) 
		{
			Format    = D3DFMT_R8G8B8;
			AltFormat = D3DFMT_R8G8B8;
		}
		if(DirectX_BPP == 16) 
		{
			Format    = D3DFMT_R5G6B5;
			AltFormat = D3DFMT_X1R5G5B5;
		}
		if(DirectX_BPP == 8) 
		{
			Format    = D3DFMT_P8;
			AltFormat = D3DFMT_P8;
		}


		if(CheckFormat(Format, FALSE, TRUE) == TRUE) 
		{
			DirectX_HAREWARE = TRUE;
		} 
		else 
		{

			if(CheckFormat(AltFormat, FALSE, TRUE) == TRUE) 
			{
				DirectX_HAREWARE = TRUE;
				Format = AltFormat;
			} 
			else 
			{

				if(CheckFormat(Format, FALSE, FALSE) == FALSE) 
				{

					if(CheckFormat(AltFormat, FALSE, FALSE) == FALSE)
						return FALSE;
					else
						Format = AltFormat;
				}
			}
		}

		DirectX_d3dpp.BackBufferFormat = Format;
	}

	DirectX_d3dpp.EnableAutoDepthStencil = FALSE;

	DirectX_d3dpp.MultiSampleType=D3DMULTISAMPLE_4_SAMPLES;
	DirectX_d3dpp.MultiSampleQuality=0;

	if(FAILED(hr=IDirect3D9_CreateDevice(DirectX_pDirect3D9,D3DADAPTER_DEFAULT, 
		(DirectX_HAREWARE == TRUE) ? D3DDEVTYPE_HAL : D3DDEVTYPE_REF, 
		hWnd, D3DCREATE_SOFTWARE_VERTEXPROCESSING,
		&DirectX_d3dpp, &DirectX_pD3DDevice)))
	{                         
		return FALSE;
	}

	IDirect3DDevice9_SetTextureStageState(DirectX_pD3DDevice, 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	IDirect3DDevice9_SetTextureStageState(DirectX_pD3DDevice, 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
	IDirect3DDevice9_SetTextureStageState(DirectX_pD3DDevice, 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );

	IDirect3DDevice9_SetTextureStageState(DirectX_pD3DDevice,0, D3DTSS_ALPHAARG1,  D3DTA_DIFFUSE);
	IDirect3DDevice9_SetTextureStageState(DirectX_pD3DDevice,0, D3DTSS_ALPHAARG2,  D3DTA_TEXTURE);   //Ignored
	IDirect3DDevice9_SetTextureStageState(DirectX_pD3DDevice,0, D3DTSS_ALPHAOP,    D3DTOP_MODULATE);


	IDirect3DDevice9_SetSamplerState(DirectX_pD3DDevice,0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	IDirect3DDevice9_SetSamplerState(DirectX_pD3DDevice,0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);

	if(FAILED(D3DXCreateSprite(DirectX_pD3DDevice, &DirectX_pD3DSprite)))
		return FALSE;

	if(FAILED(D3DXCreateTexture(DirectX_pD3DDevice,DirectX_Width,DirectX_Height,1,0,D3DFMT_A8B8G8R8,D3DPOOL_MANAGED,&DirectX_RenderTexture)))
		return FALSE;

	if(FAILED(IDirect3DDevice9_SetTexture(DirectX_pD3DDevice,0,DirectX_RenderTexture)))
		return FALSE;

	return TRUE;
}

BOOL PX_CreateWindow( int Width,int Height,char *name,BOOL bfullScreen)
{
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
		if(!RegisterClassEx(&Win_Wcx))
			return FALSE;
	}


	Win_Width=Width;
	Win_Height=Height;
	Win_bFullScreen=bfullScreen;

	////////////////////////////////////////////////////////////////////////////
	//Create window
	////////////////////////////////////////////////////////////////////////////
	Win_Hwnd = CreateWindow("WindowCls", name,
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
	CoInitialize(NULL);
	DragAcceptFiles(Win_Hwnd,TRUE);
	//////////////////////////////////////////////////////////////////////////
	//
	
	if((Win_Hwnd) == NULL)
		return FALSE;

	
	if (!PX_D3DReset(Win_Hwnd,Width,Height,bfullScreen))
	{
		return FALSE;
	}

	//////////////////////////////////////////////////////////////////////////
	//Direct input
	if(FAILED(DirectInput8Create(GetModuleHandle(NULL),DIRECTINPUT_VERSION,IID_IDirectInput8,(LPVOID *)&DInput_pDirectInput,NULL)))
	{
		return FALSE;
	}

	DInput_UpdateDevice();

	return TRUE;
}
/////////////////////////////////////////////////////////////
char			Win_Str[WIN_MAX_INPUT_STRING_LEN];
char			Win_SpecKey[WIN_MAX_INPUT_SPECKEY_LEN];
int				Win_CurrentIndex=0;
LRESULT CALLBACK AppWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg) 
	{
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
			return 0;
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
			return 0;
		}
		break;
	case WM_DROPFILES:
		{
			HDROP hdrop = (HDROP)wParam;
			DragQueryFile(hdrop, 0, DInput_AccepyFile, MAX_PATH);
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

// Reset device
HRESULT ResetDevice(D3DPRESENT_PARAMETERS d3dpp)
{
	// Check device state
	HRESULT hr =IDirect3DDevice9_TestCooperativeLevel(DirectX_pD3DDevice);

	// Device can be reset now
	if (SUCCEEDED(hr) || hr == D3DERR_DEVICENOTRESET)
	{
		// Reset device
		IDirect3DDevice9_Reset(DirectX_pD3DDevice,&d3dpp) ;

		if (!PX_D3DReset(Win_Hwnd,Win_Width,Win_Height,Win_bFullScreen))
		{
			return FALSE;
		}
	}
	// Device is still in lost state, wait
	else if (hr == D3DERR_DEVICELOST)
	{
		Sleep(10) ;
	}

	return hr ;
}

BOOL PX_SystemRender(void *raw,int witdh,int height)
{
	D3DLOCKED_RECT lrect;
	HRESULT hr;
	RECT Rect;
	unsigned char *pBits;
	

	hr=IDirect3DTexture9_LockRect(DirectX_RenderTexture,0, &lrect, NULL, 0);
	if (FAILED(hr))
	{
		return FALSE;
	}	
	pBits=(unsigned char *)lrect.pBits;	

	memcpy(pBits,raw,witdh*height*sizeof(DWORD));

	IDirect3DTexture9_UnlockRect(DirectX_RenderTexture,0);
	

	if(FAILED(IDirect3DDevice9_Clear(DirectX_pD3DDevice,0, NULL, D3DCLEAR_TARGET, D3DXCOLOR(1,0,0,1), 1.0, 0)))
		return FALSE;

	hr=DirectX_pD3DDevice->BeginScene();
	if (FAILED(hr))
	{
		return FALSE;
	}

	hr=DirectX_pD3DSprite->Begin(D3DXSPRITE_ALPHABLEND);
	if (FAILED(hr))
	{
		return FALSE;
	}
	Rect.left=0;
	Rect.top  =0;
	Rect.right =DirectX_Width;
	Rect.bottom =DirectX_Height;


	if(FAILED(DirectX_pD3DSprite->Draw(DirectX_RenderTexture,&Rect,NULL,&D3DXVECTOR3(0,0,0),D3DXCOLOR(1,1,1,1))))
	{
		return FALSE;
	}

	DirectX_pD3DSprite->End();
	DirectX_pD3DDevice->EndScene();
	if(FAILED(DirectX_pD3DDevice->Present(0,0,0,0)))
		ResetDevice(DirectX_d3dpp);

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
	return DInput_MouseState.rgbButtons[0]&0x80;
}

BOOL PX_MouseRButtonDown()
{
	return DInput_MouseState.rgbButtons[1]&0x80;
}

BOOL PX_MouseMButtonDown()
{
	return DInput_MouseState.rgbButtons[2]&0x80;
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

BOOL PX_KeyDown(char key)
{
	if (Win_SpecKey[key])
	{
		Win_SpecKey[key]=0;
		return TRUE;
	}
	return FALSE;
}