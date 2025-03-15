#include "PX_Object_ClockPan.h"
PX_OBJECT_RENDER_FUNCTION(PX_Object_ClockPanOnRender)
{
	px_float x,y,w,h;
	px_float move_angle_step, move_len_speed, move_size_speed;
	px_byte move_color_channel_speed;
	PX_Object_ClockPan* pDesc = PX_ObjectGetDesc(PX_Object_ClockPan, pObject);
	PX_OBJECT_INHERIT_CODE(pObject, x, y, w, h);

	if (pDesc->loopdot)
	{
		PX_GeoDrawBall(psurface, x, y, pObject->diameter / 2, pDesc->background_color);
		PX_GeoDrawSolidCircle(psurface, (px_int)x, (px_int)y, (px_int)pObject->diameter / 8, PX_COLOR_WHITE);
	}
	else
	{
		move_angle_step = elapsed * pDesc->speed / 1000.f;
		move_len_speed = elapsed / 8.f;
		move_size_speed = elapsed / 8.f;
		move_color_channel_speed = elapsed  > 255 ? 255 : (px_byte)(elapsed );
		move_color_channel_speed = move_color_channel_speed < 1 ? 1 : move_color_channel_speed;

		if (pDesc->a1_len != pDesc->a1_target_len)
		{
			if (PX_ABS(pDesc->a1_target_len - pDesc->a1_len) < move_len_speed)
			{
				pDesc->a1_len = pDesc->a1_target_len;
			}
			else
			{
				if (pDesc->a1_target_len > pDesc->a1_len)
				{
					pDesc->a1_len += move_len_speed;
				}
				else
				{
					pDesc->a1_len -= move_len_speed;
				}
			}
		}

		if (pDesc->a2_len != pDesc->a2_target_len)
		{
			if (PX_ABS(pDesc->a2_target_len - pDesc->a2_len) < move_len_speed)
			{
				pDesc->a2_len = pDesc->a2_target_len;
			}
			else
			{
				if (pDesc->a2_target_len > pDesc->a2_len)
				{
					pDesc->a2_len += move_len_speed;
				}
				else
				{
					pDesc->a2_len -= move_len_speed;
				}
			}
		}

		if (pDesc->a3_len != pDesc->a3_target_len)
		{
			if (PX_ABS(pDesc->a3_target_len - pDesc->a3_len) < move_len_speed)
			{
				pDesc->a3_len = pDesc->a3_target_len;
			}
			else
			{
				if (pDesc->a3_target_len > pDesc->a3_len)
				{
					pDesc->a3_len += move_len_speed;
				}
				else
				{
					pDesc->a3_len -= move_len_speed;
				}
			}
		}

		if (pDesc->a1_pen_size != pDesc->a1_target_pen_size)
		{
			if (PX_ABS(pDesc->a1_target_pen_size - pDesc->a1_pen_size) < move_size_speed)
			{
				pDesc->a1_pen_size = pDesc->a1_target_pen_size;
			}
			else
			{
				if (pDesc->a1_target_pen_size > pDesc->a1_pen_size)
				{
					pDesc->a1_pen_size += move_size_speed;
				}
				else
				{
					pDesc->a1_pen_size -= move_size_speed;
				}
			}
		}

		if (pDesc->a2_pen_size != pDesc->a2_target_pen_size)
		{
			if (PX_ABS(pDesc->a2_target_pen_size - pDesc->a2_pen_size) < move_size_speed)
			{
				pDesc->a2_pen_size = pDesc->a2_target_pen_size;
			}
			else
			{
				if (pDesc->a2_target_pen_size > pDesc->a2_pen_size)
				{
					pDesc->a2_pen_size += move_size_speed;
				}
				else
				{
					pDesc->a2_pen_size -= move_size_speed;
				}
			}
		}

		if (pDesc->a3_pen_size != pDesc->a3_target_pen_size)
		{
			if (PX_ABS(pDesc->a3_target_pen_size - pDesc->a3_pen_size) < move_size_speed)
			{
				pDesc->a3_pen_size = pDesc->a3_target_pen_size;
			}
			else
			{
				if (pDesc->a3_target_pen_size > pDesc->a3_pen_size)
				{
					pDesc->a3_pen_size += move_size_speed;
				}
				else
				{
					pDesc->a3_pen_size -= move_size_speed;
				}
			}
		}

		if (pDesc->a1_color._argb.ucolor != pDesc->a1_target_color._argb.ucolor)
		{
			if (PX_ABS(pDesc->a1_target_color._argb.a - pDesc->a1_color._argb.a) < move_color_channel_speed)
			{
				pDesc->a1_color._argb.a = pDesc->a1_target_color._argb.a;
			}
			else
			{
				if (pDesc->a1_target_color._argb.a > pDesc->a1_color._argb.a)
				{
					pDesc->a1_color._argb.a += move_color_channel_speed;
				}
				else
				{
					pDesc->a1_color._argb.a -= move_color_channel_speed;
				}
			}

			if (PX_ABS(pDesc->a1_target_color._argb.r - pDesc->a1_color._argb.r) < move_color_channel_speed)
			{
				pDesc->a1_color._argb.r = pDesc->a1_target_color._argb.r;
			}
			else
			{
				if (pDesc->a1_target_color._argb.r > pDesc->a1_color._argb.r)
				{
					pDesc->a1_color._argb.r += move_color_channel_speed;
				}
				else
				{
					pDesc->a1_color._argb.r -= move_color_channel_speed;
				}
			}

			if (PX_ABS(pDesc->a1_target_color._argb.g - pDesc->a1_color._argb.g) < move_color_channel_speed)
			{
				pDesc->a1_color._argb.g = pDesc->a1_target_color._argb.g;
			}
			else
			{
				if (pDesc->a1_target_color._argb.g > pDesc->a1_color._argb.g)
				{
					pDesc->a1_color._argb.g += move_color_channel_speed;
				}
				else
				{
					pDesc->a1_color._argb.g -= move_color_channel_speed;
				}
			}

			if (PX_ABS(pDesc->a1_target_color._argb.b - pDesc->a1_color._argb.b) < move_color_channel_speed)
			{
				pDesc->a1_color._argb.b = pDesc->a1_target_color._argb.b;
			}
			else
			{
				if (pDesc->a1_target_color._argb.b > pDesc->a1_color._argb.b)
				{
					pDesc->a1_color._argb.b += move_color_channel_speed;
				}
				else
				{
					pDesc->a1_color._argb.b -= move_color_channel_speed;
				}
			}
		}

		if (pDesc->a2_color._argb.ucolor != pDesc->a2_target_color._argb.ucolor)
		{
			if (PX_ABS(pDesc->a2_target_color._argb.a - pDesc->a2_color._argb.a) < move_color_channel_speed)
			{
				pDesc->a2_color._argb.a = pDesc->a2_target_color._argb.a;
			}
			else
			{
				if (pDesc->a2_target_color._argb.a > pDesc->a2_color._argb.a)
				{
					pDesc->a2_color._argb.a += move_color_channel_speed;
				}
				else
				{
					pDesc->a2_color._argb.a -= move_color_channel_speed;
				}
			}
			if (PX_ABS(pDesc->a2_target_color._argb.r - pDesc->a2_color._argb.r) < move_color_channel_speed)
			{
				pDesc->a2_color._argb.r = pDesc->a2_target_color._argb.r;
			}
			else
			{
				if (pDesc->a2_target_color._argb.r > pDesc->a2_color._argb.r)
				{
					pDesc->a2_color._argb.r += move_color_channel_speed;
				}
				else
				{
					pDesc->a2_color._argb.r -= move_color_channel_speed;
				}
			}
			if (PX_ABS(pDesc->a2_target_color._argb.g - pDesc->a2_color._argb.g) < move_color_channel_speed)
			{
				pDesc->a2_color._argb.g = pDesc->a2_target_color._argb.g;
			}
			else
			{
				if (pDesc->a2_target_color._argb.g > pDesc->a2_color._argb.g)
				{
					pDesc->a2_color._argb.g += move_color_channel_speed;
				}
				else
				{
					pDesc->a2_color._argb.g -= move_color_channel_speed;
				}
			}
			if (PX_ABS(pDesc->a2_target_color._argb.b - pDesc->a2_color._argb.b) < move_color_channel_speed)
			{
				pDesc->a2_color._argb.b = pDesc->a2_target_color._argb.b;
			}
			else
			{
				if (pDesc->a2_target_color._argb.b > pDesc->a2_color._argb.b)
				{
					pDesc->a2_color._argb.b += move_color_channel_speed;
				}
				else
				{
					pDesc->a2_color._argb.b -= move_color_channel_speed;
				}
			}
		}

		if (pDesc->a3_color._argb.ucolor != pDesc->a3_target_color._argb.ucolor)
		{
			if (PX_ABS(pDesc->a3_target_color._argb.a - pDesc->a3_color._argb.a) < move_color_channel_speed)
			{
				pDesc->a3_color._argb.a = pDesc->a3_target_color._argb.a;
			}
			else
			{
				if (pDesc->a3_target_color._argb.a > pDesc->a3_color._argb.a)
				{
					pDesc->a3_color._argb.a += move_color_channel_speed;
				}
				else
				{
					pDesc->a3_color._argb.a -= move_color_channel_speed;
				}
			}
			if (PX_ABS(pDesc->a3_target_color._argb.r - pDesc->a3_color._argb.r) < move_color_channel_speed)
			{
				pDesc->a3_color._argb.r = pDesc->a3_target_color._argb.r;
			}
			else
			{
				if (pDesc->a3_target_color._argb.r > pDesc->a3_color._argb.r)
				{
					pDesc->a3_color._argb.r += move_color_channel_speed;
				}
				else
				{
					pDesc->a3_color._argb.r -= move_color_channel_speed;
				}
			}
			if (PX_ABS(pDesc->a3_target_color._argb.g - pDesc->a3_color._argb.g) < move_color_channel_speed)
			{
				pDesc->a3_color._argb.g = pDesc->a3_target_color._argb.g;
			}
			else
			{
				if (pDesc->a3_target_color._argb.g > pDesc->a3_color._argb.g)
				{
					pDesc->a3_color._argb.g += move_color_channel_speed;
				}
				else
				{
					pDesc->a3_color._argb.g -= move_color_channel_speed;
				}
			}
			if (PX_ABS(pDesc->a3_target_color._argb.b - pDesc->a3_color._argb.b) < move_color_channel_speed)
			{
				pDesc->a3_color._argb.b = pDesc->a3_target_color._argb.b;
			}
			else
			{
				if (pDesc->a3_target_color._argb.b > pDesc->a3_color._argb.b)
				{
					pDesc->a3_color._argb.b += move_color_channel_speed;
				}
				else
				{
					pDesc->a3_color._argb.b -= move_color_channel_speed;
				}
			}
		}

		if (pDesc->a1_angle != pDesc->a1_target_angle)
		{
			if (PX_ABS(pDesc->a1_target_angle - pDesc->a1_angle) < move_angle_step)
			{
				pDesc->a1_angle = pDesc->a1_target_angle;
			}
			else
			{
				if (pDesc->a1_target_angle > pDesc->a1_angle)
				{
					pDesc->a1_angle += move_angle_step;
				}
				else
				{
					pDesc->a1_angle -= move_angle_step;
				}
			}
		}

		if (pDesc->a2_angle != pDesc->a2_target_angle)
		{
			if (PX_ABS(pDesc->a2_target_angle - pDesc->a2_angle) < move_angle_step)
			{
				pDesc->a2_angle = pDesc->a2_target_angle;
			}
			else
			{
				if (pDesc->a2_target_angle > pDesc->a2_angle)
				{
					pDesc->a2_angle += move_angle_step;
				}
				else
				{
					pDesc->a2_angle -= move_angle_step;
				}
			}
		}

		if (pDesc->a3_angle != pDesc->a3_target_angle)
		{
			if (PX_ABS(pDesc->a3_target_angle - pDesc->a3_angle) < move_angle_step)
			{
				pDesc->a3_angle = pDesc->a3_target_angle;
			}
			else
			{
				if (pDesc->a3_target_angle > pDesc->a3_angle)
				{
					pDesc->a3_angle += move_angle_step;
				}
				else
				{
					pDesc->a3_angle -= move_angle_step;
				}
			}
		}
		PX_GeoDrawBall(psurface, x, y, pObject->diameter / 2, pDesc->background_color);
		PX_GeoDrawPenLine(psurface, x, y, x + pDesc->a1_len * PX_cos_angle(pDesc->a1_angle), y + pDesc->a1_len * PX_sin_angle(pDesc->a1_angle), pDesc->a1_pen_size, pDesc->a1_color);
		PX_GeoDrawPenLine(psurface, x, y, x + pDesc->a2_len * PX_cos_angle(pDesc->a2_angle), y + pDesc->a2_len * PX_sin_angle(pDesc->a2_angle), pDesc->a2_pen_size, pDesc->a2_color);
		PX_GeoDrawPenLine(psurface, x, y, x + pDesc->a3_len * PX_cos_angle(pDesc->a3_angle), y + pDesc->a3_len * PX_sin_angle(pDesc->a3_angle), pDesc->a3_pen_size, pDesc->a3_color);
		PX_GeoDrawSolidCircle(psurface, (px_int)x, (px_int)y, 5, PX_COLOR_WHITE);
	}

	
}

PX_Object* PX_Object_ClockPanAttach(PX_Object *pObject, px_int attachIndex, px_float radius)
{
	px_memorypool* mp = pObject->mp;
	PX_Object_ClockPan* pDesc;
	PX_ASSERTIF(pObject == PX_NULL);
	PX_ASSERTIF(attachIndex < 0 || attachIndex >= PX_COUNTOF(pObject->pObjectDesc));
	PX_ASSERTIF(pObject->pObjectDesc[attachIndex] != PX_NULL);
	pDesc = (PX_Object_ClockPan*)PX_ObjectCreateDesc(pObject, attachIndex, PX_OBJECT_TYPE_CLOCKPAN, 0, PX_Object_ClockPanOnRender, 0, 0, sizeof(PX_Object_ClockPan));
	PX_ASSERTIF(pDesc == PX_NULL);
	pDesc->background_color = PX_COLOR(128, 255, 255, 255);
	
	pDesc->a1_color = PX_COLOR(255,	255, 255, 255);
	pDesc->a2_color = PX_COLOR(255, 255, 255, 255);
	pDesc->a3_color = PX_COLOR(255, 255, 255, 255);

	pDesc->speed = 720;
	pDesc->a1_pen_size = radius / 5.f;
	pDesc->a2_pen_size = radius / 5.f;
	pDesc->a3_pen_size = radius / 5.f;
	pDesc->a1_len = radius*1.f;
	pDesc->a2_len = radius*1.f;
	pDesc->a3_len = radius*1.f;

	pDesc->a1_target_angle = 0;
	pDesc->a2_target_angle = 0;
	pDesc->a3_target_angle = 0;
	pDesc->a1_target_pen_size = pDesc->a1_pen_size;
	pDesc->a2_target_pen_size = pDesc->a2_pen_size;
	pDesc->a3_target_pen_size = pDesc->a3_pen_size;
	pDesc->a1_target_len = pDesc->a1_len;
	pDesc->a2_target_len = pDesc->a2_len;
	pDesc->a3_target_len = pDesc->a3_len;
	pDesc->a1_target_color = pDesc->a1_color;
	pDesc->a2_target_color = pDesc->a2_color;
	pDesc->a3_target_color = pDesc->a3_color;
	pDesc->a1_angle = 0;
	pDesc->loopdot = PX_FALSE;
	return pObject;
}


PX_Object* PX_Object_ClockPanCreate(px_memorypool* mp, PX_Object* Parent, px_float x, px_float y, px_float radius)
{
	PX_Object* pObject;
	pObject = PX_ObjectCreate(mp, Parent, (px_float)x, (px_float)y, 0, (px_float)radius*2, (px_float)radius * 2, 0);
	pObject->diameter = radius * 2.f;
	PX_Object_ClockPanAttach(pObject, 0,radius);
	
	return pObject;
}

px_void PX_Object_ClockPanSetTarget1(PX_Object* pObject, px_float p1)
{
	PX_Object_ClockPan* pDesc = (PX_Object_ClockPan *)PX_ObjectGetDescByType(pObject, PX_OBJECT_TYPE_CLOCKPAN);
	pDesc->loopdot = PX_FALSE;
	pDesc->a1_target_pen_size = pObject->diameter / 5;
	pDesc->a2_target_pen_size = pObject->diameter / 5;
	pDesc->a3_target_pen_size = pObject->diameter / 5;
	pDesc->a1_target_len = pObject->diameter / 2;
	pDesc->a2_target_len = pObject->diameter / 2;
	pDesc->a3_target_len = pObject->diameter / 2;
	pDesc->a1_target_color = PX_COLOR(255, 255, 255, 255);
	pDesc->a2_target_color = PX_COLOR(255, 255, 255, 255);
	pDesc->a3_target_color = PX_COLOR(255, 255, 255, 255);
	pDesc->a1_target_angle = p1;
	pDesc->a2_target_angle = p1;
	pDesc->a3_target_angle = p1;
	

}
px_void PX_Object_ClockPanSetTarget2(PX_Object* pObject, px_float p1, px_float p2)
{
	PX_Object_ClockPan* pDesc = (PX_Object_ClockPan*)PX_ObjectGetDescByType(pObject, PX_OBJECT_TYPE_CLOCKPAN);;
	pDesc->loopdot = PX_FALSE;
	pDesc->a1_target_color = PX_COLOR(255, 255, 255, 255);
	pDesc->a2_target_color = PX_COLOR(255, 255, 255, 255);
	pDesc->a3_target_color = PX_COLOR(255, 255, 255, 255);
	pDesc->a1_target_pen_size = pObject->diameter / 5;
	pDesc->a2_target_pen_size = pObject->diameter / 5;
	pDesc->a3_target_pen_size = pObject->diameter / 5;
	pDesc->a1_target_len = pObject->diameter / 2;
	pDesc->a2_target_len = pObject->diameter / 2;
	pDesc->a3_target_len = pObject->diameter / 2;
	pDesc->a1_target_angle = p1;
	pDesc->a2_target_angle = p2;
	pDesc->a3_target_angle = p2;
}
px_void PX_Object_ClockPanSetTarget3(PX_Object* pObject, px_float p1, px_float p2, px_float p3)
{
	PX_Object_ClockPan* pDesc = (PX_Object_ClockPan*)PX_ObjectGetDescByType(pObject, PX_OBJECT_TYPE_CLOCKPAN);

	pDesc->loopdot = PX_FALSE;
	pDesc->a1_target_color = PX_COLOR(255, 255, 255, 255);
	pDesc->a2_target_color = PX_COLOR(255, 255, 255, 255);
	pDesc->a3_target_color = PX_COLOR(255, 255, 255, 255);
	pDesc->a1_target_pen_size = pObject->diameter / 5;
	pDesc->a2_target_pen_size = pObject->diameter / 5;
	pDesc->a3_target_pen_size = pObject->diameter / 5;
	pDesc->a1_target_len = pObject->diameter / 2;
	pDesc->a2_target_len = pObject->diameter / 2;
	pDesc->a3_target_len = pObject->diameter / 2;

	pDesc->a1_target_angle = p1;
	pDesc->a2_target_angle = p2;
	pDesc->a3_target_angle = p3;

}

px_void PX_Object_ClockPanSetHourMinSec(PX_Object* pObject, px_int hour, px_int min, px_int sec)
{
	PX_Object_ClockPan* pDesc = (PX_Object_ClockPan*)PX_ObjectGetDescByType(pObject, PX_OBJECT_TYPE_CLOCKPAN);
	hour %= 12;
	min %= 60;
	sec %= 60;
	pDesc->loopdot = PX_FALSE;
	pDesc->a1_target_color = PX_COLOR(255, 255, 64, 64);
	pDesc->a2_target_color = PX_COLOR(255, 64, 255, 64);
	pDesc->a3_target_color = PX_COLOR(255, 255, 192, 255);

	pDesc->a1_target_len = pObject->diameter / 2.8f;
	pDesc->a2_target_len = pObject->diameter / 2.5f;
	pDesc->a3_target_len = pObject->diameter / 2;

	pDesc->a1_target_pen_size = pObject->diameter / 12;
	pDesc->a2_target_pen_size = pObject->diameter / 16;
	pDesc->a3_target_pen_size = pObject->diameter / 20;

	pDesc->a1_target_angle = (px_float)hour / 12 * 360-90;
	pDesc->a2_target_angle = (px_float)min / 60 * 360-90;
	pDesc->a3_target_angle = (px_float)sec / 60 * 360 - 90;
}

px_void PX_Object_ClockPanSetTarget0(PX_Object* pObject)
{
	PX_Object_ClockPan* pDesc = (PX_Object_ClockPan*)PX_ObjectGetDescByType(pObject, PX_OBJECT_TYPE_CLOCKPAN);
	pDesc->loopdot = PX_FALSE;
	pDesc->a1_target_color = PX_COLOR_WHITE;
	pDesc->a2_target_color = PX_COLOR_WHITE;
	pDesc->a3_target_color = PX_COLOR_WHITE;

	pDesc->a1_target_len = 0;
	pDesc->a2_target_len = 0;
	pDesc->a3_target_len = 0;

	pDesc->a1_target_pen_size = pObject->diameter / 5;
	pDesc->a2_target_pen_size = pObject->diameter / 5;
	pDesc->a3_target_pen_size = pObject->diameter / 5;
}

px_void PX_Object_ClockPanSetBigDot(PX_Object* pObject)
{
	PX_Object_ClockPan* pDesc = (PX_Object_ClockPan*)PX_ObjectGetDescByType(pObject, PX_OBJECT_TYPE_CLOCKPAN);
	pDesc->loopdot = PX_TRUE;
	pDesc->a1_len = 0;
	pDesc->a2_len = 0;
	pDesc->a3_len = 0;
	pDesc->a1_target_len = 0;
	pDesc->a2_target_len = 0;
	pDesc->a3_target_len = 0;
}



PX_Object* PX_Object_ClockCharCreate(px_memorypool* mp, PX_Object* Parent, px_float x, px_float y, px_float radius)
{
	PX_Object_ClockChar* pDesc;
	PX_Object* pObject;
	pObject = PX_ObjectCreateEx(mp, Parent, (px_float)x, (px_float)y , 0, radius*4, radius *6, 0, PX_OBJECT_TYPE_CLOCKNUMERIC, 0, 0, 0,0,sizeof(PX_Object_ClockChar));
	if (!pObject)
	{
		return PX_NULL;
	}
	pDesc = (PX_Object_ClockChar*)PX_ObjectGetDescByType(pObject, PX_OBJECT_TYPE_CLOCKNUMERIC);
	pDesc->pObjectClockPan[0] = PX_Object_ClockPanCreate(mp, pObject, -radius, -radius*2, radius);
	pDesc->pObjectClockPan[1] = PX_Object_ClockPanCreate(mp, pObject, radius,  - radius*2, radius);
	pDesc->pObjectClockPan[2] = PX_Object_ClockPanCreate(mp, pObject, -radius, 0, radius);
	pDesc->pObjectClockPan[3] = PX_Object_ClockPanCreate(mp, pObject, radius, 0, radius);
	pDesc->pObjectClockPan[4] = PX_Object_ClockPanCreate(mp, pObject, -radius, radius*2, radius);
	pDesc->pObjectClockPan[5] = PX_Object_ClockPanCreate(mp, pObject, radius, radius*2, radius);

	return pObject;
}

px_void PX_Object_ClockCharSetChar(PX_Object* pObject, px_char c)
{
	PX_Object_ClockChar* pDesc = (PX_Object_ClockChar*)PX_ObjectGetDescByType(pObject, PX_OBJECT_TYPE_CLOCKNUMERIC);
	switch (c)
	{
	case '0':
	case 'o':
	case 'O':
		PX_Object_ClockPanSetTarget2(pDesc->pObjectClockPan[0], 0, 90);
		PX_Object_ClockPanSetTarget2(pDesc->pObjectClockPan[1], 180, 90);
		PX_Object_ClockPanSetTarget2(pDesc->pObjectClockPan[2], -90, 90);
		PX_Object_ClockPanSetTarget2(pDesc->pObjectClockPan[3], -90, 90);
		PX_Object_ClockPanSetTarget2(pDesc->pObjectClockPan[4], -90, 0);
		PX_Object_ClockPanSetTarget2(pDesc->pObjectClockPan[5], -90,180);
		break;
	case '1':
	case 'i':
	case 'I':
		PX_Object_ClockPanSetTarget1(pDesc->pObjectClockPan[0], 90);
		PX_Object_ClockPanSetTarget0(pDesc->pObjectClockPan[1]);
		PX_Object_ClockPanSetTarget2(pDesc->pObjectClockPan[2], -90, 90);
		PX_Object_ClockPanSetTarget0(pDesc->pObjectClockPan[3]);
		PX_Object_ClockPanSetTarget1(pDesc->pObjectClockPan[4], -90);
		PX_Object_ClockPanSetTarget0(pDesc->pObjectClockPan[5]);
		break;
	case '2':
		PX_Object_ClockPanSetTarget1(pDesc->pObjectClockPan[0], 0);
		PX_Object_ClockPanSetTarget2(pDesc->pObjectClockPan[1], 180, 90);
		PX_Object_ClockPanSetTarget2(pDesc->pObjectClockPan[2], 0, 90);
		PX_Object_ClockPanSetTarget2(pDesc->pObjectClockPan[3], -90, 180);
		PX_Object_ClockPanSetTarget2(pDesc->pObjectClockPan[4], -90, 0);
		PX_Object_ClockPanSetTarget1(pDesc->pObjectClockPan[5], 180);
		break;
	case '3':
		PX_Object_ClockPanSetTarget1(pDesc->pObjectClockPan[0], 0);
		PX_Object_ClockPanSetTarget2(pDesc->pObjectClockPan[1], 180, 90);
		PX_Object_ClockPanSetTarget1(pDesc->pObjectClockPan[2], 0);
		PX_Object_ClockPanSetTarget3(pDesc->pObjectClockPan[3], 180, -90,90);
		PX_Object_ClockPanSetTarget1(pDesc->pObjectClockPan[4], 0);
		PX_Object_ClockPanSetTarget2(pDesc->pObjectClockPan[5], 180, -90);
		break;
	case '4':
		PX_Object_ClockPanSetTarget1(pDesc->pObjectClockPan[0], 90);
		PX_Object_ClockPanSetTarget1(pDesc->pObjectClockPan[1], 90);
		PX_Object_ClockPanSetTarget2(pDesc->pObjectClockPan[2], -90,0);
		PX_Object_ClockPanSetTarget3(pDesc->pObjectClockPan[3], 180, -90, 90);
		PX_Object_ClockPanSetTarget0(pDesc->pObjectClockPan[4]);
		PX_Object_ClockPanSetTarget1(pDesc->pObjectClockPan[5],  -90);
		break;
	case '5':
	case 's':
	case 'S':
		PX_Object_ClockPanSetTarget2(pDesc->pObjectClockPan[0], 0,90);
		PX_Object_ClockPanSetTarget1(pDesc->pObjectClockPan[1], 180);
		PX_Object_ClockPanSetTarget2(pDesc->pObjectClockPan[2], -90, 0);
		PX_Object_ClockPanSetTarget2(pDesc->pObjectClockPan[3], 180, 90);
		PX_Object_ClockPanSetTarget1(pDesc->pObjectClockPan[4], 0);
		PX_Object_ClockPanSetTarget2(pDesc->pObjectClockPan[5], -90, 180);
		break;
	case '6':
		PX_Object_ClockPanSetTarget2(pDesc->pObjectClockPan[0], 0, 90);
		PX_Object_ClockPanSetTarget1(pDesc->pObjectClockPan[1], 180);
		PX_Object_ClockPanSetTarget3(pDesc->pObjectClockPan[2], -90, 0,90);
		PX_Object_ClockPanSetTarget2(pDesc->pObjectClockPan[3], 180, 90);
		PX_Object_ClockPanSetTarget2(pDesc->pObjectClockPan[4],-90, 0);
		PX_Object_ClockPanSetTarget2(pDesc->pObjectClockPan[5], -90, 180);
		break;
	case '7':
		PX_Object_ClockPanSetTarget1(pDesc->pObjectClockPan[0], 0);
		PX_Object_ClockPanSetTarget2(pDesc->pObjectClockPan[1], 180, 90);
		PX_Object_ClockPanSetTarget0(pDesc->pObjectClockPan[2]);
		PX_Object_ClockPanSetTarget2(pDesc->pObjectClockPan[3], -90, 90);
		PX_Object_ClockPanSetTarget0(pDesc->pObjectClockPan[4]);
		PX_Object_ClockPanSetTarget1(pDesc->pObjectClockPan[5], -90);
		break;
	case '8':
	case 'b':
	case 'B':
		PX_Object_ClockPanSetTarget2(pDesc->pObjectClockPan[0], 0, 90);
		PX_Object_ClockPanSetTarget2(pDesc->pObjectClockPan[1], 180,90);
		PX_Object_ClockPanSetTarget3(pDesc->pObjectClockPan[2], -90, 0, 90);
		PX_Object_ClockPanSetTarget3(pDesc->pObjectClockPan[3], 180, -90,90);
		PX_Object_ClockPanSetTarget2(pDesc->pObjectClockPan[4], -90, 0);
		PX_Object_ClockPanSetTarget2(pDesc->pObjectClockPan[5], -90, 180);
		break;
	case '9':
		PX_Object_ClockPanSetTarget2(pDesc->pObjectClockPan[0], 0, 90);
		PX_Object_ClockPanSetTarget2(pDesc->pObjectClockPan[1], 180, 90);
		PX_Object_ClockPanSetTarget2(pDesc->pObjectClockPan[2], -90, 0);
		PX_Object_ClockPanSetTarget3(pDesc->pObjectClockPan[3], 180, -90, 90);
		PX_Object_ClockPanSetTarget1(pDesc->pObjectClockPan[4],0);
		PX_Object_ClockPanSetTarget2(pDesc->pObjectClockPan[5], -90, 180);
		break;
	case 'a':
	case 'A':
		PX_Object_ClockPanSetTarget2(pDesc->pObjectClockPan[0], 0, 90);
		PX_Object_ClockPanSetTarget2(pDesc->pObjectClockPan[1], 180, 90);
		PX_Object_ClockPanSetTarget3(pDesc->pObjectClockPan[2], -90, 0, 90);
		PX_Object_ClockPanSetTarget3(pDesc->pObjectClockPan[3], 180, -90, 90);
		PX_Object_ClockPanSetTarget1(pDesc->pObjectClockPan[4], -90);
		PX_Object_ClockPanSetTarget1(pDesc->pObjectClockPan[5], -90);
		break;
	case 'c':
	case 'C':
		PX_Object_ClockPanSetTarget2(pDesc->pObjectClockPan[0], 0, 90);
		PX_Object_ClockPanSetTarget1(pDesc->pObjectClockPan[1], 180);
		PX_Object_ClockPanSetTarget2(pDesc->pObjectClockPan[2], -90,90);
		PX_Object_ClockPanSetTarget0(pDesc->pObjectClockPan[3]);
		PX_Object_ClockPanSetTarget2(pDesc->pObjectClockPan[4], -90,0);
		PX_Object_ClockPanSetTarget1(pDesc->pObjectClockPan[5], 180);
		break;
	case 'e':
	case 'E':
		PX_Object_ClockPanSetTarget2(pDesc->pObjectClockPan[0], 0, 90);
		PX_Object_ClockPanSetTarget1(pDesc->pObjectClockPan[1], 180);
		PX_Object_ClockPanSetTarget3(pDesc->pObjectClockPan[2], -90, 0,90);
		PX_Object_ClockPanSetTarget1(pDesc->pObjectClockPan[3],180);
		PX_Object_ClockPanSetTarget2(pDesc->pObjectClockPan[4], -90, 0);
		PX_Object_ClockPanSetTarget1(pDesc->pObjectClockPan[5], 180);
		break;
	case 'f':
	case 'F':
		PX_Object_ClockPanSetTarget2(pDesc->pObjectClockPan[0], 0, 90);
		PX_Object_ClockPanSetTarget1(pDesc->pObjectClockPan[1], 180);
		PX_Object_ClockPanSetTarget3(pDesc->pObjectClockPan[2], -90, 0, 90);
		PX_Object_ClockPanSetTarget1(pDesc->pObjectClockPan[3], 180);
		PX_Object_ClockPanSetTarget1(pDesc->pObjectClockPan[4], -90);
		PX_Object_ClockPanSetTarget0(pDesc->pObjectClockPan[5]);
		break;
	case 'g':
	case 'G':
		PX_Object_ClockPanSetTarget2(pDesc->pObjectClockPan[0], 0, 90);
		PX_Object_ClockPanSetTarget1(pDesc->pObjectClockPan[1], 180);
		PX_Object_ClockPanSetTarget2(pDesc->pObjectClockPan[2], -90, 90);
		PX_Object_ClockPanSetTarget2(pDesc->pObjectClockPan[3],90,180);
		PX_Object_ClockPanSetTarget2(pDesc->pObjectClockPan[4], -90, 0);
		PX_Object_ClockPanSetTarget2(pDesc->pObjectClockPan[5], -90,180);
		break;
	case 'h':
	case 'H':
		PX_Object_ClockPanSetTarget1(pDesc->pObjectClockPan[0], 90);
		PX_Object_ClockPanSetTarget1(pDesc->pObjectClockPan[1], 90);
		PX_Object_ClockPanSetTarget3(pDesc->pObjectClockPan[2], -90, 0, 90);
		PX_Object_ClockPanSetTarget3(pDesc->pObjectClockPan[3], 180, -90, 90);
		PX_Object_ClockPanSetTarget1(pDesc->pObjectClockPan[4], -90);
		PX_Object_ClockPanSetTarget1(pDesc->pObjectClockPan[5], -90);
		break;
	case 'j':
	case 'J':
		PX_Object_ClockPanSetTarget0(pDesc->pObjectClockPan[0]);
		PX_Object_ClockPanSetTarget2(pDesc->pObjectClockPan[1],180,90);
		PX_Object_ClockPanSetTarget0(pDesc->pObjectClockPan[2]);
		PX_Object_ClockPanSetTarget2(pDesc->pObjectClockPan[3], -90, 90);
		PX_Object_ClockPanSetTarget2(pDesc->pObjectClockPan[4], -90,0);
		PX_Object_ClockPanSetTarget2(pDesc->pObjectClockPan[5], -90,180);
		break;
	case 'k':
	case 'K':
		PX_Object_ClockPanSetTarget1(pDesc->pObjectClockPan[0], 90);
		PX_Object_ClockPanSetTarget1(pDesc->pObjectClockPan[1],135);
		PX_Object_ClockPanSetTarget3(pDesc->pObjectClockPan[2], -90, -45,45);
		PX_Object_ClockPanSetTarget0(pDesc->pObjectClockPan[3]);
		PX_Object_ClockPanSetTarget1(pDesc->pObjectClockPan[4], -90);
		PX_Object_ClockPanSetTarget1(pDesc->pObjectClockPan[5], -135);
		break;
	case 'l':
	case 'L':
		PX_Object_ClockPanSetTarget1(pDesc->pObjectClockPan[0], 90);
		PX_Object_ClockPanSetTarget0(pDesc->pObjectClockPan[1]);
		PX_Object_ClockPanSetTarget2(pDesc->pObjectClockPan[2], -90, 90);
		PX_Object_ClockPanSetTarget0(pDesc->pObjectClockPan[3]);
		PX_Object_ClockPanSetTarget2(pDesc->pObjectClockPan[4], -90,0);
		PX_Object_ClockPanSetTarget1(pDesc->pObjectClockPan[5],180);
		break;
	case 'm':
	case 'M':
		PX_Object_ClockPanSetTarget2(pDesc->pObjectClockPan[0], 45, 90);
		PX_Object_ClockPanSetTarget2(pDesc->pObjectClockPan[1], 90,135);
		PX_Object_ClockPanSetTarget1(pDesc->pObjectClockPan[2], -90);
		PX_Object_ClockPanSetTarget1(pDesc->pObjectClockPan[3], -90);
		PX_Object_ClockPanSetTarget1(pDesc->pObjectClockPan[4], -90);
		PX_Object_ClockPanSetTarget1(pDesc->pObjectClockPan[5], -90);
		break;
	case 'n':
	case 'N':
		PX_Object_ClockPanSetTarget2(pDesc->pObjectClockPan[0], 45,90);
		PX_Object_ClockPanSetTarget1(pDesc->pObjectClockPan[1], 90);
		PX_Object_ClockPanSetTarget2(pDesc->pObjectClockPan[2], -90, 90);
		PX_Object_ClockPanSetTarget3(pDesc->pObjectClockPan[3], -135, -90,90);
		PX_Object_ClockPanSetTarget1(pDesc->pObjectClockPan[4], -90);
		PX_Object_ClockPanSetTarget1(pDesc->pObjectClockPan[5], -90);
		break;
	case 'p':
	case 'P':
		PX_Object_ClockPanSetTarget2(pDesc->pObjectClockPan[0], 0, 90);
		PX_Object_ClockPanSetTarget2(pDesc->pObjectClockPan[1], 180,90);
		PX_Object_ClockPanSetTarget3(pDesc->pObjectClockPan[2], -90, 0, 90);
		PX_Object_ClockPanSetTarget2(pDesc->pObjectClockPan[3], -90,180);
		PX_Object_ClockPanSetTarget1(pDesc->pObjectClockPan[4], -90);
		PX_Object_ClockPanSetTarget0(pDesc->pObjectClockPan[5]);
		break;
	case 'q':
	case 'Q':
		PX_Object_ClockPanSetTarget2(pDesc->pObjectClockPan[0], 0, 90);
		PX_Object_ClockPanSetTarget2(pDesc->pObjectClockPan[1], 180, 90);
		PX_Object_ClockPanSetTarget2(pDesc->pObjectClockPan[2], -90, 90);
		PX_Object_ClockPanSetTarget2(pDesc->pObjectClockPan[3], -90, 90);
		PX_Object_ClockPanSetTarget2(pDesc->pObjectClockPan[4], -90, 0);
		PX_Object_ClockPanSetTarget3(pDesc->pObjectClockPan[5],-135, -90, 180);
		break;
	case 'r':
	case 'R':
		PX_Object_ClockPanSetTarget2(pDesc->pObjectClockPan[0], 0, 90);
		PX_Object_ClockPanSetTarget2(pDesc->pObjectClockPan[1], 180, 90);
		PX_Object_ClockPanSetTarget3(pDesc->pObjectClockPan[2], -90, 0, 45);
		PX_Object_ClockPanSetTarget2(pDesc->pObjectClockPan[3], -90, 180);
		PX_Object_ClockPanSetTarget1(pDesc->pObjectClockPan[4], -90);
		PX_Object_ClockPanSetTarget1(pDesc->pObjectClockPan[5],-135);
		break;
	case 't':
	case 'T':
		PX_Object_ClockPanSetTarget1(pDesc->pObjectClockPan[0], 0);
		PX_Object_ClockPanSetTarget2(pDesc->pObjectClockPan[1],180,90);
		PX_Object_ClockPanSetTarget0(pDesc->pObjectClockPan[2]);
		PX_Object_ClockPanSetTarget2(pDesc->pObjectClockPan[3],-90,90);
		PX_Object_ClockPanSetTarget0(pDesc->pObjectClockPan[4]);
		PX_Object_ClockPanSetTarget1(pDesc->pObjectClockPan[5],-90);
		break;
	case 'u':
	case 'U':
		PX_Object_ClockPanSetTarget1(pDesc->pObjectClockPan[0], 90);
		PX_Object_ClockPanSetTarget1(pDesc->pObjectClockPan[1], 90);
		PX_Object_ClockPanSetTarget2(pDesc->pObjectClockPan[2], -90, 90);
		PX_Object_ClockPanSetTarget2(pDesc->pObjectClockPan[3], -90, 90);
		PX_Object_ClockPanSetTarget2(pDesc->pObjectClockPan[4], -90, 0);
		PX_Object_ClockPanSetTarget2(pDesc->pObjectClockPan[5], -90, 180);
		break;
	case 'v':
	case 'V':
		PX_Object_ClockPanSetTarget0(pDesc->pObjectClockPan[0]);
		PX_Object_ClockPanSetTarget0(pDesc->pObjectClockPan[1]);
		PX_Object_ClockPanSetTarget2(pDesc->pObjectClockPan[2], -90, 90);
		PX_Object_ClockPanSetTarget2(pDesc->pObjectClockPan[3], -90, 90);
		PX_Object_ClockPanSetTarget2(pDesc->pObjectClockPan[4], -90, 45);
		PX_Object_ClockPanSetTarget2(pDesc->pObjectClockPan[5], -90, 135);
		break;
	case 'w':
	case 'W':
		PX_Object_ClockPanSetTarget0(pDesc->pObjectClockPan[0]);
		PX_Object_ClockPanSetTarget0(pDesc->pObjectClockPan[1]);
		PX_Object_ClockPanSetTarget1(pDesc->pObjectClockPan[2], 90);
		PX_Object_ClockPanSetTarget1(pDesc->pObjectClockPan[3], 90);
		PX_Object_ClockPanSetTarget2(pDesc->pObjectClockPan[4], -90, -45);
		PX_Object_ClockPanSetTarget2(pDesc->pObjectClockPan[5], -135,-90 );
		break;
	case 'x':
	case 'X':
		PX_Object_ClockPanSetTarget0(pDesc->pObjectClockPan[0]);
		PX_Object_ClockPanSetTarget0(pDesc->pObjectClockPan[1]);
		PX_Object_ClockPanSetTarget1(pDesc->pObjectClockPan[2], 45);
		PX_Object_ClockPanSetTarget1(pDesc->pObjectClockPan[3], 135);
		PX_Object_ClockPanSetTarget1(pDesc->pObjectClockPan[4], -45);
		PX_Object_ClockPanSetTarget1(pDesc->pObjectClockPan[5], -135);
		break;
	case 'y':
	case 'Y':
		PX_Object_ClockPanSetTarget1(pDesc->pObjectClockPan[0],90);
		PX_Object_ClockPanSetTarget2(pDesc->pObjectClockPan[1],90,135);
		PX_Object_ClockPanSetTarget2(pDesc->pObjectClockPan[2], -90,-45);
		PX_Object_ClockPanSetTarget2(pDesc->pObjectClockPan[3], -90,90);
		PX_Object_ClockPanSetTarget1(pDesc->pObjectClockPan[4], 0);
		PX_Object_ClockPanSetTarget2(pDesc->pObjectClockPan[5], -90,180);
		break;
	case 'z':
	case 'Z':
		PX_Object_ClockPanSetTarget0(pDesc->pObjectClockPan[0]);
		PX_Object_ClockPanSetTarget0(pDesc->pObjectClockPan[1]);
		PX_Object_ClockPanSetTarget1(pDesc->pObjectClockPan[2], 0);
		PX_Object_ClockPanSetTarget2(pDesc->pObjectClockPan[3], 135,180);
		PX_Object_ClockPanSetTarget2(pDesc->pObjectClockPan[4], -45,0);
		PX_Object_ClockPanSetTarget1(pDesc->pObjectClockPan[5], 180);
		break;
	case':':
		PX_Object_ClockPanSetBigDot(pDesc->pObjectClockPan[0]);
		PX_Object_ClockPanSetBigDot(pDesc->pObjectClockPan[1]);
		PX_Object_ClockPanSetBigDot(pDesc->pObjectClockPan[2]);
		PX_Object_ClockPanSetBigDot(pDesc->pObjectClockPan[3]);
		PX_Object_ClockPanSetBigDot(pDesc->pObjectClockPan[4]);
		PX_Object_ClockPanSetBigDot(pDesc->pObjectClockPan[5]);
		break;
	case'/':
		PX_Object_ClockPanSetTarget2(pDesc->pObjectClockPan[0], -45, 135);
		PX_Object_ClockPanSetTarget2(pDesc->pObjectClockPan[1], -45, 135);
		PX_Object_ClockPanSetTarget2(pDesc->pObjectClockPan[2], -45, 135);
		PX_Object_ClockPanSetTarget2(pDesc->pObjectClockPan[3], -45, 135);
		PX_Object_ClockPanSetTarget2(pDesc->pObjectClockPan[4], -45, 135);
		PX_Object_ClockPanSetTarget2(pDesc->pObjectClockPan[5], -45, 135);
		break;
	default:
		PX_Object_ClockPanSetTarget0(pDesc->pObjectClockPan[0]);
		PX_Object_ClockPanSetTarget0(pDesc->pObjectClockPan[1]);
		PX_Object_ClockPanSetTarget0(pDesc->pObjectClockPan[2]);
		PX_Object_ClockPanSetTarget0(pDesc->pObjectClockPan[3]);
		PX_Object_ClockPanSetTarget0(pDesc->pObjectClockPan[4]);
		PX_Object_ClockPanSetTarget0(pDesc->pObjectClockPan[5]);
		break;
	}
}

px_void PX_Object_ClockCharSetClockMode(PX_Object* pObject, px_int hour, px_int min, px_int sec)
{
	PX_Object_ClockChar* pDesc = (PX_Object_ClockChar*)PX_ObjectGetDescByType(pObject, PX_OBJECT_TYPE_CLOCKNUMERIC);
	PX_Object_ClockPanSetHourMinSec(pDesc->pObjectClockPan[0], hour , min, sec);
	PX_Object_ClockPanSetHourMinSec(pDesc->pObjectClockPan[1], hour, min , sec);
	PX_Object_ClockPanSetHourMinSec(pDesc->pObjectClockPan[2], hour, min, sec);
	PX_Object_ClockPanSetHourMinSec(pDesc->pObjectClockPan[3], hour, min, sec);
	PX_Object_ClockPanSetHourMinSec(pDesc->pObjectClockPan[4], hour, min, sec);
	PX_Object_ClockPanSetHourMinSec(pDesc->pObjectClockPan[5], hour, min, sec);
}
