#include "PX_Arle.h"

px_bool PX_ArleCompress(px_byte *_in,px_int input_size, px_memory* out)
{
	px_int read_offset = 0;
	px_byte last_char=0;

	while (read_offset< input_size)
	{
		if (input_size - read_offset>1)
		{
			px_int len = 2;
			last_char = _in[read_offset+1];
			if (_in[read_offset]== _in[read_offset+1])
			{
				read_offset += 2;
				while (PX_TRUE)
				{
					if (read_offset>= input_size|| _in[read_offset]!= last_char || len == 127)
					{
						px_byte data[2];
						data[0] = 0x80 | len;
						data[1] = last_char;
						PX_MemoryCat(out, data, 2);
						break;
					}
					read_offset++;
					len++;
				}
			}
			else
			{
				px_byte data[127];
				data[0] = _in[read_offset];
				data[1] = _in[read_offset + 1];
				read_offset += 2;
				while(PX_TRUE)
				{
					if (read_offset + len >= input_size || _in[read_offset] == last_char||len==127)
					{
						px_byte opcode;
						opcode = len;
						PX_MemoryCatByte(out, opcode);
						PX_MemoryCat(out, data, len);
						break;
					}
					else
					{
						data[len] = _in[read_offset];
						read_offset++;
						len++;
						last_char = _in[read_offset];
					}
				}
			}

		}
		else if(input_size - read_offset == 1)
		{
			px_byte data[2];
			data[0] = 0x01;
			data[1] = _in[read_offset];
			PX_MemoryCat(out, data, 2);
		}
		else
		{
			return PX_TRUE;
		}
		
	}
	
	return PX_TRUE;
}

px_bool PX_ArleDecompress(px_byte *_in,px_int input_size, px_memory* out)
{
	px_byte opcode;
	px_int read_offset = 0;

	while (read_offset< input_size)
	{
		px_bool thesame;
		px_int size;
		opcode = _in[read_offset];
		thesame = opcode & 0x80;
		size = opcode & 0x7f;
		read_offset++;
		if (read_offset>= input_size)
		{
			return PX_FALSE;
		}
		if (thesame)
		{
			px_byte data = _in[read_offset];
			if (!PX_MemoryCatRepeatByte(out, data, size))
				return PX_FALSE;

			read_offset++;
		}
		else
		{
			if (read_offset+size> input_size)
			{
				return PX_FALSE;
			}
			if (!PX_MemoryCat(out, _in + read_offset, size))
				return PX_FALSE;
			read_offset += size;
		}
	}
	return PX_TRUE;
}

