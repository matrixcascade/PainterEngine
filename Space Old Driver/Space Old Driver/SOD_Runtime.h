#ifndef SOD_RUNTIME_H
#define SOD_RUNTIME_H

#define SOD_RUNTIME_WINDOW_WIDTH 800
#define SOD_RUNTIME_WINDOW_HEIGHT 600

#include "../../../PainterEngine/platform/windows/Platform_Windows_D2D.h"

#pragma comment(lib,"User32.lib")
#pragma comment(lib,"Shell32.lib")
#pragma comment(lib,"Ole32.lib")
#pragma comment(lib,"winmm.lib")

#include "../../PainterEngine/Architecture/PainterEngine_Console.h"
#include "SOD_TEXT.h"

#include "stdlib.h"
#include "stdio.h"

typedef struct
{
	px_byte *buffer;
	px_uint size;
}PX_IO_Data;

typedef struct
{
	PX_Runtime runtime;
	PX_SoundPlay sound;
	PX_Console Console;
	PX_FontModule fontmodule;
}SOD_Runtime;

px_bool SOD_RuntimeInitialize(SOD_Runtime *runtime);

px_void SOD_PlaySoundFromResource(SOD_Runtime *runtime,px_char *key,px_bool loop);

PX_IO_Data PX_LoadFileToIOData(px_char *path);
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