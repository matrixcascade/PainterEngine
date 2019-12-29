#ifndef PX_WAVE_H
#define PX_WAVE_H
#include "PX_Typedef.h"

typedef struct
{
	px_byte szRiffId[4];
	px_dword dwRiffSize;
	px_byte szRiffFormat[4];
}PX_WAVE_RIFF_HEADER;

typedef struct
{
	px_word wFormatTag;
	px_word wChannels;
	px_dword dwSamplesPerSec;
	px_dword dwAvgBytesPerSec;
	px_word wBlockAlign;
	px_word wBitsPerSample;
}PX_WAVE_FORMAT;

typedef struct 
{
	px_byte szFmtID[4];   // 'f','m','t',' ' please note the
	// space character at the fourth location.
	px_dword dwFmtSize;
	PX_WAVE_FORMAT wavFormat;
}PX_WAVE_FMT_BLOCK;


typedef struct 
{
	px_byte szDataID[4];
	px_dword dwDataSize;
}PX_WAVE_DATA_BLOCK;

px_uint PX_WaveGetChannel(px_byte *buffer,px_int size);
px_bool PX_WaveVerify(px_byte *buffer,px_int size);
px_uint PX_WaveGetPCMSize(px_byte *buffer,px_int size);
px_byte* PX_WaveGetPCMDataPtr(px_int index,px_byte *buffer,px_int size);

#endif


