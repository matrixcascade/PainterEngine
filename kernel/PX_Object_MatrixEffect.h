#ifndef MXTRIXEFFECT_UI_H
#define MXTRIXEFFECT_UI_H
#include "PX_Object.h"

#define PC_UI_CHAR_HEIGHT 16
#define PC_UI_CHAR_WIDTH 8

//////////////////////////////////////////////////////////////////////////
//
//i
//0-3 light
//4-7 dark
//8 none
//t change times
//a alpha
//elpased elpased to lastUpdate
//activated 
typedef struct{
	px_char c;
	px_char i;
	px_char t;
	px_uint a;
	px_bool activated;
	px_uint elpased;
}PX_UI_MATRIXEFFECT_MAP;

typedef enum
{
	PC_UI_ADVANCE_X_INC,
	PC_UI_ADVANCE_X_DEC,
	PC_UI_ADVANCE_Y_INC,
	PC_UI_ADVANCE_Y_DEC
}PX_UI_MATRIXEFFECT_ADVANCE;



typedef struct  
{
	px_memorypool *mp;
    px_uint SWITCH_DURATION;
	px_uint ATOM_OPRAND_TIME;
	px_uint EXISTING_FLAG_TIMES;
	px_uint ADVANCE_TIME;
	px_uint AdvanceElpased;
	px_int NONE_FLAG_TIMES;
	px_int ALPHA_DEC_SPEED;
	px_int *CharAdvanceHorizontal,*CharAdvanceVertical;
	px_int CharYCount;
	px_int CharXCount;
	px_bool Done;

	px_surface SwitchTarget;
	PX_UI_MATRIXEFFECT_MAP *CharMap;
	PX_UI_MATRIXEFFECT_ADVANCE PC_Ui_CharAdvanceType;
}PX_Object_MatrixEffect;

px_bool PX_Object_MatrixEffectCreate(px_memorypool* mp, PX_Object* parent, px_surface* pMapsurface, px_int Mode_0or1);
#endif
