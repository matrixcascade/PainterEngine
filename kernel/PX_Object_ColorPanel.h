#ifndef PX_MODULE_COLORPANEL_H
#define PX_MODULE_COLORPANEL_H

#include "PX_Object.h"

typedef enum
{
	PX_Object_ColorPanel_State_Normal,
	PX_Object_ColorPanel_State_Ring,
	PX_Object_ColorPanel_State_SV,
}PX_Object_ColorPanel_State;

typedef struct
{
	px_memorypool* mp;
	PX_Object_ColorPanel_State state;
	px_color color_rgb;
	px_color_hsv color_hsv;
	PX_Object* sliderbar_r, sliderbar_g, sliderbar_b;
	px_texture panel;
}PX_Object_ColorPanel;


px_color PX_Object_ColorPanelGetColor(PX_Object* pObject);
PX_Object_ColorPanel* PX_Object_GetColorPanel(PX_Object* pObject);
PX_Object* PX_Object_ColorPanelCreate(px_memorypool* mp, PX_Object* Parent, px_int x, px_int y, px_int width, px_int height);

PX_Designer_ObjectDesc PX_Object_ColorPanelDesignerInstall();
#endif