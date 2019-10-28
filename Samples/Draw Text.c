#include "./PainterEngine/PainterEngineHelper.h"

int main()
{
	PX_Initialize("PainterEngine 绘制文本",800,600);
	//PainterEngine绘制文本非常简单
	PX_FontDrawText(PX_GetSurface(),300,300,"这个是使用PainterEngine绘制的字体",PX_COLOR(255,0,0,0),PX_FONT_ALIGN_XLEFT);
	//最后一个参数是ARGB颜色，自己调整就好了
	while(PX_Loop())
	{
	}
}