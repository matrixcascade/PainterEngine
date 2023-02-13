#ifndef PX_LAYERBOX_H
#define PX_LAYERBOX_H

#include "PX_Object.h"

#define PX_OBJECT_LAYER_MAX 128
#define PX_OBJECT_LAYER_NAME_LENGTH 64

typedef enum
{
	PX_OBJECT_LAYER_ATTRIBUTE_NORMAL,
	PX_OBJECT_LAYER_ATTRIBUTE_CLIP,
}PX_OBJECT_LAYER_ATTRIBUTE;


typedef struct  
{
	px_bool activating;
	px_char name[PX_OBJECT_LAYER_NAME_LENGTH];
	px_texture surface_preview_mini,surface_layer;
	px_bool editing;
	px_bool visible;
	PX_OBJECT_LAYER_ATTRIBUTE attribute;
}PX_Object_Layer;


typedef struct
{
	px_memorypool* ui_mp;
	px_memorypool* data_mp;
	PX_Object *widget;
	PX_Object* list,*image;
	PX_FontModule* fm;
	px_int width, height;
	PX_Object_Layer layers[PX_OBJECT_LAYER_MAX];
	px_texture tex_view;
	px_texture tex_clip, tex_new, tex_delete, tex_visible, tex_invisible, tex_edit, tex_down, tex_up;
	PX_Object* btn_new, * btn_delete,* btn_up,* btn_down,* btn_clip;
}PX_Object_LayerBox;

PX_Object* PX_Object_LayerBoxCreate(px_memorypool* ui_mp, px_memorypool* data_mp, PX_Object* Parent, px_int x, px_int y, px_int surface_width, px_int surface_height, PX_FontModule* fm);
px_texture* PX_Object_LayerBoxGetPreviewSurface(PX_Object* pObject);
px_surface* PX_Object_LayerBoxGetEditingSurface(PX_Object* pObject);
px_void PX_Object_LayerBoxUpdateLayer(PX_Object* pObject, px_int layerindex);
px_void PX_Object_LayerBoxUpdateEditingLayer(PX_Object* pObject);
#endif