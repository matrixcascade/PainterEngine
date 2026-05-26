#ifndef PX_WORLD_OBJECT_H
#define PX_WORLD_OBJECT_H

#include "../core/PX_Core.h"
#include "PX_Resource.h"
#include "PX_Object.h"

#define PX_WORLD_CALC_SIZE 1024*1024

typedef struct _PX_World
{
	px_memorypool* mp;

	px_vector	pNewObjects;
	px_vector	pObjects;//PX_World_Object;
	px_int		world_width,world_height;
	px_int		surface_width,surface_height;
	px_float	offsetx, offsety;
	px_int		auxiliaryXSpacer;
	px_int		auxiliaryYSpacer;
	px_bool		auxiliaryline;
	px_color	auxiliaryline_color;
	px_bool		showImpactRegion;

	px_texture* pbackgroundtexture;
	px_texture* pfrontwardtexture;
	PX_ResourceLibrary *presourceLibrary;
	PX_SoundPlay* psoundplay;
	PX_Quadtree Impact_Test_array[sizeof(px_dword)*8];
	px_uint64 rand_seed;
	PX_MT19937 mt19937;

	px_byte		*calc_buffer;
	px_int		calc_buffer_size;
}PX_World;

px_bool			PX_World_Initialize(px_memorypool* mp, PX_World* pWorld, px_int world_width, px_int world_height, px_int surface_width, px_int surface_height);
px_void			PX_World_SetBackwardTexture(PX_World* pWorld, px_texture* ptexture);
px_void			PX_World_SetFrontwardTexture(PX_World* pWorld, px_texture* ptexture);
px_int			PX_World_GetObjectCount(PX_World *pWorld);
PX_Object*		PX_World_GetObject(PX_World* World, px_int index);
px_void			PX_World_RemoveObjectByIndex(PX_World *pWorld,px_int i_index);
px_void			PX_World_RemoveObject(PX_World* pWorld, PX_Object* pObject);
px_void			PX_World_Clear(PX_World* world);
px_void         PX_World_SoundPlay(PX_World* pWorld, const px_char key[],px_float x,px_float y);
px_void         PX_World_SetSoundPlay(PX_World* pWorld, PX_SoundPlay* pSoundPlay);
px_void         PX_World_SetResourceLibrary(PX_World* pWorld, PX_ResourceLibrary* pResourceLibrary);
px_void			PX_World_SetSize(PX_World* pWorld, px_int world_width, px_int world_height);
px_bool			PX_World_AddObject(PX_World *pWorld,PX_Object *pObject);
px_void			PX_World_Update(PX_World *pWorld,px_uint elapsed );
px_void			PX_World_Render(px_surface *psurface,PX_World *pWorld,px_dword elapsed );
px_void         PX_World_SetCamera(PX_World *pWorld, px_float centerx, px_float centery);

px_void			PX_World_SetAuxiliaryXYSpacer(PX_World *pw,px_int x,px_int y);
px_void			PX_World_ShowAuxiliaryLine(PX_World *pw,px_bool bline);
px_void			PX_World_SetAuxiliaryLineColor(PX_World *pw,px_color color);
px_point2D      PX_World_ObjectXYtoScreenXY(PX_World *pw,px_float x,px_float y);
px_void			PX_World_PostEvent(PX_World* pw, PX_Object_Event e);
px_void         PX_World_Free(PX_World *pw);
#endif
