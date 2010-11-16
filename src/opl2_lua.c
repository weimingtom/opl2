//#include "opl2.h"
#include "opl2_int.h"

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

/******************************************************************************/

#define luaL_optbool(L,n,d) luaL_opt((L),lua_toboolean,(n),(d))

#define UDATA_TYPE(name,type) \
type *push##name(lua_State *L) \
{ \
    type *p = (type*)lua_newuserdata(L, sizeof(type)); \
    luaL_getmetatable(L, #name); \
    lua_setmetatable(L, -2); \
    return p; \
} \
int is##name(lua_State *L, int n) \
{ \
    lua_getmetatable(L, n); \
    luaL_getmetatable(L, #name); \
    int r = lua_rawequal(L, -1, -2); \
    lua_pop(L, 2); \
    return r; \
} \
type *to##name(lua_State *L, int n) \
{ \
    return (type*)(is##name(L, n) ? lua_touserdata(L, n) : NULL); \
} \
type *check##name(lua_State *L, int n) \
{ \
    return (type*)luaL_checkudata(L, n, #name); \
} \
type *opt##name(lua_State *L, int n, type *p) \
{ \
    return luaL_opt(L, check##name, n, p); \
}

/******************************************************************************/

UDATA_TYPE(pl2Character, pl2Character*);
UDATA_TYPE(pl2Light, pl2Light*);
UDATA_TYPE(pl2Camera, pl2Camera*);
UDATA_TYPE(pl2Layer, pl2Layer*);

/******************************************************************************/

static int l_pl2Character_setModel(lua_State *L)
{
    pl2Character *chr = *checkpl2Character(L, 1);
    int idx = luaL_checkint(L, 2) - 1;
    const char *name = luaL_optstring(L, 3, NULL);

    lua_pushboolean(L, pl2CharSetModel(chr, idx, name));

    return 1;
}

static int l_pl2Character_setModels(lua_State *L)
{
    pl2Character *chr = *checkpl2Character(L, 1);
    luaL_checktype(L, 2, LUA_TTABLE);    
    int keep = lua_toboolean(L, 3);

    int i; int r = 1;

    for(i = 1; i <= PL2_MAX_CHARPARTS; i++)
    {
        lua_rawgeti(L, 2, i);

        const char *name = luaL_optstring(L, -1, NULL);
        
        //DEBUGPRINT("%s: model[%d] == %#s\n", __func__, i, name);
        
        if(name || !keep)
        {
            if(!pl2CharSetModel(chr, i - 1, name))
            {
                r = 0;
            }
        }
        
        lua_pop(L, 1);
    }

    lua_pushboolean(L, r);

    return 1;
}

static int l_pl2Character_setAnim(lua_State *L)
{
    pl2Character *chr = *checkpl2Character(L, 1);
    const char *name = luaL_optstring(L, 2, NULL);

    lua_pushboolean(L, pl2CharSetAnim(chr, name));

    return 1;
}

static int l_pl2Character_setPoint(lua_State *L)
{
    pl2Character *chr = *checkpl2Character(L, 1);
    const char *name = luaL_optstring(L, 2, NULL);

    lua_pushboolean(L, pl2CharSetPoint(chr, name));

    return 1;
}

static int l_pl2Character_setVisible(lua_State *L)
{
    pl2Character *chr = *checkpl2Character(L, 1);

    chr->visible = lua_toboolean(L, 2) ? 1 : 0;

    return 0;
}

static int l_pl2Character_getVisible(lua_State *L)
{
    pl2Character *chr = *checkpl2Character(L, 1);

    lua_pushboolean(L, chr->visible);

    return 1;
}

static int l_pl2Character_setName(lua_State *L)
{
    pl2Character *chr = *checkpl2Character(L, 1);
    const char *name = luaL_optstring(L, 2, NULL);
    
    if(lua_gettop(L) > 2)
    {
        float r, g, b, a;
        r = luaL_checknumber(L, 3);
        g = luaL_checknumber(L, 4);
        b = luaL_checknumber(L, 5);
        a = luaL_optnumber(L, 6, 1);
        
        chr->nameColor = RGBAf(r, g, b, a);
    }

    lua_pushboolean(L, pl2CharSetName(chr, name));
    
    return 1;
}

static int l_pl2Character_getName(lua_State *L)
{
    pl2Character *chr = *checkpl2Character(L, 1);
    
    lua_pushstring(L, chr->name);

    return 1;
}

static int l_pl2Character_setBlack(lua_State *L)
{
    pl2Character *chr = *checkpl2Character(L, 1);

    chr->black = lua_toboolean(L, 2);

    return 0;
}

static int l_pl2Character_getBlack(lua_State *L)
{
    pl2Character *chr = *checkpl2Character(L, 1);

    lua_pushboolean(L, chr->black);

    return 1;
}

static int l_pl2Character_clear(lua_State *L)
{
    pl2Character *chr = *checkpl2Character(L, 1);
    
    pl2CharClear(chr);

    return 0;
}

static int l_pl2Character_say(lua_State *L)
{
    pl2Character *chr = *checkpl2Character(L, 1);
    const char *text = luaL_checkstring(L, 2);

    pl2SetName(chr->name, chr->nameColor);

    pl2SetText(text);

    pl2ShowText();

    return 0;
}

static luaL_Reg pl2Character_methods[] =
{
    { "setModel", l_pl2Character_setModel },
    //{ "getModel", l_pl2Character_getModel },
    { "setModels", l_pl2Character_setModels },
    { "setAnim", l_pl2Character_setAnim },
    //{ "getAnim", l_pl2Character_getAnim },
    { "setPoint", l_pl2Character_setPoint },
    //{ "getPoint", l_pl2Character_getPoint },
    { "setVisible", l_pl2Character_setVisible },
    { "getVisible", l_pl2Character_getVisible },
    { "setName", l_pl2Character_setName },
    { "getName", l_pl2Character_getName },
    { "setBlack", l_pl2Character_setBlack },
    { "getBlack", l_pl2Character_getBlack },
    { "clear", l_pl2Character_clear },
    { NULL, NULL }
};
static luaL_Reg pl2Character_meta[] =
{
    { "__call", l_pl2Character_say },
    { NULL, NULL }
};

/******************************************************************************/

static int l_pl2Light_setPosition(lua_State *L)
{
    pl2Light *light = *checkpl2Light(L, 1);

    float x, y, z;
    x = luaL_checknumber(L, 2);
    y = luaL_checknumber(L, 3);
    z = luaL_checknumber(L, 4);

    light->position.x = x;
    light->position.y = y;
    light->position.z = z;

    return 0;
}

static int l_pl2Light_getPosition(lua_State *L)
{
    pl2Light *light = *checkpl2Light(L, 1);

    lua_pushnumber(L, light->position.x);
    lua_pushnumber(L, light->position.y);
    lua_pushnumber(L, light->position.z);

    return 3;
}

static int l_pl2Light_setAmbient(lua_State *L)
{
    pl2Light *light = *checkpl2Light(L, 1);

    float r, g, b, a;
    r = luaL_checknumber(L, 2);
    g = luaL_checknumber(L, 3);
    b = luaL_checknumber(L, 4);
    a = luaL_optnumber(L, 5, 1);

    light->ambient.r = r;
    light->ambient.g = g;
    light->ambient.b = b;
    light->ambient.a = a;

    return 0;
}

static int l_pl2Light_getAmbient(lua_State *L)
{
    pl2Light *light = *checkpl2Light(L, 1);

    lua_pushnumber(L, light->ambient.r);
    lua_pushnumber(L, light->ambient.g);
    lua_pushnumber(L, light->ambient.b);
    lua_pushnumber(L, light->ambient.a);

    return 4;
}

static int l_pl2Light_setDiffuse(lua_State *L)
{
    pl2Light *light = *checkpl2Light(L, 1);

    float r, g, b, a;
    r = luaL_checknumber(L, 2);
    g = luaL_checknumber(L, 3);
    b = luaL_checknumber(L, 4);
    a = luaL_optnumber(L, 5, 1);

    light->diffuse.r = r;
    light->diffuse.g = g;
    light->diffuse.b = b;
    light->diffuse.a = a;

    return 0;
}

static int l_pl2Light_getDiffuse(lua_State *L)
{
    pl2Light *light = *checkpl2Light(L, 1);

    lua_pushnumber(L, light->diffuse.r);
    lua_pushnumber(L, light->diffuse.g);
    lua_pushnumber(L, light->diffuse.b);
    lua_pushnumber(L, light->diffuse.a);

    return 4;
}

static int l_pl2Light_setSpecular(lua_State *L)
{
    pl2Light *light = *checkpl2Light(L, 1);

    float r, g, b, a;
    r = luaL_checknumber(L, 2);
    g = luaL_checknumber(L, 3);
    b = luaL_checknumber(L, 4);
    a = luaL_optnumber(L, 5, 1);

    light->specular.r = r;
    light->specular.g = g;
    light->specular.b = b;
    light->specular.a = a;

    return 0;
}

static int l_pl2Light_getSpecular(lua_State *L)
{
    pl2Light *light = *checkpl2Light(L, 1);

    lua_pushnumber(L, light->specular.r);
    lua_pushnumber(L, light->specular.g);
    lua_pushnumber(L, light->specular.b);
    lua_pushnumber(L, light->specular.a);

    return 4;
}

static int l_pl2Light_setEnabled(lua_State *L)
{
    pl2Light *light = *checkpl2Light(L, 1);

    light->enabled = lua_toboolean(L, 2);

    return 0;
}

static int l_pl2Light_getEnabled(lua_State *L)
{
    pl2Light *light = *checkpl2Light(L, 1);

    lua_pushboolean(L, light->enabled);

    return 1;
}

static luaL_Reg pl2Light_methods[] =
{
    { "setPosition", l_pl2Light_setPosition },
    { "getPosition", l_pl2Light_getPosition },
    { "setAmbient", l_pl2Light_setAmbient },
    { "getAmbient", l_pl2Light_getAmbient },
    { "setDiffuse", l_pl2Light_setDiffuse },
    { "getDiffuse", l_pl2Light_getDiffuse },
    { "setSpecular", l_pl2Light_setSpecular },
    { "getSpecular", l_pl2Light_getSpecular },
    { "setEnabled", l_pl2Light_setEnabled },
    { "getEnabled", l_pl2Light_getEnabled },
    { NULL, NULL }
};
static luaL_Reg pl2Light_meta[] =
{
    { NULL, NULL }
};

/******************************************************************************/

static int l_pl2Camera_setEye(lua_State *L)
{
    pl2Camera *cam = *checkpl2Camera(L, 1);

    float x, y, z;
    x = luaL_checknumber(L, 2);
    y = luaL_checknumber(L, 3);
    z = luaL_checknumber(L, 4);

    cam->eye.x = x;
    cam->eye.y = y;
    cam->eye.z = z;

    return 0;
}

static int l_pl2Camera_getEye(lua_State *L)
{
    pl2Camera *cam = *checkpl2Camera(L, 1);

    lua_pushnumber(L, cam->eye.x);
    lua_pushnumber(L, cam->eye.y);
    lua_pushnumber(L, cam->eye.z);

    return 3;
}

static int l_pl2Camera_setFocus(lua_State *L)
{
    pl2Camera *cam = *checkpl2Camera(L, 1);

    float x, y, z;
    x = luaL_checknumber(L, 2);
    y = luaL_checknumber(L, 3);
    z = luaL_checknumber(L, 4);

    cam->focus.x = x;
    cam->focus.y = y;
    cam->focus.z = z;

    return 0;
}

static int l_pl2Camera_getFocus(lua_State *L)
{
    pl2Camera *cam = *checkpl2Camera(L, 1);

    lua_pushnumber(L, cam->focus.x);
    lua_pushnumber(L, cam->focus.y);
    lua_pushnumber(L, cam->focus.z);

    return 3;
}

static int l_pl2Camera_setUp(lua_State *L)
{
    pl2Camera *cam = *checkpl2Camera(L, 1);

    float x, y, z;
    x = luaL_checknumber(L, 2);
    y = luaL_checknumber(L, 3);
    z = luaL_checknumber(L, 4);

    cam->up.x = x;
    cam->up.y = y;
    cam->up.z = z;

    return 0;
}

static int l_pl2Camera_getUp(lua_State *L)
{
    pl2Camera *cam = *checkpl2Camera(L, 1);

    lua_pushnumber(L, cam->up.x);
    lua_pushnumber(L, cam->up.y);
    lua_pushnumber(L, cam->up.z);

    return 3;
}

static int l_pl2Camera_setFov(lua_State *L)
{
    pl2Camera *cam = *checkpl2Camera(L, 1);

    cam->fov = luaL_checknumber(L, 2);

    return 0;
}

static int l_pl2Camera_getFov(lua_State *L)
{
    pl2Camera *cam = *checkpl2Camera(L, 1);

    lua_pushnumber(L, cam->fov);

    return 1;
}

static int l_pl2Camera_setPath(lua_State *L)
{
    pl2Camera *cam = *checkpl2Camera(L, 1);
    const char *name = luaL_optstring(L, 2, NULL);
    int loop = lua_toboolean(L, 3);

    if(cam->path) pl2CameraPathFree(cam->path);

    cam->path = pl2CameraPathLoad(name);
    cam->time = 0;
    cam->loop = loop;

    //DEBUGPRINT("%s: cam == %p, cam->path == %p\n", __func__, cam, cam->path);

    lua_pushboolean(L, NULL != cam->path);

    return 1;
}

static int l_pl2Camera_setPoint(lua_State *L)
{
    pl2Camera *cam = *checkpl2Camera(L, 1);
    const char *name = luaL_optstring(L, 2, NULL);

    lua_pushboolean(L, pl2CameraSetPoint(cam, name));

    return 1;
}

static int l_pl2Camera_setLocked(lua_State *L)
{
    pl2Camera *cam = *checkpl2Camera(L, 1);

    cam->locked = lua_toboolean(L, 2);

    return 0;
}

static int l_pl2Camera_getLocked(lua_State *L)
{
    pl2Camera *cam = *checkpl2Camera(L, 1);

    lua_pushboolean(L, cam->locked);

    return 1;
}

static luaL_Reg pl2Camera_methods[] =
{
    { "setEye", l_pl2Camera_setEye },
    { "getEye", l_pl2Camera_getEye },
    { "setFocus", l_pl2Camera_setFocus },
    { "getFocus", l_pl2Camera_getFocus },
    { "setUp", l_pl2Camera_setUp },
    { "getUp", l_pl2Camera_getUp },
    { "setFov", l_pl2Camera_setFov },
    { "getFov", l_pl2Camera_getFov },
    { "setPath", l_pl2Camera_setPath },
    //{ "getPath", l_pl2Camera_getPath },
    { "setPoint", l_pl2Camera_setPoint },
    //{ "getPoint", l_pl2Camera_getPoint },
    { "setLocked", l_pl2Camera_setLocked },
    { "getLocked", l_pl2Camera_getLocked },
    { NULL, NULL }
};
static luaL_Reg pl2Camera_meta[] =
{
    { NULL, NULL }
};

/******************************************************************************/

static int l_pl2Layer_fade(lua_State *L)
{
    pl2Layer *layer = *checkpl2Layer(L, 1);

    float target = luaL_checknumber(L, 2);
    float length = luaL_checknumber(L, 3);

    layer->fade_target = (target < 0) ? 0 : (target > 1) ? 1 : target;
    layer->fade_length = (length < 0) ? 0 : length;
    layer->fade_time   = 0;
    return 0;
}

static luaL_Reg pl2Layer_methods[] =
{
    { "fade", l_pl2Layer_fade },
    { NULL, NULL }
};
static luaL_Reg pl2Layer_meta[] =
{
    { NULL, NULL }
};

/******************************************************************************/

static int l_pl2_character(lua_State *L)
{
    int idx = luaL_checkint(L, 1);

    if((idx < 1) || (idx > PL2_MAX_CHARS))
        return 0;

    *pushpl2Character(L) = &(pl2_chars[idx-1]);
    return 1;
}

static int l_pl2_light(lua_State *L)
{
    int idx = luaL_checkint(L, 1);

    if((idx < 1) || (idx > PL2_MAX_LIGHTS))
        return 0;

    *pushpl2Light(L) = &(pl2_lights[idx-1]);
    return 1;
}

static int l_pl2_camera(lua_State *L)
{
    int idx = luaL_checkint(L, 1);

    if((idx < 1) || (idx > PL2_MAX_CAMERAS))
        return 0;

    *pushpl2Camera(L) = &(pl2_cameras[idx-1]);
    return 1;
}

static int l_pl2_layer(lua_State *L)
{
    int idx = luaL_checkint(L, 1);

    if((idx < 1) || (idx > PL2_MAX_LAYERS))
        return 0;

    *pushpl2Layer(L) = &(pl2_layers[idx-1]);
    return 1;
}

static int l_pl2_play(lua_State *L)
{
    static const char *channels[] = { "voice", "sound", "bgsound", "music", NULL };

    int chan;

    if(lua_type(L, 1) == LUA_TNUMBER)
    {
        chan = luaL_checkint(L, 1) - 1;
    }
    else
    {
        chan = luaL_checkoption(L, 1, NULL, channels);
    }

    const char *name = luaL_optstring(L, 2, NULL);
    
    //float fade = luaL_optnumber(L, 3, 0);

    pl2Sound *sound = pl2SoundLoad(name);

    pl2SoundPlay(sound, chan);

    lua_pushboolean(L, NULL != sound);

    return 1;
}

static int l_pl2_showText(lua_State *L)
{
    const char *text = luaL_checkstring(L, 1);

    pl2SetName(NULL, 0);

    pl2SetText(text);

    pl2ShowText();

    return 0;
}

static int l_pl2_showMenu(lua_State *L)
{
    luaL_checktype(L, 1, LUA_TTABLE);

    pl2MenuClear(&pl2_menu);

    int i, n = MIN(PL2_MAX_MENUITEMS, lua_objlen(L, 1));
    for(i = 1; i <= n; i++)
    {
        lua_rawgeti(L, 1, i);

        pl2MenuAddItem(&pl2_menu, luaL_checkstring(L, -1));

        lua_pop(L, 1);
    }

    int r = pl2ShowMenu(&pl2_menu);

    if(r >= 0)
    {
        lua_pushinteger(L, r + 1);
        return 1;
    }

    return 0;
}

static int l_pl2_wait(lua_State *L)
{
    float time = luaL_checknumber(L, 1);

    pl2Wait(time);

    return 0;
}

static int l_pl2_quit(lua_State *L)
{
    pl2Quit();

    return 0;
}

static int l_pl2_setImage(lua_State *L)
{
    const char *name = luaL_checkstring(L, 1);
    
    pl2SetImage(name);

    return 0;
}

static int l_pl2_setQuit(lua_State *L)
{
    pl2_can_quit = lua_toboolean(L, 1);

    return 0;
}

static int l_pl2_getQuit(lua_State *L)
{
    lua_pushboolean(L, pl2_can_quit);

    return 1;
}

static int l_pl2_setTitle(lua_State *L)
{
    pl2_show_title = lua_toboolean(L, 1);

    return 0;
}

static int l_pl2_getTitle(lua_State *L)
{
    lua_pushboolean(L, pl2_show_title);

    return 1;
}

static int l_pl2_setWindow(lua_State *L)
{
    pl2_show_window = lua_toboolean(L, 1);

    return 0;
}

static int l_pl2_getWindow(lua_State *L)
{
    lua_pushboolean(L, pl2_show_window);

    return 1;
}

static int l_pl2_dofile(lua_State *L)
{
    const char *name = luaL_checkstring(L, 1);

    pl2PackageFile *file = pl2PackageGetFile(name);

    if(!file) return 0;

    //DEBUGPRINT("%s: loading \"%s\"\n", __func__, name);

    char temp[32];
    snprintf(temp, sizeof(temp), "@%s", name);

    int r = luaL_loadbuffer(L, (char*)(file->data), file->length, temp);

    pl2PackageFileFree(file);

    if(r) return 0;

    //DEBUGPRINT("%s: calling \"%s\"\n", __func__, name);

    int top = lua_gettop(L);

    if(lua_pcall(L, 0, LUA_MULTRET, 0))
    {
        DEBUGPRINT("%s: error: %s\n", __func__, lua_tostring(L, -1));

        lua_pushnil(L);
        lua_insert(L, -2);
        return 2;
    }

    DEBUGPRINT("%s: returned successfully\n", __func__);
    return lua_gettop(L) - top + 1;
}

static int l_pl2_ucs(lua_State *L)
{
    int i, l = lua_gettop(L);

    luaL_Buffer b;
    luaL_buffinit(L, &b);

    for(i = 1; i <= l; i++)
    {
        int c = luaL_checkint(L, i);

        if(c >= 0x10000)
        {
            luaL_addchar(&b, 0xf0 | ((c >> 18) & 0x07));
            luaL_addchar(&b, 0x80 | ((c >> 12) & 0x3f));
            luaL_addchar(&b, 0x80 | ((c >>  6) & 0x3f));
            luaL_addchar(&b, 0x80 | ((c >>  0) & 0x3f));
        }
        else if(c >= 0x800)
        {
            luaL_addchar(&b, 0xe0 | ((c >> 12) & 0x0f));
            luaL_addchar(&b, 0x80 | ((c >>  6) & 0x3f));
            luaL_addchar(&b, 0x80 | ((c >>  0) & 0x3f));
        }
        else if(c >= 0x80)
        {
            luaL_addchar(&b, 0xc0 | ((c >>  6) & 0x1f));
            luaL_addchar(&b, 0x80 | ((c >>  0) & 0x3f));
        }
        else
        {
            luaL_addchar(&b,        ((c >>  0) & 0x7f));
        }
    }

    luaL_pushresult(&b);

    return 1;
}

static luaL_Reg pl2_functions[] =
{
    { "camera", l_pl2_camera },
    { "character", l_pl2_character },
    { "layer", l_pl2_layer },
    { "light", l_pl2_light },

    { "play", l_pl2_play },
    { "showText", l_pl2_showText },
    { "showMenu", l_pl2_showMenu },
    { "wait", l_pl2_wait },
    { "quit", l_pl2_quit },

    { "setImage", l_pl2_setImage },
    { "setQuit", l_pl2_setQuit },
    { "getQuit", l_pl2_getQuit },
    { "setTitle", l_pl2_setTitle },
    { "getTitle", l_pl2_getTitle },
    { "setWindow", l_pl2_setWindow },
    { "getWindow", l_pl2_getWindow },

    { "dofile", l_pl2_dofile },
    { "ucs", l_pl2_ucs },
    { NULL, NULL }
};

/******************************************************************************/

static char pl2_lua_init[] =
"light1=pl2.light(1)"
"light2=pl2.light(2)"
"camera=pl2.camera(1)"
"imo1=pl2.character(1)"
"imo2=pl2.character(2)"
"ani=pl2.character(3)"
"room=pl2.character(4)"
"back=pl2.layer(1)"
"fore=pl2.layer(2)"
"imo=imo1";

int luaopen_pl2(lua_State *L)
{
    luaL_register(L, "pl2", pl2_functions);
    lua_pop(L, 1);

    luaL_newmetatable(L, "pl2Character");
    lua_newtable(L);
    luaL_register(L, NULL, pl2Character_methods);
    lua_setfield(L, -2, "__index");
    luaL_register(L, NULL, pl2Character_meta);
    lua_pop(L, 1);

    luaL_newmetatable(L, "pl2Light");
    lua_newtable(L);
    luaL_register(L, NULL, pl2Light_methods);
    lua_setfield(L, -2, "__index");
    luaL_register(L, NULL, pl2Light_meta);
    lua_pop(L, 1);

    luaL_newmetatable(L, "pl2Camera");
    lua_newtable(L);
    luaL_register(L, NULL, pl2Camera_methods);
    lua_setfield(L, -2, "__index");
    luaL_register(L, NULL, pl2Camera_meta);
    lua_pop(L, 1);

    luaL_newmetatable(L, "pl2Layer");
    lua_newtable(L);
    luaL_register(L, NULL, pl2Layer_methods);
    lua_setfield(L, -2, "__index");
    luaL_register(L, NULL, pl2Layer_meta);
    lua_pop(L, 1);

    int err = luaL_loadbuffer(L, pl2_lua_init, strlen(pl2_lua_init), "@init.lua");

    if(!err)
    {
        err = lua_pcall(L, 0, 0, 0);
    }

    if(err)
    {
        DEBUGPRINT("%s: Lua error: %s\n", __func__, lua_tostring(L, -1));
    }
    
    return 1;
}

