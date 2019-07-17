#define _CRT_SECURE_NO_WARNINGS
#include "PainterEngineHelper.h"
#include "stdio.h"
#pragma comment(lib,"User32.lib")
#pragma comment(lib,"Shell32.lib")
#pragma comment(lib,"Ole32.lib")
#pragma comment(lib,"winmm.lib" )

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

px_bool PX_Loop()
{   
	px_int i;
	DWORD elpased;
	POINT mousePosition;
	PX_Object_Event e;
	char *keyBoardString,*dragFileString;

	elpased=timeGetTime()-main_time;
	main_time=timeGetTime();

	if(main_renderElpased>1000/MAIN_RENDER_FPS)
	{
		
		PX_SystemReadDeviceState();
		if(PX_SystemisAvtivated())
		{
			
			e.param_ptr[0]=PX_NULL;
			mousePosition=PX_MousePosition();
			if (main_MouseLastPosition.x!=mousePosition.x||main_MouseLastPosition.y!=mousePosition.y)
			{
				if (mousePosition.x>0&&mousePosition.x<main_runtime.width&&mousePosition.y>0&&mousePosition.y<main_runtime.height)
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
					if(main_console.show)
						PX_ConsolePostEvent(&main_console,e);
					else
						PX_ObjectPostEvent(main_root,e);
				}
			}


			if (PX_MouseLButtonDown())
			{
				if (!main_MouseLDown&&mousePosition.x>0&&mousePosition.x<main_runtime.width&&mousePosition.y>0&&mousePosition.y<main_runtime.height)
				{
					e.Event=PX_OBJECT_EVENT_CURSORDOWN;
					e.Param_uint[0]=mousePosition.x;
					e.Param_uint[1]=mousePosition.y;
					if(main_console.show)
						PX_ConsolePostEvent(&main_console,e);
					else
						PX_ObjectPostEvent(main_root,e);
				}
				main_MouseLDown=PX_TRUE;
			}
			else
			{
				if (main_MouseLDown&&mousePosition.x>0&&mousePosition.x<main_runtime.width&&mousePosition.y>0&&mousePosition.y<main_runtime.height)
				{
					e.Event=PX_OBJECT_EVENT_CURSORUP;
					e.Param_uint[0]=mousePosition.x;
					e.Param_uint[1]=mousePosition.y;
					if(main_console.show)
						PX_ConsolePostEvent(&main_console,e);
					else
						PX_ObjectPostEvent(main_root,e);
				}
				main_MouseLDown=PX_FALSE;
			}


			if (keyBoardString=PX_KeyboardString())
			{
				e.Event=PX_OBJECT_EVENT_STRING;
				e.param_ptr[0]=keyBoardString;
				if(main_console.show)
					PX_ConsolePostEvent(&main_console,e);
				else
					PX_ObjectPostEvent(main_root,e);
			}

			for (i=0;i<0xff;i++)
			{
				if (PX_KeyDown(i))
				{
					e.Event=PX_OBJECT_EVENT_KEYDOWN;
					e.Param_uint[0]=i;
					if(main_console.show)
						PX_ConsolePostEvent(&main_console,e);
					else
						PX_ObjectPostEvent(main_root,e);
				}
			}
		}

		if ((dragFileString=PX_DragfileString())[0])
		{
			e.Event=PX_OBJECT_EVENT_DRAGFILE;
			e.param_ptr[0]=dragFileString;
			if(main_console.show)
				PX_ConsolePostEvent(&main_console,e);
			else
				PX_ObjectPostEvent(main_root,e);
			dragFileString[0]='\0';
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

	PX_srand(timeGetTime());




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
	PX_IO_Data io=PX_LoadFileToIOData(path);
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
