#ifndef PX_DESIGNER_H
#define PX_DESIGNER_H
#include "PX_World.h"
#include "PX_Json.h"



typedef struct
{
	PX_Object *pObject;
	px_int descIndex;
	PX_DESIGNER_OBJECT_TYPE type;
}PX_Designer_Object;


typedef struct  
{
	px_memorypool *mp;
	PX_Object *pLinkObject;
	PX_World  *pLinkWorld;
	px_void*	userptr;
	PX_Object	*widget_controllers;
	PX_Object	*widget_property,*widget_propertyscrollarea,* widget_objects;
	PX_Object* world_hscroll, *world_vscroll;
	PX_Object*	designerbox;
	PX_Object*	list_ObjectDesc;
	PX_Object* list_Objects;
	PX_Object*  button_delete;
	PX_Object*	messagebox;
	PX_Object*	menu,*menu_run,*menu_exit,menu_window_controllers,menu_window_properties;
	PX_Object*  label_propertys[PX_DESIGNER_MAX_PROPERTYS]; 
	PX_Object*  edit_propertys[PX_DESIGNER_MAX_PROPERTYS]; 
	PX_FontModule *fm;
	px_int allocID;
	px_int selectObjectIndex;
	px_bool showsliderbar;
	px_vector ObjectDesc;
	px_vector Objects;
}PX_Object_Designer;

PX_Object * PX_Object_DesignerCreate(px_memorypool *mp,PX_Object *pparent,PX_Object *pLinkObject,PX_World *pLinkWorld,PX_FontModule *fm,px_void *userptr);
px_bool PX_Object_DesignerAddObjectDescription(PX_Object* pObject, PX_Designer_ObjectDesc* desc);
px_void PX_Object_DesignerEnable(PX_Object* pObject);
px_void PX_Object_DesignerBindWorld(PX_Object* pObject, PX_World* world);
px_void PX_Object_DesignerBindRoot(PX_Object* pObject,PX_Object *root);
px_void PX_Object_DesignerDisable(PX_Object* pObject);
px_bool PX_Object_DesignerExport(PX_Object* pObject, px_string* pText);
px_bool PX_Object_DesignerImport(PX_Object* pObject, const px_char* pText);
px_bool PX_Object_DesignerImportToUIObject(px_memorypool* mp, PX_Object* DesignerObject, PX_Object* pRootObject, const px_char* pText, PX_FontModule* fm);
px_bool PX_Object_DesignerDefaultInstall(px_vector* pvector);
px_void PX_Object_DesignerClear(PX_Object* pObject);

#endif