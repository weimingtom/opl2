//#include "opl2.h"
#include "opl2_int.h"

#include <AL/al.h>
//#include <AL/alc.h>
#include <AL/alut.h>

#if WITH_TREMOR
# include <tremor/ivorbiscodec.h> //libtremor
# include <tremor/ivorbisfile.h>  //libtremor
#else
# include <vorbis/codec.h>
# include <vorbis/vorbisfile.h>
#endif

//#include <unistd.h>
//#include <pthread.h>

// Reference:
// http://www.gamedev.net/reference/articles/article2008.asp

static ALuint pl2_al_buffers[PL2_NUM_CHANNELS] = { 0 };
static ALuint pl2_al_sources[PL2_NUM_CHANNELS] = { 0 };
static pl2Sound *pl2_sounds[PL2_NUM_CHANNELS] = { NULL };

static int pl2AlPlayThread()
{
    return 0;
}

void pl2AlExit()
{
    alDeleteBuffers(PL2_NUM_CHANNELS, pl2_al_buffers);
    alDeleteSources(PL2_NUM_CHANNELS, pl2_al_sources);
    alutExit();
}


void pl2SoundPlay(pl2Sound *sound, int channel)
{
    if(sound && (channel >= 0) && (channel < PL2_NUM_CHANNELS))
    {
        if(pl2_sounds[channel]) pl2SoundFree(pl2_sounds[channel]);
        pl2_sounds[channel] = sound;
    }
}

void pl2AlSetSourcePosition(int channel, float x, float y, float z)
{
    if((channel >= 0) && (channel < PL2_NUM_CHANNELS))
    {
        alSource3f(pl2_al_sources[channel], AL_POSITION, x, y, z);
    }
}

void pl2AlSetListenerPosition(float x, float y, float z)
{
    alListener3f(AL_POSITION, x, y, z);
}

int pl2AlInit(int *argc, char *argv[])
{
    alutInit(argc, argv);
    atexit(pl2AlExit);

    alGenBuffers(PL2_NUM_CHANNELS, pl2_al_buffers);
    alGenSources(PL2_NUM_CHANNELS, pl2_al_sources);

    return 1;
}

