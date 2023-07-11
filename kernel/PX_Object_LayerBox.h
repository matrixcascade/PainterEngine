#ifndef PX_LAYERBOX_H
#define PX_LAYERBOX_H

#include "PX_Object.h"
#include "PX_CanvasVM.h"

typedef struct
{
	px_memorypool* ui_mp;
	PX_Object *widget;
	PX_Object* list,*image;
	PX_FontModule* fm;
	px_texture tex_view;
	px_texture tex_clip, tex_new, tex_delete, tex_visible, tex_invisible, tex_edit, tex_down, tex_up;
	PX_Object* btn_new, * btn_delete,* btn_up,* btn_down,* btn_clip;
	px_int reg_state_id;
	PX_CanvasVM *pCanvasVM;
}PX_Object_LayerBox;

PX_Object* PX_Object_LayerBoxCreate(px_memorypool* ui_mp,  PX_Object* Parent, px_int x, px_int y, PX_FontModule* fm,PX_CanvasVM *pCanvasVM);
px_texture* PX_Object_LayerBoxGetPreviewSurface(PX_Object* pObject);

#endif