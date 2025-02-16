// Code by DBinary.  All rights reserved.
// The codes is available under MIT licenses

#ifdef _MSC_VER
#pragma comment(lib,"SDL.lib")
#pragma comment(lib,"winmm.lib")
#pragma comment(lib, "dxguid.lib")
#endif // _MSC_VER


#include <stdio.h>
#include <SDL.h>
#include <string.h>
#include <stdlib.h>

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

extern void PX_Sleep(unsigned int ms);
///////////////////////////////////////////////

EM_JS(char*, PX_GetInputTextUTF8, (const char *_prompt_text), {
  var jsString = prompt(UTF8ToString(_prompt_text),"");
  if(jsString==null)
  {
	return 0;
  }
var lengthBytes = lengthBytesUTF8(jsString) + 1;
var stringOnWasmHeap = _malloc(lengthBytes);
stringToUTF8(jsString, stringOnWasmHeap, lengthBytes);
return stringOnWasmHeap;
	});

EM_JS(char*, PX_GetInputTextGBK, (const char* _prompt_text), {
  var jsString = prompt(GBKToString(_prompt_text),"");
  if(jsString==null)
  {
	return 0;
  }
var lengthBytes = lengthBytesGBK(jsString) + 1;
var stringOnWasmHeap = _malloc(lengthBytes);
stringToGBK(jsString, stringOnWasmHeap, lengthBytes);
return stringOnWasmHeap;
	});

const px_char* PX_InputText(const px_char prompt[], const px_char charset[])
{
	if (PX_strequ2(charset,"UTF8")|| PX_strequ2(charset, "UTF-8"))
	{
		return PX_GetInputTextUTF8(prompt);
	}
	return PX_GetInputTextGBK(prompt);
}

typedef struct
{
	void* buffer;
	int size;
	void* ptr;
	void (*func_callback)(void* buffer, int size, void* ptr);
}PX_RequestData_Info;

void px_em_async_wget_onload_func(void* arg, void*buffer, int size)
{
	PX_RequestData_Info *info = (PX_RequestData_Info *)arg;
	if (size<info->size)
	{
		memcpy(info->buffer, buffer, size);
		info->func_callback(info->buffer, size, info->ptr);
	}
	else
	{
		info->func_callback(info->buffer, 0, info->ptr);
	}
	free(info);
}

void px_em_arg_callback_func(void*ptr)
{
	PX_RequestData_Info *info = (PX_RequestData_Info *)ptr;
	info->func_callback(info->buffer, 0, info->ptr);
	free(info);
}

PX_RequestData_Info *load_file_info;
EMSCRIPTEN_KEEPALIVE int load_file(uint8_t *buffer, size_t size) 
{
  if (load_file_info)
  {
	  if (size<load_file_info->size)
	  {
		  memcpy(load_file_info->buffer, buffer, size);
		  load_file_info->func_callback(load_file_info->buffer, size, load_file_info->ptr);
	  }
	  else
	  {
		  load_file_info->func_callback(load_file_info->buffer, 0, load_file_info->ptr);
	  }
	  free(load_file_info);
	  load_file_info=PX_NULL;
  }
  return 1;
}


EM_JS(void, downloadFile, (const char* filename, const void* data, size_t size), {
  const array = new Uint8Array(Module.HEAPU8.buffer, data, size);
  const blob = new Blob([array], { type: 'arraybuffer' });
  const url = URL.createObjectURL(blob);

  const link = document.createElement('a');
  link.href = url;
  link.download = filename;
  link.click();
  URL.revokeObjectURL(url);
});


void PX_RequestDownloadFile(const char* filename, const void* data, size_t size) {
  EM_ASM({
    const filename = UTF8ToString($0);
    const dataPtr = $1;
    const dataSize = $2;

    downloadFile(filename, dataPtr, dataSize);
  }, filename, data, size);
}


void PX_RequestData(const char url[], void* buffer, int size, void* ptr, void (*func_callback)(void* buffer, int size, void* ptr))
{
	PX_RequestData_Info *info = (PX_RequestData_Info *)malloc(sizeof(PX_RequestData_Info));
	if (!info)
	{
		func_callback(buffer, 0, ptr);
		return;
	}
	
	info->buffer = buffer;
	info->size = size;
	info->ptr = ptr;
	info->func_callback = func_callback;
	if (PX_strequ2(url,"open"))
	{
EM_ASM(
  var file_selector = document.createElement('input');
  file_selector.setAttribute('type', 'file');
  file_selector.setAttribute('onchange','open_file(event)');
  file_selector.click();
);
load_file_info=info;
	}
	else if (PX_strequ2(url,"save"))
	{
		if(func_callback)
			func_callback(buffer, 0, ptr);
	}
	else if(PX_memequ(url,"download:",9))
	{
		PX_RequestDownloadFile(url+9, buffer, size);
		if(func_callback)
			func_callback(buffer, size, ptr);
	}
	else
	{
		emscripten_async_wget_data(url,info, px_em_async_wget_onload_func, px_em_arg_callback_func);
	}
}

px_int last_screen_width=0,last_screen_height=0;

void mainloop(void *ptr)
{
	px_dword elpased;
	SDL_Event event;
	px_dword timecurrent=SDL_GetTicks();
	

	elpased=timecurrent-lastupdatetime;
	lastupdatetime=timecurrent;

	if(last_screen_width!=App.runtime.surface_width||last_screen_height!=App.runtime.surface_height)
	{
		SDL_FreeSurface(screen);
		screen = SDL_SetVideoMode(App.runtime.surface_width, App.runtime.surface_height, 32, SDL_SWSURFACE);
		last_screen_width=App.runtime.surface_width;
		last_screen_height=App.runtime.surface_height;
	}


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
				PX_Object_Event e={0};;
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
		else if(event.type == SDL_MOUSEWHEEL)
    	{
			PX_Object_Event e={0};
			e.Event=PX_OBJECT_EVENT_CURSORWHEEL;
			PX_Object_Event_SetCursorX(&e,(px_float)lastcursorx);
			PX_Object_Event_SetCursorY(&e,(px_float)lastcursory);
			PX_Object_Event_SetCursorZ(&e,(px_float)event.wheel.y);
    	    PX_ApplicationPostEvent(&App,e);
    	}
		else if (SDL_MOUSEBUTTONUP== event.type) 
		{
			if(SDL_BUTTON_LEFT == event.button.button)
			{
				px_int px = event.button.x;
				px_int py = event.button.y;
				PX_Object_Event e={0};
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

			str[0]=event.key.keysym.sym;
			if (event.key.keysym.sym==SDLK_BACKSPACE)
			{
				str[0]=PX_VK_BACK;
			}
			else if (event.key.keysym.sym==SDLK_TAB)
			{
				str[0]=PX_VK_SPACE;
			}
			else if (event.key.keysym.sym==SDLK_DELETE)
			{
				str[0]=PX_VK_BACK;
			}
			else if (event.key.keysym.sym==SDLK_UP)
			{
				str[0]=PX_VK_UP;
			}
			else if (event.key.keysym.sym==SDLK_DOWN)
			{
				str[0]=PX_VK_DOWN;
			}
			else if (event.key.keysym.sym==SDLK_LEFT)
			{
				str[0]=PX_VK_LEFT;
			}
			else if (event.key.keysym.sym==SDLK_RIGHT)
			{
				str[0]=PX_VK_RIGHT;
			}
			else if(event.key.keysym.sym>=0x20&&event.key.keysym.sym<=0x7E)
			{
				str[0]=event.key.keysym.sym;
			}
			else
			{
				str[0]=0;
			}
			if(str[0])
			{
				PX_Object_Event_SetKeyDown(&e,event.key.keysym.sym);
				PX_ApplicationPostEvent(&App,e);
			}
		}
		else if(SDL_TEXTINPUT==event.type)
		{
			PX_Object_Event e;
			e.Event=PX_OBJECT_EVENT_STRING;
			PX_Object_Event_SetStringPtr(&e,event.text.text);
			PX_ApplicationPostEvent(&App,e);
		}
		else if (SDL_KEYUP == event.type)
		{
			px_char keye;
			PX_Object_Event e;
			e.Event=PX_OBJECT_EVENT_KEYUP;
			keye=event.key.keysym.sym;
			if (event.key.keysym.sym==SDLK_BACKSPACE)
			{
				keye=PX_VK_BACK;
			}
			else if (event.key.keysym.sym==SDLK_TAB)
			{
				keye=PX_VK_SPACE;
			}
			else if (event.key.keysym.sym==SDLK_DELETE)
			{
				keye=PX_VK_BACK;
			}
			else if (event.key.keysym.sym==SDLK_UP)
			{
				keye=PX_VK_UP;
			}
			else if (event.key.keysym.sym==SDLK_DOWN)
			{
				keye=PX_VK_DOWN;
			}
			else if (event.key.keysym.sym==SDLK_LEFT)
			{
				keye=PX_VK_LEFT;
			}
			else if (event.key.keysym.sym==SDLK_RIGHT)
			{
				keye=PX_VK_RIGHT;
			}
			else if(event.key.keysym.sym>=0x20&&event.key.keysym.sym<=0x7E)
			{
				keye=event.key.keysym.sym;
			}
			else
			{
				keye=0;
			}
			if(keye)
			{
				PX_Object_Event_SetKeyUp(&e,keye);
				PX_ApplicationPostEvent(&App,e);
			}
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
	PX_srand(PX_TimeGetTime());
	if(!PX_ApplicationInitialize(&App,info->current_w,info->current_h))
	{
		printf("PainterEngine startup failed.\n");
			return PX_FALSE;
	}
	printf("SDL Create surface %d %d\n",App.runtime.surface_width,App.runtime.surface_height);
	screen = SDL_SetVideoMode(App.runtime.surface_width, App.runtime.surface_height, 32, SDL_SWSURFACE);
	if (screen==PX_NULL)
	{
		printf("SDL Create surface failed.\n");
		return PX_FALSE;
	}
	last_screen_width=App.runtime.surface_width;
	last_screen_height=App.runtime.surface_height;
	lastupdatetime=SDL_GetTicks();

	emscripten_set_main_loop_arg(mainloop, 0, -1, 1);	
	return 0;
}
