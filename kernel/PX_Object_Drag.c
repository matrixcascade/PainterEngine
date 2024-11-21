#include "PX_Object_Drag.h"


PX_OBJECT_EVENT_FUNCTION(PX_Object_DragOnCursorDown)
{
	PX_Object_Drag* pPX_Object_Drag = PX_ObjectGetDescByType(pObject, PX_OBJECT_TYPE_DRAG);
	if (PX_ObjectIsCursorInRegion(pObject, e))
	{
		pPX_Object_Drag->bselect = PX_TRUE;
		pPX_Object_Drag->last_cursorx = PX_Object_Event_GetCursorX(e);
		pPX_Object_Drag->last_cursory = PX_Object_Event_GetCursorY(e);
	}
}

PX_OBJECT_EVENT_FUNCTION(PX_Object_DragOnCursorRelease)
{
	PX_Object_Drag* pPX_Object_Drag = PX_ObjectGetDescByType(pObject, PX_OBJECT_TYPE_DRAG);
	pPX_Object_Drag->bselect = PX_FALSE;
}

PX_OBJECT_EVENT_FUNCTION(PX_Object_DragOnCursorDrag)
{
	PX_Object_Drag* pPX_Object_Drag = PX_ObjectGetDescByType(pObject, PX_OBJECT_TYPE_DRAG);
	px_float x = PX_Object_Event_GetCursorX(e);
	px_float y = PX_Object_Event_GetCursorY(e);
	if (pPX_Object_Drag->bselect)
	{
		pObject->x +=  x- pPX_Object_Drag->last_cursorx;
		pObject->y += y - pPX_Object_Drag->last_cursory;
	}
	pPX_Object_Drag->last_cursorx = x;
	pPX_Object_Drag->last_cursory = y;
}


PX_Object* PX_Object_DragAttachObject(PX_Object* pObject, px_int attachIndex)
{
	PX_Object_Drag* pDesc;


	PX_ASSERTIF(pObject == PX_NULL);
	PX_ASSERTIF(attachIndex < 0 || attachIndex >= PX_COUNTOF(pObject->pObjectDesc));
	PX_ASSERTIF(pObject->pObjectDesc[attachIndex] != PX_NULL);
	pDesc = (PX_Object_Drag*)PX_ObjectCreateDesc(pObject, attachIndex, PX_OBJECT_TYPE_DRAG, 0, 0, 0, 0, sizeof(PX_Object_Drag));
	PX_ASSERTIF(pDesc == PX_NULL);
	PX_ObjectRegisterEvent(pObject, PX_OBJECT_EVENT_CURSORDRAG, PX_Object_DragOnCursorDrag, 0);
	PX_ObjectRegisterEvent(pObject, PX_OBJECT_EVENT_CURSORDOWN, PX_Object_DragOnCursorDown, 0);
	PX_ObjectRegisterEvent(pObject, PX_OBJECT_EVENT_CURSORUP, PX_Object_DragOnCursorRelease, 0);
	PX_ObjectRegisterEvent(pObject, PX_OBJECT_EVENT_CURSORMOVE, PX_Object_DragOnCursorRelease, 0);
	return pObject;
}