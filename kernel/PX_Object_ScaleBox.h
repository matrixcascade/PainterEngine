#ifndef PX_OBJECT_SCALEBOX_H
#define PX_OBJECT_SCALEBOX_H
#include "PX_Object.h"



typedef struct
{
	px_int current_selectpt;
	px_int current_cursorpt;
	px_int editpt_count;
	px_point2D cursor_pt;
	px_point2D edit_pt[4];
	px_point2D scale_pt[4];
	px_color linecolor,regioncolor;
}PX_Object_ScaleBox;
PX_Object * PX_Object_ScaleBoxCreate(px_memorypool *mp,PX_Object *Parent);
PX_Object_ScaleBox * PX_Object_GetScaleBox( PX_Object *Object );
px_point2D *PX_Object_ScaleBoxGetPoints(PX_Object *Object);



#endif

