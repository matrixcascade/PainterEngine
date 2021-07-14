#include "PainterEngine_Designer.h"

px_void PX_DesignerUpdate(PX_Object *pObject,px_dword elpased)
{
	px_int i,count;
	PX_Designer *pDesc=(PX_Designer *)pObject->pObject;

	count= pDesc->controllersDesc.size;
	PX_Object_ListClear(pDesc->list_controllers);
	for (i=0;i<count;i++)
	{
		PX_Designer_ControllerType*pController=PX_VECTORAT(PX_Designer_ControllerType,&pDesc->controllersDesc,i);
		PX_Object_ListAdd(pDesc->list_controllers, pController);
	}
}

px_void PX_Designer_ControllersListItemOnRender(px_surface* psurface,PX_Object *pObject, px_dword elpased)
{
	PX_Designer_ControllerType*pDesc=(PX_Designer_ControllerType*)PX_Object_ListItemGetData(pObject);
	PX_Designer *pDesignerDesc=PX_ObjectGetDesc(PX_Designer,(PX_Object *)pObject->User_ptr);
	px_float objx, objy, objWidth, objHeight;
	px_float inheritX, inheritY;
	PX_ObjectGetInheritXY(pObject, &inheritX, &inheritY);
	objx = (pObject->x + inheritX);
	objy = (pObject->y + inheritY);
	objWidth = pObject->Width;
	objHeight = pObject->Height;

	switch (pDesc->type)
	{
	case PX_DESIGNER_CONTROLLER_STATIC:
		PX_GeoDrawBorder(psurface,(px_int)objx+1,(px_int)objy+1,(px_int)objx+64,(px_int)objy+PX_DESIGNER_CONTROLLER_ITEM_HEIGHT-2,1,PX_COLOR(255,64,64,255));
		PX_FontModuleDrawText(psurface,pDesignerDesc->fm,(px_int)objx+32,(px_int)objy+PX_DESIGNER_CONTROLLER_ITEM_HEIGHT/2,PX_ALIGN_CENTER,"static",PX_COLOR(255,64,64,255));
		break;
	case PX_DESIGNER_CONTROLLER_DYNAMIC:
		PX_GeoDrawBorder(psurface,(px_int)objx+1,(px_int)objy+1,(px_int)objx+64,(px_int)objy+PX_DESIGNER_CONTROLLER_ITEM_HEIGHT-2,1,PX_COLOR(255,64,255,64));
		PX_FontModuleDrawText(psurface,pDesignerDesc->fm,(px_int)objx+32,(px_int)objy+PX_DESIGNER_CONTROLLER_ITEM_HEIGHT/2,PX_ALIGN_CENTER,"dynamic",PX_COLOR(255,64,255,64));
		break;
	}


}


px_bool PX_Designer_ControllersListItemOnCreate(px_memorypool* mp, PX_Object* ItemObject, px_void* userptr)
{
	ItemObject->Func_ObjectRender = PX_Designer_ControllersListItemOnRender;
	ItemObject->User_ptr = userptr;
	return PX_TRUE;
}


px_void PX_DesignerRender(px_surface* psurface,PX_Object *pObject, px_dword elpased)
{
	PX_Designer*pDesc=PX_ObjectGetDesc(PX_Designer,pObject);
	switch (pDesc->mode)
	{
	case PX_DESIGNER_MODE_DYNAMIC:
	{
		PX_WorldRender(psurface, &pDesc->world, elpased);
	}
	break;
	case PX_DESIGNER_MODE_STATIC:
	{
		PX_ObjectRender(psurface, pDesc->static_root, elpased);
	}
	break;
	case PX_DESIGNER_MODE_ALL:
	{
		PX_WorldRender(psurface, &pDesc->world, elpased);
		PX_ObjectRender(psurface, pDesc->static_root, elpased);
	}
	break;
	default:
		break;
	}
}

//////////////////////////////////////////////////////////////////////////
//Push button
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//radio button
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//edit
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//process bar
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//list
//////////////////////////////////////////////////////////////////////////




px_bool PX_DesignerStandardInitialize(PX_Designer *designer)
{

}

PX_Object * PX_DesignerCreate(px_memorypool *mp,PX_Object *pparent,px_int surface_width,int surface_height,int world_height,px_int world_width,PX_FontModule *fm, PX_DESIGNER_MODE mode)
{
	PX_Object *pObject;
	PX_Designer desc,*pdesc;
	PX_memset(&desc,0,sizeof(desc));

	pObject=PX_ObjectCreateEx(mp,pparent,0,0,0,0,0,0,0,PX_DesignerUpdate,PX_DesignerRender,PX_NULL,&desc,sizeof(desc));
	pdesc=(PX_Designer *)pObject->pObject;

	pdesc->widget_property = PX_Object_WidgetCreate(mp, pObject, 5, 5 + 256, 192, 192, "property", PX_NULL);
	if (!pdesc->widget_property)return PX_FALSE;

	pdesc->widget_controllers=PX_Object_WidgetCreate(mp,pObject,5,5,192,256,"controllers",PX_NULL);
	if(!pdesc->widget_controllers)return PX_FALSE;
	


	if (!PX_VectorInitialize(mp, &pdesc->controllersDesc, sizeof(PX_Designer_ControllerType), 16))return PX_FALSE;
	pdesc->selectcontroller=PX_NULL;
	pdesc->fm=fm;
	pdesc->menu=PX_Object_MenuCreate(mp,pObject,0,0,surface_width-1,fm);
	if(!pdesc->menu)return PX_NULL;
	pdesc->messagebox=PX_Object_MessageBoxCreate(mp,pObject,fm);
	if(!pdesc->messagebox)return PX_NULL;
	if(!PX_WorldInitialize(mp,&pdesc->world,world_width,world_height,surface_width,surface_height,1024*1024))return PX_NULL;
	pdesc->static_root=PX_ObjectCreate(mp,0,0,0,0,0,0,0);
	pdesc->list_controllers=PX_Object_ListCreate(mp,PX_Object_WidgetGetRoot(pdesc->widget_controllers),0,1,
		(px_int)PX_Object_GetWidget(pdesc->widget_controllers)->renderTarget.width-1,
		(px_int)PX_Object_GetWidget(pdesc->widget_controllers)->renderTarget.height-1,
		PX_DESIGNER_CONTROLLER_ITEM_HEIGHT,
		PX_Designer_ControllersListItemOnCreate,
		pObject
		);
	pdesc->mode = PX_DESIGNER_MODE_ALL;


	return pObject;
}
