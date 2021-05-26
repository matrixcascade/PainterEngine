#include "px_display.h"

static HWND g_hwng;
static HDC  g_targetDC;
static HDC  g_compatibleDC;
static int  g_width,g_height;

int PX_DisplayInitialize(HWND hwdn, int width, int height)
{
	g_hwng = hwdn;
	g_width = width;
	g_height = height;
	g_targetDC = GetDC(hwdn);
	if (!g_targetDC)
	{
		goto _ERROR;
	}
	g_compatibleDC = CreateCompatibleDC(g_targetDC);
	if (!g_compatibleDC)
	{
		goto _ERROR;
	}

	return 1;
_ERROR:
	if (g_targetDC) ReleaseDC(g_hwng,g_targetDC);
	if (g_compatibleDC) ReleaseDC(g_hwng,g_compatibleDC);
	g_targetDC = 0;
	g_compatibleDC = 0;
	return 0;
}

void PX_DisplayRender(void* bgra,int oftx,int ofty)
{
	typedef struct  
	{
		unsigned char r, g, b, a;
	}s_color;

	typedef struct
	{
		unsigned char b, g, r, a;
	}d_color;

	BITMAPINFO bmi;
	HBITMAP hbitmap=0;
	void* prgbdata;
	s_color* pclr = (s_color *)bgra;
	d_color* pwclr;
	int x, y;

	


	ZeroMemory(&bmi, sizeof(BITMAPINFO));
	bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmi.bmiHeader.biBitCount = 32;
	bmi.bmiHeader.biHeight = g_height;
	bmi.bmiHeader.biWidth = g_width;
	bmi.bmiHeader.biPlanes = 1;

	hbitmap = CreateDIBSection(NULL, &bmi, DIB_PAL_COLORS,&prgbdata, NULL, 0);
	pwclr = (d_color*)prgbdata;

	for (y = 0; y < g_height; y++)
		for (x = 0; x < g_width; x++)
		{
			pwclr[x + g_width * (g_height - y-1)].a = pclr[x + g_width * y].a;
			pwclr[x + g_width * (g_height - y-1)].r = pclr[x + g_width * y].r;
			pwclr[x + g_width * (g_height - y-1)].g = pclr[x + g_width * y].g;
			pwclr[x + g_width * (g_height - y-1)].b = pclr[x + g_width * y].b;
		}

	if (hbitmap)
	{
		SelectObject(g_compatibleDC, hbitmap);
		BitBlt(g_targetDC, oftx, ofty, g_width, g_height, g_compatibleDC, 0, 0, SRCCOPY);
		DeleteObject(hbitmap);
	}
	
	
}

