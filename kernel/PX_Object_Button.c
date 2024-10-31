#include "PX_Object_Button.h"

PX_Object_Button* PX_Object_GetButton(PX_Object* pObject)
{
	PX_Object_Button* pdesc = (PX_Object_Button*)PX_ObjectGetDescByType(pObject, PX_OBJECT_TYPE_BUTTON);
	PX_ASSERTIF(pdesc == PX_NULL);
	return pdesc;
}


PX_OBJECT_EVENT_FUNCTION(PX_Object_ButtonOnMouseMove)
{
	PX_Object_Button *pButton=PX_Object_GetButton(pObject);
	px_float x,y;

	x=PX_Object_Event_GetCursorX(e);
	y=PX_Object_Event_GetCursorY(e);

	if (pButton)
	{
		if(PX_ObjectIsPointInRegion(pObject,(px_float)x,(px_float)y))
		{
			if (pButton->state!=PX_OBJECT_BUTTON_STATE_ONPUSH)
			{
				if (pButton->state!=PX_OBJECT_BUTTON_STATE_ONCURSOR)
				{
					PX_Object_Event e = {0};
					e.Event=PX_OBJECT_EVENT_CURSOROVER;
					PX_ObjectExecuteEvent(pObject,e);
				}
				pButton->state=PX_OBJECT_BUTTON_STATE_ONCURSOR;
			}
		}
		else
		{
			if (pButton->state!=PX_OBJECT_BUTTON_STATE_NORMAL)
			{
				PX_Object_Event e = {0};
				e.Event=PX_OBJECT_EVENT_CURSOROUT;
				PX_ObjectExecuteEvent(pObject,e);
			}
			pButton->state=PX_OBJECT_BUTTON_STATE_NORMAL;
		}
	}
}

PX_OBJECT_EVENT_FUNCTION(PX_Object_ButtonOnMouseLButtonDown)
{
	PX_Object_Button *pButton=PX_Object_GetButton(pObject);
	px_float x,y;

	x=PX_Object_Event_GetCursorX(e);
	y=PX_Object_Event_GetCursorY(e);

	if (pButton)
	{
		if(PX_ObjectIsPointInRegion(pObject,(px_float)x,(px_float)y))
			pButton->state=PX_OBJECT_BUTTON_STATE_ONPUSH;
		else
		{
			if (pButton->state!=PX_OBJECT_BUTTON_STATE_NORMAL)
			{
				PX_Object_Event e = {0};
				e.Event=PX_OBJECT_EVENT_CURSOROUT;
				PX_ObjectExecuteEvent(pObject,e);
			}
			pButton->state=PX_OBJECT_BUTTON_STATE_NORMAL;
		}
	}
}

PX_OBJECT_EVENT_FUNCTION(PX_Object_ButtonOnMouseLButtonUp)
{
	PX_Object_Button *pButton=PX_Object_GetButton(pObject);
	px_float x,y;

	x=PX_Object_Event_GetCursorX(e);
	y=PX_Object_Event_GetCursorY(e);

	if (pButton)
	{
		if(PX_ObjectIsPointInRegion(pObject,(px_float)x,(px_float)y))
			if(pButton->state==PX_OBJECT_BUTTON_STATE_ONPUSH)
			{
				pButton->state=PX_OBJECT_BUTTON_STATE_ONCURSOR;
				e.Event=PX_OBJECT_EVENT_EXECUTE;
				PX_ObjectExecuteEvent(pObject,e);
			}
	}
}

PX_OBJECT_RENDER_FUNCTION(PX_Object_ButtonRender)
{
	PX_Object_Button* pButton = PX_ObjectGetDesc(PX_Object_Button, pObject);
	px_float objx, objy, objWidth, objHeight;
	px_float inheritX, inheritY;

	PX_ObjectGetInheritXY(pObject, &inheritX, &inheritY);

	objx = (pObject->x + inheritX);
	objy = (pObject->y + inheritY);
	objWidth = pObject->Width;
	objHeight = pObject->Height;

	if (pButton == PX_NULL)
	{
		return;
	}

	if (!pObject->Visible)
	{
		return;
	}

	if (!pObject->Enabled)
	{
		pButton->state = PX_OBJECT_BUTTON_STATE_NORMAL;
	}

	if (pObject->diameter != 0)
	{
		switch (pButton->state)
		{
		case PX_OBJECT_BUTTON_STATE_NORMAL:
			break;
		case PX_OBJECT_BUTTON_STATE_ONPUSH:
			PX_GeoDrawBall(psurface, objx, objy, (pObject->diameter / 2), pButton->PushColor);
			break;
		case PX_OBJECT_BUTTON_STATE_ONCURSOR:
			PX_GeoDrawBall(psurface, objx, objy, (pObject->diameter / 2), pButton->CursorColor);
			break;
		}
	}
	else
	{
		switch (pButton->state)
		{
		case PX_OBJECT_BUTTON_STATE_NORMAL:
			break;
		case PX_OBJECT_BUTTON_STATE_ONPUSH:
			PX_GeoDrawRect(psurface, (px_int)objx, (px_int)objy, (px_int)objx + (px_int)objWidth - 1, (px_int)objy + (px_int)objHeight - 1, pButton->PushColor);
			break;
		case PX_OBJECT_BUTTON_STATE_ONCURSOR:

			PX_GeoDrawRect(psurface, (px_int)objx, (px_int)objy, (px_int)objx + (px_int)objWidth - 1, (px_int)objy + (px_int)objHeight - 1, pButton->CursorColor);
			break;
		}
	}
}

PX_Object* PX_Object_ButtonAttachObject( PX_Object* pObject,px_int attachIndex,px_color cursor_color, px_color push_color)
{
	px_memorypool* mp=pObject->mp;

	PX_Object_Button* pButton;

	PX_ASSERTIF(pObject == PX_NULL);
	PX_ASSERTIF(attachIndex < 0 || attachIndex >= PX_COUNTOF(pObject->pObjectDesc));
	PX_ASSERTIF(pObject->pObjectDesc[attachIndex] != PX_NULL);
	pButton = (PX_Object_Button*)PX_ObjectCreateDesc(pObject, attachIndex, PX_OBJECT_TYPE_BUTTON, 0, PX_Object_ButtonRender, 0, 0, sizeof(PX_Object_Button));
	PX_ASSERTIF(pButton == PX_NULL);

	pButton->CursorColor = cursor_color;
	pButton->PushColor = push_color;
	pButton->state = PX_OBJECT_BUTTON_STATE_NORMAL;

	PX_ObjectRegisterEventEx(pObject, PX_OBJECT_EVENT_CURSORMOVE, PX_OBJECT_TYPE_BUTTON, PX_Object_ButtonOnMouseMove, PX_NULL);
	PX_ObjectRegisterEventEx(pObject, PX_OBJECT_EVENT_CURSORDRAG, PX_OBJECT_TYPE_BUTTON, PX_Object_ButtonOnMouseMove, PX_NULL);
	PX_ObjectRegisterEventEx(pObject, PX_OBJECT_EVENT_CURSORDOWN, PX_OBJECT_TYPE_BUTTON, PX_Object_ButtonOnMouseLButtonDown, PX_NULL);
	PX_ObjectRegisterEventEx(pObject, PX_OBJECT_EVENT_CURSORUP, PX_OBJECT_TYPE_BUTTON, PX_Object_ButtonOnMouseLButtonUp, PX_NULL);

	return pObject;
}



px_void PX_Object_ButtonSetCursorColor(PX_Object *pObject,px_color Color)
{
	PX_Object_Button * pButton=PX_Object_GetButton(pObject);
	if (pButton)
	{

		pButton->CursorColor=Color;
	}
}


px_void PX_Object_ButtonSetPushColor(PX_Object *pObject,px_color Color)
{
	PX_Object_Button * pButton=PX_Object_GetButton(pObject);
	if (pButton)
	{
		pButton->PushColor=Color;
	}
}

