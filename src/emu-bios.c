
#include <stdio.h>
#include <stdlib.h>
#include "emu-bios.h"
#define WBOOT 0
#define BDOS 5

#define BDOS_C_WRITER 2
#define BDOS_C_WRITESTR 9

bool bios_trap(struct Context *context)
{
    if (context->PC == BDOS)
    {
        switch (context->reg[REG_C])
        {
        case BDOS_C_WRITER:
            printf("%c", context->reg[REG_C]);
            break;
        case BDOS_C_WRITESTR:
        {
            reg8_t ch;
            int addr = (context->reg[REG_D] << 8) | context->reg[REG_E];
            while ((ch = context->memory[addr++ & 0XFFFF]) != '$')
            {
                printf("%c", ch);
            }
        }
        break;
        default:
            return false;
        }
    }
    if (context->PC == WBOOT)
    {
        printf("warm boot\n");
        exit(0);
    }
    return true;
}