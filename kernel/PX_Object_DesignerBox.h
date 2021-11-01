#ifndef PX_OBJECT_DESIGNERBOX_H
#define PX_OBJECT_DESIGNERBOX_H
#include "PX_Object.h"

typedef enum
{
	PX_Object_DesignerBox_bselect_none=0,
	PX_Object_DesignerBox_bselect_center,
	PX_Object_DesignerBox_bselect_lefttop,
	PX_Object_DesignerBox_bselect_righttop,
	PX_Object_DesignerBox_bselect_leftbottom,
	PX_Object_DesignerBox_bselect_rightbottom,
}PX_Object_DesignerBox_bselect;


typedef struct
{
	px_float lastx, lasty;

	px_color borderColor;
	PX_Object_DesignerBox_bselect bselect;
}PX_Object_DesignerBox;

PX_Object * PX_Object_DesignerBoxCreate(px_memorypool *mp,PX_Object *Parent,px_float x,px_float y,px_float width,px_float height);
PX_Object_DesignerBox* PX_Object_GetDesignerBox(PX_Object *pObject );
px_void PX_Object_DesignerBoxReset(PX_Object* pObject, px_float x, px_float y, px_float widht, px_float height);

#endif

