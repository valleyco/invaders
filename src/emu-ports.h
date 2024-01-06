#pragma once
#include "emu.h"

int port_read(Emulator *emu, int p);
void port_write(Emulator *emu, int p, int v);
