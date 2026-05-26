////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  WjCryptLib_Sha1
//
//  Implementation of SHA1 hash function.
//  Original author:  Steve Reid <sreid@sea-to-sky.net>
//  Contributions by: James H. Brown <jbrown@burgoyne.com>, Saul Kravitz <Saul.Kravitz@celera.com>,
//  and Ralph Giles <giles@ghostscript.com>
//  Modified by WaterJuice retaining Public Domain license.
//
//  This is free and unencumbered software released into the public domain - June 2013 waterjuice.org
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  IMPORTS
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "PX_Sha.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  DEFINES
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Decide whether to use the Little-Endian shortcut. If the shortcut is not used then the code will work correctly
// on either big or little endian, however if we do know it is a little endian architecture we can speed it up a bit.
// Note, there are TWO places where USE_LITTLE_ENDIAN_SHORTCUT is used. They MUST be paired together.

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  TYPES
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef union
{
    px_byte     c [64];
    px_uint32    l [16];
} CHAR64LONG16;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  INTERNAL FUNCTIONS
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Endian neutral macro for loading 32 bit value from 4 byte array (in big endian form).
#define LOAD32H(x, y)                           \
     { x = ((px_uint32)((y)[0] & 255)<<24) |     \
           ((px_uint32)((y)[1] & 255)<<16) |     \
           ((px_uint32)((y)[2] & 255)<<8)  |     \
           ((px_uint32)((y)[3] & 255)); }

#define rol(value, bits) (((value) << (bits)) | ((value) >> (32 - (bits))))

// blk0() and blk() perform the initial expand.
#ifdef USE_LITTLE_ENDIAN_SHORTCUT
    #define blk0(i) (block->l[i] = (rol(block->l[i],24)&0xFF00FF00) | (rol(block->l[i],8)&0x00FF00FF))
#else
    #define blk0(i) block->l[i]
#endif

#define blk(i) (block->l[i&15] = rol(block->l[(i+13)&15] ^ block->l[(i+8)&15] ^ block->l[(i+2)&15] ^ block->l[i&15],1))

// (R0+R1), R2, R3, R4 are the different operations used in SHA1
#define R0(v,w,x,y,z,i)  z += ((w&(x^y))^y)     + blk0(i)+ 0x5A827999 + rol(v,5); w=rol(w,30);
#define R1(v,w,x,y,z,i)  z += ((w&(x^y))^y)     + blk(i) + 0x5A827999 + rol(v,5); w=rol(w,30);
#define R2(v,w,x,y,z,i)  z += (w^x^y)           + blk(i) + 0x6ED9EBA1 + rol(v,5); w=rol(w,30);
#define R3(v,w,x,y,z,i)  z += (((w|x)&y)|(w&x)) + blk(i) + 0x8F1BBCDC + rol(v,5); w=rol(w,30);
#define R4(v,w,x,y,z,i)  z += (w^x^y)           + blk(i) + 0xCA62C1D6 + rol(v,5); w=rol(w,30);

// Loads the 128 bits from ByteArray into WordArray, treating ByteArray as big endian data
#ifdef USE_LITTLE_ENDIAN_SHORTCUT
    #define Load128BitsAsWords( WordArray, ByteArray )  \
        memcpy( WordArray, ByteArray, 64 )
#else
    #define Load128BitsAsWords( WordArray, ByteArray )      \
    {                                                       \
        px_uint32 i;                                         \
        for( i=0; i<16; i++ )                               \
        {                                                   \
            LOAD32H( (WordArray)[i], (ByteArray)+(i*4) );   \
        }                                                   \
    }
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  TransformFunction
//
//  Hash a single 512-bit block. This is the core of the algorithm
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static
px_void
    TransformFunction
    (
        px_uint32            state[5],
        px_byte const       *buffer
    )
{
    px_uint32            a;
    px_uint32            b;
    px_uint32            c;
    px_uint32            d;
    px_uint32            e;
    px_byte             workspace[64];
    CHAR64LONG16*       block = (CHAR64LONG16*) workspace;

    Load128BitsAsWords( block->l, buffer );

    // Copy context->state[] to working vars
    a = state[0];
    b = state[1];
    c = state[2];
    d = state[3];
    e = state[4];

    // 4 rounds of 20 operations each. Loop unrolled.
    R0(a,b,c,d,e, 0); R0(e,a,b,c,d, 1); R0(d,e,a,b,c, 2); R0(c,d,e,a,b, 3);
    R0(b,c,d,e,a, 4); R0(a,b,c,d,e, 5); R0(e,a,b,c,d, 6); R0(d,e,a,b,c, 7);
    R0(c,d,e,a,b, 8); R0(b,c,d,e,a, 9); R0(a,b,c,d,e,10); R0(e,a,b,c,d,11);
    R0(d,e,a,b,c,12); R0(c,d,e,a,b,13); R0(b,c,d,e,a,14); R0(a,b,c,d,e,15);
    R1(e,a,b,c,d,16); R1(d,e,a,b,c,17); R1(c,d,e,a,b,18); R1(b,c,d,e,a,19);
    R2(a,b,c,d,e,20); R2(e,a,b,c,d,21); R2(d,e,a,b,c,22); R2(c,d,e,a,b,23);
    R2(b,c,d,e,a,24); R2(a,b,c,d,e,25); R2(e,a,b,c,d,26); R2(d,e,a,b,c,27);
    R2(c,d,e,a,b,28); R2(b,c,d,e,a,29); R2(a,b,c,d,e,30); R2(e,a,b,c,d,31);
    R2(d,e,a,b,c,32); R2(c,d,e,a,b,33); R2(b,c,d,e,a,34); R2(a,b,c,d,e,35);
    R2(e,a,b,c,d,36); R2(d,e,a,b,c,37); R2(c,d,e,a,b,38); R2(b,c,d,e,a,39);
    R3(a,b,c,d,e,40); R3(e,a,b,c,d,41); R3(d,e,a,b,c,42); R3(c,d,e,a,b,43);
    R3(b,c,d,e,a,44); R3(a,b,c,d,e,45); R3(e,a,b,c,d,46); R3(d,e,a,b,c,47);
    R3(c,d,e,a,b,48); R3(b,c,d,e,a,49); R3(a,b,c,d,e,50); R3(e,a,b,c,d,51);
    R3(d,e,a,b,c,52); R3(c,d,e,a,b,53); R3(b,c,d,e,a,54); R3(a,b,c,d,e,55);
    R3(e,a,b,c,d,56); R3(d,e,a,b,c,57); R3(c,d,e,a,b,58); R3(b,c,d,e,a,59);
    R4(a,b,c,d,e,60); R4(e,a,b,c,d,61); R4(d,e,a,b,c,62); R4(c,d,e,a,b,63);
    R4(b,c,d,e,a,64); R4(a,b,c,d,e,65); R4(e,a,b,c,d,66); R4(d,e,a,b,c,67);
    R4(c,d,e,a,b,68); R4(b,c,d,e,a,69); R4(a,b,c,d,e,70); R4(e,a,b,c,d,71);
    R4(d,e,a,b,c,72); R4(c,d,e,a,b,73); R4(b,c,d,e,a,74); R4(a,b,c,d,e,75);
    R4(e,a,b,c,d,76); R4(d,e,a,b,c,77); R4(c,d,e,a,b,78); R4(b,c,d,e,a,79);

    // Add the working vars back into context.state[]
    state[0] += a;
    state[1] += b;
    state[2] += c;
    state[3] += d;
    state[4] += e;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  PUBLIC FUNCTIONS
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  Sha1Initialise
//
//  Initialises an SHA1 Context. Use this to initialise/reset a context.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
px_void
    PX_Sha1Initialise(PX_Sha1Context*        Context)
{
    // SHA1 initialisation constants
    Context->State[0] = 0x67452301;
    Context->State[1] = 0xEFCDAB89;
    Context->State[2] = 0x98BADCFE;
    Context->State[3] = 0x10325476;
    Context->State[4] = 0xC3D2E1F0;
    Context->Count[0] = 0;
    Context->Count[1] = 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  Sha1Update
//
//  Adds data to the SHA1 context. This will process the data and update the internal state of the context. Keep on
//  calling this function until all the data has been added. Then call Sha1Finalise to calculate the hash.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
px_void   PX_Sha1Update(   PX_Sha1Context*        Context,px_void  const*        Buffer,px_uint32            BufferSize)
{
    px_uint32    i;
    px_uint32    j;

    j = (Context->Count[0] >> 3) & 63;
    if( (Context->Count[0] += BufferSize << 3) < (BufferSize << 3) )
    {
        Context->Count[1]++;
    }

    Context->Count[1] += (BufferSize >> 29);
    if( (j + BufferSize) > 63 )
    {
        i = 64 - j;
        PX_memcpy( &Context->Buffer[j], Buffer, i );
        TransformFunction(Context->State, Context->Buffer);
        for( ; i + 63 < BufferSize; i += 64 )
        {
            TransformFunction(Context->State, (px_byte*)Buffer + i);
        }
        j = 0;
    }
    else
    {
        i = 0;
    }

    PX_memcpy( &Context->Buffer[j], &((px_byte*)Buffer)[i], BufferSize - i );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  Sha1Finalise
//
//  Performs the final calculation of the hash and returns the digest (20 byte buffer containing 160bit hash). After
//  calling this, Sha1Initialised must be used to reuse the context.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
px_void     PX_Sha1Finalise(PX_Sha1Context* Context,PX_SHA1_HASH* Digest)
{
    px_uint32    i;
    px_byte     finalcount[8];

    for( i=0; i<8; i++ )
    {
        finalcount[i] = (px_byte)((Context->Count[(i >= 4 ? 0 : 1)]
         >> ((3-(i & 3)) * 8) ) & 255);  // Endian independent
    }
    PX_Sha1Update( Context, (px_byte*)"\x80", 1 );
    while( (Context->Count[0] & 504) != 448 )
    {
        PX_Sha1Update( Context, (px_byte*)"\0", 1 );
    }

    PX_Sha1Update( Context, finalcount, 8 );  // Should cause a Sha1TransformFunction()
    for( i=0; i<SHA1_HASH_SIZE; i++ )
    {
        Digest->bytes[i] = (px_byte)((Context->State[i>>2] >> ((3-(i & 3)) * 8) ) & 255);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  Sha1Calculate
//
//  Combines Sha1Initialise, Sha1Update, and Sha1Finalise into one function. Calculates the SHA1 hash of the buffer.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
px_void
    PX_Sha1Calculate
    (
        px_void  const*        Buffer,         // [in]
        px_uint32            BufferSize,     // [in]
        PX_SHA1_HASH*          Digest          // [in]
    )
{
    PX_Sha1Context context;

    PX_Sha1Initialise( &context );
    PX_Sha1Update( &context, Buffer, BufferSize );
    PX_Sha1Finalise( &context, Digest );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  PX_Sha256
//
//  Implementation of SHA256 hash function.
//  Original author: Tom St Denis, tomstdenis@gmail.com, http://libtom.org
//  Modified by WaterJuice retaining Public Domain license.
//
//  This is free and unencumbered software released into the public domain - June 2013 waterjuice.org
//  Modified by DBinary retaining Public Domain license. 2019-3-11 matrixcascade@gmail.com 0-0i.com
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  MACROS
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define ror(value, bits) (((value) >> (bits)) | ((value) << (32 - (bits))))

#define MIN(x, y) ( ((x)<(y))?(x):(y) )

#define STORE32H(x, y)                                                                     \
     { (y)[0] = (px_uchar)(((x)>>24)&255); (y)[1] = (px_uchar)(((x)>>16)&255);   \
       (y)[2] = (px_uchar)(((x)>>8)&255); (y)[3] = (px_uchar)((x)&255); }

#define LOAD32H(x, y)                            \
     { x = ((px_uint32)((y)[0] & 255)<<24) | \
           ((px_uint32)((y)[1] & 255)<<16) | \
           ((px_uint32)((y)[2] & 255)<<8)  | \
           ((px_uint32)((y)[3] & 255)); }

#define STORE64H(x, y)                                                                     \
   { (y)[0] = (px_uchar)(((x)>>56)&255); (y)[1] = (px_uchar)(((x)>>48)&255);     \
     (y)[2] = (px_uchar)(((x)>>40)&255); (y)[3] = (px_uchar)(((x)>>32)&255);     \
     (y)[4] = (px_uchar)(((x)>>24)&255); (y)[5] = (px_uchar)(((x)>>16)&255);     \
     (y)[6] = (px_uchar)(((x)>>8)&255); (y)[7] = (px_uchar)((x)&255); }

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  CONSTANTS
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// The K array
static const px_uint32 SHA256_K[64] = {
    0x428a2f98UL, 0x71374491UL, 0xb5c0fbcfUL, 0xe9b5dba5UL, 0x3956c25bUL,
    0x59f111f1UL, 0x923f82a4UL, 0xab1c5ed5UL, 0xd807aa98UL, 0x12835b01UL,
    0x243185beUL, 0x550c7dc3UL, 0x72be5d74UL, 0x80deb1feUL, 0x9bdc06a7UL,
    0xc19bf174UL, 0xe49b69c1UL, 0xefbe4786UL, 0x0fc19dc6UL, 0x240ca1ccUL,
    0x2de92c6fUL, 0x4a7484aaUL, 0x5cb0a9dcUL, 0x76f988daUL, 0x983e5152UL,
    0xa831c66dUL, 0xb00327c8UL, 0xbf597fc7UL, 0xc6e00bf3UL, 0xd5a79147UL,
    0x06ca6351UL, 0x14292967UL, 0x27b70a85UL, 0x2e1b2138UL, 0x4d2c6dfcUL,
    0x53380d13UL, 0x650a7354UL, 0x766a0abbUL, 0x81c2c92eUL, 0x92722c85UL,
    0xa2bfe8a1UL, 0xa81a664bUL, 0xc24b8b70UL, 0xc76c51a3UL, 0xd192e819UL,
    0xd6990624UL, 0xf40e3585UL, 0x106aa070UL, 0x19a4c116UL, 0x1e376c08UL,
    0x2748774cUL, 0x34b0bcb5UL, 0x391c0cb3UL, 0x4ed8aa4aUL, 0x5b9cca4fUL,
    0x682e6ff3UL, 0x748f82eeUL, 0x78a5636fUL, 0x84c87814UL, 0x8cc70208UL,
    0x90befffaUL, 0xa4506cebUL, 0xbef9a3f7UL, 0xc67178f2UL
};

#define PX_SHA256_BLOCK_SIZE          64

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  INTERNAL FUNCTIONS
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Various logical functions
#define PX_SHA256_Ch( x, y, z )     (z ^ (x & (y ^ z)))
#define PX_SHA256_Maj( x, y, z )    (((x | y) & z) | (x & y))
#define PX_SHA256_S( x, n )         ror((x),(n))
#define PX_SHA256_R( x, n )         (((x)&0xFFFFFFFFUL)>>(n))
#define PX_SHA256_Sigma0( x )       (PX_SHA256_S(x, 2) ^ PX_SHA256_S(x, 13) ^ PX_SHA256_S(x, 22))
#define PX_SHA256_Sigma1( x )       (PX_SHA256_S(x, 6) ^ PX_SHA256_S(x, 11) ^ PX_SHA256_S(x, 25))
#define PX_SHA256_Gamma0( x )       (PX_SHA256_S(x, 7) ^ PX_SHA256_S(x, 18) ^ PX_SHA256_R(x, 3))
#define PX_SHA256_Gamma1( x )       (PX_SHA256_S(x, 17) ^ PX_SHA256_S(x, 19) ^ PX_SHA256_R(x, 10))

#define Sha256Round( a, b, c, d, e, f, g, h, i )       \
     t0 = h + PX_SHA256_Sigma1(e) + PX_SHA256_Ch(e, f, g) + SHA256_K[i] + W[i];   \
     t1 = PX_SHA256_Sigma0(a) + PX_SHA256_Maj(a, b, c);                    \
     d += t0;                                          \
     h  = t0 + t1;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  TransformFunction
//
//  Compress 512-bits
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static
void
PX_TransformFunction
(
    PX_Sha256Context* Context,
    px_uchar const* Buffer
)
{
    px_uint32    S[8];
    px_uint32    W[64];
    px_uint32    t0;
    px_uint32    t1;
    px_uint32    t;
    px_int         i;

    // Copy state into S
    for (i = 0; i < 8; i++)
    {
        S[i] = Context->state[i];
    }

    // Copy the state into 512-bits into W[0..15]
    for (i = 0; i < 16; i++)
    {
        LOAD32H(W[i], Buffer + (4 * i));
    }

    // Fill W[16..63]
    for (i = 16; i < 64; i++)
    {
        W[i] = PX_SHA256_Gamma1(W[i - 2]) + W[i - 7] + PX_SHA256_Gamma0(W[i - 15]) + W[i - 16];
    }

    // Compress
    for (i = 0; i < 64; i++)
    {
        Sha256Round(S[0], S[1], S[2], S[3], S[4], S[5], S[6], S[7], i);
        t = S[7];
        S[7] = S[6];
        S[6] = S[5];
        S[5] = S[4];
        S[4] = S[3];
        S[3] = S[2];
        S[2] = S[1];
        S[1] = S[0];
        S[0] = t;
    }

    // Feedback
    for (i = 0; i < 8; i++)
    {
        Context->state[i] = Context->state[i] + S[i];
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  PUBLIC FUNCTIONS
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  Sha256Initialise
//
//  Initialises a SHA256 Context. Use this to initialise/reset a context.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void
PX_Sha256Initialise
(
    PX_Sha256Context* Context         // [out]
)
{
    Context->curlen = 0;
    Context->radius = 0;
    Context->state[0] = 0x6A09E667UL;
    Context->state[1] = 0xBB67AE85UL;
    Context->state[2] = 0x3C6EF372UL;
    Context->state[3] = 0xA54FF53AUL;
    Context->state[4] = 0x510E527FUL;
    Context->state[5] = 0x9B05688CUL;
    Context->state[6] = 0x1F83D9ABUL;
    Context->state[7] = 0x5BE0CD19UL;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  Sha256Update
//
//  Adds data to the SHA256 context. This will process the data and update the internal state of the context. Keep on
//  calling this function until all the data has been added. Then call Sha256Finalise to calculate the hash.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void
PX_Sha256Update
(
    PX_Sha256Context* Context,        // [in out]
    void* Buffer,         // [in]
    px_uint32            BufferSize      // [in]
)
{
    px_uint32 n;

    if (Context->curlen > sizeof(Context->buf))
    {
        return;
    }

    while (BufferSize > 0)
    {
        if (Context->curlen == 0 && BufferSize >= PX_SHA256_BLOCK_SIZE)
        {
            PX_TransformFunction(Context, (px_uchar*)Buffer);
            Context->radius += PX_SHA256_BLOCK_SIZE * 8;
            Buffer = (px_uchar*)Buffer + PX_SHA256_BLOCK_SIZE;
            BufferSize -= PX_SHA256_BLOCK_SIZE;
        }
        else
        {
            n = MIN(BufferSize, (PX_SHA256_BLOCK_SIZE - Context->curlen));
            PX_memcpy(Context->buf + Context->curlen, (px_void*)Buffer, (px_int)n);
            Context->curlen += n;
            Buffer = (px_uchar*)Buffer + n;
            BufferSize -= n;
            if (Context->curlen == PX_SHA256_BLOCK_SIZE)
            {
                PX_TransformFunction(Context, Context->buf);
                Context->radius += 8 * PX_SHA256_BLOCK_SIZE;
                Context->curlen = 0;
            }
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  Sha256Finalise
//
//  Performs the final calculation of the hash and returns the digest (32 byte buffer containing 256bit hash). After
//  calling this, Sha256Initialised must be used to reuse the context.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void
PX_Sha256Finalise
(
    PX_Sha256Context* Context,        // [in out]
    PX_SHA256_HASH* Digest          // [out]
)
{
    px_int i;

    if (Context->curlen >= sizeof(Context->buf))
    {
        return;
    }

    // Increase the length of the message
    Context->radius += Context->curlen * 8;

    // Append the '1' bit
    Context->buf[Context->curlen++] = (px_uchar)0x80;

    // if the length is currently above 56 bytes we append zeros
    // then compress.  Then we can fall back to padding zeros and length
    // encoding like normal.
    if (Context->curlen > 56)
    {
        while (Context->curlen < 64)
        {
            Context->buf[Context->curlen++] = (px_uchar)0;
        }
        PX_TransformFunction(Context, Context->buf);
        Context->curlen = 0;
    }

    // Pad up to 56 bytes of zeroes
    while (Context->curlen < 56)
    {
        Context->buf[Context->curlen++] = (px_uchar)0;
    }

    // Store length
    STORE64H(Context->radius, Context->buf + 56);
    PX_TransformFunction(Context, Context->buf);

    // Copy output
    for (i = 0; i < 8; i++)
    {
        STORE32H(Context->state[i], Digest->bytes + (4 * i));
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  Sha256Calculate
//
//  Combines Sha256Initialise, Sha256Update, and Sha256Finalise into one function. Calculates the SHA256 hash of the
//  buffer.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void
PX_Sha256Calculate
(
    px_void* Buffer,         // [in]
    px_uint32            BufferSize,     // [in]
    PX_SHA256_HASH* Digest          // [in]
)
{
    PX_Sha256Context context;

    PX_Sha256Initialise(&context);
    PX_Sha256Update(&context, Buffer, BufferSize);
    PX_Sha256Finalise(&context, Digest);
}

void PX_Sha256CalculateHashString(px_void* buffer, px_uint32 bufferSize, px_char stringKey[32])
{
    px_int i;
    PX_SHA256_HASH hash;
    PX_Sha256Calculate(buffer, bufferSize, &hash);
    for (i = 0; i < 31; i++)
    {
        if (hash.bytes[i] == 0)
        {
            hash.bytes[i] = 1;
        }
    }
    hash.bytes[31] = 0;
    PX_memcpy(stringKey, hash.bytes, sizeof(hash.bytes));
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  WjCryptLib_Sha512
//
//  Implementation of SHA512 hash function.
//  Original author: Tom St Denis, tomstdenis@gmail.com, http://libtom.org
//  Modified by WaterJuice retaining Public Domain license.
//
//  This is free and unencumbered software released into the public domain - June 2013 waterjuice.org
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  IMPORTS
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  MACROS
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define ROR64( value, bits ) (((value) >> (bits)) | ((value) << (64 - (bits))))

#define MIN( x, y ) ( ((x)<(y))?(x):(y) )

#define LOAD64H( x, y )                                                      \
   { x = (((px_uint64)((y)[0] & 255))<<56)|(((px_uint64)((y)[1] & 255))<<48) | \
         (((px_uint64)((y)[2] & 255))<<40)|(((px_uint64)((y)[3] & 255))<<32) | \
         (((px_uint64)((y)[4] & 255))<<24)|(((px_uint64)((y)[5] & 255))<<16) | \
         (((px_uint64)((y)[6] & 255))<<8)|(((px_uint64)((y)[7] & 255))); }


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  CONSTANTS
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// The K array
static const px_uint64 SHA512_K[80] = {
    0x428a2f98d728ae22ULL, 0x7137449123ef65cdULL, 0xb5c0fbcfec4d3b2fULL, 0xe9b5dba58189dbbcULL,
    0x3956c25bf348b538ULL, 0x59f111f1b605d019ULL, 0x923f82a4af194f9bULL, 0xab1c5ed5da6d8118ULL,
    0xd807aa98a3030242ULL, 0x12835b0145706fbeULL, 0x243185be4ee4b28cULL, 0x550c7dc3d5ffb4e2ULL,
    0x72be5d74f27b896fULL, 0x80deb1fe3b1696b1ULL, 0x9bdc06a725c71235ULL, 0xc19bf174cf692694ULL,
    0xe49b69c19ef14ad2ULL, 0xefbe4786384f25e3ULL, 0x0fc19dc68b8cd5b5ULL, 0x240ca1cc77ac9c65ULL,
    0x2de92c6f592b0275ULL, 0x4a7484aa6ea6e483ULL, 0x5cb0a9dcbd41fbd4ULL, 0x76f988da831153b5ULL,
    0x983e5152ee66dfabULL, 0xa831c66d2db43210ULL, 0xb00327c898fb213fULL, 0xbf597fc7beef0ee4ULL,
    0xc6e00bf33da88fc2ULL, 0xd5a79147930aa725ULL, 0x06ca6351e003826fULL, 0x142929670a0e6e70ULL,
    0x27b70a8546d22ffcULL, 0x2e1b21385c26c926ULL, 0x4d2c6dfc5ac42aedULL, 0x53380d139d95b3dfULL,
    0x650a73548baf63deULL, 0x766a0abb3c77b2a8ULL, 0x81c2c92e47edaee6ULL, 0x92722c851482353bULL,
    0xa2bfe8a14cf10364ULL, 0xa81a664bbc423001ULL, 0xc24b8b70d0f89791ULL, 0xc76c51a30654be30ULL,
    0xd192e819d6ef5218ULL, 0xd69906245565a910ULL, 0xf40e35855771202aULL, 0x106aa07032bbd1b8ULL,
    0x19a4c116b8d2d0c8ULL, 0x1e376c085141ab53ULL, 0x2748774cdf8eeb99ULL, 0x34b0bcb5e19b48a8ULL,
    0x391c0cb3c5c95a63ULL, 0x4ed8aa4ae3418acbULL, 0x5b9cca4f7763e373ULL, 0x682e6ff3d6b2b8a3ULL,
    0x748f82ee5defb2fcULL, 0x78a5636f43172f60ULL, 0x84c87814a1f0ab72ULL, 0x8cc702081a6439ecULL,
    0x90befffa23631e28ULL, 0xa4506cebde82bde9ULL, 0xbef9a3f7b2c67915ULL, 0xc67178f2e372532bULL,
    0xca273eceea26619cULL, 0xd186b8c721c0c207ULL, 0xeada7dd6cde0eb1eULL, 0xf57d4f7fee6ed178ULL,
    0x06f067aa72176fbaULL, 0x0a637dc5a2c898a6ULL, 0x113f9804bef90daeULL, 0x1b710b35131c471bULL,
    0x28db77f523047d84ULL, 0x32caab7b40c72493ULL, 0x3c9ebe0a15c9bebcULL, 0x431d67c49c100d4cULL,
    0x4cc5d4becb3e42b6ULL, 0x597f299cfc657e2aULL, 0x5fcb6fab3ad6faecULL, 0x6c44198c4a475817ULL
};

#define PX_SHA512_BLOCK_SIZE          128

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  INTERNAL FUNCTIONS
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Various logical functions
#define PX_SHA512_Ch( x, y, z )     (z ^ (x & (y ^ z)))
#define PX_SHA512_Maj(x, y, z )     (((x | y) & z) | (x & y))
#define PX_SHA512_S( x, n )         ROR64( x, n )
#define PX_SHA512_R( x, n )         (((x)&0xFFFFFFFFFFFFFFFFULL)>>((px_uint64)n))
#define PX_SHA512_Sigma0( x )       (PX_SHA512_S(x, 28) ^ PX_SHA512_S(x, 34) ^ PX_SHA512_S(x, 39))
#define PX_SHA512_Sigma1( x )       (PX_SHA512_S(x, 14) ^ PX_SHA512_S(x, 18) ^ PX_SHA512_S(x, 41))
#define PX_SHA512_Gamma0( x )       (PX_SHA512_S(x, 1) ^ PX_SHA512_S(x, 8) ^ PX_SHA512_R(x, 7))
#define PX_SHA512_Gamma1( x )       (PX_SHA512_S(x, 19) ^ PX_SHA512_S(x, 61) ^ PX_SHA512_R(x, 6))

#define Sha512Round( a, b, c, d, e, f, g, h, i )       \
     t0 = h + PX_SHA512_Sigma1(e) + PX_SHA512_Ch(e, f, g) + SHA512_K[i] + W[i];   \
     t1 = PX_SHA512_Sigma0(a) + PX_SHA512_Maj(a, b, c);                    \
     d += t0;                                          \
     h  = t0 + t1;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  PX_SHA512_TransformFunction
//
//  Compress 1024-bits
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static
void
PX_SHA512_TransformFunction
(
    Sha512Context* Context,
    px_byte const* Buffer
)
{
    px_uint64    S[8];
    px_uint64    W[80];
    px_uint64    t0;
    px_uint64    t1;
    int         i;

    // Copy state into S
    for (i = 0; i < 8; i++)
    {
        S[i] = Context->state[i];
    }

    // Copy the state into 1024-bits into W[0..15]
    for (i = 0; i < 16; i++)
    {
        LOAD64H(W[i], Buffer + (8 * i));
    }

    // Fill W[16..79]
    for (i = 16; i < 80; i++)
    {
        W[i] = PX_SHA512_Gamma1(W[i - 2]) + W[i - 7] + PX_SHA512_Gamma0(W[i - 15]) + W[i - 16];
    }

    // Compress
    for (i = 0; i < 80; i += 8)
    {
        Sha512Round(S[0], S[1], S[2], S[3], S[4], S[5], S[6], S[7], i + 0);
        Sha512Round(S[7], S[0], S[1], S[2], S[3], S[4], S[5], S[6], i + 1);
        Sha512Round(S[6], S[7], S[0], S[1], S[2], S[3], S[4], S[5], i + 2);
        Sha512Round(S[5], S[6], S[7], S[0], S[1], S[2], S[3], S[4], i + 3);
        Sha512Round(S[4], S[5], S[6], S[7], S[0], S[1], S[2], S[3], i + 4);
        Sha512Round(S[3], S[4], S[5], S[6], S[7], S[0], S[1], S[2], i + 5);
        Sha512Round(S[2], S[3], S[4], S[5], S[6], S[7], S[0], S[1], i + 6);
        Sha512Round(S[1], S[2], S[3], S[4], S[5], S[6], S[7], S[0], i + 7);
    }

    // Feedback
    for (i = 0; i < 8; i++)
    {
        Context->state[i] = Context->state[i] + S[i];
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  PUBLIC FUNCTIONS
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  Sha512Initialise
//
//  Initialises a SHA512 Context. Use this to initialise/reset a context.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void PX_Sha512Initialise
(
    Sha512Context* Context         // [out]
)
{
    Context->curlen = 0;
    Context->radius = 0;
    Context->state[0] = 0x6a09e667f3bcc908ULL;
    Context->state[1] = 0xbb67ae8584caa73bULL;
    Context->state[2] = 0x3c6ef372fe94f82bULL;
    Context->state[3] = 0xa54ff53a5f1d36f1ULL;
    Context->state[4] = 0x510e527fade682d1ULL;
    Context->state[5] = 0x9b05688c2b3e6c1fULL;
    Context->state[6] = 0x1f83d9abfb41bd6bULL;
    Context->state[7] = 0x5be0cd19137e2179ULL;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  Sha512Update
//
//  Adds data to the SHA512 context. This will process the data and update the internal state of the context. Keep on
//  calling this function until all the data has been added. Then call Sha512Finalise to calculate the hash.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void PX_Sha512Update
(
    Sha512Context* Context,        // [in out]
    void const* Buffer,         // [in]
    px_uint32            BufferSize      // [in]
)
{
    px_uint32    n;

    if (Context->curlen > sizeof(Context->buf))
    {
        return;
    }

    while (BufferSize > 0)
    {
        if (Context->curlen == 0 && BufferSize >= PX_SHA512_BLOCK_SIZE)
        {
            PX_SHA512_TransformFunction(Context, (px_byte*)Buffer);
            Context->radius += PX_SHA512_BLOCK_SIZE * 8;
            Buffer = (px_byte*)Buffer + PX_SHA512_BLOCK_SIZE;
            BufferSize -= PX_SHA512_BLOCK_SIZE;
        }
        else
        {
            n = MIN(BufferSize, (PX_SHA512_BLOCK_SIZE - Context->curlen));
            PX_memcpy(Context->buf + Context->curlen, Buffer, (px_int)n);
            Context->curlen += n;
            Buffer = (px_byte*)Buffer + n;
            BufferSize -= n;
            if (Context->curlen == PX_SHA512_BLOCK_SIZE)
            {
                PX_SHA512_TransformFunction(Context, Context->buf);
                Context->radius += 8 * PX_SHA512_BLOCK_SIZE;
                Context->curlen = 0;
            }
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  Sha512Finalise
//
//  Performs the final calculation of the hash and returns the digest (64 byte buffer containing 512bit hash). After
//  calling this, Sha512Initialised must be used to reuse the context.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void
PX_Sha512Finalise
(
    Sha512Context* Context,        // [in out]
    SHA512_HASH* Digest          // [out]
)
{
    int i;

    if (Context->curlen >= sizeof(Context->buf))
    {
        return;
    }

    // Increase the length of the message
    Context->radius += Context->curlen * 8ULL;

    // Append the '1' bit
    Context->buf[Context->curlen++] = (px_byte)0x80;

    // If the length is currently above 112 bytes we append zeros
    // then compress.  Then we can fall back to padding zeros and length
    // encoding like normal.
    if (Context->curlen > 112)
    {
        while (Context->curlen < 128)
        {
            Context->buf[Context->curlen++] = (px_byte)0;
        }
        PX_SHA512_TransformFunction(Context, Context->buf);
        Context->curlen = 0;
    }

    // Pad up to 120 bytes of zeroes
    // note: that from 112 to 120 is the 64 MSB of the length.  We assume that you won't hash
    // > 2^64 bits of data... :-)
    while (Context->curlen < 120)
    {
        Context->buf[Context->curlen++] = (px_byte)0;
    }

    // Store length
    STORE64H(Context->radius, Context->buf + 120);
    PX_SHA512_TransformFunction(Context, Context->buf);

    // Copy output
    for (i = 0; i < 8; i++)
    {
        STORE64H(Context->state[i], Digest->bytes + (8 * i));
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  Sha512Calculate
//
//  Combines Sha512Initialise, Sha512Update, and Sha512Finalise into one function. Calculates the SHA512 hash of the
//  buffer.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void
PX_Sha512Calculate
(
    void  const* Buffer,         // [in]
    px_uint32            BufferSize,     // [in]
    SHA512_HASH* Digest          // [in]
)
{
    Sha512Context context;

    PX_Sha512Initialise(&context);
    PX_Sha512Update(&context, Buffer, BufferSize);
    PX_Sha512Finalise(&context, Digest);
}

void PX_Sha512CalculateHashString(px_void* buffer, px_uint32 bufferSize, px_char stringKey[64])
{
    px_int i;
    SHA512_HASH hash;
    PX_Sha512Calculate(buffer, bufferSize, &hash);
    for (i = 0; i < 63; i++)
    {
        if (hash.bytes[i] == 0)
        {
            hash.bytes[i] = 1;
        }
    }
    hash.bytes[63] = 0;
	PX_memcpy(stringKey, hash.bytes, sizeof(hash.bytes));
}