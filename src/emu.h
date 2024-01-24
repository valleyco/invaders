#pragma once
#include <stdlib.h>
#include <stdbool.h>
#include <stddef.h>
#include "emu-8080.h"
#include "emu-shifter.h"
#include "emu-keyboard.h"
#include "emu-sound.h"

// 100 ms
#define CPU_8080_HZ 2000000
#define TICK_INTERVAL 1000
#define CPU_8080_CLOCKS_PER_TICK (CPU_8080_HZ / TICK_INTERVAL)
#define CYCLES_PER_SCREEN_INTERRUPT ((int)(CPU_8080_HZ / 120))

#define MAX_DEVICES 10

typedef struct
{
    struct Context *context;
    int clock_ticks;
    unsigned char memory[65536];
    int devices_count;
    int intr;
    PortDevice *devices[MAX_DEVICES];
    PortDevice *dev_ticks[MAX_DEVICES];
    PortDevice *dev_read[256];
    PortRead dev_read_handler[256];
    PortDevice *dev_write[256];
    PortWrite dev_write_handler[256];
    KeyEvent key_event_handler;
    PortDevice *key_event_device;
    int dev_ticks_count;

} Emulator;

Emulator *emu_new();
void emu_free(Emulator *emulator);
int emu_execute(Emulator *emulator, int clocks_ticks);
int emu_handle_keyboard(Emulator *emulator, int keyVal, int isPressed);
