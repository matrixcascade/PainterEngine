#include "PainterEngine_Designer.h"


static px_void PX_Designer_RefreshProperties(PX_Object *pObject)
{
	px_int i;
	PX_Designer *pDesignerDesc=PX_ObjectGetDesc(PX_Designer,pObject);

	for (i=0;i<PX_COUNTOF(pDesignerDesc->edit_propertys);i++)
	{
		pDesignerDesc->edit_propertys[i]->Visible=PX_FALSE;
		pDesignerDesc->label_propertys[i]->Visible=PX_FALSE;
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
				pDesignerDesc->label_propertys[j]->Visible=PX_TRUE;
				pDesignerDesc->edit_propertys[j]->Visible=PX_TRUE;
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
					const px_char *v=pObjectDesc->properties[j].getstring(pdobject->pObject);
					if (v)
					{
						PX_Object_EditSetText(pDesignerDesc->edit_propertys[j],v);
					}
					else
					{
						PX_Object_EditSetText(pDesignerDesc->edit_propertys[j],"");
					}
					continue;
				}
			}
		}
	}
	PX_Object_ScrollAreaUpdateRange(pDesignerDesc->widget_propertyscrollarea);
}
static px_void PX_Designer_UpdateProperties(PX_Object *pObject)
{

	PX_Designer *pDesignerDesc=PX_ObjectGetDesc(PX_Designer,pObject);

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

px_void PX_DesignerOnDesignerBoxValueChanged(PX_Object *pObject,PX_Object_Event e,px_void *ptr)
{
	PX_Object_DesignerBox *pDesignerBox=(PX_Object_DesignerBox *)pObject->pObject;
	PX_Designer *pDesc=(PX_Designer *)((PX_Object *)ptr)->pObject;

	if (pDesc->selectObjectIndex>=0&&pDesc->selectObjectIndex<pDesc->Objects.size)
	{
		PX_Designer_Object *pdobject=PX_VECTORAT(PX_Designer_Object,&pDesc->Objects,pDesc->selectObjectIndex);

		switch (pdobject->type)
		{
		case PX_DESIGNER_OBJECT_TYPE_UI:
		{
			pdobject->pObject->x = pDesc->designerbox->x;
			pdobject->pObject->y = pDesc->designerbox->y;
			pdobject->pObject->Width = pDesc->designerbox->Width;
			pdobject->pObject->Height = pDesc->designerbox->Height;
		}
		break;
		case PX_DESIGNER_OBJECT_TYPE_GAME:
		{
			pdobject->pObject->x = pDesc->designerbox->x+pDesc->pLinkworld->offsetx;
			pdobject->pObject->y = pDesc->designerbox->y + pDesc->pLinkworld->offsety;
			pdobject->pObject->Width = pDesc->designerbox->Width;
			pdobject->pObject->Height = pDesc->designerbox->Height;
		}
		break;
		default:
			break;
		}
		

		PX_Designer_RefreshProperties((PX_Object *)ptr);

	}
}


px_void PX_DesignerUpdate(PX_Object *pObject,px_dword elpased)
{
	px_int i,count;
	PX_Designer *pDesc=(PX_Designer *)pObject->pObject;

	//////////////////////////////////////////////////////////////////////////
	//designer box
	if (pDesc->selectObjectIndex!=-1)
	{
		PX_Designer_Object *pdobject=PX_VECTORAT(PX_Designer_Object,&pDesc->Objects,pDesc->selectObjectIndex);
		
		switch (pdobject->type)
		{
		case PX_DESIGNER_OBJECT_TYPE_UI:
		{
			pDesc->designerbox->x = pdobject->pObject->x;
			pDesc->designerbox->y = pdobject->pObject->y;
			pDesc->designerbox->Width = pdobject->pObject->Width;
			pDesc->designerbox->Height = pdobject->pObject->Height;
			pDesc->designerbox->Visible = PX_TRUE;
		}
		break;
		case PX_DESIGNER_OBJECT_TYPE_GAME:
		{
			pDesc->designerbox->x = pdobject->pObject->x-pDesc->pLinkworld->offsetx;
			pDesc->designerbox->y = pdobject->pObject->y - pDesc->pLinkworld->offsety;
			pDesc->designerbox->Width = pdobject->pObject->Width;
			pDesc->designerbox->Height = pdobject->pObject->Height;
			pDesc->designerbox->Visible = PX_TRUE;
		}
		break;
		default:
			break;
		}
		
	}
	else
	{
		pDesc->designerbox->Visible=PX_FALSE;
	}

	//////////////////////////////////////////////////////////////////////////
	//delete button
	if (pDesc->selectObjectIndex!=-1)
	{
		pDesc->button_delete->x=pDesc->designerbox->x+ pDesc->designerbox->Width-pDesc->button_delete->Width;
		pDesc->button_delete->y= pDesc->designerbox->y-pDesc->button_delete->Height-8;
		pDesc->button_delete->Visible=PX_TRUE;
	}
	else
	{
		pDesc->button_delete->Visible=PX_FALSE;
	}

	//////////////////////////////////////////////////////////////////////////
	//properties
	if (pDesc->selectObjectIndex==-1)
	{
		px_int i;
		for (i=0;i<PX_DESIGNER_MAX_PROPERTYS;i++)
		{
			pDesc->label_propertys[i]->Visible=PX_FALSE;
			pDesc->edit_propertys[i]->Visible=PX_FALSE;
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

px_void PX_DesignerRender(px_surface* psurface,PX_Object *pObject, px_dword elpased)
{
	PX_Designer*pDesc=PX_ObjectGetDesc(PX_Designer,pObject);
	PX_DesignerUpdate(pObject, elpased);
	if (pDesc->showsliderbar)
	{
		if (pDesc->pLinkworld->world_width > pDesc->pLinkworld->surface_width)
		{
			pDesc->world_hscroll->Visible = PX_TRUE;
			pDesc->world_hscroll->x = 0;
			pDesc->world_hscroll->y = pDesc->pLinkworld->surface_height - 24.f;
			pDesc->world_hscroll->Height = 24;
			pDesc->world_hscroll->Width = pDesc->pLinkworld->surface_width-24.f;
		}
		else
		{
			pDesc->world_hscroll->Visible = PX_FALSE;
		}

		if (pDesc->pLinkworld->world_height > pDesc->pLinkworld->surface_height)
		{
			pDesc->world_vscroll->Visible = PX_TRUE;
			pDesc->world_vscroll->x = pDesc->pLinkworld->surface_width - 24.f;
			pDesc->world_vscroll->y = 0;
			pDesc->world_vscroll->Width = 24;
			pDesc->world_vscroll->Height = pDesc->pLinkworld->surface_height - 24.f;
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

//////////////////////////////////////////////////////////////////////////
const px_char *PX_Designer_GetID(PX_Object *pObject)
{
	return pObject->id;
}

px_void PX_Designer_SetID(PX_Object *pObject,const px_char id[])
{
	PX_ObjectSetId(pObject,id);
}

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

px_void PX_Designer_OnPropertyChanged(PX_Object *pObject,PX_Object_Event e,px_void *ptr)
{
	PX_Designer *pDesignerDesc=PX_ObjectGetDesc(PX_Designer,(PX_Object *)ptr);
	PX_Designer_UpdateProperties((PX_Object *)ptr);
}

px_void PX_Designer_OnPropertyRefresh(PX_Object *pObject,PX_Object_Event e,px_void *ptr)
{
	PX_Designer *pDesignerDesc=PX_ObjectGetDesc(PX_Designer,(PX_Object *)ptr);
	PX_Designer_RefreshProperties((PX_Object *)ptr);
}

px_void PX_Designer_OnDeleteCurrentObject(PX_Object *pObject,PX_Object_Event e,px_void *ptr)
{
	PX_Designer *pDesignerDesc=PX_ObjectGetDesc(PX_Designer,(PX_Object *)ptr);
	if (PX_VectorCheckIndex(&pDesignerDesc->Objects,pDesignerDesc->selectObjectIndex))
	{
		PX_Designer_Object *pdobject=PX_VECTORAT(PX_Designer_Object,&pDesignerDesc->Objects,pDesignerDesc->selectObjectIndex);

		switch(pdobject->type)
		{
		case PX_DESIGNER_OBJECT_TYPE_UI:
			{
				PX_ObjectDelete(pdobject->pObject);
			}
			break;
		case PX_DESIGNER_OBJECT_TYPE_GAME:
			{
				PX_WorldRemoveObject(pDesignerDesc->pLinkworld,pdobject->pObject);
			}
		default:
			break;
		}
		PX_VectorErase(&pDesignerDesc->Objects,pDesignerDesc->selectObjectIndex);
	}
	pDesignerDesc->selectObjectIndex=-1;
}

px_void PX_Designer_OnCursorDown(PX_Object *pObject,PX_Object_Event e,px_void *ptr)
{
	PX_Designer *pDesignerDesc=PX_ObjectGetDesc(PX_Designer,pObject);
	PX_Object *pSelectObject=PX_NULL;
	px_bool breadycreate=PX_FALSE;
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

		if (pDesignerDesc->selectObjectIndex==-1)
		{
			breadycreate=PX_TRUE;
		}
		

		pDesignerDesc->selectObjectIndex=-1;

		for (i=0;i<pDesignerDesc->Objects.size;i++)
		{
			PX_Designer_Object *pdobject=PX_VECTORAT(PX_Designer_Object,&pDesignerDesc->Objects,i);
			
			if (pdobject->type==PX_DESIGNER_OBJECT_TYPE_GAME)
			{
				px_rect rect;
				rect.x=pdobject->pObject->x-pDesignerDesc->pLinkworld->offsetx;
				rect.y=pdobject->pObject->y-pDesignerDesc->pLinkworld->offsety;
				rect.width=pdobject->pObject->Width;
				rect.height=pdobject->pObject->Height;
				if(PX_isPointInRect(PX_POINT(x,y,0),rect))
				{
					pDesignerDesc->selectObjectIndex=i;
				}
			}
			
			if (pdobject->type==PX_DESIGNER_OBJECT_TYPE_UI)
			{
				if(PX_ObjectIsCursorInRegion(pdobject->pObject,e))
				{
					pDesignerDesc->selectObjectIndex=i;
				}
			}

		}


		if (pDesignerDesc->selectObjectIndex==-1)
		{
			if (breadycreate)
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
							PX_Object *pNewObject=pObjectDesc->createfunc(pDesignerDesc->mp,pDesignerDesc->pLinkObject,x,y,100,100,pDesignerDesc->fm);
							PX_sprintf2(pNewObject->id,sizeof(pNewObject->id),"%1%2",PX_STRINGFORMAT_STRING(pObjectDesc->Name),PX_STRINGFORMAT_INT(pDesignerDesc->allocID++));

							dobject.pObject=pNewObject;
							dobject.type=PX_DESIGNER_OBJECT_TYPE_UI;
							dobject.descIndex=selectDescIndex;
							PX_VectorPushback(&pDesignerDesc->Objects,&dobject);
						}
						break;
					case PX_DESIGNER_OBJECT_TYPE_GAME:
						{
							PX_Object *pNewObject=pObjectDesc->createfunc(pDesignerDesc->mp,PX_NULL,x+pDesignerDesc->pLinkworld->offsetx,y + pDesignerDesc->pLinkworld->offsety,100,100,pDesignerDesc->fm);
							PX_sprintf2(pNewObject->id,sizeof(pNewObject->id),"%1%2",PX_STRINGFORMAT_STRING(pObjectDesc->Name),PX_STRINGFORMAT_INT(pDesignerDesc->allocID++));

							PX_WorldAddObject(pDesignerDesc->pLinkworld,pNewObject);
							dobject.pObject=pNewObject;
							dobject.type=PX_DESIGNER_OBJECT_TYPE_GAME;
							dobject.descIndex=selectDescIndex;
							PX_VectorPushback(&pDesignerDesc->Objects,&dobject);
						}
						break;
					case PX_DESIGNER_OBJECT_TYPE_FUNCTION:
						{
							PX_Object *pNewObject=pObjectDesc->createfunc(pDesignerDesc->mp,PX_NULL,x,y,100,100,pDesignerDesc->fm);
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
				else
				{
					pDesignerDesc->lastcursorx=x;
					pDesignerDesc->lastcursory=y;
				}
			}
			else
			{
				PX_Designer_RefreshProperties(pObject);
			}
		}
		else
		{
			PX_Designer_Object *pdobject=PX_VECTORAT(PX_Designer_Object,&pDesignerDesc->Objects,pDesignerDesc->selectObjectIndex);
			PX_Object_DesignerBox *pDesignerBox=PX_ObjectGetDesc(PX_Object_DesignerBox,pDesignerDesc->designerbox);
			pDesignerDesc->designerbox->x=pdobject->pObject->x;
			pDesignerDesc->designerbox->y=pdobject->pObject->y;
			pDesignerDesc->designerbox->Width=pdobject->pObject->Width;
			pDesignerDesc->designerbox->Height=pdobject->pObject->Height;

			pDesignerBox->bselect=PX_Object_DesignerBox_bselect_center;
			pDesignerBox->lastx = x;
			pDesignerBox->lasty = y;
		}

		if (pDesignerDesc->selectObjectIndex!=-1)
		{
			PX_Designer_RefreshProperties(pObject);
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
//Label
//////////////////////////////////////////////////////////////////////////
PX_Object *PX_Designer_LabelCreate(px_memorypool *mp,PX_Object *pparent,px_float x,px_float y,px_float width,px_float height,PX_FontModule *fm)
{
	return PX_Object_LabelCreate(mp,pparent,(px_int)x,(px_int)y,128,32,"Label",fm,PX_OBJECT_UI_DEFAULT_FONTCOLOR);
}

px_void PX_Designer_LabelSetText(PX_Object *pobject,const px_char text[])
{
	PX_Object_LabelSetText(pobject,text);
}

const px_char* PX_Designer_LabelGetText(PX_Object *pobject)
{
	return PX_Object_LabelGetText(pobject);
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

//////////////////////////////////////////////////////////////////////////
//Push button
//////////////////////////////////////////////////////////////////////////
PX_Object *PX_Designer_PushButtonCreate(px_memorypool *mp,PX_Object *pparent,px_float x,px_float y,px_float width,px_float height,PX_FontModule *fm)
{
	return PX_Object_PushButtonCreate(mp,pparent,(px_int)x,(px_int)y,128,64,"PushButton",fm);
}

px_void PX_Designer_PushButtonSetText(PX_Object *pobject,const px_char text[])
{
	PX_Object_PushButtonSetText(pobject,text);
}

const px_char* PX_Designer_PushButtonGetText(PX_Object *pobject)
{
	return PX_Object_PushButtonGetText(pobject);
}


//////////////////////////////////////////////////////////////////////////
//radiobox
//////////////////////////////////////////////////////////////////////////
PX_Object *PX_Designer_RadioBoxCreate(px_memorypool *mp,PX_Object *pparent,px_float x,px_float y,px_float width,px_float height,PX_FontModule *fm)
{
	return PX_Object_RadioBoxCreate(mp,pparent,(px_int)x,(px_int)y,108,28,"RadioButton",fm);
}

px_void PX_Designer_RadioBoxSetText(PX_Object *pobject,const px_char text[])
{
	PX_Object_RadioBoxSetText(pobject,text);
}

const px_char* PX_Designer_RadioBoxGetText(PX_Object *pobject)
{
	return PX_Object_RadioBoxGetText(pobject);
}

px_void PX_Designer_RadioBoxSetCheck(PX_Object *pobject,px_bool b)
{
	PX_Object_RadioBoxSetCheck(pobject,b);
}

px_bool PX_Designer_RadioBoxGetCheck(PX_Object *pobject)
{
	return PX_Object_RadioBoxGetCheck(pobject);
}

//////////////////////////////////////////////////////////////////////////
//checkbox
//////////////////////////////////////////////////////////////////////////
PX_Object *PX_Designer_CheckBoxCreate(px_memorypool *mp,PX_Object *pparent,px_float x,px_float y,px_float width,px_float height,PX_FontModule *fm)
{
	return PX_Object_CheckBoxCreate(mp,pparent,(px_int)x,(px_int)y,96,28,"CheckBox",fm);
}

px_void PX_Designer_CheckBoxSetText(PX_Object *pobject,const px_char text[])
{
	PX_Object_CheckBoxSetText(pobject,text);
}

const px_char* PX_Designer_CheckBoxGetText(PX_Object *pobject)
{
	return PX_Object_CheckBoxGetText(pobject);
}

px_void PX_Designer_CheckBoxSetCheck(PX_Object *pobject,px_bool b)
{
	PX_Object_CheckBoxSetCheck(pobject,b);
}

px_bool PX_Designer_CheckBoxGetCheck(PX_Object *pobject)
{
	return PX_Object_CheckBoxGetCheck(pobject);
}

//////////////////////////////////////////////////////////////////////////
//edit
//////////////////////////////////////////////////////////////////////////
PX_Object *PX_Designer_EditCreate(px_memorypool *mp,PX_Object *pparent,px_float x,px_float y,px_float width,px_float height,PX_FontModule *fm)
{
	return PX_Object_EditCreate(mp,pparent,(px_int)x,(px_int)y,128,28,fm);
}

px_void PX_Designer_EditSetText(PX_Object *pobject,const px_char text[])
{
	PX_Object_EditSetText(pobject,text);
}

const px_char* PX_Designer_EditGetText(PX_Object *pobject)
{
	return PX_Object_EditGetText(pobject);
}

//////////////////////////////////////////////////////////////////////////
//process bar
//////////////////////////////////////////////////////////////////////////
PX_Object* PX_Designer_ProcessBarCreate(px_memorypool* mp, PX_Object* pparent, px_float x, px_float y, px_float width, px_float height, PX_FontModule* fm)
{
	return PX_Object_ProcessBarCreate(mp, pparent, (px_int)x, (px_int)y, 192, 32);
}

px_void PX_Designer_ProcessBarSetMax(PX_Object* pobject, px_int max)
{
	PX_Object_ProcessBarSetMax(pobject, max);
}

px_int PX_Designer_ProcessBarGetMax(PX_Object* pobject)
{
	return PX_Object_ProcessBarGetMax(pobject);
}

px_void PX_Designer_ProcessBarSetValue(PX_Object* pobject, px_int value)
{
	PX_Object_ProcessBarSetValue(pobject,value);
}

px_int PX_Designer_ProcessBarGetValue(PX_Object* pobject)
{
	return PX_Object_ProcessBarGetValue(pobject);
}



//////////////////////////////////////////////////////////////////////////
//list
//////////////////////////////////////////////////////////////////////////

PX_Object* PX_Designer_ListCreate(px_memorypool* mp, PX_Object* pparent, px_float x, px_float y, px_float width, px_float height, PX_FontModule* fm)
{
	return PX_Object_ListContentCreate(mp, pparent, (px_int)x, (px_int)y, 256, 128, fm);
}


//////////////////////////////////////////////////////////////////////////
//SelectBar
//////////////////////////////////////////////////////////////////////////

PX_Object* PX_Designer_SelectBarCreate(px_memorypool* mp, PX_Object* pparent, px_float x, px_float y, px_float width, px_float height, PX_FontModule* fm)
{
	return PX_Object_SelectBarCreate(mp, pparent, (px_int)x, (px_int)y, 96, 24, fm);
}

px_void PX_Designer_SelectBarSetText(PX_Object* pobject, const px_char text[])
{
	px_char content[64];
	px_int _strlen;
	px_int i, j;
	PX_Object_SelectBarClear(pobject);
	i = 0;
	_strlen = PX_strlen(text);
	while (i < _strlen)
	{
		j = 0;
		while (text[i] != '\n' && text[i] != '\0')
		{
			if (j >= PX_COUNTOF(content) - 1)
			{
				return;
			}
			content[j++] = text[i++];
		}
		content[j] = '\0';
		if (text[i] == '\n')
		{
			i++;
		}
		PX_Object_SelectBarAddItem(pobject, content);
	}
}


const px_char *PX_Designer_SelectBarGetText(PX_Object* pobject)
{
	return "";
}


//////////////////////////////////////////////////////////////////////////
//SliderBar
//////////////////////////////////////////////////////////////////////////
PX_Object* PX_Designer_SliderBarCreate(px_memorypool* mp, PX_Object* pparent, px_float x, px_float y, px_float width, px_float height, PX_FontModule* fm)
{
	return PX_Object_SliderBarCreate(mp, pparent, (px_int)x, (px_int)y, 192, 32,PX_OBJECT_SLIDERBAR_TYPE_HORIZONTAL,PX_OBJECT_SLIDERBAR_STYLE_LINER);
}

px_void PX_Designer_SliderBarSetMax(PX_Object* pobject, px_int max)
{
	PX_Object_SliderBarSetRange(pobject,PX_ObjectGetDesc(PX_Object_SliderBar,pobject)->Min ,max);
}

px_int PX_Designer_SliderBarGetMax(PX_Object* pobject)
{
	return PX_ObjectGetDesc(PX_Object_SliderBar, pobject)->Max;
}

px_void PX_Designer_SliderBarSetMin(PX_Object* pobject, px_int min)
{
	PX_Object_SliderBarSetRange(pobject,min, PX_ObjectGetDesc(PX_Object_SliderBar, pobject)->Max);
}

px_int PX_Designer_SliderBarGetMin(PX_Object* pobject)
{
	return PX_ObjectGetDesc(PX_Object_SliderBar, pobject)->Min;
}

px_void PX_Designer_SliderBarSetType(PX_Object* pobject, px_int type)
{
	switch (type)
	{
	case PX_OBJECT_SLIDERBAR_TYPE_HORIZONTAL:
	case PX_OBJECT_SLIDERBAR_TYPE_VERTICAL:
		PX_ObjectGetDesc(PX_Object_SliderBar, pobject)->Type = (PX_OBJECT_SLIDERBAR_TYPE)type;
	default:
		break;
	}
	
}
px_int PX_Designer_SliderBarGetType(PX_Object* pobject)
{
	return (px_int)PX_ObjectGetDesc(PX_Object_SliderBar, pobject)->Type;
}


px_void PX_Designer_SliderBarSetStyle(PX_Object* pobject, px_int style)
{
	switch (style)
	{
	case PX_OBJECT_SLIDERBAR_STYLE_BOX:
	case PX_OBJECT_SLIDERBAR_STYLE_LINER:
		PX_ObjectGetDesc(PX_Object_SliderBar, pobject)->style = (PX_OBJECT_SLIDERBAR_STYLE)style;
	default:
		break;
	}

}
px_int PX_Designer_SliderBarGetStyle(PX_Object* pobject)
{
	return (px_int)PX_ObjectGetDesc(PX_Object_SliderBar, pobject)->style;
}

px_void PX_Designer_SliderBarSetValue(PX_Object* pobject, px_int value)
{
	PX_Object_SliderBarSetValue(pobject, value);
}

px_int PX_Designer_SliderBarGetValue(PX_Object* pobject)
{
	return PX_Object_SliderBarGetValue(pobject);
}


px_bool PX_DesignerStandardInitialize(PX_Designer *designer)
{

	do 
	{
		PX_Designer_ObjectDesc slabel;
		px_int i=0;
		PX_memset(&slabel,0,sizeof(slabel));
		PX_strcat(slabel.Name,"label");
		slabel.createfunc=PX_Designer_LabelCreate;
		slabel.type=PX_DESIGNER_OBJECT_TYPE_UI;

		PX_strcat(slabel.properties[i].Name,"id");
		slabel.properties[i].getstring=PX_Designer_GetID;
		slabel.properties[i].setstring=PX_Designer_SetID;
		i++;

		PX_strcat(slabel.properties[i].Name,"x");
		slabel.properties[i].getfloat=PX_Designer_GetX;
		slabel.properties[i].setfloat=PX_Designer_SetX;
		i++;
		
		PX_strcat(slabel.properties[i].Name,"y");
		slabel.properties[i].getfloat=PX_Designer_GetY;
		slabel.properties[i].setfloat=PX_Designer_SetY;
		i++;

		PX_strcat(slabel.properties[i].Name,"width");
		slabel.properties[i].getfloat=PX_Designer_GetWidth;
		slabel.properties[i].setfloat=PX_Designer_SetWidth;
		i++;

		PX_strcat(slabel.properties[i].Name,"height");
		slabel.properties[i].getfloat=PX_Designer_GetHeight;
		slabel.properties[i].setfloat=PX_Designer_SetHeight;
		i++;

		PX_strcat(slabel.properties[i].Name,"text");
		slabel.properties[i].setstring=PX_Designer_LabelSetText;
		slabel.properties[i].getstring=PX_Designer_LabelGetText;
		i++;


		PX_strcat(slabel.properties[i].Name,"align");
		slabel.properties[i].setint=PX_Designer_LabelSetAlign;
		slabel.properties[i].getint=PX_Designer_LabelGetAlign;
		i++;

		PX_VectorPushback(&designer->ObjectDesc, &slabel);
		slabel.type=PX_DESIGNER_OBJECT_TYPE_GAME;
		PX_VectorPushback(&designer->ObjectDesc, &slabel);
	} while (0);

	do 
	{
		PX_Designer_ObjectDesc spushbutton;
		px_int i=0;
		PX_memset(&spushbutton,0,sizeof(spushbutton));
		PX_strcat(spushbutton.Name,"pushbutton");
		spushbutton.createfunc=PX_Designer_PushButtonCreate;
		spushbutton.type=PX_DESIGNER_OBJECT_TYPE_UI;

		PX_strcat(spushbutton.properties[i].Name,"id");
		spushbutton.properties[i].getstring=PX_Designer_GetID;
		spushbutton.properties[i].setstring=PX_Designer_SetID;
		i++;
		
		PX_strcat(spushbutton.properties[i].Name,"x");
		spushbutton.properties[i].getfloat=PX_Designer_GetX;
		spushbutton.properties[i].setfloat=PX_Designer_SetX;
		i++;
		
		PX_strcat(spushbutton.properties[i].Name,"y");
		spushbutton.properties[i].getfloat=PX_Designer_GetY;
		spushbutton.properties[i].setfloat=PX_Designer_SetY;
		i++;

		PX_strcat(spushbutton.properties[i].Name,"width");
		spushbutton.properties[i].getfloat=PX_Designer_GetWidth;
		spushbutton.properties[i].setfloat=PX_Designer_SetWidth;
		i++;

		PX_strcat(spushbutton.properties[i].Name,"height");
		spushbutton.properties[i].getfloat=PX_Designer_GetHeight;
		spushbutton.properties[i].setfloat=PX_Designer_SetHeight;
		i++;

		PX_strcat(spushbutton.properties[i].Name,"text");
		spushbutton.properties[i].setstring=PX_Designer_PushButtonSetText;
		spushbutton.properties[i].getstring=PX_Designer_PushButtonGetText;
		i++;

		PX_VectorPushback(&designer->ObjectDesc, &spushbutton);
		spushbutton.type=PX_DESIGNER_OBJECT_TYPE_GAME;
		PX_VectorPushback(&designer->ObjectDesc, &spushbutton);
	} while (0);


	do
	{
		PX_Designer_ObjectDesc sradiobutton;
		px_int i = 0;
		PX_memset(&sradiobutton, 0, sizeof(sradiobutton));
		PX_strcat(sradiobutton.Name, "radiobutton");

		sradiobutton.createfunc = PX_Designer_RadioBoxCreate;
		sradiobutton.type = PX_DESIGNER_OBJECT_TYPE_UI;

		PX_strcat(sradiobutton.properties[i].Name, "id");
		sradiobutton.properties[i].getstring = PX_Designer_GetID;
		sradiobutton.properties[i].setstring = PX_Designer_SetID;
		i++;

		PX_strcat(sradiobutton.properties[i].Name, "x");
		sradiobutton.properties[i].getfloat = PX_Designer_GetX;
		sradiobutton.properties[i].setfloat = PX_Designer_SetX;
		i++;

		PX_strcat(sradiobutton.properties[i].Name, "y");
		sradiobutton.properties[i].getfloat = PX_Designer_GetY;
		sradiobutton.properties[i].setfloat = PX_Designer_SetY;
		i++;

		PX_strcat(sradiobutton.properties[i].Name, "width");
		sradiobutton.properties[i].getfloat = PX_Designer_GetWidth;
		sradiobutton.properties[i].setfloat = PX_Designer_SetWidth;
		i++;

		PX_strcat(sradiobutton.properties[i].Name, "height");
		sradiobutton.properties[i].getfloat = PX_Designer_GetHeight;
		sradiobutton.properties[i].setfloat = PX_Designer_SetHeight;
		i++;

		PX_strcat(sradiobutton.properties[i].Name, "text");
		sradiobutton.properties[i].setstring = PX_Designer_RadioBoxSetText;
		sradiobutton.properties[i].getstring = PX_Designer_RadioBoxGetText;
		i++;

		PX_strcat(sradiobutton.properties[i].Name, "check");
		sradiobutton.properties[i].setbool = PX_Designer_RadioBoxSetCheck;
		sradiobutton.properties[i].getbool = PX_Designer_RadioBoxGetCheck;
		i++;

		PX_VectorPushback(&designer->ObjectDesc, &sradiobutton);
		sradiobutton.type = PX_DESIGNER_OBJECT_TYPE_GAME;
		PX_VectorPushback(&designer->ObjectDesc, &sradiobutton);
	} while (0);


	do
	{
		PX_Designer_ObjectDesc scheckbox;
		px_int i = 0;
		PX_memset(&scheckbox, 0, sizeof(scheckbox));
		PX_strcat(scheckbox.Name, "checkbox");

		scheckbox.createfunc = PX_Designer_CheckBoxCreate;
		scheckbox.type = PX_DESIGNER_OBJECT_TYPE_UI;

		PX_strcat(scheckbox.properties[i].Name, "id");
		scheckbox.properties[i].getstring = PX_Designer_GetID;
		scheckbox.properties[i].setstring = PX_Designer_SetID;
		i++;

		PX_strcat(scheckbox.properties[i].Name, "x");
		scheckbox.properties[i].getfloat = PX_Designer_GetX;
		scheckbox.properties[i].setfloat = PX_Designer_SetX;
		i++;

		PX_strcat(scheckbox.properties[i].Name, "y");
		scheckbox.properties[i].getfloat = PX_Designer_GetY;
		scheckbox.properties[i].setfloat = PX_Designer_SetY;
		i++;

		PX_strcat(scheckbox.properties[i].Name, "width");
		scheckbox.properties[i].getfloat = PX_Designer_GetWidth;
		scheckbox.properties[i].setfloat = PX_Designer_SetWidth;
		i++;

		PX_strcat(scheckbox.properties[i].Name, "height");
		scheckbox.properties[i].getfloat = PX_Designer_GetHeight;
		scheckbox.properties[i].setfloat = PX_Designer_SetHeight;
		i++;

		PX_strcat(scheckbox.properties[i].Name, "text");
		scheckbox.properties[i].setstring = PX_Designer_CheckBoxSetText;
		scheckbox.properties[i].getstring = PX_Designer_CheckBoxGetText;
		i++;

		PX_strcat(scheckbox.properties[i].Name, "check");
		scheckbox.properties[i].setbool = PX_Designer_CheckBoxSetCheck;
		scheckbox.properties[i].getbool = PX_Designer_CheckBoxGetCheck;
		i++;

		PX_VectorPushback(&designer->ObjectDesc, &scheckbox);
		scheckbox.type = PX_DESIGNER_OBJECT_TYPE_GAME;
		PX_VectorPushback(&designer->ObjectDesc, &scheckbox);
	} while (0);


	do
	{
		PX_Designer_ObjectDesc edit;
		px_int i = 0;
		PX_memset(&edit, 0, sizeof(edit));
		PX_strcat(edit.Name, "edit");

		edit.createfunc = PX_Designer_EditCreate;
		edit.type = PX_DESIGNER_OBJECT_TYPE_UI;

		PX_strcat(edit.properties[i].Name, "id");
		edit.properties[i].getstring = PX_Designer_GetID;
		edit.properties[i].setstring = PX_Designer_SetID;
		i++;

		PX_strcat(edit.properties[i].Name, "x");
		edit.properties[i].getfloat = PX_Designer_GetX;
		edit.properties[i].setfloat = PX_Designer_SetX;
		i++;

		PX_strcat(edit.properties[i].Name, "y");
		edit.properties[i].getfloat = PX_Designer_GetY;
		edit.properties[i].setfloat = PX_Designer_SetY;
		i++;

		PX_strcat(edit.properties[i].Name, "width");
		edit.properties[i].getfloat = PX_Designer_GetWidth;
		edit.properties[i].setfloat = PX_Designer_SetWidth;
		i++;

		PX_strcat(edit.properties[i].Name, "height");
		edit.properties[i].getfloat = PX_Designer_GetHeight;
		edit.properties[i].setfloat = PX_Designer_SetHeight;
		i++;

		PX_strcat(edit.properties[i].Name, "text");
		edit.properties[i].setstring = PX_Designer_EditSetText;
		edit.properties[i].getstring = PX_Designer_EditGetText;
		i++;

		PX_VectorPushback(&designer->ObjectDesc, &edit);
		edit.type = PX_DESIGNER_OBJECT_TYPE_GAME;
		PX_VectorPushback(&designer->ObjectDesc, &edit);
	} while (0);


	do
	{
		PX_Designer_ObjectDesc processbar;
		px_int i = 0;
		PX_memset(&processbar, 0, sizeof(processbar));
		PX_strcat(processbar.Name, "processbar");

		processbar.createfunc = PX_Designer_ProcessBarCreate;
		processbar.type = PX_DESIGNER_OBJECT_TYPE_UI;

		PX_strcat(processbar.properties[i].Name, "id");
		processbar.properties[i].getstring = PX_Designer_GetID;
		processbar.properties[i].setstring = PX_Designer_SetID;
		i++;

		PX_strcat(processbar.properties[i].Name, "x");
		processbar.properties[i].getfloat = PX_Designer_GetX;
		processbar.properties[i].setfloat = PX_Designer_SetX;
		i++;

		PX_strcat(processbar.properties[i].Name, "y");
		processbar.properties[i].getfloat = PX_Designer_GetY;
		processbar.properties[i].setfloat = PX_Designer_SetY;
		i++;

		PX_strcat(processbar.properties[i].Name, "width");
		processbar.properties[i].getfloat = PX_Designer_GetWidth;
		processbar.properties[i].setfloat = PX_Designer_SetWidth;
		i++;

		PX_strcat(processbar.properties[i].Name, "height");
		processbar.properties[i].getfloat = PX_Designer_GetHeight;
		processbar.properties[i].setfloat = PX_Designer_SetHeight;
		i++;

		PX_strcat(processbar.properties[i].Name, "max");
		processbar.properties[i].setint = PX_Designer_ProcessBarSetMax;
		processbar.properties[i].getint = PX_Designer_ProcessBarGetMax;
		i++;

		PX_strcat(processbar.properties[i].Name, "value");
		processbar.properties[i].setint = PX_Designer_ProcessBarSetValue;
		processbar.properties[i].getint = PX_Designer_ProcessBarGetValue;
		i++;

		PX_VectorPushback(&designer->ObjectDesc, &processbar);
		processbar.type = PX_DESIGNER_OBJECT_TYPE_GAME;
		PX_VectorPushback(&designer->ObjectDesc, &processbar);
	} while (0);

	do
	{
		PX_Designer_ObjectDesc list;
		px_int i = 0;
		PX_memset(&list, 0, sizeof(list));
		PX_strcat(list.Name, "list");

		list.createfunc = PX_Designer_ListCreate;
		list.type = PX_DESIGNER_OBJECT_TYPE_UI;

		PX_strcat(list.properties[i].Name, "id");
		list.properties[i].getstring = PX_Designer_GetID;
		list.properties[i].setstring = PX_Designer_SetID;
		i++;

		PX_strcat(list.properties[i].Name, "x");
		list.properties[i].getfloat = PX_Designer_GetX;
		list.properties[i].setfloat = PX_Designer_SetX;
		i++;

		PX_strcat(list.properties[i].Name, "y");
		list.properties[i].getfloat = PX_Designer_GetY;
		list.properties[i].setfloat = PX_Designer_SetY;
		i++;

		PX_strcat(list.properties[i].Name, "width");
		list.properties[i].getfloat = PX_Designer_GetWidth;
		list.properties[i].setfloat = PX_Designer_SetWidth;
		i++;

		PX_strcat(list.properties[i].Name, "height");
		list.properties[i].getfloat = PX_Designer_GetHeight;
		list.properties[i].setfloat = PX_Designer_SetHeight;
		i++;

		PX_VectorPushback(&designer->ObjectDesc, &list);
		list.type = PX_DESIGNER_OBJECT_TYPE_GAME;
		PX_VectorPushback(&designer->ObjectDesc, &list);
	} while (0);


	do
	{
		PX_Designer_ObjectDesc selectbar;
		px_int i = 0;
		PX_memset(&selectbar, 0, sizeof(selectbar));
		PX_strcat(selectbar.Name, "selectbar");

		selectbar.createfunc = PX_Designer_SelectBarCreate;
		selectbar.type = PX_DESIGNER_OBJECT_TYPE_UI;

		PX_strcat(selectbar.properties[i].Name, "id");
		selectbar.properties[i].getstring = PX_Designer_GetID;
		selectbar.properties[i].setstring = PX_Designer_SetID;
		i++;

		PX_strcat(selectbar.properties[i].Name, "x");
		selectbar.properties[i].getfloat = PX_Designer_GetX;
		selectbar.properties[i].setfloat = PX_Designer_SetX;
		i++;

		PX_strcat(selectbar.properties[i].Name, "y");
		selectbar.properties[i].getfloat = PX_Designer_GetY;
		selectbar.properties[i].setfloat = PX_Designer_SetY;
		i++;

		PX_strcat(selectbar.properties[i].Name, "width");
		selectbar.properties[i].getfloat = PX_Designer_GetWidth;
		selectbar.properties[i].setfloat = PX_Designer_SetWidth;
		i++;

		PX_strcat(selectbar.properties[i].Name, "height");
		selectbar.properties[i].getfloat = PX_Designer_GetHeight;
		selectbar.properties[i].setfloat = PX_Designer_SetHeight;
		i++;

		PX_strcat(selectbar.properties[i].Name, "list");
		selectbar.properties[i].setstring = PX_Designer_SelectBarSetText;
		selectbar.properties[i].getstring = PX_Designer_SelectBarGetText;
		i++;

		PX_VectorPushback(&designer->ObjectDesc, &selectbar);
		selectbar.type = PX_DESIGNER_OBJECT_TYPE_GAME;
		PX_VectorPushback(&designer->ObjectDesc, &selectbar);
	} while (0);

	do
	{
		PX_Designer_ObjectDesc sliderbar;
		px_int i = 0;
		PX_memset(&sliderbar, 0, sizeof(sliderbar));
		PX_strcat(sliderbar.Name, "sliderbar");

		sliderbar.createfunc = PX_Designer_SliderBarCreate;
		sliderbar.type = PX_DESIGNER_OBJECT_TYPE_UI;

		PX_strcat(sliderbar.properties[i].Name, "id");
		sliderbar.properties[i].getstring = PX_Designer_GetID;
		sliderbar.properties[i].setstring = PX_Designer_SetID;
		i++;

		PX_strcat(sliderbar.properties[i].Name, "x");
		sliderbar.properties[i].getfloat = PX_Designer_GetX;
		sliderbar.properties[i].setfloat = PX_Designer_SetX;
		i++;

		PX_strcat(sliderbar.properties[i].Name, "y");
		sliderbar.properties[i].getfloat = PX_Designer_GetY;
		sliderbar.properties[i].setfloat = PX_Designer_SetY;
		i++;

		PX_strcat(sliderbar.properties[i].Name, "width");
		sliderbar.properties[i].getfloat = PX_Designer_GetWidth;
		sliderbar.properties[i].setfloat = PX_Designer_SetWidth;
		i++;

		PX_strcat(sliderbar.properties[i].Name, "height");
		sliderbar.properties[i].getfloat = PX_Designer_GetHeight;
		sliderbar.properties[i].setfloat = PX_Designer_SetHeight;
		i++;

		PX_strcat(sliderbar.properties[i].Name, "min");
		sliderbar.properties[i].setint = PX_Designer_SliderBarSetMin;
		sliderbar.properties[i].getint = PX_Designer_SliderBarGetMin;
		i++;

		PX_strcat(sliderbar.properties[i].Name, "max");
		sliderbar.properties[i].setint = PX_Designer_SliderBarSetMax;
		sliderbar.properties[i].getint = PX_Designer_SliderBarGetMax;
		i++;


		PX_strcat(sliderbar.properties[i].Name, "type");
		sliderbar.properties[i].setint = PX_Designer_SliderBarSetType;
		sliderbar.properties[i].getint = PX_Designer_SliderBarGetType;
		i++;

		PX_strcat(sliderbar.properties[i].Name, "style");
		sliderbar.properties[i].setint = PX_Designer_SliderBarSetStyle;
		sliderbar.properties[i].getint = PX_Designer_SliderBarGetStyle;
		i++;

		PX_strcat(sliderbar.properties[i].Name, "value");
		sliderbar.properties[i].setint = PX_Designer_SliderBarSetValue;
		sliderbar.properties[i].getint = PX_Designer_SliderBarGetValue;
		i++;

		PX_VectorPushback(&designer->ObjectDesc, &sliderbar);
		sliderbar.type = PX_DESIGNER_OBJECT_TYPE_GAME;
		PX_VectorPushback(&designer->ObjectDesc, &sliderbar);
	} while (0);

	return PX_TRUE;
}

px_void PX_DesignerOnMenuRun(px_void *userPtr)
{
	PX_Object *pObject=(PX_Object *)userPtr;
	PX_ObjectExecuteEvent(pObject,PX_OBJECT_BUILD_EVENT(PX_OBJECT_EVENT_EXECUTE));
}

px_void PX_DesignerOnMenuExit(px_void *userPtr)
{
	PX_Object *pObject=(PX_Object *)userPtr;
	PX_ObjectExecuteEvent(pObject,PX_OBJECT_BUILD_EVENT(PX_OBJECT_EVENT_CLOSE));
}

px_void PX_DesignerOnMenuWindowControllers(px_void *userPtr)
{
	PX_Object *pObject=(PX_Object *)userPtr;
	PX_Designer *pDesc=PX_ObjectGetDesc(PX_Designer,pObject);

	if(pDesc->widget_controllers->Visible)
		PX_Object_WidgetHide(pDesc->widget_controllers);
	else
		PX_Object_WidgetShow(pDesc->widget_controllers);
}


px_void PX_DesignerOnMenuViewScrollbar(px_void* userPtr)
{
	PX_Object* pObject = (PX_Object*)userPtr;
	PX_Designer* pDesc = PX_ObjectGetDesc(PX_Designer, pObject);

	pDesc->showsliderbar = !pDesc->showsliderbar;
}

px_void PX_DesignerOnMenuWindowProperties(px_void *userPtr)
{
	PX_Object *pObject=(PX_Object *)userPtr;
	PX_Designer *pDesc=PX_ObjectGetDesc(PX_Designer,pObject);
	if(pDesc->widget_property->Visible)
		PX_Object_WidgetHide(pDesc->widget_property);
	else
		PX_Object_WidgetShow(pDesc->widget_property);
}

px_void PX_Designer_OnScrollVChanged(PX_Object* pObject, PX_Object_Event e, px_void* ptr)
{
	PX_Object* pDesignerObject = (PX_Object*)ptr;
	PX_Designer* pDesc = PX_ObjectGetDesc(PX_Designer, pDesignerObject);
	px_point camera = pDesc->pLinkworld->camera_offset;
	camera.y = PX_Object_SliderBarGetValue(pObject)*1.0f;
	PX_WorldSetCamera(pDesc->pLinkworld, camera);
}

px_void PX_Designer_OnScrollHChanged(PX_Object* pObject, PX_Object_Event e, px_void* ptr)
{
	PX_Object* pDesignerObject = (PX_Object*)ptr;
	PX_Designer* pDesc = PX_ObjectGetDesc(PX_Designer, pDesignerObject);
	px_point camera = pDesc->pLinkworld->camera_offset;
	camera.x = PX_Object_SliderBarGetValue(pObject) * 1.0f;
	PX_WorldSetCamera(pDesc->pLinkworld, camera);
}

PX_Object * PX_DesignerCreate(px_memorypool *mp,PX_Object *pparent,PX_Object *pLinkObject,PX_World *pLinkWorld,PX_FontModule *fm)
{
	px_int i;
	PX_Object *pObject;
	PX_Designer desc,*pdesc;
	PX_memset(&desc,0,sizeof(desc));

	pObject=PX_ObjectCreateEx(mp,pparent,0,0,0,0,0,0,0,PX_DesignerUpdate,PX_DesignerRender,PX_NULL,&desc,sizeof(desc));
	pdesc=(PX_Designer *)pObject->pObject;
	pdesc->pLinkworld=pLinkWorld;
	pdesc->pLinkObject=pLinkObject;
	pdesc->showsliderbar = PX_TRUE;
	pdesc->widget_property = PX_Object_WidgetCreate(mp, pObject, 5, 5 + 256, 192, 192, "properties", PX_NULL);
	if (!pdesc->widget_property)return PX_FALSE;
	pdesc->widget_property->x = pdesc->pLinkworld->surface_width - pdesc->widget_property->Width - 25;

	pdesc->widget_propertyscrollarea=PX_Object_ScrollAreaCreate(mp,pdesc->widget_property,0,0,174, 162);

	do 
	{
		PX_Object_Menu_Item *pItem;
		pdesc->menu=PX_Object_MenuCreate(mp,pObject,0,0,64,PX_NULL);

		pItem=PX_Object_MenuAddItem(pdesc->menu,PX_Object_MenuGetRootItem(pdesc->menu),"debug",PX_NULL,PX_NULL);
			PX_Object_MenuAddItem(pdesc->menu,pItem,"run",PX_DesignerOnMenuRun,pObject);
			PX_Object_MenuAddItem(pdesc->menu,pItem,"exit",PX_DesignerOnMenuExit,pObject);

		pItem=PX_Object_MenuAddItem(pdesc->menu,PX_Object_MenuGetRootItem(pdesc->menu),"window",PX_NULL,PX_NULL);
			PX_Object_MenuAddItem(pdesc->menu,pItem,"controllers",PX_DesignerOnMenuWindowControllers,pObject);
			PX_Object_MenuAddItem(pdesc->menu,pItem,"properties",PX_DesignerOnMenuWindowProperties,pObject);

		pItem = PX_Object_MenuAddItem(pdesc->menu, PX_Object_MenuGetRootItem(pdesc->menu), "View", PX_NULL, PX_NULL);
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
		PX_ObjectRegisterEvent(pdesc->edit_propertys[i],PX_OBJECT_EVENT_LOSTFOCUS,PX_Designer_OnPropertyRefresh,pObject);
	}


	pdesc->widget_controllers=PX_Object_WidgetCreate(mp,pObject,5,5,192,256,"controllers",PX_NULL);
	if(!pdesc->widget_controllers)return PX_FALSE;
	pdesc->widget_controllers->x = pdesc->pLinkworld->surface_width - pdesc->widget_controllers->Width - 25;


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
	pdesc->designerbox=PX_Object_DesignerBoxCreate(mp,pObject,0,0,100,100);
	pdesc->designerbox->Visible=PX_FALSE;

	pdesc->button_delete=PX_Object_PushButtonCreate(mp,pObject,0,0,16,16,"x",PX_NULL);
	pdesc->button_delete->Visible=PX_FALSE;
	PX_ObjectRegisterEvent(pdesc->button_delete,PX_OBJECT_EVENT_EXECUTE,PX_Designer_OnDeleteCurrentObject,pObject);

	pdesc->world_hscroll = PX_Object_SliderBarCreate(mp, pObject, 0, 0, 10, 10, PX_OBJECT_SLIDERBAR_TYPE_HORIZONTAL, PX_OBJECT_SLIDERBAR_STYLE_BOX);
	
	if (pdesc->pLinkworld->world_width > pdesc->pLinkworld->surface_width)
	{
		PX_Object_SliderBarSetRange(pdesc->world_hscroll, (px_int)(pdesc->pLinkworld->surface_width / 2) - 1, (px_int)(pdesc->pLinkworld->world_width - pdesc->pLinkworld->surface_width / 2) + 1);
	}
	else
	{
		pdesc->world_hscroll->Visible = PX_FALSE;
	}
	PX_ObjectRegisterEvent(pdesc->world_hscroll, PX_OBJECT_EVENT_VALUECHANGED, PX_Designer_OnScrollHChanged, pObject);


	pdesc->world_vscroll = PX_Object_SliderBarCreate(mp, pObject, 0, 0, 10, 10, PX_OBJECT_SLIDERBAR_TYPE_VERTICAL, PX_OBJECT_SLIDERBAR_STYLE_BOX);
	if (pdesc->pLinkworld->world_height > pdesc->pLinkworld->surface_height)
	{
		PX_Object_SliderBarSetRange(pdesc->world_vscroll, (px_int)(pdesc->pLinkworld->surface_height / 2) - 1, (px_int)(pdesc->pLinkworld->world_height - pdesc->pLinkworld->surface_height / 2) + 1);
	}
	else
	{
		pdesc->world_vscroll->Visible = PX_FALSE;
	}
	PX_ObjectRegisterEvent(pdesc->world_vscroll, PX_OBJECT_EVENT_VALUECHANGED, PX_Designer_OnScrollVChanged, pObject);

	if (!PX_DesignerStandardInitialize(pdesc))
	{
		return PX_FALSE;
	}

	PX_ObjectRegisterEvent(pObject,PX_OBJECT_EVENT_CURSORDOWN,PX_Designer_OnCursorDown,pObject);
	PX_ObjectRegisterEvent(pdesc->designerbox,PX_OBJECT_EVENT_VALUECHANGED,PX_DesignerOnDesignerBoxValueChanged,pObject);

	
	return pObject;
}
