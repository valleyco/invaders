#include <stdlib.h>
#include <stddef.h>
#include "emu-shifter.h"

void emu_shifter_write_data(struct ShifterDevice *dev, int vp, int value)
{
    dev->shift_register = (dev->shift_register >> 8) | (value << 8);
}

void emu_shifter_write_shifts(struct ShifterDevice *dev, int vp, int value)
{
    dev->shift_amount = value & 7;
}

int emu_shifter_read_data(struct ShifterDevice *dev, int vp)
{
    return dev->shift_register >> (8 - dev->shift_amount);
}

static void (*port_write_array[])(void *g, int p, int v) = {emu_shifter_write_shifts, NULL, emu_shifter_write_data};

static int (*port_read_array[])(void *g, int p) = {NULL, emu_shifter_read_data, NULL};

struct ShifterDevice *emu_shifter_init()
{
    struct ShifterDevice *dev = (struct ShifterDevice *)malloc(sizeof(struct ShifterDevice));
    dev->portCount = 3;
    dev->read = port_read_array;
    dev->write = port_write_array;
    dev->shift_amount = 0;
    dev->shift_register = 0;
    return dev;
}

void emu_shifter_done(struct ShifterDevice *device)
{
    free(device);
}