#ifndef __OPL2_VM_H__
#define __OPL2_VM_H__

/******************************************************************************
 * OPL2 Vector/Matrix Operations
 ******************************************************************************/

#if _PSP_FW_VERSION

void pl2MultMatrix4f(fmatrix4_t *out, const fmatrix4_t *a, const fmatrix4_t *b);
void pl2VectorTransform4f(fvector4_t *out, const fmatrix4_t *m, const fvector4_t *v);
void pl2TransposeMatrix4f(fmatrix4_t *out, const fmatrix4_t *m);
void pl2VectorAdd4f(fvector4_t *out, const fvector4_t *a, const fvector4_t *b);
void pl2VectorSub4f(fvector4_t *out, const fvector4_t *a, const fvector4_t *b);
float pl2VectorDot4f(const fvector4_t *a, const fvector4_t *b);
void pl2VectorScale4f(fvector4_t *out, const fvector4_t *v, float s);
void pl2VectorScaleAdd4f(fvector4_t *out, const fvector4_t *v, float s);
void pl2VectorTransScaleAdd4f(fvector4_t *out, const fmatrix4_t *m, const fvector4_t *v, float s);
void pl2VectorAdd3f(fvector3_t *out, const fvector3_t *a, const fvector3_t *b);
void pl2VectorSub3f(fvector3_t *out, const fvector3_t *a, const fvector3_t *b);
float pl2VectorDot3f(const fvector3_t *a, const fvector3_t *b);
void pl2VectorCross3f(fvector3_t *out, const fvector3_t *a, const fvector3_t *b);
void pl2VectorScale3f(fvector3_t *out, const fvector3_t *v, float s);
void pl2VectorScaleAdd3f(fvector3_t *out, const fvector3_t *v, float s);
float pl2VectorLength3f(const fvector3_t *v);
void pl2VectorNormalize3f(fvector3_t *out, const fvector3_t *v);
void pl2QuatMultiply(fvector4_t *out, const fvector4_t *a, const fvector4_t *b);
void pl2QuatRotate(fvector3_t *out, const fvector3_t *v, const fvector3_t *axis, float angle);
void pl2VectorOrbit(fvector3_t *planet, const fvector3_t *sun, const fvector3_t *up, const fvector3_t *rotate);
void pl2VectorZoom(fvector3_t *obj, const fvector3_t *targ, float distance);

# else

extern void (*pl2MultMatrix4f)(fmatrix4_t *out, const fmatrix4_t *a, const fmatrix4_t *b);
extern void (*pl2VectorTransform4f)(fvector4_t *out, const fmatrix4_t *m, const fvector4_t *v);
extern void (*pl2TransposeMatrix4f)(fmatrix4_t *out, const fmatrix4_t *m);
extern void (*pl2VectorAdd4f)(fvector4_t *out, const fvector4_t *a, const fvector4_t *b);
extern void (*pl2VectorSub4f)(fvector4_t *out, const fvector4_t *a, const fvector4_t *b);
extern float (*pl2VectorDot4f)(const fvector4_t *a, const fvector4_t *b);
extern void (*pl2VectorScale4f)(fvector4_t *out, const fvector4_t *v, float s);
extern void (*pl2VectorScaleAdd4f)(fvector4_t *out, const fvector4_t *v, float s);
extern void (*pl2VectorTransScaleAdd4f)(fvector4_t *out, const fmatrix4_t *m, const fvector4_t *v, float s);
extern void (*pl2VectorAdd3f)(fvector3_t *out, const fvector3_t *a, const fvector3_t *b);
extern void (*pl2VectorSub3f)(fvector3_t *out, const fvector3_t *a, const fvector3_t *b);
extern float (*pl2VectorDot3f)(const fvector3_t *a, const fvector3_t *b);
extern void (*pl2VectorCross3f)(fvector3_t *out, const fvector3_t *a, const fvector3_t *b);
extern void (*pl2VectorScale3f)(fvector3_t *out, const fvector3_t *v, float s);
extern void (*pl2VectorScaleAdd3f)(fvector3_t *out, const fvector3_t *v, float s);
extern float (*pl2VectorLength3f)(const fvector3_t *v);
extern void (*pl2VectorNormalize3f)(fvector3_t *out, const fvector3_t *v);
extern void (*pl2QuatMultiply)(fvector4_t *out, const fvector4_t *a, const fvector4_t *b);
extern void pl2QuatRotate(fvector3_t *out, const fvector3_t *v, const fvector3_t *axis, float angle);
extern void pl2VectorOrbit(fvector3_t *planet, const fvector3_t *sun, const fvector3_t *up, const fvector3_t *rotate);
extern void pl2VectorZoom(fvector3_t *obj, const fvector3_t *targ, float distance);

# endif

#endif // __OPL2_VM_H__
