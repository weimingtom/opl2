//#include "opl2.h"
#include "opl2_int.h"

#include <stdio.h>

/******************************************************************************/

static pl2Anim *pl2AnimLoadInternal(const uint8_t *data)
{
    PRINTFREERAM();

    if(!data)
    {
        DEBUGPRINT("%s: data == NULL\n", __func__);
        PL2_ERROR(PL2_ERR_INTERNAL);
    }

    pl2Anim *anim = NEW(pl2Anim);

    if (!anim)
    {
        pl2AnimFree(anim);
        PL2_ERROR(PL2_ERR_MEMORY);
    }

    anim->magic = READUINT32(data);

    if (anim->magic != PL2_TSB_MAGIC)
    {
        DEBUGPRINT("%s: magic != TSB_MAGIC\n", __func__);
        pl2AnimFree(anim);
        PL2_ERROR(PL2_ERR_FORMAT);
    }

    anim->reserved[0] = READUINT32(data);
    anim->reserved[1] = READUINT32(data);
    anim->reserved[2] = READUINT32(data);

    anim->numBones   = READUINT32(data);
    anim->numFrames  = READUINT32(data);
    anim->loopFrame  = READUINT32(data);
    anim->numUnknown = READUINT32(data);

    anim->bones = NEWALIGN(16, anim->numFrames * anim->numBones, fmatrix4_t);

    if (!anim->bones)
    {
        DEBUGPRINT("%s: error allocating anim->bones\n", __func__);
        pl2AnimFree(anim);
        PL2_ERROR(PL2_ERR_MEMORY);
    }

    int i, j, k = 0;
    for(i = 0; i < anim->numFrames; i++)
    {
        for(j = 0; j < anim->numBones; j++)
        {
            READMATRIX4(anim->bones[k], data);
            k++;
        }
    }

    DEBUGPRINT("%s: %d bones, %d frames, loops at %d\n", __func__,
               anim->numBones, anim->numFrames, anim->loopFrame);

    PRINTFREERAM();

    return anim;
}

pl2Anim *pl2AnimLoad(const char *name)
{
    PL2_CLEAR_ERROR();

    char temp[FILENAME_MAX];
    snprintf(temp, sizeof(temp), "%s.tsb", name);

    pl2PackageFile *file = pl2PackageGetFile(temp);

    if(NULL == file)
    {
        PL2_ERROR(PL2_ERR_NOTFOUND);
    }

    pl2Anim *anim = pl2AnimLoadInternal(file->data);

    pl2PackageFileFree(file);
    return anim;
}

pl2Anim *pl2AnimLoadFile(const char *name)
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

    pl2Anim *anim = pl2AnimLoadInternal(data);

    fclose(file);
    return anim;
}

void pl2AnimFree(pl2Anim *anim)
{
    if(anim)
    {
        PRINTFREERAM();

        if(anim->bones)
        {
            DELETE(anim->bones);
        }
        if(anim->unknown)
        {
            DELETE(anim->unknown);
        }

        DELETE(anim);

        PRINTFREERAM();
    }
}

