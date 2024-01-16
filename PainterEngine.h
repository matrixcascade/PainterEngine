#ifndef PAINTERENGINE
#define PAINTERENGINE
#define main px_main

#ifdef __cplusplus 
extern "C" {
#endif 
#include "runtime/PainterEngine_Application.h"
extern PX_Object* root;
extern px_memorypool* mp,*mp_static;
extern px_int screen_width;
extern px_int screen_height;
extern px_int surface_width;
extern px_int surface_height;
extern px_surface* render_surface;
extern PX_ResourceLibrary* resource_library;
extern PX_SoundPlay* soundplay;
int px_main();
#ifdef __cplusplus 
}
#endif 



#endif