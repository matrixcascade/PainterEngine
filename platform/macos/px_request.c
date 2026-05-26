#include "platform/modules/px_file.h"
#include "platform/modules/px_request.h"
#include <string.h>
#include <stdlib.h>

void PX_RequestData(const char url[], void* buffer, int size, void* ptr, void (*func_callback)(void* buffer, int size, void* ptr))
{
    if (memcmp(url, "openurl:", 8) == 0)
    {
        char cmd[1024] = {0};
        snprintf(cmd, sizeof(cmd), "open \"%s\"", url + 8);
        system(cmd);
        if (func_callback)
            func_callback(buffer, 0, ptr);
    }
    else
    {
        PX_IO_Data io = PX_LoadFileToIOData(url);
        if (io.size > 0 && io.buffer && io.size <= (unsigned int)size)
        {
            memcpy(buffer, io.buffer, io.size);
            func_callback(buffer, io.size, ptr);
        }
        else
        {
            func_callback(buffer, 0, ptr);
        }
        PX_FreeIOData(&io);
    }
}
