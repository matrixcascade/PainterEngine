
#ifndef PX_HTTP_H__
#define PX_HTTP_H__

#include "PX_Memory.h"

px_bool PX_HttpCheckContent(const px_char content[]);
px_bool PX_HttpGetContent(const px_char content[], const px_char requestHeader[], px_char payload[], px_int payload_size);
px_int  PX_HttpGetHttpHeaderSize(const px_char content[]);
px_bool PX_HttpContentIsComplete(const px_char content[], px_int size);
px_bool PX_HttpGetRequestPath(const px_char content[], px_int content_size, px_char path[], px_int path_size);
#endif