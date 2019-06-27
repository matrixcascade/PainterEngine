#ifndef PX_HELPER_H
#define PX_HELPER_H

#define PE_MEMORY_RENDER_SURFACE   1024*1024*4 //4M
#define PE_MEMORY_CACHE_NODE       128
#define PE_MEMORY_RUNTIME_SIZE (1024*1024*128+PE_MEMORY_RENDER_SURFACE+PE_MEMORY_CACHE_NODE)//128M for runtime
#define PE_MEMORY_UI_SIZE 1024*1024*16  //8M
#define PE_MEMORY_RESOURCES_SIZE 1024*1024*96//16M
#define PE_MEMORY_GAME_SIZE 1024*1024 //1M

#ifdef __cplusplus
extern "C" {
#endif

#include "./Startup/Platform_Windows_D2D.h"
#include "./Architecture/PainterEngine_Console.h"

typedef struct
{
	px_byte *buffer;
	px_uint size;
}PX_IO_Data;


px_bool PX_Loop();
px_void PX_SetBackgroundColor(px_color clr);
px_void PX_Clear(px_color clr);
px_void PX_ShowConsole();
px_void PX_HideConsole();
px_bool PX_Initialize(px_char *name,px_int width,px_int height);
px_surface *PX_GetSurface();
PX_Object  *PX_GetUiRoot();
PX_Runtime *PX_GetRuntime();
PX_ResourceLibrary * PX_GetResourceLibrary();
px_memorypool *PX_GetMP();

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

#ifdef __cplusplus
}
#endif

#endif
