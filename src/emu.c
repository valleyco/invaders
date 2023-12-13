#include <stdlib.h>
#include <string.h>
#include "emu-8080.h"
#include "emu.h"
#include "emu-screen.h"
#include "emu-ports.h"
#include "emu-keyboard.h"
#include "emu-shifter.h"

struct Emulator *emu_new()
{
    struct Emulator *emulator = (struct Emulator *)malloc(sizeof(struct Emulator));
    emulator->context = (struct Context *)malloc(sizeof(struct Context));
    memset(emulator->dev_read, 0, 256 * sizeof(emulator->dev_read[0]));
    memset(emulator->dev_write, 0, 256 * sizeof(emulator->dev_write[0]));
    emulator->context->gData = (void *)emulator;
    emulator->context->memory = emulator->memory;
    emulator->context->port_read = port_read;
    emulator->context->port_write = port_write;
    emulator->context->PC = 0;
    emulator->context->halt = 0;

    emulator->kbDevice = emu_keyboard_init();
    emu_register_device(emulator, emulator->kbDevice, 0);
    emulator->shiftDevice = emu_shifter_init();
    emu_register_device(emulator, emulator->shiftDevice, 2);
    
    emulator->clock_ticks = 0;
    emulator->screen_int_count = CYCLES_PER_SCREEN_INTERRUPT;
    emulator->screen_int_half = 0;
    emulator->event_queue.event_head = emulator->event_queue.event_tail = emulator->event_queue.event_count = 0;

    load_rom(emulator, "../rom/");
    return emulator;
}

void emu_register_device(struct Emulator *emulator, struct PortDevice *device, int startPort)
{
    device->portOffset = startPort;
    for (int n = 0; n < device->portCount; n++)
    {
        if (device->read[n])
        {
            if (emulator->dev_read[startPort + n])
            {
                printf("device read collision on port %d\n", startPort + n);
                exit(1);
            }
            emulator->dev_read[startPort + n] = device;
        }
        if (device->write[n])
        {
            if (emulator->dev_write[startPort + n])
            {
                printf("device write collision on port %d\n", startPort + n);
                exit(1);
            }
            emulator->dev_write[startPort + n] = device;
        }
    }
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
    emu_keyboard_done(emulator->kbDevice);
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
    while (ticks < clocks_ticks)
    {
        int cycles = emu_8080_execute(emulator->context);
        if (emulator->context->PC > 16000)
        {
            printf("ERROR after %i ticks\n", ticks);
            exit(1);
        }
        emulator->screen_int_count -= cycles;
        ticks += cycles;
        if (emulator->screen_int_count < 0 && emulator->context->interrupt)
        {
            ticks += emu_8080_rst(emulator->context, emulator->screen_int_half ? 1 : 2);
            emulator->screen_int_half = 1 - emulator->screen_int_half;
            emulator->screen_int_count += CYCLES_PER_SCREEN_INTERRUPT;
        }
    }
    emulator->clock_ticks += ticks;
    return ticks;
}

size_t load_rom(struct Emulator *emulator, const char *romDir)
{
    char *ext[] = {"h", "g", "f", "e", 0};
    char filename[255];
    for (int i = 0; ext[i]; i++)
    {
        sprintf(filename, "%s/invaders.%s", romDir, ext[i]);
        // printf("%s\n",filename);exit(1);
        FILE *f = fopen(filename, "r");
        size_t count = fread(emulator->memory + (i * 2048), 1, 2048, f);
        printf("rom loaded rom size=%li\n", count);
        fclose(f);
    }
    return 0;
}

int emu_handle_keyboard(struct Emulator *emulator, int keyVal, int isPressed)
{
    return handle_keyboard_event(emulator->kbDevice, keyVal, isPressed);
}