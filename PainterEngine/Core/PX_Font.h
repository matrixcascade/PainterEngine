#ifndef PIXELSES_FONT
#define PIXELSES_FONT

#include "../Core/PX_Surface.h"

#define __PX_FONT_GBKSIZE		13
#define __PX_FONT_ASCSIZE		8
#define __PX_FONT_HEIGHT		15
////////////////////////////////////////////////////////////////////////////////////////////
///////								Private function									////
//////																					////	
////////////////////////////////////////////////////////////////////////////////////////////

#define PX_FontGetCharactorHeight() (__PX_FONT_HEIGHT)
#define PX_FontGetAscCharactorWidth() (__PX_FONT_ASCSIZE)
#define PX_FontGetGbkCharactorWidth() (__PX_FONT_GBKSIZE)

px_void PX_FontDrawGBK(px_surface *psurface,px_int x,px_int y, px_uchar *str,px_color Color);
px_void PX_FontDrawASCII(px_surface *psurface,px_int x,px_int y, px_uchar ASCI,px_color Color);
px_void PX_FontDrawText(px_surface *psurface,int x,int y,px_char *Text,px_color Color);
px_void PX_FontDrawChar(px_surface *psurface,int x,int y,px_char *Text,px_color Color);
px_int  PX_GetFontTextPixelsWidth(px_char *Text);

#endif
 
