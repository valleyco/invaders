#include <stdlib.h>
#include "emu-8080.h"
#include "emu.h"
#include "emu-screen.h"
static int port_read(int p)
{
    return 0;
}
static void port_write(int p, int v)
{
}
struct Emulator *emu_new()
{
    struct Emulator *emulator = (struct Emulator *)malloc(sizeof(struct Emulator));
    emulator->context = (struct Context *)malloc(sizeof(struct Context));
    
    emulator->context->memory = emulator->memory;
    emulator->context->port_read = port_read;
    emulator->context->port_write = port_write;
    emulator->context->PC = 0;
    emulator->context->halt = 0;

    emulator->clock_ticks = 0;
    emulator->shift_register = 0;
    emulator->screen_int_count = CYCLES_PER_SCREEN_INTERRUPT;
    emulator->screen_int_half = 0;
    emulator->event_queue.event_head = emulator->event_queue.event_tail = emulator->event_queue.event_count = 0;
    emulator->port[0] = 0b00001111;
    emulator->port[1] = 0b00001000;
    emulator->port[2] = 0b00001011;

    // load_invaders(emulator->memory + SCREEN_BUFFER_LOCATION);
    load_rom(emulator, "../rom/invaders.rom");
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
    if (emulator->event_queue.event_count)
    {
        emu_handle_events(emulator);
    }
    int ticks = 0;
    while (ticks < clocks_ticks )
    {
        int cycles = emu_8080_execute(emulator->context);
        if(emulator->context->PC > 16000){
            printf("ERROR after %i ticks\n",ticks);
            exit(1);
        }
        emulator->screen_int_count -= cycles;
        ticks += cycles;
        if (emulator->screen_int_count < 0)
        {
            ticks += emu_8080_rst(emulator->context, emulator->screen_int_half ? 2 : 1);
            emulator->screen_int_half = 1 - emulator->screen_int_half;
            emulator->screen_int_count += CYCLES_PER_SCREEN_INTERRUPT;
        }
    }
    emulator->clock_ticks += ticks;
    return ticks;
}

size_t load_rom(struct Emulator *emulator, const char *filename)
{
    FILE *f = fopen(filename, "r");
    size_t count = fread(emulator->memory, 1, 8192, f);
    printf("rom loaded rom size=%li\n", count);
    fclose(f);
    return count;
}
