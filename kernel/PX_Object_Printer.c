#include "PX_Object_Printer.h"

px_void PX_Object_PrinterUpdateLines(PX_Object* pObject)
{
	px_int i, y = 10;
	PX_Object_Printer* pDesc = PX_ObjectGetDesc(PX_Object_Printer, pObject);
	PX_Object_PrinterLine* pLine;

	while (pDesc->pObjects.size > pDesc->max_column)
	{
		pLine = (PX_VECTORAT(PX_Object_PrinterLine, &pDesc->pObjects, 0));
		PX_ObjectDelete(pLine->Object);
		PX_VectorErase(&pDesc->pObjects, 0);
	}

	for (i = 0; i < pDesc->pObjects.size; i++)
	{
		pLine = (PX_VECTORAT(PX_Object_PrinterLine, &pDesc->pObjects, i));

		switch (pLine->Object->Type)
		{
		case PX_OBJECT_TYPE_AUTOTEXT:
		{
			PX_ObjectSetPosition(pLine->Object, 0, (px_float)y, 0);
			y += (px_int)PX_ObjectGetHeight(pLine->Object);
		}
		break;
		case PX_OBJECT_TYPE_IMAGE:
		{
			PX_ObjectSetPosition(pLine->Object, 0, (px_float)y, 0);
			y += (px_int)PX_ObjectGetHeight(pLine->Object);
		}
		break;
		case PX_OBJECT_TYPE_ANIMATION:
		{
			PX_ObjectSetPosition(pLine->Object, 0, (px_float)y, 0);
			y += (px_int)PX_ObjectGetHeight(pLine->Object);
		}
		break;
		default:
		{
			PX_ObjectSetPosition(pLine->Object, 0, (px_float)y, 0);
			y += (px_int)PX_ObjectGetHeight(pLine->Object);
		}
		break;
		}
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
			obj.Object = pAutoObject;
			obj.id = pDesc->id++;
			PX_Object_AutoTextSetTextColor(pAutoObject, PX_COLOR(255, 0, 255, 0));
			PX_Object_AutoTextSetText(pAutoObject, text);
			PX_VectorPushback(&pDesc->pObjects, &obj);
			PX_Object_PrinterUpdateLines(pObject);
			PX_Object_ScrollAreaMoveToBottom(pDesc->Area);

		}
		return pAutoObject;
	}
	return PX_NULL;
}

PX_Object* PX_Object_PrinterLastPrintText(PX_Object* pObject, const px_char* text)
{
	if (pObject->Type == PX_OBJECT_TYPE_PRINTER)
	{
		PX_Object_PrinterLine* pobjColumn=PX_NULL;
		PX_Object_Printer* pDesc = PX_ObjectGetDesc(PX_Object_Printer, pObject);
		if (pDesc->pObjects.size == 0)
		{
			return PX_NULL;
		}
		pobjColumn = PX_VECTORLAST(PX_Object_PrinterLine, &pDesc->pObjects);
		pObject = pobjColumn->Object;
		if (pObject->Type == PX_OBJECT_TYPE_AUTOTEXT)
		{
			PX_Object_AutoTextSetTextColor(pObject, PX_COLOR(255, 0, 255, 0));
			PX_Object_AutoTextSetText(pObject, text);
			return pObject;
		}
		else
			return PX_NULL;
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
		pImageObject = PX_Object_ImageCreate(pObject->mp, pDesc->Area, 0, 0, pTexture->width, pTexture->height, pTexture);
		PX_Object_ImageSetAlign(pImageObject, PX_ALIGN_LEFTTOP);
		PX_ObjectSetSize(pImageObject, (px_float)pTexture->width, (px_float)pTexture->height, 0);
		obj.Object = pImageObject;
		obj.id = pDesc->id++;
		PX_VectorPushback(&pDesc->pObjects, &obj);
		PX_Object_PrinterUpdateLines(pObject);

		PX_Object_ScrollAreaMoveToBottom(pDesc->Area);


	}
	return pObject;
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
				return pCc->Object;
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
		PX_ObjectDelete(pCc->Object);
	}
	PX_VectorClear(&pDesc->pObjects);
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
	if (!(pDesc->Area = PX_Object_ScrollAreaCreate(mp, pObject, 0, 0,(px_int)width, (px_int)height))) return PX_NULL;

	PX_ObjectRegisterEvent(pDesc->Area, PX_OBJECT_EVENT_KEYDOWN, PX_Object_PrinterOnEnter, pObject);
	PX_ObjectRegisterEvent(pDesc->Area, PX_OBJECT_EVENT_CURSORDOWN, PX_Object_PrinterOnMouseDown, pObject);
	PX_Object_ScrollAreaSetBorder(pDesc->Area, PX_NULL);

	if (!(pDesc->Input = PX_Object_EditCreate(mp, (pDesc->Area), 0, 0, (px_int)width, fmheight, fm))) return PX_NULL;
	PX_Object_EditSetTextColor(pDesc->Input, PX_COLOR(255, 0, 255, 0));
	PX_Object_EditSetCursorColor(pDesc->Input, PX_COLOR(255, 0, 255, 0));
	PX_Object_EditSetTextColor(pDesc->Input, PX_COLOR(255, 0, 255, 0));
	PX_Object_EditSetBorderColor(pDesc->Input, PX_COLOR(255, 0, 255, 0));
	PX_Object_EditSetOffset(pDesc->Input, 2, 3);
	//PX_Object_EditSetLimit(pDesc->Input, "zxcvbnm,./asdfghjkl;'qwertyyuiop[]\\`1234567890-=ZXCVBNM<>?ASDFGHJKL:\"QWERTYUIOP{}|~!@#$%^&*()_+");
	PX_Object_ScrollAreaSetBkColor(pDesc->Area, PX_COLOR(255, 0, 0, 0));
	pDesc->id = 1;

	PX_VectorInitialize(mp, &pDesc->pObjects, sizeof(PX_Object_PrinterLine), PX_OBJECT_PRINTER_DEFAULT_MAX_COLUMN);

	return pObject;
}



