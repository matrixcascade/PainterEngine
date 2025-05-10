#include "runtime/PainterEngine_Application.h"

unsigned int PX_TimeGetTime()
{
	//add your time function here
}
void lcd_render(px_color* gram, px_int width, px_int height)
{
	//add your lcd render function here
}

void OnTouch(px_int x,px_int y,px_int type)
{
	PX_Object_Event e = {0};
	
	switch (type)
	{
	case 0: // Touch down
		e.Event = PX_OBJECT_EVENT_CURSORDOWN;
		break;
	case 1: // Touch move
		e.Event = PX_OBJECT_EVENT_CURSORMOVE;
		break;
	case 2: // Touch up
		e.Event = PX_OBJECT_EVENT_CURSORUP;
		break;
	default:
		return;
	}
	PX_Object_Event_SetCursorX(&e, x * 1.f);
	PX_Object_Event_SetCursorY(&e, y * 1.f);
	PX_ObjectPostEvent(App.object_root, e);
}


int main()
{
	px_int screen_width=240;
	px_int screen_height=320;
	px_dword timelasttime = 0;
	// other initialization code here

	////////////////////////////////////
	if (!PX_ApplicationInitialize(&App,screen_width,screen_height))
	{
		return 0;
	}
	timelasttime=PX_TimeGetTime();
	while (1)
	{
		px_dword now=PX_TimeGetTime();
		px_dword elapsed=now-timelasttime;
		timelasttime= now;
		PX_ApplicationUpdate(&App,elapsed);
		PX_ApplicationRender(&App,elapsed);
		lcd_render(App.runtime.RenderSurface.surfaceBuffer, App.runtime.RenderSurface.width, App.runtime.RenderSurface.height);
	}
}