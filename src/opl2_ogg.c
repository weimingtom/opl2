//#include "opl2.h"
#include "opl2_int.h"

#if WITH_TREMOR
# include <tremor/ivorbisfile.h>
#else
# include <vorbis/vorbisfile.h>
#endif

/******************************************************************************/

static size_t pl2_ov_read_func(void *ptr, size_t len, size_t num, void *src)
{
    pl2Sound *sound = (pl2Sound*)src;

    if(sound && sound->file)
    {
        //DEBUGPRINT("%s: reading %d units of %d bytes == %d total bytes\n", __func__, num, len, num*len);

        uint8_t *data = (uint8_t*)ptr;

        size_t n = MIN(len * num, sound->file->length - sound->offset);

        memcpy(data, &(sound->file->data[sound->offset]), n);
        sound->offset += n;

        //DEBUGPRINT("%s: read %d units == %d total bytes\n", __func__, n/len, n);
        return n / len;
    }

    DEBUGPRINT("%s: sound == %p, sound->file == %p\n", __func__, sound, sound->file);

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

int pl2SoundDecode(pl2Sound *sound, void *buffer, int bytes)
{
    OggVorbis_File *vf = &(sound->vf);
    char *out = (char*)buffer;

    int res = 0, bytesLeft = bytes;

    while(bytesLeft > 0)
    {
#if WITH_TREMOR
        res = ov_read(vf, out, bytesLeft, NULL);
#else
        res = ov_read(vf, out, bytesLeft, 0, 2, 1, NULL);
#endif
        if(res == OV_HOLE) continue;
        else if(res <= 0) break;
        bytesLeft -= res;
        out += res;
    }

    return bytes - bytesLeft;
}

pl2Sound *pl2SoundLoad(const char *name)
{
    PL2_CLEAR_ERROR();

    if(!name) return 0;

    char temp[FILENAME_MAX];
    snprintf(temp, sizeof(temp), "%s.ogg", name);

    pl2PackageFile *file = pl2PackageGetFile(temp);

    if(NULL == file)
    {
        DEBUGPRINT("%s: \"%s\" not found\n", __func__, temp);

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
    sound->file = file;
    sound->volume = 1;

    if(!sound)
    {
        DEBUGPRINT("%s: error creating pl2Sound structure\n", __func__);
        pl2PackageFileFree(file);
        DELETE(sound);
        PL2_ERROR(PL2_ERR_MEMORY);
    }

    int err = ov_open_callbacks(sound, &(sound->vf), NULL, 0, pl2_ov_callbacks);

    if(err < 0)
    {
        DEBUGPRINT("%s: error %d opening OGG file\n", __func__, err);
        pl2PackageFileFree(file);
        DELETE(sound);
        PL2_ERROR(PL2_ERR_FORMAT);
    }

    vorbis_info *info = ov_info(&(sound->vf), -1);
    sound->channels = info->channels;
    sound->sampleRate = info->rate;

    //pl2PackageFileFree(file);
    DEBUGPRINT("%s: sound == %p\n", __func__, sound);
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
