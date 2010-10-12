#include "opl2.h"
#include "opl2_int.h"
#include "opl2_gl.h"
#include "opl2_al.h"
#include "opl2_lua.h"

#if _PSP_FW_VERSION
# include <pspuser.h>
PSP_MODULE_INFO("opl2psp",0,1,0);
PSP_MAIN_THREAD_ATTR(PSP_THREAD_ATTR_VFPU);
PSP_HEAP_SIZE_MAX();
#endif

int pl2_error = PL2_ERR_NONE;

int pl2_screen_width = -1, pl2_screen_height = -1;
float pl2_screen_aspect = 1, pl2_screen_scale = 1;

pl2Character pl2_chars[PL2_MAX_CHARS] = { {{NULL},NULL,0,0,0} };
pl2Light pl2_lights[PL2_MAX_LIGHTS] = { {{0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},0} };
pl2Camera pl2_cameras[PL2_MAX_CAMERAS] = { {{0,0,0},{0,0,0},{0,1,0},45,0,0,NULL,0} };
pl2Layer pl2_layers[PL2_MAX_LAYERS] = { {0,0,0,0} };

/******************************************************************************/

int pl2_strlcpy(char *dst, const char *src, int len)
{
   --len;
   int i;
   for (i = 0; i < len; ++i)
   {
      dst[i] = src[i];
   }
   dst[i] = 0;
   return i;
}

/******************************************************************************/

void pl2GameInit(int *argc, char *argv[])
{
    pl2GlInit(argc, argv);
    pl2AlInit(argc, argv);

    pl2PackageBuildIndex();
    atexit(pl2PackageClearIndex);
}

int pl2GameRun()
{
    lua_State *L = luaL_newstate();

    luaL_openlibs(L);
    luaopen_pl2(L);

    int err = luaL_loadfile(L, "script.lua");

    if(!err)
    {
        err = lua_pcall(L, 0, 0, 0);
    }

    if(err)
    {
        fprintf(stderr, "Error: %s\n", lua_tostring(L, -1));
        return 0;
    }

    glutMainLoop();
	
	return 1;
}

/******************************************************************************/

void pl2ClearError()
{
    PL2_CLEAR_ERROR();
}

int pl2GetErrorCode()
{
    return pl2_error;
}

const char *pl2GetErrorMessage(int code)
{
    switch (code)
    {
        case PL2_ERR_NONE:
            return "no error";
        case PL2_ERR_MEMORY:
            return "out of memory";
        case PL2_ERR_NOTFOUND:
            return "file not found";
        case PL2_ERR_FILEIO:
            return "file I/O error";
        case PL2_ERR_FORMAT:
            return "data format error";
        case PL2_ERR_PARAM:
            return "invalid parameter";

        default:
            return "unknown error code";
    }
}

/******************************************************************************/

int main(int argc, char *argv[])
{
	pl2GameInit(&argc, argv);
	pl2GameRun();
    return 0;
}
