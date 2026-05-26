#include "PX_Zip.h"

/* -----------------------------------------------------------------------
   ZIP format constants
   ----------------------------------------------------------------------- */
#define ZIP_LOCAL_FILE_SIGNATURE    0x04034B50UL
#define ZIP_CENTRAL_DIR_SIGNATURE   0x02014B50UL
#define ZIP_END_CENTRAL_SIGNATURE   0x06054B50UL

#define ZIP_METHOD_STORED   0
#define ZIP_METHOD_DEFLATE  8

/* -----------------------------------------------------------------------
   CRC-32 (ISO 3309 / ITU-T V.42, used by ZIP)
   ----------------------------------------------------------------------- */
static px_dword px_crc32_table[256];
static px_bool   px_crc32_table_ready = PX_FALSE;

static px_void PX_Zip_CRC32_InitTable(px_void)
{
    px_dword i, j, c;
    if (px_crc32_table_ready) return;
    for (i = 0; i < 256; i++)
    {
        c = i;
        for (j = 0; j < 8; j++)
        {
            if (c & 1)
                c = 0xEDB88320UL ^ (c >> 1);
            else
                c = c >> 1;
        }
        px_crc32_table[i] = c;
    }
    px_crc32_table_ready = PX_TRUE;
}

static px_dword PX_Zip_CRC32(const px_byte *data, px_int size)
{
    px_dword crc = 0xFFFFFFFFUL;
    px_int i;
    PX_Zip_CRC32_InitTable();
    for (i = 0; i < size; i++)
        crc = px_crc32_table[(crc ^ data[i]) & 0xFF] ^ (crc >> 8);
    return crc ^ 0xFFFFFFFFUL;
}

/* -----------------------------------------------------------------------
   Little-endian helpers
   ----------------------------------------------------------------------- */
static px_bool PX_Zip_WriteU16(px_memory *out, px_word v)
{
    px_byte b[2];
    b[0] = (px_byte)(v & 0xFF);
    b[1] = (px_byte)((v >> 8) & 0xFF);
    return PX_MemoryCat(out, b, 2);
}

static px_bool PX_Zip_WriteU32(px_memory *out, px_dword v)
{
    px_byte b[4];
    b[0] = (px_byte)(v & 0xFF);
    b[1] = (px_byte)((v >> 8) & 0xFF);
    b[2] = (px_byte)((v >> 16) & 0xFF);
    b[3] = (px_byte)((v >> 24) & 0xFF);
    return PX_MemoryCat(out, b, 4);
}

static px_word PX_Zip_ReadU16(const px_byte *p)
{
    return (px_word)(p[0] | ((px_word)p[1] << 8));
}

static px_dword PX_Zip_ReadU32(const px_byte *p)
{
    return (px_dword)(p[0] | ((px_dword)p[1] << 8) | ((px_dword)p[2] << 16) | ((px_dword)p[3] << 24));
}

/* -----------------------------------------------------------------------
   Helper: build an abi key like "file[N].filename"
   ----------------------------------------------------------------------- */
static px_void PX_Zip_MakeKey(px_char *buf, px_int bufsize, const px_char *field, px_int idx)
{
    /* buf = "file[idx].field" */
    px_int i = 0;
    px_int n;
    const px_char *prefix = "file[";
    const px_char *mid    = "].";

    /* write "file[" */
    while (prefix[i] && i < bufsize - 1) { buf[i] = prefix[i]; i++; }

    /* write index digits */
    {
        px_char tmp[16];
        px_int tlen = 0, v = idx, start;
        if (v == 0) { tmp[tlen++] = '0'; }
        else { while (v > 0) { tmp[tlen++] = (px_char)('0' + v % 10); v /= 10; } }
        /* reverse */
        for (start = 0; start < tlen / 2; start++) {
            px_char c = tmp[start]; tmp[start] = tmp[tlen-1-start]; tmp[tlen-1-start] = c;
        }
        for (n = 0; n < tlen && i < bufsize - 1; n++, i++) buf[i] = tmp[n];
    }

    /* write "]." */
    for (n = 0; mid[n] && i < bufsize - 1; n++, i++) buf[i] = mid[n];

    /* write field */
    for (n = 0; field[n] && i < bufsize - 1; n++, i++) buf[i] = field[n];

    buf[i] = '\0';
}

/* -----------------------------------------------------------------------
   PX_ZipCompress
   ----------------------------------------------------------------------- */
px_bool PX_ZipCompress(px_memorypool *mp, px_abi *input, px_memory *output, px_int level)
{
    /*
     * ZIP structure:
     *   for each file:
     *     [Local File Header] [File Data (deflated or stored)]
     *   [Central Directory entries]
     *   [End of Central Directory Record]
     *
     * level == 0  => STORED (no compression)
     * level  > 0  => DEFLATE
     */
    px_int   file_count;
    px_int   i;
    px_dword central_dir_offset;
    px_dword central_dir_size_start;

    /* We'll collect per-file metadata to write the central directory later.
       We store it in a small temporary abi to keep allocations inside mp. */
    typedef struct {
        px_dword local_header_offset;
        px_dword crc32;
        px_dword compressed_size;
        px_dword uncompressed_size;
        px_word  method;
        const px_char *filename;  /* pointer into input abi - valid during the call */
    } FileEntry;

    FileEntry *entries;
    px_int     entries_cap;

    if (!mp || !input || !output) return PX_FALSE;

    /* Count files: iterate file[0], file[1], ... until filename missing */
    file_count = 0;
    {
        px_char key[64];
        while (1)
        {
            PX_Zip_MakeKey(key, sizeof(key), "filename", file_count);
            if (!PX_AbiExist(input, key)) break;
            file_count++;
        }
    }

    if (file_count == 0) return PX_FALSE;

    /* allocate entry metadata array */
    entries_cap = file_count;
    entries = (FileEntry *)MP_Malloc(mp, sizeof(FileEntry) * entries_cap);
    if (!entries) return PX_FALSE;
    PX_memset(entries, 0, sizeof(FileEntry) * entries_cap);

    /* ---- Phase 1: write local file headers + data ---- */
    for (i = 0; i < file_count; i++)
    {
        px_char      key[64];
        const px_char *filename;
        px_void      *filedata;
        px_dword      filesize;
        px_dword      crc;
        px_word       fname_len;
        px_word       method;
        px_dword      comp_size;

        /* get filename */
        PX_Zip_MakeKey(key, sizeof(key), "filename", i);
        filename = PX_AbiGet_string(input, key);
        if (!filename) { MP_Free(mp, entries); return PX_FALSE; }

        /* get bin data */
        PX_Zip_MakeKey(key, sizeof(key), "bin", i);
        filedata = PX_AbiGet_data(input, key, &filesize);
        if (!filedata && filesize != 0) { MP_Free(mp, entries); return PX_FALSE; }

        crc      = PX_Zip_CRC32((const px_byte *)filedata, (px_int)filesize);
        fname_len = (px_word)PX_strlen(filename);

        entries[i].local_header_offset = (px_dword)output->usedsize;
        entries[i].crc32               = crc;
        entries[i].uncompressed_size   = filesize;
        entries[i].filename            = filename;

        if (level == 0 || filesize == 0)
        {
            /* STORED */
            method    = ZIP_METHOD_STORED;
            comp_size = filesize;

            entries[i].method          = method;
            entries[i].compressed_size = comp_size;

            /* Local file header */
            if (!PX_Zip_WriteU32(output, ZIP_LOCAL_FILE_SIGNATURE))     goto fail;
            if (!PX_Zip_WriteU16(output, 20))   /* version needed */     goto fail;
            if (!PX_Zip_WriteU16(output, 0))    /* general purpose */    goto fail;
            if (!PX_Zip_WriteU16(output, method))                        goto fail;
            if (!PX_Zip_WriteU16(output, 0))    /* mod time */           goto fail;
            if (!PX_Zip_WriteU16(output, 0))    /* mod date */           goto fail;
            if (!PX_Zip_WriteU32(output, crc))                           goto fail;
            if (!PX_Zip_WriteU32(output, comp_size))                     goto fail;
            if (!PX_Zip_WriteU32(output, filesize))                      goto fail;
            if (!PX_Zip_WriteU16(output, fname_len))                     goto fail;
            if (!PX_Zip_WriteU16(output, 0))    /* extra field len */    goto fail;
            if (!PX_MemoryCat(output, (px_void *)filename, fname_len))  goto fail;
            if (filesize > 0)
            {
                if (!PX_MemoryCat(output, filedata, (px_int)filesize))  goto fail;
            }
        }
        else
        {
            /* DEFLATE */
            px_memory deflated;
            PX_MemoryInitialize(mp, &deflated);

            if (!PX_RFC1951Deflate((const px_byte *)filedata, filesize, &deflated, 32768))
            {
                PX_MemoryFree(&deflated);
                goto fail;
            }

            method = ZIP_METHOD_DEFLATE;
            /* PX_RFC1951Deflate prepends a 2-byte zlib header (\x78\x9c).
               ZIP method=8 requires raw deflate with no zlib wrapper,
               so we skip those 2 bytes when writing and adjust comp_size. */
            if (deflated.usedsize < 2) { PX_MemoryFree(&deflated); goto fail; }
            comp_size = (px_dword)deflated.usedsize - 2;

            entries[i].method          = method;
            entries[i].compressed_size = comp_size;

            /* Local file header */
            if (!PX_Zip_WriteU32(output, ZIP_LOCAL_FILE_SIGNATURE))     { PX_MemoryFree(&deflated); goto fail; }
            if (!PX_Zip_WriteU16(output, 20))                            { PX_MemoryFree(&deflated); goto fail; }
            if (!PX_Zip_WriteU16(output, 0))                             { PX_MemoryFree(&deflated); goto fail; }
            if (!PX_Zip_WriteU16(output, method))                        { PX_MemoryFree(&deflated); goto fail; }
            if (!PX_Zip_WriteU16(output, 0))                             { PX_MemoryFree(&deflated); goto fail; }
            if (!PX_Zip_WriteU16(output, 0))                             { PX_MemoryFree(&deflated); goto fail; }
            if (!PX_Zip_WriteU32(output, crc))                           { PX_MemoryFree(&deflated); goto fail; }
            if (!PX_Zip_WriteU32(output, comp_size))                     { PX_MemoryFree(&deflated); goto fail; }
            if (!PX_Zip_WriteU32(output, filesize))                      { PX_MemoryFree(&deflated); goto fail; }
            if (!PX_Zip_WriteU16(output, fname_len))                     { PX_MemoryFree(&deflated); goto fail; }
            if (!PX_Zip_WriteU16(output, 0))                             { PX_MemoryFree(&deflated); goto fail; }
            if (!PX_MemoryCat(output, (px_void *)filename, fname_len))  { PX_MemoryFree(&deflated); goto fail; }
            if (comp_size > 0)
            {
                /* skip the 2-byte zlib header */
                if (!PX_MemoryCat(output, deflated.buffer + 2, (px_int)comp_size)) { PX_MemoryFree(&deflated); goto fail; }
            }
            PX_MemoryFree(&deflated);
        }
    }

    /* ---- Phase 2: central directory ---- */
    central_dir_offset    = (px_dword)output->usedsize;
    central_dir_size_start = (px_dword)output->usedsize;

    for (i = 0; i < file_count; i++)
    {
        px_word fname_len = (px_word)PX_strlen(entries[i].filename);

        if (!PX_Zip_WriteU32(output, ZIP_CENTRAL_DIR_SIGNATURE))         goto fail;
        if (!PX_Zip_WriteU16(output, 20))  /* version made by */         goto fail;
        if (!PX_Zip_WriteU16(output, 20))  /* version needed */          goto fail;
        if (!PX_Zip_WriteU16(output, 0))   /* general purpose */         goto fail;
        if (!PX_Zip_WriteU16(output, entries[i].method))                 goto fail;
        if (!PX_Zip_WriteU16(output, 0))   /* mod time */                goto fail;
        if (!PX_Zip_WriteU16(output, 0))   /* mod date */                goto fail;
        if (!PX_Zip_WriteU32(output, entries[i].crc32))                  goto fail;
        if (!PX_Zip_WriteU32(output, entries[i].compressed_size))        goto fail;
        if (!PX_Zip_WriteU32(output, entries[i].uncompressed_size))      goto fail;
        if (!PX_Zip_WriteU16(output, fname_len))                         goto fail;
        if (!PX_Zip_WriteU16(output, 0))   /* extra field len */         goto fail;
        if (!PX_Zip_WriteU16(output, 0))   /* file comment len */        goto fail;
        if (!PX_Zip_WriteU16(output, 0))   /* disk number start */       goto fail;
        if (!PX_Zip_WriteU16(output, 0))   /* internal attr */           goto fail;
        if (!PX_Zip_WriteU32(output, 0))   /* external attr */           goto fail;
        if (!PX_Zip_WriteU32(output, entries[i].local_header_offset))   goto fail;
        if (!PX_MemoryCat(output, (px_void *)entries[i].filename, fname_len)) goto fail;
    }

    /* ---- Phase 3: end of central directory ---- */
    {
        px_dword central_dir_size = (px_dword)output->usedsize - central_dir_size_start;

        if (!PX_Zip_WriteU32(output, ZIP_END_CENTRAL_SIGNATURE))         goto fail;
        if (!PX_Zip_WriteU16(output, 0))   /* disk number */             goto fail;
        if (!PX_Zip_WriteU16(output, 0))   /* start disk */              goto fail;
        if (!PX_Zip_WriteU16(output, (px_word)file_count))               goto fail;
        if (!PX_Zip_WriteU16(output, (px_word)file_count))               goto fail;
        if (!PX_Zip_WriteU32(output, central_dir_size))                  goto fail;
        if (!PX_Zip_WriteU32(output, central_dir_offset))                goto fail;
        if (!PX_Zip_WriteU16(output, 0))   /* zip comment len */         goto fail;
    }

    MP_Free(mp, entries);
    return PX_TRUE;

fail:
    MP_Free(mp, entries);
    return PX_FALSE;
}

/* -----------------------------------------------------------------------
   PX_ZipDecompress
   ----------------------------------------------------------------------- */
px_bool PX_ZipDecompress(px_memorypool *mp, const px_byte *input, px_int input_size, px_abi *output)
{
    /*
     * Strategy: scan backwards for End of Central Directory Record,
     * then walk the Central Directory to find each file entry,
     * then decompress each file using its local header offset.
     */
    const px_byte *p;
    px_int  i;
    px_int  eocd_off = -1;
    px_word total_entries;
    px_dword cd_offset;
    px_dword cd_size;

    if (!mp || !input || input_size < 22 || !output) return PX_FALSE;

    /* --- Find EOCD --- */
    for (i = input_size - 22; i >= 0; i--)
    {
        if (PX_Zip_ReadU32(input + i) == ZIP_END_CENTRAL_SIGNATURE)
        {
            eocd_off = i;
            break;
        }
    }
    if (eocd_off < 0) return PX_FALSE;

    p             = input + eocd_off;
    /* skip sig(4) + disk(2) + start_disk(2) + entries_on_disk(2) */
    total_entries = PX_Zip_ReadU16(p + 10);
    cd_size       = PX_Zip_ReadU32(p + 12);
    cd_offset     = PX_Zip_ReadU32(p + 16);

    if ((px_int)(cd_offset + cd_size) > input_size) return PX_FALSE;

    /* --- Walk Central Directory --- */
    {
        px_dword cd_pos = cd_offset;
        px_int   entry_idx = 0;

        while (entry_idx < (px_int)total_entries)
        {
            px_word  method, fname_len, extra_len, comment_len;
            px_dword comp_size, uncomp_size, crc, local_offset;
            px_char  fname_buf[512];
            px_char  key[64];
            const px_byte *local_p;
            px_word  local_fname_len, local_extra_len;
            px_dword data_offset;

            if (cd_pos + 46 > (px_dword)input_size) return PX_FALSE;
            if (PX_Zip_ReadU32(input + cd_pos) != ZIP_CENTRAL_DIR_SIGNATURE) return PX_FALSE;

            method       = PX_Zip_ReadU16(input + cd_pos + 10);
            crc          = PX_Zip_ReadU32(input + cd_pos + 16);
            comp_size    = PX_Zip_ReadU32(input + cd_pos + 20);
            uncomp_size  = PX_Zip_ReadU32(input + cd_pos + 24);
            fname_len    = PX_Zip_ReadU16(input + cd_pos + 28);
            extra_len    = PX_Zip_ReadU16(input + cd_pos + 30);
            comment_len  = PX_Zip_ReadU16(input + cd_pos + 32);
            local_offset = PX_Zip_ReadU32(input + cd_pos + 42);

            if (fname_len == 0 || fname_len >= (px_word)sizeof(fname_buf)) return PX_FALSE;
            if (cd_pos + 46 + fname_len > (px_dword)input_size) return PX_FALSE;

            PX_memcpy(fname_buf, input + cd_pos + 46, fname_len);
            fname_buf[fname_len] = '\0';

            /* advance cd pointer */
            cd_pos += 46 + fname_len + extra_len + comment_len;

            /* --- Locate data in local file entry --- */
            if ((px_int)(local_offset + 30) > input_size) return PX_FALSE;
            local_p = input + local_offset;
            if (PX_Zip_ReadU32(local_p) != ZIP_LOCAL_FILE_SIGNATURE) return PX_FALSE;

            local_fname_len  = PX_Zip_ReadU16(local_p + 26);
            local_extra_len  = PX_Zip_ReadU16(local_p + 28);
            data_offset      = local_offset + 30 + local_fname_len + local_extra_len;

            if ((px_int)(data_offset + comp_size) > input_size) return PX_FALSE;

            /* --- set filename into output abi --- */
            PX_Zip_MakeKey(key, sizeof(key), "filename", entry_idx);
            if (!PX_AbiSet_string(output, key, fname_buf)) return PX_FALSE;

            /* --- decompress / copy data --- */
            PX_Zip_MakeKey(key, sizeof(key), "bin", entry_idx);

            if (method == ZIP_METHOD_STORED)
            {
                if (!PX_AbiSet_data(output, key, input + data_offset, (px_int)comp_size))
                    return PX_FALSE;
            }
            else if (method == ZIP_METHOD_DEFLATE)
            {
                /* ZIP stores raw deflate; PX_RFC1951Inflate expects a 2-byte
                   zlib header (\x78\x9c) before the raw stream, so we prepend it. */
                px_memory wrapped;
                px_memory inflated;
                px_byte zlib_hdr[2] = {0x78, 0x9c};

                PX_MemoryInitialize(mp, &wrapped);
                if (!PX_MemoryCat(&wrapped, zlib_hdr, 2))
                {
                    PX_MemoryFree(&wrapped);
                    return PX_FALSE;
                }
                if (comp_size > 0 && !PX_MemoryCat(&wrapped, input + data_offset, (px_int)comp_size))
                {
                    PX_MemoryFree(&wrapped);
                    return PX_FALSE;
                }

                PX_MemoryInitialize(mp, &inflated);
                if (!PX_RFC1951Inflate(wrapped.buffer, (px_uint)wrapped.usedsize, &inflated))
                {
                    PX_MemoryFree(&wrapped);
                    PX_MemoryFree(&inflated);
                    return PX_FALSE;
                }
                PX_MemoryFree(&wrapped);

                if ((px_dword)inflated.usedsize != uncomp_size)
                {
                    PX_MemoryFree(&inflated);
                    return PX_FALSE;
                }

                /* verify crc */
                if (PX_Zip_CRC32(inflated.buffer, inflated.usedsize) != crc)
                {
                    PX_MemoryFree(&inflated);
                    return PX_FALSE;
                }

                if (!PX_AbiSet_data(output, key, inflated.buffer, inflated.usedsize))
                {
                    PX_MemoryFree(&inflated);
                    return PX_FALSE;
                }
                PX_MemoryFree(&inflated);
            }
            else
            {
                /* unsupported compression method */
                return PX_FALSE;
            }

            entry_idx++;
        }
    }

    return PX_TRUE;
}
