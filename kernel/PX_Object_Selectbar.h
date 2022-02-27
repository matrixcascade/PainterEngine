#ifndef PX_OBJECT_SELECTBAR_H
#define PX_OBJECT_SELECTBAR_H
#include "PX_Object.h"
#define PX_SELECTBAR_CONTENT_MAX_LEN 48

typedef enum
{
	PX_OBJECT_SELECTBAR_STYLE_RECT,
	PX_OBJECT_SELECTBAR_STYLE_ROUNDRECT,
}PX_OBJECT_SELECTBAR_STYLE;


typedef struct
{
	px_char Text[PX_SELECTBAR_CONTENT_MAX_LEN];
	px_bool onCursor;
}PX_Object_SelectBar_Item;;

typedef struct
{
	px_memorypool *mp;
	px_int	maxDisplayCount;
	px_int  currentDisplayOffsetIndex;
	px_int  ItemHeight;
	px_bool activating;
	px_bool onCursor;
	px_vector Items;//PX_Object_SelectBar_Item;
	PX_FontModule *fontmodule;
	px_color backgroundColor;
	px_color fontColor;
	px_color cursorColor;
	px_color borderColor;
	px_int   selectIndex;
	PX_Object *sliderBar;
	PX_OBJECT_SELECTBAR_STYLE style;
}PX_Object_SelectBar;

PX_Object_SelectBar *PX_Object_GetSelectBar(PX_Object *pSelecrBar);
PX_Object * PX_Object_SelectBarCreate(px_memorypool *mp,PX_Object *Parent,px_int x,int y,px_int width,px_int height,PX_FontModule *fontmodule);
px_int  PX_Object_SelectBarAddItem(PX_Object *PX_Object_SelectBar,const px_char Text[]);
px_void PX_Object_SelectBarClear(PX_Object* pSelectBarObject);
px_void PX_Object_SelectBarRemoveItem(PX_Object* PX_Object_SelectBar, px_int index);
px_int PX_Object_SelectBarGetItemIndexByText(PX_Object *pObject,const px_char Text[]);
const px_char *PX_Object_SelectBarGetCurrentText(PX_Object *pObject);
const px_char* PX_Object_SelectBarGetItemText(PX_Object* pObject, px_int index);
px_void PX_Object_SelectBarSetDisplayCount(PX_Object* pObject, px_int count);
px_int  PX_Object_SelectBarGetCurrentIndex(PX_Object *pObject);
px_void  PX_Object_SelectBarSetCurrentIndex(PX_Object *pObject,px_int index);
px_void PX_Object_SelectBarSetStyle(PX_Object *pObject,PX_OBJECT_SELECTBAR_STYLE style);
px_void PX_Object_SelectBarSetFontColor(PX_Object *pObject,px_color color);
px_void PX_Object_SelectBarSetCursorColor(PX_Object *pObject,px_color color);
px_void PX_Object_SelectBarSetBorderColor(PX_Object *pObject,px_color color);
px_void PX_Object_SelectBarSetBackgroundColor(PX_Object *pObject,px_color color);
px_void PX_Object_SelectBarSetMaxDisplayCount(PX_Object* pObject, px_int color);
#endif

