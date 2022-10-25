#ifndef PAINTERENGINE_STARTUP_H
#define PAINTERENGINE_STARTUP_H

//////////////////////////////////////////////////////////////////////////
//Configures
#include "platform/modules/px_file.h"

#define PX_APPLICATION_NAME "PainterEngine"
#define PX_APPLICATION_SURFACE_SIZE 680

#define PX_APPLICATION_MEMORYPOOL_UI_SIZE 1024*1024*8
#define PX_APPLICATION_MEMORYPOOL_RESOURCES_SIZE 1024*1024*64
#define PX_APPLICATION_MEMORYPOOL_GAME_SIZE 1024*1024*8
#define PX_APPLICATION_MEMORYPOOL_SPACE_SIZE 1024*1024*8
//////////////////////////////////////////////////////////////////////////

#include "architecture/PainterEngine_Runtime.h"
#include "platform/modules/px_tcp.h"
px_bool PX_ApplicationInitializeDefault(PX_Runtime *runtime, px_int screen_width, px_int screen_height);
px_void PX_ApplicationEventDefault(PX_Runtime *runtime,PX_Object_Event e);

#ifdef PAINTERENGINE_FILE_H
px_bool PX_LoadTextureFromFile(px_memorypool *mp,px_texture *tex,const px_char path[]);
px_bool PX_LoadShapeFromFile(px_memorypool *mp,px_shape *shape,const px_char path[]);
px_bool PX_LoadSoundFromFile(px_memorypool *mp,PX_SoundData *sounddata,const px_char path[]);
px_bool PX_LoadMidiFromFile(PX_Midi* midi, const px_char path[]);
px_bool PX_LoadAnimationLibraryFromFile(px_memorypool *mp,PX_AnimationLibrary *lib,const px_char path[]);
px_bool PX_LoadScriptFromFile(px_memory *code,const px_char path[]);
px_bool PX_LoadScriptInstanceFromFile(px_memorypool *mp,PX_VM *ins,const px_char path[]);
px_bool PX_LoadLiveFromFile(px_memorypool* mp, PX_LiveFramework* pliveframework, const px_char path[]);

px_bool PX_LoadTextureToResource(PX_ResourceLibrary *presourcelib,const px_char Path[],const px_char key[]);
px_bool PX_LoadShapeToResource(PX_ResourceLibrary *presourcelib,const px_char Path[],const px_char key[]);
px_bool PX_LoadAnimationToResource(PX_ResourceLibrary *presourcelib,const px_char Path[],const px_char key[]);
px_bool PX_LoadScriptToResource(PX_ResourceLibrary *presourcelib,const px_char Path[],const px_char key[]);
px_bool PX_LoadSoundToResource(PX_ResourceLibrary *presourcelib,const px_char Path[],const px_char key[]);
px_bool PX_LoadFontModuleFromFile(PX_FontModule *fm,const px_char Path[]);
px_bool PX_LoadJsonFromFile(PX_Json *json,const px_char *path);
#endif


#endif
