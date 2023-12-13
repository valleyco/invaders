/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
#include <stdio.h>
#include "emu.h"
#include "emu-screen.h"

static void do_update_buffer(const unsigned char *buffer, GdkPixbuf *pixbuf)
{
    const int pixbuf_n_channels = gdk_pixbuf_get_n_channels(pixbuf);
    const int pixbuf_rowstride_diff = gdk_pixbuf_get_rowstride(pixbuf) - (SCREEN_WIDTH * pixbuf_n_channels);
    const unsigned char *p_image = buffer;
    guchar *pixbuf_pixels = gdk_pixbuf_get_pixels(pixbuf);
    for (int r = 0; r < SCREEN_HEIGHT; r++)
    {
        for (int c = 0; c < (SCREEN_WIDTH / 8); c++)
        {
            for (int b = 0x80; b; b >>= 1)
            {
                guchar pixel = (*p_image & b) ? 255 : 0;
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

static void do_update_buffer_flip(const unsigned char *buffer, const GdkPixbuf *pixbuf)
{
    const unsigned char *p_image = buffer;

    const guchar *pixbuf_pixels = gdk_pixbuf_get_pixels(pixbuf);
    const int pixbuf_n_channels = gdk_pixbuf_get_n_channels(pixbuf);
    const int pixbuf_rowstride = gdk_pixbuf_get_rowstride(pixbuf);
    guchar *target = pixbuf_pixels + pixbuf_rowstride * (SCREEN_WIDTH - 1);
    for (int r = 0; r < SCREEN_HEIGHT; r++)
    {
        for (int c = 0; c < (SCREEN_WIDTH / 8); c++)
        {
            for (int b = 1; b < 0x100; b <<= 1)
            {
                const guchar pixel = (*p_image & b) ? 255 : 0;
                for (int s = 0; s < pixbuf_n_channels; s++)
                {
                    *target = pixel;
                    target++;
                }
                target -= (pixbuf_rowstride + pixbuf_n_channels);
            }
            p_image++;
        }
        target += pixbuf_rowstride * SCREEN_WIDTH + pixbuf_n_channels;
    }
}

void update_pixbuffer(struct Emulator *emu, GdkPixbuf *pixbuf)
{
    do_update_buffer_flip(emu->memory + SCREEN_BUFFER_LOCATION, pixbuf);
}
