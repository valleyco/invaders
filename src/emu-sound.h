#pragma once
#include <stddef.h>
#include "emu-device.h"

PortDevice *emu_sound_init();
void emu_sound_done(PortDevice *dev);