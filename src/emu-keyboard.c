#include "emu-keyboard.h"
typedef struct // this first field should match struct PortDevice
{
    int key_status[KEY_MAX_ID + 1];
} KeyboardDeviceData;

static int port_bit_map[3][8] = {
    // MSB -> LSB
    {KEY_VIRTUAL_OFF, KEY_RIGHT, KEY_LEFT, KEY_SHOT, KEY_VIRTUAL_ON, KEY_VIRTUAL_ON, KEY_VIRTUAL_ON, KEY_DIP_4},
    {KEY_VIRTUAL_OFF, KEY_P1_RIGHT, KEY_P1_LEFT, KEY_P1_SHOT, KEY_VIRTUAL_ON, KEY_P1_START, KEY_P2_START, KEY_CREDIT},
    {KEY_DIP_7, KEY_P2_RIGHT, KEY_P2_LEFT, KEY_P2_SHOT, KEY_DIP_6, KEY_TILT, KEY_DIP_5, KEY_DIP_3},
};

static inline int emu_keyboard_read(PortDevice *device, int v_port)
{
    int result = 0;
    for (int i = 0; i < 8; i++)
    {
        result <<= 1;
        result |= ((int *)device->data)[port_bit_map[v_port][i]];
    }
    return result;
}

static int emu_keyboard_read_port_0(PortDevice *device)
{
    return emu_keyboard_read(device, 0);
}

static int emu_keyboard_read_port_1(PortDevice *device)
{
    return emu_keyboard_read(device, 1);
}

static int emu_keyboard_read_port_2(PortDevice *device)
{
    return emu_keyboard_read(device, 2);
}

static int (*port_read_array[])(PortDevice *g) = {emu_keyboard_read_port_0, emu_keyboard_read_port_1, emu_keyboard_read_port_2};

static void emu_keyboard_done(PortDevice *device)
{
    free(device->data);
    free(device);
}

PortDevice *emu_keyboard_init(KeyEvent *keyEventHandler)
{
    PortDevice *device = malloc(sizeof(PortDevice));
    memset(device, 0, sizeof(PortDevice));
    device->dispose = emu_keyboard_done;
    device->readPortCount = 3;
    device->read = (PortRead *)port_read_array;
    device->writePortCount = 0;
    device->write = NULL;
    int *key_status = malloc(sizeof(int) * (KEY_MAX_ID + 1));
    memset(key_status, 0, sizeof(int) * (KEY_MAX_ID + 1));
    key_status[KEY_VIRTUAL_ON] = 1;
    device->data = key_status;
    *keyEventHandler = handle_keyboard_event;
    return device;
}

static struct EmuKeyboardMap
{
    int key;
    int si_key;
} emu_keyboard_map[] = {
    {.key = GDK_KEY_Left, .si_key = KEY_P1_LEFT},
    {.key = GDK_KEY_Z, .si_key = KEY_P1_LEFT},
    {.key = GDK_KEY_z, .si_key = KEY_P1_LEFT},

    {.key = GDK_KEY_X, .si_key = KEY_P1_SHOT},
    {.key = GDK_KEY_x, .si_key = KEY_P1_SHOT},
    {.key = GDK_KEY_space, .si_key = KEY_P1_SHOT},

    {.key = GDK_KEY_Right, .si_key = KEY_P1_RIGHT},
    {.key = GDK_KEY_C, .si_key = KEY_P1_RIGHT},
    {.key = GDK_KEY_c, .si_key = KEY_P1_RIGHT},

    {.key = GDK_KEY_less, .si_key = KEY_P2_LEFT},
    {.key = GDK_KEY_comma, .si_key = KEY_P2_LEFT},

    {.key = GDK_KEY_greater, .si_key = KEY_P2_SHOT},
    {.key = GDK_KEY_period, .si_key = KEY_P2_SHOT},

    {.key = GDK_KEY_question, .si_key = KEY_P2_RIGHT},
    {.key = GDK_KEY_slash, .si_key = KEY_P2_RIGHT},

    {.key = GDK_KEY_1, .si_key = KEY_P1_START},
    {.key = GDK_KEY_2, .si_key = KEY_P2_START},
    {.key = GDK_KEY_I, .si_key = KEY_CREDIT},
    {.key = GDK_KEY_i, .si_key = KEY_CREDIT},
    {.key = 0},
};

int handle_keyboard_event(PortDevice *device, int keyVal, int pressed)
{
    for (struct EmuKeyboardMap *map = emu_keyboard_map; map->key; map++)
    {
        if (map->key == keyVal)
        {
            ((int *)device->data)[map->si_key] = pressed ? 1 : 0;
            return 1;
        }
    }
    return 0;
}
