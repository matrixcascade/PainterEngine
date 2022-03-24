#ifndef PX_OBJECT_PIANO_H
#define PX_OBJECT_PIANO_H

#include "PX_Object.h"

typedef enum 
{
	PX_OBJECT_PIANO_KEYSTATE_STANDBY=0,
	PX_OBJECT_PIANO_KEYSTATE_DOWN,
	PX_OBJECT_PIANO_KEYSTATE_CURSOR,
	PX_OBJECT_PIANO_KEYSTATE_ONFOCUS,
}PX_OBJECT_PIANO_KEYSTATE;

typedef struct  
{
	px_memorypool* mp;
	px_int lastKeyDownIndex;
	PX_OBJECT_PIANO_KEYSTATE keyState[88];
}PX_Object_Piano;

typedef struct 
{
	PX_Object* scrollArea;

	PX_Object* label_eq1, * sliderbar_eq1;//0-1000
	PX_Object* label_eq2, * sliderbar_eq2;//0-1000
	PX_Object* label_eq3, * sliderbar_eq3;//0-1000
	PX_Object* label_c1, * sliderbar_c1;//0-1000
	PX_Object* label_c3, * sliderbar_c3;//0-1000

	PX_Object* label_f,*sliderbar_f;//10-8000 Frequency.
	PX_Object* label_weight,*sliderbar_weight;//0-10; Key pulsation velocity weight. Between 0 and 10.
	PX_Object* label_minr,*sliderbar_minr;//0.35 minimum string radio
	PX_Object* label_maxr, *sliderbar_maxr;//2 maximum string radio
	PX_Object* label_amprl, *sliderbar_amprl;//4 radius left sigmoidal shape.
	PX_Object* label_amprr, *sliderbar_amprr;//8 radius right sigmoidal shape.
	PX_Object* label_mult_radius_core_string, * sliderbar_mult_radius_core_string;//1 cord nucleus radio.
	PX_Object* label_minL, *sliderbar_minL;//0.07 minimum string length.
	PX_Object* label_maxL, *sliderbar_maxL;//1.4 maximum string length.
	PX_Object* label_ampLl, *sliderbar_ampLl;//-4 length left sigmoidal shape.
	PX_Object* label_ampLr, *sliderbar_ampLr;//4  length right sigmoidal shape.
	PX_Object* label_mult_density_string, *sliderbar_mult_density_string;//1 string density.
	PX_Object* label_mult_modulus_string, *sliderbar_mult_modulus_string;//1 young modulus.
	PX_Object* label_mult_impedance_bridge, *sliderbar_mult_impedance_bridge;//1 bridge impedance.
	PX_Object* label_mult_impedance_hammer, *sliderbar_mult_impedance_hammer;//0 hammer impedance.
	PX_Object* label_mult_mass_hammer, *sliderbar_mult_mass_hammer;//1 mass hammer.
	PX_Object* label_mult_force_hammer, *sliderbar_mult_force_hammer;//0.2 	force hammer.
	PX_Object* label_mult_hysteresis_hammer, *sliderbar_mult_hysteresis_hammer;//1 hysteresys hammer.
	PX_Object* label_mult_stiffness_exponent_hammer, *sliderbar_mult_stiffness_exponent_hammer;//1  stiffness_exponent_hammer.
	PX_Object* label_position_hammer, * sliderbar_position_hammer;//0.142 hammer position. (0 to 1)
	PX_Object* label_mult_loss_filter, * sliderbar_mult_loss_filter;//1 string loss factor.
	PX_Object* label_detune, * sliderbar_detune;//0.0003  detuning among 3 strings.
	PX_Object* label_hammer_type, * selectbox_hammer_type;//0 or 1

	PX_PianoKey_Parameters key_param;
	PX_PianoSoundboard_Parameters soundboard_param;
	PX_Piano *pPiano;
}PX_Object_PianoTune;
PX_Object* PX_Object_PianoCreate(px_memorypool* mp, PX_Object* Parent, px_float x, px_float y, px_float width, px_float height);
px_void PX_Object_PianoSetKeyFocus(PX_Object* pObject, px_int index);
px_void PX_Object_PianoSetKeyDown(PX_Object* pObject, px_int index, px_bool bdown);
px_void PX_Object_PianoClearKeyState(PX_Object* pObject);
PX_Object* PX_Object_PianoTuneCreate(px_memorypool* mp, PX_Object* Parent, px_float x, px_float y, px_float width, px_float height);
px_void PX_Object_PianoTuneSetParameters(PX_Object* pObject, PX_Piano* pPiano, px_int keyIndex);
#endif