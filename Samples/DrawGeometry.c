#include "./PainterEngine/PainterEngineHelper.h"

int main()
{
	PX_Initialize("我的第一个PainterEngine项目",800,600);
	//画线
	PX_GeoDrawLine(PX_GetSurface(),100,100,500,200,5,PX_COLOR(255,128,255,0));

	//边框
	PX_GeoDrawBorder(PX_GetSurface(),100,100,300,200,5,PX_COLOR(255,255,0,0));

	//矩形
	PX_GeoDrawRect(PX_GetSurface(),300,100,500,500,PX_COLOR(255,0,255,0));

	//实心圆
	PX_GeoDrawSolidCircle(PX_GetSurface(),300,300,100,PX_COLOR(255,0,0,255));

	//圆
	PX_GeoDrawCircle(PX_GetSurface(),200,200,100,5,PX_COLOR(255,128,128,0));

	//环
	PX_GeoDrawRing(PX_GetSurface(),400,300,100,20,PX_COLOR(255,128,0,128),90,250);

	//扇形
	PX_GeoDrawSector(PX_GetSurface(),200,350,100,0,PX_COLOR(255,255,128,0),0,125);

	//圆角矩形
	PX_GeoDrawSolidRoundRect(PX_GetSurface(),50,50,300,150,20,PX_COLOR(255,64,128,255));

	//圆角边框
	PX_GeoDrawRoundRect(PX_GetSurface(),300,50,500,150,20,10,PX_COLOR(255,64,0,64));

	while(PX_Loop());
}