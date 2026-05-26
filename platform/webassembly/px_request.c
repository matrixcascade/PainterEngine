#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

#include "PainterEngine_Application.h"

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
	if (memcmp(url, "openurl:", 8) == 0)
	{
		EM_ASM({
			var targetUrl = UTF8ToString($0);
			window.open(targetUrl, '_blank');
		}, url + 8);
		if (func_callback)
			func_callback(buffer, 0, ptr);
		free(info);
	}
	else if (PX_strequ2(url,"open"))
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
		free(info);
	}
	else if(PX_memequ(url,"download:",9))
	{
		PX_RequestDownloadFile(url+9, buffer, size);
		if(func_callback)
			func_callback(buffer, size, ptr);
		free(info);
	}
	else
	{
		emscripten_async_wget_data(url,info, px_em_async_wget_onload_func, px_em_arg_callback_func);
	}
}
