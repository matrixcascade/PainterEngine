#include "PainterEngine/PainterEngineHelper.h"

int main()
{
	px_int i=0;
	if (!PX_Initialize("My First PainterEngine Application",800,800))
	{
		return 0;
	}
	while(PX_Loop()){}
}