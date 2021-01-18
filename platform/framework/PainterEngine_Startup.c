#include "PainterEngine_Startup.h"
#include "stdlib.h"

//////////////////////////////////////////////////////////////////////////
//Functions


#ifdef PAINTERENGIN_FILE_SUPPORT
static px_byte PX_ApplicationRuntime[PX_APPLICATION_MEMORYPOOL_UI_SIZE+PX_APPLICATION_MEMORYPOOL_RESOURCES_SIZE+PX_APPLICATION_MEMORYPOOL_GAME_SIZE+PX_APPLICATION_MEMORYPOOL_SPACE_SIZE];
px_bool PX_ApplicationInitializeDefault(PX_Runtime *runtime, px_int screen_width, px_int screen_height)
{
	px_int surface_width=0,surface_height=0;
	px_int window_width=0,window_height=0;
	px_double wdh;
	if (screen_width<screen_height)
	{
		px_int tmp=screen_height;
		screen_height=screen_width;
		screen_width=tmp;
	}
	wdh=screen_width*1.0/screen_height;
	surface_height=(px_int)(PX_sqrtd(PX_APPLICATION_SURFACE_SIZE*PX_APPLICATION_SURFACE_SIZE/wdh));
	surface_width=(px_int)(surface_height*wdh);

#ifdef PX_APPLICATION_WINDOWS
	window_width=screen_width/2;
	window_height=screen_height/2;
#endif

#ifdef PX_APPLICATION_ANDROID
	window_width=screen_width;
	window_height=screen_height;
#endif


	if(!PX_RuntimeInitialize(runtime,surface_width,surface_height,window_width,window_height,PX_ApplicationRuntime,sizeof(PX_ApplicationRuntime),PX_APPLICATION_MEMORYPOOL_UI_SIZE,PX_APPLICATION_MEMORYPOOL_RESOURCES_SIZE,PX_APPLICATION_MEMORYPOOL_GAME_SIZE))
		return PX_FALSE;
	return PX_TRUE;
}
px_void PX_ApplicationEventDefault(PX_Runtime *runtime,PX_Object_Event e)
{
	if (e.Event==PX_OBJECT_EVENT_WINDOWRESIZE)
	{
		px_int surface_width=0,surface_height=0;
		px_double wdh;

		wdh=PX_Object_Event_GetWidth(e)*1.0/PX_Object_Event_GetHeight(e);
		surface_height=(px_int)(PX_sqrtd(PX_APPLICATION_SURFACE_SIZE*PX_APPLICATION_SURFACE_SIZE/wdh));
		surface_width=(px_int)(surface_height*wdh);

		PX_RuntimeResize(runtime,surface_width,surface_height,(px_int)PX_Object_Event_GetWidth(e),(px_int)PX_Object_Event_GetHeight(e));
		return;
	}
}
px_bool PX_LoadTextureFromFile(px_memorypool *mp,px_texture *tex,const px_char path[])
{
	PX_IO_Data io;

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

px_bool PX_LoadShapeFromFile(px_memorypool *mp,px_shape *shape,const px_char path[])
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


px_bool PX_LoadSoundFromFile(px_memorypool *mp,px_shape *shape,const px_char path[])
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


px_bool PX_LoadAnimationLibraryFromFile(px_memorypool *mp,PX_Animationlibrary *lib,const px_char path[])
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

px_bool PX_LoadScriptFromFile(px_memory *code,const px_char path[])
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


px_bool PX_LoadScriptInstanceFromFile(px_memorypool *mp,PX_ScriptVM_Instance *ins,const px_char path[])
{
	PX_IO_Data io=PX_LoadFileToIOData(path);
	if (!io.size)
	{
		return PX_FALSE;
	}
	if(!PX_ScriptVM_InstanceInitialize(ins,mp,io.buffer,io.size))
		goto _ERROR;

	PX_FreeIOData(&io);
	return PX_TRUE;
_ERROR:
	PX_FreeIOData(&io);
	return PX_FALSE;
}

px_bool PX_LoadTextureToResource(PX_ResourceLibrary *ResourceLibrary,const px_char Path[],const px_char key[])
{
	PX_IO_Data io;
	io=PX_LoadFileToIOData(Path);
	if (!io.size)goto _ERROR;
	if(!PX_ResourceLibraryLoad(ResourceLibrary,PX_RESOURCE_TYPE_TEXTURE,io.buffer,io.size,key)) goto _ERROR;
	PX_FreeIOData(&io);
	return PX_TRUE;
_ERROR:
	PX_FreeIOData(&io);
	return PX_FALSE;
}

px_bool PX_LoadShapeToResource(PX_ResourceLibrary *ResourceLibrary,const px_char Path[],const px_char key[])
{
	PX_IO_Data io;
	io=PX_LoadFileToIOData(Path);
	if (!io.size)goto _ERROR;
	if(!PX_ResourceLibraryLoad(ResourceLibrary,PX_RESOURCE_TYPE_SHAPE,io.buffer,io.size,key)) goto _ERROR;
	PX_FreeIOData(&io);
	return PX_TRUE;
_ERROR:
	PX_FreeIOData(&io);
	return PX_FALSE;
}

px_bool PX_LoadAnimationToResource(PX_ResourceLibrary *ResourceLibrary,const px_char Path[],const px_char key[])
{
	PX_IO_Data io;
	io=PX_LoadFileToIOData(Path);
	if (!io.size)goto _ERROR;
	if(!PX_ResourceLibraryLoad(ResourceLibrary,PX_RESOURCE_TYPE_ANIMATIONLIBRARY,io.buffer,io.size,key)) goto _ERROR;
	PX_FreeIOData(&io);
	return PX_TRUE;
_ERROR:
	PX_FreeIOData(&io);
	return PX_FALSE;
}

px_bool PX_LoadScriptToResource(PX_ResourceLibrary *ResourceLibrary,const px_char Path[],const px_char key[])
{
	PX_IO_Data io;
	io=PX_LoadFileToIOData(Path);
	if (!io.size)goto _ERROR;
	if(!PX_ResourceLibraryLoad(ResourceLibrary,PX_RESOURCE_TYPE_SCRIPT,io.buffer,io.size,key)) goto _ERROR;
	PX_FreeIOData(&io);
	return PX_TRUE;
_ERROR:
	PX_FreeIOData(&io);
	return PX_FALSE;
}


px_bool PX_LoadSoundToResource(PX_ResourceLibrary *ResourceLibrary,const px_char Path[],const px_char key[])
{
	PX_IO_Data io;
	io=PX_LoadFileToIOData(Path);
	if (!io.size)goto _ERROR;
	if(!PX_ResourceLibraryLoad(ResourceLibrary,PX_RESOURCE_TYPE_SOUND,io.buffer,io.size,key)) goto _ERROR;
	PX_FreeIOData(&io);
	return PX_TRUE;
_ERROR:
	PX_FreeIOData(&io);
	return PX_FALSE;
}

px_bool PX_LoadFontModuleFromFile(PX_FontModule *fm,const px_char Path[])
{
	PX_IO_Data io;
	io=PX_LoadFileToIOData(Path);
	if (!io.size)goto _ERROR;
	if(!PX_FontModuleLoad(fm,io.buffer,io.size)) goto _ERROR;
	PX_FreeIOData(&io);
	return PX_TRUE;
_ERROR:
	PX_FreeIOData(&io);
	return PX_FALSE;
}

px_bool PX_LoadJsonFromFile(PX_Json *json,const px_char *path)
{
	PX_IO_Data io=PX_LoadFileToIOData((px_char *)path);
	if (!io.size)
	{
		return PX_FALSE;
	}

	if(!PX_JsonParse(json,(px_char *)io.buffer))goto _ERROR;


	PX_FreeIOData(&io);
	return PX_TRUE;
_ERROR:
	PX_FreeIOData(&io);
	return PX_FALSE;
}

#endif