#ifndef GUI_H
#define GUI_H

#include <stdint.h>

#define COLOR_RED 0xff0000
#define COLOR_GREEN 0x00ff00
#define COLOR_BLUE 0x0000ff
#define COLOR_BLACK 0x000000
#define COLOR_WHITE 0xffffff

typedef struct
{
    int fd;            // File descriptor for /dev/fb0
    char *framebuffer; // Pointer to framebuffer
    int size;          // Size of framebuffer
    int xres;          // X resolution
    int yres;          // Y resolution
    int bpp;           // Bits per pixel
    int line_length;   // Line length
} Gui_Context;

/**
 * @brief Initialize the GUI
 * @return Gui_Context* Pointer to the GUI context
 *         NULL if failed to initialize
 */
Gui_Context *gui_init();

/**
 * @brief Deinitialize the GUI
 * @param gui Pointer to the GUI context
 */
void clear_screen(Gui_Context *gui);

/**
 * @brief Draw a pixel on the screen
 * @param gui Pointer to the GUI context
 * @param x X coordinate
 * @param y Y coordinate
 * @param color Color of the pixel
 */
void draw_pixel(Gui_Context *gui, int x, int y, uint32_t color);

/**
 * @brief Draw a rectangle on the screen
 */
void draw_rect(Gui_Context *gui, int x, int y, int width, int height, uint32_t color);

/**
 * @brief Deinitialize the GUI
 * @param gui Pointer to the GUI context
 */
void gui_deinit(Gui_Context *gui);

#endif // GUI_H