//is windows
#include "../modules/px_sysmemory.h"

#ifdef _WIN32
px_bool PX_mmap_open(const px_char filename[], px_mmap_handle *phandle)
{
    PX_memset(phandle, 0, sizeof(px_mmap_handle));
    phandle->ffd = 0;
    phandle->mfd = 0;
    phandle->ffd = CreateFileA(filename, GENERIC_READ, FILE_SHARE_READ| FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	return phandle->ffd != INVALID_HANDLE_VALUE;
}

px_void *PX_mmap(px_mmap_handle *handle, px_void* phy_address, px_dword size)
{
    px_int i;
    handle->mfd = CreateFileMapping(handle->ffd, NULL, PAGE_READWRITE,
                               (DWORD) ((UINT64) size >> 32),
                               (DWORD) (size & 0xffffffff),
                               NULL);
    if (handle->mfd == 0)
    {
        return NULL;
    }
    for (i = 0; i < PX_COUNTOF(handle->data); i++)
    {
        if (handle->data[i])
        {
            continue;
        }
        handle->data[i] = MapViewOfFile(handle->mfd, FILE_MAP_READ | FILE_MAP_WRITE,
            (DWORD)(((UINT64)phy_address) >> 32),
            (DWORD)(((UINT64)phy_address) & 0xffffffff),
            size);
        if (handle->data[i] == NULL)
        {
            return PX_NULL;
        }
        return handle->data[i];
    }
    return PX_NULL;

}


px_void PX_munmap(px_mmap_handle* handle)
{
    px_int i;
    for (i = 0; i < PX_COUNTOF(handle->data); i++)
	{
		if (handle->data[i] == NULL)
		{
			continue;
		}
		if (UnmapViewOfFile(handle->data[i]) == 0)
		{
			return;
		}
		handle->data[i] = NULL;
	}
    if (CloseHandle(handle->mfd) == 0) {
        return;
    }
    handle->mfd = 0;
}
#else

px_bool PX_mmap_open(const px_char filename[], px_mmap_handle* phandle)
{
    PX_memset(phandle, 0, sizeof(px_mmap_handle));
	phandle->ffd = open(filename, O_RDWR | O_SYNC);
    return phandle->ffd != -1;
}

px_void* PX_mmap(px_mmap_handle* handle, px_void* phy_address, px_dword size)
{
    px_int i;
    for ( i = 0; i < PX_COUNTOF(handle->data); i++)
    {
        if (handle->data[i] != NULL)
        {
            continue;
        }
        handle->data[i] = mmap(PX_NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, handle->ffd, (px_dword)phy_address);
        handle->size[i] = size;
        printf("mmap %p-->%p\n", handle->data[i],phy_address);
        return handle->data[i];
    }
	return PX_NULL;
}

px_void PX_munmap(px_mmap_handle* handle)
{
    px_int i;
    for (i = 0; i < PX_COUNTOF(handle->data); i++)
    {
        if (handle->data[i] != NULL)
		{
			continue;
		}
		if (munmap(handle->data[i], handle->size[i]) == 0)
		{
			return;
		}
		handle->data[i] = NULL;
   }
   if (close(handle->ffd) == 0) 
   {
	return;
   }
   handle->ffd = 0;
}
#endif
