//#include "opl2.h"
#include "opl2_int.h"
#include "opl2_vm.h"

/******************************************************************************/

static pl2Model *pl2ModelLoadInternal(const uint8_t *data)
{
    //const uint8_t *org = data;

    PRINTFREERAM();

    if(!data)
    {
        DEBUGPRINT("%s: data == NULL\n", __func__);
        PL2_ERROR(PL2_ERR_INTERNAL);
    }

    pl2Model *model = NEW(pl2Model);

    if(!model)
    {
        PL2_ERROR(PL2_ERR_MEMORY);
    }

    uint32_t i, j;

    model->magic = READUINT32(data);

    if(model->magic != PL2_TMB_MAGIC)
    {
        pl2ModelFree(model);
        PL2_ERROR(PL2_ERR_FORMAT);
    }

    //DEBUGPRINT("%s(%d): offset == %d\n", __FILE__, __LINE__, data - org);

    model->numTextures = READUINT32(data);

    //DEBUGPRINT("%s: numTextures == %d\n", __func__, model->numTextures);

    //DEBUGPRINT("%s: allocating %d textures == %d bytes for model->textures\n",
    //           __func__, model->numTextures, ARRSIZE(model->numTextures, pl2Texture));

    model->textures = NEWARR(model->numTextures, pl2Texture);

    if(!model->textures)
    {
        DEBUGPRINT("%s: error allocating model->textures\n", __func__);
        pl2ModelFree(model);
        PL2_ERROR(PL2_ERR_MEMORY);
    }

    //DEBUGPRINT("%s(%d): offset == %d\n", __FILE__, __LINE__, data - org);

    for(i = 0; i < model->numTextures; i++)
    {
        pl2Texture *t = model->textures + i;
        //DEBUGPRINT("%s: model->textures[%d] == %p\n", __func__, i, t);
        READSTRING(32, t->name, data);
        t->width = READUINT16(data);
        t->height = READUINT16(data);

        //DEBUGPRINT("%s(%d): offset == %d\n", __FILE__, __LINE__, data - org);

        uint32_t size = 4 * t->width * t->height;
        t->pixels = malloc(size);

        if(!t->pixels)
        {
            DEBUGPRINT("%s: error allocating model texture data\n", __func__);
            pl2ModelFree(model);
            PL2_ERROR(PL2_ERR_MEMORY);
        }

        //DEBUGPRINT("%s(%d): reading %dx%d == %d bytes\n", __FILE__, __LINE__, t->width, t->height, size);

        READSTRING(size, t->pixels, data);

        //DEBUGPRINT("%s(%d): offset == %d\n", __FILE__, __LINE__, data - org);

        uint32_t *pixels = (uint32_t*)(t->pixels);

        int j, k = t->width * t->height;

        for(j = 0; j < k; j++)
        {
            pixels[j] =
                ((pixels[j] & 0x000000ff) << 16) |
                ((pixels[j] & 0x00ff0000) >> 16) |
                ((pixels[j] & 0xff00ff00));
        }
    }

    //DEBUGPRINT("%s(%d): offset == %d\n", __FILE__, __LINE__, data - org);

    model->numMaterials = READUINT32(data);

    //DEBUGPRINT("%s: numMaterials == %d\n", __func__, model->numMaterials);

    //DEBUGPRINT("%s: allocating %d bytes for model->materials\n",
    //           __func__, ARRSIZE(model->numMaterials, pl2Material));

    model->materials = NEWARR(model->numMaterials, pl2Material);

    if(!model->materials)
    {
        DEBUGPRINT("%s: error allocating model->materials\n", __func__);
        pl2ModelFree(model);
        PL2_ERROR(PL2_ERR_MEMORY);
    }

    //DEBUGPRINT("%s(%d): offset == %d\n", __FILE__, __LINE__, data - org);

    for(i = 0; i < model->numMaterials; i++)
    {
        pl2Material *m = &(model->materials[i]);
        READCOLOR4(m->ambient,  data);
        READCOLOR4(m->diffuse,  data);
        READCOLOR4(m->specular, data);
        READCOLOR4(m->emissive, data);
        m->shininess = READFLOAT(data);

        float t;
        // dirty hacks for some models that have weird lighting values
        t = fmax(m->ambient.r, fmax(m->ambient.g, m->ambient.b));
        m->ambient.r = m->ambient.g = m->ambient.b = t;
        t = fmax(m->diffuse.r, fmax(m->diffuse.g, m->diffuse.b));
        m->diffuse.r = m->diffuse.g = m->diffuse.b = t;
        t = fmax(m->specular.r, fmax(m->specular.g, m->specular.b));
        m->specular.r = m->specular.g = m->specular.b = t;
        //m->ambient.a =
        m->diffuse.a = m->specular.a = m->emissive.a = 1.0f;

        //DEBUGPRINT("%s: Material %d\n", __func__, i);
        //DEBUGPRINT("%s: m->ambient  == <%.3f,%.3f,%.3f,%.3f>\n", __func__, m->ambient .r, m->ambient .g, m->ambient .b, m->ambient .a);
        //DEBUGPRINT("%s: m->diffuse  == <%.3f,%.3f,%.3f,%.3f>\n", __func__, m->diffuse .r, m->diffuse .g, m->diffuse .b, m->diffuse .a);
        //DEBUGPRINT("%s: m->specular == <%.3f,%.3f,%.3f,%.3f>\n", __func__, m->specular.r, m->specular.g, m->specular.b, m->specular.a);
        //DEBUGPRINT("%s: m->emissive == <%.3f,%.3f,%.3f,%.3f>\n", __func__, m->emissive.r, m->emissive.g, m->emissive.b, m->emissive.a);
        //DEBUGPRINT("%s: m->shininess == %.3f\n", __func__, m->shininess);

        uint32_t texid = READUINT32(data);
        m->texture = (texid < model->numTextures) ? &(model->textures[texid]) : NULL;

        //DEBUGPRINT("%s(%d): offset == %d\n", __FILE__, __LINE__, data - org);

        //DEBUGPRINT("%s: m->texture == %p, m->texture->pixels == %p\n", __func__, m->texture, m->texture ? m->texture->pixels : NULL);
    }

    model->numObjects = READUINT32(data);

    //DEBUGPRINT("%s: numObjects == %d\n", __func__, model->numObjects);

    model->objects = NEWALIGN(16, model->numObjects, pl2Object);

    if(!model->objects)
    {
        DEBUGPRINT("%s: error allocating model->objects\n", __func__);
        pl2ModelFree(model);
        PL2_ERROR(PL2_ERR_MEMORY);
    }

    for(i = 0; i < model->numObjects; i++)
    {
        pl2Object *obj = model->objects + i;
        READSTRING(32, obj->name, data);
        READMATRIX4(obj->transform, data);
        obj->numTriangles = READUINT32(data);
        obj->reserved     = READUINT32(data);
        obj->numMaterials = READUINT32(data);

        //DEBUGPRINT("%s: allocating %d bytes for obj->materials\n",
        //           __func__, ARRSIZE(obj->numMaterials, pl2ObjMtl));
        //DEBUGPRINT("%s: allocating %d bytes for obj->vertices\n",
        //           __func__, ARRSIZE(obj->numTriangles * 3, pl2Vertex));
        //DEBUGPRINT("%s: allocating %d bytes for obj->glVertices\n",
        //           __func__, ARRSIZE(obj->numTriangles * 3, pl2GlVertex));

        obj->materials  = NEWARR(obj->numMaterials, pl2ObjMtl);
        obj->vertices   = NEWARR(obj->numTriangles * 3, pl2Vertex);
        obj->glVertices = NEWARR(obj->numTriangles * 3, pl2GlVertex);

        if(!(obj->materials && obj->vertices && obj->glVertices))
        {
            DEBUGPRINT("%s: error allocating object materials/vertices\n", __func__);
            pl2ModelFree(model);
            PL2_ERROR(PL2_ERR_MEMORY);
        }

        for(j = 0; j < obj->numMaterials; j++)
        {
            pl2ObjMtl *m = &(obj->materials[j]);

            uint32_t mtlid = READUINT32(data);
            m->material = (mtlid < model->numMaterials) ? &(model->materials[mtlid]) : NULL;
            m->start = READUINT32(data);
            m->count = READUINT32(data);
        }

        //fmatrix4_t m __attribute__((aligned(16))) = obj->transform;

        for(j = 0; j < obj->numTriangles * 3; j++)
        {
            pl2Vertex *v = &(obj->vertices[j]);

            READVECTOR3(v->vertex, data);
            v->weights[0] = READFLOAT(data);
            v->weights[1] = READFLOAT(data);
            v->weights[2] = READFLOAT(data);
            READSTRING(4, v->bones, data);
            READVECTOR3(v->normal, data);
            v->color = READUINT32(data);
            READTEXCOORD2(v->texcoord, data);

            fvector4_t t __attribute__((aligned(16))) =
                { v->vertex.x, v->vertex.y, v->vertex.z, 1 };
            pl2VectorTransform4f(&t, &(obj->transform), &t);
            v->vertex.x = t.x; v->vertex.y = t.y; v->vertex.z = t.z;

            pl2GlVertex *glv = &(obj->glVertices[j]);

            glv->texcoord = v->texcoord;
            //glv->color    = v->color;
            glv->normal   = v->normal;
            glv->vertex   = v->vertex;
        }
    }

    model->numBones = READUINT32(data);

    //DEBUGPRINT("%s: numBones == %d\n", __func__, model->numBones);

    model->bones = NEWALIGN(16, model->numBones, fmatrix4_t);

    if(model->numBones && !model->bones)
    {
        DEBUGPRINT("%s: error allocating model->bones\n", __func__);
        pl2ModelFree(model);
        PL2_ERROR(PL2_ERR_MEMORY);
    }

    for(i = 0; i < model->numBones; i++)
    {
        READMATRIX4(model->bones[i], data);
    }

    //float f = READFLOAT(data);
    //DEBUGPRINT("%s: unknown float == %g\n", __func__, f);

    model->numPoints = READUINT32(data);

    //DEBUGPRINT("%s: numPoints == %d\n", __func__, model->numPoints);

    model->points = NEWARR(model->numPoints, pl2Point);

    if(!model->points)
    {
        DEBUGPRINT("%s: error allocating model->points\n", __func__);
        pl2ModelFree(model);
        PL2_ERROR(PL2_ERR_MEMORY);
    }

    for(i = 0; i < model->numPoints; i++)
    {
        pl2Point *p = &(model->points[i]);
        READSTRING(4, p->bones, data);
        READVECTOR3(model->points[i].translate, data);
        READVECTOR3(model->points[i].rotate, data);
        READVECTOR3(model->points[i].scale, data);
    }

    char *pointName = (char*)data;

    for(i = 0; i < model->numPoints; i++)
    {
        while(*data++);
        model->points[i].name = strdup(pointName);

        /*
        DEBUGPRINT("%s: points[%d] (\"%s\") == T<%g,%g,%g>, R<%g,%g,%g>, S<%g,%g,%g>\n",
                   __func__, i, pointName,
                   model->points[i].translate.x,
                   model->points[i].translate.y,
                   model->points[i].translate.z,
                   model->points[i].rotate.x,
                   model->points[i].rotate.y,
                   model->points[i].rotate.z,
                   model->points[i].scale.x,
                   model->points[i].scale.y,
                   model->points[i].scale.z);
        */

        pointName = (char*)(data);
    }

    pl2ModelAddPoints(model);

    DEBUGPRINT("%s: total: %d textures, %d materials, %d objects, %d bones, %d points\n", __func__,
               model->numTextures, model->numMaterials, model->numObjects, model->numBones, model->numPoints);

    PRINTFREERAM();

    return model;
}

pl2Model *pl2ModelLoad(const char *name)
{
    PL2_CLEAR_ERROR();

    if(!name) return 0;

    char temp[FILENAME_MAX];
    snprintf(temp, sizeof(temp), "%s.tmb", name);

    pl2PackageFile *file = pl2PackageGetFile(temp);

    if(NULL == file)
    {
        PL2_ERROR(PL2_ERR_NOTFOUND);
    }

    pl2Model *model = pl2ModelLoadInternal(file->data);

    pl2PackageFileFree(file);
    return model;
}

pl2Model *pl2ModelLoadFile(const char *name)
{
    PL2_CLEAR_ERROR();

    if(!name) return 0;

    FILE *file = fopen(name, "rb");

    if(NULL == file)
    {
        PL2_ERROR(PL2_ERR_NOTFOUND);
    }

    if(fseek(file, 0, SEEK_END) < 0)
    {
        DEBUGPRINT("%s: seek error, %s\n", __func__, strerror(errno));
        fclose(file);
        PL2_ERROR(PL2_ERR_FILEIO);
    }

    int32_t size = ftell(file);

    DEBUGPRINT("%s: size == %d\n", __func__, size);

    fseek(file, 0, SEEK_SET);

    uint8_t *data = NEWARR(size,uint8_t);

    if(NULL == data)
    {
        fclose(file);
        PL2_ERROR(PL2_ERR_MEMORY);
    }

    int r = fread(data, 1, size, file);

    DEBUGPRINT("%s: read %d bytes\n", __func__, r);

    if(r < size)
    {
        DEBUGPRINT("%s: read error, %s\n", __func__, strerror(errno));
        fclose(file);
        PL2_ERROR(PL2_ERR_FILEIO);
    }

    pl2Model *model = pl2ModelLoadInternal(data);

    fclose(file);
    return model;
}

void pl2ModelFree(pl2Model *model)
{
    if(model)
    {
        PRINTFREERAM();

        if(model->textures)
        {
            int i;
            for(i = 0; i < model->numTextures; i++)
            {
                if(model->textures[i].pixels)
                {
                    DELETE(model->textures[i].pixels);
                }
            }

            DELETE(model->textures);
        }

        if(model->materials)
        {
            DELETE(model->materials);
        }

        if(model->objects)
        {
            int i;
            for(i = 0; i < model->numObjects; i++)
            {
                if(model->objects[i].materials)
                {
                    DELETE(model->objects[i].materials);
                }
                if(model->objects[i].vertices)
                {
                    DELETE(model->objects[i].vertices);
                }
                if(model->objects[i].glVertices)
                {
                    DELETE(model->objects[i].glVertices);
                }
            }

            DELETE(model->objects);
        }

        if(model->bones)
        {
            DELETE(model->bones);
        }

        if(model->points)
        {
            int i;
            for(i = 0; i < model->numPoints; i++)
            {
                if(model->points[i].name)
                {
                    DELETE(model->points[i].name);
                }
            }

            DELETE(model->points);
        }

        DELETE(model);

        PRINTFREERAM();
    }
}
