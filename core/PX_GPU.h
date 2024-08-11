//#define PX_GPU_ENABLE
#ifdef PX_GPU_ENABLE
#include "PX_Typedef.h"

#include "xil_cache.h"
#include "xparameters.h"


#define BLENDER_ARGB_MODE_AXXX  0
#define BLENDER_ARGB_MODE_XXXA  1

#ifdef PX_COLOR_FORMAT_RGBA
#define PX_COLOR_FORMAT BLENDER_ARGB_MODE_XXXA
#endif

#ifdef PX_COLOR_FORMAT_BGRA
#define PX_COLOR_FORMAT BLENDER_ARGB_MODE_XXXA
#endif

#ifdef PX_COLOR_FORMAT_ARGB
#define PX_COLOR_FORMAT BLENDER_ARGB_MODE_AXXX
#endif

#ifdef PX_COLOR_FORMAT_ABGR
#define PX_COLOR_FORMAT BLENDER_ARGB_MODE_AXXX
#endif

#define PX_GPU_HDMI_OUT_WIDTH 800
#define PX_GPU_HDMI_OUT_HEIGHT 480
#define PX_GPU_HDMI_OUT_PIXEL_SIZE 3

#define PX_GPU_FRAME_BUFFER_1 (XPAR_PS7_DDR_0_BASEADDRESS + 0x800000)
#define PX_GPU_FRAME_BUFFER_2 (XPAR_PS7_DDR_0_BASEADDRESS + 0xA00000)
#define PX_GPU_RUNTIME_START  (XPAR_PS7_DDR_0_BASEADDRESS + 0x1000000)

px_dword PX_GPU_Initialize();
px_bool PX_GPU_isEnable();
px_void PX_GPU_Enable(px_bool _PX_GPU_ENBALE);

px_dword PX_GPU_TimeGetTime_us();
px_void PX_GPU_Reset();
px_dword PX_GPU_GetDebug();
px_bool PX_GPU_mempcy(px_void* src, px_void* dst, px_int _4_byte_count);
px_bool PX_GPU_Render(px_void* texture_src_addr, px_dword width, px_dword x_count, px_dword y_count, px_void* texture_dst_addr, px_dword dst_width, px_dword color_format, px_dword blend);
px_bool PX_GPU_RenderTriangleRasterizer(px_void* texture_src_addr, px_dword width, px_dword x_count, px_dword y_count, px_void* texture_dst_addr, px_dword dst_width, px_dword color_format, px_dword blend, \
	px_int x1, px_int y1, px_int x2, px_int y2, px_int x3, px_int y3, px_color poscolor, px_color negcolor);
px_bool PX_GPU_Present(px_void* texture_addr,px_int width,px_int height);
px_void PX_GPU_CacheToDDR();
px_void PX_GPU_DDRToCache();

#endif // !PX_GPU
