#!/usr/bin/python3
import os
import re
import yaml
current_dir = os.path.dirname(__file__)

no_param_instructions = {}
param_instructions = {}

with open(current_dir + r'/../code_gen/opcodes.yaml', encoding='utf-8') as file:
    opcodes = yaml.full_load(file)

for opcode in opcodes:
    if opcode['name'] == '---':
        continue
    if opcode['data'] == 0:
        no_param_instructions[opcode['opcode']] = opcode['code']
    else:
        param_instructions[opcode['regex']] = opcode


def asm(line: str) -> list:
    cmd = line.strip().replace("  ", " ").upper()

    if cmd in no_param_instructions:
        return [no_param_instructions[cmd]]

    for regex, instruction in param_instructions.items():
        match = re.match(regex, cmd)
        if match:
            data_len = instruction['data']
            param = int(match.group(1))
            if data_len == 1:
                return [instruction['code'], param & 0xFF]
            else:
                return [instruction['code'], param & 0xFF, (param & 0xFF00) >> 8]
    return []


if __name__ == '__main__':
    print(asm("mov b,a"))
    print(asm("adi 8"))
    print(asm("JZ 258"))
