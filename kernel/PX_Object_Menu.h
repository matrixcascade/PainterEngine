#ifndef PX_OBJECT_MENU_H
#define PX_OBJECT_MENU_H
#include "PX_Object.h"

#define PX_MENU_CONTENT_MAX_LEN 48
#define PX_MENU_ITEM_SPACER_SIZE 2
typedef px_void (*PX_MenuExecuteFunc)(px_void *userPtr);


struct _PX_Object_Menu_Item
{
	struct _PX_Object_Menu_Item *pParent;
	px_int  x,y,width,height;
	px_bool enable;
	px_char Text[PX_MENU_CONTENT_MAX_LEN];
	px_bool onCursor;
	px_bool Activated;
	px_list Items;
	PX_MenuExecuteFunc func_callback;
	px_void *user_ptr;
};
typedef struct _PX_Object_Menu_Item PX_Object_Menu_Item;

typedef struct
{
	px_memorypool *mp;
	px_bool activating;
	px_int minimumSize;
	PX_Object_Menu_Item root;
	PX_FontModule *fontmodule;
	px_color backgroundColor;
	px_color fontColor;
	px_color disableColor;
	px_color cursorColor;
}PX_Object_Menu;


PX_Object * PX_Object_MenuCreate(px_memorypool *mp,PX_Object *Parent,px_int x,int y,px_int width,PX_FontModule *fontmodule);
PX_Object_Menu_Item * PX_Object_MenuGetRootItem(PX_Object *pMenuObject);
PX_Object_Menu_Item * PX_Object_MenuAddItem(PX_Object *pMenuObject,PX_Object_Menu_Item *parent,const px_char Text[],PX_MenuExecuteFunc _callback,px_void *ptr);


#endif

