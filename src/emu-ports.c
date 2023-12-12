#include "emu-ports.h"

int port_read(struct Emulator *emu, int p)
{
    switch (p)
    {
    case 3:
        return emu->shift_register >> (8 - emu->shift_amount);
    }
    return emu->port[p & 7];
}

void port_write(struct Emulator *emu, int p, int v)
{
    switch (p)
    {
    case 2:
        emu->shift_amount = v & 7;
        break;
    case 3: // sound - not implemented
        break;
    case 4:
        emu->shift_register = (v << 8) | (emu->shift_register >>= 8);
        break;
    case 5: // sound - not implemented
        break;
    default:
        emu->port[p & 7] = v;
        break;
    }
}
