#include "./PainterEngine/PainterEngineHelper.h"

int main()
{
	px_texture bmp;
	PX_Initialize("PainterEngineÍ¼Ïñ´¦Àí",800,600);
	PX_LoadTextureFromFile(PX_GetMP(),&bmp,"D:\\sample.traw");
	PX_TextureRender(PX_GetSurface(),&bmp,400,300,PX_TEXTURERENDER_REFPOINT_CENTER,PX_NULL);

	while(PX_Loop())
	{
	}
}