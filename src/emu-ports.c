#include "emu-ports.h"

int port_read(struct Emulator *emu, int p)
{
    return emu->port[p & 7];
}

void port_write(struct Emulator *emu, int p, int v)
{
    switch (p)
    {
    case 2:
        emu->port[3] = ((emu->shift_register << v) & 0xff00) >> 8;
        break;
    case 3: // sound - not implemented
        break;
    case 4:
        emu->shift_register >>= 8;
        emu->shift_register |= (v << 8);
        break;
    case 5: // sound - not implemented
        break;
    case 6:
        break;
    }
}
