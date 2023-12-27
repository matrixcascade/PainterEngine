#include "PX_Object_Printer.h"

px_void PX_Object_PrinterUpdateLines(PX_Object* pObject)
{
	px_int i, y = 10;
	PX_Object_Printer* pDesc = PX_ObjectGetDesc(PX_Object_Printer, pObject);
	PX_Object_PrinterLine* pLine;

	while (pDesc->pObjects.size > pDesc->max_column)
	{
		pLine = (PX_VECTORAT(PX_Object_PrinterLine, &pDesc->pObjects, 0));
		PX_ObjectDelete(pLine->pObject);
		PX_VectorErase(&pDesc->pObjects, 0);
	}

	for (i = 0; i < pDesc->pObjects.size; i++)
	{
		pLine = (PX_VECTORAT(PX_Object_PrinterLine, &pDesc->pObjects, i));

		PX_ObjectSetPosition(pLine->pObject, 0, (px_float)y, 0);
		y += (px_int)PX_ObjectGetHeight(pLine->pObject);
	}
	PX_ObjectSetPosition(pDesc->Input, 0, (px_float)y, 0);
}
PX_Object* PX_Object_PrinterPrintText(PX_Object* pObject, const px_char* text)
{
	if (pObject->Type==PX_OBJECT_TYPE_PRINTER)
	{
		PX_Object_PrinterLine obj;
		PX_Object_Printer* pDesc = PX_ObjectGetDesc(PX_Object_Printer, pObject);
		PX_Object* pAutoObject = PX_Object_AutoTextCreate(pObject->mp, pDesc->Area, 0, 0, (px_int)pDesc->Area->Width, pDesc->fm);

		if (pAutoObject)
		{
			obj.pObject = pAutoObject;
			obj.id = pDesc->id++;
			PX_Object_AutoTextSetTextColor(pAutoObject, pDesc->fontColor);
			PX_Object_AutoTextSetText(pAutoObject, text);
			PX_VectorPushback(&pDesc->pObjects, &obj);
			PX_Object_PrinterUpdateLines(pObject);
			
			PX_Object_ScrollAreaUpdateRange(pDesc->Area);
			PX_Object_ScrollAreaMoveToBottom(pDesc->Area);
		}
		return pAutoObject;
	}
	return PX_NULL;
}


PX_Object* PX_Object_PrinterLastPrintText(PX_Object* pObject, const px_char* text)
{
	px_int index;
	if (pObject->Type == PX_OBJECT_TYPE_PRINTER)
	{
		PX_Object_PrinterLine* pobjColumn=PX_NULL;
		PX_Object_Printer* pDesc = PX_ObjectGetDesc(PX_Object_Printer, pObject);
		if (pDesc->pObjects.size == 0)
		{
			return PX_Object_PrinterPrintText(pObject,text);
		}
		index=pDesc->pObjects.size - 1;
		while (index>=0)
		{
			pobjColumn = PX_VECTORAT(PX_Object_PrinterLine, &pDesc->pObjects, index);
			pObject = pobjColumn->pObject;
			if (pObject->Type == PX_OBJECT_TYPE_AUTOTEXT)
			{
				PX_Object_AutoTextSetTextColor(pObject, pDesc->fontColor);
				PX_Object_AutoTextSetText(pObject, text);
				PX_Object_ScrollAreaUpdateRange(pDesc->Area);
				return pObject;
			}

			index--;
		}
		
	}
	return PX_NULL;
}

PX_Object* PX_Object_PrinterGetArea(PX_Object* pObject)
{
	if (pObject->Type == PX_OBJECT_TYPE_PRINTER)
	{
		PX_Object_Printer* pDesc = PX_ObjectGetDesc(PX_Object_Printer, pObject);
		return pDesc->Area;
	}
	return 0;
}

PX_Object* PX_Object_PrinterPrintProcessBar(PX_Object* pObject)
{
	if (pObject->Type == PX_OBJECT_TYPE_PRINTER)
	{
		PX_Object_PrinterLine obj;
		PX_Object_Printer* pDesc = PX_ObjectGetDesc(PX_Object_Printer, pObject);
		PX_Object* pNewObject = PX_Object_ProcessBarCreate(pObject->mp, pDesc->Area, 3, 0, (px_int)pDesc->Area->Width*2/3, 24);

		if (pNewObject)
		{
			obj.pObject = pNewObject;
			obj.id = pDesc->id++;
			PX_Object_ProcessBarSetBackgroundColor(pNewObject, PX_COLOR_BLACK);
			PX_Object_ProcessBarSetColor(pNewObject, PX_COLOR(255, 0, 192, 0));
			PX_Object_ProcessBarSetTextColor(pNewObject, PX_COLOR_WHITE);
			PX_VectorPushback(&pDesc->pObjects, &obj);
			PX_Object_PrinterUpdateLines(pObject);
			PX_Object_ScrollAreaUpdateRange(pDesc->Area);
			PX_Object_ScrollAreaMoveToBottom(pDesc->Area);

		}
		return pNewObject;
	}
	return PX_NULL;
}

PX_Object* PX_Object_PrinterPrintButton(PX_Object* pObject, px_int width, px_int height, const px_char* text)
{
	if (pObject->Type == PX_OBJECT_TYPE_PRINTER)
	{
		PX_Object_PrinterLine obj;
		PX_Object_Printer* pDesc = PX_ObjectGetDesc(PX_Object_Printer, pObject);
		PX_Object* pNewObject = PX_Object_PushButtonCreate(pObject->mp, pDesc->Area, 0, 0,width, height,text, pDesc->fm);

		if (pNewObject)
		{
			obj.pObject = pNewObject;
			obj.id = pDesc->id++;
			PX_Object_PushButtonSetStyle(pNewObject, PX_OBJECT_PUSHBUTTON_STYLE_ROUNDRECT);
			PX_Object_PushButtonSetRoundRadius(pNewObject, height/2.f-1);
			PX_Object_PushButtonSetBackgroundColor(pNewObject, PX_COLOR(255,132,197,255));
			PX_Object_PushButtonSetBorderColor(pNewObject, PX_COLOR(192, 68, 100, 128));
			PX_Object_PushButtonSetCursorColor(pNewObject, PX_COLOR(255, 168, 224, 255));
			PX_Object_PushButtonSetPushColor(pNewObject, PX_COLOR(64, 0, 0, 0));
			PX_Object_PushButtonSetTextColor(pNewObject, pDesc->fontColor);
			PX_VectorPushback(&pDesc->pObjects, &obj);
			PX_Object_PrinterUpdateLines(pObject);
			PX_Object_ScrollAreaUpdateRange(pDesc->Area);
			PX_Object_ScrollAreaMoveToBottom(pDesc->Area);
		}
		return pNewObject;
	}
	return PX_NULL;
}

PX_Object* PX_Object_PrinterPrintObject(PX_Object* pObject, PX_Object* pNewObject)
{
	PX_Object_PrinterLine obj;
	PX_Object_Printer* pDesc = PX_ObjectGetDesc(PX_Object_Printer, pObject);
	PX_ASSERTIF(pNewObject == PX_NULL);
	if (pNewObject)
	{
		obj.pObject = pNewObject;
		obj.id = pDesc->id++;
		PX_VectorPushback(&pDesc->pObjects, &obj);
		PX_Object_PrinterUpdateLines(pObject);
		PX_Object_ScrollAreaUpdateRange(pDesc->Area);
		PX_Object_ScrollAreaMoveToBottom(pDesc->Area);
	}

	return pNewObject;
}

PX_Object* PX_Object_PrinterPrintSpace(PX_Object* pObject, px_int height)
{
	if (pObject->Type == PX_OBJECT_TYPE_PRINTER)
	{
		PX_Object_PrinterLine obj;
		PX_Object_Printer* pDesc = PX_ObjectGetDesc(PX_Object_Printer, pObject);
		PX_Object* pNewObject = PX_ObjectCreate(pObject->mp, pDesc->Area,0,0,0,1,height*1.f,0);
		if (pNewObject)
		{
			obj.pObject = pNewObject;
			obj.id = pDesc->id++;
			PX_VectorPushback(&pDesc->pObjects, &obj);
			PX_Object_PrinterUpdateLines(pObject);
			PX_Object_ScrollAreaUpdateRange(pDesc->Area);
			PX_Object_ScrollAreaMoveToBottom(pDesc->Area);
			return pNewObject;
		}
	}
	return PX_NULL;
}

PX_Object* PX_Object_PrinterLastProcessBarValue(PX_Object* pObject, const px_int value)
{
	px_int index;
	if (pObject->Type == PX_OBJECT_TYPE_PRINTER)
	{
		PX_Object_PrinterLine* pobjColumn = PX_NULL;
		PX_Object_Printer* pDesc = PX_ObjectGetDesc(PX_Object_Printer, pObject);
		if (pDesc->pObjects.size == 0)
		{
			PX_Object*pNewObject= PX_Object_PrinterPrintProcessBar(pObject);
			PX_Object_ProcessBarSetValue(pNewObject, value);
		}

		index = pDesc->pObjects.size - 1;
		while (index >= 0)
		{
			pobjColumn = PX_VECTORAT(PX_Object_PrinterLine, &pDesc->pObjects, index);
			pObject = pobjColumn->pObject;
			if (pObject->Type == PX_OBJECT_TYPE_PROCESSBAR)
			{
				PX_Object_ProcessBarSetValue(pObject, value);
				return pObject;
			}
			index--;
		}
	}
	return PX_NULL;
}


PX_Object* PX_Object_PrinterPrintImage(PX_Object* pObject, px_texture* pTexture)
{
	if (pObject->Type == PX_OBJECT_TYPE_PRINTER)
	{
		PX_Object_PrinterLine obj;
		PX_Object_Printer* pDesc = PX_ObjectGetDesc(PX_Object_Printer, pObject);
		PX_Object* pImageObject;
		if(pTexture)
			pImageObject = PX_Object_ImageCreate(pObject->mp, pDesc->Area, 0, 0, pTexture->width, pTexture->height, pTexture);
		else
			pImageObject = PX_Object_ImageCreate(pObject->mp, pDesc->Area, 0, 0, 100, 100, 0);
		PX_Object_ImageSetAlign(pImageObject, PX_ALIGN_LEFTTOP);
		obj.pObject = pImageObject;
		obj.id = pDesc->id++;
		PX_VectorPushback(&pDesc->pObjects, &obj);
		PX_Object_PrinterUpdateLines(pObject);
		PX_Object_ScrollAreaUpdateRange(pDesc->Area);
		PX_Object_ScrollAreaMoveToBottom(pDesc->Area);
		return pImageObject;
	}
	return PX_NULL;
}


PX_Object* PX_Object_PrinterGetObject(PX_Object* pObject, px_int id)
{
	if (pObject->Type != PX_OBJECT_TYPE_PRINTER)
	{
		PX_ASSERT();
		return PX_NULL;
	}
	else
	{
		px_int i;
		PX_Object_PrinterLine* pCc;
		PX_Object_Printer* pDesc = PX_ObjectGetDesc(PX_Object_Printer, pObject);
		for (i = 0; i < pDesc->pObjects.size; i++)
		{
			pCc = (PX_VECTORAT(PX_Object_PrinterLine, &pDesc->pObjects, i));
			if (pCc->id==id)
			{
				return pCc->pObject;
			}
		}
	}
	return PX_NULL;
}

px_void PX_Object_PrinterGets(PX_Object* pObject)
{
	if (pObject->Type != PX_OBJECT_TYPE_PRINTER)
	{
		PX_ASSERT();
		return ;
	}
	else
	{
		PX_Object_Printer* pDesc = PX_ObjectGetDesc(PX_Object_Printer, pObject);
		pDesc->bInput = PX_TRUE;
		PX_Object_EditSetFocus(pDesc->Input,PX_TRUE);
		pDesc->Input->Visible = PX_TRUE;
		PX_Object_PrinterUpdateLines(pObject);
		PX_Object_ScrollAreaMoveToBottom(pDesc->Area);
	}
}

px_void PX_Object_PrinterClear(PX_Object* pObject)
{
	PX_Object_PrinterLine* pCc;
	px_int i;
	PX_Object_Printer* pDesc = PX_ObjectGetDesc(PX_Object_Printer, pObject);

	if (pObject->Type!=PX_OBJECT_TYPE_PRINTER)
	{
		PX_ASSERT();
		return;
	}

	for (i = 0; i < pDesc->pObjects.size; i++)
	{
		pCc = (PX_VECTORAT(PX_Object_PrinterLine, &pDesc->pObjects, i));
		PX_ObjectDelete(pCc->pObject);
	}
	PX_VectorClear(&pDesc->pObjects);
}

px_void PX_Object_PrinterUpdateAll(PX_Object* pObject)
{
	if (pObject->Type == PX_OBJECT_TYPE_PRINTER)
	{
		PX_Object_Printer* pDesc = PX_ObjectGetDesc(PX_Object_Printer, pObject);
		PX_Object_PrinterUpdateLines(pObject);
		PX_Object_ScrollAreaUpdateRange(pDesc->Area);
		PX_Object_ScrollAreaMoveToBottom(pDesc->Area);
	}
	
}

px_int PX_Object_PrinterGetLastCreateId(PX_Object* pObject)
{
	if (pObject->Type != PX_OBJECT_TYPE_PRINTER)
	{
		PX_ASSERT();
		return -1;
	}
	else
	{
		PX_Object_Printer* pDesc = PX_ObjectGetDesc(PX_Object_Printer, pObject);
		if (pDesc->pObjects.size)
		{
		return	PX_VECTORAT(PX_Object_PrinterLine, &pDesc->pObjects, pDesc->pObjects.size-1)->id;
		}
		return -1;
	}
}

px_void PX_Object_PrinterOnEnter(PX_Object* pObject, PX_Object_Event e, px_void* user_ptr)
{
	PX_Object_Printer* pDesc = PX_ObjectGetDesc(PX_Object_Printer, (PX_Object *)user_ptr);
	PX_Object_Edit* pEdit = PX_Object_GetEdit(pDesc->Input);

	if (!pDesc->Input->Visible)
	{
		return;
	}

	if (e.Event == PX_OBJECT_EVENT_KEYDOWN)
	{
		if (PX_Object_Event_GetKeyDown(e) == '\r')
		{
			PX_ObjectExecuteEvent((PX_Object*)user_ptr, PX_OBJECT_BUILD_EVENT_STRING(PX_OBJECT_EVENT_EXECUTE, PX_Object_EditGetText(pDesc->Input)));
			PX_Object_EditSetText(pDesc->Input, "");
			PX_ObjectSetVisible(pDesc->Input, PX_FALSE);
			pDesc->bInput = PX_FALSE;
		}
	}

	if (e.Event == PX_OBJECT_EVENT_CURSORRDOWN)
	{
		PX_Object_EditSetFocus(pDesc->Input, PX_TRUE);
	}
}

px_void PX_Object_PrinterOnMouseDown(PX_Object* pObject, PX_Object_Event e, px_void* user_ptr)
{
	PX_Object_Printer* pDesc = PX_ObjectGetDesc(PX_Object_Printer, pObject);
	if (e.Event == PX_OBJECT_EVENT_CURSORDOWN)
	{
		PX_Object_EditSetFocus(pDesc->Input, PX_TRUE);
	}
}



px_void PX_Object_PrinterRender(px_surface *psurface,PX_Object* pObject, px_dword elapsed)
{
	PX_Object_Printer* pDesc = PX_ObjectGetDesc(PX_Object_Printer, pObject);
	if (pDesc->show)
	{
		pDesc->Area->Width = (px_float)pObject->Width;
		pDesc->Area->Height = (px_float)pObject->Height;
		pDesc->Input->Width = (px_float)pObject->Width;
		
		if (pDesc->bInput)
		{
			pDesc->Input->Visible = PX_TRUE;
		}
		else
		{
			pDesc->Input->Visible = PX_FALSE;
		}
		
	}
}

px_void PX_Object_PrinterFree(PX_Object* pObject)
{
	PX_Object_Printer* pDesc = PX_ObjectGetDesc(PX_Object_Printer, pObject);
	PX_VectorFree(&pDesc->pObjects);
}

px_void PX_Object_PrinterSetBackgroundColor(PX_Object* pObject,px_color color)
{
	PX_Object_Printer* pDesc = PX_ObjectGetDesc(PX_Object_Printer, pObject);
	PX_Object_ScrollAreaSetBackgroundColor(pDesc->Area, color);
}

PX_Object* PX_Object_PrinterCreate(px_memorypool* mp, PX_Object* Parent, px_int x, px_int y, px_int width, px_int height, PX_FontModule* fm)
{
	PX_Object* pObject;
	PX_Object_Printer Desc, * pDesc;
	px_int fmheight = PX_FontGetCharactorHeight()+4;
	//console initialize
	PX_memset(&Desc, 0, sizeof(PX_Object_Printer));
	if (fm)
	{
		fmheight = fm->max_Height + 4;
	}
	pObject = PX_ObjectCreateEx(mp, Parent, (px_float)x, (px_float)y, 0, (px_float)width, (px_float)height, 0, PX_OBJECT_TYPE_PRINTER, 0, PX_Object_PrinterRender, PX_Object_PrinterFree, &Desc, sizeof(Desc));
	pDesc= PX_ObjectGetDesc(PX_Object_Printer, pObject);

	pDesc->show = PX_TRUE;
	pDesc->max_column = PX_OBJECT_PRINTER_DEFAULT_MAX_COLUMN;
	pDesc->column = 0;
	pDesc->fm = fm;
	pDesc->id = 1;
	pDesc->fontColor = PX_COLOR_GREEN;
	if (!(pDesc->Area = PX_Object_ScrollAreaCreate(mp, pObject, 0, 0,(px_int)width, (px_int)height))) return PX_NULL;

	PX_ObjectRegisterEvent(pDesc->Area, PX_OBJECT_EVENT_KEYDOWN, PX_Object_PrinterOnEnter, pObject);
	PX_ObjectRegisterEvent(pDesc->Area, PX_OBJECT_EVENT_CURSORDOWN, PX_Object_PrinterOnMouseDown, pObject);
	PX_Object_ScrollAreaSetBorder(pDesc->Area, PX_NULL);

	if (!(pDesc->Input = PX_Object_EditCreate(mp, (pDesc->Area), 0, 0, (px_int)width, fmheight, fm))) return PX_NULL;
	PX_Object_EditSetTextColor(pDesc->Input, pDesc->fontColor);
	PX_Object_EditSetCursorColor(pDesc->Input, pDesc->fontColor);
	PX_Object_EditSetTextColor(pDesc->Input, pDesc->fontColor);
	PX_Object_EditSetBorderColor(pDesc->Input, pDesc->fontColor);
	PX_Object_EditSetOffset(pDesc->Input, 2, 3);
	//PX_Object_EditSetLimit(pDesc->Input, "zxcvbnm,./asdfghjkl;'qwertyyuiop[]\\`1234567890-=ZXCVBNM<>?ASDFGHJKL:\"QWERTYUIOP{}|~!@#$%^&*()_+");
	PX_Object_ScrollAreaSetBkColor(pDesc->Area, PX_COLOR(255, 0, 0, 0));
	
	PX_VectorInitialize(mp, &pDesc->pObjects, sizeof(PX_Object_PrinterLine), PX_OBJECT_PRINTER_DEFAULT_MAX_COLUMN);

	return pObject;
}


//set font color
px_void PX_Object_PrinterSetFontColor(PX_Object* pObject, px_color color)
{
	PX_Object_Printer* pDesc = PX_ObjectGetDesc(PX_Object_Printer, pObject);
	pDesc->fontColor = color;
	PX_Object_EditSetTextColor(pDesc->Input, pDesc->fontColor);
	PX_Object_EditSetCursorColor(pDesc->Input, pDesc->fontColor);
	PX_Object_EditSetTextColor(pDesc->Input, pDesc->fontColor);
	PX_Object_EditSetBorderColor(pDesc->Input, pDesc->fontColor);
}


/////////////////////////////////////////////////////////
//Printer
/////////////////////////////////////////////////////////
PX_Object* PX_Designer_PrinterCreate(px_memorypool* mp, PX_Object* pparent, px_float x, px_float y, px_float width, px_float height, px_void* ptr)
{
	PX_FontModule* fm = (PX_FontModule*)ptr;
	return PX_Object_PrinterCreate(mp, pparent, (px_int)x, (px_int)y, (px_int)width, (px_int)height, fm);
}

PX_Designer_ObjectDesc PX_Object_PrinterDesignerInstall()
{
	PX_Designer_ObjectDesc printer;
	px_int i = 0;
	PX_memset(&printer, 0, sizeof(printer));
	PX_strcat(printer.Name, "printer");

	printer.createfunc = PX_Designer_PrinterCreate;
	printer.type = PX_DESIGNER_OBJECT_TYPE_UI;

	PX_strcat(printer.properties[i].Name, "id");
	printer.properties[i].getstring = PX_Designer_GetID;
	printer.properties[i].setstring = PX_Designer_SetID;
	i++;

	PX_strcat(printer.properties[i].Name, "x");
	printer.properties[i].getfloat = PX_Designer_GetX;
	printer.properties[i].setfloat = PX_Designer_SetX;
	i++;

	PX_strcat(printer.properties[i].Name, "y");
	printer.properties[i].getfloat = PX_Designer_GetY;
	printer.properties[i].setfloat = PX_Designer_SetY;
	i++;

	PX_strcat(printer.properties[i].Name, "width");
	printer.properties[i].getfloat = PX_Designer_GetWidth;
	printer.properties[i].setfloat = PX_Designer_SetWidth;
	i++;

	PX_strcat(printer.properties[i].Name, "height");
	printer.properties[i].getfloat = PX_Designer_GetHeight;
	printer.properties[i].setfloat = PX_Designer_SetHeight;
	i++;

	return printer;
}
