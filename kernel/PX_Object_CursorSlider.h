#ifndef PX_OBJECT_CURSORSLIDER_H
#define PX_OBJECT_CURSORSLIDER_H
#include "PX_Object.h"

#define PX_OBJECT_CURSORSLIDER_COUNT 16

typedef struct  
{
	px_float reg_x, reg_y;
	px_float reg_velocity;
	px_color color;
	px_dword atom_update_duration,elapsed;
	px_point2D sliderpoints[PX_OBJECT_CURSORSLIDER_COUNT];
	px_texture* ptexture;
}PX_Object_CursorSlider;

px_void PX_Object_CursorSliderSetXY(PX_Object* pObject, px_float x, px_float y);
PX_Object* PX_Object_CursorSliderCreate(px_memorypool* mp, PX_Object* Parent, px_float x, px_float y, px_float radius, px_color Color);

#endif