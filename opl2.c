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

/******************************************************************************/

int main(int argc, char *argv[])
{
    pl2GlInit(&argc, argv);
    pl2AlInit(&argc, argv);

    pl2PackageBuildIndex();
    atexit(pl2PackageClearIndex);

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
        return -1;
    }

    void glutMainLoop();
    glutMainLoop();
    return 0;
}
