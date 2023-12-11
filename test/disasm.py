#!/usr/bin/python3
import os
import yaml

decode_table = {}


def decode(buffer, offset: int):
    if (buffer[offset] not in decode_table):
        return None
    opcode = decode_table[buffer[offset]]
    data_len = opcode['data']
    # if offset+ data_len >= bytes.
    result = {"bytes": [], "asm": None}
    for i in range(data_len + 1):
        result["bytes"].append(buffer[offset + i])

    if data_len == 0:
        result["asm"] = opcode['format']
        return result
    elif data_len == 1:
        param = buffer[offset + 1]
    else:
        param = buffer[offset + 1] + (buffer[offset + 2] << 8)

    result["asm"] = opcode['format'].format(param)
    return result


def init():
    current_dir = os.path.dirname(__file__)
    with open( f'{current_dir}/../code_gen/opcodes.yaml', encoding='utf-8') as file:
        opcodes = yaml.full_load(file)

    for opcode in opcodes:
        if opcode['name'] == '---':
            continue
        decode_table[opcode['code']] = opcode


if __name__ == '__main__':
    init()
    pos = 0
    code = [71, 198, 8, 202, 2, 1]
    code = [64, 72, 80, 88, 96, 104, 70, 120]
    while pos < len(code):
        data = decode(code, pos)
        print(data)
        pos += len(data["bytes"])
