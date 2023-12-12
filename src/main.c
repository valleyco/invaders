/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */

///#include <config.h>
#include <stdio.h>
#include <time.h>
#include <gtk/gtk.h>
#include <glib/gi18n.h>
#include <gtk/gtktypes.h>
#include <limits.h>
#include "emu.h"
#include "emu-screen.h"
#include "utils.h"
// called when window is closed

static GdkPixbuf *pixbuf;
static GtkImage *screenImage;
static gint64 emu_cycle_last_run;
static char app_dir[PATH_MAX];

/*
static void on_window_main_destroy(GtkWidget *window, GApplication *app)
{
    gtk_widget_destroy(window);
    g_application_quit(app);
}
*/
/*
    Will be called periodically to run another time slice of the emulator and
    to update the screen from the emulator screen buffer memory
*/
static gboolean emulation_update(gpointer user_data)
{
    struct Emulator *em = (struct Emulator *)user_data;
    gint64 now = g_get_monotonic_time();
    gint64 diff = (now - emu_cycle_last_run) / 1000; // find how much time we need to cover in miliseconds
    int cycles = (CPU_8080_HZ * diff) / 1000;
    if (em)
    {
        emu_execute(em, cycles);
    }
    emu_cycle_last_run = now;
    // update the screen
    update_pixbuffer(em, pixbuf);
    gtk_image_set_from_pixbuf(screenImage, pixbuf);

    printf("time elapsed %li, %i           \r", diff, cycles);
    return TRUE;
}

static void on_app_activate(GtkApplication *app, GtkBuilder *builder)
{
    gtk_builder_add_from_file(builder, "invaders.glade", NULL);
    gtk_builder_connect_signals(builder, NULL);

    GtkWidget *window = GTK_WIDGET(gtk_builder_get_object(builder, "gtk_main_window"));
    gtk_application_add_window(app, (GtkWindow *)(window));
    screenImage = GTK_IMAGE(gtk_builder_get_object(builder, "ID_SCREEN_IMAGE"));
    gtk_widget_show_all(GTK_WIDGET(window));
}

int main(int argc, char *argv[])
{
    // create new GtkApplication with an unique application ID
    set_app_folder(argv[0],0);
    GtkApplication *app = gtk_application_new("valleyco.emu.i8080", G_APPLICATION_FLAGS_NONE);
    GtkBuilder *builder = gtk_builder_new();
    struct Emulator *emu = emu_new();

    g_signal_connect(app, "activate", G_CALLBACK(on_app_activate), builder);

    pixbuf = gdk_pixbuf_new(GDK_COLORSPACE_RGB, 0, 8, SCREEN_WIDTH, SCREEN_WIDTH);
    //pixbuf = gdk_pixbuf_new(GDK_COLORSPACE_RGB, 0, 8, SCREEN_HEIGHT, SCREEN_WIDTH);

    // start the application, terminate by closing the window
    // GtkApplication* is upcast to GApplication* with G_APPLICATION() macro
    g_timeout_add(100, emulation_update, emu);
    int status = g_application_run(G_APPLICATION(app), argc, argv);
    // deallocate the application object
    emu_free(emu);
    g_object_unref(pixbuf);
    g_object_unref(builder);
    g_object_unref(app);
    return status;
}
