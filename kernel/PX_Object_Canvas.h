#ifndef PX_OBJECT_CANVAS_H
#define PX_OBJECT_CANVAS_H
#include "PX_Object.h"

typedef enum 
{
	PX_OBJECT_CANVAS_OPCODE_SELECTEDIT,
	PX_OBJECT_CANVAS_OPCODE_CREATELAYER,
	PX_OBJECT_CANVAS_OPCODE_PEN,
	PX_OBJECT_CANVAS_OPCODE_PAINT,
	PX_OBJECT_CANVAS_OPCODE_ERASER,
	PX_OBJECT_CANVAS_OPCODE_BRUSH,
}PX_OBJECT_CANVAS_OPCODE;

typedef enum
{
	PX_OBJECT_CANVAS_MODE_STANDBY,
	PX_OBJECT_CANVAS_MODE_EDIT,
}PX_OBJECT_CANVAS_MODE;

typedef struct 
{
	px_memorypool* mp;
	px_float view_x, view_y;
	px_float scale,min_scale,max_scale;
	PX_Object* pLayerBoxObject,* pPainterBoxObject;
	PX_Object* pHSliderBar, * pVSliderBar;
	px_int activating_texture_index;
	px_texture backward_texture;
	px_texture editing_texture;
	px_texture frontward_texture;
	px_texture cache_texture;
	px_texture view_surface;
	PX_OBJECT_CANVAS_MODE mode;
	px_float reg_x, reg_y,reg_layer;
}PX_Object_Canvas;

PX_Object * PX_Object_CanvasCreate(px_memorypool *mp,PX_Object *Parent,px_int x,px_int y,px_int width,px_int height, PX_Object* pLayerObject, PX_Object* pPainterBoxObject);
PX_Object_Canvas*PX_Object_GetCanvas(PX_Object *Object);



#endif



