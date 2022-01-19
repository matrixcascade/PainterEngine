#include <architecture/PainterEngine_Designer.h>
#include "PainterEngine_Application.h"
PX_Application App;

px_void OnButtonExecute(PX_Object *pObject,PX_Object_Event e,px_void *ptr); //按钮点击事件
px_void PX_Application_DesignerExport(PX_Object *pObject,PX_Object_Event e,px_void *ptr);//在这个函数中,将当前的UI布局保存到ui.json文件

/*
 * 初始化函数
 * pApp 是应用的描述结构
 * Screen_width 是当前屏幕的像素宽度
 * Screen_height是当前屏幕的像素高度
 */
px_bool PX_ApplicationInitialize(PX_Application *pApp,px_int screen_width,px_int screen_height)
{
	PX_ApplicationInitializeDefault(&pApp->runtime, screen_width, screen_height); //默认的PE部署初始化函数

	pApp->ui_root=PX_ObjectCreate(&pApp->runtime.mp_ui,0,0,0,0,0,0,0);

	//按钮
	pApp->button=PX_Object_PushButtonCreate(&pApp->runtime.mp_ui,pApp->ui_root,100,100,84,32,"Button",PX_NULL);
	PX_ObjectRegisterEvent(pApp->button,PX_OBJECT_EVENT_EXECUTE,OnButtonExecute,pApp);//注册按钮事件
	//文本输入框
	pApp->edit=PX_Object_EditCreate(&pApp->runtime.mp_ui,pApp->ui_root,100,300,128,32,PX_NULL);
    PX_Object_EditSetFocus( pApp->edit,1);

	//加载图片纹理
	if(PX_LoadTextureFromFile(&pApp->runtime.mp_resources,&pApp->MyTexture,"assets/test.traw"))
	{
		//加载成功
	}
	else
	{
		//加载失败

	}

    //初始化字模
    PX_FontModuleInitialize(&pApp->runtime.mp_resources,&pApp->fm);
    //加载字模
    PX_LoadFontModuleFromFile(&pApp->fm,"gbk_18.pxf");
    //创建根对象
    PX_Object *root=PX_ObjectCreate(&pApp->runtime.mp_ui,PX_NULL,0,0,0,0,0,0);
    //创建设计器
    pApp->designer=PX_DesignerCreate(&pApp->runtime.mp_game,&pApp->runtime.mp_ui,&pApp->runtime.mp_game,root,pApp->ui_root,PX_NULL,&pApp->fm);
    //绑定设计器execute事件
    PX_ObjectRegisterEvent(pApp->designer,PX_OBJECT_EVENT_EXECUTE,PX_Application_DesignerExport,PX_NULL);
    //激活设计器
    PX_DesignerActivate(pApp->designer);

    //创建虚拟软键盘
	PX_Object_VirtualKeyBoardCreate(px_memorypool *mp, PX_Object *Parent,px_int x,px_int y,px_int width,px_int height);

	return PX_TRUE;
}
/*
 * PE运行环境会循环调用这个函数
 * elapsed 是上次更新后经历的时间，单位是毫秒
 */
px_void PX_ApplicationUpdate(PX_Application *pApp,px_dword elapsed)
{
	PX_ObjectUpdate(pApp->ui_root,elapsed);
}
/*
 * 渲染函数,在大部分的情况下,PE运行环境会循环调用这个函数（实时渲染）
 *PE的绘制流程是，先将surface成背景色，然后在上面绘制图像，在下一帧，再将这个surface重新刷成背景色，再画下一帧的图像
 * elapsed 是上次更新后经历的时间，单位是毫秒
 */
px_void PX_ApplicationRender(PX_Application *pApp,px_dword elapsed)
{
	px_surface *pRenderSurface=&pApp->runtime.RenderSurface;	//默认渲染表面
	PX_RuntimeRenderClear(&pApp->runtime,PX_OBJECT_UI_DEFAULT_BACKGROUNDCOLOR); //用某种颜色清空当前的渲染表面
	//pApp->runtime.surface_width; 	//来获取当前渲染表面的像素宽度
	//pApp->runtime.surface_height;	//来获取当前渲染表面的像素高度

	//绘制反走样线段
	PX_GeoDrawLine(pRenderSurface,100,100,600,100,2,PX_COLOR(255,255,0,0));
	PX_GeoDrawLine(pRenderSurface,100,200,600,200,6,PX_COLOR(255,0,255,0));
	PX_GeoDrawLine(pRenderSurface,100,300,600,300,12,PX_COLOR(255,0,0,255));
	//绘制一个边框
	PX_GeoDrawBorder(pRenderSurface,100,100,300,300,3,PX_COLOR(255,0,0,0));
	//绘制一个实心矩形
	PX_GeoDrawRect(pRenderSurface,100,100,300,300,PX_COLOR(128,255,0,0));
	//绘制一个反走样实心圆
	PX_GeoDrawSolidCircle(pRenderSurface,300,300,200,PX_COLOR(255,255,0,255));
	//绘制一个反走样圆
	PX_GeoDrawCircle(pRenderSurface,300,300,200,10,PX_COLOR(255,255,0,255));
	//绘制一个反走样环
	PX_GeoDrawRing(pRenderSurface,300,300,200,50,PX_COLOR(255,0,0,255),45,270);
	//绘制一个反走样扇形
	PX_GeoDrawSector(pRenderSurface,300,300,200,0,PX_COLOR(255,0,0,255),45,270);
	//绘制一个反走样圆角矩形
	PX_GeoDrawRoundRect(pRenderSurface,100,100,600,200,25,5,PX_COLOR(255,0,0,255));
	//绘制一个反走样实心圆角矩形
	PX_GeoDrawSolidRoundRect(pRenderSurface,100,100,600,200,10,PX_COLOR(255,0,0,255));
	//绘制ANSI文本
	PX_FontDrawText(pRenderSurface,10,100,PX_ALIGN_MIDBOTTOM,"Hello World 123",PX_COLOR(255,0,0,128));
	//创建一个按钮交互控件
	PX_ObjectRender(pRenderSurface,pApp->ui_root,elapsed);
	//渲染一个纹理到表面
    PX_TextureRender(pRenderSurface,&pApp->MyTexture,pApp->runtime.surface_width/2,pApp->runtime.window_height/2,PX_ALIGN_CENTER,PX_NULL);

}
/*
 * 用户响应用户输入事件
 * e是事件类型,类型见kernel/PX_Object.h
 */
px_void PX_ApplicationPostEvent(PX_Application *pApp,PX_Object_Event e)
{
	PX_ApplicationEventDefault(&pApp->runtime, e);

	PX_ObjectPostEvent(pApp->ui_root,e);

}

/*
 *按钮点击处理函数
 */
px_void OnButtonExecute(PX_Object *pObject,PX_Object_Event e,px_void *ptr)
{

}

//在这个函数中,将当前的UI布局保存到ui.json文件
px_void PX_Application_DesignerExport(PX_Object *pObject,PX_Object_Event e,px_void *ptr)
{
    px_string string;
    PX_StringInitialize(&App.runtime.mp_game,&string);
    PX_DesignerExport(pObject,&string);
    PX_SaveDataToFile(string.buffer,PX_strlen(string.buffer)+1,"ui.json");
    PX_StringFree(&string);

    return;

}