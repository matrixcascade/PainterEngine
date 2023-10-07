#ifndef PX_OBJECT_CANVAS_H
#define PX_OBJECT_CANVAS_H
#include "PX_Object.h"
#include "PX_CanvasVM.h"
typedef enum 
{
	PX_OBJECT_CANVAS_OPCODE_SELECTEDIT,
	PX_OBJECT_CANVAS_OPCODE_CREATELAYER,
	PX_OBJECT_CANVAS_OPCODE_PEN,
	PX_OBJECT_CANVAS_OPCODE_PAINT,
	PX_OBJECT_CANVAS_OPCODE_ERASER,
	PX_OBJECT_CANVAS_OPCODE_BRUSH,
}PX_OBJECT_CANVAS_OPCODE;



typedef struct 
{
	px_memorypool* mp;
	PX_Object* pHSliderBar, * pVSliderBar;
	px_float reg_x, reg_y, reg_z,reg_layer;
	px_float pin_x, pin_y;
	px_float pin_view_x, pin_view_y;
	px_texture *reg_ptarget;
	PX_CanvasVM *pCanvasVM;
}PX_Object_Canvas;

PX_Object* PX_Object_CanvasCreate(px_memorypool* mp, PX_Object* Parent, px_int x, px_int y, PX_CanvasVM* pCanvasVM);
PX_Object_Canvas*PX_Object_GetCanvas(PX_Object *pObject);



#endif



