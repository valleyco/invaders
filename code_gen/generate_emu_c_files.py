from itertools import cycle
import yaml
import numpy as np
from jinja2 import FileSystemLoader, Environment, PackageLoader, select_autoescape
from jinja2 import Template


env = Environment(
    loader=FileSystemLoader('./')
)

with open(r'./instructions.yaml') as file:
    instructions = yaml.full_load(file)
with open(r'./opcodes.yaml') as file:
    opcodes = yaml.full_load(file)

template = env.get_template('./template-emu-8080.c')

output = template.render(instructions=instructions, opcodes= opcodes)
with open(r'./emu-8080.c','w') as file:
    file.write(output);
#print(output)
cycles=[]
#print(instructions)
def get_cycles():
    cycles = np.zeros(256, dtype=int)
    for i in range(256):
        cycles[i] =instructions[i]['cycles']

