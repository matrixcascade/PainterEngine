#ifndef PX_ARLE_H
#define PX_ARLE_H
#include "PX_Typedef.h"

px_void PX_ArleCompress(px_byte *_in,px_uint input_size,px_byte *_out,px_uint *_outsize);
px_void PX_ArleDecompress(px_byte *_in,px_uint input_size,px_byte *_out,px_uint *_outsize);
#endif
