
#ifndef PAINTERNEGINT_JPEG_H
#define PAINTERNEGINT_JPEG_H

#include "stdio.h"
#include "stdlib.h"
#include "memory.h"
#include "../PainterEngine/Core/PX_Core.h"

#include "./libjpeg/jpeglib.h"
#pragma comment(lib,"libjpeg.lib")

px_bool PX_TextureCreateFromJpegFile(px_memorypool *mp,px_texture *pTexture,const char *path);
px_bool PX_SurfaceToJpegFile(px_surface *psurface,const px_char *filename);

#endif