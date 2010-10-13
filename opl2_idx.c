#include "opl2.h"
#include "opl2_int.h"

/******************************************************************************/

#include <dirent.h>

//#define PACKAGELOG 1

/* Must be >= likely maximum number of packages (and should be prime) */
#define PACKAGE_INDEX_SIZE 16381

static struct
{
   char name[28];
   pl2Package *package;
}
pl2PackageIndex[PACKAGE_INDEX_SIZE];

static size_t index_size = 0, hash_collisions = 0;

//static FILE *packagelog;

/******************************************************************************/

/* Based on code from Paul Hsieh's "SuperFastHash"
 * (http://www.azillionmonkeys.com/qed/hash.html)
 */
#if (defined(__GNUC__) && defined(__i386__)) || defined(__WATCOMC__) \
  || defined(_MSC_VER) || defined (__BORLANDC__) || defined (__TURBOC__)
#define get16bits(d) (*((const uint16_t *) (d)))
#else
#define get16bits(d) ((((uint32_t)(((const uint8_t *)(d))[1])) << 8)\
                       +(uint32_t)(((const uint8_t *)(d))[0]) )
#endif

static uint32_t pl2PackageIndexHash(const char *data, size_t len)
{
    size_t t = strlen(data);
    if(t < len) len = t;
    uint32_t hash = len, tmp;
    int rem;

    if (len <= 0 || data == NULL) return 0;

    //DEBUGPRINT("%s: hashing %u characters of \"%s\"\n", __func__, len, data);

    rem = len & 3;
    len >>= 2;

    /* Main loop */
    for (;len > 0; len--) {
        hash  += get16bits (data);
        tmp    = (get16bits (data+2) << 11) ^ hash;
        hash   = (hash << 16) ^ tmp;
        data  += 2*sizeof (uint16_t);
        hash  += hash >> 11;
    }

    /* Handle end cases */
    switch (rem) {
        case 3: hash += get16bits (data);
                hash ^= hash << 16;
                hash ^= data[sizeof (uint16_t)] << 18;
                hash += hash >> 11;
                break;
        case 2: hash += get16bits (data);
                hash ^= hash << 11;
                hash += hash >> 17;
                break;
        case 1: hash += *data;
                hash ^= hash << 10;
                hash += hash >> 1;
    }

    /* Force "avalanching" of final 127 bits */
    hash ^= hash << 3;
    hash += hash >> 5;
    hash ^= hash << 4;
    hash += hash >> 17;
    hash ^= hash << 25;
    hash += hash >> 6;

    //DEBUGPRINT("%s: result == 0x%08x\n", __func__, hash);
    return hash;
}

static int pl2PackageIndexInsert(const char *filename)
{
    pl2Package *package = pl2PackageOpen(filename);

    if (!package)
    {
        DEBUGPRINT("%s: error opening package \"%s\": %s\n", __func__,
                   filename, pl2GetErrorMessage(pl2GetErrorCode()));
        return 0;
    }

    int i;

    //pl2PackageFile *attr = pl2PackageGetFile(package, "attribute");
    //if (!attr) return;

    for (i = 0; i < package->numEntries; ++i)
    {
        pl2PackageEntry *entry = &(package->entries[i]);
        uint32_t hash = pl2PackageIndexHash(entry->name, 28) % PACKAGE_INDEX_SIZE;

        uint32_t index = hash;

        do
        {
            if (strncmp(entry->name, pl2PackageIndex[index].name, 28) == 0)
            {
                // already in index, don't add it
                break;
            }

            if (!pl2PackageIndex[index].package)
            {
//#if PACKAGELOG
//                //printf("\t%s\n", package->entry[i].name);
//                if (packagelog) fprintf(packagelog, "\t%s\n", entry->name);
//#endif

                // empty slot, add to index
                pl2PackageIndex[index].package = package;
                pl2_strlcpy(pl2PackageIndex[index].name, entry->name, 28);
                //DEBUGPRINT("Added \"%s\" @ index %u\n", pl2PackageIndex[index].name, index);

                ++index_size;

                break;
            }

            ++hash_collisions;

            if ((index += 17) >= PACKAGE_INDEX_SIZE)
            {
                index -= PACKAGE_INDEX_SIZE;
            }
        }
        while (pl2PackageIndex[hash].package && (index != hash));
    }

    // close file handle (but don't free)
    pl2PackageClose(package);
    return 1;
}

static pl2Package *pl2PackageIndexSearch(const char *name)
{
    PL2_CLEAR_ERROR();

    if (!name)
    {
        PL2_ERROR(PL2_ERR_PARAM);
    }

    char temp[28];
    memset(temp, 0, sizeof(temp));
    pl2_strlcpy(temp, name, sizeof(temp));

    uint32_t hash = pl2PackageIndexHash(temp, 28) % PACKAGE_INDEX_SIZE;

    uint32_t index = hash;

    do
    {
        //DEBUGPRINT("%s: index[%u] == \"%s\"\n", __func__, index, pl2PackageIndex[index].name);

        if (strncmp(temp, pl2PackageIndex[index].name, 28) == 0)
        {
            DEBUGPRINT("%s: found \"%s\" @ index %u\n", __func__, pl2PackageIndex[index].name, index);

            return pl2PackageIndex[index].package;
        }

        if ((index += 17) >= PACKAGE_INDEX_SIZE)
        {
            index -= PACKAGE_INDEX_SIZE;
        }
    }
    while (pl2PackageIndex[index].package && (index != hash));

    DEBUGPRINT("%s: \"%s\" not found\n", __func__, name);

    PL2_ERROR(PL2_ERR_NOTFOUND);
}

int pl2PackageBuildIndex()
{
   memset(pl2PackageIndex, 0, sizeof(pl2PackageIndex));

//#if PACKAGELOG
//   if (!packagelog) packagelog = fopen("package.log", "wb");
//   if (!packagelog) { PL2_ERROR(PL2_ERR_FILEIO); }
//#endif

   DIR *dir = opendir("add-ons");
   if (NULL == dir)
   {
//#if PACKAGELOG
//      //printf("Error opening 'add-ons' directory\n");
//      fprintf(packagelog, "Error opening 'add-ons' directory\n");
//      fclose(packagelog);
//#endif

      PL2_ERROR(PL2_ERR_NOTFOUND);
   }

   char filename[64] = { 0 };

//#if PACKAGELOG
//   //printf("Add-ons found:\n");
//   fprintf(packagelog, "Add-ons found:\n");
//#endif

    DEBUGPRINT("%s: building package index...\n", __func__);

   size_t count = 0;

   struct dirent *entry;
   while (NULL != (entry = readdir(dir)))
   {
      int l = strlen(entry->d_name);

      if (0 == strcasecmp(entry->d_name + l - 4, ".pl2"))
      {
//#if PACKAGELOG
//         //printf("\n%s\n", entry);
//         fprintf(packagelog, "\n%s\n", entry->d_name);
//#endif

         snprintf(filename, sizeof(filename), "add-ons/%s", entry->d_name);

         if(pl2PackageIndexInsert(filename))
         {
             ++count;
         }
      }
   }

    DEBUGPRINT("%s: indexed %u files in %u packages with %u collisions\n", __func__, index_size, count, hash_collisions);

   closedir(dir);
   return 1;
}

void pl2PackageClearIndex()
{
   int i;
   for (i = 0; i < PACKAGE_INDEX_SIZE; ++i)
   {
      if (pl2PackageIndex[i].package)
      {
         pl2PackageClose(pl2PackageIndex[i].package);
      }
   }
}

pl2PackageFile *pl2PackageGetFile(const char *filename)
{
   PL2_CLEAR_ERROR();

   if (!filename)
   {
      PL2_ERROR(PL2_ERR_PARAM);
   }

   pl2Package *package = pl2PackageIndexSearch(filename);

   if (package)
   {
      pl2PackageFile *file = pl2PackageRead(package, filename);

      pl2PackageClose(package);

      return file;
   }

   PL2_ERROR(PL2_ERR_NOTFOUND);
}
