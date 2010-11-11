//#include "opl2.h"
#include "opl2_int.h"

#include <math.h>

/******************************************************************************/

void pl2MultMatrix4f(fmatrix4_t *out, const fmatrix4_t *a, const fmatrix4_t *b)
{
#if 0 //_PSP_FW_VERSION
    asm volatile (
        "ulv.q   c100,  0+%1\n"
        "ulv.q   c110, 16+%1\n"
        "ulv.q   c120, 32+%1\n"
        "ulv.q   c130, 48+%1\n"
        "ulv.q   c200,  0+%2\n"
        "ulv.q   c210, 16+%2\n"
        "ulv.q   c220, 32+%2\n"
        "ulv.q   c230, 48+%2\n"
        "vmmul.q m000, m100, m200\n"
        "usv.q   c000,  0+%0\n"
        "usv.q   c010,  0+%0\n"
        "usv.q   c020,  0+%0\n"
        "usv.q   c030,  0+%0\n"
        :"=m"(*out)
        :"m"(*a), "m"(*b)
    );
#elif WITH_SSE
/*
R = [[ Axx*Bxx+Ayx*Bxy+Azx*Bxz+Awx*Bxw, ... ]]
*/
    asm volatile(
        "movups  0 %2, %%xmm4\n"
        "movups 16 %2, %%xmm5\n"
        "movups 32 %2, %%xmm6\n"
        "movups 48 %2, %%xmm7\n"

        "movss   0 %1, %%xmm0\n"
        "movss   4 %1, %%xmm1\n"
        "movss   8 %1, %%xmm2\n"
        "movss  12 %1, %%xmm3\n"
        "shufps $0, %%xmm0, %%xmm0\n"
        "shufps $0, %%xmm1, %%xmm1\n"
        "shufps $0, %%xmm2, %%xmm2\n"
        "shufps $0, %%xmm3, %%xmm3\n"
        "mulps  %%xmm4, %%xmm0\n"
        "mulps  %%xmm5, %%xmm1\n"
        "mulps  %%xmm6, %%xmm2\n"
        "mulps  %%xmm7, %%xmm3\n"
        "addps  %%xmm1, %%xmm0\n"
        "addps  %%xmm3, %%xmm2\n"
        "addps  %%xmm2, %%xmm0\n"
        "movups %%xmm0,  0 %0\n"

        "movss  16 %1, %%xmm0\n"
        "movss  20 %1, %%xmm1\n"
        "movss  24 %1, %%xmm2\n"
        "movss  28 %1, %%xmm3\n"
        "shufps $0, %%xmm0, %%xmm0\n"
        "shufps $0, %%xmm1, %%xmm1\n"
        "shufps $0, %%xmm2, %%xmm2\n"
        "shufps $0, %%xmm3, %%xmm3\n"
        "mulps  %%xmm4, %%xmm0\n"
        "mulps  %%xmm5, %%xmm1\n"
        "mulps  %%xmm6, %%xmm2\n"
        "mulps  %%xmm7, %%xmm3\n"
        "addps  %%xmm1, %%xmm0\n"
        "addps  %%xmm3, %%xmm2\n"
        "addps  %%xmm2, %%xmm0\n"
        "movups %%xmm0, 16 %0\n"

        "movss  32 %1, %%xmm0\n"
        "movss  36 %1, %%xmm1\n"
        "movss  40 %1, %%xmm2\n"
        "movss  44 %1, %%xmm3\n"
        "shufps $0, %%xmm0, %%xmm0\n"
        "shufps $0, %%xmm1, %%xmm1\n"
        "shufps $0, %%xmm2, %%xmm2\n"
        "shufps $0, %%xmm3, %%xmm3\n"
        "mulps  %%xmm4, %%xmm0\n"
        "mulps  %%xmm5, %%xmm1\n"
        "mulps  %%xmm6, %%xmm2\n"
        "mulps  %%xmm7, %%xmm3\n"
        "addps  %%xmm1, %%xmm0\n"
        "addps  %%xmm3, %%xmm2\n"
        "addps  %%xmm2, %%xmm0\n"
        "movups %%xmm0, 32 %0\n"

        "movss  48 %1, %%xmm0\n"
        "movss  52 %1, %%xmm1\n"
        "movss  56 %1, %%xmm2\n"
        "movss  60 %1, %%xmm3\n"
        "shufps $0, %%xmm0, %%xmm0\n"
        "shufps $0, %%xmm1, %%xmm1\n"
        "shufps $0, %%xmm2, %%xmm2\n"
        "shufps $0, %%xmm3, %%xmm3\n"
        "mulps  %%xmm4, %%xmm0\n"
        "mulps  %%xmm5, %%xmm1\n"
        "mulps  %%xmm6, %%xmm2\n"
        "mulps  %%xmm7, %%xmm3\n"
        "addps  %%xmm1, %%xmm0\n"
        "addps  %%xmm3, %%xmm2\n"
        "addps  %%xmm2, %%xmm0\n"
        "movups %%xmm0, 48 %0\n"

        :"=m"(*out)
        :"m"(*a), "m"(*b)
    );
#else
    fmatrix4_t c;
# define MMUL(i,j) \
    c.i.j = a->i.x * b->x.j + a->i.y * b->y.j + a->i.z * b->z.j + a->i.w * b->w.j
    MMUL(x,x); MMUL(y,x); MMUL(z,x); MMUL(w,x);
    MMUL(x,y); MMUL(y,y); MMUL(z,y); MMUL(w,y);
    MMUL(x,z); MMUL(y,z); MMUL(z,z); MMUL(w,z);
    MMUL(x,w); MMUL(y,w); MMUL(z,w); MMUL(w,w);
# undef MMUL
    *out = c;
#endif // _PSP_FW_VERSION
}

void pl2VectorTransform4f(fvector4_t *out, const fmatrix4_t *m, const fvector4_t *v)
{
#if 0 //_PSP_FW_VERSION
    asm volatile (
        "ulv.q   c100,  0+%1\n"
        "ulv.q   c110, 16+%1\n"
        "ulv.q   c120, 32+%1\n"
        "ulv.q   c130, 48+%1\n"
        "ulv.q   c010,  0+%2\n"
        "vtfm4.q c000, m100, c010\n"
        "usv.q   c000,  0+%0\n"
        :"=m"(*out)
        :"m"(*m), "m"(*v)
    );
#elif WITH_SSE
    asm volatile(
        "movaps  0 %1, %%xmm4\n"
        "movaps 16 %1, %%xmm5\n"
        "movaps 32 %1, %%xmm6\n"
        "movaps 48 %1, %%xmm7\n"

        "movss   0 %2, %%xmm0\n"
        "movss   4 %2, %%xmm1\n"
        "movss   8 %2, %%xmm2\n"
        "movss  12 %2, %%xmm3\n"
        "shufps $0, %%xmm0, %%xmm0\n"
        "shufps $0, %%xmm1, %%xmm1\n"
        "shufps $0, %%xmm2, %%xmm2\n"
        "shufps $0, %%xmm3, %%xmm3\n"
        "mulps  %%xmm4, %%xmm0\n"
        "mulps  %%xmm5, %%xmm1\n"
        "mulps  %%xmm6, %%xmm2\n"
        "mulps  %%xmm7, %%xmm3\n"
        "addps  %%xmm1, %%xmm0\n"
        "addps  %%xmm3, %%xmm2\n"
        "addps  %%xmm2, %%xmm0\n"
        "movups %%xmm0,  0 %0\n"
        :"=m"(*out)
        :"m"(*m), "m"(*v)
    );
#else
    fvector4_t u;
# define VTFM(i) \
    u.i = m->x.i * v->x + m->y.i * v->y + m->z.i * v->z + m->w.i*v->w
    VTFM(x); VTFM(y); VTFM(z); VTFM(w);
# undef VTFM
    *out = u;
#endif // _PSP_FW_VERSION
}

void pl2TransposeMatrix4f(fmatrix4_t *out, const fmatrix4_t *m)
{
#if 0 //_PSP_FW_VERSION
    asm volatile(
        "ulv.q  c000,  0+%1\n"
        "ulv.q  c010, 16+%1\n"
        "ulv.q  c020, 32+%1\n"
        "ulv.q  c030, 48+%1\n"
        "usv.q  r000,  0+%0\n"
        "usv.q  r001, 16+%0\n"
        "usv.q  r002, 32+%0\n"
        "usv.q  r003, 48+%0\n"
        :"=m"(*out) :"m"(*m)
    );
//#elif WITH_SSE
//# error SSE not implemented
#else
# define MTRAN(i,j) \
    n.i.j = m->j.i

    fmatrix4_t n;
    MTRAN(x,x); MTRAN(y,x); MTRAN(z,x); MTRAN(w,x);
    MTRAN(x,y); MTRAN(y,y); MTRAN(z,y); MTRAN(w,y);
    MTRAN(x,z); MTRAN(y,z); MTRAN(z,z); MTRAN(w,z);
    MTRAN(x,w); MTRAN(y,w); MTRAN(z,w); MTRAN(w,w);
    *out = n;

# undef MTRAN
#endif
}

void pl2VectorAdd4f(fvector4_t *out, const fvector4_t *a, const fvector4_t *b)
{
#if 0 //_PSP_FW_VERSION
    asm volatile(
        "ulv.q  c010, 0+%1\n"
        "ulv.q  c020, 0+%2\n"
        "vadd.q c000, c010, c020\n"
        "usv.q  c000, 0+%0\n"
        :"=m"(*out)
        :"m"(*a), "m"(*b)
    );
#elif WITH_SSE
    asm volatile(
        "movups %1, %%xmm0\n"
        "movups %2, %%xmm1\n"
        "addps  %%xmm1, %%xmm0\n"
        "movups %%xmm0, %0\n"
        :"=m"(*out)
        :"m"(*a), "m"(*b)
    );
#else
    out->x = a->x + b->x;
    out->y = a->y + b->y;
    out->z = a->z + b->z;
    out->w = a->w + b->w;
#endif
}

void pl2VectorSub4f(fvector4_t *out, const fvector4_t *a, const fvector4_t *b)
{
#if 0 //_PSP_FW_VERSION
    asm volatile(
        "ulv.q  c010, 0+%1\n"
        "ulv.q  c020, 0+%2\n"
        "vsub.q c000, c010, c020\n"
        "usv.q  c000, 0+%0\n"
        :"=m"(*out)
        :"m"(*a), "m"(*b)
    );
#elif WITH_SSE
    asm volatile(
        "movups %1, %%xmm0\n"
        "movups %2, %%xmm1\n"
        "subps  %%xmm1, %%xmm0\n"
        "movups %%xmm0, %0\n"
        :"=m"(*out)
        :"m"(*a), "m"(*b)
    );
#else
    out->x = a->x - b->x;
    out->y = a->y - b->y;
    out->z = a->z - b->z;
    out->w = a->w - b->w;
#endif
}

float pl2VectorDot4f(const fvector4_t *a, const fvector4_t *b)
{
    return a->x * b->x + a->y * b->y + a->z * b->z + a->w * b->w;
}

void pl2VectorScale4f(fvector4_t *out, const fvector4_t *v, float s)
{
#if 0 //_PSP_FW_VERSION
    asm volatile(
        "mtv    %2, s020\n"
        "ulv.q  c010, 0+%1\n"
        "vscl.q c000, c010, s020\n"
        "usv.q  c000, 0+%0\n"
        :"=m"(*out)
        :"m"(*v), "r"(s)
    );
#elif WITH_SSE
    asm volatile(
        "movss  %2, %%xmm1\n"
        "movups %1, %%xmm0\n"
        "shufps $0, %%xmm1, %%xmm1\n"
        "mulps  %%xmm1, %%xmm0\n"
        "movups %%xmm0, %0\n"
        :"=m"(*out)
        :"m"(*v), "m"(s)
    );
#else
    out->x = v->x * s;
    out->y = v->y * s;
    out->z = v->z * s;
    out->w = v->w * s;
#endif
}

void pl2VectorScaleAdd4f(fvector4_t *out, const fvector4_t *v, float s)
{
#if 0 //_PSP_FW_VERSION
    asm volatile(
        "mtv    %2, s020\n"
        "ulv.q  c000, 0+%0\n"
        "ulv.q  c010, 0+%1\n"
        "vscl.q c010, c010, s020\n"
        "vadd.q c000, c000, c010\n"
        "usv.q  c000, 0+%0\n"
        :"+m"(*out)
        :"m"(*v), "r"(s)
    );
#elif WITH_SSE
    asm volatile(
        "movss  %2, %%xmm2\n"
        "movups %1, %%xmm1\n"
        "movups %0, %%xmm0\n"
        "shufps $0, %%xmm2, %%xmm2\n"
        "mulps  %%xmm2, %%xmm1\n"
        "addps  %%xmm1, %%xmm0\n"
        "movups %%xmm0, %0\n"
        :"=m"(*out)
        :"m"(*v), "m"(s)
    );
#else
    out->x += v->x * s;
    out->y += v->y * s;
    out->z += v->z * s;
    out->w += v->w * s;
#endif
}

void pl2VectorAdd3f(fvector3_t *out, const fvector3_t *a, const fvector3_t *b)
{
#if WITH_SSE
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
#else
    out->x = a->x + b->x;
    out->y = a->y + b->y;
    out->z = a->z + b->z;
#endif
}

void pl2VectorSub3f(fvector3_t *out, const fvector3_t *a, const fvector3_t *b)
{
#if WITH_SSE
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
#else
    out->x = a->x - b->x;
    out->y = a->y - b->y;
    out->z = a->z - b->z;
#endif
}

float pl2VectorDot3f(const fvector3_t *a, const fvector3_t *b)
{
   return a->x * b->x + a->y * b->y + a->z * b->z;
}

void pl2VectorCross3f(fvector3_t *out, const fvector3_t *a, const fvector3_t *b)
{
   fvector3_t c;
   c.x = a->y * b->z - a->z * b->y;
   c.y = a->z * b->x - a->x * b->z;
   c.z = a->x * b->y - a->y * b->x;
   *out = c;
}

void pl2VectorScale3f(fvector3_t *out, const fvector3_t *v, float s)
{
#if WITH_SSE
    asm volatile(
        "movss  %2, %%xmm1\n"
        "movups %1, %%xmm0\n"
        "shufps $0, %%xmm1, %%xmm1\n"
        "mulps  %%xmm1, %%xmm0\n"
        "movlps %%xmm0, 0 %0\n"
        "movhlps %%xmm0, %%xmm0\n"
        "movss  %%xmm0, 8 %0\n"
        :"=m"(*out)
        :"m"(*v), "m"(s)
    );
#else
   out->x = v->x * s;
   out->y = v->y * s;
   out->z = v->z * s;
#endif
}

void pl2VectorScaleAdd3f(fvector3_t *out, const fvector3_t *v, float s)
{
#if 0 //_PSP_FW_VERSION
    asm volatile (
        "mtv    %2, s020\n"
        "ulv.q  c010, 0+%1\n"
        "vscl.t c000, c010, s020\n"
        "sv.s   s000, 0+%0\n"
        "sv.s   s001, 4+%0\n"
        "sv.s   s002, 8+%0\n"
        :"=m"(*out)
        :"m"(*v), "r"(s)
    );
#elif WITH_SSE
    asm volatile(
        "movss  %2, %%xmm2\n"
        "movups %1, %%xmm1\n"
        "movups %0, %%xmm0\n"
        "shufps $0, %%xmm2, %%xmm2\n"
        "mulps  %%xmm2, %%xmm1\n"
        "addps  %%xmm1, %%xmm0\n"
        "movlps %%xmm0, 0 %0\n"
        "movhlps %%xmm0, %%xmm0\n"
        "movss  %%xmm0, 8 %0\n"
        :"=m"(*out)
        :"m"(*v), "m"(s)
    );
#else
    if(s != s) { DEBUGPRINT("%s: s is NaN!\n", __func__); }
    out->x += v->x * s;
    out->y += v->y * s;
    out->z += v->z * s;
#endif // _PSP_FW_VERSION
}

float pl2VectorLength3f(const fvector3_t *v)
{
   return sqrtf(pl2VectorDot3f(v, v));
}

void pl2VectorNormalize3f(fvector3_t *out, const fvector3_t *v)
{
   float len = pl2VectorLength3f(v);
   if(len) len = 1.0f / len;
   pl2VectorScale3f(out, v, len);
}

void pl2QuatMultiply(fvector4_t *out, const fvector4_t *a, const fvector4_t *b)
{
#if 0 //_PSP_FW_VERSION
    asm volatile (
        "ulv.q  c010, 0+%1\n"
        "ulv.q  c020, 0+%2\n"
        "vqmul.q c000, c010, c020\n"
        "usv.q  c000, 0+%0\n"
        :"=m"(*out)
        :"m"(*a), "m"(*b)
    );
#elif 0 // WITH_SSE
    asm volatile(
        "movups 0 %1, %%xmm0\n"
        "movups 0 %2, %%xmm1\n"
        "movaps %%xmm0, %%xmm4\n"
        "movaps %%xmm0, %%xmm2\n"
        "movaps %%xmm1, %%xmm3\n"
        "shufps $0xff, %%xmm4, %%xmm4\n"
        "mulps %%xmm1, %%xmm4\n"
        :"=m"(*out)
        :"m"(*a), "m"(*b)
    );
#else
    fvector4_t c;
    c.x = a->w * b->x + a->x * b->w + a->y * b->z - a->z * b->y;
    c.y = a->w * b->y + a->y * b->w + a->z * b->x - a->x * b->z;
    c.z = a->w * b->z + a->z * b->w + a->x * b->y - a->y * b->x;
    c.w = a->w * b->w - a->x * b->x - a->y * b->y - a->z * b->z;
    *out = c;
#endif
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

    int numBones = (model->numBones > anim->numBones) ? model->numBones : anim->numBones;

    if(numBones <= 0) return;

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
                fvector3_t tv = { 0, 0, 0 }, tn = { 0, 0, 0 };

                pl2VectorTransform4f(&t, &(bones[vert->bones[0]]), &v);
                pl2VectorScaleAdd3f(&tv, (fvector3_t*)&t, w0);
                pl2VectorTransform4f(&t, &(bones[vert->bones[0]]), &n);
                pl2VectorScaleAdd3f(&tn, (fvector3_t*)&t, w0);

                if(vert->bones[1] != 255)
                {
                    pl2VectorTransform4f(&t, &(bones[vert->bones[1]]), &v);
                    pl2VectorScaleAdd3f(&tv, (fvector3_t*)&t, w1);
                    pl2VectorTransform4f(&t, &(bones[vert->bones[1]]), &n);
                    pl2VectorScaleAdd3f(&tn, (fvector3_t*)&t, w1);

                    if(vert->bones[2] != 255)
                    {
                        pl2VectorTransform4f(&t, &(bones[vert->bones[2]]), &v);
                        pl2VectorScaleAdd3f(&tv, (fvector3_t*)&t, w2);
                        pl2VectorTransform4f(&t, &(bones[vert->bones[2]]), &n);
                        pl2VectorScaleAdd3f(&tn, (fvector3_t*)&t, w2);

                        if(vert->bones[3] != 255)
                        {
                            pl2VectorTransform4f(&t, &(bones[vert->bones[3]]), &v);
                            pl2VectorScaleAdd3f(&tv, (fvector3_t*)&t, w3);
                            pl2VectorTransform4f(&t, &(bones[vert->bones[3]]), &n);
                            pl2VectorScaleAdd3f(&tn, (fvector3_t*)&t, w3);
                        }
                    }
                }

                obj->glVertices[j].vertex = tv;
                obj->glVertices[j].normal = tn;
            }
            else
            {
                obj->glVertices[j].vertex = vert->vertex;
                obj->glVertices[j].normal = vert->normal;
            }
        }
    }
}

void pl2CharAnimate(pl2Character *chr, float dt)
{
    if(chr && chr->anim)
    {
        chr->time += dt;

        if(chr->visible > 0)
        {
            int frame = 30 * chr->time;
            int count = chr->anim->numFrames;
            int loop  = chr->anim->loopFrame;

            if(frame >= count)
            {
                frame = loop + (frame - loop) % (count - loop);
            }

            if(frame != chr->frame)
            {
                int i;
                for(i = 0; i < PL2_MAX_CHARPARTS; i++)
                {
                    pl2ModelAnimate(chr->models[i], chr->anim, frame);
                }

                chr->frame = frame;
            }
        }
    }
}
