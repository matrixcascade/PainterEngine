#ifndef PX_OBJECT_SCOREPANEL_H
#define PX_OBJECT_SCOREPANEL_H
#include "PX_Object.h"

typedef struct 
{
	px_memorypool * mp;
	px_float score;
	px_float current_displayscore;
	px_int lastupdateelapsed;
	px_texture scaleTexture;
	PX_FontModule* fm64, * fm32;
	px_int speed;
}PX_Object_ScorePanel;


PX_Object* PX_Object_ScorePanelCreate(px_memorypool* mp, PX_Object* parent, px_float x, px_float y, PX_FontModule* fm64, px_int speed);
px_void PX_Object_ScorePanelAddScore(PX_Object* pObject, px_int score);
px_void PX_Object_ScorePanelSetScore(PX_Object* pObject, px_int score);
#endif