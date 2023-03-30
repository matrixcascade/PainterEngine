#include "PX_RFC1951.h"

static px_bool PX_RFC1951_InflateUncompressData(const px_byte* in, px_uint insize, px_uint32* pbit_position, px_memory* out)
{
	px_uint p;
	px_int len, nlen;

	while (((*pbit_position) & 0x7))(*pbit_position)++;

	p = (*pbit_position) / 8;

	if (p >= insize - 4)
		return PX_FALSE;

	len = in[p] +  (in[p + 1]<<8);
	p += 2;
	nlen = in[p] + (in[p + 1]<<8);
	p += 2;

	if (len + nlen != 65535)
		return PX_FALSE;

	if (p + len > insize)
		return PX_FALSE;

	if(!PX_MemoryCat(out, &in[p], len))
		return PX_FALSE;

	p += len;
	(*pbit_position) = p * 8;
	return PX_TRUE;
}

static px_bool PX_RFC1951_DeflateUncompressData(const px_byte* in, px_word insize, px_memory* out)
{
	px_word len, nlen;
	px_byte b;
	len = insize;
	nlen = 65535 - len;

	b = (px_byte)(len);
	if (!PX_MemoryCat(out, &b, 1))return PX_FALSE;
	b = (px_byte)(len >> 8);
	if (!PX_MemoryCat(out, &b, 1))return PX_FALSE;

	b = (px_byte)(nlen );
	if (!PX_MemoryCat(out, &b, 1))return PX_FALSE;
	b = (px_byte)(nlen >> 8);
	if (!PX_MemoryCat(out, &b, 1))return PX_FALSE;

	if (!PX_MemoryCat(out, in, len))return PX_FALSE;
	return PX_TRUE;
}

px_bool PX_RFC1951InflateBlock(px_byte* _in, px_uint input_size, px_uint32 *pbit_position, px_uint32 type, px_memory* _out)
{
	switch (type)
	{
		//uncompress data
	case 0:
	{
		if (!PX_RFC1951_InflateUncompressData(_in, input_size, pbit_position, _out))
			return PX_FALSE;
	}
	break;
	case 1:
	case 2:
	{

		px_memory mem;
		PX_MemoryInitialize(_out->mp, &mem);
		if (!PX_HuffmanInflateCodeData(_in, pbit_position, input_size, &mem, (PX_HUFFMAN_TREE_TYPE)type))
		{
			PX_MemoryFree(&mem);
			return PX_FALSE;
		}
		if (mem.usedsize & 1)
		{
			PX_MemoryFree(&mem);
			return PX_FALSE;
		}

		if (!PX_LZ77Inflate((px_word*)mem.buffer, mem.usedsize / 2, _out))
		{
			PX_MemoryFree(&mem);
			return PX_FALSE;
		}

		PX_MemoryFree(&mem);
	}
	break;
	default:
		return PX_FALSE;
	}
	return PX_TRUE;
}



px_bool PX_RFC1951Inflate(px_byte* _in, px_uint input_size, px_memory* _out)
{
	px_uint32 bit_position = 0;
	px_bool isLastBlock = 0;

	do
	{
		px_uint32 type;
		px_int sum = 0;
		//out of range
		if (bit_position / 8 > input_size)
		{
			return PX_FALSE;
		}

		isLastBlock = PX_ReadBitsLE(&bit_position, _in, 1);
		type = PX_ReadBitsLE(&bit_position, _in, 2);
		
		if (!PX_RFC1951InflateBlock(_in, input_size, &bit_position, type, _out))
			return PX_FALSE;

	} while (!isLastBlock);

	return PX_TRUE;
}



px_bool PX_RFC1951DeflateBlock(px_byte* _in, px_uint input_size,px_bool isLastBlock, px_memory* _out)
{
	px_int raw_size, dynamic_size, fixed_size;
	px_memory mem,test_mem;

	if (input_size == 0) return PX_FALSE;
	PX_MemoryInitialize(_out->mp, &mem);
	PX_MemoryInitialize(_out->mp, &test_mem);

	if (!PX_LZ77Deflate(_in, input_size, &mem, 128))
	{
		PX_MemoryFree(&mem);
		return PX_FALSE;
	}
	do 
	{
		px_word end = 256;
		if (!PX_MemoryCat(&mem, &end, 2))
		{
			PX_MemoryFree(&mem);
			return PX_FALSE;
		}
	} while (0);

	raw_size = input_size + 4;
	
	if (!PX_HuffmanDeflateCodeData((px_word*)mem.buffer, mem.usedsize / 2, &test_mem, PX_HUFFMAN_TREE_TYPE_DYNAMIC))
	{
		PX_MemoryClear(&test_mem);
		dynamic_size = 0x7fffffff;
	}
	else
	{
		dynamic_size = test_mem.usedsize + 1;
		PX_MemoryClear(&test_mem);
	}
	

	if (!PX_HuffmanDeflateCodeData((px_word*)mem.buffer, mem.usedsize / 2, &test_mem, PX_HUFFMAN_TREE_TYPE_FIXED))
	{
		PX_MemoryFree(&test_mem);
		PX_MemoryFree(&mem);
		return PX_FALSE;
	}
	fixed_size = test_mem.usedsize+1;
	PX_MemoryClear(&test_mem);

	PX_MemoryFree(&test_mem);
	

 	if (raw_size<= dynamic_size&&raw_size<= fixed_size)
 	{
 
 		px_byte bheader=0;
 		if (!PX_MemoryCatBit(_out, isLastBlock))return PX_FALSE;
 		if (!PX_MemoryCatBits(_out, &bheader,2))return PX_FALSE;
 
 		if (!PX_RFC1951_DeflateUncompressData(_in, input_size, _out))
 		{
 			PX_MemoryFree(&mem);
 			return PX_FALSE;
 		}
 	}
 	else if (fixed_size <= raw_size && fixed_size <= dynamic_size)
 	{
 
 		px_byte bheader = 1;
 		if (!PX_MemoryCatBit(_out, isLastBlock))return PX_FALSE;
 		if (!PX_MemoryCatBits(_out, &bheader, 2)) return PX_FALSE;
 
 		if (!PX_HuffmanDeflateCodeData((px_word*)mem.buffer, mem.usedsize / 2, _out, PX_HUFFMAN_TREE_TYPE_FIXED))
 		{
 			PX_MemoryFree(&mem);
 			return PX_FALSE;
 		}
 	}
 	else
	{
		px_byte bheader = 2;
		if (!PX_MemoryCatBit(_out, isLastBlock))return PX_FALSE;
		if (!PX_MemoryCatBits(_out, &bheader, 2))return PX_FALSE;

		if (!PX_HuffmanDeflateCodeData((px_word*)mem.buffer, mem.usedsize / 2, _out, PX_HUFFMAN_TREE_TYPE_DYNAMIC))
		{
			PX_MemoryFree(&mem);
			return PX_FALSE;
		}
	}
	PX_MemoryFree(&mem);
	return PX_TRUE;
}

#define PX_RFC1951_MAX_BLOCK 32768

px_bool PX_RFC1951Deflate(px_byte* _in, px_uint input_size, px_memory* _out)
{
	px_int block = input_size / PX_RFC1951_MAX_BLOCK;
	px_int remain = input_size % PX_RFC1951_MAX_BLOCK;
	px_int i;
	for (i=0;i< block;i++)
	{
		px_bool last_block = 0;
		if (remain==0&&i==block-1)
		{
			last_block = 1;
		}
		if (!PX_RFC1951DeflateBlock(_in+i* PX_RFC1951_MAX_BLOCK, PX_RFC1951_MAX_BLOCK, last_block, _out))
			return PX_FALSE;
	}
	if (remain)
	{
		if (!PX_RFC1951DeflateBlock(_in + i * PX_RFC1951_MAX_BLOCK, remain,PX_TRUE, _out))
			return PX_FALSE;

	}
	return PX_TRUE;

}