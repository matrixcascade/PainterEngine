#ifndef PX_OBJECT_CHECKBOX_H
#define PX_OBJECT_CHECKBOX_H
#include "PX_Object.h"


#define PX_OBJECT_CHECKBOX_MAX_CONTENT 64


typedef enum
{
	PX_OBJECT_CHECKBOX_STATE_ONCURSOR,
	PX_OBJECT_CHECKBOX_STATE_ONPUSH,
	PX_OBJECT_CHECKBOX_STATE_NORMAL,
}PX_Object_CHECKBOX_STATE;

typedef struct  
{
	px_dword Align;
	px_bool Border;
	px_color TextColor;
	px_color BorderColor;
	px_color BackgroundColor;
	px_color CursorColor;
	px_color PushColor;
	px_char Text[PX_OBJECT_CHECKBOX_MAX_CONTENT];
	px_bool bCheck;
	PX_FontModule *fm;
	PX_Object_CHECKBOX_STATE state;
}PX_Object_CheckBox;

PX_Object_CheckBox *PX_Object_GetCheckBox(PX_Object *Object);
PX_Object * PX_Object_CheckBoxCreate(px_memorypool *mp, PX_Object *Parent,px_int x,px_int y,px_int Width,px_int Height,const char text[],PX_FontModule *fm);
px_bool PX_Object_CheckBoxGetCheck(PX_Object *Object);
px_void PX_Object_CheckBoxSetBackgroundColor(PX_Object *Object,px_color clr);
px_void PX_Object_CheckBoxSetBorderColor(PX_Object *Object,px_color clr);
px_void PX_Object_CheckBoxSetPushColor(PX_Object *Object,px_color clr);
px_void PX_Object_CheckBoxSetCursorColor(PX_Object *Object,px_color clr);
px_void PX_Object_CheckBoxSetText(PX_Object *Object,const px_char text[]);
px_char *PX_Object_CheckBoxGetText(PX_Object *Object);
px_void PX_Object_CheckBoxSetTextColor(PX_Object *Object,px_color clr);
px_void PX_Object_CheckBoxSetCheck(PX_Object *Object,px_bool check);





#endif

