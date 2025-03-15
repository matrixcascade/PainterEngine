#ifndef PX_OBJECT_CLOCKPAN_H
#define PX_OBJECT_CLOCKPAN_H
#include "PX_Object.h"

typedef struct
{
	px_bool loopdot;
	px_float a1_angle, a2_angle, a3_angle;
	px_float speed;
	px_float a1_target_angle, a2_target_angle, a3_target_angle;
	px_float a1_pen_size, a2_pen_size, a3_pen_size;
	px_float a1_target_pen_size, a2_target_pen_size, a3_target_pen_size;
	px_float a1_len, a2_len, a3_len;
	px_float a1_target_len, a2_target_len, a3_target_len;
	px_color a1_color, a2_color, a3_color;
	px_color a1_target_color, a2_target_color, a3_target_color;
	px_color background_color;
}PX_Object_ClockPan;

PX_Object* PX_Object_ClockPanCreate(px_memorypool* mp, PX_Object* Parent, px_float x, px_float y, px_float radius);
px_void PX_Object_ClockPanSetTarget1(PX_Object* pClockPan, px_float a1_angle);
px_void PX_Object_ClockPanSetTarget2(PX_Object* pClockPan, px_float a1_angle, px_float a2_angle);
px_void PX_Object_ClockPanSetTarget3(PX_Object* pClockPan, px_float a1_angle, px_float a2_angle,px_float a3_angle);
px_void PX_Object_ClockPanSetHourMinSec(PX_Object* pObject, px_int hour, px_int min, px_int sec);
px_void PX_Object_ClockPanSetTarget0(PX_Object* pObject);
px_void PX_Object_ClockPanSetBigDot(PX_Object* pObject);
typedef struct
{
	PX_Object* pObjectClockPan[6];
}PX_Object_ClockChar;

PX_Object* PX_Object_ClockCharCreate(px_memorypool* mp, PX_Object* Parent, px_float x, px_float y, px_float radius);
px_void PX_Object_ClockCharSetChar(PX_Object* pObject,px_char c);
px_void PX_Object_ClockCharSetClockMode(PX_Object* pObject, px_int hour, px_int min, px_int sec);
#endif // !PX_OBJECT_CLOCKPAN_H
