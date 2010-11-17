//#include "opl2.h"
#include "opl2_int.h"
#include "opl2_vm.h"

#include <GL/gl.h>
#include <GL/glu.h>

#if 0 // WITH_GLUT
//#include <GL/glut.h>
#include <GL/freeglut.h>

/* use freeglut extensions, if available */
//#if FREEGLUT
//# include <GL/freeglut_ext.h>
//#endif
#endif

#include <math.h>

// Reference:
// http://www.opengl.org/sdk/docs/man/

#if 0 // _PSP_FW_VERSION
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
        //float color[4] = { 0.0f, 0.0f, 0.0f, 1.0f - layer->fade_level };
        //glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, color);
        glDisable(GL_TEXTURE_2D);

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
        glEnable(GL_TEXTURE_2D);

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
        glEnable(GL_TEXTURE_2D);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glColor4f(((color >>  0) & 255) / 255.0f,
                  ((color >>  8) & 255) / 255.0f,
                  ((color >> 16) & 255) / 255.0f,
                  ((color >> 24) & 255) / 255.0f);

        int w = font->glyphSize, h = font->glyphSize;

        int left = x;

        //int len = pl2Ucs4Strlen(text);

        int i, j;

        int pl2FontFindChar(pl2Font *font, uint32_t code);

        for(i = 0; i < len; i++)
        {
            if(text[i] == '\n')
            {
                y += h;
                x = left;
            }
            else if(text[i] >= 32)
            {
                j = pl2FontFindChar(font, text[i]);

                if(j >= 0)
                {
                    //pl2GlClearErrors();
                    glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, w, h, 0, GL_ALPHA,
                                 GL_UNSIGNED_BYTE, font->chars[j].glyph);
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

void pl2ModelRender(const pl2Model *model, bool black)
{
    if(!model) return;

    //glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);

    //glMatrixMode(GL_MODELVIEW);

    int i, j;

    static GLfloat mtlBlack[4] = { 0, 0, 0, 0.5f };

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

                glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, (GLfloat*)&(mtl->emissive));
                glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT,  black ? mtlBlack : (GLfloat*)&(mtl->ambient));
                glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE,  black ? mtlBlack : (GLfloat*)&(mtl->diffuse));
                glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, (GLfloat*)&(mtl->specular));
                glMaterialf (GL_FRONT_AND_BACK, GL_SHININESS, mtl->shininess);

                pl2Texture *tex = mtl->texture;

#if !_PSP_FW_VERSION
                if(tex && !black)
                {
                    glTexImage2D(GL_TEXTURE_2D, 0, 4, tex->width, tex->height,
                                 0, GL_RGBA, GL_UNSIGNED_BYTE, tex->pixels);
                }
#endif

                glDrawArrays(GL_TRIANGLES, m->start, m->count * 3);
            }
        }

        //glPopMatrix();
    }
}

void pl2CharRender(pl2Character *chr)
{
    if(chr && chr->visible)
    {
        int i;

        glColor4f(1, 1, 1, 1);
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

        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
        glEnable(GL_LIGHTING);

        if(chr->black)
        {
            glDisable(GL_TEXTURE_2D);
        }
        else
        {
            glEnable(GL_TEXTURE_2D);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        }

        for(i = 0; i < PL2_MAX_CHARPARTS; i++)
        {
            pl2ModelRender(chr->models[i], chr->black);
        }

        glMatrixMode(GL_MODELVIEW);
        glPopMatrix();
    }
}

/******************************************************************************/

void pl2GlRenderFrame(float dt)
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

    pl2CameraConfig(pl2_active_camera);

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

    if(pl2_font && !pl2_hide_overlay)
    {
        if(pl2_menu_showing)
        {
            int x = (int)(0.5f * pl2_screen_aspect * (float)PL2_NOMINAL_SCREEN_HEIGHT);
            int y = (int)(0.5f * (float)(PL2_NOMINAL_SCREEN_HEIGHT - (2 * pl2_menu.numItems - 1) * pl2_font->glyphSize));

            int h = 2 * pl2_font->glyphSize;

            glBegin(GL_QUADS);
            for(i = 0; i < pl2_menu.numItems; i++)
            {
                int y1 = y + i*h, y2 = y1 + (i+1)*h;
                glVertex2i(0, y1); glVertex2i(2*x, y1);
                glVertex2i(2*x, y2); glVertex2i(0, y2);
            }
            glEnd();

            for(i = 0; i < pl2_menu.numItems; i++)
            {
                uint32_t color = pl2_menu.items[i].enabled ? 0xff000000ul : 0x80000000ul;
                color |= (i == pl2_menu.selection) ? 0xfffffful : 0x000000ul;
                pl2FontUcsPrintCenter(pl2_font, x, y, color, pl2_menu.items[i].text);
                y += 2 * pl2_font->glyphSize;
            }
        }

        else if(pl2_show_window)
        {
            int x = 0; //(int)(0.5f * pl2_screen_aspect * (float)PL2_NOMINAL_SCREEN_HEIGHT);
            int y = (int)(0.8f * (float)PL2_NOMINAL_SCREEN_HEIGHT);

            if(pl2_name_color)
            {
                pl2FontUcsPrint(pl2_font, x, y, pl2_name_color, pl2_name_text);
                y += pl2_font->glyphSize;
            }

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

    glEnable(GL_LIGHTING);

    //glSampleCoverage(0.5f, 0);
}

