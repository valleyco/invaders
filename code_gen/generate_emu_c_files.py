#!/usr/bin/python3
import os
import yaml
from jinja2 import FileSystemLoader, Environment

current_dir = os.path.dirname(__file__)
env = Environment(
    loader=FileSystemLoader('./')
)

with open(f'{current_dir}/instructions.yaml', encoding='utf-8') as file:
    instructions = yaml.full_load(file)
with open('{current_dir}/opcodes.yaml', encoding='utf-8') as file:
    opcodes = yaml.full_load(file)

template = env.get_template(current_dir + '/template-emu-8080.c')

output = template.render(instructions=instructions, opcodes=opcodes)
with open( f'{current_dir}/emu-8080.c', 'w', encoding='utf-8') as file:
    file.write(output)
