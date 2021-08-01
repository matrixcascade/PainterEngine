#ifndef PX_OBJECT_JOYSTICK_H
#define PX_OBJECT_JOYSTICK_H

#include"PX_Object.h"

typedef struct {
    px_color DeferentColor;
    px_color EpicycleColor;
    px_float Distance;
    px_float Angle;
    px_float DeferentRadius;
    px_float EpicycleRadius;
    px_float SenseRadius;
    px_bool IsAciton;
}PX_Object_Joystick;

PX_Object* PX_Object_JoystickCreate(px_memorypool* mp, PX_Object* Parent,
    px_float x, px_float y,
    px_float deferentR, px_float epicycleR, px_float senseR,
    px_color deferentColor, px_color epicycleColoe);

PX_Object_Joystick* PX_Object_GetJoystick(PX_Object* pObject);

px_double PX_Object_JoystickGetAngle(PX_Object* pObject);

px_double PX_Object_JoystickGetDistance(PX_Object* pObject);

px_point2D PX_Object_JoystickGetVector(PX_Object *pObject);

#endif

