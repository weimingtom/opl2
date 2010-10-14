#include "opl2.h"
#include "opl2_int.h"

#include <vorbis/vorbisfile.h>

/******************************************************************************/

static size_t pl2_ov_read_func(void *ptr, size_t len, size_t num, void *src)
{
    pl2Sound *sound = (pl2Sound*)src;

    if(sound && sound->file)
    {
        uint8_t *data = (uint8_t*)ptr;

        size_t i;

        for(i = 0; i < num; i++)
        {
            if(sound->offset + len <= sound->file->length)
            {
                memcpy(data, &(sound->file->data[sound->offset]), len);
                sound->offset += len;
            }
            else break;
        }

        return i;
    }

    return -1;
}

static int pl2_ov_seek_func(void *src, ogg_int64_t ofs, int whence)
{
    pl2Sound *sound = (pl2Sound*)src;

    if(sound && sound->file)
    {
        switch(whence)
        {
            default:
            case SEEK_SET:
                break;
            case SEEK_CUR:
                ofs += sound->offset;
                break;
            case SEEK_END:
                ofs += sound->file->length;
                break;
        }

        if(ofs < 0)
            ofs = 0;
        else if(ofs > sound->file->length)
            ofs = sound->file->length;

        return sound->offset = ofs;
    }
    return -1;
}

static int pl2_ov_close_func(void *src)
{
    pl2Sound *sound = (pl2Sound*)src;

    if(sound && sound->file)
    {
        pl2PackageFileFree(sound->file);
        sound->file = NULL;
        sound->offset = 0;
        return 0;
    }
    return -1;
}

static long pl2_ov_tell_func(void *src)
{
    pl2Sound *sound = (pl2Sound*)src;

    if(sound && sound->file)
    {
        return sound->offset;
    }
    return -1;
}

static ov_callbacks pl2_ov_callbacks =
{
    pl2_ov_read_func,
    pl2_ov_seek_func,
    pl2_ov_close_func,
    pl2_ov_tell_func,
};

pl2Sound *pl2SoundLoad(const char *name)
{
    PL2_CLEAR_ERROR();

    char temp[FILENAME_MAX];
    snprintf(temp, sizeof(temp), "%s.ogg", name);

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

    pl2Sound *sound = NEW(pl2Sound);

    if(!sound)
    {
        pl2PackageFileFree(file);
        PL2_ERROR(PL2_ERR_MEMORY);
    }

    if(ov_open_callbacks(sound, &(sound->vf), NULL, 0, pl2_ov_callbacks) < 0)
    {
        pl2PackageFileFree(file);
        PL2_ERROR(PL2_ERR_FORMAT);
    }

    //pl2PackageFileFree(file);
    return sound;
}

void pl2SoundFree(pl2Sound *sound)
{
    if(sound)
    {
        ov_clear(&(sound->vf));
        //pl2PackageFileFree(sound->file);
        DELETE(sound);
    }
}
