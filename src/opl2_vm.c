#include "opl2_int.h"
#include "opl2_vm.h"

/******************************************************************************/

void pl2CameraRotate1P(pl2Camera *cam, float xr, float yr)
{
    fvector3_t r = { yr, -xr, 0 };
    pl2VectorOrbit(&cam->focus, &cam->eye, &cam->up, &r);
}

void pl2CameraRotate3P(pl2Camera *cam, float xr, float yr)
{
    fvector3_t r = { yr, xr, 0 };
    pl2VectorOrbit(&cam->eye, &cam->focus, &cam->up, &r);
}

void pl2CameraZoom(pl2Camera *cam, float distance)
{
    pl2VectorZoom(&cam->eye, &cam->focus, distance);
}
