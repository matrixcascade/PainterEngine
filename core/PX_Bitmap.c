#include "PX_Bitmap.h"

px_uint PX_BitmapGetHeight( void *BitmapBuffer )
{
	return (px_dword)(*(px_dword *)((px_char *)BitmapBuffer+22));
}

px_uint PX_BitmapGetWidth( void *BitmapBuffer )
{
	return (px_dword)(*(px_dword *)((px_char *)BitmapBuffer+18));
}

px_word PX_BitmapGetBitCount( void *BitmapBuffer )
{
	return (px_word)(*(px_word *)((px_char *)BitmapBuffer+28));
}

px_dword PX_BitmapGetDataOffset(void *BitmapBuffer)
{
	return (px_dword)(*(px_dword *)((px_char *)BitmapBuffer+10));
}

px_void PX_BitmapRender(px_surface *psurface, void *BitmapBuffer,px_int BufferSize,px_int x,px_int y )
{
	px_int i,j;
	px_int Height;
	px_int Width;
	px_int RowBytes;
	px_word bitCount;
	px_char  *ByteColor;
	PX_BITMAP_RGB32	 *rgb32;
	PX_BITMAP_RGB24	 *rgb24;
	px_word *rgb16;
	px_color     PXcolor32;
	if (x>psurface->width||y>psurface->height)
	{
		return;
	}

	Height=PX_BitmapGetHeight(BitmapBuffer);
	Width=PX_BitmapGetWidth(BitmapBuffer);
	bitCount=PX_BitmapGetBitCount(BitmapBuffer);
	if (Height>0)
	{
		if (x<-Width||y<-Height)
		{
			return;
		}
	}
	else
	{
		if (x<-Width||y<Height)
		{
			return;
		}
	}
	
	//Skip to Bitmap file header & information struct
	ByteColor=((px_char *)BitmapBuffer+PX_BitmapGetDataOffset(BitmapBuffer));

	//32bit Color for A8R8G8B8(X8R8G8B8)
	if(bitCount==32)
	{
		rgb32=(PX_BITMAP_RGB32 *)ByteColor;
		if (Height*Width>BufferSize-(px_int)PX_BitmapGetDataOffset(BitmapBuffer))
		{
			return;
		}
		if(Height>0)
		{
			for (i=0;i<Height;i++)
			{
				for (j=0;j<Width;j++)
				{
					px_color clr;
					clr._argb.a = rgb32[Width * (Height - 1 - i) + j].A;
					clr._argb.r = rgb32[Width * (Height - 1 - i) + j].R;
					clr._argb.g = rgb32[Width * (Height - 1 - i) + j].G;
					clr._argb.b = rgb32[Width * (Height - 1 - i) + j].B;
					PX_SurfaceDrawPixel(psurface,j+x,i+y, clr);
				}
			}
		}
		else
		{
			for (i=0;i<-Height;i++)
			{
				for (j=0;j<Width;j++)
				{
					px_color clr;
					clr._argb.a = rgb32[Width * (Height - 1 - i) + j].A;
					clr._argb.r = rgb32[Width * (Height - 1 - i) + j].R;
					clr._argb.g = rgb32[Width * (Height - 1 - i) + j].G;
					clr._argb.b = rgb32[Width * (Height - 1 - i) + j].B;
					PX_SurfaceDrawPixel(psurface,j+x,i+y, clr);
				}
			}
		}
	}


	//24bit Color for R8G8B8
	if(bitCount==24)
	{
	   //Translate buffer as format
		
		if((Width*3)%4)
			RowBytes=Width*3+4-((Width*3)%4);
		else
			RowBytes=Width*3;

		if (Height*RowBytes>BufferSize-(px_int)PX_BitmapGetDataOffset(BitmapBuffer))
		{
			return;
		}
		if(Height>0)
		{
			for (i=0;i<Height;i++)
			{
				rgb24=(PX_BITMAP_RGB24 *)(ByteColor+RowBytes*(Height-1-i));
				for (j=0;j<Width;j++)
				{
					PXcolor32._argb.a=255;
					PXcolor32._argb.r=rgb24[j].R;
					PXcolor32._argb.g=rgb24[j].G;
					PXcolor32._argb.b=rgb24[j].B;
					if(j+x<psurface->width&&i+y<psurface->height)
					PX_SurfaceSetPixel(psurface,j+x,i+y,PXcolor32);
				}
			}
		}
		else
		{
			for (i=0;i<-Height;i++)
			{
				rgb24=(PX_BITMAP_RGB24 *)(ByteColor+RowBytes*(i));
				for (j=0;j<Width;j++)
				{
					PXcolor32._argb.a=255;
					PXcolor32._argb.r=rgb24[j].R;
					PXcolor32._argb.g=rgb24[j].G;
					PXcolor32._argb.b=rgb24[j].B;
					if(j+x>0&&i+y>0&&j+x<psurface->width&&i+y<psurface->height)
					PX_SurfaceSetPixel(psurface,j+x,i+y,PXcolor32);
				}
			}
		}
		return;
	}

	//16bit Color for R5G6B5
	if(bitCount==16)
	{
		//Translate buffer as format
		rgb16=(px_word *)ByteColor;
		if((Width*2)%4)
			RowBytes=Width*2+4-((Width*2)%4);
		else
			RowBytes=Width*2;

		if (Height*RowBytes>BufferSize-(px_int)PX_BitmapGetDataOffset(BitmapBuffer))
		{
			return;
		}
		if(Height>0)
		{
			for (i=0;i<Height;i++)
			{
				rgb16=(px_word *)(ByteColor+RowBytes*(Height-1-i));
				for (j=0;j<Width;j++)
				{
					PXcolor32._argb.a=255;
					PXcolor32._argb.b= ((rgb16[j]&0x1F)		*255)/31;
					PXcolor32._argb.g=(((rgb16[j]>>5)&0x1F)	*255)/31;
					PXcolor32._argb.r=(((rgb16[j]>>10)&0x1F)*255)/31;
					if(j+x>0&&i+y>0&&j+x<psurface->width&&i+y<psurface->height)
					PX_SurfaceSetPixel(psurface,j+x,i+y,PXcolor32);
				}
			}
		}
		else
		{
			for (i=0;i<-Height;i++)
			{
				rgb16=(px_word *)(ByteColor+RowBytes*(i));
				for (j=0;j<Width;j++)
				{
					PXcolor32._argb.a=255;
					PXcolor32._argb.b= ((rgb16[j]&0x1F)		*255)/31;
					PXcolor32._argb.g=(((rgb16[j]>>5)&0x1F)	*255)/31;
					PXcolor32._argb.r=(((rgb16[j]>>10)&0x1F)*255)/31;
					if(j+x>0&&i+y>0&&j+x<psurface->width&&i+y<psurface->height)
					PX_SurfaceSetPixel(psurface,j+x,i+y,PXcolor32);
				}
			}
		}
		return;
	}

	//Other bit format is no support
	return;
}

px_bool PX_BitmapVerify(void *BitmapBuffer,px_int BufferSize)
{

	px_int Height;
	px_int Width;
	px_int RowBytes;
	px_word bitCount;

	if (BufferSize<54)
	{
		return PX_FALSE;
	}
	if (((px_char *)BitmapBuffer)[0]!='B')
	{
		return PX_FALSE;
	}

	if (((px_char *)BitmapBuffer)[1]!='M')
	{
		return PX_FALSE;
	}


	Height=PX_BitmapGetHeight(BitmapBuffer);
	Width=PX_BitmapGetWidth(BitmapBuffer);
	bitCount=PX_BitmapGetBitCount(BitmapBuffer);

	//32bit Color for A8R8G8B8(X8R8G8B8)
	if(bitCount==32)
	{
		if (Height*Width>BufferSize-(px_int)PX_BitmapGetDataOffset(BitmapBuffer))
		{
			return PX_FALSE;
		}
	}


	//24bit Color for R8G8B8
	if(bitCount==24)
	{
		//Translate buffer as format

		if((Width*3)%4)
		RowBytes=Width*3+4-((Width*3)%4);
		else
		RowBytes=Width*3;

		if (Height*RowBytes>BufferSize-(px_int)PX_BitmapGetDataOffset(BitmapBuffer))
		{
			return PX_FALSE;
		}
	}

	//16bit Color for R5G6B5
	if(bitCount==16)
	{
		//Translate buffer as format
		if((Width*2)%4)
		RowBytes=Width*2+4-((Width*2)%4);
		else
		RowBytes=Width*2;
		
		if (Height*RowBytes>BufferSize-(px_int)PX_BitmapGetDataOffset(BitmapBuffer))
		{
			return PX_FALSE;
		}
	}

	return PX_TRUE;
}

px_bool PX_BitmapBuild(px_surface *psurface,px_char *BitmapBuffer,px_int *size)
{
	px_int _outsize,x,y,rowAppendSize;
	PX_BITMAPFILEHEADER fileheader;
	PX_BITMAPINFOHEADER infoheader;
	PX_BITMAP_RGB24 *pdata;

	rowAppendSize=psurface->width%4;


	_outsize=psurface->height*psurface->width*3+sizeof(PX_BITMAPFILEHEADER)+sizeof(PX_BITMAPINFOHEADER)+rowAppendSize*psurface->height;

	*size=_outsize;
	if (BitmapBuffer)
	{
		px_dword bfoftsize=sizeof(PX_BITMAPFILEHEADER)+sizeof(PX_BITMAPINFOHEADER);
		fileheader.bfType[0]='B';
		fileheader.bfType[1]='M';
		PX_memcpy(fileheader.bfSize,&_outsize,4);
		fileheader.bfReserbed[0]=0;
		fileheader.bfReserbed[1]=0;
		fileheader.bfReserbed[2]=0;
		fileheader.bfReserbed[3]=0;
		PX_memcpy(fileheader.bfOffbits,&bfoftsize,4);

		infoheader.biBitCount=24;
		infoheader.biClrImportant=0;
		infoheader.biClrUsed=0;
		infoheader.biCompression=0;
		infoheader.biHeight=psurface->height;
		infoheader.biWidth=psurface->width;
		infoheader.biPlanes=1;
		infoheader.biSize=40;
		infoheader.biSizeImage=psurface->height*psurface->width*3;
		infoheader.biXPelsPerMeter=2834;
		infoheader.biYPelsPerMeter=2834;

		PX_memcpy(BitmapBuffer,&fileheader,sizeof(fileheader));
		PX_memcpy(BitmapBuffer+14,&infoheader,sizeof(infoheader));

		pdata=(PX_BITMAP_RGB24 *)(BitmapBuffer+sizeof(PX_BITMAPFILEHEADER)+sizeof(PX_BITMAPINFOHEADER));
		for (y=psurface->height-1;y>=0;y--)
		{
			for (x=0;x<psurface->width;x++)
			{
				(*pdata).R=(px_char)(PX_SURFACECOLOR(psurface,x,y)._argb.r*PX_SURFACECOLOR(psurface,x,y)._argb.a/256);
				(*pdata).G= (px_char)(PX_SURFACECOLOR(psurface,x,y)._argb.g*PX_SURFACECOLOR(psurface,x,y)._argb.a/256);
				(*pdata).B= (px_char)(PX_SURFACECOLOR(psurface,x,y)._argb.b*PX_SURFACECOLOR(psurface,x,y)._argb.a/256);
				pdata++;
			}
			pdata=(PX_BITMAP_RGB24 *)(((px_char *)pdata)+rowAppendSize);
		}
		return PX_TRUE;
	}
	else
		return PX_TRUE;
}
