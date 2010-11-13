//#include "opl2.h"
#include "opl2_int.h"

/******************************************************************************/

pl2Image *pl2ImageLoad(const char *name)
{
    PL2_CLEAR_ERROR();

    char temp[FILENAME_MAX];
    snprintf(temp, sizeof(temp), "%s.psd", name);

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

    pl2Image *image = NEW(pl2Image);

    if(!image)
    {
        pl2PackageFileFree(file);
        PL2_ERROR(PL2_ERR_MEMORY);
    }

    pl2PackageFileFree(file);
    return image;
}

