#pragma once
#include <stddef.h>
#include "emu-device.h"
typedef struct
{
    int portCount;
    int portOffset;
    int (**read)(void *g, int p);
    void (**write)(void *g, int p, int v);
    void *data;
} SoundDevice;
SoundDevice *emu_sound_init();
void emu_sound_done(SoundDevice *dev);