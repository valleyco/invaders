#!/usr/bin/python3
from itertools import cycle
import yaml
import numpy as np
from jinja2 import FileSystemLoader, Environment, PackageLoader, select_autoescape
from jinja2 import Template

env = Environment(
    loader=FileSystemLoader('./')
)

with open(r'./opcodes.yaml') as file:
    opcodes = yaml.full_load(file)

template = env.get_template('./disasm-8080-template.c')

output = template.render(opcodes=opcodes)
with open(r'./disasm-8080.c', 'w') as file:
    file.write(output)
