#ifndef PAINTERNEGINT_PNG_H
#define PAINTERNEGINT_PNG_H

#include "stdio.h"
#include "stdlib.h"
#include "memory.h"
#include "libpng/png.h"
#include "../PainterEngine/Core/PX_Core.h"

#ifdef _DEBUG
#pragma comment(lib,"libpngd.lib")
#pragma comment(lib,"zlibd.lib")
#else
#pragma comment(lib,"libpng.lib")
#pragma comment(lib,"zlib.lib")
#endif


px_bool PX_TextureCreateFromPngFile(px_memorypool *mp,px_texture *pTexture,const char *path);
px_bool PX_SurfaceToPngFile(px_surface *psurface,const px_char *filename);

#endif
