#include "PainterEngine_Application.h"
PX_Application App;


px_void PX_ApplicationInitializeData(PX_Application* pApp)
{
	px_int i;
	px_double x=0;
	px_double max=0;
	px_double d = 1.0 / PX_COUNTOF(pApp->x1) * 2 * PX_PI;

	for (i=0;i< PX_COUNTOF(pApp->x1);i++)
	{
		pApp->x1[i] = i*1.0 / PX_COUNTOF(pApp->x1);

		pApp->y1[i] = i/250+1;
	}

	for (i = 0; i < PX_COUNTOF(pApp->x1); i++)
	{
		pApp->x2[i] = (i * 1.0 / PX_COUNTOF(pApp->x1));
		pApp->x3[i] = (i * 1.0 / PX_COUNTOF(pApp->x1));
	}


	for (i = 0; i < PX_COUNTOF(pApp->x1); i++)
	{
		pApp->y2[i] = 1;
	}


	for (i=0;i< PX_COUNTOF(pApp->x3)-200;i++)
	{
		px_int j;
		pApp->y3[i] = 0;
		for (j=0;j<50;j++)
		{
			pApp->y3[i] += pApp->y1[i + j] * pApp->y2[j];
		}
		if (PX_ABS(pApp->y3[i])>max)
		{
			max = PX_ABS(pApp->y3[i]);
		}
	}

	for (i = 0; i < PX_COUNTOF(pApp->x3) -200; i++)
	{
		pApp->y3[i] /= max;
	}
}

px_bool PX_ApplicationInitialize(PX_Application *pApp,px_int screen_width,px_int screen_height)
{
	PX_ApplicationInitializeDefault(&pApp->runtime, screen_width, screen_height);
	return PX_TRUE;
}

px_void PX_ApplicationUpdate(PX_Application *pApp,px_dword elapsed)
{
	
}

px_void PX_ApplicationRender(PX_Application *pApp,px_dword elapsed)
{
	px_surface *pRenderSurface=&pApp->runtime.RenderSurface;
	PX_RuntimeRenderClear(&pApp->runtime,PX_OBJECT_UI_DEFAULT_BACKGROUNDCOLOR);

	PX_ObjectRender(pRenderSurface, pApp->root, elapsed);
}

px_void PX_ApplicationPostEvent(PX_Application *pApp,PX_Object_Event e)
{
	PX_ApplicationEventDefault(&pApp->runtime, e);
	PX_ObjectPostEvent(pApp->root, e);
}

