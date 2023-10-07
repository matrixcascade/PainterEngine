#ifndef PX_OBJECT_PANEL_H
#define PX_OBJECT_PANEL_H
#include "PX_Object.h"


typedef struct 
{
	px_texture rendersurface;
}PX_Object_Panel;

PX_Object * PX_Object_PanelCreate(px_memorypool *mp,PX_Object *Parent,px_int x,px_int y,px_int width,px_int height);
px_surface* PX_Object_PanelGetSurface(PX_Object *pObject);

#endif



