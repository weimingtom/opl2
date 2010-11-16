//#include "opl2.h"
#include "opl2_int.h"

#include <math.h>

static int skipWhiteSpace(const char **p)
{
    const char *t = *p;

    //DEBUGPRINT("%s: skip whitespace\n", __func__);

    while(**p && (**p == '#' || **p <= 32))
    {
        if(**p == '#')
        {
            //DEBUGPRINT("%s: comment...\n", __func__);
            while(**p && **p != 13 && **p != 10)
                (*p)++;
        }
        while(**p && **p <= 32)
        {
            (*p)++;
        }
    }

    return t != *p;
}

static int checkString(const char **p, const char *s)
{
    skipWhiteSpace(p);

    //DEBUGPRINT("%s: string: ", __func__);

    //DEBUGPRINT("\"%c%c%c%c%c...\"", (*p)[0], (*p)[1], (*p)[2], (*p)[3], (*p)[4]);

    int l = strlen(s);

    if(strncmp(*p, s, l))
    {
        //DEBUGPRINT("ERROR\n");
        return 0;
    }

    *p += l;

    //DEBUGPRINT("found\n");
    return 1;
}

static int parseInt(const char **p, int *x)
{
    const char *t = *p;

    skipWhiteSpace(p);

    //DEBUGPRINT("%s: integer: ", __func__);

    int i = 0, n = 1;

    if(**p == '-')
    {
        n = -1;
        (*p)++;
    }
    while(**p && **p >= '0' && **p <= '9')
    {
        i *= 10;
        i += (*(*p)++) - '0';
    }

    *x = n*i;

    //DEBUGPRINT("%d\n", *x);

    return (t != *p);
}

static int parseFloat(const char **p, float *x)
{
    const char *t = *p;

    skipWhiteSpace(p);

    //DEBUGPRINT("%s: float: ", __func__);

    int i = 0, f = 0, n = 1, d = 0, e = 0;

    if(**p == '-')
    {
        n = -1;
        (*p)++;
    }
    while(**p && **p >= '0' && **p <= '9')
    {
        i *= 10;
        i += (*(*p)++) - '0';
    }
    if(**p == '.')
    {
        (*p)++;
        while(**p && **p >= '0' && **p <= '9')
        {
            d++;
            f *= 10;
            f += (*(*p)++) - '0';
        }
    }
    if((**p | 0x20) == 'e')
    {
        (*p)++;
        int s = 1;
        if(**p == '-')
        {
            s = -1;
            (*p)++;
        }
        while(**p && **p >= '0' && **p <= '9')
        {
            e *= 10;
            e += (*(*p)++) - '0';
        }
        e *= s;
    }

    *x = n*(((float)i) + ((float)f)*powf(10,-d))*powf(10,e);

    //DEBUGPRINT("%g\n", *x);

    return (t != *p);
}

static pl2CameraPath *pl2CameraPathLoadInternal(const char *data)
{
    PRINTFREERAM();

    if(!data)
    {
        DEBUGPRINT("%s: data == NULL\n", __func__);
        PL2_ERROR(PL2_ERR_INTERNAL);
    }

    if(!checkString(&data, "@TCM100"))
    {
        DEBUGPRINT("%s: parse error: bad file ID\n", __func__);
        PL2_ERROR(PL2_ERR_FORMAT);
    }

    if(!checkString(&data, "[CAMERAANIM]"))
    {
        DEBUGPRINT("%s: parse error: bad section header\n", __func__);
        PL2_ERROR(PL2_ERR_FORMAT);
    }

    int first, last;

    if(!parseInt(&data, &first))
    {
        DEBUGPRINT("%s: parse error: loop flag\n", __func__);
        PL2_ERROR(PL2_ERR_FORMAT);
    }
    if(!parseInt(&data, &last))
    {
        DEBUGPRINT("%s: parse error: frame count\n", __func__);
        PL2_ERROR(PL2_ERR_FORMAT);
    }

    int numFrames = last - first + 1;

    DEBUGPRINT("%s: first == %d, last == %d, numFrames == %d\n", __func__, first, last, numFrames);

    pl2CameraPath *path = NEWVAR(pl2CameraPath, numFrames * sizeof(pl2CameraFrame));

    path->numFrames = numFrames;

    int i;
    for(i = 0; i < numFrames; i++)
    {
        float x, y, z, lx, ly, lz, fov;

        if(!parseFloat(&data, &x)  ||
           !parseFloat(&data, &y)  ||
           !parseFloat(&data, &z)  ||
           !parseFloat(&data, &lx) ||
           !parseFloat(&data, &ly) ||
           !parseFloat(&data, &lz) ||
           !parseFloat(&data, &fov))
        {
            DEBUGPRINT("%s: parse error: frame %d\n", __func__, i);
            pl2CameraPathFree(path);
            PL2_ERROR(PL2_ERR_FORMAT);
        }

        //DEBUGPRINT("%s: frame %d: %g %g %g %g %g %g %g\n", __func__, i, x, y, z, lx, ly, lz, fov);

        path->frames[i].eye.x = x;
        path->frames[i].eye.y = y;
        path->frames[i].eye.z = z;
        path->frames[i].focus.x = lx;
        path->frames[i].focus.y = ly;
        path->frames[i].focus.z = lz;
        path->frames[i].fov = fov;
    }

    if(!checkString(&data, "<___end___>"))
    {
        DEBUGPRINT("%s: parse error: bad trailer\n", __func__);
        PL2_ERROR(PL2_ERR_FORMAT);
    }

    DEBUGPRINT("%s: loaded OK\n", __func__);

    PRINTFREERAM();

    return path;
}

pl2CameraPath *pl2CameraPathLoad(const char *name)
{
    PL2_CLEAR_ERROR();

    char temp[FILENAME_MAX];
    snprintf(temp, sizeof(temp), "%s.tcm", name);

    pl2PackageFile *file = pl2PackageGetFile(temp);

    if(NULL == file)
    {
        PL2_ERROR(PL2_ERR_NOTFOUND);
    }

    pl2CameraPath *path = pl2CameraPathLoadInternal((const char*)(file->data));

    pl2PackageFileFree(file);
    return path;
}

pl2CameraPath *pl2CameraPathLoadFile(const char *name)
{
    PL2_CLEAR_ERROR();

    FILE *file = fopen(name, "rb");

    if(NULL == file)
    {
        PL2_ERROR(PL2_ERR_NOTFOUND);
    }

    if(fseek(file, 0, SEEK_END) < 0)
    {
        fclose(file);
        PL2_ERROR(PL2_ERR_FILEIO);
    }

    int32_t size = ftell(file);

    uint8_t *data = NEWARR(size,uint8_t);

    if(NULL == data)
    {
        fclose(file);
        PL2_ERROR(PL2_ERR_MEMORY);
    }

    if(fread(data, size, 1, file) < 1)
    {
        fclose(file);
        PL2_ERROR(PL2_ERR_FILEIO);
    }

    pl2CameraPath *path = pl2CameraPathLoadInternal((const char*)data);

    fclose(file);
    return path;
}

void pl2CameraPathFree(pl2CameraPath *path)
{
    if(path)
    {
        PRINTFREERAM();

        DELETE(path);

        PRINTFREERAM();
    }
}

