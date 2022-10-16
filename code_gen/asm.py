#!/usr/bin/python3
import yaml
import re

no_param_instructions = {}
param_instructions = {}

with open(r'./opcodes.yaml') as file:
    opcodes = yaml.full_load(file)

for opcode in opcodes:
    if opcode['name'] == '---':
        continue
    if opcode['data'] == 0:
        no_param_instructions[opcode['opcode']] = [opcode['code'], 0]
    else:
        param_instructions[opcode['regex']] = [opcode['code'], opcode['data']]

def asm(line: str):
    cmd = line.strip().replace("  ", " ").upper()

    if cmd in no_param_instructions:
        return [no_param_instructions[cmd][0]]

    for regex in param_instructions:
        match = re.match(regex, cmd)
        if match:
            date_len = param_instructions[regex][1]
            param = int(match.group(1))
            if(date_len == 1):
                return [param_instructions[regex][0], param & 0xFF]
            else:
                return [param_instructions[regex][0], param & 0xFF, (param & 0xFF00) >> 8]
    return []

if __name__ == '__main__':
    print(asm("mov b,a"))
    print(asm("adi 8"))
    print(asm("JZ 258"))
