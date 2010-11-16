#include "opl2.h"

static int showUsage()
{
    fprintf(stderr, "Usage: dumptmb <tmbfile>\n");
    return 1;
}

int main(int argc, char *argv[])
{
    const char *filename = NULL;

    int i, j;

    for(i = 1; i < argc; i++)
    {
        if(argv[i][0] == '-')
        {
            //else
            {
                fprintf(stderr, "%s: unrecognized switch \"%s\"\n", argv[0], argv[i]);
            }

            for(j = i + 1; j < argc; j++)
                argv[j-1] = argv[j];

            argc--;
        }
        else if(!filename)
        {
            filename = argv[i];
        }
        else
        {
            fprintf(stderr, "%s: extra arguments on command line\n", argv[0]);
        }
    }

    if(!filename) return showUsage();

    pl2Model *model = pl2ModelLoadFile(filename);

    if(!model)
    {
        fprintf(stderr, "%s: error loading model: %s\n", argv[0], pl2GetErrorMessage(pl2GetErrorCode()));
        return -1;
    }

    printf("Textures (%d):\n", model->numTextures);
    for(i = 0; i < model->numTextures; i++)
    {
        printf("\t#%3d \"%s\": %dx%d\n", i, model->textures[i].name,
               model->textures[i].width, model->textures[i].height);
    }
    putchar('\n');

    printf("Materials (%d):\n", model->numMaterials);
    for(i = 0; i < model->numMaterials; i++)
    {
        printf("\t#%3d: ", i);

        if(model->materials[i].texture)
            printf("texture #%d", model->materials[i].texture - model->textures);

        putchar('\n');

        printf("\t\tAmbient:  <%g, %g, %g, %g>\n",
               model->materials[i].ambient.r,
               model->materials[i].ambient.g,
               model->materials[i].ambient.b,
               model->materials[i].ambient.a);
        printf("\t\tDiffuse:  <%g, %g, %g, %g>\n",
               model->materials[i].diffuse.r,
               model->materials[i].diffuse.g,
               model->materials[i].diffuse.b,
               model->materials[i].diffuse.a);
        printf("\t\tSpecular: <%g, %g, %g, %g>\n",
               model->materials[i].specular.r,
               model->materials[i].specular.g,
               model->materials[i].specular.b,
               model->materials[i].specular.a);
        printf("\t\tEmissive: <%g, %g, %g, %g>\n",
               model->materials[i].emissive.r,
               model->materials[i].emissive.g,
               model->materials[i].emissive.b,
               model->materials[i].emissive.a);
        printf("\t\tShininess: %g\n",
               model->materials[i].shininess);
    }
    putchar('\n');

    pl2ModelFree(model);
    return 0;
}

