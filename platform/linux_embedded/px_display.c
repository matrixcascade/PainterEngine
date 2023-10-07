#include "px_display.h"
int px_main_initializedisplay(int * px_main_screen_width, int* px_main_screen_height)
{
    int fbfd = 0;
    struct fb_var_screeninfo vinfo;

    fbfd = open("/dev/fb0", O_RDWR);
    if (!fbfd)
    {
        printf("Error: cannot open framebuffer device.\n");
        return 0;
    }

    if (ioctl(fbfd, FBIOGET_VSCREENINFO, &vinfo))
    {
        printf("Error reading variable information.\n");
        return 0;
    }

    *px_main_screen_width = vinfo.xres;
    *px_main_screen_height = vinfo.yres;

    printf("screen width:%d height:%d bpp:%d\n", vinfo.xres, vinfo.yres, vinfo.bits_per_pixel);

    if (vinfo.bits_per_pixel != 32)
    {
        printf("Error: not supported bits_per_pixel, it only supports 32 bit color\n");
        return 0;
    }

    return fbfd;
}

void px_main_displaygram(int fbfd, void* gram,int size)
{
    void *gram32 = (void*)mmap(0, size, PROT_READ | PROT_WRITE, MAP_SHARED, fbfd, 0);
    memcpy(gram32, gram, size);
    munmap(gram32, size);
}

