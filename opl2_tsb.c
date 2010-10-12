#include "opl2.h"
#include "opl2_int.h"

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

    uint8_t *data = file->data;

    if(!data)
    {
        pl2PackageFileFree(file);
        DEBUGPRINT("%s: file->data == NULL\n", __func__);
        PL2_ERROR(PL2_ERR_INTERNAL);
    }

    pl2Anim *anim = NEW(pl2Anim);

    if (!anim)
    {
        pl2PackageFileFree(file);
        pl2AnimFree(anim);
        PL2_ERROR(PL2_ERR_MEMORY);
    }

    anim->magic = READUINT32(data);

    if (anim->magic != PL2_TSB_MAGIC)
    {
        DEBUGPRINT("%s: magic != TSB_MAGIC\n", __func__);
        pl2PackageFileFree(file);
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

    anim->bones = NEWARR(anim->numFrames * anim->numBones, fmatrix4_t);

    if (!anim->bones)
    {
        pl2PackageFileFree(file);
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

    pl2PackageFileFree(file);
    return anim;
}

void pl2AnimFree(pl2Anim *anim)
{
    if(anim)
    {
        if(anim->bones)
        {
            DELETE(anim->bones);
        }
        if(anim->unknown)
        {
            DELETE(anim->unknown);
        }

        DELETE(anim);
    }
}
