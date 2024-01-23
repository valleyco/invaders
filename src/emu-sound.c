#include <stdbool.h>
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

typedef enum
{
    BIT_UFO_LOW_PITCH = 1,
    BIT_SHOOT = 2,
    BIT_INVADER_KILLED = 8,
    BIT_UFO_HIGH_PITCH,
} SoundBits_0;

typedef enum
{
    BIT_FAST_INVADER_1 = 1,
    BIT_FAST_INVADER_2 = 2,
    BIT_FAST_INVADER_3 = 4,
    BIT_FAST_INVADER_4 = 8,
    BIT_EXPLOSION = 16
} SoundBits_2;

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
    bool is_playing;  // position to the audio buffer to be played

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
            audio16_buffer[i][s] = ((Uint8)audio_buffer[s]) * 64;
        }
        callback_data[i].audio_pos = audio_buffer_len[i];
        SDL_FreeWAV(audio_buffer);
    }
}

static void audio_callback(CallbackData *userdata, Uint8 *stream, int len);

static SDL_AudioSpec dev_wav_spec;

static void port_write_0(PortDevice *g, int v)
{
    g->data = NULL;
    callback_data[UFO_LOW_PITCH].is_playing = v & BIT_UFO_LOW_PITCH;
    callback_data[SHOOT].is_playing = v & BIT_SHOOT;
    callback_data[INVADER_KILLED].is_playing = v & BIT_INVADER_KILLED;

    if (!callback_data[UFO_LOW_PITCH].is_playing)
    {
        callback_data[UFO_LOW_PITCH].audio_pos = 0;
    }
    if (!callback_data[SHOOT].is_playing)
    {
        callback_data[SHOOT].audio_pos = 0;
    }
    if (!callback_data[INVADER_KILLED].is_playing)
    {
        callback_data[INVADER_KILLED].audio_pos = 0;
    }
}
static void port_write_1(PortDevice *g, int v)
{
    g->data = NULL;
    callback_data[FAST_INVADER_1].is_playing = v & BIT_FAST_INVADER_1;
    callback_data[FAST_INVADER_2].is_playing = v & BIT_FAST_INVADER_2;
    callback_data[FAST_INVADER_3].is_playing = v & BIT_FAST_INVADER_3;
    callback_data[FAST_INVADER_4].is_playing = v & BIT_FAST_INVADER_4;
    callback_data[EXPLOSION].is_playing = v & BIT_EXPLOSION;
    if (!callback_data[FAST_INVADER_1].is_playing)
    {
        callback_data[FAST_INVADER_1].audio_pos = 0;
    }
    if (!callback_data[FAST_INVADER_2].is_playing)
    {
        callback_data[FAST_INVADER_2].audio_pos = 0;
    }
    if (!callback_data[FAST_INVADER_3].is_playing)
    {
        callback_data[FAST_INVADER_3].audio_pos = 0;
    }
    if (!callback_data[FAST_INVADER_4].is_playing)
    {
        callback_data[FAST_INVADER_4].audio_pos = 0;
    }
    if (!callback_data[EXPLOSION].is_playing)
    {
        callback_data[EXPLOSION].audio_pos = 0;
    }
}

static void (*port_write_array[])(PortDevice *g, int v) = {port_write_0, port_write_1};

PortDevice *emu_sound_init()
{
    if (!init_count++)
    {
        if (SDL_Init(SDL_INIT_AUDIO) == 0)
        {
            SDL_memset(callback_data, 0, sizeof(callback_data));
            load_wave_files();
            dev_wav_spec.channels = 1;
            dev_wav_spec.freq = 11025;
            dev_wav_spec.silence = 128;
            dev_wav_spec.samples = 512;
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
    PortDevice *device = (PortDevice *)malloc(sizeof(PortDevice));
    memset(device, 0, sizeof(PortDevice));
    device->dispose = emu_sound_done;
    device->readPortCount = 0;
    device->writePortCount = 2;
    device->write = (PortWrite *)port_write_array;
    // device->read =
    return device;
}

void emu_sound_done(PortDevice *device)
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
    free(device);
}

static void audio_callback(CallbackData *userdata, Uint8 *stream, int len)
{
    // printf("---------\n");
    SDL_memset(stream, 0, len);
    for (int i = 0; i < SOUND_COUNT; i++)
    {
        const int total_len = audio_buffer_len[i];
        const Uint32 audio_pos = userdata[i].audio_pos;
        const int bytes_left = (total_len - audio_pos) * (int)sizeof(Sint16);
        if (!userdata[i].is_playing || bytes_left == 0)
        {
            continue;
        }
        int byte_count = len > bytes_left ? bytes_left : len;
        // printf("\tb: sound: %d,total_len:%d, audio_pos: %d, byte_count: %d\n", i, total_len, userdata[i].audio_pos, byte_count);
        SDL_MixAudioFormat(stream, (Uint8 *)(audio16_buffer[i] + audio_pos), dev_wav_spec.format, byte_count, SDL_MIX_MAXVOLUME); // mix from one buffer into another
        userdata[i].audio_pos += byte_count / sizeof(Sint16);
        // printf("\ta: sound: %d,total_len:%d, audio_pos: %d, byte_count: %d\n", i,total_len, userdata[i].audio_pos, byte_count);
    }
}