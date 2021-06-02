#ifndef PX_OBJECT_CURSORBUTTON_H
#define PX_OBJECT_CURSORBUTTON_H
#include "PX_Object.h"

typedef struct 
{
	PX_Object *pushbutton;
	px_float c_distance;
	px_float c_distance_far;
	px_float c_distance_near;
	px_color c_color;
	px_int c_width;
	px_bool enter;
}PX_Object_CursorButton;

//use pushbutton function to operate cursor-button
PX_Object *PX_Object_CursorButtonCreate(px_memorypool *mp,PX_Object *Parent,px_int x,px_int y,px_int Width,px_int Height,const px_char *Text,PX_FontModule *fontmodule,px_color Color);
PX_Object_CursorButton * PX_Object_GetCursorButton( PX_Object *Object );
PX_Object * PX_Object_GetCursorButtonPushButton(PX_Object *Object);

#endif

