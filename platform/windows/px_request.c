#include "../modules/px_file.h"
#include "../modules/px_request.h"
#include "string.h"
#include "windows.h"
#include "shellapi.h"

extern char* PX_OpenFileDialog(char filter[]);
extern char* PX_SaveFileDialog(char filter[], const char defname[]);

px_char PX_RequestData_Extern[128] = {0};

void PX_RequestData(const char url[], void* buffer, int size, void* ptr, void (*func_callback)(void* buffer, int size, void* ptr))
{
	px_char filter[128] = { 0 };
	char* ppath;

	if (memcmp(url, "openurl:", 8) == 0)
	{
		ShellExecuteA(NULL, "open", url + 8, NULL, NULL, SW_SHOWNORMAL);
		if (func_callback)
			func_callback(buffer, 0, ptr);
	}
	else if (strstr(url,"open"))
	{
		PX_strcat(filter, url + 5);
		PX_strcat(filter, "\0");
		PX_strcat(filter + PX_strlen(filter) + 1, "*");
		PX_strcat(filter + PX_strlen(filter) + 1, url + 5);
		char* ppath = PX_OpenFileDialog(filter);
		if (ppath&&ppath[0])
		{
			PX_IO_Data io = PX_LoadFileToIOData(ppath);
			if (io.size > 0 && io.buffer && io.size <= (unsigned int)size)
			{
				PX_FileGetName(ppath, PX_RequestData_Extern, sizeof(PX_RequestData_Extern));
				memcpy(buffer, io.buffer, io.size);
				if(func_callback)
				func_callback(buffer, io.size, ptr);
			}
			else
			{
				if (func_callback)
				func_callback(buffer, 0, ptr);
			}
			PX_FreeIOData(&io);
		}
		else
		{
			if (func_callback)
			func_callback(buffer, 0, ptr);
		}
	}
	else if (memcmp(url, "save:", 5) == 0)
	{
		PX_strcat(filter, url + 5);
		PX_strcat(filter, "\0");
		PX_strcat(filter + PX_strlen(filter) + 1, "*");
		PX_strcat(filter + PX_strlen(filter) + 1, url + 5);
		ppath = PX_SaveFileDialog(filter, url + 5);
		if (ppath && ppath[0])
		{
			PX_SaveDataToFile(buffer, size, ppath);
		}
		else
		{
			if (func_callback)
			func_callback(buffer, 0, ptr);
		}
	}
	else if (memcmp(url, "download:", 9)==0)
	{
		PX_SaveDataToFile(buffer,size, (char*)url + 9);
	}
	else
	{
		PX_IO_Data io = PX_LoadFileToIOData(url);
		if (io.size > 0 && io.buffer && io.size <= (unsigned int)size)
		{
			memcpy(buffer, io.buffer, io.size);
			if (func_callback)
			func_callback(buffer, io.size, ptr);
		}
		else
		{
			if (func_callback)
			func_callback(buffer, 0, ptr);
		}
		PX_FreeIOData(&io);
	}
}
