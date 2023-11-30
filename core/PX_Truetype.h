#ifndef PX_TRUETYPE_H
#define PX_TRUETYPE_H
#include "PX_MemoryPool.h"

typedef struct
{
	px_byte* data;
	px_int cursor;
	px_int size;
} PX_tt_buf;

typedef struct
{
	px_ushort x0, y0, x1, y1;
	px_float xoff, yoff, xadvance;
} PX_ttbakedchar;

typedef struct 
{
	px_memorypool* mp;
	px_void* userdata;
	px_byte* data;
	px_int              fontstart;
	px_int numGlyphs;
	px_int loca, head, glyf, hhea, hmtx, kern, gpos, svg;
	px_int index_map;
	px_int indexToLocFormat;
	PX_tt_buf cff;
	PX_tt_buf charstrings;
	PX_tt_buf gsubrs;
	PX_tt_buf subrs;
	PX_tt_buf fontdicts;
	PX_tt_buf fdselect;
}PX_ttfontinfo;

px_int PX_ttInitFont(px_memorypool* mp, PX_ttfontinfo* info, const px_byte* data, px_int offset);
px_int PX_ttFindGlyphIndex(const PX_ttfontinfo* info, px_int unicode_codepoint);
px_float PX_ttScaleForPixelHeight(const PX_ttfontinfo* info, px_float pixels);
px_float PX_ttScaleForMappingEmToPixels(const PX_ttfontinfo* info, px_float pixels);
px_void PX_ttGetFontVMetrics(const PX_ttfontinfo* info, px_int* ascent, px_int* descent, px_int* lineGap);
px_int  PX_ttGetFontVMetricsOS2(const PX_ttfontinfo* info, px_int* typoAscent, px_int* typoDescent, px_int* typoLineGap);
px_void PX_ttGetFontBoundingBox(const PX_ttfontinfo* info, px_int* x0, px_int* y0, px_int* x1, px_int* y1);
px_void PX_ttGetCodepointHMetrics(const PX_ttfontinfo* info, px_int codepoint, px_int* advanceWidth, px_int* leftSideBearing);
px_int  PX_ttGetCodepointKernAdvance(const PX_ttfontinfo* info, px_int ch1, px_int ch2);
px_int PX_ttGetCodepointBox(const PX_ttfontinfo* info, px_int codepoint, px_int* x0, px_int* y0, px_int* x1, px_int* y1);
px_void PX_ttGetGlyphHMetrics(const PX_ttfontinfo* info, px_int glyph_index, px_int* advanceWidth, px_int* leftSideBearing);
px_int  PX_ttGetGlyphKernAdvance(const PX_ttfontinfo* info, px_int glyph1, px_int glyph2);
px_int  PX_ttGetGlyphBox(const PX_ttfontinfo* info, px_int glyph_index, px_int* x0, px_int* y0, px_int* x1, px_int* y1);
px_void PX_ttGetCodepointBitmapBox(const PX_ttfontinfo* font, px_int codepoint, px_float scale_x, px_float scale_y, px_int* ix0, px_int* iy0, px_int* ix1, px_int* iy1);
px_void PX_ttMakeCodepointBitmap(const PX_ttfontinfo* info, px_byte* output, px_int out_w, px_int out_h, px_int out_stride, px_float scale_x, px_float scale_y, px_int codepoint);
#endif // !PX_TRUETYPE_H
