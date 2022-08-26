#ifndef PX_LZ77_H
#define PX_LZ77_H
#include "PX_Memory.h"


px_bool PX_LZ77Deflate(px_byte* pbuffer, px_int size, px_memory* out,px_int watch);
px_bool PX_LZ77Inflate(px_word* pbuffer, px_int size, px_memory* out);

#endif