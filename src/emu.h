#pragma once
#include "emu-8080.h"
// 100 ms
#define CPU_8080_HZ 2000000
#define TICK_INTERVAL 100
#define CPU_8080_CLOCKS_PER_TICK (CPU_8080_HZ / TICK_INTERVAL)  


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
struct EventQueue {
    struct Event event[EVENT_QUEUE_LENGTH];
    int event_count;
    int event_tail;
    int event_head;
};

struct Emulator
{
    struct Context *context;
    int clock_ticks;
    int int_count;
    char memory[16768];
    struct EventQueue event_queue;
};

struct Emulator *emu_new();
void emu_set_mem(struct Emulator *emulator, int pos, int length, char *data);
void emu_get_mem(struct Emulator *emulator, int pos, int length, char *buffer);
void emu_free(struct Emulator *emulator);
void emu_event_add(struct Emulator *emulator, struct Event event);
int emu_execute(struct Emulator *emulator, int clocks_ticks);
