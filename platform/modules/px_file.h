
#ifndef PAINTERENGINE_FILE_H
#define PAINTERENGINE_FILE_H

#define PAINTERENGIN_FILE_SUPPORT
//////////////////////////////////////////////////////////////////////////
#include "stdio.h"
#include "stdlib.h"
#include "string.h"

typedef struct
{
	unsigned char *buffer;
	unsigned int size;
}PX_IO_Data;

typedef enum
{
	PX_FILEENUM_TYPE_ANY,
	PX_FILEENUM_TYPE_FOLDER,
	PX_FILEENUM_TYPE_FILE,
	PX_FILEENUM_TYPE_DEVICE,
}PX_FILEENUM_TYPE;


PX_IO_Data PX_LoadFileToIOData(const char path[]);
int PX_FileExist(const char respath[]);
int PX_CopyFile(const char respath[],const char destpath[]);
int PX_SaveDataToFile(void *buffer,int size,const char path[]);
void PX_FreeIOData(PX_IO_Data *io);

int PX_FileGetDirectoryFileCount(const char path[],PX_FILEENUM_TYPE type,const char *filter);
int PX_FileGetDirectoryFileName(const char path[],int count,char FileName[][260],PX_FILEENUM_TYPE type,const char *filter);

//////////////////////////////////////////////////////////////////////////
#define PX_OBJECT_REQUEST_FUNCTION(name) px_void name(void* buffer, int size, void* ptr)
#include "../../kernel/PX_Kernel.h"
px_bool PX_LoadTextureFromFile(px_memorypool *mp,px_texture *tex,const px_char path[]);
px_bool PX_LoadStringFromFile(px_memorypool *mp,px_string *str,const px_char path[]);
px_bool PX_LoadShapeFromFile(px_memorypool *mp,px_shape *shape,const px_char path[]);
px_bool PX_LoadSoundFromFile(px_memorypool *mp,PX_SoundData *sounddata,const px_char path[]);
px_bool PX_LoadMidiFromFile(PX_Midi* midi, const px_char path[]);
px_bool PX_LoadAnimationLibraryFromFile(px_memorypool *mp,PX_AnimationLibrary *lib,const px_char path[]);
px_bool PX_LoadScriptFromFile(px_memory *code,const px_char path[]);
px_bool PX_LoadScriptInstanceFromFile(px_memorypool *mp,PX_VM *ins,const px_char path[]);
px_bool PX_LoadLiveFromFile(px_memorypool* mp, PX_LiveFramework* pliveframework, const px_char path[]);
px_bool PX_LoadUIFormFile(px_memorypool* mp, PX_Object* designerObject, PX_Object* proot, PX_FontModule* fm, const px_char path[]);
px_bool PX_LoadFontModuleFromTTF(px_memorypool* mp, PX_FontModule* fm, const px_char Path[], PX_FONTMODULE_CODEPAGE codepage, px_int fontsize);
px_bool PX_LoadJsonFromJsonFile(px_memorypool* mp, PX_Json* json, const px_char* path);
px_bool PX_LoadTextureToResource(PX_ResourceLibrary *presourcelib,const px_char Path[],const px_char key[]);
px_bool PX_LoadShapeToResource(PX_ResourceLibrary *presourcelib,const px_char Path[],const px_char key[]);
px_bool PX_LoadAnimationToResource(PX_ResourceLibrary *presourcelib,const px_char Path[],const px_char key[]);
px_bool PX_LoadStringToResource(PX_ResourceLibrary* ResourceLibrary, const px_char Path[], const px_char key[]);
px_bool PX_LoadScriptToResource(PX_ResourceLibrary *presourcelib,const px_char Path[],const px_char key[]);
px_bool PX_LoadSoundToResource(PX_ResourceLibrary *presourcelib,const px_char Path[],const px_char key[]);
px_bool PX_LoadFontModuleFromFile(PX_FontModule *fm,const px_char Path[]);
px_bool PX_LoadJsonFromFile(PX_Json *json,const px_char *path);

px_bool PX_LoadGifFromFile(px_memorypool* mp, px_gif* gif, const px_char path[]);
px_bool PX_LoadLive2DFromFile(px_memorypool* mp, PX_LiveFramework* liveframework, const px_char path[]);

#endif

