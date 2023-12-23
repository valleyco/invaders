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

typedef int (*PORT_READ)(void *, int);
typedef void (*PORT_WRITE)(void *, int, int);

ShifterDevice *emu_shifter_init()
{
    ShifterDevice *dev = (ShifterDevice *)malloc(sizeof(ShifterDevice));
    dev->portCount = 3;
    dev->read = (PORT_READ *)port_read_array;
    dev->write = (PORT_WRITE *)port_write_array;
    dev->shift_amount = 0;
    dev->shift_register = 0;
    return dev;
}

void emu_shifter_done(ShifterDevice *device)
{
    free(device);
}