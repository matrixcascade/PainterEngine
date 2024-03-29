#include "PainterEngine_Application.h"

int main()
{
	if (!PX_ApplicationInitialize(&App, width, height))
	{
		return 0;
	}
	while (1)
	{
		PX_ApplicationUpdate(&App, 1);
		PX_ApplicationRender(&App, 1);
	}
}