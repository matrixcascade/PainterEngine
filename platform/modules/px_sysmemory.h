#ifndef  PX_SYSMEMORY_H
#define PX_SYSMEMORY_H
#include "stdlib.h"
#include "string.h"
#include "stdio.h"
#include "../../core/PX_Core.h"
#ifdef _WIN32
#include <Windows.h>
typedef struct
{
    HANDLE ffd;
    HANDLE mfd;
    px_void* data[64];
}px_mmap_handle;
#else
#include <stddef.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
typedef struct
{
    int ffd;
    px_void* data[64];
    px_uint size[64];
}px_mmap_handle;
#endif

px_bool PX_mmap_open(const px_char filename[], px_mmap_handle* phandle);
px_void PX_mmap_close(px_mmap_handle* phandle);
px_void* PX_mmap(px_mmap_handle* handle, px_void* phy_address, px_dword size);
#endif // ! PX_SYSMEMORY_H
