#pragma once
#include <gtk/gtk.h>

#define SCREEN_WIDTH 256
#define SCREEN_HEIGHT 224
#define SCREEN_BUFFER_LOCATION 9216

void do_update_buffer(const char* buffer,  GdkPixbuf *pixbuf);
void update_pixbuffer(struct Emulator *emu, GdkPixbuf *pixbuf);
int load_invaders(char *buffer);
