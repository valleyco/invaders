#pragma once
#include <stdlib.h>
#include <stddef.h>
#include "emu-8080.h"
#include "emu-shifter.h"
#include "emu-keyboard.h"
#include "emu-sound.h"

// 100 ms
#define CPU_8080_HZ 2000000
#define TICK_INTERVAL 100
#define CPU_8080_CLOCKS_PER_TICK (CPU_8080_HZ / TICK_INTERVAL)
#define CYCLES_PER_SCREEN_INTERRUPT ((int)(CPU_8080_HZ / 120))

enum event_types
{
    EVENT_KB
};

struct Event
{
    enum event_types type;
    int data;
};

#define EVENT_QUEUE_LENGTH 16
struct EventQueue
{
    struct Event event[EVENT_QUEUE_LENGTH];
    int event_count;
    int event_tail;
    int event_head;
};

typedef struct
{
    struct Context *context;
    int clock_ticks;
    unsigned char memory[65536];
    char port[8];
    PortDevice *dev_read[256];
    PortDevice *dev_write[256];
    struct EventQueue event_queue;
    int screen_int_count;
    int screen_int_half;
    KeyboardDevice *kbDevice;
    ShifterDevice *shiftDevice;
    SoundDevice *soundDevice;
} Emulator;

Emulator *emu_new();
void emu_free(Emulator *emulator);
void emu_event_add(Emulator *emulator, struct Event event);
int emu_execute(Emulator *emulator, int clocks_ticks);
int emu_handle_keyboard(Emulator *emulator, int keyVal, int isPressed);
