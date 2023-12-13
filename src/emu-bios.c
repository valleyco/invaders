#include "emu-bios.h"

int bios_trap(struct Context *context)
{
    if (context->PC == 0 || context->PC == 5){
        return 1;
    }
    return 0;
}