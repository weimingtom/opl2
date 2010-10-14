#ifndef __OPL2_AL_H__
#define __OPL2_AL_H__

#include <AL/al.h>
#include <AL/alut.h>

int pl2AlInit(int *argc, char *argv[]);
void pl2AlSetListenerPosition(float x, float y, float z);
void pl2AlSetSourcePosition(int channel, float x, float y, float z);
void pl2AlPlaySound(int channel, pl2Sound *sound);

#endif // __OPL2_AL_H__
