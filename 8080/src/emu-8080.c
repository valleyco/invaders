#include <stdio.h>
#include <stdlib.h>
#include "emu-8080.h"
#include "emu-8080.c.h"
#pragma GCC diagnostic ignored "-Wunused-parameter"

// http://www.nacad.ufrj.br/online/intel/vtune/users_guide/mergedProjects/analyzer_ec/mergedProjects/reference_olh/mergedProjects/instructions/instruct32_hh/vc71.htm
static inline int inst_8080_mov(struct Context *context, int op)
{
    const int cycles = 5;
    if ((op & 7) == REG_M)
    {
        context->reg[(op >> 7) & 7] = get_m(context);
    }
    else if ((op & 070) == REG_M << 3)
    {
        set_m(context, context->reg[op & 7]);
    }
    else
    {
        context->reg[(op >> 7) & 7] = context->reg[op & 7];
    }
    return cycles;
}

static inline int inst_8080_mvi(struct Context *context, int op)
{
    const int cycles = 7;
    if (op == 0b00110110)
    {
        set_m(context, fetch_pc_byte(context));
        return cycles + 3 ;
    }
    else
    {
        context->reg[op >> 7] = fetch_pc_byte(context);
        return cycles;
    }
}

static inline int inst_8080_lxi(struct Context *context, int op)
{
    const int cycles = 10;
    switch (op & 0x30)
    {
    case RP_BC:
        context->reg[REG_C] = fetch_pc_byte(context);
        context->reg[REG_B] = fetch_pc_byte(context);
        break;
    case RP_DE:
        context->reg[REG_E] = fetch_pc_byte(context);
        context->reg[REG_D] = fetch_pc_byte(context);
        break;
    case RP_HL:
        context->reg[REG_L] = fetch_pc_byte(context);
        context->reg[REG_H] = fetch_pc_byte(context);
        break;
    case RP_SP:
        context->SP = fetch_pc_word(context);
        break;
    }
    return cycles;
}

static inline int inst_8080_lda(struct Context *context, int op)
{
    const int cycles = 13;
    context->reg[REG_A] = context->memory[fetch_pc_word(context)];
    return cycles;
}

static inline int inst_8080_sta(struct Context *context, int op)
{
    const int cycles = 13;
    context->memory[fetch_pc_word(context)] = context->reg[REG_A];
    return cycles;
}

static inline int inst_8080_lhld(struct Context *context, int op)
{
    const int cycles = 16;
    int addr = fetch_pc_word(context);
    context->reg[REG_L] = context->memory[addr];
    context->reg[REG_H] = context->memory[addr + 1];
    return cycles;
}

static inline int inst_8080_shld(struct Context *context, int op)
{
    const int cycles = 16;
    int addr = fetch_pc_word(context);
    context->memory[addr] = context->reg[REG_L];
    context->memory[addr + 1] = context->reg[REG_H];
    return cycles;
}

static inline int inst_8080_ldax(struct Context *context, int op)
{
    const int cycles = 7;
    int addr;
    switch (op & 0x30)
    {
    case RP_BC:
        addr = context->reg[REG_C] + (context->reg[REG_B] << 8);
        break;
    case RP_DE:
        addr = context->reg[REG_E] + (context->reg[REG_D] << 8);
        break;
    }
    context->reg[REG_A] = context->memory[addr];
    return cycles;
}

static inline int inst_8080_stax(struct Context *context, int op)
{
    const int cycles = 7;
    int addr;
    switch (op & 0x30)
    {
    case RP_BC:
        addr = context->reg[REG_C] + (context->reg[REG_B] << 8);
        break;
    case RP_DE:
        addr = context->reg[REG_E] + (context->reg[REG_D] << 8);
        break;
    }
    context->memory[addr] = context->reg[REG_A];
    return cycles;
}

static inline int inst_8080_xchg(struct Context *context, int op)
{
    const int cycles = 5;
    int tmp = context->reg[REG_E];
    context->reg[REG_E] = context->reg[REG_L];
    context->reg[REG_L] = tmp;
    tmp = context->reg[REG_D];
    context->reg[REG_D] = context->reg[REG_H];
    context->reg[REG_H] = tmp;
    return cycles;
}

static inline void inst_8080_add_common(struct Context *context, int val, int c)
{
    context->flag[A_FLAG] = ((context->reg[REG_A] & 0xf) + ((val + c) & 0xf)) > 0xf;
    context->reg[REG_A] += (val + c);
    update_flags(context, REG_A, 1);

}

static inline int inst_8080_add(struct Context *context, int op)
{
    const int cycles = 4;
    const int val = (op == 0b10000110) ? get_m(context) : context->reg[op & 7];
    inst_8080_add_common(context, val, 0);
    return cycles;
}

static inline int inst_8080_adi(struct Context *context, int op)
{
    const int cycles = 7;
    const int val = fetch_pc_byte(context);
    inst_8080_add_common(context, val, 0);
    return cycles;
}

static inline int inst_8080_adc(struct Context *context, int op)
{
    const int cycles = 4;
    const int val = (op == 0b10000110) ? get_m(context) : context->reg[op & 7];
    inst_8080_add_common(context, val, context->flag[C_FLAG] == 0 ? 0 :1);
    return cycles;
}

static inline int inst_8080_aci(struct Context *context, int op)
{
    const int cycles = 7;
    const int val = fetch_pc_byte(context);
    inst_8080_add_common(context, val, context->flag[C_FLAG] == 0 ? 0 :1);
    return cycles;
}

static inline int inst_8080_sub(struct Context *context, int op)
{
    const int cycles = 4;
    // instruction not implemented yet
    return cycles;
}

static inline int inst_8080_sui(struct Context *context, int op)
{
    const int cycles = 7;
    // instruction not implemented yet
    return cycles;
}

static inline int inst_8080_sbb(struct Context *context, int op)
{
    const int cycles = 4;
    // instruction not implemented yet
    return cycles;
}

static inline int inst_8080_sbi(struct Context *context, int op)
{
    const int cycles = 7;
    // instruction not implemented yet
    return cycles;
}

static inline int inst_8080_inr(struct Context *context, int op)
{
    const int cycles = 5;
    const int reg = op >> 3;
    int val = (op == 0b00110100)? val = get_m(context): context->reg[reg];
    context->flag[A_FLAG] = (val & 0xf) == 0xf ? 1 : 0;
    val++;
    if(op == 0b00110100){
         set_m(context, val);
    } else {
        context->reg[reg] = val;
    }
    update_flags(context, reg, 0);
    return cycles;
}

static inline int inst_8080_dcr(struct Context *context, int op)
{
    const int cycles = 5;
    const int reg = op >> 3;
    int val = (op == 0b00110101)? val = get_m(context): context->reg[reg];
    context->flag[A_FLAG] = (val & 0xf) == 0x0 ? 1 : 0;
    val--;
    if(op == 0b00110101){
         set_m(context, val);
    } else {
        context->reg[op >> 3] = val;
    }
    update_flags(context, reg, 0);
    return cycles;
}

static inline int inst_8080_inx(struct Context *context, int op)
{
    const int cycles = 5;
    // instruction not implemented yet
    return cycles;
}

static inline int inst_8080_dcx(struct Context *context, int op)
{
    const int cycles = 5;
    // instruction not implemented yet
    return cycles;
}

static inline int inst_8080_dad(struct Context *context, int op)
{
    const int cycles = 10;
    // instruction not implemented yet
    return cycles;
}

static inline int inst_8080_daa(struct Context *context, int op)
{
    const int cycles = 4;
    // instruction not implemented yet
    return cycles;
}

static inline int inst_8080_ana(struct Context *context, int op)
{
    const int cycles = 4;
    // instruction not implemented yet
    return cycles;
}

static inline int inst_8080_ani(struct Context *context, int op)
{
    const int cycles = 7;
    // instruction not implemented yet
    return cycles;
}

static inline int inst_8080_ora(struct Context *context, int op)
{
    const int cycles = 4;
    // instruction not implemented yet
    return cycles;
}

static inline int inst_8080_ori(struct Context *context, int op)
{
    const int cycles = 7;
    // instruction not implemented yet
    return cycles;
}

static inline int inst_8080_xra(struct Context *context, int op)
{
    const int cycles = 4;
    // instruction not implemented yet
    return cycles;
}

static inline int inst_8080_xri(struct Context *context, int op)
{
    const int cycles = 7;
    // instruction not implemented yet
    return cycles;
}

static inline int inst_8080_cmp(struct Context *context, int op)
{
    const int cycles = 4;
    // instruction not implemented yet
    return cycles;
}

static inline int inst_8080_cpi(struct Context *context, int op)
{
    const int cycles = 7;
    // instruction not implemented yet
    return cycles;
}

static inline int inst_8080_rlc(struct Context *context, int op)
{
    const int cycles = 4;
    // instruction not implemented yet
    return cycles;
}

static inline int inst_8080_rrc(struct Context *context, int op)
{
    const int cycles = 4;
    // instruction not implemented yet
    return cycles;
}

static inline int inst_8080_ral(struct Context *context, int op)
{
    const int cycles = 4;
    // instruction not implemented yet
    return cycles;
}

static inline int inst_8080_rar(struct Context *context, int op)
{
    const int cycles = 4;
    // instruction not implemented yet
    return cycles;
}

static inline int inst_8080_cma(struct Context *context, int op)
{
    const int cycles = 4;
    // instruction not implemented yet
    return cycles;
}

static inline int inst_8080_cmc(struct Context *context, int op)
{
    const int cycles = 4;
    // instruction not implemented yet
    return cycles;
}

static inline int inst_8080_stc(struct Context *context, int op)
{
    const int cycles = 4;
    // instruction not implemented yet
    return cycles;
}

static inline int inst_8080_jmp(struct Context *context, int op)
{
    const int cycles = 10;
    // instruction not implemented yet
    return cycles;
}

static inline int inst_8080_j(struct Context *context, int op)
{
    const int cycles = 10;
    // instruction not implemented yet
    return cycles;
}

static inline int inst_8080_call(struct Context *context, int op)
{
    const int cycles = 17;
    // instruction not implemented yet
    return cycles;
}

static inline int inst_8080_c(struct Context *context, int op)
{
    const int cycles = 17;
    // instruction not implemented yet
    return cycles;
}

static inline int inst_8080_ret(struct Context *context, int op)
{
    const int cycles = 10;
    // instruction not implemented yet
    return cycles;
}

static inline int inst_8080_r(struct Context *context, int op)
{
    const int cycles = 11;
    // instruction not implemented yet
    return cycles;
}

static inline int inst_8080_rst(struct Context *context, int op)
{
    const int cycles = 11;
    // instruction not implemented yet
    return cycles;
}

static inline int inst_8080_pchl(struct Context *context, int op)
{
    const int cycles = 5;
    // instruction not implemented yet
    return cycles;
}

static inline int inst_8080_push(struct Context *context, int op)
{
    const int cycles = 11;
    // instruction not implemented yet
    return cycles;
}

static inline int inst_8080_pop(struct Context *context, int op)
{
    const int cycles = 10;
    // instruction not implemented yet
    return cycles;
}

static inline int inst_8080_xthl(struct Context *context, int op)
{
    const int cycles = 18;
    // instruction not implemented yet
    return cycles;
}

static inline int inst_8080_sphl(struct Context *context, int op)
{
    const int cycles = 5;
    // instruction not implemented yet
    return cycles;
}

static inline int inst_8080_in(struct Context *context, int op)
{
    const int cycles = 10;
    // instruction not implemented yet
    return cycles;
}

static inline int inst_8080_out(struct Context *context, int op)
{
    const int cycles = 10;
    // instruction not implemented yet
    return cycles;
}

static inline int inst_8080_ei(struct Context *context, int op)
{
    const int cycles = 4;
    // instruction not implemented yet
    return cycles;
}

static inline int inst_8080_di(struct Context *context, int op)
{
    const int cycles = 4;
    // instruction not implemented yet
    return cycles;
}

static inline int inst_8080_hlt(struct Context *context, int op)
{
    const int cycles = 7;
    // instruction not implemented yet
    return cycles;
}

static inline int inst_8080_nop(struct Context *context, int op)
{
    const int cycles = 4;
    // instruction not implemented yet
    return cycles;
}

inline static int inst_8080_illegal(struct Context *context, int op)
{
    return 0;
}

int emu_8080_rst(struct Context *context, int n)
{
    return inst_8080_rst(context, 0b11000111 | (n << 3));
}

int emu_8080_execute(struct Context *context)
{
    int opcode = fetch_pc_byte(context);
    switch (opcode)
    {
    case 0x00:
        return inst_8080_nop(context, opcode);

    case 0x01:
        return inst_8080_lxi(context, opcode);

    case 0x02:
        return inst_8080_stax(context, opcode);

    case 0x03:
        return inst_8080_inx(context, opcode);

    case 0x04:
        return inst_8080_inr(context, opcode);

    case 0x05:
        return inst_8080_dcr(context, opcode);

    case 0x06:
        return inst_8080_mvi(context, opcode);

    case 0x07:
        return inst_8080_rlc(context, opcode);

    case 0x08:
        return inst_8080_illegal(context, opcode);
    case 0x09:
        return inst_8080_dad(context, opcode);

    case 0x0a:
        return inst_8080_ldax(context, opcode);

    case 0x0b:
        return inst_8080_dcx(context, opcode);

    case 0x0c:
        return inst_8080_inr(context, opcode);

    case 0x0d:
        return inst_8080_dcr(context, opcode);

    case 0x0e:
        return inst_8080_mvi(context, opcode);

    case 0x0f:
        return inst_8080_rrc(context, opcode);

    case 0x10:
        return inst_8080_illegal(context, opcode);
    case 0x11:
        return inst_8080_lxi(context, opcode);

    case 0x12:
        return inst_8080_stax(context, opcode);

    case 0x13:
        return inst_8080_inx(context, opcode);

    case 0x14:
        return inst_8080_inr(context, opcode);

    case 0x15:
        return inst_8080_dcr(context, opcode);

    case 0x16:
        return inst_8080_mvi(context, opcode);

    case 0x17:
        return inst_8080_ral(context, opcode);

    case 0x18:
        return inst_8080_illegal(context, opcode);
    case 0x19:
        return inst_8080_dad(context, opcode);

    case 0x1a:
        return inst_8080_ldax(context, opcode);

    case 0x1b:
        return inst_8080_dcx(context, opcode);

    case 0x1c:
        return inst_8080_inr(context, opcode);

    case 0x1d:
        return inst_8080_dcr(context, opcode);

    case 0x1e:
        return inst_8080_mvi(context, opcode);

    case 0x1f:
        return inst_8080_rar(context, opcode);

    case 0x20:
        return inst_8080_illegal(context, opcode);
    case 0x21:
        return inst_8080_lxi(context, opcode);

    case 0x22:
        return inst_8080_shld(context, opcode);

    case 0x23:
        return inst_8080_inx(context, opcode);

    case 0x24:
        return inst_8080_inr(context, opcode);

    case 0x25:
        return inst_8080_dcr(context, opcode);

    case 0x26:
        return inst_8080_mvi(context, opcode);

    case 0x27:
        return inst_8080_daa(context, opcode);

    case 0x28:
        return inst_8080_illegal(context, opcode);
    case 0x29:
        return inst_8080_dad(context, opcode);

    case 0x2a:
        return inst_8080_lhld(context, opcode);

    case 0x2b:
        return inst_8080_dcx(context, opcode);

    case 0x2c:
        return inst_8080_inr(context, opcode);

    case 0x2d:
        return inst_8080_dcr(context, opcode);

    case 0x2e:
        return inst_8080_mvi(context, opcode);

    case 0x2f:
        return inst_8080_cma(context, opcode);

    case 0x30:
        return inst_8080_illegal(context, opcode);
    case 0x31:
        return inst_8080_lxi(context, opcode);

    case 0x32:
        return inst_8080_sta(context, opcode);

    case 0x33:
        return inst_8080_inx(context, opcode);

    case 0x34:
        return inst_8080_inr(context, opcode);

    case 0x35:
        return inst_8080_dcr(context, opcode);

    case 0x36:
        return inst_8080_mvi(context, opcode);

    case 0x37:
        return inst_8080_stc(context, opcode);

    case 0x38:
        return inst_8080_illegal(context, opcode);
    case 0x39:
        return inst_8080_dad(context, opcode);

    case 0x3a:
        return inst_8080_lda(context, opcode);

    case 0x3b:
        return inst_8080_dcx(context, opcode);

    case 0x3c:
        return inst_8080_inr(context, opcode);

    case 0x3d:
        return inst_8080_dcr(context, opcode);

    case 0x3e:
        return inst_8080_mvi(context, opcode);

    case 0x3f:
        return inst_8080_cmc(context, opcode);

    case 0x40:
        return inst_8080_mov(context, opcode);

    case 0x41:
        return inst_8080_mov(context, opcode);

    case 0x42:
        return inst_8080_mov(context, opcode);

    case 0x43:
        return inst_8080_mov(context, opcode);

    case 0x44:
        return inst_8080_mov(context, opcode);

    case 0x45:
        return inst_8080_mov(context, opcode);

    case 0x46:
        return inst_8080_mov(context, opcode);

    case 0x47:
        return inst_8080_mov(context, opcode);

    case 0x48:
        return inst_8080_mov(context, opcode);

    case 0x49:
        return inst_8080_mov(context, opcode);

    case 0x4a:
        return inst_8080_mov(context, opcode);

    case 0x4b:
        return inst_8080_mov(context, opcode);

    case 0x4c:
        return inst_8080_mov(context, opcode);

    case 0x4d:
        return inst_8080_mov(context, opcode);

    case 0x4e:
        return inst_8080_mov(context, opcode);

    case 0x4f:
        return inst_8080_mov(context, opcode);

    case 0x50:
        return inst_8080_mov(context, opcode);

    case 0x51:
        return inst_8080_mov(context, opcode);

    case 0x52:
        return inst_8080_mov(context, opcode);

    case 0x53:
        return inst_8080_mov(context, opcode);

    case 0x54:
        return inst_8080_mov(context, opcode);

    case 0x55:
        return inst_8080_mov(context, opcode);

    case 0x56:
        return inst_8080_mov(context, opcode);

    case 0x57:
        return inst_8080_mov(context, opcode);

    case 0x58:
        return inst_8080_mov(context, opcode);

    case 0x59:
        return inst_8080_mov(context, opcode);

    case 0x5a:
        return inst_8080_mov(context, opcode);

    case 0x5b:
        return inst_8080_mov(context, opcode);

    case 0x5c:
        return inst_8080_mov(context, opcode);

    case 0x5d:
        return inst_8080_mov(context, opcode);

    case 0x5e:
        return inst_8080_mov(context, opcode);

    case 0x5f:
        return inst_8080_mov(context, opcode);

    case 0x60:
        return inst_8080_mov(context, opcode);

    case 0x61:
        return inst_8080_mov(context, opcode);

    case 0x62:
        return inst_8080_mov(context, opcode);

    case 0x63:
        return inst_8080_mov(context, opcode);

    case 0x64:
        return inst_8080_mov(context, opcode);

    case 0x65:
        return inst_8080_mov(context, opcode);

    case 0x66:
        return inst_8080_mov(context, opcode);

    case 0x67:
        return inst_8080_mov(context, opcode);

    case 0x68:
        return inst_8080_mov(context, opcode);

    case 0x69:
        return inst_8080_mov(context, opcode);

    case 0x6a:
        return inst_8080_mov(context, opcode);

    case 0x6b:
        return inst_8080_mov(context, opcode);

    case 0x6c:
        return inst_8080_mov(context, opcode);

    case 0x6d:
        return inst_8080_mov(context, opcode);

    case 0x6e:
        return inst_8080_mov(context, opcode);

    case 0x6f:
        return inst_8080_mov(context, opcode);

    case 0x70:
        return inst_8080_mov(context, opcode);

    case 0x71:
        return inst_8080_mov(context, opcode);

    case 0x72:
        return inst_8080_mov(context, opcode);

    case 0x73:
        return inst_8080_mov(context, opcode);

    case 0x74:
        return inst_8080_mov(context, opcode);

    case 0x75:
        return inst_8080_mov(context, opcode);

    case 0x76:
        return inst_8080_hlt(context, opcode);

    case 0x77:
        return inst_8080_mov(context, opcode);

    case 0x78:
        return inst_8080_mov(context, opcode);

    case 0x79:
        return inst_8080_mov(context, opcode);

    case 0x7a:
        return inst_8080_mov(context, opcode);

    case 0x7b:
        return inst_8080_mov(context, opcode);

    case 0x7c:
        return inst_8080_mov(context, opcode);

    case 0x7d:
        return inst_8080_mov(context, opcode);

    case 0x7e:
        return inst_8080_mov(context, opcode);

    case 0x7f:
        return inst_8080_mov(context, opcode);

    case 0x80:
        return inst_8080_add(context, opcode);

    case 0x81:
        return inst_8080_add(context, opcode);

    case 0x82:
        return inst_8080_add(context, opcode);

    case 0x83:
        return inst_8080_add(context, opcode);

    case 0x84:
        return inst_8080_add(context, opcode);

    case 0x85:
        return inst_8080_add(context, opcode);

    case 0x86:
        return inst_8080_add(context, opcode);

    case 0x87:
        return inst_8080_add(context, opcode);

    case 0x88:
        return inst_8080_adc(context, opcode);

    case 0x89:
        return inst_8080_adc(context, opcode);

    case 0x8a:
        return inst_8080_adc(context, opcode);

    case 0x8b:
        return inst_8080_adc(context, opcode);

    case 0x8c:
        return inst_8080_adc(context, opcode);

    case 0x8d:
        return inst_8080_adc(context, opcode);

    case 0x8e:
        return inst_8080_adc(context, opcode);

    case 0x8f:
        return inst_8080_adc(context, opcode);

    case 0x90:
        return inst_8080_sub(context, opcode);

    case 0x91:
        return inst_8080_sub(context, opcode);

    case 0x92:
        return inst_8080_sub(context, opcode);

    case 0x93:
        return inst_8080_sub(context, opcode);

    case 0x94:
        return inst_8080_sub(context, opcode);

    case 0x95:
        return inst_8080_sub(context, opcode);

    case 0x96:
        return inst_8080_sub(context, opcode);

    case 0x97:
        return inst_8080_sub(context, opcode);

    case 0x98:
        return inst_8080_sbb(context, opcode);

    case 0x99:
        return inst_8080_sbb(context, opcode);

    case 0x9a:
        return inst_8080_sbb(context, opcode);

    case 0x9b:
        return inst_8080_sbb(context, opcode);

    case 0x9c:
        return inst_8080_sbb(context, opcode);

    case 0x9d:
        return inst_8080_sbb(context, opcode);

    case 0x9e:
        return inst_8080_sbb(context, opcode);

    case 0x9f:
        return inst_8080_sbb(context, opcode);

    case 0xa0:
        return inst_8080_ana(context, opcode);

    case 0xa1:
        return inst_8080_ana(context, opcode);

    case 0xa2:
        return inst_8080_ana(context, opcode);

    case 0xa3:
        return inst_8080_ana(context, opcode);

    case 0xa4:
        return inst_8080_ana(context, opcode);

    case 0xa5:
        return inst_8080_ana(context, opcode);

    case 0xa6:
        return inst_8080_ana(context, opcode);

    case 0xa7:
        return inst_8080_ana(context, opcode);

    case 0xa8:
        return inst_8080_xra(context, opcode);

    case 0xa9:
        return inst_8080_xra(context, opcode);

    case 0xaa:
        return inst_8080_xra(context, opcode);

    case 0xab:
        return inst_8080_xra(context, opcode);

    case 0xac:
        return inst_8080_xra(context, opcode);

    case 0xad:
        return inst_8080_xra(context, opcode);

    case 0xae:
        return inst_8080_xra(context, opcode);

    case 0xaf:
        return inst_8080_xra(context, opcode);

    case 0xb0:
        return inst_8080_ora(context, opcode);

    case 0xb1:
        return inst_8080_ora(context, opcode);

    case 0xb2:
        return inst_8080_ora(context, opcode);

    case 0xb3:
        return inst_8080_ora(context, opcode);

    case 0xb4:
        return inst_8080_ora(context, opcode);

    case 0xb5:
        return inst_8080_ora(context, opcode);

    case 0xb6:
        return inst_8080_ora(context, opcode);

    case 0xb7:
        return inst_8080_ora(context, opcode);

    case 0xb8:
        return inst_8080_cmp(context, opcode);

    case 0xb9:
        return inst_8080_cmp(context, opcode);

    case 0xba:
        return inst_8080_cmp(context, opcode);

    case 0xbb:
        return inst_8080_cmp(context, opcode);

    case 0xbc:
        return inst_8080_cmp(context, opcode);

    case 0xbd:
        return inst_8080_cmp(context, opcode);

    case 0xbe:
        return inst_8080_cmp(context, opcode);

    case 0xbf:
        return inst_8080_cmp(context, opcode);

    case 0xc0:
        return inst_8080_r(context, opcode);

    case 0xc1:
        return inst_8080_pop(context, opcode);

    case 0xc2:
        return inst_8080_j(context, opcode);

    case 0xc3:
        return inst_8080_jmp(context, opcode);

    case 0xc4:
        return inst_8080_c(context, opcode);

    case 0xc5:
        return inst_8080_push(context, opcode);

    case 0xc6:
        return inst_8080_adi(context, opcode);

    case 0xc7:
        return inst_8080_rst(context, opcode);

    case 0xc8:
        return inst_8080_r(context, opcode);

    case 0xc9:
        return inst_8080_ret(context, opcode);

    case 0xca:
        return inst_8080_j(context, opcode);

    case 0xcb:
        return inst_8080_illegal(context, opcode);
    case 0xcc:
        return inst_8080_c(context, opcode);

    case 0xcd:
        return inst_8080_call(context, opcode);

    case 0xce:
        return inst_8080_aci(context, opcode);

    case 0xcf:
        return inst_8080_rst(context, opcode);

    case 0xd0:
        return inst_8080_r(context, opcode);

    case 0xd1:
        return inst_8080_pop(context, opcode);

    case 0xd2:
        return inst_8080_j(context, opcode);

    case 0xd3:
        return inst_8080_out(context, opcode);

    case 0xd4:
        return inst_8080_c(context, opcode);

    case 0xd5:
        return inst_8080_push(context, opcode);

    case 0xd6:
        return inst_8080_sui(context, opcode);

    case 0xd7:
        return inst_8080_rst(context, opcode);

    case 0xd8:
        return inst_8080_r(context, opcode);

    case 0xd9:
        return inst_8080_illegal(context, opcode);
    case 0xda:
        return inst_8080_j(context, opcode);

    case 0xdb:
        return inst_8080_in(context, opcode);

    case 0xdc:
        return inst_8080_c(context, opcode);

    case 0xdd:
        return inst_8080_illegal(context, opcode);
    case 0xde:
        return inst_8080_sbi(context, opcode);

    case 0xdf:
        return inst_8080_rst(context, opcode);

    case 0xe0:
        return inst_8080_r(context, opcode);

    case 0xe1:
        return inst_8080_pop(context, opcode);

    case 0xe2:
        return inst_8080_j(context, opcode);

    case 0xe3:
        return inst_8080_xthl(context, opcode);

    case 0xe4:
        return inst_8080_c(context, opcode);

    case 0xe5:
        return inst_8080_push(context, opcode);

    case 0xe6:
        return inst_8080_ani(context, opcode);

    case 0xe7:
        return inst_8080_rst(context, opcode);

    case 0xe8:
        return inst_8080_r(context, opcode);

    case 0xe9:
        return inst_8080_pchl(context, opcode);

    case 0xea:
        return inst_8080_j(context, opcode);

    case 0xeb:
        return inst_8080_xchg(context, opcode);

    case 0xec:
        return inst_8080_c(context, opcode);

    case 0xed:
        return inst_8080_illegal(context, opcode);
    case 0xee:
        return inst_8080_xri(context, opcode);

    case 0xef:
        return inst_8080_rst(context, opcode);

    case 0xf0:
        return inst_8080_r(context, opcode);

    case 0xf1:
        return inst_8080_pop(context, opcode);

    case 0xf2:
        return inst_8080_j(context, opcode);

    case 0xf3:
        return inst_8080_di(context, opcode);

    case 0xf4:
        return inst_8080_c(context, opcode);

    case 0xf5:
        return inst_8080_push(context, opcode);

    case 0xf6:
        return inst_8080_ori(context, opcode);

    case 0xf7:
        return inst_8080_rst(context, opcode);

    case 0xf8:
        return inst_8080_r(context, opcode);

    case 0xf9:
        return inst_8080_sphl(context, opcode);

    case 0xfa:
        return inst_8080_j(context, opcode);

    case 0xfb:
        return inst_8080_ei(context, opcode);

    case 0xfc:
        return inst_8080_c(context, opcode);

    case 0xfd:
        return inst_8080_illegal(context, opcode);
    case 0xfe:
        return inst_8080_cpi(context, opcode);

    case 0xff:
        return inst_8080_rst(context, opcode);
    }
    return 0;
}
void emu_8080_context_init(struct Context *context, const int mem_size)
{
    context->memory = (char *)malloc(mem_size);
}

void emu_8080_context_free(struct Context *context)
{
    free(context->memory);
}