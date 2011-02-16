//#include "opl2.h"
#include "opl2_int.h"

#define ILUT_USE_OPENGL
#include <IL/il.h>
#include <IL/ilu.h>
#include <IL/ilut.h>

/******************************************************************************/

pl2Image *pl2ImageLoad(const char *name)
{
    PL2_CLEAR_ERROR();

    if(!name) return 0;

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

    ilGenImages(1, &(image->iltex));

    if(!ilLoadL(IL_TYPE_UNKNOWN, file->data, file->length))
    {
        pl2ImageFree(image);
        pl2PackageFileFree(file);
        PL2_ERROR(PL2_ERR_FILEIO);
    }

    image->width = ilGetInteger(IL_IMAGE_WIDTH);
    image->height = ilGetInteger(IL_IMAGE_HEIGHT);
    image->gltex = ilutGLBindTexImage();

    pl2PackageFileFree(file);
    return image;
}

void pl2ImageFree(pl2Image *image)
{
    if(image)
    {
        DELETE(image);
    }
}

