#pragma once
#include "emu.h"
#ifdef __cplusplus
extern "C"
{
#endif
    int port_read(Emulator *emu, int p);
    void port_write(Emulator *emu, int p, int v);
#ifdef __cplusplus
}
#endif