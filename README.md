<div align=center><img src ="https://raw.githubusercontent.com/matrixcascade/PainterEngine/master/logo/foxlogo.png"/></div>
<h1 align="center">PainterEngine</h1>
&emsp;&emsp;PainterEngine 是一个高度可移植完整开源的图像渲染器,它内部集成了内存管理(包含内存分配,回收,调试器),多个信号/图形图像/数据结构处理算法,拥有一套完善的StoryScript脚本编译系统及StoryVM虚拟机运行系统,集成了粒子系统,2DX动画,基础字模库及多个UI控件及交互式信号处理方案,PainterEngine完全独立于C语言标准库,它可以被移植到任意有C语言编译器的平台中,你可以很容易地将PainterEngine用于游戏及图形及脚本逻辑交互的开发中.
<br/>&emsp;&emsp;PainterEngine已经被使用在多个Windows/Linux/Android/iOS/嵌入式项目中,PainterEngine最终将渲染一个RGBA(32bit)的图形缓存,您可以使用OpenGL/ES,DirectX,Direct2D,GDI,Qt....等任意方式将图形缓存显示出来.
<br/><br/><br/>
===========================================================================<br/>
移植说明:<br/>
1-PainterEngine核心文件包含PainterEngine\Architecture,PainterEngine\Core,PainterEngine\Kernel三个文件夹内容
可以无需任何修改进行移植.<br/>
2-PainterEngine\Startup为引导代码,包括实现平台上窗口创建,提供读取IO(键盘输入/触摸操控/鼠标事件),将PainterEngine渲染完成的图像buffer显示等代码
需要依据对应平台进行修改.<br/>
3-PainterEngine\PainterEngineHelper为辅助实现代码,底层基于PainterEngine核心及引导代码,提供文件系统文件读写操作(PainterEngine由特定资源管理器管理资源并无文件系统设计),在裸片开发的嵌入式环境中不建议移植,其它提供文件系统的平台可无需修改进行移植.<br/>
4-默认Startup引导代码为Windows平台Direct2D实现<br/>
5-其它平台(Android ubuntu iOS) 引导代码将在近期陆续更新,在PainterEngine\platform中可以找到需要移植直接替换就可以了.<br/>
===========================================================================
<br/><br/><br/>

*教程地址:<br/>
https://www.bilibili.com/video/av55520561<br/>
https://www.bilibili.com/video/av55522040<br/>
https://www.bilibili.com/video/av55525935<br/>
https://www.bilibili.com/video/av55527434<br/>
https://www.bilibili.com/video/av55528321<br/>
https://www.bilibili.com/video/av55531233<br/>
https://www.bilibili.com/video/av55598615<br/>
https://www.bilibili.com/video/av55637752/<br/>
<br/><br/>

**交流QQ群419410284<br/><br/>

后续DEMO将持续更新,PainterEngine也在持续完善中.
