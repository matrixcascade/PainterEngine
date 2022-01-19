#include "PainterEngine_Application.h"
#include "./architecture/PainterEngine_Designer.h"
PX_Application App;

PX_FontModule fm;
PX_Object *root,*ui_root,*designer;
PX_Object *edit_weight, *edit_stature, *label_result, *pushbutton_execute;

PX_Object* soft_keyboard;
px_bool is_need_show_keyboard;

px_void PX_ApplicationExecute(PX_Object *pObject, PX_Object_Event e, px_void *ptr)
{
	px_char content[64];
	px_float w = PX_atof(PX_Object_EditGetText(edit_weight));//取得体重
	px_float s = PX_atof(PX_Object_EditGetText(edit_stature));//取得身高
	if (s>0)
	{
		px_float bmi = w / (s / 100.f) / (s / 100.f);//计算BMI
		PX_sprintf1(content, sizeof(content), "BMI result: %1", PX_STRINGFORMAT_FLOAT(bmi));//构造字符串
		PX_Object_LabelSetText(label_result, content);//设置label文本
	}
}

//软键盘按下处理
px_void keyboard_down(PX_Application *pApp,PX_Object_Event e, px_void *ptr)
{
	px_char c=PX_Object_VirtualKeyBoardGetCode(soft_keyboard);	//取得键盘返回值
	PX_Object_LabelSetText(label_result, c);//设置label文本
}
//显示软键盘
px_void show_keyboard(PX_Application *pApp,PX_Object_Event e, px_void *ptr)
{
	PX_ObjectSetVisible(soft_keyboard,PX_TRUE);
	printf("...........show_keyboard, Height:%d  Width:%d",soft_keyboard->Height,soft_keyboard->Width);
	printf("...........窗口尺寸, pApp->runtime.window_height:%d  pApp->runtime.window_width:%d",pApp->runtime.window_height,pApp->runtime.window_width);
}
//隐藏软键盘
px_void hide_keyboard(PX_Application *pApp,PX_Object_Event e, px_void *ptr)
{
	PX_ObjectSetVisible(soft_keyboard,PX_FALSE);
	printf("...........show_keyboard, Height:%d  Width:%d",soft_keyboard->Height,soft_keyboard->Width);
	printf("...........窗口尺寸, pApp->runtime.window_height:%d  pApp->runtime.window_width:%d",pApp->runtime.window_height,pApp->runtime.window_width);
}

px_bool PX_ApplicationInitialize(PX_Application *pApp,px_int screen_width,px_int screen_height)
{
	PX_IO_Data data;
	PX_ApplicationInitializeDefault(&pApp->runtime, screen_width, screen_height);//默认显示函数
	PX_FontModuleInitialize(&pApp->runtime.mp_resources,&fm);//初始化字模
	PX_LoadFontModuleFromFile(&fm,"assets/utf8_24.pxf");//加载中文字模
	root=PX_ObjectCreate(&pApp->runtime.mp_ui,PX_NULL,0,0,0,0,0,0);//创建根对象
	ui_root=PX_ObjectCreate(&pApp->runtime.mp_ui,root,0,0,0,0,0,0);//创建ui根对象
	designer=PX_DesignerCreate(&pApp->runtime.mp_game,&pApp->runtime.mp_ui,&pApp->runtime.mp_game,root,ui_root,PX_NULL,&fm);//创建设计器
	data = PX_LoadFileToIOData("assets/calc_ui.json");//加载ui布局文件
	PX_DesignerImport(designer, data.buffer);//设计器导入ui布局文件并部署到ui
	PX_FreeIOData(&data);//释放加载数据

	printf("...........PX_ApplicationInitialize, screen_height:%d  screen_width:%d",screen_height,screen_width);

	edit_weight = PX_ObjectGetObject(ui_root, ".edit_weight");//取得体重编辑框的对象指针
	edit_stature = PX_ObjectGetObject(ui_root, ".edit_stature");//取得身高编辑框的对象指针
	label_result = PX_ObjectGetObject(ui_root, ".label_result");//取得结果label的对象指针
	pushbutton_execute = PX_ObjectGetObject(ui_root, ".pushbutton_execute");//取得pushbutton的对象指针
	PX_ObjectRegisterEvent(pushbutton_execute, PX_OBJECT_EVENT_EXECUTE, PX_ApplicationExecute, PX_NULL);//绑定按钮事件

	//创建全键虚拟键盘
	soft_keyboard=PX_Object_VirtualKeyBoardCreate(&pApp->runtime.mp_ui, PX_NULL,0,pApp->runtime.surface_height*3/5,pApp->runtime.surface_width,pApp->runtime.surface_height*2/5);
	PX_Object_VirtualKeyBoardSetBackgroundColor( soft_keyboard,PX_COLOR(255,125,125,125) );//设置键盘背景色

	PX_Object_VirtualKeyBoardSetLinkerObject(soft_keyboard,root);
	//soft_keyboard->Visible=PX_TRUE;
	PX_ObjectSetVisible(soft_keyboard,PX_TRUE);
	//PX_ObjectRegisterEvent(soft_keyboard, PX_OBJECT_EVENT_KEYDOWN, keyboard_down, PX_NULL);//绑定按钮事件

	PX_ObjectRegisterEvent(edit_weight, PX_OBJECT_EVENT_ONFOCUS, show_keyboard, edit_weight);//获取焦点显示键盘
	PX_ObjectRegisterEvent(edit_weight, PX_OBJECT_EVENT_LOSTFOCUS, hide_keyboard, edit_weight);//失去焦点隐藏键盘

	PX_ObjectRegisterEvent(edit_stature, PX_OBJECT_EVENT_ONFOCUS, show_keyboard, edit_stature);//获取焦点显示键盘
	PX_ObjectRegisterEvent(edit_stature, PX_OBJECT_EVENT_LOSTFOCUS, hide_keyboard, edit_stature);//失去焦点隐藏键盘

	return PX_TRUE;
}

px_void PX_ApplicationUpdate(PX_Application *pApp,px_dword elpased)
{
	PX_ObjectUpdate(root,elpased);
	soft_keyboard->x=0;
	soft_keyboard->y=pApp->runtime.surface_height*3/5;
	soft_keyboard->Width=pApp->runtime.surface_width;
	soft_keyboard->Height=pApp->runtime.surface_height*2/5;

}

px_void PX_ApplicationRender(PX_Application *pApp,px_dword elpased)
{
	px_surface *pRenderSurface=&pApp->runtime.RenderSurface;
	PX_RuntimeRenderClear(&pApp->runtime,PX_OBJECT_UI_DEFAULT_BACKGROUNDCOLOR);
	PX_ObjectRender(pRenderSurface,root,elpased);
	PX_ObjectRender(pRenderSurface,soft_keyboard,elpased);
}

px_void PX_ApplicationPostEvent(PX_Application *pApp,PX_Object_Event e)
{
	PX_ApplicationEventDefault(&pApp->runtime, e);
	//PX_ObjectPostEvent(root,e);
	//键盘事件单独处理
	if (soft_keyboard->Visible&& PX_ObjectIsCursorInRegion(soft_keyboard,e))
	{
		PX_ObjectPostEvent(soft_keyboard, e);
	}
	else
	{
		PX_ObjectPostEvent(root, e);
	}
}
