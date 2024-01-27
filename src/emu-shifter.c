#include <stdlib.h>
#include <string.h>
#include "emu-shifter.h"

static void emu_shifter_write_data(PortDevice *device, int value)
{
    ShifterDevice *shift = device->data;
    shift->shift_register = (shift->shift_register >> 8) | (value << 8);
}

static void emu_shifter_write_shifts(PortDevice *device, int value)
{
    ShifterDevice *shift = device->data;
    shift->shift_amount = value & 7;
}

static int emu_shifter_read_data(PortDevice *device)
{
    ShifterDevice *shift = device->data;
    return shift->shift_register >> (8 - shift->shift_amount);
}

static int (*port_read_array[])(PortDevice *g) = {emu_shifter_read_data};

static void (*port_write_array[])(PortDevice *g, int v) = {emu_shifter_write_shifts, emu_shifter_write_data};

static void emu_shifter_done(PortDevice *device)
{
    free(device->data);
    free(device);
}

PortDevice *emu_shifter_init()
{
    PortDevice *device = malloc(sizeof(PortDevice));
    memset(device, 0, sizeof(PortDevice));
    device->dispose = emu_shifter_done;
    device->readPortCount = 1;
    device->read = (PortRead *)port_read_array;
    device->writePortCount = 2;
    device->write = (PortWrite *)port_write_array;

    ShifterDevice *shift = (ShifterDevice *)malloc(sizeof(ShifterDevice));
    shift->shift_amount = 0;
    shift->shift_register = 0;
    device->data = shift;
    return device;
}
