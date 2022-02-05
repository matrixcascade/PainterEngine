#include "PX_Object_IPBox.h"

PX_Object_IPBox* PX_Object_GetIPBox(PX_Object* pObject)
{
	if (pObject->Type==PX_OBJECT_TYPE_IPBOX)
	{
		return PX_ObjectGetDesc(PX_Object_IPBox, pObject);
	}
	return PX_NULL;
}

px_void PX_Object_IPBoxOnConfirm(PX_Object* pObject, PX_Object_Event e, px_void* ptr)
{
	PX_Object_IPBox* pdesc = PX_ObjectGetDesc(PX_Object_IPBox, ((PX_Object*)ptr));
	if (!PX_IsValidIPAddress(PX_Object_VariousGetText(pdesc->various_ip)))
	{
		PX_Object_VariousSetTextColor(pdesc->various_ip, PX_COLOR_RED);
	}
	else
	{
		PX_ObjectExecuteEvent((PX_Object*)ptr, e);
		PX_Object_IPBoxClose((PX_Object*)ptr);
	}
	
}

px_void PX_Object_IPBoxOnCancel(PX_Object* pObject, PX_Object_Event e, px_void* ptr)
{
	PX_Object_IPBoxClose((PX_Object*)ptr);
}

px_void PX_Object_IPBoxOnEditChanged(PX_Object* pObject, PX_Object_Event e, px_void* ptr)
{
	PX_Object_IPBox* pdesc = PX_ObjectGetDesc(PX_Object_IPBox,((PX_Object *)ptr));

	PX_Object_VariousSetTextColor(pdesc->various_ip, PX_OBJECT_UI_DEFAULT_FONTCOLOR);
}

PX_Object* PX_Object_IPBoxCreate(px_memorypool* mp, PX_Object* Parent, int x, int y, int width, int height, const px_char title[], PX_FontModule* fontmodule)
{
	PX_Object_IPBox desc, * pdesc;
	PX_Object* pObject;
	pObject = PX_ObjectCreateEx(mp, Parent, (px_float)x, (px_float)y, 0, (px_float)width, (px_float)height, 0, PX_OBJECT_TYPE_IPBOX, PX_NULL, PX_NULL, PX_NULL, &desc, sizeof(desc));
	pdesc = PX_ObjectGetDesc(PX_Object_IPBox, pObject);
	pdesc->widget = PX_Object_WidgetCreate(mp, pObject, 0, 0, width, height, title, fontmodule);
	PX_Object_WidgetShowHideCloseButton(pdesc->widget, PX_FALSE);
	pdesc->various_ip = PX_Object_VariousCreate(mp, pdesc->widget , width / 2 - 128, height / 2 - 64, 256, 32, "IP:",PX_OBJECT_VARIOUS_TYPE_EDIT,fontmodule);
	PX_Object_VariousSetEditStyle(pdesc->various_ip, PX_OBJECT_VARIOUS_EDIT_TYPE_STRING);
	pdesc->various_port = PX_Object_VariousCreate(mp, pdesc->widget, width / 2 - 128, height / 2 - 24, 256, 32, "Port:", PX_OBJECT_VARIOUS_TYPE_EDIT, fontmodule);
	PX_Object_VariousSetEditStyle(pdesc->various_port, PX_OBJECT_VARIOUS_EDIT_TYPE_INT);
	pdesc->btn_ok = PX_Object_PushButtonCreate(mp, pdesc->widget, width - 256, height - 68, 96, 32, "OK", fontmodule);
	pdesc->btn_close = PX_Object_PushButtonCreate(mp, pdesc->widget, width - 128, height - 68, 96, 32, "Cancel", fontmodule);

	PX_ObjectRegisterEvent(pdesc->btn_ok, PX_OBJECT_EVENT_EXECUTE, PX_Object_IPBoxOnConfirm, pObject);
	PX_ObjectRegisterEvent(pdesc->btn_close, PX_OBJECT_EVENT_EXECUTE, PX_Object_IPBoxOnCancel, pObject);
	PX_ObjectRegisterEvent(pdesc->various_ip, PX_OBJECT_EVENT_VALUECHANGED, PX_Object_IPBoxOnEditChanged, pObject);

	pdesc->widget->Visible = PX_TRUE;
	pObject->Visible = PX_FALSE;
	return pObject;
}

px_void PX_Object_IPBoxShow(PX_Object* pObject)
{
	if (pObject->Type != PX_OBJECT_TYPE_IPBOX)
	{
		PX_ASSERT();
		return;
	}
	pObject->Visible = PX_TRUE;
	PX_ObjectSetFocus(pObject);
}

px_void PX_Object_IPBoxClose(PX_Object* pObject)
{
	if (pObject->Type != PX_OBJECT_TYPE_IPBOX)
	{
		PX_ASSERT();
		return;
	}
	pObject->Visible = PX_FALSE;
	PX_ObjectClearFocus(pObject);
}

px_void PX_Object_IPBoxSetDefault(PX_Object* pObject, const px_char ip[], const px_char port[])
{
	if (pObject->Type != PX_OBJECT_TYPE_IPBOX)
	{
		PX_ASSERT();
		return;
	}
	PX_Object_VariousSetText(PX_Object_GetIPBox(pObject)->various_ip, ip);
	PX_Object_VariousSetText(PX_Object_GetIPBox(pObject)->various_port, port);
	return;
}

px_void PX_Object_IPBoxHide(PX_Object* pObject)
{
	if (pObject->Type != PX_OBJECT_TYPE_IPBOX)
	{
		PX_ASSERT();
		return;
	}
	pObject->Visible = PX_FALSE;
	PX_ObjectClearFocus(pObject);
}