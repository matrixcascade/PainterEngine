#ifndef PX_REQUEST_H
#define PX_REQUEST_H

void PX_RequestData(const char url[], void* buffer, int size, void* ptr, void (*func_callback)(void* buffer, int size, void* ptr));

#endif