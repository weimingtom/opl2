#include "opl2.h"
#include "opl2_int.h"

#include <AL/al.h>
#include <AL/alut.h>

int pl2AlInit(int *argc, char *argv[])
{
    alutInit(argc, argv);
    atexit(alutExit);

    return 1;
}
