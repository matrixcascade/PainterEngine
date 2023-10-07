#include "PX_Object_Designer.h"

static px_void PX_Designer_RefreshProperties(PX_Object *pObject)
{
	px_int i;
	PX_Object_Designer *pDesignerDesc=PX_ObjectGetDesc(PX_Object_Designer,pObject);

	for (i=0;i<PX_COUNTOF(pDesignerDesc->edit_propertys);i++)
	{
		PX_ObjectSetVisible(pDesignerDesc->edit_propertys[i],PX_FALSE);
		PX_ObjectSetVisible(pDesignerDesc->label_propertys[i],PX_FALSE);
	}

	if (PX_VectorCheckIndex(&pDesignerDesc->Objects,pDesignerDesc->selectObjectIndex))
	{
		PX_Designer_Object *pdobject=PX_VECTORAT(PX_Designer_Object,&pDesignerDesc->Objects,pDesignerDesc->selectObjectIndex);
		if (PX_VectorCheckIndex(&pDesignerDesc->ObjectDesc,pdobject->descIndex))
		{
			px_int j;
			PX_Designer_ObjectDesc *pObjectDesc=PX_VECTORAT(PX_Designer_ObjectDesc,&pDesignerDesc->ObjectDesc,pdobject->descIndex);
			for (j=0;j<PX_COUNTOF(pObjectDesc->properties);j++)
			{
				if (!pObjectDesc->properties[j].Name[0])
				{
					break;
				}
				PX_ObjectSetVisible(pDesignerDesc->label_propertys[j],PX_TRUE);
				PX_ObjectSetVisible(pDesignerDesc->edit_propertys[j],PX_TRUE);
				PX_Object_LabelSetText(pDesignerDesc->label_propertys[j],pObjectDesc->properties[j].Name);

				if(pObjectDesc->properties[j].getbool)
				{
					px_bool v=pObjectDesc->properties[j].getbool(pdobject->pObject);
					if (v)
					{
						PX_Object_EditSetText(pDesignerDesc->edit_propertys[j],"1");
					}
					else
					{
						PX_Object_EditSetText(pDesignerDesc->edit_propertys[j],"0");
					}
					continue;
				}

				if(pObjectDesc->properties[j].getfloat)
				{
					px_float v=pObjectDesc->properties[j].getfloat(pdobject->pObject);
					px_char content[32];
					PX_ftoa(v,content,32,2);
					if (v)
					{
						PX_Object_EditSetText(pDesignerDesc->edit_propertys[j],content);
					}
					else
					{
						PX_Object_EditSetText(pDesignerDesc->edit_propertys[j],content);
					}
					continue;
				}
				if(pObjectDesc->properties[j].getint)
				{
					px_int v=pObjectDesc->properties[j].getint(pdobject->pObject);
					px_char content[32];
					PX_itoa(v,content,32,10);
					PX_Object_EditSetText(pDesignerDesc->edit_propertys[j],content);
					continue;
				}

				if(pObjectDesc->properties[j].getstring)
				{
					px_string str;
					PX_StringInitialize(pDesignerDesc->mp, &str);
					;
					if (pObjectDesc->properties[j].getstring(pdobject->pObject,&str))
					{
						PX_Object_EditSetText(pDesignerDesc->edit_propertys[j],str.buffer);
					}
					else
					{
						PX_Object_EditSetText(pDesignerDesc->edit_propertys[j],"");
					}

					PX_StringFree(&str);

					continue;
				}
			}
		}
	}
	PX_Object_ScrollAreaUpdateRange(pDesignerDesc->widget_propertyscrollarea);
}
static px_void PX_Designer_UpdateProperties(PX_Object *pObject)
{

	PX_Object_Designer *pDesignerDesc=PX_ObjectGetDesc(PX_Object_Designer,pObject);

	if (PX_VectorCheckIndex(&pDesignerDesc->Objects,pDesignerDesc->selectObjectIndex))
	{
		PX_Designer_Object *pdobject=PX_VECTORAT(PX_Designer_Object,&pDesignerDesc->Objects,pDesignerDesc->selectObjectIndex);
		if (PX_VectorCheckIndex(&pDesignerDesc->ObjectDesc,pdobject->descIndex))
		{
			px_int j;
			PX_Designer_ObjectDesc *pObjectDesc=PX_VECTORAT(PX_Designer_ObjectDesc,&pDesignerDesc->ObjectDesc,pdobject->descIndex);
			for (j=0;j<PX_COUNTOF(pObjectDesc->properties);j++)
			{
				if (!pObjectDesc->properties[j].Name[0])
				{
					break;
				}

				if(pObjectDesc->properties[j].setbool)
				{
					const px_char *pContent=PX_Object_EditGetText(pDesignerDesc->edit_propertys[j]);;
					if (pContent[0]=='0')
					{
						pObjectDesc->properties[j].setbool(pdobject->pObject,PX_FALSE);
					}
					else
					{
						pObjectDesc->properties[j].setbool(pdobject->pObject,PX_TRUE);
					}
					continue;
				}

				if(pObjectDesc->properties[j].setfloat)
				{
					const px_char *pContent=PX_Object_EditGetText(pDesignerDesc->edit_propertys[j]);;

					pObjectDesc->properties[j].setfloat(pdobject->pObject,PX_atof(pContent));

					continue;
				}

				if(pObjectDesc->properties[j].setint)
				{
					const px_char *pContent=PX_Object_EditGetText(pDesignerDesc->edit_propertys[j]);;
					pObjectDesc->properties[j].setint(pdobject->pObject,PX_atoi(pContent));
					continue;
				}

				if(pObjectDesc->properties[j].setstring)
				{
					const px_char *pContent=PX_Object_EditGetText(pDesignerDesc->edit_propertys[j]);;
					pObjectDesc->properties[j].setstring(pdobject->pObject,pContent);
					continue;
				}
			}
		}
	}
}
static px_void PX_Designer_UpdateObjectsList(PX_Object* pObject)
{
	px_int i;
	PX_Object_Designer* pDesignerDesc = PX_ObjectGetDesc(PX_Object_Designer, pObject);
	PX_Object_ListClear(pDesignerDesc->list_Objects);
	for (i = 0; i < pDesignerDesc->Objects.size; i++)
	{
		PX_Object_ListAdd(pDesignerDesc->list_Objects,PX_VECTORAT(PX_Designer_Object,&pDesignerDesc->Objects,i));
	}

}
px_void PX_DesignerOnDesignerBoxValueChanged(PX_Object *pObject,PX_Object_Event e,px_void *ptr)
{
	PX_Object_DesignerBox *pDesignerBox=(PX_Object_DesignerBox *)pObject->pObjectDesc;
	PX_Object_Designer *pDesc=(PX_Object_Designer *)((PX_Object *)ptr)->pObjectDesc;

	if (pDesc->selectObjectIndex>=0&&pDesc->selectObjectIndex<pDesc->Objects.size)
	{
		PX_Designer_Object *pdobject=PX_VECTORAT(PX_Designer_Object,&pDesc->Objects,pDesc->selectObjectIndex);

		switch (pdobject->type)
		{
		case PX_DESIGNER_OBJECT_TYPE_UI:
		{
			if (!pDesc->pLinkObject)
			{
				return;
			}
			pdobject->pObject->x = pDesc->designerbox->x;
			pdobject->pObject->y = pDesc->designerbox->y;
			pdobject->pObject->Width = pDesc->designerbox->Width;
			pdobject->pObject->Height = pDesc->designerbox->Height;
		}
		break;
		case PX_DESIGNER_OBJECT_TYPE_GAME:
		{
			if (!pDesc->pLinkWorld)
			{
				return;
			}

			pdobject->pObject->x = pDesc->designerbox->x+pDesc->pLinkWorld->offsetx+ pDesc->designerbox->Width/2;
			pdobject->pObject->y = pDesc->designerbox->y + pDesc->pLinkWorld->offsety+ pDesc->designerbox->Height/2;
			pdobject->pObject->Width = pDesc->designerbox->Width;
			pdobject->pObject->Height = pDesc->designerbox->Height;
		}
		break;
		default:
			return;
		}
		

		PX_Designer_RefreshProperties((PX_Object *)ptr);

	}
}
px_void PX_DesignerUpdate(PX_Object *pObject,px_dword elapsed)
{
	px_int i,count;
	PX_Object_Designer *pDesc=(PX_Object_Designer *)pObject->pObjectDesc;

	//////////////////////////////////////////////////////////////////////////
	//designer box
	if (pDesc->selectObjectIndex!=-1)
	{
		PX_Designer_Object *pdobject=PX_VECTORAT(PX_Designer_Object,&pDesc->Objects,pDesc->selectObjectIndex);
		
		switch (pdobject->type)
		{
		case PX_DESIGNER_OBJECT_TYPE_UI:
		{
			if (!pDesc->pLinkObject)
			{
				return;
			}
			pDesc->designerbox->x = pdobject->pObject->x;
			pDesc->designerbox->y = pdobject->pObject->y;
			pDesc->designerbox->Width = pdobject->pObject->Width;
			pDesc->designerbox->Height = pdobject->pObject->Height;
			pDesc->designerbox->Visible = PX_TRUE;
		}
		break;
		case PX_DESIGNER_OBJECT_TYPE_GAME:
		{
			if (!pDesc->pLinkWorld)
			{
				return;
			}
			pDesc->designerbox->x = pdobject->pObject->x-pDesc->pLinkWorld->offsetx- pdobject->pObject->Width/2;
			pDesc->designerbox->y = pdobject->pObject->y - pDesc->pLinkWorld->offsety- pdobject->pObject->Height/2;
			pDesc->designerbox->Width = pdobject->pObject->Width;
			pDesc->designerbox->Height = pdobject->pObject->Height;
			PX_ObjectSetVisible(pDesc->designerbox,PX_TRUE);
		}
		break;
		default:
			break;
		}
		
	}
	else
	{
		PX_ObjectSetVisible(pDesc->designerbox,PX_FALSE);
	}

	//////////////////////////////////////////////////////////////////////////
	//delete button
	if (pDesc->selectObjectIndex!=-1)
	{
		pDesc->button_delete->x=pDesc->designerbox->x+ pDesc->designerbox->Width-pDesc->button_delete->Width;
		pDesc->button_delete->y= pDesc->designerbox->y-pDesc->button_delete->Height-8;
		PX_ObjectSetVisible(pDesc->button_delete,PX_TRUE);
	}
	else
	{
		PX_ObjectSetVisible(pDesc->button_delete,PX_FALSE);
	}

	//////////////////////////////////////////////////////////////////////////
	//properties
	if (pDesc->selectObjectIndex==-1)
	{
		px_int i;
		for (i=0;i<PX_DESIGNER_MAX_PROPERTYS;i++)
		{
			PX_ObjectSetVisible(pDesc->label_propertys[i],PX_FALSE);
			PX_ObjectSetVisible(pDesc->edit_propertys[i],PX_FALSE);
		}
	}
	

	count= pDesc->ObjectDesc.size;
	PX_Object_ListClear(pDesc->list_ObjectDesc);
	for (i=0;i<count;i++)
	{
		PX_Designer_ObjectDesc*pController=PX_VECTORAT(PX_Designer_ObjectDesc,&pDesc->ObjectDesc,i);
		PX_Object_ListAdd(pDesc->list_ObjectDesc, pController);
	}

}

px_void PX_Designer_ControllersListItemOnRender(px_surface* psurface,PX_Object *pObject, px_dword elapsed)
{
	PX_Designer_ObjectDesc*pDesc=(PX_Designer_ObjectDesc*)PX_Object_ListItemGetData(pObject);
	PX_Object_Designer *pDesignerDesc=PX_ObjectGetDesc(PX_Object_Designer,(PX_Object *)pObject->User_ptr);
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

	switch (pDesc->type)
	{
	case PX_DESIGNER_OBJECT_TYPE_UI:
		PX_GeoDrawBorder(psurface,(px_int)objx+1,(px_int)objy+1,(px_int)objx+64,(px_int)objy+PX_DESIGNER_CONTROLLER_ITEM_HEIGHT-2,1,PX_COLOR(255,192,64,255));
		PX_FontModuleDrawText(psurface,pDesignerDesc->fm,(px_int)objx+32,(px_int)objy+PX_DESIGNER_CONTROLLER_ITEM_HEIGHT/2,PX_ALIGN_CENTER,"UI",PX_COLOR(255,192,64,255));
		PX_FontModuleDrawText(psurface, pDesignerDesc->fm, (px_int)objx + 68, (px_int)objy + PX_DESIGNER_CONTROLLER_ITEM_HEIGHT / 2, PX_ALIGN_LEFTMID, pDesc->Name, PX_COLOR(255, 192, 64, 255));
		break;
	case PX_DESIGNER_OBJECT_TYPE_GAME:
		PX_GeoDrawBorder(psurface,(px_int)objx+1,(px_int)objy+1,(px_int)objx+64,(px_int)objy+PX_DESIGNER_CONTROLLER_ITEM_HEIGHT-2,1,PX_COLOR(255,128,255,64));
		PX_FontModuleDrawText(psurface,pDesignerDesc->fm,(px_int)objx+32,(px_int)objy+PX_DESIGNER_CONTROLLER_ITEM_HEIGHT/2,PX_ALIGN_CENTER,"Game",PX_COLOR(255,128,255,64));
		PX_FontModuleDrawText(psurface, pDesignerDesc->fm, (px_int)objx + 68, (px_int)objy + PX_DESIGNER_CONTROLLER_ITEM_HEIGHT / 2, PX_ALIGN_LEFTMID, pDesc->Name, PX_COLOR(255, 128, 255, 64));
		break;
	case PX_DESIGNER_OBJECT_TYPE_FUNCTION:
		PX_GeoDrawBorder(psurface,(px_int)objx+1,(px_int)objy+1,(px_int)objx+64,(px_int)objy+PX_DESIGNER_CONTROLLER_ITEM_HEIGHT-2,1,PX_COLOR(255,255,128,64));
		PX_FontModuleDrawText(psurface,pDesignerDesc->fm,(px_int)objx+32,(px_int)objy+PX_DESIGNER_CONTROLLER_ITEM_HEIGHT/2,PX_ALIGN_CENTER,"Function",PX_COLOR(255,255,128,64));
		PX_FontModuleDrawText(psurface, pDesignerDesc->fm, (px_int)objx + 68, (px_int)objy + PX_DESIGNER_CONTROLLER_ITEM_HEIGHT / 2, PX_ALIGN_LEFTMID, pDesc->Name, PX_COLOR(255, 255, 128, 64));
		break;
	}

	
}

px_bool PX_Designer_ControllersListItemOnCreate(px_memorypool* mp, PX_Object* ItemObject, px_void* userptr)
{
	ItemObject->Func_ObjectRender = PX_Designer_ControllersListItemOnRender;
	ItemObject->User_ptr = userptr;
	return PX_TRUE;
}


px_void PX_Designer_ObjectsListItemOnRender(px_surface* psurface, PX_Object* pObject, px_dword elapsed)
{
	PX_Designer_Object* pDesc = (PX_Designer_Object*)PX_Object_ListItemGetData(pObject);
	PX_Object_Designer* pDesignerDesc = PX_ObjectGetDesc(PX_Object_Designer, (PX_Object*)pObject->User_ptr);
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

	switch (pDesc->type)
	{
	case PX_DESIGNER_OBJECT_TYPE_UI:
		PX_GeoDrawBorder(psurface, (px_int)objx + 1, (px_int)objy + 1, (px_int)objx + 64, (px_int)objy + PX_DESIGNER_CONTROLLER_ITEM_HEIGHT - 2, 1, PX_COLOR(255, 192, 64, 255));
		PX_FontModuleDrawText(psurface, pDesignerDesc->fm, (px_int)objx + 32, (px_int)objy + PX_DESIGNER_CONTROLLER_ITEM_HEIGHT / 2, PX_ALIGN_CENTER, "UI", PX_COLOR(255, 192, 64, 255));
		PX_FontModuleDrawText(psurface, pDesignerDesc->fm, (px_int)objx + 68, (px_int)objy + PX_DESIGNER_CONTROLLER_ITEM_HEIGHT / 2, PX_ALIGN_LEFTMID, pDesc->pObject->id, PX_COLOR(255, 192, 64, 255));
		break;
	case PX_DESIGNER_OBJECT_TYPE_GAME:
		PX_GeoDrawBorder(psurface, (px_int)objx + 1, (px_int)objy + 1, (px_int)objx + 64, (px_int)objy + PX_DESIGNER_CONTROLLER_ITEM_HEIGHT - 2, 1, PX_COLOR(255, 128, 255, 64));
		PX_FontModuleDrawText(psurface, pDesignerDesc->fm, (px_int)objx + 32, (px_int)objy + PX_DESIGNER_CONTROLLER_ITEM_HEIGHT / 2, PX_ALIGN_CENTER, "Game", PX_COLOR(255, 128, 255, 64));
		PX_FontModuleDrawText(psurface, pDesignerDesc->fm, (px_int)objx + 68, (px_int)objy + PX_DESIGNER_CONTROLLER_ITEM_HEIGHT / 2, PX_ALIGN_LEFTMID, pDesc->pObject->id, PX_COLOR(255, 128, 255, 64));
		break;
	case PX_DESIGNER_OBJECT_TYPE_FUNCTION:
		PX_GeoDrawBorder(psurface, (px_int)objx + 1, (px_int)objy + 1, (px_int)objx + 64, (px_int)objy + PX_DESIGNER_CONTROLLER_ITEM_HEIGHT - 2, 1, PX_COLOR(255, 255, 128, 64));
		PX_FontModuleDrawText(psurface, pDesignerDesc->fm, (px_int)objx + 32, (px_int)objy + PX_DESIGNER_CONTROLLER_ITEM_HEIGHT / 2, PX_ALIGN_CENTER, "Function", PX_COLOR(255, 255, 128, 64));
		PX_FontModuleDrawText(psurface, pDesignerDesc->fm, (px_int)objx + 68, (px_int)objy + PX_DESIGNER_CONTROLLER_ITEM_HEIGHT / 2, PX_ALIGN_LEFTMID, pDesc->pObject->id, PX_COLOR(255, 255, 128, 64));
		break;
	}


}

px_bool PX_Designer_ObjectsListItemOnCreate(px_memorypool* mp, PX_Object* ItemObject, px_void* userptr)
{
	ItemObject->Func_ObjectRender = PX_Designer_ObjectsListItemOnRender;
	ItemObject->User_ptr = userptr;
	return PX_TRUE;
}
px_void PX_DesignerRender(px_surface* psurface,PX_Object *pObject, px_dword elapsed)
{
	PX_Object_Designer*pDesc=PX_ObjectGetDesc(PX_Object_Designer,pObject);
	PX_DesignerUpdate(pObject, elapsed);
	if (pDesc->pLinkWorld&&pDesc->showsliderbar)
	{
		if (pDesc->pLinkWorld->world_width > pDesc->pLinkWorld->surface_width)
		{
			pDesc->world_hscroll->Visible = PX_TRUE;
			pDesc->world_hscroll->x = 0;
			pDesc->world_hscroll->y = pDesc->pLinkWorld->surface_height - 24.f;
			pDesc->world_hscroll->Height = 24;
			pDesc->world_hscroll->Width = pDesc->pLinkWorld->surface_width-24.f;
		}
		else
		{
			pDesc->world_hscroll->Visible = PX_FALSE;
		}

		if (pDesc->pLinkWorld->world_height > pDesc->pLinkWorld->surface_height)
		{
			pDesc->world_vscroll->Visible = PX_TRUE;
			pDesc->world_vscroll->x = pDesc->pLinkWorld->surface_width - 24.f;
			pDesc->world_vscroll->y = 0;
			pDesc->world_vscroll->Width = 24;
			pDesc->world_vscroll->Height = pDesc->pLinkWorld->surface_height - 24.f;
		}
		else
		{
			pDesc->world_vscroll->Visible = PX_FALSE;
		}

	}
	else
	{
		pDesc->world_hscroll->Visible = PX_FALSE;
		pDesc->world_vscroll->Visible = PX_FALSE;
	}

	
}


px_void PX_Designer_OnPropertyChanged(PX_Object *pObject,PX_Object_Event e,px_void *ptr)
{
	PX_Object_Designer *pDesignerDesc=PX_ObjectGetDesc(PX_Object_Designer,(PX_Object *)ptr);
	PX_Designer_UpdateProperties((PX_Object *)ptr);
}

px_void PX_Designer_OnObjectsListChanged(PX_Object* pObject, PX_Object_Event e, px_void* ptr)
{
	PX_Object_Designer* pDesignerDesc = PX_ObjectGetDesc(PX_Object_Designer, (PX_Object*)ptr);
	pDesignerDesc->selectObjectIndex=PX_Object_ListGetCurrentSelectIndex(pDesignerDesc->list_Objects);
	PX_Designer_RefreshProperties((PX_Object *)ptr);
}

px_void PX_Designer_OnPropertyRefresh(PX_Object *pObject,PX_Object_Event e,px_void *ptr)
{
	PX_Object_Designer *pDesignerDesc=PX_ObjectGetDesc(PX_Object_Designer,(PX_Object *)ptr);
	PX_Designer_RefreshProperties((PX_Object *)ptr);
}

px_void PX_Designer_OnDeleteCurrentObject(PX_Object *pObject,PX_Object_Event e,px_void *ptr)
{
	PX_Object_Designer *pDesignerDesc=PX_ObjectGetDesc(PX_Object_Designer,(PX_Object *)ptr);
	if (PX_VectorCheckIndex(&pDesignerDesc->Objects,pDesignerDesc->selectObjectIndex))
	{
		PX_Designer_Object *pdobject=PX_VECTORAT(PX_Designer_Object,&pDesignerDesc->Objects,pDesignerDesc->selectObjectIndex);

		switch(pdobject->type)
		{
		case PX_DESIGNER_OBJECT_TYPE_UI:
			{
			if (!pDesignerDesc->pLinkObject)
			{
				PX_ASSERT();
				return;
			}
				PX_ObjectDelete(pdobject->pObject);
			}
			break;
		case PX_DESIGNER_OBJECT_TYPE_GAME:
			{
				if (!pDesignerDesc->pLinkWorld)
				{
					PX_ASSERT();
					return;
				}
				PX_WorldRemoveObject(pDesignerDesc->pLinkWorld,pdobject->pObject);
			}
		default:
			break;
		}
		PX_VectorErase(&pDesignerDesc->Objects,pDesignerDesc->selectObjectIndex);
	}
	pDesignerDesc->selectObjectIndex=-1;
	PX_Designer_UpdateObjectsList((PX_Object*)ptr);
}

px_void PX_Designer_OnCursorDown(PX_Object *pObject,PX_Object_Event e,px_void *ptr)
{
	PX_Object_Designer *pDesignerDesc=PX_ObjectGetDesc(PX_Object_Designer,pObject);
	PX_Object *pSelectObject=PX_NULL;
	px_int selectObjectIndex=-1;

	if (e.Event==PX_OBJECT_EVENT_CURSORDOWN)
	{
		px_int i;
		px_float x,y;
		x=PX_Object_Event_GetCursorX(e);
		y=PX_Object_Event_GetCursorY(e);

		if (pDesignerDesc->selectObjectIndex!=-1)
		{
			if (PX_isXYInRegion(x,y,pDesignerDesc->designerbox->x-6,pDesignerDesc->designerbox->y-6,pDesignerDesc->designerbox->Width+12,pDesignerDesc->designerbox->Height+12))
			{
				return;
			}

			if (PX_ObjectIsCursorInRegion(pDesignerDesc->button_delete,e))
			{
				return;
			}
		}

		if (pDesignerDesc->world_vscroll->Visible)
		{
			if (PX_ObjectIsCursorInRegion(pDesignerDesc->world_vscroll, e))
			{
				return;
			}
		}

		if (pDesignerDesc->world_hscroll->Visible)
		{
			if (PX_ObjectIsCursorInRegion(pDesignerDesc->world_hscroll, e))
			{
				return;
			}
		}


		for (i=0;i<pDesignerDesc->Objects.size;i++)
		{
			PX_Designer_Object *pdobject=PX_VECTORAT(PX_Designer_Object,&pDesignerDesc->Objects,i);
			
			if (pDesignerDesc->pLinkWorld&&pdobject->type==PX_DESIGNER_OBJECT_TYPE_GAME)
			{
				px_rect rect;
				rect.x=pdobject->pObject->x-pDesignerDesc->pLinkWorld->offsetx- pdobject->pObject->Width/2;
				rect.y=pdobject->pObject->y-pDesignerDesc->pLinkWorld->offsety- pdobject->pObject->Height/2;
				rect.width=pdobject->pObject->Width;
				rect.height=pdobject->pObject->Height;
				if(PX_isPointInRect(PX_POINT(x,y,0),rect))
				{
					selectObjectIndex=i;
				}
			}
			
			if (pDesignerDesc->pLinkObject && pdobject->type==PX_DESIGNER_OBJECT_TYPE_UI)
			{
				if(PX_ObjectIsCursorInRegion(pdobject->pObject,e))
				{
					selectObjectIndex=i;
				}
			}

		}


		if (selectObjectIndex==-1&&pDesignerDesc->selectObjectIndex==-1)
		{
				px_int selectDescIndex=PX_Object_ListGetCurrentSelectIndex(pDesignerDesc->list_ObjectDesc);
				PX_Designer_Object dobject;
				if (selectDescIndex!=-1)
				{
					//create new object
					PX_Designer_ObjectDesc *pObjectDesc=PX_VECTORAT(PX_Designer_ObjectDesc,&pDesignerDesc->ObjectDesc,selectDescIndex);

					switch (pObjectDesc->type)
					{
					case PX_DESIGNER_OBJECT_TYPE_UI:
						{
						if (pDesignerDesc->pLinkObject)
						{
							PX_Object* pNewObject = pObjectDesc->createfunc(pDesignerDesc->mp, pDesignerDesc->pLinkObject, x, y, 100, 100, pDesignerDesc->userptr);
							PX_sprintf2(pNewObject->id, sizeof(pNewObject->id), "%1%2", PX_STRINGFORMAT_STRING(pObjectDesc->Name), PX_STRINGFORMAT_INT(pDesignerDesc->allocID++));

							dobject.pObject = pNewObject;
							dobject.type = PX_DESIGNER_OBJECT_TYPE_UI;
							dobject.descIndex = selectDescIndex;
							PX_VectorPushback(&pDesignerDesc->Objects, &dobject);
							PX_Designer_UpdateObjectsList(pObject);
						}
						}
						break;
					case PX_DESIGNER_OBJECT_TYPE_GAME:
						{
						if (pDesignerDesc->pLinkWorld)
						{
							PX_Object* pNewObject = pObjectDesc->createfunc(pDesignerDesc->mp, PX_NULL, x + pDesignerDesc->pLinkWorld->offsetx, y + pDesignerDesc->pLinkWorld->offsety, 100, 100, pDesignerDesc->userptr);
							PX_sprintf2(pNewObject->id, sizeof(pNewObject->id), "%1%2", PX_STRINGFORMAT_STRING(pObjectDesc->Name), PX_STRINGFORMAT_INT(pDesignerDesc->allocID++));

							PX_WorldAddObject(pDesignerDesc->pLinkWorld, pNewObject);
							dobject.pObject = pNewObject;
							dobject.type = PX_DESIGNER_OBJECT_TYPE_GAME;
							dobject.descIndex = selectDescIndex;
							PX_VectorPushback(&pDesignerDesc->Objects, &dobject);
							PX_Designer_UpdateObjectsList(pObject);
						}
						}
						break;
					case PX_DESIGNER_OBJECT_TYPE_FUNCTION:
						{
							PX_Object *pNewObject=pObjectDesc->createfunc(pDesignerDesc->mp,PX_NULL,x,y,100,100,pDesignerDesc->userptr);
							if (pNewObject!=PX_NULL)
							{
								PX_ASSERT();
							}
						}
						break;
					default:
						break;
					}

				}
			
		}
		
		if (selectObjectIndex!=-1)
		{
			PX_Object_ListSetCurrentSelectIndex(pDesignerDesc->list_Objects,selectObjectIndex);
			if (pDesignerDesc->selectObjectIndex!=selectObjectIndex)
			{
				PX_Designer_Object *pdobject=PX_VECTORAT(PX_Designer_Object,&pDesignerDesc->Objects,selectObjectIndex);
				PX_Object_DesignerBox *pDesignerBox=PX_ObjectGetDesc(PX_Object_DesignerBox,pDesignerDesc->designerbox);

				pDesignerDesc->selectObjectIndex=selectObjectIndex;

				switch (pdobject->type)
				{
				case PX_DESIGNER_OBJECT_TYPE_UI:
					pDesignerDesc->designerbox->x = pdobject->pObject->x;
					pDesignerDesc->designerbox->y = pdobject->pObject->y;
					pDesignerDesc->designerbox->Width = pdobject->pObject->Width;
					pDesignerDesc->designerbox->Height = pdobject->pObject->Height;
					break;
				case PX_DESIGNER_OBJECT_TYPE_GAME:
				case PX_DESIGNER_OBJECT_TYPE_FUNCTION:
					pDesignerDesc->designerbox->x = pdobject->pObject->x-pDesignerDesc->pLinkWorld->offsetx- pdobject->pObject->Width/2;
					pDesignerDesc->designerbox->y = pdobject->pObject->y - pDesignerDesc->pLinkWorld->offsety - pdobject->pObject->Height/2;
					pDesignerDesc->designerbox->Width = pdobject->pObject->Width;
					pDesignerDesc->designerbox->Height = pdobject->pObject->Height;
					break;
				}
				pDesignerBox->bselect=PX_Object_DesignerBox_bselect_center;
				pDesignerBox->lastx = x;
				pDesignerBox->lasty = y;

				PX_Designer_RefreshProperties(pObject);
			}
		}
		else
		{
			pDesignerDesc->selectObjectIndex=-1;
		}

	}

	
}
//////////////////////////////////////////////////////////////////////////
/*
typedef PX_Object *(*px_designer_createfunc)(px_memorypool *mp,PX_Object *pparent,px_float x,px_float y,px_float width,px_float height,PX_FontModule *fm);
typedef px_float (*px_designer_getproperty_float)(PX_Object *pObject);
typedef px_int (*px_designer_getproperty_int)(PX_Object *pObject);
typedef px_bool (*px_designer_getproperty_string)(PX_Object *pObject);
typedef px_bool (*px_designer_getproperty_bool)(PX_Object *pObject);

typedef px_void (*px_designer_setproperty_float)(PX_Object *pObject,px_float v);
typedef px_void (*px_designer_setproperty_int)(PX_Object *pObject,px_int v);
typedef px_void (*px_designer_setproperty_string)(PX_Object *pObject,const px_char v[]);
typedef px_void (*px_designer_setproperty_bool)(PX_Object *pObject,px_bool v);
typedef struct
*/

//////////////////////////////////////////////////////////////////////////
//Label
//////////////////////////////////////////////////////////////////////////
/*
PX_Object *PX_Designer_LabelCreate(px_memorypool *mp,PX_Object *pparent,px_float x,px_float y,px_float width,px_float height,PX_FontModule *fm)
{
	return PX_Object_LabelCreate(mp,pparent,(px_int)x,(px_int)y,128,32,"Label",fm,PX_OBJECT_UI_DEFAULT_FONTCOLOR);
}

px_void PX_Designer_LabelSetText(PX_Object *pobject,const px_char text[])
{
	PX_Object_LabelSetText(pobject,text);
}

px_bool PX_Designer_LabelGetText(PX_Object *pobject,px_string *str)
{
	return PX_StringSet(str, PX_Object_LabelGetText(pobject));
}

px_void PX_Designer_LabelSetAlign(PX_Object *pobject,px_int align)
{
	switch(align)
	{
	case		PX_ALIGN_LEFTTOP:
	case 		PX_ALIGN_MIDTOP:
	case 		PX_ALIGN_RIGHTTOP:
	case 		PX_ALIGN_LEFTMID:
	case 		PX_ALIGN_CENTER:
	case 		PX_ALIGN_RIGHTMID:
	case 		PX_ALIGN_LEFTBOTTOM:
	case 		PX_ALIGN_MIDBOTTOM:
	case 		PX_ALIGN_RIGHTBOTTOM:
		{
			PX_Object_LabelSetAlign(pobject,(PX_ALIGN)align);
		}
	default:
		break;
	}
	
}

px_int PX_Designer_LabelGetAlign(PX_Object *pobject)
{
	return PX_Object_GetLabel(pobject)->Align;
}
*/
px_void PX_DesignerOnMenuOpen(px_void *userPtr)
{
	PX_Object *pObject=(PX_Object *)userPtr;
	PX_ObjectExecuteEvent(pObject,PX_OBJECT_BUILD_EVENT(PX_OBJECT_EVENT_OPEN));
}

px_void PX_DesignerOnMenuSave(px_void* userPtr)
{
	PX_Object* pObject = (PX_Object*)userPtr;
	PX_ObjectExecuteEvent(pObject, PX_OBJECT_BUILD_EVENT(PX_OBJECT_EVENT_SAVE));
}

px_void PX_DesignerOnMenuExit(px_void *userPtr)
{
	PX_Object *pObject=(PX_Object *)userPtr;
	PX_ObjectExecuteEvent(pObject,PX_OBJECT_BUILD_EVENT(PX_OBJECT_EVENT_CLOSE));
	PX_Object_DesignerDisable(pObject);
}

px_void PX_DesignerOnMenuWindowControllers(px_void *userPtr)
{
	PX_Object *pObject=(PX_Object *)userPtr;
	PX_Object_Designer *pDesc=PX_ObjectGetDesc(PX_Object_Designer,pObject);

	if(pDesc->widget_controllers->Visible)
		PX_Object_WidgetHide(pDesc->widget_controllers);
	else
		PX_Object_WidgetShow(pDesc->widget_controllers);
}


px_void PX_DesignerOnMenuViewScrollbar(px_void* userPtr)
{
	PX_Object* pObject = (PX_Object*)userPtr;
	PX_Object_Designer* pDesc = PX_ObjectGetDesc(PX_Object_Designer, pObject);

	pDesc->showsliderbar = !pDesc->showsliderbar;
}

px_void PX_DesignerOnMenuWindowProperties(px_void *userPtr)
{
	PX_Object *pObject=(PX_Object *)userPtr;
	PX_Object_Designer *pDesc=PX_ObjectGetDesc(PX_Object_Designer,pObject);
	if(pDesc->widget_property->Visible)
		PX_Object_WidgetHide(pDesc->widget_property);
	else
		PX_Object_WidgetShow(pDesc->widget_property);
}

px_void PX_DesignerOnMenuWindowObjects(px_void* userPtr)
{
	PX_Object* pObject = (PX_Object*)userPtr;
	PX_Object_Designer* pDesc = PX_ObjectGetDesc(PX_Object_Designer, pObject);
	if (pDesc->widget_objects->Visible)
		PX_Object_WidgetHide(pDesc->widget_objects);
	else
		PX_Object_WidgetShow(pDesc->widget_objects);
}

px_void PX_Designer_OnScrollVChanged(PX_Object* pObject, PX_Object_Event e, px_void* ptr)
{
	PX_Object* pDesignerObject = (PX_Object*)ptr;
	PX_Object_Designer* pDesc = PX_ObjectGetDesc(PX_Object_Designer, pDesignerObject);
	if (pDesc->pLinkWorld)
	{
		px_point camera = pDesc->pLinkWorld->camera_offset;
		camera.y = PX_Object_SliderBarGetValue(pObject) * 1.0f;
		PX_WorldSetCamera(pDesc->pLinkWorld, camera);
	}
}

px_void PX_Designer_OnScrollHChanged(PX_Object* pObject, PX_Object_Event e, px_void* ptr)
{
	PX_Object* pDesignerObject = (PX_Object*)ptr;
	PX_Object_Designer* pDesc = PX_ObjectGetDesc(PX_Object_Designer, pDesignerObject);
	if (pDesc->pLinkWorld)
	{
		px_point camera = pDesc->pLinkWorld->camera_offset;
		camera.x = PX_Object_SliderBarGetValue(pObject) * 1.0f;
		PX_WorldSetCamera(pDesc->pLinkWorld, camera);
	}
}




px_bool PX_Object_DesignerDefaultInstall(px_vector *pdescstack)
{
	PX_Designer_ObjectDesc desc;

	desc = PX_Object_LabelDesignerInstall();
	PX_VectorPushback(pdescstack,&desc);
	
	desc = PX_Object_PushButtonDesignerInstall();
	PX_VectorPushback(pdescstack, &desc);

	desc = PX_Object_RadioBoxDesignerInstall();
	PX_VectorPushback(pdescstack, &desc);

	desc = PX_Object_CheckBoxDesignerInstall();
	PX_VectorPushback(pdescstack, &desc);

	desc = PX_Object_EditDesignerInstall();
	PX_VectorPushback(pdescstack, &desc);

	desc = PX_Object_ProcessBarDesignerInstall();
	PX_VectorPushback(pdescstack, &desc);

	desc = PX_Object_ListDesignerInstall();
	PX_VectorPushback(pdescstack, &desc);

	desc = PX_Object_SelectBarDesignerInstall();
	PX_VectorPushback(pdescstack, &desc);

	desc = PX_Object_SliderBarDesignerInstall();
	PX_VectorPushback(pdescstack, &desc);
	
	desc = PX_Object_PrinterDesignerInstall();
	PX_VectorPushback(pdescstack, &desc);

	desc = PX_Object_PianoDesignerInstall();
	PX_VectorPushback(pdescstack, &desc);
	return PX_TRUE;
}

px_void PX_Object_DesignerClear(PX_Object* pObject)
{
	PX_Object_Designer *pDesc=PX_ObjectGetDesc(PX_Object_Designer,pObject);
	if (pObject->Type!= 0x21080210)
	{
		return;
	}
	if (pDesc->pLinkObject)
	{
		PX_ObjectDeleteChilds(pDesc->pLinkObject);
		PX_VectorClear(&pDesc->Objects);
	}
	if (pDesc->pLinkWorld)
	{
		PX_WorldClear(pDesc->pLinkWorld);
	}
	PX_Designer_UpdateObjectsList(pObject);
}


px_void PX_Object_DesignerFree(PX_Object* pObject)
{
	PX_Object_Designer* pDesc = PX_ObjectGetDesc(PX_Object_Designer, pObject);
	pDesc->pLinkObject = PX_NULL;
	pDesc->pLinkWorld = PX_NULL;
	PX_Object_DesignerClear(pObject);
	PX_VectorFree(&pDesc->ObjectDesc);
	PX_VectorFree(&pDesc->Objects);
}

PX_Object * PX_Object_DesignerCreate(px_memorypool *mp,PX_Object *pparent,PX_Object *pLinkObject,PX_World *pLinkWorld,PX_FontModule *fm,px_void *userptr)
{
	px_int i;
	PX_Object *pObject;
	PX_Object_Designer desc,*pdesc;
	PX_memset(&desc,0,sizeof(desc));

	pObject=PX_ObjectCreateEx(mp,pparent,0,0,0,0,0,0,0x21080210,PX_DesignerUpdate,PX_DesignerRender, PX_Object_DesignerFree,&desc,sizeof(desc));
	pdesc=(PX_Object_Designer *)pObject->pObjectDesc;
	pdesc->pLinkWorld=pLinkWorld;
	pdesc->pLinkObject=pLinkObject;
	pdesc->showsliderbar = PX_TRUE;
	pdesc->userptr = userptr;
	pdesc->widget_property = PX_Object_WidgetCreate(mp, pObject, 5, 36 + 256, 192, 192, "properties", PX_NULL);
	if (!pdesc->widget_property)return PX_FALSE;
	if(pdesc->pLinkWorld)
		pdesc->widget_property->x = pdesc->pLinkWorld->surface_width - pdesc->widget_property->Width - 25;

	pdesc->widget_propertyscrollarea=PX_Object_ScrollAreaCreate(mp,pdesc->widget_property,0,0,192, 162);

	do 
	{
		PX_Object_Menu_Item *pItem;
		pdesc->menu=PX_Object_MenuCreate(mp,pObject,0,0,64,PX_NULL);

		pItem=PX_Object_MenuAddItem(pdesc->menu,PX_Object_MenuGetRootItem(pdesc->menu),"designer",PX_NULL,PX_NULL);
			PX_Object_MenuAddItem(pdesc->menu,pItem,"open",PX_DesignerOnMenuOpen,pObject);
			PX_Object_MenuAddItem(pdesc->menu, pItem, "save", PX_DesignerOnMenuSave, pObject);
			PX_Object_MenuAddItem(pdesc->menu,pItem,"exit",PX_DesignerOnMenuExit,pObject);

		pItem=PX_Object_MenuAddItem(pdesc->menu,PX_Object_MenuGetRootItem(pdesc->menu),"window",PX_NULL,PX_NULL);
			PX_Object_MenuAddItem(pdesc->menu,pItem,"controllers",PX_DesignerOnMenuWindowControllers,pObject);
			PX_Object_MenuAddItem(pdesc->menu,pItem,"properties",PX_DesignerOnMenuWindowProperties,pObject);
			PX_Object_MenuAddItem(pdesc->menu, pItem, "objects", PX_DesignerOnMenuWindowObjects, pObject);

		pItem = PX_Object_MenuAddItem(pdesc->menu, PX_Object_MenuGetRootItem(pdesc->menu), "view", PX_NULL, PX_NULL);
			PX_Object_MenuAddItem(pdesc->menu, pItem, "ScrollBar", PX_DesignerOnMenuViewScrollbar, pObject);

	} while (0);
	
	

	for (i=0;i<PX_DESIGNER_MAX_PROPERTYS;i++)
	{
		pdesc->label_propertys[i]=PX_Object_LabelCreate(mp,pdesc->widget_propertyscrollarea,5,3+i*24,64,24,"property",fm,PX_OBJECT_UI_DEFAULT_FONTCOLOR);
		PX_Object_LabelSetAlign(pdesc->label_propertys[i],PX_ALIGN_LEFTMID);
		pdesc->label_propertys[i]->Visible=PX_FALSE;
		pdesc->edit_propertys[i]=PX_Object_EditCreate(mp,pdesc->widget_propertyscrollarea,74,3+i*24,96,24,fm);
		PX_Object_GetEdit(pdesc->edit_propertys[i])->multiLines = PX_TRUE;
		pdesc->edit_propertys[i]->Visible=PX_FALSE;
		PX_ObjectRegisterEvent(pdesc->edit_propertys[i],PX_OBJECT_EVENT_VALUECHANGED,PX_Designer_OnPropertyChanged,pObject);
		//PX_ObjectRegisterEvent(pdesc->edit_propertys[i],PX_OBJECT_EVENT_LOSTFOCUS,PX_Designer_OnPropertyRefresh,pObject);
	}


	pdesc->widget_controllers=PX_Object_WidgetCreate(mp,pObject,5,25,192,256,"controllers",PX_NULL);
	if(!pdesc->widget_controllers)return PX_FALSE;
	if (pdesc->pLinkWorld)
		pdesc->widget_controllers->x = pdesc->pLinkWorld->surface_width - pdesc->widget_controllers->Width - 25;

	pdesc->widget_objects = PX_Object_WidgetCreate(mp, pObject, 205, 25, 192, 256, "objects", PX_NULL);
	if (!pdesc->widget_objects)return PX_FALSE;



	if (!PX_VectorInitialize(mp, &pdesc->ObjectDesc, sizeof(PX_Designer_ObjectDesc), 16))return PX_FALSE;
	if (!PX_VectorInitialize(mp, &pdesc->Objects, sizeof(PX_Designer_Object), 16))return PX_FALSE;
	pdesc->mp=mp;
	pdesc->fm=fm;
	pdesc->selectObjectIndex=-1;
	pdesc->menu=PX_Object_MenuCreate(mp,pObject,0,0,64,fm);
	if(!pdesc->menu)return PX_NULL;
	pdesc->messagebox=PX_Object_MessageBoxCreate(mp,pObject,fm);
	if(!pdesc->messagebox)return PX_NULL;

	pdesc->list_ObjectDesc=PX_Object_ListCreate(mp,PX_Object_WidgetGetRoot(pdesc->widget_controllers),0,1,
		(px_int)PX_Object_GetWidget(pdesc->widget_controllers)->renderTarget.width-1,
		(px_int)PX_Object_GetWidget(pdesc->widget_controllers)->renderTarget.height-1,
		PX_DESIGNER_CONTROLLER_ITEM_HEIGHT,
		PX_Designer_ControllersListItemOnCreate,
		pObject
		);

	pdesc->list_Objects = PX_Object_ListCreate(mp, PX_Object_WidgetGetRoot(pdesc->widget_objects), 0, 1,
		(px_int)PX_Object_GetWidget(pdesc->widget_objects)->renderTarget.width - 1,
		(px_int)PX_Object_GetWidget(pdesc->widget_objects)->renderTarget.height - 1,
		PX_DESIGNER_CONTROLLER_ITEM_HEIGHT,
		PX_Designer_ObjectsListItemOnCreate,
		pObject
	);
	PX_ObjectRegisterEvent(pdesc->list_Objects, PX_OBJECT_EVENT_VALUECHANGED, PX_Designer_OnObjectsListChanged, pObject);

	pdesc->designerbox=PX_Object_DesignerBoxCreate(mp,pObject,0,0,100,100);
	pdesc->designerbox->Visible=PX_FALSE;

	pdesc->button_delete=PX_Object_PushButtonCreate(mp,pObject,0,0,16,16,"x",PX_NULL);
	pdesc->button_delete->Visible=PX_FALSE;
	PX_ObjectRegisterEvent(pdesc->button_delete,PX_OBJECT_EVENT_EXECUTE,PX_Designer_OnDeleteCurrentObject,pObject);

	pdesc->world_hscroll = PX_Object_SliderBarCreate(mp, pObject, 0, 0, 10, 10, PX_OBJECT_SLIDERBAR_TYPE_HORIZONTAL, PX_OBJECT_SLIDERBAR_STYLE_BOX);
	
	if (pdesc->pLinkWorld&&pdesc->pLinkWorld->world_width > pdesc->pLinkWorld->surface_width)
	{
		PX_Object_SliderBarSetRange(pdesc->world_hscroll, (px_int)(pdesc->pLinkWorld->surface_width / 2) - 1, (px_int)(pdesc->pLinkWorld->world_width - pdesc->pLinkWorld->surface_width / 2) + 1);
	}
	else
	{
		pdesc->world_hscroll->Visible = PX_FALSE;
	}
	PX_ObjectRegisterEvent(pdesc->world_hscroll, PX_OBJECT_EVENT_VALUECHANGED, PX_Designer_OnScrollHChanged, pObject);


	pdesc->world_vscroll = PX_Object_SliderBarCreate(mp, pObject, 0, 0, 10, 10, PX_OBJECT_SLIDERBAR_TYPE_VERTICAL, PX_OBJECT_SLIDERBAR_STYLE_BOX);
	if (pdesc->pLinkWorld && pdesc->pLinkWorld->world_height > pdesc->pLinkWorld->surface_height)
	{
		PX_Object_SliderBarSetRange(pdesc->world_vscroll, (px_int)(pdesc->pLinkWorld->surface_height / 2) - 1, (px_int)(pdesc->pLinkWorld->world_height - pdesc->pLinkWorld->surface_height / 2) + 1);
	}
	else
	{
		pdesc->world_vscroll->Visible = PX_FALSE;
	}
	PX_ObjectRegisterEvent(pdesc->world_vscroll, PX_OBJECT_EVENT_VALUECHANGED, PX_Designer_OnScrollVChanged, pObject);


	PX_ObjectRegisterEvent(pObject,PX_OBJECT_EVENT_CURSORDOWN,PX_Designer_OnCursorDown,pObject);
	PX_ObjectRegisterEvent(pdesc->designerbox,PX_OBJECT_EVENT_VALUECHANGED,PX_DesignerOnDesignerBoxValueChanged,pObject);

	pObject->Visible = PX_FALSE;

	return pObject;
}

px_bool PX_Object_DesignerAddObjectDescription(PX_Object* pObject, PX_Designer_ObjectDesc* desc)
{
	PX_Object_Designer* pdesc=PX_ObjectGetDesc(PX_Object_Designer,pObject);
	if (pObject->Type != 0x21080210)
	{
		PX_ASSERT();
		return PX_FALSE;
	}
	return PX_VectorPushback(&pdesc->ObjectDesc, desc);
}


px_void PX_Object_DesignerResizeUpdate(PX_Object* pObject)
{
	PX_Object_Designer* pdesc;
	if (pObject->Type != 0x21080210)
	{
		PX_ASSERT();
		return;
	}
	pdesc = PX_ObjectGetDesc(PX_Object_Designer, pObject);
	if (pdesc->pLinkWorld && pdesc->pLinkWorld->world_width > pdesc->pLinkWorld->surface_width)
	{
		PX_Object_SliderBarSetRange(pdesc->world_hscroll, (px_int)(pdesc->pLinkWorld->surface_width / 2) - 1, (px_int)(pdesc->pLinkWorld->world_width - pdesc->pLinkWorld->surface_width / 2) + 1);
	}
	else
	{
		pdesc->world_hscroll->Visible = PX_FALSE;
	}


	if (pdesc->pLinkWorld && pdesc->pLinkWorld->world_height > pdesc->pLinkWorld->surface_height)
	{
		PX_Object_SliderBarSetRange(pdesc->world_vscroll, (px_int)(pdesc->pLinkWorld->surface_height / 2) - 1, (px_int)(pdesc->pLinkWorld->world_height - pdesc->pLinkWorld->surface_height / 2) + 1);
	}
	else
	{
		pdesc->world_vscroll->Visible = PX_FALSE;
	}
}


px_void PX_Object_DesignerEnable(PX_Object* pObject)
{
	if (pObject->Type== 0x21080210)
	{
		PX_ObjectSetVisible(pObject, PX_TRUE);
		PX_ObjectSetFocus(pObject);
		PX_Object_DesignerResizeUpdate(pObject);
	}
}

px_void PX_Object_DesignerBindWorld(PX_Object* pObject, PX_World* pLinkWorld)
{
	PX_Object_Designer* pdesc = PX_ObjectGetDesc(PX_Object_Designer, pObject);
	if (pObject->Type== 0x21080210)
	{
		pdesc->pLinkWorld = pLinkWorld;
		if (pdesc->pLinkWorld)
			pdesc->widget_property->x = pdesc->pLinkWorld->surface_width - pdesc->widget_property->Width - 25;
		if (pdesc->pLinkWorld)
			pdesc->widget_controllers->x = pdesc->pLinkWorld->surface_width - pdesc->widget_controllers->Width - 25;
		if (pdesc->pLinkWorld && pdesc->pLinkWorld->world_width > pdesc->pLinkWorld->surface_width)
		{
			PX_Object_SliderBarSetRange(pdesc->world_hscroll, (px_int)(pdesc->pLinkWorld->surface_width / 2) - 1, (px_int)(pdesc->pLinkWorld->world_width - pdesc->pLinkWorld->surface_width / 2) + 1);
		}
		else
		{
			pdesc->world_hscroll->Visible = PX_FALSE;
		}

	}
	return;
}

px_void PX_Object_DesignerBindRoot(PX_Object* pObject, PX_Object* root)
{
	PX_Object_Designer* pdesc = PX_ObjectGetDesc(PX_Object_Designer, pObject);
	if (pObject->Type == 0x21080210)
	{
		pdesc->pLinkObject = root;
	}
}

px_void PX_Object_DesignerDisable(PX_Object* pObject)
{
	if (pObject->Type == 0x21080210)
	{
		PX_ObjectSetVisible(pObject, PX_FALSE);
		PX_ObjectClearFocus(pObject);
	}
}



px_bool PX_Object_DesignerExport(PX_Object *pObject, px_string* pText)
{
	px_int i;
	PX_Object_Designer* designer;
	if (pObject->Type!=0x21080210)
	{
		PX_ASSERT();
		return PX_FALSE;
	}
	designer=PX_ObjectGetDesc(PX_Object_Designer,pObject);

	if (designer->Objects.size==0)
	{
		return PX_TRUE;
	}

	PX_StringCatEx(pText,"{\n");

	for (i=0;i<designer->Objects.size;i++)
	{
		PX_Designer_Object* pdobject = PX_VECTORAT(PX_Designer_Object, &designer->Objects, i);
		PX_Designer_ObjectDesc* pobjectdesc = PX_VECTORAT(PX_Designer_ObjectDesc, &designer->ObjectDesc, pdobject->descIndex);
		px_int j;
		if (!pobjectdesc)
		{
			PX_ASSERT();
			goto _ERROR;;
		}
		//pObject Name
		if(!PX_StringCatEx(pText,"\""))goto _ERROR;
		if(!PX_StringCatEx(pText,pobjectdesc->Name))goto _ERROR;
		if(!PX_StringCatEx(pText,"\":{\n"))goto _ERROR;

		//pObject Type
		switch (pdobject->type)
		{
		case PX_DESIGNER_OBJECT_TYPE_UI:
			if(!PX_StringCatEx(pText,"\"type\":\"ui\",\n"))goto _ERROR;
			break;
		case PX_DESIGNER_OBJECT_TYPE_GAME:
			if(!PX_StringCatEx(pText,"\"type\":\"game\",\n"))goto _ERROR;
				break;
		case PX_DESIGNER_OBJECT_TYPE_FUNCTION:
			PX_ASSERT();
			break;
		default:
			return PX_FALSE;
		}
		
		for (j=0;j<PX_COUNTOF(pobjectdesc->properties);j++)
		{
			if (!pobjectdesc->properties[j].Name[0])
			{
				break;
			}
			
			if (pobjectdesc->properties[j].getbool)
			{
				if (pobjectdesc->properties[j].getbool(pdobject->pObject))
				{
					if(!PX_StringCatEx(pText,"\""))goto _ERROR;
					if(!PX_StringCatEx(pText,pobjectdesc->properties[j].Name))goto _ERROR;
					if(!PX_StringCatEx(pText,"\":true"))goto _ERROR;
				}
				else
				{
					if(!PX_StringCatEx(pText,"\""))goto _ERROR;
					if(!PX_StringCatEx(pText,pobjectdesc->Name))goto _ERROR;
					if(!PX_StringCatEx(pText,"\":false"))goto _ERROR;
				}
			}
			else if (pobjectdesc->properties[j].getfloat)
			{
				PX_RETURN_STRING ret;
				if(!PX_StringCatEx(pText,"\""))goto _ERROR;
				if(!PX_StringCatEx(pText,pobjectdesc->properties[j].Name))goto _ERROR;
				if(!PX_StringCatEx(pText,"\":"))goto _ERROR;
				ret = PX_ftos(pobjectdesc->properties[j].getfloat(pdobject->pObject), 2);
				if(!PX_StringCatEx(pText,ret.data))goto _ERROR;
			}
			else if (pobjectdesc->properties[j].getint)
			{
				PX_RETURN_STRING ret;
				if(!PX_StringCatEx(pText,"\""))goto _ERROR;
				if(!PX_StringCatEx(pText,pobjectdesc->properties[j].Name))goto _ERROR;
				if(!PX_StringCatEx(pText,"\":"))goto _ERROR;
				ret = PX_itos(pobjectdesc->properties[j].getint(pdobject->pObject), 10);
				if(!PX_StringCatEx(pText,ret.data))goto _ERROR;
			}
			else if (pobjectdesc->properties[j].getstring)
			{
				px_string str;
				if(!PX_StringCatEx(pText,"\""))goto _ERROR;
				if(!PX_StringCatEx(pText,pobjectdesc->properties[j].Name))goto _ERROR;
				if(!PX_StringCatEx(pText,"\":\""))goto _ERROR;
				PX_StringInitialize(designer->mp, &str);
				pobjectdesc->properties[j].getstring(pdobject->pObject, &str);
				
				if (!PX_StringCatEx(pText,str.buffer ))
				{
					PX_StringFree(&str);
					goto _ERROR;
				}
				PX_StringFree(&str);
				if(!PX_StringCatEx(pText,"\""))goto _ERROR;
			}
			if(!PX_StringCatEx(pText,",\n"))goto _ERROR;
		}
		PX_StringBackspaceEx(pText);
		PX_StringBackspaceEx(pText);
		PX_StringCatEx(pText,"\n},\n");
	}
	PX_StringBackspaceEx(pText);
	PX_StringBackspaceEx(pText);

	PX_StringCatEx(pText,"\n}\n");

	return PX_TRUE;
_ERROR:
	PX_StringClear(pText);
	return PX_FALSE;

}
px_bool PX_Object_DesignerImportToUIObject(px_memorypool *mp,PX_Object *DesignerObject, PX_Object* pRootObject, const px_char* pText,PX_FontModule *fm)
{
	PX_Json Json;
	px_int i;
	
	PX_Object_Designer* pDesignerDesc = PX_ObjectGetDesc(PX_Object_Designer, DesignerObject);
	px_vector* pdescvec = &pDesignerDesc->ObjectDesc;
	if (!PX_JsonInitialize(mp, &Json))return PX_FALSE;
	if (!PX_JsonParse(&Json, pText))
	{
		goto _ERROR;
	}

	for (i = 0; i < Json.rootValue._object.values.size; i++)
	{
		PX_Json_Value* pJsonValue = PX_JsonGetObjectValueByIndex(&Json.rootValue, i);
		PX_Designer_ObjectDesc* pObjectDesc;
		
		px_int j, descindex = -1;
		if (!pJsonValue || pJsonValue->type != PX_JSON_VALUE_TYPE_OBJECT)
		{
			continue;
		}
		pObjectDesc = PX_NULL;
		for (j = 0; j < pdescvec->size; j++)
		{
			PX_Designer_ObjectDesc* pEnumObjectDesc = PX_VECTORAT(PX_Designer_ObjectDesc, pdescvec, j);
			if (PX_strequ(pJsonValue->name.buffer, pEnumObjectDesc->Name))
			{
				pObjectDesc = pEnumObjectDesc;
				descindex = j;
				break;
			}
		}
		if (pObjectDesc)
		{
			PX_Json_Value* TypeValue = PX_JsonGetObjectValue(pJsonValue, "type");
			PX_DESIGNER_OBJECT_TYPE type;
			PX_Object* pNewObject = PX_NULL;
			PX_Designer_Object dobject;
			if (TypeValue && TypeValue->type == PX_JSON_VALUE_TYPE_STRING)
			{
				if (PX_strequ(TypeValue->_string.buffer, "ui"))
				{
					type = PX_DESIGNER_OBJECT_TYPE_UI;
				}
				else if (PX_strequ(TypeValue->_string.buffer, "game"))
				{
					type = PX_DESIGNER_OBJECT_TYPE_GAME;
				}
				else
				{
					type = PX_DESIGNER_OBJECT_TYPE_FUNCTION;
				}

				switch (pObjectDesc->type)
				{
				case PX_DESIGNER_OBJECT_TYPE_UI:
				{
					if (pDesignerDesc->pLinkObject)
					{
						pNewObject = pObjectDesc->createfunc(pDesignerDesc->mp, pDesignerDesc->pLinkObject, 0, 0, 100, 100, pDesignerDesc->userptr);
						dobject.pObject = pNewObject;
						dobject.type = PX_DESIGNER_OBJECT_TYPE_UI;
						dobject.descIndex = descindex;
						PX_VectorPushback(&pDesignerDesc->Objects, &dobject);
						PX_Designer_UpdateObjectsList(DesignerObject);
					}
				}
				break;
				case PX_DESIGNER_OBJECT_TYPE_GAME:
				{
					if (pDesignerDesc->pLinkWorld)
					{
						pNewObject = pObjectDesc->createfunc(pDesignerDesc->mp, PX_NULL, 0 + pDesignerDesc->pLinkWorld->offsetx, 0 + pDesignerDesc->pLinkWorld->offsety, 100, 100, pDesignerDesc->userptr);
						PX_WorldAddObject(pDesignerDesc->pLinkWorld, pNewObject);
						dobject.pObject = pNewObject;
						dobject.type = PX_DESIGNER_OBJECT_TYPE_GAME;
						dobject.descIndex = descindex;
						PX_VectorPushback(&pDesignerDesc->Objects, &dobject);
						PX_Designer_UpdateObjectsList(DesignerObject);
					}
				}
				break;
				case PX_DESIGNER_OBJECT_TYPE_FUNCTION:
				{
					pNewObject = pObjectDesc->createfunc(pDesignerDesc->mp, PX_NULL, 0, 0, 100, 100, pDesignerDesc->userptr);
					if (pNewObject != PX_NULL)
					{
						PX_ASSERT();
					}
				}
				break;
				default:
					break;
				}
			}

			if (pNewObject)
			{
				for (j = 0; j < PX_COUNTOF(pObjectDesc->properties); j++)
				{
					if (pObjectDesc->properties[j].Name[0] == '\0')
					{
						break;
					}
					if (pObjectDesc->properties[j].setbool)
					{
						PX_Json_Value* pvalue = PX_JsonGetObjectValue(pJsonValue, pObjectDesc->properties[j].Name);
						if (pvalue && pvalue->type == PX_JSON_VALUE_TYPE_BOOLEAN)
						{
							pObjectDesc->properties[j].setbool(pNewObject, pvalue->_boolean);
						}
					}
					else if (pObjectDesc->properties[j].setfloat)
					{
						PX_Json_Value* pvalue = PX_JsonGetObjectValue(pJsonValue, pObjectDesc->properties[j].Name);
						if (pvalue && pvalue->type == PX_JSON_VALUE_TYPE_NUMBER)
						{
							pObjectDesc->properties[j].setfloat(pNewObject, (px_float)pvalue->_number);
						}
					}
					else if (pObjectDesc->properties[j].setint)
					{
						PX_Json_Value* pvalue = PX_JsonGetObjectValue(pJsonValue, pObjectDesc->properties[j].Name);
						if (pvalue && pvalue->type == PX_JSON_VALUE_TYPE_NUMBER)
						{
							pObjectDesc->properties[j].setint(pNewObject, (px_int)pvalue->_number);
						}
					}
					else if (pObjectDesc->properties[j].setstring)
					{
						PX_Json_Value* pvalue = PX_JsonGetObjectValue(pJsonValue, pObjectDesc->properties[j].Name);
						if (pvalue && pvalue->type == PX_JSON_VALUE_TYPE_STRING)
						{
							pObjectDesc->properties[j].setstring(pNewObject, pvalue->_string.buffer);
						}
					}
				}
			}

		}
	}
	PX_JsonFree(&Json);
	return PX_TRUE;
_ERROR:
	PX_JsonFree(&Json);
	return PX_FALSE;
}

px_bool PX_Object_DesignerImport(PX_Object* pObject, const px_char* pText)
{
	PX_Json Json;
	px_int i;
	PX_Object_Designer * pdesigner=PX_ObjectGetDesc(PX_Object_Designer,pObject);
	if(!PX_JsonInitialize(pdesigner->mp,&Json))return PX_FALSE;
	if (!PX_JsonParse(&Json,pText))
	{
		goto _ERROR;
	}
	for (i=0;i<Json.rootValue._object.values.size;i++)
	{
		PX_Json_Value *pJsonValue=PX_JsonGetObjectValueByIndex(&Json.rootValue,i);
		PX_Designer_ObjectDesc *pObjectDesc;
		px_int j,descindex=-1;
		if (!pJsonValue||pJsonValue->type!=PX_JSON_VALUE_TYPE_OBJECT)
		{
			continue;
		}
		pObjectDesc=PX_NULL;
		for (j=0;j<pdesigner->ObjectDesc.size;j++)
		{
			PX_Designer_ObjectDesc *pEnumObjectDesc=PX_VECTORAT(PX_Designer_ObjectDesc,&pdesigner->ObjectDesc,j);
			if (PX_strequ(pJsonValue->name.buffer,pEnumObjectDesc->Name))
			{
				pObjectDesc=pEnumObjectDesc;
				descindex=j;
				break;
			}
		}
		if (pObjectDesc)
		{
			PX_Json_Value *TypeValue=PX_JsonGetObjectValue(pJsonValue,"type");
			PX_DESIGNER_OBJECT_TYPE type;
			PX_Object *pNewObject=PX_NULL;
			if (TypeValue&&TypeValue->type==PX_JSON_VALUE_TYPE_STRING)
			{
				if (PX_strequ(TypeValue->_string.buffer,"ui"))
				{
					type=PX_DESIGNER_OBJECT_TYPE_UI;
				}
				else if (PX_strequ(TypeValue->_string.buffer,"game"))
				{
					type=PX_DESIGNER_OBJECT_TYPE_GAME;
				}
				else
				{
					type=PX_DESIGNER_OBJECT_TYPE_FUNCTION;
				}

				switch (type)
				{
				case PX_DESIGNER_OBJECT_TYPE_UI:
				{
					PX_Designer_Object dobject;
					pNewObject = pObjectDesc->createfunc(pdesigner->mp, pdesigner->pLinkObject, 0, 0, 100, 100, pdesigner->userptr);
					dobject.pObject = pNewObject;
					dobject.type = PX_DESIGNER_OBJECT_TYPE_UI;
					dobject.descIndex = descindex;
					PX_VectorPushback(&pdesigner->Objects, &dobject);
				}
				break;
				case PX_DESIGNER_OBJECT_TYPE_GAME:
				{
					PX_Designer_Object dobject;
					pNewObject = pObjectDesc->createfunc(pdesigner->mp, PX_NULL, 0, 0, 100, 100, pdesigner->userptr);
					PX_WorldAddObject(pdesigner->pLinkWorld, pNewObject);
					dobject.pObject = pNewObject;
					dobject.type = PX_DESIGNER_OBJECT_TYPE_GAME;
					dobject.descIndex = descindex;
					PX_VectorPushback(&pdesigner->Objects, &dobject);
				}
				break;
				default:
					break;
				}
			}

			if (pNewObject)
			{
				for(j=0;j<PX_COUNTOF(pObjectDesc->properties);j++)
				{
					if (pObjectDesc->properties[j].Name[0]=='\0')
					{
						break;
					}
					if (pObjectDesc->properties[j].setbool)
					{
						PX_Json_Value *pvalue=PX_JsonGetObjectValue(pJsonValue,pObjectDesc->properties[j].Name);
						if (pvalue&&pvalue->type==PX_JSON_VALUE_TYPE_BOOLEAN)
						{
							pObjectDesc->properties[j].setbool(pNewObject,pvalue->_boolean);
						}
					}
					else if(pObjectDesc->properties[j].setfloat)
					{
						PX_Json_Value *pvalue=PX_JsonGetObjectValue(pJsonValue,pObjectDesc->properties[j].Name);
						if (pvalue&&pvalue->type==PX_JSON_VALUE_TYPE_NUMBER)
						{
							pObjectDesc->properties[j].setfloat(pNewObject,(px_float)pvalue->_number);
						}
					}
					else if(pObjectDesc->properties[j].setint)
					{
						PX_Json_Value *pvalue=PX_JsonGetObjectValue(pJsonValue,pObjectDesc->properties[j].Name);
						if (pvalue&&pvalue->type==PX_JSON_VALUE_TYPE_NUMBER)
						{
							pObjectDesc->properties[j].setint(pNewObject,(px_int)pvalue->_number);
						}
					}
					else if(pObjectDesc->properties[j].setstring)
					{
						PX_Json_Value *pvalue=PX_JsonGetObjectValue(pJsonValue,pObjectDesc->properties[j].Name);
						if (pvalue&&pvalue->type==PX_JSON_VALUE_TYPE_STRING)
						{
							pObjectDesc->properties[j].setstring(pNewObject,pvalue->_string.buffer);
						}
					}
				}
			}
			
		}
	}
	PX_Designer_UpdateObjectsList(pObject);
	PX_JsonFree(&Json);
	return PX_TRUE;
_ERROR:
	PX_JsonFree(&Json);
	return PX_FALSE;
}
