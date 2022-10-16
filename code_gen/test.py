#!/usr/bin/python3
import yaml
with open('../test/test.yml') as file:
    tests = yaml.full_load(file)

def set_registers(reg: dict):
    pass

def set_memory(memory: dict):
    pass

def run_test(test: dict):
    pass    

for test in tests:
    run_test(test)

