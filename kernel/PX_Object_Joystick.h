#ifndef PX_OBJECT_JOYSTICK_H
#define PX_OBJECT_JOYSTICK_H

#include "PX_Object.h"

typedef struct PX_Object_Joystick
{
    px_color ActionAreaColor;
    px_color JoystickColor;
    px_float ActionAreaRadius;
    px_float SenseRadius;
    px_float JoystickRadius;
    px_float Angle;
    px_float Distance;
    px_bool IsActive;
} PX_Object_Joystick;

PX_Object *PX_Object_JoystickCreate(px_memorypool *mp, PX_Object *Parent,
                                    px_float x, px_float y,
                                    px_float actionAreaRadius, px_float joystickRadius, px_float senseRadius,
                                    px_color actionAreaColor, px_color joystickColor);

PX_Object_Joystick *PX_Object_GetJoystick(PX_Object *pObject);

px_double PX_Object_JoystickGetAngle(PX_Object *pObject);

px_double PX_Object_JoystickGetDistance(PX_Object *pObject);

px_point2D PX_Object_JoystickGetVector(PX_Object *pObject);

#endif
