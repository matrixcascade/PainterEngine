#ifndef GIFDEC_H
#define GIFDEC_H

#include "PX_Texture.h"


typedef struct px_gif_Palette 
{
    px_int size;
    px_byte colors[0x100 * 3];
} px_gif_Palette;

typedef struct px_gif_GCE 
{
    px_ushort delay;
    px_byte tindex;
    px_byte disposal;
    px_int input;
    px_int transparency;
} px_gif_GCE;

typedef struct _px_gif
{
    px_memorypool* mp;
    px_memory data;
    px_int roffset;
    px_int anim_start;
    px_ushort width, height;
    px_ushort depth;
    px_ushort loop_count;
    px_gif_GCE gce;
    px_gif_Palette *palette;
    px_gif_Palette lct, gct;
    px_void (*plain_text)(struct _px_gif*gif, px_ushort tx, px_ushort ty,px_ushort tw, px_ushort th, px_byte cw, px_byte ch,px_byte fg, px_byte bg);
    px_void (*comment)(struct _px_gif*gif);
    px_void (*application)(struct _px_gif*gif, px_char id[8], px_char auth[3]);
    px_ushort fx, fy, fw, fh;
    px_byte bgindex;
    px_byte* canvas,*frame;
    px_texture texture;
    px_bool loop;
    px_dword elapsed;
} px_gif;

px_bool PX_GifCreate(px_memorypool* mp, px_gif* pgif,px_void *buffer,px_int size);
px_void PX_GifUpdate(px_gif *gif,px_dword elapsed);
px_void PX_GifReset(px_gif *gif);
px_void PX_GifFree(px_gif *gif);
px_void PX_GifSetLoop(px_gif *gif,px_bool loop);
px_texture *PX_GifGetTexture(px_gif *gif);


#endif /* GIFDEC_H */
