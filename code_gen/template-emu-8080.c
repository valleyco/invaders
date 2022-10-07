#include <stdio.h>
#include <stdlib.h>
#include "emu-8080.h"

extern int fetch_pc_byte(struct Context* context){
    return context->memory[(context->PC++ & 0xffff)];
}
extern int fetch_pc_word(struct Context* context){
    return fetch_pc_byte(context) + fetch_pc_byte(context) << 8;
}

static inline int get_source(int op, struct Context* context){
    return op & 0x7;
}

static inline int get_destination(int op, struct Context* context){
    return (op >> 3) & 0x7;
}

static inline int get_rp(int op){
    return (op >> 4) & 0x3;
}

static inline int get_condition(int op){
    return (op >> 3) & 0x7;
}

static inline int is_parity_even(int b){
    b ^= b >> 4;
    b ^= b >> 2;
    b ^= b >> 1;
    return (~b) & 1;
}
// Z_FLAG = 0;
// C_FLAG = 1;
// P_FLAG = 2;
// S_FLAG = 3;

// AC is not update here will be impleented in the relevant instruction
static inline void update_flags(struct Context *context, int reg){
    context->flag[Z_FLAG] = context->reg[reg] == 0;
    context->flag[C_FLAG] = (context->reg[reg] & 0xf00) > 0;
    context->flag[P_FLAG] = is_parity_even(context->reg[reg]) ;
    context->flag[S_FLAG] = context->reg[reg] & 0x80;
    context->reg[reg] &= 0xff;

} 

static inline void pack_flags(struct Context *context){
        context->reg[REG_FLAG] = 
            context->flag[C_FLAG] ?  0x01 : 0 |
                                     0x02     |
            context->flag[P_FLAG] ?  0x04 : 0 |
                                     0x08 * 0 |
            context->flag[A_FLAG] ?  0x10 : 0 |
                                     0x20 * 0 | 
            context->flag[Z_FLAG] ?  0x40 : 0 |
            context->flag[S_FLAG] ?  0x80 : 0;

}

static inline void unpack_flags(struct Context *context)
{
    context->flag[C_FLAG] = context->reg[REG_FLAG] & 0x01;
    context->flag[P_FLAG] = context->reg[REG_FLAG] & 0x04;
    context->flag[A_FLAG] = context->reg[REG_FLAG] & 0x10;
    context->flag[Z_FLAG] = context->reg[REG_FLAG] & 0x40;
    context->flag[S_FLAG] = context->reg[REG_FLAG] & 0x80;
}

static inline int get_m(struct Context *context){
    return context->memory[context->reg[REG_L] + (context->reg[REG_H] << 8) ];
}

static inline void set_m(struct Context *context, int val){
    context->memory[context->reg[REG_L] + (context->reg[REG_H] << 8) ] = val;
}


// http://www.nacad.ufrj.br/online/intel/vtune/users_guide/mergedProjects/analyzer_ec/mergedProjects/reference_olh/mergedProjects/instructions/instruct32_hh/vc71.htm
{% for inst in instructions -%}
static inline int inst_8080_{{ inst.name | lower }}(struct Context* context, int op){
    const int cycles = {{ inst.cycles }}; 
    // instruction not implemented yet
    return cycles;
}

{% endfor %}
inline static int inst_8080_illegal(struct Context* context, int op){
    return 0;
}

int emu_8080_rst(struct Context* context, int n){
    inst_8080_rst(context, 0b11000111 | (n << 3));
}

int emu_8080_execute(struct Context* context){
    int opcode = fetch_pc_byte(context);
    switch(opcode){
{%- for opcode in opcodes %}
    case 0x{{ "%02x" | format( opcode.code | int) }}:
{%- if opcode.name != '---' %}
        return inst_8080_{{ opcode.name | lower }}(context, opcode);
{% else %}
    return inst_8080_illegal(context, opcode);
{%- endif %}
{%- endfor %}
    }
    return 0;
}
void emu_8080_context_init(struct Context *context, const int mem_size){
    context->memory = (char*)malloc(mem_size);
}

void emu_8080_context_free(struct Context *context){
    free(context->memory);
}
