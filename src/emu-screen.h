#pragma once
#include <gtk/gtk.h>
#include <stddef.h>

#define SCREEN_WIDTH 256
#define SCREEN_HEIGHT 224
#define SCREEN_BUFFER_LOCATION 0x2400
typedef struct {
    int ticks;
    int* intr;
} ScreenDevice;

// void do_update_buffer(const unsigned char *buffer, GdkPixbuf *pixbuf);
void update_pixbuffer(Emulator *emu, GdkPixbuf *pixbuf);

PortDevice *emu_screen_init(int* intr);