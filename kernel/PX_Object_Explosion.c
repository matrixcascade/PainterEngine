#include "PX_Object_Explosion.h"
px_void PX_Object_ExplosionRender(px_surface* psurface, PX_Object *pObject, px_dword elapsed)
{
	PX_Object_Explosion* pDesc = PX_ObjectGetDesc(PX_Object_Explosion, pObject);
	px_float objx, objy, objWidth, objHeight;
	px_float inheritX, inheritY;

	PX_ObjectGetInheritXY(pObject, &inheritX, &inheritY);

	objx = (pObject->x + inheritX);
	objy = (pObject->y + inheritY);
	objWidth = pObject->Width;
	objHeight = pObject->Height;

	pDesc->elapsed += elapsed;
	if (pDesc->elapsed > pDesc->alive)
	{
		PX_ObjectDelayDelete(pObject);
	}
	else
	{
		pDesc->launcher.LauncherInfo.position = PX_POINT(objx, objy, 0);
		PX_ParticalLauncherRender(psurface, &pDesc->launcher, elapsed);
	}
	
}



px_void PX_Object_ExplosionFree(PX_Object *pObject)
{
	PX_Object_Explosion* pDesc = PX_ObjectGetDesc(PX_Object_Explosion, pObject);
	PX_ParticalLauncherFree(&pDesc->launcher);
}

PX_Object* PX_Object_Explosion01Create(px_memorypool* mp,PX_Object *parent, px_float x, px_float y, px_int count, px_float size, px_float power, px_int alive, px_texture* ptexture)
{
	PX_Object* pObject;
	PX_Object_Explosion desc;
	PX_ParticalLauncher_InitializeInfo info;
	PX_memset(&desc, 0, sizeof(desc));
	desc.mp = mp;
	desc.alive = alive;
	desc.power = power;
	desc.ptexture = ptexture;
	desc.size = size;

	PX_ParticalLauncherInitializeDefaultInfo(&info);
	info.velocity = power*100;
	info.deviation_velocity_max = 0;
	info.deviation_velocity_min = -80*power;
	info.direction = PX_POINT(0, -1, 0);
	info.ak = 0.65f;
	info.alpha = 1;
	info.launchCount = count;
	info.generateDuration = 0;
	info.maxCount = count;
	info.alphaincrease = -1.0f/(alive/1000.f);
	info.alive = alive;
	info.sizeincrease = 0.25f;
	info.rotation = 180;
	info.deviation_rotation = 90;
	info.deviation_atomsize_max = 0;
	info.deviation_atomsize_min = -0.8f;
	info.deviation_rangAngle = 360;
	info.deviation_position_distanceRange = size * 32;
	info.position = PX_POINT(x, y, 0);
	info.tex = ptexture;
	info.atomsize = size;

	PX_ParticalLauncherInitialize(&desc.launcher,mp, info);
	pObject= PX_ObjectCreateEx(mp, parent, x, y, 0, 0, 0, 0, PX_OBJECT_TYPE_EXPLOSIONX, 0, PX_Object_ExplosionRender, PX_Object_ExplosionFree, &desc, sizeof(desc));
	return pObject;
}


px_void PX_Object_Explosion02Render(px_surface* psurface, PX_Object* pObject, px_dword elapsed)
{
	PX_Object_Explosion02* pDesc = PX_ObjectGetDesc(PX_Object_Explosion02, pObject);
	px_float a;
	px_float size;
	px_float angle;
	
	pDesc->elapsed += elapsed;
	if (pDesc->elapsed > 500.f)
	{
		PX_ObjectDelayDelete(pObject);
		return;
	}

	a = 1 - pDesc->elapsed / 500.f;
	size = 100 + pDesc->elapsed / 10.0f;
	angle = pDesc->elapsed / 500.f * 90.f;
	PX_GeoDrawBorder(psurface, (px_int)(pObject->x - size / 2), (px_int)(pObject->y - size / 2), \
		(px_int)(pObject->x + size / 2), (px_int)(pObject->y + size / 2), 3, \
		PX_COLOR((px_byte)(a * 255), pDesc->color._argb.r, pDesc->color._argb.g, pDesc->color._argb.b)\
	);
	PX_GeoDrawRing(psurface, (px_int)pObject->x, (px_int)pObject->y, 50, 5, PX_COLOR((px_byte)(a * 255), pDesc->color._argb.r, pDesc->color._argb.g, pDesc->color._argb.b), -90 + (px_int)angle, (px_int)angle);
	PX_GeoDrawRing(psurface, (px_int)pObject->x, (px_int)pObject->y, 50, 5, PX_COLOR((px_byte)(a * 255), pDesc->color._argb.r, pDesc->color._argb.g, pDesc->color._argb.b), 90 + (px_int)angle, (px_int)angle + 180);
	//PX_GeoDrawSolidCircle(psurface, (px_int)pObject->x, (px_int)pObject->y, 5, PX_COLOR((px_byte)(a * 255), pDesc->color._argb.r, pDesc->color._argb.g, pDesc->color._argb.b));
	PX_ParticalLauncherRender(psurface, &pDesc->launcher, elapsed);

	
}


px_void PX_Object_Explosion02Free(PX_Object* pObject)
{
	PX_Object_Explosion02* pDesc = PX_ObjectGetDesc(PX_Object_Explosion02, pObject);
	PX_ParticalLauncherFree(&pDesc->launcher);
	PX_TextureFree(&pDesc->rect);
}

PX_Object* PX_Object_Explosion02Create(px_memorypool *mp,PX_Object *parent, px_float x, px_float y,px_color color)
{
	PX_Object_Explosion02 Desc,*pDesc;
	PX_Object* pObject;
	PX_ParticalLauncher_InitializeInfo info;
	PX_memset(&Desc, 0, sizeof(Desc));
	pObject = PX_ObjectCreateEx(mp, parent, x, y, 0, 0, 0, 0, 0, 0, PX_Object_Explosion02Render, PX_Object_Explosion02Free, &Desc, sizeof(Desc));
	pDesc = PX_ObjectGetDesc(PX_Object_Explosion02, pObject);

	pDesc->elapsed = 0;
	pDesc->mp = mp;
	pDesc->color = color;
	if (!PX_TextureCreate(mp, &pDesc->rect, 16, 16))
		return PX_NULL;
	PX_GeoDrawRect(&pDesc->rect, 0, 0, 15, 15, color);

	PX_ParticalLauncherInitializeDefaultInfo(&info);
	info.velocity = 600;
	info.deviation_velocity_max = 0;
	info.deviation_velocity_min = -580;
	info.direction = PX_POINT(0, -1, 0);
	info.ak = 3;
	info.launchCount = 8;
	info.generateDuration = 0;
	info.maxCount = 8;
	info.alphaincrease = -2;
	info.alive = 500;
	info.deviation_atomsize_max = 0;
	info.deviation_atomsize_min = -0.8f;
	info.deviation_rangAngle = 180;
	info.position = PX_POINT(x, y, 0);
	info.tex = &pDesc->rect;
	PX_ParticalLauncherInitialize(&pDesc->launcher,mp, info);
	
	return pObject;
}


void PX_Object_Explosion03Update(PX_Object *pObject, px_dword elapsed)
{
	px_int i;
	px_dword update_atom;
	PX_Object_Explosion03* pdesc= PX_ObjectGetDesc(PX_Object_Explosion03, pObject);
	pdesc->elapsed += elapsed;
	if (elapsed > 1000)
	{
		elapsed = 1000;
	}
	while (elapsed)
	{
		if (elapsed > 10)
		{
			update_atom = 10;
			elapsed -= 10;
		}
		else
		{
			update_atom = elapsed;
			elapsed = 0;
		}

		pdesc->switch_elapsed += update_atom;
		if (pdesc->switch_elapsed >= pdesc->switch_duration)
		{
			pdesc->switch_elapsed -= pdesc->switch_duration;
			if ((PX_rand() %1000)>=500)
			{
				pdesc->vx = pdesc->dirx*1.0f;
				pdesc->vy = 0;
			}
			else
			{
				pdesc->vx = 0;
				pdesc->vy = pdesc->diry*1.0f;
			}

		}

		pdesc->array[0].x += pdesc->vx * update_atom / 1000;
		pdesc->array[0].y += pdesc->vy * update_atom / 1000;

		pdesc->delay += update_atom;

		while (pdesc->delay > 10)
		{
			pdesc->delay -= 10;
			for (i = PX_COUNTOF(pdesc->array) - 1; i > 0; i--)
			{
				pdesc->array[i] = pdesc->array[i - 1];
			}
		}

		

	}
}

px_void PX_Object_Explosion03Render(px_surface* psurface, PX_Object* pObject,  px_dword elapsed)
{
	px_int i;
	px_float scale;
	px_color color;
	PX_Object_Explosion03* pdesc = PX_ObjectGetDesc(PX_Object_Explosion03, pObject);

	PX_Object_Explosion03Update(pObject, elapsed);

	if (pdesc->elapsed>pdesc->alive)
	{
		PX_ObjectDelayDelete(pObject);
		return;
	}


	for (i = PX_COUNTOF(pdesc->array) - 1; i >= 0; i--)
	{
		px_float a = 1.0f- pdesc->elapsed/ (1.0f * pdesc->alive);
		color = pdesc->clr;
		color._argb.a = (px_uchar)(color._argb.a * a );
		scale = 1.0f-i / (1.0f*PX_COUNTOF(pdesc->array))  ;

		color._argb.a = (px_uchar)(color._argb.a * scale);
		PX_GeoDrawRect(psurface, \
			(px_int)(pdesc->array[i].x-pdesc->size/2), (px_int)(pdesc->array[i].y - pdesc->size / 2),\
			(px_int)(pdesc->array[i].x + pdesc->size / 2), (px_int)(pdesc->array[i].y + pdesc->size / 2),\
			 color);
	}
}

PX_Object* PX_Object_Explosion03Create(px_memorypool* mp, PX_Object* parent, px_float x, px_float y, px_int dirx, px_int diry,  px_float size,px_dword switch_duration, px_dword alive, px_color color)
{
	PX_Object_Explosion03 Desc,*pDesc;
	PX_Object* pObject;


	px_int i;
	PX_memset(&Desc, 0, sizeof(Desc));
	Desc.clr = color;
	Desc.delay = 0;
	Desc.dirx = dirx;
	Desc.diry = diry;
	Desc.size = size;
	Desc.switch_duration = switch_duration;
	Desc.switch_elapsed = switch_duration;
	Desc.alive = alive;
	pObject = PX_ObjectCreateEx(mp, parent, x, y, 0, 0, 0, 0, 0, 0, PX_Object_Explosion03Render, 0, &Desc, sizeof(Desc));

	pDesc = PX_ObjectGetDesc(PX_Object_Explosion03, pObject);
	for (i = 0; i < PX_COUNTOF(pDesc->array); i++)
	{
		pDesc->array[i] = PX_POINT(x, y, 0);
	}

	
	return pObject;
}

px_void PX_Object_Explosion04Render(px_surface* psurface, PX_Object* pObject, px_dword elpased)
{
	PX_Object_Explosion04* pDesc =PX_ObjectGetDesc(PX_Object_Explosion04, pObject);
	px_float fr;
	px_float objx, objy, objWidth, objHeight;
	px_float inheritX, inheritY;
	PX_ObjectGetInheritXY(pObject, &inheritX, &inheritY);

	objx = (pObject->x + inheritX);
	objy = (pObject->y + inheritY);
	objWidth = pObject->Width;
	objHeight = pObject->Height;

	pDesc->elapsed += elpased;
	if (pDesc->elapsed > pDesc->alive)
	{
		PX_ObjectDelayDelete(pObject);
		return;
	}
	fr = pDesc->elapsed / (px_float)pDesc->alive;
	PX_GeoDrawPenRing(psurface, objx, objy,(PX_sqrt(fr) * pDesc->range * 4), ((1-fr) * pDesc->range * 0.3f+1), pDesc->color);
}

PX_Object* PX_Object_Explosion04Create(px_memorypool* mp, PX_Object* parent, px_float x, px_float y, px_color color, px_float range,px_dword alive)
{
	PX_Object_Explosion04 * pDesc;
	PX_Object* pObject;
	pObject = PX_ObjectCreateEx(mp, parent, x, y, 0, 0, 0, 0, 0, PX_NULL, PX_Object_Explosion04Render, 0, PX_NULL, sizeof(PX_Object_Explosion04));
	pDesc=PX_ObjectGetDesc(PX_Object_Explosion04, pObject);
	pDesc->color = color;
	pDesc->range = range;
	pDesc->elapsed = 0;
	pDesc->alive = alive;
	return pObject;
}



