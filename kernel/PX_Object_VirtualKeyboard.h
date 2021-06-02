#ifndef PX_OBJECT_VKBOARD_H
#define PX_OBJECT_VKBOARD_H
#include "PX_Object.h"
typedef struct
{
	px_int x,y,width,height;
	px_char u_key[8];
	px_char d_key[8];
	px_bool bCursor;
	px_bool bDown;
	px_bool bhold;

}PX_Object_VirtualKey;

typedef struct
{
	PX_Object_VirtualKey Keys[14+14+13+12+3];
	px_color backgroundColor;
	px_color borderColor;
	px_color cursorColor;
	px_color pushColor;
	px_char functionCode;
	PX_Object *LinkerObject;
	px_bool bTab,bUpper,bShift,bCtrl,bAlt;
}PX_Object_VirtualKeyBoard;

PX_Object* PX_Object_VirtualKeyBoardCreate(px_memorypool *mp, PX_Object *Parent,px_int x,px_int y,px_int width,px_int height);
px_void PX_Object_VirtualKeyBoardSetBackgroundColor( PX_Object *pObject,px_color Color );
px_void PX_Object_VirtualKeyBoardSetBorderColor( PX_Object *pObject,px_color Color );
px_void PX_Object_VirtualKeyBoardSetCursorColor( PX_Object *pObject,px_color Color );
px_void PX_Object_VirtualKeyBoardSetPushColor( PX_Object *pObject,px_color Color );
px_char PX_Object_VirtualKeyBoardGetCode(PX_Object *pObject);
px_void PX_Object_VirtualKeyBoardSetLinkerObject(PX_Object *pObject,PX_Object *linker);

typedef struct
{
	PX_Object_VirtualKey Keys[17];
	px_color backgroundColor;
	px_color borderColor;
	px_color cursorColor;
	px_color pushColor;
	px_char functionCode;
	PX_Object *LinkerObject;
}PX_Object_VirtualNumberKeyBoard;

PX_Object* PX_Object_VirtualNumberKeyBoardCreate(px_memorypool *mp, PX_Object *Parent,px_int x,px_int y,px_int width,px_int height);
px_void PX_Object_VirtualNumberKeyBoardSetBackgroundColor( PX_Object *pObject,px_color Color );
px_void PX_Object_VirtualNumberKeyBoardSetBorderColor( PX_Object *pObject,px_color Color );
px_void PX_Object_VirtualNumberKeyBoardSetCursorColor( PX_Object *pObject,px_color Color );
px_void PX_Object_VirtualNumberKeyBoardSetPushColor( PX_Object *pObject,px_color Color );
px_char PX_Object_VirtualNumberKeyBoardGetCode(PX_Object *pObject);
px_void PX_Object_VirtualNumberKeyBoardSetLinkerObject(PX_Object *pObject,PX_Object *linker);


#endif

