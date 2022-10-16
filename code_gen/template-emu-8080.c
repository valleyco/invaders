#include <stdio.h>
#include <stdlib.h>
#include "emu-8080.h"
#include "emu-8080.c.h"

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
