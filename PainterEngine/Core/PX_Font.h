#ifndef PIXELSES_FONT
#define PIXELSES_FONT

#include "../Core/PX_Surface.h"
#include "../Core/PX_Texture.h"
#include "PX_Hashmap.h"

#define __PX_FONT_GBKSIZE		13
#define __PX_FONT_ASCSIZE		8
#define __PX_FONT_HEIGHT		15
#define __PX_FONT_MODULE_XSPACE_SIZE 8
#define __PX_FONT_MODULE_YSPACE_SIZE 8
////////////////////////////////////////////////////////////////////////////////////////////
///////								Private function									////
//////																					////	
////////////////////////////////////////////////////////////////////////////////////////////

#define PX_FontGetCharactorHeight() (__PX_FONT_HEIGHT)
#define PX_FontGetAscCharactorWidth() (__PX_FONT_ASCSIZE)
#define PX_FontGetGbkCharactorWidth() (__PX_FONT_GBKSIZE)

typedef struct
{
	union
	{
		px_char  c_magic[4];//PXFM
		px_dword magic;
	};
	px_dword charactor_code;
	px_dword BearingX;
	px_dword BearingY;
	px_dword Advance;
	px_dword Font_Width;
	px_dword Font_Height;
	px_dword align_dummy;
}PX_FontModule_Charactor_Header;

typedef struct
{
	PX_FontModule_Charactor_Header header;
	px_shape shape;
}PX_FontModule_Charactor;

typedef struct  
{
	px_memorypool *mp;
	px_map characters_map;
	px_int xspacer;
	px_int yspacer;
}PX_FontModule;

typedef enum
{
	PX_FONT_ALIGN_XLEFT,
	PX_FONT_ALIGN_XCENTER,
	PX_FONT_ALIGN_XRIGHT,
}PX_FONT_ALIGN;

px_void PX_FontDrawGBK(px_surface *psurface,px_int x,px_int y, px_uchar *str,px_color Color);
px_void PX_FontDrawASCII(px_surface *psurface,px_int x,px_int y, px_uchar ASCI,px_color Color);
px_void PX_FontDrawText(px_surface *psurface,int x,int y,const px_char *Text,px_color Color,PX_FONT_ALIGN align);
px_void PX_FontDrawChar(px_surface *psurface,int x,int y,const px_char *Text,px_color Color);
px_int  PX_GetFontTextPixelsWidth(px_char *Text);

px_void PX_FontModule_atow(const char *a,px_word *w);
px_bool PX_FontModuleInitialize(px_memorypool *mp,PX_FontModule *module);
px_bool PX_FontModuleLoad(PX_FontModule *module,px_byte *buffer,px_int size);
px_void PX_FontModuleFree(PX_FontModule *module);
px_int  PX_FontModuleGetTextPixelsWidth(PX_FontModule *mod,px_word *Text);
px_void PX_FontModuleDrawText(px_surface *psurface,int x,int y,const px_word *Text,px_color Color,PX_FontModule *mod,PX_FONT_ALIGN align);
px_void PX_FontModuleSetXYSpace(PX_FontModule *module,int x,int y);

#endif
 
