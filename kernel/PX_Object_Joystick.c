#include "PX_Object_Joystick.h"

PX_Object_Joystick *PX_Object_GetJoystick(PX_Object *pObject)
{
    if (pObject->Type != PX_OBJECT_TYPE_JOYSTICK)
    {
        return PX_NULL;
    }
    return (PX_Object_Joystick *)pObject->pObjectDesc;
}

px_double PX_Object_JoystickGetAngle(PX_Object *pObject)
{
    if (pObject->Type != PX_OBJECT_TYPE_JOYSTICK)
    {
        PX_ASSERT();
    }
    return ((PX_Object_Joystick *)(pObject->pObjectDesc))->Angle;
}

px_double PX_Object_JoystickGetDistance(PX_Object *pObject)
{
    if (pObject->Type != PX_OBJECT_TYPE_JOYSTICK)
    {
        PX_ASSERT();
    }
    return ((PX_Object_Joystick *)(pObject->pObjectDesc))->Distance;
}

px_point2D PX_Object_JoystickGetVector(PX_Object *pObject)
{
    px_point2D p;
    if (pObject->Type != PX_OBJECT_TYPE_JOYSTICK)
    {
        PX_ASSERT();
    }
    else
    {
        p.x = ((PX_Object_Joystick *)(pObject->pObjectDesc))->Distance *
              (px_float)PX_cosd(((PX_Object_Joystick *)(pObject->pObjectDesc))->Angle);
        p.y = ((PX_Object_Joystick *)(pObject->pObjectDesc))->Distance *
              (px_float)PX_sind(((PX_Object_Joystick *)(pObject->pObjectDesc))->Angle);
    }
    return p;
}

///////////////////////////////////////////////

px_void Func_JoystickRender(px_surface *pSurface, PX_Object *pObject, px_uint elapsed);
px_void Func_JoystickOnCursorDown(PX_Object *pObject, PX_Object_Event e, px_void *ptr);
px_void Func_JoystickOnCursorDrag(PX_Object *pObject, PX_Object_Event e, px_void *ptr);
px_void Func_JoystickOnCursorUp(PX_Object *pObject, PX_Object_Event e, px_void *ptr);

PX_Object *PX_Object_JoystickCreate(
    px_memorypool *mp, PX_Object *Parent,
    px_float x, px_float y,
    px_float actionAreaRadius, px_float joystickRadius, px_float senseRadius,
    px_color actionAreaColor, px_color joystickColor)
{

    px_float z = 0.0f;
    PX_Object *pObject;
    PX_Object_Joystick joystick;

    joystick.ActionAreaColor = actionAreaColor;
    joystick.JoystickColor = joystickColor;
    joystick.Distance = 0.0;
    joystick.Angle = 0.0;
    joystick.ActionAreaRadius = actionAreaRadius;
    joystick.JoystickRadius = joystickRadius;
    joystick.SenseRadius = senseRadius > actionAreaRadius + joystickRadius ? senseRadius : actionAreaRadius + joystickRadius;

    pObject = PX_ObjectCreateEx(mp, Parent, x, y, z, 0.0f, 0.0f, 0.0f, PX_OBJECT_TYPE_JOYSTICK, PX_NULL, Func_JoystickRender, PX_NULL, &joystick, sizeof(PX_Object_Joystick));

    if (pObject == PX_NULL)
    {
        return PX_NULL;
    }

    pObject->diameter = 2 * joystick.SenseRadius;

    PX_ObjectRegisterEvent(pObject, PX_OBJECT_EVENT_CURSORDOWN, Func_JoystickOnCursorDown, PX_NULL);
    PX_ObjectRegisterEvent(pObject, PX_OBJECT_EVENT_CURSORDRAG, Func_JoystickOnCursorDrag, PX_NULL);
    PX_ObjectRegisterEvent(pObject, PX_OBJECT_EVENT_CURSORUP, Func_JoystickOnCursorUp, PX_NULL);

    return pObject;
}

px_void Func_JoystickRender(px_surface *pSurface, PX_Object *pObject, px_uint elapsed)
{
    PX_Object_Joystick *pJoystick = (PX_Object_Joystick *)pObject->pObjectDesc;
    px_float objx, objy, objWidth, objHeight;
    px_float inheritX, inheritY;
    PX_ObjectGetInheritXY(pObject, &inheritX, &inheritY);

    objx = (pObject->x + inheritX);
    objy = (pObject->y + inheritY);
    objWidth = pObject->Width;
    objHeight = pObject->Height;

    PX_GeoDrawSolidCircle(
        pSurface,
        (px_int)objx,
        (px_int)objy,
        (px_int)pJoystick->ActionAreaRadius,
        pJoystick->ActionAreaColor);
    PX_GeoDrawBall(
        pSurface,
        (px_float)(objx + pJoystick->Distance * PX_cosd(pJoystick->Angle)),
        (px_float)(objy + pJoystick->Distance * PX_sind(pJoystick->Angle)),
        (px_float)(pJoystick->JoystickRadius),
        pJoystick->JoystickColor);
}

px_void Func_JoystickOnCursorDown(PX_Object *pObject, PX_Object_Event e, px_void *ptr)
{
    PX_Object_Joystick *pJoystick = (PX_Object_Joystick *)pObject->pObjectDesc;
    px_float objx, objy, objWidth, objHeight;
    px_float inheritX, inheritY;
    px_float x, y, r;
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

px_void Func_JoystickOnCursorDrag(PX_Object *pObject, PX_Object_Event e, px_void *ptr)
{
    PX_Object_Joystick *pJoystick = (PX_Object_Joystick *)pObject->pObjectDesc;
    px_float objx, objy, objWidth, objHeight;
    px_float inheritX, inheritY;
    px_float x, y, r;
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

px_void Func_JoystickOnCursorUp(PX_Object *pObject, PX_Object_Event e, px_void *ptr)
{
    PX_Object_Joystick *pJoystick = (PX_Object_Joystick *)pObject->pObjectDesc;
    px_float objx, objy, objWidth, objHeight;
    px_float inheritX, inheritY;
    px_float x, y, r;
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