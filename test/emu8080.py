import os
from ctypes import cdll, c_int, c_char, c_ubyte, c_void_p, CFUNCTYPE, POINTER, pointer, create_string_buffer, Structure

emu8080 = cdll.LoadLibrary(
    os.path.dirname(__file__) +
    "/../8080/src/.libs/libemu_8080.so"
)
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
    'm': REG_M,
}

flag_map = {
    'Z': Z_FLAG,
    'C': C_FLAG,
    'P': P_FLAG,
    'S': S_FLAG,
    'A': A_FLAG,
}

reg8_t = c_ubyte
reg16_t = c_int


class Emu8080Context(Structure):
    """interface to the C structure"""
    _fields_ = [
        ("registers", reg8_t * 8),
        ("SP", reg16_t),
        ("PC", reg16_t),
        ("flags", c_int * 6),
        ("halt", c_int),
        ("interrupt", c_int),
        ("memory", POINTER(c_char * 32768)),
        ("port_read", CFUNCTYPE(c_void_p, c_int)),
        ("port_write", CFUNCTYPE(c_void_p, c_int, c_int)),
        ('address_mask', c_int),
        ('rom_size', c_int),
        ('M', reg8_t),
        ('gData', c_void_p),
    ]

    def __init__(self):
        super().__init__()
        self.memory = pointer(create_string_buffer(32768))
        self.address_mask = 0x3FFF
        self.rom_size = 0

    def print(self):
        m_addr = self.registers[REG_L] + (self.registers[REG_H] * 256)
        print("REG: a  b  c  d  e  h  l  M  pc   sp     Z C P S A")
        print("     {0:02x} {1:02x} {2:02x} {3:02x} {4:02x} {5:02x} {6:02x} {7:02x} {8:04x} {9:04x}   {10:1} {11:1} {12:1} {13:1} {14:1}".format(
            self.registers[REG_A],
            self.registers[REG_B],
            self.registers[REG_C],
            self.registers[REG_D],
            self.registers[REG_E],
            self.registers[REG_H],
            self.registers[REG_L],
            ord(self.memory.contents[m_addr]) if m_addr < 32768 else 0,
            self.PC,
            self.SP,
            self.flags[Z_FLAG] != 0,
            self.flags[C_FLAG] != 0,
            self.flags[P_FLAG] != 0,
            self.flags[S_FLAG] != 0,
            self.flags[A_FLAG] != 0,
        ))
