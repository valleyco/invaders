#include "emu-device.h"
#include "emu-ports.h"

int port_read(Emulator *emu, int p)
{
    if ((emu->dev_read_handler[p]))
    {
        return emu->dev_read_handler[p](emu->dev_read[p]);
    }
    return 0;
}

void port_write(Emulator *emu, int p, int v)
{
    if (emu->dev_write_handler[p])
    {
        emu->dev_write_handler[p](emu->dev_write[p], v);
    }
}