#ifndef PX_OBJECT_WAITING_H
#define PX_OBJECT_WAITING_H
#include "PX_Object_CursorSlider.h"
typedef struct
{
	px_float lastx, lasty;
	px_float elapsed;
	px_float speed;
	px_float radius;
	px_string text;
	PX_Object* cursor;
	px_color color;
	PX_FontModule* fm;
}PX_Object_Waiting;

PX_Object* PX_Object_WaitingCreate(px_memorypool* mp, PX_Object* Parent, px_float x, px_float y, px_float r, px_float round_radius, px_float speed, px_color Color);
px_void PX_Object_WaitingSetText(PX_Object* pObject, const px_char text[],PX_FontModule *fm);


#endif // !PX_OBJECT_WAITING_H
