#include "opl2.h"
#include "opl2_int.h"

#include <AL/al.h>
#include <AL/alut.h>

void pl2AlExit()
{
    alutExit();
}

int pl2AlInit(int *argc, char *argv[])
{
    alutInit(argc, argv);
    atexit(pl2AlExit);

    return 1;
}
