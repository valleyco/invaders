#!/usr/bin/python3
#import numpy as np
import yaml
import os
current_dir = os.path.dirname(__file__)
with open(current_dir + r'/../code_gen/opcodes.yaml') as file:
    opcodes = yaml.full_load(file)
decode_table = {}

for opcode in opcodes:
    if opcode['name'] == '---':
        continue
    decode_table[opcode['code']] = opcode


def decode(bytes, offset: int):
    if(bytes[offset] not in decode_table):
        return None
    decode = decode_table[bytes[offset]]
    data_len = decode['data']
    # if offset+ data_len >= bytes.
    data = {"bytes": [], "asm": None}
    for i in range(data_len + 1):
        data["bytes"].append(bytes[offset + i])

    if data_len == 0:
        data["asm"] = decode['format']
        return data
    elif data_len == 1:
        param = bytes[offset + 1]
    else:
        param = bytes[offset + 1] + (bytes[offset + 2] << 8)

    data["asm"] = decode['format'].format(param)
    return data


if __name__ == '__main__':
    pos = 0
    code = [71, 198, 8, 202, 2, 1]
    code =[64, 72, 80, 88, 96, 104, 70, 120]
    while pos < len(code):
        data = decode(code, pos)
        print(data)
        pos += len(data["bytes"])
