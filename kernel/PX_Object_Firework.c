#include "PX_Object_Firework.h"

typedef enum 
{
	PX_FIREWORK01_STATE_RAISING,
	PX_FIREWORK01_STATE_BLOOMING,
}PX_FIREWORK01_STATE;

typedef struct  
{
	px_point array[32];
	px_dword delay;
	px_float size;
	px_float vx, vy;
	px_color clr;
}PX_Firework01_paritcal;

typedef struct  
{
	PX_Firework01_paritcal particals[96];
	PX_FIREWORK01_STATE state;
	px_dword elapsed;
	px_dword raisingTime;
}PX_Object_Firework01;


void PX_Firework01ParticalUpdate(PX_Firework01_paritcal* pdesc, px_dword elapsed)
{
	px_int i;
	px_dword update_atom;
	if (elapsed>1000)
	{
		elapsed = 1000;
	}
	while (elapsed)
	{
		if (elapsed>10)
		{
			update_atom = 10;
			elapsed -= 10;
		}
		else
		{
			update_atom = elapsed;
			elapsed = 0;
		}

		pdesc->array[0].x += pdesc->vx * update_atom / 1000;
		pdesc->array[0].y += pdesc->vy * update_atom / 1000;

		pdesc->vx -= pdesc->vx * 0.9f * update_atom / 1000;
		pdesc->vy -= pdesc->vy * 0.9f * update_atom / 1000;

		pdesc->delay += update_atom;


		while (pdesc->delay > 20)
		{
			pdesc->delay -= 20;
			for (i = PX_COUNTOF(pdesc->array) - 1; i > 0; i--)
			{
				pdesc->array[i] = pdesc->array[i - 1];
			}
		}
	}
}

void PX_Object_Firework01ParticalRender(PX_Firework01_paritcal* pdesc, px_surface * psurface, px_bool bb,px_dword elapsed)
{
	px_int i;
	px_float scale;
	px_color color;

	for (i = PX_COUNTOF(pdesc->array) - 1; i >= 0; i--)
	{
		color = pdesc->clr;
		if (bb)
		{
			if (elapsed > 3000)
			{
				elapsed = 3000;
			}
			color._argb.a = (px_uchar)((3000 - elapsed) / 3000.f * 255);
			scale = (PX_COUNTOF(pdesc->array) - i) * (0.5f + elapsed / 2000.f) / PX_COUNTOF(pdesc->array);
		}
		else
		{
			scale = (PX_COUNTOF(pdesc->array) - i) *1.0f/ PX_COUNTOF(pdesc->array);
		}
		
		color._argb.a = (px_uchar)(color._argb.a*scale);
		PX_GeoDrawPenCircleDecay(psurface, pdesc->array[i].x, pdesc->array[i].y, (pdesc->size * scale), color,1);
	}
}

PX_OBJECT_RENDER_FUNCTION(PX_Object_Firework01Render)
{
	PX_Object_Firework01* pdesc = PX_ObjectGetDesc(PX_Object_Firework01, pObject);
	pdesc->elapsed += elapsed;
	if (pdesc->state==PX_FIREWORK01_STATE_RAISING)
	{
		PX_Firework01ParticalUpdate(pdesc->particals, elapsed);
		PX_Object_Firework01ParticalRender(pdesc->particals, psurface,0,pdesc->elapsed);

		if (pdesc->elapsed>= pdesc->raisingTime)
		{
			px_int i;
			px_int j;
			px_color clr;
			pdesc->elapsed = 0;
			pdesc->state = PX_FIREWORK01_STATE_BLOOMING;

			for (i = 0; i < PX_COUNTOF(pdesc->particals); i++)
			{
				for (j = 0; j < PX_COUNTOF(pdesc->particals->array); j++)
				{
					pdesc->particals[i].array[j].x = pdesc->particals[0].array[0].x;
					pdesc->particals[i].array[j].y = pdesc->particals[0].array[0].y;
				}
			}
			clr = PX_COLOR(255, 128 + (PX_rand() % 128), 128 + (PX_rand() % 128), 128 + (PX_rand() % 128));
			for (i=0;i<32;i++)
			{
				
				pdesc->particals[i].vx = 300 * PX_sin_angle(360 * i / 32.f);
				pdesc->particals[i].vy = 300 * PX_cos_angle(360 * i / 32.f);
				pdesc->particals[i].clr = clr;
				pdesc->particals[i].size = 8;
			}

			clr = PX_COLOR(255, 128 + (PX_rand() % 128), 128 + (PX_rand() % 128), 128 + (PX_rand() % 128));
			for (i = 32; i < 64; i++)
			{
				pdesc->particals[i].vx = 200 * PX_sin_angle(360 * (i - 32) / 32.f);
				pdesc->particals[i].vy = 200 * PX_cos_angle(360 * (i - 32) / 32.f);
				pdesc->particals[i].clr = clr;
				pdesc->particals[i].size = 6;
			}

			clr = PX_COLOR(255, 128 + (PX_rand() % 128), 128 + (PX_rand() % 128), 128 + (PX_rand() % 128));
			for (i = 64; i < 96; i++)
			{
				pdesc->particals[i].vx = 100 * PX_sin_angle(360 * (i - 64) / 32.f);
				pdesc->particals[i].vy = 100 * PX_cos_angle(360 * (i - 64) / 32.f);
				pdesc->particals[i].clr = clr;
				pdesc->particals[i].size = 3;
			}
		}

	}
	else if (pdesc->state == PX_FIREWORK01_STATE_BLOOMING)
	{
		px_int i;
		for (i=0;i<PX_COUNTOF(pdesc->particals);i++)
		{
			PX_Firework01ParticalUpdate(pdesc->particals+i, elapsed);
			PX_Object_Firework01ParticalRender(pdesc->particals+i, psurface,1, pdesc->elapsed);
		}
		if (pdesc->elapsed>3000)
		{
			PX_ObjectDelayDelete(pObject);
		}
	}
}


PX_Object* PX_Object_Firework01Create(px_memorypool* mp, PX_Object* Parent, px_float x, px_float y)
{
	px_int i;
	PX_Object* pObject;
	PX_Object_Firework01* pdesc;
	pObject = PX_ObjectCreateEx(mp, Parent, x, y, 0, 0, 0, 0, PX_OBJECT_TYPE_FIREWORK, 0, PX_Object_Firework01Render, 0, 0, sizeof(PX_Object_Firework01));
	if (!pObject)
	{
		return pObject;
	}
	pdesc = PX_ObjectGetDesc0(PX_Object_Firework01, pObject);
	pdesc->state = PX_FIREWORK01_STATE_RAISING;
	pdesc->elapsed = 0;
	for (i=0;i<PX_COUNTOF(pdesc->particals[0].array);i++)
	{
		pdesc->particals[0].array[i].x = x;
		pdesc->particals[0].array[i].y = y;
	}
	pdesc->particals[0].vx = 0;
	pdesc->particals[0].vy = -400;
	pdesc->particals[0].size = 3;
	pdesc->raisingTime=800+PX_rand()%800;
	pdesc->particals[0].clr = PX_COLOR(255, 128 + (PX_rand() % 128), 128 + (PX_rand() % 128), 128 + (PX_rand() % 128));
	return pObject;
}



#define PX_FIREWORK02_PointNum 96
#define PX_FIREWORK02_ParticleNum 96
#define PX_FIREWORK02_ParticleForward 24
#define PX_FIREWORK02_BoomTime 800
#define PX_FIREWORK02_DisappearTime 2500
#define PX_FIREWORK02_DeleteTime 3500
#define PX_FIREWORK02_BoomSize 6
#define PX_FIREWORK02_CLRNUM 3

typedef struct {
	px_int x, y;
}MY_Point;

typedef struct {
	px_int ar, ag, ab;
	px_int br, bg, bb;
}MY_COLOR;

typedef enum {
	RISE, BOOM, DISAPPEAR
}FW_STATE;

typedef enum {
	FIRE = 0, PINK = 1, BULE = 2, YELL = 3
}COLORTYPE;

typedef struct {
	MY_Point pointList[PX_FIREWORK02_PointNum];
	px_float x, y;
	px_float vx, vy;
}PX_Object_Firework02_MY_Particle;

typedef struct {
	PX_Object_Firework02_MY_Particle particleList[PX_FIREWORK02_ParticleNum];
	MY_COLOR boomClr;
	FW_STATE state;
	COLORTYPE type;
	px_uint livingTime;
	px_uint particleTime;
}PX_Object_Firework02;

void PX_Firework02ParticleUpdate(PX_Object_Firework02* pdesc, px_uint elapsed) 
{
	px_int i, j;
	pdesc->particleTime += elapsed;

	if (pdesc->state == RISE) 
	{
		for (i = 0; i < PX_FIREWORK02_PointNum; i++)
		{
			pdesc->particleList[i].y += pdesc->particleList[i].vy * elapsed / 1000.0f;
			pdesc->particleList[i].vy += 300 * elapsed / 1000.0f;
			pdesc->particleList[i].pointList[PX_FIREWORK02_PointNum - 1].y = (px_int)pdesc->particleList[i].y;
		}

		if (pdesc->livingTime > PX_FIREWORK02_BoomTime) 
		{
			pdesc->state = BOOM;
			for (i = 0; i < PX_FIREWORK02_ParticleNum; i++) 
			{
				px_int v = PX_rand() % 400 + 100;
				pdesc->particleList[i].vx = (px_float)PX_cosd(i * 2 * 3.14159f / PX_FIREWORK02_ParticleForward) * v;
				pdesc->particleList[i].vy = (px_float)PX_sind(i * 2 * 3.14159f / PX_FIREWORK02_ParticleForward) * v;

				for (j = 0; j < PX_FIREWORK02_PointNum; j++) {
					pdesc->particleList[i].pointList[j].x = (px_int)pdesc->particleList[i].x;
					pdesc->particleList[i].pointList[j].y = (px_int)pdesc->particleList[i].y;
				}
			}
		}
	}
	else 
	{
		if (pdesc->livingTime > PX_FIREWORK02_DisappearTime) 
		{
			pdesc->state = DISAPPEAR;
		}
		for (i = 0; i < PX_FIREWORK02_ParticleNum; i++) {
			pdesc->particleList[i].x += pdesc->particleList[i].vx * elapsed / 1000.0f;
			pdesc->particleList[i].y += pdesc->particleList[i].vy * elapsed / 1000.0f;
			if (pdesc->particleTime > 20) {
				pdesc->particleList[i].vx *= 0.96f;
				pdesc->particleList[i].vy *= 0.96f;
			}
			pdesc->particleList[i].vy += 100 * elapsed / 1000.0f;
			pdesc->particleList[i].pointList[PX_FIREWORK02_PointNum - 1].x = (px_int)pdesc->particleList[i].x;
			pdesc->particleList[i].pointList[PX_FIREWORK02_PointNum - 1].y = (px_int)pdesc->particleList[i].y;
		}

	}
	if (pdesc->particleTime > 20) 
	{
		pdesc->particleTime -= 20;
		for (i = 0; i < PX_FIREWORK02_ParticleNum; i++) 
		{
			for (j = 0; j < PX_FIREWORK02_PointNum - 1; j++) 
			{
				pdesc->particleList[i].pointList[j] = pdesc->particleList[i].pointList[j + 1];
			}
		}
	}
}

void PX_Firework02ParticleRender(px_surface* psurface, PX_Object_Firework02* pdesc) {
	px_int i, j;
	if (pdesc->state == RISE) 
	{

		px_float riseSize = 3.0f;
		px_color riseClr = PX_COLOR(255, 255, 64, 0);
		for (i = PX_FIREWORK02_PointNum - 1; i > PX_FIREWORK02_PointNum / 2; i--) 
		{
			riseClr._argb.a = (px_uchar)(255.0f * (i - PX_FIREWORK02_PointNum / 2) / (PX_FIREWORK02_PointNum / 2));
			PX_GeoDrawPenCircleDecay(psurface, pdesc->particleList[0].x*1.0f, pdesc->particleList[0].pointList[i].y * 1.0f, riseSize, riseClr,1);
		}
	}
	else 
	{
		px_float boomSize = PX_FIREWORK02_BoomSize;
		for (i = 0; i < PX_FIREWORK02_ParticleNum; i++) 
		{
			for (j = 0; j < PX_FIREWORK02_PointNum - 1; j++) 
			{//j=0,clr=red
				px_float percent = (px_float)j / PX_FIREWORK02_PointNum;
				px_int R =(px_int) ((1 - percent) * pdesc->boomClr.ar + pdesc->boomClr.br * percent);
				px_int G = (px_int)((1 - percent) * pdesc->boomClr.ag + pdesc->boomClr.bg * percent);
				px_int B = (px_int)((1 - percent) * pdesc->boomClr.ab + pdesc->boomClr.bb * percent);
				px_color boomClr = PX_COLOR(255, R, G, B);
				if (pdesc->state == DISAPPEAR) {
					boomClr._argb.a = (px_uchar)(255.0f * (PX_FIREWORK02_DeleteTime - pdesc->livingTime) / (PX_FIREWORK02_DeleteTime - PX_FIREWORK02_DisappearTime));
					boomSize = (px_float)PX_FIREWORK02_BoomSize * (PX_FIREWORK02_DeleteTime - pdesc->livingTime) / (PX_FIREWORK02_DeleteTime - PX_FIREWORK02_DisappearTime);
				}
				PX_GeoDrawPenCircleDecay(psurface, pdesc->particleList[i].pointList[j].x*1.0f, pdesc->particleList[i].pointList[j].y*1.0f, boomSize * j / PX_FIREWORK02_PointNum, boomClr,1);
			}
		}
	}
}

PX_OBJECT_RENDER_FUNCTION(PX_Object_Firework02Render)
{
	PX_Object_Firework02* pdesc = PX_ObjectGetDesc(PX_Object_Firework02, pObject);

	pdesc->livingTime += elapsed;
	PX_Firework02ParticleUpdate(pdesc, elapsed);
	PX_Firework02ParticleRender(psurface, pdesc);

	if (pdesc->livingTime > PX_FIREWORK02_DeleteTime) {
		PX_ObjectDelayDelete(pObject);
	}
}

PX_Object* PX_Object_Firework02Create(px_memorypool* mp, PX_Object* Parent, px_float x, px_float y)
{
	PX_Object* pObject;
	PX_Object_Firework02* pdesc;
	px_int i, j;
	pObject = PX_ObjectCreateEx(mp, Parent, x, y, 0, 0, 0, 0, 0, 0, PX_Object_Firework02Render, 0, 0, sizeof(PX_Object_Firework02));
	if (!pObject)
	{
		return pObject;
	}
	pdesc = PX_ObjectGetDesc0(PX_Object_Firework02, pObject);
	for (i = 0; i < PX_FIREWORK02_ParticleNum; i++)
	{
		pdesc->particleList[i].x = x;
		pdesc->particleList[i].y = y;
		pdesc->particleList[i].vx = 0;
		pdesc->particleList[i].vy = -500;
		for (j = 0; j < PX_FIREWORK02_PointNum; j++) {
			pdesc->particleList[i].pointList[j].x = (px_int)x;
			pdesc->particleList[i].pointList[j].y = (px_int)y;
		}
	}
	pdesc->state = RISE;
	pdesc->livingTime = 0;
	pdesc->particleTime = 0;
	//COLOR
	pdesc->type =(COLORTYPE)(PX_rand() % PX_FIREWORK02_CLRNUM);
	switch (pdesc->type) {
	case FIRE:
		pdesc->boomClr.ar = 0xff;
		pdesc->boomClr.ab = 0x00;
		pdesc->boomClr.ag = 0x00;
		pdesc->boomClr.br = 0xff;
		pdesc->boomClr.bg = 0xff;
		pdesc->boomClr.bb = 0x00; break;
	case PINK:
		pdesc->boomClr.ar = 0x29;
		pdesc->boomClr.ab = 0xb3;
		pdesc->boomClr.ag = 0x36;
		pdesc->boomClr.br = 0xa5;
		pdesc->boomClr.bg = 0xff;
		pdesc->boomClr.bb = 0xad; break;
	case BULE:
		pdesc->boomClr.ar = 0xf0;
		pdesc->boomClr.ab = 0x00;
		pdesc->boomClr.ag = 0xff;
		pdesc->boomClr.br = 0xfa;
		pdesc->boomClr.bg = 0xab;
		pdesc->boomClr.bb = 0xff; break;
	default:
		break;
	}
	return pObject;
}

typedef struct {
	PX_Object_Firework02_MY_Particle particleList[PX_FIREWORK02_ParticleNum];
	MY_COLOR boomClr;
	FW_STATE state;
	COLORTYPE type;
	px_uint livingTime;
	px_uint boomTime;
	px_uint particleTime;
}PX_Object_Firework03;

px_void ParticleUpdate(PX_Object_Firework03* pdesc, px_uint elapsed) 
{
	px_int i, j;
	pdesc->particleTime += elapsed;

	if (pdesc->state == RISE) 
	{
		for (i = 0; i < PX_FIREWORK02_PointNum; i++)
		{
			pdesc->particleList[i].y += pdesc->particleList[i].vy * elapsed / 1000.0f;
			pdesc->particleList[i].vy += 300 * elapsed / 1000.0f;
			pdesc->particleList[i].pointList[PX_FIREWORK02_PointNum - 1].y = (px_int)pdesc->particleList[i].y;
		}

		if (pdesc->livingTime > pdesc->boomTime) {
			pdesc->state = BOOM;
			for (i = 0; i < PX_FIREWORK02_ParticleNum; i++) 
			{
				px_int v = PX_rand() % 400 + 100;
				pdesc->particleList[i].vx = (px_float)PX_cosd(i * 2 * 3.14159f / PX_FIREWORK02_ParticleForward) * v;
				pdesc->particleList[i].vy = (px_float)PX_sind(i * 2 * 3.14159f / PX_FIREWORK02_ParticleForward) * v;

				for (j = 0; j < PX_FIREWORK02_PointNum; j++) 
				{
					pdesc->particleList[i].pointList[j].x = (px_int)pdesc->particleList[i].x;
					pdesc->particleList[i].pointList[j].y = (px_int)pdesc->particleList[i].y;
				}
			}
		}
	}
	else {
		if (pdesc->livingTime > PX_FIREWORK02_DisappearTime) {
			pdesc->state = DISAPPEAR;
		}
		for (i = 0; i < PX_FIREWORK02_ParticleNum; i++) {
			pdesc->particleList[i].x += pdesc->particleList[i].vx * elapsed / 1000.0f;
			pdesc->particleList[i].y += pdesc->particleList[i].vy * elapsed / 1000.0f;
			if (pdesc->particleTime > 20) {
				pdesc->particleList[i].vx *= 0.96f;
				pdesc->particleList[i].vy *= 0.96f;
			}
			pdesc->particleList[i].vy += 100 * elapsed / 1000.0f;
			pdesc->particleList[i].pointList[PX_FIREWORK02_PointNum - 1].x = (px_int)pdesc->particleList[i].x;
			pdesc->particleList[i].pointList[PX_FIREWORK02_PointNum - 1].y = (px_int)pdesc->particleList[i].y;
		}

	}
	if (pdesc->particleTime > 20) 
	{
		pdesc->particleTime -= 20;
		for (i = 0; i < PX_FIREWORK02_ParticleNum; i++) 
		{
			for (j = 0; j < PX_FIREWORK02_PointNum - 1; j++) 
			{
				pdesc->particleList[i].pointList[j].x = pdesc->particleList[i].pointList[j + 1].x + ((px_int)(PX_rand() % 2)) - 1;
				pdesc->particleList[i].pointList[j].y = pdesc->particleList[i].pointList[j + 1].y + ((px_int)(PX_rand() % 2)) - 1;
			}
		}
	}
}

px_void ParticleRender(px_surface* psurface, PX_Object_Firework03* pdesc) 
{
	px_int i, j;
	if (pdesc->state == RISE) 
	{
		px_float riseSize = 3.0f;
		px_color riseClr = PX_COLOR(255, 255, 64, 0);
		for (i = PX_FIREWORK02_PointNum - 1; i > PX_FIREWORK02_PointNum / 2; i--)
		{
			riseClr._argb.a = (px_uchar)(255.0f * (i - PX_FIREWORK02_PointNum / 2) / (PX_FIREWORK02_PointNum / 2));
			PX_GeoDrawPenCircleDecay(psurface, pdesc->particleList[0].x * 1.0f, pdesc->particleList[0].pointList[i].y * 1.0f, riseSize, riseClr, 1);
		}
	}
	else {
		px_float boomSize = 4;
		for (i = 0; i < PX_FIREWORK02_ParticleNum; i++) 
		{
			for (j = 0; j < PX_FIREWORK02_PointNum - 1; j++) 
			{
				px_float percent = (px_float)j / PX_FIREWORK02_PointNum;
				px_int R = (px_int)((1 - percent) * pdesc->boomClr.ar + pdesc->boomClr.br * percent);
				px_int G = (px_int)((1 - percent) * pdesc->boomClr.ag + pdesc->boomClr.bg * percent);
				px_int B = (px_int)((1 - percent) * pdesc->boomClr.ab + pdesc->boomClr.bb * percent);
				px_color boomClr = PX_COLOR(255, R, G, B);
				if (pdesc->state == DISAPPEAR) {
					boomClr._argb.a =(px_uchar)(255.0f * (PX_FIREWORK02_DeleteTime - pdesc->livingTime) / (PX_FIREWORK02_DeleteTime - PX_FIREWORK02_DisappearTime));
					boomSize = 4.0f * (PX_FIREWORK02_DeleteTime - pdesc->livingTime) / (PX_FIREWORK02_DeleteTime - PX_FIREWORK02_DisappearTime);
				}
				PX_GeoDrawPenCircleDecay(psurface, pdesc->particleList[i].pointList[j].x*1.f, pdesc->particleList[i].pointList[j].y * 1.f, boomSize * j / PX_FIREWORK02_PointNum, boomClr,1);
			}
		}
	}
}

PX_OBJECT_RENDER_FUNCTION(PX_Object_Firework03Render)
{
	PX_Object_Firework03* pdesc = PX_ObjectGetDesc(PX_Object_Firework03, pObject);

	pdesc->livingTime += elapsed;
	ParticleUpdate(pdesc, elapsed);
	ParticleRender(psurface, pdesc);

	if (pdesc->livingTime > PX_FIREWORK02_DeleteTime) {
		PX_ObjectDelayDelete(pObject);
	}
}

PX_Object* PX_Object_Firework03Create(px_memorypool* mp, PX_Object* Parent, px_float x, px_float y)
{
	PX_Object* pObject;
	PX_Object_Firework03* pdesc;
	px_int i, j;
	pObject = PX_ObjectCreateEx(mp, Parent, x, y, 0, 0, 0, 0, 0, 0, PX_Object_Firework03Render, 0, 0, sizeof(PX_Object_Firework03));
	if (!pObject)
	{
		return pObject;
	}
	pdesc = PX_ObjectGetDesc0(PX_Object_Firework03, pObject);
	for (i = 0; i < PX_FIREWORK02_ParticleNum; i++)
	{
		pdesc->particleList[i].x = x;
		pdesc->particleList[i].y = y;
		pdesc->particleList[i].vx = 0;
		pdesc->particleList[i].vy = -500;
		for (j = 0; j < PX_FIREWORK02_PointNum; j++)
		{
			pdesc->particleList[i].pointList[j].x = (px_int)x;
			pdesc->particleList[i].pointList[j].y = (px_int)y;
		}
	}
	pdesc->state = RISE;
	pdesc->livingTime = 0;
	pdesc->boomTime = 1000 - PX_rand() % 600;
	pdesc->particleTime = 0;
	pdesc->type = (COLORTYPE)(PX_rand() % 4);
	switch (pdesc->type)
	{
	case FIRE:
		pdesc->boomClr.ar = 0x00;
		pdesc->boomClr.ag = 0xc8;
		pdesc->boomClr.ab = 0x00;
		pdesc->boomClr.br = 0xff;
		pdesc->boomClr.bg = 0xff;
		pdesc->boomClr.bb = 0xff; break;
	case PINK:
		pdesc->boomClr.ar = 0x00;
		pdesc->boomClr.ag = 0xff;
		pdesc->boomClr.ab = 0xb2;
		pdesc->boomClr.br = 0xff;
		pdesc->boomClr.bg = 0xff;
		pdesc->boomClr.bb = 0xff; break;
	case BULE:
		pdesc->boomClr.ar = 0xf0;
		pdesc->boomClr.ag = 0x00;
		pdesc->boomClr.ab = 0xff;
		pdesc->boomClr.br = 0xff;
		pdesc->boomClr.bg = 0xff;
		pdesc->boomClr.bb = 0xff; break;
	case YELL:
		pdesc->boomClr.ar = 0xff;
		pdesc->boomClr.ag = 0xd0;
		pdesc->boomClr.ab = 0x00;
		pdesc->boomClr.br = 0xff;
		pdesc->boomClr.bg = 0xff;
		pdesc->boomClr.bb = 0xff; break;
	default:
		break;
	}
	return pObject;
}


/////////////////////////////////////////////////////////////////////////
// Firework04: Double burst - first explosion triggers secondary explosions
/////////////////////////////////////////////////////////////////////////

#define PX_FIREWORK04_TRAIL_LEN 32
#define PX_FIREWORK04_PRIMARY_NUM 16
#define PX_FIREWORK04_SECONDARY_NUM 12
#define PX_FIREWORK04_TOTAL_NUM (PX_FIREWORK04_PRIMARY_NUM * (1 + PX_FIREWORK04_SECONDARY_NUM))

typedef enum {
	PX_FIREWORK04_STATE_RAISING,
	PX_FIREWORK04_STATE_BLOOM1,
	PX_FIREWORK04_STATE_BLOOM2,
}PX_FIREWORK04_STATE;

typedef struct {
	px_point trail[PX_FIREWORK04_TRAIL_LEN];
	px_float vx, vy;
	px_float size;
	px_color clr;
	px_dword trailDelay;
}PX_Firework04_Particle;

typedef struct {
	PX_Firework04_Particle raising;
	PX_Firework04_Particle primary[PX_FIREWORK04_PRIMARY_NUM];
	PX_Firework04_Particle secondary[PX_FIREWORK04_PRIMARY_NUM * PX_FIREWORK04_SECONDARY_NUM];
	PX_FIREWORK04_STATE state;
	px_dword elapsed;
	px_dword raisingTime;
	px_color clr1;
	px_color clr2;
}PX_Object_Firework04;

static void PX_Firework04_ParticleUpdate(PX_Firework04_Particle* p, px_float gravity, px_float drag, px_dword elapsed)
{
	px_dword atom;
	px_int i;
	while (elapsed)
	{
		atom = elapsed > 10 ? 10 : elapsed;
		elapsed -= atom;

		p->trail[0].x += p->vx * atom / 1000.f;
		p->trail[0].y += p->vy * atom / 1000.f;
		p->vx -= p->vx * drag * atom / 1000.f;
		p->vy -= p->vy * drag * atom / 1000.f;
		p->vy += gravity * atom / 1000.f;

		p->trailDelay += atom;
		while (p->trailDelay > 20)
		{
			p->trailDelay -= 20;
			for (i = PX_FIREWORK04_TRAIL_LEN - 1; i > 0; i--)
				p->trail[i] = p->trail[i - 1];
		}
	}
}

static void PX_Firework04_ParticleRender(PX_Firework04_Particle* p, px_surface* psurface, px_float alphaScale)
{
	px_int i;
	for (i = PX_FIREWORK04_TRAIL_LEN - 1; i >= 0; i--)
	{
		px_float scale = (PX_FIREWORK04_TRAIL_LEN - i) * 1.0f / PX_FIREWORK04_TRAIL_LEN;
		px_color c = p->clr;
		c._argb.a = (px_uchar)(c._argb.a * scale * alphaScale);
		PX_GeoDrawPenCircleDecay(psurface, p->trail[i].x, p->trail[i].y, p->size * scale, c, 1);
	}
}

PX_OBJECT_RENDER_FUNCTION(PX_Object_Firework04Render)
{
	px_int i, j, k;
	PX_Object_Firework04* pdesc = PX_ObjectGetDesc(PX_Object_Firework04, pObject);
	pdesc->elapsed += elapsed;

	if (pdesc->state == PX_FIREWORK04_STATE_RAISING)
	{
		PX_Firework04_ParticleUpdate(&pdesc->raising, 0, 0.5f, elapsed);
		PX_Firework04_ParticleRender(&pdesc->raising, psurface, 1.0f);

		if (pdesc->elapsed >= pdesc->raisingTime)
		{
			px_float cx = pdesc->raising.trail[0].x;
			px_float cy = pdesc->raising.trail[0].y;
			pdesc->state = PX_FIREWORK04_STATE_BLOOM1;
			pdesc->elapsed = 0;

			for (i = 0; i < PX_FIREWORK04_PRIMARY_NUM; i++)
			{
				px_float angle = 360.0f * i / PX_FIREWORK04_PRIMARY_NUM + ((px_int)(PX_rand() % 30) - 15);
				px_float speed = 200.0f + PX_rand() % 150;
				for (j = 0; j < PX_FIREWORK04_TRAIL_LEN; j++)
				{
					pdesc->primary[i].trail[j].x = cx;
					pdesc->primary[i].trail[j].y = cy;
				}
				pdesc->primary[i].vx = speed * PX_sin_angle(angle);
				pdesc->primary[i].vy = speed * PX_cos_angle(angle);
				pdesc->primary[i].size = 5.0f + (PX_rand() % 30) / 10.0f;
				pdesc->primary[i].clr = pdesc->clr1;
				pdesc->primary[i].clr._argb.r = (px_uchar)(pdesc->clr1._argb.r - (px_int)(PX_rand() % 40));
				pdesc->primary[i].clr._argb.g = (px_uchar)(pdesc->clr1._argb.g - (px_int)(PX_rand() % 40));
				pdesc->primary[i].trailDelay = 0;
			}
		}
	}
	else if (pdesc->state == PX_FIREWORK04_STATE_BLOOM1)
	{
		px_float alpha = 1.0f;
		if (pdesc->elapsed > 600)
			alpha = (1000.0f - pdesc->elapsed) / 400.0f;
		if (alpha < 0) alpha = 0;

		for (i = 0; i < PX_FIREWORK04_PRIMARY_NUM; i++)
		{
			PX_Firework04_ParticleUpdate(&pdesc->primary[i], 50, 1.5f, elapsed);
			PX_Firework04_ParticleRender(&pdesc->primary[i], psurface, alpha);
		}

		if (pdesc->elapsed >= 500)
		{
			pdesc->state = PX_FIREWORK04_STATE_BLOOM2;
			pdesc->elapsed = 0;

			for (i = 0; i < PX_FIREWORK04_PRIMARY_NUM; i++)
			{
				px_float cx = pdesc->primary[i].trail[0].x;
				px_float cy = pdesc->primary[i].trail[0].y;
				px_color subClr = PX_COLOR(255, 128 + PX_rand() % 128, 128 + PX_rand() % 128, 128 + PX_rand() % 128);
				for (k = 0; k < PX_FIREWORK04_SECONDARY_NUM; k++)
				{
					px_int idx = i * PX_FIREWORK04_SECONDARY_NUM + k;
					px_float angle = 360.0f * k / PX_FIREWORK04_SECONDARY_NUM + ((px_int)(PX_rand() % 40) - 20);
					px_float speed = 80.0f + PX_rand() % 160;
					for (j = 0; j < PX_FIREWORK04_TRAIL_LEN; j++)
					{
						pdesc->secondary[idx].trail[j].x = cx;
						pdesc->secondary[idx].trail[j].y = cy;
					}
					pdesc->secondary[idx].vx = speed * PX_sin_angle(angle);
					pdesc->secondary[idx].vy = speed * PX_cos_angle(angle);
					pdesc->secondary[idx].size = 2.0f + (PX_rand() % 20) / 10.0f;
					pdesc->secondary[idx].clr = subClr;
					pdesc->secondary[idx].clr._argb.r = (px_uchar)(subClr._argb.r - (px_int)(PX_rand() % 30));
					pdesc->secondary[idx].clr._argb.g = (px_uchar)(subClr._argb.g - (px_int)(PX_rand() % 30));
					pdesc->secondary[idx].trailDelay = 0;
				}
			}
		}
	}
	else if (pdesc->state == PX_FIREWORK04_STATE_BLOOM2)
	{
		px_float alpha;
		if (pdesc->elapsed > 2000) pdesc->elapsed = 2000;
		alpha = (2000.0f - pdesc->elapsed) / 2000.0f;

		for (i = 0; i < PX_FIREWORK04_PRIMARY_NUM * PX_FIREWORK04_SECONDARY_NUM; i++)
		{
			PX_Firework04_ParticleUpdate(&pdesc->secondary[i], 80, 1.2f, elapsed);
			PX_Firework04_ParticleRender(&pdesc->secondary[i], psurface, alpha);
		}

		if (pdesc->elapsed >= 2000)
		{
			PX_ObjectDelayDelete(pObject);
		}
	}
}

PX_Object* PX_Object_Firework04Create(px_memorypool* mp, PX_Object* Parent, px_float x, px_float y)
{
	px_int i;
	PX_Object* pObject;
	PX_Object_Firework04 *pdesc;
	pObject = PX_ObjectCreateEx(mp, Parent, x, y, 0, 0, 0, 0, 0, 0, PX_Object_Firework04Render, 0, 0, sizeof(PX_Object_Firework04));
	if (!pObject)
	{
		return pObject;
	}
	pdesc = PX_ObjectGetDesc0(PX_Object_Firework04, pObject);
	pdesc->state = PX_FIREWORK04_STATE_RAISING;
	pdesc->elapsed = 0;
	pdesc->raisingTime = 700 + PX_rand() % 600;

	for (i = 0; i < PX_FIREWORK04_TRAIL_LEN; i++)
	{
		pdesc->raising.trail[i].x = x;
		pdesc->raising.trail[i].y = y;
	}
	pdesc->raising.vx = ((px_int)(PX_rand() % 60) - 30) * 1.0f;
	pdesc->raising.vy = -450;
	pdesc->raising.size = 3;
	pdesc->raising.clr = PX_COLOR(255, 255, 200, 100);

	pdesc->clr1 = PX_COLOR(255, 128 + PX_rand() % 128, 128 + PX_rand() % 128, 128 + PX_rand() % 128);
	pdesc->clr2 = PX_COLOR(255, 128 + PX_rand() % 128, 128 + PX_rand() % 128, 128 + PX_rand() % 128);
	return pObject;
	 
}


/////////////////////////////////////////////////////////////////////////
// Firework05: Spiral helix - particles travel in spiral/helix pattern
/////////////////////////////////////////////////////////////////////////

#define PX_FIREWORK05_TRAIL_LEN 48
#define PX_FIREWORK05_ARM_NUM 6
#define PX_FIREWORK05_PARTICLES_PER_ARM 8
#define PX_FIREWORK05_TOTAL (PX_FIREWORK05_ARM_NUM * PX_FIREWORK05_PARTICLES_PER_ARM)

typedef enum {
	PX_FIREWORK05_STATE_RAISING,
	PX_FIREWORK05_STATE_SPIRAL,
}PX_FIREWORK05_STATE;

typedef struct {
	px_point trail[PX_FIREWORK05_TRAIL_LEN];
	px_float angle;
	px_float radius;
	px_float radialSpeed;
	px_float angularSpeed;
	px_float size;
	px_color clr;
	px_dword trailDelay;
}PX_Firework05_Particle;

typedef struct {
	PX_Firework05_Particle raising;
	PX_Firework05_Particle particles[PX_FIREWORK05_TOTAL];
	PX_FIREWORK05_STATE state;
	px_dword elapsed;
	px_dword raisingTime;
	px_float centerX, centerY;
}PX_Object_Firework05;

static void PX_Firework05_RaisingUpdate(PX_Firework05_Particle* p, px_dword elapsed)
{
	px_dword atom;
	px_int i;
	while (elapsed)
	{
		atom = elapsed > 10 ? 10 : elapsed;
		elapsed -= atom;

		p->trail[0].x += 0;
		p->trail[0].y += p->angularSpeed * atom / 1000.f;
		p->angularSpeed += 200 * atom / 1000.f;

		p->trailDelay += atom;
		while (p->trailDelay > 20)
		{
			p->trailDelay -= 20;
			for (i = PX_FIREWORK05_TRAIL_LEN - 1; i > 0; i--)
				p->trail[i] = p->trail[i - 1];
		}
	}
}

static void PX_Firework05_SpiralUpdate(PX_Firework05_Particle* p, px_float cx, px_float cy, px_dword elapsed)
{
	px_dword atom;
	px_int i;
	while (elapsed)
	{
		atom = elapsed > 10 ? 10 : elapsed;
		elapsed -= atom;

		p->angle += p->angularSpeed * atom / 1000.f;
		p->radius += p->radialSpeed * atom / 1000.f;
		p->radialSpeed -= p->radialSpeed * 0.3f * atom / 1000.f;

		p->trail[0].x = cx + p->radius * PX_sin_angle(p->angle);
		p->trail[0].y = cy + p->radius * PX_cos_angle(p->angle);

		p->trailDelay += atom;
		while (p->trailDelay > 15)
		{
			p->trailDelay -= 15;
			for (i = PX_FIREWORK05_TRAIL_LEN - 1; i > 0; i--)
				p->trail[i] = p->trail[i - 1];
		}
	}
}

static void PX_Firework05_TrailRender(PX_Firework05_Particle* p, px_surface* psurface, px_float alphaScale)
{
	px_int i;
	for (i = PX_FIREWORK05_TRAIL_LEN - 1; i >= 0; i--)
	{
		px_float scale = (PX_FIREWORK05_TRAIL_LEN - i) * 1.0f / PX_FIREWORK05_TRAIL_LEN;
		px_color c = p->clr;
		c._argb.a = (px_uchar)(c._argb.a * scale * alphaScale);
		PX_GeoDrawPenCircleDecay(psurface, p->trail[i].x, p->trail[i].y, p->size * scale, c, 1);
	}
}

PX_OBJECT_RENDER_FUNCTION(PX_Object_Firework05Render)
{
	px_int i;
	PX_Object_Firework05* pdesc = PX_ObjectGetDesc(PX_Object_Firework05, pObject);
	pdesc->elapsed += elapsed;

	if (pdesc->state == PX_FIREWORK05_STATE_RAISING)
	{
		PX_Firework05_RaisingUpdate(&pdesc->raising, elapsed);
		PX_Firework05_TrailRender(&pdesc->raising, psurface, 1.0f);

		if (pdesc->elapsed >= pdesc->raisingTime)
		{
			px_int arm, p_idx;
			px_color armColors[PX_FIREWORK05_ARM_NUM];
			pdesc->centerX = pdesc->raising.trail[0].x;
			pdesc->centerY = pdesc->raising.trail[0].y;
			pdesc->state = PX_FIREWORK05_STATE_SPIRAL;
			pdesc->elapsed = 0;

			for (i = 0; i < PX_FIREWORK05_ARM_NUM; i++)
			{
				armColors[i] = PX_COLOR(255, 128 + PX_rand() % 128, 128 + PX_rand() % 128, 128 + PX_rand() % 128);
			}

			for (arm = 0; arm < PX_FIREWORK05_ARM_NUM; arm++)
			{
				px_float baseAngle = 360.0f * arm / PX_FIREWORK05_ARM_NUM;
				for (p_idx = 0; p_idx < PX_FIREWORK05_PARTICLES_PER_ARM; p_idx++)
				{
					px_int idx = arm * PX_FIREWORK05_PARTICLES_PER_ARM + p_idx;
					px_int j;
					for (j = 0; j < PX_FIREWORK05_TRAIL_LEN; j++)
					{
						pdesc->particles[idx].trail[j].x = pdesc->centerX;
						pdesc->particles[idx].trail[j].y = pdesc->centerY;
					}
					pdesc->particles[idx].angle = baseAngle;
					pdesc->particles[idx].radius = 0;
					pdesc->particles[idx].radialSpeed = 100.0f + p_idx * 30.0f + PX_rand() % 30;
					pdesc->particles[idx].angularSpeed = 200.0f + PX_rand() % 100;
					pdesc->particles[idx].size = 5.0f - p_idx * 0.4f;
					pdesc->particles[idx].clr = armColors[arm];
					pdesc->particles[idx].trailDelay = 0;
				}
			}
		}
	}
	else
	{
		px_float alpha;
		if (pdesc->elapsed > 3500) pdesc->elapsed = 3500;
		alpha = pdesc->elapsed < 2500 ? 1.0f : (3500.0f - pdesc->elapsed) / 1000.0f;
		if (alpha < 0) alpha = 0;

		for (i = 0; i < PX_FIREWORK05_TOTAL; i++)
		{
			PX_Firework05_SpiralUpdate(&pdesc->particles[i], pdesc->centerX, pdesc->centerY, elapsed);
			PX_Firework05_TrailRender(&pdesc->particles[i], psurface, alpha);
		}

		if (pdesc->elapsed >= 3500)
		{
			PX_ObjectDelayDelete(pObject);
		}
	}
}

PX_Object* PX_Object_Firework05Create(px_memorypool* mp, PX_Object* Parent, px_float x, px_float y)
{
	px_int i;
	PX_Object* pObject;
	PX_Object_Firework05* pdesc;
	pObject = PX_ObjectCreateEx(mp, Parent, x, y, 0, 0, 0, 0, 0, 0, PX_Object_Firework05Render, 0, 0, sizeof(PX_Object_Firework05));
	if (!pObject)
	{
		return pObject;
	}
	pdesc = PX_ObjectGetDesc0(PX_Object_Firework05, pObject);
	pdesc->state = PX_FIREWORK05_STATE_RAISING;
	pdesc->elapsed = 0;
	pdesc->raisingTime = 700 + PX_rand() % 600;

	for (i = 0; i < PX_FIREWORK05_TRAIL_LEN; i++)
	{
		pdesc->raising.trail[i].x = x;
		pdesc->raising.trail[i].y = y;
	}
	pdesc->raising.angularSpeed = -500;
	pdesc->raising.size = 3;
	pdesc->raising.clr = PX_COLOR(255, 255, 220, 128);

	return pObject;
}


/////////////////////////////////////////////////////////////////////////
// Firework06: Willow/waterfall - particles burst then droop downward
/////////////////////////////////////////////////////////////////////////

#define PX_FIREWORK06_TRAIL_LEN 40
#define PX_FIREWORK06_PARTICLE_NUM 128

typedef enum {
	PX_FIREWORK06_STATE_RAISING,
	PX_FIREWORK06_STATE_BLOOMING,
}PX_FIREWORK06_STATE;

typedef struct {
	px_point trail[PX_FIREWORK06_TRAIL_LEN];
	px_float vx, vy;
	px_float size;
	px_color clr;
	px_dword trailDelay;
}PX_Firework06_Particle;

typedef struct {
	PX_Firework06_Particle raising;
	PX_Firework06_Particle particles[PX_FIREWORK06_PARTICLE_NUM];
	PX_FIREWORK06_STATE state;
	px_dword elapsed;
	px_dword raisingTime;
}PX_Object_Firework06;

static void PX_Firework06_ParticlePhysics(PX_Firework06_Particle* p, px_float gravity, px_float drag, px_dword elapsed)
{
	px_dword atom;
	px_int i;
	while (elapsed)
	{
		atom = elapsed > 10 ? 10 : elapsed;
		elapsed -= atom;

		p->trail[0].x += p->vx * atom / 1000.f;
		p->trail[0].y += p->vy * atom / 1000.f;
		p->vx -= p->vx * drag * atom / 1000.f;
		p->vy -= p->vy * drag * atom / 1000.f;
		p->vy += gravity * atom / 1000.f;

		p->trailDelay += atom;
		while (p->trailDelay > 25)
		{
			p->trailDelay -= 25;
			for (i = PX_FIREWORK06_TRAIL_LEN - 1; i > 0; i--)
				p->trail[i] = p->trail[i - 1];
		}
	}
}

static void PX_Firework06_TrailRender(PX_Firework06_Particle* p, px_surface* psurface, px_float alphaScale)
{
	px_int i;
	for (i = PX_FIREWORK06_TRAIL_LEN - 1; i >= 0; i--)
	{
		px_float scale = (PX_FIREWORK06_TRAIL_LEN - i) * 1.0f / PX_FIREWORK06_TRAIL_LEN;
		px_color c = p->clr;
		px_uchar a = (px_uchar)(c._argb.a * scale * alphaScale);
		c._argb.a = a;
		PX_GeoDrawPenCircleDecay(psurface, p->trail[i].x, p->trail[i].y, p->size * (0.3f + 0.7f * scale), c, 1);
	}
}

PX_OBJECT_RENDER_FUNCTION(PX_Object_Firework06Render)
{
	px_int i;
	PX_Object_Firework06* pdesc = PX_ObjectGetDesc(PX_Object_Firework06, pObject);
	pdesc->elapsed += elapsed;

	if (pdesc->state == PX_FIREWORK06_STATE_RAISING)
	{
		PX_Firework06_ParticlePhysics(&pdesc->raising, 0, 0.5f, elapsed);
		PX_Firework06_TrailRender(&pdesc->raising, psurface, 1.0f);

		if (pdesc->elapsed >= pdesc->raisingTime)
		{
			px_float cx = pdesc->raising.trail[0].x;
			px_float cy = pdesc->raising.trail[0].y;
			px_color baseClr1 = PX_COLOR(255, 180 + PX_rand() % 76, 140 + PX_rand() % 116, 60 + PX_rand() % 100);
			px_color baseClr2 = PX_COLOR(255, 180 + PX_rand() % 76, 140 + PX_rand() % 116, 60 + PX_rand() % 100);
			pdesc->state = PX_FIREWORK06_STATE_BLOOMING;
			pdesc->elapsed = 0;

			for (i = 0; i < PX_FIREWORK06_PARTICLE_NUM; i++)
			{
				px_int j;
				px_float angle = (PX_rand() % 3600) / 10.0f;
				px_float speed = 100.0f + PX_rand() % 250;
				px_float t = (PX_rand() % 100) / 100.0f;
				px_color pclr;
				for (j = 0; j < PX_FIREWORK06_TRAIL_LEN; j++)
				{
					pdesc->particles[i].trail[j].x = cx;
					pdesc->particles[i].trail[j].y = cy;
				}
				pdesc->particles[i].vx = speed * PX_sin_angle(angle) + ((px_int)(PX_rand() % 40) - 20);
				pdesc->particles[i].vy = speed * PX_cos_angle(angle) - 30 - (px_int)(PX_rand() % 40);
				pdesc->particles[i].size = 2.0f + (PX_rand() % 40) / 10.0f;
				pclr._argb.a = 255;
				pclr._argb.r = (px_uchar)(baseClr1._argb.r * (1 - t) + baseClr2._argb.r * t);
				pclr._argb.g = (px_uchar)(baseClr1._argb.g * (1 - t) + baseClr2._argb.g * t);
				pclr._argb.b = (px_uchar)(baseClr1._argb.b * (1 - t) + baseClr2._argb.b * t);
				pdesc->particles[i].clr = pclr;
				pdesc->particles[i].trailDelay = 0;
			}
		}
	}
	else
	{
		px_float alpha;
		px_dword bloomElapsed = pdesc->elapsed;
		if (bloomElapsed > 4000) bloomElapsed = 4000;
		alpha = bloomElapsed < 2500 ? 1.0f : (4000.0f - bloomElapsed) / 1500.0f;
		if (alpha < 0) alpha = 0;

		for (i = 0; i < PX_FIREWORK06_PARTICLE_NUM; i++)
		{
			PX_Firework06_ParticlePhysics(&pdesc->particles[i], 150, 0.6f, elapsed);
			PX_Firework06_TrailRender(&pdesc->particles[i], psurface, alpha);
		}

		if (pdesc->elapsed >= 4000)
		{
			PX_ObjectDelayDelete(pObject);
		}
	}
}

PX_Object* PX_Object_Firework06Create(px_memorypool* mp, PX_Object* Parent, px_float x, px_float y)
{
	px_int i;
	PX_Object* pObject;
	PX_Object_Firework06* pdesc;
	pObject = PX_ObjectCreateEx(mp, Parent, x, y, 0, 0, 0, 0, 0, 0, PX_Object_Firework06Render, 0, 0, sizeof(PX_Object_Firework06));
	if (!pObject)
	{
		return pObject;
	}
	pdesc = PX_ObjectGetDesc0(PX_Object_Firework06, pObject);
	pdesc->state = PX_FIREWORK06_STATE_RAISING;
	pdesc->elapsed = 0;
	pdesc->raisingTime = 800 + PX_rand() % 500;

	for (i = 0; i < PX_FIREWORK06_TRAIL_LEN; i++)
	{
		pdesc->raising.trail[i].x = x;
		pdesc->raising.trail[i].y = y;
	}
	pdesc->raising.vx = 0;
	pdesc->raising.vy = -420;
	pdesc->raising.size = 3;
	pdesc->raising.clr = PX_COLOR(255, 255, 200, 100);

	return pObject;
}
