#ifndef PX_DISPLAY_H
#define PX_DISPLAY_H
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <linux/fb.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <linux/input.h>


int px_main_initializedisplay(int* px_main_screen_width, int* px_main_screen_height);
void px_main_displaygram(int fbfd, void* gram, int size);
#endif // !PX_DISPLAY_H

