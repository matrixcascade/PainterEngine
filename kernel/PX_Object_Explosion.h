#ifndef PX_OBJECT_EXPLOSION_H
#define PX_OBJECT_EXPLOSION_H
#include "PX_Object.h"

typedef struct
{
	px_memorypool *mp;
	px_float size;
	px_float power;
	px_dword alive;
	px_texture* ptexture;
	px_dword elapsed;
	PX_ParticalLauncher launcher;
}PX_Object_Explosion;

typedef struct
{
	px_memorypool* mp;
	px_dword elapsed;
	px_color color;
	px_texture rect;
	PX_ParticalLauncher launcher;
}PX_Object_Explosion02;

typedef struct
{
	px_point array[32];
	px_dword delay;
	px_float size;
	px_float vx, vy;
	px_int   dirx, diry;
	px_dword switch_elapsed;
	px_dword switch_duration;
	px_dword elapsed;
	px_dword alive;
	px_color clr;
}PX_Object_Explosion03;

typedef struct
{
	px_color color;
	px_dword alive;
	px_dword elapsed;
	px_float range;
}PX_Object_Explosion04;



PX_Object* PX_Object_Explosion01Create(px_memorypool* mp, PX_Object* parent, px_float x, px_float y, px_int count, px_float size, px_float power,  px_int alive, px_texture* ptexture);
PX_Object* PX_Object_Explosion02Create(px_memorypool* mp, PX_Object* parent, px_float x, px_float y, px_color color);
PX_Object* PX_Object_Explosion03Create(px_memorypool* mp, PX_Object* parent, px_float x, px_float y, px_int dirx, px_int diry, px_float size, px_dword switch_duration,px_dword alive, px_color color);
PX_Object* PX_Object_Explosion04Create(px_memorypool* mp, PX_Object* parent, px_float x, px_float y, px_color color, px_float range, px_dword alive);
#endif