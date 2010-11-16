#ifndef __OPL2_INT_H__
#define __OPL2_INT_H__

/******************************************************************************
 * OPL2 Internals
 ******************************************************************************/

#include <SDL/SDL.h>
#include <SDL/SDL_opengl.h>

#ifndef NDEBUG
# define DEBUGPRINT(x...) (fprintf(stderr,x))
#else
# define DEBUGPRINT(x...)
#endif

#include <math.h>
#include <errno.h>

#ifndef MIN
# define MIN(a,b) ((a)<(b)?(a):(b))
#endif
#ifndef MAX
# define MAX(a,b) ((a)>(b)?(a):(b))
#endif

#define DEG(r) ((r)*180.0f/M_PI)
#define RAD(d) ((d)*M_PI/180.0f)

#define RGBAf(r,g,b,a) ((uint32_t)( \
    ((uint8_t)(255.0f*(r)) <<  0) | \
    ((uint8_t)(255.0f*(g)) <<  8) | \
    ((uint8_t)(255.0f*(b)) << 16) | \
    ((uint8_t)(255.0f*(a)) << 24)))

int pl2_strlcpy(char *dst, const char *src, int len);

/******************************************************************************/

#if defined(__MINGW32__) || defined(__WIN32__)
# include <mm_malloc.h>
# define memalign(a,x) _mm_malloc(x,a)
# undef DELETE
#endif

#define NEWOBJ(t)   ((t*)calloc(sizeof(t),1))
#define NEWARR(n,t) ((t*)calloc(sizeof(t),(n)))
#define NEWALIGN(a,n,t) ((t*)memalign((a),sizeof(t)*(n)))
#define NEWVAR(t,x) ((t*)calloc(sizeof(t)+(x),1))
#define NEW NEWOBJ

#define ARRLEN(a) (sizeof(a)/sizeof((a)[0]))
#define ARRSIZE(n,t) (sizeof(t)*(n))
#define VARSIZE(t,x) (sizeof(t)+(x))

#define DELETE(p) (free((p)),(p)=0)

/******************************************************************************/

#include "opl2.h"

/******************************************************************************/

#if _PSP_FW_VERSION

uint32_t pl2GetFreeRam();

# define PRINTFREERAM() DEBUGPRINT("%s(%d): %s: %u kB free\n", \
                        __FILE__, __LINE__, __func__, pl2GetFreeRam()>>10)
#else
# define PRINTFREERAM()

void pl2DetectSSE();

#endif

#define STUB() DEBUGPRINT("%s(%d): %s is a stub function!\n", \
               __FILE__, __LINE__, __func__)

/******************************************************************************/

extern int pl2_error;
#define PL2_SET_ERROR(err) ({if(!pl2_error){pl2_error=(err);}})
#define PL2_CLEAR_ERROR() (pl2_error=PL2_ERR_NONE)
#define PL2_ERROR(err) ({PL2_SET_ERROR((err));return 0;})

extern int pl2_screen_width, pl2_screen_height;
extern float pl2_screen_aspect, pl2_screen_scale;

extern bool pl2_censor, pl2_running, pl2_can_quit;
extern bool pl2_show_window, pl2_show_title;
extern bool pl2_text_showing, pl2_menu_showing, pl2_hide_overlay;

extern pl2Font *pl2_font;
extern pl2Character pl2_chars[PL2_MAX_CHARS];
extern pl2Light pl2_lights[PL2_MAX_LIGHTS];
extern pl2Camera pl2_cameras[PL2_MAX_CAMERAS];
extern pl2Layer pl2_layers[PL2_MAX_LAYERS];
extern pl2Menu pl2_menu;
extern uint32_t pl2_text[];
extern uint32_t pl2_name_text[];
extern uint32_t pl2_name_color;

extern pl2Camera *pl2_active_camera;
extern pl2Image *pl2_current_image;

/******************************************************************************/

// (READ*BE macros used for reading PSD images)

#define READUINT8(p)  (*(uint8_t *)(p)++)
#define READUINT16LE(p) ({uint8_t x = READUINT8(p), y = READUINT8(p); x | (y << 8);})
#define READUINT16BE(p) ({uint8_t x = READUINT8(p), y = READUINT8(p); y | (x << 8);})
#define READUINT32LE(p) ({uint16_t x = READUINT16LE(p), y = READUINT16LE(p); x | (y << 16);})
#define READUINT32BE(p) ({uint16_t x = READUINT16BE(p), y = READUINT16BE(p); y | (x << 16);})

#define READUINT16(p) READUINT16LE(p)
#define READUINT32(p) READUINT32LE(p)

#define READINT16LE(p) ((int16_t)READUINT16LE(p))
#define READINT16BE(p) ((int16_t)READUINT16BE(p))
#define READINT32LE(p) ((int32_t)READUINT32LE(p))
#define READINT32BE(p) ((int32_t)READUINT32BE(p))

#define READINT16(p) READINT16LE(p)
#define READINT32(p) READINT32LE(p)

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

