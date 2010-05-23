#ifndef __OPL2_H__
#define __OPL2_H__

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <sys/param.h>

#ifdef __cplusplus
extern "C" {
#endif

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
	float weight[3];
	uint8_t bone[4];
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
	uint32_t nvertices;
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
	uint16_t text[63];
	bool enabled;
}
pl2MenuItem;

typedef struct
{
	uint32_t selection;
	pl2MenuItem items[PL2_MAX_MENUITEMS];
}
pl2Menu;

#ifdef __cplusplus
};
#endif

#endif // __OPL2_H__
