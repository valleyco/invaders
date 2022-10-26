#include <stdlib.h>
typedef unsigned char byte;
struct Opcode
{
    int code;
    int len;
    char *format;
};

struct Opcode opcode[256] = {
{% for opcode in opcodes -%}
{% if opcode.name == '---' %}
    { -1, -1, NULL},{% else %}  
    { {{opcode.code}}, {{opcode.data}},"char"},
{%- endif %}
{%- endfor %}
};

int disasm(const byte *mem, const char *buffer)
{
    return 3;
}
int main()
{
    return 0;
}