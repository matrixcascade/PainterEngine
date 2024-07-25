//#define PX_GPU_ENABLE
#ifdef PX_GPU_ENABLE
#include "PX_Typedef.h"

typedef enum
{
	VIDEO_DISPLAY_MODE_1280_720 = 0,
	VIDEO_DISPLAY_MODE_480_272 = 1,
	VIDEO_DISPLAY_MODE_640_480 = 2,
	VIDEO_DISPLAY_MODE_800_480 = 3,
	VIDEO_DISPLAY_MODE_800_600 = 4,
	VIDEO_DISPLAY_MODE_1024_768 = 5,
	VIDEO_DISPLAY_MODE_1920_1080 = 6,
}VIDEO_DISPLAY_MODE;

typedef enum
{
	DVI_RGB_MODE_ARGB = 0,
	DVI_RGB_MODE_RGBA = 1,
	DVI_RGB_MODE_ABGR = 2,
	DVI_RGB_MODE_BGRA = 3,
}DVI_RGB_MODE;

#define BLENDER_ARGB_MODE_AXXX  0
#define BLENDER_ARGB_MODE_XXXA  1

px_dword PX_GPU_CheckEnable();
px_void PX_GPU_Reset();
px_dword PX_GPU_GetDebug();
px_bool PX_GPU_mempcy(px_void* src, px_void* dst, px_int _4_byte_count);
px_bool PX_GPU_Render(px_void* texture_src_addr, px_dword width, px_dword x_count, px_dword y_count, px_void* texture_dst_addr, px_dword dst_width, px_dword color_format, px_dword blend);
px_bool PX_GPU_Present(px_void* texture_addr, px_dword width, px_dword height, VIDEO_DISPLAY_MODE display_mode, DVI_RGB_MODE dvi_rgb_mode);


#endif // !PX_GPU
