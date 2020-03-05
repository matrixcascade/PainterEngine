#define _CRT_SECURE_NO_WARNINGS
#include "PainterEngineHelper.h"


//mouse informations

//////////////////////////////////////////////////////////////////////////
//system
POINT		main_MouseLastPosition;
BOOL		main_MouseLDown=0;
HWAVEOUT	main_hWaveOut;
HANDLE		main_waveEvent;
#define		MAIN_WAVEBUFFER_BLOCK_LEN 8*1024
#define		MAIN_WAVEBUFFER_BLOCK_COUNT 16
#define     MAIN_RENDER_FPS             60
px_byte		main_wavebuffer[MAIN_WAVEBUFFER_BLOCK_LEN*MAIN_WAVEBUFFER_BLOCK_COUNT]={0};


//////////////////////////////////////////////////////////////////////////
//Painter Engine runtime
px_surface		main_DrawSurface;
PX_Runtime		main_runtime;
PX_Object		*main_root;
PX_Console      main_console;

px_dword		main_time=0;
px_dword        main_renderElpased=0;

px_color        main_backgroundColor;

px_void PX_SetBackgroundColor(px_color clr)
{
	main_backgroundColor=clr;
}

px_void PX_Clear(px_color clr)
{
	PX_SurfaceClear(&main_DrawSurface,0,0,main_DrawSurface.width-1,main_DrawSurface.height-1,clr);
}

px_void PX_ShowConsole()
{
	PX_ConsoleShow(&main_console,PX_TRUE);
}

px_void PX_HideConsole()
{
	PX_ConsoleShow(&main_console,PX_FALSE);
}

//mouse informations
POINT main_zoomPoint;
px_int main_ZoomRegion;
static POINT LastDownPoint;

px_bool PX_Loop()
{   
	DWORD elpased=0;
	PX_Object_Event e;
	WM_MESSAGE msg;

	elpased=timeGetTime()-main_time;
	main_time=timeGetTime();


	if(main_renderElpased>1000/MAIN_RENDER_FPS)
	{
		
		PX_SystemReadDeviceState();

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
						if(main_console.show)
							PX_ConsolePostEvent(&main_console,e);
						else
							PX_ObjectPostEvent(main_root,e);
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
					LastDownPoint.x=((msg.lparam)&0xffff);
					LastDownPoint.y=((msg.lparam>>16)&0xffff);
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
					RECT rect;
					GetWindowRect(PX_GetWindowHwnd(),&rect);
					e.Event=PX_OBJECT_EVENT_CURSORWHEEL;
					e.Param_int[0]=((msg.lparam)&0xffff)-rect.left;
					e.Param_int[1]=((msg.lparam>>16)&0xffff)-rect.top;
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
				if(main_console.show)
					PX_ConsolePostEvent(&main_console,e);
				else
					PX_ObjectPostEvent(main_root,e);
			}

		}

		PX_SurfaceClear(&main_runtime.RenderSurface,0,0,main_runtime.RenderSurface.width-1,main_runtime.RenderSurface.height-1,main_backgroundColor);
		PX_SurfaceRender(&main_runtime.RenderSurface,&main_DrawSurface,0,0,PX_TEXTURERENDER_REFPOINT_LEFTTOP,PX_NULL);
		if (main_console.show)
		{
			PX_ConsoleUpdate(&main_console,main_renderElpased);
			PX_ConsoleRender(&main_console,main_renderElpased);
		}
		else
		{
			PX_ObjectUpdate(main_root,main_renderElpased);
			PX_ObjectRender(&main_runtime.RenderSurface,main_root,main_renderElpased);
		}
		
		PX_SystemRender(main_runtime.RenderSurface.surfaceBuffer,main_runtime.width,main_runtime.height);
		main_renderElpased=0;
	}
	else
	{
		main_renderElpased+=elpased;
		PX_SystemLoop();
	}
	return PX_TRUE;
}



px_bool PX_Initialize(px_char *name,px_int width,px_int height)
{

	PX_srand(314159265);

	if (!PX_CreateWindow(width,height,name,PX_FALSE))
	{
		return 0;
	}

	if (!PX_RuntimeInitialize(&main_runtime,width,height,malloc(PE_MEMORY_RUNTIME_SIZE),PE_MEMORY_RUNTIME_SIZE,PE_MEMORY_UI_SIZE,PE_MEMORY_RESOURCES_SIZE,PE_MEMORY_GAME_SIZE))
		return PX_FALSE;

	if (!PX_ConsoleInitialize(&main_runtime,&main_console))
		return PX_FALSE;

	if(!PX_SurfaceCreate(&main_runtime.mp,width,height,&main_DrawSurface))
	{
		return PX_FALSE;
	}

	main_root=PX_ObjectCreate(&main_runtime.mp_ui,PX_NULL,0,0,0,0,0,0);
	main_time=timeGetTime();

	main_backgroundColor=PX_COLOR(255,255,255,255);

	return PX_TRUE;
}

PX_Object * PX_GetUiRoot()
{
	return main_root;
}

PX_Runtime * PX_GetRuntime()
{
	return &main_runtime;
}

PX_ResourceLibrary * PX_GetResourceLibrary()
{
	return &main_runtime.ResourceLibrary;
};

px_memorypool * PX_GetMP()
{
	return &main_runtime.mp_resources;
}

px_surface * PX_GetSurface()
{
	return &main_DrawSurface;
}


//////////////////////////////////////////////////////////////////////////
//Functions

PX_IO_Data PX_LoadFileToIOData(px_char *path)
{
	PX_IO_Data io;
	px_int fileoft=0;
	FILE *pf=fopen(path,"rb");
	px_int filesize;
	if (!pf)
	{
		goto _ERROR;
	}
	fseek(pf,0,SEEK_END);
	filesize=ftell(pf);
	fseek(pf,0,SEEK_SET);

	io.buffer=(px_byte *)malloc(filesize+1);
	if (!io.buffer)
	{
		goto _ERROR;
	}

	while (!feof(pf))
	{
		fileoft+=(px_int)fread(io.buffer+fileoft,1,1024,pf);
	}
	fclose(pf);

	io.buffer[filesize]='\0';
	io.size=filesize;
	return io;
_ERROR:
	io.buffer=PX_NULL;
	io.size=0;
	return io;
}

px_void PX_FreeIOData(PX_IO_Data *io)
{
	if (io->size&&io->buffer)
	{
		free(io->buffer);
		io->size=0;
		io->buffer=PX_NULL;
	}
}

px_bool PX_LoadTextureFromFile(px_memorypool *mp,px_texture *tex,px_char *path)
{
	PX_IO_Data io;
	
#ifdef PAINTERENGINE_IMAGEDECODER_H
	if (PX_TextureCreateFromPngFile(mp,tex,path))
	{
		return PX_TRUE;
	}
	if (PX_TextureCreateFromJpegFile(mp,tex,path))
	{
		return PX_TRUE;
	}
#endif

	io=PX_LoadFileToIOData(path);
	if (!io.size)
	{
		return PX_FALSE;
	}
	if (PX_TextureCreateFromMemory(mp,io.buffer,io.size,tex))
	{
		PX_FreeIOData(&io);
		return PX_TRUE;
	}
	PX_FreeIOData(&io);
	return PX_FALSE;
}

px_bool PX_LoadShapeFromFile(px_memorypool *mp,px_shape *shape,px_char *path)
{
	PX_IO_Data io=PX_LoadFileToIOData(path);
	if (!io.size)
	{
		return PX_FALSE;
	}
	if (PX_ShapeCreateFromMemory(mp,io.buffer,io.size,shape))
	{
		PX_FreeIOData(&io);
		return PX_TRUE;
	}
	PX_FreeIOData(&io);
	return PX_FALSE;

}


px_bool PX_LoadSoundFromFile(px_memorypool *mp,px_shape *shape,px_char *path)
{
	PX_IO_Data io=PX_LoadFileToIOData(path);
	if (!io.size)
	{
		return PX_FALSE;
	}
	if (PX_ShapeCreateFromMemory(mp,io.buffer,io.size,shape))
	{
		PX_FreeIOData(&io);
		return PX_TRUE;
	}
	PX_FreeIOData(&io);
	return PX_FALSE;
}


px_bool PX_LoadAnimationLibraryFromFile(px_memorypool *mp,px_animationlibrary *lib,px_char *path)
{
	PX_IO_Data io;
	io=PX_LoadFileToIOData(path);
	if (!io.size)
	{
		return PX_FALSE;
	}
	if (PX_AnimationLibraryCreateFromMemory(mp,lib,io.buffer,io.size))
	{
		PX_FreeIOData(&io);
		return PX_TRUE;
	}
	PX_FreeIOData(&io);
	return PX_FALSE;

}

px_bool PX_LoadScriptFromFile(px_memory *code,px_char *path)
{
	PX_IO_Data io=PX_LoadFileToIOData(path);
	if (!io.size)
	{
		return PX_FALSE;
	}
	if(!PX_MemoryCopy(code,io.buffer,0,io.size))
		goto _ERROR;

	PX_FreeIOData(&io);
	return PX_TRUE;
_ERROR:
	PX_FreeIOData(&io);
	return PX_FALSE;

}


px_bool PX_LoadScriptInstanceFromFile(px_memorypool *mp,PX_ScriptVM_Instance *ins,px_char *path)
{
	PX_IO_Data io=PX_LoadFileToIOData(path);
	if (!io.size)
	{
		return PX_FALSE;
	}
	if(!PX_ScriptVM_InstanceInit(ins,mp,io.buffer,io.size))
		goto _ERROR;

	PX_FreeIOData(&io);
	return PX_TRUE;
_ERROR:
	PX_FreeIOData(&io);
	return PX_FALSE;
}

px_bool PX_LoadTextureToResource(PX_Runtime *runtime,px_char Path[],px_char key[])
{
	PX_IO_Data io;
	io=PX_LoadFileToIOData(Path);
	if (!io.size)goto _ERROR;
	if(!PX_ResourceLibraryLoad(&runtime->ResourceLibrary,PX_RESOURCE_TYPE_TEXTURE,io.buffer,io.size,key)) goto _ERROR;
	PX_FreeIOData(&io);
	return PX_TRUE;
_ERROR:
	PX_FreeIOData(&io);
	return PX_FALSE;
}

px_bool PX_LoadShapeToResource(PX_Runtime *runtime,px_char Path[],px_char key[])
{
	PX_IO_Data io;
	io=PX_LoadFileToIOData(Path);
	if (!io.size)goto _ERROR;
	if(!PX_ResourceLibraryLoad(&runtime->ResourceLibrary,PX_RESOURCE_TYPE_SHAPE,io.buffer,io.size,key)) goto _ERROR;
	PX_FreeIOData(&io);
	return PX_TRUE;
_ERROR:
	PX_FreeIOData(&io);
	return PX_FALSE;
}

px_bool PX_LoadAnimationToResource(PX_Runtime *runtime,px_char Path[],px_char key[])
{
	PX_IO_Data io;
	io=PX_LoadFileToIOData(Path);
	if (!io.size)goto _ERROR;
	if(!PX_ResourceLibraryLoad(&runtime->ResourceLibrary,PX_RESOURCE_TYPE_ANIMATIONLIBRARY,io.buffer,io.size,key)) goto _ERROR;
	PX_FreeIOData(&io);
	return PX_TRUE;
_ERROR:
	PX_FreeIOData(&io);
	return PX_FALSE;
}

px_bool PX_LoadScriptToResource(PX_Runtime *runtime,px_char Path[],px_char key[])
{
	PX_IO_Data io;
	io=PX_LoadFileToIOData(Path);
	if (!io.size)goto _ERROR;
	if(!PX_ResourceLibraryLoad(&runtime->ResourceLibrary,PX_RESOURCE_TYPE_SCRIPT,io.buffer,io.size,key)) goto _ERROR;
	PX_FreeIOData(&io);
	return PX_TRUE;
_ERROR:
	PX_FreeIOData(&io);
	return PX_FALSE;
}


px_bool PX_LoadSoundToResource(PX_Runtime *runtime,px_char Path[],px_char key[])
{
	PX_IO_Data io;
	io=PX_LoadFileToIOData(Path);
	if (!io.size)goto _ERROR;
	if(!PX_ResourceLibraryLoad(&runtime->ResourceLibrary,PX_RESOURCE_TYPE_SOUND,io.buffer,io.size,key)) goto _ERROR;
	PX_FreeIOData(&io);
	return PX_TRUE;
_ERROR:
	PX_FreeIOData(&io);
	return PX_FALSE;
}
