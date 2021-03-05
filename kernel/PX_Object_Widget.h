#ifndef PX_OBJECT_WIDGET_H
#define PX_OBJECT_WIDGET_H
#include "PX_Object.h"

#define PX_OBJECT_WIDGET_TITLE_LEN 64
#define PX_OBJECT_WIDGET_BAR_SIZE 24
typedef struct
{
	PX_Object	*root;
	PX_Object	*btn_close;
	px_color    borderColor,barColor,backgroundcolor,focusColor;
	px_char		title[PX_OBJECT_WIDGET_TITLE_LEN];
	PX_FontModule *fontmodule;
	px_texture  renderTarget;//optimization for render
	px_point    lastDragPosition;
	px_bool     bDraging;
	px_bool     bmodel;
	px_bool		event_optimization;
	px_bool     bevent_update;
}PX_Object_Widget;

px_void PX_Object_WidgetShow(PX_Object *pObject);
px_void PX_Object_WidgetHide(PX_Object *pObject);
PX_Object * PX_Object_WidgetCreate(px_memorypool *mp,PX_Object *Parent,int x,int y,int width,int height,const px_char title[],PX_FontModule *fontmodule);
PX_Object_Widget * PX_Object_GetWidget( PX_Object *Object );
px_void PX_Object_WidgetSetModel(PX_Object *Object,px_bool model);
PX_Object * PX_Object_WidgetGetRoot(PX_Object *pObject);
px_int PX_Object_WidgetGetRenderTargetHeight(PX_Object *pObject);
px_int PX_Object_WidgetGetRenderTargetWidth(PX_Object *pObject);


#endif