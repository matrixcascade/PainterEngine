#include "PainterEngine_Application.h"
PX_Application App;
/*
 * 初始化函数
 * pApp 是应用的描述结构
 * Screen_width 是当前屏幕的像素宽度
 * Screen_height是当前屏幕的像素高度
 */
px_bool PX_ApplicationInitialize(PX_Application *pApp,px_int screen_width,px_int screen_height)
{
	PX_ApplicationInitializeDefault(&pApp->runtime, screen_width, screen_height); //默认的PE部署初始化函数
	return PX_TRUE;
}
/*
 * PE运行环境会循环调用这个函数
 * elapsed 是上次更新后经历的时间，单位是毫秒
 */
px_void PX_ApplicationUpdate(PX_Application *pApp,px_dword elapsed)
{

}
/*
 * 渲染函数,在大部分的情况下,PE运行环境会循环调用这个函数（实时渲染）
 *PE的绘制流程是，先将surface成背景色，然后在上面绘制图像，在下一帧，再将这个surface重新刷成背景色，再画下一帧的图像
 * elapsed 是上次更新后经历的时间，单位是毫秒
 */
px_void PX_ApplicationRender(PX_Application *pApp,px_dword elapsed)
{
	px_surface *pRenderSurface=&pApp->runtime.RenderSurface;	//渲染表面
	PX_RuntimeRenderClear(&pApp->runtime,PX_OBJECT_UI_DEFAULT_BACKGROUNDCOLOR); //用某种颜色清空当前的渲染表面
}
/*
 * 用户响应用户输入事件
 * e是事件类型,类型见kernel/PX_Object.h
 */
px_void PX_ApplicationPostEvent(PX_Application *pApp,PX_Object_Event e)
{
	PX_ApplicationEventDefault(&pApp->runtime, e);

}

