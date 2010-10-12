#ifndef __OPL2_H__
#define __OPL2_H__

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <sys/param.h>

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************/

#define PL2_TMB_MAGIC ((uint32_t)(0x30424d54)) /* "TMB0" */
#define PL2_TSB_MAGIC ((uint32_t)(0x30425354)) /* "TSB0" */

/******************************************************************************/

#define PL2_MAX_CHARPARTS 16
#define PL2_MAX_MENUITEMS 8

#define PL2_MAX_CHARS 4
#define PL2_MAX_LIGHTS 4
#define PL2_MAX_CAMERAS 2
#define PL2_MAX_LAYERS 2

enum pl2ErrorCode
{
    PL2_ERR_NONE     =    0,
    PL2_ERR_MEMORY   =   -1,
    PL2_ERR_NOTFOUND =   -2,
    PL2_ERR_FILEIO   =   -3,
    PL2_ERR_FORMAT   =   -4,
    PL2_ERR_PARAM    =   -5,
    PL2_ERR_INTERNAL = -100,
};

enum pl2CharPart
{
    PL2_PART_BODY,
    PL2_PART_EYE,
    PL2_PART_UNDER_A,
    PL2_PART_UNDER_B,
    PL2_PART_SOCKS,
    PL2_PART_COS_A,
    PL2_PART_COS_B,
    PL2_PART_HEAD,
    PL2_PART_FACE,
    PL2_PART_NECK,
    PL2_PART_ARM,
    PL2_PART_SHOES,
    PL2_PART_HAIR,
};

enum pl2SoundChannel
{
    PL2_CHAN_VOICE,
    PL2_CHAN_SOUND,
    PL2_CHAN_BGSOUND,
    PL2_CHAN_MUSIC,
};

/******************************************************************************/

typedef struct
{
    float u, v;
}
ftexcoord2_t;

typedef struct
{
    float x, y, z;
}
__attribute__((packed))
fvector3_t;

typedef struct
{
    float x, y, z, w;
}
__attribute__((packed))
fvector4_t;

typedef struct
{
    fvector3_t x, y, z;
}
__attribute__((packed))
fmatrix3_t;

typedef struct
{
    fvector4_t x, y, z, w;
}
__attribute__((packed))
fmatrix4_t;

typedef struct
{
    float r, g, b, a;
}
__attribute__((packed))
fcolor4_t;

/******************************************************************************/

typedef struct
{
    uint32_t reserved[4];
}
__attribute__((packed))
pl2PackageHeader;

typedef struct
{
    char name[32];
    uint32_t offset;
    uint32_t packedLength;
    uint32_t length;
    uint32_t reserved;
}
__attribute__((packed))
pl2PackageEntry;

typedef struct
{
    FILE *file;
    char filename[FILENAME_MAX];
    uint32_t numEntries;
    pl2PackageHeader header;
    pl2PackageEntry entries[1];
}
pl2Package;

typedef struct
{
    char name[32];
    uint32_t length;
    uint8_t data[];
}
pl2PackageFile;

/******************************************************************************/

typedef struct
{
    uint32_t magic;
    uint32_t reserved[3];
    uint32_t numBones;
    uint32_t numFrames;
    uint32_t loopFrame;
    uint32_t numUnknown;
    fmatrix4_t *bones;
    fmatrix3_t *unknown;
}
__attribute__((packed))
pl2Anim;

/******************************************************************************/

typedef struct
{
    char name[32];
    uint16_t width, height;
    uint8_t *pixels;
    uint32_t flags;
}
__attribute__((packed))
pl2Texture;

typedef struct
{
    fcolor4_t ambient;
    fcolor4_t diffuse;
    fcolor4_t specular;
    fcolor4_t emissive;
    float shininess;
    pl2Texture *texture;
}
__attribute__((packed))
pl2Material;

typedef struct
{
    fvector3_t vertex; //float x, y, z;
    float weights[3];
    uint8_t bones[4];
    fvector3_t normal; //float nx, ny, nz;
    uint32_t color;
    ftexcoord2_t texcoord; //float u, v;
}
__attribute__((packed))
pl2Vertex;

typedef struct
{
    ftexcoord2_t texcoord; //float u, v;
    //uint32_t color;
    fvector3_t normal; //float nx, ny, nz;
    fvector3_t vertex; //float x, y, z;
}
__attribute__((packed))
pl2GlVertex;

typedef struct
{
    pl2Material *material;
    int32_t start;
    uint32_t count;
}
__attribute__((packed))
pl2ObjMtl;

typedef struct
{
    char name[32];
    fmatrix4_t transform;
    uint32_t numTriangles;
    uint32_t reserved;
    uint32_t numMaterials;
    pl2ObjMtl *materials;
    pl2Vertex *vertices;
    pl2GlVertex *glVertices;
}
__attribute__((packed))
pl2Object;

typedef struct
{
    float reserved[7];
    float unknown[3];
}
__attribute__((packed))
pl2Point;

typedef struct
{
    uint32_t magic;
    uint32_t numTextures;
    uint32_t numMaterials;
    uint32_t numObjects;
    uint32_t numBones;
    uint32_t numPoints;
    pl2Texture *textures;
    pl2Material *materials;
    pl2Object *objects;
    fmatrix4_t *bones;
}
__attribute__((packed))
pl2Model;

/******************************************************************************/

typedef struct
{
    fvector3_t position;
    fcolor4_t ambient;
    fcolor4_t diffuse;
    fcolor4_t specular;
    bool enabled;
}
pl2Light;

typedef struct
{
    fvector3_t eye;
    fvector3_t focus;
    float fov;
}
pl2CameraFrame;

typedef struct
{
    uint32_t numFrames;
    pl2CameraFrame frames[];
}
pl2CameraPath;

typedef struct
{
    fvector3_t eye;
    fvector3_t focus;
    fvector3_t up;
    float fov;
    bool loop;
    bool locked;
    pl2CameraPath *path;
    float time;
}
pl2Camera;

typedef struct
{
    pl2Model *models[PL2_MAX_CHARPARTS];
    pl2Anim *anim;
    uint32_t frame;
    float time;
    bool visible;
}
pl2Character;

typedef struct
{
    float fade_target;
    float fade_level;
    float fade_length;
    float fade_time;
}
pl2Layer;

typedef struct
{
    uint16_t text[64];
}
pl2MenuItem;

typedef struct
{
    uint32_t selection;
    pl2MenuItem items[PL2_MAX_MENUITEMS];
}
pl2Menu;

/******************************************************************************/

int pl2GetErrorCode();
const char *pl2GetErrorMessage(int code);
void pl2ClearError();

/******************************************************************************/

/**
 * Build PL2 package index.
 */
int pl2PackageBuildIndex();
/**
 * Clear PL2 package index.
 */
void pl2PackageClearIndex();
/**
 * Get package entry for a filename.
 */
pl2PackageFile *pl2PackageGetFile(const char *name);

/**
 * Open package and read contents.
 */
pl2Package *pl2PackageOpen(const char *filename);
/**
 * Reopen package file (but read nothing).
 */
int pl2PackageReopen(pl2Package *package);
/**
 * Close package (but keep contents).
 */
void pl2PackageClose(pl2Package *package);
/**
 * Close package and free contents.
 */
void pl2PackageFree(pl2Package *package);

/**
 * Read file entry from a package.
 */
pl2PackageFile *pl2PackageRead(pl2Package *package, const char *name);
/**
 * Free package file entry.
 */
void pl2PackageFileFree(pl2PackageFile *file);

/******************************************************************************/

pl2Anim *pl2AnimLoad(const char *name);
void pl2AnimFree(pl2Anim *anim);

/******************************************************************************/

pl2Model *pl2ModelLoad(const char *name);
void pl2ModelFree(pl2Model *model);

/******************************************************************************/

pl2CameraPath *pl2CameraPathLoad(const char *name);
void pl2CameraPathFree(pl2CameraPath *path);

/******************************************************************************/

void pl2CharacterSetanim(pl2Character *character, pl2Anim *anim);
void pl2CharacterUpdate(pl2Character *character, float dt);
void pl2CharacterDraw(pl2Character *character);

/******************************************************************************/

void pl2CameraSetPath(pl2Camera *camera, pl2CameraPath *path);
void pl2CameraUpdate(pl2Camera *camera, float dt);
void pl2CameraDraw(pl2Camera *camera);
void pl2CameraRotate1P(pl2Camera *camera, float angle, const fvector3_t *axis);
void pl2CameraRotate3P(pl2Camera *camera, float angle, const fvector3_t *axis);

/******************************************************************************/

void pl2MenuClear(pl2Menu *menu);
void pl2MenuAddItem(pl2Menu *menu, const char *item);
void pl2MenuDraw(pl2Menu *menu);

/******************************************************************************/

void pl2LayerFade(pl2Layer *layer, float target, float length);
void pl2LayerUpdate(pl2Layer *layer, float dt);
void pl2LayerDraw(pl2Layer *layer);

/******************************************************************************/

#ifdef __cplusplus
};
#endif

#endif // __OPL2_H__
