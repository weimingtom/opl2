#include "opl2_int.h"
#include "opl2_vm.h"

# include <pspuser.h>
PSP_MODULE_INFO("opl2psp",0,1,0);
PSP_MAIN_THREAD_ATTR(PSP_THREAD_ATTR_VFPU);
PSP_HEAP_SIZE_MAX();

/******************************************************************************/

void pl2MultMatrix4f(fmatrix4_t *out, const fmatrix4_t *a, const fmatrix4_t *b)
{
    asm volatile(
        "lv.q   c100,  0+%1\n"
        "lv.q   c110, 16+%1\n"
        "lv.q   c120, 32+%1\n"
        "lv.q   c130, 48+%1\n"
        "lv.q   c200,  0+%2\n"
        "lv.q   c210, 16+%2\n"
        "lv.q   c220, 32+%2\n"
        "lv.q   c230, 48+%2\n"
        "vmmul.q m000, m100, m200\n"
        "sv.q   c000,  0+%0\n"
        "sv.q   c010,  0+%0\n"
        "sv.q   c020,  0+%0\n"
        "sv.q   c030,  0+%0\n"
        :"=m"(*out)
        :"m"(*a), "m"(*b)
    );
}

void pl2VectorTransform4f(fvector4_t *out, const fmatrix4_t *m, const fvector4_t *v)
{
    asm volatile(
        "lv.q   c100,  0+%1\n"
        "lv.q   c110, 16+%1\n"
        "lv.q   c120, 32+%1\n"
        "lv.q   c130, 48+%1\n"
        "lv.q   c010,  0+%2\n"
        "vtfm4.q c000, m100, c010\n"
        "sv.q   c000,  0+%0\n"
        :"=m"(*out)
        :"m"(*m), "m"(*v)
    );
}

void pl2TransposeMatrix4f(fmatrix4_t *out, const fmatrix4_t *m)
{
    asm volatile(
        "lv.q  c000,  0+%1\n"
        "lv.q  c010, 16+%1\n"
        "lv.q  c020, 32+%1\n"
        "lv.q  c030, 48+%1\n"
        "sv.q  r000,  0+%0\n"
        "sv.q  r001, 16+%0\n"
        "sv.q  r002, 32+%0\n"
        "sv.q  r003, 48+%0\n"
        :"=m"(*out) :"m"(*m)
    );
}

void pl2VectorAdd4f(fvector4_t *out, const fvector4_t *a, const fvector4_t *b)
{
    asm volatile(
        "lv.q  c010, 0+%1\n"
        "lv.q  c020, 0+%2\n"
        "vadd.q c000, c010, c020\n"
        "sv.q  c000, 0+%0\n"
        :"=m"(*out)
        :"m"(*a), "m"(*b)
    );
}

void pl2VectorSub4f(fvector4_t *out, const fvector4_t *a, const fvector4_t *b)
{
    asm volatile(
        "lv.q  c010, 0+%1\n"
        "lv.q  c020, 0+%2\n"
        "vsub.q c000, c010, c020\n"
        "sv.q  c000, 0+%0\n"
        :"=m"(*out)
        :"m"(*a), "m"(*b)
    );
}

float pl2VectorDot4f(const fvector4_t *a, const fvector4_t *b)
{
    float r;
    asm volatile(
        "lv.q   c010, 0+%1\n"
        "lv.q   c020, 0+%2\n"
        "vdot.q s000, c010, c020\n"
        "mfv    %0, s000\n"
        :"=r"(r) :"m"(*a), "m"(*b)
    );
    return r;
}

void pl2VectorScale4f(fvector4_t *out, const fvector4_t *v, float s)
{
    asm volatile(
        "mtv    %2, s020\n"
        "lv.q  c010, 0+%1\n"
        "vscl.q c000, c010, s020\n"
        "sv.q  c000, 0+%0\n"
        :"=m"(*out)
        :"m"(*v), "r"(s)
    );
}

void pl2VectorScaleAdd4f(fvector4_t *out, const fvector4_t *v, float s)
{
    asm volatile(
        "mtv    %2, s020\n"
        "lv.q  c000, 0+%0\n"
        "lv.q  c010, 0+%1\n"
        "vscl.q c010, c010, s020\n"
        "vadd.q c000, c000, c010\n"
        "sv.q  c000, 0+%0\n"
        :"+m"(*out)
        :"m"(*v), "r"(s)
    );
}

void pl2VectorTransScaleAdd4f(fvector4_t *out, const fmatrix4_t *m, const fvector4_t *v, float s)
{
    asm volatile(
        "lv.q   c000, 0+%0\n"
        "lv.q   c100,  0+%1\n"
        "lv.q   c110, 16+%1\n"
        "lv.q   c120, 32+%1\n"
        "lv.q   c130, 48+%1\n"
        "lv.q   c020, 0+%2\n"
        "mtv    %3, s030\n"
        "vtfm4.q c010, m100, c020\n"
        "vscl.q c010, c010, s030\n"
        "vadd.q c000, c000, c010\n"
        "sv.q  c000, 0+%0\n"
        :"+m"(*out)
        :"m"(*m), "m"(*v), "r"(s)
    );
}

void pl2VectorAdd3f(fvector3_t *out, const fvector3_t *a, const fvector3_t *b)
{
    asm volatile(
        "ulv.q  c010, %1\n"
        "ulv.q  c020, %2\n"
        "vadd.t c000, c010, c020\n"
        "sv.s   s000, 0+%0\n"
        "sv.s   s001, 4+%0\n"
        "sv.s   s002, 8+%0\n"
        :"=m"(*out)
        :"m"(*a), "m"(*b)
    );
}

void pl2VectorSub3f(fvector3_t *out, const fvector3_t *a, const fvector3_t *b)
{
    asm volatile(
        "ulv.q  c010, %1\n"
        "ulv.q  c020, %2\n"
        "vsub.t c000, c010, c020\n"
        "sv.s   s000, 0+%0\n"
        "sv.s   s001, 4+%0\n"
        "sv.s   s002, 8+%0\n"
        :"=m"(*out)
        :"m"(*a), "m"(*b)
    );
}

float pl2VectorDot3f(const fvector3_t *a, const fvector3_t *b)
{
    float r;
    asm volatile(
        "ulv.q  c010, %1\n"
        "ulv.q  c020, %2\n"
        "vdot.t s000, c010, c020\n"
        "mfv %0, s000\n"
        :"=r"(r)
        :"m"(*a), "m"(*b)
    );
    return r;
}

void pl2VectorCross3f(fvector3_t *out, const fvector3_t *a, const fvector3_t *b)
{
    asm volatile(
        "ulv.q  c010, %1\n"
        "ulv.q  c020, %2\n"
        "vcrsp.t c000, c010, c020\n"
        "sv.s   s000, 0+%0\n"
        "sv.s   s001, 4+%0\n"
        "sv.s   s002, 8+%0\n"
        :"=m"(*out)
        :"m"(*a), "m"(*b)
    );
}

void pl2VectorScale3f(fvector3_t *out, const fvector3_t *v, float s)
{
    asm volatile(
        "mtv    %2, s020\n"
        "ulv.q  c010, 0+%1\n"
        "vscl.t c000, c010, s020\n"
        "sv.s   s000, 0+%0\n"
        "sv.s   s001, 4+%0\n"
        "sv.s   s002, 8+%0\n"
        :"=m"(*out)
        :"m"(*v), "r"(s)
    );
}

void pl2VectorScaleAdd3f(fvector3_t *out, const fvector3_t *v, float s)
{
    asm volatile(
        "mtv    %2, s020\n"
        "ulv.q  c000, 0+%0\n"
        "ulv.q  c010, 0+%1\n"
        "vscl.t c010, c010, s020\n"
        "vadd.t c000, c000, c010\n"
        "sv.s   s000, 0+%0\n"
        "sv.s   s001, 4+%0\n"
        "sv.s   s002, 8+%0\n"
        :"+m"(*out)
        :"m"(*v), "r"(s)
    );
}

float pl2VectorLength3f(const fvector3_t *v)
{
    float r;
    asm volatile(
        "ulv.q  c010, %1\n"
        "vdot.t s000, c010, c010\n"
        "vsqrt.s s000, s000\n"
        "mfv %0, s000\n"
        :"=r"(r)
        :"m"(*v)
    );
    return r;
}

void pl2VectorNormalize3f(fvector3_t *out, const fvector3_t *v)
{
    asm volatile(
        "ulv.q  c010, %1\n"
        "vdot.t s003, c010, c010\n"
        "vrsq.s s003, s003\n"
        "vscl.t c000, c010, s003\n"
        "sv.s   s000, 0+%0\n"
        "sv.s   s001, 4+%0\n"
        "sv.s   s002, 8+%0\n"
        :"=m"(*out)
        :"m"(*v)
    );
}

void pl2QuatMultiply(fvector4_t *out, const fvector4_t *a, const fvector4_t *b)
{
    asm volatile(
        "ulv.q  c010, 0+%1\n"
        "ulv.q  c020, 0+%2\n"
        "vqmul.q c000, c010, c020\n"
        "usv.q  c000, 0+%0\n"
        :"=m"(*out)
        :"m"(*a), "m"(*b)
    );
}

void pl2QuatRotate(fvector3_t *out, const fvector3_t *v, const fvector3_t *axis, float angle)
{
    float s = sinf(0.5f * angle) / pl2VectorLength3f(axis);
    float c = cosf(0.5f * angle);

    fvector4_t q = { s * axis->x, s * axis->y, s * axis->z, c };
    fvector4_t t = { v->x, v->y, v->z, 0.0f };
    fvector4_t r;

    pl2QuatMultiply(&r, &q, &t);
    q.x = -q.x; q.y = -q.y; q.z = -q.z;
    pl2QuatMultiply(&t, &r, &q);

    out->x = t.x; out->y = t.y; out->z = t.z;
}

void pl2VectorOrbit(fvector3_t *planet, const fvector3_t *sun, const fvector3_t *up, const fvector3_t *rotate)
{
    asm volatile (
        //"lv.s       S000, 0 + %0\n"   // planet
        //"lv.s       S001, 4 + %0\n"
        //"lv.s       S002, 8 + %0\n"
        "ulv.q      C000, 0 + %0\n"
        //"lv.s       S010, 0 + %1\n"   // sun
        //"lv.s       S011, 4 + %1\n"
        //"lv.s       S012, 8 + %1\n"
        "ulv.q      C010, 0 + %1\n"
        //"lv.s       S020, 0 + %2\n"   // up
        //"lv.s       S021, 4 + %2\n"
        //"lv.s       S022, 8 + %2\n"
        "ulv.q      C020, 0 + %2\n"

        // get fwd and right vectors
        "vsub.q     C030, C000[x,y,z,0], C010[x,y,z,0]\n" // = planet - sun = fwd
        "vcrsp.t    R003, C020, C030\n" // = up x fwd = right

        // get half-angles
        "ulv.q      C100, 0 + %3\n"
        "vcst.s     S103, VFPU_1_PI\n"
        "vscl.t     C100, C100, S103\n"

        // calculate sin/cos of half-angles
        "vrot.p     R120, S100, [s, c]\n"
        "vrot.p     R121, S101, [s, c]\n"
        "vrot.p     R122, S102, [s, c]\n"

        // build quaternions
        "vdot.t     S103, R003, R003\n" // x
        "vdot.t     S113, C020, C020\n" // y
        "vdot.t     S123, C030, C030\n" // z
        "vrsq.t     R103, R103\n"
        //"vrsq.s     S103, S103\n" // x
        //"vrsq.s     S113, S113\n" // y
        //"vrsq.s     S123, S123\n" // z
        "vmul.t     R103, R103, C120\n" // sin * rsq
        "vscl.t     R100, R003, S103\n"
        "vscl.t     R101, C020, S113\n"
        "vscl.t     R102, C030, S123\n"

        // apply quaternions
        "vqmul.q    C200, R100, C030\n"
        //"vqmul.q    C220, C100, C020\n" // *
        "vneg.t     R100, R100\n"
        "vqmul.q    C210, C200, R100\n"
        "vmov.t     C030, C210\n"
        //"vqmul.q    C230, C220, R100\n" // *
        "vqmul.q    C200, R101, C210\n"
        "vneg.t     R101, R101\n"
        "vqmul.q    C210, C200, R101\n"
        "vqmul.q    C200, R102, C210\n"
        "vneg.t     R102, R102\n"
        "vqmul.q    C210, C200, R102\n"

        "vadd.t     C200, C210, C010\n"

        // *TODO: check for crossing the y-axis
        //"vidt.q     C210\n"
        //"vdot.t     S230, C210, C210\n"
        //"vcmp.s     EN, S230\n"
        //"vcmovt.t   C200, C000, 0\n"

        "sv.s    S200, 0 + %0\n"
        "sv.s    S201, 4 + %0\n"
        "sv.s    S202, 8 + %0\n"

        : "+m"(*planet)
        : "m"(*sun),
          "m"(*up),
          "m"(*rotate)
    );
}

void pl2VectorZoom(fvector3_t *obj, const fvector3_t *targ, float distance)
{
    asm volatile (
        "ulv.q   C000, 0 + %0\n"
        "ulv.q   C010, 0 + %1\n"
        "mtv %2, S033\n"
        "vsub.t  C020, C010, C000\n"
        "vdot.t  S032, C020, C020\n"
        "vrsq.s  S032, S032\n"
        "vmul.s  S033, S032, S033\n"
        "vscl.t  C010, C020, S033\n"
        "vadd.t  C000, C000, C010\n"
        "sv.s    S000, 0 + %0\n"
        "sv.s    S001, 4 + %0\n"
        "sv.s    S002, 8 + %0\n"
        : "+m"(*obj)
        : "m"(*targ), "r"(distance)
    );
}


#define MEMORYSIZE (64<<20)
#define MEMBLOCKBITS (20)
#define MINBLOCKBITS (4)
#define MEMBLOCKSIZE (1<<(MEMBLOCKBITS))
#define MINBLOCKSIZE (1<<(MINBLOCKBITS))
#define MEMBLOCKCOUNT ((MEMORYSIZE/MEMBLOCKSIZE)+MEMBLOCKBITS-MINBLOCKBITS+1)

uint32_t pl2GetFreeRam()
{
    void *temp[MEMBLOCKCOUNT];

    uint32_t freeSize = 0;
    int blockSize = MEMBLOCKSIZE;

    int i = 0;
    while((i < MEMBLOCKCOUNT) && (blockSize >= MINBLOCKSIZE))
    {
        if(NULL == (temp[i] = malloc(blockSize)))
        {
            blockSize >>= 1;
        }
        else
        {
            freeSize += blockSize;
            i++;
        }
    }

    while(i > 0)
        free(temp[--i]);

    return freeSize;
}

/******************************************************************************/

int usleep(useconds_t usec)
{
    return sceKernelDelayThread(usec);
}

/******************************************************************************/

int main(int argc, char *argv[])
{
    atexit(sceKernelExitGame);

    if(!pl2GameInit(&argc, argv))
        return 1;

    pl2GameRun();
    return 0;
}
