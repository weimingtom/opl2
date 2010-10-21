#include "opl2.h"
#include "opl2_int.h"
#include "opl2_gl.h"
#include "opl2_al.h"
#include "opl2_lua.h"

#include <ctype.h>

#if _PSP_FW_VERSION
# include <pspuser.h>
PSP_MODULE_INFO("opl2psp",0,1,0);
PSP_MAIN_THREAD_ATTR(PSP_THREAD_ATTR_VFPU);
PSP_HEAP_SIZE_MAX();
#endif

int pl2_screen_width = -1, pl2_screen_height = -1;
float pl2_screen_aspect = 1, pl2_screen_scale = 1;

int pl2_censor = 0;

pl2Font *pl2_font = NULL;
pl2Character pl2_chars[PL2_MAX_CHARS] = { {{NULL},NULL,NULL,0,0,0} };
pl2Light pl2_lights[PL2_MAX_LIGHTS] = { {{0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},0} };
pl2Camera pl2_cameras[PL2_MAX_CAMERAS] = { {{0,0,0},{0,0,0},{0,1,0},45,0,0,NULL,0} };
pl2Layer pl2_layers[PL2_MAX_LAYERS] = { {0,0,0,0} };
pl2Menu pl2_menu = { 0,0, {{0,{0}}} };
uint32_t pl2_text[1024] = { 0 };

int pl2_text_showing = 0, pl2_menu_showing = 0;

/******************************************************************************/

static uint8_t utf8TrailingBytes[] =
{
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // 0x00
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // 0x20
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // 0x40
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // 0x60
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // 0x80
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // 0xA0
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, // 0xC0
    2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2, 3,3,3,3,3,3,3,3,4,4,4,4,5,5,5,5, // 0xE0
};
static uint32_t utf8Offsets[] =
{
    0x00000000, 0x00003080, 0x000e2080,
    0x03c82080, 0xfa082080, 0x82082080
};

size_t pl2Utf8Strlen(const char *text)
{
    if(!text) return 0;

    int len = 0, i = 0;

    while(text[i])
    {
        if((text[i++] & 0xc0) == 0xc0)
        {
            while((text[i++] & 0xc0) == 0x80);
        }
        len++;
    }

    return len;
}

static struct { uint16_t ascii, jascii; } jasciitable[] = {
    { '0', 0xff10 },
    { '1', 0xff11 },
    { '2', 0xff12 },
    { '3', 0xff13 },
    { '4', 0xff14 },
    { '5', 0xff15 },
    { '6', 0xff16 },
    { '7', 0xff17 },
    { '8', 0xff18 },
    { '9', 0xff19 },
    { 'A', 0xff21 },
    { 'B', 0xff22 },
    { 'C', 0xff23 },
    { 'D', 0xff24 },
    { 'E', 0xff25 },
    { 'F', 0xff26 },
    { 'G', 0xff27 },
    { 'H', 0xff28 },
    { 'I', 0xff29 },
    { 'J', 0xff2a },
    { 'K', 0xff2b },
    { 'L', 0xff2c },
    { 'M', 0xff2d },
    { 'N', 0xff2e },
    { 'O', 0xff2f },
    { 'P', 0xff30 },
    { 'Q', 0xff31 },
    { 'R', 0xff32 },
    { 'S', 0xff33 },
    { 'T', 0xff34 },
    { 'U', 0xff35 },
    { 'V', 0xff36 },
    { 'W', 0xff37 },
    { 'X', 0xff38 },
    { 'Y', 0xff39 },
    { 'Z', 0xff3a },
    { 'a', 0xff41 },
    { 'b', 0xff42 },
    { 'c', 0xff43 },
    { 'd', 0xff44 },
    { 'e', 0xff45 },
    { 'f', 0xff46 },
    { 'g', 0xff47 },
    { 'h', 0xff48 },
    { 'i', 0xff49 },
    { 'j', 0xff4a },
    { 'k', 0xff4b },
    { 'l', 0xff4c },
    { 'm', 0xff4d },
    { 'n', 0xff4e },
    { 'o', 0xff4f },
    { 'p', 0xff50 },
    { 'q', 0xff51 },
    { 'r', 0xff52 },
    { 's', 0xff53 },
    { 't', 0xff54 },
    { 'u', 0xff55 },
    { 'v', 0xff56 },
    { 'w', 0xff57 },
    { 'x', 0xff58 },
    { 'y', 0xff59 },
    { 'z', 0xff5a },
};

size_t pl2Utf8ToUcs4(uint32_t *ucs, size_t size, const char *text, int length)
{
    uint8_t *utf = (uint8_t*)text;

    int i = 0, j = 0;

    if(length < 0) length = strlen(text);

    size--;

    while((j < size) && (i < length))
    {
        DEBUGPRINT("%s: %02x ", __func__, utf[i]);

        uint32_t c = utf[i++];

        int n = utf8TrailingBytes[c];

        switch(n)
        {
            case 5: { DEBUGPRINT("%02x ", utf[i]); c <<= 6; c += utf[i++]; }
            case 4: { DEBUGPRINT("%02x ", utf[i]); c <<= 6; c += utf[i++]; }
            case 3: { DEBUGPRINT("%02x ", utf[i]); c <<= 6; c += utf[i++]; }
            case 2: { DEBUGPRINT("%02x ", utf[i]); c <<= 6; c += utf[i++]; }
            case 1: { DEBUGPRINT("%02x ", utf[i]); c <<= 6; c += utf[i++]; }
            case 0: { break; }
        }

        c -= utf8Offsets[n];

        DEBUGPRINT("-> %08x\n", c);

#if 1
        // ASCII -> JASCII
        //if((c >= '0' && c <= '9') || (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z'))
        if(c >= '!' && c <= '~')
            c += 0xfee0;
        else if(c == ' ')
            c = 0x3000;
#endif

        ucs[j++] = c;
    }

    ucs[j] = 0;

    return j;
}

/******************************************************************************/

static int pl2_running = 1;
void pl2Exit()
{
    pl2_running = 0;
}

int pl2DoFrame()
{
    glutMainLoopEvent();
    glutPostRedisplay();
    return pl2_running;
}

/******************************************************************************/

void pl2SetText(const char *text)
{
    pl2Utf8ToUcs4(pl2_text, ARRLEN(pl2_text), text, -1);
}

void pl2ShowText()
{
    pl2_text_showing = 1;

    while(pl2_text_showing)
    {
        pl2DoFrame();
    }
}

void pl2TextAdvance()
{
    pl2_text_showing = 0;
}

/******************************************************************************/

void pl2MenuClear(pl2Menu *menu)
{
    menu->numItems = 0;
    //menu->selection = -1;
}

int pl2MenuAddItem(pl2Menu *menu, const char *text)
{
    if(text && (menu->numItems < PL2_MAX_MENUITEMS))
    {
        pl2MenuItem *item = &(menu->items[menu->numItems]);

        if(*text == '*')
        {
            item->enabled = 0;
            text++;
        }
        else
        {
            item->enabled = 1;
        }

        pl2Utf8ToUcs4(item->text, ARRLEN(item->text), text, -1);

        DEBUGPRINT("%s: text == ", __func__);
        int i;
        for(i = 0; item->text[i]; i++)
            DEBUGPRINT("%08x ", item->text[i]);
        DEBUGPRINT("\n");

        menu->numItems++;

        return 1;
    }
    return 0;
}

int pl2MenuSelectNext(pl2Menu *menu)
{
    if(menu && menu->numItems)
    {
        return menu->selection = (menu->selection + 1) % menu->numItems;
    }
    return -1;
}

int pl2MenuSelectPrev(pl2Menu *menu)
{
    if(menu && menu->numItems)
    {
        return menu->selection = (menu->selection + menu->numItems - 1) % menu->numItems;
    }
    return -1;
}

int pl2MenuSelect(pl2Menu *menu, uint32_t item)
{
    if(menu)
    {
        if((item < menu->numItems) && (menu->items[item].enabled))
        {
            menu->selection = item;
        }
        return menu->selection;
    }
    return -1;
}

int pl2MenuConfirm(pl2Menu *menu)
{
    if(menu && pl2_menu_showing)
    {
        if(((unsigned)menu->selection < menu->numItems) && (menu->items[menu->selection].enabled))
        {
            pl2MenuClear(menu);
            pl2_menu_showing = 0;
            return menu->selection;
        }
    }
    return -1;
}

int pl2ShowMenu(pl2Menu *menu)
{
    if(menu && menu->numItems)
    {
        pl2_menu_showing = 1;

        menu->selection = 0;

        while(pl2_menu_showing)
        {
            pl2DoFrame();
        }

        return menu->selection;
    }
    return -1;
}

/******************************************************************************/

void pl2Wait(float sec)
{
    int until = glutGet(GLUT_ELAPSED_TIME) + sec * 1000;

    while(glutGet(GLUT_ELAPSED_TIME) < until)
    {
        pl2DoFrame();
    }
}

/******************************************************************************/

void pl2GameInit(int *argc, char *argv[])
{
    pl2GlInit(argc, argv);
    pl2AlInit(argc, argv);

    pl2PackageBuildIndex();
    atexit(pl2PackageClearIndex);

    pl2_font = pl2FontLoad("font");
}

static lua_State *pl2_L = NULL;

static lua_State *pl2LuaGetState()
{
    if(!pl2_L)
    {
        pl2_L = luaL_newstate();
        luaL_openlibs(pl2_L);
        luaopen_pl2(pl2_L);
    }
    return pl2_L;
}
static void pl2LuaCloseState()
{
    if(pl2_L)
    {
        lua_close(pl2_L);
        pl2_L = NULL;
    }
}

int pl2GameRun()
{
    lua_State *L = pl2LuaGetState();
    atexit(pl2LuaCloseState);

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

    //DEBUGPRINT("%s: entering main loop\n", __func__);
    //glutMainLoop();

    pl2LuaCloseState();

    return 1;
}

/******************************************************************************/

int main(int argc, char *argv[])
{
#if _PSP_FW_VERSION
    atexit(sceKernelExitGame);
#else
    //freopen("opl2.log", "wt", stdout);
    //freopen("error.log", "wt", stderr);
#endif

    pl2GameInit(&argc, argv);
    pl2GameRun();
    return 0;
}
