/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
#include <stdio.h>
#include "emu.h"
#include "emu-screen.h"

void do_update_buffer(const char const *buffer, GdkPixbuf *pixbuf)
{
    const char const *p_image = buffer;

    guchar *pixbuf_pixels = gdk_pixbuf_get_pixels(pixbuf);
    int pixbuf_n_channels = gdk_pixbuf_get_n_channels(pixbuf);
    int pixbuf_rowstride_diff = gdk_pixbuf_get_rowstride(pixbuf) - (SCREEN_WIDTH * pixbuf_n_channels);
    for (int r = 0; r < SCREEN_HEIGHT ; r++)
    {
        for (int c = 0; c < (SCREEN_WIDTH / 8); c++)
        {
            for (int b = 0x80; b ; b >>= 1)
//            for (int b = 1; b < 0x100; b <<= 1)
            {
                int pixel = (*p_image & b) ? 255 : 0;
                for (int s = 0; s < pixbuf_n_channels; s++)
                {
                    *pixbuf_pixels = pixel;
                    pixbuf_pixels++;
                }
            }
            p_image++;
        }
        pixbuf_pixels += pixbuf_rowstride_diff;
    }
}

void update_pixbuffer(struct Emulator *emu, GdkPixbuf *pixbuf)
{
    do_update_buffer(emu->memory + SCREEN_BUFFER_LOCATION, pixbuf);
}

int load_invaders(char *buffer)
{
    FILE *f = fopen("invaders.bmp", "r");
    fseek(f, 62, SEEK_SET);
    return fread(buffer, 1, 7168, f);
}