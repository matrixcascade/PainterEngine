<p align="center">
  <a href="https://painterengine.com" target="_blank" rel="noopener noreferrer">
    <img src="images/title.png" alt="PainterEngine Title">
  </a>
</p>

PainterEngine 是一个由 C 语言编写的跨平台图形引擎，支持 Windows/Linux/iOS/Android/WebAssembly 甚至无操作系统的裸嵌入式平台，它基于组件化的设计模式，即使是 C 语言初学者，也可以在几分钟内掌握它的使用，[PainterEngine Make](https://www.painterengine.com/) 允许您一键将您的 PainterEngine 项目编译到多个平台。
它涵盖了基础数据结构、图形学、声学、数字信号处理、编译原理、虚拟机系统、密码学、人机交互、游戏引擎、FPGA-GPU 图形 IP 设计等多个领域，你既可以用它制作微应用，也可以将它作为学习项目。

现在了解并使用 PainterEngine $\longrightarrow$
*__[PainterEngine 快速入门手册（中文版）](./documents/PainterEngine_the_book_zh-CN.md)__*

PainterEngine is a cross-platform graphics engine written in C language, with support for Windows, Linux, iOS, Android, WebAssembly, and even bare-metal embedded platforms without OS. It is built on a component-based design pattern, making it accessible to even C language beginners. [PainterEngine Make](https://www.painterengine.com/) enables you to compile your PainterEngine project for multiple platforms with just one click.
It covers various fields including basic data structures, graphics, acoustics, digital signal processing, compiler design, virtual machine systems, cryptography, human-computer interaction, game engines, FPGA-GPU graphics acceleration, and more. You can use it to create mini-applications or as a learning project for acquiring knowledge.

Now, learn and use PainterEngine $\longrightarrow$
*__[PainterEngine Quick Start Guide (English Version)](./documents/PainterEngine_the_book_en.md)__*

## 30 秒速览 PainterEngine

## 30-Second Quick Start Guide to PainterEngine

将 PainterEngine 引入到您的 C/C++ 项目中，仅仅需要 `#include "PainterEngine.h"`。

To incorporate PainterEngine into your project, all you need is:
```c
#include "PainterEngine.h"
```

使用 `PainterEngine_Initialize`，快速创建一个图形化的交互式界面：

Utilize `PainterEngine_Initialize` to swiftly create a graphical interactive interface:
```c
#include "PainterEngine.h"
int main()
{
	PainterEngine_Initialize(800, 600);
	return 1;
}
```

创建组件，或者……创造自己的组件：

Create components or even craft your own:

```c
#include "PainterEngine.h"
int main()
{
	PainterEngine_Initialize(800, 600);
	PX_Object_Firework01Create(mp, root,200,600);
	PX_Object_Firework01Create(mp, root,400,600);
	PX_Object_Firework01Create(mp, root,600,600);
	return 1;
}
```

<p align="center">
  <img src="images/firework.png" alt="PainterEngine firework">
</p>

使用 [PainterEngine Make](https://www.painterengine.com/) 快速将您的项目编译到 Windows、Linux、WebAssembly、Android 等任意平台，一键编译部署，源码无需修改，零成本移植。

Use ["PainterEngine Make"](https://www.painterengine.com/) to quickly compile and deploy your project to various platforms such as Windows, Linux, WebAssembly, Android, and more. One-click compilation and deployment, with no need to modify the source code, enabling seamless portability at zero cost.

<p align="center">
  <a href="https://cdn.painterengine.com/PainterEngine_make.zip" target="_blank" rel="noopener noreferrer">
  	<img src="images/pemake.png" alt="PainterEngine make">
  </a>
</p>

<p align="center">
  <a href="https://cdn.painterengine.com/PainterEngine_make_en.zip" target="_blank" rel="noopener noreferrer">
    <img src="images/pemakeen.png" alt="PainterEngine make">
  </a>
</p>

## 快速开发，无缝迁移

## Swift development and smooth transitions

如果您不需要 PainterEngine Make 提供的一键编译功能，希望使用自己常用的 IDE 开发 PainterEngine 程序或组件，您只需要：

1. 将 "PainterEngine/core"、"PainterEngine/kernel"、"PainterEngine/runtime" 的所有代码，添加到您的项目中。 

2. 在 "PainterEngine/platform" 中选择您的工作平台（例如 Windows 中选择 "PainterEngine/platform/windows"），并将对应文件夹中的所有代码添加到您的项目中。

3. 将 PainterEngine 所在目录添加到包含目录中。

4. 将您的代码添加进项目中。

即可使用您的 IDE 完成 PainterEngine 的编译，PainterEngine 库将尽力保证所有平台的的运行结果一致性，在 Windows 上开发，同样在 Android/web/Linux/iOS/…… 中能够得到一致的结果。

If you don't need the one-key compilation feature provided by PainterEngine Make and prefer to develop PainterEngine programs or components using your preferred IDE, you just need to:

1. Add all the code from "PainterEngine/core", "PainterEngine/kernel", and "PainterEngine/runtime" to your project.

2. Select your working platform in "PainterEngine/platform" (eg: choose "PainterEngine/platform/windows" for Windows), and add all the code from the corresponding folder to your project.

3. Include the directory containing PainterEngine source code in your project's include directories.

4. Add your code to the project.

You can now use your IDE to compile PainterEngine with these steps. PainterEngine library will strive to ensure consistent results across all platforms. What you develop on Windows will yield consistent results on Android, web, Linux, iOS, and more.

## 不仅是图形库，更是应用程序框架

## Not just a graphics library but also an application framework

| Functions             | Support                                                      | Description                                                                       |
| --------------------- | ------------------------------------------------------------ | ------------------------------------------------------------------------          |
| 内存池                 | alloc/free                                                   | 平台无关的内存池实现                              |
| 数学库               | sin/cos/tan/arcsin/log/exp/relu/...                | 绝大部分 C 标准数学库的完整实现                          |
| 信号处理               | dft/dct/fft/dwt/window functions/mfcc/...                | 傅里叶/余弦/小波变换、常用窗函数、mfcc 等特征采集算法……等等信号处理相关基础函数及上层特征采集算法                         |
| 数据结构               | string/vector/list/map/stack/fifo/circular-buffer/...                | 平台无关的数据结构算法实现                          |
| 密码学               | curve25519/AES/SHAx/MD5/...                | 包含常用的密钥对称算法及密钥协商算法                          |
| 图片支持               | PNG/JPG/GIF/BMP                                              | 支持 PNG/JPG/GIF/BMP 解码及 PNG 编码           |
| 音频支持               | WAV/MP3                                              | 支持 Wav、Mp3 解码及 Wav 编码|
| 字模支持               | ttf                                              | 支持 ttf 字模文件（由 stb_truetype.c 移植而来）|
| 几何绘制               | Line/Triangle/Rectangle/Circle/Ring/Sector/Rounded/...      | 常用几何光栅化实现                                                                 |
| 渲染器                | 2D/3D                                                        | 2D/3D 渲染器实现及一个高质量制图引擎                                                 |
| 动画                  | 2dx/live2D                                                   | 2D 动画和一个类 Live2D 骨骼动画系统                                                  |
| 声学模型               | mixer/piano/ks                                               | 包含一个混音器实现、一个相位声码器、一个物理建模的钢琴及 karplus-strong 合成的拨弦模型，直接合成 PCM 音频流   |
| 脚本引擎               | Compiler/VM/Debugger                                         | 一个完整的脚本引擎，包含编译器虚拟机调试器                                           |
| UI 框架                 | button/radio/image/edit/label/list/...                       | UI 框架实现                                                                        |
| 协议                   | MQTT/MODBUS/Game-network-synchronization                     | 常用的通讯协议                                                                    |
| 游戏引擎               |                                                              | 集成一个游戏世界框架                                                               |
| FPGA-GPU               |2D accelerator                                               | 实现了基于 FPGA 的 GPU 图形加速器，能够为 PainterEngine 提供不低于 50Mpps 的 2D Blender 及图元光栅化加速，支持 HDMI 输出，目前已在 zynq7000 系列 Soc 上完成验证|

还有更多探索……

---

| functions             | support                                                      | Description                                                                       |
| --------------------- | ------------------------------------------------------------ | ------------------------------------------------------------------------          |
| memorypool            | alloc/free                                                   | Platform-agnostic memory management implementation                               |
| math               | sin/cos/tan/arcsin/log/exp/relu/...                | The complete implementation of the vast majority of C standard math libraries                          |
| signal processing     | dft/dct/fft/dwt/window functions/mfcc/...                | Fourier/Cosine/Wavelet Transform, common window functions, MFCC (Mel-Frequency Cepstral Coefficients), and other signal processing fundamental functions and higher-level feature extraction algorithms, etc.                         |
| Data structures       | string/vector/list/map/stack/fifo/circular-buffer/...                | Platform-agnostic common data structures and algorithms                          |
| Cryptography          | curve25519/AES/SHAx/MD5/...                | This includes commonly used symmetric key algorithms and key exchange algorithms                          |
| image codec           | PNG/JPG/GIF/BMP                                              | Supports decoding of PNG/JPG/GIF/BMP images and encoding of PNG images           |
| audio codec           | Wav/Mp3                                              | Supports decoding of Wav/Mp3 and encoding of Wav           |
| fontmodule            | TTF                                                  | Support fontmodule of truetype(reference: stb_truetype.c)|
| Geometric             | Line/Triangle/Rectangle/Circle/Ring/Sector/Rounded/...      | Geometric rasterization                                                           |
| Renderer              | 2D/3D                                                        | Implementation of a 2D/3D software renderer. A painting  engine                  |
| sound                 | VC codec/mixer/piano/ks                                               | Includes an implementation of a VC(voice conversion), a mixer, a physically-modeled piano and Karplus-Strong string synthesis. Direct synthesis of PCM audio streams|
| Animation             | 2dx/live2D                                                   | 2D frame-by-frame animation and a Live2D-style animation system                  |
| PainterScript         | Compiler/VM/Debugger                                         | A complete scripting engine, including a compiler, virtual machine, and debugger |
| UI                    | button/radio/image/edit/label/list/...                       | Implementation of common UI controls                                             |
| protocals             | MQTT/MODBUS/Game-network-synchronization                     | Common communication protocols                                                   |
| Game framework.       |                                                              | PainterEngine Game Framework                                                      |
| FPGA-GPU               |2D accelerator                                               | Implemented an FPGA-based GPU graphics accelerator that provides at least 50 million pixels per second (Mpps) 2D Blender acceleration for PainterEngine and supports HDMI output.validation has been completed on the Zynq-7000 series SoC|

Many more to explore...

## FPGA-GPU 嵌入式图形加速方案

## The FPGA-GPU Graphics Acceleration Solution

提供一个基于 FPGA 的 GPU IP 核，已在 Zynq7020 上完成功能验证，提供不低于 50Mpps 的 2D Blender 图形渲染加速，支持 HDMI 输出，目前已在 zynq7000 系列 SoC 上完成验证。

A GPU IP core based on FPGA has been developed and functionally verified on the Zynq 7020 platform. It provides 2D Blender graphics rendering acceleration with a performance of no less than 50Mpps (Million pixels per second) and supports HDMI output. This solution has been successfully validated on the Zynq 7000 series SoC.

<p align="center"><img src="images/gpu_block_design.png" alt="PainterEngine designer"></p>

<p align="center"><img src="images/gpu_demo.png" alt="PainterEngine designer"></p>

<p align="center"><img src="images/gpu_demo2.png" alt="PainterEngine designer"></p>

## 组件化开发，支持设计器模式，简单的不能再简单

## Component-based development with support for a designer mode, making it as simple as it gets

<p align="center"><img src="images/designer.png" alt="PainterEngine designer"></p>

## 海量组件，创意无界

## An abundance of components to unleash your creativity without limitations.

<p align="center"><img width="600" src="images/market.png" alt="PainterEngine market"></p>

<p align="center"><img width="600" src="images/paint.png" alt="PainterEngine market"></p>

<p align="center"><img width="600" src="images/piano.png" alt="PainterEngine market"></p>

<p align="center"><img width="600" src="images/l2d.png" alt="PainterEngine market"></p>

### 现在，访问 PainterEngine.com，参与建设

### Now, join the PainterEngine.com contribute to its development

<p align="center">
<img src="documents/assets/mini/1.png" alt="logo">
<img src="documents/assets/mini/2.png" alt="logo">
<img src="documents/assets/mini/3.png" alt="logo">
<img src="documents/assets/mini/4.png" alt="logo">
<img src="documents/assets/mini/5.png" alt="logo">
<img src="documents/assets/mini/6.png" alt="logo">
</p>

<p align="center">
<img src="documents/assets/mini/7.png" alt="logo">
<img src="documents/assets/mini/8.png" alt="logo">
<img src="documents/assets/mini/9.png" alt="logo">
<img src="documents/assets/mini/10.png" alt="logo">
<img src="documents/assets/mini/11.png" alt="logo">
<img src="documents/assets/mini/12.png" alt="logo">
</p>

<p align="center">
<img src="documents/assets/mini/13.png" alt="logo">
<img src="documents/assets/mini/14.png" alt="logo">
<img src="documents/assets/mini/15.png" alt="logo">
<img src="documents/assets/mini/16.png" alt="logo">
<img src="documents/assets/mini/17.png" alt="logo">
<img src="documents/assets/mini/18.png" alt="logo">
</p>

<p align="center">
<img src="documents/assets/mini/19.png" alt="logo">
<img src="documents/assets/mini/20.png" alt="logo">
<img src="documents/assets/mini/21.png" alt="logo">
<img src="documents/assets/mini/22.png" alt="logo">
<img src="documents/assets/mini/23.png" alt="logo">
<img src="documents/assets/mini/24.png" alt="logo">
</p>


