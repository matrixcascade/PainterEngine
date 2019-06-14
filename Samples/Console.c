#include "./PainterEngine/PainterEngineHelper.h"

int main()
{
	PX_Initialize("PainterEngine 控制台",800,600);
	//现在演示的是PainterEngine控制台功能
	//控制台功能主要用于调试，它是PainterEngine内部集成在StoryVM上的一个虚拟机控制环境
	//控制台使用StoryScript脚本，使用PX_ShowConsole函数来调出控制台
	PX_ShowConsole();
	//有关更多的详细解释请参照说明文档
	while(PX_Loop())
	{
	}
}