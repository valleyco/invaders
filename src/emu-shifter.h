#pragma once
#include <stddef.h>
#include "emu-device.h"

typedef struct
{
    int shift_register;
    int shift_amount;
} ShifterDevice;
// mapped to port 2 on SI

#define SHIFTER_SHIFT_AMOUNT 0
// mapped to port 3 on SI
#define SHIFTER_DATE_READ 1
// mapped to port 4 on SI
#define SHIFTER_DATE_WRITE 2

PortDevice *emu_shifter_init();
void emu_shifter_done(PortDevice *device);