#ifndef PX_OBJECT_BUTTON_H
#define PX_OBJECT_BUTTON_H
#include "PX_Object.h"


#define PX_OBJECT_BUTTON_ROUNDRADIUS 8.0f
typedef enum
{
	PX_OBJECT_BUTTON_STATE_ONCURSOR,
	PX_OBJECT_BUTTON_STATE_ONPUSH,
	PX_OBJECT_BUTTON_STATE_NORMAL,
}PX_Object_BUTTON_STATE;


typedef struct 
{
	px_color CursorColor;
	px_color PushColor;
	PX_Object_BUTTON_STATE state;
}PX_Object_Button;

PX_Object* PX_Object_ButtonAttachObject(PX_Object* pObject, px_int attachIndex, px_color cursor_color, px_color push_color);
px_void PX_Object_ButtonSetPushColor( PX_Object *pObject,px_color Color );
px_void PX_Object_ButtonSetCursorColor( PX_Object *pObject,px_color Color );
PX_Object_Button* PX_Object_GetButton(PX_Object* pObject);
#endif

