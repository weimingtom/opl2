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
#define PL2_FNT_MAGIC ((uint32_t)(0x544e4f46)) /* "FONT" */

#define PL2_NOMINAL_SCREEN_WIDTH 800
#define PL2_NOMINAL_SCREEN_HEIGHT 600

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

#define PL2_NUM_CHANNELS 4

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
//__attribute__((packed))
fvector3_t;

typedef struct
{
    float x, y, z, w;
}
//__attribute__((packed))
__attribute__((aligned(16)))
fvector4_t;

typedef struct
{
    fvector3_t x, y, z;
}
//__attribute__((packed))
fmatrix3_t;

typedef struct
{
    fvector4_t x, y, z, w;
}
//__attribute__((packed))
__attribute__((aligned(16)))
fmatrix4_t;

typedef struct
{
    float r, g, b, a;
}
//__attribute__((packed))
fcolor4_t;

/******************************************************************************/

typedef struct
{
    uint32_t reserved[4];
}
//__attribute__((packed))
pl2PackageHeader;

typedef struct
{
    char name[32];
    uint32_t offset;
    uint32_t packedLength;
    uint32_t length;
    uint32_t reserved;
}
//__attribute__((packed))
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
//__attribute__((packed))
pl2Anim;

/******************************************************************************/

typedef struct
{
    char name[32];
    uint16_t width, height;
    uint8_t *pixels;
    uint32_t flags;
}
//__attribute__((packed))
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
//__attribute__((packed))
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
//__attribute__((packed))
pl2Vertex;

typedef struct
{
    ftexcoord2_t texcoord; //float u, v;
    //uint32_t color;
    fvector3_t normal; //float nx, ny, nz;
    fvector3_t vertex; //float x, y, z;
}
//__attribute__((packed))
pl2GlVertex;

typedef struct
{
    pl2Material *material;
    int32_t start;
    uint32_t count;
}
//__attribute__((packed))
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
//__attribute__((packed))
pl2Object;

typedef struct
{
    uint8_t bones[4];
    fvector3_t translate;
    fvector3_t rotate;
    fvector3_t scale;
    char *name;
}
//__attribute__((packed))
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
    pl2Point *points;
}
//__attribute__((packed))
pl2Model;

/******************************************************************************/

typedef struct
{
    uint32_t magic;
    uint32_t numGlyphs;
    uint32_t glyphSize;
    uint32_t *chars;
    uint8_t *glyphs;
}
pl2Font;

/******************************************************************************/

typedef struct
{
    uint32_t left, top, width, height;
    uint8_t *data;
}
pl2ImageLayer;

typedef struct
{
    uint32_t width, height;
    uint16_t channels, bits;
    pl2ImageLayer *layers;
    uint8_t *data;
}
//__attribute__((packed))
pl2Image;

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
    pl2Point *point;
    pl2CameraPath *path;
    float time;
    bool loop;
    bool locked;
}
pl2Camera;

typedef struct
{
    pl2Model *models[PL2_MAX_CHARPARTS];
    pl2Anim *anim;
    pl2Point *point;
    uint32_t frame;
    float time;
    bool visible;
    bool black;
    const char *name;
    uint32_t nameColor;
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
    int enabled;
    uint32_t text[63];
}
pl2MenuItem;

typedef struct
{
    int selection;
    uint32_t numItems;
    pl2MenuItem items[PL2_MAX_MENUITEMS];
}
pl2Menu;

#include <vorbis/vorbisfile.h>

typedef struct
{
    pl2PackageFile *file;
    uint32_t offset;
    OggVorbis_File vf;
    int16_t buffer[4096];
}
pl2Sound;

/******************************************************************************/

int pl2GameInit(int *argc, char *argv[]);
int pl2GameRun();

void pl2Exit();
bool pl2Running();

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
 * Read file entry from a package by name.
 */
pl2PackageFile *pl2PackageRead(pl2Package *package, const char *name);
/**
 * Read file entry from a package by index.
 */
pl2PackageFile *pl2PackageReadIndex(pl2Package *package, int index);
/**
 * Free package file entry.
 */
void pl2PackageFileFree(pl2PackageFile *file);

/******************************************************************************/

pl2Anim *pl2AnimLoad(const char *name);
pl2Anim *pl2AnimLoadFile(const char *name);
void pl2AnimFree(pl2Anim *anim);

/******************************************************************************/

pl2Model *pl2ModelLoad(const char *name);
pl2Model *pl2ModelLoadFile(const char *name);
void pl2ModelFree(pl2Model *model);
int pl2ModelAddPoints(pl2Model *model);
void pl2ModelAnimate(pl2Model *model, const pl2Anim *anim, uint32_t frame);

/******************************************************************************/

pl2Image *pl2ImageLoad(const char *name);
void pl2ImageFree(pl2Image *image);
void pl2ImageDraw(pl2Image *image, int x, int y, int cx, int cy, float alpha);

/******************************************************************************/

pl2Sound *pl2SoundLoad(const char *name);
void pl2SoundFree(pl2Sound *sound);
void pl2SoundPlay(pl2Sound *sound, int channel);

/******************************************************************************/

pl2CameraPath *pl2CameraPathLoad(const char *name);
pl2CameraPath *pl2CameraPathLoadFile(const char *name);
void pl2CameraPathFree(pl2CameraPath *path);

/******************************************************************************/

pl2Font *pl2FontLoad(const char *name);
void pl2FontFree(pl2Font *font);
int pl2FontMeasureText(pl2Font *font, const char *text);
void pl2FontUcsPrint(pl2Font *font, float x, float y, uint32_t color, const uint32_t *text);
void pl2FontUcsPrintEx(pl2Font *font, float x, float y, uint32_t color, const uint32_t *text, size_t len);
void pl2FontUcsPrintCenter(pl2Font *font, float x, float y, uint32_t color, const uint32_t *text);
void pl2FontUcsPrintRight(pl2Font *font, float x, float y, uint32_t color, const uint32_t *text);
void pl2FontPrint(pl2Font *font, float x, float y, uint32_t color, const char *text);
void pl2FontPrintCenter(pl2Font *font, float x, float y, uint32_t color, const char *text);
void pl2FontPrintRight(pl2Font *font, float x, float y, uint32_t color, const char *text);

size_t pl2Ucs4Strlen(const uint32_t *text);
size_t pl2Utf8Strlen(const char *text);
size_t pl2Utf8ToUcs4(uint32_t *ucs, size_t size, const char *text, int length);

/******************************************************************************/

void pl2CharAnimate(pl2Character *character, float dt);
void pl2CharRender(pl2Character *character);
int pl2CharSetName(pl2Character *chr, const char *name);
int pl2CharSetModel(pl2Character *chr, int idx, const char *name);
int pl2CharSetAnim(pl2Character *chr, const char *name);
int pl2CharSetPoint(pl2Character *chr, const char *name);
void pl2CharClear(pl2Character *chr);

/******************************************************************************/

void pl2CameraSetPath(pl2Camera *camera, pl2CameraPath *path);
void pl2CameraUpdate(pl2Camera *camera, float dt);
void pl2CameraDraw(pl2Camera *camera);
void pl2CameraRotate1P(pl2Camera *camera, float xr, float yr);
void pl2CameraRotate3P(pl2Camera *camera, float xr, float yr);
void pl2CameraZoom(pl2Camera *camera, float dist);
int pl2CameraSetPoint(pl2Camera *camera, const char *name);

/******************************************************************************/

void pl2MenuClear(pl2Menu *menu);
int pl2MenuAddItem(pl2Menu *menu, const char *item);
void pl2MenuDraw(pl2Menu *menu);
int pl2MenuSelect(pl2Menu *menu, uint32_t item);
int pl2ShowMenu();

int pl2MenuSelectNext(pl2Menu *menu);
int pl2MenuSelectPrev(pl2Menu *menu);
int pl2MenuConfirm(pl2Menu *menu);

/******************************************************************************/

void pl2LayerFade(pl2Layer *layer, float target, float length);
void pl2LayerUpdate(pl2Layer *layer, float dt);
void pl2LayerDraw(pl2Layer *layer);

/******************************************************************************/

void pl2SetText(const char *text);
void pl2SetName(const char *name, uint32_t color);
void pl2ShowText();
void pl2TextAdvance();
void pl2ToggleOverlay();

void pl2Wait(float sec);
void pl2Quit();

/******************************************************************************/

int pl2SetImage(const char *name);

/******************************************************************************/

#ifdef __cplusplus
};
#endif

#endif // __OPL2_H__

