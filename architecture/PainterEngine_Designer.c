#include "PainterEngine_Designer.h"

px_void PX_DesignerUpdate(PX_Object *pObject,px_dword elpased)
{
	px_int i,count;
	PX_Designer *pDesc=(PX_Designer *)pObject->pObject;

	count= pDesc->ObjectDesc.size;
	PX_Object_ListClear(pDesc->list_ObjectDesc);
	for (i=0;i<count;i++)
	{
		PX_Designer_ObjectDesc*pController=PX_VECTORAT(PX_Designer_ObjectDesc,&pDesc->ObjectDesc,i);
		PX_Object_ListAdd(pDesc->list_ObjectDesc, pController);
	}
}

px_void PX_Designer_ControllersListItemOnRender(px_surface* psurface,PX_Object *pObject, px_dword elpased)
{
	PX_Designer_ObjectDesc*pDesc=(PX_Designer_ObjectDesc*)PX_Object_ListItemGetData(pObject);
	PX_Designer *pDesignerDesc=PX_ObjectGetDesc(PX_Designer,(PX_Object *)pObject->User_ptr);
	px_float objx, objy, objWidth, objHeight;
	px_float inheritX, inheritY;
	PX_ObjectGetInheritXY(pObject, &inheritX, &inheritY);
	objx = (pObject->x + inheritX);
	objy = (pObject->y + inheritY);
	objWidth = pObject->Width;
	objHeight = pObject->Height;

	//////////////////////////////////////////////////////////////////////////
	//designer controller
	//////////////////////////////////////////////////////////////////////////
	//box
	if (pDesignerDesc)
	{
	}


	switch (pDesc->type)
	{
	case PX_DESIGNER_OBJECT_TYPE_STATIC:
		PX_GeoDrawBorder(psurface,(px_int)objx+1,(px_int)objy+1,(px_int)objx+64,(px_int)objy+PX_DESIGNER_CONTROLLER_ITEM_HEIGHT-2,1,PX_COLOR(255,128,64,255));
		PX_FontModuleDrawText(psurface,pDesignerDesc->fm,(px_int)objx+32,(px_int)objy+PX_DESIGNER_CONTROLLER_ITEM_HEIGHT/2,PX_ALIGN_CENTER,"static",PX_COLOR(255,128,64,255));
		PX_FontModuleDrawText(psurface, pDesignerDesc->fm, (px_int)objx + 68, (px_int)objy + PX_DESIGNER_CONTROLLER_ITEM_HEIGHT / 2, PX_ALIGN_LEFTMID, pDesc->Name, PX_COLOR(255, 128, 64, 255));
		break;
	case PX_DESIGNER_OBJECT_TYPE_DYNAMIC:
		PX_GeoDrawBorder(psurface,(px_int)objx+1,(px_int)objy+1,(px_int)objx+64,(px_int)objy+PX_DESIGNER_CONTROLLER_ITEM_HEIGHT-2,1,PX_COLOR(255,128,255,64));
		PX_FontModuleDrawText(psurface,pDesignerDesc->fm,(px_int)objx+32,(px_int)objy+PX_DESIGNER_CONTROLLER_ITEM_HEIGHT/2,PX_ALIGN_CENTER,"dynamic",PX_COLOR(255,128,255,64));
		PX_FontModuleDrawText(psurface, pDesignerDesc->fm, (px_int)objx + 68, (px_int)objy + PX_DESIGNER_CONTROLLER_ITEM_HEIGHT / 2, PX_ALIGN_LEFTMID, pDesc->Name, PX_COLOR(255, 128, 255, 64));
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
	PX_DesignerUpdate(pObject, elpased);
	PX_WorldRender(psurface, &pDesc->world, elpased);
	PX_ObjectRender(psurface, pDesc->static_root, elpased);
}


//////////////////////////////////////////////////////////////////////////
px_float PX_Designer_GetX(PX_Object *pObject)
{
	return pObject->x;
}

px_float PX_Designer_GetY(PX_Object *pObject)
{
	return pObject->y;
}

px_float PX_Designer_GetWidth(PX_Object *pObject)
{
	return pObject->Width;
}

px_float PX_Designer_GetHeight(PX_Object *pObject)
{
	return pObject->Height;
}

px_void PX_Designer_SetX(PX_Object *pObject,px_float v)
{
	 pObject->x=v;
}

px_void PX_Designer_SetY(PX_Object *pObject,px_float v)
{
	 pObject->y=v;
}

px_void PX_Designer_SetWidth(PX_Object *pObject,px_float v)
{
	 pObject->Width=v;
}

px_void PX_Designer_SetHeight(PX_Object *pObject,px_float v)
{
	 pObject->Height=v;
}



px_void PX_Designer_OnCursorDown(PX_Object *pObject,PX_Object_Event e,px_void *ptr)
{
	PX_Designer *pDesignerDesc=PX_ObjectGetDesc(PX_Designer,pObject);
	PX_Object *pSelectObject=PX_NULL;
	if (e.Event==PX_OBJECT_EVENT_CURSORDOWN)
	{
		px_int i;
		px_float x,y;
		x=PX_Object_Event_GetCursorX(e);
		y=PX_Object_Event_GetCursorY(e);
		pDesignerDesc->selectObjectIndex=-1;

		for (i=0;i<pDesignerDesc->Objects.size;i++)
		{
			PX_Designer_Object *pdobject=PX_VECTORAT(PX_Designer_Object,&pDesignerDesc->Objects,i);
			
			if (pdobject->type==PX_DESIGNER_OBJECT_TYPE_DYNAMIC)
			{
				px_rect rect;
				rect.x=pdobject->pObject->x-pDesignerDesc->world.offsetx;
				rect.y=pdobject->pObject->y-pDesignerDesc->world.offsety;
				rect.width=pdobject->pObject->Width;
				rect.height=pdobject->pObject->Height;
				if(PX_isPointInRect(PX_POINT(x,y,0),rect))
				{
					pDesignerDesc->selectObjectIndex=i;
				}
			}
			
			if (pdobject->type==PX_DESIGNER_OBJECT_TYPE_STATIC)
			{
				if(PX_ObjectIsCursorInRegion(pdobject->pObject,e))
				{
					pDesignerDesc->selectObjectIndex=i;
				}
			}

		}


		if (pDesignerDesc->selectObjectIndex==-1)
		{
			px_int selectDescIndex=PX_Object_ListGetCurrentSelectIndex(pDesignerDesc->list_ObjectDesc);
			PX_Designer_Object dobject;
			if (selectDescIndex!=-1)
			{
				//create new object
				PX_Designer_ObjectDesc *pObjectDesc=PX_VECTORAT(PX_Designer_ObjectDesc,&pDesignerDesc->ObjectDesc,selectDescIndex);
				
				switch (pObjectDesc->type)
				{
				case PX_DESIGNER_OBJECT_TYPE_STATIC:
					{
						PX_Object *pNewObject=pObjectDesc->createfunc(pDesignerDesc->mp,pDesignerDesc->static_root,x,y,100,100,pDesignerDesc->fm);
						dobject.pObject=pNewObject;
						dobject.type=PX_DESIGNER_OBJECT_TYPE_STATIC;
						PX_VectorPushback(&pDesignerDesc->Objects,&dobject);
					}
					break;
				case PX_DESIGNER_OBJECT_TYPE_DYNAMIC:
					{
						PX_Object *pNewObject=pObjectDesc->createfunc(pDesignerDesc->mp,PX_NULL,x,y,100,100,pDesignerDesc->fm);
						PX_WorldAddObject(&pDesignerDesc->world,pNewObject);
						dobject.pObject=pNewObject;
						dobject.type=PX_DESIGNER_OBJECT_TYPE_DYNAMIC;
						PX_VectorPushback(&pDesignerDesc->Objects,&dobject);
					}
					break;
				default:
					break;
				}

			}
			else
			{
				pDesignerDesc->lastcursorx=x;
				pDesignerDesc->lastcursory=y;
			}
		}
		else
		{
			PX_Designer_Object *pdobject=PX_VECTORAT(PX_Designer_Object,&pDesignerDesc->Objects,pDesignerDesc->selectObjectIndex);
			pDesignerDesc->designerbox->x=pdobject->pObject->x;
			pDesignerDesc->designerbox->y=pdobject->pObject->y;
			pDesignerDesc->designerbox->Width=pdobject->pObject->Width;
			pDesignerDesc->designerbox->Height=pdobject->pObject->Height;
		}

	}

	
}
//////////////////////////////////////////////////////////////////////////
/*
typedef PX_Object *(*px_designer_createfunc)(px_memorypool *mp,PX_Object *pparent,px_float x,px_float y,px_float width,px_float height,PX_FontModule *fm);
typedef px_float (*px_designer_getproperty_float)(PX_Object *pObject);
typedef px_int (*px_designer_getproperty_int)(PX_Object *pObject);
typedef const px_char *(*px_designer_getproperty_string)(PX_Object *pObject);
typedef px_bool (*px_designer_getproperty_bool)(PX_Object *pObject);

typedef px_void (*px_designer_setproperty_float)(PX_Object *pObject,px_float v);
typedef px_void (*px_designer_setproperty_int)(PX_Object *pObject,px_int v);
typedef px_void (*px_designer_setproperty_string)(PX_Object *pObject,const px_char v[]);
typedef px_void (*px_designer_setproperty_bool)(PX_Object *pObject,px_bool v);
typedef struct
*/
//////////////////////////////////////////////////////////////////////////
//Push button
//////////////////////////////////////////////////////////////////////////
PX_Object *PX_Designer_PushButtonCreate(px_memorypool *mp,PX_Object *pparent,px_float x,px_float y,px_float width,px_float height,PX_FontModule *fm)
{
	return PX_Object_PushButtonCreate(mp,pparent,(px_int)x,(px_int)y,128,64,"PushButton",fm);
}



//////////////////////////////////////////////////////////////////////////
//radiobox
//////////////////////////////////////////////////////////////////////////
PX_Object *PX_Designer_RadioBoxCreate(px_memorypool *mp,PX_Object *pparent,px_float x,px_float y,px_float width,px_float height,PX_FontModule *fm)
{
	return PX_Object_RadioBoxCreate(mp,pparent,(px_int)x,(px_int)y,128,64,"RadioBox",fm);
}


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
	do 
	{
		PX_Designer_ObjectDesc spushbutton;
		PX_memset(&spushbutton,0,sizeof(spushbutton));
		PX_strcat(spushbutton.Name,"pushbutton");
		spushbutton.createfunc=PX_Designer_PushButtonCreate;
		spushbutton.type=PX_DESIGNER_OBJECT_TYPE_STATIC;
		
		PX_strcat(spushbutton.properties[0].Name,"x");
		spushbutton.properties[0].getfloat=PX_Designer_GetX;
		spushbutton.properties[0].setfloat=PX_Designer_SetX;
		
		PX_strcat(spushbutton.properties[1].Name,"y");
		spushbutton.properties[1].getfloat=PX_Designer_GetY;
		spushbutton.properties[1].setfloat=PX_Designer_SetY;

		PX_strcat(spushbutton.properties[2].Name,"width");
		spushbutton.properties[2].getfloat=PX_Designer_GetWidth;
		spushbutton.properties[2].setfloat=PX_Designer_SetWidth;

		PX_strcat(spushbutton.properties[3].Name,"height");
		spushbutton.properties[3].getfloat=PX_Designer_GetHeight;
		spushbutton.properties[3].setfloat=PX_Designer_SetHeight;

		PX_VectorPushback(&designer->ObjectDesc, &spushbutton);

	} while (0);
	return PX_TRUE;
}

PX_Object * PX_DesignerCreate(px_memorypool *mp,PX_Object *pparent,px_int surface_width,int surface_height,int world_height,px_int world_width,PX_FontModule *fm)
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
	


	if (!PX_VectorInitialize(mp, &pdesc->ObjectDesc, sizeof(PX_Designer_ObjectDesc), 16))return PX_FALSE;
	if (!PX_VectorInitialize(mp, &pdesc->Objects, sizeof(PX_Designer_Object), 16))return PX_FALSE;
	pdesc->mp=mp;
	pdesc->fm=fm;
	pdesc->menu=PX_Object_MenuCreate(mp,pObject,0,0,surface_width-1,fm);
	if(!pdesc->menu)return PX_NULL;
	pdesc->messagebox=PX_Object_MessageBoxCreate(mp,pObject,fm);
	if(!pdesc->messagebox)return PX_NULL;
	if(!PX_WorldInitialize(mp,&pdesc->world,world_width,world_height,surface_width,surface_height,1024*1024))return PX_NULL;
	pdesc->static_root=PX_ObjectCreate(mp,0,0,0,0,0,0,0);
	pdesc->list_ObjectDesc=PX_Object_ListCreate(mp,PX_Object_WidgetGetRoot(pdesc->widget_controllers),0,1,
		(px_int)PX_Object_GetWidget(pdesc->widget_controllers)->renderTarget.width-1,
		(px_int)PX_Object_GetWidget(pdesc->widget_controllers)->renderTarget.height-1,
		PX_DESIGNER_CONTROLLER_ITEM_HEIGHT,
		PX_Designer_ControllersListItemOnCreate,
		pObject
		);
	pdesc->designerbox=PX_Object_DesignerBoxCreate(mp,pObject,0,0,100,100);
	pdesc->designerbox->Visible=PX_FALSE;

	if (!PX_DesignerStandardInitialize(pdesc))
	{
		return PX_FALSE;
	}

	PX_ObjectRegisterEvent(pObject,PX_OBJECT_EVENT_CURSORDOWN,PX_Designer_OnCursorDown,pObject);
	return pObject;
}
