#ifndef __OPL2_H__
#define __OPL2_H__

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <sys/param.h>

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************/

#define PL2_MAX_CHARPARTS 16
#define PL2_MAX_MENUITEMS 8

enum pl2ErrorCode
{
	PL2_ERR_NONE,
	PL2_ERR_MEMORY,
	PL2_ERR_NOTFOUND,
	PL2_ERR_FILEIO,
	PL2_ERR_FORMAT,
	PL2_ERR_PARAM,
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
	uint32_t length;
	uint32_t declen;
	uint32_t reserved;
}
__attribute__((packed))
pl2PackageEntry;

typedef struct
{
	FILE *fp;
	char filename[MAXPATHLEN];
	uint32_t nentries;
	pl2PackageHeader header;
	pl2PackageEntry entries[];
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
	uint32_t nbones;
	uint32_t nframes;
	uint32_t loopframe;
	uint32_t nspecials;
	fmatrix4_t *bones;
	fmatrix3_t *specials;
}
__attribute__((packed))
pl2Sequence;

/******************************************************************************/

typedef struct
{
	char name[32];
	uint16_t hsize, vsize;
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
	fcolor4_t emission;
	float shininess;
	union {
		uint32_t texid;
		pl2Texture *texture;
	};
}
__attribute__((packed))
pl2Material;

typedef struct
{
	fvector3_t vertex;
	float weights[3];
	uint8_t bones[4];
	fvector3_t normal;
	uint32_t color;
	ftexcoord2_t texcoord;
}
__attribute__((packed))
pl2Vertex;

typedef struct
{
	ftexcoord2_t texcoord;
	uint32_t color;
	fvector3_t normal;
	fvector3_t vertex;
}
__attribute__((packed))
pl2GlVertex;

typedef struct
{
	union {
		uint32_t mtlid;
		pl2Material *material;
	};
	int32_t start;
	uint32_t count;
}
__attribute__((packed))
pl2ObjMtl;

typedef struct
{
	char name[32];
	fmatrix4_t transform;
	uint32_t ntriangles;
	uint32_t reserved;
	uint32_t nmaterials;
	pl2ObjMtl *materials;
	pl2Vertex *vertices;
	pl2GlVertex *glvertices;
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
	uint32_t ntextures;
	uint32_t nmaterials;
	uint32_t nobjects;
	uint32_t nbones;
	uint32_t npoints;
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
	fvector3_t center;
	float fov;
}
pl2CameraFrame;

typedef struct
{
	uint32_t nframes;
	pl2CameraFrame frames[];
}
pl2CameraPath;

typedef struct
{
	fvector3_t eye;
	fvector3_t center;
	fvector3_t up;
	float fov;
	bool loop, lock;
	pl2CameraPath *path;
	float time;
}
pl2Camera;

typedef struct
{
	pl2Model *models[PL2_MAX_CHARPARTS];
	pl2Sequence *sequence;
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
 * Builds an index of PL2 packages in the `add-ons' directory.
 */
int pl2PackageBuildIndex();
void pl2PackageClearIndex();
pl2PackageFile *pl2PackageGetFile(const char *name);

pl2Package *pl2PackageOpen(const char *filename);
int pl2PackageReopen(pl2Package *package);
void pl2PackageClose(pl2Package *package);
void pl2PackageFree(pl2Package *package);

pl2PackageFile *pl2PackageRead(pl2Package *package, const char *name);
void pl2PackageFileFree(pl2PackageFile *file);

/******************************************************************************/

pl2Sequence *pl2SequenceLoad(const char *name);
void pl2SequenceFree(pl2Sequence *sequence);

/******************************************************************************/

pl2Model *pl2ModelLoad(const char *name);
void pl2ModelFree(pl2Model *model);

/******************************************************************************/

pl2CameraPath *pl2CameraPathLoad(const char *name);
void pl2CameraPathFree(pl2CameraPath *path);

/******************************************************************************/

void pl2CharacterSetSequence(pl2Character *character, pl2Sequence *sequence);
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

#define NEWARRAY(n,t) ((t*)calloc(sizeof(t),(n)))
#define NEW(t)        ((t*)calloc(sizeof(t),1))
#define NEWVAR(t,x)   ((t*)calloc(sizeof(t)+(x),1))

/******************************************************************************/

/* TODO: make these endian-portable */

#define READUINT8(p)  (*(uint8_t *)(p)++)
#define READUINT16(p) (*(uint16_t*)(p)++)
#define READUINT32(p) (*(uint32_t*)(p)++)

#define READSTRING(n,o,p) { memcpy(o, p, n); p = (uint8_t*)(p) + n; }
#define READFLOAT(p)  (*(float*)(p)++)

#define READTEXCOORD2(T,p) { \
	ftexcoord2_t *t = (ftexcoord2_t*)(T); \
	t->u = READFLOAT(p); \
	t->v = READFLOAT(p); \
	}
#define READVECTOR3(V,p) { \
	fvector3_t *v = (fvector3_t*)(V); \
	v->x = READFLOAT(p); \
	v->y = READFLOAT(p); \
	v->z = READFLOAT(p); \
	}
#define READVECTOR4(V,p) { \
	fvector4_t *v = (fvector4_t*)(V); \
	v->x = READFLOAT(p); \
	v->y = READFLOAT(p); \
	v->z = READFLOAT(p); \
	v->w = READFLOAT(p); \
	}
#define READMATRIX3(M,p) { \
	fmatrix3_t *m = (fmatrix3_t*)(M); \
	READVECTOR3(&(m->x), p); \
	READVECTOR3(&(m->y), p); \
	READVECTOR3(&(m->z), p); \
	}
#define READMATRIX4(M,p) { \
	fmatrix4_t *m = (fmatrix4_t*)(M); \
	READVECTOR4(&(m->x), p); \
	READVECTOR4(&(m->y), p); \
	READVECTOR4(&(m->z), p); \
	READVECTOR4(&(m->w), p); \
	}
#define READCOLOR4(C,p) { \
	fcolor4_t *c = (fcolor4_t*)(C); \
	c->r = READFLOAT(p); \
	c->g = READFLOAT(p); \
	c->b = READFLOAT(p); \
	c->a = READFLOAT(p); \
	}

/******************************************************************************/

#ifdef __cplusplus
};
#endif

#endif // __OPL2_H__
