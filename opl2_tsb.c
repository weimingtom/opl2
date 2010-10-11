#include "opl2.h"
#include "opl2_int.h"

pl2Sequence *pl2SequenceLoad(const char *name)
{
    PL2_CLEAR_ERROR();

    pl2PackageFile *file = pl2PackageGetFile(name);

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

    pl2Sequence *sequence = NEW(pl2Sequence);

    if (!sequence)
    {
        pl2PackageFileFree(file);
        pl2SequenceFree(sequence);
        PL2_ERROR(PL2_ERR_MEMORY);
    }

    sequence->magic = READUINT32(data);

    if (sequence->magic != PL2_TSB_MAGIC)
    {
        DEBUGPRINT("%s: magic != TSB_MAGIC\n", __func__);
        pl2PackageFileFree(file);
        pl2SequenceFree(sequence);
        PL2_ERROR(PL2_ERR_FORMAT);
    }

    sequence->reserved[0] = READUINT32(data);
    sequence->reserved[1] = READUINT32(data);
    sequence->reserved[2] = READUINT32(data);

    sequence->numBones   = READUINT32(data);
    sequence->numFrames  = READUINT32(data);
    sequence->loopFrame  = READUINT32(data);
    sequence->numUnknown = READUINT32(data);

    sequence->bones = NEWARR(sequence->numFrames * sequence->numBones, fmatrix4_t);

    if (!sequence->bones)
    {
        pl2PackageFileFree(file);
        pl2SequenceFree(sequence);
        PL2_ERROR(PL2_ERR_MEMORY);
    }

    int i, j, k = 0;
    for(i = 0; i < sequence->numFrames; i++)
    {
        for(j = 0; j < sequence->numBones; j++)
        {
            READMATRIX4(sequence->bones[k], data);
            k++;
        }
    }

    pl2PackageFileFree(file);
    return sequence;
}

void pl2SequenceFree(pl2Sequence *sequence)
{
    if(sequence)
    {
        if(sequence->bones)
        {
            DELETE(sequence->bones);
        }
        if(sequence->unknown)
        {
            DELETE(sequence->unknown);
        }

        DELETE(sequence);
    }
}
