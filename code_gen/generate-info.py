#!/usr/bin/python3
import os
import sys
import re
import yaml

opcodes = [{}] * 256
for i in range(256):
    opcodes[i] = {'name': "---", 'code': i}

REG = ['B', 'C', 'D', 'E', 'H', 'L', 'M', 'A']  # regular (8 bit registers)
REG_P = ['BC', 'DE', 'HL', 'SP']  # register pair (16 bit)
REG_PA = ['BC', 'DE', 'HL', 'PSW']  # alternative register pair
COND = ['NZ', 'Z', 'NC', 'C', 'PO', 'PE', 'P', 'M']  # jmp/call conditions
INST_REGEX = "([A-Z]+) *(D,S|D,#|D|S|RP|ccc|n)*"
INST_REGEX = "([A-Z]+)(ccc){0,1} *(n|p|a|#)*(D,S|D,#|D|S|RP|RP,#)* +([01SDNRPC]{8})"
CYCLES_REGEX = "^(\w+) (RP,#|M,S|D,M|D,S|D,#|M,#|RP|D|M|S|#|a|n|p)*\s+[01DSCNRP]{8} ([0-9]*)"
CYCLES_REGEX = "^(\w+) (D,S|M,S|D,M|D,#|M,#|RP,#|RP|D|M|S|#|a|n|p)*\s*([01]{2}DDDSSS|[01]{2}DDD[01]{3}|[01]{5}SSS|[01]{8}|[01]{2}CCC[01]{3}|[01]{2}NNN[01]{3}|[01]{2}RP[01]{4}) (\d*)"
CYCLES = [
    4, 10, 7, 5, 5, 5, 7, 4, 4, 10, 7, 5, 5, 5, 7, 4,  # 0x00..0x0f
    4, 10, 7, 5, 5, 5, 7, 4, 4, 10, 7, 5, 5, 5, 7, 4,  # 0x10..0x1f
    4, 10, 16, 5, 5, 5, 7, 4, 4, 10, 16, 5, 5, 5, 7, 4,  # etc
    4, 10, 13, 5, 10, 10, 10, 4, 4, 10, 13, 5, 5, 5, 7, 4,

    5, 5, 5, 5, 5, 5, 7, 5, 5, 5, 5, 5, 5, 5, 7, 5,  # 0x40..0x4f
    5, 5, 5, 5, 5, 5, 7, 5, 5, 5, 5, 5, 5, 5, 7, 5,
    5, 5, 5, 5, 5, 5, 7, 5, 5, 5, 5, 5, 5, 5, 7, 5,
    7, 7, 7, 7, 7, 7, 7, 7, 5, 5, 5, 5, 5, 5, 7, 5,

    4, 4, 4, 4, 4, 4, 7, 4, 4, 4, 4, 4, 4, 4, 7, 4,  # 0x80..8x4f
    4, 4, 4, 4, 4, 4, 7, 4, 4, 4, 4, 4, 4, 4, 7, 4,
    4, 4, 4, 4, 4, 4, 7, 4, 4, 4, 4, 4, 4, 4, 7, 4,
    4, 4, 4, 4, 4, 4, 7, 4, 4, 4, 4, 4, 4, 4, 7, 4,

    11, 10, 10, 10, 17, 11, 7, 11, 11, 10, 10, 10, 10, 17, 7, 11,  # 0xc0..0xcf
    11, 10, 10, 10, 17, 11, 7, 11, 11, 10, 10, 10, 10, 17, 7, 11,
    11, 10, 10, 18, 17, 11, 7, 11, 11, 5, 10, 5, 17, 17, 7, 11,
    11, 10, 10, 4, 17, 11, 7, 11, 11, 5, 10, 4, 17, 17, 7, 11,
]


def get_opcode_mask(code):
    i = 0
    for ch in code:
        i = i << 1
        if ch == '1':
            i |= 1
    return i


def fix_hex(n):
    return "0x{0:02x}".format(n)


def expand_ds(m):
    opcode_base = get_opcode_mask(m[4])
    result = []
    for dest in range(8):
        for source in range(8):
            code = opcode_base | (dest << 3) | source
            inst = m[0] + ' ' + REG[dest] + ',' + REG[source]
            result.append({
                'name': m[0],
                'code': code,
                'opcode': inst,
                'format': inst,
                'data': 0,
                'cycles': CYCLES[code]
            })
    return result


def expand_destination(m):
    opcode_base = get_opcode_mask(m[4])
    result = []
    for dest in range(8):
        code = opcode_base | (dest << 3)
        inst = m[0] + ' ' + REG[dest]
        result.append({
            'name': m[0],
            'code': code,
            'opcode': inst,
            'format': inst,
            'data': 0,
            'cycles': CYCLES[code]
        })
    return result


def expand_destination_c(m):
    opcode_base = get_opcode_mask(m[4])
    result = []
    for dest in range(8):
        code = opcode_base | (dest << 3)
        inst = m[0] + ' ' + REG[dest] + ', #'
        result.append({
            'name': m[0],
            'code': code,
            'opcode': inst,
            'format': m[0] + ' ' + REG[dest] + ', {0:02x}',
            'data': 1,
            'regex': m[0] + ' ' + REG[dest] + ',[\\t ]*([0-9]+)',
            'cycles': CYCLES[code]
        })
    return result


def expand_source(m):
    opcode_base = get_opcode_mask(m[4])
    result = []
    for source in range(8):
        code = opcode_base | source
        inst = m[0] + ' ' + REG[source]
        result.append({
            'name': m[0],
            'code': code,
            'opcode': inst,
            'format': inst,
            'data': 0,
            'cycles': CYCLES[code]
        })
    return result


def expand_register_pair(m):
    if m[0] == 'LDAX' or m[0] == 'STAX':
        l = 2
    else:
        l = 4
    if m[0] == 'PUSH' or m[0] == 'POP':
        reg = REG_PA
    else:
        reg = REG_P
    opcode_base = get_opcode_mask(m[4])
    result = []
    for rp in range(l):
        code = opcode_base | rp << 4
        inst = m[0] + ' ' + reg[rp]
        result.append({
            'name': m[0],
            'code': code,
            'opcode': inst,
            'format': inst,
            'data': 0,
            'cycles': CYCLES[code]
        })
    return result


def expand_register_pair_c(m):
    opcode_base = get_opcode_mask(m[4])
    result = []
    for rp in range(4):
        code = opcode_base | rp << 4
        inst = m[0] + ' ' + REG_P[rp] + ', #16'
        result.append({
            'name': m[0],
            'code': code,
            'opcode': inst,
            'format': m[0] + ' ' + REG_P[rp] + ', {0:04x}',
            'data': 2,
            'regex': m[0] + ' ' + REG_P[rp] + ',[\\t ]*([0-9]+)',
            'cycles': CYCLES[code]
        })
    return result


def expand_n(m):
    opcode_base = get_opcode_mask(m[4])
    result = []
    for n in range(8):
        code = opcode_base | n << 3
        inst = m[0] + ' ' + str(n)
        result.append({
            'name': m[0],
            'code': code,
            'opcode': inst,
            'format': inst,
            'data': 0,
            'cycles': CYCLES[code]
        })
    return result


def expand_condition(m):
    opcode_base = get_opcode_mask(m[4])
    result = []
    for con in range(8):
        code = opcode_base | con << 3
        regex = None
        if m[2] == 'a':  # ret
            inst = m[0] + COND[con] + ' a'
            fmt = m[0] + COND[con] + ' {0:04x}'
            regex = m[0] + COND[con] + '[\\t ]+([0-9]+)'
            data = 2
        else:
            inst = m[0] + COND[con]
            fmt = inst
            data = 0
        record = {
            'name': m[0],
            'code': code,
            'opcode': inst,
            'format': fmt,
            'data': data,
            'cycles': CYCLES[code]
        }
        if regex is not None:
            record['regex'] = regex

        result.append(record)
    return result


def expand_none(m):
    code = get_opcode_mask(m[4])
    regex = None
    result = []
    if m[2] == '#' or m[2] == 'p':
        inst = m[0] + ' ' + m[2]
        fmt = m[0] + ' ' + '{0:02x}'
        regex = m[0] + '[\\t ]*([0-9]+)'
        data = 1
    elif m[2] == 'a':
        inst = m[0] + ' ' + m[2]
        fmt = m[0] + ' ' + '{0:04x}'
        regex = m[0] + '[\\t ]*([0-9]+)'
        data = 2
    else:
        inst = m[0]
        fmt = m[0]
        data = 0
    record = {
        'name': m[0],
        'code': code,
        'opcode': inst,
        'format': fmt,
        'data': data,
        'cycles': CYCLES[code]
    }
    if regex is not None:
        record['regex'] = regex
    result.append(record)
    return result


def expand_inst(m):
    if m[1] == 'ccc':
        opcode = expand_condition(m)
    elif m[2] == 'n':
        opcode = expand_n(m)
    elif m[3] == 'D,S':
        opcode = expand_ds(m)
    elif m[3] == 'D':
        opcode = expand_destination(m)
    elif m[3] == 'D,#':
        opcode = expand_destination_c(m)
    elif m[3] == 'RP,#':
        opcode = expand_register_pair_c(m)
    elif m[3] == 'S':
        opcode = expand_source(m)
    elif m[3] == 'RP':
        opcode = expand_register_pair(m)
    elif m[3] == '':
        opcode = expand_none(m)
    else:
        print(m)
        sys.exit(1)
    for op in opcode:
        opcodes[op['code']] = op


def get_instruction_info(m):
    code = get_opcode_mask(m[4])
    return {
        'name': m[0],
        'data': opcodes[code]['data'],
        'cycles': CYCLES[code]
    }


if __name__ == '__main__':
    current_dir = os.path.dirname(__file__)

    my_file = open(f'{current_dir}/instruction_codes.txt',
                   'r', encoding='utf-8')
    content_list = my_file.read()
    inst_regex = re.compile(INST_REGEX)
    matches = inst_regex.findall(content_list)

    for m in matches:
        #    print(m)
        expand_inst(m)
    # for inst in opcodes:
    #    print(inst)

    instructions = []
    for m in matches:
        instructions.append(get_instruction_info(m))

    with open( f'{current_dir}/instructions.yaml', 'w', encoding='utf-8') as file:
        documents = yaml.dump(instructions, file)

    with open(current_dir + f'{current_dir}/opcodes.yaml', 'w', encoding='utf-8') as file:
        documents = yaml.dump(opcodes, file)
