#ifndef PX_OBJECT_LIST_H
#define PX_OBJECT_LIST_H
#include "PX_Object.h"


typedef px_bool (*PX_Object_ListItemOnCreate)(px_memorypool *mp,PX_Object *ItemObject,px_void *userptr);


typedef struct
{
	px_void *pdata;
}PX_Object_ListItem;

typedef struct
{
	px_memorypool *mp;
	px_int ItemWidth;
	px_int ItemHeight;
	px_int offsetx;
	px_int offsety;
	px_int currentSelectedIndex,currentCursorIndex;
	px_color BorderColor;
	px_color BackgroundColor;
	px_color CursorColor;
	px_color SelectCursor;
	px_surface renderSurface;
	px_bool showCursor;
	px_bool doubleCursorCancel;
	px_vector Items;
	px_vector pData;
	PX_Object *SliderBar;
	PX_Object_ListItemOnCreate CreateFunctions;
}PX_Object_List;

PX_Object_ListItem * PX_Object_GetListItem( PX_Object *Object );
PX_Object * PX_Object_ListCreate(px_memorypool *mp, PX_Object *Parent,px_int x,px_int y,px_int Width,px_int Height,px_int ItemHeight,PX_Object_ListItemOnCreate _CreateFunc,px_void *userptr);
px_void PX_Object_ListClear(PX_Object *pListObj);
px_int PX_Object_ListGetCurrentSelectIndex(PX_Object *pObject);
px_int PX_Object_ListAdd(PX_Object *pListObj,px_void *ptr);
px_void *PX_Object_ListGetItemData(PX_Object *pListObject,px_int index);
px_void PX_Object_ListRemoveItem(PX_Object *pListObject,px_int index);
px_void PX_Object_ListSetBackgroundColor(PX_Object *pListObject,px_color color);
px_void PX_Object_ListSetBorderColor(PX_Object *pListObject,px_color color);
px_void PX_Object_ListSetDoubleClickCancel(PX_Object *pListObject,px_bool b);


#endif