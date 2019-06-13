#ifndef PX_HUFFMAN_H
#define PX_HUFFMAN_H
#include "PX_Quicksort.h"

px_void PX_huffmanCompress(px_byte *_in,px_uint input_size,px_byte *_out,px_uint *out_size);
px_void PX_huffmanDecompress(px_byte *_in,px_uint input_size,px_byte *_out,px_uint *out_size);
#endif
