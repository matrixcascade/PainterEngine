#ifndef PX_PACKET_H
#define PX_PACKET_H

#include "PX_Arle.h"
#include "PX_Huffman.h"

#define PX_PACKET_ALOGRITHM_HUFFMAN 0
#define PX_PACKET_ALOGRITHM_ARLE 1


typedef struct
{
	px_char		magic[4];//PACK
	px_dword	algorithm;
	px_dword    CRC;
}PX_Packet_Header;

px_bool PX_PacketCompress(px_byte *_in,px_uint input_size,px_byte *_out,px_uint *out_size,px_dword algotithm);
px_bool PX_PacketDecompress(px_byte *_in,px_uint input_size,px_byte *_out,px_uint *out_size);
#endif
