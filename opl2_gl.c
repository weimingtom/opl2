//#include "opl2.h"
#include "opl2_int.h"
#include "opl2_vm.h"

#include <GL/gl.h>
#include <GL/glu.h>
//#include <GL/glut.h>
#include <GL/freeglut.h>

/* use freeglut extensions, if available */
//#if FREEGLUT
//# include <GL/freeglut_ext.h>
//#endif

#include <math.h>

// Reference:
// http://www.opengl.org/sdk/docs/man/

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
            return 1000 * (this_tick - first_tick) / (tick_res);
        }
    }
    return -1;
}

#endif

void pl2GlClearErrors()
{
    int err;
    while(GL_NO_ERROR != (err = glGetError()));
}

#define pl2GlPrintErrors() _pl2GlPrintErrors(__func__, __LINE__)
void _pl2GlPrintErrors(const char *func, int line)
{
#define GLERRORCASE(e) case e: DEBUGPRINT("%s(%d): %s\n", func, line, #e); break;
    int err;
    while(GL_NO_ERROR != (err = glGetError()))
    {
        switch(err)
        {
            GLERRORCASE(GL_NO_ERROR);
            GLERRORCASE(GL_INVALID_ENUM);
            GLERRORCASE(GL_INVALID_VALUE);
            GLERRORCASE(GL_INVALID_OPERATION);
            GLERRORCASE(GL_STACK_OVERFLOW);
            GLERRORCASE(GL_STACK_UNDERFLOW);
            GLERRORCASE(GL_OUT_OF_MEMORY);
            //GLERRORCASE(GL_TABLE_TOO_LARGE);

            default:
                DEBUGPRINT("%s: %s\n", __func__, "unknown GL error");
                break;
        }
    }
}

static int pl2_gl_2d = 0;

void pl2GlBegin2D()
{
    if(!pl2_gl_2d)
    {
        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        glLoadIdentity();
        glOrtho(0, pl2_screen_width, pl2_screen_height, 0, -1, 1);

        //glOrtho(0, pl2_screen_aspect * PL2_NOMINAL_SCREEN_HEIGHT, PL2_NOMINAL_SCREEN_HEIGHT, 0, -1, 1);
        //glTranslatef(0.5f * pl2_screen_aspect * (float)PL2_NOMINAL_SCREEN_HEIGHT, 0, 0);
        //glScalef(pl2_screen_scale, pl2_screen_scale, 1);

        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glLoadIdentity();
        //glTranslatef(0.5f * pl2_screen_aspect * (float)PL2_NOMINAL_SCREEN_HEIGHT, 0, 0);
        //glScalef(pl2_screen_scale, pl2_screen_scale, 1);

        glDisable(GL_DEPTH_TEST);
        glDisable(GL_LIGHTING);

        pl2_gl_2d = 1;
    }
}

void pl2GlEnd2D()
{
    if(pl2_gl_2d)
    {
        glMatrixMode(GL_PROJECTION);
        glPopMatrix();
        glMatrixMode(GL_MODELVIEW);
        glPopMatrix();

        pl2_gl_2d = 0;
    }
}

void pl2GlTransform(fvector3_t *translate, fvector3_t *rotate, fvector3_t *scale)
{
#if 0
    float sx = sinf(rotate->x), cx = cosf(rotate->x);
    float sy = sinf(rotate->y), cy = cosf(rotate->y);
    float sz = sinf(rotate->z), cz = cosf(rotate->z);
    //sincosf(rotate->x, &sx, &cx);
    //sincosf(rotate->y, &sy, &cy);
    //sincosf(rotate->z, &sz, &cz);

    fmatrix4_t m = {
        { cy*cz, cy*sz, -sy, 0 },
        { -cx*sz+sx*sy*cz, cx*cz+sx*sy*sz, sx*sy, 0 },
        { sx*sz+cx+sy+cz, -sx*cz+cx*sy*sz, cx*cy, 0 },
        { translate->x, translate->y, translate->z, 1 },
    };
    glMultMatrixf((GLfloat*)&m);
#else
    glTranslatef(translate->x, translate->y, translate->z);
    glRotatef(DEG(rotate->x), 1, 0, 0);
    glRotatef(DEG(rotate->y), 0, 1, 0);
    glRotatef(DEG(rotate->z), 0, 0, 1);
#endif
}

void pl2GlTransformInv(fvector3_t *translate, fvector3_t *rotate, fvector3_t *scale)
{
#if 0
    float sx = sinf(rotate->x), cx = cosf(rotate->x);
    float sy = sinf(rotate->y), cy = cosf(rotate->y);
    float sz = sinf(rotate->z), cz = cosf(rotate->z);
    //sincosf(-rotate->x, &sx, &cx);
    //sincosf(-rotate->y, &sy, &cy);
    //sincosf(-rotate->z, &sz, &cz);

    fmatrix4_t m = {
        { cy*cz, cy*sz, -sy, 0 },
        { -cx*sz+sx*sy*cz, cx*cz+sx*sy*sz, sx*sy, 0 },
        { sx*sz+cx+sy+cz, -sx*cz+cx*sy*sz, cx*cy, 0 },
        { -translate->x, -translate->y, -translate->z, 1 },
    };
    glMultMatrixf((GLfloat*)&m);
#else
    glRotatef(-DEG(rotate->x), 1, 0, 0);
    glRotatef(-DEG(rotate->y), 0, 1, 0);
    glRotatef(-DEG(rotate->z), 0, 0, 1);
    glTranslatef(-translate->x, -translate->y, -translate->z);
#endif
}

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

            //DEBUGPRINT("%s: fade_level == %.3g\n", __func__, layer->fade_level);
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
        glColor4f(0.0f, 0.0f, 0.0f, 1.0f - layer->fade_level);

        const struct { float x, y, z; } rect[4] = {
            { 0,                0,                 0 },
            { 0,                pl2_screen_height, 0 },
            { pl2_screen_width, pl2_screen_height, 0 },
            { pl2_screen_width, 0,                 0 }
        };
        glInterleavedArrays(GL_V3F, 0, rect);
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    }
}

/******************************************************************************/

void pl2ImageDraw(pl2Image *image, int x, int y, int cx, int cy, float alpha)
{
    if(image)
    {
        glColor4f(1.0f, 1.0f, 1.0f, alpha);

        int w = image->width, h = image->height;
        const struct { float x, y, z; } rect[4] = {
            { x - cx,     y - cy,     0 },
            { x - cx,     y - cy + h, 0 },
            { x - cx + w, y - cy + h, 0 },
            { x - cx + w, y - cy,     0 }
        };

        glTexImage2D(GL_TEXTURE_2D, 0, 4, image->width, image->height,
                     0, GL_RGBA, GL_UNSIGNED_BYTE, image->data);

        glInterleavedArrays(GL_V3F, 0, rect);
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    }
}

/******************************************************************************/

void pl2FontPrintInternal(pl2Font *font, float x, float y, uint32_t color, const uint32_t *text, size_t len)
{
    if(font && text)
    {
        glColor4f(((color >>  0) & 255) / 255.0f,
                  ((color >>  8) & 255) / 255.0f,
                  ((color >> 16) & 255) / 255.0f,
                  ((color >> 24) & 255) / 255.0f);

        int w = font->glyphSize, h = font->glyphSize;

        int left = x;

        //int len = pl2Ucs4Strlen(text);

        int i, j;

        for(i = 0; i < len; i++)
        {
            if(text[i] == '\n')
            {
                y += h;
                x = left;
            }
            else if(text[i] >= 32)
            {
                for(j = 0; j < font->numGlyphs; j++)
                {
                    if(font->chars[j] == text[i]) break;
                }

                if(j < font->numGlyphs)
                {
                    //pl2GlClearErrors();
                    glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, w, h, 0, GL_ALPHA,
                                 GL_UNSIGNED_BYTE, font->glyphs + w * h * j);
                    //pl2GlPrintErrors();

                    const struct { float u, v, x, y, z; } rect[4] = {
                        { 0, 0, x,     y,     0 },
                        { 0, 1, x,     y + h, 0 },
                        { 1, 1, x + w, y + h, 0 },
                        { 1, 0, x + w, y,     0 }
                    };

                    glInterleavedArrays(GL_T2F_V3F, 0, rect);
                    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
                }

                x += w;
            }
        }
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
            glLightfv(GL_LIGHT0 + id, GL_AMBIENT,  (GLfloat*)&(light->ambient ));
            glLightfv(GL_LIGHT0 + id, GL_DIFFUSE,  (GLfloat*)&(light->diffuse ));
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
        //glViewport(0, 0, pl2_screen_width, pl2_screen_height);
        gluPerspective(cam->fov, pl2_screen_aspect, 1.0f, 1000.0f);

        //DEBUGPRINT("%s: screen == %dx%d (%g:1)\n", __func__,
        //           pl2_screen_width, pl2_screen_height, pl2_screen_aspect);

        //fvector3_t e = cam->eye, f = cam->focus, u = cam->up;

        if(cam->point)
        {
            //pl2VectorAdd3f(&e, &e, &(cam->point->translate));
            //pl2VectorAdd3f(&f, &f, &(cam->point->translate));
        }

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        gluLookAt(cam->eye.x, cam->eye.y, cam->eye.z,
                  cam->focus.x, cam->focus.y, cam->focus.z,
                  cam->up.x, cam->up.y, cam->up.z);

        if(cam->point)
        {
            pl2Point *point = cam->point;
            pl2GlTransformInv(&(point->translate), &(point->rotate), &(point->scale));
        }


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

        cam->eye   = cam->path->frames[frame].eye;
        cam->focus = cam->path->frames[frame].focus;
        cam->fov   = cam->path->frames[frame].fov;
    }
}

/******************************************************************************/

void pl2ModelRender(const pl2Model *model, float alpha)
{
    if(!model) return;

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glEnable(GL_TEXTURE_2D);
    //glEnable(GL_LIGHTING);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    //glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);

    //glMatrixMode(GL_MODELVIEW);

    int i, j;

    for(i = 0; i < model->numObjects; i++)
    {
        pl2Object *obj = &(model->objects[i]);

        //DEBUGPRINT("%s: obj == %p, obj->glVertices == %p\n", __func__, obj, obj ? obj->glVertices : NULL);

        //glPushMatrix();
        //glMultMatrixf((GLfloat*)&(obj->transform));

        glInterleavedArrays(GL_T2F_N3F_V3F, sizeof(pl2GlVertex), obj->glVertices);

        for (j = 0; j < obj->numMaterials; ++j)
        {
            pl2ObjMtl *m = &(obj->materials[j]);

            if (m->material && m->start >= 0)
            {
                pl2Material *mtl = m->material;

                float diffuse[4] = {
                    mtl->diffuse.r,
                    mtl->diffuse.g,
                    mtl->diffuse.b,
                    mtl->diffuse.a * alpha
                };

                glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, (GLfloat*)&(mtl->emissive));
                glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT,  (GLfloat*)&(mtl->ambient));
                glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE,  diffuse);
                glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, (GLfloat*)&(mtl->specular));
                glMaterialf (GL_FRONT_AND_BACK, GL_SHININESS, mtl->shininess);

                pl2Texture *tex = mtl->texture;

                if(tex)
                {
                    glTexImage2D(GL_TEXTURE_2D, 0, 4, tex->width, tex->height,
                                 0, GL_RGBA, GL_UNSIGNED_BYTE, tex->pixels);
                }

                glDrawArrays(GL_TRIANGLES, m->start, m->count * 3);
            }
        }

        //glPopMatrix();
    }
}

void pl2CharRender(pl2Character *chr)
{
    if(chr && (chr->visible > 0))
    {
        int i;

        glColor4f(1, 1, 1, chr->visible);
        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();

        if(chr->point)
        {
            /*
            DEBUGPRINT("%s: T<%g,%g,%g> R<%g,%g,%g>\n", __func__,
                       chr->point->translate.x,
                       chr->point->translate.y,
                       chr->point->translate.z,
                       chr->point->rotate.x,
                       chr->point->rotate.y,
                       chr->point->rotate.z);
            */

            pl2Point *point = chr->point;
            pl2GlTransform(&(point->translate), &(point->rotate), &(point->scale));
        }

        for(i = 0; i < PL2_MAX_CHARPARTS; i++)
        {
            pl2ModelRender(chr->models[i], chr->visible);
        }

        glMatrixMode(GL_MODELVIEW);
        glPopMatrix();
    }
}

/******************************************************************************/

static void pl2GlRenderFrame(float dt)
{
    int i;

    for(i = 0; i < PL2_MAX_LAYERS; i++)
    {
        pl2LayerUpdate(&(pl2_layers[i]), dt);
    }
    for(i = 0; i < PL2_MAX_CAMERAS; i++)
    {
        pl2CameraUpdate(&(pl2_cameras[i]), dt);
    }
#if !_PSP_FW_VERSION
    for(i = 0; i < PL2_MAX_CHARS; i++)
    {
        pl2CharAnimate(&(pl2_chars[i]), dt);
    }
#endif

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    pl2CameraConfig(&(pl2_cameras[0]));

    for(i = 0; i < PL2_MAX_LIGHTS; i++)
    {
        pl2LightConfig(&(pl2_lights[i]), i);
    }

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();

    glColor3f(1,1,1);
    for(i = 0; i < PL2_MAX_CHARS; i++)
    {
        pl2CharRender(&(pl2_chars[i]));
    }

    glPopMatrix();

    pl2GlBegin2D();

    pl2LayerDraw(&(pl2_layers[0]));

    glPushMatrix();
    glScalef(pl2_screen_scale, pl2_screen_scale, 1);

    //DEBUGPRINT("%s: half_screen_width == %d\n", __func__, pl2_screen_width >> 1);

    // TODO: draw 2d overlay
    if(pl2_font && !pl2_hide_overlay)
    {
        if(pl2_menu_showing)
        {
            int x = (int)(0.5f * pl2_screen_aspect * (float)PL2_NOMINAL_SCREEN_HEIGHT);
            int y = (int)(0.5f * (float)(PL2_NOMINAL_SCREEN_HEIGHT - (2 * pl2_menu.numItems - 1) * pl2_font->glyphSize));

            for(i = 0; i < pl2_menu.numItems; i++)
            {
                uint32_t color = pl2_menu.items[i].enabled ? 0xff000000ul : 0x80000000ul;
                color |= (i == pl2_menu.selection) ? 0xfffffful : 0x000000ul;
                pl2FontUcsPrintCenter(pl2_font, x, y, color, pl2_menu.items[i].text);
                y += 2 * pl2_font->glyphSize;
            }
        }

        else if(pl2_text_showing)
        {
            int x = 0; //(int)(0.5f * pl2_screen_aspect * (float)PL2_NOMINAL_SCREEN_HEIGHT);
            int y = (int)(0.8f * (float)PL2_NOMINAL_SCREEN_HEIGHT);
            pl2FontUcsPrint(pl2_font, x, y, 0xfffffffful, pl2_text);
        }
    }

    glPopMatrix();

    pl2LayerDraw(&(pl2_layers[1]));

    pl2GlEnd2D();
}

/******************************************************************************/

void pl2GlInit()
{
    glClearColor(0, 0, 0, 1);

    glCullFace(GL_BACK);
    glEnable(GL_CULL_FACE);

    glDepthFunc(GL_LESS);
    glEnable(GL_DEPTH_TEST);

    glAlphaFunc(GL_GREATER, 0);
    glEnable(GL_ALPHA_TEST);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);

    //glEnable(GL_LIGHTING);

    //glSampleCoverage(0.5f, 0);

    //float light_pos[] = { 0, 12, -10, 0 };
    //float light_amb[] = { 0.2f, 0.2f, 0.2f, 1 };
    //float light_dif[] = { 0.8f, 0.8f, 0.8f, 1 };
    //float light_spc[] = { 1.0f, 1.0f, 1.0f, 1 };

    //glEnable(GL_LIGHT0);
    //glLightfv(GL_LIGHT0, GL_POSITION, light_pos);
    //glLightfv(GL_LIGHT0, GL_AMBIENT, light_amb);
    //glLightfv(GL_LIGHT0, GL_DIFFUSE, light_dif);
    //glLightfv(GL_LIGHT0, GL_SPECULAR, light_spc);
}

/******************************************************************************/

#if WITH_GLUT

static int pl2_init_screen_width  = PL2_NOMINAL_SCREEN_WIDTH;
static int pl2_init_screen_height = PL2_NOMINAL_SCREEN_HEIGHT;

static void pl2GlutIdleFunc()
{
    glutReshapeWindow(pl2_init_screen_width, pl2_init_screen_height);
    glutPostRedisplay();
}

static void pl2GlutDisplayFunc()
{
    static uint32_t last_time = 0;
    if(last_time == 0) last_time = glutGet(GLUT_ELAPSED_TIME);

    const uint32_t this_time = glutGet(GLUT_ELAPSED_TIME);

    const float dt = (float)(this_time - last_time)*0.001f;

    last_time = this_time;

    pl2GlRenderFrame(dt);

    glutSwapBuffers();
}

static void pl2GlutReshapeFunc(int w, int h)
{
    if(0 == pl2_init_screen_width)
        pl2_init_screen_width = w;
    if(0 == pl2_init_screen_height)
        pl2_init_screen_height = h;

    pl2_screen_width  = w;
    pl2_screen_height = h;

    float f1 = (float)w / (float)(PL2_NOMINAL_SCREEN_WIDTH);
    float f2 = (float)h / (float)(PL2_NOMINAL_SCREEN_HEIGHT);

    pl2_screen_scale = (f1 < f2) ? f1 : f2;

    if(!h) h++;
    pl2_screen_aspect = (float)w/(float)h;

    DEBUGPRINT("%s: window resize to %dx%d (aspect == %g, scale == %g)\n", __func__,
               w, h, pl2_screen_aspect, pl2_screen_scale);

    //glMatrixMode(GL_PROJECTION);
    //glLoadIdentity();
    glViewport(0, 0, w, h);
    //gluPerspective(45, pl2_screen_aspect, 1.0f, 1000.0f);

    //glMatrixMode(GL_MODELVIEW);
    //glLoadIdentity();
}

#if _PSP_FW_VERSION

#include <pspctrl.h>

static void pl2GlutJoystickFunc(unsigned int buttons, int x, int y, int z)
{
    if((x * x + y * y) >= 10000)
    {
        float x_angle = 2.0f * M_PI * (float)x / 1000.0f;
        float y_angle = 2.0f * M_PI * (float)y / 1000.0f;

        if(buttons & PSP_CTRL_SQUARE)
        {
            pl2CameraRotate1P(&(pl2_cameras[0]), x_angle, y_angle);
        }
        else
        {
            pl2CameraRotate3P(&(pl2_cameras[0]), x_angle, y_angle);
        }
    }

    //if(buttons & PSP_CTRL_START) pl2Exit();
}

#else // !_PSP_FW_VERSION

static void pl2GlutKeyboardFunc(unsigned char k, int x, int y)
{
    if(k == 13)
    {
        pl2TextAdvance();
        pl2MenuConfirm(&pl2_menu);
        //glutFullScreen();
        //glutSetCursor(GLUT_CURSOR_NONE);

        /*
        static int gm = 0;
        if(!gm)
        {
            gm = 1;
            glutEnterGameMode();
        }
        else
        {
            gm = 0;
            glutLeaveGameMode();
        }
        */
    }
    //if(k == 'q') pl2Exit();
}

static void pl2GlutSpecialFunc(int k, int x, int y)
{
    switch(k)
    {
        case GLUT_KEY_UP:
            pl2MenuSelectPrev(&pl2_menu);
            break;
        case GLUT_KEY_DOWN:
            pl2MenuSelectNext(&pl2_menu);
            break;
    }
}

enum
{
   MOVE_NONE  = 0,
   MOVE_3P    = 1,
   MOVE_ORTHO = 2,
   MOVE_1P    = 3,
};

static int move_mode = 0, mouse_x = -1, mouse_y = -1;

static void pl2GlutMouseFunc(int button, int state, int x, int y)
{
    DEBUGPRINT("%s: button %d %s\n", __func__, button, (state == GLUT_DOWN) ? "pressed" : "released");

    switch (state)
    {
        case GLUT_DOWN:
            switch (button)
            {
                case GLUT_LEFT_BUTTON:
                    move_mode |= MOVE_3P;
                    break;
                case GLUT_RIGHT_BUTTON:
                    move_mode |= MOVE_ORTHO;
                    break;
                //case GLUT_MIDDLE_BUTTON:
                //   move_mode |= MOVE_ORTHO;
                //   break;
            }
            break;

        case GLUT_UP:
            switch (button)
            {
                case GLUT_LEFT_BUTTON:
                    move_mode &= ~MOVE_3P;
                    break;
                case GLUT_RIGHT_BUTTON:
                    move_mode &= ~MOVE_ORTHO;
                    break;
                //case GLUT_MIDDLE_BUTTON:
                //   move_mode &= ~MOVE_ORTHO;
                //   break;
            }
            break;
    }

    glutSetCursor(move_mode ? GLUT_CURSOR_NONE : GLUT_CURSOR_INHERIT);
    mouse_x = x; mouse_y = y;
}

static void pl2GlutMotionFunc(int x, int y)
{
   int dx = x - mouse_x, dy = y - mouse_y;

    static int warp = 0;

    if(warp)
        warp = 0;
    else
    {
       float x_angle = 2.0f * M_PI * (float)dx / (float)pl2_screen_width;
       float y_angle = 2.0f * M_PI * (float)dy / (float)pl2_screen_height;

       //fvector3_t rotate = { y_angle, x_angle, 0 };

       switch (move_mode)
       {
          case MOVE_3P:
             //printf("left dragging @ (%4d, %4d) [x_angle:%6.3f y_angle:%6.3f]\n", x, y, x_angle, y_angle);
             pl2CameraRotate3P(&(pl2_cameras[0]), x_angle, y_angle);
             break;

          case MOVE_1P:
             pl2CameraRotate1P(&(pl2_cameras[0]), x_angle, y_angle);
             break;

          case MOVE_ORTHO:
             pl2CameraZoom(&(pl2_cameras[0]), -10.0f * (float)dy / (float)pl2_screen_height);
             break;
       }

       //mouse_x = x; mouse_y = y;
       mouse_x = pl2_screen_width >> 1; mouse_y = pl2_screen_height >> 1;

       glutWarpPointer(pl2_screen_width >> 1, pl2_screen_height >> 1);
       warp = 1;
   }
   glutPostRedisplay();
}

static void pl2GlutPassiveMotionFunc(int x, int y)
{
}

static void pl2GlutMouseWheelFunc(int btn, int dir, int x, int y)
{
}

static struct { int width, height, bpp; }
pl2_displayModes[] =
{
    {   -1,   -1, 32 }, // reserved for user-specified mode
    { 1600, 1200, 32 }, { 1600, 1200, 16 }, //{ 1600, 1200, 0 },
    { 1400, 1050, 32 }, { 1400, 1050, 16 }, //{ 1400, 1050, 0 },
    { 1280, 1024, 32 }, { 1280, 1024, 16 }, //{ 1280, 1024, 0 },
    { 1280,  800, 32 }, { 1280,  800, 16 }, //{ 1280,  800, 0 },
    { 1280,  768, 32 }, { 1280,  768, 16 }, //{ 1280,  768, 0 },
    { 1024,  768, 32 }, { 1024,  768, 16 }, //{ 1024,  768, 0 },
    { 1024,  600, 32 }, { 1024,  600, 16 }, //{ 1024,  600, 0 },
    {  800,  600, 32 }, {  800,  600, 16 }, //{  800,  600, 0 },
    {  640,  480, 32 }, {  640,  480, 16 }, //{  640,  480, 0 },
    {  640,  360, 32 }, {  640,  360, 16 }, //{  640,  360, 0 },
    {  480,  272, 32 }, {  480,  272, 16 }, //{  480,  272, 0 },
    {  320,  240, 32 }, {  320,  240, 16 }, //{  320,  240, 0 },
    {    0,    0,  0 },
};

static int pl2GlutTryGameMode()
{
    int i;
    for(i = 0; pl2_displayModes[i].width && pl2_displayModes[i].height; i++)
    {
        if((pl2_displayModes[i].width < 0) || (pl2_displayModes[i].height < 0))
            continue;

        char mode[32];
        snprintf(mode, sizeof(mode),
                 pl2_displayModes[i].bpp ? "%dx%d:%d" : "%dx%d",
                 pl2_displayModes[i].width,
                 pl2_displayModes[i].height,
                 pl2_displayModes[i].bpp);

        DEBUGPRINT("%s: trying fullscreen mode %s\n", __func__, mode);

        glutGameModeString(mode);

        if(glutGameModeGet(GLUT_GAME_MODE_POSSIBLE) && glutEnterGameMode())
        {
            atexit(glutLeaveGameMode);
            DEBUGPRINT("%s: using fullscreen mode %s\n", __func__, mode);
            return 1;
        }
    }

    return 0;
}

#endif // !_PSP_FW_VERSION

/******************************************************************************/

int pl2GlutDoFrame()
{
    glutMainLoopEvent();
    glutPostRedisplay();
    return pl2_running;
}

int pl2GlutInit(int *argc, char *argv[])
{
    //int init_width = PL2_NOMINAL_SCREEN_WIDTH, init_height = PL2_NOMINAL_SCREEN_HEIGHT;

#if !_PSP_FW_VERSION
    int windowed = 0;

    int i;
    for(i = 1; i < *argc; i++)
    {
        DEBUGPRINT("%s: argv[%d] == \"%s\"\n", __func__, i, argv[i]);

        if(!strcmp(argv[i], "-window"))
        {
            argv[i] = "-geometry";
            windowed = 1;

            /*
            if(((i + 1) < *argc) && (argv[i+1][0] != '-'))
            {
                i++;
                int r = sscanf(argv[i], "%dx%d", &init_width, &init_height);

                DEBUGPRINT("%s: sscanf returned %d\n", __func__, r);
            }
            */
        }
        else if(!strcmp(argv[i], "-fullscreen"))
        {
            windowed = 0;

            if(((i + 1) < *argc) && (argv[i+1][0] != '-'))
            {
                i++;
                //int r =
                sscanf(argv[i], "%dx%d:%d",
                       &pl2_displayModes[0].width,
                       &pl2_displayModes[0].height,
                       &pl2_displayModes[0].bpp);

                //DEBUGPRINT("%s: sscanf returned %d\n", __func__, r);
            }
        }
        else if(!strcmp(argv[i], "-censor"))
        {
            pl2_censor = 1;
        }
    }
#endif // !_PSP_FW_VERSION

    //DEBUGPRINT("%s: before glutInit, argc == %d\n", __func__, *argc);
    glutInitWindowSize(PL2_NOMINAL_SCREEN_WIDTH, PL2_NOMINAL_SCREEN_HEIGHT);
    glutInit(argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE | GLUT_MULTISAMPLE);
    //DEBUGPRINT("%s: after glutInit, argc == %d\n", __func__, *argc);
#if FREEGLUT
    //atexit(glutExit);
#endif

#if _PSP_FW_VERSION
    glutCreateWindow("OPL2");
#else
    if(windowed || !pl2GlutTryGameMode())
    {
        DEBUGPRINT("%s: using windowed mode\n", __func__);
        glutCreateWindow("OPL2");
    }
#endif

    glutIdleFunc(pl2GlutIdleFunc);
    glutDisplayFunc(pl2GlutDisplayFunc);
    glutReshapeFunc(pl2GlutReshapeFunc);
#if _PSP_FW_VERSION
    glutJoystickFunc(pl2GlutJoystickFunc, 0);
#else
    glutKeyboardFunc(pl2GlutKeyboardFunc);
    glutSpecialFunc(pl2GlutSpecialFunc);
    glutMouseFunc(pl2GlutMouseFunc);
    glutMotionFunc(pl2GlutMotionFunc);
    glutPassiveMotionFunc(pl2GlutPassiveMotionFunc);
    glutMouseWheelFunc(pl2GlutMouseWheelFunc);
#endif

    glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_EXIT);

    pl2GlInit();

    return 1;
}

#endif // WITH_GLUT
