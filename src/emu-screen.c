/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
#include <stdio.h>
#include "emu.h"
#include "emu-screen.h"

static void color_rect(const GdkPixbuf *pixbuf, int x, int y, int w, int h, uint32_t color)
{
    const int pixbuf_n_channels = gdk_pixbuf_get_n_channels(pixbuf);
    const int pixbuf_rowstride = gdk_pixbuf_get_rowstride(pixbuf);
    const int pixbuf_rowstride_diff = pixbuf_rowstride - (w * pixbuf_n_channels);
    guchar *pixbuf_pixels = gdk_pixbuf_get_pixels(pixbuf);
    guchar *target = pixbuf_pixels + pixbuf_rowstride * y + x * pixbuf_n_channels;
    guchar *target_end = pixbuf_pixels + pixbuf_rowstride * (y + h) + (x + w) * pixbuf_n_channels;
    do
    {
        for (guchar *width_end = target + w * pixbuf_n_channels; target < width_end;)
        {
            uint32_t pixel = color;
            for (int s = 0; s < pixbuf_n_channels; s++)
            {
                *target &= pixel & 0xff;
                target++;
                pixel >>= 8;
            }
        }
        target += pixbuf_rowstride_diff;
    } while (target < target_end);
}

void do_update_buffer(const unsigned char *buffer, GdkPixbuf *pixbuf)
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

    guchar *pixbuf_pixels = gdk_pixbuf_get_pixels(pixbuf);
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
    color_rect(pixbuf, 0, 32, 224, 32, 0x0000FF);   // RED
    color_rect(pixbuf, 0, 184, 224, 56, 0x00FF00);  // GREEN
    color_rect(pixbuf, 16, 240, 118, 16, 0x00FF00); // GREEN
}

void update_pixbuffer(Emulator *emu, GdkPixbuf *pixbuf)
{
    do_update_buffer_flip(emu->memory + SCREEN_BUFFER_LOCATION, pixbuf);
}
