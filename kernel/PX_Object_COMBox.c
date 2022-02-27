#include "PX_Object_COMBox.h"


PX_Object_COMBox_Return PX_Object_COMBoxGetReturn(PX_Object *pObject)
{
	PX_Object_COMBox_Return info;
	PX_Object_COMBox* pDesc;

	if (pObject->Type!=PX_OBJECT_TYPE_COMBOX)
	{
		PX_ASSERT();
		PX_zeromemory(&info, sizeof(info));
		return info;
	}
	pDesc = PX_ObjectGetDesc(PX_Object_COMBox, pObject);
	PX_strcpy(info.com, PX_Object_VariousGetText(pDesc->various_com),sizeof(info.com));
	switch (PX_Object_VariousSelectBarGetCurrentIndex(pDesc->various_baudrate))
	{
	case 0:
		info.baudRate=300;
		break;
	case 1:
		info.baudRate=600;
		break;
	case 2:
		info.baudRate=1200;
		break;
	case 3:
		info.baudRate=2400;
		break;
	case 4:
		info.baudRate=4800;
		break;
	case 5:
		info.baudRate=9600;
		break;
	case 6:
		info.baudRate=19200;
		break;
	case 7:
		info.baudRate=38400;
		break;
	case 8:
		info.baudRate=56000;
		break;
	case 9:
		info.baudRate=57600;
		break;
	case 10:
		info.baudRate=115200;
		break;
	default:
		info.baudRate=128000;
		break;
	}

	switch(PX_Object_SelectBarGetCurrentIndex(pDesc->various_databits))
	{
	case 0:
		info.DataBits=5;
		break;
	case 1:
		info.DataBits=6;
		break;
	case 2:
		info.DataBits=7;
		break;
	case 3:
		info.DataBits=8;
		break;
	default:
		info.DataBits=8;
		break;
	}

	switch(PX_Object_SelectBarGetCurrentIndex(pDesc->various_stopbits))
	{
	case 0:
		info.stopBit=1;
		break;
	case 1:
		info.stopBit=1;
		break;
	case 2:
		info.stopBit=2;
		break;
	default:
		info.stopBit=1;
		break;
	}

	switch(PX_Object_SelectBarGetCurrentIndex(pDesc->various_parity))
	{
	case 0:
		info.ParityType='N';
		break;
	case 1:
		info.ParityType='E';
		break;
	case 2:
		info.ParityType='O';
		break;
	case 3:
		info.ParityType='M';
		break;
	default:
		info.ParityType='N';
		break;
	}
	return info;
}

px_void PX_Object_COMBoxOnConfirm(PX_Object* pObject, PX_Object_Event e, px_void* ptr)
{
	PX_Object_COMBox* pdesc = PX_ObjectGetDesc(PX_Object_COMBox, ((PX_Object*)ptr));

	PX_ObjectExecuteEvent(((PX_Object*)ptr), PX_OBJECT_BUILD_EVENT(PX_OBJECT_EVENT_EXECUTE));
	PX_Object_COMBoxClose((PX_Object*)ptr);
}


px_void PX_Object_COMBoxOnCancel(PX_Object* pObject, PX_Object_Event e, px_void* ptr)
{
	PX_Object_COMBoxClose((PX_Object*)ptr);
}




PX_Object_COMBox* PX_Object_GetCOMBox(PX_Object* pObject)
{
	if (pObject->Type==PX_OBJECT_TYPE_COMBOX)
	{
		return PX_ObjectGetDesc(PX_Object_COMBox, pObject);
	}
	return PX_NULL;
}

PX_Object* PX_Object_COMBoxCreate(px_memorypool* mp, PX_Object* Parent, int x, int y, int width, int height, const px_char title[], PX_FontModule* fontmodule)
{
	PX_Object_COMBox desc, * pdesc;
	PX_Object* pObject;
	pObject = PX_ObjectCreateEx(mp, Parent, (px_float)x, (px_float)y, 0, (px_float)width, (px_float)height, 0, PX_OBJECT_TYPE_COMBOX, PX_NULL, PX_NULL, PX_NULL, &desc, sizeof(desc));
	pdesc = PX_ObjectGetDesc(PX_Object_COMBox, pObject);

	pdesc->widget = PX_Object_WidgetCreate(mp, pObject, 0, 0, width, height, "", fontmodule);
	PX_Object_WidgetShowHideCloseButton(pdesc->widget, PX_FALSE);

	pdesc->various_com = PX_Object_VariousCreate(mp, pdesc->widget, width / 2 - 128, 32, 256, 32, "COM:", PX_OBJECT_VARIOUS_TYPE_SELECTBAR, fontmodule);
	pdesc->various_baudrate = PX_Object_VariousCreate(mp, pdesc->widget, width / 2 - 128, 72, 256, 32, "BaudRate:", PX_OBJECT_VARIOUS_TYPE_SELECTBAR, fontmodule);
	PX_Object_VariousAddItem(pdesc->various_baudrate, "300");
	PX_Object_VariousAddItem(pdesc->various_baudrate, "600");
	PX_Object_VariousAddItem(pdesc->various_baudrate, "1200");
	PX_Object_VariousAddItem(pdesc->various_baudrate, "2400");
	PX_Object_VariousAddItem(pdesc->various_baudrate, "4800");
	PX_Object_VariousAddItem(pdesc->various_baudrate, "9600");
	PX_Object_VariousAddItem(pdesc->various_baudrate, "19200");
	PX_Object_VariousAddItem(pdesc->various_baudrate, "38400");
	PX_Object_VariousAddItem(pdesc->various_baudrate, "56000");
	PX_Object_VariousAddItem(pdesc->various_baudrate, "57600");
	PX_Object_VariousAddItem(pdesc->various_baudrate, "115200");

	pdesc->various_databits = PX_Object_VariousCreate(mp, pdesc->widget, width / 2 - 128, 112, 256, 32, "DataBits:", PX_OBJECT_VARIOUS_TYPE_SELECTBAR, fontmodule);
	PX_Object_VariousAddItem(pdesc->various_databits, "5");
	PX_Object_VariousAddItem(pdesc->various_databits, "6");
	PX_Object_VariousAddItem(pdesc->various_databits, "7");
	PX_Object_VariousAddItem(pdesc->various_databits, "8");


	pdesc->various_stopbits = PX_Object_VariousCreate(mp, pdesc->widget, width / 2 - 128, 152, 256, 32, "StopBits:", PX_OBJECT_VARIOUS_TYPE_SELECTBAR, fontmodule);
	PX_Object_VariousAddItem(pdesc->various_stopbits, "0");
	PX_Object_VariousAddItem(pdesc->various_stopbits, "1");
	PX_Object_VariousAddItem(pdesc->various_stopbits, "2");

	pdesc->various_parity = PX_Object_VariousCreate(mp, pdesc->widget, width / 2 - 128, 192, 256, 32, "Parity:", PX_OBJECT_VARIOUS_TYPE_SELECTBAR, fontmodule);
	PX_Object_VariousAddItem(pdesc->various_parity, "N");
	PX_Object_VariousAddItem(pdesc->various_parity, "E");
	PX_Object_VariousAddItem(pdesc->various_parity, "O");
	PX_Object_VariousAddItem(pdesc->various_parity, "M");

	pdesc->btn_ok = PX_Object_PushButtonCreate(mp, pdesc->widget, width - 256, height - 68, 96, 32, "OK", fontmodule);
	pdesc->btn_close = PX_Object_PushButtonCreate(mp, pdesc->widget, width - 128, height - 68, 96, 32, "Cancel", fontmodule);

	PX_ObjectRegisterEvent(pdesc->btn_ok, PX_OBJECT_EVENT_EXECUTE, PX_Object_COMBoxOnConfirm, pObject);
	PX_ObjectRegisterEvent(pdesc->btn_close, PX_OBJECT_EVENT_EXECUTE, PX_Object_COMBoxOnCancel, pObject);

	pdesc->widget->Visible = PX_TRUE;
	pObject->Visible = PX_FALSE;
	return pObject;
}

px_void PX_Object_COMBoxShow(PX_Object* pObject)
{
	pObject->Visible = PX_TRUE;
	PX_ObjectSetFocus(pObject);
}

px_void PX_Object_COMBoxClose(PX_Object* pObject)
{
	pObject->Visible = PX_FALSE;
	PX_ObjectClearFocus(pObject);
}
