#include "PX_GPU.h"

#ifdef PX_GPU_ENABLE



#define PX_GPU_BASE_ADDRESS XPAR_PAINTERENGINE_GPU_0_BASEADDR
#define PX_GPU_VDMA_BASE_ADDRESS XPAR_AXI_VDMA_0_BASEADDR

#define PX_GPU_REGISTER(x) *((volatile px_dword *)(PX_GPU_BASE_ADDRESS)+(x))
#define PX_GPU_OPCODE PX_GPU_REGISTER(0)
#define PX_GPU_PARAM(x) PX_GPU_REGISTER(x+1)

#define PX_GPU_VERSION PX_GPU_REGISTER(16)
#define PX_GPU_STATE (PX_GPU_REGISTER(16+1)&0xff)
#define PX_GPU_GPUINFO_RETURN PX_GPU_REGISTER(16+2)
#define PX_GPU_GPUINFO_STATE PX_GPU_REGISTER(16+3)
#define PX_GPU_MEMCPY_STATE PX_GPU_REGISTER(16+4)
#define PX_GPU_RENDERER_STATE PX_GPU_REGISTER(16+5)
#define PX_GPU_COLORCONVERT_STATE PX_GPU_REGISTER(16+6)

static px_bool PX_GPU_ENBALE=1;
static px_bool PX_GPU_isInitialized=0;


px_bool PX_GPU_isEnable()
{
	return PX_GPU_ENBALE && PX_GPU_isInitialized;
}

px_void PX_GPU_Enable(px_bool _PX_GPU_ENBALE)
{
	PX_GPU_ENBALE = _PX_GPU_ENBALE;
}


static px_void PX_GPU_Out32(px_dword Addr, px_dword Value)
{
	volatile px_dword* LocalAddr = (volatile px_dword*)Addr;
	*LocalAddr = Value;
}

static px_dword PX_GPU_In32(px_dword Addr)
{
	return *(volatile px_dword*)Addr;
}


px_void PX_GPU_Reset()
{
	if (PX_GPU_VERSION != 0x31415930)
		return;

	PX_GPU_OPCODE = 0;
	while (PX_GPU_STATE != 0);
}


px_dword PX_GPU_Initialize()
{
	volatile px_dword state;

	if (PX_GPU_VERSION != 0x31415930)
		return PX_FALSE;
	
	PX_memset(PX_GPU_FRAME_BUFFER_1, 0xff, PX_GPU_HDMI_OUT_WIDTH * PX_GPU_HDMI_OUT_HEIGHT * PX_GPU_HDMI_OUT_PIXEL_SIZE);
	PX_memset(PX_GPU_FRAME_BUFFER_2, 0xff, PX_GPU_HDMI_OUT_WIDTH * PX_GPU_HDMI_OUT_HEIGHT * PX_GPU_HDMI_OUT_PIXEL_SIZE);

	Xil_DCacheFlush();

	PX_GPU_Out32(PX_GPU_VDMA_BASE_ADDRESS+ 0x0, 0x4); //reset

	PX_GPU_Out32(PX_GPU_VDMA_BASE_ADDRESS+ 0x0, 0x8); //gen-lock

	PX_GPU_Out32(PX_GPU_VDMA_BASE_ADDRESS + 0x00, 0x00000001); // enable park mode

	PX_GPU_Out32(PX_GPU_VDMA_BASE_ADDRESS + 0x5c, PX_GPU_FRAME_BUFFER_1); // start address

	PX_GPU_Out32(PX_GPU_VDMA_BASE_ADDRESS + 0x60, PX_GPU_FRAME_BUFFER_2); // start address

	PX_GPU_Out32(PX_GPU_VDMA_BASE_ADDRESS + 0x58, (PX_GPU_HDMI_OUT_WIDTH * 3)); // h offset

	PX_GPU_Out32(PX_GPU_VDMA_BASE_ADDRESS + 0x54, (PX_GPU_HDMI_OUT_WIDTH * 3)); // h size

	PX_GPU_Out32(PX_GPU_VDMA_BASE_ADDRESS + 0x50, PX_GPU_HDMI_OUT_HEIGHT); // v size


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
			PX_GPU_isInitialized = PX_TRUE;
			return PX_GPU_GPUINFO_RETURN;
		}
	}
	return 0;
}

px_bool PX_GPU_Present(px_void* texture_addr, px_int width, px_int height)
{
	px_dword PARK_PTR_REG;
	px_int x,y;
	px_color *pColor=(px_color *)texture_addr;
    px_byte  *pDst;
	if (width<=0||height<=0)
	{
		return PX_FALSE;
	}

	PARK_PTR_REG = PX_GPU_In32(PX_GPU_VDMA_BASE_ADDRESS + 0x028);

	if (PARK_PTR_REG&0xf)
	{
		PARK_PTR_REG = PARK_PTR_REG & 0xfffffff0;
		PARK_PTR_REG = PARK_PTR_REG | 1;
		pDst = (px_byte *)PX_GPU_FRAME_BUFFER_2;
	}
	else
	{
		PARK_PTR_REG = PARK_PTR_REG & 0xfffffff0;
		pDst = (px_byte *)PX_GPU_FRAME_BUFFER_1;
	}
    px_dword latency;
	if (width==PX_GPU_HDMI_OUT_WIDTH&&height==PX_GPU_HDMI_OUT_HEIGHT)
	{
		PX_GPU_ColorConvert(texture_addr, pDst, PX_GPU_HDMI_OUT_WIDTH * PX_GPU_HDMI_OUT_HEIGHT );
	}
	else
	{
		for (y = 0; y < PX_GPU_HDMI_OUT_HEIGHT && y < height; y++)
			for (x = 0; x < PX_GPU_HDMI_OUT_WIDTH && x < width; x++)
			{
#ifdef PX_COLOR_FORMAT_RGBA
				pDst[(y * PX_GPU_HDMI_OUT_WIDTH + x) * 3 + 0] = pColor[y * width + x]._argb.b;
				pDst[(y * PX_GPU_HDMI_OUT_WIDTH + x) * 3 + 1] = pColor[y * width + x]._argb.g;
				pDst[(y * PX_GPU_HDMI_OUT_WIDTH + x) * 3 + 2] = pColor[y * width + x]._argb.r;
#endif

#ifdef PX_COLOR_FORMAT_BGRA
				pDst[(y * PX_GPU_HDMI_OUT_WIDTH + x) * 3 + 0] = pColor[y * width + x]._argb.r;
				pDst[(y * PX_GPU_HDMI_OUT_WIDTH + x) * 3 + 1] = pColor[y * width + x]._argb.g;
				pDst[(y * PX_GPU_HDMI_OUT_WIDTH + x) * 3 + 2] = pColor[y * width + x]._argb.b;
#endif

#ifdef PX_COLOR_FORMAT_ARGB
				pDst[(y * PX_GPU_HDMI_OUT_WIDTH + x) * 3 + 0] = pColor[y * width + x]._argb.b;
				pDst[(y * PX_GPU_HDMI_OUT_WIDTH + x) * 3 + 1] = pColor[y * width + x]._argb.g;
				pDst[(y * PX_GPU_HDMI_OUT_WIDTH + x) * 3 + 2] = pColor[y * width + x]._argb.r;
#endif

#ifdef PX_COLOR_FORMAT_ABGR
				pDst[(y * PX_GPU_HDMI_OUT_WIDTH + x) * 3 + 0] = pColor[y * width + x]._argb.r;
				pDst[(y * PX_GPU_HDMI_OUT_WIDTH + x) * 3 + 1] = pColor[y * width + x]._argb.g;
				pDst[(y * PX_GPU_HDMI_OUT_WIDTH + x) * 3 + 2] = pColor[y * width + x]._argb.b;
#endif
			}
	}
	Xil_DCacheFlush();
	PX_GPU_Out32((PX_GPU_VDMA_BASE_ADDRESS + 0x028), PARK_PTR_REG);
}

px_dword PX_GPU_TimeGetTime_us()
{
	volatile px_dword state;
	if (PX_GPU_VERSION != 0x31415930)
		return PX_FALSE;

	PX_GPU_Reset();
	PX_GPU_PARAM(0) = 3;
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
    Xil_DCacheFlushRange((px_dword)src,_4_byte_count*4);
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

		if (state == GPU_MEMCPY_STATE_DONE)
		{
            Xil_DCacheInvalidateRange((px_dword)dst, _4_byte_count*4);
			return PX_TRUE;
		}
	}
	return 0;
}


#define GPU_COLORCONVERT_STATE_INIT						0x00
#define GPU_COLORCONVERT_STATE_PUSH_PARAM				0x01
#define GPU_COLORCONVERT_STATE_CALC_PROCESS				0x02
#define GPU_COLORCONVERT_STATE_READ						0x03
#define GPU_COLORCONVERT_STATE_READ_WAIT				0x04
#define GPU_COLORCONVERT_STATE_WRITE					0x05
#define GPU_COLORCONVERT_STATE_WRITE_WAIT				0x06
#define GPU_COLORCONVERT_STATE_CHECKSIZE				0x07
#define GPU_COLORCONVERT_STATE_DONE						0x08
#define GPU_COLORCONVERT_STATE_LENGTH_ERROR				0x09
#define GPU_COLORCONVERT_STATE_DMA_READER_ERROR			0x0A
#define GPU_COLORCONVERT_STATE_DMA_WRITER_ERROR			0x0B

px_bool PX_GPU_ColorConvert(px_void* src, px_void* dst, px_int _4_byte_count)
{
	px_byte colormode = 0;
	px_byte convert_mode = 0;
	volatile px_dword state;
	Xil_DCacheFlushRange((px_dword)src, _4_byte_count * 4);
	PX_GPU_Reset();
	PX_GPU_PARAM(0) = (px_dword)(src);
	PX_GPU_PARAM(1) = (px_dword)(dst);
	PX_GPU_PARAM(2) = (px_dword)(_4_byte_count);

#ifdef PX_COLOR_FORMAT_RGBA
	colormode = 0;
#endif

#ifdef PX_COLOR_FORMAT_BGRA
	colormode = 1;
#endif

#ifdef PX_COLOR_FORMAT_ARGB
	colormode = 2;
#endif

#ifdef PX_COLOR_FORMAT_ABGR
	colormode = 3;
#endif
	PX_GPU_PARAM(3) = (px_dword)(colormode)+(0<<2);


	PX_GPU_OPCODE = 4;

	while (PX_TRUE)
	{
		state = PX_GPU_COLORCONVERT_STATE;
		if (state == GPU_COLORCONVERT_STATE_LENGTH_ERROR)
		{
			PX_LOG("gpu COLORCONVERT length error");
			return PX_FALSE;
		}
		if (state == GPU_COLORCONVERT_STATE_DMA_READER_ERROR)
		{
			PX_LOG("gpu dma reader error");
			return PX_FALSE;
		}

		if (state == GPU_COLORCONVERT_STATE_DMA_WRITER_ERROR)
		{
			PX_LOG("gpu dma writer error");
			return PX_FALSE;
		}

		if (state == GPU_COLORCONVERT_STATE_DONE)
		{
			Xil_DCacheInvalidateRange((px_dword)dst, _4_byte_count * 4);
			return PX_TRUE;
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


#define GPU_RENDERER_STATE_INIT				0x00
#define GPU_RENDERER_STATE_CHECKX			0x01
#define GPU_RENDERER_STATE_CHECKY			0x02
#define GPU_RENDERER_STATE_CALC				0x03
#define GPU_RENDERER_STATE_CALC2			0x04
#define GPU_RENDERER_STATE_CALC3			0x05
#define GPU_RENDERER_STATE_CALC_ADDRESS		0x06
#define GPU_RENDERER_STATE_1_READING		0x07
#define GPU_RENDERER_STATE_2_READING		0x08
#define GPU_RENDERER_STATE_WRITING			0x09
#define GPU_RENDERER_STATE_INC				0x0A
#define GPU_RENDERER_STATE_DONE				0x0B
#define GPU_RENDERER_STATE_READER1_ERROR	0x0C
#define GPU_RENDERER_STATE_READER2_ERROR	0x0D
#define GPU_RENDERER_STATE_WRITER_ERROR		0x0E



px_bool PX_GPU_Render(px_void* texture_src_addr, px_dword width, px_dword x_count, px_dword y_count, px_void* texture_dst_addr, px_dword dst_width,px_dword color_format, px_dword blend)
{
    Xil_DCacheFlush();
	PX_GPU_Reset();
	PX_GPU_PARAM(0) = (px_dword)texture_src_addr;
	PX_GPU_PARAM(1) = (px_dword)width;
	PX_GPU_PARAM(2) = (px_dword)x_count;
	PX_GPU_PARAM(3) = (px_dword)y_count;
	PX_GPU_PARAM(4) = (px_dword)texture_dst_addr;
	PX_GPU_PARAM(5) = (px_dword)dst_width;
	PX_GPU_PARAM(6) = (px_dword)color_format;
	PX_GPU_PARAM(7) = (px_dword)blend;
	PX_GPU_PARAM(8) = (px_dword)0;//mode
	PX_GPU_PARAM(9) = (px_dword)0;//pt1
	PX_GPU_PARAM(10) = (px_dword)0;//pt2
	PX_GPU_PARAM(11) = (px_dword)0;//pt3
	PX_GPU_PARAM(12) = (px_dword)0;//yes color
	PX_GPU_PARAM(13) = (px_dword)0;//no color
	PX_GPU_OPCODE = 3;
	while (PX_TRUE)
	{
		px_dword state = PX_GPU_RENDERER_STATE;

		switch (state)
		{
		case GPU_RENDERER_STATE_DONE:
            Xil_DCacheInvalidate();
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
			continue;
		}
	}
	return 0;
}

px_bool PX_GPU_RenderTriangleRasterizer(px_void* texture_src_addr, px_dword width, px_dword x_count, px_dword y_count, px_void* texture_dst_addr, px_dword dst_width, px_dword color_format, px_dword blend,\
	px_int x1, px_int y1, px_int x2, px_int y2, px_int x3, px_int y3, px_color poscolor, px_color negcolor)
{
	Xil_DCacheFlush();
	PX_GPU_Reset();
	PX_GPU_PARAM(0) = (px_dword)texture_src_addr;
	PX_GPU_PARAM(1) = (px_dword)width;
	PX_GPU_PARAM(2) = (px_dword)x_count;
	PX_GPU_PARAM(3) = (px_dword)y_count;
	PX_GPU_PARAM(4) = (px_dword)texture_dst_addr;
	PX_GPU_PARAM(5) = (px_dword)dst_width;
	PX_GPU_PARAM(6) = (px_dword)color_format;
	PX_GPU_PARAM(7) = (px_dword)blend;
	PX_GPU_PARAM(8) = (px_dword)1;//mode
	PX_GPU_PARAM(9) = (px_dword)(x1+(y1<<16));//pt1
	PX_GPU_PARAM(10) = (px_dword)(x2 + (y2 << 16));//pt2
	PX_GPU_PARAM(11) = (px_dword)(x3 + (y3 << 16));//pt3
	PX_GPU_PARAM(12) = (px_dword)poscolor._argb.ucolor;//yes color
	PX_GPU_PARAM(13) = (px_dword)negcolor._argb.ucolor;//no color
	PX_GPU_OPCODE = 3;

	while (PX_TRUE)
	{
		px_dword state = PX_GPU_RENDERER_STATE;

		switch (state)
		{
		case GPU_RENDERER_STATE_DONE:
			Xil_DCacheInvalidate();
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
			continue;
		}
	}
	return 0;
}
#endif