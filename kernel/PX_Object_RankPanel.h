#ifndef PX_OBJECT_RANKPANEL
#define PX_OBJECT_RANKPANEL

#include "PX_Object.h"

#define PX_OBJECT_RANKPANEL_MAX_STAGE_COLOR_COUNT 8
#define PX_OBJECT_RANKPANEL_RING1_NORMAL_SPEED 60
#define PX_OBJECT_RANKPANEL_RING1_MAX_SPEED 360
#define PX_OBJECT_RANKPANEL_RING2_NORMAL_SPEED 90
#define PX_OBJECT_RANKPANEL_RING2_MAX_SPEED 540
typedef struct  
{
	px_float max;
	px_float target_value;
	px_float current_value;
	px_float speed;
	px_int   stage;
	px_float a1_angle,a2_angle;
	px_color backgroundColor;
	PX_FontModule* numericFM;
	px_color rankColor[PX_OBJECT_RANKPANEL_MAX_STAGE_COLOR_COUNT+1];
}PX_Object_RankPanel;


PX_Object* PX_Object_RankPanelCreate(px_memorypool* mp, PX_Object* Parent,px_float x,px_float y,px_float radius,PX_FontModule *fm);
PX_Object_RankPanel* PX_Object_GetRankPanel(PX_Object* pObject);
px_void PX_Object_RankPanelSetValue(PX_Object* pObject, px_float value);
px_void PX_Object_RankPanelSetMaxValue(PX_Object* pObject, px_float value);
px_void PX_Object_RankPanelSetStage(PX_Object* pObject, px_int stage);
px_void PX_Object_RankPanelSetCurrentValue(PX_Object* pObject, px_float value);
px_float PX_Object_RankPanelGetValue(PX_Object* pObject);
px_void PX_Object_RankPanelSetSpeed(PX_Object* pObject, px_float value);
#endif