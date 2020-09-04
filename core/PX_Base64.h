#ifndef PX_BASE64_H
#define PX_BASE64_H
#include "PX_Typedef.h"
px_uint PX_Base64Encode(const px_byte *_in, px_uint input_size, px_char *out);
px_uint PX_Base64Decode(const px_char *_in, px_uint input_size, px_byte *out);
px_uint PX_Base64GetEncodeLen(px_uint codeLen);
px_uint PX_Base64GetDecodeLen(px_uint codeLen);

#endif
