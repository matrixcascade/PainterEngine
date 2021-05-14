#ifndef PIXELSES_BITMAP
#define PIXELSES_BITMAP

#include "PX_Surface.h"

typedef struct PX_BITMAPFILEHEADER {
	px_char    bfType[2];              //Must be "BM" offset 0-1
	px_char    bfSize[4];				//offset 2-5
	px_char	bfReserbed[4];          //offset 6-13
	px_uchar bfOffbits[4];
} PX_BITMAPFILEHEADER;				//14 Byte 

typedef struct PX_BITMAPINFOHEADER{
	px_dword			biSize;			//offset 14-17 
	px_dword			biWidth;		//offset 18-21 
	px_dword			biHeight;		//offset 22-25 
	px_word  biPlanes;		//offset 26-27
	px_word  biBitCount;		//offset 28-29
	px_dword	        biCompression;
	px_dword	        biSizeImage;
	px_dword		    biXPelsPerMeter;
	px_dword		    biYPelsPerMeter;
	px_dword		    biClrUsed;
	px_dword	        biClrImportant;
} PX_BITMAPINFOHEADER;

typedef struct PX_BITMAP_tagRBG24
{
	px_char B;
	px_char G;
	px_char R;
}PX_BITMAP_RGB24;

typedef struct PX_BITMAP_tagRBG32
{
	px_char B;
	px_char G;
	px_char R;
	px_char A;
}PX_BITMAP_RGB32;

//Bitmap Verify
px_bool PX_BitmapVerify(void *BitmapBuffer,px_int size);

//Get bitmap height
px_uint PX_BitmapGetHeight(void *BitmapBuffer);
//Get bitmap width
px_uint PX_BitmapGetWidth(void *BitmapBuffer);
//Get bitmap BitCount
px_word PX_BitmapGetBitCount(void *BitmapBuffer);
//////////////////////////////////////////////////////////////////////////
//	Draw bitmap to panel
// param:void *BitmapBuffer -Point to buffer of bitmap
// param:int x -Offset of X
// param:int y -Offset of Y
/////////////////////////////////////////////////////////////////////////
px_void PX_BitmapRender(px_surface *psurface,void *BitmapBuffer,px_int BufferSize,int x,int y);

//////////////////////////////////////////////////////////////////////////
// build bitmap from surface
// param:px_memory *BitmapBuffer -Pointer of px_buffer to save bmpfile data
/////////////////////////////////////////////////////////////////////////

px_bool PX_BitmapBuild(px_surface *psurface,px_char *BitmapBuffer,px_int *size);

#endif
