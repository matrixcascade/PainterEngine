#ifndef PX_VISUALOS_H
#define PX_VISUALOS_H
#include "px_socket.h"
#include "px_sysmemory.h"
#include "kernel/PX_Kernel.h"
PX_Object* PX_Object_VisualOSCreate(px_memorypool* mp, PX_Object* Parent, const px_char host[], px_word port, const px_byte cookie[16],\
	px_float x, px_float y, px_float target_x, px_float target_y, px_float width, px_float height);
#endif