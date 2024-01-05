#include <stdlib.h>
#include <string.h>
#include "emu.h"
#include "emu-ports.h"
#include "emu-screen.h"
#include "invaders.rom.h"

static void emu_register_device(Emulator *emulator, PortDevice *device, int startPort);

Emulator *emu_new()
{
    Emulator *emulator = (Emulator *)malloc(sizeof(Emulator));
    emulator->context = (struct Context *)malloc(sizeof(struct Context));
    memset(emulator->dev_read, 0, 256 * sizeof(emulator->dev_read[0]));
    memset(emulator->dev_write, 0, 256 * sizeof(emulator->dev_write[0]));
    emulator->context->gData = (void *)emulator;
    emulator->context->memory = emulator->memory;
    emulator->context->port_read = (int (*)(void *, int))port_read;
    emulator->context->port_write = (void (*)(void *, int, int))port_write;
    emulator->context->PC = 0;
    emulator->context->halt = 0;
    emulator->context->address_mask = 0x3FFF;
    emulator->context->rom_size = 0x2000;

    emulator->kbDevice = emu_keyboard_init();
    emu_register_device(emulator, (PortDevice *)emulator->kbDevice, 0);
    emulator->shiftDevice = emu_shifter_init();
    emu_register_device(emulator, (PortDevice *)emulator->shiftDevice, 2);
    emulator->soundDevice = emu_sound_init();
    emu_register_device(emulator, (PortDevice *)emulator->soundDevice, 3);
    emulator->clock_ticks = 0;
    emulator->screen_int_count = CYCLES_PER_SCREEN_INTERRUPT;
    emulator->screen_int_half = 0;
    emulator->event_queue.event_head = emulator->event_queue.event_tail = emulator->event_queue.event_count = 0;
    memcpy(emulator->memory,invaders_rom,invaders_rom_len);
    return emulator;
}

static void emu_register_device(Emulator *emulator, PortDevice *device, int startPort)
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

void emu_free(Emulator *emulator)
{
    free(emulator->context);
    emu_keyboard_done(emulator->kbDevice);
    emu_shifter_done(emulator->shiftDevice);
    emu_sound_done(emulator->soundDevice);
    free(emulator);
}

void emu_event_add(Emulator *emulator, struct Event event)
{
    if (emulator->event_queue.event_count == EVENT_QUEUE_LENGTH)
    {
        return;
    }
    emulator->event_queue.event_count++;
    emulator->event_queue.event[(emulator->event_queue.event_head++) % 0x0F] = event;
}

static void emu_event_drop(Emulator *emulator)
{
    emulator->event_queue.event_count--;
    emulator->event_queue.event_tail = (emulator->event_queue.event_tail + 1) % 0x0F;
}

static void emu_handle_events(Emulator *emulator)
{
    emu_event_drop(emulator);
}

int emu_execute(Emulator *emulator, int clocks_ticks)
{
    if (emulator->event_queue.event_count)
    {
        emu_handle_events(emulator);
    }
    int ticks = 0;
    while (ticks < clocks_ticks)
    {
        int cycles = emu_8080_execute(emulator->context);
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

int emu_handle_keyboard(Emulator *emulator, int keyVal, int isPressed)
{
    return handle_keyboard_event(emulator->kbDevice, keyVal, isPressed);
}