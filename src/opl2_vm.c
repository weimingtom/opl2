#include "opl2_int.h"
#include "opl2_vm.h"

#include <math.h>

/******************************************************************************/

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

static fmatrix4_t pl2_temp_bones[256];

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

    int i, j;

    fmatrix4_t *bones = pl2_temp_bones;
    fmatrix4_t *mdlBones = model->bones;
    fmatrix4_t *seqBones = anim->bones + (anim->numBones * frame);

    for(i = 0; i < numBones; i++)
    {
        pl2MultMatrix4f(&(bones[i]), &(mdlBones[i]), &(seqBones[i]));
    }
    bones[255] = (fmatrix4_t){{1,0,0,0},{0,1,0,0},{0,0,1,0},{0,0,0,1}};

    for(i = 0; i < model->numObjects; i++)
    {
        pl2Object *obj = &(model->objects[i]);

        uint32_t numVertices = obj->numTriangles * 3;

        for(j = 0; j < numVertices; j++)
        {
            pl2Vertex *vert = &(obj->vertices[j]);

            DEBUGPRINTIF(vert->bones[0] >= numBones,
                         "%s: need bone %d but only %d bones\n",
                         __func__, vert->bones[0], numBones);
            DEBUGPRINTIF(vert->bones[1] >= numBones,
                         "%s: need bone %d but only %d bones\n",
                         __func__, vert->bones[1], numBones);
            DEBUGPRINTIF(vert->bones[2] >= numBones,
                         "%s: need bone %d but only %d bones\n",
                         __func__, vert->bones[2], numBones);
            DEBUGPRINTIF(vert->bones[3] >= numBones,
                         "%s: need bone %d but only %d bones\n",
                         __func__, vert->bones[3], numBones);

            float w0 = vert->weights[0],
                  w1 = vert->weights[1],
                  w2 = vert->weights[2],
                  w3 = 1.0f - w0 - w1 - w2;

            fvector4_t v = { vert->vertex.x, vert->vertex.y, vert->vertex.z, 1.0f };
            fvector4_t n = { vert->normal.x, vert->normal.y, vert->normal.z, 0.0f };
            //fvector4_t t;
            fvector4_t tv = { 0, 0, 0 }, tn = { 0, 0, 0 };

            //pl2VectorTransform4f(&t, &(bones[vert->bones[0]]), &v);
            //pl2VectorScaleAdd4f(&tv, &t, w0);
            //pl2VectorTransform4f(&t, &(bones[vert->bones[0]]), &n);
            //pl2VectorScaleAdd4f(&tn, &t, w0);
            pl2VectorTransScaleAdd4f(&tv, &(bones[vert->bones[0]]), &v, w0);
            pl2VectorTransScaleAdd4f(&tn, &(bones[vert->bones[0]]), &n, w0);

            //pl2VectorTransform4f(&t, &(bones[vert->bones[1]]), &v);
            //pl2VectorScaleAdd4f(&tv, &t, w1);
            //pl2VectorTransform4f(&t, &(bones[vert->bones[1]]), &n);
            //pl2VectorScaleAdd4f(&tn, &t, w1);
            pl2VectorTransScaleAdd4f(&tv, &(bones[vert->bones[1]]), &v, w1);
            pl2VectorTransScaleAdd4f(&tn, &(bones[vert->bones[1]]), &n, w1);

            //pl2VectorTransform4f(&t, &(bones[vert->bones[2]]), &v);
            //pl2VectorScaleAdd4f(&tv, &t, w2);
            //pl2VectorTransform4f(&t, &(bones[vert->bones[2]]), &n);
            //pl2VectorScaleAdd4f(&tn, &t, w2);
            pl2VectorTransScaleAdd4f(&tv, &(bones[vert->bones[2]]), &v, w2);
            pl2VectorTransScaleAdd4f(&tn, &(bones[vert->bones[2]]), &n, w2);

            //pl2VectorTransform4f(&t, &(bones[vert->bones[3]]), &v);
            //pl2VectorScaleAdd4f(&tv, &t, w3);
            //pl2VectorTransform4f(&t, &(bones[vert->bones[3]]), &n);
            //pl2VectorScaleAdd4f(&tn, &t, w3);
            pl2VectorTransScaleAdd4f(&tv, &(bones[vert->bones[3]]), &v, w3);
            pl2VectorTransScaleAdd4f(&tn, &(bones[vert->bones[3]]), &n, w3);

            obj->glVertices[j].vertex.x = tv.x;
            obj->glVertices[j].vertex.y = tv.y;
            obj->glVertices[j].vertex.z = tv.z;
            obj->glVertices[j].normal.x = tn.x;
            obj->glVertices[j].normal.y = tn.y;
            obj->glVertices[j].normal.z = tn.z;
        }
    }
}
