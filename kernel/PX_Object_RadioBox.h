#ifndef PX_OBJECT_RADIOBOX_H
#define PX_OBJECT_RADIOBOX_H
#include "PX_Object.h"
typedef enum
{
	PX_OBJECT_RADIOBOX_STATE_ONCURSOR,
	PX_OBJECT_RADIOBOX_STATE_ONPUSH,
	PX_OBJECT_RADIOBOX_STATE_NORMAL,
}PX_Object_RADIOBOX_STATE;
#define PX_OBJECT_RADIOBOX_MAX_CONTENT 64
typedef struct  
{
	PX_ALIGN Align;
	px_bool Border;
	px_color TextColor;
	px_color BorderColor;
	px_color BackgroundColor;
	px_color CursorColor;
	px_color PushColor;
	px_char Text[PX_OBJECT_RADIOBOX_MAX_CONTENT];
	px_bool bCheck;
	PX_FontModule *fm;
	PX_Object_RADIOBOX_STATE state;
}PX_Object_RadioBox;

PX_Object_RadioBox *PX_Object_GetRadioBox(PX_Object *Object);
PX_Object * PX_Object_RadioBoxCreate(px_memorypool *mp, PX_Object *Parent,px_int x,px_int y,px_int Width,px_int Height,const char text[],PX_FontModule *fm);
px_bool PX_Object_RadioBoxGetCheck(PX_Object *Object);
px_void PX_Object_RadioBoxSetBackgroundColor(PX_Object *Object,px_color clr);
px_void PX_Object_RadioBoxSetBorderColor(PX_Object *Object,px_color clr);
px_void PX_Object_RadioBoxSetPushColor(PX_Object *Object,px_color clr);
px_void PX_Object_RadioBoxSetCursorColor(PX_Object *Object,px_color clr);
px_void PX_Object_RadioBoxSetText(PX_Object *Object,const px_char text[]);
const px_char *PX_Object_RadioBoxGetText(PX_Object *Object);
px_void PX_Object_RadioBoxSetTextColor(PX_Object *Object,px_color clr);
px_void PX_Object_RadioBoxSetCheck(PX_Object *Object,px_bool check);



#endif

