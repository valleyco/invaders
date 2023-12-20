
#include "emu-bios.h"
#define BDOS_C_WRITER 2
#define BDOS_C_WRITESTR 9

bool bios_trap(struct Context *context)
{
    // context->PC != 0 &&
    if (context->PC != 5)
    {
        return false;
    }
    switch (context->reg[REG_C])
    {
    case BDOS_C_WRITER:
        printf("%c", context->reg[REG_C]);
        break;
    case BDOS_C_WRITESTR:
        reg8_t ch;
        int addr = (context->reg[REG_D] << 8) | context->reg[REG_E];
        while ((ch = context->memory[addr & 0XFFFF]) != '$')
        {
            printf("%c", ch);
        }
    default:
        return false;
    }
    return true;
}