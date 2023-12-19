#pragma once

#include "emu-8080.h"

extern inline void set_mem_byte(struct Context *context, int addr, reg8_t val)
{
    addr = addr & context->address_mask;
    if (addr >= context->rom_size)
    {
        context->memory[addr & context->address_mask] = val & 0xFF;
    }
}

extern inline reg8_t get_mem_byte(struct Context *context, int addr)
{
    return context->memory[addr & context->address_mask];
}

extern inline void set_mem_word(struct Context *context, int addr, reg16_t val)
{
    set_mem_byte(context, addr, val);
    set_mem_byte(context, addr + 1, val >> 8);
}

extern inline reg16_t get_mem_word(struct Context *context, int addr)
{
    return get_mem_byte(context, addr) | ((int)get_mem_byte(context, addr + 1) << 8);
}

extern inline int fetch_pc_byte(struct Context *context)
{
    int byte = get_mem_byte(context, context->PC);
    context->PC = (context->PC + 1) & 0xFFFF;
    return byte;
}

extern inline int fetch_pc_word(struct Context *context)
{
    int word = fetch_pc_byte(context);
    return word | (fetch_pc_byte(context) << 8);
}

extern inline int check_condition(struct Context *context, int op)
{
    switch ((op >> 3) & 0x7)
    {
    case C_NZ:
        return !context->flag[Z_FLAG];
    case C_Z:
        return context->flag[Z_FLAG];
    case C_NC:
        return !context->flag[C_FLAG];
    case C_C:
        return context->flag[C_FLAG];
    case C_PO:
        return !context->flag[P_FLAG];
    case C_PE:
        return context->flag[P_FLAG];
    case C_P:
        return !context->flag[S_FLAG];
    case C_M:
        return context->flag[S_FLAG];
    }
    return 0;
}

extern inline int is_parity_even(int b)
{
    b ^= b >> 4;
    b ^= b >> 2;
    b ^= b >> 1;
    return (~b) & 1;
}
extern inline reg8_t get_m_reg(struct Context *context)
{
    const int addr = context->reg[REG_L] | (context->reg[REG_H] << 8);
    return context->M = get_mem_byte(context, addr);
}

extern inline void set_m_reg(struct Context *context, reg8_t val)
{
    const int addr = context->reg[REG_L] | (context->reg[REG_H] << 8);
    set_mem_byte(context, addr, (context->M = val));
}

extern inline reg8_t get_reg_val(struct Context *context, int reg)
{
    return (reg == REG_M) ? get_m_reg(context) : context->reg[reg];
}

extern inline reg16_t get_rp_val(struct Context *context, int rp)
{
    switch (rp & 0x30)
    {
    case RP_BC:
        return context->reg[REG_C] | (context->reg[REG_B] << 8);
    case RP_DE:
        return context->reg[REG_E] | (context->reg[REG_D] << 8);
    case RP_HL:
        return context->reg[REG_L] | (context->reg[REG_H] << 8);
    case RP_SP:
        return context->SP;
    }
}

extern inline void set_rp_val(struct Context *context, int rp, reg16_t val)
{
    switch (rp & 0x30)
    {
    case RP_BC:
        context->reg[REG_C] = val & 0xff;
        context->reg[REG_B] = (val >> 8) & 0xff;
        break;
    case RP_DE:

        context->reg[REG_E] = val & 0xff;
        context->reg[REG_D] = (val >> 8) & 0xff;
        break;

    case RP_HL:
        context->reg[REG_L] = val & 0xff;
        context->reg[REG_H] = (val >> 8) & 0xff;
        break;

    case RP_SP:
        context->SP = val;
        break;
    }
}

extern inline void set_reg_val(struct Context *context, int reg, reg8_t val)
{
    if (reg == REG_M)
    {
        set_m_reg(context, val);
    }
    else
    {
        context->reg[reg] = val;
    }
}

// Z_FLAG = 0;
// C_FLAG = 1;
// P_FLAG = 2;
// S_FLAG = 3;

// AC is not update here will be implemented in the relevant instruction
extern inline void update_flags(struct Context *context, int val, int inc_c)
{
    if (inc_c)
    {
        context->flag[C_FLAG] = (val & 0xf00) > 0;
    }
    context->flag[Z_FLAG] = (val & 0xff) == 0;
    context->flag[P_FLAG] = is_parity_even(val);
    context->flag[S_FLAG] = (val & 0x80) != 0;
}

extern inline int pack_flags(struct Context *context)
{
    return context->flag[C_FLAG] ? 0x01 : 0 | 1                   ? 0x02
                                      : 0 | context->flag[P_FLAG] ? 0x04
                                      : 0 | 0                     ? 0x08
                                      : 0 | context->flag[A_FLAG] ? 0x10
                                      : 0 | 0                     ? 0x20
                                      : 0 | context->flag[Z_FLAG] ? 0x40
                                      : 0 | context->flag[S_FLAG] ? 0x80
                                                                  : 0;
}

extern inline void unpack_flags(struct Context *context, int flags)
{
    context->flag[C_FLAG] = (flags & 0x01) != 0;
    context->flag[P_FLAG] = (flags & 0x04) != 0;
    context->flag[A_FLAG] = (flags & 0x10) != 0;
    context->flag[Z_FLAG] = (flags & 0x40) != 0;
    context->flag[S_FLAG] = (flags & 0x80) != 0;
}
