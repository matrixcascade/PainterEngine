#include "PX_Object_Joystick.h"

PX_Object_Joystick *PX_Object_GetJoystick(PX_Object *pObject)
{
    if (pObject->Type != PX_OBJECT_TYPE_JOYSTICK)
    {
        return PX_NULL;
    }
    return pObject->pObject;
}

px_double PX_Object_JoystickGetAngle(PX_Object *pObject)
{
    if (pObject->Type != PX_OBJECT_TYPE_JOYSTICK)
    {
        return 0.0;
    }
    return ((PX_Object_Joystick *)(pObject->pObject))->Angle;
}

px_double PX_Object_JoystickGetDistance(PX_Object *pObject)
{
    if (pObject->Type != PX_OBJECT_TYPE_JOYSTICK)
    {
        return 0.0;
    }
    return ((PX_Object_Joystick *)(pObject->pObject))->Distance;
}

px_point2D PX_Object_JoystickGetVector(PX_Object *pObject)
{
    px_point2D p;
    if (pObject->Type != PX_OBJECT_TYPE_JOYSTICK)
    {
        p.x = 0.0;
        p.y = 0.0;
    }
    else
    {
        p.x = ((PX_Object_Joystick *)(pObject->pObject))->Distance *
              PX_cosd(((PX_Object_Joystick *)(pObject->pObject))->Angle);
        p.y = ((PX_Object_Joystick *)(pObject->pObject))->Distance *
              PX_sind(((PX_Object_Joystick *)(pObject->pObject))->Angle);
    }
    return p;
}

///////////////////////////////////////////////

px_void Func_JoystickRender(px_surface *pSurface, PX_Object *pObject, px_uint elpased);
px_void Func_JoystickOnCursorDown(PX_Object *pObject, PX_Object_Event e, px_void *ptr);
px_void Func_JoystickOnCursorDrag(PX_Object *pObject, PX_Object_Event e, px_void *ptr);
px_void Func_JoystickOnCursorUp(PX_Object *pObject, PX_Object_Event e, px_void *ptr);

PX_Object *PX_Object_CreateJoystick(px_memorypool *mp, PX_Object *Parent,
                                    px_float x, px_float y,
                                    px_float deferentR, px_float epicycleR, px_float senseR,
                                    px_color deferentColor, px_color epicycleColor)
{

    PX_Object *pObject;
    PX_Object_Joystick joystick;

    joystick.DeferentColor = deferentColor;
    joystick.EpicycleColor = epicycleColor;
    joystick.Position.x = x;
    joystick.Position.y = y;
    joystick.Distance = 0.0;
    joystick.Angle = 0.0;
    joystick.DeferentRadius = deferentR;
    joystick.EpicycleRadius = epicycleR;
    joystick.SenseRadius = senseR;

    pObject = PX_ObjectCreateEx(mp, Parent, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0,
                                PX_NULL, Func_JoystickRender, PX_NULL, &joystick, sizeof(PX_Object_Joystick));

    if (pObject == PX_NULL)
    {
        return PX_NULL;
    }

    pObject->Type = PX_OBJECT_TYPE_JOYSTICK;

    PX_ObjectRegisterEvent(pObject, PX_OBJECT_EVENT_CURSORDOWN, Func_JoystickOnCursorDown, PX_NULL);
    PX_ObjectRegisterEvent(pObject, PX_OBJECT_EVENT_CURSORDRAG, Func_JoystickOnCursorDrag, PX_NULL);
    PX_ObjectRegisterEvent(pObject, PX_OBJECT_EVENT_CURSORUP, Func_JoystickOnCursorUp, PX_NULL);

    return pObject;
}

px_void Func_JoystickRender(px_surface *pSurface, PX_Object *pObject, px_uint elpased)
{
    PX_Object_Joystick *pJoystick = pObject->pObject;

    PX_GeoDrawSolidCircle(
        pSurface,
        pJoystick->Position.x,
        pJoystick->Position.y,
        pJoystick->DeferentRadius, pJoystick->DeferentColor);
    PX_GeoDrawSolidCircle(
        pSurface,
        pJoystick->Position.x + pJoystick->Distance * PX_cosd(pJoystick->Angle),
        pJoystick->Position.y + pJoystick->Distance * PX_sind(pJoystick->Angle),
        pJoystick->EpicycleRadius, pJoystick->EpicycleColor);
}

px_void Func_JoystickOnCursorDown(PX_Object *pObject, PX_Object_Event e, px_void *ptr)
{
    PX_Object_Joystick *pJoystick = pObject->pObject;
    px_float
        x = PX_Object_Event_GetCursorX(e) - pJoystick->Position.x,
        y = PX_Object_Event_GetCursorY(e) - pJoystick->Position.y,
        r = PX_sqrt(x * x + y * y);

    if (r < pJoystick->SenseRadius)
    {
        pJoystick->Distance = r;
        pJoystick->Angle = PX_atan2(y, x);
    }
    else
    {
        pJoystick->Distance = 0.0f;
    }
}

px_void Func_JoystickOnCursorDrag(PX_Object *pObject, PX_Object_Event e, px_void *ptr)
{
    PX_Object_Joystick *pJoystick = pObject->pObject;
    px_float
        x = PX_Object_Event_GetCursorX(e) - pJoystick->Position.x,
        y = PX_Object_Event_GetCursorY(e) - pJoystick->Position.y,
        r = PX_sqrt(x * x + y * y);

    pJoystick->Angle = PX_atan2(y, x);
    if (r > pJoystick->DeferentRadius)
    {
        pJoystick->Distance = pJoystick->DeferentRadius;
    }
    else
    {
        pJoystick->Distance = r;
    }
}

px_void Func_JoystickOnCursorUp(PX_Object *pObject, PX_Object_Event e, px_void *ptr)
{
    PX_Object_Joystick *pJoystick = pObject->pObject;
    px_float
        x = PX_Object_Event_GetCursorX(e) - pJoystick->Position.x,
        y = PX_Object_Event_GetCursorY(e) - pJoystick->Position.y,
        r = PX_sqrt(x * x + y * y);

    pJoystick->Distance = 0.0f;
}