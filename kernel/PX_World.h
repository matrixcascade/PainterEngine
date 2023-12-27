#ifndef PX_WORLD_OBJECT_H
#define PX_WORLD_OBJECT_H

#include "../core/PX_Core.h"
#include "PX_Script_VM.h"
#include "PX_Resource.h"
#include "PX_Object.h"

#define PX_WORLD_OBJECT_TYPE_NAME_LEN 32
#define PX_WORLD_CALC_SIZE 1024*1024*2
typedef struct
{
	px_bool DeleteMark;
	PX_Object *pObject;
}PX_WorldObject;


typedef struct _PX_World
{
	px_vector pNewObjects;
	px_vector pObjects;//PX_World_Object;
	px_int     aliveCount;
	px_int	   world_width,world_height;
	px_int	   surface_width,surface_height;
	px_float     offsetx;
	px_float     offsety;
	px_int     auxiliaryXSpacer;
	px_int     auxiliaryYSpacer;
	px_bool    auxiliaryline;
	px_color   auxiliaryline_color;
	px_bool    showImpactRegion;
	px_point   camera_offset;
	px_memorypool *mp;
	px_memorypool mp_WorldCalc;
	px_map idSearchmap;
	PX_FontModule* fontmodule;
	px_texture* pbackgroundtexture;
	px_texture* pfrontwardtexture;
	PX_ResourceLibrary *presourceLibrary;
	PX_SoundPlay* psoundplay;
	PX_Quadtree Impact_Test_array[sizeof(px_dword)*8];
	PX_Compiler compiler;
	px_memory vmbin;
	PX_VM_DebuggerMap debugmap;
	PX_VM  vm;
	px_uint64 rand_seed;
	px_map classes;
}PX_World;

typedef PX_Object* (*PX_WorldCreateObjectFunc)(PX_World* pWorld, px_float x, px_float y, px_float z, px_float width, px_float height, px_float length, px_abi* abi);

#define PX_WORLD_CREATE_OBJECT_FUNCTION(name) PX_Object *name(PX_World *pWorld,px_float x,px_float y,px_float z,px_float width,px_float height,px_float length,px_abi *pabi)
typedef struct
{
	px_char name[PX_WORLD_OBJECT_TYPE_NAME_LEN];
	PX_WorldCreateObjectFunc func;
}PX_WorldClass;

typedef PX_Object *(*px_world_func_CreateObject)(PX_World *pWorld,px_float x,px_float y,px_float z,px_float width,px_float height,px_float length,px_void *user,px_abi *abi);
typedef struct
{
	const px_char *name;
	px_world_func_CreateObject func;
}PX_WorldObjectType;

px_bool			PX_WorldInitialize(px_memorypool* mp, PX_World* pWorld, px_int world_width, px_int world_height, px_int surface_width, px_int surface_height, const px_char script[]);
px_void			PX_WorldSetBackwardTexture(PX_World* pWorld, px_texture* ptexture);
px_void			PX_WorldSetFrontwardTexture(PX_World* pWorld, px_texture* ptexture);
px_int			PX_WorldGetCount(PX_World *pWorld);
px_int			PX_WorldGetAliveCount(PX_World* World);
PX_WorldObject* PX_WorldGetWorldObject(PX_World* World, px_int index);
PX_Object*		PX_WorldGetObject(PX_World* World, px_int index);
px_void         PX_WorldRemoveObject(PX_World *pWorld,PX_Object *pObject);
px_void			PX_WorldRemoveObjectByIndex(PX_World *pWorld,px_int i_index);
px_void			PX_WorldClear(PX_World* world);
px_void         PX_WorldSoundPlay(PX_World* pWorld, const px_char key[],px_float x,px_float y);
px_void         PX_WorldBindSoundPlay(PX_World* pWorld, PX_SoundPlay* pSoundPlay);
px_void         PX_WorldBindResourceLibrary(PX_World* pWorld, PX_ResourceLibrary* pResourceLibrary);
//LIMIT-Only used to ObjectUpdate Function 
_LIMIT px_int	PX_WorldSearchRegion(PX_World *pWorld,px_float centerX,px_float centerY,px_float raduis,PX_Object *pObject[],px_int MaxSearchCount,px_dword impact_test_type);
_LIMIT px_int	PX_WorldSearch(PX_World* world, PX_Object* pObject[], px_int MaxSearchCount, px_dword impact_object_type);
_LIMIT PX_Object* PX_WorldSearchObject(PX_World *pWorld,const px_char id[]);
px_void			PX_WorldSetSize(PX_World* pWorld, px_int world_width, px_int world_height);
px_bool			PX_WorldAddObject(PX_World *pWorld,PX_Object *pObject);
px_void			PX_WorldUpdate(PX_World *pWorld,px_uint elapsed );
px_void			PX_WorldUpdateOffset(PX_World *pw);
px_void			PX_WorldRender(px_surface *psurface,PX_World *pWorld,px_dword elapsed );
px_void         PX_WorldSetCamera(PX_World *pWorld,px_point camera_center_point);
px_void			PX_WorldSetImpact(PX_Object *pObj,px_dword type,px_dword impact);
px_void			PX_WorldSetAuxiliaryXYSpacer(PX_World *pw,px_int x,px_int y);
px_void			PX_WorldEnableAuxiliaryLine(PX_World *pw,px_bool bline);
px_void			PX_WorldSetAuxiliaryLineColor(PX_World *pw,px_color color);
px_point        PX_WorldObjectXYtoScreenXY(PX_World *pw,px_float x,px_float y);
px_void			PX_WorldPostEvent(PX_World* pw, PX_Object_Event e);
px_bool			PX_WorldRegisterClass(PX_World* pw, PX_WorldClass cls);
PX_Object*      PX_WorldCreateClassObject(PX_World* pWorld,px_char name[], px_float x, px_float y, px_float z, px_float width, px_float height, px_float length, px_abi* abi);
px_void         PX_WorldFree(PX_World *pw);
#endif
