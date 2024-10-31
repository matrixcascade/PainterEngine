#ifndef PX_OBJECT_DRAG_H
#define PX_OBJECT_DRAG_H
#include "PX_Object.h"

typedef struct
{
	px_float last_cursorx, last_cursory;
	px_bool bselect;
}PX_Object_Drag;

PX_Object* PX_Object_DragAttachObject(PX_Object* pObject, px_int attachIndex);

#endif