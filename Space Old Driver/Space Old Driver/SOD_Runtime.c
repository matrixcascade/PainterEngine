#include "SOD_Runtime.h"

px_byte SOD_RUNTIME_MEMORY[1024*1024*72];

#define SOD_RUNTIME_UI_SIZE 1024*1024*2
#define SOD_RUNTIME_RESOURCES_SIZE 1024*1024*48
#define SOD_RUNTIME_GAME_SIZE 1024*1024*16

px_bool SOD_RuntimeInitialize(SOD_Runtime *sodruntime)
{
	PX_IO_Data data;

	if (!PX_RuntimeInitialize(&sodruntime->runtime,SOD_RUNTIME_WINDOW_WIDTH,SOD_RUNTIME_WINDOW_HEIGHT,SOD_RUNTIME_MEMORY,sizeof(SOD_RUNTIME_MEMORY),SOD_RUNTIME_UI_SIZE,SOD_RUNTIME_RESOURCES_SIZE,SOD_RUNTIME_GAME_SIZE))
	{
		return PX_FALSE;
	}

	if (!PX_ConsoleInitialize(&sodruntime->runtime,&sodruntime->Console))
	{
		return PX_FALSE;
	}

	if(!PX_SoundPlayInit(&sodruntime->runtime.mp,&sodruntime->sound))
	{
		return PX_FALSE;
	}
	
	data=PX_LoadFileToIOData(SOD_PATH_FONTMODULE);
	
	if (data.size==0)
	{
		return PX_FALSE;
	}
	if (!PX_FontModuleInitialize(&sodruntime->runtime.mp_resources,&sodruntime->fontmodule))
	{
		return PX_FALSE;
	}

	if (!PX_FontModuleLoad(&sodruntime->fontmodule,data.buffer,data.size))
	{
		return PX_FALSE;
	}

	if(!PX_LoadSoundToResource(&sodruntime->runtime,SOD_PATH_BTN_ALERT,SOD_KEY_BTN_ALERT))
	{
		return PX_FALSE;
	}

	if(!PX_LoadSoundToResource(&sodruntime->runtime,SOD_PATH_BTN_CLICK,SOD_KEY_BTN_CLICK))
	{
		return PX_FALSE;
	}

	return PX_TRUE;

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



px_void SOD_PlaySoundFromResource(SOD_Runtime *runtime,px_char *key,px_bool loop)
{
	PX_SoundData *pdata;
	pdata=PX_ResourceLibraryGetSound(&runtime->runtime.ResourceLibrary,key);
	if (pdata)
	{
		PX_SoundPlayAdd(&runtime->sound,PX_SoundCreate(pdata,loop));
	}
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
