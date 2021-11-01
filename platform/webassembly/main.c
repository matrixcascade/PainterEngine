// Copyright 2011 The Emscripten Authors.  All rights reserved.
// Emscripten is available under two separate licenses, the MIT license and the
// University of Illinois/NCSA Open Source License.  Both these licenses can be
// found in the LICENSE file.

#ifdef _MSC_VER
#pragma comment(lib,"SDL.lib")
#pragma comment(lib,"winmm.lib")
#pragma comment(lib, "dxguid.lib")
#endif // _MSC_VER


#include <stdio.h>
#include <SDL.h>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

#include "PainterEngine_Application.h"

///////////////////////////////////////////////
//global
SDL_Surface *screen;
const SDL_VideoInfo *info;
px_dword lastupdatetime;
px_int lastcursorx=-1,lastcursory=-1;
px_bool mouseldown=PX_FALSE;
///////////////////////////////////////////////




void mainloop(void *ptr)
{
	px_dword elpased;
	SDL_Event event;
	px_dword timecurrent=SDL_GetTicks();
	

	elpased=timecurrent-lastupdatetime;
	lastupdatetime=timecurrent;

	PX_ApplicationUpdate(&App,elpased);
	PX_ApplicationRender(&App,elpased);


	if (SDL_MUSTLOCK(screen)) SDL_LockSurface(screen);
	PX_memcpy(screen->pixels,App.runtime.RenderSurface.surfaceBuffer,App.runtime.surface_width*App.runtime.surface_height*4);
	if (SDL_MUSTLOCK(screen)) SDL_UnlockSurface(screen);
	SDL_Flip(screen); 

	while (SDL_PollEvent(&event))
	{
		if (SDL_MOUSEBUTTONDOWN == event.type) 
		{
			if(SDL_BUTTON_LEFT == event.button.button)
			{
				px_int px = event.button.x;
				px_int py = event.button.y;
				PX_Object_Event e;
				lastcursorx=px;
				lastcursory=py;
				mouseldown=PX_TRUE;
				e.Event=PX_OBJECT_EVENT_CURSORDOWN;
				PX_Object_Event_SetCursorX(&e,(px_float)px);
				PX_Object_Event_SetCursorY(&e,(px_float)py);
				PX_ApplicationPostEvent(&App,e);
			}
			else if(SDL_BUTTON_RIGHT == event.button.button)
			{
				px_int px = event.button.x;
				px_int py = event.button.y;
				PX_Object_Event e;
				e.Event=PX_OBJECT_EVENT_CURSORRDOWN;
				PX_Object_Event_SetCursorX(&e,(px_float)px);
				PX_Object_Event_SetCursorY(&e,(px_float)py);
				PX_ApplicationPostEvent(&App,e);
			}
		}
		else if (SDL_MOUSEBUTTONUP== event.type) 
		{
			if(SDL_BUTTON_LEFT == event.button.button)
			{
				px_int px = event.button.x;
				px_int py = event.button.y;
				PX_Object_Event e;
				mouseldown=PX_FALSE;
				if (px==lastcursorx&&py==lastcursory)
				{
					e.Event=PX_OBJECT_EVENT_CURSORCLICK;
					PX_Object_Event_SetCursorX(&e,(px_float)px);
					PX_Object_Event_SetCursorY(&e,(px_float)py);
					PX_ApplicationPostEvent(&App,e);
				}

				lastcursorx=px;
				lastcursory=py;
				e.Event=PX_OBJECT_EVENT_CURSORUP;
				PX_Object_Event_SetCursorX(&e,(px_float)px);
				PX_Object_Event_SetCursorY(&e,(px_float)py);
				PX_ApplicationPostEvent(&App,e);
			}
			else if(SDL_BUTTON_RIGHT == event.button.button)
			{
				px_int px = event.button.x;
				px_int py = event.button.y;
				PX_Object_Event e;
				e.Event=PX_OBJECT_EVENT_CURSORRUP;
				PX_Object_Event_SetCursorX(&e,(px_float)px);
				PX_Object_Event_SetCursorY(&e,(px_float)py);
				PX_ApplicationPostEvent(&App,e);
			}
		}
		else if (SDL_MOUSEMOTION == event.type) 
		{
			px_int px = event.button.x;
			px_int py = event.button.y;
			PX_Object_Event e;

			if (mouseldown)
				e.Event=PX_OBJECT_EVENT_CURSORDRAG;
			else
				e.Event=PX_OBJECT_EVENT_CURSORMOVE;

			PX_Object_Event_SetCursorX(&e,(px_float)px);
			PX_Object_Event_SetCursorY(&e,(px_float)py);
			PX_ApplicationPostEvent(&App,e);
		}
		else if (SDL_KEYDOWN == event.type)
		{
			PX_Object_Event e;
			px_char str[2]={0};
			e.Event=PX_OBJECT_EVENT_KEYDOWN;
			PX_Object_Event_SetKeyDown(&e,event.key.keysym.sym);
			PX_ApplicationPostEvent(&App,e);

			str[0]=event.key.keysym.sym;
			if ((str[0]>=0x20&&str[0]<=0x7E)||str[0]==0x08||str[0]==0x0d||str[0]==0x0a)
			{
				e.Event=PX_OBJECT_EVENT_STRING;
				PX_Object_Event_SetStringPtr(&e,str);
				PX_ApplicationPostEvent(&App,e);
			}

		}
		else if (SDL_KEYUP == event.type)
		{
			PX_Object_Event e;
			e.Event=PX_OBJECT_EVENT_KEYUP;
			PX_Object_Event_SetKeyUp(&e,event.key.keysym.sym);
			PX_ApplicationPostEvent(&App,e);
		}
		if (event.type == SDL_QUIT) {};
	}

}


#ifdef __cplusplus
extern "C"
#endif
int main(int argc, char *argv[])
{
	
	printf("PainterEngine Webassembly initializating...\n");
	SDL_Init(SDL_INIT_VIDEO);
	info=SDL_GetVideoInfo();
	if(!PX_ApplicationInitialize(&App,info->current_w,info->current_h))
	{
		printf("PainterEngine startup failed.\n");
			return PX_FALSE;
	}

	screen = SDL_SetVideoMode(App.runtime.surface_width, App.runtime.surface_height, 32, SDL_SWSURFACE);
	lastupdatetime=SDL_GetTicks();

	emscripten_set_main_loop_arg(mainloop, 0, -1, 1);	
	return 0;
}
