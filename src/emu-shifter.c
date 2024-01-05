#include <stdlib.h>
#include <stddef.h>
#include "emu-shifter.h"

static void emu_shifter_write_data(ShifterDevice *dev, int v_port, int value)
{
    (void)v_port;
    dev->shift_register = (dev->shift_register >> 8) | (value << 8);
}

static void emu_shifter_write_shifts(ShifterDevice *dev, int v_port, int value)
{
    (void)v_port;
    dev->shift_amount = value & 7;
}

static int emu_shifter_read_data(ShifterDevice *dev, int v_port)
{
    (void)v_port;
    return dev->shift_register >> (8 - dev->shift_amount);
}

static int (*port_read_array[])(ShifterDevice *g, int p) = {NULL, emu_shifter_read_data, NULL};

static void (*port_write_array[])(ShifterDevice *g, int p, int v) = {emu_shifter_write_shifts, NULL, emu_shifter_write_data};

ShifterDevice *emu_shifter_init()
{
    ShifterDevice *device = (ShifterDevice *)malloc(sizeof(ShifterDevice));
    device->portCount = 3;
    device->read = (PORT_READ *)port_read_array;
    device->write = (PORT_WRITE *)port_write_array;
    device->shift_amount = 0;
    device->shift_register = 0;
    return device;
}

void emu_shifter_done(ShifterDevice *device)
{
    free(device);
}