#ifndef PX_ARLE_H
#define PX_ARLE_H
#include "PX_Memory.h"

px_bool PX_ArleCompress(px_byte *_in,px_int input_size,px_memory *out);
px_bool PX_ArleDecompress(px_byte *_in,px_int input_size, px_memory* out);
#endif
