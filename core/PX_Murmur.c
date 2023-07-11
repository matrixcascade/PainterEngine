#include "PX_Murmur.h"

px_dword PX_MurmurHash(const px_byte* key, px_dword len)
{
	px_dword c1 = 0xcc9e2d51;
	px_dword c2 = 0x1b873593;
	px_dword r1 = 15;
	px_dword r2 = 13;
	px_dword m = 5;
	px_dword n = 0xe6546b64;
	px_dword h = 0;
	px_dword k = 0;
	px_byte* d = (px_byte*)key; // 32 bit extract from `key'
	const px_dword* chunks = PX_NULL;
	const px_byte* tail = PX_NULL; // tail - last 8 bytes
	px_int i = 0;
	px_int l = len / 4; // chunk length

	h = 0x31415926;

	chunks = (const px_dword*)(d + l * 4); // body
	tail = (const px_byte*)(d + l * 4); // last 8 byte chunk of `key'

	// for each 4 byte chunk of `key'
	for (i = -l; i != 0; ++i) {
		// next 4 byte chunk of `key'
		k = chunks[i];

		// encode next 4 byte chunk of `key'
		k *= c1;
		k = (k << r1) | (k >> (32 - r1));
		k *= c2;

		// append to hash
		h ^= k;
		h = (h << r2) | (h >> (32 - r2));
		h = h * m + n;
	}

	k = 0;

	// remainder
	switch (len & 3) { // `len % 4'
	case 3: k ^= (tail[2] << 16);
	case 2: k ^= (tail[1] << 8);

	case 1:
		k ^= tail[0];
		k *= c1;
		k = (k << r1) | (k >> (32 - r1));
		k *= c2;
		h ^= k;
	}

	h ^= len;

	h ^= (h >> 16);
	h *= 0x85ebca6b;
	h ^= (h >> 13);
	h *= 0xc2b2ae35;
	h ^= (h >> 16);

	return h;
}
