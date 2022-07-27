#ifndef PAINTERENGINE_APPLICATION_H
#define PAINTERENGINE_APPLICATION_H

#ifdef __cplusplus
extern "C"{
#endif

#include "PainterEngine_Startup.h"
#define PX_APPLICATION_WAVE_LEN 1000
	typedef struct
	{
		PX_Object *root,* wav1, * wav2,*wav3,*sliderbar;
		px_double x1[PX_APPLICATION_WAVE_LEN], y1[PX_APPLICATION_WAVE_LEN];
		px_double x2[PX_APPLICATION_WAVE_LEN], y2[PX_APPLICATION_WAVE_LEN];
		px_double x3[PX_APPLICATION_WAVE_LEN], y3[PX_APPLICATION_WAVE_LEN];

		PX_Runtime runtime;
	}PX_Application;

	extern PX_Application App;

	px_bool PX_ApplicationInitialize(PX_Application* App, px_int screen_Width, px_int screen_Height);
	px_void PX_ApplicationUpdate(PX_Application* App, px_dword elapsed);
	px_void PX_ApplicationRender(PX_Application* App, px_dword elapsed);
	px_void PX_ApplicationPostEvent(PX_Application* App, PX_Object_Event e);
#ifdef __cplusplus
}
#endif

#endif
