#include "PX_Object_IPBox.h"

PX_Object_IPBox* PX_Object_GetIPBox(PX_Object* pObject)
{
	return (PX_Object_IPBox*)PX_ObjectGetDescByType(pObject, PX_OBJECT_TYPE_IPBOX);
}

px_void PX_Object_IPBoxOnConfirm(PX_Object* pObject, PX_Object_Event e, px_void* ptr)
{
	PX_Object_IPBox* pdesc = PX_Object_GetIPBox((PX_Object*)ptr);
	PX_ASSERTIF(!pdesc);
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
	PX_Object_IPBox* pdesc = PX_Object_GetIPBox((PX_Object*)ptr);
	PX_ASSERTIF(!pdesc);

	PX_Object_VariousSetTextColor(pdesc->various_ip, PX_OBJECT_UI_DEFAULT_FONTCOLOR);
}

PX_Object* PX_Object_IPBoxAttachObject(PX_Object* pObject,px_int attachIndex,px_int x, px_int y, px_int width, px_int height, const px_char title[], PX_FontModule* fontmodule)
{
	px_memorypool* mp=pObject->mp;
	PX_Object_IPBox * pdesc;

	PX_ASSERTIF(pObject == PX_NULL);
	PX_ASSERTIF(attachIndex < 0 || attachIndex >= PX_COUNTOF(pObject->pObjectDesc));
	PX_ASSERTIF(pObject->pObjectDesc[attachIndex] != PX_NULL);
	pdesc = (PX_Object_IPBox*)PX_ObjectCreateDesc(pObject, attachIndex, PX_OBJECT_TYPE_IPBOX, 0, 0, 0, 0, sizeof(PX_Object_IPBox));
	PX_ASSERTIF(pdesc == PX_NULL);

	pdesc->widget = PX_Object_WidgetCreate(mp, pObject, 0, 0, width, height, title, fontmodule);
	PX_Object_WidgetShowHideCloseButton(pdesc->widget, PX_FALSE);
	pdesc->various_ip = PX_Object_VariousCreate(mp, pdesc->widget, width / 2 - 128, height / 2 - 64, 256, 32, "IP:", PX_OBJECT_VARIOUS_TYPE_EDIT, fontmodule);
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
PX_Object* PX_Object_IPBoxCreate(px_memorypool* mp, PX_Object* Parent, px_int x, px_int y, px_int width, px_int height, const px_char title[], PX_FontModule* fontmodule)
{
	PX_Object* pObject;
	pObject = PX_ObjectCreate(mp, Parent, (px_float)x, (px_float)y, 0, (px_float)width, (px_float)height, 0);
	if (!pObject)
	{
		return PX_NULL;
	}
	if (!PX_Object_IPBoxAttachObject(pObject, 0, 0, 0, width, height, title, fontmodule))
	{
		PX_ObjectDelete(pObject);
		return PX_NULL;
	}
	
	return pObject;
}

px_void PX_Object_IPBoxShow(PX_Object* pObject)
{
	PX_ASSERTIF(PX_Object_GetIPBox(pObject) == PX_NULL);
	pObject->Visible = PX_TRUE;
	PX_ObjectSetFocus(pObject);
}

px_void PX_Object_IPBoxClose(PX_Object* pObject)
{
	PX_ASSERTIF(PX_Object_GetIPBox(pObject) == PX_NULL);
	pObject->Visible = PX_FALSE;
	PX_ObjectClearFocus(pObject);
}

px_void PX_Object_IPBoxSetDefault(PX_Object* pObject, const px_char ip[], const px_char port[])
{
	PX_ASSERTIF(PX_Object_GetIPBox(pObject) == PX_NULL);
	PX_Object_VariousSetText(PX_Object_GetIPBox(pObject)->various_ip, ip);
	PX_Object_VariousSetText(PX_Object_GetIPBox(pObject)->various_port, port);
	return;
}

px_void PX_Object_IPBoxHide(PX_Object* pObject)
{
	PX_ASSERTIF(PX_Object_GetIPBox(pObject) == PX_NULL);
	pObject->Visible = PX_FALSE;
	PX_ObjectClearFocus(pObject);
}