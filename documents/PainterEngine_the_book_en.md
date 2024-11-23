# The Book Of PainterEngine 

## Preface

Welcome to our first lesson of PainterEngine, but before that, I believe you are not clear about what PainterEngine is. Maybe you already have some impression of it on the Internet. It may be a graphics library or a game engine. You may have seen something based on it: acoustics, cryptography, neural networks, digital signal processing, compilers, virtual machines, etc., even a GPU IP core based on FPGA. So you may think it is a large hybrid of various libraries. 

In fact, all of the above are correct, but overall, I am more inclined to think of PainterEngine as an application framework. The ultimate reason for its birth is to solve the extremely troublesome third-party library (even standard library) dependency problem in the program development process, and to simplify the platform migration and compilation difficulty of the program to the greatest extent.

Therefore, as you can see, the compilation of PainterEngine will not let you fall into various third-party dependencies. At present, it can run on almost all platforms that provide C language compilation environment. It does not depend on the operating system and file system, and can run in the bare MCU environment. Even the homepage of PainterEngine is developed by PainterEngine.

PainterEngine has always followed the most simple design principle, and PainterEngine uses C language as its main development language. The built-in script engine of PainterEngine also maximizes compatibility with C language syntax, and makes a small amount of abstraction and generalization of C language types, further reducing its ease of use and use. C language, as a long-standing language, is almost a compulsory course for all major engineering disciplines. It has always maintained strong competitiveness and wide recognition in the development of computer programming, and has become a fact standard that almost all hardware platforms need and provide support. C language maintains a delicate balance between learning and development costs, so you can learn and get started with C language in a short time. With PainterEngine, you can run your program on all platforms and deeply feel the charm of programming art.

PainterEngine has also experienced nearly ten years of development, but for a long time, it has been less officially promoted in the public domain as a private library. One is that in the process of its iteration, many interfaces and functions are still unstable, and we must ensure the rationality and ease of use of the interface design in long-term practice, distinguishing which is "really useful and easy to use" and which is "a flash of inspiration that looks bright and beautiful, but actually useless". Therefore, for a long time, PainterEngine did not have detailed and stable documents, and after so many years of iteration, we finally published those stable, easy-to-use, and simple-to-learn designs, and finally brought them to you in this document.

Finally, I don't want to write the preface too long, and it's time to get to the point. We will start with the environment construction of PainterEngine, the first lesson of PainterEngine. If you have any questions or find bugs, you can ask questions in the PainterEngine forum, or directly send the questions to matrixcascade@gmail.com, and I will give you feedback as soon as possible.

![](assets/mini/1.png)

## 1. A Simplest PainterEngine Program

Before setting up the development environment, let's write the simplest PainterEngine program. Let's create a `main.c` file (the file name can be arbitrary), and then enter the following code in it:

```c
#include "PainterEngine.h"
int main()
{
    PainterEngine_Initialize(800, 480);
    return 1;
}
```

This is a fairly simple PainterEngine program. Simply put, in the first line, we use `include` to include the PainterEngine header file. In the `main` function, we use `PainterEngine_Initialize` to initialize PainterEngine. `PainterEngine_Initialize` takes 2 parameters, which are the width and height of the window (or screen). After running the program, you will probably see the following result:

![](assets/img/1.1.png)

Of course, we haven't used PainterEngine to draw anything on the window yet, so you see a blank screen. It is worth noting that in the PainterEngine framework, when the `main` function returns, the program does not end immediately. In fact, in `PainterEngine.h`, the `main` function is replaced by `px_main`, and the real `main` function is implemented in `px_main.c`. However, you don't need to worry about this for now. Just remember that after the `main` function returns, the program is still running normally. If you want to exit the program, you can call the C language `exit` function yourself, but in many cases in PainterEngine, you don't need to do this. Because in embedded microcontrollers, web pages, driver programs, most of the time you don't need to design an exit function in the program, even on Android or iOS platforms, most of the time you don't need to design an exit function in the program.

## 2. Compiling PainterEngine Programs

### Compiling with PainterEngine Make

If you want to compile PainterEngine project files, the simplest way is to use PainterEngine Make, a compilation tool that you can download from PainterEngine.com. You can find the download button at the bottom of the homepage:

![](assets/img/2.1.png)

After unzipping it, run PainterEngine make.exe, and you will see the following interface:

![](assets/img/2.2.png)

Then select the platform you want to compile, and select the C language code file we created earlier:

![](assets/img/2.3.png)

Then just wait for the compilation to complete:

![](assets/img/2.4.png)

### Compiling with Visual Studio Code

If you want to use Visual Studio Code for compilation, you need to make sure that you have installed the C language development environment for Visual Studio Code. We will skip this step because there are already many tutorials on the Internet.

Then go to PainterEngine and download the source code of PainterEngine:

![](assets/img/2.5.png)

Extract the downloaded source code to a directory on your computer, then you need to note the location of this directory and create a new environment variable named `PainterEnginePath` in the Windows environment variables, and set it to the directory where the PainterEngine library is located:

![](assets/img/2.6.png)

![](assets/img/2.7.png)

Please copy the PainterEngine/Platform/.vscode directory to the side of the source code file:

![](assets/img/2.8.png)

![](assets/img/2.9.png)

Then open the `main.c` file with Visual Studio Code, and you can compile and run it:

![](assets/img/2.10.png)

![](assets/img/2.11.png)

### Compiling with Visual Studio

Of course, if you need a complete IDE development experience, it is still recommended to use Visual Studio for development and compilation. To develop PainterEngine using Visual Studio, you need to open Visual Studio and create an empty project:

![](assets/img/2.12.png)

![](assets/img/2.13.png)

After creating the project, I strongly recommend that you create a new filter:

![](assets/img/2.14.png)

Then add all the files in the `core`, `kernel`, `runtime`, and `platform/windows` directories of the PainterEngine directory to this filter:

![](assets/img/2.15.png)

![](assets/img/2.16.png)

Then add the `main.c` file we wrote earlier to the project:

![](assets/img/2.17.png)

Open `Project` → `Properties` → `VC++ Directories`, include the directory where PainterEngine is located:

![](assets/img/2.18.png)

![](assets/img/2.19.png)

![](assets/img/2.20.png)

It is particularly important to check whether the configuration is consistent with the current configuration of Visual Studio, which is a point that is easy to go wrong:

![](assets/img/2.21.png)

Finally, you can compile, run, and debug:

![](assets/img/2.22.png)

## 3. PainterEngine Lesson One: Outputting the Text "Hello PainterEngine"

As you can see, PainterEngine is a graphical application framework, but according to tradition, our first lesson is still how to output text using PainterEngine; however, it's more accurate to say that we're drawing text rather than just outputting it. Using PainterEngine to draw text is very simple; check out the following code:

```c
#include "PainterEngine.h"
int main()
{
    PainterEngine_Initialize(800, 480);
    // PainterEngine_DrawText
    // Parameter 1: x-coordinate
    // Parameter 2: y-coordinate
    // Parameter 3: text content
    // Parameter 4: alignment
    // Parameter 5: color
    PainterEngine_DrawText(400, 240, "Hello PainterEngine", PX_ALIGN_CENTER, PX_COLOR(255, 255, 0, 0));
    return 1;
}
```

Let's take a look at the `PainterEngine_DrawText` function, which is a text drawing function with 5 parameters. When you run the program, you will see this result:

![](assets/img/3.1.png)

The entire function is quite easy to understand, but here we will explain in detail the meaning of the `alignment` and `color` parameters, as these two concepts will often be mentioned in subsequent tutorials:

Among them, the `alignment` is the alignment of the content when it is drawn on the screen. The alignment methods in PainterEngine include the following formats:

```c
typedef enum
{
    PX_ALIGN_LEFTTOP = 7,     // left top alignment
    PX_ALIGN_MIDTOP = 8,      // middle top alignment
    PX_ALIGN_RIGHTTOP = 9,    // right top alignment
    PX_ALIGN_LEFTMID = 4,     // left middle alignment
    PX_ALIGN_CENTER = 5,      // center alignment
    PX_ALIGN_RIGHTMID = 6,    // right middle alignment
    PX_ALIGN_LEFTBOTTOM = 1,  // left bottom alignment
    PX_ALIGN_MIDBOTTOM = 2,   // middle bottom alignment
    PX_ALIGN_RIGHTBOTTOM = 3, // right bottom alignment
}PX_ALIGN;
```

This alignment method enumeration is designed so that you can directly look at your numeric keypad; its alignment method corresponds to the values on the numeric keypad.

The `color format` is defined as a structure called `px_color`, which has 4 bytes and contains 4 member variables a, r, g, b, representing the transparency, red channel, green channel, and blue channel of the color, respectively. Each component has a value range of 0-255, for example, red, the higher this value, the redder the color.

Therefore, you can see that in the above demonstration code, we drew a red text `Hello PainterEngine`. Now let's try Chinese characters, modify the code above to the following format:

```c
#include "PainterEngine.h"
int main()
{
    PainterEngine_Initialize(800, 480);
    // PainterEngine_DrawText
    // Parameter 1: x-coordinate
    // Parameter 2: y-coordinate
    // Parameter 3: text content
    // Parameter 4: alignment
    // Parameter 5: color
    PainterEngine_DrawText(400, 240, "你好PainterEngine", PX_ALIGN_CENTER, PX_COLOR(255, 255, 0, 0));
    return 1;
}
```

![](assets/img/3.2.png)

However, the Chinese characters do not display correctly. This is because PainterEngine, by default, only has English font models. What should we do if we want to support Chinese? It's not difficult either. For this, we need to prepare a TTF font file first. For example, here I have prepared a YouYuan font, so I just need to load this font:

```c
#include "PainterEngine.h"
int main()
{
    PainterEngine_Initialize(800, 480);
    PainterEngine_LoadFontModule("assets/font.ttf", PX_FONTMODULE_CODEPAGE_GBK, 24);
    PainterEngine_DrawText(400, 240, "你好 PainterEngine", PX_ALIGN_CENTER, PX_COLOR(255, 255, 0, 0));
    return 1;
}
```

![](assets/img/3.3.png)

The first parameter of the `PainterEngine_LoadFontModule` function is the path of the TTF font file, while the relative paths are relative to the location of the exe file. The second parameter is the character set. By default, Visual Studio uses the GBK character set. If you use Visual Studio Code, then the default is UTF8 encoding, and the second parameter should be changed to `PX_FONTMODULE_CODEPAGE_GBK`. The last parameter is the size of the font.

## 4. Drawing Geometric Shapes Using PainterEngine

In addition to drawing text, PainterEngine can also directly draw the following geometric shapes:

`px_void PainterEngine_DrawLine(px_int x1, px_int y1, px_int x2, px_int y2, px_int linewidth, px_color color);`
This function is used to draw a line segment.
- x1, y1: The starting coordinates of the line segment.
- x2, y2: The ending coordinates of the line segment.
- linewidth: The width of the line segment.
- color: The color of the line segment.

```c
#include "PainterEngine.h"
int main()
{
    PainterEngine_Initialize(800, 480);
    // Set the starting and ending coordinates
    px_int x1 = 50;
    px_int y1 = 50;
    px_int x2 = 200;
    px_int y2 = 200;

    // Set the line width and color
    px_int linewidth = 5;
    px_color color = PX_COLOR(255, 0, 0, 255); // Red

    // Draw the line segment
    PainterEngine_DrawLine(x1, y1, x2, y2, linewidth, color);
    return 1;
}
```

![](assets/img/3.4.png)


`px_void PainterEngine_DrawRect(px_int x, px_int y, px_int width, px_int height, px_color color);`
This function is used to draw a rectangle.
- x, y: The top-left corner coordinates of the rectangle.
- width: The width of the rectangle.
- height: The height of the rectangle.
- color: The color of the rectangle.

![](assets/img/3.5.png)

```c
#include "PainterEngine.h"
int main()
{
    PainterEngine_Initialize(800, 480);
    // Set the top-left corner coordinates of the rectangle
    px_int x = 100;
    px_int y = 100;

    // Set the width and height of the rectangle
    px_int width = 150;
    px_int height = 100;

    // Set the color of the rectangle
    px_color color = PX_COLOR(255, 0, 255, 0); // Green

    // Draw the rectangle
    PainterEngine_DrawRect(x, y, width, height, color);
    return 1;
}
```


`px_void PainterEngine_DrawCircle(px_int x, px_int y, px_int radius, px_int linewidth, px_color color);`
This function is used to draw a circle (ring).
- x, y: The center coordinates of the circle.
- radius: The radius of the circle.
- linewidth: The line width of the circle.
- color: The color of the circle.

```c
#include "PainterEngine.h"
int main()
{
    PainterEngine_Initialize(800, 480);
    // Set the center coordinates of the circle
    px_int x = 200;
    px_int y = 200;
    // Set the radius of the circle
    px_int radius = 50;
    // Set the line width of the circle
    px_int linewidth = 5;
    // Set the color of the circle
    px_color color = PX_COLOR(255, 0, 0, 255); // Blue
    // Draw the circle
    PainterEngine_DrawCircle(x, y, radius, linewidth, color);
    return 1;
}
```

![](assets/img/3.6.png)


`px_void PainterEngine_DrawSolidCircle(px_int x, px_int y, px_int radius, px_color color);`
This function is used to draw a solid circle.
- x, y: The center coordinates of the circle.
- radius: The radius of the circle.
- color: The color of the circle.

```c
#include "PainterEngine.h"
int main()
{
    PainterEngine_Initialize(800, 480);

    // Set the center coordinates of the circle
    px_int x = 100;
    px_int y = 100;

    // Set the radius of the circle
    px_int radius = 50;

    // Set the color of the circle
    px_color color = PX_COLOR(255, 255, 0, 255); 

    // Draw the solid circle
    PainterEngine_DrawSolidCircle(x, y, radius, color);
    return 1;
}
```

![](assets/img/3.7.png)


`px_void PainterEngine_DrawSector(px_int x, px_int y, px_int inside_radius, px_int outside_radius, px_int start_angle, px_int end_angle, px_color color);`
This function is used to draw a sector.
Parameter description:
- x, y: The center coordinates of the sector.
- inside_radius: The inner radius of the sector.
- outside_radius: The outer radius of the sector.
- start_angle: The starting angle of the sector (in degrees, negative angles are supported).
- end_angle: The ending angle of the sector (in degrees, negative angles are supported).
- color: The color of the sector.

```c
#include "PainterEngine.h"
int main()
{
    PainterEngine_Initialize(800, 480);
    // Set the center coordinates of the sector
    px_int x = 100;
    px_int y = 100;
    // Set the radii of the sector
    px_int inside_radius = 50;
    px_int outside_radius = 100;
    // Set the starting and ending angles of the sector
    px_int start_angle = 0;
    px_int end_angle = 135;
    // Set the color of the sector
    px_color color = PX_COLOR(255, 255, 0, 0); // Red
    // Draw the sector
    PainterEngine_DrawSector(x, y, inside_radius, outside_radius, start_angle, end_angle, color);
    return 1;
}
```
![](assets/img/3.8.png)


`px_void PainterEngine_DrawPixel(px_int x, px_int y, px_color color);`
This function is used to draw a pixel.
- x, y: The coordinates of the pixel.
- color: The color of the pixel.

This is just for drawing a single pixel, so no example image is shown.

## 5. Drawing Images Using PainterEngine

Using PainterEngine to draw images is still quite simple, but before drawing an image, we need to load it first.

PainterEngine can directly load images from files, and it natively supports loading four types of static image formats: PNG, JPG, BMP, and TRAW. To store the loaded image, we need a structure called "texture".

In PainterEngine, textures are described using the `px_texture` structure, so to load a texture, we need the `PX_LoadTextureFromFile` function, which is a three-parameter image file loading function. The first parameter is the memory pool, which I will explain in more detail in later sections. By default, PainterEngine provides two default memory pools: one is `mp`, and the other is `mp_static`. The former is generally used for elements that require frequent allocation and deallocation, while the latter is used for storing static resources. Since images are usually static resources, filling in `mp_static` is sufficient. The second parameter is a pointer to our texture structure; after the image is successfully loaded, this structure will be initialized and used to save the image data. The last parameter is the path to the image file.

After successfully loading the file, we use the `PainterEngine_DrawTexture` function to draw it. This is a four-parameter function:

- The first parameter is the pointer to our texture structure;
- The second and third parameters are the x and y coordinates where the image needs to be drawn;
- The fourth parameter is the alignment method we discussed earlier.

Refer to the following code:

```c
#include "PainterEngine.h"
px_texture mytexture; // Texture
int main()
{
    PainterEngine_Initialize(512, 512);
    if (!PX_LoadTextureFromFile(mp_static, &mytexture, "assets/demo.png"))
    {
        // Failed to load texture
        return 0;
    }
    PainterEngine_DrawTexture(&mytexture, 0, 0, PX_ALIGN_LEFTTOP);
    
    return 1;
}
```

![](assets/img/4.1.png)

## 6. PainterEngine Memory Pool Management Mechanism

Since PainterEngine has no dependency on system or standard libraries, an independent memory management mechanism, separate from the system and standard library, must be implemented within PainterEngine to manage its internal memory. Therefore, PainterEngine uses a memory pool as a dynamic memory management system.

The implementation of the PainterEngine memory pool is also very straightforward. To use memory, you must prepare a segment of available memory space in advance, which will be managed as the memory space of the memory pool. For example, in the following code, we can define a large global array in C language, then use this array space as the allocation space for the memory pool:

```c
#include "PainterEngine.h"
unsigned char my_memory_cache[1024 * 1024];
int main()
{
    px_memorypool mp;
    px_void* myalloc;
    mp = PX_MemorypoolCreate(my_memory_cache, sizeof(my_memory_cache)); // Create memory pool
    myalloc = MP_Malloc(&mp, 1024); // Allocate 1024 bytes in the memory pool
    return 1;
}
```

It should be noted that, **_the space allocated from the memory pool will be slightly less than the space allocated to the memory pool. If the space you allocate exceeds the memory pool, it will result in a system halt error._**

```c
#include "PainterEngine.h"
unsigned char my_memory_cache[1024 * 1024];
int main()
{
    px_memorypool mp;
    px_void* myalloc;
    mp = PX_MemorypoolCreate(my_memory_cache, sizeof(my_memory_cache)); // Create memory pool
    myalloc = MP_Malloc(&mp, 1024*1024); // Allocate 1024*1024 bytes in the memory pool, but the actual capacity of the memory pool is less than the allocated capacity, so here there is not enough memory, and it will enter an interruption here
    return 1;
}
```

If you do not want a system halt error due to insufficient memory pool, you can use the following two methods:

1. You can set up an error callback to handle the memory pool errors yourself:

```c
#include "PainterEngine.h"
unsigned char my_memory_cache[1024 * 1024];

PX_MEMORYPOOL_ERROR_FUNCTION(my_memory_cache_error)
{
    switch (error)
    {
    case PX_MEMORYPOOL_ERROR_OUTOFMEMORY:
        printf("Memory access error\n");
        break;
    case PX_MEMORYPOOL_ERROR_INVALID_ACCESS:
        printf("Unable to access memory\n");
        break;
    case PX_MEMORYPOOL_ERROR_INVALID_ADDRESS:
        printf("Invalid memory address (UAF or double free)\n");
        break;
    default:
        break;
    }
}
int main()
{
    px_memorypool mp;
    px_void* myalloc;
    mp = PX_MemorypoolCreate(my_memory_cache, sizeof(my_memory_cache)); // Create memory pool
    MP_ErrorCatch(&mp, my_memory_cache_error, 0); // Set error callback
    myalloc = MP_Malloc(&mp, 1024*1024); // Allocate 1024*1024 bytes in the memory pool
    return 1;
}
```

2. Or you can directly turn off the error exception handling of the memory pool, so when the memory pool cannot allocate enough memory normally, it will directly return `NULL`:

```c
#include "PainterEngine.h"
unsigned char my_memory_cache[1024 * 1024];

int main()
{
    px_memorypool mp;
    px_void* myalloc;
    mp = PX_MemorypoolCreate(my_memory_cache, sizeof(my_memory_cache)); // Create memory pool
    MP_NoCatchError(&mp, PX_TRUE); // Set the memory pool not to catch errors
    myalloc = MP_Malloc(&mp, 1024*1024); // Allocate 1024*1024 bytes in the memory pool, but since the memory pool does not catch errors, it will directly return NULL
    return 1;
}
```

In PainterEngine, there are 2 system default memory pools, they are `mp` and `mp_static`. You can open the `PainterEngine_Application.h` file and find the definitions of these two memory pools inside, but what we need to pay most attention to is the following code:

```c
#define PX_APPLICATION_NAME "PainterEngine"
#define PX_APPLICATION_MEMORYPOOL_STATIC_SIZE (1024*1024*64)
#define PX_APPLICATION_MEMORYPOOL_DYNAMIC_SIZE (1024*1024*32)
#define PX_APPLICATION_MEMORYPOOL_SPACE_SIZE (1024*1024*16)
```

These are the configuration macros directly related to these two memory pools, where `PX_APPLICATION_MEMORYPOOL_STATIC_SIZE` represents the memory allocation size of the `mp_static` memory pool, and `PX_APPLICATION_MEMORYPOOL_DYNAMIC_SIZE` is the memory allocation size of the `mp` memory pool, while `PX_APPLICATION_MEMORYPOOL_SPACE_SIZE` refers to other system resources. When the PainterEngine program starts running, it will at least occupy the memory accumulated by these three macros, and subsequent memory allocations will revolve around these few memory pools. If you find that the memory available for PainterEngine's operation is not enough, you can manually expand the size of the memory pool. Of course, if you hope to save some memory, you can also manually reduce their sizes.

## 7. 使用 PainterEngine 创建 GUI 按钮

在本章节中，我们将第一次接触 PainterEngine 的组件。现在，我们将使用 PainterEngine 创建一个经典 GUI 组件——按钮。

在 PainterEngine 中，所有的组件都是由 `PX_Object` 结构体进行描述的，创建组件返回的都是一个 `PX_Object *` 类型的指针。

但在本章节中，我们并不需要考虑的那么复杂，我们只需要创建一个按钮出来即可。在 PainterEngine 中，最常用的按钮是 `PX_Object_PushButton` 类型。

```c
#include "PainterEngine.h"
int main()
{
    PX_Object* myButtonObject;
    PainterEngine_Initialize(800, 480);
    PainterEngine_LoadFontModule("assets/font.ttf", PX_FONTMODULE_CODEPAGE_GBK, 20);
    myButtonObject = PX_Object_PushButtonCreate(mp, root, 300, 200, 200, 80, "我是一个按钮", PainterEngine_GetFontModule());
    return 1;
}
```
![](assets/img/7.1.gif)

现在，我们来详细看看 `PX_Object_PushButtonCreate` 函数。其中，第一个参数是一个内存池，在之前我们说过 PainterEngine 有 2 个系统默认的内存池，其实这里填 `mp` 或者 `mp_static` 都是没有问题的，但考虑到界面可能会变动设计对象分配与销毁，所以我们还是选择 `mp` 内存池。

第二个参数 `root` 是 PainterEngine 的根对象，PainterEngine 对象管理机制我们将在之后讨论。在这里，你只需要理解为，这里填 `root` 的意思是 **_创建一个按钮对象作为根对象的子对象_**。这样按钮就能链接到系统对象树中，进行事件响应和渲染。

然后是按钮的 x，y，width，height，也就是位置和宽度高度等信息。

最后一个是字模指针，也就是之前我们加载的 ttf 字模文件，如果没有它，我们的按钮就不能显示中文汉字了。当然你可以选择其他的字体，以实现不同的风格。

## 8. PainterEngine 对象传递机制

在上一个章节中，我们初略了解了根对象 `root`，那么在本章节，我们将学习一下 PainterEngine 的对象管理机制。

正如我们之前所说的，在 PainterEngine 中，所有的组件都是由 `PX_Object` 结构体进行描述的，PainterEngine 的对象是以树的形式存在的：

![](assets/img/8.1.png)

每一个 `PX_Object` 都是这个树中的一个节点，都可以有自己的子节点（可能多个）和自己的父节点（只能有一个）。同时，每一个 `PX_Object` 都有以下四个基本功能函数：

`Create`：对象创建函数，或者说是对象初始化函数，在 PainterEngine 中它一般是 `PX_Object_xxxxxCreate` 这种形式的，其中 `xxxxx` 就是这个对象的名称，比如上一章节的 `PushButton`，`Create` 函数一般是对象的一些初始化处理，并会将自己连接到对象树中。

`Update`：对象的物理信息更新工作基本在这个函数中完成，一般会处理对象的一些物理信息，比如位置大小速度等，常见于游戏设计中的物体，在 GUI 对象中则比较少见，其设计是与之后的 `Render` 也就是绘制函数进行区分，因为在例如游戏服务端中，对象并不需要进行绘制，且绘制是非常消耗性能的。

`Render`：对象的绘制工作基本在这个函数中完成，用于 `PX_Object` 的绘制功能，将图像数据渲染到屏幕上，当然有些情况下物理信息也会在这个函数中做，是因为这个对象的物理信息并不会影响游戏的实际运行结果，例如一些特效和粒子效果，多数的 GUI 组件也几乎只用得到 `Render` 函数。

`Free`：对象的释放工作基本在这个函数中完成，例如在 `Create` 中加载了纹理，或者申请了内存，在这个函数中应该被释放。

以上 `Update`、`Render`、`Free` 函数具有传递的特性，也就是说：

- 如果某个对象节点执行了 `Update`，那么它的所有子对象也会执行 `Update`
- 如果某个对象节点执行了 `Render`，那么它的所有子对象也会执行 `Render`
- 如果某个对象节点执行了 `Free`，那么它的所有子对象也会执行 `Free`，父对象被删除了，它的子节点也会被删除，并且将会一直迭代到以这个节点为根节点的所有子节点都被删除。

因此，在上一章节我们创建了按钮，并将它连接到了 `root` 节点，那么我们是不需要自己再手动执行 `Update`、`Render`、`Free` 函数的（在 `PX_Object_PushButton.c` 中它们已经被写好了），因为根节点 `root` 是被自动更新渲染和释放的，我们只需要负责 `Create` 就可以了。

当然，如果你希望删除这个对象的话，你只需要调用 `PX_ObjectDelayDelete` 或者 `PX_ObjectDelete` 就可以了：

```c
#include "PainterEngine.h"
int main()
{
    PX_Object* myButtonObject;
    PainterEngine_Initialize(800, 480);
    PainterEngine_LoadFontModule("assets/font.ttf", PX_FONTMODULE_CODEPAGE_GBK, 20);
    myButtonObject = PX_Object_PushButtonCreate(mp, root, 300, 200, 200, 80, "我是一个按钮", PainterEngine_GetFontModule());
    PX_ObjectDelayDelete(myButtonObject); // 删除对象
    return 1;
}
```

这两个函数的功能和参数都是一样的，但是 `PX_ObjectDelayDelete` 会在更新和渲染完成后才执行删除，`PX_ObjectDelete` 则是立即删除，我建议使用 `PX_ObjectDelayDelete`，这样你就可以避免在某些情况下因为对象被立即删除了，而其它对象仍然引用了这个对象的数据，这会导致其访问失效内存。

## 9. PainterEngine 消息机制

现在，虽然我们创建了一个按钮，但我们却没办法响应它，为了响应按钮事件，我们需要将按钮控件和消息进行绑定，请查看以下代码：

```c
#include "PainterEngine.h"

PX_OBJECT_EVENT_FUNCTION(OnButtonClick)
{
    PX_Object_PushButtonSetText(pObject, "我被点击了");
}

int main()
{
    PX_Object* myButtonObject;
    PainterEngine_Initialize(800, 480);
    PainterEngine_LoadFontModule("assets/font.ttf", PX_FONTMODULE_CODEPAGE_GBK, 20);
    myButtonObject = PX_Object_PushButtonCreate(mp, root, 300, 200, 200, 80, "我是一个按钮", PainterEngine_GetFontModule());
    PX_ObjectRegisterEvent(myButtonObject, PX_OBJECT_EVENT_EXECUTE, OnButtonClick, 0);
    
    return 1;
}
```

![](assets/img/9.1.gif)

其中，`PX_OBJECT_EVENT_FUNCTION` 是一个宏，因为事件响应函数是一个固定的格式，因此非常建议你使用宏的方式来申明它，它的定义原型如下：

```c
#define PX_OBJECT_EVENT_FUNCTION(name) px_void name(PX_Object *pObject, PX_Object_Event e, px_void * ptr)
```

可以看到，这个回调函数有 3 个参数，第一个是响应时间的对象的指针，因为是按钮点击被触发了，所以这个指针指向的就是这个按钮对象；第二个参数是事件类型 `e`，它是触发的事件类型；最后一个参数则是用户传递来的指针，它在注册时间响应函数 `PX_ObjectRegisterEvent` 被调用时就被传递进来了。

事件类型有以下几种：

```c
#define PX_OBJECT_EVENT_ANY           0  // 任意事件
#define PX_OBJECT_EVENT_CURSORMOVE    1  // 鼠标移动
#define PX_OBJECT_EVENT_CURSORUP      2  // 鼠标左键弹起或触摸屏弹起
#define PX_OBJECT_EVENT_CURSORRDOWN   3  // 鼠标右键按下
#define PX_OBJECT_EVENT_CURSORDOWN    4  // 鼠标左键按下或触摸屏按下
#define PX_OBJECT_EVENT_CURSORRUP     5  // 鼠标右键弹起
#define PX_OBJECT_EVENT_CURSOROVER    6  // 鼠标进入范围
#define PX_OBJECT_EVENT_CURSOROUT     7  // 鼠标离开范围
#define PX_OBJECT_EVENT_CURSORWHEEL   8  // 鼠标滚轮
#define PX_OBJECT_EVENT_CURSORCLICK   9  // 鼠标左键点击
#define PX_OBJECT_EVENT_CURSORDRAG    10 // 鼠标拖拽
#define PX_OBJECT_EVENT_STRING        11 // 字符串事件（输入法输入）
#define PX_OBJECT_EVENT_EXECUTE       12 // 执行事件，不同组件有不同的执行方式
#define PX_OBJECT_EVENT_VALUECHANGED  13 // 值改变事件，例如滑动条的值改变，或者文本框的值改变，或者列表框的选中项改变
#define PX_OBJECT_EVENT_DRAGFILE      14 // 拖拽文件
#define PX_OBJECT_EVENT_KEYDOWN       15 // 键盘按下
#define PX_OBJECT_EVENT_KEYUP         16 // 键盘弹起
#define PX_OBJECT_EVENT_IMPACT        17 // 碰撞事件
#define PX_OBJECT_EVENT_SCALE         18 // 缩放事件
#define PX_OBJECT_EVENT_WINDOWRESIZE  19 // 窗口大小改变
#define PX_OBJECT_EVENT_ONFOCUS       20 // 获得焦点
#define PX_OBJECT_EVENT_LOSTFOCUS     21 // 失去焦点
#define PX_OBJECT_EVENT_CANCEL        22 // 取消事件
#define PX_OBJECT_EVENT_CLOSE         23 // 关闭事件
#define PX_OBJECT_EVENT_CURSORMUP     24 // 鼠标中键弹起
#define PX_OBJECT_EVENT_CURSORMDOWN   25 // 鼠标中键按下
#define PX_OBJECT_EVENT_REQUESTDATA   26 // 请求数据
#define PX_OBJECT_EVENT_OPEN          27 // 打开事件
#define PX_OBJECT_EVENT_SAVE          28 // 保存事件
#define PX_OBJECT_EVENT_TIMEOUT       29 // 超时事件
#define PX_OBJECT_EVENT_DAMAGE        30 // 伤害事件
```

以上事件并非全部都是任何组件都会响应的，例如在上面例子中的 `PX_OBJECT_EVENT_EXECUTE`，它是按钮被单击时会被触发的事件，或者是文本框中按下回车会触发的事件，但有些例如滚动条和进度条，并不会触发这个事件。也就是说有些事件是专属的。

但是类似于带有 `CURSOR` 或 `KEY` 的事件，是所有连接在 `root` 节点的组件都会收到的事件（但不一定响应）。需要注意的是，类似于鼠标或触摸屏的 `CURSOR` 事件，并非只有鼠标或触摸屏移动到组件所在位置与范围时才会触发，只要有这类事件投递到 `root` 节点，他就会逐层传递给它的所有子节点。如果你希望实现类似于按钮中的 "仅在鼠标点击到按钮时" 才触发，你必须自行实现范围判断。

你可以使用

```c
px_float PX_Object_Event_GetCursorX(PX_Object_Event e); // 获取cursor事件的 x 坐标
px_float PX_Object_Event_GetCursorY(PX_Object_Event e); // 获取cursor事件的 y 坐标
px_float PX_Object_Event_GetCursorZ(PX_Object_Event e); // 获取cursor事件的 z 坐标，一般用于鼠标中键滚轮
```

来获取 `cursor` 事件中类似于 "鼠标现在在哪里" 的功能。

让我们回到源代码 `OnButtonClick` 中做的很简单，就是用 `PX_Object_PushButtonSetText` 改变了按钮文本的内容。

最后让我们来到 `PX_ObjectRegisterEvent` 函数，这个函数用于将事件与 C 语言函数绑定在一起，第一个参数是我们之前创建好的按钮组件的指针，第二个参数是我们想要绑定的事件类型，这里的 `PX_OBJECT_EVENT_EXECUTE` 就是按钮被点击时触发的，第三个则是用户指针，它会被传递到回调函数中，如果你用不到，你可以直接填 `PX_NULL`。

## 10. 小例子，用 PainterEngine 实现一个电子相册

现在，让我们用一个小例子来开启 PainterEngine 组件化开发的第一步。在本例程中，我将使用按钮和图片框组件，开发一个电子相册功能。本文中的美术资源，你可以在 `documents/logo` 中找到。

```c
#include "PainterEngine.h"

PX_Object* Previous, * Next, * Image;
px_texture my_texture[10]; // 存放图片的数组
px_int index = 0; // 当前图片的索引

PX_OBJECT_EVENT_FUNCTION(OnButtonPreClick)
{
    index--;
    if (index < 0)
    {
        index = 9;
    }
    PX_Object_ImageSetTexture(Image, &my_texture[index]); // 设置图片
}

PX_OBJECT_EVENT_FUNCTION(OnButtonNextClick)
{
    index++;
    if (index > 9)
    {
        index = 0;
    }
    PX_Object_ImageSetTexture(Image, &my_texture[index]);
}

int main()
{
    px_int i;
    PainterEngine_Initialize(512, 560); // 初始化
    for(i = 0; i < 10; i++)
    {
        px_char path[256];
        PX_sprintf1(path, 256, "assets/%1.png", PX_STRINGFORMAT_INT(i+1));
        if (!PX_LoadTextureFromFile(mp_static, &my_texture[i], path)) // 加载图片
        {
            // 加载失败
            printf("加载失败");
            return 0;
        }
    }
    PainterEngine_LoadFontModule("assets/font.ttf", PX_FONTMODULE_CODEPAGE_GBK, 20); // 加载字体
    Image = PX_Object_ImageCreate(mp, root, 0, 0, 512, 512, 0); // 创建图片对象
    Previous = PX_Object_PushButtonCreate(mp, root, 0, 512, 256, 48, "上一张", PainterEngine_GetFontModule()); // 创建按钮对象
    Next = PX_Object_PushButtonCreate(mp, root, 256, 512, 256, 48, "下一张", PainterEngine_GetFontModule()); // 创建按钮对象
    PX_ObjectRegisterEvent(Previous, PX_OBJECT_EVENT_EXECUTE, OnButtonPreClick, PX_NULL); // 注册按钮事件
    PX_ObjectRegisterEvent(Next, PX_OBJECT_EVENT_EXECUTE, OnButtonNextClick, PX_NULL); // 注册按钮事件
    return 1;
}
```

在上述代码中 `OnButtonPreClick` 和 `OnButtonNextClick` 分别是上一张和下一张按钮的回调函数，我们使用 `PX_Object_ImageSetTexture` 函数，对图片框进行切换。

而在 `main` 函数中，我们先加载了 ttf 字体，然后用 `PX_Object_ImageCreate` 创建了一个图片组件，之后我们创建了 2 个按钮，并用 `PX_ObjectRegisterEvent` 绑定了事件回调函数。最后，看看运行结果：

![](assets/img/10.1.gif)

## 11. 更多常用的 PainterEngine 组件

你可以在 `PainterEngine/kernel` 的文件中，找到 PainterEngine 的内置组件，所有的组件名称都是以 `PX_Object_XXXXX` 开头的，在这里，我为你列举一些常用的组件及示范代码：

- 文本框：

```c
#include "PainterEngine.h"
PX_OBJECT_EVENT_FUNCTION(PX_Object_EditOnTextChanged)
{
    // 文本改变后后这里会被执行
}

int main()
{
    PX_Object* pObject;
    PainterEngine_Initialize(600, 400);
    // 创建文本框
    pObject = PX_Object_EditCreate(mp, root, 200, 180, 200, 40, 0);
    // 注册编辑框文本改变事件
    PX_ObjectRegisterEvent(pObject, PX_OBJECT_EVENT_VALUECHANGED, PX_Object_EditOnTextChanged, PX_NULL);
    return 0;
}
```

![](assets/img/11.1.gif)

- 列表框：

```c
#include "PainterEngine.h"

PX_OBJECT_RENDER_FUNCTION(PX_Object_OnMyListItemRender)
{
    px_float objx, objy, objWidth, objHeight;
    PX_Object_ListItem *pItem = PX_Object_GetListItem(pObject);
    PX_OBJECT_INHERIT_CODE(pObject, objx, objy, objWidth, objHeight);
    // 绘制出其文本
    PX_FontModuleDrawText(psurface, 0, (px_int)objx + 3, (px_int)objy + 3, PX_ALIGN_LEFTTOP, (const px_char *)pItem->pdata, PX_COLOR_WHITE);
}


PX_OBJECT_LIST_ITEM_CREATE_FUNCTION(PX_Object_OnMyListItemCreate)
{
    // 绑定 ListItem 的渲染函数
    ItemObject->Func_ObjectRender[0] = PX_Object_OnMyListItemRender;
    return PX_TRUE;
}

PX_OBJECT_EVENT_FUNCTION(PX_Object_ListOnSelectChanged)
{
    // 当选中项改变时
    return;
}

int main()
{
    PX_Object* pObject;
    PainterEngine_Initialize(600, 400);

    // 创建 list
    pObject = PX_Object_ListCreate(mp, root, 100, 100, 400, 200, 24, PX_Object_OnMyListItemCreate, 0);
    PX_Object_ListAdd(pObject, "Item1");
    PX_Object_ListAdd(pObject, "Item2");
    PX_Object_ListAdd(pObject, "Item3");
    PX_Object_ListAdd(pObject, "Item4");
    PX_Object_ListAdd(pObject, "Item5");
    PX_ObjectRegisterEvent(pObject, PX_OBJECT_EVENT_VALUECHANGED, PX_Object_ListOnSelectChanged, 0);
    return 0;
}
```

![](assets/img/11.2.gif)

- 滑动条：

```c
#include "PainterEngine.h"

PX_OBJECT_EVENT_FUNCTION(SliderChanged)
{
    // 垂直滑动条值改变后执行这里的代码
    return;
}

int main()
{
    PX_Object* pObject;
    PainterEngine_Initialize(600, 400);
    // 水平滑动条
    PX_Object_SliderBarCreate(mp, root, 200, 50, 200, 24, PX_OBJECT_SLIDERBAR_TYPE_HORIZONTAL, PX_OBJECT_SLIDERBAR_STYLE_BOX);
    // 垂直滑动条
    pObject = PX_Object_SliderBarCreate(mp, root, 200, 100, 24, 200, PX_OBJECT_SLIDERBAR_TYPE_VERTICAL, PX_OBJECT_SLIDERBAR_STYLE_BOX);
    PX_ObjectRegisterEvent(pObject, PX_OBJECT_EVENT_VALUECHANGED, SliderChanged, 0);
    return 0;
}
```
![](assets/img/11.3.gif)

- 下拉框：

```c
#include "PainterEngine.h"

int main()
{
    PX_Object* pObject;
    PainterEngine_Initialize(600, 400);
    pObject = PX_Object_SelectBarCreate(mp, root, 200, 150, 200, 24, 0);
    PX_Object_SelectBarAddItem(pObject, "Item1");
    PX_Object_SelectBarAddItem(pObject, "Item2");
    PX_Object_SelectBarAddItem(pObject, "Item3");
    PX_Object_SelectBarAddItem(pObject, "Item4");
    PX_Object_SelectBarAddItem(pObject, "Item5");
    return 0;
}
```
![](assets/img/11.4.gif)

- 示波器：

```c
#include "PainterEngine.h"

// 必须是生存域内有效可访问的数据，这里定义为全局变量
px_double data_x[100];
px_double data_y[100];

int main()
{
    PX_Object_OscilloscopeData data;
    PX_Object* pObject;

    px_int i;
    PainterEngine_Initialize(600, 600);
    
    // 初始化一个测试数据
    for (i = 0; i < 100; i++)
    {
        data_x[i] = i;
        data_y[i] = i+PX_randRange(-10, 10);
    }
    
    pObject = PX_Object_OscilloscopeCreate(mp, root, 0, 0, 600, 600, 0);

    // 设置水平坐标最小值最大值
    PX_Object_OscilloscopeSetHorizontalMin(pObject, 0);
    PX_Object_OscilloscopeSetHorizontalMax(pObject, 100);

    // 设置垂直坐标（左边）最小值最大值 0-100
    PX_Object_OscilloscopeSetLeftVerticalMin(pObject, 0);
    PX_Object_OscilloscopeSetLeftVerticalMax(pObject, 100);

    // 数据类型
    data.Color = PX_COLOR(255, 192, 255, 128); // 数据颜色
    data.ID = 0;
    data.linewidth = 3; // 数据线宽
    data.Map = PX_OBJECT_OSCILLOSCOPE_OSCILLOSCOPEDATA_MAP_LEFT; // 数据映射到左边垂直坐标
    data.MapHorizontalArray = data_x; // 数据水平坐标
    data.MapVerticalArray = data_y; // 数据垂直坐标
    data.Size = 100; // 数据大小
    data.Visibled = PX_TRUE; // 数据可见
    data.Normalization = 1; // 数据归一化系数为 1
    
    // 添加数据
    PX_Object_OscilloscopeAddData(pObject, data);
    return 0;
}
```

![](assets/img/11.5.gif)

因为实在太多了，我无法为你列举所有的组件，如果你希望知道某个组件的具体用法和某个组件到底是做什么的，你可以访问 PainterEngine 的 [组件市场](https://market.painterengine.com/)，在那里你可以找到 PainterEngine 内置组件和三方组件的说明和示例代码。

![](assets/img/11.6.png)

## 12. 实现自己的 PainterEngine 组件

PainterEngine 鼓励组件式的开发架构。也就是说，不论是游戏还是 GUI 交互程序，甚至是程序功能，我们都可以用组件的形式去开发它。

组件式开发有点类似于 C++ 中的 Class，每一个组件，都要实现自己的 `Create`、`Update`、`Render`、`Free` 函数。关于上面四个函数，你可以参考 [前面的对象传递机制](#8painterengine-对象传递机制) 这一章节。

为了演示这一点，让我们来实现一个“可控拖动旋转图片组件”，即我们可以用鼠标拖动图片在界面的位置，并用鼠标中键来旋转它。

为了实现这一个功能，让我们一步一步完成这个步骤。首先，为了创建一个组件，我们需要一个结构体来描述我们的组件。我们需要绘制图片，所以我们需要一个 `px_texture` 类型。同时，我们还需要旋转图片，因此它还有一个 `rotation` 用于描述旋转的角度：

```c
#include "PainterEngine.h"
typedef struct
{
    px_texture image;
    px_int rotation;
}PX_Object_MyObject;

px_int main()
{
    PainterEngine_Initialize(800, 480);
    return PX_TRUE;
}
```

之后，我们需要定义我们的 `Create`、`Update`、`Render` 和 `Free` 函数，其中 `Update`、`Render`、`Free` 有对应的格式，它们都有一个宏来简化我们的定义过程：

```c
#define PX_OBJECT_RENDER_FUNCTION(name) px_void name(px_surface *psurface, PX_Object *pObject, px_int idesc, px_dword elapsed)
#define PX_OBJECT_UPDATE_FUNCTION(name) px_void name(PX_Object *pObject, px_int idesc, px_dword elapsed)
#define PX_OBJECT_FREE_FUNCTION(name) px_void name(PX_Object *pObject, px_int idesc)
```

那么，在主函数中，我们就可以这样定义我们的这几个函数：

```c
#include "PainterEngine.h"
typedef struct
{
    px_texture image;
    px_int rotation;
}PX_Object_MyObject;

PX_OBJECT_UPDATE_FUNCTION(MyObjectUpdate)
{
}

PX_OBJECT_RENDER_FUNCTION(MyObjectRender)
{
    PX_Object_MyObject *pMyObject = PX_ObjectGetDesc(PX_Object_MyObject, pObject);
    PX_TextureRenderEx(psurface, &pMyObject->image, (px_int)pObject->x, (px_int)pObject->y, PX_ALIGN_CENTER, 0, 1, pMyObject->rotation);
}

PX_OBJECT_FREE_FUNCTION(MyObjectFree)
{
    PX_Object_MyObject *pMyObject = PX_ObjectGetDesc(PX_Object_MyObject, pObject);
    PX_TextureFree(&pMyObject->image);
}

px_int main()
{
    PainterEngine_Initialize(800, 480);
    return PX_TRUE;
}
```

其中，因为我们不需要更新一些物理信息，所以 `MyObjectUpdate` 函数中我们可以什么都不写，在 `MyObjectRender` 中我们只需要把图片绘制出来就可以了，这里我们先使用 `PX_ObjectGetDesc` 函数获得我们定义好的结构体指针，它的第一个参数是结构体类型，第二个参数则是函数传递进来的 `pObject` 指针，然后我们只需要用 `PX_TextureRenderEx` 函数把图片绘制出来就可以了。

多提一句，`PX_TextureRenderEx` 函数用于在指定的表面上渲染纹理，并提供了对齐、混合、缩放和旋转等扩展选项。其中：
  - `psurface`：指向要渲染纹理的表面的指针。
  - `resTexture`：指向要渲染的纹理资源的指针。
  - `x`：在表面上绘制纹理的 x 坐标。
  - `y`：在表面上绘制纹理的 y 坐标。
  - `refPoint`：对齐的参考点（例如，中心，左上角等）。
  - `blend`：指向混合选项结构的指针（如果不需要混合，可以为 `NULL`）。
  - `scale`：纹理的缩放因子（1.0 表示不缩放）。
  - `Angle`：纹理的旋转角度，以度为单位。

最后，是时候编写创建新对象的函数了，这里我们需要用到 `PX_ObjectCreateEx` 函数，`PX_ObjectCreateEx` 函数用于创建一个扩展对象，并初始化其属性和回调函数。它的参数说明如下：

- `mp`：指向内存池的指针，用于分配对象所需的内存。
- `Parent`：指向父对象的指针，如果没有父对象则为 `NULL`。
- `x`：对象在 x 轴上的初始位置。
- `y`：对象在 y 轴上的初始位置。
- `z`：对象在 z 轴上的初始位置，z 坐标会影响其渲染的先后顺序。
- `Width`：对象的宽度。
- `Height`：对象的高度。
- `Lenght`：对象的长度，2D 对象，一般可以是 0。
- `type`：对象的类型。
- `Func_ObjectUpdate`：指向对象更新函数的指针。
- `Func_ObjectRender`：指向对象渲染函数的指针。
- `Func_ObjectFree`：指向对象释放函数的指针。
- `desc`：指向对象描述数据的指针。你可以设置为 0，创建时会把这个对象类型的数据填充为 0。
- `size`：描述数据的大小，就是你定义的对象结构体类型的大小，创建对象函数会在内存池申请一段内存空间，并用于存储你的对象结构体。

在创建好一个空对象后，我们使用 `PX_ObjectGetDescIndex` 将对象中的对象结构体指针取出来，这是一个三参数的函数，第一个参数是对象结构体类型，第二个参数则是 `PX_Object *` 指针类型，因为一个 `PX_Object` 可以将多个对象结构体组合在一起，这个组合结构体我们将在之后的教程中会进一步描述，但现在我们只需要知道，调用 `PX_ObjectCreateEx` 函数后，其第一个存储的对象结构体索引是 0 就可以了。

取出结构体指针后，我们对其进行一系列初始化，比如加载图片和初始化旋转角度，最后在 `main` 函数中我们创建这个对象：

```c
#include "PainterEngine.h"
typedef struct
{
    px_texture image;
    px_int rotation;
}PX_Object_MyObject;

PX_OBJECT_UPDATE_FUNCTION(MyObjectUpdate)
{
}

PX_OBJECT_RENDER_FUNCTION(MyObjectRender)
{
    PX_Object_MyObject *pMyObject = PX_ObjectGetDesc(PX_Object_MyObject, pObject);
    PX_TextureRenderEx(psurface, &pMyObject->image, (px_int)pObject->x, (px_int)pObject->y, PX_ALIGN_CENTER, 0, 1, pMyObject->rotation); // 渲染图片
}

PX_OBJECT_FREE_FUNCTION(MyObjectFree)
{
    PX_Object_MyObject *pMyObject = PX_ObjectGetDesc(PX_Object_MyObject, pObject);
    PX_TextureFree(&pMyObject->image); // 释放图片
}

PX_Object* PX_Object_MyObjectCreate(px_memorypool* mp, PX_Object* parent, px_float x, px_float y)
{
    PX_Object *pObject = PX_ObjectCreateEx(mp, parent, x, y, 0, 128, 128, 0, 0, MyObjectUpdate, MyObjectRender, MyObjectFree, 0, sizeof(PX_Object_MyObject)); // 创建一个空的自定义对象
    PX_Object_MyObject* pMyObject = PX_ObjectGetDescIndex(PX_Object_MyObject, pObject, 0); // 取得自定义对象数据
    pMyObject->rotation = 0;
    if (!PX_LoadTextureFromFile(mp, &pMyObject->image, "assets/test.png")) // 加载图片
    {
        PX_ObjectDelete(pObject); // 加载失败则删除对象
        return PX_NULL;
    }
    return pObject;
}

px_int main()
{
    PainterEngine_Initialize(800, 480);
    PX_Object_MyObjectCreate(mp, root, 400, 240); // 创建一个自定义对象
    return PX_TRUE;
}
```

那么它的运行效果是这样的：

![](assets/img/12.1.png)

但现在还没有结束，我们怎么让我们的组件，响应鼠标中键实现旋转呢？还记得我们之前在 [PushButton](#8painterengine-对象传递机制) 中的对象传递机制么？现在，我们也要让我们的组件响应鼠标中键的信息，因此我们给它注册一个 `PX_OBJECT_EVENT_CURSORWHEEL` 事件的回调函数，代码如下：

```c
#include "PainterEngine.h"
typedef struct
{
    px_texture image;
    px_float rotation;
}PX_Object_MyObject;

PX_OBJECT_UPDATE_FUNCTION(MyObjectUpdate)
{
}

PX_OBJECT_RENDER_FUNCTION(MyObjectRender)
{
    PX_Object_MyObject *pMyObject = PX_ObjectGetDesc(PX_Object_MyObject, pObject);
    PX_TextureRenderEx(psurface, &pMyObject->image, (px_int)pObject->x, (px_int)pObject->y, PX_ALIGN_CENTER, 0, 1, pMyObject->rotation); // 渲染图片
}

PX_OBJECT_FREE_FUNCTION(MyObjectFree)
{
    PX_Object_MyObject *pMyObject = PX_ObjectGetDesc(PX_Object_MyObject, pObject);
    PX_TextureFree(&pMyObject->image); // 释放图片
}

PX_OBJECT_EVENT_FUNCTION(MyObjectOnCursorWheel)
{
    PX_Object_MyObject *pMyObject = PX_ObjectGetDescIndex(PX_Object_MyObject, pObject, 0);
    if (PX_ObjectIsCursorInRegion(pObject, e)) // Object 是鼠标位置是否选中当前组件，e 是事件
        pMyObject->rotation += (px_float)PX_Object_Event_GetCursorZ(e)/10;
}

PX_Object* PX_Object_MyObjectCreate(px_memorypool* mp, PX_Object* parent, px_float x, px_float y)
{
    PX_Object *pObject = PX_ObjectCreateEx(mp, parent, x, y, 0, 128, 128, 0, 0, MyObjectUpdate, MyObjectRender, MyObjectFree, 0, sizeof(PX_Object_MyObject)); // 创建一个空的自定义对象
    PX_Object_MyObject* pMyObject = PX_ObjectGetDescIndex(PX_Object_MyObject, pObject, 0); // 取得自定义对象数据
    pMyObject->rotation = 0;
    if (!PX_LoadTextureFromFile(mp, &pMyObject->image, "assets/test.png")) // 加载图片
    {
        PX_ObjectDelete(pObject); // 加载失败则删除对象
        return PX_NULL;
    }
    PX_ObjectRegisterEvent(pObject, PX_OBJECT_EVENT_CURSORWHEEL, MyObjectOnCursorWheel, 0); // 注册鼠标滚轮事件
    return pObject;
}

px_int main()
{
    PainterEngine_Initialize(800, 480);
    PX_Object_MyObjectCreate(mp, root, 400, 240); // 创建一个自定义对象
    return PX_TRUE;
}
```

运行结果如下：

![](assets/img/12.2.gif)

如果你觉得旋转图的质量不好，有很多锯齿，这是因为 `PX_TextureRenderEx` 旋转时是对原图直接采样的。如果你想要高质量的旋转图，你可以用 `PX_TextureRenderRotation` 函数来替换原函数：

```c
PX_OBJECT_RENDER_FUNCTION(MyObjectRender)
{
    PX_Object_MyObject *pMyObject = PX_ObjectGetDesc(PX_Object_MyObject, pObject);
    PX_TextureRenderRotation(psurface, &pMyObject->image, (px_int)pObject->x, (px_int)pObject->y, PX_ALIGN_CENTER, 0, pMyObject->rotation); // 渲染图片
}

```

![](assets/img/12.3.gif)

那么，我们如何实现拖动效果呢？想要做到拖动效果，我们需要在对象结构体中，新增 `float` 类型的变量 `x`、`y`，用来记录当鼠标选中图片时的位置，同时我们加入了 `bool` 类型的变量 `bselect`，表示当前的图标是否被选中。当鼠标点击我们的图标以后，我们就可以监听 `PX_OBJECT_EVENT_CURSORDRAG` 事件，这是鼠标在屏幕上拖动时会产生的事件，我们通过坐标的偏移，移动我们的组件。最后，不论鼠标非拖动时的移动或鼠标左键抬起，都会取消我们组件的选中状态，在对应处理函数中取消选中状态即可。

```c
#include "PainterEngine.h"
typedef struct
{
    px_float last_cursorx, last_cursory;
    px_bool bselect;
    px_texture image;
    px_float rotation;
}PX_Object_MyObject;

PX_OBJECT_UPDATE_FUNCTION(MyObjectUpdate)
{
}

PX_OBJECT_RENDER_FUNCTION(MyObjectRender)
{
    PX_Object_MyObject *pMyObject = PX_ObjectGetDesc(PX_Object_MyObject, pObject);
    PX_TextureRenderRotation(psurface, &pMyObject->image, (px_int)pObject->x, (px_int)pObject->y, PX_ALIGN_CENTER, 0, (px_int)pMyObject->rotation); // 渲染图片
}

PX_OBJECT_FREE_FUNCTION(MyObjectFree)
{
    PX_Object_MyObject *pMyObject = PX_ObjectGetDesc(PX_Object_MyObject, pObject);
    PX_TextureFree(&pMyObject->image); // 释放图片
}

PX_OBJECT_EVENT_FUNCTION(MyObjectOnCursorWheel)
{
    PX_Object_MyObject *pMyObject = PX_ObjectGetDescIndex(PX_Object_MyObject, pObject, 0);
    if (PX_ObjectIsCursorInRegionAlign(pObject, e, PX_ALIGN_CENTER)) // Object 是鼠标位置是否选中当前组件，e 是事件
        pMyObject->rotation += (px_float)PX_Object_Event_GetCursorZ(e)/10;
}

PX_OBJECT_EVENT_FUNCTION(MyObjectOnCursorDown)
{
    PX_Object_MyObject* pMyObject = PX_ObjectGetDescIndex(PX_Object_MyObject, pObject, 0);
    if (PX_ObjectIsCursorInRegionAlign(pObject, e, PX_ALIGN_CENTER)) // Object 是鼠标位置是否选中当前组件，e 是事件
    {
        pMyObject->bselect = PX_TRUE;
        pMyObject->last_cursorx = PX_Object_Event_GetCursorX(e);
        pMyObject->last_cursory = PX_Object_Event_GetCursorY(e);
    }
}

PX_OBJECT_EVENT_FUNCTION(MyObjectOnCursorRelease)
{
    PX_Object_MyObject* pMyObject = PX_ObjectGetDescIndex(PX_Object_MyObject, pObject, 0);
    pMyObject->bselect = PX_FALSE;
}

PX_OBJECT_EVENT_FUNCTION(MyObjectOnCursorDrag)
{
    PX_Object_MyObject* pMyObject = PX_ObjectGetDescIndex(PX_Object_MyObject, pObject, 0);
    if (pMyObject->bselect)
    {
        pObject->x += PX_Object_Event_GetCursorX(e) - pMyObject->last_cursorx;
        pObject->y += PX_Object_Event_GetCursorY(e) - pMyObject->last_cursory;
    }
    pMyObject->last_cursorx = PX_Object_Event_GetCursorX(e);
    pMyObject->last_cursory = PX_Object_Event_GetCursorY(e);
}

PX_Object* PX_Object_MyObjectCreate(px_memorypool* mp, PX_Object* parent, px_float x, px_float y)
{
    PX_Object *pObject = PX_ObjectCreateEx(mp, parent, x, y, 0, 128, 128, 0, 0, MyObjectUpdate, MyObjectRender, MyObjectFree, 0, sizeof(PX_Object_MyObject)); // 创建一个空的自定义对象
    PX_Object_MyObject* pMyObject = PX_ObjectGetDescIndex(PX_Object_MyObject, pObject, 0); // 取得自定义对象数据
    pMyObject->rotation = 0;
    if (!PX_LoadTextureFromFile(mp, &pMyObject->image, "assets/test.png")) // 加载图片
    {
        PX_ObjectDelete(pObject); // 加载失败则删除对象
        return PX_NULL;
    }
    PX_ObjectRegisterEvent(pObject, PX_OBJECT_EVENT_CURSORWHEEL, MyObjectOnCursorWheel, 0); // 注册鼠标滚轮事件
    PX_ObjectRegisterEvent(pObject, PX_OBJECT_EVENT_CURSORDRAG, MyObjectOnCursorDrag, 0); // 注册鼠标拖拽事件
    PX_ObjectRegisterEvent(pObject, PX_OBJECT_EVENT_CURSORDOWN, MyObjectOnCursorDown, 0); // 注册鼠标按下事件
    PX_ObjectRegisterEvent(pObject, PX_OBJECT_EVENT_CURSORUP, MyObjectOnCursorRelease, 0); // 注册鼠标释放事件
    PX_ObjectRegisterEvent(pObject, PX_OBJECT_EVENT_CURSORMOVE, MyObjectOnCursorRelease, 0); // 注册鼠标释放事件
    return pObject;
}

px_int main()
{
    PainterEngine_Initialize(800, 480);
    PX_Object_MyObjectCreate(mp, root, 400, 240); // 创建一个自定义对象
    return PX_TRUE;
}
```
![](assets/img/12.4.gif)

当然，你可以调用 `PX_Object_MyObjectCreate` 多次，创建多个组件对象，它们的功能都是一样的：

![](assets/img/12.5.gif)

## 13. 组合式组件设计

PainterEngine 的组件允许同时拥有多种组件类型，例如，当我们将一个图片框组件和一个按钮进行组合，我们就可以得到一个组合式组件图片按钮。

参考如下代码：

```c
#include "PainterEngine.h"
px_texture tex1, tex2;
PX_Object* image;

PX_OBJECT_EVENT_FUNCTION(ButtonEvent)
{
    PX_Object_Image *pImage = PX_Object_GetImage(pObject); // 取得 Image 对象数据
    PX_Object_Button *pButton = PX_Object_GetButton(pObject); // 取得 Button 对象数据
    if (pImage->pTexture == &tex1)
    {
        PX_Object_ImageSetTexture(pObject, &tex2);
    }
    else
    {
        PX_Object_ImageSetTexture(pObject, &tex1);
    }
}

px_int main()
{
    PainterEngine_Initialize(800, 480);
    if (!PX_LoadTextureFromFile(mp_static, &tex1, "assets/1.png")) return 0; // 加载纹理 1
    if (!PX_LoadTextureFromFile(mp_static, &tex2, "assets/2.png")) return 0; // 加载纹理 2
    image = PX_Object_ImageCreate(mp, root, 300, 140, 200, 200, &tex1); // 创建 Image 对象
    PX_Object_ButtonAttachObject(image, 1, PX_COLOR(64, 255, 255, 255), PX_COLOR(96, 255, 255, 255)); // 将一个 Button 对象类型组合到 Image 对象上
    PX_ObjectRegisterEvent(image, PX_OBJECT_EVENT_EXECUTE, ButtonEvent, 0); // 这里实际上是注册 Button 对象的事件
    return 1;
}
```

我们创建了一个 Image 图像框类型，然后将一个 Button 对象类型组合上去，这样我们就获得了一个图片按钮：

![](assets/img/13.1.gif)

那么，我们如何设计我们自己的可组合对象呢？回到我们的第十二章节，现在，我们就将 "可拖拽" 这个功能设计成一个组合式组件。

首先，仍然是定义一个组件对象结构体，为实现拖拽功能，我们需要鼠标按下时的 x, y 坐标，同时需要一个 bool 类型记录是否是选中状态，然后我们需要注册 `CURSOR` 事件，这些事件在上一章节我们已经写过了，最后，我们用 `PX_ObjectCreateDesc` 函数创建一个对象结构体，并将它 Attach 到我们的对象上。

`PX_ObjectCreateDesc` 是一个对象结构体创建函数，它的定义原型如下：

```c
px_void* PX_ObjectCreateDesc(PX_Object* pObject, px_int idesc, px_int type, Function_ObjectUpdate Func_ObjectUpdate, Function_ObjectRender Func_ObjectRender, Function_ObjectFree Func_ObjectFree, px_void* pDesc, px_int descSize)
```

第一个参数是需要 Attach 的对象，第二个参数是 Attach 到的对象索引。还记得我们之前提到的对象数据索引么，使用 `PX_ObjectCreateEx` 默认使用的是索引 0，因此，如果我们要附加到一个对象上，我们应该选 1，当然如果 1 也被占用了，它就是 2，以此类推。第三个参数是对象类型，我们使用 `PX_ObjectGetDescByType` 时，可以通过对象类型取出对应的指针，然后就是我们熟悉的 `Update`、`Render`、`Free` 三件套了，最后一个参数给出其结构体描述和结构体大小。请参阅下面的代码：

```c
#include "PainterEngine.h"
typedef struct
{
    px_float last_cursorx, last_cursory;
    px_bool bselect;
}PX_Object_Drag;


PX_OBJECT_EVENT_FUNCTION(PX_Object_DragOnCursorDown)
{
    PX_Object_Drag* pPX_Object_Drag = PX_ObjectGetDescByType(pObject, PX_OBJECT_TYPE_DRAG);
    if (PX_ObjectIsCursorInRegionAlign(pObject, e, PX_ALIGN_LEFTTOP))
    {
        pPX_Object_Drag->bselect = PX_TRUE;
        pPX_Object_Drag->last_cursorx = PX_Object_Event_GetCursorX(e);
        pPX_Object_Drag->last_cursory = PX_Object_Event_GetCursorY(e);
    }
}

PX_OBJECT_EVENT_FUNCTION(PX_Object_DragOnCursorRelease)
{
    PX_Object_Drag* pPX_Object_Drag = PX_ObjectGetDescByType(pObject, PX_OBJECT_TYPE_DRAG);
    pPX_Object_Drag->bselect = PX_FALSE;
}

PX_OBJECT_EVENT_FUNCTION(PX_Object_DragOnCursorDrag)
{
    PX_Object_Drag* pPX_Object_Drag = PX_ObjectGetDescByType(pObject, PX_OBJECT_TYPE_DRAG);
    if (pPX_Object_Drag->bselect)
    {
        pObject->x += PX_Object_Event_GetCursorX(e) - pPX_Object_Drag->last_cursorx;
        pObject->y += PX_Object_Event_GetCursorY(e) - pPX_Object_Drag->last_cursory;
    }
    pPX_Object_Drag->last_cursorx = PX_Object_Event_GetCursorX(e);
    pPX_Object_Drag->last_cursory = PX_Object_Event_GetCursorY(e);
}


PX_Object* PX_Object_DragAttachObject(PX_Object* pObject, px_int attachIndex)
{
    PX_Object_Drag* pDesc;


    PX_ASSERTIF(pObject == PX_NULL);
    PX_ASSERTIF(attachIndex < 0 || attachIndex >= PX_COUNTOF(pObject->pObjectDesc));
    PX_ASSERTIF(pObject->pObjectDesc[attachIndex] != PX_NULL);
    pDesc = (PX_Object_Drag*)PX_ObjectCreateDesc(pObject, attachIndex, PX_OBJECT_TYPE_DRAG, 0, 0, 0, 0, sizeof(PX_Object_Drag));
    PX_ASSERTIF(pDesc == PX_NULL);
    PX_ObjectRegisterEvent(pObject, PX_OBJECT_EVENT_CURSORDRAG, PX_Object_DragOnCursorDrag, 0);
    PX_ObjectRegisterEvent(pObject, PX_OBJECT_EVENT_CURSORDOWN, PX_Object_DragOnCursorDown, 0);
    PX_ObjectRegisterEvent(pObject, PX_OBJECT_EVENT_CURSORUP, PX_Object_DragOnCursorRelease, 0);
    PX_ObjectRegisterEvent(pObject, PX_OBJECT_EVENT_CURSORMOVE, PX_Object_DragOnCursorRelease, 0);
    return pObject;
}

px_texture tex1;
PX_Object* image;

px_int main()
{
    PainterEngine_Initialize(800, 480);
    if (!PX_LoadTextureFromFile(mp_static, &tex1, "assets/1.png")) return 0; // 加载纹理 1
    image = PX_Object_ImageCreate(mp, root, 300, 140, 200, 200, &tex1); // 创建 Image 对象
    PX_Object_DragAttachObject(image, 1); // 将一个 Drag 对象类型组合到 Image 对象上
    return 1;
}
```

运行结果如下：

![](assets/img/13.2.gif)


## 14. 粒子系统

PainterEngine 提供了一个粒子系统实现，下面是一个粒子系统的示范程序：

```c
#include "PainterEngine.h"

PX_OBJECT_EVENT_FUNCTION(MyClick)
{
    px_float x = PX_Object_Event_GetCursorX(e);
    px_float y = PX_Object_Event_GetCursorY(e);

    PX_Object_Explosion05Create(mp, root, x, y, 10, 20);
}

px_int main()
{
    PainterEngine_Initialize(800, 600);
    PX_ObjectRegisterEvent(root, PX_OBJECT_EVENT_CURSORDOWN, MyClick, PX_NULL);
    return 0;
}
```

![](assets/img/14.1.gif)

这是一个用组件包装起来的粒子系统实现，另外一种是提供了更加详细的粒子系统参数配置：

```c
#include "PainterEngine.h"
px_texture texture;

int main()
{
    PX_Object* pObject;
    PX_ParticalLauncher_InitializeInfo ParticalInfo;
    PainterEngine_Initialize(600, 400);
    PX_LoadTextureFromFile(mp_static, &texture, "assets/star.traw");

    PX_ParticalLauncherInitializeDefaultInfo(&ParticalInfo);
    ParticalInfo.deviation_rangAngle = 360;
    ParticalInfo.deviation_velocity_max = 50;
    ParticalInfo.deviation_velocity_min = -50;
    ParticalInfo.direction = PX_POINT(0, -1, 0);
    ParticalInfo.generateDuration = 100;
    ParticalInfo.launchCount = -1;
    ParticalInfo.maxCount = 100;
    ParticalInfo.position = PX_POINT(0, 0, 0);
    ParticalInfo.tex = &texture;
    ParticalInfo.velocity = 100;
    ParticalInfo.alive = 5000;
    ParticalInfo.rotation = 180;
    ParticalInfo.deviation_rotation = 180;
    ParticalInfo.atomsize = 0.7f;
    ParticalInfo.deviation_atomsize_max = 0.7f;
    ParticalInfo.deviation_atomsize_min = -0.5f;
    ParticalInfo.alpha = 0.8f;
    ParticalInfo.deviation_alpha = 0.3f;
    ParticalInfo.deviation_hdrR = 0.5f;
    ParticalInfo.deviation_hdrG = 0.5f;
    ParticalInfo.deviation_hdrB = 0.5f;
    ParticalInfo.alphaincrease = -0.2f;

    pObject = PX_Object_ParticalCreate(mp, root, 300, 200, ParticalInfo);
    return 0;
}
```

以下是这段代码的主要功能和流程解释：

1. `#include "PainterEngine.h"`：引入 PainterEngine 的头文件，以便使用引擎的功能。

2. `px_texture texture;`：声明一个名为 `texture` 的变量，用于存储纹理信息。

3. `int main()`：主函数的入口点。

4. `PX_Object* pObject;`：声明一个名为 `pObject` 的指向 `PX_Object` 类型的指针，将用于创建粒子系统对象。

5. `PX_ParticalLauncher_InitializeInfo ParticalInfo;`：声明一个名为 `ParticalInfo` 的结构体变量，用于配置粒子发射器的初始化信息。

6. `PainterEngine_Initialize(600, 400);`：初始化 PainterEngine，设置窗口的宽度为 600 像素，高度为 400 像素。

7. `PX_LoadTextureFromFile(mp_static, &texture, "assets/star.traw");`：从文件加载纹理，将纹理数据存储在 `texture` 变量中。纹理文件路径为 "assets/star.traw"。

8. `PX_ParticalLauncherInitializeDefaultInfo(&ParticalInfo);`：初始化 `ParticalInfo` 结构体，设置了一些默认的粒子发射器属性。

9. 针对 `ParticalInfo` 的各个属性进行了具体的配置，包括粒子的位置、速度、寿命、大小、旋转等。这些属性决定了粒子的外观和行为。

10. `pObject = PX_Object_ParticalCreate(mp, root, 300, 200, ParticalInfo);`：使用配置好的 `ParticalInfo` 创建一个粒子系统对象，并将其存储在 `pObject` 中。这个粒子系统对象将会在窗口中的位置 (300, 200) 处发射粒子。

其中 `PX_ParticalLauncher_InitializeInfo` 用于配置粒子发射器的初始化信息，即在创建粒子系统时，可以通过填充这个结构体来指定粒子系统的各种属性和行为。以下是该结构体的各个成员的说明：

1. `px_void *userptr;`：一个指向任意类型数据的指针，可用于存储用户自定义的数据。

2. `px_texture *tex;`：指向纹理数据的指针，用于指定粒子的纹理图像。

3. `px_point position;`：一个包含 x、y、z 坐标的点，表示粒子系统的初始位置。

4. `px_float deviation_position_distanceRange;`：一个浮点数，用于指定粒子的位置偏移范围。

5. `px_point direction;`：一个包含 x、y、z 坐标的点，表示粒子的初始运动方向。

6. `px_float deviation_rangAngle;`：一个浮点数，用于指定粒子的初始运动方向偏移范围（角度）。

7. `px_float velocity;`：一个浮点数，表示粒子的初始速度。

8. `px_float deviation_velocity_max;`：一个浮点数，表示粒子速度的最大偏移值。

9. `px_float deviation_velocity_min;`：一个浮点数，表示粒子速度的最小偏移值。

10. `px_float atomsize;`：一个浮点数，表示粒子的初始大小。

11. `px_float deviation_atomsize_max;`：一个浮点数，表示粒子大小的最大偏移值。

12. `px_float deviation_atomsize_min;`：一个浮点数，表示粒子大小的最小偏移值。

13. `px_float rotation;`：一个浮点数，表示粒子的初始旋转角度。

14. `px_float deviation_rotation;`：一个浮点数，表示粒子旋转角度的偏移范围。

15. `px_float alpha;`：一个浮点数，表示粒子的初始透明度。

16. `px_float deviation_alpha;`：一个浮点数，表示粒子透明度的偏移范围。

17. `px_float hdrR;`：一个浮点数，表示粒子的初始红色通道值。

18. `px_float deviation_hdrR;`：一个浮点数，表示粒子红色通道值的偏移范围。

19. `px_float hdrG;`：一个浮点数，表示粒子的初始绿色通道值。

20. `px_float deviation_hdrG;`：一个浮点数，表示粒子绿色通道值的偏移范围。

21. `px_float hdrB;`：一个浮点数，表示粒子的初始蓝色通道值。

22. `px_float deviation_hdrB;`：一个浮点数，表示粒子蓝色通道值的偏移范围。

23. `px_float sizeincrease;`：一个浮点数，表示粒子大小的增加率。

24. `px_float alphaincrease;`：一个浮点数，表示粒子透明度的增加率。

25. `px_point a;`：一个包含 x、y、z 坐标的点，用于自定义属性。

26. `px_float ak;`：一个浮点数，用于自定义属性。

27. `px_int alive;`：一个整数，表示粒子的生存时间（毫秒）。

28. `px_int generateDuration;`：一个整数，表示粒子发射器的生成周期（毫秒）。

29. `px_int maxCount;`：一个整数，表示粒子系统中最大的粒子数量。

30. `px_int launchCount;`：一个整数，表示粒子系统的发射次数。

31. `PX_ParticalLauncher_CreateAtom Create_func;`：一个函数指针，用于指定自定义的粒子创建函数。

32. `PX_ParticalLauncher_UpdateAtom Update_func;`：一个函数指针，用于指定自定义的粒子更新函数。

这个结构体允许你灵活地配置粒子系统的各种属性，以满足不同场景和效果的需求。通过调整这些属性，你可以控制粒子的外观、运动轨迹、生命周期等方面的行为。

![](assets/img/14.2.gif)

## 15. 使用 PainterEngine 播放音乐

PainterEngine 内置了对 wav 及 mp3 格式音乐的原生支持，使用 PainterEngine 播放音乐的代码十分简单：

```c
#include "PainterEngine.h"

PX_SoundData sounddata; // 定义音乐格式
int main()
{
    PX_Object* pObject;
    PainterEngine_Initialize(600, 400);
    PainterEngine_InitializeAudio(); // 初始化混音器及音乐设备
    if (!PX_LoadSoundFromFile(mp_static, &sounddata, "assets/bliss.wav")) return PX_FALSE; // 加载音乐，支持 wav 及 mp3 格式
    PX_SoundPlayAdd(soundplay, PX_SoundCreate(&sounddata, PX_TRUE)); // 播放音乐
    pObject = PX_Object_SoundViewCreate(mp, root, 0, 0, 600, 400, soundplay); // 音乐频谱可视化组件，可选
    return 0;
}
```

![](assets/img/15.1.gif)

其中，`PX_LoadSoundFromFile` 函数从文件中加载音乐，并解码成 `sounddata` 类型。`PX_SoundCreate` 可以用 `sounddata` 创建一个播放实例，第二个参数表示这个实例是否循环播放，最后使用 `PX_SoundPlayAdd` 将播放实例送入混音器中，即可完成音乐播放。

## 16. PainterEngine Live2D 动画系统

PainterEngine 内置了一个类 live2D 动画系统，可以加载 live2d 动画，参考代码如下：

```c
#include "PainterEngine.h"

PX_LiveFramework liveframework;
PX_Object* pObject;

PX_OBJECT_EVENT_FUNCTION(onClick)
{
    PX_Object_Live2DPlayAnimationRandom(pObject);
}

int main()
{
    
    PX_IO_Data iodata;
    PainterEngine_Initialize(600, 600);
    // 加载模型数据
    iodata = PX_LoadFileToIOData("assets/release.live");
    if (iodata.size == 0) return PX_FALSE;
    PX_LiveFrameworkImport(mp_static, &liveframework, iodata.buffer, iodata.size);
    PX_FreeIOData(&iodata);
    // 创建 Live2D 对象
    pObject = PX_Object_Live2DCreate(mp, root, 300, 300, &liveframework);
    PX_ObjectRegisterEvent(pObject, PX_OBJECT_EVENT_CURSORDOWN, onClick, PX_NULL);

    return 0;
}

```

以下是与 Live2D 模型预览器相关的函数的说明：

`PX_Object_Live2DCreate`

```c
PX_Object* PX_Object_Live2DCreate(px_memorypool* mp, PX_Object* Parent, px_int x, px_int y, PX_LiveFramework *pLiveFramework);
```

- **描述**：创建一个 Live2D 模型预览器对象，用于在图形界面中显示和交互 Live2D 模型。
- **参数**：
  - `mp`：内存池指针，用于分配内存。
  - `Parent`：父对象，Live2D 模型预览器对象将作为其子对象。
  - `x`, `y`：Live2D 模型预览器对象的位置坐标。
  - `pLiveFramework`：Live2D 模型框架的指针，包括模型数据、纹理等信息。
- **返回值**：创建的 Live2D 模型预览器对象的指针。

`PX_Object_Live2DPlayAnimation`

```c
px_void PX_Object_Live2DPlayAnimation(PX_Object *pObject, px_char *name);
```

- **描述**：播放指定名称的 Live2D 模型动画。
- **参数**：
  - `pObject`：Live2D 模型预览器对象的指针。
  - `name`：动画名称。
- **返回值**：无。

`PX_Object_Live2DPlayAnimationRandom`

```c
px_void PX_Object_Live2DPlayAnimationRandom(PX_Object* pObject);
```

- **描述**：随机播放 Live2D 模型的动画。
- **参数**：
  - `pObject`：Live2D 模型预览器对象的指针。
- **返回值**：无。

`PX_Object_Live2DPlayAnimationIndex`

```c
px_void PX_Object_Live2DPlayAnimationIndex(PX_Object* pObject, px_int index);
```

- **描述**：播放 Live2D 模型的指定索引处的动画。
- **参数**：
  - `pObject`：Live2D 模型预览器对象的指针。
  - `index`：动画的索引。
- **返回值**：无。

这些函数用于创建、配置和管理 Live2D 模型预览器对象，以在图形用户界面中显示和交互 Live2D 模型。可以使用这些函数播放 Live2D 模型的动画，包括指定名称、随机选择和指定索引处的动画。

![](assets/img/16.1.gif)

## 17. PainterEngine 脚本引擎

PainterEngine 内置了一个平台无关的脚本引擎系统，集成了编译，运行，调试等功能，你可以很轻松地在脚本之上，实现并行调度功能。PainterEngine Script 的设计，最大程度和 C 语言保持一致性，并对一些类型进行的拓展和简化。

例如在脚本中，支持 `int`、`float`、`string`、`memory` 四种类型，`int` 类型是一个 32 位的有符号整数，`float` 是一个浮点数类型，这个和 C 语言的类型保持了一致。`string` 类型类似于 C++ 的 `string`，它允许直接用 `+` 法运算符进行字符串拼接，使用 `strlen` 来获取其字符串长度，而 `memory` 是一个二进制数据存储类型，同样支持 `+` 运算进行拼接。

在脚本中如果需要调用 C 语言函数，应该使用 `PX_VM_HOST_FUNCTION` 宏进行定义声明。和组件回调函数一样，`PX_VM_HOST_FUNCTION` 的定义如下：

```c
#define PX_VM_HOST_FUNCTION(name) px_bool name(PX_VM *Ins, px_void *userptr)
```
在下面的内容中，我将以一个简单的脚本实例作为范例，为你演示如何使用 PainterEngine 的脚本引擎：

```c
const px_char shellcode[] = "\
#name \"main\"\n\
host void print(string s);\n\
host void sleep(int ms);\n\
int main()\n\
{\n\
 int i, j;\n\
 for(i = 1; i <= 9; i++)\n\
 {\n\
  for(j = 1; j <= i; j++)\n\
   {\n\
     print(string(i) + \" * \" + string(j) + \" = \" + string(i * j));\n\
     sleep(1000);\n\
   }\n\
 }\n\
}";

PX_VM_HOST_FUNCTION(host_print)
{
    if (PX_VM_HOSTPARAM(Ins, 0).type == PX_VARIABLE_TYPE_STRING)
    {
        PainterEngine_Print(PX_VM_HOSTPARAM(Ins, 0)._string.buffer);
    }
    return PX_TRUE;
}

PX_VM_HOST_FUNCTION(host_sleep)
{
    if (PX_VM_HOSTPARAM(Ins, 0).type == PX_VARIABLE_TYPE_INT)
    {
        PX_VM_Sleep(Ins, PX_VM_HOSTPARAM(Ins, 0)._int);
    }
    return PX_TRUE;
}

```

首先，`shellcode` 数组中存储着一个输出九九乘法表的程序，其中需要调用两个 `host` 函数（脚本调用 C 语言函数称为 host call，因此 host 函数实际就是专门提供给脚本调用的 C 语言函数），一个是 `print` 函数，一个是 `sleep` 函数。因此在下面，我们定义了两个 `host` 函数，`PX_VM_HOSTPARAM` 用于取得脚本传递过来的参数。在这里，我们需要判断传递过来的参数类型是否符合我们的调用规则，像 `host_print` 函数，作用是在 PainterEngine 中输出字符串，而 `sleep` 函数，则是用来延迟一段时间。

现在，PainterEngine Script 是一个编译型脚本，我们需要将上面的代码编译成二进制形式，然后将它送入虚拟机中运行，观察以下代码：

```c
PX_VM vm;
PX_OBJECT_UPDATE_FUNCTION(VMUpdate)
{
    PX_VMRun(&vm, 0xffff, elapsed); // 运行虚拟机
}

px_int main()
{
    PX_Compiler compiler;
    px_memory bin;
    PainterEngine_Initialize(800, 600);
    PainterEngine_SetBackgroundColor(PX_COLOR_BLACK);
    PX_CompilerInitialize(mp, &compiler); // 初始化编译器
    PX_CompilerAddSource(&compiler, shellcode); // 编译器中添加代码
    PX_MemoryInitialize(mp, &bin); // 初始化内存/用于存储编译后的结果
    if (!PX_CompilerCompile(&compiler, &bin, 0, "main"))
    {
        // 编译失败
        return 0;
    }
    PX_CompilerFree(&compiler); // 释放编译器
    PX_VMInitialize(&vm, mp, bin.buffer, bin.usedsize); // 初始化虚拟机
    PX_VMRegisterHostFunction(&vm, "print", host_print, 0); // 注册主机函数 print
    PX_VMRegisterHostFunction(&vm, "sleep", host_sleep, 0); // 注册主机函数 sleep
    PX_VMBeginThreadFunction(&vm, 0, "main", PX_NULL, 0); // 开始运行虚拟机函数
    PX_ObjectSetUpdateFunction(root, VMUpdate, 0); // 设置更新函数

    return 0;
}
```

首先我们用 `PX_Compiler` 编译我们的脚本，然后我们注册我们的 host call，`PX_VMBeginThreadFunction` 的功能是 C 语言调用脚本语言中函数，在这里我们调用脚本中的 `main` 开始运行我们的脚本函数，最后我们将一个 `Update` 函数绑定到 root 节点，以循环更新虚拟机，来执行脚本。

最后，看看运行的结果。

![](assets/img/17.1.gif)

如果我们想要对脚本进行调试，我们还可以在编译期间，创建一个符号映射表，这样我们就可以直接使用 `PX_Object_DebuggerMap` 对脚本进行调试。

```c
px_int main()
{
    PX_Compiler compiler;
    px_memory bin;
    PainterEngine_Initialize(800, 480);
    PX_VMDebuggerMapInitialize(mp, &debugmap);
    PainterEngine_SetBackgroundColor(PX_COLOR_BLACK);
    PX_CompilerInitialize(mp, &compiler); // 初始化编译器
    PX_CompilerAddSource(&compiler, shellcode); // 编译器中添加代码
    PX_MemoryInitialize(mp, &bin); // 初始化内存/用于存储编译后的结果
    if (!PX_CompilerCompile(&compiler, &bin, &debugmap, "main"))
    {
        // 编译失败
        return 0;
    }
    PX_CompilerFree(&compiler); // 释放编译器
    PX_VMInitialize(&vm, mp, bin.buffer, bin.usedsize); // 初始化虚拟机
    PX_VMRegisterHostFunction(&vm, "print", host_print, 0); // 注册主机函数 print
    PX_VMRegisterHostFunction(&vm, "sleep", host_sleep, 0); // 注册主机函数 sleep
    PX_VMBeginThreadFunction(&vm, 0, "main", PX_NULL, 0); // 开始运行虚拟机函数
    PX_Object *pDbgObject = PX_Object_AsmDebuggerCreate(mp, root, 0, 0, 800, 480, 0);
    pDbgObject->Visible = PX_TRUE;
    PX_Object_AsmDebuggerAttach(pDbgObject, &debugmap, &vm);
    return 0;
}
```

![](assets/img/17.2.png)

## 18. 使用 PainterEngine 快速创作一个小游戏

为了更好地演示 PainterEngine 的使用，我将用 PainterEngine 创作一个简单的小游戏，你可以在 documents/demo/game 下找到有关这个游戏的所有源码及原始素材。得益于 PainterEngine 的全平台可移植性，你也可以在 [PainterEngine 在线应用 APP——打地鼠](https://www.painterengine.com/main/app/documentgame/) 中，直接玩到这个在线小游戏。

在这个小游戏中，我将充分为你展示，如何使用 PainterEngine 的组件化开发模式，快速创建一个 App Game。

让我们先开始游戏创作的第一步，我们先准备好所需的美术资源及素材：

![](assets/img/18.1.png)

这是一个简单的游戏背景素材，然后我们就可以开始创建我们的 `main.c` 源代码文件，在 PainterEngine 中我们输入下面的代码：

```c
px_int main()
{
    px_int i;
    PainterEngine_Initialize(800, 480);
    PX_FontModuleInitialize(mp_static, &score_fm);
    PX_FontModuleSetCodepage(&score_fm, PX_FONTMODULE_CODEPAGE_GBK);
    if (!PX_LoadTextureToResource(PainterEngine_GetResourceLibrary(), "assets/rasing.png", "fox_rasing")) return 0;
    if (!PX_LoadTextureToResource(PainterEngine_GetResourceLibrary(), "assets/taunt.png", "fox_taunt")) return 0;
    if (!PX_LoadTextureToResource(PainterEngine_GetResourceLibrary(), "assets/escape.png", "fox_escape")) return 0;
    if (!PX_LoadTextureToResource(PainterEngine_GetResourceLibrary(), "assets/beat.png", "fox_beat")) return 0;
    if (!PX_LoadTextureToResource(PainterEngine_GetResourceLibrary(), "assets/hurt.png", "fox_hurt")) return 0;
    if (!PX_LoadTextureToResource(PainterEngine_GetResourceLibrary(), "assets/mask.png", "fox_mask")) return 0;
    if (!PX_LoadTextureToResource(PainterEngine_GetResourceLibrary(), "assets/background.png", "background")) return 0;
    if (!PX_LoadAnimationToResource(PainterEngine_GetResourceLibrary(), "assets/song.2dx", "song")) return 0;
    PainterEngine_SetBackgroundTexture(PX_ResourceLibraryGetTexture(PainterEngine_GetResourceLibrary(), "background"));
    for (i = 0; i <= 9; i++)
    {
        px_texture tex;
        px_char path[64];
        PX_sprintf1(path, 64, "assets/%1.png", PX_STRINGFORMAT_INT(i));
        if (PX_LoadTextureFromFile(mp, &tex, path))
        {
            PX_FontModuleAddNewTextureCharacter(&score_fm, '0' + i, &tex);
        }
        PX_TextureFree(&tex);
    }
}
```

在代码的开始阶段，我们初始化了一个 800x480 的窗口，然后我们初始化了字模，并用 `PX_FontModuleSetCodepage` 函数设置了其为 GBK 字符集，再后面，我们就是把资源加载进 PainterEngine 的资源管理器中了。

### 加载资源及设置背景

PainterEngine 内置了一个资源管理器，它在 `PainterEngine_Initialize` 中就被初始化了，使用的是 `mp_static` 内存池。资源管理器的作用是像数据库一样，将图片、音频、脚本等等素材加载到内存中，并将它映射为一个 `key`，之后对资源的访问都是通过 `key` 进行的。资源管理器的映射做了专门的优化，因此你不必太担心映射查询带来的性能损耗问题。

`PX_LoadTextureToResource` 函数用于将一个文件系统的资源加载到资源管理器中，第一个参数是这个资源管理器的实例指针，PainterEngine 在初始化阶段会默认创建一个这样的管理器实例，因此你可以直接用 `PainterEngine_GetResourceLibrary` 获得它。第二个参数是需要加载文件的所在路径，第三个参数则是我们想映射的 `key` 了。

在代码的下一步，我们使用 `PX_LoadTextureToResource` 加载了若干图片，`PX_LoadAnimationToResource` 加载了一个 2dx 动画（请到应用市场查看 2DX 动画详细说明）。最后，在游戏里我们并没有使用 TTF 字模文件，我们循环加载了 `0.png` 到 `9.png`，并将这些纹理作为图片插入到字模中，这样这个字模绘制数字时，实际显示的就是我们的图片。

同时我们还调用了 `PainterEngine_SetBackgroundTexture` 设置 PainterEngine 界面的背景，请注意 `PX_ResourceLibraryGetTexture` 函数，它的作用是使用一个查询 `key`，从资源管理器中取得这个图片的数据结构指针。以上完成后你将可以看到这样的界面：

![](assets/img/18.2.png)

### 设计游戏对象

我们先来设计第一个游戏对象，就是 `开始游戏按钮`。这一部分我们并不要写太多的代码，因为 PainterEngine 内置就有这种按钮的功能：

```c
startgame = PX_Object_PushButtonCreate(mp, root, 300, 200, 200, 90, "Start Game", 0);
startgame->Visible = PX_TRUE;
PX_Object_PushButtonSetBackgroundColor(startgame, PX_COLOR(96, 255, 255, 255));
PX_Object_PushButtonSetPushColor(startgame, PX_COLOR(224, 255, 255, 255));
PX_Object_PushButtonSetCursorColor(startgame, PX_COLOR(168, 255, 255, 255));
```

我们使用了一系列函数，改变了按钮的背景颜色、鼠标悬停颜色和鼠标按下的颜色，因此你可以看到这样的情况：

![](assets/img/18.3.png)

然后我们需要创建我们的游戏里的地鼠对象，这是游戏里最复杂的对象，我贴上详细代码，以逐步解释它们：

```c
typedef enum
{
    PX_OBJECT_FOX_STATE_IDLE,   // 狐狸还在洞里
    PX_OBJECT_FOX_STATE_RASING, // 狐狸正在升起
    PX_OBJECT_FOX_STATE_TAUNT,  // 狐狸在嘲讽
    PX_OBJECT_FOX_STATE_ESCAPE, // 狐狸逃跑
    PX_OBJECT_FOX_STATE_BEAT,   // 狐狸被打
    PX_OBJECT_FOX_STATE_HURT,   // 狐狸受伤后逃跑
}PX_OBJECT_FOX_STATE;

typedef struct
{
    PX_OBJECT_FOX_STATE state;            // 狐狸状态
    px_dword elapsed;                     // 状态持续时间
    px_float texture_render_offset;       // 纹理渲染偏移
    px_dword gen_rand_time;               // 生成随机时间
    px_float rasing_down_speed;           // 升起速度
    px_texture render_target;             // 渲染目标
    px_texture* pcurrent_display_texture; // 当前显示的纹理
    px_texture* ptexture_mask;            // 遮罩
}PX_Object_Fox;

PX_OBJECT_UPDATE_FUNCTION(PX_Object_FoxOnUpdate)
{
    PX_Object_Fox* pfox = PX_ObjectGetDescByType(pObject, PX_OBJECT_TYPE_FOX);
    switch (pfox->state)
    {
        case PX_OBJECT_FOX_STATE_IDLE:
        {
            if (pfox->gen_rand_time == 0)
            {
                pfox->gen_rand_time = PX_rand() % 3000 + 1000; // 狐狸在洞里的时间，时间到了就升起来
            }
            else
            {
                if (pfox->gen_rand_time <elapsed) // 时间到了
                {
                    // 升起
                    pfox->state = PX_OBJECT_FOX_STATE_RASING;
                    pfox->elapsed = 0;
                    pfox->gen_rand_time = 0;
                    pfox->texture_render_offset = pObject->Height;
                    // 改变纹理
                    pfox->pcurrent_display_texture = PX_ResourceLibraryGetTexture(PainterEngine_GetResourceLibrary(), "fox_rasing");
                }
                else
                {
                    pfox->gen_rand_time -= elapsed;
                }
            }
        }
        break;
        case PX_OBJECT_FOX_STATE_RASING: // 狐狸升起
        {
            pfox->elapsed += elapsed;
            // 升起纹理偏移量
            pfox->texture_render_offset -= pfox->rasing_down_speed * elapsed / 1000;
            if (pfox->texture_render_offset <= 0)
            {
                pfox->texture_render_offset = 0;
                pfox->state = PX_OBJECT_FOX_STATE_TAUNT; // 升起后嘲讽
                pfox->elapsed = 0;
            }
        }
        break;
        case PX_OBJECT_FOX_STATE_TAUNT: // 狐狸嘲讽
        {
            pfox->elapsed += elapsed;
            if (pfox->elapsed>600&& pfox->elapsed <1500) // 嘲讽时间
            {
                pfox->pcurrent_display_texture = PX_ResourceLibraryGetTexture(PainterEngine_GetResourceLibrary(), "fox_taunt"); // 嘲讽纹理
            }
            else if (pfox->elapsed>1500) // 嘲讽结束
            {
                pfox->texture_render_offset = 0;
                pfox->state = PX_OBJECT_FOX_STATE_ESCAPE; // 逃跑
                pfox->pcurrent_display_texture = PX_ResourceLibraryGetTexture(PainterEngine_GetResourceLibrary(), "fox_escape"); // 逃跑纹理
                pfox->elapsed = 0;
            }
        }
        break;
        case PX_OBJECT_FOX_STATE_BEAT: // 狐狸被打
        {
            pfox->elapsed += elapsed;
            if (pfox->elapsed>800)
            {
                pfox->pcurrent_display_texture = PX_ResourceLibraryGetTexture(PainterEngine_GetResourceLibrary(), "fox_hurt"); // 受伤纹理
                pfox->state = PX_OBJECT_FOX_STATE_ESCAPE; // 逃跑
            }
        }
        break;
        case PX_OBJECT_FOX_STATE_ESCAPE:
        {
            pfox->elapsed += elapsed;
            pfox->texture_render_offset += pfox->rasing_down_speed * elapsed / 1000;
            if (pfox->texture_render_offset >= pObject->Height)
            {
                pfox->texture_render_offset = pObject->Height;
                pfox->state = PX_OBJECT_FOX_STATE_IDLE; // 逃跑结束
                pfox->elapsed = 0; // 重置时间
                pfox->pcurrent_display_texture = PX_NULL;
            }
        }
        break;
    default:
        break;
    }
}

PX_OBJECT_RENDER_FUNCTION(PX_Object_FoxOnRender)
{
    PX_Object_Fox* pfox = PX_ObjectGetDescByType(pObject, PX_OBJECT_TYPE_FOX);
    px_float x, y, width, height;
    PX_OBJECT_INHERIT_CODE(pObject, x, y, width, height);
    PX_TextureClearAll(&pfox->render_target, PX_COLOR_NONE); // 清空渲染目标
    if (pfox->pcurrent_display_texture)
    {
        PX_TextureRender(&pfox->render_target, pfox->pcurrent_display_texture, (px_int)pfox->render_target.width/2, (px_int)pfox->texture_render_offset, PX_ALIGN_MIDTOP, PX_NULL); // 渲染狐狸
    }
    PX_TextureRenderMask(psurface, pfox->ptexture_mask, &pfox->render_target, (px_int)x, (px_int)y, PX_ALIGN_MIDBOTTOM, PX_NULL); // 以遮罩形式绘制纹理
}


PX_OBJECT_FREE_FUNCTION(PX_Object_FoxFree)
{
    PX_Object_Fox* pfox = PX_ObjectGetDescByType(pObject, PX_OBJECT_TYPE_FOX);
    PX_TextureFree(&pfox->render_target);
}

PX_OBJECT_EVENT_FUNCTION(PX_Object_FoxOnClick) // 狐狸被点击
{
    PX_Object_Fox* pfox = PX_ObjectGetDescByType(pObject, PX_OBJECT_TYPE_FOX);
    if (pfox->state == PX_OBJECT_FOX_STATE_TAUNT|| pfox->state == PX_OBJECT_FOX_STATE_RASING) // 狐狸嘲讽或者升起时点击有效
    {
        if (PX_ObjectIsCursorInRegionAlign(pObject, e, PX_ALIGN_MIDBOTTOM)) // 点击有效区域
        {
            px_int x = (px_int)PX_Object_Event_GetCursorX(e);
            px_int y = (px_int)PX_Object_Event_GetCursorY(e);
            x = (px_int)(x - (pObject->x - pObject->Width/2));
            y = (px_int)(y - (pObject->y - pObject->Height));
            if (x>32&&y<128)
            {
                pfox->pcurrent_display_texture = PX_ResourceLibraryGetTexture(PainterEngine_GetResourceLibrary(), "fox_beat");
                pfox->state = PX_OBJECT_FOX_STATE_BEAT;
                pfox->elapsed = 0;
                PX_Object_ScorePanelAddScore(scorePanel, 100);
            }
        }
    }
}

PX_OBJECT_EVENT_FUNCTION(PX_Object_FoxOnReset)
{
    PX_Object_Fox* pfox = PX_ObjectGetDescByType(pObject, PX_OBJECT_TYPE_FOX);
    pfox->state = PX_OBJECT_FOX_STATE_IDLE;
    pfox->elapsed = 0;
    pfox->texture_render_offset = pObject->Height;
    pfox->gen_rand_time = 0;
    pfox->pcurrent_display_texture = PX_NULL;

}

PX_Object *PX_Object_FoxCreate(px_memorypool *mp, PX_Object *parent, px_float x, px_float y)
{
    PX_Object_Fox* pfox;
    px_texture *ptexture = PX_ResourceLibraryGetTexture(PainterEngine_GetResourceLibrary(), "fox_rasing"); // 从资源管理器中获取纹理
    PX_Object* pObject = PX_ObjectCreateEx(mp, parent, x, y, 0, ptexture->width*1.f, ptexture->height*1.f, 0, PX_OBJECT_TYPE_FOX, PX_Object_FoxOnUpdate, PX_Object_FoxOnRender, PX_Object_FoxFree, 0, sizeof(PX_Object_Fox));
    pfox = PX_ObjectGetDescByType(pObject, PX_OBJECT_TYPE_FOX);
    pfox->state = PX_OBJECT_FOX_STATE_IDLE; // 狐狸状态
    pfox->rasing_down_speed = 512; // 升起速度
    pfox->ptexture_mask = PX_ResourceLibraryGetTexture(PainterEngine_GetResourceLibrary(), "fox_mask"); // 遮罩
    if (!PX_TextureCreate(mp, &pfox->render_target, ptexture->width, ptexture->height))
    {
        PX_ObjectDelete(pObject);
        return 0;
    }
    PX_ObjectRegisterEvent(pObject, PX_OBJECT_EVENT_CURSORDOWN, PX_Object_FoxOnClick, 0); // 注册点击事件
    PX_ObjectRegisterEvent(pObject, PX_OBJECT_EVENT_RESET, PX_Object_FoxOnReset, 0); // 注册重置事件
    return pObject;
}

```

- 首先是 `PX_Object_FoxOnUpdate`，这是对象三件套中的 `update` 函数，在这个函数中，我们判断当前这个 `地鼠` 的状态，到底是升起、嘲讽，还是缩回去。
- 然后是 `PX_Object_FoxOnRender`，这是执行 `render` 的函数，我们通过偏移量把纹理绘制出来，当然在这里我们调用了 `PX_TextureRenderMask` 函数，这是一个带纹理遮罩的绘制函数。
- `PX_Object_FoxFree` 函数中，主要是对临时渲染表面的释放处理，虽然在本项目中并没有用到。
- `PX_Object_FoxOnClick` 函数，表示当前的地鼠被击打了，其中是一些命中范围的判断，如果被击中了，应该把状态设置为受伤。
- `PX_Object_FoxOnReset` 用于执行复位，即游戏结束后，所有地鼠都应该是重置状态，这是一个 `PX_OBJECT_EVENT_RESET` 的回调，你可以在 `PX_Object_FoxCreate` 中找到它。
- 最后是 `PX_Object_FoxCreate` 函数，在这个函数中我们做了一些初始化工作，为 `地鼠` 注册了事件回调，最终完成这个组件的开发设计。


![](assets/img/18.4.gif)


然后，我们需要创建一个 `锤子` 对象来改变我们鼠标的样式。锤子对象的设计很简单，它只有 2 个纹理，一个是鼠标没有按下时的状态，一个是按下时的状态。不同的状态对应不同的纹理：

```c
typedef struct
{
    px_texture ham01; // 锤子纹理 1，没有按下
    px_texture ham02; // 锤子纹理 2，按下
    px_bool bHit; // 是否按下
}PX_Object_Hammer;

PX_OBJECT_RENDER_FUNCTION(PX_Object_HammerRender) // 锤子渲染
{
    PX_Object_Hammer* phammer = PX_ObjectGetDescByType(pObject, PX_OBJECT_TYPE_HAMMER);
    px_float x, y, width, height;
    PX_OBJECT_INHERIT_CODE(pObject, x, y, width, height);
    if (phammer->bHit)
    {
        PX_TextureRender(psurface, &phammer->ham02, (px_int)x, (px_int)y, PX_ALIGN_CENTER, PX_NULL); // 按下
    }
    else
    {
        PX_TextureRender(psurface, &phammer->ham01, (px_int)x, (px_int)y, PX_ALIGN_CENTER, PX_NULL); // 未按下
    }
    
}

PX_OBJECT_FREE_FUNCTION(PX_Object_HammerFree)
{
    PX_Object_Hammer* phammer = PX_ObjectGetDescByType(pObject, PX_OBJECT_TYPE_HAMMER);
    PX_TextureFree(&phammer->ham01);
    PX_TextureFree(&phammer->ham02);
}

PX_OBJECT_EVENT_FUNCTION(PX_Object_HammerOnMove)
{
    pObject->x = PX_Object_Event_GetCursorX(e); // 锤子跟随鼠标移动
    pObject->y = PX_Object_Event_GetCursorY(e);
}

PX_OBJECT_EVENT_FUNCTION(PX_Object_HammerOnCursorDown)
{
    PX_Object_Hammer* phammer = PX_ObjectGetDescByType(pObject, PX_OBJECT_TYPE_HAMMER);
    phammer->bHit = PX_TRUE; // 按下
}

PX_OBJECT_EVENT_FUNCTION(PX_Object_HammerOnCursorUp)
{
    PX_Object_Hammer* phammer = PX_ObjectGetDescByType(pObject, PX_OBJECT_TYPE_HAMMER);
    phammer->bHit = PX_FALSE; // 抬起
}

PX_Object* PX_Object_HammerCreate(px_memorypool* mp, PX_Object* parent)
{
    PX_Object_Hammer* phammer;
    PX_Object* pObject = PX_ObjectCreateEx(mp, parent, 0, 0, 0, 0, 0, 0, PX_OBJECT_TYPE_HAMMER, 0, PX_Object_HammerRender, PX_Object_HammerFree, 0, sizeof(PX_Object_Hammer));
    phammer = PX_ObjectGetDescByType(pObject, PX_OBJECT_TYPE_HAMMER);
    phammer->bHit = PX_FALSE;
    if (!PX_LoadTextureFromFile(mp_static, &phammer->ham01, "assets/ham1.png")) return PX_NULL;
    if (!PX_LoadTextureFromFile(mp_static, &phammer->ham02, "assets/ham2.png")) return PX_NULL;
    PX_ObjectRegisterEvent(pObject, PX_OBJECT_EVENT_CURSORMOVE, PX_Object_HammerOnMove, PX_NULL); // 注册移动事件
    PX_ObjectRegisterEvent(pObject, PX_OBJECT_EVENT_CURSORDRAG, PX_Object_HammerOnMove, PX_NULL); // 注册拖拽事件
    PX_ObjectRegisterEvent(pObject, PX_OBJECT_EVENT_CURSORDOWN, PX_Object_HammerOnCursorDown, PX_NULL); // 注册按下事件
    PX_ObjectRegisterEvent(pObject, PX_OBJECT_EVENT_CURSORDOWN, PX_Object_HammerOnMove, PX_NULL); // 注册按下事件
    PX_ObjectRegisterEvent(pObject, PX_OBJECT_EVENT_CURSORUP, PX_Object_HammerOnCursorUp, PX_NULL); // 注册抬起事件

    return pObject;
}
```

最后则是一个倒计时框，它中间其实是一个 2dx 的动画对象（PainterEngine 直接支持 gif 动画，其实 gif 也可以），外围是一个环，环形的弧度不断减少，以实现一个 `倒计时` 的显示效果：

```c
typedef struct
{
    PX_Animation animation; // 动画
    px_dword time; // 倒计时时间
    px_dword elapsed; // 倒计时开始后已经过去的时间
}PX_Object_Clock;


PX_OBJECT_UPDATE_FUNCTION(PX_Object_ClockUpdate)
{
    PX_Object_Clock* clock = PX_ObjectGetDescByType(pObject, PX_OBJECT_TYPE_CLOCK);
    clock->elapsed += elapsed;
    if (clock->elapsed >= clock->time)
    {
        clock->elapsed = 0;
        PX_ObjectPostEvent(game, PX_OBJECT_BUILD_EVENT(PX_OBJECT_EVENT_RESET)); // 重置狐狸状态，给 game 对象发送重置事件
        game->Visible = PX_FALSE;
        game->Enabled = PX_FALSE;
        startgame->Visible = PX_TRUE;
        pObject->Visible = PX_FALSE;
        pObject->Enabled = PX_FALSE;
    }

}

PX_OBJECT_RENDER_FUNCTION(PX_Object_ClockRender)
{
    PX_Object_Clock* clock = PX_ObjectGetDescByType(pObject, PX_OBJECT_TYPE_CLOCK);
    PX_AnimationUpdate(&clock->animation, elapsed); // 更新动画
    PX_AnimationRender(psurface, &clock->animation, (px_int)pObject->x, (px_int)pObject->y, PX_ALIGN_CENTER, PX_NULL); // 绘制动画
    // draw ring
    PX_GeoDrawCircle(psurface, (px_int)pObject->x, (px_int)pObject->y, 38, 8, PX_COLOR_BLACK); // 绘制倒计时环边框
    PX_GeoDrawRing(psurface, (px_int)pObject->x, (px_int)pObject->y, 36, 6, PX_COLOR(128, 192, 255, 32), -90, -90 + (px_int)(360 * (1 - clock->elapsed * 1.0f / clock->time))); // 绘制倒计时环
}

PX_OBJECT_FREE_FUNCTION(PX_Object_ClockFree)
{
    PX_Object_Clock* clock = PX_ObjectGetDescByType(pObject, PX_OBJECT_TYPE_CLOCK);
    PX_AnimationFree(&clock->animation);
}

px_void PX_Object_ClockBegin(PX_Object* pClock, px_dword time) // 开始倒计时
{
    PX_Object_Clock* clock = PX_ObjectGetDescByType(pClock, PX_OBJECT_TYPE_CLOCK);
    clock->time = time;
    pClock->Visible = PX_TRUE;
    pClock->Enabled = PX_TRUE;
}

PX_Object* PX_Object_ClockCreate(px_memorypool* mp, PX_Object* parent, px_float x, px_float y)
{
    PX_Object_Clock* clock;
    PX_Object* pObject = PX_ObjectCreateEx(mp, parent, x, y, 0, 0, 0, 0, PX_OBJECT_TYPE_CLOCK, PX_Object_ClockUpdate, PX_Object_ClockRender, PX_Object_ClockFree, 0, sizeof(PX_Object_Clock));
    clock = PX_ObjectGetDescByType(pObject, PX_OBJECT_TYPE_CLOCK);
    clock->time = 0;
    clock->elapsed = 0;
    if (!PX_AnimationCreate(&clock->animation, PX_ResourceLibraryGetAnimationLibrary(PainterEngine_GetResourceLibrary(), "song"))) // 从资源管理器中获取动画
    {
        PX_ObjectDelete(pObject);
        return PX_NULL;
    }
    pObject->Enabled = PX_FALSE;
    pObject->Visible = PX_FALSE;
    return pObject;
}
```

### 放置对象，完成游戏

在 `main` 函数中，我们将上述对象一一创建，并放置在游戏场景中，最终完成这个游戏：

```c
// 创建地鼠
game = PX_ObjectCreate(mp, root, 0, 0, 0, 0, 0, 0);
PX_Object_FoxCreate(mp, game, 173, 326);
PX_Object_FoxCreate(mp, game, 401, 326);
PX_Object_FoxCreate(mp, game, 636, 326);
PX_Object_FoxCreate(mp, game, 173, 476);
PX_Object_FoxCreate(mp, game, 401, 476);
PX_Object_FoxCreate(mp, game, 636, 476);
game->Visible = PX_FALSE;
game->Enabled = PX_FALSE;

// 创建锤子
PX_Object_HammerCreate(mp, root);
scorePanel = PX_Object_ScorePanelCreate(mp, root, 400, 60, &score_fm, 100);
// 创建倒计时框
gameclock = PX_Object_ClockCreate(mp, root, 680, 60);
```

在这里，我放上整个游戏的完整代码：

```c
#include "PainterEngine.h"

#define PX_OBJECT_TYPE_FOX    24103001
#define PX_OBJECT_TYPE_HAMMER 24103002
#define PX_OBJECT_TYPE_CLOCK  24103003

PX_FontModule score_fm;
PX_Object* scorePanel;
PX_Object* game, *startgame, *gameclock;

typedef enum
{
    PX_OBJECT_FOX_STATE_IDLE,   // 狐狸还在洞里
    PX_OBJECT_FOX_STATE_RASING, // 狐狸正在升起
    PX_OBJECT_FOX_STATE_TAUNT,  // 狐狸在嘲讽
    PX_OBJECT_FOX_STATE_ESCAPE, // 狐狸逃跑
    PX_OBJECT_FOX_STATE_BEAT,   // 狐狸被打
    PX_OBJECT_FOX_STATE_HURT,   // 狐狸受伤后逃跑
}PX_OBJECT_FOX_STATE;

typedef struct
{
    PX_OBJECT_FOX_STATE state;            // 狐狸状态
    px_dword elapsed;                     // 状态持续时间
    px_float texture_render_offset;       // 纹理渲染偏移
    px_dword gen_rand_time;               // 生成随机时间
    px_float rasing_down_speed;           // 升起速度
    px_texture render_target;             // 渲染目标
    px_texture* pcurrent_display_texture; // 当前显示的纹理
    px_texture* ptexture_mask;            // 遮罩
}PX_Object_Fox;

typedef struct
{
    px_texture ham01; // 锤子纹理 1，没有按下
    px_texture ham02; // 锤子纹理 2，按下
    px_bool bHit; // 是否按下
}PX_Object_Hammer;

typedef struct
{
    PX_Animation animation; // 动画
    px_dword time; // 倒计时时间
    px_dword elapsed; // 倒计时开始后已经过去的时间
}PX_Object_Clock;


PX_OBJECT_UPDATE_FUNCTION(PX_Object_ClockUpdate)
{
    PX_Object_Clock* clock = PX_ObjectGetDescByType(pObject, PX_OBJECT_TYPE_CLOCK);
    clock->elapsed += elapsed;
    if (clock->elapsed >= clock->time)
    {
        clock->elapsed = 0;
        PX_ObjectPostEvent(game, PX_OBJECT_BUILD_EVENT(PX_OBJECT_EVENT_RESET)); // 重置狐狸状态，给 game 对象发送重置事件
        game->Visible = PX_FALSE;
        game->Enabled = PX_FALSE;
        startgame->Visible = PX_TRUE;
        pObject->Visible = PX_FALSE;
        pObject->Enabled = PX_FALSE;
    }

}

PX_OBJECT_RENDER_FUNCTION(PX_Object_ClockRender)
{
    PX_Object_Clock* clock = PX_ObjectGetDescByType(pObject, PX_OBJECT_TYPE_CLOCK);
    PX_AnimationUpdate(&clock->animation, elapsed); // 更新动画
    PX_AnimationRender(psurface, &clock->animation, (px_int)pObject->x, (px_int)pObject->y, PX_ALIGN_CENTER, PX_NULL); // 绘制动画
    // draw ring
    PX_GeoDrawCircle(psurface, (px_int)pObject->x, (px_int)pObject->y, 38, 8, PX_COLOR_BLACK); // 绘制倒计时环边框
    PX_GeoDrawRing(psurface, (px_int)pObject->x, (px_int)pObject->y, 36, 6, PX_COLOR(128, 192, 255, 32), -90, -90 + (px_int)(360 * (1 - clock->elapsed * 1.0f / clock->time))); // 绘制倒计时环
}

PX_OBJECT_FREE_FUNCTION(PX_Object_ClockFree)
{
    PX_Object_Clock* clock = PX_ObjectGetDescByType(pObject, PX_OBJECT_TYPE_CLOCK);
    PX_AnimationFree(&clock->animation);
}

px_void PX_Object_ClockBegin(PX_Object* pClock, px_dword time) // 开始倒计时
{
    PX_Object_Clock* clock = PX_ObjectGetDescByType(pClock, PX_OBJECT_TYPE_CLOCK);
    clock->time = time;
    pClock->Visible = PX_TRUE;
    pClock->Enabled = PX_TRUE;
}

PX_Object* PX_Object_ClockCreate(px_memorypool* mp, PX_Object* parent, px_float x, px_float y)
{
    PX_Object_Clock* clock;
    PX_Object* pObject = PX_ObjectCreateEx(mp, parent, x, y, 0, 0, 0, 0, PX_OBJECT_TYPE_CLOCK, PX_Object_ClockUpdate, PX_Object_ClockRender, PX_Object_ClockFree, 0, sizeof(PX_Object_Clock));
    clock = PX_ObjectGetDescByType(pObject, PX_OBJECT_TYPE_CLOCK);
    clock->time = 0;
    clock->elapsed = 0;
    if (!PX_AnimationCreate(&clock->animation, PX_ResourceLibraryGetAnimationLibrary(PainterEngine_GetResourceLibrary(), "song"))) // 从资源管理器中获取动画
    {
        PX_ObjectDelete(pObject);
        return PX_NULL;
    }
    pObject->Enabled = PX_FALSE;
    pObject->Visible = PX_FALSE;
    return pObject;
}

PX_OBJECT_UPDATE_FUNCTION(PX_Object_FoxOnUpdate)
{
    PX_Object_Fox* pfox = PX_ObjectGetDescByType(pObject, PX_OBJECT_TYPE_FOX);
    switch (pfox->state)
    {
        case PX_OBJECT_FOX_STATE_IDLE:
        {
            if (pfox->gen_rand_time == 0)
            {
                pfox->gen_rand_time = PX_rand() % 3000 + 1000; // 狐狸在洞里的时间，时间到了就升起来
            }
            else
            {
                if (pfox->gen_rand_time <elapsed) // 时间到了
                {
                    // 升起
                    pfox->state = PX_OBJECT_FOX_STATE_RASING;
                    pfox->elapsed = 0;
                    pfox->gen_rand_time = 0;
                    pfox->texture_render_offset = pObject->Height;
                    // 改变纹理
                    pfox->pcurrent_display_texture = PX_ResourceLibraryGetTexture(PainterEngine_GetResourceLibrary(), "fox_rasing");
                }
                else
                {
                    pfox->gen_rand_time -= elapsed;
                }
            }
        }
        break;
        case PX_OBJECT_FOX_STATE_RASING: // 狐狸升起
        {
            pfox->elapsed += elapsed;
            // 升起纹理偏移量
            pfox->texture_render_offset -= pfox->rasing_down_speed * elapsed / 1000;
            if (pfox->texture_render_offset <= 0)
            {
                pfox->texture_render_offset = 0;
                pfox->state = PX_OBJECT_FOX_STATE_TAUNT; // 升起后嘲讽
                pfox->elapsed = 0;
            }
        }
        break;
        case PX_OBJECT_FOX_STATE_TAUNT: // 狐狸嘲讽
        {
            pfox->elapsed += elapsed;
            if (pfox->elapsed>600&& pfox->elapsed <1500) // 嘲讽时间
            {
                pfox->pcurrent_display_texture = PX_ResourceLibraryGetTexture(PainterEngine_GetResourceLibrary(), "fox_taunt"); // 嘲讽纹理
            }
            else if (pfox->elapsed>1500) // 嘲讽结束
            {
                pfox->texture_render_offset = 0;
                pfox->state = PX_OBJECT_FOX_STATE_ESCAPE; // 逃跑
                pfox->pcurrent_display_texture = PX_ResourceLibraryGetTexture(PainterEngine_GetResourceLibrary(), "fox_escape"); // 逃跑纹理
                pfox->elapsed = 0;
            }
        }
        break;
        case PX_OBJECT_FOX_STATE_BEAT: // 狐狸被打
        {
            pfox->elapsed += elapsed;
            if (pfox->elapsed>800)
            {
                pfox->pcurrent_display_texture = PX_ResourceLibraryGetTexture(PainterEngine_GetResourceLibrary(), "fox_hurt"); // 受伤纹理
                pfox->state = PX_OBJECT_FOX_STATE_ESCAPE; // 逃跑
            }
        }
        break;
        case PX_OBJECT_FOX_STATE_ESCAPE:
        {
            pfox->elapsed += elapsed;
            pfox->texture_render_offset += pfox->rasing_down_speed * elapsed / 1000;
            if (pfox->texture_render_offset >= pObject->Height)
            {
                pfox->texture_render_offset = pObject->Height;
                pfox->state = PX_OBJECT_FOX_STATE_IDLE; // 逃跑结束
                pfox->elapsed = 0; // 重置时间
                pfox->pcurrent_display_texture = PX_NULL;
            }
        }
        break;
    default:
        break;
    }
}

PX_OBJECT_RENDER_FUNCTION(PX_Object_FoxOnRender)
{
    PX_Object_Fox* pfox = PX_ObjectGetDescByType(pObject, PX_OBJECT_TYPE_FOX);
    px_float x, y, width, height;
    PX_OBJECT_INHERIT_CODE(pObject, x, y, width, height);
    PX_TextureClearAll(&pfox->render_target, PX_COLOR_NONE); // 清空渲染目标
    if (pfox->pcurrent_display_texture)
    {
        PX_TextureRender(&pfox->render_target, pfox->pcurrent_display_texture, (px_int)pfox->render_target.width/2, (px_int)pfox->texture_render_offset, PX_ALIGN_MIDTOP, PX_NULL); // 渲染狐狸
    }
    PX_TextureRenderMask(psurface, pfox->ptexture_mask, &pfox->render_target, (px_int)x, (px_int)y, PX_ALIGN_MIDBOTTOM, PX_NULL); // 以遮罩形式绘制纹理
}

PX_OBJECT_FREE_FUNCTION(PX_Object_FoxFree)
{
    PX_Object_Fox* pfox = PX_ObjectGetDescByType(pObject, PX_OBJECT_TYPE_FOX);
    PX_TextureFree(&pfox->render_target);
}

PX_OBJECT_EVENT_FUNCTION(PX_Object_FoxOnClick) // 狐狸被点击
{
    PX_Object_Fox* pfox = PX_ObjectGetDescByType(pObject, PX_OBJECT_TYPE_FOX);
    if (pfox->state == PX_OBJECT_FOX_STATE_TAUNT|| pfox->state == PX_OBJECT_FOX_STATE_RASING) // 狐狸嘲讽或者升起时点击有效
    {
        if (PX_ObjectIsCursorInRegionAlign(pObject, e, PX_ALIGN_MIDBOTTOM)) // 点击有效区域
        {
            px_int x = (px_int)PX_Object_Event_GetCursorX(e);
            px_int y = (px_int)PX_Object_Event_GetCursorY(e);
            x = (px_int)(x - (pObject->x - pObject->Width/2));
            y = (px_int)(y - (pObject->y - pObject->Height));
            if (x>32&&y<128)
            {
                pfox->pcurrent_display_texture = PX_ResourceLibraryGetTexture(PainterEngine_GetResourceLibrary(), "fox_beat");
                pfox->state = PX_OBJECT_FOX_STATE_BEAT;
                pfox->elapsed = 0;
                PX_Object_ScorePanelAddScore(scorePanel, 100);
            }
        }
    }
}

PX_OBJECT_EVENT_FUNCTION(PX_Object_FoxOnReset)
{
    PX_Object_Fox* pfox = PX_ObjectGetDescByType(pObject, PX_OBJECT_TYPE_FOX);
    pfox->state = PX_OBJECT_FOX_STATE_IDLE;
    pfox->elapsed = 0;
    pfox->texture_render_offset = pObject->Height;
    pfox->gen_rand_time = 0;
    pfox->pcurrent_display_texture = PX_NULL;

}

PX_Object *PX_Object_FoxCreate(px_memorypool *mp, PX_Object *parent, px_float x, px_float y)
{
    PX_Object_Fox* pfox;
    px_texture *ptexture = PX_ResourceLibraryGetTexture(PainterEngine_GetResourceLibrary(), "fox_rasing"); // 从资源管理器中获取纹理
    PX_Object* pObject = PX_ObjectCreateEx(mp, parent, x, y, 0, ptexture->width*1.f, ptexture->height*1.f, 0, PX_OBJECT_TYPE_FOX, PX_Object_FoxOnUpdate, PX_Object_FoxOnRender, PX_Object_FoxFree, 0, sizeof(PX_Object_Fox));
    pfox = PX_ObjectGetDescByType(pObject, PX_OBJECT_TYPE_FOX);
    pfox->state = PX_OBJECT_FOX_STATE_IDLE; // 狐狸状态
    pfox->rasing_down_speed = 512; // 升起速度
    pfox->ptexture_mask = PX_ResourceLibraryGetTexture(PainterEngine_GetResourceLibrary(), "fox_mask"); // 遮罩
    if (!PX_TextureCreate(mp, &pfox->render_target, ptexture->width, ptexture->height))
    {
        PX_ObjectDelete(pObject);
        return 0;
    }
    PX_ObjectRegisterEvent(pObject, PX_OBJECT_EVENT_CURSORDOWN, PX_Object_FoxOnClick, 0); // 注册点击事件
    PX_ObjectRegisterEvent(pObject, PX_OBJECT_EVENT_RESET, PX_Object_FoxOnReset, 0); // 注册重置事件
    return pObject;
}

PX_OBJECT_RENDER_FUNCTION(PX_Object_HammerRender) // 锤子渲染
{
    PX_Object_Hammer* phammer = PX_ObjectGetDescByType(pObject, PX_OBJECT_TYPE_HAMMER);
    px_float x, y, width, height;
    PX_OBJECT_INHERIT_CODE(pObject, x, y, width, height);
    if (phammer->bHit)
    {
        PX_TextureRender(psurface, &phammer->ham02, (px_int)x, (px_int)y, PX_ALIGN_CENTER, PX_NULL); // 按下
    }
    else
    {
        PX_TextureRender(psurface, &phammer->ham01, (px_int)x, (px_int)y, PX_ALIGN_CENTER, PX_NULL); // 未按下
    }
    
}

PX_OBJECT_FREE_FUNCTION(PX_Object_HammerFree)
{
    PX_Object_Hammer* phammer = PX_ObjectGetDescByType(pObject, PX_OBJECT_TYPE_HAMMER);
    PX_TextureFree(&phammer->ham01);
    PX_TextureFree(&phammer->ham02);
}

PX_OBJECT_EVENT_FUNCTION(PX_Object_HammerOnMove)
{
    pObject->x = PX_Object_Event_GetCursorX(e); // 锤子跟随鼠标移动
    pObject->y = PX_Object_Event_GetCursorY(e);
}

PX_OBJECT_EVENT_FUNCTION(PX_Object_HammerOnCursorDown)
{
    PX_Object_Hammer* phammer = PX_ObjectGetDescByType(pObject, PX_OBJECT_TYPE_HAMMER);
    phammer->bHit = PX_TRUE; // 按下
}

PX_OBJECT_EVENT_FUNCTION(PX_Object_HammerOnCursorUp)
{
    PX_Object_Hammer* phammer = PX_ObjectGetDescByType(pObject, PX_OBJECT_TYPE_HAMMER);
    phammer->bHit = PX_FALSE; // 抬起
}

PX_Object* PX_Object_HammerCreate(px_memorypool* mp, PX_Object* parent)
{
    PX_Object_Hammer* phammer;
    PX_Object* pObject = PX_ObjectCreateEx(mp, parent, 0, 0, 0, 0, 0, 0, PX_OBJECT_TYPE_HAMMER, 0, PX_Object_HammerRender, PX_Object_HammerFree, 0, sizeof(PX_Object_Hammer));
    phammer = PX_ObjectGetDescByType(pObject, PX_OBJECT_TYPE_HAMMER);
    phammer->bHit = PX_FALSE;
    if (!PX_LoadTextureFromFile(mp_static, &phammer->ham01, "assets/ham1.png")) return PX_NULL;
    if (!PX_LoadTextureFromFile(mp_static, &phammer->ham02, "assets/ham2.png")) return PX_NULL;
    PX_ObjectRegisterEvent(pObject, PX_OBJECT_EVENT_CURSORMOVE, PX_Object_HammerOnMove, PX_NULL); // 注册移动事件
    PX_ObjectRegisterEvent(pObject, PX_OBJECT_EVENT_CURSORDRAG, PX_Object_HammerOnMove, PX_NULL); // 注册拖拽事件
    PX_ObjectRegisterEvent(pObject, PX_OBJECT_EVENT_CURSORDOWN, PX_Object_HammerOnCursorDown, PX_NULL); // 注册按下事件
    PX_ObjectRegisterEvent(pObject, PX_OBJECT_EVENT_CURSORDOWN, PX_Object_HammerOnMove, PX_NULL); // 注册按下事件
    PX_ObjectRegisterEvent(pObject, PX_OBJECT_EVENT_CURSORUP, PX_Object_HammerOnCursorUp, PX_NULL); // 注册抬起事件

    return pObject;
}

PX_OBJECT_EVENT_FUNCTION(PX_Object_StartGameOnClick)
{
    game->Visible = PX_TRUE;
    startgame->Visible = PX_FALSE;
    game->Enabled = PX_TRUE;
    PX_Object_ScorePanelSetScore(scorePanel, 0);
    PX_Object_ClockBegin(gameclock, 30000); // 开始游戏，游戏时间 30 秒
}




px_int main()
{
    px_int i;
    PainterEngine_Initialize(800, 480);
    PX_FontModuleInitialize(mp_static, &score_fm);
    PX_FontModuleSetCodepage(&score_fm, PX_FONTMODULE_CODEPAGE_GBK);
    if (!PX_LoadTextureToResource(PainterEngine_GetResourceLibrary(), "assets/rasing.png", "fox_rasing")) return 0;
    if (!PX_LoadTextureToResource(PainterEngine_GetResourceLibrary(), "assets/taunt.png", "fox_taunt")) return 0;
    if (!PX_LoadTextureToResource(PainterEngine_GetResourceLibrary(), "assets/escape.png", "fox_escape")) return 0;
    if (!PX_LoadTextureToResource(PainterEngine_GetResourceLibrary(), "assets/beat.png", "fox_beat")) return 0;
    if (!PX_LoadTextureToResource(PainterEngine_GetResourceLibrary(), "assets/hurt.png", "fox_hurt")) return 0;
    if (!PX_LoadTextureToResource(PainterEngine_GetResourceLibrary(), "assets/mask.png", "fox_mask")) return 0;
    if (!PX_LoadTextureToResource(PainterEngine_GetResourceLibrary(), "assets/background.png", "background")) return 0;
    if (!PX_LoadAnimationToResource(PainterEngine_GetResourceLibrary(), "assets/song.2dx", "song")) return 0;
    PainterEngine_SetBackgroundTexture(PX_ResourceLibraryGetTexture(PainterEngine_GetResourceLibrary(), "background"));
    for (i = 0; i <= 9; i++)
    {
        px_texture tex;
        px_char path[64];
        PX_sprintf1(path, 64, "assets/%1.png", PX_STRINGFORMAT_INT(i));
        if (PX_LoadTextureFromFile(mp, &tex, path))
        {
            PX_FontModuleAddNewTextureCharacter(&score_fm, '0' + i, &tex);
        }
        PX_TextureFree(&tex);
    }
    
    startgame = PX_Object_PushButtonCreate(mp, root, 300, 200, 200, 90, "Start Game", 0);
    startgame->Visible = PX_TRUE;
    PX_Object_PushButtonSetBackgroundColor(startgame, PX_COLOR(96, 255, 255, 255));
    PX_Object_PushButtonSetPushColor(startgame, PX_COLOR(224, 255, 255, 255));
    PX_Object_PushButtonSetCursorColor(startgame, PX_COLOR(168, 255, 255, 255));
    PX_ObjectRegisterEvent(startgame, PX_OBJECT_EVENT_EXECUTE, PX_Object_StartGameOnClick, 0);

    
    
    game = PX_ObjectCreate(mp, root, 0, 0, 0, 0, 0, 0);
    PX_Object_FoxCreate(mp, game, 173, 326);
    PX_Object_FoxCreate(mp, game, 401, 326);
    PX_Object_FoxCreate(mp, game, 636, 326);
    PX_Object_FoxCreate(mp, game, 173, 476);
    PX_Object_FoxCreate(mp, game, 401, 476);
    PX_Object_FoxCreate(mp, game, 636, 476);
    game->Visible = PX_FALSE;
    game->Enabled = PX_FALSE;

    
    PX_Object_HammerCreate(mp, root);
    scorePanel = PX_Object_ScorePanelCreate(mp, root, 400, 60, &score_fm, 100);

    gameclock = PX_Object_ClockCreate(mp, root, 680, 60);
    
    return PX_TRUE;
}
```

你可以在 documents/demo/game 中找到这个游戏的完整资源，并用 PainterEngine 直接编译。

![](assets/img/18.5.gif)

在线试玩：[PainterEngine 在线应用 APP——打地鼠](https://www.painterengine.com/main/app/documentgame/)

