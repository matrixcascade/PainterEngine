#include "./PainterEngine/PainterEngineHelper.h"

PX_Object *PushButton,*Edit;

//事件处理函数,注意这个函数的声明定义必须是这种格式
px_void PX_OnButtonDown(PX_Object *pObject,PX_Object_Event e,px_void *ptr)
{
	//我们弹窗显示这个输入的文本
	MessageBoxA(0,PX_Object_EditGetText(Edit),"",MB_OK);
}

int main()
{
	PX_Initialize("PainterEngine UI控件",800,600);
	//现在给大家带来的时PainterEngine的UI控件功能,PainterEngine有多个可用控件,你可以在PX_Object.h中找到他们
	//这节课我们主要讲解最具有代表性的两个控件,输入框和按钮

	//先讲解下按钮的用法,使用按钮前,我们需要先定义一个PX_Object对象
	//创建按钮
	PushButton=PX_Object_PushButtonCreate(PX_GetUiRoot()->mp,PX_GetUiRoot(),300,300,96,24,"确定",PX_COLOR(255,0,0,255));
	//我们可以尝试改变按钮的样式
	PX_Object_GetPushButton(PushButton)->style=PX_OBJECT_PUSHBUTTON_STYLE_ROUNDRECT;

	//当然,颜色之类的也没问题
	PX_Object_GetPushButton(PushButton)->BorderColor=PX_COLOR(255,255,0,0);
	PX_Object_GetPushButton(PushButton)->CursorColor=PX_COLOR(255,255,128,128);

	//我们需要添加按钮的处理事件,对于PainterEngine事件处理机制,请翻阅帮助文档
	//大部分情况下,我们处理按钮的click事件

	PX_ObjectRegisterEvent(PushButton,PX_OBJECT_EVENT_CURSORCLICK,PX_OnButtonDown,PX_NULL);

	//试试加上个编辑框
	Edit=PX_Object_EditCreate(PX_GetUiRoot()->mp,PX_GetUiRoot(),300,200,200,24,PX_COLOR(255,0,0,255));


	while(PX_Loop())
	{
	}
}