#include "./PainterEngine/PainterEngineHelper.h"

px_bool VM_DrawLine(PX_ScriptVM_Instance *ins)//host 函数必须是这种格式
{
	PX_GeoDrawLine(PX_GetSurface(),\
		PX_ScriptVM_HOSTPARAM(ins,0)._int,\
		PX_ScriptVM_HOSTPARAM(ins,1)._int,\
		PX_ScriptVM_HOSTPARAM(ins,2)._int,\
		PX_ScriptVM_HOSTPARAM(ins,3)._int,\
		2,\
		PX_COLOR(255,0,0,255)
		);
	return PX_TRUE;
}


int main()
{
	//现在我们来讲解PainterEngine中StoryScript的使用
	//StoryScript是PainterEngine中集成的一个脚本语言,在PainterEngine中集成了StoryScript的编译器和执行虚拟机环境
	//通过编写脚本,来控制程序的逻辑进程,这个脚本控制逻辑的方案在游戏引擎中的使用已经非常普遍了,今天我们使用脚本来绘制一个三角形

	//先编写一个脚本
	//在tools中,提供了对StoryScript的编译工具
	//Script.bin是编译后的文件,现在我们来编写内部处理函数
	PX_ScriptVM_Instance Ins;//虚拟机实例化
	px_memory shell;//用来加载脚本
	if (!PX_Initialize("StoryScript示范",800,600))//初始化PainterEngine
	{
		return 0;
	}
	PX_MemoryInit(PX_GetMP(),&shell);
	PX_LoadScriptFromFile(&shell,"Script.bin");//加载编译shell文件
	PX_ScriptVM_InstanceInit(&Ins,PX_GetMP(),shell.buffer,shell.usedsize);//初始化虚拟机,建立脚本运行环境
	PX_ScriptVM_RegistryHostFunction(&Ins,"DRAWLINE",VM_DrawLine);//注册Host函数
	PX_ScriptVM_InstanceRunFunction(&Ins,0,PX_NULL,"DrawTriangle",0);//调用脚本中的DrawTriangle函数,当然,StoryScript是大小写无关的脚本,写成DRAWTRIANGLE也可以

	while(PX_Loop());
}