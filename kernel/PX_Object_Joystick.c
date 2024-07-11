#include "PX_Object_Joystick.h"

PX_Object_Joystick *PX_Object_GetJoystick(PX_Object *pObject)
{
    return (PX_Object_Joystick *)PX_ObjectGetDescByType(pObject, PX_OBJECT_TYPE_JOYSTICK);
    
}

px_double PX_Object_JoystickGetAngle(PX_Object *pObject)
{
    PX_ASSERTIF(PX_Object_GetJoystick(pObject));
    return PX_Object_GetJoystick(pObject)->Angle;
}

px_double PX_Object_JoystickGetDistance(PX_Object *pObject)
{
    PX_ASSERTIF(PX_Object_GetJoystick(pObject));
    return PX_Object_GetJoystick(pObject)->Distance;
}

px_point2D PX_Object_JoystickGetVector(PX_Object *pObject)
{
    px_point2D p = {0};
    PX_Object_Joystick *pDesc = PX_Object_GetJoystick(pObject);
    PX_ASSERTIF(!pDesc);
    p.x = (pDesc->Distance *(px_float)PX_cosd(pDesc->Angle));
    p.y = (pDesc->Distance *(px_float)PX_sind(pDesc->Angle));
    return p;
}

///////////////////////////////////////////////

PX_OBJECT_RENDER_FUNCTION(Func_JoystickRender)
{
    PX_Object_Joystick* pJoystick = PX_ObjectGetDesc(PX_Object_Joystick, pObject);
    px_float objx, objy, objWidth, objHeight;
    px_float inheritX, inheritY;
    PX_ObjectGetInheritXY(pObject, &inheritX, &inheritY);

    objx = (pObject->x + inheritX);
    objy = (pObject->y + inheritY);
    objWidth = pObject->Width;
    objHeight = pObject->Height;

    PX_GeoDrawSolidCircle(
        psurface,
        (px_int)objx,
        (px_int)objy,
        (px_int)pJoystick->ActionAreaRadius,
        pJoystick->ActionAreaColor);
    PX_GeoDrawBall(
        psurface,
        (px_float)(objx + pJoystick->Distance * PX_cosd(pJoystick->Angle)),
        (px_float)(objy + pJoystick->Distance * PX_sind(pJoystick->Angle)),
        (px_float)(pJoystick->JoystickRadius),
        pJoystick->JoystickColor);
}

PX_OBJECT_EVENT_FUNCTION(Func_JoystickOnCursorDown)
{
    PX_Object_Joystick* pJoystick = PX_Object_GetJoystick(pObject);
    px_float objx, objy, objWidth, objHeight;
    px_float inheritX, inheritY;
    px_float x, y, r;
    PX_ASSERTIF(!pJoystick);
    PX_ObjectGetInheritXY(pObject, &inheritX, &inheritY);

    objx = (pObject->x + inheritX);
    objy = (pObject->y + inheritY);
    objWidth = pObject->Width;
    objHeight = pObject->Height;
    x = PX_Object_Event_GetCursorX(e) - objx,
        y = PX_Object_Event_GetCursorY(e) - objy,
        r = PX_sqrt(x * x + y * y);

    if (pJoystick->IsActive)
    {
        return;
    }

    if (r < pJoystick->SenseRadius)
    {
        pJoystick->IsActive = PX_TRUE;
        pJoystick->Distance = r < pJoystick->ActionAreaRadius ? r : pJoystick->ActionAreaRadius;
        pJoystick->Angle = (px_float)PX_atan2(y, x);
    }
    else
    {
        pJoystick->Distance = 0.0f;
    }
}

PX_OBJECT_EVENT_FUNCTION(Func_JoystickOnCursorDrag)
{
    PX_Object_Joystick* pJoystick = PX_Object_GetJoystick(pObject);
    px_float objx, objy, objWidth, objHeight;
    px_float inheritX, inheritY;
    px_float x, y, r;
    PX_ASSERTIF(!pJoystick);
    PX_ObjectGetInheritXY(pObject, &inheritX, &inheritY);

    objx = (pObject->x + inheritX);
    objy = (pObject->y + inheritY);
    objWidth = pObject->Width;
    objHeight = pObject->Height;

    x = PX_Object_Event_GetCursorX(e) - objx,
        y = PX_Object_Event_GetCursorY(e) - objy,
        r = PX_sqrt(x * x + y * y);

    if (!pJoystick->IsActive)
    {
        return;
    }

    if (r <= pJoystick->SenseRadius)
    {
        pJoystick->Distance = r < pJoystick->ActionAreaRadius ? r : pJoystick->ActionAreaRadius;
    }
    else
    {
        pJoystick->Distance = 0.0f;
        pJoystick->IsActive = PX_FALSE;
        return;
    }

    pJoystick->Angle = (px_float)PX_atan2(y, x);
}

PX_OBJECT_EVENT_FUNCTION(Func_JoystickOnCursorUp)
{
    PX_Object_Joystick* pJoystick = PX_Object_GetJoystick(pObject);
    px_float objx, objy, objWidth, objHeight;
    px_float inheritX, inheritY;
    px_float x, y, r;
    PX_ASSERTIF(!pJoystick);
    PX_ObjectGetInheritXY(pObject, &inheritX, &inheritY);

    objx = (pObject->x + inheritX);
    objy = (pObject->y + inheritY);
    objWidth = pObject->Width;
    objHeight = pObject->Height;

    x = PX_Object_Event_GetCursorX(e) - objx,
        y = PX_Object_Event_GetCursorY(e) - objy,
        r = PX_sqrt(x * x + y * y);

    if (r <= pJoystick->SenseRadius)
    {
        pJoystick->Distance = 0.0f;
        pJoystick->IsActive = PX_FALSE;
    }
}

PX_Object* PX_Object_JoystickAttachObject( PX_Object* pObject,px_int attachIndex,px_float x, px_float y,px_float actionAreaRadius, px_float joystickRadius, px_float senseRadius, px_color actionAreaColor, px_color joystickColor)
{
    px_memorypool* mp=pObject->mp;
    PX_Object_Joystick joystick = {0},*pdesc;
    joystick.ActionAreaColor = actionAreaColor;
    joystick.JoystickColor = joystickColor;
    joystick.Distance = 0.0;
    joystick.Angle = 0.0;
    joystick.ActionAreaRadius = actionAreaRadius;
    joystick.JoystickRadius = joystickRadius;
    joystick.SenseRadius = senseRadius > actionAreaRadius + joystickRadius ? senseRadius : actionAreaRadius + joystickRadius;


    PX_ASSERTIF(pObject == PX_NULL);
    PX_ASSERTIF(attachIndex < 0 || attachIndex >= PX_COUNTOF(pObject->pObjectDesc));
    PX_ASSERTIF(pObject->pObjectDesc[attachIndex] != PX_NULL);
    pdesc = (PX_Object_Joystick*)PX_ObjectCreateDesc(pObject, attachIndex, PX_OBJECT_TYPE_JOYSTICK, 0, Func_JoystickRender, 0, &joystick, sizeof(PX_Object_Joystick));
    PX_ASSERTIF(pdesc == PX_NULL);

    pObject->diameter = 2 * joystick.SenseRadius;

    PX_ObjectRegisterEvent(pObject, PX_OBJECT_EVENT_CURSORDOWN, Func_JoystickOnCursorDown, PX_NULL);
    PX_ObjectRegisterEvent(pObject, PX_OBJECT_EVENT_CURSORDRAG, Func_JoystickOnCursorDrag, PX_NULL);
    PX_ObjectRegisterEvent(pObject, PX_OBJECT_EVENT_CURSORUP, Func_JoystickOnCursorUp, PX_NULL);
    return pObject;
}

PX_Object *PX_Object_JoystickCreate(
    px_memorypool *mp, PX_Object *Parent,
    px_float x, px_float y,
    px_float actionAreaRadius, px_float joystickRadius, px_float senseRadius,
    px_color actionAreaColor, px_color joystickColor)
{

    PX_Object *pObject;
    pObject = PX_ObjectCreate(mp, Parent, x, y, 0, 0.0f, 0.0f, 0.0f);
    if (pObject == PX_NULL)
    {
        return PX_NULL;
    }
    if (!PX_Object_JoystickAttachObject(pObject, 0, x, y, actionAreaRadius, joystickRadius, senseRadius, actionAreaColor, joystickColor))
	{
		PX_ObjectDelete(pObject);
		return PX_NULL;
	}
    return pObject;
}

