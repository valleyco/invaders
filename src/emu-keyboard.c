#include "emu-keyboard.h"

static int port_bit_map[3][8] = {
    {KEY_DIP_4, KEY_VIRTUAL_ON, KEY_VIRTUAL_ON, KEY_VIRTUAL_ON, KEY_SHOT, KEY_LEFT, KEY_RIGHT, KEY_VIRTUAL_OFF},
    {KEY_CREDIT, KEY_P2_START, KEY_P1_START, KEY_VIRTUAL_ON, KEY_P1_SHOT, KEY_P1_LEFT, KEY_P1_RIGHT, KEY_VIRTUAL_OFF},
    {KEY_DIP_3, KEY_DIP_5, KEY_TILT, KEY_DIP_6, KEY_P2_SHOT, KEY_P2_LEFT, KEY_P2_RIGHT, KEY_DIP_6},
};

static int get_key_action(int keyVal);

int handle_keyboard_event(struct KeyboardDevice *device, int keyVal, int pressed)
{
    int key = get_key_action(keyVal);
    device->key_status[key] = pressed ? 1 : 0;
    printf("handle_keyboard_event key: %d -> %d\n", keyVal, key);
    return key > 0;
}

static int emu_keyboard_read(struct KeyboardDevice *dev, int v_port)
{
    int result = 0;
    for (int i = 7; i >= 0; i--)
    {
        result <<= 1;
        result |= dev->key_status[port_bit_map[v_port][i]];
    }
    if (result && result !=8)
        printf("in %d -> %d\n", v_port, result);
    return result;
}

static int (*port_read_array[])(void *g, int p) = {emu_keyboard_read, emu_keyboard_read, emu_keyboard_read};

static void (*port_write_array[])(void *g, int p, int v) = {NULL, NULL, NULL};

struct KeyboardDevice *emu_keyboard_init()
{
    struct KeyboardDevice *dev = malloc(sizeof(struct KeyboardDevice));
    dev->portCount = 3;
    dev->read = port_read_array;
    dev->write = port_write_array;
    for (int i; i < KEY_MAX_ID + 1; i++)
    {
        dev->key_status[i] = 0;
    }
    dev->key_status[KEY_VIRTUAL_ON] = 1;
    return dev;
}

void emu_keyboard_done(struct KeyboardDevice *dev)
{
    free(dev);
}

static int get_key_action(int keyVal)
{
    switch (keyVal)
    {
    case GDK_KEY_Z:
    case GDK_KEY_z:
        return KEY_P1_LEFT;
    case GDK_KEY_X:
    case GDK_KEY_x:
        return KEY_P1_SHOT;
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
    case GDK_KEY_space:
        return KEY_SHOT;
    case GDK_KEY_Left:
        return KEY_LEFT;
    case GDK_KEY_Right:
        return KEY_RIGHT;

    default:
        return 0;
    }
}
