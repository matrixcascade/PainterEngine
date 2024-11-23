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

## 7. Creating a GUI Button Using PainterEngine

In this chapter, we will first encounter PainterEngine components. Now, we will use PainterEngine to create a classic GUI component—a button.

In PainterEngine, all components are described using the `PX_Object` structure, and creating a component returns a pointer of type `PX_Object *`.

However, in this chapter, we don't need to consider things that complex; we just need to create a button. In PainterEngine, the most commonly used button type is `PX_Object_PushButton`.

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

Now, let's take a detailed look at the `PX_Object_PushButtonCreate` function. The first parameter is a memory pool. As we mentioned earlier, PainterEngine has two system default memory pools, and it doesn't matter whether you fill in `mp` or `mp_static` here. However, considering that the change of the interface may involve object allocation and destruction, we choose the `mp` memory pool.

The second parameter `root` is the root object of PainterEngine. We will discuss the PainterEngine object management mechanism later. Here, you just need to understand that filling in `root` means **_create a button object as a child object of the root object_**. This way, the button can be linked to the system object tree for event response and rendering.

Then come the x, y, width, and height of the button, which specify its position and dimensions.

The last parameter is the font module pointer, which is the TTF font file we loaded earlier. Without it, our button won't be able to display Chinese characters. Of course, you can choose other fonts to achieve different styles.

## 8. PainterEngine Object Passing Mechanism

In the previous chapter, we briefly introduced the root object `root`. In this chapter, we will learn about the object management mechanism of PainterEngine.

As we mentioned earlier, in PainterEngine, all components are described using the `PX_Object` structure, and PainterEngine objects exist in the form of a tree:

![](assets/img/8.1.png)

Each `PX_Object` is a node in this tree and can have its own child nodes (possibly multiple) and parent node (only one). Additionally, each `PX_Object` has the following four basic functional functions:

- `Create`: The object creation function, or rather, the object initialization function. In PainterEngine, it is generally in the form of `PX_Object_xxxxxCreate`, where `xxxxx` is the name of the object, such as `PushButton` in the previous chapter. The `Create` function generally handles some initialization processes of the object and connects it to the object tree.

- `Update`: The physical information update work of the object is basically completed in this function. It usually handles some physical information of the object, such as position, size, speed, etc., which is common in game design objects and less common in GUI objects. Its design is to distinguish from the subsequent `Render` function, because in game servers for example, objects do not need to be rendered, and rendering is very performance-intensive.

- `Render`: The rendering work of the object is basically completed in this function, used for the rendering functionality of `PX_Object`, rendering image data to the screen. In some cases, physical information is also handled in this function because the physical information of this object does not affect the actual operation of the game, such as special effects and particle effects. Most GUI components also almost only use the `Render` function.

- `Free`: The release work of the object is basically completed in this function, such as releasing textures loaded in `Create` or freeing allocated memory.

The above `Update`, `Render`, and `Free` functions have a passing characteristic, meaning:

- If an object node executes `Update`, then all its child objects will also execute `Update`.
- If an object node executes `Render`, then all its child objects will also execute `Render`.
- If an object node executes `Free`, then all its child objects will also execute `Free`. When the parent object is deleted, its child nodes will also be deleted, and this process will continue until all child nodes under this node are deleted.

Therefore, in the previous chapter, when we created a button and connected it to the `root` node, we do not need to manually execute the `Update`, `Render`, and `Free` functions (they are already implemented in `PX_Object_PushButton.c`). The root node `root` is automatically updated, rendered, and released, so we only need to handle the `Create` function.

Of course, if you want to delete this object, you can simply call `PX_ObjectDelayDelete` or `PX_ObjectDelete`:

```c
#include "PainterEngine.h"
int main()
{
    PX_Object* myButtonObject;
    PainterEngine_Initialize(800, 480);
    PainterEngine_LoadFontModule("assets/font.ttf", PX_FONTMODULE_CODEPAGE_GBK, 20);
    myButtonObject = PX_Object_PushButtonCreate(mp, root, 300, 200, 200, 80, "我是一个按钮", PainterEngine_GetFontModule());
    PX_ObjectDelayDelete(myButtonObject); // Delete the object
    return 1;
}
```

Both functions have the same functionality and parameters, but `PX_ObjectDelayDelete` will execute the deletion after updating and rendering are completed, while `PX_ObjectDelete` performs immediate deletion. I recommend using `PX_ObjectDelayDelete`, as this can prevent issues where other objects still reference the data of an object that has been immediately deleted, leading to accessing invalid memory.

## 9. PainterEngine Message Mechanism

Now, although we have created a button, we cannot respond to it. To respond to button events, we need to bind the button control to messages. Please see the following code:

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

Here, `PX_OBJECT_EVENT_FUNCTION` is a macro. Since event handler functions have a fixed format, it is highly recommended to declare them using a macro. Its definition prototype is as follows:

```c
#define PX_OBJECT_EVENT_FUNCTION(name) px_void name(PX_Object *pObject, PX_Object_Event e, px_void * ptr)
```

As you can see, this callback function has 3 parameters. The first is a pointer to the object that triggered the event, which in this case is the button object. The second parameter is the event type `e`, which is the type of the triggered event. The last parameter is a user-passed pointer, which is passed in when the event handler function `PX_ObjectRegisterEvent` is called.

There are several event types:

```c
#define PX_OBJECT_EVENT_ANY           0  // Any event
#define PX_OBJECT_EVENT_CURSORMOVE    1  // Mouse move
#define PX_OBJECT_EVENT_CURSORUP      2  // Left mouse button up or touch screen up
#define PX_OBJECT_EVENT_CURSORRDOWN   3  // Right mouse button down
#define PX_OBJECT_EVENT_CURSORDOWN    4  // Left mouse button down or touch screen down
#define PX_OBJECT_EVENT_CURSORRUP     5  // Right mouse button up
#define PX_OBJECT_EVENT_CURSOROVER    6  // Mouse enters range
#define PX_OBJECT_EVENT_CURSOROUT     7  // Mouse leaves range
#define PX_OBJECT_EVENT_CURSORWHEEL   8  // Mouse wheel
#define PX_OBJECT_EVENT_CURSORCLICK   9  // Left mouse button click
#define PX_OBJECT_EVENT_CURSORDRAG    10 // Mouse drag
#define PX_OBJECT_EVENT_STRING        11 // String event (input by the input method)
#define PX_OBJECT_EVENT_EXECUTE       12 // Execute event, different components have different execution methods
#define PX_OBJECT_EVENT_VALUECHANGED  13 // Value changed event, such as slider value change, or text box value change, or list box selection change
#define PX_OBJECT_EVENT_DRAGFILE      14 // Drag file
#define PX_OBJECT_EVENT_KEYDOWN       15 // Key down
#define PX_OBJECT_EVENT_KEYUP         16 // Key up
#define PX_OBJECT_EVENT_IMPACT        17 // Impact event
#define PX_OBJECT_EVENT_SCALE         18 // Scale event
#define PX_OBJECT_EVENT_WINDOWRESIZE  19 // Window resize
#define PX_OBJECT_EVENT_ONFOCUS       20 // Gain focus
#define PX_OBJECT_EVENT_LOSTFOCUS     21 // Lose focus
#define PX_OBJECT_EVENT_CANCEL        22 // Cancel event
#define PX_OBJECT_EVENT_CLOSE         23 // Close event
#define PX_OBJECT_EVENT_CURSORMUP     24 // Middle mouse button up
#define PX_OBJECT_EVENT_CURSORMDOWN   25 // Middle mouse button down
#define PX_OBJECT_EVENT_REQUESTDATA   26 // Request data
#define PX_OBJECT_EVENT_OPEN          27 // Open event
#define PX_OBJECT_EVENT_SAVE          28 // Save event
#define PX_OBJECT_EVENT_TIMEOUT       29 // Timeout event
#define PX_OBJECT_EVENT_DAMAGE        30 // Damage event
```

Not all of these events are responded to by every component. For example, in the above example, `PX_OBJECT_EVENT_EXECUTE` is an event that is triggered when a button is clicked or when the <kbd>Enter</kbd> key is pressed in a text box, but components like scroll bars and progress bars do not trigger this event. That means some events are specific to certain components.

However, events like those prefixed with `CURSOR` or `KEY` are received by all components connected to the `root` node (though they may not all respond). It is important to note that `CURSOR` events, such as mouse or touch screen events, are not only triggered when the mouse or touch screen moves over the component's position and range. Whenever such an event is dispatched to the `root` node, it will be passed down layer by layer to all its child nodes. If you want to implement something like "only trigger when the mouse clicks on the button," you need to implement your own range check.

You can use the following functions to get the functionality of "where the mouse is now" in a `cursor` event:

```c
px_float PX_Object_Event_GetCursorX(PX_Object_Event e); // Get the x coordinate of the cursor event
px_float PX_Object_Event_GetCursorY(PX_Object_Event e); // Get the y coordinate of the cursor event
px_float PX_Object_Event_GetCursorZ(PX_Object_Event e); // Get the z coordinate of the cursor event, generally used for middle mouse button scroll wheel
```

Let's go back to the `OnButtonClick` function in the source code, which simply changes the button text using `PX_Object_PushButtonSetText`.

Finally, let's look at the `PX_ObjectRegisterEvent` function. This function binds events to C language functions. The first parameter is a pointer to the button component we created earlier. The second parameter is the event type we want to bind, in this case, `PX_OBJECT_EVENT_EXECUTE`, which is triggered when the button is clicked. The third parameter is a user pointer, which is passed to the callback function. If you don't need it, you can directly pass `PX_NULL`.

## 10. A Small Example: Implementing a Digital Photo Album with PainterEngine

Now, let's start the first step in component-based development with PainterEngine using a small example. In this example, I will use button and image frame components to develop a digital photo album feature. The art resources used in this article can be found in `documents/logo`.

```c
#include "PainterEngine.h"

PX_Object* Previous, *Next, *Image;
px_texture my_texture[10]; // Array to store images
px_int index = 0; // Index of the current image

PX_OBJECT_EVENT_FUNCTION(OnButtonPreClick)
{
    index--;
    if (index < 0)
    {
        index = 9;
    }
    PX_Object_ImageSetTexture(Image, &my_texture[index]); // Set the image
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
    PainterEngine_Initialize(512, 560); // Initialization
    for (i = 0; i < 10; i++)
    {
        px_char path[256];
        PX_sprintf1(path, 256, "assets/%1.png", PX_STRINGFORMAT_INT(i + 1));
        if (!PX_LoadTextureFromFile(mp_static, &my_texture[i], path)) // Load the image
        {
            // Load failed
            printf("加载失败");
            return 0;
        }
    }
    PainterEngine_LoadFontModule("assets/font.ttf", PX_FONTMODULE_CODEPAGE_GBK, 20); // Load the font
    Image = PX_Object_ImageCreate(mp, root, 0, 0, 512, 512, 0); // Create an image object
    Previous = PX_Object_PushButtonCreate(mp, root, 0, 512, 256, 48, "Previous", PainterEngine_GetFontModule()); // Create a button object
    Next = PX_Object_PushButtonCreate(mp, root, 256, 512, 256, 48, "Next", PainterEngine_GetFontModule()); // Create a button object
    PX_ObjectRegisterEvent(Previous, PX_OBJECT_EVENT_EXECUTE, OnButtonPreClick, PX_NULL); // Register the button event
    PX_ObjectRegisterEvent(Next, PX_OBJECT_EVENT_EXECUTE, OnButtonNextClick, PX_NULL); // Register the button event
    return 1;
}
```

In the above code, `OnButtonPreClick` and `OnButtonNextClick` are the callback functions for the "Previous" and "Next" buttons, respectively. We use the `PX_Object_ImageSetTexture` function to switch the images in the image frame.

In the `main` function, we first load the TTF font, then create an image component using `PX_Object_ImageCreate`. After that, we create two buttons and bind their event callback functions using `PX_ObjectRegisterEvent`. Finally, here is the result:

![](assets/img/10.1.gif)

## 11. More Commonly Used PainterEngine Components

You can find PainterEngine's built-in components in the `PainterEngine/kernel` files. All component names start with `PX_Object_XXXXX`. Here, I will list some commonly used components along with sample code:

- Text Box:

```c
#include "PainterEngine.h"
PX_OBJECT_EVENT_FUNCTION(PX_Object_EditOnTextChanged)
{
    // This will be executed when the text changes
}

int main()
{
    PX_Object* pObject;
    PainterEngine_Initialize(600, 400);
    // Create a text box
    pObject = PX_Object_EditCreate(mp, root, 200, 180, 200, 40, 0);
    // Register the text change event for the edit box
    PX_ObjectRegisterEvent(pObject, PX_OBJECT_EVENT_VALUECHANGED, PX_Object_EditOnTextChanged, PX_NULL);
    return 0;
}
```

![](assets/img/11.1.gif)

- List Box:

```c
#include "PainterEngine.h"

PX_OBJECT_RENDER_FUNCTION(PX_Object_OnMyListItemRender)
{
    px_float objx, objy, objWidth, objHeight;
    PX_Object_ListItem *pItem = PX_Object_GetListItem(pObject);
    PX_OBJECT_INHERIT_CODE(pObject, objx, objy, objWidth, objHeight);
    // Draw its text
    PX_FontModuleDrawText(psurface, 0, (px_int)objx + 3, (px_int)objy + 3, PX_ALIGN_LEFTTOP, (const px_char *)pItem->pdata, PX_COLOR_WHITE);
}

PX_OBJECT_LIST_ITEM_CREATE_FUNCTION(PX_Object_OnMyListItemCreate)
{
    // Bind the render function to the ListItem
    ItemObject->Func_ObjectRender[0] = PX_Object_OnMyListItemRender;
    return PX_TRUE;
}

PX_OBJECT_EVENT_FUNCTION(PX_Object_ListOnSelectChanged)
{
    // When the selected item changes
    return;
}

int main()
{
    PX_Object* pObject;
    PainterEngine_Initialize(600, 400);

    // Create a list
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

- Slider:

```c
#include "PainterEngine.h"

PX_OBJECT_EVENT_FUNCTION(SliderChanged)
{
    // Code to be executed when the vertical slider value changes
    return;
}

int main()
{
    PX_Object* pObject;
    PainterEngine_Initialize(600, 400);
    // Horizontal slider
    PX_Object_SliderBarCreate(mp, root, 200, 50, 200, 24, PX_OBJECT_SLIDERBAR_TYPE_HORIZONTAL, PX_OBJECT_SLIDERBAR_STYLE_BOX);
    // Vertical slider
    pObject = PX_Object_SliderBarCreate(mp, root, 200, 100, 24, 200, PX_OBJECT_SLIDERBAR_TYPE_VERTICAL, PX_OBJECT_SLIDERBAR_STYLE_BOX);
    PX_ObjectRegisterEvent(pObject, PX_OBJECT_EVENT_VALUECHANGED, SliderChanged, 0);
    return 0;
}
```

![](assets/img/11.3.gif)

- Drop-down Box:

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

- Oscilloscope:

```c
#include "PainterEngine.h"

// Must be valid and accessible data within the lifetime domain, defined here as a global variable
px_double data_x[100];
px_double data_y[100];

int main()
{
    PX_Object_OscilloscopeData data;
    PX_Object* pObject;

    px_int i;
    PainterEngine_Initialize(600, 600);
    
    // Initialize test data
    for (i = 0; i < 100; i++)
    {
        data_x[i] = i;
        data_y[i] = i + PX_randRange(-10, 10);
    }
    
    pObject = PX_Object_OscilloscopeCreate(mp, root, 0, 0, 600, 600, 0);

    // Set the minimum and maximum values for the horizontal axis
    PX_Object_OscilloscopeSetHorizontalMin(pObject, 0);
    PX_Object_OscilloscopeSetHorizontalMax(pObject, 100);

    // Set the minimum and maximum values for the vertical axis (left side) 0-100
    PX_Object_OscilloscopeSetLeftVerticalMin(pObject, 0);
    PX_Object_OscilloscopeSetLeftVerticalMax(pObject, 100);

    // Data type
    data.Color = PX_COLOR(255, 192, 255, 128); // Data color
    data.ID = 0;
    data.linewidth = 3; // Line width of the data
    data.Map = PX_OBJECT_OSCILLOSCOPE_OSCILLOSCOPEDATA_MAP_LEFT; // Map data to the left vertical axis
    data.MapHorizontalArray = data_x; // Horizontal coordinates of the data
    data.MapVerticalArray = data_y; // Vertical coordinates of the data
    data.Size = 100; // Size of the data
    data.Visibled = PX_TRUE; // Data visibility
    data.Normalization = 1; // Data normalization factor is 1
    
    // Add data
    PX_Object_OscilloscopeAddData(pObject, data);
    return 0;
}
```

![](assets/img/11.5.gif)

Since there are too many components, I cannot list all of them. If you want to know the specific usage and purpose of a particular component, you can visit the [Component Market](https://market.painterengine.com/) of PainterEngine. There, you can find descriptions and sample code for both built-in and third-party components of PainterEngine.

![](assets/img/11.6.png)

## 12. Implementing Your Own PainterEngine Component

PainterEngine encourages a component-based development architecture. That is, whether it's a game, a GUI interactive program, or even program functionality, we can develop it in the form of a component.

Component-based development is somewhat similar to Classes in C++. Each component needs to implement its own `Create`, `Update`, `Render`, and `Free` functions. For the above four functions, you can refer to the [previous section on object passing mechanism](#8-painterengine-object-passing-mechanism).

To demonstrate this, let's implement a "draggable and rotatable image component," which allows us to drag the image around the interface with the mouse and rotate it using the middle mouse button.

To achieve this function, let's complete this step-by-step. First, to create a component, we need a structure to describe our component. Since we need to draw an image, we require a `px_texture` type. Additionally, since we need to rotate the image, it also has a `rotation` to describe the angle of rotation:

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

Afterwards, we need to define our `Create`, `Update`, `Render`, and `Free` functions. Among these, `Update`, `Render`, and `Free` have corresponding formats, each with a macro to simplify our definition process:

```c
#define PX_OBJECT_RENDER_FUNCTION(name) px_void name(px_surface *psurface, PX_Object *pObject, px_int idesc, px_dword elapsed)
#define PX_OBJECT_UPDATE_FUNCTION(name) px_void name(PX_Object *pObject, px_int idesc, px_dword elapsed)
#define PX_OBJECT_FREE_FUNCTION(name) px_void name(PX_Object *pObject, px_int idesc)
```

So, in the main function, we can define these functions as follows:

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
    PX_TextureRenderEx(psurface, &pMyObject->image, (px_int)pObject->x, (px_int)pObject->y, PX_ALIGN_CENTER, 0, 1, pMyObject->rotation); // Render the image
}

PX_OBJECT_FREE_FUNCTION(MyObjectFree)
{
    PX_Object_MyObject *pMyObject = PX_ObjectGetDesc(PX_Object_MyObject, pObject);
    PX_TextureFree(&pMyObject->image); // Free the image
}

px_int main()
{
    PainterEngine_Initialize(800, 480);
    return PX_TRUE;
}
```

Here, since we don't need to update any physical information, we can leave the `MyObjectUpdate` function empty. In the `MyObjectRender` function, we only need to draw the image. Here, we first use the `PX_ObjectGetDesc` function to obtain the pointer to our defined structure. Its first parameter is the structure type, and the second parameter is the `pObject` pointer passed into the function. Then, we just need to use the `PX_TextureRenderEx` function to draw the image.

As a side note, the `PX_TextureRenderEx` function is used to render a texture on a specified surface and provides extended options such as alignment, blending, scaling, and rotation. Specifically:
  - `psurface`: Pointer to the surface where the texture will be rendered.
  - `resTexture`: Pointer to the texture resource to be rendered.
  - `x`: X-coordinate where the texture will be drawn on the surface.
  - `y`: Y-coordinate where the texture will be drawn on the surface.
  - `refPoint`: Alignment reference point (e.g., center, top-left, etc.).
  - `blend`: Pointer to a blend option structure (can be `NULL` if no blending is required).
  - `scale`: Scaling factor of the texture (1.0 indicates no scaling).
  - `Angle`: Rotation angle of the texture, in degrees.

Finally, it's time to write the function for creating a new object. Here, we need to use the `PX_ObjectCreateEx` function, which is used to create an extended object and initialize its properties and callback functions. The parameters are described as follows:

- `mp`: Pointer to the memory pool used for allocating the memory required by the object.
- `Parent`: Pointer to the parent object, or `NULL` if there is no parent.
- `x`: Initial position of the object on the x-axis.
- `y`: Initial position of the object on the y-axis.
- `z`: Initial position of the object on the z-axis, affecting the rendering order.
- `Width`: Width of the object.
- `Height`: Height of the object.
- `Lenght`: Length of the object, generally 0 for 2D objects.
- `type`: Type of the object.
- `Func_ObjectUpdate`: Pointer to the object's update function.
- `Func_ObjectRender`: Pointer to the object's render function.
- `Func_ObjectFree`: Pointer to the object's free function.
- `desc`: Pointer to the object's description data. You can set this to 0, and the function will zero-initialize the object type data during creation.
- `size`: Size of the description data, which is the size of the object structure type you defined. The object creation function will allocate a block of memory from the memory pool to store your object structure.

After creating an empty object, we use `PX_ObjectGetDescIndex` to extract the pointer to the object structure within the object. This is a three-parameter function; the first parameter is the object structure type, and the second parameter is the `PX_Object *` pointer type. Because a single `PX_Object` can combine multiple object structures, we will further describe this combined structure in later tutorials. For now, we only need to know that after calling the `PX_ObjectCreateEx` function, the index of the first stored object structure is 0.

After extracting the structure pointer, we perform a series of initializations, such as loading the image and initializing the rotation angle. Finally, in the `main` function, we create this object:

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
    PX_TextureRenderEx(psurface, &pMyObject->image, (px_int)pObject->x, (px_int)pObject->y, PX_ALIGN_CENTER, 0, 1, pMyObject->rotation); // Render the image
}

PX_OBJECT_FREE_FUNCTION(MyObjectFree)
{
    PX_Object_MyObject *pMyObject = PX_ObjectGetDesc(PX_Object_MyObject, pObject);
    PX_TextureFree(&pMyObject->image); // Free the image
}

PX_Object* PX_Object_MyObjectCreate(px_memorypool* mp, PX_Object* parent, px_float x, px_float y)
{
    PX_Object *pObject = PX_ObjectCreateEx(mp, parent, x, y, 0, 128, 128, 0, 0, MyObjectUpdate, MyObjectRender, MyObjectFree, 0, sizeof(PX_Object_MyObject)); // Create an empty custom object
    PX_Object_MyObject* pMyObject = PX_ObjectGetDescIndex(PX_Object_MyObject, pObject, 0); // Get the custom object data
    pMyObject->rotation = 0;
    if (!PX_LoadTextureFromFile(mp, &pMyObject->image, "assets/test.png")) // Load the image
    {
        PX_ObjectDelete(pObject); // Delete the object if loading fails
        return PX_NULL;
    }
    return pObject;
}

px_int main()
{
    PainterEngine_Initialize(800, 480);
    PX_Object_MyObjectCreate(mp, root, 400, 240); // Create a custom object
    return PX_TRUE;
}
```

The running effect is as follows:

![](assets/img/12.1.png)

But it's not over yet. How do we make our component respond to the middle mouse button to enable rotation? Do you remember the object passing mechanism in [PushButton](#8-painterengine-object-passing-mechanism)? Now, we also need to make our component respond to the middle mouse button information, so we register a callback function for the `PX_OBJECT_EVENT_CURSORWHEEL` event. The code is as follows:

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
    PX_TextureRenderEx(psurface, &pMyObject->image, (px_int)pObject->x, (px_int)pObject->y, PX_ALIGN_CENTER, 0, 1, pMyObject->rotation); // Render the image
}

PX_OBJECT_FREE_FUNCTION(MyObjectFree)
{
    PX_Object_MyObject *pMyObject = PX_ObjectGetDesc(PX_Object_MyObject, pObject);
    PX_TextureFree(&pMyObject->image); // Free the image
}

PX_OBJECT_EVENT_FUNCTION(MyObjectOnCursorWheel)
{
    PX_Object_MyObject *pMyObject = PX_ObjectGetDescIndex(PX_Object_MyObject, pObject, 0);
    if (PX_ObjectIsCursorInRegion(pObject, e)) // Check if the mouse cursor is over the component, e is the event
        pMyObject->rotation += (px_float)PX_Object_Event_GetCursorZ(e)/10;
}

PX_Object* PX_Object_MyObjectCreate(px_memorypool* mp, PX_Object* parent, px_float x, px_float y)
{
    PX_Object *pObject = PX_ObjectCreateEx(mp, parent, x, y, 0, 128, 128, 0, 0, MyObjectUpdate, MyObjectRender, MyObjectFree, 0, sizeof(PX_Object_MyObject)); // Create an empty custom object
    PX_Object_MyObject* pMyObject = PX_ObjectGetDescIndex(PX_Object_MyObject, pObject, 0); // Get the custom object data
    pMyObject->rotation = 0;
    if (!PX_LoadTextureFromFile(mp, &pMyObject->image, "assets/test.png")) // Load the image
    {
        PX_ObjectDelete(pObject); // Delete the object if loading fails
        return PX_NULL;
    }
    PX_ObjectRegisterEvent(pObject, PX_OBJECT_EVENT_CURSORWHEEL, MyObjectOnCursorWheel, 0); // Register the mouse wheel event
    return pObject;
}

px_int main()
{
    PainterEngine_Initialize(800, 480);
    PX_Object_MyObjectCreate(mp, root, 400, 240); // Create a custom object
    return PX_TRUE;
}
```

The running result is as follows:

![](assets/img/12.2.gif)

If you think the quality of the rotated image is poor and there are many jagged edges, this is because `PX_TextureRenderEx` directly samples the original image when rotating. If you want a high-quality rotated image, you can replace the original function with the `PX_TextureRenderRotation` function:

```c
PX_OBJECT_RENDER_FUNCTION(MyObjectRender)
{
    PX_Object_MyObject *pMyObject = PX_ObjectGetDesc(PX_Object_MyObject, pObject);
    PX_TextureRenderRotation(psurface, &pMyObject->image, (px_int)pObject->x, (px_int)pObject->y, PX_ALIGN_CENTER, 0, pMyObject->rotation); // Render the image
}

```

![](assets/img/12.3.gif)

So, how do we implement the dragging effect? To achieve the dragging effect, we need to add `float` type variables `x` and `y` to the object structure to record the position of the mouse when the image is selected. We also add a `bool` type variable `bselect` to indicate whether the current icon is selected. When the mouse clicks on our icon, we can listen for the `PX_OBJECT_EVENT_CURSORDRAG` event, which is generated when the mouse is dragged on the screen. By offsetting the coordinates, we move our component. Finally, whether the mouse is moved without dragging or the left mouse button is released, it will cancel the selection state of our component. Simply unselect the component in the corresponding handler functions.

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
    PX_TextureRenderRotation(psurface, &pMyObject->image, (px_int)pObject->x, (px_int)pObject->y, PX_ALIGN_CENTER, 0, (px_int)pMyObject->rotation); // Render the image
}

PX_OBJECT_FREE_FUNCTION(MyObjectFree)
{
    PX_Object_MyObject *pMyObject = PX_ObjectGetDesc(PX_Object_MyObject, pObject);
    PX_TextureFree(&pMyObject->image); // Free the image
}

PX_OBJECT_EVENT_FUNCTION(MyObjectOnCursorWheel)
{
    PX_Object_MyObject *pMyObject = PX_ObjectGetDescIndex(PX_Object_MyObject, pObject, 0);
    if (PX_ObjectIsCursorInRegionAlign(pObject, e, PX_ALIGN_CENTER)) // Check if the mouse cursor is over the component, e is the event
        pMyObject->rotation += (px_float)PX_Object_Event_GetCursorZ(e)/10;
}

PX_OBJECT_EVENT_FUNCTION(MyObjectOnCursorDown)
{
    PX_Object_MyObject* pMyObject = PX_ObjectGetDescIndex(PX_Object_MyObject, pObject, 0);
    if (PX_ObjectIsCursorInRegionAlign(pObject, e, PX_ALIGN_CENTER)) // Check if the mouse cursor is over the component, e is the event
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
    PX_Object *pObject = PX_ObjectCreateEx(mp, parent, x, y, 0, 128, 128, 0, 0, MyObjectUpdate, MyObjectRender, MyObjectFree, 0, sizeof(PX_Object_MyObject)); // Create an empty custom object
    PX_Object_MyObject* pMyObject = PX_ObjectGetDescIndex(PX_Object_MyObject, pObject, 0); // Get the custom object data
    pMyObject->rotation = 0;
    if (!PX_LoadTextureFromFile(mp, &pMyObject->image, "assets/test.png")) // Load the image
    {
        PX_ObjectDelete(pObject); // Delete the object if loading fails
        return PX_NULL;
    }
    PX_ObjectRegisterEvent(pObject, PX_OBJECT_EVENT_CURSORWHEEL, MyObjectOnCursorWheel, 0); // Register the mouse wheel event
    PX_ObjectRegisterEvent(pObject, PX_OBJECT_EVENT_CURSORDRAG, MyObjectOnCursorDrag, 0); // Register the mouse drag event
    PX_ObjectRegisterEvent(pObject, PX_OBJECT_EVENT_CURSORDOWN, MyObjectOnCursorDown, 0); // Register the mouse down event
    PX_ObjectRegisterEvent(pObject, PX_OBJECT_EVENT_CURSORUP, MyObjectOnCursorRelease, 0); // Register the mouse up event
    PX_ObjectRegisterEvent(pObject, PX_OBJECT_EVENT_CURSORMOVE, MyObjectOnCursorRelease, 0); // Register the mouse up event
    return pObject;
}

px_int main()
{
    PainterEngine_Initialize(800, 480);
    PX_Object_MyObjectCreate(mp, root, 400, 240); // Create a custom object
    return PX_TRUE;
}
```
![](assets/img/12.4.gif)

Of course, you can call `PX_Object_MyObjectCreate` multiple times to create multiple component objects, and they will all have the same functionality:

![](assets/img/12.5.gif)

## 13. Composite Component Design

PainterEngine's components allow for the simultaneous possession of multiple component types, for example, when we combine an image box component with a button, we can get a composite component image button.

Refer to the following code:

```c
#include "PainterEngine.h"
px_texture tex1, tex2;
PX_Object* image;

PX_OBJECT_EVENT_FUNCTION(ButtonEvent)
{
    PX_Object_Image *pImage = PX_Object_GetImage(pObject); // Get the Image object data
    PX_Object_Button *pButton = PX_Object_GetButton(pObject); // Get the Button object data
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
    if (!PX_LoadTextureFromFile(mp_static, &tex1, "assets/1.png")) return 0; // Load texture 1
    if (!PX_LoadTextureFromFile(mp_static, &tex2, "assets/2.png")) return 0; // Load texture 2
    image = PX_Object_ImageCreate(mp, root, 300, 140, 200, 200, &tex1); // Create an Image object
    PX_Object_ButtonAttachObject(image, 1, PX_COLOR(64, 255, 255, 255), PX_COLOR(96, 255, 255, 255)); // Attach a Button object type to the Image object
    PX_ObjectRegisterEvent(image, PX_OBJECT_EVENT_EXECUTE, ButtonEvent, 0); // Here we actually register the Button object's event
    return 1;
}
```

We created an Image image box type, then attached a Button object type to it, thus obtaining an image button:

![](assets/img/13.1.gif)

So, how do we design our own composable objects? Let's go back to our twelfth chapter, and turn the "draggable" feature into a composite component.

Firstly, still define a component object structure. To implement the drag function, we need the x, y coordinates when the mouse is pressed, as well as a bool type var to record whether it is selected, then we need to register `CURSOR` events, these events were already written in the previous chapter. Finally, we use the `PX_ObjectCreateDesc` function to create an object structure and attach it to our object.

`PX_ObjectCreateDesc` is an object structure creation function, its prototype definition is as follows:

```c
px_void* PX_ObjectCreateDesc(PX_Object* pObject, px_int idesc, px_int type, Function_ObjectUpdate Func_ObjectUpdate, Function_ObjectRender Func_ObjectRender, Function_ObjectFree Func_ObjectFree, px_void* pDesc, px_int descSize)
```

The first parameter is the object that needs to be Attached, the second parameter is the index of the object being Attached to. Do you remember the object data index we mentioned before? Using `PX_ObjectCreateEx` defaults to using index 0, so if we want to attach to an object, we should choose 1. Of course if 1 is also occupied, it would be 2, and so on. The third parameter is the object type, we can retrieve the corresponding pointer through the object type when using `PX_ObjectGetDescByType`, followed by the familiar `Update`, `Render`, `Free` trio. The last parameter provides its structure description and structure size. Please refer to the following code:

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
    if (!PX_LoadTextureFromFile(mp_static, &tex1, "assets/1.png")) return 0; // Load texture 1
    image = PX_Object_ImageCreate(mp, root, 300, 140, 200, 200, &tex1); // Create an Image object
    PX_Object_DragAttachObject(image, 1); // Attach a Drag object type to the Image object
    return 1;
}
```

The running result is as follows:

![](assets/img/13.2.gif)

## 14. Particle System

PainterEngine provides an implementation of a particle system. Below is a demonstration program for a particle system:

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

This is a particle system implementation wrapped in a component. Another approach offers more detailed configuration parameters for the particle system:

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

Below is the main functionality and flow explanation of this code:

1. `#include "PainterEngine.h"`: Includes the PainterEngine header file to enable the use of engine features.
2. `px_texture texture;`: Declares a variable named `texture` to store texture information.
3. `int main()`: Entry point of the main function.
4. `PX_Object* pObject;`: Declares a pointer to `PX_Object` type named `pObject`, which will be used to create a particle system object.
5. `PX_ParticalLauncher_InitializeInfo ParticalInfo;`: Declares a structure variable named `ParticalInfo` to configure the initialization information of the particle launcher.
6. `PainterEngine_Initialize(600, 400);`: Initializes PainterEngine, setting the window width to 600 pixels and height to 400 pixels.
7. `PX_LoadTextureFromFile(mp_static, &texture, "assets/star.traw");`: Loads a texture from a file, storing the texture data in the `texture` variable. The texture file path is "assets/star.traw".
8. `PX_ParticalLauncherInitializeDefaultInfo(&ParticalInfo);`: Initializes the `ParticalInfo` structure, setting some default properties of the particle launcher.
9. Specific configurations are made for various attributes of `ParticalInfo`, including the position, velocity, lifetime, size, and rotation of particles. These attributes determine the appearance and behavior of particles.
10. `pObject = PX_Object_ParticalCreate(mp, root, 300, 200, ParticalInfo);`: Creates a particle system object using the configured `ParticalInfo` and stores it in `pObject`. This particle system object will emit particles at the position (300, 200) in the window.

The `PX_ParticalLauncher_InitializeInfo` is used to configure the initialization information of the particle launcher. When creating a particle system, you can specify various attributes and behaviors of the particle system by filling out this structure. Here is an explanation of the members of this structure:

1. `px_void *userptr;`: A pointer to any type of data, which can be used to store user-defined data.
2. `px_texture *tex;`: A pointer to texture data, used to specify the texture image of the particles.
3. `px_point position;`: A point containing x, y, z coordinates, representing the initial position of the particle system.
4. `px_float deviation_position_distanceRange;`: A floating-point number specifying the range of position offset for particles.
5. `px_point direction;`: A point containing x, y, z coordinates, indicating the initial motion direction of the particles.
6. `px_float deviation_rangAngle;`: A floating-point number specifying the range of initial motion direction offset (in degrees) for particles.
7. `px_float velocity;`: A floating-point number representing the initial velocity of the particles.
8. `px_float deviation_velocity_max;`: A floating-point number representing the maximum velocity offset value for particles.
9. `px_float deviation_velocity_min;`: A floating-point number representing the minimum velocity offset value for particles.
10. `px_float atomsize;`: A floating-point number representing the initial size of the particles.
11. `px_float deviation_atomsize_max;`: A floating-point number representing the maximum size offset value for particles.
12. `px_float deviation_atomsize_min;`: A floating-point number representing the minimum size offset value for particles.
13. `px_float rotation;`: A floating-point number representing the initial rotation angle of the particles.
14. `px_float deviation_rotation;`: A floating-point number representing the range of rotation angle offset for particles.
15. `px_float alpha;`: A floating-point number representing the initial transparency of the particles.
16. `px_float deviation_alpha;`: A floating-point number representing the range of transparency offset for particles.
17. `px_float hdrR;`: A floating-point number representing the initial red channel value of the particles.
18. `px_float deviation_hdrR;`: A floating-point number representing the range of red channel value offset for particles.
19. `px_float hdrG;`: A floating-point number representing the initial green channel value of the particles.
20. `px_float deviation_hdrG;`: A floating-point number representing the range of green channel value offset for particles.
21. `px_float hdrB;`: A floating-point number representing the initial blue channel value of the particles.
22. `px_float deviation_hdrB;`: A floating-point number representing the range of blue channel value offset for particles.
23. `px_float sizeincrease;`: A floating-point number representing the rate of increase in particle size.
24. `px_float alphaincrease;`: A floating-point number representing the rate of increase in particle transparency.
25. `px_point a;`: A point containing x, y, z coordinates, used for custom attributes.
26. `px_float ak;`: A floating-point number used for custom attributes.
27. `px_int alive;`: An integer representing the lifetime of particles (in milliseconds).
28. `px_int generateDuration;`: An integer representing the generation period of the particle launcher (in milliseconds).
29. `px_int maxCount;`: An integer representing the maximum number of particles in the particle system.
30. `px_int launchCount;`: An integer representing the number of launches of the particle system.
31. `PX_ParticalLauncher_CreateAtom Create_func;`: A function pointer specifying a custom particle creation function.
32. `PX_ParticalLauncher_UpdateAtom Update_func;`: A function pointer specifying a custom particle update function.

This structure allows you to flexibly configure various attributes of the particle system to meet the needs of different scenarios and effects. By adjusting these attributes, you can control aspects of particle appearance, trajectory, lifecycle, and other behaviors.

![](assets/img/14.2.gif)

## 15. Playing Music with PainterEngine

PainterEngine natively supports playing music in wav and mp3 formats. The code to play music using PainterEngine is very simple:

```c
#include "PainterEngine.h"

PX_SoundData sounddata; // Define the music format
int main()
{
    PX_Object* pObject;
    PainterEngine_Initialize(600, 400);
    PainterEngine_InitializeAudio(); // Initialize the mixer and audio device
    if (!PX_LoadSoundFromFile(mp_static, &sounddata, "assets/bliss.wav")) return PX_FALSE; // Load the music, supporting wav and mp3 formats
    PX_SoundPlayAdd(soundplay, PX_SoundCreate(&sounddata, PX_TRUE)); // Play the music
    pObject = PX_Object_SoundViewCreate(mp, root, 0, 0, 600, 400, soundplay); // Optional music spectrum visualization component
    return 0;
}
```

![](assets/img/15.1.gif)

Here, the `PX_LoadSoundFromFile` function loads music from a file and decodes it into the `sounddata` type. `PX_SoundCreate` can create a playback instance using `sounddata`. The second parameter indicates whether this instance should loop. Finally, `PX_SoundPlayAdd` sends the playback instance to the mixer to complete the music playback.

## 16. PainterEngine Live2D Animation System

PainterEngine includes a Live2D-like animation system that can load Live2D animations. The reference code is as follows:

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
    // Load model data
    iodata = PX_LoadFileToIOData("assets/release.live");
    if (iodata.size == 0) return PX_FALSE;
    PX_LiveFrameworkImport(mp_static, &liveframework, iodata.buffer, iodata.size);
    PX_FreeIOData(&iodata);
    // Create a Live2D object
    pObject = PX_Object_Live2DCreate(mp, root, 300, 300, &liveframework);
    PX_ObjectRegisterEvent(pObject, PX_OBJECT_EVENT_CURSORDOWN, onClick, PX_NULL);

    return 0;
}
```

Below are explanations of functions related to the Live2D model previewer:

`PX_Object_Live2DCreate`

```c
PX_Object* PX_Object_Live2DCreate(px_memorypool* mp, PX_Object* Parent, px_int x, px_int y, PX_LiveFramework *pLiveFramework);
```

- **Description**: Creates a Live2D model previewer object for displaying and interacting with Live2D models in the graphical interface.
- **Parameters**:
  - `mp`: Pointer to the memory pool used for memory allocation.
  - `Parent`: Parent object; the Live2D model previewer object will be a child of this object.
  - `x`, `y`: Position coordinates of the Live2D model previewer object.
  - `pLiveFramework`: Pointer to the Live2D model framework, which includes model data, textures, and other information.
- **Return Value**: Pointer to the created Live2D model previewer object.

`PX_Object_Live2DPlayAnimation`

```c
px_void PX_Object_Live2DPlayAnimation(PX_Object *pObject, px_char *name);
```

- **Description**: Plays a specified animation of the Live2D model.
- **Parameters**:
  - `pObject`: Pointer to the Live2D model previewer object.
  - `name`: Name of the animation.
- **Return Value**: None.

`PX_Object_Live2DPlayAnimationRandom`

```c
px_void PX_Object_Live2DPlayAnimationRandom(PX_Object* pObject);
```

- **Description**: Plays a random animation of the Live2D model.
- **Parameters**:
  - `pObject`: Pointer to the Live2D model previewer object.
- **Return Value**: None.

`PX_Object_Live2DPlayAnimationIndex`

```c
px_void PX_Object_Live2DPlayAnimationIndex(PX_Object* pObject, px_int index);
```

- **Description**: Plays the animation at the specified index of the Live2D model.
- **Parameters**:
  - `pObject`: Pointer to the Live2D model previewer object.
  - `index`: Index of the animation.
- **Return Value**: None.

These functions are used to create, configure, and manage Live2D model previewer objects to display and interact with Live2D models in the graphical user interface. You can use these functions to play animations of the Live2D model, including those specified by name, randomly selected, and at a specific index.

![](assets/img/16.1.gif)

## 17. PainterEngine Script Engine

PainterEngine includes a platform-independent script engine system that integrates compilation, execution, debugging, and other functionalities. You can easily implement parallel scheduling on top of scripts. The design of PainterEngine Script maintains maximum consistency with the C language and extends and simplifies some types.

For example, in the script, four types are supported: `int`, `float`, `string`, and `memory`. The `int` type is a 32-bit signed integer, and the `float` type is a floating-point number, both of which are consistent with C language types. The `string` type is similar to C++'s `string`, allowing direct concatenation of strings using the `+` operator and obtaining the string length using `strlen`. The `memory` type is a binary data storage type that also supports concatenation using the `+` operator.

If you need to call C language functions in the script, you should define them using the `PX_VM_HOST_FUNCTION` macro. Similar to component callback functions, the definition of `PX_VM_HOST_FUNCTION` is as follows:

```c
#define PX_VM_HOST_FUNCTION(name) px_bool name(PX_VM *Ins, px_void *userptr)
```

In the following content, I will use a simple script example to demonstrate how to use PainterEngine's script engine:

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

First, the `shellcode` array stores a program that outputs a multiplication table, which requires calling two `host` functions (calling C language functions from scripts is called a host call, so host functions are actually C language functions specifically provided for script calls). One is the `print` function, and the other is the `sleep` function. Therefore, we define two `host` functions below. `PX_VM_HOSTPARAM` is used to retrieve parameters passed from the script. Here, we need to check if the passed parameters match our calling rules. For example, the `host_print` function is used to output strings in PainterEngine, while the `sleep` function is used to delay for a certain period.

Now, PainterEngine Script is a compiled script. We need to compile the above code into binary form and then send it to the virtual machine for execution. Observe the following code:

```c
PX_VM vm;
PX_OBJECT_UPDATE_FUNCTION(VMUpdate)
{
    PX_VMRun(&vm, 0xffff, elapsed); // Run the virtual machine
}

px_int main()
{
    PX_Compiler compiler;
    px_memory bin;
    PainterEngine_Initialize(800, 600);
    PainterEngine_SetBackgroundColor(PX_COLOR_BLACK);
    PX_CompilerInitialize(mp, &compiler); // Initialize the compiler
    PX_CompilerAddSource(&compiler, shellcode); // Add code to the compiler
    PX_MemoryInitialize(mp, &bin); // Initialize memory for storing the compiled result
    if (!PX_CompilerCompile(&compiler, &bin, 0, "main"))
    {
        // Compilation failed
        return 0;
    }
    PX_CompilerFree(&compiler); // Free the compiler
    PX_VMInitialize(&vm, mp, bin.buffer, bin.usedsize); // Initialize the virtual machine
    PX_VMRegisterHostFunction(&vm, "print", host_print, 0); // Register the host function print
    PX_VMRegisterHostFunction(&vm, "sleep", host_sleep, 0); // Register the host function sleep
    PX_VMBeginThreadFunction(&vm, 0, "main", PX_NULL, 0); // Start running the virtual machine function
    PX_ObjectSetUpdateFunction(root, VMUpdate, 0); // Set the update function

    return 0;
}
```

First, we compile our script using `PX_Compiler`, then register our host calls. The function `PX_VMBeginThreadFunction` is used to call a function in the script language from C. Here, we call the `main` function in the script to start running our script function. Finally, we bind an `Update` function to the root node to cyclically update the virtual machine and execute the script.

Finally, let's look at the result of the run.

![](assets/img/17.1.gif)

If we want to debug the script, we can create a symbol mapping table during compilation. This way, we can directly use `PX_Object_DebuggerMap` to debug the script.

```c
px_int main()
{
    PX_Compiler compiler;
    px_memory bin;
    PainterEngine_Initialize(800, 480);
    PX_VMDebuggerMapInitialize(mp, &debugmap);
    PainterEngine_SetBackgroundColor(PX_COLOR_BLACK);
    PX_CompilerInitialize(mp, &compiler); // Initialize the compiler
    PX_CompilerAddSource(&compiler, shellcode); // Add code to the compiler
    PX_MemoryInitialize(mp, &bin); // Initialize memory for storing the compiled result
    if (!PX_CompilerCompile(&compiler, &bin, &debugmap, "main"))
    {
        // Compilation failed
        return 0;
    }
    PX_CompilerFree(&compiler); // Free the compiler
    PX_VMInitialize(&vm, mp, bin.buffer, bin.usedsize); // Initialize the virtual machine
    PX_VMRegisterHostFunction(&vm, "print", host_print, 0); // Register the host function print
    PX_VMRegisterHostFunction(&vm, "sleep", host_sleep, 0); // Register the host function sleep
    PX_VMBeginThreadFunction(&vm, 0, "main", PX_NULL, 0); // Start running the virtual machine function
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

