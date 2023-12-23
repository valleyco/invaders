#pragma once

#include "emu.h"
struct PortDevice
{
  int portCount;
  int portOffset;
  int (**read)(void *g, int p);
  void (**write)(void *g, int p, int v);
};

int port_read(Emulator *emu, int p);
void port_write(Emulator *emu, int p, int v);
