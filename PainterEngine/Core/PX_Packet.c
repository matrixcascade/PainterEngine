#include "PX_Packet.h"


px_bool PX_PacketCompress(px_byte *_in,px_uint input_size,px_byte *_out,px_uint *out_size,px_dword algorithm)
{
	PX_Packet_Header *pHeader=(PX_Packet_Header *)_out;
	px_byte *pw=PX_NULL;

	if (!_out)
	{
		switch (algorithm)
		{
		case PX_PACKET_ALOGRITHM_HUFFMAN:
			{
				PX_huffmanCompress(_in,input_size,_out,out_size);
				*out_size+=sizeof(PX_Packet_Header);
			}
			break;
		case PX_PACKET_ALOGRITHM_ARLE:
			{
				PX_ArleCompress(_in,input_size,_out,out_size);
				*out_size+=sizeof(PX_Packet_Header);
			}
			break;
		}
		return PX_TRUE;
	}

	pHeader->algorithm=algorithm;
	PX_memcpy(pHeader->magic,"PACK",4);
	pHeader->CRC=PX_crc32(_in,input_size);

	switch (algorithm)
	{
	case PX_PACKET_ALOGRITHM_HUFFMAN:
		{
			PX_huffmanCompress(_in,input_size,_out+sizeof(PX_Packet_Header),out_size);
			*out_size+=sizeof(PX_Packet_Header);
		}
		break;
	case PX_PACKET_ALOGRITHM_ARLE:
		{
			PX_ArleCompress(_in,input_size,_out+sizeof(PX_Packet_Header),out_size);
			*out_size+=sizeof(PX_Packet_Header);
		}
		break;
	}
	return PX_TRUE;
}

px_bool PX_PacketDecompress(px_byte *_in,px_uint input_size,px_byte *_out,px_uint *out_size)
{
	PX_Packet_Header *pHeader=(PX_Packet_Header *)_in;
	px_byte *pw=PX_NULL;

	if (!_out)
	{
		switch (pHeader->algorithm)
		{
		case PX_PACKET_ALOGRITHM_HUFFMAN:
			{
				PX_huffmanDecompress(_in+sizeof(PX_Packet_Header),input_size-sizeof(PX_Packet_Header),_out,out_size);
			}
			break;
		case PX_PACKET_ALOGRITHM_ARLE:
			{
				PX_ArleDecompress(_in+sizeof(PX_Packet_Header),input_size-sizeof(PX_Packet_Header),_out,out_size);
			}
			break;
		}
		return PX_TRUE;
	}


	switch (pHeader->algorithm)
	{
	case PX_PACKET_ALOGRITHM_HUFFMAN:
		{
			PX_huffmanDecompress(_in+sizeof(PX_Packet_Header),input_size-sizeof(PX_Packet_Header),_out,out_size);
			if (PX_crc32(_out,*out_size)!=pHeader->CRC)
			{
				return PX_FALSE;
			}
			return PX_TRUE;
		}
		break;
	case PX_PACKET_ALOGRITHM_ARLE:
		{
			PX_ArleDecompress(_in+sizeof(PX_Packet_Header),input_size-sizeof(PX_Packet_Header),_out,out_size);
			if (PX_crc32(_out,*out_size)!=pHeader->CRC)
			{
				return PX_FALSE;
			}
			return PX_TRUE;
		}
		break;
	}
	return PX_FALSE;
}

