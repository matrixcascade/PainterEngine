#include "PX_GPU.h"

#ifdef PX_GPU_ENABLE
#include "xil_cache.h"
#define PX_GPU_BASE_ADDRESS 0x40000000
#define PX_GPU_REGISTER(x) *((px_dword *)(PX_GPU_BASE_ADDRESS)+(x))
#define PX_GPU_OPCODE PX_GPU_REGISTER(0)
#define PX_GPU_PARAM(x) PX_GPU_REGISTER(x+1)

#define PX_GPU_VERSION PX_GPU_REGISTER(16)
#define PX_GPU_STATE (PX_GPU_REGISTER(16+1)&0xff)
#define PX_GPU_GPUINFO_RETURN PX_GPU_REGISTER(16+2)
#define PX_GPU_GPUINFO_STATE PX_GPU_REGISTER(16+3)
#define PX_GPU_MEMCPY_STATE PX_GPU_REGISTER(16+4)
#define PX_GPU_DISPLAY_STATE PX_GPU_REGISTER(16+5)
#define PX_GPU_RENDERER_STATE PX_GPU_REGISTER(16+6)


px_void PX_GPU_Reset()
{
	if (PX_GPU_VERSION != 0x31415930)
		return;

	PX_GPU_OPCODE = 0;
	while (PX_GPU_STATE != 0);
}

px_dword PX_GPU_CheckEnable()
{
	volatile px_dword state;
	if (PX_GPU_VERSION != 0x31415930)
		return PX_FALSE;

	PX_GPU_Reset();
	PX_GPU_PARAM(0) = 1;
	PX_GPU_OPCODE = 1;

	while (PX_TRUE)
	{
		state = PX_GPU_GPUINFO_STATE;
		if (state==2)
		{
			return PX_FALSE;
		}
		if (state == 3)
		{
			return PX_GPU_GPUINFO_RETURN;
		}
	}
	return 0;
}

px_dword PX_GPU_GetDebug()
{
	volatile px_dword state;
	PX_GPU_Reset();
	PX_GPU_PARAM(0) = 2;
	PX_GPU_OPCODE = 1;

	while (PX_TRUE)
	{
		state = PX_GPU_GPUINFO_STATE;
		if (state == 2)
		{
			return PX_FALSE;
		}
		if (state == 3)
		{
			return PX_GPU_GPUINFO_RETURN;
		}
	}
	return 0;
}

#define GPU_MEMCPY_STATE_INIT						0x00
#define GPU_MEMCPY_STATE_PUSH_PARAM					0x01
#define GPU_MEMCPY_STATE_CALC_PROCESS				0x02
#define GPU_MEMCPY_STATE_READ						0x03
#define GPU_MEMCPY_STATE_READ_WAIT					0x04
#define GPU_MEMCPY_STATE_WRITE						0x05
#define GPU_MEMCPY_STATE_WRITE_WAIT					0x06
#define GPU_MEMCPY_STATE_CHECKSIZE					0x07
#define GPU_MEMCPY_STATE_DONE						0x08
#define GPU_MEMCPY_STATE_LENGTH_ERROR				0x09
#define GPU_MEMCPY_STATE_DMA_READER_ERROR			0x0A
#define GPU_MEMCPY_STATE_DMA_WRITER_ERROR			0x0B
        
px_bool PX_GPU_mempcy(px_void* src, px_void* dst, px_int _4_byte_count)
{
	volatile px_dword state;
	Xil_DCacheDisable();
	PX_GPU_Reset();
	PX_GPU_PARAM(0) = (px_dword)(src);
	PX_GPU_PARAM(1) = (px_dword)(dst);
	PX_GPU_PARAM(2) = (px_dword)(_4_byte_count);
	PX_GPU_OPCODE = 2;

	while (PX_TRUE)
	{
		state = PX_GPU_MEMCPY_STATE;
		if (state == GPU_MEMCPY_STATE_LENGTH_ERROR)
		{
			PX_LOG("gpu memcpy length error");
			return PX_FALSE;
		}
		if (state == GPU_MEMCPY_STATE_DMA_READER_ERROR)
		{
			PX_LOG("gpu dma reader error");
			return PX_FALSE;
		}

		if (state == GPU_MEMCPY_STATE_DMA_WRITER_ERROR)
		{
			PX_LOG("gpu dma writer error");
			return PX_FALSE;
		}

		if(state==GPU_MEMCPY_STATE_DONE)
		{
			Xil_DCacheDisable();
			return PX_TRUE;
		}
	}
	return 0;
}

#define DISPLAY_STATE_INIT 			  0
#define DISPLAY_STATE_CALC1 		  1
#define DISPLAY_STATE_CALC2 		  2
#define DISPLAY_STATE_STREAMING 	  3
#define DISPLAY_STATE_CHECK		 	  4
#define DISPLAY_STATE_DONE 			  5
#define DISPLAY_STATE_ERROR 		  6

px_bool PX_GPU_Present(px_void* texture_addr, px_dword width, px_dword height, VIDEO_DISPLAY_MODE display_mode, DVI_RGB_MODE dvi_rgb_mode)
{
	PX_GPU_Reset();
	Xil_DCacheDisable();
	PX_GPU_PARAM(0) = (px_dword)texture_addr;
	PX_GPU_PARAM(1) = (px_dword)width;
	PX_GPU_PARAM(2) = (px_dword)height;
	PX_GPU_PARAM(3) = (px_dword)(display_mode)+((px_dword)dvi_rgb_mode<<4);//[2:0]bit displaymode 4-6 argb color mode 
	PX_GPU_OPCODE = 3;
   
	while (PX_TRUE)
	{
		px_dword state = PX_GPU_DISPLAY_STATE;
        px_dword gpu_state=PX_GPU_STATE;
		switch (state&0x7)
		{
		case DISPLAY_STATE_DONE:
			return PX_TRUE;
		case DISPLAY_STATE_ERROR:
			return PX_FALSE;
		default:
			break;
		}
	}
	return PX_FALSE;
}

#define GPU_RENDERER_STATE_INIT 			0x00
#define GPU_RENDERER_STATE_CALC 			0x01
#define GPU_RENDERER_STATE_CALC2 			0x02
#define GPU_RENDERER_STATE_1_READING 		0x03
#define GPU_RENDERER_STATE_2_READING 		0x04
#define GPU_RENDERER_STATE_WRITING 			0x05
#define GPU_RENDERER_STATE_DONE 			0x06
#define GPU_RENDERER_STATE_READER1_ERROR 	0x07
#define GPU_RENDERER_STATE_READER2_ERROR 	0x08
#define GPU_RENDERER_STATE_WRITER_ERROR 	0x09


px_bool PX_GPU_Render(px_void* texture_src_addr, px_dword width, px_dword x_count, px_dword y_count, px_void* texture_dst_addr, px_dword dst_width,px_dword color_format, px_dword blend)
{
	PX_GPU_Reset();
	Xil_DCacheDisable();
	PX_GPU_PARAM(0) = (px_dword)texture_src_addr;
	PX_GPU_PARAM(1) = (px_dword)width;
	PX_GPU_PARAM(2) = (px_dword)x_count;
	PX_GPU_PARAM(3) = (px_dword)y_count;
	PX_GPU_PARAM(4) = (px_dword)texture_dst_addr;
	PX_GPU_PARAM(5) = (px_dword)dst_width;
	PX_GPU_PARAM(6) = (px_dword)color_format;
	PX_GPU_PARAM(7) = (px_dword)blend;
	PX_GPU_OPCODE = 4;

	while (PX_TRUE)
	{
		px_dword state = PX_GPU_RENDERER_STATE;

		switch (state)
		{
		case GPU_RENDERER_STATE_INIT:
			PX_LOG("renderer initializing");
			break;
		case GPU_RENDERER_STATE_CALC:
			PX_LOG("renderer calculating");
			break;
		case GPU_RENDERER_STATE_1_READING:
			PX_LOG("renderer fifo 1 reading");
			break;
		case GPU_RENDERER_STATE_2_READING:
			PX_LOG("renderer fifo 2 reading");
			break;
		case GPU_RENDERER_STATE_WRITING:
			PX_LOG("renderer fifo writing");
			break;
		case GPU_RENDERER_STATE_DONE:
			PX_LOG("renderer done");
            return PX_TRUE;
			break;
		case GPU_RENDERER_STATE_READER1_ERROR:
			PX_LOG("Reader1 error");
			return PX_FALSE;
			break;
		case GPU_RENDERER_STATE_READER2_ERROR:
			PX_LOG("Reader2 error");
			return PX_FALSE;
			break;
		case GPU_RENDERER_STATE_WRITER_ERROR:
			PX_LOG("Writer error");
			return PX_FALSE;
			break;
		default:
			PX_LOG("GPU unknow error");
			return PX_FALSE;
		}
	}
	return 0;
}
#endif