#include "PX_Png.h"


#define PX_PNG_MAKE_DWORD(a,b,c,d) (((a) << 24) | ((b) << 16) | ((c) << 8) | (d))
#define PX_PNG_MAKE_DWORD_PTR(p) PX_PNG_MAKE_DWORD((p)[0], (p)[1], (p)[2], (p)[3])

#define PX_PNG_CHUNK_IHDR PX_PNG_MAKE_DWORD('I','H','D','R')
#define PX_PNG_CHUNK_IDAT PX_PNG_MAKE_DWORD('I','D','A','T')
#define PX_PNG_CHUNK_IEND PX_PNG_MAKE_DWORD('I','E','N','D')


PX_PNG_FORMAT PX_PngGetFormat(px_int color_depth, PX_PNG_COLOR_TYPE color_type)
{
	switch (color_type) 
	{
	case PX_PNG_COLOR_TYPE_LUM:
		switch (color_depth) {
		case 1:
			return PX_PNG_FORMAT_LUMINANCE1;
		case 2:
			return PX_PNG_FORMAT_LUMINANCE2;
		case 4:
			return PX_PNG_FORMAT_LUMINANCE4;
		case 8:
			return PX_PNG_FORMAT_LUMINANCE8;
		default:
			return PX_PNG_FORMAT_ERROR;
		}
	case PX_PNG_COLOR_TYPE_RGB:
		switch (color_depth) {
		case 8:
			return PX_PNG_FORMAT_R8G8B8;
		case 16:
			return PX_PNG_FORMAT_R16G16B16;
		default:
			return PX_PNG_FORMAT_ERROR;
		}
	case PX_PNG_COLOR_TYPE_LUMA:
		switch (color_depth) {
		case 1:
			return PX_PNG_FORMAT_LUMINANCE_ALPHA1;
		case 2:
			return PX_PNG_FORMAT_LUMINANCE_ALPHA2;
		case 4:
			return PX_PNG_FORMAT_LUMINANCE_ALPHA4;
		case 8:
			return PX_PNG_FORMAT_LUMINANCE_ALPHA8;
		default:
			return PX_PNG_FORMAT_ERROR;
		}
	case PX_PNG_COLOR_TYPE_RGBA:
		switch (color_depth) {
		case 8:
			return PX_PNG_FORMAT_R8G8B8A8;
		case 16:
			return PX_PNG_FORMAT_R16G16B16A16;
		default:
			return PX_PNG_FORMAT_ERROR;
		}
	default:
		return PX_PNG_FORMAT_ERROR;
	}
}
px_int PX_PngGetFormatBits(PX_PNG_FORMAT fmt)
{
	switch (fmt)
	{
	case PX_PNG_FORMAT_LUMINANCE1:
		return 1;
	case	PX_PNG_FORMAT_LUMINANCE2:
		return 2;
	case	PX_PNG_FORMAT_LUMINANCE4:
		return 4;
	case	PX_PNG_FORMAT_LUMINANCE8:
		return 8;
	case	PX_PNG_FORMAT_LUMINANCE_ALPHA1:
		return 2;
	case	PX_PNG_FORMAT_LUMINANCE_ALPHA2:
		return 4;
	case	PX_PNG_FORMAT_LUMINANCE_ALPHA4:
		return 8;
	case	PX_PNG_FORMAT_LUMINANCE_ALPHA8:
		return 16;
	case	PX_PNG_FORMAT_R8G8B8:
		return 24;
	case	PX_PNG_FORMAT_R16G16B16:
		return 48;
	case	PX_PNG_FORMAT_R8G8B8A8:
		return 32;
	case	PX_PNG_FORMAT_R16G16B16A16:
		return 64;
	case	PX_PNG_FORMAT_ERROR:
		return 0;
	default:
		break;
	}
	return 0;
}
px_bool PX_PngVerify(px_byte* ppngbuffer, px_int size,px_int *width,px_int *height, PX_PNG_FORMAT *format)
{
	px_int  color_depth;
	PX_PNG_COLOR_TYPE color_type;
	PX_PNG_FORMAT fmt;
	
	do //check header
	{
		const px_byte magic_header[] = { 137,80,78,71,13,10,26,10 };
		if (size < 29)return PX_FALSE;
		if (!PX_memequ(ppngbuffer, magic_header, sizeof(magic_header)))return PX_FALSE;
		if (!PX_memequ(ppngbuffer + 12, "IHDR", 4))return PX_FALSE;
		if(width)
			*width = (ppngbuffer[16] << 24) + (ppngbuffer[17] << 16) + (ppngbuffer[18] << 8) + (ppngbuffer[19]);
		if(height)
			*height = (ppngbuffer[20] << 24) + (ppngbuffer[21] << 16) + (ppngbuffer[22] << 8) + (ppngbuffer[23]);
		color_depth = ppngbuffer[24];
		color_type = (PX_PNG_COLOR_TYPE)ppngbuffer[25];
		
			fmt = PX_PngGetFormat(color_depth, color_type);
		if (fmt == PX_PNG_FORMAT_ERROR)return PX_FALSE;
		if (format)
			*format = fmt;
		if (ppngbuffer[26] | ppngbuffer[27] | ppngbuffer[28])return PX_FALSE;
	} while (0);

	return PX_TRUE;
}




static px_int PX_png_paeth_predictor(px_int a, px_int b, px_int c)
{
	px_int p = a + b - c;
	px_int pa = p > a ? p - a : a - p;
	px_int pb = p > b ? p - b : b - p;
	px_int pc = p > c ? p - c : c - p;

	if (pa <= pb && pa <= pc)
		return a;
	else if (pb <= pc)
		return b;
	else
		return c;
}

static px_bool PX_png_unfilter_scanline( px_byte* recon, const px_byte* scanline, const px_byte* precon, px_uint bytewidth, px_byte filterType, px_uint length)
{
	px_uint i;
	switch (filterType) {
	case 0:
		for (i = 0; i < length; i++)
			recon[i] = scanline[i];
		break;
	case 1:
		for (i = 0; i < bytewidth; i++)
			recon[i] = scanline[i];
		for (i = bytewidth; i < length; i++)
			recon[i] = scanline[i] + recon[i - bytewidth];
		break;
	case 2:
		if (precon)
			for (i = 0; i < length; i++)
				recon[i] = scanline[i] + precon[i];
		else
			for (i = 0; i < length; i++)
				recon[i] = scanline[i];
		break;
	case 3:
		if (precon) {
			for (i = 0; i < bytewidth; i++)
				recon[i] = scanline[i] + precon[i] / 2;
			for (i = bytewidth; i < length; i++)
				recon[i] = scanline[i] + ((recon[i - bytewidth] + precon[i]) / 2);
		}
		else {
			for (i = 0; i < bytewidth; i++)
				recon[i] = scanline[i];
			for (i = bytewidth; i < length; i++)
				recon[i] = scanline[i] + recon[i - bytewidth] / 2;
		}
		break;
	case 4:
		if (precon) {
			for (i = 0; i < bytewidth; i++)
				recon[i] = (px_byte)(scanline[i] + PX_png_paeth_predictor(0, precon[i], 0));
			for (i = bytewidth; i < length; i++)
				recon[i] = (px_byte)(scanline[i] + PX_png_paeth_predictor(recon[i - bytewidth], precon[i], precon[i - bytewidth]));
		}
		else {
			for (i = 0; i < bytewidth; i++)
				recon[i] = scanline[i];
			for (i = bytewidth; i < length; i++)
				recon[i] = (px_byte)(scanline[i] + PX_png_paeth_predictor(recon[i - bytewidth], 0, 0));
		}
		break;
	default:
		return PX_FALSE;
	}
	return PX_TRUE;
}

static px_bool PX_png_unfilter( px_byte* out, const px_byte* in, px_uint w, px_uint h, px_uint bpp)
{
	
	px_uint y;
	px_byte* prevline = 0;

	px_uint bytewidth = (bpp + 7) / 8;	
	px_uint linebytes = (w * bpp + 7) / 8;

	for (y = 0; y < h; y++) {
		px_uint outindex = linebytes * y;
		px_uint inindex = (1 + linebytes) * y;	
		px_byte filterType = in[inindex];

		if (!PX_png_unfilter_scanline(&out[outindex], &in[inindex + 1], prevline, bytewidth, filterType, linebytes))
			return PX_FALSE;
		

		prevline = &out[outindex];
	}
	return PX_TRUE;
}

static px_void PX_png_remove_padding_bits(px_byte* out, const px_byte* in, px_uint olinebits, px_uint ilinebits, px_uint h)
{
	px_uint y;
	px_uint diff = ilinebits - olinebits;
	px_uint obp = 0, ibp = 0;	/*bit pointers */
	for (y = 0; y < h; y++) {
		px_uint x;
		for (x = 0; x < olinebits; x++) {
			px_byte bit = (px_byte)((in[(ibp) >> 3] >> (7 - ((ibp) & 0x7))) & 1);
			ibp++;

			if (bit == 0)
				out[(obp) >> 3] &= (px_byte)(~(1 << (7 - ((obp) & 0x7))));
			else
				out[(obp) >> 3] |= (1 << (7 - ((obp) & 0x7)));
			++obp;
		}
		ibp += diff;
	}
}


static px_bool PX_png_process_scanlines( px_byte* out, const px_byte* in,px_uint width,px_uint height,px_uint bpp)
{

	if (bpp == 0) {
		return PX_FALSE;
	}

	if (bpp < 8 && width * bpp != ((width * bpp + 7) / 8) * 8) 
	{
		if(!PX_png_unfilter(out, in, width, height, bpp))
			return PX_FALSE;

		PX_png_remove_padding_bits(out, in, width * bpp, ((width * bpp + 7) / 8) * 8, height);
	}
	else 
	{
		if (!PX_png_unfilter(out, in, width, height, bpp))
			return PX_FALSE;
	}
	return PX_TRUE;
}

static px_bool PX_PngChunkCRCVerify(const px_byte* pchunk)
{
	px_int chunk_length = PX_PNG_MAKE_DWORD_PTR(pchunk);
	px_dword crc=PX_crc32(pchunk+4, chunk_length + 4);
	const px_byte* pcrc = (pchunk + 8 + chunk_length);
	px_dword vcrc = (pcrc[0]<<24)+ (pcrc[1] << 16)+ (pcrc[2] << 8)+ (pcrc[3] << 0);
	return crc == vcrc;
}

static px_dword PX_PngChunkGetCRC(const px_byte* pchunk)
{
	px_int chunk_length = PX_PNG_MAKE_DWORD_PTR(pchunk);
	px_dword crc = PX_crc32(pchunk + 4, chunk_length + 4);
	return crc;
}

px_int PX_PngGetSize(px_byte* ppngbuffer,px_int in_size)
{
	px_int width, height;
	PX_PNG_FORMAT format;
	px_byte* pchunk;
	px_byte* compressed = 0;

	px_int compressed_size = 0;


	if (!PX_PngVerify(ppngbuffer, 29, &width, &height, &format))return PX_FALSE;

	do
	{
		px_int chunk_length;
		px_byte* pdata;
		pchunk = ppngbuffer + 33;
		while (pchunk < ppngbuffer + in_size)
		{
			if ((px_int)(pchunk - ppngbuffer + 12) > in_size)return PX_FALSE;
			if (!PX_PngChunkCRCVerify(pchunk)) return PX_FALSE;
			chunk_length = PX_PNG_MAKE_DWORD_PTR(pchunk);
			if ((px_int)(pchunk - ppngbuffer + 12 + chunk_length) > in_size) return PX_FALSE;
			pdata = pchunk + 8;

			switch (PX_PNG_MAKE_DWORD_PTR(pchunk + 4))
			{
			case PX_PNG_CHUNK_IDAT:
			{
				compressed_size += chunk_length;
			}
			break;
			case PX_PNG_CHUNK_IEND:
				break;
			default:
				if ((pchunk[4] & 32) != 0)
					break;
				return PX_FALSE;
			}
			

			if (PX_PNG_MAKE_DWORD_PTR(pchunk + 4) == PX_PNG_CHUNK_IEND)
			{

				pchunk += (chunk_length + 12);
				break;
			}

			pchunk += (chunk_length + 12);
		}
		return (px_int)(pchunk - ppngbuffer);
	} while (0);
}

px_bool PX_PngToXBuffer(px_memorypool * mp,px_byte* ppngbuffer, px_int size,px_surface *prendersurface,px_shape *pshape)
{
	px_int width, height;
	PX_PNG_FORMAT format;
	px_byte* pchunk;
	px_byte* compressed=0;
	px_byte* imagedata;
	px_int compressed_size=0, offset=0;
	px_memory inflate_data;
	px_int i,j;
	if (!PX_PngVerify(ppngbuffer, size, &width, &height, &format))return PX_FALSE;

	do
	{
		px_int chunk_length;
		px_byte* pdata;
		pchunk = ppngbuffer + 33;
		while (pchunk < ppngbuffer + size)
		{
			if ((px_int)(pchunk- ppngbuffer+12)> size)return PX_FALSE;
			if (!PX_PngChunkCRCVerify(pchunk)) return PX_FALSE;
			chunk_length = PX_PNG_MAKE_DWORD_PTR(pchunk);
			if ((px_int)(pchunk - ppngbuffer + 12 + chunk_length) > size) return PX_FALSE;
			pdata = pchunk + 8;

			switch (PX_PNG_MAKE_DWORD_PTR(pchunk+4))
			{
			case PX_PNG_CHUNK_IDAT:
			{
				compressed_size += chunk_length;
			}
			break;
			case PX_PNG_CHUNK_IEND:
				break;
			default:
				if((pchunk[4]&32)!=0)
					break;
				return PX_FALSE;
			}
			pchunk += (chunk_length + 12);
			if (PX_PNG_MAKE_DWORD_PTR(pchunk + 4) == PX_PNG_CHUNK_IEND)
			{
				break;
			}
		}
	} while (0);

	do 
	{
		px_int chunk_length;
		px_byte* pdata;
		compressed = (px_byte *)MP_Malloc(mp, compressed_size);
		if (!compressed)return PX_FALSE;
		pchunk = ppngbuffer + 33;
		while (pchunk < ppngbuffer + size)
		{
			if ((px_int)(pchunk - ppngbuffer + 12) > size)return PX_FALSE;
			chunk_length = PX_PNG_MAKE_DWORD_PTR(pchunk);
			if ((px_int)(pchunk - ppngbuffer + 12 + chunk_length) > size) return PX_FALSE;
			pdata = pchunk + 8;

			switch (PX_PNG_MAKE_DWORD_PTR(pchunk + 4))
			{
			case PX_PNG_CHUNK_IDAT:
			{
				PX_memcpy(compressed + offset, pdata, chunk_length);
				offset += chunk_length;
			}
			break;
			case PX_PNG_CHUNK_IEND:
				break;
			default:
				if ((pchunk[4] & 32) != 0)
					break;
				return PX_FALSE;
			}
			pchunk += (chunk_length + 12);
		}
	} while (0);

	do 
	{
		PX_MemoryInitialize(mp, &inflate_data);
		
		if ((compressed[0] * 256 + compressed[1]) % 31 != 0) 
		{ PX_MemoryFree(&inflate_data); if (compressed)MP_Free(mp, compressed); return PX_FALSE;}
		if ((compressed[0] & 15) != 8 || ((compressed[0] >> 4) & 15) > 7) 
		{ PX_MemoryFree(&inflate_data); if (compressed)MP_Free(mp, compressed); return PX_FALSE; }
		if (((compressed[1] >> 5) & 1) != 0) 
		{ PX_MemoryFree(&inflate_data); if (compressed)MP_Free(mp, compressed); return PX_FALSE; }

		if (!PX_RFC1951Inflate(compressed+2, compressed_size-2, &inflate_data))
		{
			PX_MemoryFree(&inflate_data);
			if (compressed)MP_Free(mp, compressed);
			return PX_FALSE;
		}
		
		MP_Free(mp, compressed);
		compressed = 0;
	} while (0);

	do
	{
		
		px_int imagesize = (height * width * PX_PngGetFormatBits(format) + 7) / 8;
		imagedata = (px_byte *)MP_Malloc(mp, imagesize);
		if (!imagedata)
		{
			PX_MemoryFree(&inflate_data);
			return PX_FALSE;
		}
		if (!PX_png_process_scanlines(imagedata, inflate_data.buffer, width, height, PX_PngGetFormatBits(format)))
		{
			PX_MemoryFree(&inflate_data);
			MP_Free(mp, imagedata);
			return PX_FALSE;
		}
		PX_MemoryFree(&inflate_data);
	} while (0);


	switch (format)
	{
	case PX_PNG_FORMAT_LUMINANCE1:
	{
		px_uint bp=0;
		for (j = 0; j < height; j++)
		{
			for (i = 0; i < width; i++)
			{
				px_byte a = PX_ReadBitsLE(&bp, imagedata, 1)?255:0;
				if(prendersurface)
					PX_SurfaceSetPixel(prendersurface, i, j, PX_COLOR(255, a, a, a));
				if (pshape)
					PX_ShapeSetPixel(pshape, i, j, a);
			}
		}
		MP_Free(mp, imagedata);
	}	
	return PX_TRUE;
	case	PX_PNG_FORMAT_LUMINANCE2:
	{
		px_uint bp = 0;
		for (j = 0; j < height; j++)
		{
			for (i = 0; i < width; i++)
			{
				px_byte a = (px_byte)(PX_ReadBitsLE(&bp, imagedata, 2) *(255/3));
				if (prendersurface)
					PX_SurfaceSetPixel(prendersurface, i, j, PX_COLOR(255, a, a, a));
				if (pshape)
					PX_ShapeSetPixel(pshape, i, j, a);
			}
		}
		MP_Free(mp, imagedata);
	}
	return PX_TRUE;
	case	PX_PNG_FORMAT_LUMINANCE4:
	{
		px_uint bp = 0;
		for (j = 0; j < height; j++)
		{
			for (i = 0; i < width; i++)
			{
				px_byte a = (px_byte)(PX_ReadBitsLE(&bp, imagedata, 4) * (255 / 15));
				if (prendersurface)
					PX_SurfaceSetPixel(prendersurface, i, j, PX_COLOR(255, a, a, a));
				if (pshape)
					PX_ShapeSetPixel(pshape, i, j, a);
			}
		}
		MP_Free(mp, imagedata);
	}
	return PX_TRUE;
	case	PX_PNG_FORMAT_LUMINANCE8:
	{
		
		for (j = 0; j < height; j++)
		{
			for (i = 0; i < width; i++)
			{
				px_byte a = imagedata[width*j+i];
				if (prendersurface)
					PX_SurfaceSetPixel(prendersurface, i, j, PX_COLOR(255, a, a, a));
				if (pshape)
					PX_ShapeSetPixel(pshape, i, j, a);
			}
		}
		MP_Free(mp, imagedata);
	}
	return PX_TRUE;
	case	PX_PNG_FORMAT_LUMINANCE_ALPHA1:
	{
		px_uint bp = 0;
		for (j = 0; j < height; j++)
		{
			for (i = 0; i < width; i++)
			{
				px_byte a = PX_ReadBitsLE(&bp, imagedata, 1) ? 255 : 0;
				if (prendersurface)
					PX_SurfaceSetPixel(prendersurface, i, j, PX_COLOR(a, 255, 255, 255));
				if (pshape)
					PX_ShapeSetPixel(pshape, i, j, a);
			}
		}
		MP_Free(mp, imagedata);
	}
	return PX_TRUE;
	case	PX_PNG_FORMAT_LUMINANCE_ALPHA2:
	{
		px_uint bp = 0;
		for (j = 0; j < height; j++)
		{
			for (i = 0; i < width; i++)
			{
				px_byte a = (px_byte)(PX_ReadBitsLE(&bp, imagedata, 2) * (255 / 3));
				if (prendersurface)
					PX_SurfaceSetPixel(prendersurface, i, j, PX_COLOR(a, 255, 255, 255));
				if (pshape)
					PX_ShapeSetPixel(pshape, i, j, a);
			}
		}
		MP_Free(mp, imagedata);
	}
	return PX_TRUE;

	case	PX_PNG_FORMAT_LUMINANCE_ALPHA4:
	{
		px_uint bp = 0;
		for (j = 0; j < height; j++)
		{
			for (i = 0; i < width; i++)
			{
				px_byte a = (px_byte)(PX_ReadBitsLE(&bp, imagedata, 2) * (255 / 15));
				if (prendersurface)
					PX_SurfaceSetPixel(prendersurface, i, j, PX_COLOR(a, 255, 255, 255));
				if (pshape)
					PX_ShapeSetPixel(pshape, i, j, a);
			}
		}
		MP_Free(mp, imagedata);
	}
	return PX_TRUE;
	case	PX_PNG_FORMAT_LUMINANCE_ALPHA8:
	{
		
		for (j = 0; j < height; j++)
		{
			for (i = 0; i < width; i++)
			{
				px_byte a = imagedata[width * j + i];
				if (prendersurface)
					PX_SurfaceSetPixel(prendersurface, i, j, PX_COLOR(a, 255, 255, 255));
				if (pshape)
					PX_ShapeSetPixel(pshape, i, j, a);
			}
		}
		MP_Free(mp, imagedata);
	}
	return PX_TRUE;
	case	PX_PNG_FORMAT_R8G8B8:
	{
		
		for (j = 0; j < height; j++)
		{
			for (i = 0; i < width; i++)
			{
				px_byte a =255;
				px_byte r = imagedata[(width * j + i)*3];
				px_byte g = imagedata[(width * j + i) * 3+1];
				px_byte b = imagedata[(width * j + i) * 3+2];
				
				if (prendersurface)
					PX_SurfaceSetPixel(prendersurface, i, j, PX_COLOR(a, r, g, b));
				if (pshape)
					PX_ShapeSetPixel(pshape, i, j, (px_uchar)(0.2126*r + 0.7152*g + 0.0722*b));
			}
		}
		MP_Free(mp, imagedata);
	}
	return PX_TRUE;
	case	PX_PNG_FORMAT_R16G16B16:
	{
		
		for (j = 0; j < height; j++)
		{
			for (i = 0; i < width; i++)
			{
				px_byte a = 255;
				px_byte r = *((px_word *)(imagedata+(width * j + i) * 3*2)+0)>>8;
				px_byte g = *((px_word*)(imagedata + (width * j + i) * 3 * 2) + 1) >> 8;
				px_byte b = *((px_word*)(imagedata + (width * j + i) * 3 * 2) + 2) >> 8;
				if (prendersurface)
					PX_SurfaceSetPixel(prendersurface, i, j, PX_COLOR(a, r, g, b));
				if (pshape)
					PX_ShapeSetPixel(pshape, i, j, (px_uchar)(0.2126 * r + 0.7152 * g + 0.0722 * b));
			}
		}
		MP_Free(mp, imagedata);
	}
	return PX_TRUE;
	case	PX_PNG_FORMAT_R8G8B8A8:
	{
		
		for (j = 0; j < height; j++)
		{
			for (i = 0; i < width; i++)
			{
				
				px_byte r = imagedata[(width * j + i) * 4];
				px_byte g = imagedata[(width * j + i) * 4 + 1];
				px_byte b = imagedata[(width * j + i) * 4 + 2];
				px_byte a = imagedata[(width * j + i) * 4 + 3];
				if (prendersurface)
					PX_SurfaceSetPixel(prendersurface, i, j, PX_COLOR(a, r, g, b));
				if (pshape)
					PX_ShapeSetPixel(pshape, i, j, a);
			}
		}
		MP_Free(mp, imagedata);
	}
	return PX_TRUE;
	case	PX_PNG_FORMAT_R16G16B16A16:
	{

		for (j = 0; j < height; j++)
		{
			for (i = 0; i < width; i++)
			{
				px_byte r = *((px_word*)(imagedata + (width * j + i) * 4 * 2) + 0) >> 8;
				px_byte g = *((px_word*)(imagedata + (width * j + i) * 4 * 2) + 1) >> 8;
				px_byte b = *((px_word*)(imagedata + (width * j + i) * 4 * 2) + 2) >> 8;
				px_byte a = *((px_word*)(imagedata + (width * j + i) * 4 * 2) + 3) >> 8;
				if (prendersurface)
					PX_SurfaceSetPixel(prendersurface, i, j, PX_COLOR(a, r, g, b));
				if (pshape)
					PX_ShapeSetPixel(pshape, i, j, a);
			}
		}
		MP_Free(mp, imagedata);
	}
	return PX_TRUE;
	case	PX_PNG_FORMAT_ERROR:
		return 0;
	default:
		break;
	}
	return 0;
	return PX_TRUE;
}

px_bool PX_PngToRenderBuffer(px_memorypool* calcBuffer, px_byte* ppngbuffer, px_int size, px_surface* prenderbuffer)
{
	return PX_PngToXBuffer(calcBuffer, ppngbuffer, size, prenderbuffer, PX_NULL);
}

px_bool PX_PngToShapeBuffer(px_memorypool* calcBuffer, px_byte* ppngbuffer, px_int size, px_shape* pshapebuffer)
{
	return PX_PngToXBuffer(calcBuffer, ppngbuffer, size, PX_NULL, pshapebuffer);
}





px_bool PX_PngSurfaceToBuffer(px_surface* prenderbuffer, px_memory* out)
{
	px_byte* rgba_buffer = 0;
	px_memory filter_data;
	px_memory deflate_data;
	px_dword adler32;
	px_int width = prenderbuffer->width;
	px_int height = prenderbuffer->height;
	//header
	do
	{
		const px_byte magic_header[] = { 137,80,78,71,13,10,26,10 };
		px_byte byte_code;
		px_int pchunkindex;
		px_dword crc32;
		if (!PX_MemoryCat(out, magic_header, sizeof(magic_header)))return PX_FALSE;
		pchunkindex = out->usedsize;
		if (!PX_MemoryCat(out, "\0\0\0\x0dIHDR", 8))return PX_FALSE;

		//width
		byte_code = ((width) >> 24) & 0xff;
		if (!PX_MemoryCat(out,&byte_code, 1))return PX_FALSE;
		byte_code = ((width) >> 16) & 0xff;
		if (!PX_MemoryCat(out, &byte_code, 1))return PX_FALSE;
		byte_code = ((width) >> 8) & 0xff;
		if (!PX_MemoryCat(out, &byte_code, 1))return PX_FALSE;
		byte_code = ((width) >> 0) & 0xff;
		if (!PX_MemoryCat(out, &byte_code, 1))return PX_FALSE;

		//height
		byte_code = ((height) >> 24) & 0xff;
		if (!PX_MemoryCat(out, &byte_code, 1))return PX_FALSE;
		byte_code = ((height) >> 16) & 0xff;
		if (!PX_MemoryCat(out, &byte_code, 1))return PX_FALSE;
		byte_code = ((height) >> 8) & 0xff;
		if (!PX_MemoryCat(out, &byte_code, 1))return PX_FALSE;
		byte_code = ((height) >> 0) & 0xff;
		if (!PX_MemoryCat(out, &byte_code, 1))return PX_FALSE;

		//color depth
		byte_code = 8;
		if (!PX_MemoryCat(out, &byte_code, 1))return PX_FALSE;

		//color type
		byte_code = PX_PNG_COLOR_TYPE_RGBA;
		if (!PX_MemoryCat(out, &byte_code, 1))return PX_FALSE;

		//26 27 28--->0,0,0
		byte_code = 0;
		if (!PX_MemoryCat(out, &byte_code, 1))return PX_FALSE;
		if (!PX_MemoryCat(out, &byte_code, 1))return PX_FALSE;
		if (!PX_MemoryCat(out, &byte_code, 1))return PX_FALSE;

		crc32 =PX_PngChunkGetCRC(out->buffer+ pchunkindex);

		byte_code = ((crc32) >> 24) & 0xff;
		if (!PX_MemoryCat(out, &byte_code, 1))return PX_FALSE;
		byte_code = ((crc32) >> 16) & 0xff;
		if (!PX_MemoryCat(out, &byte_code, 1))return PX_FALSE;
		byte_code = ((crc32) >> 8) & 0xff;
		if (!PX_MemoryCat(out, &byte_code, 1))return PX_FALSE;
		byte_code = ((crc32) >> 0) & 0xff;
		if (!PX_MemoryCat(out, &byte_code, 1))return PX_FALSE;

	} while (0);

	//BGRA-->RGBA
	do
	{
		px_int x,y;
		rgba_buffer = (px_byte *)MP_Malloc(out->mp, width * height * 4);
		if (!rgba_buffer)
		{
			return PX_FALSE;
		}
		for (y = 0; y < height; y++)
		{
			for ( x = 0; x < width; x++)
			{
				rgba_buffer[y * width * 4 + x * 4 + 0] = prenderbuffer->surfaceBuffer[width * y + x]._argb.r;
				rgba_buffer[y * width * 4 + x * 4 + 1] = prenderbuffer->surfaceBuffer[width * y + x]._argb.g;
				rgba_buffer[y * width * 4 + x * 4 + 2] = prenderbuffer->surfaceBuffer[width * y + x]._argb.b;
				rgba_buffer[y * width * 4 + x * 4 + 3] = prenderbuffer->surfaceBuffer[width * y + x]._argb.a;
			}
		}

		//to differential signals
		for (y = height-1; y >0 ; y--)
		{
			for (x = 0; x < width*4; x++)
			{
				rgba_buffer[y * width * 4 + x] -= rgba_buffer[(y-1) * width * 4 + x];
			}
		}
	} while (0);
	
	
	//filter
	do
	{
		px_int y;
		
		PX_MemoryInitialize(out->mp, &filter_data);
		for (y = 0; y < height; y++)
		{
				px_byte code = y?2:0;
				px_byte* pcur = &rgba_buffer[y* width * 4];
				if (!PX_MemoryCat(&filter_data, &code, 1))
				{
					MP_Free(out->mp, rgba_buffer);
					PX_MemoryFree(&filter_data);
					return PX_FALSE;
				}
				if (!PX_MemoryCat(&filter_data, pcur, width * 4))
				{
					MP_Free(out->mp, rgba_buffer);
					PX_MemoryFree(&filter_data);
					return PX_FALSE;
				}
		}
		MP_Free(out->mp, rgba_buffer);
	} while (0);

	

	do
	{
		px_byte byte_code;
		PX_MemoryInitialize(out->mp, &deflate_data);
		if (!PX_MemoryCat(&deflate_data,"\x78\x9c",2))
		{
			PX_MemoryFree(&filter_data);
			PX_MemoryFree(&deflate_data);
			return PX_FALSE;
		}

		adler32 = PX_adler32(filter_data.buffer, filter_data.usedsize);


		if (!PX_RFC1951Deflate(filter_data.buffer,filter_data.usedsize,&deflate_data))
		{
			PX_MemoryFree(&filter_data);
			PX_MemoryFree(&deflate_data);
			return PX_FALSE;
		}

		byte_code = (adler32 >> 24) & 0xff;
		if (!PX_MemoryCat(&deflate_data, &byte_code, 1))return PX_FALSE;
		byte_code = (adler32 >> 16) & 0xff;
		if (!PX_MemoryCat(&deflate_data, &byte_code, 1))return PX_FALSE;
		byte_code = (adler32 >> 8) & 0xff;
		if (!PX_MemoryCat(&deflate_data, &byte_code, 1))return PX_FALSE;
		byte_code = (adler32 >> 0) & 0xff;
		if (!PX_MemoryCat(&deflate_data, &byte_code, 1))return PX_FALSE;
		
		PX_MemoryFree(&filter_data);
	} while (0);

	do
	{
		//write chunk
		px_byte byte_code;
		px_dword crc32;
		px_int pchunkindex = out->usedsize;

		//write chunk length
		byte_code = ((deflate_data.usedsize) >> 24) & 0xff;
		if (!PX_MemoryCat(out, &byte_code, 1))
		{
			PX_MemoryFree(&deflate_data);
			return PX_FALSE;
		}
		byte_code = ((deflate_data.usedsize) >> 16) & 0xff;
		if (!PX_MemoryCat(out, &byte_code, 1))
		{
			PX_MemoryFree(&deflate_data);
			return PX_FALSE;
		}
		byte_code = ((deflate_data.usedsize) >> 8) & 0xff;
		if (!PX_MemoryCat(out, &byte_code, 1))
		{
			PX_MemoryFree(&deflate_data);
			return PX_FALSE;
		}
		byte_code = ((deflate_data.usedsize) >> 0) & 0xff;
		if (!PX_MemoryCat(out, &byte_code, 1))
		{
			PX_MemoryFree(&deflate_data);
			return PX_FALSE;
		}

		byte_code = 'I';
		if (!PX_MemoryCat(out, &byte_code, 1))
		{
			PX_MemoryFree(&deflate_data);
			return PX_FALSE;
		}
		byte_code = 'D';
		if (!PX_MemoryCat(out, &byte_code, 1))
		{
			PX_MemoryFree(&deflate_data);
			return PX_FALSE;
		}
		byte_code = 'A';
		if (!PX_MemoryCat(out, &byte_code, 1))
		{
			PX_MemoryFree(&deflate_data);
			return PX_FALSE;
		}
		byte_code = 'T';
		if (!PX_MemoryCat(out, &byte_code, 1))
		{
			PX_MemoryFree(&deflate_data);
			return PX_FALSE;
		}

		if (!PX_MemoryCat(out, deflate_data.buffer, deflate_data.usedsize)) 
		{
			PX_MemoryFree(&deflate_data);
			return PX_FALSE;
		}

		crc32= PX_PngChunkGetCRC(out->buffer+ pchunkindex);
		PX_MemoryFree(&deflate_data);


		byte_code = (crc32 >> 24) & 0xff;
		if (!PX_MemoryCat(out, &byte_code, 1))return PX_FALSE;
		byte_code = (crc32 >> 16) & 0xff;
		if (!PX_MemoryCat(out, &byte_code, 1))return PX_FALSE;
		byte_code = (crc32 >> 8) & 0xff;
		if (!PX_MemoryCat(out, &byte_code, 1))return PX_FALSE;
		byte_code = (crc32 >> 0) & 0xff;
		if (!PX_MemoryCat(out, &byte_code, 1))return PX_FALSE;

	} while (0);

	do
	{
		//write chunk
		px_byte byte_code;
		byte_code = 0;
		if (!PX_MemoryCat(out, &byte_code, 1)){	return PX_FALSE;}
		if (!PX_MemoryCat(out, &byte_code, 1)) { return PX_FALSE; }
		if (!PX_MemoryCat(out, &byte_code, 1)) { return PX_FALSE; }
		if (!PX_MemoryCat(out, &byte_code, 1)) { return PX_FALSE; }

		if (!PX_MemoryCat(out, "IEND", 4))
		{
			return PX_FALSE;
		}
		
		do
		{
			px_dword crc32 = PX_crc32("IEND", 4);
			byte_code = (crc32 >> 24) & 0xff;
			if (!PX_MemoryCat(out, &byte_code, 1))return PX_FALSE;
			byte_code = (crc32 >> 16) & 0xff;
			if (!PX_MemoryCat(out, &byte_code, 1))return PX_FALSE;
			byte_code = (crc32 >> 8) & 0xff;
			if (!PX_MemoryCat(out, &byte_code, 1))return PX_FALSE;
			byte_code = (crc32 >> 0) & 0xff;
			if (!PX_MemoryCat(out, &byte_code, 1))return PX_FALSE;
		} while (0);


	} while (0);

	return PX_TRUE;
}