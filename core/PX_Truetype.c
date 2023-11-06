#include "PX_Truetype.h"

typedef char PX_tt_check_size32[sizeof(px_int32)==4 ? 1 : -1];
typedef char PX_tt_check_size16[sizeof(px_short)==2 ? 1 : -1];

#define STBTT_ifloor(x)   ((px_int) PX_floor(x))
#define STBTT_iceil(x)    ((px_int) PX_ceil(x))
#define STBTT_sqrt(x)      PX_sqrtd(x)
#define STBTT_pow(x,y)     PX_pow(x,y)
#define STBTT_fmod(x,y)    PX_fmod(x,y)
#define STBTT_cos(x)       PX_cosd(x)
#define STBTT_acos(x)      PX_acos(x)
#define STBTT_fabs(x)      PX_ABS(x)
#define STBTT_strlen(x)    PX_strlen(x)
#define STBTT_memcpy       PX_memcpy
#define STBTT_memset       PX_memset
#define STBTT_POINT_SIZE(x)   (-(x))
#define STBTT_assert(x)    
#define STBTT_MACSTYLE_DONTCARE     0
#define STBTT_MACSTYLE_BOLD         1
#define STBTT_MACSTYLE_ITALIC       2
#define STBTT_MACSTYLE_UNDERSCORE   4
#define STBTT_MACSTYLE_NONE         8  
#define __STB_INCLUDE_STB_TRUETYPE_H__


px_int PX_ttBakeFontBitmap(px_memorypool* mp, const px_byte *data, px_int offset,  
                                px_float pixel_height,                     
                                px_byte *pixels, px_int pw, px_int ph,  
                                px_int first_char, px_int num_chars,          
                                PX_ttbakedchar *chardata);             
typedef struct
{
   px_float x0,y0,s0,t0; 
   px_float x1,y1,s1,t1; 
} PX_ttaligned_quad;
px_void PX_ttGetBakedQuad(const PX_ttbakedchar *chardata, px_int pw, px_int ph,  
                               px_int char_index,             
                               px_float *xpos, px_float *ypos,   
                               PX_ttaligned_quad *q,      
                               px_int opengl_fillrule);       
px_void PX_ttGetScaledFontVMetrics(px_memorypool* mp, const px_byte *fontdata, px_int index, px_float size, px_float *ascent, px_float *descent, px_float *lineGap);
typedef struct
{
   px_ushort x0,y0,x1,y1; 
   px_float xoff,yoff,xadvance;
   px_float xoff2,yoff2;
} PX_ttpackedchar;
typedef struct PX_ttpack_context PX_ttpack_context;
typedef struct stbrp_rect stbrp_rect;
px_int  PX_ttPackBegin(px_memorypool* mp, PX_ttpack_context *spc, px_byte *pixels, px_int width, px_int height, px_int stride_in_bytes, px_int padding, px_void *alloc_context);
px_void PX_ttPackEnd  (px_memorypool* mp, PX_ttpack_context *spc);

px_int  PX_ttPackFontRange(px_memorypool* mp, PX_ttpack_context *spc, const px_byte *fontdata, px_int font_index, px_float font_size,
                                px_int first_unicode_char_in_range, px_int num_chars_in_range, PX_ttpackedchar *chardata_for_range);
typedef struct
{
   px_float font_size;
   px_int first_unicode_codepoint_in_range;  
   px_int *array_of_unicode_codepoints;       
   px_int num_chars;
   PX_ttpackedchar *chardata_for_range; 
   px_byte h_oversample, v_oversample; 
} PX_ttpack_range;
px_int  PX_ttPackFontRanges(px_memorypool* mp, PX_ttpack_context *spc, const px_byte *fontdata, px_int font_index, PX_ttpack_range *ranges, px_int num_ranges);
px_void PX_ttPackSetOversampling(PX_ttpack_context *spc, px_uint h_oversample, px_uint v_oversample);
px_void PX_ttPackSetSkipMissingCodepoints(PX_ttpack_context *spc, px_int skip);
px_void PX_ttGetPackedQuad(const PX_ttpackedchar *chardata, px_int pw, px_int ph,  
                               px_int char_index,             
                               px_float *xpos, px_float *ypos,   
                               PX_ttaligned_quad *q,      
                               px_int align_to_integer);
px_int  PX_ttPackFontRangesGatherRects(PX_ttpack_context *spc, const PX_ttfontinfo *info, PX_ttpack_range *ranges, px_int num_ranges, stbrp_rect *rects);
px_void PX_ttPackFontRangesPackRects(PX_ttpack_context *spc, stbrp_rect *rects, px_int num_rects);
px_int  PX_ttPackFontRangesRenderIntoRects(PX_ttpack_context *spc, const PX_ttfontinfo *info, PX_ttpack_range *ranges, px_int num_ranges, stbrp_rect *rects);
struct PX_ttpack_context {
   px_void *user_allocator_context;
   px_void *pack_info;
   px_int   width;
   px_int   height;
   px_int   stride_in_bytes;
   px_int   padding;
   px_int   skip_missing;
   px_uint   h_oversample, v_oversample;
   px_byte *pixels;
   px_void  *nodes;
};
px_int PX_ttGetNumberOfFonts(const px_byte *data);
px_int PX_ttGetFontOffsetForIndex(const px_byte *data, px_int index);



typedef struct PX_ttkerningentry
{
   px_int glyph1; 
   px_int glyph2;
   px_int advance;
} PX_ttkerningentry;
px_int  PX_ttGetKerningTableLength(const PX_ttfontinfo *info);
px_int  PX_ttGetKerningTable(const PX_ttfontinfo *info, PX_ttkerningentry* table, px_int table_length);
enum {
      STBTT_vmove=1,
      STBTT_vline,
      STBTT_vcurve,
      STBTT_vcubic
};
#define PX_ttvertex_type short 
   typedef struct
   {
      PX_ttvertex_type x,y,cx,cy,cx1,cy1;
      px_byte type,padding;
   } PX_ttvertex;
px_int PX_ttIsGlyphEmpty(const PX_ttfontinfo *info, px_int glyph_index);
px_int PX_ttGetCodepointShape(const PX_ttfontinfo *info, px_int unicode_codepoint, PX_ttvertex **vertices);
px_int PX_ttGetGlyphShape(const PX_ttfontinfo *info, px_int glyph_index, PX_ttvertex **vertices);
px_void PX_ttFreeShape(const PX_ttfontinfo *info, PX_ttvertex *vertices);
px_byte *PX_ttFindSVGDoc(const PX_ttfontinfo *info, px_int gl);
px_int PX_ttGetCodepointSVG(const PX_ttfontinfo *info, px_int unicode_codepoint, const char **svg);
px_int PX_ttGetGlyphSVG(const PX_ttfontinfo *info, px_int gl, const char **svg);
px_void PX_ttFreeBitmap(px_memorypool *mp,px_byte *bitmap, px_void *userdata);
px_byte *PX_ttGetCodepointBitmap(const PX_ttfontinfo *info, px_float scale_x, px_float scale_y, px_int codepoint, px_int *width, px_int *height, px_int *xoff, px_int *yoff);
px_byte *PX_ttGetCodepointBitmapSubpixel(const PX_ttfontinfo *info, px_float scale_x, px_float scale_y, px_float shift_x, px_float shift_y, px_int codepoint, px_int *width, px_int *height, px_int *xoff, px_int *yoff);

px_void PX_ttMakeCodepointBitmapSubpixel(const PX_ttfontinfo *info, px_byte *output, px_int out_w, px_int out_h, px_int out_stride, px_float scale_x, px_float scale_y, px_float shift_x, px_float shift_y, px_int codepoint);
px_void PX_ttMakeCodepointBitmapSubpixelPrefilter(const PX_ttfontinfo *info, px_byte *output, px_int out_w, px_int out_h, px_int out_stride, px_float scale_x, px_float scale_y, px_float shift_x, px_float shift_y, px_int oversample_x, px_int oversample_y, px_float *sub_x, px_float *sub_y, px_int codepoint);

px_void PX_ttGetCodepointBitmapBoxSubpixel(const PX_ttfontinfo *font, px_int codepoint, px_float scale_x, px_float scale_y, px_float shift_x, px_float shift_y, px_int *ix0, px_int *iy0, px_int *ix1, px_int *iy1);
px_byte *PX_ttGetGlyphBitmap(const PX_ttfontinfo *info, px_float scale_x, px_float scale_y, px_int glyph, px_int *width, px_int *height, px_int *xoff, px_int *yoff);
px_byte *PX_ttGetGlyphBitmapSubpixel(const PX_ttfontinfo *info, px_float scale_x, px_float scale_y, px_float shift_x, px_float shift_y, px_int glyph, px_int *width, px_int *height, px_int *xoff, px_int *yoff);
px_void PX_ttMakeGlyphBitmap(const PX_ttfontinfo *info, px_byte *output, px_int out_w, px_int out_h, px_int out_stride, px_float scale_x, px_float scale_y, px_int glyph);
px_void PX_ttMakeGlyphBitmapSubpixel(const PX_ttfontinfo *info, px_byte *output, px_int out_w, px_int out_h, px_int out_stride, px_float scale_x, px_float scale_y, px_float shift_x, px_float shift_y, px_int glyph);
px_void PX_ttMakeGlyphBitmapSubpixelPrefilter(const PX_ttfontinfo *info, px_byte *output, px_int out_w, px_int out_h, px_int out_stride, px_float scale_x, px_float scale_y, px_float shift_x, px_float shift_y, px_int oversample_x, px_int oversample_y, px_float *sub_x, px_float *sub_y, px_int glyph);
px_void PX_ttGetGlyphBitmapBox(const PX_ttfontinfo *font, px_int glyph, px_float scale_x, px_float scale_y, px_int *ix0, px_int *iy0, px_int *ix1, px_int *iy1);
px_void PX_ttGetGlyphBitmapBoxSubpixel(const PX_ttfontinfo *font, px_int glyph, px_float scale_x, px_float scale_y,px_float shift_x, px_float shift_y, px_int *ix0, px_int *iy0, px_int *ix1, px_int *iy1);
typedef struct
{
   px_int w,h,stride;
   px_byte *pixels;
} PX_tt_bitmap;
px_void PX_ttRasterize(px_memorypool *mp,
                               PX_tt_bitmap *result,        
                               px_float flatness_in_pixels,     
                               PX_ttvertex *vertices,       
                               px_int num_verts,                
                               px_float scale_x, px_float scale_y, 
                               px_float shift_x, px_float shift_y, 
                               px_int x_off, px_int y_off,         
                               px_int invert,                   
                               px_void *userdata);              
px_void PX_ttFreeSDF(px_memorypool* mp, px_byte *bitmap);
px_byte * PX_ttGetGlyphSDF(const PX_ttfontinfo *info, px_float scale, px_int glyph, px_int padding, px_byte onedge_value, px_float pixel_dist_scale, px_int *width, px_int *height, px_int *xoff, px_int *yoff);
px_byte * PX_ttGetCodepointSDF(const PX_ttfontinfo *info, px_float scale, px_int codepoint, px_int padding, px_byte onedge_value, px_float pixel_dist_scale, px_int *width, px_int *height, px_int *xoff, px_int *yoff);
px_int PX_ttFindMatchingFont(const px_byte *fontdata, const char *name, px_int flags);
 
px_int PX_ttCompareUTF8toUTF16_bigendian(const char *s1, px_int len1, const char *s2, px_int len2);
const char *PX_ttGetFontNameString(const PX_ttfontinfo *font, px_int *length, px_int platformID, px_int encodingID, px_int languageID, px_int nameID);
enum { 
   STBTT_PLATFORM_ID_UNICODE   =0,
   STBTT_PLATFORM_ID_MAC       =1,
   STBTT_PLATFORM_ID_ISO       =2,
   STBTT_PLATFORM_ID_MICROSOFT =3
};
enum { 
   STBTT_UNICODE_EID_UNICODE_1_0    =0,
   STBTT_UNICODE_EID_UNICODE_1_1    =1,
   STBTT_UNICODE_EID_ISO_10646      =2,
   STBTT_UNICODE_EID_UNICODE_2_0_BMP=3,
   STBTT_UNICODE_EID_UNICODE_2_0_FULL=4
};
enum { 
   STBTT_MS_EID_SYMBOL        =0,
   STBTT_MS_EID_UNICODE_BMP   =1,
   STBTT_MS_EID_SHIFTJIS      =2,
   STBTT_MS_EID_UNICODE_FULL  =10
};
enum { 
   STBTT_MAC_EID_ROMAN        =0,   STBTT_MAC_EID_ARABIC       =4,
   STBTT_MAC_EID_JAPANESE     =1,   STBTT_MAC_EID_HEBREW       =5,
   STBTT_MAC_EID_CHINESE_TRAD =2,   STBTT_MAC_EID_GREEK        =6,
   STBTT_MAC_EID_KOREAN       =3,   STBTT_MAC_EID_RUSSIAN      =7
};
enum { 
       
   STBTT_MS_LANG_ENGLISH     =0x0409,   STBTT_MS_LANG_ITALIAN     =0x0410,
   STBTT_MS_LANG_CHINESE     =0x0804,   STBTT_MS_LANG_JAPANESE    =0x0411,
   STBTT_MS_LANG_DUTCH       =0x0413,   STBTT_MS_LANG_KOREAN      =0x0412,
   STBTT_MS_LANG_FRENCH      =0x040c,   STBTT_MS_LANG_RUSSIAN     =0x0419,
   STBTT_MS_LANG_GERMAN      =0x0407,   STBTT_MS_LANG_SPANISH     =0x0409,
   STBTT_MS_LANG_HEBREW      =0x040d,   STBTT_MS_LANG_SWEDISH     =0x041D
};
enum { 
   STBTT_MAC_LANG_ENGLISH      =0 ,   STBTT_MAC_LANG_JAPANESE     =11,
   STBTT_MAC_LANG_ARABIC       =12,   STBTT_MAC_LANG_KOREAN       =23,
   STBTT_MAC_LANG_DUTCH        =4 ,   STBTT_MAC_LANG_RUSSIAN      =32,
   STBTT_MAC_LANG_FRENCH       =1 ,   STBTT_MAC_LANG_SPANISH      =6 ,
   STBTT_MAC_LANG_GERMAN       =2 ,   STBTT_MAC_LANG_SWEDISH      =5 ,
   STBTT_MAC_LANG_HEBREW       =10,   STBTT_MAC_LANG_CHINESE_SIMPLIFIED =33,
   STBTT_MAC_LANG_ITALIAN      =3 ,   STBTT_MAC_LANG_CHINESE_TRAD =19
};


#define STBTT_MAX_OVERSAMPLE   8
typedef px_int PX_tt_test_oversample_pow2[(STBTT_MAX_OVERSAMPLE & (STBTT_MAX_OVERSAMPLE-1)) == 0 ? 1 : -1];
#define STBTT_RASTERIZER_VERSION 2

px_byte PX_tt_buf_get8(PX_tt_buf *b)
{
   if (b->cursor >= b->size)
      return 0;
   return b->data[b->cursor++];
}
px_byte PX_tt_buf_peek8(PX_tt_buf *b)
{
   if (b->cursor >= b->size)
      return 0;
   return b->data[b->cursor];
}
px_void PX_tt_buf_seek(PX_tt_buf *b, px_int o)
{
   STBTT_assert(!(o > b->size || o < 0));
   b->cursor = (o > b->size || o < 0) ? b->size : o;
}
px_void PX_tt_buf_skip(PX_tt_buf *b, px_int o)
{
   PX_tt_buf_seek(b, b->cursor + o);
}
px_uint PX_tt_buf_get(PX_tt_buf *b, px_int n)
{
   px_uint v = 0;
   px_int i;
   STBTT_assert(n >= 1 && n <= 4);
   for (i = 0; i < n; i++)
      v = (v << 8) | PX_tt_buf_get8(b);
   return v;
}
PX_tt_buf PX_tt_new_buf(const px_void *p, px_uint size)
{
   PX_tt_buf r;
   STBTT_assert(size < 0x40000000);
   r.data = (px_byte*) p;
   r.size = (px_int) size;
   r.cursor = 0;
   return r;
}
#define PX_tt_buf_get16(b)  PX_tt_buf_get((b), 2)
#define PX_tt_buf_get32(b)  PX_tt_buf_get((b), 4)
PX_tt_buf PX_tt_buf_range(const PX_tt_buf *b, px_int o, px_int s)
{
   PX_tt_buf r = PX_tt_new_buf(PX_NULL, 0);
   if (o < 0 || s < 0 || o > b->size || s > b->size - o) return r;
   r.data = b->data + o;
   r.size = s;
   return r;
}
PX_tt_buf PX_tt_cff_get_index(PX_tt_buf *b)
{
   px_int count, start, offsize;
   start = b->cursor;
   count = PX_tt_buf_get16(b);
   if (count) {
      offsize = PX_tt_buf_get8(b);
      STBTT_assert(offsize >= 1 && offsize <= 4);
      PX_tt_buf_skip(b, offsize * count);
      PX_tt_buf_skip(b, PX_tt_buf_get(b, offsize) - 1);
   }
   return PX_tt_buf_range(b, start, b->cursor - start);
}
px_uint PX_tt_cff_int(PX_tt_buf *b)
{
   px_int b0 = PX_tt_buf_get8(b);
   if (b0 >= 32 && b0 <= 246)       return b0 - 139;
   else if (b0 >= 247 && b0 <= 250) return (b0 - 247)*256 + PX_tt_buf_get8(b) + 108;
   else if (b0 >= 251 && b0 <= 254) return -(b0 - 251)*256 - PX_tt_buf_get8(b) - 108;
   else if (b0 == 28)               return PX_tt_buf_get16(b);
   else if (b0 == 29)               return PX_tt_buf_get32(b);
   STBTT_assert(0);
   return 0;
}
px_void PX_tt_cff_skip_operand(PX_tt_buf *b) {
   px_int v, b0 = PX_tt_buf_peek8(b);
   STBTT_assert(b0 >= 28);
   if (b0 == 30) {
      PX_tt_buf_skip(b, 1);
      while (b->cursor < b->size) {
         v = PX_tt_buf_get8(b);
         if ((v & 0xF) == 0xF || (v >> 4) == 0xF)
            break;
      }
   } else {
      PX_tt_cff_int(b);
   }
}
PX_tt_buf PX_tt_dict_get(PX_tt_buf *b, px_int key)
{
   PX_tt_buf_seek(b, 0);
   while (b->cursor < b->size) {
      px_int start = b->cursor, end, op;
      while (PX_tt_buf_peek8(b) >= 28)
         PX_tt_cff_skip_operand(b);
      end = b->cursor;
      op = PX_tt_buf_get8(b);
      if (op == 12)  op = PX_tt_buf_get8(b) | 0x100;
      if (op == key) return PX_tt_buf_range(b, start, end-start);
   }
   return PX_tt_buf_range(b, 0, 0);
}
px_void PX_tt_dict_get_ints(PX_tt_buf *b, px_int key, px_int outcount, px_uint *out)
{
   px_int i;
   PX_tt_buf operands = PX_tt_dict_get(b, key);
   for (i = 0; i < outcount && operands.cursor < operands.size; i++)
      out[i] = PX_tt_cff_int(&operands);
}
px_int PX_tt_cff_index_count(PX_tt_buf *b)
{
   PX_tt_buf_seek(b, 0);
   return PX_tt_buf_get16(b);
}
PX_tt_buf PX_tt_cff_index_get(PX_tt_buf b, px_int i)
{
   px_int count, offsize, start, end;
   PX_tt_buf_seek(&b, 0);
   count = PX_tt_buf_get16(&b);
   offsize = PX_tt_buf_get8(&b);
   STBTT_assert(i >= 0 && i < count);
   STBTT_assert(offsize >= 1 && offsize <= 4);
   PX_tt_buf_skip(&b, i*offsize);
   start = PX_tt_buf_get(&b, offsize);
   end = PX_tt_buf_get(&b, offsize);
   return PX_tt_buf_range(&b, 2+(count+1)*offsize+start, end - start);
}
#define ttBYTE(p)     (* (px_byte *) (p))
#define ttCHAR(p)     (* (px_char *) (p))
#define ttFixed(p)    ttLONG(p)
px_ushort ttUSHORT(px_byte *p) { return p[0]*256 + p[1]; }
px_short ttSHORT(px_byte *p)   { return p[0]*256 + p[1]; }
px_uint ttULONG(px_byte *p)  { return (p[0]<<24) + (p[1]<<16) + (p[2]<<8) + p[3]; }
px_int32 ttLONG(px_byte *p)    { return (p[0]<<24) + (p[1]<<16) + (p[2]<<8) + p[3]; }
#define PX_tttag4(p,c0,c1,c2,c3) ((p)[0] == (c0) && (p)[1] == (c1) && (p)[2] == (c2) && (p)[3] == (c3))
#define PX_tttag(p,str)           PX_tttag4(p,str[0],str[1],str[2],str[3])
px_int PX_tt_isfont(px_byte *font)
{
   
   if (PX_tttag4(font, '1',0,0,0))  return 1; 
   if (PX_tttag(font, "typ1"))   return 1; 
   if (PX_tttag(font, "OTTO"))   return 1; 
   if (PX_tttag4(font, 0,1,0,0)) return 1; 
   if (PX_tttag(font, "true"))   return 1; 
   return 0;
}
px_uint PX_tt_find_table(px_byte *data, px_uint fontstart, const char *tag)
{
   px_int32 num_tables = ttUSHORT(data+fontstart+4);
   px_uint tabledir = fontstart + 12;
   px_int32 i;
   for (i=0; i < num_tables; ++i) {
      px_uint loc = tabledir + 16*i;
      if (PX_tttag(data+loc+0, tag))
         return ttULONG(data+loc+8);
   }
   return 0;
}
px_int PX_ttGetFontOffsetForIndex_internal(px_byte *font_collection, px_int index)
{
   
   if (PX_tt_isfont(font_collection))
      return index == 0 ? 0 : -1;
   
   if (PX_tttag(font_collection, "ttcf")) {
      
      if (ttULONG(font_collection+4) == 0x00010000 || ttULONG(font_collection+4) == 0x00020000) {
         px_int32 n = ttLONG(font_collection+8);
         if (index >= n)
            return -1;
         return ttULONG(font_collection+12+index*4);
      }
   }
   return -1;
}
px_int PX_ttGetNumberOfFonts_internal(px_byte *font_collection)
{
   
   if (PX_tt_isfont(font_collection))
      return 1;
   
   if (PX_tttag(font_collection, "ttcf")) {
      
      if (ttULONG(font_collection+4) == 0x00010000 || ttULONG(font_collection+4) == 0x00020000) {
         return ttLONG(font_collection+8);
      }
   }
   return 0;
}
PX_tt_buf PX_tt_get_subrs(PX_tt_buf cff, PX_tt_buf fontdict)
{
   px_uint subrsoff = 0, private_loc[2] = { 0, 0 };
   PX_tt_buf pdict;
   PX_tt_dict_get_ints(&fontdict, 18, 2, private_loc);
   if (!private_loc[1] || !private_loc[0]) return PX_tt_new_buf(PX_NULL, 0);
   pdict = PX_tt_buf_range(&cff, private_loc[1], private_loc[0]);
   PX_tt_dict_get_ints(&pdict, 19, 1, &subrsoff);
   if (!subrsoff) return PX_tt_new_buf(PX_NULL, 0);
   PX_tt_buf_seek(&cff, private_loc[1]+subrsoff);
   return PX_tt_cff_get_index(&cff);
}
px_int PX_tt_get_svg(PX_ttfontinfo *info)
{
   px_uint t;
   if (info->svg < 0) {
      t = PX_tt_find_table(info->data, info->fontstart, "SVG ");
      if (t) {
         px_uint offset = ttULONG(info->data + t + 2);
         info->svg = t + offset;
      } else {
         info->svg = 0;
      }
   }
   return info->svg;
}
px_int PX_ttInitFont_internal(px_memorypool* mp, PX_ttfontinfo *info, px_byte *data, px_int fontstart)
{
   px_uint cmap, t;
   px_int32 i,numTables;
   info->mp = mp;
   info->data = data;
   info->fontstart = fontstart;
   info->cff.cursor=0;
   info->cff.size=0;
   info->cff.data = 0;
   cmap = PX_tt_find_table(data, fontstart, "cmap");       
   info->loca = PX_tt_find_table(data, fontstart, "loca"); 
   info->head = PX_tt_find_table(data, fontstart, "head"); 
   info->glyf = PX_tt_find_table(data, fontstart, "glyf"); 
   info->hhea = PX_tt_find_table(data, fontstart, "hhea"); 
   info->hmtx = PX_tt_find_table(data, fontstart, "hmtx"); 
   info->kern = PX_tt_find_table(data, fontstart, "kern"); 
   info->gpos = PX_tt_find_table(data, fontstart, "GPOS"); 
   if (!cmap || !info->head || !info->hhea || !info->hmtx)
      return 0;
   if (info->glyf) {
      
      if (!info->loca) return 0;
   } else {
      
      PX_tt_buf b, topdict, topdictidx;
      px_uint cstype = 2, charstrings = 0, fdarrayoff = 0, fdselectoff = 0;
      px_uint cff;
      cff = PX_tt_find_table(data, fontstart, "CFF ");
      if (!cff) return 0;
      info->fontdicts = PX_tt_new_buf(PX_NULL, 0);
      info->fdselect = PX_tt_new_buf(PX_NULL, 0);
      
      info->cff = PX_tt_new_buf(data+cff, 512*1024*1024);
      b = info->cff;
      
      PX_tt_buf_skip(&b, 2);
      PX_tt_buf_seek(&b, PX_tt_buf_get8(&b)); 
      
      
      PX_tt_cff_get_index(&b);  
      topdictidx = PX_tt_cff_get_index(&b);
      topdict = PX_tt_cff_index_get(topdictidx, 0);
      PX_tt_cff_get_index(&b);  
      info->gsubrs = PX_tt_cff_get_index(&b);
      PX_tt_dict_get_ints(&topdict, 17, 1, &charstrings);
      PX_tt_dict_get_ints(&topdict, 0x100 | 6, 1, &cstype);
      PX_tt_dict_get_ints(&topdict, 0x100 | 36, 1, &fdarrayoff);
      PX_tt_dict_get_ints(&topdict, 0x100 | 37, 1, &fdselectoff);
      info->subrs = PX_tt_get_subrs(b, topdict);
      
      if (cstype != 2) return 0;
      if (charstrings == 0) return 0;
      if (fdarrayoff) {
         
         if (!fdselectoff) return 0;
         PX_tt_buf_seek(&b, fdarrayoff);
         info->fontdicts = PX_tt_cff_get_index(&b);
         info->fdselect = PX_tt_buf_range(&b, fdselectoff, b.size-fdselectoff);
      }
      PX_tt_buf_seek(&b, charstrings);
      info->charstrings = PX_tt_cff_get_index(&b);
   }
   t = PX_tt_find_table(data, fontstart, "maxp");
   if (t)
      info->numGlyphs = ttUSHORT(data+t+4);
   else
      info->numGlyphs = 0xffff;
   info->svg = -1;
   
   
   
   numTables = ttUSHORT(data + cmap + 2);
   info->index_map = 0;
   for (i=0; i < numTables; ++i) {
      px_uint encoding_record = cmap + 4 + 8 * i;
      
      switch(ttUSHORT(data+encoding_record)) {
         case STBTT_PLATFORM_ID_MICROSOFT:
            switch (ttUSHORT(data+encoding_record+2)) {
               case STBTT_MS_EID_UNICODE_BMP:
               case STBTT_MS_EID_UNICODE_FULL:
                  
                  info->index_map = cmap + ttULONG(data+encoding_record+4);
                  break;
            }
            break;
        case STBTT_PLATFORM_ID_UNICODE:
            
            
            info->index_map = cmap + ttULONG(data+encoding_record+4);
            break;
      }
   }
   if (info->index_map == 0)
      return 0;
   info->indexToLocFormat = ttUSHORT(data+info->head + 50);
   return 1;
}
px_int PX_ttFindGlyphIndex(const PX_ttfontinfo *info, px_int unicode_codepoint)
{
   px_byte *data = info->data;
   px_uint index_map = info->index_map;
   px_ushort format = ttUSHORT(data + index_map + 0);
   if (format == 0) { 
      px_int32 bytes = ttUSHORT(data + index_map + 2);
      if (unicode_codepoint < bytes-6)
         return ttBYTE(data + index_map + 6 + unicode_codepoint);
      return 0;
   } else if (format == 6) {
      px_uint first = ttUSHORT(data + index_map + 6);
      px_uint count = ttUSHORT(data + index_map + 8);
      if ((px_uint) unicode_codepoint >= first && (px_uint) unicode_codepoint < first+count)
         return ttUSHORT(data + index_map + 10 + (unicode_codepoint - first)*2);
      return 0;
   } else if (format == 2) {
      STBTT_assert(0); 
      return 0;
   } else if (format == 4) { 
      px_ushort segcount = ttUSHORT(data+index_map+6) >> 1;
      px_ushort searchRange = ttUSHORT(data+index_map+8) >> 1;
      px_ushort entrySelector = ttUSHORT(data+index_map+10);
      px_ushort rangeShift = ttUSHORT(data+index_map+12) >> 1;
      
      px_uint endCount = index_map + 14;
      px_uint search = endCount;
      if (unicode_codepoint > 0xffff)
         return 0;
      
      
      if (unicode_codepoint >= ttUSHORT(data + search + rangeShift*2))
         search += rangeShift*2;
      
      search -= 2;
      while (entrySelector) {
         px_ushort end;
         searchRange >>= 1;
         end = ttUSHORT(data + search + searchRange*2);
         if (unicode_codepoint > end)
            search += searchRange*2;
         --entrySelector;
      }
      search += 2;
      {
         px_ushort offset, start, last;
         px_ushort item = (px_ushort) ((search - endCount) >> 1);
         start = ttUSHORT(data + index_map + 14 + segcount*2 + 2 + 2*item);
         last = ttUSHORT(data + endCount + 2*item);
         if (unicode_codepoint < start || unicode_codepoint > last)
            return 0;
         offset = ttUSHORT(data + index_map + 14 + segcount*6 + 2 + 2*item);
         if (offset == 0)
            return (px_ushort) (unicode_codepoint + ttSHORT(data + index_map + 14 + segcount*4 + 2 + 2*item));
         return ttUSHORT(data + offset + (unicode_codepoint-start)*2 + index_map + 14 + segcount*6 + 2 + 2*item);
      }
   } else if (format == 12 || format == 13) {
      px_uint ngroups = ttULONG(data+index_map+12);
      px_int32 low,high;
      low = 0; high = (px_int32)ngroups;
      
      while (low < high) {
         px_int32 mid = low + ((high-low) >> 1); 
         px_uint start_char = ttULONG(data+index_map+16+mid*12);
         px_uint end_char = ttULONG(data+index_map+16+mid*12+4);
         if ((px_uint) unicode_codepoint < start_char)
            high = mid;
         else if ((px_uint) unicode_codepoint > end_char)
            low = mid+1;
         else {
            px_uint start_glyph = ttULONG(data+index_map+16+mid*12+8);
            if (format == 12)
               return start_glyph + unicode_codepoint-start_char;
            else 
               return start_glyph;
         }
      }
      return 0; 
   }
   
   STBTT_assert(0);
   return 0;
}
px_int PX_ttGetCodepointShape(const PX_ttfontinfo *info, px_int unicode_codepoint, PX_ttvertex **vertices)
{
   return PX_ttGetGlyphShape(info, PX_ttFindGlyphIndex(info, unicode_codepoint), vertices);
}
px_void PX_ttsetvertex(PX_ttvertex *v, px_byte type, px_int32 x, px_int32 y, px_int32 cx, px_int32 cy)
{
   v->type = type;
   v->x = (px_short) x;
   v->y = (px_short) y;
   v->cx = (px_short) cx;
   v->cy = (px_short) cy;
}
px_int PX_tt_GetGlyfOffset(const PX_ttfontinfo *info, px_int glyph_index)
{
   px_int g1,g2;
   STBTT_assert(!info->cff.size);
   if (glyph_index >= info->numGlyphs) return -1; 
   if (info->indexToLocFormat >= 2)    return -1; 
   if (info->indexToLocFormat == 0) {
      g1 = info->glyf + ttUSHORT(info->data + info->loca + glyph_index * 2) * 2;
      g2 = info->glyf + ttUSHORT(info->data + info->loca + glyph_index * 2 + 2) * 2;
   } else {
      g1 = info->glyf + ttULONG (info->data + info->loca + glyph_index * 4);
      g2 = info->glyf + ttULONG (info->data + info->loca + glyph_index * 4 + 4);
   }
   return g1==g2 ? -1 : g1; 
}
px_int PX_tt_GetGlyphInfoT2(const PX_ttfontinfo *info, px_int glyph_index, px_int *x0, px_int *y0, px_int *x1, px_int *y1);
px_int PX_ttGetGlyphBox(const PX_ttfontinfo *info, px_int glyph_index, px_int *x0, px_int *y0, px_int *x1, px_int *y1)
{
   if (info->cff.size) {
      PX_tt_GetGlyphInfoT2(info, glyph_index, x0, y0, x1, y1);
   } else {
      px_int g = PX_tt_GetGlyfOffset(info, glyph_index);
      if (g < 0) return 0;
      if (x0) *x0 = ttSHORT(info->data + g + 2);
      if (y0) *y0 = ttSHORT(info->data + g + 4);
      if (x1) *x1 = ttSHORT(info->data + g + 6);
      if (y1) *y1 = ttSHORT(info->data + g + 8);
   }
   return 1;
}
px_int PX_ttGetCodepointBox(const PX_ttfontinfo *info, px_int codepoint, px_int *x0, px_int *y0, px_int *x1, px_int *y1)
{
   return PX_ttGetGlyphBox(info, PX_ttFindGlyphIndex(info,codepoint), x0,y0,x1,y1);
}
px_int PX_ttIsGlyphEmpty(const PX_ttfontinfo *info, px_int glyph_index)
{
   px_short numberOfContours;
   px_int g;
   if (info->cff.size)
      return PX_tt_GetGlyphInfoT2(info, glyph_index, PX_NULL, PX_NULL, PX_NULL, PX_NULL) == 0;
   g = PX_tt_GetGlyfOffset(info, glyph_index);
   if (g < 0) return 1;
   numberOfContours = ttSHORT(info->data + g);
   return numberOfContours == 0;
}
px_int PX_tt_close_shape(PX_ttvertex *vertices, px_int num_vertices, px_int was_off, px_int start_off,
    px_int32 sx, px_int32 sy, px_int32 scx, px_int32 scy, px_int32 cx, px_int32 cy)
{
   if (start_off) {
      if (was_off)
         PX_ttsetvertex(&vertices[num_vertices++], STBTT_vcurve, (cx+scx)>>1, (cy+scy)>>1, cx,cy);
      PX_ttsetvertex(&vertices[num_vertices++], STBTT_vcurve, sx,sy,scx,scy);
   } else {
      if (was_off)
         PX_ttsetvertex(&vertices[num_vertices++], STBTT_vcurve,sx,sy,cx,cy);
      else
         PX_ttsetvertex(&vertices[num_vertices++], STBTT_vline,sx,sy,0,0);
   }
   return num_vertices;
}
px_int PX_tt_GetGlyphShapeTT(const PX_ttfontinfo *info, px_int glyph_index, PX_ttvertex **pvertices)
{
   px_short numberOfContours;
   px_byte *endPtsOfContours;
   px_byte *data = info->data;
   PX_ttvertex *vertices=0;
   px_int num_vertices=0;
   px_int g = PX_tt_GetGlyfOffset(info, glyph_index);
   *pvertices = PX_NULL;
   if (g < 0) return 0;
   numberOfContours = ttSHORT(data + g);
   if (numberOfContours > 0) {
      px_byte flags=0,flagcount;
      px_int32 ins, i,j=0,m,n, next_move, was_off=0, off, start_off=0;
      px_int32 x,y,cx,cy,sx,sy, scx,scy;
      px_byte *points;
      endPtsOfContours = (data + g + 10);
      ins = ttUSHORT(data + g + 10 + numberOfContours * 2);
      points = data + g + 10 + numberOfContours * 2 + 2 + ins;
      n = 1+ttUSHORT(endPtsOfContours + numberOfContours*2-2);
      m = n + 2*numberOfContours;  
      vertices = (PX_ttvertex *)MP_Malloc(info->mp, m * sizeof(vertices[0]));
      if (vertices == 0)
         return 0;
      next_move = 0;
      flagcount=0;
      
      
      
      off = m - n; 
      
      for (i=0; i < n; ++i) {
         if (flagcount == 0) {
            flags = *points++;
            if (flags & 8)
               flagcount = *points++;
         } else
            --flagcount;
         vertices[off+i].type = flags;
      }
      
      x=0;
      for (i=0; i < n; ++i) {
         flags = vertices[off+i].type;
         if (flags & 2) {
            px_short dx = *points++;
            x += (flags & 16) ? dx : -dx; 
         } else {
            if (!(flags & 16)) {
               x = x + (px_short) (points[0]*256 + points[1]);
               points += 2;
            }
         }
         vertices[off+i].x = (px_short) x;
      }
      
      y=0;
      for (i=0; i < n; ++i) {
         flags = vertices[off+i].type;
         if (flags & 4) {
            px_short dy = *points++;
            y += (flags & 32) ? dy : -dy; 
         } else {
            if (!(flags & 32)) {
               y = y + (px_short) (points[0]*256 + points[1]);
               points += 2;
            }
         }
         vertices[off+i].y = (px_short) y;
      }
      
      num_vertices=0;
      sx = sy = cx = cy = scx = scy = 0;
      for (i=0; i < n; ++i) {
         flags = vertices[off+i].type;
         x     = (px_short) vertices[off+i].x;
         y     = (px_short) vertices[off+i].y;
         if (next_move == i) {
            if (i != 0)
               num_vertices = PX_tt_close_shape(vertices, num_vertices, was_off, start_off, sx,sy,scx,scy,cx,cy);
            
            start_off = !(flags & 1);
            if (start_off) {
               
               
               scx = x;
               scy = y;
               if (!(vertices[off+i+1].type & 1)) {
                  
                  sx = (x + (px_int32) vertices[off+i+1].x) >> 1;
                  sy = (y + (px_int32) vertices[off+i+1].y) >> 1;
               } else {
                  
                  sx = (px_int32) vertices[off+i+1].x;
                  sy = (px_int32) vertices[off+i+1].y;
                  ++i; 
               }
            } else {
               sx = x;
               sy = y;
            }
            PX_ttsetvertex(&vertices[num_vertices++], STBTT_vmove,sx,sy,0,0);
            was_off = 0;
            next_move = 1 + ttUSHORT(endPtsOfContours+j*2);
            ++j;
         } else {
            if (!(flags & 1)) { 
               if (was_off) 
                  PX_ttsetvertex(&vertices[num_vertices++], STBTT_vcurve, (cx+x)>>1, (cy+y)>>1, cx, cy);
               cx = x;
               cy = y;
               was_off = 1;
            } else {
               if (was_off)
                  PX_ttsetvertex(&vertices[num_vertices++], STBTT_vcurve, x,y, cx, cy);
               else
                  PX_ttsetvertex(&vertices[num_vertices++], STBTT_vline, x,y,0,0);
               was_off = 0;
            }
         }
      }
      num_vertices = PX_tt_close_shape(vertices, num_vertices, was_off, start_off, sx,sy,scx,scy,cx,cy);
   } else if (numberOfContours < 0) {
      
      px_int more = 1;
      px_byte *comp = data + g + 10;
      num_vertices = 0;
      vertices = 0;
      while (more) {
         px_ushort flags, gidx;
         px_int comp_num_verts = 0, i;
         PX_ttvertex *comp_verts = 0, *tmp = 0;
         px_float mtx[6] = {1,0,0,1,0,0}, m, n;
         flags = ttSHORT(comp); comp+=2;
         gidx = ttSHORT(comp); comp+=2;
         if (flags & 2) { 
            if (flags & 1) { 
               mtx[4] = ttSHORT(comp); comp+=2;
               mtx[5] = ttSHORT(comp); comp+=2;
            } else {
               mtx[4] = ttCHAR(comp); comp+=1;
               mtx[5] = ttCHAR(comp); comp+=1;
            }
         }
         else {
            
            STBTT_assert(0);
         }
         if (flags & (1<<3)) { 
            mtx[0] = mtx[3] = ttSHORT(comp)/16384.0f; comp+=2;
            mtx[1] = mtx[2] = 0;
         } else if (flags & (1<<6)) { 
            mtx[0] = ttSHORT(comp)/16384.0f; comp+=2;
            mtx[1] = mtx[2] = 0;
            mtx[3] = ttSHORT(comp)/16384.0f; comp+=2;
         } else if (flags & (1<<7)) { 
            mtx[0] = ttSHORT(comp)/16384.0f; comp+=2;
            mtx[1] = ttSHORT(comp)/16384.0f; comp+=2;
            mtx[2] = ttSHORT(comp)/16384.0f; comp+=2;
            mtx[3] = ttSHORT(comp)/16384.0f; comp+=2;
         }
         
         m = (px_float) STBTT_sqrt(mtx[0]*mtx[0] + mtx[1]*mtx[1]);
         n = (px_float) STBTT_sqrt(mtx[2]*mtx[2] + mtx[3]*mtx[3]);
         
         comp_num_verts = PX_ttGetGlyphShape(info, gidx, &comp_verts);
         if (comp_num_verts > 0) {
            
            for (i = 0; i < comp_num_verts; ++i) {
               PX_ttvertex* v = &comp_verts[i];
               PX_ttvertex_type x,y;
               x=v->x; y=v->y;
               v->x = (PX_ttvertex_type)(m * (mtx[0]*x + mtx[2]*y + mtx[4]));
               v->y = (PX_ttvertex_type)(n * (mtx[1]*x + mtx[3]*y + mtx[5]));
               x=v->cx; y=v->cy;
               v->cx = (PX_ttvertex_type)(m * (mtx[0]*x + mtx[2]*y + mtx[4]));
               v->cy = (PX_ttvertex_type)(n * (mtx[1]*x + mtx[3]*y + mtx[5]));
            }
            
            tmp = (PX_ttvertex*)MP_Malloc(info->mp,(num_vertices+comp_num_verts)*sizeof(PX_ttvertex));
            if (!tmp) {
               if (vertices) MP_Free(info->mp,vertices);
               if (comp_verts) MP_Free(info->mp, comp_verts);
               return 0;
            }
            if (num_vertices > 0 && vertices) STBTT_memcpy(tmp, vertices, num_vertices*sizeof(PX_ttvertex));
            STBTT_memcpy(tmp+num_vertices, comp_verts, comp_num_verts*sizeof(PX_ttvertex));
            if (vertices) MP_Free(info->mp, vertices);
            vertices = tmp;
            MP_Free(info->mp, comp_verts);
            num_vertices += comp_num_verts;
         }
         
         more = flags & (1<<5);
      }
   } else {
      
   }
   *pvertices = vertices;
   return num_vertices;
}
typedef struct
{
   px_int bounds;
   px_int started;
   px_float first_x, first_y;
   px_float x, y;
   px_int32 min_x, max_x, min_y, max_y;
   PX_ttvertex *pvertices;
   px_int num_vertices;
} PX_tt_csctx;
#define STBTT__CSCTX_INIT(bounds) {bounds,0, 0,0, 0,0, 0,0,0,0, PX_NULL, 0}
px_void PX_tt_track_vertex(PX_tt_csctx *c, px_int32 x, px_int32 y)
{
   if (x > c->max_x || !c->started) c->max_x = x;
   if (y > c->max_y || !c->started) c->max_y = y;
   if (x < c->min_x || !c->started) c->min_x = x;
   if (y < c->min_y || !c->started) c->min_y = y;
   c->started = 1;
}
px_void PX_tt_csctx_v(PX_tt_csctx *c, px_byte type, px_int32 x, px_int32 y, px_int32 cx, px_int32 cy, px_int32 cx1, px_int32 cy1)
{
   if (c->bounds) {
      PX_tt_track_vertex(c, x, y);
      if (type == STBTT_vcubic) {
         PX_tt_track_vertex(c, cx, cy);
         PX_tt_track_vertex(c, cx1, cy1);
      }
   } else {
      PX_ttsetvertex(&c->pvertices[c->num_vertices], type, x, y, cx, cy);
      c->pvertices[c->num_vertices].cx1 = (px_short) cx1;
      c->pvertices[c->num_vertices].cy1 = (px_short) cy1;
   }
   c->num_vertices++;
}
px_void PX_tt_csctx_close_shape(PX_tt_csctx *ctx)
{
   if (ctx->first_x != ctx->x || ctx->first_y != ctx->y)
      PX_tt_csctx_v(ctx, STBTT_vline, (px_int)ctx->first_x, (px_int)ctx->first_y, 0, 0, 0, 0);
}
px_void PX_tt_csctx_rmove_to(PX_tt_csctx *ctx, px_float dx, px_float dy)
{
   PX_tt_csctx_close_shape(ctx);
   ctx->first_x = ctx->x = ctx->x + dx;
   ctx->first_y = ctx->y = ctx->y + dy;
   PX_tt_csctx_v(ctx, STBTT_vmove, (px_int)ctx->x, (px_int)ctx->y, 0, 0, 0, 0);
}
px_void PX_tt_csctx_rline_to(PX_tt_csctx *ctx, px_float dx, px_float dy)
{
   ctx->x += dx;
   ctx->y += dy;
   PX_tt_csctx_v(ctx, STBTT_vline, (px_int)ctx->x, (px_int)ctx->y, 0, 0, 0, 0);
}
px_void PX_tt_csctx_rccurve_to(PX_tt_csctx *ctx, px_float dx1, px_float dy1, px_float dx2, px_float dy2, px_float dx3, px_float dy3)
{
   px_float cx1 = ctx->x + dx1;
   px_float cy1 = ctx->y + dy1;
   px_float cx2 = cx1 + dx2;
   px_float cy2 = cy1 + dy2;
   ctx->x = cx2 + dx3;
   ctx->y = cy2 + dy3;
   PX_tt_csctx_v(ctx, STBTT_vcubic, (px_int)ctx->x, (px_int)ctx->y, (px_int)cx1, (px_int)cy1, (px_int)cx2, (px_int)cy2);
}
PX_tt_buf PX_tt_get_subr(PX_tt_buf idx, px_int n)
{
   px_int count = PX_tt_cff_index_count(&idx);
   px_int bias = 107;
   if (count >= 33900)
      bias = 32768;
   else if (count >= 1240)
      bias = 1131;
   n += bias;
   if (n < 0 || n >= count)
      return PX_tt_new_buf(PX_NULL, 0);
   return PX_tt_cff_index_get(idx, n);
}
PX_tt_buf PX_tt_cid_get_glyph_subrs(const PX_ttfontinfo *info, px_int glyph_index)
{
   PX_tt_buf fdselect = info->fdselect;
   px_int nranges, start, end, v, fmt, fdselector = -1, i;
   PX_tt_buf_seek(&fdselect, 0);
   fmt = PX_tt_buf_get8(&fdselect);
   if (fmt == 0) {
      
      PX_tt_buf_skip(&fdselect, glyph_index);
      fdselector = PX_tt_buf_get8(&fdselect);
   } else if (fmt == 3) {
      nranges = PX_tt_buf_get16(&fdselect);
      start = PX_tt_buf_get16(&fdselect);
      for (i = 0; i < nranges; i++) {
         v = PX_tt_buf_get8(&fdselect);
         end = PX_tt_buf_get16(&fdselect);
         if (glyph_index >= start && glyph_index < end) {
            fdselector = v;
            break;
         }
         start = end;
      }
   }
   if (fdselector == -1) PX_tt_new_buf(PX_NULL, 0);
   return PX_tt_get_subrs(info->cff, PX_tt_cff_index_get(info->fontdicts, fdselector));
}
px_int PX_tt_run_charstring(const PX_ttfontinfo *info, px_int glyph_index, PX_tt_csctx *c)
{
   px_int in_header = 1, maskbits = 0, subr_stack_height = 0, sp = 0, v, i, b0;
   px_int has_subrs = 0, clear_stack;
   px_float s[48];
   PX_tt_buf subr_stack[10], subrs = info->subrs, b;
   px_float f;
#define STBTT__CSERR(s) (0)
   
   b = PX_tt_cff_index_get(info->charstrings, glyph_index);
   while (b.cursor < b.size) {
      i = 0;
      clear_stack = 1;
      b0 = PX_tt_buf_get8(&b);
      switch (b0) {
      
      case 0x13: 
      case 0x14: 
         if (in_header)
            maskbits += (sp / 2); 
         in_header = 0;
         PX_tt_buf_skip(&b, (maskbits + 7) / 8);
         break;
      case 0x01: 
      case 0x03: 
      case 0x12: 
      case 0x17: 
         maskbits += (sp / 2);
         break;
      case 0x15: 
         in_header = 0;
         if (sp < 2) return STBTT__CSERR("rmoveto stack");
         PX_tt_csctx_rmove_to(c, s[sp-2], s[sp-1]);
         break;
      case 0x04: 
         in_header = 0;
         if (sp < 1) return STBTT__CSERR("vmoveto stack");
         PX_tt_csctx_rmove_to(c, 0, s[sp-1]);
         break;
      case 0x16: 
         in_header = 0;
         if (sp < 1) return STBTT__CSERR("hmoveto stack");
         PX_tt_csctx_rmove_to(c, s[sp-1], 0);
         break;
      case 0x05: 
         if (sp < 2) return STBTT__CSERR("rlineto stack");
         for (; i + 1 < sp; i += 2)
            PX_tt_csctx_rline_to(c, s[i], s[i+1]);
         break;
      
      
      case 0x07: 
         if (sp < 1) return STBTT__CSERR("vlineto stack");
         goto vlineto;
      case 0x06: 
         if (sp < 1) return STBTT__CSERR("hlineto stack");
         for (;;) {
            if (i >= sp) break;
            PX_tt_csctx_rline_to(c, s[i], 0);
            i++;
      vlineto:
            if (i >= sp) break;
            PX_tt_csctx_rline_to(c, 0, s[i]);
            i++;
         }
         break;
      case 0x1F: 
         if (sp < 4) return STBTT__CSERR("hvcurveto stack");
         goto hvcurveto;
      case 0x1E: 
         if (sp < 4) return STBTT__CSERR("vhcurveto stack");
         for (;;) {
            if (i + 3 >= sp) break;
            PX_tt_csctx_rccurve_to(c, 0, s[i], s[i+1], s[i+2], s[i+3], (sp - i == 5) ? s[i + 4] : 0.0f);
            i += 4;
      hvcurveto:
            if (i + 3 >= sp) break;
            PX_tt_csctx_rccurve_to(c, s[i], 0, s[i+1], s[i+2], (sp - i == 5) ? s[i+4] : 0.0f, s[i+3]);
            i += 4;
         }
         break;
      case 0x08: 
         if (sp < 6) return STBTT__CSERR("rcurveline stack");
         for (; i + 5 < sp; i += 6)
            PX_tt_csctx_rccurve_to(c, s[i], s[i+1], s[i+2], s[i+3], s[i+4], s[i+5]);
         break;
      case 0x18: 
         if (sp < 8) return STBTT__CSERR("rcurveline stack");
         for (; i + 5 < sp - 2; i += 6)
            PX_tt_csctx_rccurve_to(c, s[i], s[i+1], s[i+2], s[i+3], s[i+4], s[i+5]);
         if (i + 1 >= sp) return STBTT__CSERR("rcurveline stack");
         PX_tt_csctx_rline_to(c, s[i], s[i+1]);
         break;
      case 0x19: 
         if (sp < 8) return STBTT__CSERR("rlinecurve stack");
         for (; i + 1 < sp - 6; i += 2)
            PX_tt_csctx_rline_to(c, s[i], s[i+1]);
         if (i + 5 >= sp) return STBTT__CSERR("rlinecurve stack");
         PX_tt_csctx_rccurve_to(c, s[i], s[i+1], s[i+2], s[i+3], s[i+4], s[i+5]);
         break;
      case 0x1A: 
      case 0x1B: 
         if (sp < 4) return STBTT__CSERR("(vv|hh)curveto stack");
         f = 0.0;
         if (sp & 1) { f = s[i]; i++; }
         for (; i + 3 < sp; i += 4) {
            if (b0 == 0x1B)
               PX_tt_csctx_rccurve_to(c, s[i], f, s[i+1], s[i+2], s[i+3], 0.0);
            else
               PX_tt_csctx_rccurve_to(c, f, s[i], s[i+1], s[i+2], 0.0, s[i+3]);
            f = 0.0;
         }
         break;
      case 0x0A: 
         if (!has_subrs) {
            if (info->fdselect.size)
               subrs = PX_tt_cid_get_glyph_subrs(info, glyph_index);
            has_subrs = 1;
         }
         
      case 0x1D: 
         if (sp < 1) return STBTT__CSERR("call(g|)subr stack");
         v = (px_int) s[--sp];
         if (subr_stack_height >= 10) return STBTT__CSERR("recursion limit");
         subr_stack[subr_stack_height++] = b;
         b = PX_tt_get_subr(b0 == 0x0A ? subrs : info->gsubrs, v);
         if (b.size == 0) return STBTT__CSERR("subr not found");
         b.cursor = 0;
         clear_stack = 0;
         break;
      case 0x0B: 
         if (subr_stack_height <= 0) return STBTT__CSERR("return outside subr");
         b = subr_stack[--subr_stack_height];
         clear_stack = 0;
         break;
      case 0x0E: 
         PX_tt_csctx_close_shape(c);
         return 1;
      case 0x0C: { 
         px_float dx1, dx2, dx3, dx4, dx5, dx6, dy1, dy2, dy3, dy4, dy5, dy6;
         px_float dx, dy;
         px_int b1 = PX_tt_buf_get8(&b);
         switch (b1) {
         
         
         case 0x22: 
            if (sp < 7) return STBTT__CSERR("hflex stack");
            dx1 = s[0];
            dx2 = s[1];
            dy2 = s[2];
            dx3 = s[3];
            dx4 = s[4];
            dx5 = s[5];
            dx6 = s[6];
            PX_tt_csctx_rccurve_to(c, dx1, 0, dx2, dy2, dx3, 0);
            PX_tt_csctx_rccurve_to(c, dx4, 0, dx5, -dy2, dx6, 0);
            break;
         case 0x23: 
            if (sp < 13) return STBTT__CSERR("flex stack");
            dx1 = s[0];
            dy1 = s[1];
            dx2 = s[2];
            dy2 = s[3];
            dx3 = s[4];
            dy3 = s[5];
            dx4 = s[6];
            dy4 = s[7];
            dx5 = s[8];
            dy5 = s[9];
            dx6 = s[10];
            dy6 = s[11];
            
            PX_tt_csctx_rccurve_to(c, dx1, dy1, dx2, dy2, dx3, dy3);
            PX_tt_csctx_rccurve_to(c, dx4, dy4, dx5, dy5, dx6, dy6);
            break;
         case 0x24: 
            if (sp < 9) return STBTT__CSERR("hflex1 stack");
            dx1 = s[0];
            dy1 = s[1];
            dx2 = s[2];
            dy2 = s[3];
            dx3 = s[4];
            dx4 = s[5];
            dx5 = s[6];
            dy5 = s[7];
            dx6 = s[8];
            PX_tt_csctx_rccurve_to(c, dx1, dy1, dx2, dy2, dx3, 0);
            PX_tt_csctx_rccurve_to(c, dx4, 0, dx5, dy5, dx6, -(dy1+dy2+dy5));
            break;
         case 0x25: 
            if (sp < 11) return STBTT__CSERR("flex1 stack");
            dx1 = s[0];
            dy1 = s[1];
            dx2 = s[2];
            dy2 = s[3];
            dx3 = s[4];
            dy3 = s[5];
            dx4 = s[6];
            dy4 = s[7];
            dx5 = s[8];
            dy5 = s[9];
            dx6 = dy6 = s[10];
            dx = dx1+dx2+dx3+dx4+dx5;
            dy = dy1+dy2+dy3+dy4+dy5;
            if (STBTT_fabs(dx) > STBTT_fabs(dy))
               dy6 = -dy;
            else
               dx6 = -dx;
            PX_tt_csctx_rccurve_to(c, dx1, dy1, dx2, dy2, dx3, dy3);
            PX_tt_csctx_rccurve_to(c, dx4, dy4, dx5, dy5, dx6, dy6);
            break;
         default:
            return STBTT__CSERR("unimplemented");
         }
      } break;
      default:
         if (b0 != 255 && b0 != 28 && b0 < 32)
            return STBTT__CSERR("reserved operator");
         
         if (b0 == 255) {
            f = (px_float)(px_int32)PX_tt_buf_get32(&b) / 0x10000;
         } else {
            PX_tt_buf_skip(&b, -1);
            f = (px_float)(px_short)PX_tt_cff_int(&b);
         }
         if (sp >= 48) return STBTT__CSERR("push stack overflow");
         s[sp++] = f;
         clear_stack = 0;
         break;
      }
      if (clear_stack) sp = 0;
   }
   return STBTT__CSERR("no endchar");
#undef STBTT__CSERR
}
px_int PX_tt_GetGlyphShapeT2(const PX_ttfontinfo *info, px_int glyph_index, PX_ttvertex **pvertices)
{
   
   PX_tt_csctx count_ctx = STBTT__CSCTX_INIT(1);
   PX_tt_csctx output_ctx = STBTT__CSCTX_INIT(0);
   if (PX_tt_run_charstring(info, glyph_index, &count_ctx)) {
      *pvertices = (PX_ttvertex*)MP_Malloc(info->mp,count_ctx.num_vertices*sizeof(PX_ttvertex));
      output_ctx.pvertices = *pvertices;
      if (PX_tt_run_charstring(info, glyph_index, &output_ctx)) {
         STBTT_assert(output_ctx.num_vertices == count_ctx.num_vertices);
         return output_ctx.num_vertices;
      }
   }
   *pvertices = PX_NULL;
   return 0;
}
px_int PX_tt_GetGlyphInfoT2(const PX_ttfontinfo *info, px_int glyph_index, px_int *x0, px_int *y0, px_int *x1, px_int *y1)
{
   PX_tt_csctx c = STBTT__CSCTX_INIT(1);
   px_int r = PX_tt_run_charstring(info, glyph_index, &c);
   if (x0)  *x0 = r ? c.min_x : 0;
   if (y0)  *y0 = r ? c.min_y : 0;
   if (x1)  *x1 = r ? c.max_x : 0;
   if (y1)  *y1 = r ? c.max_y : 0;
   return r ? c.num_vertices : 0;
}
px_int PX_ttGetGlyphShape(const PX_ttfontinfo *info, px_int glyph_index, PX_ttvertex **pvertices)
{
   if (!info->cff.size)
      return PX_tt_GetGlyphShapeTT(info, glyph_index, pvertices);
   else
      return PX_tt_GetGlyphShapeT2(info, glyph_index, pvertices);
}
px_void PX_ttGetGlyphHMetrics(const PX_ttfontinfo *info, px_int glyph_index, px_int *advanceWidth, px_int *leftSideBearing)
{
   px_ushort numOfLongHorMetrics = ttUSHORT(info->data+info->hhea + 34);
   if (glyph_index < numOfLongHorMetrics) {
      if (advanceWidth)     *advanceWidth    = ttSHORT(info->data + info->hmtx + 4*glyph_index);
      if (leftSideBearing)  *leftSideBearing = ttSHORT(info->data + info->hmtx + 4*glyph_index + 2);
   } else {
      if (advanceWidth)     *advanceWidth    = ttSHORT(info->data + info->hmtx + 4*(numOfLongHorMetrics-1));
      if (leftSideBearing)  *leftSideBearing = ttSHORT(info->data + info->hmtx + 4*numOfLongHorMetrics + 2*(glyph_index - numOfLongHorMetrics));
   }
}
px_int  PX_ttGetKerningTableLength(const PX_ttfontinfo *info)
{
   px_byte *data = info->data + info->kern;
   
   if (!info->kern)
      return 0;
   if (ttUSHORT(data+2) < 1) 
      return 0;
   if (ttUSHORT(data+8) != 1) 
      return 0;
   return ttUSHORT(data+10);
}
px_int PX_ttGetKerningTable(const PX_ttfontinfo *info, PX_ttkerningentry* table, px_int table_length)
{
   px_byte *data = info->data + info->kern;
   px_int k, length;
   
   if (!info->kern)
      return 0;
   if (ttUSHORT(data+2) < 1) 
      return 0;
   if (ttUSHORT(data+8) != 1) 
      return 0;
   length = ttUSHORT(data+10);
   if (table_length < length)
      length = table_length;
   for (k = 0; k < length; k++)
   {
      table[k].glyph1 = ttUSHORT(data+18+(k*6));
      table[k].glyph2 = ttUSHORT(data+20+(k*6));
      table[k].advance = ttSHORT(data+22+(k*6));
   }
   return length;
}
px_int PX_tt_GetGlyphKernInfoAdvance(const PX_ttfontinfo *info, px_int glyph1, px_int glyph2)
{
   px_byte *data = info->data + info->kern;
   px_uint needle, straw;
   px_int l, r, m;
   
   if (!info->kern)
      return 0;
   if (ttUSHORT(data+2) < 1) 
      return 0;
   if (ttUSHORT(data+8) != 1) 
      return 0;
   l = 0;
   r = ttUSHORT(data+10) - 1;
   needle = glyph1 << 16 | glyph2;
   while (l <= r) {
      m = (l + r) >> 1;
      straw = ttULONG(data+18+(m*6)); 
      if (needle < straw)
         r = m - 1;
      else if (needle > straw)
         l = m + 1;
      else
         return ttSHORT(data+22+(m*6));
   }
   return 0;
}
px_int32 PX_tt_GetCoverageIndex(px_byte *coverageTable, px_int glyph)
{
   px_ushort coverageFormat = ttUSHORT(coverageTable);
   switch (coverageFormat) {
      case 1: {
         px_ushort glyphCount = ttUSHORT(coverageTable + 2);
         
         px_int32 l=0, r=glyphCount-1, m;
         px_int straw, needle=glyph;
         while (l <= r) {
            px_byte *glyphArray = coverageTable + 4;
            px_ushort glyphID;
            m = (l + r) >> 1;
            glyphID = ttUSHORT(glyphArray + 2 * m);
            straw = glyphID;
            if (needle < straw)
               r = m - 1;
            else if (needle > straw)
               l = m + 1;
            else {
               return m;
            }
         }
         break;
      }
      case 2: {
         px_ushort rangeCount = ttUSHORT(coverageTable + 2);
         px_byte *rangeArray = coverageTable + 4;
         
         px_int32 l=0, r=rangeCount-1, m;
         px_int strawStart, strawEnd, needle=glyph;
         while (l <= r) {
            px_byte *rangeRecord;
            m = (l + r) >> 1;
            rangeRecord = rangeArray + 6 * m;
            strawStart = ttUSHORT(rangeRecord);
            strawEnd = ttUSHORT(rangeRecord + 2);
            if (needle < strawStart)
               r = m - 1;
            else if (needle > strawEnd)
               l = m + 1;
            else {
               px_ushort startCoverageIndex = ttUSHORT(rangeRecord + 4);
               return startCoverageIndex + glyph - strawStart;
            }
         }
         break;
      }
      default: return -1; 
   }
   return -1;
}
px_int32  PX_tt_GetGlyphClass(px_byte *classDefTable, px_int glyph)
{
   px_ushort classDefFormat = ttUSHORT(classDefTable);
   switch (classDefFormat)
   {
      case 1: {
         px_ushort startGlyphID = ttUSHORT(classDefTable + 2);
         px_ushort glyphCount = ttUSHORT(classDefTable + 4);
         px_byte *classDef1ValueArray = classDefTable + 6;
         if (glyph >= startGlyphID && glyph < startGlyphID + glyphCount)
            return (px_int32)ttUSHORT(classDef1ValueArray + 2 * (glyph - startGlyphID));
         break;
      }
      case 2: {
         px_ushort classRangeCount = ttUSHORT(classDefTable + 2);
         px_byte *classRangeRecords = classDefTable + 4;
         
         px_int32 l=0, r=classRangeCount-1, m;
         px_int strawStart, strawEnd, needle=glyph;
         while (l <= r) {
            px_byte *classRangeRecord;
            m = (l + r) >> 1;
            classRangeRecord = classRangeRecords + 6 * m;
            strawStart = ttUSHORT(classRangeRecord);
            strawEnd = ttUSHORT(classRangeRecord + 2);
            if (needle < strawStart)
               r = m - 1;
            else if (needle > strawEnd)
               l = m + 1;
            else
               return (px_int32)ttUSHORT(classRangeRecord + 4);
         }
         break;
      }
      default:
         return -1; 
   }
   
   return 0;
}
#define STBTT_GPOS_TODO_assert(x)
px_int32 PX_tt_GetGlyphGPOSInfoAdvance(const PX_ttfontinfo *info, px_int glyph1, px_int glyph2)
{
   px_ushort lookupListOffset;
   px_byte *lookupList;
   px_ushort lookupCount;
   px_byte *data;
   px_int32 i, sti;
   if (!info->gpos) return 0;
   data = info->data + info->gpos;
   if (ttUSHORT(data+0) != 1) return 0; 
   if (ttUSHORT(data+2) != 0) return 0; 
   lookupListOffset = ttUSHORT(data+8);
   lookupList = data + lookupListOffset;
   lookupCount = ttUSHORT(lookupList);
   for (i=0; i<lookupCount; ++i) {
      px_ushort lookupOffset = ttUSHORT(lookupList + 2 + 2 * i);
      px_byte *lookupTable = lookupList + lookupOffset;
      px_ushort lookupType = ttUSHORT(lookupTable);
      px_ushort subTableCount = ttUSHORT(lookupTable + 4);
      px_byte *subTableOffsets = lookupTable + 6;
      if (lookupType != 2) 
         continue;
      for (sti=0; sti<subTableCount; sti++) {
         px_ushort subtableOffset = ttUSHORT(subTableOffsets + 2 * sti);
         px_byte *table = lookupTable + subtableOffset;
         px_ushort posFormat = ttUSHORT(table);
         px_ushort coverageOffset = ttUSHORT(table + 2);
         px_int32 coverageIndex = PX_tt_GetCoverageIndex(table + coverageOffset, glyph1);
         if (coverageIndex == -1) continue;
         switch (posFormat) {
            case 1: {
               px_int32 l, r, m;
               px_int straw, needle;
               px_ushort valueFormat1 = ttUSHORT(table + 4);
               px_ushort valueFormat2 = ttUSHORT(table + 6);
               if (valueFormat1 == 4 && valueFormat2 == 0) { 
                  px_int32 valueRecordPairSizeInBytes = 2;
                  px_ushort pairSetCount = ttUSHORT(table + 8);
                  px_ushort pairPosOffset = ttUSHORT(table + 10 + 2 * coverageIndex);
                  px_byte *pairValueTable = table + pairPosOffset;
                  px_ushort pairValueCount = ttUSHORT(pairValueTable);
                  px_byte *pairValueArray = pairValueTable + 2;
                  if (coverageIndex >= pairSetCount) return 0;
                  needle=glyph2;
                  r=pairValueCount-1;
                  l=0;
                  
                  while (l <= r) {
                     px_ushort secondGlyph;
                     px_byte *pairValue;
                     m = (l + r) >> 1;
                     pairValue = pairValueArray + (2 + valueRecordPairSizeInBytes) * m;
                     secondGlyph = ttUSHORT(pairValue);
                     straw = secondGlyph;
                     if (needle < straw)
                        r = m - 1;
                     else if (needle > straw)
                        l = m + 1;
                     else {
                        px_short xAdvance = ttSHORT(pairValue + 2);
                        return xAdvance;
                     }
                  }
               } else
                  return 0;
               break;
            }
            case 2: {
               px_ushort valueFormat1 = ttUSHORT(table + 4);
               px_ushort valueFormat2 = ttUSHORT(table + 6);
               if (valueFormat1 == 4 && valueFormat2 == 0) { 
                  px_ushort classDef1Offset = ttUSHORT(table + 8);
                  px_ushort classDef2Offset = ttUSHORT(table + 10);
                  px_int glyph1class = PX_tt_GetGlyphClass(table + classDef1Offset, glyph1);
                  px_int glyph2class = PX_tt_GetGlyphClass(table + classDef2Offset, glyph2);
                  px_ushort class1Count = ttUSHORT(table + 12);
                  px_ushort class2Count = ttUSHORT(table + 14);
                  px_byte *class1Records, *class2Records;
                  px_short xAdvance;
                  if (glyph1class < 0 || glyph1class >= class1Count) return 0; 
                  if (glyph2class < 0 || glyph2class >= class2Count) return 0; 
                  class1Records = table + 16;
                  class2Records = class1Records + 2 * (glyph1class * class2Count);
                  xAdvance = ttSHORT(class2Records + 2 * glyph2class);
                  return xAdvance;
               } else
                  return 0;
               break;
            }
            default:
               return 0; 
         }
      }
   }
   return 0;
}
px_int  PX_ttGetGlyphKernAdvance(const PX_ttfontinfo *info, px_int g1, px_int g2)
{
   px_int xAdvance = 0;
   if (info->gpos)
      xAdvance += PX_tt_GetGlyphGPOSInfoAdvance(info, g1, g2);
   else if (info->kern)
      xAdvance += PX_tt_GetGlyphKernInfoAdvance(info, g1, g2);
   return xAdvance;
}
px_int  PX_ttGetCodepointKernAdvance(const PX_ttfontinfo *info, px_int ch1, px_int ch2)
{
   if (!info->kern && !info->gpos) 
      return 0;
   return PX_ttGetGlyphKernAdvance(info, PX_ttFindGlyphIndex(info,ch1), PX_ttFindGlyphIndex(info,ch2));
}
px_void PX_ttGetCodepointHMetrics(const PX_ttfontinfo *info, px_int codepoint, px_int *advanceWidth, px_int *leftSideBearing)
{
   PX_ttGetGlyphHMetrics(info, PX_ttFindGlyphIndex(info,codepoint), advanceWidth, leftSideBearing);
}
px_void PX_ttGetFontVMetrics(const PX_ttfontinfo *info, px_int *ascent, px_int *descent, px_int *lineGap)
{
   if (ascent ) *ascent  = ttSHORT(info->data+info->hhea + 4);
   if (descent) *descent = ttSHORT(info->data+info->hhea + 6);
   if (lineGap) *lineGap = ttSHORT(info->data+info->hhea + 8);
}
px_int  PX_ttGetFontVMetricsOS2(const PX_ttfontinfo *info, px_int *typoAscent, px_int *typoDescent, px_int *typoLineGap)
{
   px_int tab = PX_tt_find_table(info->data, info->fontstart, "OS/2");
   if (!tab)
      return 0;
   if (typoAscent ) *typoAscent  = ttSHORT(info->data+tab + 68);
   if (typoDescent) *typoDescent = ttSHORT(info->data+tab + 70);
   if (typoLineGap) *typoLineGap = ttSHORT(info->data+tab + 72);
   return 1;
}
px_void PX_ttGetFontBoundingBox(const PX_ttfontinfo *info, px_int *x0, px_int *y0, px_int *x1, px_int *y1)
{
   *x0 = ttSHORT(info->data + info->head + 36);
   *y0 = ttSHORT(info->data + info->head + 38);
   *x1 = ttSHORT(info->data + info->head + 40);
   *y1 = ttSHORT(info->data + info->head + 42);
}
px_float PX_ttScaleForPixelHeight(const PX_ttfontinfo *info, px_float height)
{
   px_int fheight = ttSHORT(info->data + info->hhea + 4) - ttSHORT(info->data + info->hhea + 6);
   return (px_float) height / fheight;
}
px_float PX_ttScaleForMappingEmToPixels(const PX_ttfontinfo *info, px_float pixels)
{
   px_int unitsPerEm = ttUSHORT(info->data + info->head + 18);
   return pixels / unitsPerEm;
}
px_void PX_ttFreeShape(const PX_ttfontinfo *info, PX_ttvertex *v)
{
    MP_Free(info->mp, v);
}
px_byte *PX_ttFindSVGDoc(const PX_ttfontinfo *info, px_int gl)
{
   px_int i;
   px_byte *data = info->data;
   px_byte *svg_doc_list = data + PX_tt_get_svg((PX_ttfontinfo *) info);
   px_int numEntries = ttUSHORT(svg_doc_list);
   px_byte *svg_docs = svg_doc_list + 2;
   for(i=0; i<numEntries; i++) {
      px_byte *svg_doc = svg_docs + (12 * i);
      if ((gl >= ttUSHORT(svg_doc)) && (gl <= ttUSHORT(svg_doc + 2)))
         return svg_doc;
   }
   return 0;
}
px_int PX_ttGetGlyphSVG(const PX_ttfontinfo *info, px_int gl, const char **svg)
{
   px_byte *data = info->data;
   px_byte *svg_doc;
   if (info->svg == 0)
      return 0;
   svg_doc = PX_ttFindSVGDoc(info, gl);
   if (svg_doc != PX_NULL) {
      *svg = (char *) data + info->svg + ttULONG(svg_doc + 4);
      return ttULONG(svg_doc + 8);
   } else {
      return 0;
   }
}
px_int PX_ttGetCodepointSVG(const PX_ttfontinfo *info, px_int unicode_codepoint, const char **svg)
{
   return PX_ttGetGlyphSVG(info, PX_ttFindGlyphIndex(info, unicode_codepoint), svg);
}
px_void PX_ttGetGlyphBitmapBoxSubpixel(const PX_ttfontinfo *font, px_int glyph, px_float scale_x, px_float scale_y,px_float shift_x, px_float shift_y, px_int *ix0, px_int *iy0, px_int *ix1, px_int *iy1)
{
   px_int x0=0,y0=0,x1,y1; 
   if (!PX_ttGetGlyphBox(font, glyph, &x0,&y0,&x1,&y1)) {
      
      if (ix0) *ix0 = 0;
      if (iy0) *iy0 = 0;
      if (ix1) *ix1 = 0;
      if (iy1) *iy1 = 0;
   } else {
      
      if (ix0) *ix0 = STBTT_ifloor( x0 * scale_x + shift_x);
      if (iy0) *iy0 = STBTT_ifloor(-y1 * scale_y + shift_y);
      if (ix1) *ix1 = STBTT_iceil ( x1 * scale_x + shift_x);
      if (iy1) *iy1 = STBTT_iceil (-y0 * scale_y + shift_y);
   }
}
px_void PX_ttGetGlyphBitmapBox(const PX_ttfontinfo *font, px_int glyph, px_float scale_x, px_float scale_y, px_int *ix0, px_int *iy0, px_int *ix1, px_int *iy1)
{
   PX_ttGetGlyphBitmapBoxSubpixel(font, glyph, scale_x, scale_y,0.0f,0.0f, ix0, iy0, ix1, iy1);
}
px_void PX_ttGetCodepointBitmapBoxSubpixel(const PX_ttfontinfo *font, px_int codepoint, px_float scale_x, px_float scale_y, px_float shift_x, px_float shift_y, px_int *ix0, px_int *iy0, px_int *ix1, px_int *iy1)
{
   PX_ttGetGlyphBitmapBoxSubpixel(font, PX_ttFindGlyphIndex(font,codepoint), scale_x, scale_y,shift_x,shift_y, ix0,iy0,ix1,iy1);
}
px_void PX_ttGetCodepointBitmapBox(const PX_ttfontinfo *font, px_int codepoint, px_float scale_x, px_float scale_y, px_int *ix0, px_int *iy0, px_int *ix1, px_int *iy1)
{
   PX_ttGetCodepointBitmapBoxSubpixel(font, codepoint, scale_x, scale_y,0.0f,0.0f, ix0,iy0,ix1,iy1);
}
typedef struct PX_tt_hheap_chunk
{
   struct PX_tt_hheap_chunk *next;
} PX_tt_hheap_chunk;
typedef struct PX_tt_hheap
{
   struct PX_tt_hheap_chunk *head;
   px_void   *first_free;
   px_int    num_remaining_in_head_chunk;
} PX_tt_hheap;
px_void *PX_tt_hheap_alloc(px_memorypool *mp, PX_tt_hheap *hh, px_uint size, px_void *userdata)
{
   if (hh->first_free) {
      px_void *p = hh->first_free;
      hh->first_free = * (px_void **) p;
      return p;
   } else {
      if (hh->num_remaining_in_head_chunk == 0) {
         px_int count = (size < 32 ? 2000 : size < 128 ? 800 : 100);
         PX_tt_hheap_chunk *c = (PX_tt_hheap_chunk *)MP_Malloc(mp,sizeof(PX_tt_hheap_chunk) + size * count);
         if (c == PX_NULL)
            return PX_NULL;
         c->next = hh->head;
         hh->head = c;
         hh->num_remaining_in_head_chunk = count;
      }
      --hh->num_remaining_in_head_chunk;
      return (char *) (hh->head) + sizeof(PX_tt_hheap_chunk) + size * hh->num_remaining_in_head_chunk;
   }
}
px_void PX_tt_hheap_free(PX_tt_hheap *hh, px_void *p)
{
   *(px_void **) p = hh->first_free;
   hh->first_free = p;
}
px_void PX_tt_hheap_cleanup(px_memorypool *mp,PX_tt_hheap *hh, px_void *userdata)
{
   PX_tt_hheap_chunk *c = hh->head;
   while (c) {
      PX_tt_hheap_chunk *n = c->next;
      MP_Free(mp,c);
      c = n;
   }
}
typedef struct PX_tt_edge {
   px_float x0,y0, x1,y1;
   px_int invert;
} PX_tt_edge;
typedef struct PX_tt_active_edge
{
   struct PX_tt_active_edge *next;
   #if STBTT_RASTERIZER_VERSION==1
   px_int x,dx;
   px_float ey;
   px_int direction;
   #elif STBTT_RASTERIZER_VERSION==2
   px_float fx,fdx,fdy;
   px_float direction;
   px_float sy;
   px_float ey;
   #else
   #error "Unrecognized value of STBTT_RASTERIZER_VERSION"
   #endif
} PX_tt_active_edge;
PX_tt_active_edge *PX_tt_new_active(px_memorypool *mp, PX_tt_hheap *hh, PX_tt_edge *e, px_int off_x, px_float start_point, px_void *userdata)
{
   PX_tt_active_edge *z = (PX_tt_active_edge *) PX_tt_hheap_alloc(mp,hh, sizeof(*z), userdata);
   px_float dxdy = (e->x1 - e->x0) / (e->y1 - e->y0);
   STBTT_assert(z != PX_NULL);
   
   if (!z) return z;
   z->fdx = dxdy;
   z->fdy = dxdy != 0.0f ? (1.0f/dxdy) : 0.0f;
   z->fx = e->x0 + dxdy * (start_point - e->y0);
   z->fx -= off_x;
   z->direction = e->invert ? 1.0f : -1.0f;
   z->sy = e->y0;
   z->ey = e->y1;
   z->next = 0;
   return z;
}
px_void PX_tt_handle_clipped_edge(px_float *scanline, px_int x, PX_tt_active_edge *e, px_float x0, px_float y0, px_float x1, px_float y1)
{
   if (y0 == y1) return;
   STBTT_assert(y0 < y1);
   STBTT_assert(e->sy <= e->ey);
   if (y0 > e->ey) return;
   if (y1 < e->sy) return;
   if (y0 < e->sy) {
      x0 += (x1-x0) * (e->sy - y0) / (y1-y0);
      y0 = e->sy;
   }
   if (y1 > e->ey) {
      x1 += (x1-x0) * (e->ey - y1) / (y1-y0);
      y1 = e->ey;
   }
   if (x0 == x)
      STBTT_assert(x1 <= x+1);
   else if (x0 == x+1)
      STBTT_assert(x1 >= x);
   else if (x0 <= x)
      STBTT_assert(x1 <= x);
   else if (x0 >= x+1)
      STBTT_assert(x1 >= x+1);
   else
      STBTT_assert(x1 >= x && x1 <= x+1);
   if (x0 <= x && x1 <= x)
      scanline[x] += e->direction * (y1-y0);
   else if (x0 >= x+1 && x1 >= x+1)
      ;
   else {
      STBTT_assert(x0 >= x && x0 <= x+1 && x1 >= x && x1 <= x+1);
      scanline[x] += e->direction * (y1-y0) * (1-((x0-x)+(x1-x))/2); 
   }
}
px_float PX_tt_sized_trapezoid_area(px_float height, px_float top_width, px_float bottom_width)
{
   STBTT_assert(top_width >= 0);
   STBTT_assert(bottom_width >= 0);
   return (top_width + bottom_width) / 2.0f * height;
}
px_float PX_tt_position_trapezoid_area(px_float height, px_float tx0, px_float tx1, px_float bx0, px_float bx1)
{
   return PX_tt_sized_trapezoid_area(height, tx1 - tx0, bx1 - bx0);
}
px_float PX_tt_sized_triangle_area(px_float height, px_float width)
{
   return height * width / 2;
}
px_void PX_tt_fill_active_edges_new(px_float *scanline, px_float *scanline_fill, px_int len, PX_tt_active_edge *e, px_float y_top)
{
   px_float y_bottom = y_top+1;
   while (e) {
      
      
      STBTT_assert(e->ey >= y_top);
      if (e->fdx == 0) {
         px_float x0 = e->fx;
         if (x0 < len) {
            if (x0 >= 0) {
               PX_tt_handle_clipped_edge(scanline,(px_int) x0,e, x0,y_top, x0,y_bottom);
               PX_tt_handle_clipped_edge(scanline_fill-1,(px_int) x0+1,e, x0,y_top, x0,y_bottom);
            } else {
               PX_tt_handle_clipped_edge(scanline_fill-1,0,e, x0,y_top, x0,y_bottom);
            }
         }
      } else {
         px_float x0 = e->fx;
         px_float dx = e->fdx;
         px_float xb = x0 + dx;
         px_float x_top, x_bottom;
         px_float sy0,sy1;
         px_float dy = e->fdy;
         STBTT_assert(e->sy <= y_bottom && e->ey >= y_top);
         
         
         
         if (e->sy > y_top) {
            x_top = x0 + dx * (e->sy - y_top);
            sy0 = e->sy;
         } else {
            x_top = x0;
            sy0 = y_top;
         }
         if (e->ey < y_bottom) {
            x_bottom = x0 + dx * (e->ey - y_top);
            sy1 = e->ey;
         } else {
            x_bottom = xb;
            sy1 = y_bottom;
         }
         if (x_top >= 0 && x_bottom >= 0 && x_top < len && x_bottom < len) {
            
            if ((px_int) x_top == (px_int) x_bottom) {
               px_float height;
               
               px_int x = (px_int) x_top;
               height = (sy1 - sy0) * e->direction;
               STBTT_assert(x >= 0 && x < len);
               scanline[x]      += PX_tt_position_trapezoid_area(height, x_top, x+1.0f, x_bottom, x+1.0f);
               scanline_fill[x] += height; 
            } else {
               px_int x,x1,x2;
               px_float y_crossing, y_final, step, sign, area;
               
               if (x_top > x_bottom) {
                  
                  px_float t;
                  sy0 = y_bottom - (sy0 - y_top);
                  sy1 = y_bottom - (sy1 - y_top);
                  t = sy0, sy0 = sy1, sy1 = t;
                  t = x_bottom, x_bottom = x_top, x_top = t;
                  dx = -dx;
                  dy = -dy;
                  t = x0, x0 = xb, xb = t;
               }
               STBTT_assert(dy >= 0);
               STBTT_assert(dx >= 0);
               x1 = (px_int) x_top;
               x2 = (px_int) x_bottom;
               
               y_crossing = y_top + dy * (x1+1 - x0);
               
               y_final = y_top + dy * (x2 - x0);
               
               
               
               
               
               
               
               
               
               
               
               
               
               
               
               
               
               
               if (y_crossing > y_bottom)
                  y_crossing = y_bottom;
               sign = e->direction;
               
               area = sign * (y_crossing-sy0);
               
               scanline[x1] += PX_tt_sized_triangle_area(area, x1+1 - x_top);
               
               if (y_final > y_bottom) {
                  y_final = y_bottom;
                  dy = (y_final - y_crossing ) / (x2 - (x1+1)); 
               }
               
               
               
               
               
               
               
               
               
               step = sign * dy * 1; 
               
               
               for (x = x1+1; x < x2; ++x) {
                  scanline[x] += area + step/2; 
                  area += step;
               }
               STBTT_assert(STBTT_fabs(area) <= 1.01f); 
               STBTT_assert(sy1 > y_final-0.01f);
               
               
               scanline[x2] += area + sign * PX_tt_position_trapezoid_area(sy1-y_final, (px_float) x2, x2+1.0f, x_bottom, x2+1.0f);
               
               scanline_fill[x2] += sign * (sy1-sy0);
            }
         } else {
            
            
            
            
            
            
            
            px_int x;
            for (x=0; x < len; ++x) {
               
               
               
               
               
               
               
               
               
               
               
               
               
               px_float y0 = y_top;
               px_float x1 = (px_float) (x);
               px_float x2 = (px_float) (x+1);
               px_float x3 = xb;
               px_float y3 = y_bottom;
               
               
               
               px_float y1 = (x - x0) / dx + y_top;
               px_float y2 = (x+1 - x0) / dx + y_top;
               if (x0 < x1 && x3 > x2) {         
                  PX_tt_handle_clipped_edge(scanline,x,e, x0,y0, x1,y1);
                  PX_tt_handle_clipped_edge(scanline,x,e, x1,y1, x2,y2);
                  PX_tt_handle_clipped_edge(scanline,x,e, x2,y2, x3,y3);
               } else if (x3 < x1 && x0 > x2) {  
                  PX_tt_handle_clipped_edge(scanline,x,e, x0,y0, x2,y2);
                  PX_tt_handle_clipped_edge(scanline,x,e, x2,y2, x1,y1);
                  PX_tt_handle_clipped_edge(scanline,x,e, x1,y1, x3,y3);
               } else if (x0 < x1 && x3 > x1) {  
                  PX_tt_handle_clipped_edge(scanline,x,e, x0,y0, x1,y1);
                  PX_tt_handle_clipped_edge(scanline,x,e, x1,y1, x3,y3);
               } else if (x3 < x1 && x0 > x1) {  
                  PX_tt_handle_clipped_edge(scanline,x,e, x0,y0, x1,y1);
                  PX_tt_handle_clipped_edge(scanline,x,e, x1,y1, x3,y3);
               } else if (x0 < x2 && x3 > x2) {  
                  PX_tt_handle_clipped_edge(scanline,x,e, x0,y0, x2,y2);
                  PX_tt_handle_clipped_edge(scanline,x,e, x2,y2, x3,y3);
               } else if (x3 < x2 && x0 > x2) {  
                  PX_tt_handle_clipped_edge(scanline,x,e, x0,y0, x2,y2);
                  PX_tt_handle_clipped_edge(scanline,x,e, x2,y2, x3,y3);
               } else {  
                  PX_tt_handle_clipped_edge(scanline,x,e, x0,y0, x3,y3);
               }
            }
         }
      }
      e = e->next;
   }
}
px_void PX_tt_rasterize_sorted_edges(px_memorypool *mp,PX_tt_bitmap *result, PX_tt_edge *e, px_int n, px_int vsubsample, px_int off_x, px_int off_y, px_void *userdata)
{
   PX_tt_hheap hh = { 0, 0, 0 };
   PX_tt_active_edge *active = PX_NULL;
   px_int y,j=0, i;
   px_float scanline_data[129], *scanline, *scanline2;

   if (result->w > 64)
      scanline = (px_float *) MP_Malloc(mp,(result->w*2+1) * sizeof(px_float));
   else
      scanline = scanline_data;
   scanline2 = scanline + result->w;
   y = off_y;
   e[n].y0 = (px_float) (off_y + result->h) + 1;
   while (j < result->h) {
      
      px_float scan_y_top    = y + 0.0f;
      px_float scan_y_bottom = y + 1.0f;
      PX_tt_active_edge **step = &active;
      STBTT_memset(scanline , 0, result->w*sizeof(scanline[0]));
      STBTT_memset(scanline2, 0, (result->w+1)*sizeof(scanline[0]));
      
      
      while (*step) {
         PX_tt_active_edge * z = *step;
         if (z->ey <= scan_y_top) {
            *step = z->next; 
            STBTT_assert(z->direction);
            z->direction = 0;
            PX_tt_hheap_free(&hh, z);
         } else {
            step = &((*step)->next); 
         }
      }
      
      while (e->y0 <= scan_y_bottom) {
         if (e->y0 != e->y1) {
            PX_tt_active_edge *z = PX_tt_new_active(mp,&hh, e, off_x, scan_y_top, userdata);
            if (z != PX_NULL) {
               if (j == 0 && off_y != 0) {
                  if (z->ey < scan_y_top) {
                     
                     z->ey = scan_y_top;
                  }
               }
               
               
               z->next = active;
               active = z;
            }
         }
         ++e;
      }
      
      if (active)
         PX_tt_fill_active_edges_new(scanline, scanline2+1, result->w, active, scan_y_top);
      {
         px_float sum = 0;
         for (i=0; i < result->w; ++i) {
            px_float k;
            px_int m;
            sum += scanline2[i];
            k = scanline[i] + sum;
            k = (px_float) STBTT_fabs(k)*255 + 0.5f;
            m = (px_int) k;
            if (m > 255) m = 255;
            result->pixels[j*result->stride + i] = (px_byte) m;
         }
      }
      
      step = &active;
      while (*step) {
         PX_tt_active_edge *z = *step;
         z->fx += z->fdx; 
         step = &((*step)->next); 
      }
      ++y;
      ++j;
   }
   PX_tt_hheap_cleanup(mp,&hh, userdata);
   if (scanline != scanline_data)
       MP_Free(mp, scanline);
}
#define STBTT__COMPARE(a,b)  ((a)->y0 < (b)->y0)
px_void PX_tt_sort_edges_ins_sort(PX_tt_edge *p, px_int n)
{
   px_int i,j;
   for (i=1; i < n; ++i) {
      PX_tt_edge t = p[i], *a = &t;
      j = i;
      while (j > 0) {
         PX_tt_edge *b = &p[j-1];
         px_int c = STBTT__COMPARE(a,b);
         if (!c) break;
         p[j] = p[j-1];
         --j;
      }
      if (i != j)
         p[j] = t;
   }
}
px_void PX_tt_sort_edges_quicksort(PX_tt_edge *p, px_int n)
{
   /* threshold for transitioning to insertion sort */
   while (n > 12) {
      PX_tt_edge t;
      px_int c01,c12,c,m,i,j;
      /* compute median of three */
      m = n >> 1;
      c01 = STBTT__COMPARE(&p[0],&p[m]);
      c12 = STBTT__COMPARE(&p[m],&p[n-1]);
      /* if 0 >= mid >= end, or 0 < mid < end, then use mid */
      if (c01 != c12) {
         /* otherwise, we'll need to swap something else to middle */
         px_int z;
         c = STBTT__COMPARE(&p[0],&p[n-1]);
         /* 0>mid && mid<n:  0>n => n; 0<n => 0 */
         /* 0<mid && mid>n:  0>n => 0; 0<n => n */
         z = (c == c12) ? 0 : n-1;
         t = p[z];
         p[z] = p[m];
         p[m] = t;
      }
      /* now p[m] is the median-of-three */
      /* swap it to the beginning so it won't move around */
      t = p[0];
      p[0] = p[m];
      p[m] = t;
      /* partition loop */
      i=1;
      j=n-1;
      for(;;) {
         /* handling of equality is crucial here */
         /* for sentinels & efficiency with duplicates */
         for (;;++i) {
            if (!STBTT__COMPARE(&p[i], &p[0])) break;
         }
         for (;;--j) {
            if (!STBTT__COMPARE(&p[0], &p[j])) break;
         }
         /* make sure we haven't crossed */
         if (i >= j) break;
         t = p[i];
         p[i] = p[j];
         p[j] = t;
         ++i;
         --j;
      }
      /* recurse on smaller side, iterate on larger */
      if (j < (n-i)) {
         PX_tt_sort_edges_quicksort(p,j);
         p = p+i;
         n = n-i;
      } else {
         PX_tt_sort_edges_quicksort(p+i, n-i);
         n = j;
      }
   }
}
px_void PX_tt_sort_edges(PX_tt_edge *p, px_int n)
{
   PX_tt_sort_edges_quicksort(p, n);
   PX_tt_sort_edges_ins_sort(p, n);
}
typedef struct
{
   px_float x,y;
} PX_tt_point;
px_void PX_tt_rasterize(px_memorypool *mp,PX_tt_bitmap *result, PX_tt_point *pts, px_int *wcount, px_int windings, px_float scale_x, px_float scale_y, px_float shift_x, px_float shift_y, px_int off_x, px_int off_y, px_int invert, px_void *userdata)
{
   px_float y_scale_inv = invert ? -scale_y : scale_y;
   PX_tt_edge *e;
   px_int n,i,j,k,m;
   px_int vsubsample = 1;
   
   
   n = 0;
   for (i=0; i < windings; ++i)
      n += wcount[i];
   e = (PX_tt_edge *) MP_Malloc(mp,sizeof(*e) * (n+1)); 
   if (e == 0) return;
   n = 0;
   m=0;
   for (i=0; i < windings; ++i) {
      PX_tt_point *p = pts + m;
      m += wcount[i];
      j = wcount[i]-1;
      for (k=0; k < wcount[i]; j=k++) {
         px_int a=k,b=j;
         
         if (p[j].y == p[k].y)
            continue;
         
         e[n].invert = 0;
         if (invert ? p[j].y > p[k].y : p[j].y < p[k].y) {
            e[n].invert = 1;
            a=j,b=k;
         }
         e[n].x0 = p[a].x * scale_x + shift_x;
         e[n].y0 = (p[a].y * y_scale_inv + shift_y) * vsubsample;
         e[n].x1 = p[b].x * scale_x + shift_x;
         e[n].y1 = (p[b].y * y_scale_inv + shift_y) * vsubsample;
         ++n;
      }
   }
   
   
   PX_tt_sort_edges(e, n);
   
   PX_tt_rasterize_sorted_edges(mp,result, e, n, vsubsample, off_x, off_y, userdata);
   MP_Free(mp, e);
}
px_void PX_tt_add_point(PX_tt_point *points, px_int n, px_float x, px_float y)
{
   if (!points) return; 
   points[n].x = x;
   points[n].y = y;
}
px_int PX_tt_tesselate_curve(PX_tt_point *points, px_int *num_points, px_float x0, px_float y0, px_float x1, px_float y1, px_float x2, px_float y2, px_float objspace_flatness_squared, px_int n)
{
   
   px_float mx = (x0 + 2*x1 + x2)/4;
   px_float my = (y0 + 2*y1 + y2)/4;
   
   px_float dx = (x0+x2)/2 - mx;
   px_float dy = (y0+y2)/2 - my;
   if (n > 16) 
      return 1;
   if (dx*dx+dy*dy > objspace_flatness_squared) { 
      PX_tt_tesselate_curve(points, num_points, x0,y0, (x0+x1)/2.0f,(y0+y1)/2.0f, mx,my, objspace_flatness_squared,n+1);
      PX_tt_tesselate_curve(points, num_points, mx,my, (x1+x2)/2.0f,(y1+y2)/2.0f, x2,y2, objspace_flatness_squared,n+1);
   } else {
      PX_tt_add_point(points, *num_points,x2,y2);
      *num_points = *num_points+1;
   }
   return 1;
}
px_void PX_tt_tesselate_cubic(PX_tt_point *points, px_int *num_points, px_float x0, px_float y0, px_float x1, px_float y1, px_float x2, px_float y2, px_float x3, px_float y3, px_float objspace_flatness_squared, px_int n)
{
   
   px_float dx0 = x1-x0;
   px_float dy0 = y1-y0;
   px_float dx1 = x2-x1;
   px_float dy1 = y2-y1;
   px_float dx2 = x3-x2;
   px_float dy2 = y3-y2;
   px_float dx = x3-x0;
   px_float dy = y3-y0;
   px_float longlen = (px_float) (STBTT_sqrt(dx0*dx0+dy0*dy0)+STBTT_sqrt(dx1*dx1+dy1*dy1)+STBTT_sqrt(dx2*dx2+dy2*dy2));
   px_float shortlen = (px_float) STBTT_sqrt(dx*dx+dy*dy);
   px_float flatness_squared = longlen*longlen-shortlen*shortlen;
   if (n > 16) 
      return;
   if (flatness_squared > objspace_flatness_squared) {
      px_float x01 = (x0+x1)/2;
      px_float y01 = (y0+y1)/2;
      px_float x12 = (x1+x2)/2;
      px_float y12 = (y1+y2)/2;
      px_float x23 = (x2+x3)/2;
      px_float y23 = (y2+y3)/2;
      px_float xa = (x01+x12)/2;
      px_float ya = (y01+y12)/2;
      px_float xb = (x12+x23)/2;
      px_float yb = (y12+y23)/2;
      px_float mx = (xa+xb)/2;
      px_float my = (ya+yb)/2;
      PX_tt_tesselate_cubic(points, num_points, x0,y0, x01,y01, xa,ya, mx,my, objspace_flatness_squared,n+1);
      PX_tt_tesselate_cubic(points, num_points, mx,my, xb,yb, x23,y23, x3,y3, objspace_flatness_squared,n+1);
   } else {
      PX_tt_add_point(points, *num_points,x3,y3);
      *num_points = *num_points+1;
   }
}
PX_tt_point *PX_ttFlattenCurves(px_memorypool *mp,PX_ttvertex *vertices, px_int num_verts, px_float objspace_flatness, px_int **contour_lengths, px_int *num_contours, px_void *userdata)
{
   PX_tt_point *points=0;
   px_int num_points=0;
   px_float objspace_flatness_squared = objspace_flatness * objspace_flatness;
   px_int i,n=0,start=0, pass;
   
   for (i=0; i < num_verts; ++i)
      if (vertices[i].type == STBTT_vmove)
         ++n;
   *num_contours = n;
   if (n == 0) return 0;
   *contour_lengths = (px_int *)MP_Malloc(mp,sizeof(**contour_lengths) * n);
   if (*contour_lengths == 0) {
      *num_contours = 0;
      return 0;
   }
   
   for (pass=0; pass < 2; ++pass) {
      px_float x=0,y=0;
      if (pass == 1) {
         points = (PX_tt_point *)MP_Malloc(mp,num_points * sizeof(points[0]));
         if (points == PX_NULL) goto error;
      }
      num_points = 0;
      n= -1;
      for (i=0; i < num_verts; ++i) {
         switch (vertices[i].type) {
            case STBTT_vmove:
               
               if (n >= 0)
                  (*contour_lengths)[n] = num_points - start;
               ++n;
               start = num_points;
               x = vertices[i].x, y = vertices[i].y;
               PX_tt_add_point(points, num_points++, x,y);
               break;
            case STBTT_vline:
               x = vertices[i].x, y = vertices[i].y;
               PX_tt_add_point(points, num_points++, x, y);
               break;
            case STBTT_vcurve:
               PX_tt_tesselate_curve(points, &num_points, x,y,
                                        vertices[i].cx, vertices[i].cy,
                                        vertices[i].x,  vertices[i].y,
                                        objspace_flatness_squared, 0);
               x = vertices[i].x, y = vertices[i].y;
               break;
            case STBTT_vcubic:
               PX_tt_tesselate_cubic(points, &num_points, x,y,
                                        vertices[i].cx, vertices[i].cy,
                                        vertices[i].cx1, vertices[i].cy1,
                                        vertices[i].x,  vertices[i].y,
                                        objspace_flatness_squared, 0);
               x = vertices[i].x, y = vertices[i].y;
               break;
         }
      }
      (*contour_lengths)[n] = num_points - start;
   }
   return points;
error:
   MP_Free(mp, points);
   MP_Free(mp, *contour_lengths);
   *contour_lengths = 0;
   *num_contours = 0;
   return PX_NULL;
}
px_void PX_ttRasterize(px_memorypool *mp,PX_tt_bitmap *result, px_float flatness_in_pixels, PX_ttvertex *vertices, px_int num_verts, px_float scale_x, px_float scale_y, px_float shift_x, px_float shift_y, px_int x_off, px_int y_off, px_int invert, px_void *userdata)
{
   px_float scale            = scale_x > scale_y ? scale_y : scale_x;
   px_int winding_count      = 0;
   px_int *winding_lengths   = PX_NULL;
   PX_tt_point *windings = PX_ttFlattenCurves(mp,vertices, num_verts, flatness_in_pixels / scale, &winding_lengths, &winding_count, userdata);
   if (windings) {
      PX_tt_rasterize(mp,result, windings, winding_lengths, winding_count, scale_x, scale_y, shift_x, shift_y, x_off, y_off, invert, userdata);
      MP_Free(mp,winding_lengths);
      MP_Free(mp,windings);
   }
}
px_void PX_ttFreeBitmap(px_memorypool *mp,px_byte *bitmap, px_void *userdata)
{
   MP_Free(mp,bitmap);
}
px_byte *PX_ttGetGlyphBitmapSubpixel(const PX_ttfontinfo *info, px_float scale_x, px_float scale_y, px_float shift_x, px_float shift_y, px_int glyph, px_int *width, px_int *height, px_int *xoff, px_int *yoff)
{
   px_int ix0,iy0,ix1,iy1;
   PX_tt_bitmap gbm;
   PX_ttvertex *vertices;
   px_int num_verts = PX_ttGetGlyphShape(info, glyph, &vertices);
   if (scale_x == 0) scale_x = scale_y;
   if (scale_y == 0) {
      if (scale_x == 0) {
         MP_Free(info->mp,vertices);
         return PX_NULL;
      }
      scale_y = scale_x;
   }
   PX_ttGetGlyphBitmapBoxSubpixel(info, glyph, scale_x, scale_y, shift_x, shift_y, &ix0,&iy0,&ix1,&iy1);
   
   gbm.w = (ix1 - ix0);
   gbm.h = (iy1 - iy0);
   gbm.pixels = PX_NULL; 
   if (width ) *width  = gbm.w;
   if (height) *height = gbm.h;
   if (xoff  ) *xoff   = ix0;
   if (yoff  ) *yoff   = iy0;
   if (gbm.w && gbm.h) {
      gbm.pixels = (px_byte *) MP_Malloc(info->mp, gbm.w * gbm.h);
      if (gbm.pixels) {
         gbm.stride = gbm.w;
         PX_ttRasterize(info->mp, &gbm, 0.35f, vertices, num_verts, scale_x, scale_y, shift_x, shift_y, ix0, iy0, 1, info->userdata);
      }
   }
   MP_Free(info->mp, vertices);
   return gbm.pixels;
}
px_byte *PX_ttGetGlyphBitmap(const PX_ttfontinfo *info, px_float scale_x, px_float scale_y, px_int glyph, px_int *width, px_int *height, px_int *xoff, px_int *yoff)
{
   return PX_ttGetGlyphBitmapSubpixel(info, scale_x, scale_y, 0.0f, 0.0f, glyph, width, height, xoff, yoff);
}
px_void PX_ttMakeGlyphBitmapSubpixel(const PX_ttfontinfo *info, px_byte *output, px_int out_w, px_int out_h, px_int out_stride, px_float scale_x, px_float scale_y, px_float shift_x, px_float shift_y, px_int glyph)
{
   px_int ix0,iy0;
   PX_ttvertex *vertices;
   px_int num_verts = PX_ttGetGlyphShape(info, glyph, &vertices);
   PX_tt_bitmap gbm;
   PX_ttGetGlyphBitmapBoxSubpixel(info, glyph, scale_x, scale_y, shift_x, shift_y, &ix0,&iy0,0,0);
   gbm.pixels = output;
   gbm.w = out_w;
   gbm.h = out_h;
   gbm.stride = out_stride;
   if (gbm.w && gbm.h)
      PX_ttRasterize(info->mp, &gbm, 0.35f, vertices, num_verts, scale_x, scale_y, shift_x, shift_y, ix0,iy0, 1, info->userdata);
   MP_Free(info->mp, vertices);
}
px_void PX_ttMakeGlyphBitmap(const PX_ttfontinfo *info, px_byte *output, px_int out_w, px_int out_h, px_int out_stride, px_float scale_x, px_float scale_y, px_int glyph)
{
   PX_ttMakeGlyphBitmapSubpixel(info, output, out_w, out_h, out_stride, scale_x, scale_y, 0.0f,0.0f, glyph);
}
px_byte *PX_ttGetCodepointBitmapSubpixel(const PX_ttfontinfo *info, px_float scale_x, px_float scale_y, px_float shift_x, px_float shift_y, px_int codepoint, px_int *width, px_int *height, px_int *xoff, px_int *yoff)
{
   return PX_ttGetGlyphBitmapSubpixel(info, scale_x, scale_y,shift_x,shift_y, PX_ttFindGlyphIndex(info,codepoint), width,height,xoff,yoff);
}
px_void PX_ttMakeCodepointBitmapSubpixelPrefilter(const PX_ttfontinfo *info, px_byte *output, px_int out_w, px_int out_h, px_int out_stride, px_float scale_x, px_float scale_y, px_float shift_x, px_float shift_y, px_int oversample_x, px_int oversample_y, px_float *sub_x, px_float *sub_y, px_int codepoint)
{
   PX_ttMakeGlyphBitmapSubpixelPrefilter(info, output, out_w, out_h, out_stride, scale_x, scale_y, shift_x, shift_y, oversample_x, oversample_y, sub_x, sub_y, PX_ttFindGlyphIndex(info,codepoint));
}
px_void PX_ttMakeCodepointBitmapSubpixel(const PX_ttfontinfo *info, px_byte *output, px_int out_w, px_int out_h, px_int out_stride, px_float scale_x, px_float scale_y, px_float shift_x, px_float shift_y, px_int codepoint)
{
   PX_ttMakeGlyphBitmapSubpixel(info, output, out_w, out_h, out_stride, scale_x, scale_y, shift_x, shift_y, PX_ttFindGlyphIndex(info,codepoint));
}
px_byte *PX_ttGetCodepointBitmap(const PX_ttfontinfo *info, px_float scale_x, px_float scale_y, px_int codepoint, px_int *width, px_int *height, px_int *xoff, px_int *yoff)
{
   return PX_ttGetCodepointBitmapSubpixel(info, scale_x, scale_y, 0.0f,0.0f, codepoint, width,height,xoff,yoff);
}
 px_void PX_ttMakeCodepointBitmap(const PX_ttfontinfo *info, px_byte *output, px_int out_w, px_int out_h, px_int out_stride, px_float scale_x, px_float scale_y, px_int codepoint)
{
   PX_ttMakeCodepointBitmapSubpixel(info, output, out_w, out_h, out_stride, scale_x, scale_y, 0.0f,0.0f, codepoint);
}
px_int PX_ttBakeFontBitmap_internal(px_memorypool* mp, px_byte *data, px_int offset,  
                                px_float pixel_height,                     
                                px_byte *pixels, px_int pw, px_int ph,  
                                px_int first_char, px_int num_chars,          
                                PX_ttbakedchar *chardata)
{
   px_float scale;
   px_int x,y,bottom_y, i;
   PX_ttfontinfo f;
   f.userdata = PX_NULL;
   if (!PX_ttInitFont(mp,&f, data, offset))
      return -1;
   STBTT_memset(pixels, 0, pw*ph); 
   x=y=1;
   bottom_y = 1;
   scale = PX_ttScaleForPixelHeight(&f, pixel_height);
   for (i=0; i < num_chars; ++i) {
      px_int advance, lsb, x0,y0,x1,y1,gw,gh;
      px_int g = PX_ttFindGlyphIndex(&f, first_char + i);
      PX_ttGetGlyphHMetrics(&f, g, &advance, &lsb);
      PX_ttGetGlyphBitmapBox(&f, g, scale,scale, &x0,&y0,&x1,&y1);
      gw = x1-x0;
      gh = y1-y0;
      if (x + gw + 1 >= pw)
         y = bottom_y, x = 1; 
      if (y + gh + 1 >= ph) 
         return -i;
      STBTT_assert(x+gw < pw);
      STBTT_assert(y+gh < ph);
      PX_ttMakeGlyphBitmap(&f, pixels+x+y*pw, gw,gh,pw, scale,scale, g);
      chardata[i].x0 = (px_short) x;
      chardata[i].y0 = (px_short) y;
      chardata[i].x1 = (px_short) (x + gw);
      chardata[i].y1 = (px_short) (y + gh);
      chardata[i].xadvance = scale * advance;
      chardata[i].xoff     = (px_float) x0;
      chardata[i].yoff     = (px_float) y0;
      x = x + gw + 1;
      if (y+gh+1 > bottom_y)
         bottom_y = y+gh+1;
   }
   return bottom_y;
}
px_void PX_ttGetBakedQuad(const PX_ttbakedchar *chardata, px_int pw, px_int ph, px_int char_index, px_float *xpos, px_float *ypos, PX_ttaligned_quad *q, px_int opengl_fillrule)
{
   px_float d3d_bias = opengl_fillrule ? 0 : -0.5f;
   px_float ipw = 1.0f / pw, iph = 1.0f / ph;
   const PX_ttbakedchar *b = chardata + char_index;
   px_int round_x = STBTT_ifloor((*xpos + b->xoff) + 0.5f);
   px_int round_y = STBTT_ifloor((*ypos + b->yoff) + 0.5f);
   q->x0 = round_x + d3d_bias;
   q->y0 = round_y + d3d_bias;
   q->x1 = round_x + b->x1 - b->x0 + d3d_bias;
   q->y1 = round_y + b->y1 - b->y0 + d3d_bias;
   q->s0 = b->x0 * ipw;
   q->t0 = b->y0 * iph;
   q->s1 = b->x1 * ipw;
   q->t1 = b->y1 * iph;
   *xpos += b->xadvance;
}
#ifndef STB_RECT_PACK_VERSION
typedef px_int stbrp_coord;
typedef struct
{
   px_int width,height;
   px_int x,y,bottom_y;
} stbrp_context;
typedef struct
{
   px_byte x;
} stbrp_node;
struct stbrp_rect
{
   stbrp_coord x,y;
   px_int id,w,h,was_packed;
};
px_void stbrp_init_target(stbrp_context *con, px_int pw, px_int ph, stbrp_node *nodes, px_int num_nodes)
{
   con->width  = pw;
   con->height = ph;
   con->x = 0;
   con->y = 0;
   con->bottom_y = 0;
}
px_void stbrp_pack_rects(stbrp_context *con, stbrp_rect *rects, px_int num_rects)
{
   px_int i;
   for (i=0; i < num_rects; ++i) {
      if (con->x + rects[i].w > con->width) {
         con->x = 0;
         con->y = con->bottom_y;
      }
      if (con->y + rects[i].h > con->height)
         break;
      rects[i].x = con->x;
      rects[i].y = con->y;
      rects[i].was_packed = 1;
      con->x += rects[i].w;
      if (con->y + rects[i].h > con->bottom_y)
         con->bottom_y = con->y + rects[i].h;
   }
   for (   ; i < num_rects; ++i)
      rects[i].was_packed = 0;
}
#endif
px_int PX_ttPackBegin(px_memorypool *mp,PX_ttpack_context *spc, px_byte *pixels, px_int pw, px_int ph, px_int stride_in_bytes, px_int padding, px_void *alloc_context)
{
   stbrp_context *context = (stbrp_context *) MP_Malloc(mp,sizeof(*context));
   px_int            num_nodes = pw - padding;
   stbrp_node    *nodes   = (stbrp_node    *)MP_Malloc(mp, sizeof(*nodes  ) * num_nodes);
   if (context == PX_NULL || nodes == PX_NULL) {
      if (context != PX_NULL) MP_Free(mp,context);
      if (nodes   != PX_NULL) MP_Free(mp, nodes);
      return 0;
   }
   spc->user_allocator_context = alloc_context;
   spc->width = pw;
   spc->height = ph;
   spc->pixels = pixels;
   spc->pack_info = context;
   spc->nodes = nodes;
   spc->padding = padding;
   spc->stride_in_bytes = stride_in_bytes != 0 ? stride_in_bytes : pw;
   spc->h_oversample = 1;
   spc->v_oversample = 1;
   spc->skip_missing = 0;
   stbrp_init_target(context, pw-padding, ph-padding, nodes, num_nodes);
   if (pixels)
      STBTT_memset(pixels, 0, pw*ph); 
   return 1;
}
px_void PX_ttPackEnd  (px_memorypool *mp,PX_ttpack_context *spc)
{
   MP_Free(mp,spc->nodes);
   MP_Free(mp,spc->pack_info);
}
px_void PX_ttPackSetOversampling(PX_ttpack_context *spc, px_uint h_oversample, px_uint v_oversample)
{
   STBTT_assert(h_oversample <= STBTT_MAX_OVERSAMPLE);
   STBTT_assert(v_oversample <= STBTT_MAX_OVERSAMPLE);
   if (h_oversample <= STBTT_MAX_OVERSAMPLE)
      spc->h_oversample = h_oversample;
   if (v_oversample <= STBTT_MAX_OVERSAMPLE)
      spc->v_oversample = v_oversample;
}
px_void PX_ttPackSetSkipMissingCodepoints(PX_ttpack_context *spc, px_int skip)
{
   spc->skip_missing = skip;
}
#define STBTT__OVER_MASK  (STBTT_MAX_OVERSAMPLE-1)
px_void PX_tt_h_prefilter(px_byte *pixels, px_int w, px_int h, px_int stride_in_bytes, px_uint kernel_width)
{
   px_byte buffer[STBTT_MAX_OVERSAMPLE];
   px_int safe_w = w - kernel_width;
   px_int j;
   STBTT_memset(buffer, 0, STBTT_MAX_OVERSAMPLE); 
   for (j=0; j < h; ++j) {
      px_int i;
      px_uint total;
      STBTT_memset(buffer, 0, kernel_width);
      total = 0;
      
      switch (kernel_width) {
         case 2:
            for (i=0; i <= safe_w; ++i) {
               total += pixels[i] - buffer[i & STBTT__OVER_MASK];
               buffer[(i+kernel_width) & STBTT__OVER_MASK] = pixels[i];
               pixels[i] = (px_byte) (total / 2);
            }
            break;
         case 3:
            for (i=0; i <= safe_w; ++i) {
               total += pixels[i] - buffer[i & STBTT__OVER_MASK];
               buffer[(i+kernel_width) & STBTT__OVER_MASK] = pixels[i];
               pixels[i] = (px_byte) (total / 3);
            }
            break;
         case 4:
            for (i=0; i <= safe_w; ++i) {
               total += pixels[i] - buffer[i & STBTT__OVER_MASK];
               buffer[(i+kernel_width) & STBTT__OVER_MASK] = pixels[i];
               pixels[i] = (px_byte) (total / 4);
            }
            break;
         case 5:
            for (i=0; i <= safe_w; ++i) {
               total += pixels[i] - buffer[i & STBTT__OVER_MASK];
               buffer[(i+kernel_width) & STBTT__OVER_MASK] = pixels[i];
               pixels[i] = (px_byte) (total / 5);
            }
            break;
         default:
            for (i=0; i <= safe_w; ++i) {
               total += pixels[i] - buffer[i & STBTT__OVER_MASK];
               buffer[(i+kernel_width) & STBTT__OVER_MASK] = pixels[i];
               pixels[i] = (px_byte) (total / kernel_width);
            }
            break;
      }
      for (; i < w; ++i) {
         STBTT_assert(pixels[i] == 0);
         total -= buffer[i & STBTT__OVER_MASK];
         pixels[i] = (px_byte) (total / kernel_width);
      }
      pixels += stride_in_bytes;
   }
}
px_void PX_tt_v_prefilter(px_byte *pixels, px_int w, px_int h, px_int stride_in_bytes, px_uint kernel_width)
{
   px_byte buffer[STBTT_MAX_OVERSAMPLE];
   px_int safe_h = h - kernel_width;
   px_int j;
   STBTT_memset(buffer, 0, STBTT_MAX_OVERSAMPLE); 
   for (j=0; j < w; ++j) {
      px_int i;
      px_uint total;
      STBTT_memset(buffer, 0, kernel_width);
      total = 0;
      
      switch (kernel_width) {
         case 2:
            for (i=0; i <= safe_h; ++i) {
               total += pixels[i*stride_in_bytes] - buffer[i & STBTT__OVER_MASK];
               buffer[(i+kernel_width) & STBTT__OVER_MASK] = pixels[i*stride_in_bytes];
               pixels[i*stride_in_bytes] = (px_byte) (total / 2);
            }
            break;
         case 3:
            for (i=0; i <= safe_h; ++i) {
               total += pixels[i*stride_in_bytes] - buffer[i & STBTT__OVER_MASK];
               buffer[(i+kernel_width) & STBTT__OVER_MASK] = pixels[i*stride_in_bytes];
               pixels[i*stride_in_bytes] = (px_byte) (total / 3);
            }
            break;
         case 4:
            for (i=0; i <= safe_h; ++i) {
               total += pixels[i*stride_in_bytes] - buffer[i & STBTT__OVER_MASK];
               buffer[(i+kernel_width) & STBTT__OVER_MASK] = pixels[i*stride_in_bytes];
               pixels[i*stride_in_bytes] = (px_byte) (total / 4);
            }
            break;
         case 5:
            for (i=0; i <= safe_h; ++i) {
               total += pixels[i*stride_in_bytes] - buffer[i & STBTT__OVER_MASK];
               buffer[(i+kernel_width) & STBTT__OVER_MASK] = pixels[i*stride_in_bytes];
               pixels[i*stride_in_bytes] = (px_byte) (total / 5);
            }
            break;
         default:
            for (i=0; i <= safe_h; ++i) {
               total += pixels[i*stride_in_bytes] - buffer[i & STBTT__OVER_MASK];
               buffer[(i+kernel_width) & STBTT__OVER_MASK] = pixels[i*stride_in_bytes];
               pixels[i*stride_in_bytes] = (px_byte) (total / kernel_width);
            }
            break;
      }
      for (; i < h; ++i) {
         STBTT_assert(pixels[i*stride_in_bytes] == 0);
         total -= buffer[i & STBTT__OVER_MASK];
         pixels[i*stride_in_bytes] = (px_byte) (total / kernel_width);
      }
      pixels += 1;
   }
}
px_float PX_tt_oversample_shift(px_int oversample)
{
   if (!oversample)
      return 0.0f;
   
   
   
   
   return (px_float)-(oversample - 1) / (2.0f * (px_float)oversample);
}
px_int PX_ttPackFontRangesGatherRects(PX_ttpack_context *spc, const PX_ttfontinfo *info, PX_ttpack_range *ranges, px_int num_ranges, stbrp_rect *rects)
{
   px_int i,j,k;
   px_int missing_glyph_added = 0;
   k=0;
   for (i=0; i < num_ranges; ++i) {
      px_float fh = ranges[i].font_size;
      px_float scale = fh > 0 ? PX_ttScaleForPixelHeight(info, fh) : PX_ttScaleForMappingEmToPixels(info, -fh);
      ranges[i].h_oversample = (px_byte) spc->h_oversample;
      ranges[i].v_oversample = (px_byte) spc->v_oversample;
      for (j=0; j < ranges[i].num_chars; ++j) {
         px_int x0,y0,x1,y1;
         px_int codepoint = ranges[i].array_of_unicode_codepoints == PX_NULL ? ranges[i].first_unicode_codepoint_in_range + j : ranges[i].array_of_unicode_codepoints[j];
         px_int glyph = PX_ttFindGlyphIndex(info, codepoint);
         if (glyph == 0 && (spc->skip_missing || missing_glyph_added)) {
            rects[k].w = rects[k].h = 0;
         } else {
            PX_ttGetGlyphBitmapBoxSubpixel(info,glyph,
                                            scale * spc->h_oversample,
                                            scale * spc->v_oversample,
                                            0,0,
                                            &x0,&y0,&x1,&y1);
            rects[k].w = (stbrp_coord) (x1-x0 + spc->padding + spc->h_oversample-1);
            rects[k].h = (stbrp_coord) (y1-y0 + spc->padding + spc->v_oversample-1);
            if (glyph == 0)
               missing_glyph_added = 1;
         }
         ++k;
      }
   }
   return k;
}
px_void PX_ttMakeGlyphBitmapSubpixelPrefilter(const PX_ttfontinfo *info, px_byte *output, px_int out_w, px_int out_h, px_int out_stride, px_float scale_x, px_float scale_y, px_float shift_x, px_float shift_y, px_int prefilter_x, px_int prefilter_y, px_float *sub_x, px_float *sub_y, px_int glyph)
{
   PX_ttMakeGlyphBitmapSubpixel(info,
                                 output,
                                 out_w - (prefilter_x - 1),
                                 out_h - (prefilter_y - 1),
                                 out_stride,
                                 scale_x,
                                 scale_y,
                                 shift_x,
                                 shift_y,
                                 glyph);
   if (prefilter_x > 1)
      PX_tt_h_prefilter(output, out_w, out_h, out_stride, prefilter_x);
   if (prefilter_y > 1)
      PX_tt_v_prefilter(output, out_w, out_h, out_stride, prefilter_y);
   *sub_x = PX_tt_oversample_shift(prefilter_x);
   *sub_y = PX_tt_oversample_shift(prefilter_y);
}
px_int PX_ttPackFontRangesRenderIntoRects(PX_ttpack_context *spc, const PX_ttfontinfo *info, PX_ttpack_range *ranges, px_int num_ranges, stbrp_rect *rects)
{
   px_int i,j,k, missing_glyph = -1, return_value = 1;
   
   px_int old_h_over = spc->h_oversample;
   px_int old_v_over = spc->v_oversample;
   k = 0;
   for (i=0; i < num_ranges; ++i) {
      px_float fh = ranges[i].font_size;
      px_float scale = fh > 0 ? PX_ttScaleForPixelHeight(info, fh) : PX_ttScaleForMappingEmToPixels(info, -fh);
      px_float recip_h,recip_v,sub_x,sub_y;
      spc->h_oversample = ranges[i].h_oversample;
      spc->v_oversample = ranges[i].v_oversample;
      recip_h = 1.0f / spc->h_oversample;
      recip_v = 1.0f / spc->v_oversample;
      sub_x = PX_tt_oversample_shift(spc->h_oversample);
      sub_y = PX_tt_oversample_shift(spc->v_oversample);
      for (j=0; j < ranges[i].num_chars; ++j) {
         stbrp_rect *r = &rects[k];
         if (r->was_packed && r->w != 0 && r->h != 0) {
            PX_ttpackedchar *bc = &ranges[i].chardata_for_range[j];
            px_int advance, lsb, x0,y0,x1,y1;
            px_int codepoint = ranges[i].array_of_unicode_codepoints == PX_NULL ? ranges[i].first_unicode_codepoint_in_range + j : ranges[i].array_of_unicode_codepoints[j];
            px_int glyph = PX_ttFindGlyphIndex(info, codepoint);
            stbrp_coord pad = (stbrp_coord) spc->padding;
            
            r->x += pad;
            r->y += pad;
            r->w -= pad;
            r->h -= pad;
            PX_ttGetGlyphHMetrics(info, glyph, &advance, &lsb);
            PX_ttGetGlyphBitmapBox(info, glyph,
                                    scale * spc->h_oversample,
                                    scale * spc->v_oversample,
                                    &x0,&y0,&x1,&y1);
            PX_ttMakeGlyphBitmapSubpixel(info,
                                          spc->pixels + r->x + r->y*spc->stride_in_bytes,
                                          r->w - spc->h_oversample+1,
                                          r->h - spc->v_oversample+1,
                                          spc->stride_in_bytes,
                                          scale * spc->h_oversample,
                                          scale * spc->v_oversample,
                                          0,0,
                                          glyph);
            if (spc->h_oversample > 1)
               PX_tt_h_prefilter(spc->pixels + r->x + r->y*spc->stride_in_bytes,
                                  r->w, r->h, spc->stride_in_bytes,
                                  spc->h_oversample);
            if (spc->v_oversample > 1)
               PX_tt_v_prefilter(spc->pixels + r->x + r->y*spc->stride_in_bytes,
                                  r->w, r->h, spc->stride_in_bytes,
                                  spc->v_oversample);
            bc->x0       = (px_short)  r->x;
            bc->y0       = (px_short)  r->y;
            bc->x1       = (px_short) (r->x + r->w);
            bc->y1       = (px_short) (r->y + r->h);
            bc->xadvance =                scale * advance;
            bc->xoff     =       (px_float)  x0 * recip_h + sub_x;
            bc->yoff     =       (px_float)  y0 * recip_v + sub_y;
            bc->xoff2    =                (x0 + r->w) * recip_h + sub_x;
            bc->yoff2    =                (y0 + r->h) * recip_v + sub_y;
            if (glyph == 0)
               missing_glyph = j;
         } else if (spc->skip_missing) {
            return_value = 0;
         } else if (r->was_packed && r->w == 0 && r->h == 0 && missing_glyph >= 0) {
            ranges[i].chardata_for_range[j] = ranges[i].chardata_for_range[missing_glyph];
         } else {
            return_value = 0; 
         }
         ++k;
      }
   }
   
   spc->h_oversample = old_h_over;
   spc->v_oversample = old_v_over;
   return return_value;
}
px_void PX_ttPackFontRangesPackRects(PX_ttpack_context *spc, stbrp_rect *rects, px_int num_rects)
{
   stbrp_pack_rects((stbrp_context *) spc->pack_info, rects, num_rects);
}
px_int PX_ttPackFontRanges(px_memorypool *mp,PX_ttpack_context *spc, const px_byte *fontdata, px_int font_index, PX_ttpack_range *ranges, px_int num_ranges)
{
   PX_ttfontinfo info;
   px_int i,j,n, return_value = 1;
   
   stbrp_rect    *rects;
   
   for (i=0; i < num_ranges; ++i)
      for (j=0; j < ranges[i].num_chars; ++j)
         ranges[i].chardata_for_range[j].x0 =
         ranges[i].chardata_for_range[j].y0 =
         ranges[i].chardata_for_range[j].x1 =
         ranges[i].chardata_for_range[j].y1 = 0;
   n = 0;
   for (i=0; i < num_ranges; ++i)
      n += ranges[i].num_chars;
   rects = (stbrp_rect *) MP_Malloc(mp,sizeof(*rects) * n);
   if (rects == PX_NULL)
      return 0;
   info.userdata = spc->user_allocator_context;
   PX_ttInitFont(mp, &info, fontdata, PX_ttGetFontOffsetForIndex(fontdata,font_index));
   n = PX_ttPackFontRangesGatherRects(spc, &info, ranges, num_ranges, rects);
   PX_ttPackFontRangesPackRects(spc, rects, n);
   return_value = PX_ttPackFontRangesRenderIntoRects(spc, &info, ranges, num_ranges, rects);
   MP_Free(mp,rects);
   return return_value;
}
px_int PX_ttPackFontRange(px_memorypool *mp,PX_ttpack_context *spc, const px_byte *fontdata, px_int font_index, px_float font_size,
            px_int first_unicode_codepoint_in_range, px_int num_chars_in_range, PX_ttpackedchar *chardata_for_range)
{
   PX_ttpack_range range;
   range.first_unicode_codepoint_in_range = first_unicode_codepoint_in_range;
   range.array_of_unicode_codepoints = PX_NULL;
   range.num_chars                   = num_chars_in_range;
   range.chardata_for_range          = chardata_for_range;
   range.font_size                   = font_size;
   return PX_ttPackFontRanges(mp,spc, fontdata, font_index, &range, 1);
}
px_void PX_ttGetScaledFontVMetrics(px_memorypool* mp, const px_byte *fontdata, px_int index, px_float size, px_float *ascent, px_float *descent, px_float *lineGap)
{
   px_int i_ascent, i_descent, i_lineGap;
   px_float scale;
   PX_ttfontinfo info;
   PX_ttInitFont(mp,&info, fontdata, PX_ttGetFontOffsetForIndex(fontdata, index));
   scale = size > 0 ? PX_ttScaleForPixelHeight(&info, size) : PX_ttScaleForMappingEmToPixels(&info, -size);
   PX_ttGetFontVMetrics(&info, &i_ascent, &i_descent, &i_lineGap);
   *ascent  = (px_float) i_ascent  * scale;
   *descent = (px_float) i_descent * scale;
   *lineGap = (px_float) i_lineGap * scale;
}
px_void PX_ttGetPackedQuad(const PX_ttpackedchar *chardata, px_int pw, px_int ph, px_int char_index, px_float *xpos, px_float *ypos, PX_ttaligned_quad *q, px_int align_to_integer)
{
   px_float ipw = 1.0f / pw, iph = 1.0f / ph;
   const PX_ttpackedchar *b = chardata + char_index;
   if (align_to_integer) {
      px_float x = (px_float) STBTT_ifloor((*xpos + b->xoff) + 0.5f);
      px_float y = (px_float) STBTT_ifloor((*ypos + b->yoff) + 0.5f);
      q->x0 = x;
      q->y0 = y;
      q->x1 = x + b->xoff2 - b->xoff;
      q->y1 = y + b->yoff2 - b->yoff;
   } else {
      q->x0 = *xpos + b->xoff;
      q->y0 = *ypos + b->yoff;
      q->x1 = *xpos + b->xoff2;
      q->y1 = *ypos + b->yoff2;
   }
   q->s0 = b->x0 * ipw;
   q->t0 = b->y0 * iph;
   q->s1 = b->x1 * ipw;
   q->t1 = b->y1 * iph;
   *xpos += b->xadvance;
}
#define STBTT_min(a,b)  ((a) < (b) ? (a) : (b))
#define STBTT_max(a,b)  ((a) < (b) ? (b) : (a))
px_int PX_tt_ray_intersect_bezier(px_float orig[2], px_float ray[2], px_float q0[2], px_float q1[2], px_float q2[2], px_float hits[2][2])
{
   px_float q0perp = q0[1]*ray[0] - q0[0]*ray[1];
   px_float q1perp = q1[1]*ray[0] - q1[0]*ray[1];
   px_float q2perp = q2[1]*ray[0] - q2[0]*ray[1];
   px_float roperp = orig[1]*ray[0] - orig[0]*ray[1];
   px_float a = q0perp - 2*q1perp + q2perp;
   px_float b = q1perp - q0perp;
   px_float c = q0perp - roperp;
   px_float s0 = 0., s1 = 0.;
   px_int num_s = 0;
   if (a != 0.0) {
      px_float discr = b*b - a*c;
      if (discr > 0.0) {
         px_float rcpna = -1 / a;
         px_float d = (px_float) STBTT_sqrt(discr);
         s0 = (b+d) * rcpna;
         s1 = (b-d) * rcpna;
         if (s0 >= 0.0 && s0 <= 1.0)
            num_s = 1;
         if (d > 0.0 && s1 >= 0.0 && s1 <= 1.0) {
            if (num_s == 0) s0 = s1;
            ++num_s;
         }
      }
   } else {
      
      
      s0 = c / (-2 * b);
      if (s0 >= 0.0 && s0 <= 1.0)
         num_s = 1;
   }
   if (num_s == 0)
      return 0;
   else {
      px_float rcp_len2 = 1 / (ray[0]*ray[0] + ray[1]*ray[1]);
      px_float rayn_x = ray[0] * rcp_len2, rayn_y = ray[1] * rcp_len2;
      px_float q0d =   q0[0]*rayn_x +   q0[1]*rayn_y;
      px_float q1d =   q1[0]*rayn_x +   q1[1]*rayn_y;
      px_float q2d =   q2[0]*rayn_x +   q2[1]*rayn_y;
      px_float rod = orig[0]*rayn_x + orig[1]*rayn_y;
      px_float q10d = q1d - q0d;
      px_float q20d = q2d - q0d;
      px_float q0rd = q0d - rod;
      hits[0][0] = q0rd + s0*(2.0f - 2.0f*s0)*q10d + s0*s0*q20d;
      hits[0][1] = a*s0+b;
      if (num_s > 1) {
         hits[1][0] = q0rd + s1*(2.0f - 2.0f*s1)*q10d + s1*s1*q20d;
         hits[1][1] = a*s1+b;
         return 2;
      } else {
         return 1;
      }
   }
}
px_int equal(px_float *a, px_float *b)
{
   return (a[0] == b[0] && a[1] == b[1]);
}
px_int PX_tt_compute_crossings_x(px_float x, px_float y, px_int nverts, PX_ttvertex *verts)
{
   px_int i;
   px_float orig[2], ray[2] = { 1, 0 };
   px_float y_frac;
   px_int winding = 0;
   
   y_frac = (px_float) STBTT_fmod(y, 1.0f);
   if (y_frac < 0.01f)
      y += 0.01f;
   else if (y_frac > 0.99f)
      y -= 0.01f;
   orig[0] = x;
   orig[1] = y;
   
   for (i=0; i < nverts; ++i) {
      if (verts[i].type == STBTT_vline) {
         px_int x0 = (px_int) verts[i-1].x, y0 = (px_int) verts[i-1].y;
         px_int x1 = (px_int) verts[i  ].x, y1 = (px_int) verts[i  ].y;
         if (y > STBTT_min(y0,y1) && y < STBTT_max(y0,y1) && x > STBTT_min(x0,x1)) {
            px_float x_inter = (y - y0) / (y1 - y0) * (x1-x0) + x0;
            if (x_inter < x)
               winding += (y0 < y1) ? 1 : -1;
         }
      }
      if (verts[i].type == STBTT_vcurve) {
         px_int x0 = (px_int) verts[i-1].x , y0 = (px_int) verts[i-1].y ;
         px_int x1 = (px_int) verts[i  ].cx, y1 = (px_int) verts[i  ].cy;
         px_int x2 = (px_int) verts[i  ].x , y2 = (px_int) verts[i  ].y ;
         px_int ax = STBTT_min(x0,STBTT_min(x1,x2)), ay = STBTT_min(y0,STBTT_min(y1,y2));
         px_int by = STBTT_max(y0,STBTT_max(y1,y2));
         if (y > ay && y < by && x > ax) {
            px_float q0[2],q1[2],q2[2];
            px_float hits[2][2];
            q0[0] = (px_float)x0;
            q0[1] = (px_float)y0;
            q1[0] = (px_float)x1;
            q1[1] = (px_float)y1;
            q2[0] = (px_float)x2;
            q2[1] = (px_float)y2;
            if (equal(q0,q1) || equal(q1,q2)) {
               x0 = (px_int)verts[i-1].x;
               y0 = (px_int)verts[i-1].y;
               x1 = (px_int)verts[i  ].x;
               y1 = (px_int)verts[i  ].y;
               if (y > STBTT_min(y0,y1) && y < STBTT_max(y0,y1) && x > STBTT_min(x0,x1)) {
                  px_float x_inter = (y - y0) / (y1 - y0) * (x1-x0) + x0;
                  if (x_inter < x)
                     winding += (y0 < y1) ? 1 : -1;
               }
            } else {
               px_int num_hits = PX_tt_ray_intersect_bezier(orig, ray, q0, q1, q2, hits);
               if (num_hits >= 1)
                  if (hits[0][0] < 0)
                     winding += (hits[0][1] < 0 ? -1 : 1);
               if (num_hits >= 2)
                  if (hits[1][0] < 0)
                     winding += (hits[1][1] < 0 ? -1 : 1);
            }
         }
      }
   }
   return winding;
}
px_float PX_tt_cuberoot( px_float x )
{
   if (x<0)
      return -(px_float) STBTT_pow(-x,1.0f/3.0f);
   else
      return  (px_float) STBTT_pow( x,1.0f/3.0f);
}
px_int PX_tt_solve_cubic(px_float a, px_float b, px_float c, px_float* r)
{
   px_float s = -a / 3;
   px_float p = b - a*a / 3;
   px_float q = a * (2*a*a - 9*b) / 27 + c;
   px_float p3 = p*p*p;
   px_float d = q*q + 4*p3 / 27;
   if (d >= 0) {
      px_float z = (px_float) STBTT_sqrt(d);
      px_float u = (-q + z) / 2;
      px_float v = (-q - z) / 2;
      u = PX_tt_cuberoot(u);
      v = PX_tt_cuberoot(v);
      r[0] = s + u + v;
      return 1;
   } else {
      px_float u = (px_float) STBTT_sqrt(-p/3);
      px_float v = (px_float) STBTT_acos(-STBTT_sqrt(-27/p3) * q / 2) / 3; 
      px_float m = (px_float) STBTT_cos(v);
      px_float n = (px_float) STBTT_cos(v-3.141592/2)*1.732050808f;
      r[0] = s + u * 2 * m;
      r[1] = s - u * (m + n);
      r[2] = s - u * (m - n);
      
      
      
      return 3;
   }
}
px_byte * PX_ttGetGlyphSDF(const PX_ttfontinfo *info, px_float scale, px_int glyph, px_int padding, px_byte onedge_value, px_float pixel_dist_scale, px_int *width, px_int *height, px_int *xoff, px_int *yoff)
{
   px_float scale_x = scale, scale_y = scale;
   px_int ix0,iy0,ix1,iy1;
   px_int w,h;
   px_byte *data;
   if (scale == 0) return PX_NULL;
   PX_ttGetGlyphBitmapBoxSubpixel(info, glyph, scale, scale, 0.0f,0.0f, &ix0,&iy0,&ix1,&iy1);
   
   if (ix0 == ix1 || iy0 == iy1)
      return PX_NULL;
   ix0 -= padding;
   iy0 -= padding;
   ix1 += padding;
   iy1 += padding;
   w = (ix1 - ix0);
   h = (iy1 - iy0);
   if (width ) *width  = w;
   if (height) *height = h;
   if (xoff  ) *xoff   = ix0;
   if (yoff  ) *yoff   = iy0;
   
   scale_y = -scale_y;
   {
      px_int x,y,i,j;
      px_float *precompute;
      PX_ttvertex *verts;
      px_int num_verts = PX_ttGetGlyphShape(info, glyph, &verts);
      data = (px_byte *) MP_Malloc(info->mp,w * h);
      precompute = (px_float *)MP_Malloc(info->mp, num_verts * sizeof(px_float));
      for (i=0,j=num_verts-1; i < num_verts; j=i++) {
         if (verts[i].type == STBTT_vline) {
            px_float x0 = verts[i].x*scale_x, y0 = verts[i].y*scale_y;
            px_float x1 = verts[j].x*scale_x, y1 = verts[j].y*scale_y;
            px_float dist = (px_float) STBTT_sqrt((x1-x0)*(x1-x0) + (y1-y0)*(y1-y0));
            precompute[i] = (dist == 0) ? 0.0f : 1.0f / dist;
         } else if (verts[i].type == STBTT_vcurve) {
            px_float x2 = verts[j].x *scale_x, y2 = verts[j].y *scale_y;
            px_float x1 = verts[i].cx*scale_x, y1 = verts[i].cy*scale_y;
            px_float x0 = verts[i].x *scale_x, y0 = verts[i].y *scale_y;
            px_float bx = x0 - 2*x1 + x2, by = y0 - 2*y1 + y2;
            px_float len2 = bx*bx + by*by;
            if (len2 != 0.0f)
               precompute[i] = 1.0f / (bx*bx + by*by);
            else
               precompute[i] = 0.0f;
         } else
            precompute[i] = 0.0f;
      }
      for (y=iy0; y < iy1; ++y) {
         for (x=ix0; x < ix1; ++x) {
            px_float val;
            px_float min_dist = 999999.0f;
            px_float sx = (px_float) x + 0.5f;
            px_float sy = (px_float) y + 0.5f;
            px_float x_gspace = (sx / scale_x);
            px_float y_gspace = (sy / scale_y);
            px_int winding = PX_tt_compute_crossings_x(x_gspace, y_gspace, num_verts, verts); 
            for (i=0; i < num_verts; ++i) {
               px_float x0 = verts[i].x*scale_x, y0 = verts[i].y*scale_y;
               if (verts[i].type == STBTT_vline && precompute[i] != 0.0f) {
                  px_float x1 = verts[i-1].x*scale_x, y1 = verts[i-1].y*scale_y;
                  px_float dist,dist2 = (x0-sx)*(x0-sx) + (y0-sy)*(y0-sy);
                  if (dist2 < min_dist*min_dist)
                     min_dist = (px_float) STBTT_sqrt(dist2);
                  
                  
                  
                  dist = (px_float) STBTT_fabs((x1-x0)*(y0-sy) - (y1-y0)*(x0-sx)) * precompute[i];
                  STBTT_assert(i != 0);
                  if (dist < min_dist) {
                     
                     
                     
                     px_float dx = x1-x0, dy = y1-y0;
                     px_float px = x0-sx, py = y0-sy;
                     
                     
                     px_float t = -(px*dx + py*dy) / (dx*dx + dy*dy);
                     if (t >= 0.0f && t <= 1.0f)
                        min_dist = dist;
                  }
               } else if (verts[i].type == STBTT_vcurve) {
                  px_float x2 = verts[i-1].x *scale_x, y2 = verts[i-1].y *scale_y;
                  px_float x1 = verts[i  ].cx*scale_x, y1 = verts[i  ].cy*scale_y;
                  px_float box_x0 = STBTT_min(STBTT_min(x0,x1),x2);
                  px_float box_y0 = STBTT_min(STBTT_min(y0,y1),y2);
                  px_float box_x1 = STBTT_max(STBTT_max(x0,x1),x2);
                  px_float box_y1 = STBTT_max(STBTT_max(y0,y1),y2);
                  
                  if (sx > box_x0-min_dist && sx < box_x1+min_dist && sy > box_y0-min_dist && sy < box_y1+min_dist) {
                     px_int num=0;
                     px_float ax = x1-x0, ay = y1-y0;
                     px_float bx = x0 - 2*x1 + x2, by = y0 - 2*y1 + y2;
                     px_float mx = x0 - sx, my = y0 - sy;
                     px_float res[3] = {0.f,0.f,0.f};
                     px_float px,py,t,it,dist2;
                     px_float a_inv = precompute[i];
                     if (a_inv == 0.0) { 
                        px_float a = 3*(ax*bx + ay*by);
                        px_float b = 2*(ax*ax + ay*ay) + (mx*bx+my*by);
                        px_float c = mx*ax+my*ay;
                        if (a == 0.0) { 
                           if (b != 0.0) {
                              res[num++] = -c/b;
                           }
                        } else {
                           px_float discriminant = b*b - 4*a*c;
                           if (discriminant < 0)
                              num = 0;
                           else {
                              px_float root = (px_float) STBTT_sqrt(discriminant);
                              res[0] = (-b - root)/(2*a);
                              res[1] = (-b + root)/(2*a);
                              num = 2; 
                           }
                        }
                     } else {
                        px_float b = 3*(ax*bx + ay*by) * a_inv; 
                        px_float c = (2*(ax*ax + ay*ay) + (mx*bx+my*by)) * a_inv;
                        px_float d = (mx*ax+my*ay) * a_inv;
                        num = PX_tt_solve_cubic(b, c, d, res);
                     }
                     dist2 = (x0-sx)*(x0-sx) + (y0-sy)*(y0-sy);
                     if (dist2 < min_dist*min_dist)
                        min_dist = (px_float) STBTT_sqrt(dist2);
                     if (num >= 1 && res[0] >= 0.0f && res[0] <= 1.0f) {
                        t = res[0], it = 1.0f - t;
                        px = it*it*x0 + 2*t*it*x1 + t*t*x2;
                        py = it*it*y0 + 2*t*it*y1 + t*t*y2;
                        dist2 = (px-sx)*(px-sx) + (py-sy)*(py-sy);
                        if (dist2 < min_dist * min_dist)
                           min_dist = (px_float) STBTT_sqrt(dist2);
                     }
                     if (num >= 2 && res[1] >= 0.0f && res[1] <= 1.0f) {
                        t = res[1], it = 1.0f - t;
                        px = it*it*x0 + 2*t*it*x1 + t*t*x2;
                        py = it*it*y0 + 2*t*it*y1 + t*t*y2;
                        dist2 = (px-sx)*(px-sx) + (py-sy)*(py-sy);
                        if (dist2 < min_dist * min_dist)
                           min_dist = (px_float) STBTT_sqrt(dist2);
                     }
                     if (num >= 3 && res[2] >= 0.0f && res[2] <= 1.0f) {
                        t = res[2], it = 1.0f - t;
                        px = it*it*x0 + 2*t*it*x1 + t*t*x2;
                        py = it*it*y0 + 2*t*it*y1 + t*t*y2;
                        dist2 = (px-sx)*(px-sx) + (py-sy)*(py-sy);
                        if (dist2 < min_dist * min_dist)
                           min_dist = (px_float) STBTT_sqrt(dist2);
                     }
                  }
               }
            }
            if (winding == 0)
               min_dist = -min_dist;  
            val = onedge_value + pixel_dist_scale * min_dist;
            if (val < 0)
               val = 0;
            else if (val > 255)
               val = 255;
            data[(y-iy0)*w+(x-ix0)] = (px_byte) val;
         }
      }
      MP_Free(info->mp,precompute);
      MP_Free(info->mp, verts);
   }
   return data;
}
px_byte * PX_ttGetCodepointSDF(const PX_ttfontinfo *info, px_float scale, px_int codepoint, px_int padding, px_byte onedge_value, px_float pixel_dist_scale, px_int *width, px_int *height, px_int *xoff, px_int *yoff)
{
   return PX_ttGetGlyphSDF(info, scale, PX_ttFindGlyphIndex(info, codepoint), padding, onedge_value, pixel_dist_scale, width, height, xoff, yoff);
}
px_void PX_ttFreeSDF(px_memorypool *mp,px_byte *bitmap)
{
    MP_Free(mp,bitmap);
}
px_int32 PX_tt_CompareUTF8toUTF16_bigendian_prefix(px_byte *s1, px_int32 len1, px_byte *s2, px_int32 len2)
{
   px_int32 i=0;
   
   while (len2) {
      px_ushort ch = s2[0]*256 + s2[1];
      if (ch < 0x80) {
         if (i >= len1) return -1;
         if (s1[i++] != ch) return -1;
      } else if (ch < 0x800) {
         if (i+1 >= len1) return -1;
         if (s1[i++] != 0xc0 + (ch >> 6)) return -1;
         if (s1[i++] != 0x80 + (ch & 0x3f)) return -1;
      } else if (ch >= 0xd800 && ch < 0xdc00) {
         px_uint c;
         px_ushort ch2 = s2[2]*256 + s2[3];
         if (i+3 >= len1) return -1;
         c = ((ch - 0xd800) << 10) + (ch2 - 0xdc00) + 0x10000;
         if (s1[i++] != 0xf0 + (c >> 18)) return -1;
         if (s1[i++] != 0x80 + ((c >> 12) & 0x3f)) return -1;
         if (s1[i++] != 0x80 + ((c >>  6) & 0x3f)) return -1;
         if (s1[i++] != 0x80 + ((c      ) & 0x3f)) return -1;
         s2 += 2; 
         len2 -= 2;
      } else if (ch >= 0xdc00 && ch < 0xe000) {
         return -1;
      } else {
         if (i+2 >= len1) return -1;
         if (s1[i++] != 0xe0 + (ch >> 12)) return -1;
         if (s1[i++] != 0x80 + ((ch >> 6) & 0x3f)) return -1;
         if (s1[i++] != 0x80 + ((ch     ) & 0x3f)) return -1;
      }
      s2 += 2;
      len2 -= 2;
   }
   return i;
}
px_int PX_ttCompareUTF8toUTF16_bigendian_internal(char *s1, px_int len1, char *s2, px_int len2)
{
   return len1 == PX_tt_CompareUTF8toUTF16_bigendian_prefix((px_byte*) s1, len1, (px_byte*) s2, len2);
}
const char *PX_ttGetFontNameString(const PX_ttfontinfo *font, px_int *length, px_int platformID, px_int encodingID, px_int languageID, px_int nameID)
{
   px_int32 i,count,stringOffset;
   px_byte *fc = font->data;
   px_uint offset = font->fontstart;
   px_uint nm = PX_tt_find_table(fc, offset, "name");
   if (!nm) return PX_NULL;
   count = ttUSHORT(fc+nm+2);
   stringOffset = nm + ttUSHORT(fc+nm+4);
   for (i=0; i < count; ++i) {
      px_uint loc = nm + 6 + 12 * i;
      if (platformID == ttUSHORT(fc+loc+0) && encodingID == ttUSHORT(fc+loc+2)
          && languageID == ttUSHORT(fc+loc+4) && nameID == ttUSHORT(fc+loc+6)) {
         *length = ttUSHORT(fc+loc+8);
         return (const char *) (fc+stringOffset+ttUSHORT(fc+loc+10));
      }
   }
   return PX_NULL;
}
px_int PX_tt_matchpair(px_byte *fc, px_uint nm, px_byte *name, px_int32 nlen, px_int32 target_id, px_int32 next_id)
{
   px_int32 i;
   px_int32 count = ttUSHORT(fc+nm+2);
   px_int32 stringOffset = nm + ttUSHORT(fc+nm+4);
   for (i=0; i < count; ++i) {
      px_uint loc = nm + 6 + 12 * i;
      px_int32 id = ttUSHORT(fc+loc+6);
      if (id == target_id) {
         
         px_int32 platform = ttUSHORT(fc+loc+0), encoding = ttUSHORT(fc+loc+2), language = ttUSHORT(fc+loc+4);
         
         if (platform == 0 || (platform == 3 && encoding == 1) || (platform == 3 && encoding == 10)) {
            px_int32 slen = ttUSHORT(fc+loc+8);
            px_int32 off = ttUSHORT(fc+loc+10);
            
            px_int32 matchlen = PX_tt_CompareUTF8toUTF16_bigendian_prefix(name, nlen, fc+stringOffset+off,slen);
            if (matchlen >= 0) {
               
               if (i+1 < count && ttUSHORT(fc+loc+12+6) == next_id && ttUSHORT(fc+loc+12) == platform && ttUSHORT(fc+loc+12+2) == encoding && ttUSHORT(fc+loc+12+4) == language) {
                  slen = ttUSHORT(fc+loc+12+8);
                  off = ttUSHORT(fc+loc+12+10);
                  if (slen == 0) {
                     if (matchlen == nlen)
                        return 1;
                  } else if (matchlen < nlen && name[matchlen] == ' ') {
                     ++matchlen;
                     if (PX_ttCompareUTF8toUTF16_bigendian_internal((char*) (name+matchlen), nlen-matchlen, (char*)(fc+stringOffset+off),slen))
                        return 1;
                  }
               } else {
                  
                  if (matchlen == nlen)
                     return 1;
               }
            }
         }
         
      }
   }
   return 0;
}
px_int PX_tt_matches(px_byte *fc, px_uint offset, px_byte *name, px_int32 flags)
{
   px_int32 nlen = (px_int32) STBTT_strlen((char *) name);
   px_uint nm,hd;
   if (!PX_tt_isfont(fc+offset)) return 0;
   
   if (flags) {
      hd = PX_tt_find_table(fc, offset, "head");
      if ((ttUSHORT(fc+hd+44) & 7) != (flags & 7)) return 0;
   }
   nm = PX_tt_find_table(fc, offset, "name");
   if (!nm) return 0;
   if (flags) {
      
      if (PX_tt_matchpair(fc, nm, name, nlen, 16, -1))  return 1;
      if (PX_tt_matchpair(fc, nm, name, nlen,  1, -1))  return 1;
      if (PX_tt_matchpair(fc, nm, name, nlen,  3, -1))  return 1;
   } else {
      if (PX_tt_matchpair(fc, nm, name, nlen, 16, 17))  return 1;
      if (PX_tt_matchpair(fc, nm, name, nlen,  1,  2))  return 1;
      if (PX_tt_matchpair(fc, nm, name, nlen,  3, -1))  return 1;
   }
   return 0;
}
px_int PX_ttFindMatchingFont_internal(px_byte *font_collection, char *name_utf8, px_int32 flags)
{
   px_int32 i;
   for (i=0;;++i) {
      px_int32 off = PX_ttGetFontOffsetForIndex(font_collection, i);
      if (off < 0) return off;
      if (PX_tt_matches((px_byte *) font_collection, off, (px_byte*) name_utf8, flags))
         return off;
   }
}
#if defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-qual"
#endif
px_int PX_ttBakeFontBitmap(px_memorypool *mp,const px_byte *data, px_int offset,
                                px_float pixel_height, px_byte *pixels, px_int pw, px_int ph,
                                px_int first_char, px_int num_chars, PX_ttbakedchar *chardata)
{
   return PX_ttBakeFontBitmap_internal(mp, (px_byte *) data, offset, pixel_height, pixels, pw, ph, first_char, num_chars, chardata);
}
px_int PX_ttGetFontOffsetForIndex(const px_byte *data, px_int index)
{
   return PX_ttGetFontOffsetForIndex_internal((px_byte *) data, index);
}
px_int PX_ttGetNumberOfFonts(const px_byte *data)
{
   return PX_ttGetNumberOfFonts_internal((px_byte *) data);
}
px_int PX_ttInitFont(px_memorypool* mp, PX_ttfontinfo *info, const px_byte *data, px_int offset)
{
   return PX_ttInitFont_internal(mp, info, (px_byte *) data, offset);
}
px_int PX_ttFindMatchingFont(const px_byte *fontdata, const char *name, px_int flags)
{
   return PX_ttFindMatchingFont_internal((px_byte *) fontdata, (char *) name, flags);
}
px_int PX_ttCompareUTF8toUTF16_bigendian(const char *s1, px_int len1, const char *s2, px_int len2)
{
   return PX_ttCompareUTF8toUTF16_bigendian_internal((char *) s1, len1, (char *) s2, len2);
}
#if defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic pop
#endif
