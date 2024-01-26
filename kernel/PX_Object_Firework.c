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

void PX_Object_Firework01Render(px_surface* psurface, PX_Object* pObject, px_dword elapsed)
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
	PX_Object_Firework01 desc;
	PX_memset(&desc, 0, sizeof(desc));
	desc.state = PX_FIREWORK01_STATE_RAISING;
	desc.elapsed = 0;
	for (i=0;i<PX_COUNTOF(desc.particals[0].array);i++)
	{
		desc.particals[0].array[i].x = x;
		desc.particals[0].array[i].y = y;
	}
	desc.particals[0].vx = 0;
	desc.particals[0].vy = -400;
	desc.particals[0].size = 3;
	desc.raisingTime=800+PX_rand()%800;
	desc.particals[0].clr = PX_COLOR(255, 128 + (PX_rand() % 128), 128 + (PX_rand() % 128), 128 + (PX_rand() % 128));
	return PX_ObjectCreateEx(mp, Parent, x, y, 0, 0, 0, 0, PX_OBJECT_TYPE_FIREWORK, 0, PX_Object_Firework01Render, 0, &desc, sizeof(desc));
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
void PX_Object_Firework02Render(px_surface* psurface, PX_Object* pObject, px_uint elapsed)
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
	PX_Object_Firework02 desc;
	px_int i, j;
	for (i = 0; i < PX_FIREWORK02_ParticleNum; i++) 
	{
		desc.particleList[i].x = x;
		desc.particleList[i].y = y;
		desc.particleList[i].vx = 0;
		desc.particleList[i].vy = -500;
		for (j = 0; j < PX_FIREWORK02_PointNum; j++) {
			desc.particleList[i].pointList[j].x = (px_int)x;
			desc.particleList[i].pointList[j].y = (px_int)y;
		}
	}
	desc.state = RISE;
	desc.livingTime = 0;
	desc.particleTime = 0;
	//COLOR  
	desc.type =(COLORTYPE)(PX_rand() % PX_FIREWORK02_CLRNUM);
	switch (desc.type) {
	case FIRE:
		desc.boomClr.ar = 0xff;
		desc.boomClr.ab = 0x00;
		desc.boomClr.ag = 0x00;
		desc.boomClr.br = 0xff;
		desc.boomClr.bg = 0xff;
		desc.boomClr.bb = 0x00; break;
	case PINK:
		desc.boomClr.ar = 0x29;
		desc.boomClr.ab = 0xb3;
		desc.boomClr.ag = 0x36;
		desc.boomClr.br = 0xa5;
		desc.boomClr.bg = 0xff;
		desc.boomClr.bb = 0xad; break;
	case BULE:
		desc.boomClr.ar = 0xf0;
		desc.boomClr.ab = 0x00;
		desc.boomClr.ag = 0xff;
		desc.boomClr.br = 0xfa;
		desc.boomClr.bg = 0xab;
		desc.boomClr.bb = 0xff; break;
	default:
		break;
	}
	return PX_ObjectCreateEx(mp, Parent, x, y, 0, 0, 0, 0, 0, 0, PX_Object_Firework02Render, 0, &desc, sizeof(desc));
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

void ParticleUpdate(PX_Object_Firework03* pdesc, px_uint elapsed) 
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
				pdesc->particleList[i].pointList[j].x = pdesc->particleList[i].pointList[j + 1].x + (PX_rand() % 2) - 1;
				pdesc->particleList[i].pointList[j].y = pdesc->particleList[i].pointList[j + 1].y + (PX_rand() % 2) - 1;
			}
		}
	}
}

void ParticleRender(px_surface* psurface, PX_Object_Firework03* pdesc) 
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
px_void PX_Object_Firework03Render(px_surface* psurface, PX_Object* pObject, px_uint elapsed)
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
	PX_Object_Firework03 desc = {0};
	px_int i, j;
	for (i = 0; i < PX_FIREWORK02_ParticleNum; i++) 
	{
		desc.particleList[i].x = x;
		desc.particleList[i].y = y;
		desc.particleList[i].vx = 0;
		desc.particleList[i].vy = -500;
		for (j = 0; j < PX_FIREWORK02_PointNum; j++) 
		{
			desc.particleList[i].pointList[j].x = (px_int)x;
			desc.particleList[i].pointList[j].y = (px_int)y;
		}
	}
	desc.state = RISE;
	desc.livingTime = 0;
	desc.boomTime = 1000 - PX_rand() % 600;
	desc.particleTime = 0;
	desc.type = (COLORTYPE)(PX_rand() % 4);
	switch (desc.type) 
	{
	case FIRE:
		desc.boomClr.ar = 0x00;
		desc.boomClr.ag = 0xc8;
		desc.boomClr.ab = 0x00;
		desc.boomClr.br = 0xff;
		desc.boomClr.bg = 0xff;
		desc.boomClr.bb = 0xff; break;
	case PINK:
		desc.boomClr.ar = 0x00;
		desc.boomClr.ag = 0xff;
		desc.boomClr.ab = 0xb2;
		desc.boomClr.br = 0xff;
		desc.boomClr.bg = 0xff;
		desc.boomClr.bb = 0xff; break;
	case BULE:
		desc.boomClr.ar = 0xf0;
		desc.boomClr.ag = 0x00;
		desc.boomClr.ab = 0xff;
		desc.boomClr.br = 0xff;
		desc.boomClr.bg = 0xff;
		desc.boomClr.bb = 0xff; break;
	case YELL:
		desc.boomClr.ar = 0xff;
		desc.boomClr.ag = 0xd0;
		desc.boomClr.ab = 0x00;
		desc.boomClr.br = 0xff;
		desc.boomClr.bg = 0xff;
		desc.boomClr.bb = 0xff; break;
	default:
		break;
	}
	return PX_ObjectCreateEx(mp, Parent, x, y, 0, 0, 0, 0, 0, 0, PX_Object_Firework03Render, 0, &desc, sizeof(desc));
}
