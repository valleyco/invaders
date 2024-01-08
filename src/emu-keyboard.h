#pragma once
#include <stdio.h>
#include <time.h>
#include <gtk/gtk.h>
#include <glib/gi18n.h>
#include <gtk/gtktypes.h>
#include "emu-device.h"

#define KEY_SHOT 1
#define KEY_LEFT 2
#define KEY_RIGHT 3
#define KEY_P1_START 4
#define KEY_P2_START 5
#define KEY_P1_SHOT 6
#define KEY_P1_LEFT 7
#define KEY_P1_RIGHT 8
#define KEY_P2_SHOT 9
#define KEY_P2_LEFT 10
#define KEY_P2_RIGHT 11
#define KEY_DIP_3 12
#define KEY_DIP_4 13
#define KEY_DIP_5 14
#define KEY_DIP_6 15
#define KEY_DIP_7 16
#define KEY_CREDIT 17
#define KEY_TILT 18
#define KEY_VIRTUAL_ON 19
#define KEY_VIRTUAL_OFF 20

#define KEY_MAX_ID KEY_VIRTUAL_OFF

typedef struct // this first field should match struct PortDevice
{
    int portCount;
    int portOffset;
    PortRead *read;
    PortWrite *write;
    int key_status[KEY_MAX_ID + 1];
} KeyboardDevice;

int handle_keyboard_event(KeyboardDevice *device, int keyVal, int pressed);
KeyboardDevice *emu_keyboard_init();
void emu_keyboard_done(KeyboardDevice *dev);