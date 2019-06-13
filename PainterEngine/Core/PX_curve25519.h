#ifndef PX_CURVE25519_H
#define PX_CURVE25519_H
#include "PX_MemoryPool.h"
/*
To generate a private key, generate 32 random bytes and:
px_byte mysecret[32];
mysecret[0] &= 248;
mysecret[31] &= 127;
mysecret[31] |= 64;
To generate the public key, just do:

static const uint8_t basepoint[32] = {9};
curve25519_donna(mypublic, mysecret, basepoint);
To generate a shared key do:

uint8_t shared_key[32];
curve25519_donna(shared_key, mysecret, theirpublic);
*/
px_void PX_Curve25519(px_byte *mypublic, const px_byte *secret, const px_byte *basepoint);
#endif
