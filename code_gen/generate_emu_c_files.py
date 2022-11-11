#!/usr/bin/python3
import os
import yaml
import numpy as np
from jinja2 import FileSystemLoader, Environment


current_dir = os.path.dirname(__file__)
env = Environment(
    loader=FileSystemLoader('./')
)

with open(current_dir + r'/instructions.yaml') as file:
    instructions = yaml.full_load(file)
with open(current_dir + '/opcodes.yaml') as file:
    opcodes = yaml.full_load(file)

template = env.get_template(current_dir + '/template-emu-8080.c')

output = template.render(instructions=instructions, opcodes= opcodes)
with open(current_dir + r'/emu-8080.c','w') as file:
    file.write(output);
#print(output)
cycles=[]
#print(instructions)
def get_cycles():
    cycles = np.zeros(256, dtype=int)
    for i in range(256):
        cycles[i] =instructions[i]['cycles']

