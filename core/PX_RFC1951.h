#ifndef PX_RFC1951_H
#define PX_RFC1951_H
#include "PX_Huffman.h"
#include "PX_LZ77.h"

px_bool PX_RFC1951Inflate(px_byte* _in, px_uint input_size, px_memory* _out);
px_bool PX_RFC1951Deflate(px_byte* _in, px_uint input_size, px_memory* _out);
#endif // !PX_RFC1951_H
