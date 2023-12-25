/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */

/// #include <config.h>
#include <stdio.h>
#include <time.h>
#include <gtk/gtk.h>
#include <glib/gi18n.h>
#include <gtk/gtktypes.h>
#include <limits.h>
#include "emu.h"
#include "emu-screen.h"
#include "utils.h"
#include "emu-keyboard.h"
#include "emu-ports.h"

static GdkPixbuf *pixbuf;
static GtkImage *screenImage;
static gint64 emu_cycle_last_run;
Emulator *emu;

// called when window is closed
/*
static void on_window_main_destroy(GtkWidget *window, GApplication *app)
{
    gtk_widget_destroy(window);
    g_application_quit(app);
}
*/
static gboolean emu_key_function(GtkWidget *gtkWidget, GdkEventKey *event, gpointer data)
{
    (void)gtkWidget;
    Emulator *emu = (Emulator *)data;
    return emu_handle_keyboard(emu, event->keyval, event->type == GDK_KEY_PRESS);
}

/*
    Will be called periodically to run another time slice of the emulator and
    to update the screen from the emulator screen buffer memory
*/
static gboolean emulation_update(gpointer user_data)
{
    Emulator *em = (Emulator *)user_data;
    gint64 now = g_get_monotonic_time() / 1000;
    gint64 diff = now - emu_cycle_last_run; // find how much time we need to cover in miliseconds
    int cycles = (CPU_8080_HZ / 1000) * diff;
    if (em)
    {
        emu_execute(em, cycles);
    }
    emu_cycle_last_run = now;
    // update the screen
    update_pixbuffer(em, pixbuf);
    gtk_image_set_from_pixbuf(screenImage, pixbuf);

    //   printf("\rtime elapsed %li, %i                               \r", diff, cycles);
    return TRUE;
    ;
}

static void on_app_activate(GtkApplication *app, GtkBuilder *builder)
{
    gtk_builder_add_from_file(builder, "invaders.glade", NULL);
    gtk_builder_connect_signals(builder, NULL);

    GtkWidget *window = GTK_WIDGET(gtk_builder_get_object(builder, "gtk_main_window"));
    gtk_widget_add_events(window, GDK_KEY_PRESS_MASK | GDK_KEY_RELEASE_MASK);
    g_signal_connect(G_OBJECT(window), "key_press_event", G_CALLBACK(emu_key_function), emu);
    g_signal_connect(G_OBJECT(window), "key_release_event", G_CALLBACK(emu_key_function), emu);

    gtk_application_add_window(app, (GtkWindow *)(window));
    screenImage = GTK_IMAGE(gtk_builder_get_object(builder, "ID_SCREEN_IMAGE"));
    gtk_widget_show_all(GTK_WIDGET(window));
}

int main(int argc, char *argv[])
{
    // create new GtkApplication with an unique application ID
#if GLIB_CHECK_VERSION(2, 74, 0)
   GtkApplication *app = gtk_application_new("valleyco.emu.i8080", G_APPLICATION_DEFAULT_FLAGS);
#else
	GtkApplication *app=gtk_application_new(NULL,G_APPLICATION_FLAGS_NONE);
#endif
    GtkBuilder *builder = gtk_builder_new();
    emu = emu_new();

    g_signal_connect(app, "activate", G_CALLBACK(on_app_activate), builder);

    pixbuf = gdk_pixbuf_new(GDK_COLORSPACE_RGB, 0, 8, SCREEN_HEIGHT, SCREEN_WIDTH); // we rotate the image so we swap the screen dimensions

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
