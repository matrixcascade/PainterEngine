# The Book Of PainterEngine 

## 导言 

欢迎开始我们的PainterEngine的第一课,但在此之前相信大家并不清楚PainterEngine到底是一个做什么的
,也许您已经在互联网上对它有些许印象,它或许是一个图形库,或者是游戏引擎,你可能已经看过了基于它的
关于声学、密码学、神经网络、数字信号处理、编译器、虚拟机等等相关内容，甚至还有一个基于FPGA的GPU
 IP核。所以你可能认为它是一个集合了各式各样程式库的大混合体。

以上都对，但总的来说，我更倾向于将PainterEngine认为是一个应用程序框架，它诞生之初的最终原因，
是为了解决程序开发过程中极其麻烦的三方库（甚至是标准库）依赖问题，最大程度简化程序的平台移植和
编译难度。

因此如你所见，PainterEngine的编译不会让你陷入各式各样的三方依赖中，目前它可以在几乎所有提供C
语言编译环境的平台上运行，它没有操作系统及文件系统依赖，可以运行在裸MCU环境中，甚至
PainterEngine的官网首页也是由PainterEngine开发的。

PainterEngine始终遵循着最简设计原则，而PainterEngine使用了C语言作为其主要开发语言，而其
内置的脚本引擎，同样最大程度的兼容C语言语法，并对C语言的类型做了少量的抽象及泛化，进一步减少它
的上手及使用门槛。C语言作为一门历史悠久的语言，如今几乎是各大工科类专业必学的一门课程，其在计算
机编程开发的发展中，始终保持着强大的竞争力及广泛认可，并成为了几乎所有硬件平台，所需要并提供支持
的事实标准。C语言在学习与开发成本维持着一个微秒的平衡，因此你可以在很短的时间学习并上手C语言，
配合PainterEngine，你就能将你的程序运行在全平台，并深刻感受编程艺术所带来的魅力。

PainterEngine同样经历了近乎十年的发展，但在很长的一段时间，其作为一个私用库较少在公开领域正式
推广，一个是在其迭代的过程中，很多的接口和函数仍然未稳定，我们必须在长期的实践中，保证接口设计
的合理性和易用性，区分哪些是“真正拥有且好用”的，哪些只是“灵光一闪看上去光鲜亮丽，其实没啥用的”
因此PainterEngine在很长一段时间，都没有详细且稳定的文档，而经过那么多年的迭代，我们最终可以将
那些稳定、好用、简单易学的设计公布出来，并最终给大家带来这篇文档。

最后，我并不希望将导言写的太长，是时候马上切入主题了，我们将从PainterEngine的环境搭建开始，
开始PainterEngine的第一课，如果你有相关问题或发现了bug，你可以在PainterEngine论坛中提问，
或者直接将问题发送到matrixcascade@gmail.com,我将在第一时间给你反馈。

![](assets/mini/1.png)

## 一个最简单的PainterEngine程序

在搭建开发环境之前，让我们先编写一个最简单的PainterEngine程序，让我们新建一个".c"文件,然后在
其中输入以下代码

```c
#include "PainterEngine.h"
int main()
{
    PainterEngine_Initialize(800,480);
    return 1;
}
```

这是一个相当简单的PainterEngine程序,简单来说,在第一行我们使用include将PainterEngine的头文件
包含了进来,在main函数中,我们使用`PainterEngine_Initialize`对PainterEngine进行初始化操作,
`PainterEngine_Initialize`接受2个参数,分别是窗口(或者是屏幕)的宽度和高度,在程序运行后,你大概能
看到这样的结果。
![](assets/img/1.1.png)

当然,现在我们还没有使用PainterEngine在窗口上绘制任何东西,因此你看到的是一个空白的画面,需要注意的是,在PainterEngine的框架中,main函数返回后,程序并不会立即结束,实际上在`PainterEngine.h`中,`main`函数被替换成了`px_main`,真正的`main`函数,在`px_main.c`中实现,但用户目前可以用不着关心这个,只需要记住`main`函数返回后,程序仍然是在正常运行的,如果你希望退出程序,你可以自行调用C语言的`exit`函数,但在PainterEngine中的很多时候你并不需要这么做,因为像在嵌入式单片机、网页、驱动程序中，大部分是用不到退出这个概念的，哪怕是在Android，iOS平台，绝大部分时候也不需要你在程序中设计退出功能。

## 编译PainterEngine程序

### 使用PainterEngine Make编译

如果你想要编译PainterEngine的项目文件，最简单的做法是使用PainterEngine Make，这是一款你能够在PainterEngine.com中下载到的一款编译工具,
