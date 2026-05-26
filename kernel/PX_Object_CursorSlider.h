#ifndef PX_OBJECT_CURSORSLIDER_H
#define PX_OBJECT_CURSORSLIDER_H
#include "PX_Object.h"

#define PX_OBJECT_CURSORSLIDER_COUNT 16

typedef struct  
{
	px_float reg_x, reg_y;
	px_float reg_velocity;
	px_float radius;
	px_color color;
	px_dword atom_update_duration,elapsed;
	px_point2D sliderpoints[PX_OBJECT_CURSORSLIDER_COUNT];
	px_texture* ptexture;
	px_bool disable_cursor_tracking;
}PX_Object_CursorSlider;
PX_Object* PX_Object_CursorSliderAttachObject(PX_Object* pObject, px_int attachIndex, px_float x, px_float y, px_float radius, px_color Color);
px_void PX_Object_CursorSliderSetXY(PX_Object* pObject, px_float x, px_float y);
px_void PX_Object_CursorSliderReset(PX_Object* pObject);
px_void PX_Object_CursorSliderDisableCursorTracking(PX_Object* pObject, px_bool disable);
PX_Object* PX_Object_CursorSliderCreate(px_memorypool* mp, PX_Object* Parent, px_float x, px_float y, px_float radius, px_color Color);

#endif