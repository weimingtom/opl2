//#include "opl2.h"
#include "opl2_int.h"

#include <SDL/SDL_thread.h>

#include <AL/al.h>
#include <AL/alc.h>
//#include <AL/alut.h>

#if WITH_TREMOR
# include <tremor/ivorbiscodec.h> //libtremor
# include <tremor/ivorbisfile.h>  //libtremor
#else
# include <vorbis/codec.h>
# include <vorbis/vorbisfile.h>
#endif

// Reference:
// http://www.gamedev.net/reference/articles/article2008.asp

ALCdevice *pl2_alc_device = 0;
ALCcontext *pl2_alc_context = 0;

static ALuint pl2_al_buffers[PL2_NUM_CHANNELS] = { 0 };
static ALuint pl2_al_sources[PL2_NUM_CHANNELS] = { 0 };
extern pl2Sound *pl2_sounds[PL2_NUM_CHANNELS];

static SDL_Thread *pl2_play_thread = NULL;

static int pl2AlPlayThread(void *ud)
{
    while(pl2_running)
    {
        int i, j; Uint32 n;

        for(i = 0; i < PL2_NUM_CHANNELS; i++)
        {
            alGetError(); // clear errors

            ALint queued, done;
            alGetSourcei(pl2_al_sources[i], AL_BUFFERS_QUEUED, &queued);
            alGetSourcei(pl2_al_sources[i], AL_BUFFERS_PROCESSED, &done);

            if(done || !queued)
            {
                ALuint temp[done];
                alSourceUnqueueBuffers(pl2_al_sources[i], done, temp);

                pl2Sound *sound = pl2_sounds[i];

                if(sound)
                {
                    int bytesRead = pl2SoundDecode(sound, sound->buffer, sizeof(sound->buffer));

                    alBufferData(pl2_al_buffers[i], (sound->channels == 2) ?
                                 AL_FORMAT_STEREO16 : AL_FORMAT_MONO16,
                                 sound->buffer, bytesRead, sound->sampleRate);
                    alSourceQueueBuffers(pl2_al_sources[i], 1, &(pl2_al_buffers[i]));

                    DEBUGPRINT("%s: queueing another buffer for channel %d\n", __func__, i);

                    j++;
                }
            }

        }

        int r = alGetError();
        if(r) DEBUGPRINT("%s: alGetError() returned %d\n", __func__, r);
        usleep(1000);
    }

    return 0;
}

void pl2AlExit()
{
    SDL_WaitThread(pl2_play_thread, NULL);
    //alutExit();

    alDeleteBuffers(2*PL2_NUM_CHANNELS, pl2_al_buffers);
    alDeleteSources(PL2_NUM_CHANNELS, pl2_al_sources);

    alcMakeContextCurrent(NULL);

    alcDestroyContext(pl2_alc_context);
    pl2_alc_context = NULL;

    if(!alcCloseDevice(pl2_alc_device))
    {
        DEBUGPRINT("%s: error closing AL device\n", __func__);
    }
    pl2_alc_device = NULL;
}

void pl2SoundPlay(pl2Sound *sound, int channel)
{
    DEBUGPRINT("%s: playing pl2Sound<%p> on channel %d\n",
               __func__, sound, channel);
    if((unsigned)channel < PL2_NUM_CHANNELS)
    {
        alSourceStop(pl2_al_sources[channel]);

        if(pl2_sounds[channel]) pl2SoundFree(pl2_sounds[channel]);

        pl2_sounds[channel] = sound;

        alSourcePlay(pl2_al_sources[channel]);
    }
}

void pl2AlSetSourcePosition(int channel, const fvector3_t *v)
{
    if((channel >= 0) && (channel < PL2_NUM_CHANNELS))
    {
        alSourcefv(pl2_al_sources[channel], AL_POSITION, (ALfloat*)v);
    }
}

void pl2AlSetListenerPosition(const fvector3_t *v)
{
    alListenerfv(AL_POSITION, (ALfloat*)v);
}

int pl2AlInit(int *argc, char *argv[])
{
    //alutInit(argc, argv);

    atexit(pl2AlExit);

    pl2_alc_device = alcOpenDevice(NULL);

    if(!pl2_alc_device)
    {
        DEBUGPRINT("%s: error opening AL device\n", __func__);
        return 0;
    }

    DEBUGPRINT("%s: opened AL device\n", __func__);

    ALCint attrs[] =
    {
        ALC_FREQUENCY, 44100,
        ALC_MONO_SOURCES, 2,
        ALC_STEREO_SOURCES, 2,
        0, 0
    };

    pl2_alc_context = alcCreateContext(pl2_alc_device, attrs);

    if(!pl2_alc_context)
    {
        DEBUGPRINT("%s: error creating context\n", __func__);
        return 0;
    }

    DEBUGPRINT("%s: created context\n", __func__);

    if(!alcMakeContextCurrent(pl2_alc_context))
    {
        DEBUGPRINT("%s: error making context current\n", __func__);
        return 0;
    }

    DEBUGPRINT("%s: made context current\n", __func__);

    alGenBuffers(PL2_NUM_CHANNELS, pl2_al_buffers);
    alGenSources(PL2_NUM_CHANNELS, pl2_al_sources);

    DEBUGPRINT("%s: created buffers and sources\n", __func__);

    pl2_play_thread = SDL_CreateThread(pl2AlPlayThread, NULL);

    if(!pl2_play_thread)
    {
        DEBUGPRINT("%s: error starting playback thread\n", __func__);
        return 0;
    }

    DEBUGPRINT("%s: started playback thread\n", __func__);
    return 1;
}

