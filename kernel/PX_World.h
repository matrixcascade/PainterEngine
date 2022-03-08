#ifndef PX_WORLD_OBJECT_H
#define PX_WORLD_OBJECT_H

#include "../core/PX_Core.h"
#include "PX_Script_VM.h"
#include "PX_Resource.h"
#include "PX_Object.h"

#define PX_WORLD_OBJECT_TYPE_NAME_LEN 32

typedef struct
{
	px_bool DeleteMark;
	PX_Object *pObject;
}PX_WorldObject;



typedef struct 
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

	PX_Quadtree Impact_Test_array[sizeof(px_dword)*8];
}PX_World;

typedef PX_Object *(*px_world_func_CreateObject)(PX_World *world,px_float x,px_float y,px_float z,px_float width,px_float height,px_float length,px_void *user);
typedef struct
{
	const px_char *name;
	px_world_func_CreateObject func;
}PX_WorldObjectType;

px_bool			PX_WorldInitialize(px_memorypool *mp,PX_World *World,px_int world_width,px_int world_height,px_int surface_width,px_int surface_height,px_dword calcsize);
px_int			PX_WorldGetCount(PX_World *World);
px_void         PX_WorldRemoveObject(PX_World *world,PX_Object *pObject);
px_void			PX_WorldRemoveObjectByIndex(PX_World *world,px_int i_index);

//LIMIT-Only used to ObjectUpdate Function 
_LIMIT px_int	PX_WorldSearchRegion(PX_World *world,px_float centerX,px_float centerY,px_float raduis,PX_Object *Object[],px_int MaxSearchCount,px_dword impact_test_type);
px_bool			PX_WorldAddObject(PX_World *World,PX_Object *pObject);
px_void			PX_WorldUpdate(PX_World *World,px_uint elapsed );
px_void			PX_WorldUpdateOffset(PX_World *pw);
px_void			PX_WorldRender(px_surface *psurface,PX_World *World,px_dword elapsed );
px_void         PX_WorldSetCamera(PX_World *World,px_point camera_center_point);
px_void			PX_WorldSetImpact(PX_Object *pObj,px_dword type,px_dword impact);
px_void			PX_WorldSetAuxiliaryXYSpacer(PX_World *pw,px_int x,px_int y);
px_void			PX_WorldEnableAuxiliaryLine(PX_World *pw,px_bool bline);
px_void			PX_WorldSetAuxiliaryLineColor(PX_World *pw,px_color color);
px_point        PX_WolrdObjectXYtoScreenXY(PX_World *pw,px_float x,px_float y);
px_void			PX_WorldPostEvent(PX_World* pw, PX_Object_Event e);
px_void         PX_WorldFree(PX_World *pw);
#endif
