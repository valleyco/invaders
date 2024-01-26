#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "emu.h"
#include "emu-ports.h"
#include "emu-screen.h"
#include "invaders.rom.h"

static void emu_register_device(Emulator *emulator, PortDevice *device, const int *readPortMap, const int *writePortMap);
static const int emu_keyboard_read_map[] = {0, 1, 2};
static const int emu_shifter_read_map[] = {3};
static const int emu_shifter_write_map[] = {2, 4};
static const int emu_sound_write_map[] = {3, 5};

Emulator *emu_new()
{
    Emulator *emulator = (Emulator *)malloc(sizeof(Emulator));
    memset(emulator, 0, sizeof(*emulator));
    emulator->context = (struct Context *)malloc(sizeof(struct Context));
    emulator->context->gData = (void *)emulator;
    emulator->context->memory = emulator->memory;
    emulator->context->port_read = (int (*)(void *, int))port_read;
    emulator->context->port_write = (void (*)(void *, int, int))port_write;
    emulator->context->PC = 0;
    emulator->context->halt = 0;
    emulator->context->address_mask = 0x3FFF;
    emulator->context->rom_size = 0x2000;

    emulator->key_event_device = emu_keyboard_init(&emulator->key_event_handler);
    emu_register_device(emulator, emulator->key_event_device, emu_keyboard_read_map, NULL);
    emu_register_device(emulator, emu_screen_init(&emulator->intr), NULL, NULL);
    emu_register_device(emulator, emu_shifter_init(), emu_shifter_read_map, emu_shifter_write_map);
    emu_register_device(emulator, emu_sound_init(), NULL, emu_sound_write_map);
    emulator->clock_ticks = 0;
    memcpy(emulator->memory, invaders_rom, invaders_rom_len);
    return emulator;
}

static void emu_register_device(Emulator *emulator, PortDevice *device, const int *readPortMap, const int *writePortMap)
{
    emulator->devices[emulator->devices_count++] = device;
    for (int n = 0; n < device->readPortCount; n++)
    {
        if (device->read[n])
        {
            if (emulator->dev_read_handler[readPortMap[n]])
            {
                printf("device read collision on port %d\n", readPortMap[n]);
                exit(1);
            }
            emulator->dev_read[readPortMap[n]] = device;
            emulator->dev_read_handler[readPortMap[n]] = device->read[n];
        }
    }
    for (int n = 0; n < device->writePortCount; n++)
    {
        if (device->write[n])
        {
            if (emulator->dev_write_handler[writePortMap[n]])
            {
                printf("device write collision on port %d\n", writePortMap[n]);
                exit(1);
            }
            emulator->dev_write[writePortMap[n]] = device;
            emulator->dev_write_handler[writePortMap[n]] = device->write[n];
        }
    }
    if (device->clock_ticks)
    {
        emulator->dev_ticks[emulator->dev_ticks_count++] = device;
    }
}

void emu_free(Emulator *emulator)
{
    while (emulator->devices_count--)
    {
        (emulator->devices[emulator->devices_count])->dispose(emulator->devices[emulator->devices_count]);
    }
    free(emulator->context);
    free(emulator);
}

static inline bool handleTicks(Emulator *emulator, int cycles)
{
    emulator->clock_ticks += cycles;
    if (emulator->clock_ticks >= CPU_8080_CLOCKS_PER_TICK)
    {
        emulator->clock_ticks -= CPU_8080_CLOCKS_PER_TICK;
        return true;
    }
    return false;
}

static inline bool handleIntr(Emulator *emulator)
{
    int mask = 1;
    int intr = 0;
    if (!emulator->intr)
    {
        return 0;
    }
    while (intr < 8)
    {
        if (emulator->intr & mask)
        {
            emulator->intr = emulator->intr & ~mask;
            return emu_8080_rst(emulator->context, intr);
        }
        mask <<= 1;
        intr++;
    }
    return 0;
}

int emu_execute(Emulator *emulator, int clocks_ticks)
{
    int ticks = 0;
    while (ticks < clocks_ticks)
    {
        int intrCycle = handleIntr(emulator);
        int cycles = intrCycle ? intrCycle : emu_8080_execute(emulator->context);
        if (handleTicks(emulator, cycles))
        {
            for (PortDevice **pd = emulator->dev_ticks; *pd; pd++)
            {
                (*pd)->clock_ticks(*pd);
            }
        }
        ticks += cycles;
    }
    return ticks;
}

int emu_handle_keyboard(Emulator *emulator, int keyVal, int isPressed)
{
    return emulator->key_event_handler ? emulator->key_event_handler(emulator->key_event_device, keyVal, isPressed) : 0;
}