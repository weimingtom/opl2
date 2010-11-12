#include "opl2.h"
#include "opl2_int.h"

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
//#include <glob.h>
#include <errno.h>
#include <fcntl.h>

#ifndef __MINGW32__
# define mkdir(p) mkdir((p),0777)
#endif

static int showUsage()
{
    fprintf(stderr, "Usage: pl2ex [-l] <package> [<files>...]\n");
    fprintf(stderr, "\t-l\tList only (do not extract)\n");
    return 1;
}

int main(int argc, char *argv[])
{
    //const char *package_name = NULL;
    //char **file_filters = NULL;

    int list_only = 0;

    int i, j;

    for(i = 1; i < argc; i++)
    {
        if(argv[i][0] == '-')
        {
            if(!strcmp(argv[i], "-l"))
            {
                list_only = 1;
            }
            else
            {
                fprintf(stderr, "%s: unrecognized switch \"%s\"\n", argv[0], argv[i]);
            }

            for(j = i + 1; j < argc; j++)
                argv[j-1] = argv[j];

            argc--;
        }
        /*
        else if(!package_name)
        {
            package_name = argv[i];
        }
        else if(!file_filters)
        {
            file_filters = &(argv[i]);
        }
        */
    }

    //DEBUGPRINT("%s: list_only == %d\n", __func__, list_only);
    //DEBUGPRINT("%s: package_name == \"%s\"\n", __func__, package_name);
    //DEBUGPRINT("%s: file_filters == %p (%d)\n", __func__, file_filters, argc - (file_filters - argv));

    if(argc < 2) return showUsage();

    for(i = 1; i < argc; i++)
    {
        fprintf(stderr, "%s:\n", argv[i]);

        pl2Package *pkg = pl2PackageOpen(argv[i]);

        if(!pkg)
        {
            perror("error opening package");
            continue;
        }

        char dir[FILENAME_MAX];

        if(!list_only)
        {
            snprintf(dir, sizeof(dir), "%s_files", argv[i]);

            //DEBUGPRINT("%s: creating \"%s\"\n", __func__, dir);

            if((mkdir(dir) < 0) && (errno != EEXIST))
            {
                perror("error creating directory");
            }
        }

        for(j = 0; j < pkg->numEntries; j++)
        {
            pl2PackageFile *file = pl2PackageReadIndex(pkg, j);

            if(!file)
            {
                fprintf(stderr, "%s: warning: failed reading #%d \"%s\"\n", argv[0], j, pkg->entries[j].name);
                continue;
            }

            if(list_only)
            {
                fprintf(stderr, "\t%s", file->name);
            }
            else
            {
                fprintf(stderr, "\t%s ... ", file->name);

                char fn[FILENAME_MAX];
                snprintf(fn, sizeof(fn), "%s/%s", dir, file->name);

                //DEBUGPRINT("%s: writing \"%s\"\n", __func__, fn);

                FILE *f = fopen(fn, "wb");

                if(f == NULL)
                {
                    perror("error opening file");
                }

                fwrite(file->data, file->length, 1, f);

                fclose(f);
            }

            pl2PackageFileFree(file);

            fprintf(stderr, "\n");
        }

        pl2PackageFree(pkg);
    }

    return 0;
}
