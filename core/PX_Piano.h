#ifndef PX_PIANO_H
#define PX_PIANO_H

#include "PX_LTI.h"
#include "PX_Lagrange.h"

typedef struct {
	px_float z;
	px_float load;
	px_float a[2];
}PX_Piano_dwg_node;


typedef struct {
	px_memorypool* mp;
	px_int di;
	px_int d1;
	px_int size;
	px_int mask;
	px_int cursor;
	px_float* x;
}PX_Piano_Delay;

struct __PX_Piano_dwgs;
typedef struct {
	px_memorypool* mp;
	px_int del1;
	px_int del2;
	px_int nl;
	px_int nr;
	px_int pl[2];
	px_int pr[2];
	PX_Piano_dwg_node* cl[2];
	PX_Piano_dwg_node* cr[2];
	PX_Piano_dwg_node* l, * r;
	px_float loadl, loadr;
	px_float al[2];
	px_float ar[2];
	px_float alphalthis;
	px_float alpharthis;
	px_float alphal[2];
	px_float alphar[2];

	PX_Piano_Delay d[2];
	struct __PX_Piano_dwgs* parent;
	px_int commute;
}PX_Piano_dwg;

typedef struct __PX_Piano_dwgs {
	PX_Thirian dispersion[4];
	PX_FilterC1C3 lowpass;
	PX_Lagrange fracdelay3;
	px_int M;
	PX_Piano_dwg d[4];
}PX_Piano_dwgs;

typedef struct
{
	px_float a;
	px_float mi;
	px_float K;
	px_float p;
	px_float Fs;
	px_float F;
	px_float Z2i;
	px_float upprev;
	px_float alpha;
	px_float x;
	px_float v;
	px_int S;
	px_float dt;
	px_float dti;

}PX_Piano_StulovHammer;

typedef struct {
	px_float lastout;
	px_float iFs;
}PX_Piano_Integrator;

typedef struct {
	px_float lastin;
}PX_Piano_UnitDelay;

typedef struct
{
	px_float a;
	px_float mi;
	px_float K;
	px_float p;
	px_float Fs;
	px_float F;
	px_float Z2i;
	px_float vh;
	px_float oldvin;
	PX_Piano_Integrator intv;
	PX_Piano_Integrator intvh;
	PX_Piano_UnitDelay undel;
}PX_Piano_BanksHammer;

typedef struct 
{
	px_float f;
	px_float Fs;
	px_float weight;
	px_float minr;
	px_float maxr;
	px_float amprl;
	px_float amprr;
	px_float mult_radius_core_string;
	px_float minL;
	px_float maxL;
	px_float ampLl;
	px_float ampLr;
	px_float mult_density_string;
	px_float mult_modulus_string;
	px_float mult_impedance_bridge;
	px_float mult_impedance_hammer;
	px_float mult_mass_hammer;
	px_float mult_force_hammer;
	px_float mult_hysteresis_hammer;
	px_float mult_stiffness_exponent_hammer;
	px_float position_hammer;
	px_float mult_loss_filter;
	px_float detune;
	px_int hammer_type;
}PX_PianoKey_Parameters;

typedef struct 
{
	px_float eq1, eq2, eq3;
	px_float c1, c3;
}PX_PianoSoundboard_Parameters;


typedef struct {
	px_float Z;
	px_float Zb;
	px_float Zh;

	PX_PianoKey_Parameters param;


	px_int nstrings;
	PX_Piano_dwgs string[3];
	px_int nSampleCount;
	px_void* hammer;
	union
	{
		PX_Piano_StulovHammer StulovHammer;
		PX_Piano_BanksHammer BanksHammer;
	};

}PX_PianoKey;

typedef struct {
	px_memorypool* mp;
	PX_FilterC1C3 decay[8];
	PX_CircularBuffer delay[8];
	px_float mix;
	px_float A[8][8];
	px_float o[8];
	px_float b[8];
	px_float c[8];
}PX_Piano_DWGReverb;

typedef struct
{
	PX_PianoSoundboard_Parameters param;
	PX_Piano_DWGReverb soundboard;
	PX_Biquad shaping1;
	PX_Biquad shaping2;
	PX_Biquad shaping3;
}PX_PianoSoundBoard;

typedef struct 
{
	PX_PianoKey keys[88];
	PX_PianoSoundBoard soundboard;
}PX_Piano;

typedef enum 
{
	PX_PIANO_STYLE_DEFAULT,
	PX_PIANO_STYLE_DEBUG
}PX_PIANO_STYLE;

px_bool PX_PianoKeyInitialize(px_memorypool* mp, PX_PianoKey* pPianoKey, PX_PianoKey_Parameters* param);
px_void PX_PianoKeyTrigger(PX_PianoKey* pPianoKey, px_float v);
px_int PX_PianoKeyGo(PX_PianoKey* pPianoKey, px_float* out, px_int samples);
px_void PX_PianoKeyFree(PX_PianoKey* pPianoKey);
px_int PX_PianoKeyNameToIndex(const px_char keyName[]);
px_void PX_PianoIndexToKey(px_int index, px_char keyName[]);

px_bool PX_PianoSoundBoardInitialize(px_memorypool* mp, PX_PianoSoundBoard* psb, PX_PianoSoundboard_Parameters *param);
px_void PX_PianoSoundBoardGo(PX_PianoSoundBoard* psb, px_float in[], px_float out[], px_int count);
px_void PX_PianoSoundBoardFree(PX_PianoSoundBoard* psb);


px_bool PX_PianoInitialize(px_memorypool* mp,PX_Piano* pPiano, PX_PIANO_STYLE style);
px_bool PX_PianoInitializeEx(px_memorypool* mp, PX_Piano* pPiano, PX_PianoKey_Parameters keyparam[88], PX_PianoSoundboard_Parameters *soundboardparam);
px_void PX_PianoIndexToKey(px_int index, px_char keyName[]);
px_void PX_PianoTriggerKey(PX_Piano* pPiano, const px_char keyName[], px_float v);
px_void PX_PianoTriggerIndex(PX_Piano* pPiano, const px_int index, px_float v);
px_void PX_PianoGo(PX_Piano* pPiano, px_float* out, px_int samples);
px_void PX_PianoFree(PX_Piano* pPiano);


#define PX_PIANOREVERB_MAX_PULSATION 64

typedef struct
{
	px_float pcm[44100 * 3];
}PX_PianoSoundNote;

typedef struct  
{
	px_int cursor;
	px_int note;
}PX_PianoModelPulsation;

typedef struct 
{
	volatile px_bool lock;
	PX_PianoModelPulsation pulsation[PX_PIANOREVERB_MAX_PULSATION];
	PX_PianoSoundNote note[88];
}PX_PianoModel;
px_void PX_PianoModelInitializeData(PX_PianoModel* pModel);
px_bool PX_PianoModelInitializeNote(PX_PianoModel* pModel, px_int i, PX_PianoKey_Parameters* keyparam, PX_PianoSoundboard_Parameters* soundboardparam);
px_bool PX_PianoModelInitialize(PX_PianoModel* pModel, PX_PianoKey_Parameters keyparam[88], PX_PianoSoundboard_Parameters* soundboardparam);
px_void PX_PianoModelGo(PX_PianoModel* pModel, px_float out[], px_int count);
px_void PX_PianoModelTrigger(PX_PianoModel* pModel,px_int index);
px_void PX_PianoModelFree(PX_PianoModel* pModel);
#endif
