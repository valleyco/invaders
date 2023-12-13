#pragma once

#include "emu.h"

#define PORT_0_READ_DIP4 1
#define PORT_0_READ_FIRE 16
#define PORT_0_READ_LEFT 32
#define PORT_0_READ_RIGHT 64

#define PORT_1_READ_CREDIT 1
#define PORT_1_READ_P2_START 2
#define PORT_1_READ_P1_START 4
#define PORT_1_READ_P1_SHOOT 16
#define PORT_1_READ_P1_LEFT 32
#define PORT_1_READ_P1_RIGHT 64

#define PORT_2_READ_LIFE_SHIPS 3
#define PORT_2_READ_LIFE_TILT 4
#define PORT_2_READ_LIFE_BONUS 8
#define PORT_2_READ_P2_SHOOT 16
#define PORT_2_READ_P2_LEFT 32
#define PORT_2_READ_P2_RIGHT 64
#define PORT_2_READ_COIN_INFO 128
struct PortDevice
{
  int portCount;
  int portOffset;
  int (**read)(void *g, int p);
  void (**write)(void *g, int p, int v);
};

int port_read(struct Emulator *emu, int p);
void port_write(struct Emulator *emu, int p, int v);
