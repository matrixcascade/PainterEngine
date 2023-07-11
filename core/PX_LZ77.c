#include "PX_LZ77.h"

px_bool PX_LZ77Deflate(px_byte* pbuffer, px_int size, px_memory* out, px_int backward_watch_distance)
{
	px_int cursor = 0;
	if (backward_watch_distance <= 0)
	{
		PX_ASSERT();
		return PX_FALSE;
	}

	while (cursor < size)
	{
		px_int distance = 0;
		px_int length = 0;
		px_int i;

		for (i = 1; i <= backward_watch_distance; i++) // i as compare distance
		{
			px_int backward_cursor = cursor - i;
			px_int forward_cursor = cursor;
			px_int j;

			if (backward_cursor < 0) break;
			if (forward_cursor >= size) break;

			distance = i;
			length = 0;
			for (j = 0; j < 32768; j++)
			{
				if (forward_cursor + j >= size)
					break;

				if (length>=256)
				{
					break;
				}

				if (pbuffer[backward_cursor + (j % i)] == pbuffer[forward_cursor + j])
				{
					length++;
				}
				else
				{
					break;
				}
			}
			if (length >= 3)
				break;
		}

		if (length >= 3)//encode while lg than 3 bytes
		{
			px_word length_table[29] = { 3, 4, 5, 6, 7, 8, 9, 10, 11, 13, 15, 17, 19, 23, 27, 31, 35, 43, 51, 59,67, 83, 99, 115, 131, 163, 195, 227, 258 };
			px_word index = 0;
			px_word data = 0;
			for (index = 0; index < 28; index++)
			{
				if (length < length_table[index + 1])
				{
					break;
				}
			}
			data = 257 + index;
			if (!PX_MemoryCat(out, &data, 2))return PX_FALSE;
			data = length;
			if (!PX_MemoryCat(out, &data, 2))return PX_FALSE;
			data = distance;
			if (!PX_MemoryCat(out, &data, 2))return PX_FALSE;
			cursor += length;
		}
		else
		{
			px_word data = pbuffer[cursor];
			if (!PX_MemoryCat(out, &data, 2))return PX_FALSE;
			cursor++;
		}

	}


	return PX_TRUE;
}

px_bool PX_LZ77Inflate(px_word* pbuffer, px_int size, px_memory* out)
{
	px_int i;
	for ( i = 0; i < size; i++)
	{
		if (pbuffer[i]<256)
		{
			px_byte symbol = (px_byte)pbuffer[i];
			if (!PX_MemoryCat(out, &symbol, 1))
				return PX_FALSE;
		}
		else if (pbuffer[i] == 256)
		{
			continue;
		}
		else
		{
			px_word length = pbuffer[i + 1];
			px_word distance = pbuffer[i + 2];
			px_int cursor = out->usedsize - distance;
			px_int j;
			if (cursor<0||distance==0||length==0)
			{
				return PX_FALSE;
			}

			for ( j = 0; j < length; j++)
			{
				px_byte symbol = ((px_byte *)out->buffer)[cursor+(j%distance)];
				if (!PX_MemoryCat(out, &symbol, 1))
					return PX_FALSE;
			}
			i += 2;
		}
	}
	return PX_TRUE;
}