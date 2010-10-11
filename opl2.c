#include "opl2.h"
#include "opl2_int.h"

#include <AL/al.h>
#include <AL/alut.h>

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

/* use freeglut extensions, if available */
#if FREEGLUT
# include <GL/freeglut_ext.h>
#endif

#if _PSP_FW_VERSION
# include <pspuser.h>
# include <psprtc.h>

PSP_MODULE_INFO("opl2psp",0,1,0);
PSP_MAIN_THREAD_ATTR(PSP_THREAD_ATTR_VFPU);
PSP_HEAP_SIZE_MAX();

static SceUInt32 tick_res = 0;
static SceUInt64 first_tick = 0;

int glutGet(GLenum what)
{
    switch(what)
    {
        case GLUT_ELAPSED_TIME:
        {
            if(!first_tick)
            {
                tick_res = sceRtcGetTickResolution();
                sceRtcGetCurrentTick(&first_tick);
            }

            SceUInt64 this_tick;
            sceRtcGetCurrentTick(&this_tick);
            return (float)(this_tick - first_tick) / (float)(tick_res);
        }
    }
    return -1;
}

#endif

/******************************************************************************/

void pl2MultMatrix(fmatrix4_t *out, const fmatrix4_t *a, const fmatrix4_t *b)
{
#if _PSP_FW_VERSION
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
#else
    fmatrix4_t c;
# define MMUL(i,j) \
    c.i.j = a->x.j * b->i.x + a->y.j * b->i.y + a->z.j * b->i.z + a->w.j*b->i.w
    MMUL(x,x); MMUL(y,x); MMUL(z,x); MMUL(w,x);
    MMUL(x,y); MMUL(y,y); MMUL(z,y); MMUL(w,y);
    MMUL(x,z); MMUL(y,z); MMUL(z,z); MMUL(w,z);
    MMUL(x,w); MMUL(y,w); MMUL(z,w); MMUL(w,w);
# undef MMUL
    *out = c;
#endif // _PSP_FW_VERSION
}

void pl2TransformVector(fvector4_t *out, const fmatrix4_t *m, const fvector4_t *v)
{
#if _PSP_FW_VERSION
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
#else
    fvector4_t u;
# define VTFM(i) \
    u.i = m->x.i * v->x + m->y.i * v->y + m->z.i * v->z + m->w.i*v->w
    VTFM(x); VTFM(y); VTFM(z); VTFM(w);
# undef VTFM
    *out = u;
#endif // _PSP_FW_VERSION
}

void pl2AddScaledVector(fvector3_t *out, const fvector3_t *v, float s)
{
#if 0 // _PSP_FW_VERSION
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
#else
    if(s != s) DEBUGPRINT("%s: s is NaN!\n", __func__);
    out->x += v->x * s;
    out->y += v->y * s;
    out->z += v->z * s;
#endif // _PSP_FW_VERSION
}

void pl2ModelAnimate(pl2Model *model, const pl2Sequence *sequence, uint32_t frame)
{
    if(!(model && sequence)) return;

    if(frame >= sequence->numFrames)
    {
        frame = sequence->loopFrame +
                (frame - sequence->loopFrame) %
                (sequence->numFrames - sequence->loopFrame);
    }

    int numBones = (model->numBones > sequence->numBones) ? model->numBones : sequence->numBones;

    fmatrix4_t bones[numBones];

    int i, j;

    fmatrix4_t *mdlBones = model->bones, *seqBones = sequence->bones + (sequence->numBones * frame);

    for(i = 0; i < numBones; i++)
    {
        pl2MultMatrix(&(bones[i]), &(seqBones[i]), &(mdlBones[i]));
    }

    for(i = 0; i < model->numObjects; i++)
    {
        pl2Object *obj = &(model->objects[i]);

        uint32_t numVertices = obj->numTriangles * 3;

        for(j = 0; j < numVertices; j++)
        {
            pl2Vertex *vert = &(obj->vertices[j]);

            float w0 = vert->weights[0],
                  w1 = vert->weights[1],
                  w2 = vert->weights[2],
                  w3 = 1.0f - w0 - w1 - w2;

            fvector4_t v = { vert->vertex.x, vert->vertex.y, vert->vertex.z, 1.0f };
            fvector4_t n = { vert->normal.x, vert->normal.y, vert->normal.z, 0.0f };
            fvector4_t t;
            fvector3_t tv = { 0, 0, 0 }, tn = { 0, 0, 0 };

            if(vert->bones[0] != 255)
            {
                pl2TransformVector(&t, &(bones[vert->bones[0]]), &v);
                pl2AddScaledVector(&tv, (fvector3_t*)&t, w0);
                pl2TransformVector(&t, &(bones[vert->bones[0]]), &n);
                pl2AddScaledVector(&tn, (fvector3_t*)&t, w0);
            }

            if(vert->bones[1] != 255)
            {
                pl2TransformVector(&t, &(bones[vert->bones[1]]), &v);
                pl2AddScaledVector(&tv, (fvector3_t*)&t, w1);
                pl2TransformVector(&t, &(bones[vert->bones[1]]), &n);
                pl2AddScaledVector(&tn, (fvector3_t*)&t, w1);
            }

            if(vert->bones[2] != 255)
            {
                pl2TransformVector(&t, &(bones[vert->bones[2]]), &v);
                pl2AddScaledVector(&tv, (fvector3_t*)&t, w2);
                pl2TransformVector(&t, &(bones[vert->bones[2]]), &n);
                pl2AddScaledVector(&tn, (fvector3_t*)&t, w2);
            }

            if(vert->bones[3] != 255)
            {
                pl2TransformVector(&t, &(bones[vert->bones[3]]), &v);
                pl2AddScaledVector(&tv, (fvector3_t*)&t, w3);
                pl2TransformVector(&t, &(bones[vert->bones[3]]), &n);
                pl2AddScaledVector(&tn, (fvector3_t*)&t, w3);
            }

            obj->glVertices[j].vertex = tv;
            obj->glVertices[j].normal = tn;
        }
    }
}

void pl2CharAnimate(const pl2Character *chr, uint32_t frame)
{
    if(chr && chr->visible)
    {
        int i;
        for(i = 0; i < PL2_MAX_CHARPARTS; i++)
        {
            pl2ModelAnimate(chr->models[i], chr->sequence, frame);
        }
    }
}

void pl2ModelRender(const pl2Model *model)
{
    if(!model) return;

    glMatrixMode(GL_MODELVIEW);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glEnable(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);

    int i, j;

    for(i = 0; i < model->numObjects; i++)
    {
        pl2Object *obj = &(model->objects[i]);

        //DEBUGPRINT("%s: obj == %p, obj->glVertices == %p\n", __func__, obj, obj ? obj->glVertices : NULL);

        glPushMatrix();
        glMultMatrixf((GLfloat*)&(obj->transform));

        glInterleavedArrays(GL_T2F_N3F_V3F, 0, obj->glVertices);

        for (j = 0; j < obj->numMaterials; ++j)
        {
            pl2ObjMtl *m = &(obj->materials[j]);

            if (m->material && m->start >= 0)
            {
                pl2Material *mtl = m->material;

                //glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION,  (GLfloat*)&(mtl->emissive));
                //glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT,   (GLfloat*)&(mtl->ambient));
                //glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE,   (GLfloat*)&(mtl->diffuse));
                //glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR,  (GLfloat*)&(mtl->specular));
                glMaterialf (GL_FRONT_AND_BACK, GL_SHININESS, mtl->shininess);

                pl2Texture *tex = mtl->texture;

                if(tex)
                {
                    glTexImage2D(GL_TEXTURE_2D, 0, 4, tex->width, tex->height, 0,
                                 GL_RGBA, GL_UNSIGNED_BYTE, tex->pixels);
                }

                glDrawArrays(GL_TRIANGLES, m->start, m->count * 3);
            }
        }

        glPopMatrix();
    }
}

void pl2CharRender(const pl2Character *chr)
{
    if(chr && chr->visible)
    {
        int i;
        for(i = 0; i < PL2_MAX_CHARPARTS; i++)
        {
            pl2ModelRender(chr->models[i]);
        }
    }
}

/******************************************************************************/

static pl2Character chars[4] = { { { NULL }, NULL, 0, 0, 0 } };

static void pl2GlutIdleFunc()
{
    glutPostRedisplay();
}

static void pl2GlutDisplayFunc()
{
    const float t = glutGet(GLUT_ELAPSED_TIME) / 1000.0;
    const float angle = t*10.0f;

    int i;

    for(i = 0; i < 4; i++) pl2CharAnimate(&(chars[i]), 30 * t);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();

    glRotatef(angle, 0, 1, 0);

    glColor3f(1,1,1);
    //glutSolidCube(1);

    for(i = 0; i < 4; i++) pl2CharRender(&(chars[i]));

    glPopMatrix();

    glutSwapBuffers();
}

static void pl2GlutReshapeFunc(int w, int h)
{
    DEBUGPRINT("%s: window resize to %dx%d\n", __func__, w, h);

    if(!h) h++;

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0f, (float)w/(float)h, 1.0f, 1000.0f);
    gluLookAt(0,12,-20, 0,12,0, 0,1,0);
    glViewport(0, 0, w, h);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

static void pl2GlutKeyboardFunc(unsigned char k, int x, int y)
{
    if(k == 27) exit(0);
}

static void pl2GlutSpecialFunc(int k, int x, int y)
{
}

static void pl2GlutMouseFunc(int btn, int state, int x, int y)
{
    fprintf(stderr, "%s: Button %d %s @ <%d, %d>\n", __func__, btn, state ? "up" : "down", x, y);
}

static void pl2GlutMotionFunc(int x, int y)
{
    //fprintf(stderr, "%s: <%d, %d>\n", __func__, x, y);
}

int main(int argc, char *argv[])
{
    glutInitWindowSize(800, 600);
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);

    alutInit(&argc, argv);
    atexit(alutExit);

    glutCreateWindow("OPL2");

    glutIdleFunc(pl2GlutIdleFunc);
    glutDisplayFunc(pl2GlutDisplayFunc);
    glutReshapeFunc(pl2GlutReshapeFunc);
    glutKeyboardFunc(pl2GlutKeyboardFunc);
    glutSpecialFunc(pl2GlutSpecialFunc);
    glutMouseFunc(pl2GlutMouseFunc);
    glutMotionFunc(pl2GlutMotionFunc);

#if FREEGLUT
    glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);
#endif

    glClearColor(0, 0, 0, 1);

    glCullFace(GL_BACK);
    glEnable(GL_CULL_FACE);

    glDepthFunc(GL_LESS);
    glEnable(GL_DEPTH_TEST);

    glAlphaFunc(GL_GREATER, 0);
    glEnable(GL_ALPHA_TEST);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);

    glEnable(GL_LIGHTING);

    float light_pos[] = { 0, 12, -10, 0 };
    float light_amb[] = { 0.2f, 0.2f, 0.2f, 1 };
    float light_dif[] = { 0.8f, 0.8f, 0.8f, 1 };
    float light_spc[] = { 1.0f, 1.0f, 1.0f, 1 };

    glEnable(GL_LIGHT0);
    glLightfv(GL_LIGHT0, GL_POSITION, light_pos);
    glLightfv(GL_LIGHT0, GL_AMBIENT, light_amb);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_dif);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_spc);

    pl2PackageBuildIndex();
    atexit(pl2PackageClearIndex);

    int n = 0;
    chars[0].models[n++] = pl2ModelLoad("class01.tmb");
    //chars[0].models[n++] = pl2ModelLoad("beach01a.tmb");
    //chars[0].models[n++] = pl2ModelLoad("beach01b.tmb");
    chars[0].visible = 1;

    n = 0;
    chars[1].models[n++] = pl2ModelLoad("imo_bodyA_00.tmb");
    chars[1].models[n++] = pl2ModelLoad("imo_eye_00.tmb");
    chars[1].models[n++] = pl2ModelLoad("imo_hairA_00.tmb");
    chars[1].models[n++] = pl2ModelLoad("imo_cos_vocaloid_10A.tmb");
    chars[1].models[n++] = pl2ModelLoad("imo_cos_vocaloid_10B.tmb");
    chars[1].sequence = pl2SequenceLoad("event_01.tsb");
    chars[1].visible = 1;

    glutMainLoop();
    return 0;
}
