#ifndef PX_VISUALOS_H
#define PX_VISUALOS_H
#include "px_socket.h"
#include "px_sysmemory.h"
#include "px_thread.h"
#include "../../kernel/PX_Kernel.h"

typedef px_void(*PX_Object_VisualOS_init)(PX_Object* pObject, px_int width, px_int height);
#define PX_VISUALOS_INIT_FUNCTION(func) px_void func(PX_Object* pObject, px_int width, px_int height)
px_bool PX_Object_VisualOSLogin(PX_Object* pObject, px_int target_x, px_int target_y, px_int width, px_int height);
PX_Object* PX_Object_VisualOSCreate(px_memorypool* mp, PX_Object* Parent, const px_char host[], px_word port, const px_byte cookie[16], px_float x, px_float y, PX_Object_VisualOS_init init);
#endif