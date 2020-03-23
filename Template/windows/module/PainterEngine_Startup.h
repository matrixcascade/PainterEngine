#ifndef PAINTERENGINE_STARTUP_H
#define PAINTERENGINE_STARTUP_H

#include "../../../PainterEngine/platform/windows/Platform_Windows_D2D.h"
#include "../../../PainterEngine/Architecture/PainterEngine_Console.h"


typedef struct
{
	PX_Runtime runtime;
	PX_Console console;
	PX_SoundPlay soundplay;
}PX_Instance;


//////////////////////////////////////////////////////////////////////////

typedef struct
{
	px_byte *buffer;
	px_uint size;
}PX_IO_Data;

px_bool PX_InstanceInitialize(PX_Instance *Instance,px_char *name,px_int width,px_int height,px_uint ui_size,px_uint resource_size,px_uint game_size);
PX_IO_Data PX_LoadFileToIOData(px_char *path);
px_void PX_FreeIOData(PX_IO_Data *io);
px_bool PX_LoadTextureFromFile(px_memorypool *mp,px_texture *tex,px_char *path);
px_bool PX_LoadShapeFromFile(px_memorypool *mp,px_shape *shape,px_char *path);
px_bool PX_LoadSoundFromFile(px_memorypool *mp,px_shape *shape,px_char *path);
px_bool PX_LoadAnimationLibraryFromFile(px_memorypool *mp,px_animationlibrary *lib,px_char *path);
px_bool PX_LoadScriptFromFile(px_memory *code,px_char *path);
px_bool PX_LoadScriptInstanceFromFile(px_memorypool *mp,PX_ScriptVM_Instance *ins,px_char *path);
px_bool PX_LoadTextureToResource(PX_Runtime *runtime,px_char Path[],px_char key[]);
px_bool PX_LoadShapeToResource(PX_Runtime *runtime,px_char Path[],px_char key[]);
px_bool PX_LoadAnimationToResource(PX_Runtime *runtime,px_char Path[],px_char key[]);
px_bool PX_LoadScriptToResource(PX_Runtime *runtime,px_char Path[],px_char key[]);
px_bool PX_LoadSoundToResource(PX_Runtime *runtime,px_char Path[],px_char key[]);


#endif
