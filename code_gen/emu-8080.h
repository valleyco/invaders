#pragma once

#define REG_B 0
#define REG_C 1
#define REG_D 2
#define REG_E 3
#define REG_H 4
#define REG_L 5
// will not be used as index
#define REG_M 6
#define REG_FLAG 6
#define REG_A 7

#define RP_BC 0x00
#define RP_DE 0x10
#define RP_HL 0x20
#define RP_SP 0x30

/*
0 C: Carry flag; set to carry out of bit 7 in result
1 V: Undocumented signed overflow flag* (8085 only, on 8080 bit is always 1 in PSW)
2 P: Parity flag; set if the number of bits in the result is even, and reset if it's odd
3    No flag, bit is always 0 in PSW
4 A: Auxiliary Carry (AC) flag; set to carry out of bit 3 in result
5 K: Undocumented signed Underflow Indicator (UI/X5) and comparison flag* (8085 only, on 8080 bit is always 0 in PSW)
6 Z: Zero flag; set if the result was zero, reset otherwise
7 S: Sign flag; set to bit 7 of result
*/
#define Z_FLAG 0
#define C_FLAG 1
#define P_FLAG 2
#define S_FLAG 3
#define A_FLAG 4

#define C_NZ 0
#define C_Z 1
#define C_NC 2
#define C_C 3
#define C_PO 4
#define C_PE 5
#define C_P 6
#define C_M 7

typedef unsigned char reg8_t;
typedef int reg16_t;

struct Context
{
    reg8_t reg[8];
    reg16_t SP;
    reg16_t PC;
    int flag[6];
    int halt;
    int interrupt;
    unsigned char *memory;
    int (*port_read)(void *g, int p);
    void (*port_write)(void *g, int p, int v);
    int address_mask;
    int rom_size;
    reg8_t M;
    void *gData;
};
#ifdef __cplusplus
extern "C"
{
#endif

    int emu_8080_rst(struct Context *context, int n);

    int emu_8080_execute(struct Context *context);
#ifdef __cplusplus
}
#endif
