#include <SDL2/SDL.h>
#include "emu-sound.h"
typedef enum
{
    SHOOT,
    EXPLOSION,
    INVADER_KILLED,
    FAST_INVADER_1,
    FAST_INVADER_2,
    FAST_INVADER_3,
    FAST_INVADER_4,
    UFO_HIGH_PITCH,
    UFO_LOW_PITCH,
} Sounds;

static const char *wave_file_name[] = {
    "../wav/shoot.wav",
    "../wav/explosion.wav",
    "../wav/invaderkilled.wav",
    "../wav/fastinvader1.wav",
    "../wav/fastinvader2.wav",
    "../wav/fastinvader3.wav",
    "../wav/fastinvader4.wav",
    "../wav/ufo_highpitch.wav",
    "../wav/ufo_lowpitch.wav",
};

#define SOUND_COUNT ((int)sizeof(wave_file_name) / (int)sizeof(wave_file_name[0]))

static Sint16 *audio16_buffer[SOUND_COUNT];

static Uint32 audio_buffer_len[SOUND_COUNT];

static int init_count = 0;

typedef struct
{
    int wave_id;
    Uint32 audio_pos; // position to the audio buffer to be played

} CallbackData;

CallbackData callback_data[SOUND_COUNT];

static void load_wave_files()
{
    Uint8 *audio_buffer;
    static SDL_AudioSpec wav_spec;

    for (int i = 0; i < SOUND_COUNT; i++)
    {
        if (SDL_LoadWAV(wave_file_name[i], &wav_spec, &audio_buffer, audio_buffer_len + i) == NULL)
        {
            continue;
        }
        audio16_buffer[i] = malloc(sizeof(Sint16) * audio_buffer_len[i]);
        for (Uint32 s = 0; s < audio_buffer_len[i]; s++)
        {
            audio16_buffer[i][s] = ((Sint8)audio_buffer[s]) * 64;
        }
        SDL_FreeWAV(audio_buffer);
    }
}

static void audio_callback(CallbackData *userdata, Uint8 *stream, int len);

static SDL_AudioSpec dev_wav_spec;

static void port_write(SoundDevice *g, int p, int v)
{
    g->portCount = g->portCount;
    switch (p)
    {
    case 0:
        if (v & 1)
        {
            callback_data[UFO_LOW_PITCH].audio_pos = 0;
        }
        if (v & 2)
        {
            callback_data[SHOOT].audio_pos = 0;
        }
        if (v & 8)
        {
            callback_data[INVADER_KILLED].audio_pos = 0;
        }
        break;
    case 2:
        if (v & 1)
        {
            callback_data[FAST_INVADER_1].audio_pos = 0;
        }
        if (v & 2)
        {
            callback_data[FAST_INVADER_2].audio_pos = 0;
        }
        if (v & 4)
        {
            callback_data[FAST_INVADER_3].audio_pos = 0;
        }
        if (v & 8)
        {
            callback_data[FAST_INVADER_4].audio_pos = 0;
        }
        break;
    }
}


static int (*port_read_array[])(SoundDevice *g, int p) = {NULL, NULL, NULL};

static void (*port_write_array[])(SoundDevice *g, int p, int v) = {port_write, NULL, port_write};

SoundDevice *emu_sound_init()
{
    if (!init_count++)
    {
        if (SDL_Init(SDL_INIT_AUDIO) == 0)
        {
            load_wave_files();
            SDL_memset(callback_data, 0, sizeof(callback_data));
            dev_wav_spec.channels = 1;
            dev_wav_spec.freq = 11025;
            dev_wav_spec.silence = 128;
            dev_wav_spec.samples = 2048;
            dev_wav_spec.padding = 0;
            dev_wav_spec.size = 0;
            dev_wav_spec.format = 0x8010;
            dev_wav_spec.callback = (SDL_AudioCallback)audio_callback;
            dev_wav_spec.userdata = callback_data;

            /* Open the audio device */
            if (SDL_OpenAudio(&dev_wav_spec, NULL) == 0)
            {
                SDL_PauseAudio(0);
            }

            /* Start playing */
        }
    }
    SoundDevice *device = (SoundDevice *)malloc(sizeof(SoundDevice));
    device->portCount = 3;
    device->read = (PORT_READ *)port_read_array;
    device->write = (PORT_WRITE *)port_write_array;
    // device->read =
    return device;
}

void emu_sound_done(SoundDevice *dev)
{
    if (!--init_count)
    {
        SDL_CloseAudio();
        for (int i = 0; i < SOUND_COUNT; i++)
        {
            free(audio16_buffer[i]);
        }
        SDL_Quit();
    }
    free(dev);
}

static void audio_callback(CallbackData *userdata, Uint8 *stream, int len)
{
    SDL_memset(stream, 0, len);
    for (int i = 0; i < SOUND_COUNT; i++)
    {
        const int total_len = audio_buffer_len[i];
        const Uint32 audio_pos = userdata[i].audio_pos;
        const int left = total_len - audio_pos;
        if (left == 0)
        {
            continue;
        }
        int count = len > left ? left : len;
        // printf("sound: %d,len:%d, left: %d, count: %d\n", i,len, left, count);
        SDL_MixAudio(stream, (Uint8 *)(audio16_buffer[i] + audio_pos), count, SDL_MIX_MAXVOLUME / 4); // mix from one buffer into another
        userdata[i].audio_pos += count;
    }
}