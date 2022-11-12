#pragma once

#include "emu-8080.h"

extern inline int fetch_pc_byte(struct Context *context)
{
    return context->memory[(context->PC++ & 0xffff)];
}
extern inline int fetch_pc_word(struct Context *context)
{   int word = fetch_pc_byte(context);
    return word + (fetch_pc_byte(context) << 8);
}

extern inline int get_source(int op, struct Context *context)
{
    return op & 0x7;
}

extern inline int get_destination(int op, struct Context *context)
{
    return (op >> 3) & 0x7;
}

extern inline int get_rp(int op)
{
    return (op >> 4) & 0x3;
}

extern inline int get_condition(int op)
{
    return (op >> 3) & 0x7;
}

extern inline int is_parity_even(int b)
{
    b ^= b >> 4;
    b ^= b >> 2;
    b ^= b >> 1;
    return (~b) & 1;
}
// Z_FLAG = 0;
// C_FLAG = 1;
// P_FLAG = 2;
// S_FLAG = 3;

// AC is not update here will be implemented in the relevant instruction
extern inline void update_flags(struct Context *context, int reg, int inc_c)
{
    if(inc_c){
        context->flag[C_FLAG] = (context->reg[reg] & 0xf00) > 0;
    }
    context->reg[reg] &= 0xff;
    context->flag[Z_FLAG] = context->reg[reg] == 0;
    context->flag[P_FLAG] = is_parity_even(context->reg[reg]);
    context->flag[S_FLAG] = context->reg[reg] & 0x80;
}

extern inline void pack_flags(struct Context *context)
{
    context->reg[REG_FLAG] =
        context->flag[C_FLAG] ? 0x01 : 0 | 0x02 | context->flag[P_FLAG]   ? 0x04
                                   : 0 | 0x08 * 0 | context->flag[A_FLAG] ? 0x10
                                   : 0 | 0x20 * 0 | context->flag[Z_FLAG] ? 0x40
                                   : 0 | context->flag[S_FLAG]            ? 0x80
                                                                          : 0;
}

extern inline void unpack_flags(struct Context *context)
{
    context->flag[C_FLAG] = context->reg[REG_FLAG] & 0x01;
    context->flag[P_FLAG] = context->reg[REG_FLAG] & 0x04;
    context->flag[A_FLAG] = context->reg[REG_FLAG] & 0x10;
    context->flag[Z_FLAG] = context->reg[REG_FLAG] & 0x40;
    context->flag[S_FLAG] = context->reg[REG_FLAG] & 0x80;
}

extern inline int get_m(struct Context *context)
{
    return context->memory[context->reg[REG_L] + (context->reg[REG_H] << 8)];
}

extern inline void set_m(struct Context *context, int val)
{
    context->memory[context->reg[REG_L] + (context->reg[REG_H] << 8)] = val;
}