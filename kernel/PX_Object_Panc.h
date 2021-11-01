#ifndef PX_OBJECT_PANC_H
#define PX_OBJECT_PANC_H
#include "PX_Object.h"

typedef struct  
{
	px_float x,y;
	px_float width;
	px_float height;
	px_float sourceX,sourceY;
	px_float currentX, currentY;
}PX_PancMatrix;

typedef enum
{
	PX_Object_Panc_bselect_none=0,
	PX_Object_Panc_bselect_center,
	PX_Object_Panc_bselect_lefttop,
	PX_Object_Panc_bselect_righttop,
	PX_Object_Panc_bselect_leftbottom,
	PX_Object_Panc_bselect_rightbottom,
}PX_Object_Panc_bselect;


typedef struct
{
	px_float sourceX,sourceY;
	px_float currentX,currentY;
	px_color borderColor;
	PX_Object_Panc_bselect bselect;
}PX_Object_Panc;

PX_Object * PX_Object_PancCreate(px_memorypool *mp,PX_Object *Parent,px_float x,px_float y,px_float width,px_float height);
PX_Object_Panc * PX_Object_GetPanc(PX_Object *pObject );
px_void PX_Object_PancReset(PX_Object* pObject, px_float x, px_float y, px_float widht, px_float height);
PX_PancMatrix PX_Object_PancGetMatrix(PX_Object* pObject);

#endif

