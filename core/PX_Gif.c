#include "PX_Gif.h"


#define PX_GIF_MIN(A, B) ((A) < (B) ? (A) : (B))
#define PX_GIF_MAX(A, B) ((A) > (B) ? (A) : (B))

typedef struct 
{
    px_ushort length;
    px_ushort prefix;
    px_byte  suffix;
} px_gif_Entry;

typedef struct 
{
    px_int bulk;
    px_int nentries;
    px_gif_Entry *entries;
} px_gif_Table;

static px_ushort px_gif_read_num(px_byte *buffer)
{
    return buffer[0] + (((px_ushort)buffer[1]) << 8);
}

px_bool PX_GifCreate(px_memorypool* mp, px_gif* gif, px_void* buffer, px_int size)
{
    px_byte *pbyteBuffer=(px_byte*)buffer,sigver[3];
    px_ushort width=0, height=0, depth=0;
    px_byte fdsz=0, bgidx=0, aspect=0;
    px_int i;
    px_byte *bgcolor=PX_NULL;
    px_int gct_sz=0;
    px_int read_offset = 0;

    PX_memcpy(sigver, pbyteBuffer+ read_offset, 3);
    read_offset+= 3;
    if (PX_memequ(sigver, "GIF", 3) == PX_FALSE) 
    {
        PX_LOG("invalid gif file");
        return PX_FALSE;
    }

    PX_memcpy(sigver, pbyteBuffer + read_offset, 3);
    read_offset += 3;
    if (PX_memequ(sigver, "89a", 3) == PX_FALSE)
    {
        PX_LOG("only support 89a");
        return PX_FALSE;
    }

    /* Width x Height */
    width  = px_gif_read_num(pbyteBuffer + read_offset);
    read_offset += 2;
    height = px_gif_read_num(pbyteBuffer + read_offset);
    read_offset += 2;

    /* FDSZ */
    PX_memcpy(&fdsz, pbyteBuffer + read_offset, 1);
    read_offset += 1;
    /* Presence of GCT */
    if (!(fdsz & 0x80)) {
        PX_LOG("no global color table");
        return PX_FALSE;
    }
    /* Color Space's Depth */
    depth = ((fdsz >> 4) & 7) + 1;
    /* Ignore Sort Flag. */
    /* GCT Size */
    gct_sz = 1 << ((fdsz & 0x07) + 1);
    /* Background Color Index */
  
    PX_memcpy(&bgidx, pbyteBuffer + read_offset, 1);
    read_offset += 1;
    /* Aspect Ratio */
    PX_memcpy(&aspect, pbyteBuffer + read_offset, 1);
    read_offset += 1;

    /* Create px_gif_GIF Structure. */
    PX_memset(gif, 0, sizeof(px_gif));
    gif->mp = mp;
    PX_MemoryInitialize(mp, &gif->data);
    if (!PX_MemoryCat(&gif->data, pbyteBuffer, size))
    {
        return PX_FALSE;
    }

    gif->width  = width;
    gif->height = height;
    gif->depth  = depth;
    /* Read GCT */
    gif->gct.size = gct_sz;
    
    PX_memcpy(gif->gct.colors, pbyteBuffer + read_offset, 3 * gif->gct.size);
    read_offset += 3 * gif->gct.size;

    gif->palette = &gif->gct;
    gif->bgindex = bgidx;
   
    gif->frame = (px_byte *)MP_Malloc(mp,4* width * height);
    if (!gif->frame) 
    {
        PX_MemoryFree(&gif->data);
        return PX_FALSE;
    }
    gif->canvas = &gif->frame[width * height];

    if (gif->bgindex)
        PX_memset(gif->frame, gif->bgindex, gif->width * gif->height);

    bgcolor = &gif->palette->colors[gif->bgindex*3];

    if (bgcolor[0] || bgcolor[1] || bgcolor [2])
        for (i = 0; i < gif->width * gif->height; i++)
            PX_memcpy(&gif->canvas[i*3], bgcolor, 3);

    gif->anim_start = read_offset;
    gif->roffset = read_offset;

    //create texture
    if (!PX_TextureCreate(mp,  &gif->texture, gif->width, gif->height))
    {
        PX_MemoryFree(&gif->data);
        MP_Free(mp, gif->frame);
        return PX_FALSE;
    }
    
    return PX_TRUE;
}

static px_void px_gif_discard_sub_blocks(px_gif *gif)
{
    px_byte size;

    do {
        size = gif->data.buffer[gif->roffset];
        gif->roffset++;
        gif->roffset += size;
    } while (size);
}

static px_void px_gif_read_plain_text_ext(px_gif *gif)
{
    if (gif->plain_text) 
    {
        px_ushort tx=0, ty=0, tw=0, th=0;
        px_byte cw=0, ch=0, fg=0, bg=0;
        px_int sub_block;
        //lseek(gif->fd, 1, SEEK_CUR); /* block size = 12 */
        gif->roffset++;
        tx = px_gif_read_num(gif->data.buffer+gif->roffset);
        gif->roffset += 2;
        ty = px_gif_read_num(gif->data.buffer + gif->roffset);
        gif->roffset += 2;
        tw = px_gif_read_num(gif->data.buffer + gif->roffset);
        gif->roffset += 2;
        th = px_gif_read_num(gif->data.buffer + gif->roffset);
        gif->roffset += 2;
       // read(gif->fd, &cw, 1);
        PX_memcpy(&cw, gif->data.buffer + gif->roffset, 1);
        gif->roffset++;
	   // read(gif->fd, &ch, 1);
        PX_memcpy(&ch, gif->data.buffer + gif->roffset, 1);
        gif->roffset++;
	   // read(gif->fd, &fg, 1);
        PX_memcpy(&fg, gif->data.buffer + gif->roffset, 1);
        gif->roffset++;
	   // read(gif->fd, &bg, 1);
        PX_memcpy(&bg, gif->data.buffer + gif->roffset, 1);
        gif->roffset++;
		sub_block = gif->roffset;
		gif->plain_text(gif, tx, ty, tw, th, cw, ch, fg, bg);
		gif->roffset = sub_block;

    } 
    else 
    {
        /* Discard plain text metadata. */
        gif->roffset += 13;
    }
    /* Discard plain text sub-blocks. */
    px_gif_discard_sub_blocks(gif);
}

static px_void px_gif_read_graphic_control_ext(px_gif *gif)
{
    px_byte rdit=0;

    /* Discard block size (always 0x04). */
    gif->roffset++;
    //lseek(gif->fd, 1, SEEK_CUR);
    //read(gif->fd, &rdit, 1);
    PX_memcpy(&rdit, gif->data.buffer + gif->roffset, 1);
	gif->roffset++;
    gif->gce.disposal = (rdit >> 2) & 3;
    gif->gce.input = rdit & 2;
    gif->gce.transparency = rdit & 1;
    gif->gce.delay = px_gif_read_num(gif->data.buffer+gif->roffset);
gif->roffset += 2;
    //read(gif->fd, &gif->gce.tindex, 1);
    PX_memcpy(&gif->gce.tindex, gif->data.buffer + gif->roffset, 1);
    gif->roffset++;
    /* Skip block terminator. */
    //lseek(gif->fd, 1, SEEK_CUR);
	gif->roffset++;
}

static px_void px_gif_read_comment_ext(px_gif *gif)
{
    if (gif->comment)
    {
        px_int sub_block=0;
        //px_int sub_block = lseek(gif->fd, 0, SEEK_CUR);
        sub_block = gif->roffset;
        gif->comment(gif);
        //lseek(gif->fd, sub_block, SEEK_SET);
        gif->roffset = sub_block;
    }
    /* Discard comment sub-blocks. */
    px_gif_discard_sub_blocks(gif);
}

static px_void read_application_ext(px_gif *gif)
{
    px_char app_id[8] = {0};
    px_char app_auth_code[3] = {0};

    /* Discard block size (always 0x0B). */
    //lseek(gif->fd, 1, SEEK_CUR);
    gif->roffset++;
    /* Application Identifier. */
    //read(gif->fd, app_id, 8);
	PX_memcpy(app_id, gif->data.buffer + gif->roffset, 8);
    gif->roffset += 8;
    /* Application Authentication Code. */
    //read(gif->fd, app_auth_code, 3);
	PX_memcpy(app_auth_code, gif->data.buffer + gif->roffset, 3);
    gif->roffset += 3;

    if (PX_memequ(app_id, "NETSCAPE", sizeof(app_id))) 
    {
        /* Discard block size (0x03) and constant byte (0x01). */
        //lseek(gif->fd, 2, SEEK_CUR);
        gif->roffset += 2;
        gif->loop_count = px_gif_read_num(gif->data.buffer+gif->roffset);
        gif->roffset += 2;
        /* Skip block terminator. */
        //lseek(gif->fd, 1, SEEK_CUR);
        gif->roffset++;
    } 
    else if (gif->application) 
    {
        px_int sub_block = gif->roffset;
        gif->application(gif, app_id, app_auth_code);
        //lseek(gif->fd, sub_block, SEEK_SET);
        gif->roffset = sub_block;
        px_gif_discard_sub_blocks(gif);
    } else 
    {
        px_gif_discard_sub_blocks(gif);
    }
}

static px_void read_ext(px_gif *gif)
{
    px_byte label;

    //read(gif->fd, &label, 1);
    PX_memcpy(&label, gif->data.buffer + gif->roffset, 1);
    gif->roffset++;
    switch (label) {
    case 0x01:
        px_gif_read_plain_text_ext(gif);
        break;
    case 0xF9:
        px_gif_read_graphic_control_ext(gif);
        break;
    case 0xFE:
        px_gif_read_comment_ext(gif);
        break;
    case 0xFF:
        read_application_ext(gif);
        break;
    default:
        PX_LOG("unknown extension");
    }
}

static px_gif_Table *new_table(px_gif* gif, px_int key_size)
{
    px_int key=0;
    px_int init_bulk = PX_GIF_MAX(1 << (key_size + 1), 0x100);
    px_gif_Table *table = (px_gif_Table *)MP_Malloc(gif->mp, sizeof(*table) + sizeof(px_gif_Entry) * init_bulk);
    if (table) {
        table->bulk = init_bulk;
        table->nentries = (1 << key_size) + 2;
        table->entries = (px_gif_Entry *) &table[1];
        for (key = 0; key < (1 << key_size); key++)
        {
            table->entries[key].length = 1;
            table->entries[key].prefix = 0xFFF;
            table->entries[key].suffix = (px_byte) key;
        }
    }
    return table;
}

/* Add table entry. Return value:
 *  0 on success
 *  +1 if key size must be incremented after this addition
 *  -1 if could not realloc table */
static px_int add_entry(px_memorypool *mp,px_gif_Table **tablep, px_ushort length, px_ushort prefix, px_byte suffix)
{
    px_gif_Table *table = *tablep;
    px_gif_Table *new_table=PX_NULL;
    if (table->nentries == table->bulk) {
        table->bulk *= 2;

        new_table = (px_gif_Table *)MP_Malloc(mp, sizeof(*table) + sizeof(px_gif_Entry) * table->bulk);
        if (!new_table) return -1;
		PX_memcpy(new_table, table, sizeof(*table) + sizeof(px_gif_Entry) * table->nentries);
        MP_Free(mp, table);
        table = new_table;
        
        if (!table) return -1;
        table->entries = (px_gif_Entry *) &table[1];
        *tablep = table;
    }
    table->entries[table->nentries].length = length;
    table->entries[table->nentries].prefix = prefix;
    table->entries[table->nentries].suffix = suffix;

    table->nentries++;
    if ((table->nentries & (table->nentries - 1)) == 0)
        return 1;
    return 0;
}

static px_ushort get_key(px_gif *gif, px_int key_size, px_byte *sub_len, px_byte *shift, px_byte *byte)
{
    px_int bits_read=0;
    px_int rpad=0;
    px_int frag_size=0;
    px_ushort key=0;

    key = 0;
    for (bits_read = 0; bits_read < key_size; bits_read += frag_size) {
        rpad = (*shift + bits_read) % 8;
        if (rpad == 0) {
            /* Update byte. */
            if (*sub_len == 0) {
                //read(gif->fd, sub_len, 1); /* Must be nonzero! */
                PX_memcpy(sub_len, gif->data.buffer + gif->roffset, 1);
                gif->roffset++;
                if (*sub_len == 0)
                    return 0x1000;
            }
            //read(gif->fd, byte, 1);
            PX_memcpy(byte, gif->data.buffer + gif->roffset, 1);
			gif->roffset++;
            (*sub_len)--;
        }
        frag_size = PX_GIF_MIN(key_size - bits_read, 8 - rpad);
        key |= ((px_ushort) ((*byte) >> rpad)) << bits_read;
    }
    /* Clear extra bits to the left. */
    key &= (1 << key_size) - 1;
    *shift = (*shift + key_size) % 8;
    return key;
}

/* Compute output index of y-th input line, in frame of height h. */
static px_int interlaced_line_index(px_int h, px_int y)
{
    px_int p; /* number of lines in current pass */

    p = (h - 1) / 8 + 1;
    if (y < p) /* pass 1 */
        return y * 8;
    y -= p;
    p = (h - 5) / 8 + 1;
    if (y < p) /* pass 2 */
        return y * 8 + 4;
    y -= p;
    p = (h - 3) / 4 + 1;
    if (y < p) /* pass 3 */
        return y * 4 + 2;
    y -= p;
    /* pass 4 */
    return y * 2 + 1;
}

/* Decompress image pixels.
 * Return 0 on success or -1 on out-of-memory (w.r.t. LZW code table). */
static px_int read_image_data(px_gif *gif, px_int interlace)
{
    px_byte sub_len=0, shift=0, byte=0;
    px_int init_key_size=0, key_size=0, table_is_full=0;
    px_int frm_off=0, frm_size=0, str_len=0, i, p, x, y;
    px_ushort key=0, clear=0, stop=0;
    px_int ret=0;
    px_gif_Table *table=0;
    px_gif_Entry entry = {0};
    px_int start, end;

    //read(gif->fd, &byte, 1);
    PX_memcpy(&byte, gif->data.buffer + gif->roffset, 1);
    gif->roffset++;
    
    key_size = (px_int) byte;
    if (key_size < 2 || key_size > 8)
        return -1;
    
    //start = lseek(gif->fd, 0, SEEK_CUR);
    start = gif->roffset;
    px_gif_discard_sub_blocks(gif);
    //end = lseek(gif->fd, 0, SEEK_CUR);
    end = gif->roffset;
    //lseek(gif->fd, start, SEEK_SET);
    gif->roffset = start;
    clear = 1 << key_size;
    stop = clear + 1;
    table = new_table(gif, key_size);
    key_size++;
    init_key_size = key_size;
    sub_len = shift = 0;
    key = get_key(gif, key_size, &sub_len, &shift, &byte); /* clear code */
    frm_off = 0;
    ret = 0;
    frm_size = gif->fw*gif->fh;
    while (frm_off < frm_size) {
        if (key == clear) {
            key_size = init_key_size;
            table->nentries = (1 << (key_size - 1)) + 2;
            table_is_full = 0;
        } else if (!table_is_full) {
            ret = add_entry(gif->mp, &table, str_len + 1, key, entry.suffix);
            if (ret == -1) {
                MP_Free(gif->mp, table);
                return -1;
            }
            if (table->nentries == 0x1000) {
                ret = 0;
                table_is_full = 1;
            }
        }
        key = get_key(gif, key_size, &sub_len, &shift, &byte);
        if (key == clear) continue;
        if (key == stop || key == 0x1000) break;
        if (ret == 1) key_size++;
        entry = table->entries[key];
        str_len = entry.length;
        for (i = 0; i < str_len; i++) {
            p = frm_off + entry.length - 1;
            x = p % gif->fw;
            y = p / gif->fw;
            if (interlace)
                y = interlaced_line_index((px_int) gif->fh, y);
            gif->frame[(gif->fy + y) * gif->width + gif->fx + x] = entry.suffix;
            if (entry.prefix == 0xFFF)
                break;
            else
                entry = table->entries[entry.prefix];
        }
        frm_off += str_len;
        if (key < table->nentries - 1 && !table_is_full)
            table->entries[table->nentries - 1].suffix = entry.suffix;
    }
    MP_Free(gif->mp, table);
    if (key == stop)
    {
        //read(gif->fd, &sub_len, 1); /* Must be zero! */
        PX_memcpy(&sub_len, gif->data.buffer + gif->roffset, 1);
        gif->roffset++;
		if (sub_len != 0)
			return -1;
    }
   // lseek(gif->fd, end, SEEK_SET);
    gif->roffset = end;
    return 0;
}

/* Read image.
 * Return 0 on success or -1 on out-of-memory (w.r.t. LZW code table). */
static px_int read_image(px_gif *gif)
{
    px_byte fisrz=0;
    px_int interlace=0;

    /* Image Descriptor. */
    //gif->fx = px_gif_read_num(gif->fd);
    PX_memcpy(&gif->fx, gif->data.buffer + gif->roffset, 2);
    gif->roffset += 2;
    //gif->fy = px_gif_read_num(gif->fd);
	PX_memcpy(&gif->fy, gif->data.buffer + gif->roffset, 2);
    gif->roffset += 2;

    
    if (gif->fx >= gif->width || gif->fy >= gif->height)
        return -1;
    
    //gif->fw = px_gif_read_num(gif->fd);
    //gif->fh = px_gif_read_num(gif->fd);
    PX_memcpy(&gif->fw, gif->data.buffer + gif->roffset, 2);
    gif->roffset += 2;
    PX_memcpy(&gif->fh, gif->data.buffer + gif->roffset, 2);
    gif->roffset += 2;

    
    gif->fw = PX_GIF_MIN(gif->fw, gif->width - gif->fx);
    gif->fh = PX_GIF_MIN(gif->fh, gif->height - gif->fy);
    
   // read(gif->fd, &fisrz, 1);
    PX_memcpy(&fisrz, gif->data.buffer + gif->roffset, 1);
	gif->roffset++;
    interlace = fisrz & 0x40;
    /* Ignore Sort Flag. */
    /* Local Color Table? */
    if (fisrz & 0x80) 
    {
        /* Read LCT */
        gif->lct.size = 1 << ((fisrz & 0x07) + 1);
        //read(gif->fd, gif->lct.colors, 3 * gif->lct.size);
        PX_memcpy(gif->lct.colors, gif->data.buffer + gif->roffset, 3 * gif->lct.size);
        gif->roffset += 3 * gif->lct.size;
        gif->palette = &gif->lct;
    } else
        gif->palette = &gif->gct;
    /* Image Data. */
    return read_image_data(gif, interlace);
}

static px_void render_frame_rect(px_gif *gif, px_byte *buffer)
{
    px_int i, j, k;
    px_byte index, *color;
    i = gif->fy * gif->width + gif->fx;
    for (j = 0; j < gif->fh; j++) {
        for (k = 0; k < gif->fw; k++) {
            index = gif->frame[(gif->fy + j) * gif->width + gif->fx + k];
            color = &gif->palette->colors[index*3];
            if (!gif->gce.transparency || index != gif->gce.tindex)
                PX_memcpy(&buffer[(i+k)*3], color, 3);
        }
        i += gif->width;
    }
}

static px_void render_frame_texture(px_gif* gif, px_texture* ptexture)
{
    px_int i, j, k;
    px_byte index, * color;
    i = gif->fy * gif->width + gif->fx;
    for (j = 0; j < gif->fh; j++) {
        for (k = 0; k < gif->fw; k++) {
            index = gif->frame[(gif->fy + j) * gif->width + gif->fx + k];
            color = &gif->palette->colors[index * 3];
            if (!gif->gce.transparency || index != gif->gce.tindex)
            {
                PX_SurfaceSetPixel(ptexture, k, j, PX_COLOR(255, color[0], color[1], color[2]));
            }
        }
        i += gif->width;
    }
}

static px_void dispose(px_gif *gif)
{
    px_int i, j, k;
    px_byte *bgcolor=0;
    switch (gif->gce.disposal) {
    case 2: /* Restore to background color. */
        bgcolor = &gif->palette->colors[gif->bgindex*3];
        i = gif->fy * gif->width + gif->fx;
        for (j = 0; j < gif->fh; j++) {
            for (k = 0; k < gif->fw; k++)
                PX_memcpy(&gif->canvas[(i+k)*3], bgcolor, 3);
            i += gif->width;
        }
        break;
    case 3: /* Restore to previous, i.e., don't update canvas.*/
        break;
    default:
        /* Add frame non-transparent pixels to canvas. */
        render_frame_rect(gif, gif->canvas);
    }
}

px_void px_gif_render_to_frame_texture(px_gif* gif, px_texture* ptexture)
{
    px_int x, y;
    //PX_memcpy(buffer, gif->canvas, gif->width * gif->height * 3);
    for (y = 0; y < gif->height; y++)
    {
        for (x = 0; x < gif->width; x++)
        {
            PX_SurfaceSetPixel(ptexture, x, y, PX_COLOR(255, gif->canvas[(y * gif->width + x) * 3], gif->canvas[(y * gif->width + x) * 3 + 1], gif->canvas[(y * gif->width + x) * 3 + 2]));
        }
    }
    render_frame_texture(gif, ptexture);
}


/* Return 1 if got a frame; 0 if got GIF trailer; -1 if error. */
px_int px_gif_get_frame(px_gif *gif)
{
    px_char sep=0;

    dispose(gif);
    //read(gif->fd, &sep, 1);
    PX_memcpy(&sep, gif->data.buffer + gif->roffset, 1);
    gif->roffset++;
    while (sep != ',') {
        if (sep == ';')
            return 0;
        if (sep == '!')
            read_ext(gif);
        else return -1;
        //read(gif->fd, &sep, 1);
        PX_memcpy(&sep, gif->data.buffer + gif->roffset, 1);
        gif->roffset++;
    }
    if (read_image(gif) == -1)
        return -1;

    px_gif_render_to_frame_texture(gif, &gif->texture);
    return 1;
}

px_dword px_gif_get_delay(px_gif* gif)
{
    return gif->gce.delay;
}

px_void px_gif_rewind(px_gif* gif)
{
    //lseek(gif->fd, gif->anim_start, SEEK_SET);
    gif->roffset = gif->anim_start;
    gif->elapsed = 0;
}


px_void PX_GifUpdate(px_gif* gif, px_dword elapsed)
{
    gif->elapsed += elapsed;
    while (gif->elapsed>=(px_dword)gif->gce.delay*10)
    {
        px_int ret;
        gif->elapsed -= gif->gce.delay * 10;
        ret = px_gif_get_frame(gif);
        if (ret== 0)
        {
		    px_gif_rewind(gif);
        }
        else if (ret == -1)
        {
            gif->elapsed = 0;
            break;
        }
            
    }
}

px_void PX_GifReset(px_gif* gif)
{
    gif->elapsed = 0;
    px_gif_rewind(gif);
    px_gif_get_frame(gif);
}


px_int px_gif_is_bgcolor(px_gif *gif, px_byte color[3])
{
    return !PX_memcmp(&gif->palette->colors[gif->bgindex*3], color, 3);
}


px_void PX_GifFree(px_gif *gif)
{
    PX_MemoryFree(&gif->data);
	MP_Free(gif->mp, gif->frame);
    PX_TextureFree(&gif->texture);
}

px_void PX_GifSetLoop(px_gif* gif, px_bool loop)
{
    gif->loop = loop;
}

px_texture* PX_GifGetTexture(px_gif* gif)
{
    return &gif->texture;
}
