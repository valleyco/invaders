#pragma once
#include <gtk/gtk.h>
#include <stddef.h>

#define SCREEN_WIDTH 256 
#define SCREEN_HEIGHT 224 
#define SCREEN_BUFFER_LOCATION 0x2400

//void do_update_buffer(const unsigned char *buffer, GdkPixbuf *pixbuf);
void update_pixbuffer(struct Emulator *emu, GdkPixbuf *pixbuf);
size_t load_invaders(unsigned char *buffer);
