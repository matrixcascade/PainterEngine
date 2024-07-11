//#define PX_GPU_ENABLE
#ifdef PX_GPU_ENABLE
#include "PX_Typedef.h"

px_dword PX_GPU_CheckEnable();
px_void PX_GPU_Reset();
px_dword PX_GPU_GetDebug();
px_bool PX_GPU_mempcy(px_void* src, px_void* dst, px_int size);
px_bool PX_GPU_Render(px_void* texture_src_addr, px_dword width, px_dword x_count, px_dword y_count, px_void* texture_dst_addr, px_dword dst_width, px_dword color_format, px_dword blend);
px_bool PX_GPU_Present(px_void* texture_addr, px_dword width, px_dword height, px_dword present_size_mode, px_dword rgba_mode);


#endif // !PX_GPU
