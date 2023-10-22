#ifndef PX_MP3_H
#define PX_MP3_H

#include "PX_Memory.h"

#define PX_MP3_MAX_SAMPLES_PER_FRAME (1152)

typedef struct
{
	px_float mdct_overlap[2][9 * 32], qmf_state[15 * 2 * 32];
	px_int reserv, free_format_bytes;
	px_byte header[4], reserv_buf[511];
}PX_MP3Decoder_t;

typedef struct
{
	const px_byte* data;
	px_int size;
	px_int offset;
	px_int samplerate;
	px_int channels;
	px_int bit_rate_kbps;
	px_double fifo[PX_MP3_MAX_SAMPLES_PER_FRAME * 2];//1 frames,2 channels
	px_int fifo_count;
	PX_SoundResampler resampler1, resampler2;
	PX_MP3Decoder_t decoder;
}PX_MP3Decoder;

px_bool PX_MP3DecoderInitialize(PX_MP3Decoder* mp3, const px_byte* data, px_int size);
px_int PX_MP3DecoderRead_44100HZ_2Channel(PX_MP3Decoder* mp3, px_double samples[], px_int count);


#endif // !PX_MP3_H
