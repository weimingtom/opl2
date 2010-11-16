//#include "opl2.h"
#include "opl2_int.h"

/******************************************************************************/

pl2Package *pl2PackageOpen(const char *filename)
{
    PL2_CLEAR_ERROR();

    if (!filename)
    {
        PL2_ERROR(PL2_ERR_PARAM);
    }

    pl2Package *package = NEW(pl2Package);

    if (!package)
    {
        PL2_ERROR(PL2_ERR_MEMORY);
    }

    if (NULL == (package->file = fopen(filename, "rb")))
    {
        pl2PackageFree(package);
        PL2_ERROR(PL2_ERR_NOTFOUND);
    }

    //package->open = true;

    pl2_strlcpy(package->filename, filename, sizeof(package->filename));

    fread(&package->header,     sizeof(pl2PackageHeader), 1, package->file);
    fread(&package->entries[0], sizeof(pl2PackageEntry),  1, package->file);

    package->numEntries = (package->entries[0].offset - 16) / sizeof(pl2PackageEntry);

    if (package->numEntries > 1)
    {
        int newsize = sizeof(pl2Package) + (package->numEntries - 1) * sizeof(pl2PackageEntry);
        //DEBUGPRINT("%s: realloc(%p) to %d bytes\n", __func__, package, newsize);
        void *temp = realloc(package, newsize);

        //DEBUGPRINT("%s: got space @ %p\n", __func__, temp);

        if (!temp)
        {
            pl2PackageFree(package);
            PL2_ERROR(PL2_ERR_MEMORY);
        }

        package = (pl2Package *)temp;
    }

    fread(&package->entries[1], sizeof(pl2PackageEntry), package->numEntries - 1, package->file);

    return package;
}

void pl2PackageClose(pl2Package *package)
{
   if (package)
   {
      if (package->file)
      {
         fclose(package->file);
         package->file = NULL;
      }
      //free(package);
   }
}

int pl2PackageReopen(pl2Package *package)
{
    PL2_CLEAR_ERROR();

    if (!package)
    {
        PL2_ERROR(PL2_ERR_PARAM);
    }

    if(package->file)
    {
        fclose(package->file);
    }

    package->file = fopen(package->filename, "rb");

    if (!package->file)
    {
        PL2_ERROR(PL2_ERR_FILEIO);
    }

    return true;
}

void pl2PackageFree(pl2Package *package)
{
   if (package)
   {
      pl2PackageClose(package);
      free(package);
   }
}

/******************************************************************************/

#define LZBUFSIZE 4096

static size_t pl2PackageDecompressFile(uint8_t *src, size_t srcsize,
                                       uint8_t *dst, size_t dstsize)
{
   if (!(src && srcsize && dst && dstsize))
   {
      return 0;
   }

   uint8_t buffer[LZBUFSIZE];
   uint8_t mask = 0;
   uint8_t flags = 0;

   memset(buffer, 0, LZBUFSIZE);

   size_t srcpos = 0, dstpos = 0;

   while ((srcpos < srcsize) && (dstpos < dstsize))
   {
      if (mask == 0)
      {
         flags = src[srcpos++];
         mask = 1;
      }

      if (srcpos >= srcsize) break;

      if ((flags & mask) != 0) // raw byte
      {
         uint8_t b = src[srcpos++];
         buffer[dstpos % LZBUFSIZE] = b;
         dst[dstpos++] = b;
      }
      else // compression code: 0xPP 0xPL (P=pointer, L=length)
      {
         uint8_t x = src[srcpos++];
         uint8_t y = src[srcpos++];

         int off = (((y & 0xf0) << 4) | x) + 18;
         int len = (y & 0x0f) + 3;

         while ((len-- > 0) && (dstpos < dstsize))
         {
            uint8_t b = buffer[off++ % LZBUFSIZE];
            buffer[dstpos % LZBUFSIZE] = b;
            dst[dstpos++] = b;
         }
      }

      mask += mask;
   }

   return dstpos;
}

pl2PackageFile *pl2PackageReadIndex(pl2Package *package, int index)
{
    PL2_CLEAR_ERROR();

    if (!package)
    {
        DEBUGPRINT("%s: package == NULL\n", __func__);
        PL2_ERROR(PL2_ERR_PARAM);
    }

    if ((index < 0) || (index >= package->numEntries))
    {
        DEBUGPRINT("%s: index %d out of range\n", __func__, index);
        PL2_ERROR(PL2_ERR_PARAM);
    }

    pl2PackageEntry *entry = &(package->entries[index]);

    pl2PackageFile *file = NEWVAR(pl2PackageFile, entry->length);

    if(!file)
    {
        PL2_ERROR(PL2_ERR_MEMORY);
    }

    memcpy(file->name, entry->name, 32);
    file->length = entry->length;

    if(!pl2PackageReopen(package))
    {
        pl2PackageFileFree(file);
        PL2_ERROR(PL2_ERR_FILEIO);
    }

    fseek(package->file, entry->offset, SEEK_SET);

    if (entry->length == entry->packedLength)
    {
        fread(file->data, file->length, 1, package->file);
    }
    else
    {
        uint8_t *temp = malloc(entry->packedLength);

        if (!temp)
        {
            pl2PackageFileFree(file);
            PL2_ERROR(PL2_ERR_MEMORY);
        }

        fread(temp, entry->packedLength, 1, package->file);

        pl2PackageDecompressFile(temp, entry->packedLength,
                                 file->data, entry->length);

        free(temp);
    }
    return file;
}

pl2PackageFile *pl2PackageRead(pl2Package *package, const char *name)
{
    PL2_CLEAR_ERROR();

    if (!package)
    {
        DEBUGPRINT("%s: package == NULL\n", __func__);
        PL2_ERROR(PL2_ERR_PARAM);
    }

    if (!name)
    {
        DEBUGPRINT("%s: filename == NULL\n", __func__);
        PL2_ERROR(PL2_ERR_PARAM);
    }

    int i;
    for (i = 0; i < package->numEntries; ++i)
    {
        pl2PackageEntry *entry = &(package->entries[i]);
        if (0 == strncmp(entry->name, name, 32))
        {
            pl2PackageFile *file = NEWVAR(pl2PackageFile, entry->length);

            if(!file)
            {
                PL2_ERROR(PL2_ERR_MEMORY);
            }

            memcpy(file->name, entry->name, 32);
            file->length = entry->length;

            if(!pl2PackageReopen(package))
            {
                pl2PackageFileFree(file);
                PL2_ERROR(PL2_ERR_FILEIO);
            }

            fseek(package->file, entry->offset, SEEK_SET);

            if (entry->length == entry->packedLength)
            {
                fread(file->data, file->length, 1, package->file);
            }
            else
            {
                uint8_t *temp = malloc(entry->packedLength);

                if (!temp)
                {
                    pl2PackageFileFree(file);
                    PL2_ERROR(PL2_ERR_MEMORY);
                }

                fread(temp, entry->packedLength, 1, package->file);

                pl2PackageDecompressFile(temp, entry->packedLength,
                                         file->data, entry->length);

                free(temp);
            }
            return file;
        }
    }

    //DEBUGPRINT("%s: \"%s\" not found\n", __func__, filename);
    PL2_ERROR(PL2_ERR_NOTFOUND);
}

void pl2PackageFileFree(pl2PackageFile *file)
{
   if (file)
   {
      free(file);
   }
}

