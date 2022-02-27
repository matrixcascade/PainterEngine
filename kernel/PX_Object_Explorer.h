#ifndef PX_OBJECT_EXPLORER_H
#define PX_OBJECT_EXPLORER_H
#include "PX_Object.h"
#define PX_EXPLORER_MAX_PATH_LEN 260
#define PX_EXPLORER_MAX_ITEMS  1024
#define PX_OBJECT_EXPOLRER_MENU_HEIGHT 48
#define PX_OBJECT_EXPOLRER_ITEM_HEIGHT 38
#define PX_OBJECT_EXPOLRER_SLIDERBAR_WIDTH 24
#define PX_OBJECT_EXPOLRER_BUTTON_WIDTH 64
typedef px_int (*PX_ExplorerGetPathFolderCount)(const px_char *path,const char *filter);
typedef px_int (*PX_ExplorerGetPathFileCount)(const px_char *path,const char *filter);
typedef px_int (*PX_ExplorerGetPathFolderName)(const char path[],int count,char FileName[][260],const char *filter);
typedef px_int (*PX_ExplorerGetPathFileName)(const char path[],int count,char FileName[][260],const char *filter);

typedef struct  
{
	px_bool bFolder;
	px_bool bcursor;
	px_bool bselect;
	px_char name[260];
}PX_Object_Explorer_Item;

typedef enum
{
	PX_OBJECT_EXPLORER_RETURN_CONFIRM,
	PX_OBJECT_EXPLORER_RETURN_CANCEL,
}PX_OBJECT_EXPLORER_RETURN;


typedef struct  
{
	PX_ExplorerGetPathFolderCount func_getpathfoldercount;
	PX_ExplorerGetPathFileCount func_getpathfilecount;
	PX_ExplorerGetPathFolderName func_getpathfoldername;
	PX_ExplorerGetPathFileName func_getpathfilename;
	px_bool savemode;
	px_int ItemCount;
	px_int MaxSelectedCount;
	PX_FontModule *fontmodule;
	PX_Object_Explorer_Item Items[PX_EXPLORER_MAX_ITEMS];

	PX_Object *edit_Path;
	PX_Object* edit_FileName;
	PX_Object *btn_Back,*btn_go,*btn_Ok,*btn_Cancel;
	PX_Object *SliderBar;
	px_char filter[PX_EXPLORER_MAX_PATH_LEN];

	px_shape file,folder;

	px_color backgroundcolor,cursorcolor,fontcolor,bordercolor,pushcolor;

	PX_OBJECT_EXPLORER_RETURN returnType;

}PX_Object_Explorer;

PX_Object_Explorer *PX_Object_GetExplorer(PX_Object *Object);
PX_Object * PX_Object_ExplorerCreate(px_memorypool *mp, PX_Object *Parent,px_int x,px_int y,px_int Width,px_int Height,PX_FontModule *fm,
	PX_ExplorerGetPathFolderCount _func_gpfdc,
	PX_ExplorerGetPathFileCount _func_gpfec,
	PX_ExplorerGetPathFolderName _func_gpfdn,
	PX_ExplorerGetPathFileName _func_gpfcn,
	const px_char path[260]
);
px_void PX_Object_ExplorerSetBorderColor(PX_Object *Object,px_color clr);
px_void PX_Object_ExplorerSetPushColor(PX_Object *Object,px_color clr);
px_void PX_Object_ExplorerSetCursorColor(PX_Object *Object,px_color clr);
px_void PX_Object_ExplorerSetTextColor(PX_Object *Object,px_color clr);
px_void PX_Object_ExplorerRefresh(PX_Object *Object);
px_int PX_Object_ExplorerGetSelectedCount(PX_Object *Object);
px_void PX_Object_ExplorerGetPath(PX_Object *Object,px_char path[PX_EXPLORER_MAX_PATH_LEN],px_int index);
px_void PX_Object_ExplorerOpen(PX_Object *Object);
px_void PX_Object_ExplorerSave(PX_Object* Object);
px_void PX_Object_ExplorerClose(PX_Object *Object);
px_void PX_Object_ExplorerSetFilter(PX_Object *Object,const px_char *filter);
px_void PX_Object_ExplorerSetMaxSelectCount(PX_Object *Object,int selectCount);


#endif
