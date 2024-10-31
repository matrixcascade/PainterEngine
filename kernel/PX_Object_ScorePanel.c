#include "PX_Object_ScorePanel.h"

PX_OBJECT_RENDER_FUNCTION(PX_Object_ScorePanelRender)
{
	px_float objx, objy, objWidth, objHeight;
	px_float inheritX, inheritY;
	px_char content[64];
	px_float dis;
	PX_Object_ScorePanel* pDesc = PX_ObjectGetDesc(PX_Object_ScorePanel, pObject);

	PX_ObjectGetInheritXY(pObject, &inheritX, &inheritY);

	objx = (pObject->x + inheritX);
	objy = (pObject->y + inheritY);
	objWidth = pObject->Width;
	objHeight = pObject->Height;

	pDesc->lastupdateelapsed += elapsed;
	dis = pDesc->score- pDesc->current_displayscore;
	if (dis>600)
	{
		pDesc->current_displayscore = pDesc->score - 600;
	}
	else if (dis>elapsed/5)
	{
		pDesc->current_displayscore += elapsed / 2.f;
	}
	else
	{
		pDesc->current_displayscore = pDesc->score;
	}

	PX_itoa((px_int)pDesc->current_displayscore, content, sizeof(content), 10);
	PX_SurfaceClear(&pDesc->scaleTexture, 0, 0, pDesc->scaleTexture.width, pDesc->scaleTexture.height, PX_COLOR(0, 0, 0, 0));
	PX_FontModuleDrawText(&pDesc->scaleTexture, pDesc->fm64, pDesc->scaleTexture.width / 2, pDesc->scaleTexture.height / 2, PX_ALIGN_CENTER, content, PX_COLOR_WHITE);
	if (pDesc->lastupdateelapsed>300)
	{
		PX_TextureRender(psurface, &pDesc->scaleTexture, (px_int)objx, (px_int)objy, PX_ALIGN_CENTER, PX_NULL);
	}
	else
	{
		PX_TEXTURERENDER_BLEND blend;
		blend.alpha = pDesc->lastupdateelapsed / 300.f;
		blend.hdr_R = 1;
		blend.hdr_G = 1;
		blend.hdr_B = 1;
		PX_TextureRenderEx(psurface, &pDesc->scaleTexture, (px_int)objx, (px_int)objy, PX_ALIGN_CENTER, &blend, 2 - blend.alpha, 0);
	}

}

PX_OBJECT_FREE_FUNCTION(PX_Object_ScorePanelFree)
{
	PX_Object_ScorePanel* pDesc = PX_ObjectGetDesc(PX_Object_ScorePanel, pObject);
	PX_TextureFree(&pDesc->scaleTexture);
}

PX_Object* PX_Object_ScorePanelCreate(px_memorypool* mp, PX_Object* parent,px_float x,px_float y, PX_FontModule* fm64,px_int speed)
{
	PX_Object* pObject;
	PX_Object_ScorePanel desc;
	PX_memset(&desc, 0, sizeof(desc));
	desc.mp = mp;

	desc.fm64 = fm64;
	desc.score = 0;
	desc.current_displayscore = 0;
	desc.speed = speed;
	PX_TextureCreate(mp, &desc.scaleTexture, fm64->max_Width*6, fm64->max_Height);
	pObject = PX_ObjectCreateEx(mp, parent, x, y, 0, 0, 0, 0, PX_OBJECT_TYPE_SCOREPANEL, 0, PX_Object_ScorePanelRender, PX_Object_ScorePanelFree, &desc, sizeof(desc));
	return pObject;
}

px_void PX_Object_ScorePanelAddScore(PX_Object* pObject,px_int score)
{
	PX_Object_ScorePanel* pDesc = (PX_Object_ScorePanel *)PX_ObjectGetDescByType(pObject, PX_OBJECT_TYPE_SCOREPANEL);
	pDesc->score += (px_float)(score);
	pDesc->lastupdateelapsed = 0;
}

px_void PX_Object_ScorePanelSetScore(PX_Object* pObject, px_int score)
{
	PX_Object_ScorePanel* pDesc = (PX_Object_ScorePanel *)PX_ObjectGetDescByType(pObject, PX_OBJECT_TYPE_SCOREPANEL);
	pDesc->score = (px_float)(score);
	pDesc->lastupdateelapsed = 0;
}

