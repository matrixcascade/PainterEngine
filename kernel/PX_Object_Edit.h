#ifndef PX_OBJECT_EDIT_H
#define PX_OBJECT_EDIT_H
#include "PX_Object.h"

typedef enum
{
	PX_OBJECT_EDIT_STATE_ONCURSOR,
	PX_OBJECT_EDIT_STATE_NORMAL,
}PX_OBJECT_EDIT_STATE;

typedef enum
{
	PX_OBJECT_EDIT_STYLE_RECT,
	PX_OBJECT_EDIT_STYLE_ROUNDRECT,
}PX_OBJECT_EDIT_STYLE;

typedef enum
{
	PX_OBJECT_EDIT_INPUT_MODE_NORMAL,
	PX_OBJECT_EDIT_INPUT_MODE_LOWERCASE,
	PX_OBJECT_EDIT_INPUT_MODE_UPPERCASE,
}PX_OBJECT_EDIT_INPUT_MODE;


typedef struct 
{
	px_string text;
	px_bool onFocus;
	px_bool Border;
	px_color TextColor;
	px_color BorderColor;
	px_color CursorColor;
	px_color BackgroundColor;
	px_uint elapsed;
	px_bool Password;
	px_bool AutoNewline;
	px_int AutoNewLineSpacing;
	px_int XOffset,YOffset;
	px_int VerticalOffset,HorizontalOffset;
	px_int xFontSpacing,yFontSpacing;
	px_int cursor_index;
	px_int max_length;
	px_bool multiLines;
	px_surface EditSurface;
	PX_FontModule *fontModule;
	px_char Limit[128];
	PX_OBJECT_EDIT_INPUT_MODE inputmode;
	PX_OBJECT_EDIT_STATE state;
	PX_OBJECT_EDIT_STYLE style;
}PX_Object_Edit;

PX_Object* PX_Object_EditCreate(px_memorypool *mp, PX_Object *Parent,px_int x,px_int y,px_int Width,px_int Height,PX_FontModule *fontModule );
PX_Object_Edit * PX_Object_GetEdit( PX_Object *Object );
px_char * PX_Object_EditGetText( PX_Object *pObject );
px_void PX_Object_EditSetMaxTextLength(PX_Object *pObject,px_int max_length);
px_void PX_Object_EditSetText( PX_Object *pObject,const px_char *Text );
px_void PX_Object_EditAppendText( PX_Object *pObject,const px_char *Text );
px_void PX_Object_EditSetFocus( PX_Object *pObject,px_bool OnFocus);
px_void PX_Object_EditSetPasswordStyle( PX_Object *pObject,px_uchar Enabled );
px_void PX_Object_EditSetBackgroundColor( PX_Object *pObject,px_color Color );
px_void PX_Object_EditSetBorderColor( PX_Object *pObject,px_color Color );
px_void PX_Object_EditSetCursorColor( PX_Object *pObject,px_color Color );
px_void PX_Object_EditSetTextColor( PX_Object *pObject,px_color Color );
px_void PX_Object_EditSetLimit(PX_Object *pObject,const px_char *Limit);
px_void PX_Object_EditSetStyle(PX_Object *pObject,PX_OBJECT_EDIT_STYLE style);
px_void PX_Object_EditSetBorder( PX_Object *pObj,px_bool Border );
px_void PX_Object_EditRender(px_surface *psurface, PX_Object *pObject,px_uint elapsed);
px_void PX_Object_EditFree( PX_Object *pObject );
px_void PX_Object_EditAddString(PX_Object *pObject,px_char *Text);
px_void PX_Object_EditBackspace(PX_Object *pObject);
px_void PX_Object_EditAutoNewLine(PX_Object *pObject,px_bool b,px_int AutoNewLineSpacing);
px_void PX_Object_EditSetOffset(PX_Object *pObject,px_int TopOffset,px_int LeftOffset);
px_void PX_Object_EditSetXYOffset(PX_Object* pObject, px_int XOffset, px_int YOffset);
px_void PX_Object_EditSetInputMode(PX_Object* pObject, PX_OBJECT_EDIT_INPUT_MODE mode);
#endif

