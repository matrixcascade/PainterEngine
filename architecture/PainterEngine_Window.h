#ifndef PX_WINDOW_H
#define PX_WINDOW_H

#include "PainterEngine_Runtime.h"

#define PX_WINDOW_TITLE_LEN 32
#define PX_WINDOW_BAR_SIZE 24
typedef struct
{
	PX_Runtime *pruntime;
	px_memorypool *mp;
	px_float      x,y,z;
	px_int      width,height;
	PX_Object	*Area;
	PX_Object	*btn_close;
	PX_Object	*HScroll,*VScroll;
	px_bool		onFocus;
	px_color    borderColor,barColor;
	px_char		title[PX_WINDOW_TITLE_LEN];
	PX_FontModule *fontmodule;
	px_bool		visible;
	px_texture  renderTarget;
	px_point    lastDragPosition;
	px_bool     bDraging;
}PX_Window;

px_bool PX_WindowInitialize(PX_Runtime *runtime,px_memorypool *mp,PX_Window *pwindow,px_int x,int y,px_int width,px_int height,const px_char title[],PX_FontModule *fontmodule);
px_void PX_WindowPostEvent(PX_Window *pwindow,PX_Object_Event e);
px_void PX_WindowUpdate(PX_Window *pwindow,px_dword elpased);
px_void PX_WindowClose(PX_Window *pwindow);
px_void PX_WindowRender(px_surface *pSurface,PX_Window *pwindow,px_dword elpased);


#endif