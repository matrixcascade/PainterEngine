#ifndef PX_MENU_H
#define PX_MENU_H

#define PX_MENU_CONTENT_MAX_LEN 48
#define PX_MENU_ITEM_SPACER_SIZE 2
#include "PainterEngine_Runtime.h"


typedef px_void (*PX_MenuExecuteFunc)(px_void *userPtr);


typedef struct _PX_Menu_Item
{
	struct _PX_Menu_Item *pParent;
	px_int  x,y,width,height;
	px_char Text[PX_MENU_CONTENT_MAX_LEN];
	px_bool onCursor;
	px_bool Activated;
	px_list Items;
	PX_MenuExecuteFunc func_callback;
	px_void *user_ptr;
};
typedef struct _PX_Menu_Item PX_Menu_Item;

typedef struct
{
	px_memorypool *mp;
	px_bool enabled;
	px_bool OnFocus;
	px_int x,y;
	px_int minimumSize;
	PX_Menu_Item root;
	PX_FontModule *fontmodule;
	px_color backgroundColor;
	px_color fontColor;
	px_color cursorColor;
}PX_Menu;


px_bool PX_MenuInitialize(PX_Runtime *runtime,px_memorypool *mp,PX_Menu *pMenu,px_int x,int y,px_int width,PX_FontModule *fontmodule);
px_void PX_MenuPostEvent(PX_Menu *pMenu,PX_Object_Event e);
px_void PX_MenuUpdate(PX_Menu *pMenu,px_dword elpased);
px_void PX_MenuRender(px_surface *pSurface,PX_Menu *pMenu,px_dword elpased);
px_void PX_MenuFree(PX_Menu *pMenu);
PX_Menu_Item * PX_MenuAddItem(PX_Menu *pMenu,PX_Menu_Item *parent,const px_char Text[],PX_MenuExecuteFunc _callback,px_void *ptr);
#endif