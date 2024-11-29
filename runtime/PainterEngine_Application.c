#include "PainterEngine_Application.h"

extern int px_main();
//////////////////////////////////////////////////////////////////////////
PX_Application App;
PX_Object* root;

px_memorypool* mp,*mp_static;
px_int screen_width;
px_int screen_height;
px_int surface_width;
px_int surface_height;
px_surface* render_surface;
PX_ResourceLibrary *resource_library;
PX_SoundPlay* soundplay;
//////////////////////////////////////////////////////////////////////////


px_char painterengine_loadbuffer[1024 * 1024 * 8];
//打印文本到打印机对象
//content: 要打印的文本内容
px_void PainterEngine_Print(const px_char content[])
{
	//如果打印机对象不可见,则设置为可见
	if (App.object_printer->Visible==PX_FALSE)
	{
		PX_ObjectSetVisible(App.object_printer, PX_TRUE);
	}
	//调用打印机对象的打印文本函数
	PX_Object_PrinterPrintText(App.object_printer, content);
}

//清空打印机对象的内容
px_void PainterEngine_PrintClear()
{
	PX_Object_PrinterClear(App.object_printer);
}

//设置打印机对象的背景颜色
//clr: 要设置的背景颜色
px_void PainterEngine_PrintSetBackgroundColor(px_color clr)
{
	PX_Object_PrinterSetBackgroundColor(App.object_printer, clr);
}

//设置打印机对象的字体颜色
//clr: 要设置的字体颜色
px_void PainterEngine_PrintSetFontColor(px_color clr)
{
	PX_Object_PrinterSetFontColor(App.object_printer, clr);
}

//在打印机对象中打印图片
//path: 图片文件路径
px_void PainterEngine_PrintImage(const px_char path[])
{
	PX_Object* pObject;
	if (App.object_printer->Visible == PX_FALSE)
	{
		PX_ObjectSetVisible(App.object_printer, PX_TRUE);
	}
	pObject=PX_Object_PrinterPrintImage(App.object_printer, PX_NULL);
	if (pObject)
	{
		PX_IO_Data data = PX_LoadFileToIOData(path);
		if (PX_strstr2(path, ".gif"))
		{
			PX_Object_ImageLoadGif(pObject, data.buffer, data.size);
		}
		else
		{
			PX_Object_ImageLoadTexture(pObject, data.buffer, data.size);
		}
		PX_Object_ImageAutoSize(pObject);
		PX_Object_PrinterUpdateAll(App.object_printer);
		PX_FreeIOData(&data);
	}
}

//在打印机对象中绘制像素点
//x,y: 像素点坐标
//color: 像素点颜色
px_void PainterEngine_DrawPixel( px_int x, px_int y, px_color color)
{
	if (App.object_printer->Visible == PX_FALSE)
	{
		PX_ObjectSetVisible(App.object_printer, PX_TRUE);
	}
	PX_SurfaceDrawPixel(PX_Object_PanelGetSurface(App.object_printer),  x, y, color);
}

//在打印机对象中绘制文本
//x,y: 文本起始坐标
//text: 要绘制的文本内容
//align: 对齐方式
//color: 文本颜色
px_void PainterEngine_DrawText(px_int x, px_int y, const px_char text[],PX_ALIGN align, px_color color)
{
	if (App.object_printer->Visible == PX_FALSE)
	{
		PX_ObjectSetVisible(App.object_printer, PX_TRUE);
	}
	PX_FontModuleDrawText(PX_Object_PanelGetSurface(App.object_printer), App.pfontmodule, x, y, align, text, color);
	
}

//在打印机对象中绘制纹理
//ptexture: 要绘制的纹理
//x,y: 纹理起始坐标
//align: 对齐方式
px_void PainterEngine_DrawTexture(px_texture *ptexture,px_int x,px_int y,PX_ALIGN align)
{
	if (App.object_printer->Visible == PX_FALSE)
	{
		PX_ObjectSetVisible(App.object_printer, PX_TRUE);
	}
	PX_TextureRender(PX_Object_PanelGetSurface(App.object_printer), ptexture, x, y, align, 0);
}

//在打印机对象中绘制线段
//x1,y1: 起点坐标
//x2,y2: 终点坐标
//linewidth: 线段宽度
//color: 线段颜色
px_void PainterEngine_DrawLine(px_int x1,px_int y1,px_int x2,px_int y2,px_int linewidth,px_color color)
{
	if (App.object_printer->Visible == PX_FALSE)
	{
		PX_ObjectSetVisible(App.object_printer, PX_TRUE);
	}
	PX_GeoDrawLine(PX_Object_PanelGetSurface(App.object_printer), x1, y1, x2, y2, linewidth, color);
}

//在打印机对象中绘制矩形
//x,y: 矩形左上角坐标
//width,height: 矩形宽高
//color: 矩形颜色
px_void PainterEngine_DrawRect(px_int x,px_int y,px_int width,px_int height,px_color color)
{
	if (App.object_printer->Visible == PX_FALSE)
	{
		PX_ObjectSetVisible(App.object_printer, PX_TRUE);
	}
	PX_GeoDrawRect(PX_Object_PanelGetSurface(App.object_printer), x, y, x+width-1, y+height-1, color);
}

//在打印机对象中绘制圆形
//x,y: 圆心坐标
//radius: 半径
//linewidth: 线宽
//color: 圆形颜色
px_void PainterEngine_DrawCircle(px_int x,px_int y,px_int radius,px_int linewidth,px_color color)
{
	if (App.object_printer->Visible == PX_FALSE)
	{
		PX_ObjectSetVisible(App.object_printer, PX_TRUE);
	}
	PX_GeoDrawCircle(PX_Object_PanelGetSurface(App.object_printer), x, y, radius, linewidth, color);
}

//在打印机对象中绘制实心圆
//x,y: 圆心坐标
//radius: 半径
//color: 圆形颜色
px_void PainterEngine_DrawSolidCircle(px_int x, px_int y, px_int radius,  px_color color)
{
	if (App.object_printer->Visible == PX_FALSE)
	{
		PX_ObjectSetVisible(App.object_printer, PX_TRUE);
	}
	PX_GeoDrawSolidCircle(PX_Object_PanelGetSurface(App.object_printer), x, y, radius, color);
}

//在打印机对象中绘制扇形
//x,y: 圆心坐标
//radius_inside: 内圆半径
//radius_outside: 外圆半径
//start_angle: 起始角度
//end_angle: 结束角度
//color: 扇形颜色
px_void PainterEngine_DrawSector(px_int x, px_int y, px_int radius_inside, px_int radius_outside, px_int start_angle, px_int end_angle, px_color color)
{
	if (App.object_printer->Visible == PX_FALSE)
	{
		PX_ObjectSetVisible(App.object_printer, PX_TRUE);
	}
	PX_GeoDrawSector(PX_Object_PanelGetSurface(App.object_printer), x, y, (px_float)radius_inside, (px_float)radius_outside, color, start_angle, end_angle);
}



//设置字体大小
//size: 要设置的字体大小
px_void PainterEngine_SetFontSize(px_int size)
{
if(App.pfontmodule)
		PX_FontModuleSetSize(App.pfontmodule, size);
}

//在打印机对象中打印按钮
//width,height: 按钮宽高
//text: 按钮文本
//返回值: 创建的按钮对象指针
PX_Object* PainterEngine_PrintButton(px_int width,px_int height,const px_char text[])
{
	PX_Object* pObject;
	if (App.object_printer->Visible == PX_FALSE)
	{
		PX_ObjectSetVisible(App.object_printer, PX_TRUE);
	}

	pObject = PX_Object_PrinterPrintButton(App.object_printer, width, height, text);
	return pObject;
}

//在打印机对象中打印进度条
//返回值: 创建的进度条对象指针
PX_Object* PainterEngine_PrintProcessBar()
{
	PX_Object* pObject;
	if (App.object_printer->Visible == PX_FALSE)
	{
		PX_ObjectSetVisible(App.object_printer, PX_TRUE);
	}

	pObject = PX_Object_PrinterPrintProcessBar(App.object_printer);
	return pObject;
}

//在打印机对象中打印空白区域
//height: 空白区域高度
//返回值: 创建的空白区域对象指针
PX_Object* PainterEngine_PrintSpace(px_int height)
{
	PX_Object* pObject;
	if (App.object_printer->Visible == PX_FALSE)
	{
		PX_ObjectSetVisible(App.object_printer, PX_TRUE);
	}

	pObject = PX_Object_PrinterPrintSpace(App.object_printer,height);
	return pObject;
}

//在打印机对象中打印一个对象
//pNewObject: 要打印的对象指针
px_void  PainterEngine_PrintObject(PX_Object* pNewObject)
{
	if (App.object_printer->Visible == PX_FALSE)
	{
		PX_ObjectSetVisible(App.object_printer, PX_TRUE);
	}
	PX_Object_PrinterPrintObject(App.object_printer, pNewObject);
}

//获取打印机对象的打印区域
//返回值: 打印区域对象指针
PX_Object*  PainterEngine_PrintGetArea()
{
	return PX_Object_PrinterGetArea(App.object_printer);
}

//获取语言翻译
//tr: 要翻译的文本
//返回值: 翻译后的文本
const px_char* PainterEngine_Language(const px_char tr[])
{
	if (App.language.mp)
	{
		const px_char *ptr=PX_JsonGetString(&App.language, tr);
		if (!ptr[0])
			ptr = tr;
		return ptr;
	}
	else
	{
		return tr;
	}
}

//设置字体编码页
//codepage: 要设置的编码页
px_void PainterEngine_PrintSetCodepage(PX_FONTMODULE_CODEPAGE codepage)
{
	if(App.pfontmodule)
		PX_FontModuleSetCodepage(App.pfontmodule, codepage);
}

//获取字体模块
//返回值: 字体模块指针
PX_FontModule* PainterEngine_GetFontModule()
{
	return App.pfontmodule;
}

//从TTF文件加载字体模块
//path: TTF文件路径
//codepage: 编码页
//size: 字体大小
//返回值: 是否加载成功
px_bool PainterEngine_LoadFontModule(const px_char path[],PX_FONTMODULE_CODEPAGE codepage,px_int size)
{
#ifdef PAINTERENGIN_FILE_SUPPORT
	if (!App.pfontmodule)
	{
		if (PX_LoadFontModuleFromTTF(&App.runtime.mp_static, &App.fontmodule, path, codepage, size))
		{
			App.pfontmodule = &App.fontmodule;
			return PX_TRUE;
		}
		else
		{
			return PX_FALSE;
		}
	}
	return PX_FALSE;
#else
	return PX_FALSE;
	#endif
}

//初始化绘图引擎
//_screen_width,_screen_height: 屏幕宽高
//返回值: 是否初始化成功
px_bool PainterEngine_Initialize(px_int _screen_width,px_int _screen_height)
{
	PX_Runtime* runtime = &App.runtime;
	if (!PX_RuntimeInitialize(runtime, _screen_width, _screen_height, _screen_width, _screen_height, App.cache, sizeof(App.cache), PX_APPLICATION_MEMORYPOOL_STATIC_SIZE, PX_APPLICATION_MEMORYPOOL_DYNAMIC_SIZE))
		return PX_FALSE;
#ifdef PX_AUDIO_H
	PX_SoundPlayInitialize(mp, &App.soundplay);
	PainterEngine_InitializeAudio();
#endif
	if (_screen_width == 0 || _screen_height == 0)
		return PX_TRUE;
#ifdef PAINTERENGIN_FILE_SUPPORT
	
	App.pfontmodule = PX_NULL;
	if(!PX_JsonInitialize(&runtime->mp_static, &App.language))
	{
		return PX_FALSE;
	}
	if (!PX_LoadJsonFromFile(&App.language, "assets/language.json"))
	{
		PX_JsonFree(&App.language);
		PX_memset(&App.language, 0, sizeof(App.language));
	}
#endif

	App.object_root = PX_ObjectCreateRoot(&runtime->mp_static);
	App.object_printer=PX_Object_PrinterCreate(&runtime->mp, App.object_root, 0, 0, _screen_width, _screen_height, App.pfontmodule);
	PX_Object_PanelAttachObject(App.object_printer, PX_ObjectGetFreeDescIndex(App.object_printer));
	PX_Object_PrinterSetBackgroundColor(App.object_printer, PX_COLOR_NONE);
	App.object_printer->Visible = PX_FALSE;
	

	App.object_messagebox = PX_Object_MessageBoxCreate(&runtime->mp_static, App.object_root, 0);
	root = App.object_root;
	mp = &runtime->mp_dynamic;
	mp_static=&runtime->mp_static;
	screen_width = _screen_width;
	screen_height = _screen_height;
	surface_width = App.runtime.surface_width;
	surface_height = App.runtime.surface_height;
	render_surface=&App.runtime.RenderSurface;
	resource_library =&App.runtime.ResourceLibrary;
	soundplay = &App.soundplay;

	return PX_TRUE;
}


//获取根对象
//返回值: 根对象指针
PX_Object* PainterEngine_GetRoot()
{
	return App.object_root;
}

//获取渲染表面
//返回值: 渲染表面指针
px_surface * PainterEngine_GetSurface()
{
	return &App.runtime.RenderSurface;
}

//获取运行时
//返回值: 运行时指针
PX_Runtime* PainterEngine_GetRuntime()
{
	return &App.runtime;
}

//获取资源库
//返回值: 资源库指针
PX_ResourceLibrary* PainterEngine_GetResourceLibrary()
{
	return &App.runtime.ResourceLibrary;
}

//获取动态内存池
//返回值: 动态内存池指针
px_memorypool * PainterEngine_GetDynamicMP()
{
	return &App.runtime.mp_dynamic;
}

//获取静态内存池
//返回值: 静态内存池指针
px_memorypool* PainterEngine_GetStaticMP()
{
	return &App.runtime.mp_static;
}


//初始化音频系统
//返回值: 是否初始化成功
px_bool PainterEngine_InitializeAudio()
{
#ifdef PX_AUDIO_H
	static px_bool audioInit = PX_FALSE;
	if (!audioInit)
	{
		audioInit = PX_TRUE;
		return PX_AudioInitialize(&App.soundplay);
	}
	return PX_TRUE;
#else
	return PX_FALSE;
#endif //  PX_AUDIO_H
}

//设置背景颜色
//color: 要设置的背景颜色
px_void PainterEngine_SetBackgroundColor(px_color color)
{
	App.backgroundColor = color;
}

//设置背景纹理
//pTexture: 要设置的背景纹理
px_void PainterEngine_SetBackgroundTexture(px_texture *pTexture)
{
	App.pBackgroudTexture = pTexture;
}

//获取音频播放器
//返回值: 音频播放器指针
PX_SoundPlay* PainterEngine_GetSoundPlay()
{
	return &App.soundplay;
}


//////////////////////////////////////////////////////////////////////////
//Functions


//初始化应用程序
//pApp: 应用程序指针
//screen_width,screen_height: 屏幕宽高
//返回值: 是否初始化成功
px_bool PX_ApplicationInitialize(PX_Application *pApp,px_int screen_width,px_int screen_height)
{
	PX_memset(pApp, 0, sizeof(PX_Application));
	screen_width = screen_width;
	screen_height = screen_height;
	PainterEngine_SetBackgroundColor(PX_COLOR_BACKGROUNDCOLOR);

	px_main();
	
	if (pApp->runtime.window_height&&pApp->runtime.window_width&& pApp->runtime.surface_height&& pApp->runtime.surface_width)
	{
		return PX_TRUE;
	}
	return PX_FALSE;
}

//更新应用程序
//pApp: 应用程序指针
//elapsed: 距离上次更新的时间间隔
px_void PX_ApplicationUpdate(PX_Application *pApp,px_dword elapsed)
{
	PX_ObjectUpdate(pApp->object_root, elapsed);
}

//渲染应用程序
//pApp: 应用程序指针
//elapsed: 距离上次渲染的时间间隔
px_void PX_ApplicationRender(PX_Application *pApp,px_dword elapsed)
{
	px_surface *pRenderSurface=&pApp->runtime.RenderSurface;
	PX_RuntimeRenderClear(&pApp->runtime,pApp->backgroundColor);
	if(pApp->pBackgroudTexture)
	{
		PX_TextureRender(pRenderSurface, pApp->pBackgroudTexture, 0, 0, PX_ALIGN_LEFTTOP, 0);
	}
	PX_ObjectRender(pRenderSurface, pApp->object_root,elapsed);
}

//发送事件到应用程序
//pApp: 应用程序指针
//e: 要发送的事件
px_void PX_ApplicationPostEvent(PX_Application *pApp,PX_Object_Event e)
{
	PX_ObjectPostEvent(pApp->object_root,e);
}
