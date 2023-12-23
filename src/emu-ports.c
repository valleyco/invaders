#include "emu-ports.h"

int port_read(Emulator *emu, int p)
{
    struct PortDevice *dev;
    if ((dev = emu->dev_read[p]))
    {
        const int v_port = p - dev->portOffset;
        return dev->read[v_port] ? dev->read[v_port](dev, v_port) : 0;
    }
    return 0;
}

void port_write(Emulator *emu, int p, int v)
{
    struct PortDevice *dev;
    if ((dev = emu->dev_write[p]))
    {
        dev->write[p - dev->portOffset](dev, p - dev->portOffset, v);
    }
}