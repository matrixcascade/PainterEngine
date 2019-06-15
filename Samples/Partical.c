//现在给大家讲解PainterEngine粒子系统的编写
//其中涉及到脚本编译和图片加载,如果你不熟悉这部分的内容,请跳转到PainterEngine系列教程的第一 第三 和 第七课
//需要创建一个粒子系统,我们需要为这个粒子系统准备一个纹理,一个用于粒子生成和计算的脚本文件

//纹理我已经准备好了一个星形的纹理,你可以将这个纹理替换成你自己需要的纹理,关于traw格式的生成如果你不熟悉可以参阅第三课
//现在我们需要准备脚本,粒子系统的脚本有一个模板,你可以在Kernel目录下找到他
//现在编译这个脚本
//这个编译如果不会请参考第七课

//现在把粒子系统代码写出来

#include "./PainterEngine/PainterEngineHelper.h"

px_texture star;
PX_ScriptVM_Instance shell;
int main()
{
	PX_Initialize("粒子系统",800,600);
	PX_SetBackgroundColor(PX_COLOR(255,0,0,0));//把背景换成黑色
	PX_LoadTextureFromFile(PX_GetMP(),&star,"star.traw");
	PX_LoadScriptInstanceFromFile(PX_GetMP(),&shell,"ParticalScript.bin");
	PX_Object_ParticalCreate(PX_GetUiRoot(),400,300,&star,&shell,"Partical_Init","Partical_Create",PX_NULL);//创建粒子发生器,绑定纹理和脚本Partical_Init是脚本里粒子发生器初始化函数
	//Partical_Create是粒子发生器创建函数,最后一个
	//是更新函数,因为不需要所以写PX_NULL
	while(PX_Loop());
}

