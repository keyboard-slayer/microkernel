#include <unistd.h>
#include <cap.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

#include "../inc/font.h"

static size_t cur_x;
static size_t cur_y;


static void draw_pixel_fb(fb_t *self, size_t x, size_t y, uint8_t r, uint8_t g, uint8_t b)
{
    size_t index = x + (self->pitch / sizeof(uint32_t)) * y;
    ((uint32_t *) self->addr)[index] = r << 16 | g << 8 | b;
}

void tty_putc(fb_t *self, uint8_t c)
{
    if (c == '\n')
    {
        cur_y += FONT_HEIGHT * 1.5 ;
        cur_x = 0;

        return;
    }

    for (size_t y = 0; y < FONT_HEIGHT; y++)
    {
        for (size_t x = 0; x < FONT_WIDTH; x++)
        {
            if (tty_font[((c - ' ') * FONT_HEIGHT) + y] >> (FONT_WIDTH - x) & 1)
            {
                draw_pixel_fb(self, x + cur_x, y + cur_y, 0xff, 0xff, 0xff);
            }
        }

    }

    cur_x += FONT_WIDTH;
}

void tty_puts(fb_t *self, const char *s)
{
    while (*s)
    {
        tty_putc(self, *s);
        s++;
    }
}

__attribute__((noreturn)) int _start(void)
{
    fb_t *framebuffer = (fb_t *) syscall(SYS_CAP, CAP_FB);

    assert(framebuffer != NULL);
    assert(framebuffer->addr != 0);

    cur_x = 0;
    cur_y = 0;

    tty_puts(framebuffer, "Hello from the framebuffer !\nIt works !");
    // tty_putc(framebuffer, 0);

    for (;;);
    __builtin_unreachable();
}
