#include "gui.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <linux/fb.h>
#include <errno.h>

Gui_Context *gui_init()
{
    Gui_Context *gui = (Gui_Context *)malloc(sizeof(Gui_Context));
    if (gui == NULL)
    {
        return NULL;
    }

    gui->fd = open("/dev/fb0", O_RDWR);
    if (gui->fd == -1)
    {
        free(gui);
        return NULL;
    }

    struct fb_var_screeninfo vinfo;
    if (ioctl(gui->fd, FBIOGET_VSCREENINFO, &vinfo))
    {
        close(gui->fd);
        free(gui);
        return NULL;
    }

    gui->xres = vinfo.xres;
    gui->yres = vinfo.yres;
    gui->bpp = vinfo.bits_per_pixel;
    gui->line_length = vinfo.xres_virtual * vinfo.bits_per_pixel / 8;
    gui->size = gui->line_length * vinfo.yres_virtual;

    gui->framebuffer = (char *)mmap(NULL, gui->size, PROT_READ | PROT_WRITE, MAP_SHARED, gui->fd, 0);
    if (gui->framebuffer == MAP_FAILED)
    {
        close(gui->fd);
        free(gui);
        return NULL;
    }

    return gui;
}

void clear_screen(Gui_Context *gui)
{
    if (!gui)
        return;
    memset(gui->framebuffer, 0, gui->size);
}

void draw_pixel(Gui_Context *gui, int x, int y, uint32_t color)
{
    if (!gui)
        return;

    if (x < 0 || x >= gui->xres || y < 0 || y >= gui->yres)
    {
        return;
    }

    int location = x * (gui->bpp / 8) + y * gui->line_length;
    *((uint32_t *)(gui->framebuffer + location)) = color;
}

void draw_rect(Gui_Context *gui, int x, int y, int width, int height, uint32_t color)
{
    if (!gui)
        return;

    for (int i = x; i < x + width; i++)
    {
        for (int j = y; j < y + height; j++)
        {
            draw_pixel(gui, i, j, color);
        }
    }
}

void gui_deinit(Gui_Context *gui)
{
    if (!gui)
        return;

    munmap(gui->framebuffer, gui->size);
    close(gui->fd);
    free(gui);
}

#ifdef GUI_TEST
int main(int argc, char *argv[])
{
    Gui_Context *gui = gui_init();
    if (!gui)
    {
        perror("gui_init failed");
        return -1;
    }

    clear_screen(gui);
    draw_rect(gui, 100, 100, 200, 200, COLOR_BLUE);

    sleep(5);

    gui_deinit(gui);

    return 0;
}
#endif