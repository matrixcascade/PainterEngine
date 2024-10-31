#include "PainterEngine.h"

PX_Object* Previous, * Next, * Image;
px_texture my_texture[10];//存放图片的数组
px_int index = 0;//当前图片的索引

PX_OBJECT_EVENT_FUNCTION(OnButtonPreClick)
{
    index--;
	if(index < 0)
	{
		index = 9;
	}
	PX_Object_ImageSetTexture(Image, &my_texture[index]);//设置图片
}

PX_OBJECT_EVENT_FUNCTION(OnButtonNextClick)
{
	index++;
	if(index > 9)
	{
		index = 0;
	}
	PX_Object_ImageSetTexture(Image, &my_texture[index]);
}

int main()
{
    px_int i;
    PainterEngine_Initialize(512, 560);//初始化
    for(i=0;i<10;i++)
	{
        px_char path[256];
        PX_sprintf1(path,256, "assets/%1.png", PX_STRINGFORMAT_INT(i+1));
		if(!PX_LoadTextureFromFile(mp_static, &my_texture[i],path))//加载图片
		{
            //加载失败
            printf("加载失败");
			return 0;
		}
	}
    PainterEngine_LoadFontModule("assets/font.ttf", PX_FONTMODULE_CODEPAGE_GBK, 20);//加载字体
    Image = PX_Object_ImageCreate(mp, root, 0, 0, 512, 512, my_texture);//创建图片对象,默认显示第一张图片
    Previous= PX_Object_PushButtonCreate(mp, root, 0, 512, 256, 48, "上一张",PainterEngine_GetFontModule());//创建按钮对象
    Next = PX_Object_PushButtonCreate(mp, root, 256, 512, 256, 48, "下一张", PainterEngine_GetFontModule());//创建按钮对象
	PX_ObjectRegisterEvent(Previous, PX_OBJECT_EVENT_EXECUTE, OnButtonPreClick, PX_NULL);//注册按钮事件
	PX_ObjectRegisterEvent(Next, PX_OBJECT_EVENT_EXECUTE, OnButtonNextClick, PX_NULL);//注册按钮事件
    return 1;
}