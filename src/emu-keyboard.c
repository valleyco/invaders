#include "emu-keyboard.h"

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

void emu_keyboard_done(PortDevice *device)
{
    free(device->data);
    free(device);
}

static int get_key_action(int keyVal)
{
    switch (keyVal)
    {
    case GDK_KEY_Left:
    case GDK_KEY_Z:
    case GDK_KEY_z:
        return KEY_P1_LEFT;
    case GDK_KEY_X:
    case GDK_KEY_x:
    case GDK_KEY_space:
        return KEY_P1_SHOT;
    case GDK_KEY_Right:
    case GDK_KEY_C:
    case GDK_KEY_c:
        return KEY_P1_RIGHT;
    case GDK_KEY_less:
    case GDK_KEY_comma:
        return KEY_P2_LEFT;
    case GDK_KEY_greater:
    case GDK_KEY_period:
        return KEY_P2_SHOT;
    case GDK_KEY_question:
    case GDK_KEY_slash:
        return KEY_P2_RIGHT;
    case GDK_KEY_1:
        return KEY_P1_START;
    case GDK_KEY_2:
        return KEY_P2_START;
    // case GDK_KEY_space:
    //     return KEY_SHOT;
    // case GDK_KEY_Left:
    //     return KEY_LEFT;
    // case GDK_KEY_Right:
    //     return KEY_RIGHT;
    case GDK_KEY_I:
    case GDK_KEY_i:
        return KEY_CREDIT;
    default:
        return 0;
    }
}

int handle_keyboard_event(PortDevice *device, int keyVal, int pressed)
{
    int key = get_key_action(keyVal);
    ((int *)device->data)[key] = pressed ? 1 : 0;
    return key > 0;
}
