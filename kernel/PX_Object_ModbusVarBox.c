#include "PX_Object_ModbusVarBox.h"


PX_Object_ModbusVarBox* PX_Object_GetModbusVarBox(PX_Object* pObject)
{
	if (pObject->Type==PX_OBJECT_TYPE_MODBUSVARBOX)
	{
		return PX_ObjectGetDesc(PX_Object_ModbusVarBox, pObject);
	}
	return PX_NULL;
}

px_void PX_Object_ModbusVarBoxOnConfirm(PX_Object* pObject, PX_Object_Event e, px_void* ptr)
{
	PX_Object_ModbusVarBox* pdesc = PX_ObjectGetDesc(PX_Object_ModbusVarBox, ((PX_Object*)ptr));
	if (!*PX_Object_VariousGetText(pdesc->various_name))
	{
		return;
	}
	if (!*PX_Object_VariousGetText(pdesc->various_address))
	{
		return;
	}
	if (PX_strlen(PX_Object_VariousGetText(pdesc->various_address))>4)
	{
		return;
	}

	if(pdesc->EditMode)
		PX_ObjectExecuteEvent(((PX_Object*)ptr), PX_OBJECT_BUILD_EVENT(PX_OBJECT_EVENT_VALUECHANGED));
	else
		PX_ObjectExecuteEvent(((PX_Object*)ptr), PX_OBJECT_BUILD_EVENT(PX_OBJECT_EVENT_EXECUTE));
	PX_Object_ModbusVarBoxClose((PX_Object*)ptr);
}

px_void PX_Object_ModbusVarBoxOnKeyDown(PX_Object* pObject, PX_Object_Event e, px_void* ptr)
{
	if (e.Event == PX_OBJECT_EVENT_KEYDOWN && PX_Object_Event_GetKeyDown(e) == '\r')
	{
		PX_Object_ModbusVarBoxOnConfirm(pObject, PX_OBJECT_BUILD_EVENT(PX_OBJECT_EVENT_EXECUTE), ptr);
	}
}


px_void PX_Object_ModbusVarBoxOnCancel(PX_Object* pObject, PX_Object_Event e, px_void* ptr)
{
	PX_Object_ModbusVarBoxClose((PX_Object*)ptr);
}

px_void PX_Object_ModbusVarBoxOnTypeChanged(PX_Object* pObject, PX_Object_Event e, px_void* ptr)
{
	PX_Object_ModbusVarBox* pDesc = (PX_Object_ModbusVarBox*)ptr;
}

PX_Object* PX_Object_ModbusVarBoxCreate(px_memorypool* mp, PX_Object* Parent, int x, int y, int width, int height, const px_char title[], PX_FontModule* fontmodule)
{
	PX_Object_ModbusVarBox desc, * pdesc;
	PX_Object* pObject;
	pObject = PX_ObjectCreateEx(mp, Parent, (px_float)x, (px_float)y, 0, (px_float)width, (px_float)height, 0, PX_OBJECT_TYPE_MODBUSVARBOX, PX_NULL, PX_NULL, PX_NULL, &desc, sizeof(desc));
	pdesc = PX_ObjectGetDesc(PX_Object_ModbusVarBox, pObject);
	pdesc->widget = PX_Object_WidgetCreate(mp, pObject, 0, 0, width, height, "", fontmodule);
	PX_Object_WidgetShowHideCloseButton(pdesc->widget, PX_FALSE);
	pdesc->various_name = PX_Object_VariousCreate(mp, pdesc->widget, width / 2 - 128, 32, 256, 32, "Name:",PX_OBJECT_VARIOUS_TYPE_EDIT, fontmodule);
	PX_Object_VariousSetEditStyle(pdesc->various_name, PX_OBJECT_VARIOUS_EDIT_TYPE_STRING);
	pdesc->various_address = PX_Object_VariousCreate(mp, pdesc->widget, width / 2 - 128, 72, 256, 32, "Address:", PX_OBJECT_VARIOUS_TYPE_EDIT, fontmodule);
	PX_Object_VariousSetEditStyle(pdesc->various_address, PX_OBJECT_VARIOUS_EDIT_TYPE_HEX);

	pdesc->various_value = PX_Object_VariousCreate(mp, pdesc->widget, width / 2 - 128, 112, 256, 32, "Value:", PX_OBJECT_VARIOUS_TYPE_EDIT, fontmodule);
	PX_Object_VariousSetEditStyle(pdesc->various_value, PX_OBJECT_VARIOUS_EDIT_TYPE_HEX);

	pdesc->selectbar_type = PX_Object_SelectBarCreate(mp, pdesc->widget, width / 2 - 128, 152, 256, 32, fontmodule);
	PX_Object_SelectBarSetMaxDisplayCount(pdesc->selectbar_type, 5);

	PX_Object_SelectBarAddItem(pdesc->selectbar_type, "Type: Boolean");		    //0
	PX_Object_SelectBarAddItem(pdesc->selectbar_type, "Type: Register");		//1

	PX_ObjectRegisterEvent(pdesc->selectbar_type, PX_OBJECT_EVENT_VALUECHANGED, PX_Object_ModbusVarBoxOnTypeChanged, pdesc);

	pdesc->checkbox_readonly = PX_Object_CheckBoxCreate(mp, pdesc->widget, width / 2 - 128, 192, 256, 32, "Read Only", fontmodule);
	pdesc->btn_ok = PX_Object_PushButtonCreate(mp, pdesc->widget, width - 256, height - 68, 96, 32, "OK", fontmodule);
	pdesc->btn_close = PX_Object_PushButtonCreate(mp, pdesc->widget, width - 128, height - 68, 96, 32, "Cancel", fontmodule);

	PX_ObjectRegisterEvent(pdesc->btn_ok, PX_OBJECT_EVENT_EXECUTE, PX_Object_ModbusVarBoxOnConfirm, pObject);
	PX_ObjectRegisterEvent(pdesc->btn_close, PX_OBJECT_EVENT_EXECUTE, PX_Object_ModbusVarBoxOnCancel, pObject);
	PX_ObjectRegisterEvent(pdesc->widget, PX_OBJECT_EVENT_KEYDOWN, PX_Object_ModbusVarBoxOnKeyDown, pObject);
	pdesc->widget->Visible = PX_TRUE;
	pdesc->EditMode = PX_FALSE;
	pObject->Visible = PX_FALSE;
	return pObject;
}

px_void PX_Object_ModbusVarBoxShow(PX_Object* pObject)
{
	
	PX_Object_ModbusVarBox* pDesc = PX_ObjectGetDesc(PX_Object_ModbusVarBox,pObject);
	if (pObject->Type != PX_OBJECT_TYPE_MODBUSVARBOX)
	{
		PX_ASSERT();
		return;
	}
	pDesc->various_address->Enabled = PX_TRUE;
	pDesc->various_name->Enabled = PX_TRUE;
	pDesc->checkbox_readonly->Enabled = PX_TRUE;

	pDesc->various_value->Visible = PX_FALSE;
	pDesc->EditMode = PX_FALSE;
	pObject->Visible = PX_TRUE;

	PX_ObjectSetFocus(pObject);
}

px_void PX_Object_ModbusVarBoxShowEdit(PX_Object* pObject,const px_char name[],px_dword address,px_int type,px_word value)
{
	PX_Object_ModbusVarBox* pDesc = PX_ObjectGetDesc(PX_Object_ModbusVarBox, pObject);
	if (pObject->Type != PX_OBJECT_TYPE_MODBUSVARBOX)
	{
		PX_ASSERT();
		return;
	}

	PX_Object_VariousSetText(pDesc->various_name, name);
	PX_Object_VariousSetHex(pDesc->various_address, address);
	PX_Object_VariousSetHex(pDesc->various_value, value);
	PX_Object_SelectBarSetCurrentIndex(pDesc->selectbar_type, type);

	pDesc->various_address->Enabled = PX_FALSE;
	pDesc->various_name->Enabled = PX_FALSE;
	pDesc->checkbox_readonly->Enabled = PX_FALSE;
	pDesc->EditMode = PX_TRUE;
	pDesc->various_value->Visible = PX_TRUE;

	pObject->Visible = PX_TRUE;
	PX_ObjectSetFocus(pObject);
}

px_void PX_Object_ModbusVarBoxClose(PX_Object* pObject)
{
	if (pObject->Type != PX_OBJECT_TYPE_MODBUSVARBOX)
	{
		PX_ASSERT();
		return;
	}
	pObject->Visible = PX_FALSE;
	PX_ObjectClearFocus(pObject);
}

px_void PX_Object_ModbusVarBoxHide(PX_Object* pObject)
{
	if (pObject->Type != PX_OBJECT_TYPE_MODBUSVARBOX)
	{
		PX_ASSERT();
		return;
	}
	pObject->Visible = PX_FALSE;
	PX_ObjectClearFocus(pObject);
}