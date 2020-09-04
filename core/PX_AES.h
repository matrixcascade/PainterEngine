#ifndef __PX_MATH_AES
#define __PX_MATH_AES
#include "PX_Typedef.h"

#define  PX_AES_BLOCK_BYTES_SIZE		16
#define  PX_AES_SEED_KEY_SIZE			32
#define  PX_AES_KEYSCHEDULE_MAX_ROW	    60
#define  PX_AES_KEYSCHEDULE_MAX_COLUMN  4
#define  PX_AES_STATE_MATRIX_SIZE		4

typedef enum 
{
	PX_AES_KeySize_Bits128, PX_AES_KeySize_Bits192, PX_AES_KeySize_Bits256
}PX_AES_KeySize;

typedef px_uchar AES_BYTE;

typedef struct
{
	AES_BYTE				key[PX_AES_SEED_KEY_SIZE];													    // the seed key. size will be 4 * keySize from ctor.
	AES_BYTE				w[PX_AES_KEYSCHEDULE_MAX_ROW][PX_AES_KEYSCHEDULE_MAX_COLUMN];						// key schedule array. 
	AES_BYTE				State[PX_AES_STATE_MATRIX_SIZE][PX_AES_STATE_MATRIX_SIZE];						// State matrix always 4x4
	px_int					Nb;																			// block size in 32-bit words.  Always 4 for AES.  (128 bits).
	px_int					Nk;																			// key size in 32-bit words.  4, 6, 8.  (128, 192, 256 bits).
	px_int					Nr;																			// number of rounds. 10, 12, 14.
}PX_AES;

px_void					PX_AES_Initialize(PX_AES *aesStruct,PX_AES_KeySize keySize,AES_BYTE keyByte[]);
px_bool                 PX_AES_CipherBuffer(PX_AES *aesStruct,px_byte input[],px_int size,px_byte output[]); 
px_bool					PX_AES_Cipher(PX_AES *aesStruct,px_byte input[16], px_byte output[16]); /* encipher 16-bit input */;
px_void					PX_AES_InvCipher(PX_AES *aesStruct,px_byte input[16], px_byte output[16]); /* decipher 16-bit input */;
px_bool					PX_AES_InvCipherBuffer(PX_AES *aesStruct,px_byte input[],px_int size,px_byte output[]);
#endif
