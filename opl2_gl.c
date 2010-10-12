#include "opl2.h"
#include "opl2_int.h"
#include "opl2_vm.h"

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

static SceUInt32 tick_res = 0;
static SceUInt64 first_tick = 0;

int glutGet(GLenum what)
{
    switch(what)
    {
        case GLUT_SCREEN_WIDTH:
        case GLUT_INIT_WINDOW_WIDTH:
            return pl2_screen_width;

        case GLUT_SCREEN_HEIGHT:
        case GLUT_INIT_WINDOW_HEIGHT:
            return pl2_screen_height;

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

void pl2LayerUpdate(pl2Layer *layer, float delta)
{
    if (layer)
    {
        layer->fade_time += delta;

        if (layer->fade_time < layer->fade_length)
        {
            layer->fade_level += delta *
                (layer->fade_target - layer->fade_level) /
                (layer->fade_length - layer->fade_time);

            if (layer->fade_level < 0.0f)
                layer->fade_level = 0.0f;
            else if (layer->fade_level > 1.0f)
                layer->fade_level = 1.0f;
        }
        else
        {
            layer->fade_level = layer->fade_target;
        }
    }
}

void pl2LayerDraw(pl2Layer *layer)
{
    if(layer)
    {
        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        glLoadIdentity();
        glOrtho(0, 1, 1, 0, 0, 0);
        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glLoadIdentity();

        glDisable(GL_TEXTURE_2D);
        glDisable(GL_DEPTH_TEST);
        //glDisable(GL_CULL_FACE);

        glDisable(GL_LIGHTING);

        glColor4f(0.0f, 0.0f, 0.0f, 1.0f - layer->fade_level);
        //float mtl[] = { 0.0f, 0.0f, 0.0f, 1.0f - layer->fade_level };
        //glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, (GLfloat*)mtl);

        glBegin(GL_TRIANGLE_FAN);
            glVertex2f(0, 0);
            glVertex2f(0, 1);
            glVertex2f(1, 1);
            glVertex2f(1, 0);
        glEnd();

        glEnable(GL_LIGHTING);

        glMatrixMode(GL_PROJECTION);
        glPopMatrix();
        glMatrixMode(GL_MODELVIEW);
        glPopMatrix();
    }
}

/******************************************************************************/

void pl2LightConfig(pl2Light *light, int id)
{
    if(light)
    {
        if(light->enabled)
        {
            glLightfv(GL_LIGHT0 + id, GL_POSITION, (GLfloat*)&(light->position));
            glLightfv(GL_LIGHT0 + id, GL_AMBIENT, (GLfloat*)&(light->ambient));
            glLightfv(GL_LIGHT0 + id, GL_DIFFUSE, (GLfloat*)&(light->diffuse));
            glLightfv(GL_LIGHT0 + id, GL_SPECULAR, (GLfloat*)&(light->specular));
            glEnable(GL_LIGHT0 + id);
        }
        else
        {
            glDisable(GL_LIGHT0 + id);
        }
    }
}

/******************************************************************************/

void pl2CameraConfig(pl2Camera *cam)
{
    if(cam)
    {
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glViewport(0, 0, pl2_screen_width, pl2_screen_height);
        gluPerspective(cam->fov, pl2_screen_aspect, 1.0f, 1000.0f);

        //DEBUGPRINT("%s: screen == %dx%d (%g:1)\n", __func__,
        //           pl2_screen_width, pl2_screen_height, pl2_screen_aspect);

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        gluLookAt(cam->eye  .x, cam->eye  .y, cam->eye  .z,
                  cam->focus.x, cam->focus.y, cam->focus.z,
                  cam->up   .x, cam->up   .y, cam->up   .z);

        //DEBUGPRINT("%s: camera == <%g,%g,%g> <%g,%g,%g> <%g,%g,%g> %g\n", __func__,
        //           cam->eye.x, cam->eye.y, cam->eye.z,
        //           cam->focus.x, cam->focus.y, cam->focus.z,
        //           cam->up.x, cam->up.y, cam->up.z,
        //           cam->fov);
    }
}

void pl2CameraUpdate(pl2Camera *cam, float dt)
{
    if(cam && cam->path)
    {
        cam->time += dt;

        int frame = (int)(cam->time * 30.0f);

        if(frame >= cam->path->numFrames)
        {
            if(cam->loop)
            {
                frame %= (cam->path->numFrames);
            }
            else
            {
                DEBUGPRINT("%s: end of path (frame == %d, numFrames == %d, dt == %g)\n", __func__, frame, cam->path->numFrames, dt);

                frame = cam->path->numFrames - 1;
                cam->eye = cam->path->frames[frame].eye;
                cam->focus = cam->path->frames[frame].focus;
                cam->fov = cam->path->frames[frame].fov;

                pl2CameraPathFree(cam->path);
                cam->path = NULL;
                return;
            }
        }

        //DEBUGPRINT("%s: frame %d\n", __func__, frame);

        cam->eye = cam->path->frames[frame].eye;
        cam->focus = cam->path->frames[frame].focus;
        cam->fov = cam->path->frames[frame].fov;
    }
}

void pl2CameraRotate1P(pl2Camera *cam, const fvector3_t *rotate)
{
   pl2VectorOrbit(&cam->focus, &cam->eye, &cam->up, rotate);
}

void pl2CameraRotate3P(pl2Camera *cam, const fvector3_t *rotate)
{
   pl2VectorOrbit(&cam->eye, &cam->focus, &cam->up, rotate);
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

    fmatrix4_t bones[numBones];

    int i, j;

    fmatrix4_t *mdlBones = model->bones, *seqBones = anim->bones + (anim->numBones * frame);

    for(i = 0; i < numBones; i++)
    {
        pl2MultMatrix4f(&(bones[i]), &(seqBones[i]), &(mdlBones[i]));
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

        if(chr->visible)
        {
            int frame = 30 * chr->time;
            int count = chr->anim->numFrames;
            int loop  = chr->anim->loopFrame;

            if(frame >= count)
            {
                frame = loop + (frame - loop) % (count - loop);
            }

            int i;
            for(i = 0; i < PL2_MAX_CHARPARTS; i++)
            {
                pl2ModelAnimate(chr->models[i], chr->anim, frame);
            }

            chr->frame = frame;
        }
    }
}

/******************************************************************************/

void pl2ModelRender(const pl2Model *model)
{
    if(!model) return;

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glEnable(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);

    glMatrixMode(GL_MODELVIEW);

    int i, j;

    for(i = 0; i < model->numObjects; i++)
    {
        pl2Object *obj = &(model->objects[i]);

        //DEBUGPRINT("%s: obj == %p, obj->glVertices == %p\n", __func__, obj, obj ? obj->glVertices : NULL);

        glPushMatrix();
        //glMultMatrixf((GLfloat*)&(obj->transform));

        glInterleavedArrays(GL_T2F_N3F_V3F, 0, obj->glVertices);

        for (j = 0; j < obj->numMaterials; ++j)
        {
            pl2ObjMtl *m = &(obj->materials[j]);

            if (m->material && m->start >= 0)
            {
                pl2Material *mtl = m->material;

                glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION,  (GLfloat*)&(mtl->emissive));
                glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT,   (GLfloat*)&(mtl->ambient));
                glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE,   (GLfloat*)&(mtl->diffuse));
                glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR,  (GLfloat*)&(mtl->specular));
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

static void pl2GlutIdleFunc()
{
    glutPostRedisplay();
}

static void pl2GlutDisplayFunc()
{
    static float last_time = 0;
    if(last_time == 0) last_time = glutGet(GLUT_ELAPSED_TIME) * 0.001f;

    const float t = glutGet(GLUT_ELAPSED_TIME) * 0.001f;
    const float dt = t - last_time;
    last_time = t;

    int i;

    for(i = 0; i < PL2_MAX_LAYERS; i++)
    {
        pl2LayerUpdate(&(pl2_layers[i]), dt);
    }
    for(i = 0; i < PL2_MAX_CAMERAS; i++)
    {
        pl2CameraUpdate(&(pl2_cameras[i]), dt);
    }
    for(i = 0; i < PL2_MAX_CHARS; i++)
    {
        pl2CharAnimate(&(pl2_chars[i]), dt);
    }

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    for(i = 0; i < PL2_MAX_LIGHTS; i++)
    {
        pl2LightConfig(&(pl2_lights[i]), i);
    }
    pl2CameraConfig(&(pl2_cameras[0]));

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();

    glColor3f(1,1,1);
    for(i = 0; i < PL2_MAX_CHARS; i++)
    {
        pl2CharRender(&(pl2_chars[i]));
    }

    glPopMatrix();

    pl2LayerDraw(&(pl2_layers[0]));

    glutSwapBuffers();
}

static void pl2GlutReshapeFunc(int w, int h)
{
    pl2_screen_width = w;
    pl2_screen_height = h;

    float f1 = (float)w / 800.f;
    float f2 = (float)h / 600.f;

    pl2_screen_scale = (f1 < f2) ? f1 : f2;

    if(!h) h++;
    pl2_screen_aspect = (float)w/(float)h;

    DEBUGPRINT("%s: window resize to %dx%d (aspect == %g, scale == %g)\n", __func__,
               pl2_screen_width, pl2_screen_height, pl2_screen_aspect, pl2_screen_scale);

    //glMatrixMode(GL_PROJECTION);
    //glLoadIdentity();
    glViewport(0, 0, w, h);
    //gluPerspective(45, pl2_screen_aspect, 1.0f, 1000.0f);

    //glMatrixMode(GL_MODELVIEW);
    //glLoadIdentity();
}

static void pl2GlutKeyboardFunc(unsigned char k, int x, int y)
{
#if !_PSP_FW_VERSION
    if(k == 13)
    {
        glutFullScreen();
        glutSetCursor(GLUT_CURSOR_NONE);
        //static int gm = 0;
        //gm = !gm;
        //if(gm) glutEnterGameMode();
        //else   glutLeaveGameMode();
    }
    if(k == 27) exit(0);
#endif
}

static void pl2GlutSpecialFunc(int k, int x, int y)
{
#if _PSP_FW_VERSION
    if(k == GLUT_KEY_HOME) exit(0);
#endif
}

enum
{
   MOVE_NONE, MOVE_3P, MOVE_1P, MOVE_ORTHO,
};

static int move_mode = 0, mouse_x = -1, mouse_y = -1;

static void pl2GlutMouseFunc(int button, int state, int x, int y)
{
   switch (state)
   {
      case GLUT_DOWN:
         switch (button)
         {
            case GLUT_LEFT_BUTTON:
               move_mode |= MOVE_3P;
               break;
            case GLUT_RIGHT_BUTTON:
               move_mode |= MOVE_1P;
               break;
         }
         break;

      case GLUT_UP:
         switch (button)
         {
            case GLUT_LEFT_BUTTON:
               move_mode &= ~MOVE_3P;
               break;
            case GLUT_RIGHT_BUTTON:
               move_mode &= ~MOVE_1P;
               break;
         }
         break;
   }

   mouse_x = x; mouse_y = y;
}

#include <math.h>

static void pl2GlutMotionFunc(int x, int y)
{
   int dx = x - mouse_x, dy = y - mouse_y;

   float x_angle = 2.0f * M_PI * (float)dx / (float)pl2_screen_width;
   float y_angle = 2.0f * M_PI * (float)dy / (float)pl2_screen_height;

   fvector3_t rotate = { y_angle, x_angle, 0 };

   switch (move_mode)
   {
      case MOVE_3P:
         //printf("left dragging @ (%4d, %4d) [x_angle:%6.3f y_angle:%6.3f]\n", x, y, x_angle, y_angle);
         pl2CameraRotate3P(&(pl2_cameras[0]), &rotate);
         break;

      case MOVE_1P:
         pl2CameraRotate1P(&(pl2_cameras[0]), &rotate);
         break;

      case MOVE_ORTHO:
         pl2CameraZoom(&(pl2_cameras[0]), -5.0f * (float)dy / (float)pl2_screen_height);
         break;
   }

   mouse_x = x; mouse_y = y;
}

int pl2GlInit(int *argc, char *argv[])
{
    glutInitWindowSize(800, 600);
    glutInit(argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);

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

    //glAlphaFunc(GL_GREATER, 0);
    //glEnable(GL_ALPHA_TEST);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);

    glEnable(GL_LIGHTING);

    //float light_pos[] = { 0, 12, -10, 0 };
    //float light_amb[] = { 0.2f, 0.2f, 0.2f, 1 };
    //float light_dif[] = { 0.8f, 0.8f, 0.8f, 1 };
    //float light_spc[] = { 1.0f, 1.0f, 1.0f, 1 };

    //glEnable(GL_LIGHT0);
    //glLightfv(GL_LIGHT0, GL_POSITION, light_pos);
    //glLightfv(GL_LIGHT0, GL_AMBIENT, light_amb);
    //glLightfv(GL_LIGHT0, GL_DIFFUSE, light_dif);
    //glLightfv(GL_LIGHT0, GL_SPECULAR, light_spc);

    return 1;
}
