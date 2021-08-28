#ifndef PX_OBJECT_PUSHBUTTON_H
#define PX_OBJECT_PUSHBUTTON_H
#include "PX_Object.h"


#define PX_OBJECT_PUSHBUTTON_ROUNDRADIUS 8.0f
typedef enum
{
	PX_OBJECT_BUTTON_STATE_ONCURSOR,
	PX_OBJECT_BUTTON_STATE_ONPUSH,
	PX_OBJECT_BUTTON_STATE_NORMAL,
}PX_Object_PUSHBUTTON_STATE;


typedef enum
{
	PX_OBJECT_PUSHBUTTON_STYLE_RECT,
	PX_OBJECT_PUSHBUTTON_STYLE_ROUNDRECT,
}PX_OBJECT_PUSHBUTTON_STYLE;

typedef struct 
{
	px_bool Border;
	px_color TextColor;
	px_color BorderColor;
	px_color BackgroundColor;
	px_color CursorColor;
	px_color PushColor;
	px_char *Text;
	PX_FontModule *fontModule;
	px_texture *Texture;
	px_shape *shape;
	px_float roundradius;
	PX_OBJECT_PUSHBUTTON_STYLE style;
	PX_Object_PUSHBUTTON_STATE state;
}PX_Object_PushButton;

PX_Object *PX_Object_PushButtonCreate(px_memorypool *mp,PX_Object *Parent,px_int x,px_int y,px_int Width,px_int Height,const px_char *Text,PX_FontModule *fontmodule);
PX_Object_PushButton * PX_Object_GetPushButton( PX_Object *Object );
px_char * PX_Object_PushButtonGetText( PX_Object *PushButton );
px_void PX_Object_PushButtonSetText( PX_Object *pObject,const px_char *Text );
px_void PX_Object_PushButtonSetBackgroundColor( PX_Object *pObject,px_color Color );
px_void PX_Object_PushButtonSetCursorColor( PX_Object *pObject,px_color Color );
px_void PX_Object_PushButtonSetStyle(PX_Object *pObject,PX_OBJECT_PUSHBUTTON_STYLE style);
px_void PX_Object_PushButtonSetPushColor( PX_Object *pObject,px_color Color );
px_void PX_Object_PushButtonSetBorderColor( PX_Object *pObject,px_color Color );
px_void PX_Object_PushButtonSetBorder( PX_Object *Object,px_bool Border );
px_void PX_Object_PushButtonRender(px_surface *psurface, PX_Object *pObject,px_uint elapsed);
px_void PX_Object_PushButtonSetTextColor( PX_Object *pObject,px_color Color );
px_void PX_Object_PushButtonSetTexture(PX_Object *pObject,px_texture *texture);
px_void PX_Object_PushButtonSetShape(PX_Object *pObject,px_shape *pshape);
px_void PX_Object_PushButtonFree( PX_Object *Obj );



#endif

