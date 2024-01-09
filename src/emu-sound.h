#pragma once
#include <stddef.h>
#include "emu-device.h"
typedef struct
{
    void *data;
} SoundDevice;
PortDevice *emu_sound_init();
void emu_sound_done(PortDevice *dev);