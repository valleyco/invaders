#include <stdlib.h>
#include "emu-8080.h"
#include "emu.h"
#include "emu-screen.h"

struct Emulator *emu_new()
{
    struct Emulator *emulator = (struct Emulator *)malloc(sizeof(struct Emulator));
    emulator->context = (struct Context *)malloc(sizeof(struct Context));
    emulator->clock_ticks = 0;
    emulator->int_count = 0;
    emulator->context->memory = emulator->memory;
    emulator->event_queue.event_head = emulator->event_queue.event_tail = emulator->event_queue.event_count = 0;
    load_invaders(emulator->memory + SCREEN_BUFFER_LOCATION);
    return emulator;
}

void emu_set_mem(struct Emulator *emulator, int pos, int length, char *data)
{
    for (int i = 0; i < length; i++)
    {
        emulator->memory[i + pos] = data[i];
    }
}

void emu_get_mem(struct Emulator *emulator, int pos, int length, char *buffer)
{
    for (int i = 0; i < length; i++)
    {
        buffer[i] = emulator->memory[i + pos];
    }
}

void emu_free(struct Emulator *emulator)
{
    free(emulator->context);
    free(emulator);
}

void emu_event_add(struct Emulator *emulator, struct Event event)
{
    if (emulator->event_queue.event_count == EVENT_QUEUE_LENGTH)
    {
        return;
    }
    emulator->event_queue.event_count++;
    emulator->event_queue.event[(emulator->event_queue.event_head++) % 0x0F] = event;
}
static void emu_event_drop(struct Emulator *emulator)
{
    emulator->event_queue.event_count--;
    emulator->event_queue.event_tail = (emulator->event_queue.event_tail + 1) % 0x0F;
}

static void emu_handle_events(struct Emulator *emulator)
{
    emu_event_drop(emulator);
}

int emu_execute(struct Emulator *emulator, int clocks_ticks)
{
    int ticks = 0;
    if(emulator->event_queue.event_count){
        emu_handle_events(emulator);
    }
    while (clocks_ticks < ticks)
    {
        emulator->int_count = (emulator->int_count + 1) % (CPU_8080_HZ / 60);
        if (emulator->int_count)
        {
            ticks += emu_8080_execute(emulator->context);
        } else { // 1/60 sec interrupt
            ticks += emu_8080_rst(emulator->context, 7);
        }
    }
    emulator->clock_ticks += ticks - clocks_ticks;
    return ticks;
}