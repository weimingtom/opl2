//#include "opl2.h"
#include "opl2_int.h"
//#include "opl2_vm.h"

#include <math.h>

/******************************************************************************/

typedef void (*PL2MULTMATRIX4F)(fmatrix4_t *out, const fmatrix4_t *a, const fmatrix4_t *b);
typedef void (*PL2VECTORTRANSFORM4F)(fvector4_t *out, const fmatrix4_t *m, const fvector4_t *v);
typedef void (*PL2TRANSPOSEMATRIX4F)(fmatrix4_t *out, const fmatrix4_t *m);
typedef void (*PL2VECTORADD4F)(fvector4_t *out, const fvector4_t *a, const fvector4_t *b);
typedef void (*PL2VECTORSUB4F)(fvector4_t *out, const fvector4_t *a, const fvector4_t *b);
typedef float (*PL2VECTORDOT4F)(const fvector4_t *a, const fvector4_t *b);
typedef void (*PL2VECTORSCALE4F)(fvector4_t *out, const fvector4_t *v, float s);
typedef void (*PL2VECTORSCALEADD4F)(fvector4_t *out, const fvector4_t *v, float s);
typedef void (*PL2VECTORADD3F)(fvector3_t *out, const fvector3_t *a, const fvector3_t *b);
typedef void (*PL2VECTORSUB3F)(fvector3_t *out, const fvector3_t *a, const fvector3_t *b);
typedef float (*PL2VECTORDOT3F)(const fvector3_t *a, const fvector3_t *b);
typedef void (*PL2VECTORCROSS3F)(fvector3_t *out, const fvector3_t *a, const fvector3_t *b);
typedef void (*PL2VECTORSCALE3F)(fvector3_t *out, const fvector3_t *v, float s);
typedef void (*PL2VECTORSCALEADD3F)(fvector3_t *out, const fvector3_t *v, float s);
typedef float (*PL2VECTORLENGTH3F)(const fvector3_t *v);
typedef void (*PL2VECTORNORMALIZE3F)(fvector3_t *out, const fvector3_t *v);
typedef void (*PL2QUATMULTIPLY)(fvector4_t *out, const fvector4_t *a, const fvector4_t *b);
typedef void (*PL2QUATROTATE)(fvector3_t *out, const fvector3_t *v, const fvector3_t *axis, float angle);
typedef void (*PL2VECTORORBIT)(fvector3_t *planet, const fvector3_t *sun, const fvector3_t *up, const fvector3_t *rotate);
typedef void (*PL2VECTORZOOM)(fvector3_t *obj, const fvector3_t *targ, float distance);

/******************************************************************************/

#if _PSP_FW_VERSION

#else

void pl2MultMatrix4f_SSE(fmatrix4_t *out, const fmatrix4_t *a, const fmatrix4_t *b)
{
/*
R = [[ Axx*Bxx+Ayx*Bxy+Azx*Bxz+Awx*Bxw, ... ]]
*/
    asm volatile(
        "movaps  0 %2, %%xmm4\n"
        "movaps 16 %2, %%xmm5\n"
        "movaps 32 %2, %%xmm6\n"
        "movaps 48 %2, %%xmm7\n"

        "pshufd $0x00,  0 %1, %%xmm0\n"
        "pshufd $0x55,  0 %1, %%xmm1\n"
        "pshufd $0xaa,  0 %1, %%xmm2\n"
        "pshufd $0xff,  0 %1, %%xmm3\n"        
        "mulps  %%xmm4, %%xmm0\n"
        "mulps  %%xmm5, %%xmm1\n"
        "mulps  %%xmm6, %%xmm2\n"
        "mulps  %%xmm7, %%xmm3\n"
        "addps  %%xmm1, %%xmm0\n"
        "addps  %%xmm3, %%xmm2\n"
        "addps  %%xmm2, %%xmm0\n"
        "movaps %%xmm0,  0 %0\n"

        "pshufd $0x00, 16 %1, %%xmm0\n"
        "pshufd $0x55, 16 %1, %%xmm1\n"
        "pshufd $0xaa, 16 %1, %%xmm2\n"
        "pshufd $0xff, 16 %1, %%xmm3\n"
        "mulps  %%xmm4, %%xmm0\n"
        "mulps  %%xmm5, %%xmm1\n"
        "mulps  %%xmm6, %%xmm2\n"
        "mulps  %%xmm7, %%xmm3\n"
        "addps  %%xmm1, %%xmm0\n"
        "addps  %%xmm3, %%xmm2\n"
        "addps  %%xmm2, %%xmm0\n"
        "movaps %%xmm0, 16 %0\n"

        "pshufd $0x00, 32 %1, %%xmm0\n"
        "pshufd $0x55, 32 %1, %%xmm1\n"
        "pshufd $0xaa, 32 %1, %%xmm2\n"
        "pshufd $0xff, 32 %1, %%xmm3\n"
        "mulps  %%xmm4, %%xmm0\n"
        "mulps  %%xmm5, %%xmm1\n"
        "mulps  %%xmm6, %%xmm2\n"
        "mulps  %%xmm7, %%xmm3\n"
        "addps  %%xmm1, %%xmm0\n"
        "addps  %%xmm3, %%xmm2\n"
        "addps  %%xmm2, %%xmm0\n"
        "movaps %%xmm0, 32 %0\n"

        "pshufd $0x00, 48 %1, %%xmm0\n"
        "pshufd $0x55, 48 %1, %%xmm1\n"
        "pshufd $0xaa, 48 %1, %%xmm2\n"
        "pshufd $0xff, 48 %1, %%xmm3\n"
        "mulps  %%xmm4, %%xmm0\n"
        "mulps  %%xmm5, %%xmm1\n"
        "mulps  %%xmm6, %%xmm2\n"
        "mulps  %%xmm7, %%xmm3\n"
        "addps  %%xmm1, %%xmm0\n"
        "addps  %%xmm3, %%xmm2\n"
        "addps  %%xmm2, %%xmm0\n"
        "movaps %%xmm0, 48 %0\n"

        :"=m"(*out)
        :"m"(*a), "m"(*b)
    );
}

void pl2MultMatrix4f_NoSSE(fmatrix4_t *out, const fmatrix4_t *a, const fmatrix4_t *b)
{
    fmatrix4_t c;
# define MMUL(i,j) \
    c.i.j = a->i.x * b->x.j + a->i.y * b->y.j + a->i.z * b->z.j + a->i.w * b->w.j
    MMUL(x,x); MMUL(y,x); MMUL(z,x); MMUL(w,x);
    MMUL(x,y); MMUL(y,y); MMUL(z,y); MMUL(w,y);
    MMUL(x,z); MMUL(y,z); MMUL(z,z); MMUL(w,z);
    MMUL(x,w); MMUL(y,w); MMUL(z,w); MMUL(w,w);
# undef MMUL
    *out = c;
}

PL2MULTMATRIX4F pl2MultMatrix4f = pl2MultMatrix4f_NoSSE;

#endif // _PSP_FW_VERSION

#if _PSP_FW_VERSION

#else

void pl2VectorTransform4f_SSE(fvector4_t *out, const fmatrix4_t *m, const fvector4_t *v)
{
    asm volatile(
        "movaps  0 %1, %%xmm4\n"
        "movaps 16 %1, %%xmm5\n"
        "movaps 32 %1, %%xmm6\n"
        "movaps 48 %1, %%xmm7\n"
#if 0
        "pshufd $0x00, 0 %2, %%xmm0\n"
        "pshufd $0x55, 0 %2, %%xmm1\n"
        "pshufd $0xaa, 0 %2, %%xmm2\n"
        "pshufd $0xff, 0 %2, %%xmm3\n"
#else
        "movaps 0 %2, %%xmm3\n"
        "pshufd $0x00, %%xmm3, %%xmm0\n"
        "pshufd $0x55, %%xmm3, %%xmm1\n"
        "pshufd $0xaa, %%xmm3, %%xmm2\n"
        "pshufd $0xff, %%xmm3, %%xmm3\n"
#endif
        "mulps  %%xmm4, %%xmm0\n"
        "mulps  %%xmm5, %%xmm1\n"
        "mulps  %%xmm6, %%xmm2\n"
        "mulps  %%xmm7, %%xmm3\n"
        "addps  %%xmm1, %%xmm0\n"
        "addps  %%xmm3, %%xmm2\n"
        "addps  %%xmm2, %%xmm0\n"
        "movaps %%xmm0,  0 %0\n"
        :"=m"(*out)
        :"m"(*m), "m"(*v)
    );
}

void pl2VectorTransform4f_NoSSE(fvector4_t *out, const fmatrix4_t *m, const fvector4_t *v)
{
    fvector4_t u;
# define VTFM(i) \
    u.i = m->x.i * v->x + m->y.i * v->y + m->z.i * v->z + m->w.i*v->w
    VTFM(x); VTFM(y); VTFM(z); VTFM(w);
# undef VTFM
    *out = u;
}

PL2VECTORTRANSFORM4F pl2VectorTransform4f = pl2VectorTransform4f_NoSSE;

#endif // _PSP_FW_VERSION

#if _PSP_FW_VERSION

#else

void pl2TransposeMatrix4f_NoSSE(fmatrix4_t *out, const fmatrix4_t *m)
{
# define MTRAN(i,j) \
    n.i.j = m->j.i

    fmatrix4_t n;
    MTRAN(x,x); MTRAN(y,x); MTRAN(z,x); MTRAN(w,x);
    MTRAN(x,y); MTRAN(y,y); MTRAN(z,y); MTRAN(w,y);
    MTRAN(x,z); MTRAN(y,z); MTRAN(z,z); MTRAN(w,z);
    MTRAN(x,w); MTRAN(y,w); MTRAN(z,w); MTRAN(w,w);
    *out = n;

# undef MTRAN
}

PL2TRANSPOSEMATRIX4F pl2TransposeMatrix4f = pl2TransposeMatrix4f_NoSSE;

#endif // _PSP_FW_VERSION

#if _PSP_FW_VERSION

#else

void pl2VectorAdd4f_SSE(fvector4_t *out, const fvector4_t *a, const fvector4_t *b)
{
    asm volatile(
        "movaps %1, %%xmm0\n"
        "movaps %2, %%xmm1\n"
        "addps  %%xmm1, %%xmm0\n"
        "movaps %%xmm0, %0\n"
        :"=m"(*out)
        :"m"(*a), "m"(*b)
    );
}

void pl2VectorAdd4f_NoSSE(fvector4_t *out, const fvector4_t *a, const fvector4_t *b)
{
    out->x = a->x + b->x;
    out->y = a->y + b->y;
    out->z = a->z + b->z;
    out->w = a->w + b->w;
}

PL2VECTORADD4F pl2VectorAdd4f = pl2VectorAdd4f_NoSSE;

#endif

#if _PSP_FW_VERSION

#else

void pl2VectorSub4f_SSE(fvector4_t *out, const fvector4_t *a, const fvector4_t *b)
{
    asm volatile(
        "movaps %1, %%xmm0\n"
        "movaps %2, %%xmm1\n"
        "subps  %%xmm1, %%xmm0\n"
        "movaps %%xmm0, %0\n"
        :"=m"(*out)
        :"m"(*a), "m"(*b)
    );
}

void pl2VectorSub4f_NoSSE(fvector4_t *out, const fvector4_t *a, const fvector4_t *b)
{
    out->x = a->x - b->x;
    out->y = a->y - b->y;
    out->z = a->z - b->z;
    out->w = a->w - b->w;
}

PL2VECTORSUB4F pl2VectorSub4f = pl2VectorSub4f_NoSSE;

#endif // _PSP_FW_VERSION

#if _PSP_FW_VERSION

#else

float pl2VectorDot4f_SSE(const fvector4_t *a, const fvector4_t *b)
{
    float r;
    asm volatile(
    "movaps %1, %%xmm0\n"
    "movaps %2, %%xmm1\n"
    "mulps  %%xmm1, %%xmm0\n"
    "pshufd $1, %%xmm0, %%xmm1\n"
    "pshufd $2, %%xmm0, %%xmm2\n"
    "pshufd $3, %%xmm0, %%xmm3\n"
    "addss %%xmm1, %%xmm0\n"
    "addss %%xmm2, %%xmm0\n"
    "addss %%xmm3, %%xmm0\n"
    "movss %%xmm0, %0\n"
    :"=m"(r) :"m"(*a), "m"(*b)
    );
    return r;
}

float pl2VectorDot4f_NoSSE(const fvector4_t *a, const fvector4_t *b)
{
    return a->x * b->x + a->y * b->y + a->z * b->z + a->w * b->w;
}

PL2VECTORDOT4F pl2VectorDot4f = pl2VectorDot4f_NoSSE;

#endif // _PSP_FW_VERSION

#if _PSP_FW_VERSION

#else

void pl2VectorScale4f_SSE(fvector4_t *out, const fvector4_t *v, float s)
{
    asm volatile(
        "movss  %2, %%xmm1\n"
        "movaps %1, %%xmm0\n"
        "shufps $0, %%xmm1, %%xmm1\n"
        //"pshufd $0, %2, %%xmm1\n"
        "mulps  %%xmm1, %%xmm0\n"
        "movaps %%xmm0, %0\n"
        :"=m"(*out)
        :"m"(*v), "m"(s)
    );
}

void pl2VectorScale4f_NoSSE(fvector4_t *out, const fvector4_t *v, float s)
{
    out->x = v->x * s;
    out->y = v->y * s;
    out->z = v->z * s;
    out->w = v->w * s;
}

PL2VECTORSCALE4F pl2VectorScale4f = pl2VectorScale4f_NoSSE;

#endif // _PSP_FW_VERSION

#if _PSP_FW_VERSION

#else

void pl2VectorScaleAdd4f_SSE(fvector4_t *out, const fvector4_t *v, float s)
{
    asm volatile(
        "movss  %2, %%xmm2\n"
        "movaps %1, %%xmm1\n"
        "shufps $0, %%xmm2, %%xmm2\n"
        "movaps %0, %%xmm0\n"
        //"pshufd $0, %2, %%xmm2\n"
        "mulps  %%xmm2, %%xmm1\n"
        "addps  %%xmm1, %%xmm0\n"
        "movaps %%xmm0, %0\n"
        :"=m"(*out)
        :"m"(*v), "m"(s)
    );
}

void pl2VectorScaleAdd4f_NoSSE(fvector4_t *out, const fvector4_t *v, float s)
{
    out->x += v->x * s;
    out->y += v->y * s;
    out->z += v->z * s;
    out->w += v->w * s;
}

PL2VECTORSCALEADD4F pl2VectorScaleAdd4f = pl2VectorScaleAdd4f_NoSSE;

#endif // _PSP_FW_VERSION

#if _PSP_FW_VERSION

#else

void pl2VectorAdd3f_SSE(fvector3_t *out, const fvector3_t *a, const fvector3_t *b)
{
    asm volatile(
        "movups %1, %%xmm0\n"
        "movups %2, %%xmm1\n"
        "addps  %%xmm1, %%xmm0\n"
        "movlps %%xmm0, 0 %0\n"
        "movhlps %%xmm0, %%xmm0\n"
        "movss  %%xmm0, 8 %0\n"
        :"=m"(*out)
        :"m"(*a), "m"(*b)
    );
}

void pl2VectorAdd3f_NoSSE(fvector3_t *out, const fvector3_t *a, const fvector3_t *b)
{
    out->x = a->x + b->x;
    out->y = a->y + b->y;
    out->z = a->z + b->z;
}

PL2VECTORADD3F pl2VectorAdd3f = pl2VectorAdd3f_NoSSE;

#endif // _PSP_FW_VERSION

#if _PSP_FW_VERSION

#else

void pl2VectorSub3f_SSE(fvector3_t *out, const fvector3_t *a, const fvector3_t *b)
{
    asm volatile(
        "movups %1, %%xmm0\n"
        "movups %2, %%xmm1\n"
        "subps  %%xmm1, %%xmm0\n"
        "movlps %%xmm0, 0 %0\n"
        "movhlps %%xmm0, %%xmm0\n"
        "movss  %%xmm0, 8 %0\n"
        :"=m"(*out)
        :"m"(*a), "m"(*b)
    );
}

void pl2VectorSub3f_NoSSE(fvector3_t *out, const fvector3_t *a, const fvector3_t *b)
{
    out->x = a->x - b->x;
    out->y = a->y - b->y;
    out->z = a->z - b->z;
}

PL2VECTORSUB3F pl2VectorSub3f = pl2VectorSub3f_NoSSE;

#endif // _PSP_FW_VERSION

#if _PSP_FW_VERSION

#else

float pl2VectorDot3f_SSE(const fvector3_t *a, const fvector3_t *b)
{
    float r;
    asm volatile(
    "movups %1, %%xmm0\n"
    "movups %2, %%xmm1\n"
    "mulps  %%xmm1, %%xmm0\n"
    "pshufd $1, %%xmm0, %%xmm1\n"
    "pshufd $2, %%xmm0, %%xmm2\n"
    "addss %%xmm1, %%xmm0\n"
    "addss %%xmm2, %%xmm0\n"
    "movss %%xmm0, %0\n"
    :"=m"(r) :"m"(*a), "m"(*b)
    );
    return r;
}

float pl2VectorDot3f_NoSSE(const fvector3_t *a, const fvector3_t *b)
{
    return a->x * b->x + a->y * b->y + a->z * b->z;
}

PL2VECTORDOT3F pl2VectorDot3f = pl2VectorDot3f_NoSSE;

#endif // _PSP_FW_VERSION

#if _PSP_FW_VERSION

#else

void pl2VectorCross3f_NoSSE(fvector3_t *out, const fvector3_t *a, const fvector3_t *b)
{
    fvector3_t c;
    c.x = a->y * b->z - a->z * b->y;
    c.y = a->z * b->x - a->x * b->z;
    c.z = a->x * b->y - a->y * b->x;
    *out = c;
}

PL2VECTORCROSS3F pl2VectorCross3f = pl2VectorCross3f_NoSSE;

#endif

#if _PSP_FW_VERSION

#else

void pl2VectorScale3f_SSE(fvector3_t *out, const fvector3_t *v, float s)
{
    asm volatile(
        "movss  %2, %%xmm1\n"
        "movups %1, %%xmm0\n"
        "shufps $0, %%xmm1, %%xmm1\n"
        //"pshufd $0, %2, %%xmm1\n"
        "mulps  %%xmm1, %%xmm0\n"
        "movlps %%xmm0, 0 %0\n"
        "movhlps %%xmm0, %%xmm0\n"
        "movss  %%xmm0, 8 %0\n"
        :"=m"(*out)
        :"m"(*v), "m"(s)
    );
}

void pl2VectorScale3f_NoSSE(fvector3_t *out, const fvector3_t *v, float s)
{
    out->x = v->x * s;
    out->y = v->y * s;
    out->z = v->z * s;
}

PL2VECTORSCALE3F pl2VectorScale3f = pl2VectorScale3f_NoSSE;

#endif // _PSP_FW_VERSION

#if _PSP_FW_VERSION

#else

void pl2VectorScaleAdd3f_SSE(fvector3_t *out, const fvector3_t *v, float s)
{
    asm volatile(
        "movss  %2, %%xmm2\n"
        "movups %1, %%xmm1\n"
        "movups %0, %%xmm0\n"
        "shufps $0, %%xmm2, %%xmm2\n"
        //"pshufd $0, %2, %%xmm1\n"
        "mulps  %%xmm2, %%xmm1\n"
        "addps  %%xmm1, %%xmm0\n"
        "movlps %%xmm0, 0 %0\n"
        "movhlps %%xmm0, %%xmm0\n"
        "movss  %%xmm0, 8 %0\n"
        :"=m"(*out)
        :"m"(*v), "m"(s)
    );
}

void pl2VectorScaleAdd3f_NoSSE(fvector3_t *out, const fvector3_t *v, float s)
{
    if(s != s) { DEBUGPRINT("%s: s is NaN!\n", __func__); }
    out->x += v->x * s;
    out->y += v->y * s;
    out->z += v->z * s;
}

PL2VECTORSCALEADD3F pl2VectorScaleAdd3f = pl2VectorScaleAdd3f_NoSSE;

#endif // _PSP_FW_VERSION

#if _PSP_FW_VERSION

#else

float pl2VectorLength3f_NoSSE(const fvector3_t *v)
{
    return sqrtf(pl2VectorDot3f(v, v));
}

PL2VECTORLENGTH3F pl2VectorLength3f = pl2VectorLength3f_NoSSE;

#endif

#if _PSP_FW_VERSION

#else

void pl2VectorNormalize3f_NoSSE(fvector3_t *out, const fvector3_t *v)
{
    float len = pl2VectorLength3f(v);
    if(len) len = 1.0f / len;
    pl2VectorScale3f(out, v, len);
}

PL2VECTORNORMALIZE3F pl2VectorNormalize3f = pl2VectorNormalize3f_NoSSE;

#endif

#if _PSP_FW_VERSION

#else

/* From:
http://listengine.tuxfamily.org/lists.tuxfamily.org/eigen/2009/03/msg00019.html
*/
void pl2QuatMultiply_SSE(fvector4_t *out, const fvector4_t *a, const fvector4_t *b)
{
    static const uint32_t mask[] __attribute__((aligned(16))) =
        { 0x00000000, 0x00000000, 0x00000000, 0x80000000 };
    asm volatile(
        "movaps %2, %%xmm1\n"
        "movaps %1, %%xmm0\n"
        "pshufd $0xff, %%xmm1, %%xmm7\n"
        "pshufd $0x09, %%xmm1, %%xmm6\n"
        "pshufd $0x64, %%xmm1, %%xmm5\n"
        "pshufd $0x12, %%xmm0, %%xmm4\n"
        "pshufd $0x7f, %%xmm0, %%xmm3\n"
        "pshufd $0x89, %%xmm0, %%xmm2\n"
        "pshufd $0x92, %%xmm1, %%xmm1\n"
        "mulps  %%xmm5, %%xmm3\n"
        "mulps  %%xmm1, %%xmm2\n"
        "movaps %3, %%xmm1\n"
        "mulps  %%xmm6, %%xmm4\n"
        "mulps  %%xmm7, %%xmm0\n"
        "xorps  %%xmm1, %%xmm2\n"
        "xorps  %%xmm1, %%xmm3\n"
        "subps  %%xmm4, %%xmm0\n"
        "addps  %%xmm3, %%xmm2\n"
        "addps  %%xmm2, %%xmm0\n"
        "movaps %%xmm0, %0\n"
        :"=m"(*out)
        :"m"(*a), "m"(*b), "m"(mask[0])
    );
}

void pl2QuatMultiply_NoSSE(fvector4_t *out, const fvector4_t *a, const fvector4_t *b)
{
    fvector4_t c;
    c.x = a->w * b->x + a->x * b->w + a->y * b->z - a->z * b->y;
    c.y = a->w * b->y + a->y * b->w + a->z * b->x - a->x * b->z;
    c.z = a->w * b->z + a->z * b->w + a->x * b->y - a->y * b->x;
    c.w = a->w * b->w - a->x * b->x - a->y * b->y - a->z * b->z;
    *out = c;
}

PL2QUATMULTIPLY pl2QuatMultiply = pl2QuatMultiply_NoSSE;

#endif // _PSP_FW_VERSION

#if _PSP_FW_VERSION

#else

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
    //DEBUGPRINT("glmOrbit(<%6.3f %6.3f %6.3f>, <%6.3f %6.3f %6.3f>, <%6.3f %6.3f %6.3f>, <%6.3f %6.3f %6.3f>)",
    //   planet->x, planet->y, planet->z, sun->x, sun->y, sun->z, up->x, up->y, up->z, rotate->x, rotate->y, rotate->z);
    fvector3_t fwd, right;
    pl2VectorSub3f(&fwd, planet, sun);
    pl2VectorCross3f(&right, up, &fwd);

    fvector3_t r = fwd;
    pl2QuatRotate(&r, &r, &right, rotate->x);
    pl2QuatRotate(&r, &r, up,     rotate->y);
    pl2QuatRotate(&r, &r, &fwd,   rotate->z);
    pl2VectorAdd3f(planet, &r, sun);

    //DEBUGPRINT(" == <%6.3f %6.3f %6.3f>\n", planet->x, planet->y, planet->z);
}

void pl2VectorZoom(fvector3_t *obj, const fvector3_t *targ, float distance)
{
    fvector3_t t;
    pl2VectorSub3f(&t, targ, obj);
    pl2VectorScaleAdd3f(obj, &t, distance / pl2VectorLength3f(&t));
}

#endif // _PSP_FW_VERSION

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

/******************************************************************************/

void pl2ModelAnimate(pl2Model *model, const pl2Anim *anim, uint32_t frame)
{
    if(!(model && anim)) return;

    if(frame >= anim->numFrames)
    {
        frame = anim->loopFrame +
                (frame - anim->loopFrame) %
                (anim->numFrames - anim->loopFrame);
    }

    int numBones = (model->numBones < anim->numBones) ? model->numBones : anim->numBones;

    if(numBones <= 0) return;
    
    //DEBUGPRINT("%s: model->numBones == %d, anim->numBones == %d, numBones == %d\n",
    //           __func__, model->numBones, anim->numBones, numBones);

    fmatrix4_t *bones = (fmatrix4_t*)alloca(numBones * sizeof(fmatrix4_t));
    //fmatrix4_t bones[numBones] __attribute__((aligned(16)));
    //fmatrix4_t bones_[numBones+1];
    //fmatrix4_t *bones = (fmatrix4_t*)((((uint32_t)(bones_)) + 15) & ~15);

    int i, j;

    fmatrix4_t *mdlBones = model->bones;
    fmatrix4_t *seqBones = anim->bones + (anim->numBones * frame);

    for(i = 0; i < numBones; i++)
    {
        pl2MultMatrix4f(&(bones[i]), &(mdlBones[i]), &(seqBones[i]));
    }

    for(i = 0; i < model->numObjects; i++)
    {
        pl2Object *obj = &(model->objects[i]);

        uint32_t numVertices = obj->numTriangles * 3;

        for(j = 0; j < numVertices; j++)
        {
            pl2Vertex *vert = &(obj->vertices[j]);

            if(vert->bones[0] != 255)
            {
                float w0 = vert->weights[0],
                      w1 = vert->weights[1],
                      w2 = vert->weights[2],
                      w3 = 1.0f - w0 - w1 - w2;

                fvector4_t v = { vert->vertex.x, vert->vertex.y, vert->vertex.z, 1.0f };
                fvector4_t n = { vert->normal.x, vert->normal.y, vert->normal.z, 0.0f };
                fvector4_t t;
                fvector4_t tv = { 0, 0, 0 }, tn = { 0, 0, 0 };

                pl2VectorTransform4f(&t, &(bones[vert->bones[0]]), &v);
                pl2VectorScaleAdd4f(&tv, &t, w0);
                pl2VectorTransform4f(&t, &(bones[vert->bones[0]]), &n);
                pl2VectorScaleAdd4f(&tn, &t, w0);

                if(vert->bones[1] != 255)
                {
                    pl2VectorTransform4f(&t, &(bones[vert->bones[1]]), &v);
                    pl2VectorScaleAdd4f(&tv, &t, w1);
                    pl2VectorTransform4f(&t, &(bones[vert->bones[1]]), &n);
                    pl2VectorScaleAdd4f(&tn, &t, w1);

                    if(vert->bones[2] != 255)
                    {
                        pl2VectorTransform4f(&t, &(bones[vert->bones[2]]), &v);
                        pl2VectorScaleAdd4f(&tv, &t, w2);
                        pl2VectorTransform4f(&t, &(bones[vert->bones[2]]), &n);
                        pl2VectorScaleAdd4f(&tn, &t, w2);

                        if(vert->bones[3] != 255)
                        {
                            pl2VectorTransform4f(&t, &(bones[vert->bones[3]]), &v);
                            pl2VectorScaleAdd4f(&tv, &t, w3);
                            pl2VectorTransform4f(&t, &(bones[vert->bones[3]]), &n);
                            pl2VectorScaleAdd4f(&tn, &t, w3);
                        }
                    }
                }

                obj->glVertices[j].vertex.x = tv.x;
                obj->glVertices[j].vertex.y = tv.y;
                obj->glVertices[j].vertex.z = tv.z;
                obj->glVertices[j].normal.x = tn.x;
                obj->glVertices[j].normal.y = tn.y;
                obj->glVertices[j].normal.z = tn.z;
            }
            else
            {
                obj->glVertices[j].vertex = vert->vertex;
                obj->glVertices[j].normal = vert->normal;
            }
        }
    }
}

/******************************************************************************/

void pl2DetectSSE()
{
    if(SDL_HasSSE())
    {
        pl2MultMatrix4f = pl2MultMatrix4f_SSE;
        pl2VectorTransform4f = pl2VectorTransform4f_SSE;
        //pl2TransposeMatrix4f = pl2TransposeMatrix4f_SSE;
        pl2VectorAdd4f = pl2VectorAdd4f_SSE;
        pl2VectorSub4f = pl2VectorSub4f_SSE;
        pl2VectorDot4f = pl2VectorDot4f_SSE;
        pl2VectorScale4f = pl2VectorScale4f_SSE;
        pl2VectorScaleAdd4f = pl2VectorScaleAdd4f_SSE;
        pl2VectorAdd3f = pl2VectorAdd3f_SSE;
        pl2VectorSub3f = pl2VectorSub3f_SSE;
        pl2VectorDot3f = pl2VectorDot3f_SSE;
        //pl2VectorCross3f = pl2VectorCross3f_SSE;
        pl2VectorScale3f = pl2VectorScale3f_SSE;
        pl2VectorScaleAdd3f = pl2VectorScaleAdd3f_SSE;
        //pl2VectorLength3f = pl2VectorLength3f_SSE;
        //pl2VectorNormalize3f = pl2VectorNormalize3f_SSE;
        pl2QuatMultiply = pl2QuatMultiply_SSE;
    }
}

/******************************************************************************/

#ifdef VMTEST

#include <time.h>

#define NUM_ROUNDS 1000000

#define TIME(f, x...) \
    start = clock(); \
    for(i = 0; i < NUM_ROUNDS; i++) { \
        f(x); } \
    stop = clock(); \
    DEBUGPRINT("%s: %d rounds in %.2fs = %.1f rounds/s\n", #f, NUM_ROUNDS, \
        (float)(stop - start) / (float)(CLOCKS_PER_SEC), \
        (float)((long long)NUM_ROUNDS * (long long)CLOCKS_PER_SEC) / (float)(stop - start));

int main(int argc, char *argv[])
{
    const fvector4_t zero = { 0, 0, 0, 0 }, one = { 1, 1, 1, 1 };

    fvector4_t a, b, c;
    fmatrix4_t m, n, o;
    
    clock_t start, stop; int i;
    
    TIME(pl2VectorTransform4f_SSE, &a, &m, &b);
    TIME(pl2VectorTransform4f_NoSSE, &a, &m, &b);
    TIME(pl2MultMatrix4f_SSE, &m, &n, &o);
    TIME(pl2MultMatrix4f_NoSSE, &m, &n, &o);

    return 0;
}

#endif

