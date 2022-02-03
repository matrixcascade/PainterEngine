#include "PX_Object_VarBox.h"


PX_Object_VarBox* PX_Object_GetVarBox(PX_Object* pObject)
{
	if (pObject->Type==PX_OBJECT_TYPE_VARBOX)
	{
		return PX_ObjectGetDesc(PX_Object_VarBox, pObject);
	}
	return PX_NULL;
}

px_void PX_Object_VarBoxOnConfirm(PX_Object* pObject, PX_Object_Event e, px_void* ptr)
{
	PX_Object_VarBox* pdesc = PX_ObjectGetDesc(PX_Object_VarBox, ((PX_Object*)ptr));
	if (!*PX_Object_VariousGetText(pdesc->various_name))
	{
		return;
	}
	if (!*PX_Object_VariousGetText(pdesc->various_address))
	{
		return;
	}
	if (PX_strlen(PX_Object_VariousGetText(pdesc->various_address))>16)
	{
		return;
	}
	if (!*PX_Object_VariousGetText(pdesc->various_size))
	{
		return;
	}
	if (PX_atoi(PX_Object_VariousGetText(pdesc->various_size))==0)
	{
		return;
	}
	
	PX_ObjectExecuteEvent(((PX_Object*)ptr), PX_OBJECT_BUILD_EVENT(PX_OBJECT_EVENT_EXECUTE));
	PX_Object_VarBoxClose((PX_Object*)ptr);
}

px_void PX_Object_VarBoxOnKeyDown(PX_Object* pObject, PX_Object_Event e, px_void* ptr)
{
	if (e.Event == PX_OBJECT_EVENT_KEYDOWN && PX_Object_Event_GetKeyDown(e) == '\r')
	{
		PX_Object_VarBoxOnConfirm(pObject, PX_OBJECT_BUILD_EVENT(PX_OBJECT_EVENT_EXECUTE), ptr);
	}
}


px_void PX_Object_VarBoxOnCancel(PX_Object* pObject, PX_Object_Event e, px_void* ptr)
{
	PX_Object_VarBoxClose((PX_Object*)ptr);
}

px_void PX_Object_VarBoxOnTypeChanged(PX_Object* pObject, PX_Object_Event e, px_void* ptr)
{
	PX_Object_VarBox* pDesc = (PX_Object_VarBox*)ptr;
	switch (PX_Object_SelectBarGetCurrentIndex(pObject))
	{
	case 0:
	case 4:
		PX_Object_VariousSetInt(pDesc->various_size, 1);
		pDesc->various_size->Enabled = PX_FALSE;
		pDesc->various_size->Visible = PX_TRUE;
		break;
	case 1:
	case 5:
		PX_Object_VariousSetInt(pDesc->various_size, 2);
		pDesc->various_size->Enabled = PX_FALSE;
		pDesc->various_size->Visible = PX_TRUE;
		break;
	case 2:
	case 6:
	case 8:
		PX_Object_VariousSetInt(pDesc->various_size, 4);
		pDesc->various_size->Enabled = PX_FALSE;
		pDesc->various_size->Visible = PX_TRUE;
		break;
	case 3:
	case 7:
	case 9:
		PX_Object_VariousSetInt(pDesc->various_size, 8);
		pDesc->various_size->Enabled = PX_FALSE;
		pDesc->various_size->Visible = PX_TRUE;
		break;
	case 10:
	case 11:
		PX_Object_VariousSetInt(pDesc->various_size, 32);
		pDesc->various_size->Enabled = PX_TRUE;
		pDesc->various_size->Visible = PX_TRUE;
		break;
	case 12:
		PX_Object_VariousSetInt(pDesc->various_size, 1);
		pDesc->various_size->Enabled = PX_FALSE;
		pDesc->various_size->Visible = PX_FALSE;
		break;
	default:
		break;
	}
}

PX_Object* PX_Object_VarBoxCreate(px_memorypool* mp, PX_Object* Parent, int x, int y, int width, int height, const px_char title[], PX_FontModule* fontmodule)
{
	PX_Object_VarBox desc, * pdesc;
	PX_Object* pObject;
	pObject = PX_ObjectCreateEx(mp, Parent, (px_float)x, (px_float)y, 0, (px_float)width, (px_float)height, 0, PX_OBJECT_TYPE_VARBOX, PX_NULL, PX_NULL, PX_NULL, &desc, sizeof(desc));
	pdesc = PX_ObjectGetDesc(PX_Object_VarBox, pObject);
	pdesc->widget = PX_Object_WidgetCreate(mp, pObject, 0, 0, width, height, "", fontmodule);
	PX_Object_WidgetShowHideCloseButton(pdesc->widget, PX_FALSE);
	pdesc->various_name = PX_Object_VariousCreate(mp, pdesc->widget, width / 2 - 128, 32, 256, 32, "Name:",PX_OBJECT_VARIOUS_TYPE_EDIT, fontmodule);
	PX_Object_VariousSetEditStyle(pdesc->various_name, PX_OBJECT_VARIOUS_EDIT_TYPE_STRING);
	pdesc->various_address = PX_Object_VariousCreate(mp, pdesc->widget, width / 2 - 128, 72, 256, 32, "Address:", PX_OBJECT_VARIOUS_TYPE_EDIT, fontmodule);
	PX_Object_VariousSetEditStyle(pdesc->various_address, PX_OBJECT_VARIOUS_EDIT_TYPE_HEX);
	pdesc->various_size = PX_Object_VariousCreate(mp, pdesc->widget, width / 2 - 128, 112, 256, 32, "Size:", PX_OBJECT_VARIOUS_TYPE_EDIT, fontmodule);
	PX_Object_VariousSetEditStyle(pdesc->various_size, PX_OBJECT_VARIOUS_EDIT_TYPE_INT);
	PX_Object_VariousSetInt(pdesc->various_size, 1);
	pdesc->various_size->Enabled = PX_FALSE;
	pdesc->selectbar_type = PX_Object_SelectBarCreate(mp, pdesc->widget, width / 2 - 128, 152, 256, 32, fontmodule);
	PX_Object_SelectBarSetMaxDisplayCount(pdesc->selectbar_type, 5);

	PX_Object_SelectBarAddItem(pdesc->selectbar_type, "Type: int8(LE)");		//0
	PX_Object_SelectBarAddItem(pdesc->selectbar_type, "Type: int16(LE)");		//1
	PX_Object_SelectBarAddItem(pdesc->selectbar_type, "Type: int32(LE)");		//2
	PX_Object_SelectBarAddItem(pdesc->selectbar_type, "Type: int64(LE)");		//3
	PX_Object_SelectBarAddItem(pdesc->selectbar_type, "Type: uint8(LE)");		//4
	PX_Object_SelectBarAddItem(pdesc->selectbar_type, "Type: uint16(LE)");		//5
	PX_Object_SelectBarAddItem(pdesc->selectbar_type, "Type: uint32(LE)");		//6
	PX_Object_SelectBarAddItem(pdesc->selectbar_type, "Type: uint64(LE)");		//7
	PX_Object_SelectBarAddItem(pdesc->selectbar_type, "Type: float32(LE IEEE754)");	//8
	PX_Object_SelectBarAddItem(pdesc->selectbar_type, "Type: double64(LE IEEE754)");	//9
	PX_Object_SelectBarAddItem(pdesc->selectbar_type, "Type: string");					//10
	PX_Object_SelectBarAddItem(pdesc->selectbar_type, "Type: hexadecimal");				//11
	PX_Object_SelectBarAddItem(pdesc->selectbar_type, "Type: boolean");					//12
	PX_ObjectRegisterEvent(pdesc->selectbar_type, PX_OBJECT_EVENT_VALUECHANGED, PX_Object_VarBoxOnTypeChanged, pdesc);

	pdesc->checkbox_readonly = PX_Object_CheckBoxCreate(mp, pdesc->widget, width / 2 - 128, 192, 256, 32, "Read Only", fontmodule);
	pdesc->btn_ok = PX_Object_PushButtonCreate(mp, pdesc->widget, width - 256, height - 68, 96, 32, "OK", fontmodule);
	pdesc->btn_close = PX_Object_PushButtonCreate(mp, pdesc->widget, width - 128, height - 68, 96, 32, "Cancel", fontmodule);

	PX_ObjectRegisterEvent(pdesc->btn_ok, PX_OBJECT_EVENT_EXECUTE, PX_Object_VarBoxOnConfirm, pObject);
	PX_ObjectRegisterEvent(pdesc->btn_close, PX_OBJECT_EVENT_EXECUTE, PX_Object_VarBoxOnCancel, pObject);
	PX_ObjectRegisterEvent(pdesc->widget, PX_OBJECT_EVENT_KEYDOWN, PX_Object_VarBoxOnKeyDown, pObject);
	pdesc->widget->Visible = PX_TRUE;
	pObject->Visible = PX_FALSE;
	return pObject;
}

px_void PX_Object_VarBoxShow(PX_Object* pObject)
{
	if (pObject->Type != PX_OBJECT_TYPE_VARBOX)
	{
		PX_ASSERT();
		return;
	}
	pObject->Visible = PX_TRUE;
	PX_ObjectSetFocus(pObject);
}

px_void PX_Object_VarBoxClose(PX_Object* pObject)
{
	if (pObject->Type != PX_OBJECT_TYPE_VARBOX)
	{
		PX_ASSERT();
		return;
	}
	pObject->Visible = PX_FALSE;
	PX_ObjectClearFocus(pObject);
}

px_void PX_Object_VarBoxHide(PX_Object* pObject)
{
	if (pObject->Type != PX_OBJECT_TYPE_VARBOX)
	{
		PX_ASSERT();
		return;
	}
	pObject->Visible = PX_FALSE;
	PX_ObjectClearFocus(pObject);
}