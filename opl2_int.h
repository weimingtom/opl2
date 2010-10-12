#ifndef __OPL2_INT_H__
#define __OPL2_INT_H__

/******************************************************************************
 * OPL2 Internals
 ******************************************************************************/

#ifndef NDEBUG
# define DEBUGPRINT(x...) (fprintf(stderr,x))
#else
# define DEBUGPRINT(x...)
#endif

#define STUB() DEBUGPRINT("%s(%d): %s is a stub function!\n", __FILE__, __LINE__, __func__);

int pl2_strlcpy(char *dst, const char *src, int len);

/******************************************************************************/

#define NEWOBJ(t)   ((t*)calloc(sizeof(t),1))
#define NEWARR(n,t) ((t*)calloc(sizeof(t),(n)))
#define NEWVAR(t,x) ((t*)calloc(sizeof(t)+(x),1))
#define NEW NEWOBJ

#define ARRSIZE(n,t) (sizeof(t)*(n))
#define VARSIZE(t,x) (sizeof(t)+(x))

#define DELETE(p) (free((p)),(p)=0)

/******************************************************************************/

extern int pl2_error;
#define PL2_SET_ERROR(err) ({if(!pl2_error){pl2_error=(err);}})
#define PL2_CLEAR_ERROR() (pl2_error=PL2_ERR_NONE)
#define PL2_ERROR(err) ({PL2_SET_ERROR((err));return 0;})

extern int pl2_screen_width, pl2_screen_height;
extern float pl2_screen_aspect, pl2_screen_scale;

extern pl2Character pl2_chars[PL2_MAX_CHARS];
extern pl2Light pl2_lights[PL2_MAX_LIGHTS];
extern pl2Camera pl2_cameras[PL2_MAX_CAMERAS];
extern pl2Layer pl2_layers[PL2_MAX_LAYERS];

/******************************************************************************/

/* TODO: make these endian-portable */

#define READUINT8(p)  (*(uint8_t *)(p)++)
#define READUINT16(p) ({uint8_t x = READUINT8(p), y = READUINT8(p); x | (y << 8);})
#define READUINT32(p) ({uint16_t x = READUINT16(p), y = READUINT16(p); x | (y << 16);})

#define READSTRING(n,o,p) { memcpy((o), (p), (n)); (p) = (uint8_t*)(p) + (n); }
#define READFLOAT(p)  ({union{float f; uint32_t i;}t; t.i = READUINT32(p); t.f;})

#define READTEXCOORD2(_T,p) { \
    ftexcoord2_t *_t = (ftexcoord2_t*)&(_T); \
    _t->u = READFLOAT(p); \
    _t->v = READFLOAT(p); \
    }
#define READVECTOR3(_V,p) { \
    fvector3_t *_v = (fvector3_t*)&(_V); \
    _v->x = READFLOAT(p); \
    _v->y = READFLOAT(p); \
    _v->z = READFLOAT(p); \
    }
#define READVECTOR4(_V,p) { \
    fvector4_t *_v = (fvector4_t*)&(_V); \
    _v->x = READFLOAT(p); \
    _v->y = READFLOAT(p); \
    _v->z = READFLOAT(p); \
    _v->w = READFLOAT(p); \
    }
#define READMATRIX3(_M,p) { \
    fmatrix3_t *_m = (fmatrix3_t*)&(_M); \
    READVECTOR3(_m->x, p); \
    READVECTOR3(_m->y, p); \
    READVECTOR3(_m->z, p); \
    }
#define READMATRIX4(_M,p) { \
    fmatrix4_t *_m = (fmatrix4_t*)&(_M); \
    READVECTOR4(_m->x, p); \
    READVECTOR4(_m->y, p); \
    READVECTOR4(_m->z, p); \
    READVECTOR4(_m->w, p); \
    }
#define READCOLOR4(_C,p) { \
    fcolor4_t *_c = (fcolor4_t*)&(_C); \
    _c->r = READFLOAT(p); \
    _c->g = READFLOAT(p); \
    _c->b = READFLOAT(p); \
    _c->a = READFLOAT(p); \
    }

/******************************************************************************/

#endif // __OPL2_INT_H__