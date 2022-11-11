#!/usr/bin/python3
import os
import sys
import traceback
from ctypes import *
import yaml
from asm import asm

current_dir = os.path.dirname(__file__)
emu8080 = cdll.LoadLibrary(current_dir +
                           "/../8080/src/.libs/libemu_8080.so")
REG_B = 0
REG_C = 1
REG_D = 2
REG_E = 3
REG_H = 4
REG_L = 5
REG_M = 6
REG_A = 7

Z_FLAG = 0
C_FLAG = 1
P_FLAG = 2
S_FLAG = 3
A_FLAG = 4

reg_map = {
    'a': REG_A,
    'b': REG_B,
    'c': REG_C,
    'd': REG_D,
    'e': REG_E,
    'h': REG_H,
    'l': REG_L,
    'm': REG_A,
}

flag_map = {
    'Z': Z_FLAG,
    'C': C_FLAG,
    'P': P_FLAG,
    'S': S_FLAG,
    'A': A_FLAG,
}


class Emu8080Context(Structure):
    _fields_ = [
        ("registers", c_int * 8),
        ("SP", c_int),
        ("PC", c_int),
        ("flag", c_int * 6),
        ("halt", c_int),
        ("interrupt", c_int),
        ("memory", POINTER(c_char * 32768)),
        ("port_read", CFUNCTYPE(c_int, c_int)),
        ("port_write", CFUNCTYPE(c_int, c_int, c_int)),
        ('address_mask', c_int),
    ]

    def __init__(self):
        super().__init__()
        self.memory = pointer(create_string_buffer(32768))

    def print(self):
        pass
        print("REG: a  b  c  d  e  h  l  pc  sp")
        print("     {0:02x} {1:02x} {2:02x} {3:02x} {4:02x} {5:02x} {6:02x} {7:04x} {8:04x} ".format(
            self.registers[REG_A],
            self.registers[REG_B],
            self.registers[REG_C],
            self.registers[REG_D],
            self.registers[REG_E],
            self.registers[REG_H],
            self.registers[REG_L],
            self.PC,
            self.SP,
        ))


tests = None
with open(current_dir + "/test.yml", "r") as stream:
    try:
        tests = yaml.safe_load(stream)
    except yaml.YAMLError as exc:
        print(exc)
        exit(1)


def expected_memory_location(location: int, data: list, context: Emu8080Context):
    for i, expected in enumerate(data):
        actual = ord(context.memory.contents[location + i])
        if(expected != actual):
            raise Exception(
                f"memory location {location + i} expected: {expected} found: {actual}")


def expected_memory(expected: list, context: Emu8080Context):
    for e in expected:
        expected_memory_location(e["location"], e["data"], context)


def expected_registers(registers: dict, context: Emu8080Context):
    for reg in registers:
        if reg == 'sp':
            actual = context.SP
        elif reg == 'pc':
            actual = context.PC
        else:
            actual = context.registers[reg_map[reg]]
        expected = registers[reg]
        if(expected != actual):
            raise Exception(
                f"register '{reg}' expected: {expected} found: {actual}")


def check_results(expected: dict, context: Emu8080Context):
    if 'registers' in expected:
        expected_registers(expected['registers'], context)
    if 'memory' in expected:
        expected_memory(expected['memory'], context)


def test_init_memory(locations: list, context: Emu8080Context):
    for location in locations:
        offset = location['location']
        for i, value in enumerate(location['data']):
            context.memory.contents[offset + i] = value


def test_init_flags(flags: dict, context: Emu8080Context):
    for flag in flag:
        context.flag[flag_map[flag]] = flags[flag]


def test_init_registers(registers: dict, context: Emu8080Context):
    for reg in registers:
        if reg == "pc":
            context.PC = registers[reg]
        elif reg == "sp":
            context.SP = registers[reg]
        elif reg == "halt":
            context.halt = registers[reg]
        else:
            context.registers[reg_map[reg]] = registers[reg]


def test_init_state(state: dict, context: Emu8080Context):
    if 'registers' in state:
        test_init_registers(state['registers'], context)
    if 'memory' in state:
        test_init_memory(state['memory'], context)
    if 'flags' in state:
        test_init_flags(state['flags'], context)
    # context.print()


def test_reset_state(context: Emu8080Context):
    for i in range(0, 32768):
        context.memory.contents[i] = 0
    for i in range(0, 8):
        context.registers[i] = 0
    context.PC = 0
    context.SP = 0


def asm_code(instructions: list) -> list:
    buffer = []
    for line in instructions:
        code = asm(line)
        if len(code) == 0:
            raise Exception("invalid instruction: {0}".format(line))
        buffer += code
    return buffer


def run_test_code(instructions: list, context: Emu8080Context):
    code = asm_code(instructions)
    print(code, len(instructions))
    for i, b in enumerate(code):
        context.memory.contents[i] = b
    for i in range(0, len(instructions)):
        emu8080.emu_8080_execute(byref(context))


def run_test(test, context: Emu8080Context):
    if (not 'reset' in test) or test['reset']:
        test_reset_state(context)
    if ('state' in test):
        test_init_state(test['state'], context)
    run_test_code(test['code'], context)
    check_results(test['expected'], context)


def run_tests(tests: dict, context: Emu8080Context):
    for test in tests:
        try:
            print(f"executing : '{test['name']}'")
            run_test(test, context)
            print(f"success : '{test['name']}'")

        except Exception as ex:
            print(f"failed : '{test['name']}'")
            context.print()
            for msg in ex.args:
                tb = sys.exc_info()[-1]
                print(tb)
                stk = traceback.extract_tb(tb, 1)
                fname = stk[0][2]
                print(f"\t- {msg} in {fname}")


context = Emu8080Context()

run_tests(tests, context)


# print("size of python context ",sizeof(Emu8080Context))
# context.reg[7] = 9
# emu_8080_context_init = lib.emu_8080_context_init
# emu_8080_context_init.argtypes = [POINTER(Emu8080Context), c_int]
# emu_8080_context_init(byref(context), 100)
# print(context.memory.contents)
# print(memory.contents[1])
