#ifndef PX_RESOURCE_H
#define PX_RESOURCE_H

#include "../core/PX_Core.h"
#include "PX_Script_VM.h"
#include "PX_Animation.h"
#include "PX_Partical.h"
#include "PX_Script_Interpreter.h"
#include "PX_Script_ASMCompiler.h"

typedef enum 
{
	PX_RESOURCE_TYPE_NULL,
	PX_RESOURCE_TYPE_ANIMATIONLIBRARY,
	PX_RESOURCE_TYPE_SCRIPT,
	PX_RESOURCE_TYPE_TEXTURE,
	PX_RESOURCE_TYPE_SHAPE,
	PX_RESOURCE_TYPE_SOUND,
	PX_RESOURCE_TYPE_DATA,
}PX_RESOURCE_TYPE;


typedef struct
{
	PX_RESOURCE_TYPE  Type;
	union
	{
		px_texture texture;
		px_shape  shape;
		PX_AnimationLibrary animationlibrary;
		PX_ScriptVM_Instance Script;
		PX_SoundData sound;
		px_memory data;
	};
}PX_Resource;

typedef struct
{
	px_memorypool *mp;
	px_list resources;
	px_map map;
}PX_ResourceLibrary;



px_bool PX_ResourceLibraryInitialize(px_memorypool *mp,PX_ResourceLibrary *lib);
px_bool  PX_ResourceLibraryLoad(PX_ResourceLibrary *lib,PX_RESOURCE_TYPE type,px_byte *data,px_uint datasize,const px_char key[]);
px_bool PX_ResourceLibraryAddTexture(PX_ResourceLibrary *lib,const px_char key[],px_texture *pTexture);
PX_Resource * PX_ResourceLibraryGet(PX_ResourceLibrary *lib,const px_char key[]);
px_texture *PX_ResourceLibraryGetTexture(PX_ResourceLibrary *lib,const px_char key[]);
px_shape *PX_ResourceLibraryGetShape(PX_ResourceLibrary *lib,const px_char key[]);
PX_AnimationLibrary *PX_ResourceLibraryGetAnimationLibrary(PX_ResourceLibrary *lib,const px_char key[]);
PX_ScriptVM_Instance *PX_ResourceLibraryGetScript(PX_ResourceLibrary *lib,const px_char key[]);
PX_SoundData *PX_ResourceLibraryGetSound(PX_ResourceLibrary *lib,const px_char key[]);
px_memory *PX_ResourceLibraryGetData(PX_ResourceLibrary *lib,const px_char key[]);

px_void PX_ResourceLibraryDelete(PX_ResourceLibrary *lib,const px_char key[]);
px_void PX_ResourceLibraryFree(PX_ResourceLibrary *lib);


#endif


