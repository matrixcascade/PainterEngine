#ifndef PX_OBJECT_WIDGET_H
#define PX_OBJECT_WIDGET_H
#include "PX_Object.h"

#define PX_OBJECT_WIDGET_TITLE_LEN 64
#define PX_OBJECT_WIDGET_BAR_SIZE 28
typedef struct
{
	PX_Object	*root;
	PX_Object	*btn_close;
	px_color    borderColor,barColor,backgroundcolor,focusColor,fontcolor;
	px_char		title[PX_OBJECT_WIDGET_TITLE_LEN];
	PX_FontModule *fontmodule;
	px_texture  renderTarget;//optimization for render
	px_point    lastDragPosition;
	px_bool     bDraging;
	px_bool     bmoveable;
	px_bool     bmodel;
	px_bool		event_optimization;
	px_bool     bevent_update;
	px_bool		showShader;
	px_bool		focusWidget;
}PX_Object_Widget;

PX_Object_Widget * PX_Object_GetWidget( PX_Object *pObject );
PX_Object * PX_Object_WidgetCreate(px_memorypool *mp,PX_Object *Parent,px_int x,px_int y,px_int width,px_int height,const px_char title[],PX_FontModule *fontmodule);
px_void PX_Object_WidgetShow(PX_Object *pObject);
px_void PX_Object_WidgetHide(PX_Object *pObject);
px_void PX_Object_WidgetShowHideCloseButton(PX_Object *pObject,px_bool show);
px_void PX_Object_WidgetSetModel(PX_Object *pObject,px_bool model);
PX_Object * PX_Object_WidgetGetRoot(PX_Object *pObject);
px_int PX_Object_WidgetGetRenderTargetHeight(PX_Object *pObject);
px_int PX_Object_WidgetGetRenderTargetWidth(PX_Object *pObject);
px_surface *PX_Object_WidgetGetRenderTarget(PX_Object *pObject);

px_void PX_Object_WidgetSetBorderColor(PX_Object *pObject,px_color clr);
px_void PX_Object_WidgetSetBarColor(PX_Object *pObject,px_color clr);
px_void PX_Object_WidgetSetBackgroundColor(PX_Object *pObject,px_color clr);
px_void PX_Object_WidgetSetFocusColor(PX_Object *pObject,px_color clr);
px_void PX_Object_WidgetSetFontColor(PX_Object *pObject,px_color clr);
px_void PX_Object_WidgetSetTitle(PX_Object *pObject,const px_char title[]);
px_void PX_Object_WidgetShowShader(PX_Object* pObject, px_bool show);
px_void PX_Object_WidgetSetFocusWidget(PX_Object* pObject, px_bool bf);
#endif

