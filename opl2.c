//#include "opl2.h"
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

bool pl2_censor = 0;
bool pl2_text_showing = 0;
bool pl2_menu_showing = 0;
bool pl2_hide_overlay = 0;
bool pl2_can_quit = 0;
bool pl2_show_window = 0;
bool pl2_show_title = 0;

pl2Font *pl2_font = NULL;

pl2Character pl2_chars[PL2_MAX_CHARS] =
{
    { { NULL }, NULL, NULL, 0, 0, false, false, NULL, 0 },
    { { NULL }, NULL, NULL, 0, 0, false, false, NULL, 0 },
    { { NULL }, NULL, NULL, 0, 0, false, false, NULL, 0 },
    { { NULL }, NULL, NULL, 0, 0, false, false, NULL, 0 },
};

pl2Light pl2_lights[PL2_MAX_LIGHTS] =
{
    { { 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, false },
    { { 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, false },
    { { 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, false },
    { { 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, false },
};

pl2Camera pl2_cameras[PL2_MAX_CAMERAS] =
{
    { { 0, 0, 0 }, { 0, 0, 0 }, { 0, 1, 0 }, 45, NULL, NULL, 0, false, false },
    { { 0, 0, 0 }, { 0, 0, 0 }, { 0, 1, 0 }, 45, NULL, NULL, 0, false, false },
};

pl2Layer pl2_layers[PL2_MAX_LAYERS] =
{
    { 0, 0, 0, 0 },
    { 0, 0, 0, 0 },
};

pl2Menu pl2_menu = { 0, 0, { { 0, { 0 } } } };

uint32_t pl2_text[1024] = { 0 };
uint32_t pl2_name_text[32] = { 0 };
uint32_t pl2_name_color = 0;

pl2Camera *pl2_active_camera = &(pl2_cameras[0]);

pl2Image *pl2_current_image = NULL;

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

size_t pl2Ucs4Strlen(const uint32_t *text)
{
    if(!text) return 0;
    int len = 0;
    while(text[len]) len++;
    return len;
}

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

/*
static struct { uint16_t ascii, jascii; } jasciitable[] = {
    { '0', 0xff10 }, { '1', 0xff11 }, { '2', 0xff12 }, { '3', 0xff13 },
    { '4', 0xff14 }, { '5', 0xff15 }, { '6', 0xff16 }, { '7', 0xff17 },
    { '8', 0xff18 }, { '9', 0xff19 }, { 'A', 0xff21 }, { 'B', 0xff22 },
    { 'C', 0xff23 }, { 'D', 0xff24 }, { 'E', 0xff25 }, { 'F', 0xff26 },
    { 'G', 0xff27 }, { 'H', 0xff28 }, { 'I', 0xff29 }, { 'J', 0xff2a },
    { 'K', 0xff2b }, { 'L', 0xff2c }, { 'M', 0xff2d }, { 'N', 0xff2e },
    { 'O', 0xff2f }, { 'P', 0xff30 }, { 'Q', 0xff31 }, { 'R', 0xff32 },
    { 'S', 0xff33 }, { 'T', 0xff34 }, { 'U', 0xff35 }, { 'V', 0xff36 },
    { 'W', 0xff37 }, { 'X', 0xff38 }, { 'Y', 0xff39 }, { 'Z', 0xff3a },
    { 'a', 0xff41 }, { 'b', 0xff42 }, { 'c', 0xff43 }, { 'd', 0xff44 },
    { 'e', 0xff45 }, { 'f', 0xff46 }, { 'g', 0xff47 }, { 'h', 0xff48 },
    { 'i', 0xff49 }, { 'j', 0xff4a }, { 'k', 0xff4b }, { 'l', 0xff4c },
    { 'm', 0xff4d }, { 'n', 0xff4e }, { 'o', 0xff4f }, { 'p', 0xff50 },
    { 'q', 0xff51 }, { 'r', 0xff52 }, { 's', 0xff53 }, { 't', 0xff54 },
    { 'u', 0xff55 }, { 'v', 0xff56 }, { 'w', 0xff57 }, { 'x', 0xff58 },
    { 'y', 0xff59 }, { 'z', 0xff5a },
};
*/
size_t pl2Utf8ToUcs4(uint32_t *ucs, size_t size, const char *text, int length)
{
    uint8_t *utf = (uint8_t*)text;

    int i = 0, j = 0;

    if(length < 0) length = text ? strlen(text) : 0;

    size--;

    while((j < size) && (i < length))
    {
        //DEBUGPRINT("%s: %02x ", __func__, utf[i]);

        uint32_t c = utf[i++];

        int n = utf8TrailingBytes[c];

        switch(n)
        {
            case 5: { /*DEBUGPRINT("%02x ", utf[i]);*/ c <<= 6; c += utf[i++]; }
            case 4: { /*DEBUGPRINT("%02x ", utf[i]);*/ c <<= 6; c += utf[i++]; }
            case 3: { /*DEBUGPRINT("%02x ", utf[i]);*/ c <<= 6; c += utf[i++]; }
            case 2: { /*DEBUGPRINT("%02x ", utf[i]);*/ c <<= 6; c += utf[i++]; }
            case 1: { /*DEBUGPRINT("%02x ", utf[i]);*/ c <<= 6; c += utf[i++]; }
            case 0: { break; }
        }

        c -= utf8Offsets[n];

        //DEBUGPRINT("-> %08x\n", c);

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

    if(j < size) ucs[j] = 0;

    return j;
}

/******************************************************************************/

float pl2Tick()
{
    static Uint32 old_ticks = 0;
    if(!old_ticks) old_ticks = SDL_GetTicks();
    Uint32 new_ticks = SDL_GetTicks();
    float dt = (float)(new_ticks - old_ticks) * 0.001f;
    old_ticks = new_ticks;
    return dt;
}

enum
{
   MOVE_NONE = 0,
   MOVE_3P   = 1,
   MOVE_1P   = 2,
   MOVE_ZOOM = 3,
};

void pl2Reshape(int w, int h)
{
    pl2_screen_width  = w;
    pl2_screen_height = h;

    float f1 = (float)w / (float)(PL2_NOMINAL_SCREEN_WIDTH);
    float f2 = (float)h / (float)(PL2_NOMINAL_SCREEN_HEIGHT);

    pl2_screen_scale = (f1 < f2) ? f1 : f2;

    if(!h) h++;
    pl2_screen_aspect = (float)w/(float)h;

    DEBUGPRINT("%s: window resize to %dx%d (aspect == %g, scale == %g)\n", __func__,
               pl2_screen_width, pl2_screen_height, pl2_screen_aspect, pl2_screen_scale);

    //glMatrixMode(GL_PROJECTION);
    //glLoadIdentity();
    glViewport(0, 0, w, h);
}

int pl2DoFrame()
{
    static int move_mode = 0;

    SDL_Event event;

    //SDL_PumpEvents();

    PRINTFREERAM();

    while(SDL_PollEvent(&event))
    {
        switch(event.type)
        {
            case SDL_KEYDOWN:
            {
                switch(event.key.keysym.sym)
                {
                    case SDLK_ESCAPE:
                        event.type = SDL_QUIT;
                        SDL_PushEvent(&event);
                        break;

                    case SDLK_RETURN:
                        pl2TextAdvance();
                        pl2MenuConfirm(&pl2_menu);
                        break;

                    case SDLK_SPACE:
                        pl2ToggleOverlay();
                        break;

                    case SDLK_UP:
                        pl2MenuSelectPrev(&pl2_menu);
                        break;

                    case SDLK_DOWN:
                        pl2MenuSelectNext(&pl2_menu);
                        break;

                    default:
                        break;
                }
                break;
            }

            case SDL_MOUSEMOTION:
            {
                int dx = event.motion.xrel, dy = event.motion.yrel;

                float x_angle = 2.0f * M_PI * (float)dx / (float)pl2_screen_width;
                float y_angle = 2.0f * M_PI * (float)dy / (float)pl2_screen_height;

                switch(move_mode)
                {
                    case MOVE_3P:
                        if(!(pl2_active_camera->locked || pl2_active_camera->path))
                        {
                            pl2CameraRotate3P(pl2_active_camera, x_angle, y_angle);
                        }
                        break;

                    case MOVE_1P:
                        if(!(pl2_active_camera->locked || pl2_active_camera->path))
                        {
                            pl2CameraRotate1P(pl2_active_camera, x_angle, y_angle);
                        }
                        break;

                    case MOVE_ZOOM:
                        if(!(pl2_active_camera->locked || pl2_active_camera->path))
                        {
                            pl2CameraZoom(pl2_active_camera, -10.0f * (float)dy / (float)pl2_screen_height);
                        }
                        break;
                }
                break;
            }

            case SDL_MOUSEBUTTONDOWN:
            {
                switch(event.button.button)
                {
                    case SDL_BUTTON_LEFT:
                        if(!(pl2_active_camera->locked || pl2_active_camera->path))
                        {
                            move_mode |= MOVE_3P;
                        }
                        break;

                    case SDL_BUTTON_RIGHT:
                        if(!(pl2_active_camera->locked || pl2_active_camera->path))
                        {
                            move_mode |= MOVE_1P;
                        }
                        break;

#if SDL_VERSION_ATLEAST(1,2,5)
                    case SDL_BUTTON_WHEELUP:
                        if(pl2_menu_showing && (pl2_active_camera->locked || pl2_active_camera->path))
                            pl2MenuSelectPrev(&pl2_menu);
                        else
                            pl2CameraZoom(pl2_active_camera,  2.0f);
                        break;

                    case SDL_BUTTON_WHEELDOWN:
                        if(pl2_menu_showing && (pl2_active_camera->locked || pl2_active_camera->path))
                            pl2MenuSelectNext(&pl2_menu);
                        else
                            pl2CameraZoom(pl2_active_camera, -2.0f);
                        break;
#endif
                }

                SDL_ShowCursor(!move_mode);
                break;
            }

            case SDL_MOUSEBUTTONUP:
            {
                switch(event.button.button)
                {
                    case SDL_BUTTON_LEFT:
                        move_mode &= ~MOVE_3P;
                        break;

                    case SDL_BUTTON_RIGHT:
                        move_mode &= ~MOVE_1P;
                        break;
                }

                SDL_ShowCursor(!move_mode);
                break;
            }
                
            case SDL_VIDEORESIZE:
            {
                pl2Reshape(event.resize.w, event.resize.h);
                break;
            }

            case SDL_QUIT:
            {
                if(pl2_can_quit)
                    pl2Quit();
                break;
            }

            default: break;
        }
    }

    float dt = pl2Tick();

    pl2GlRenderFrame(dt);

#ifndef NDEBUG    
    static int frames = 0;
    static float sec = 0, fps = 0;
    
    frames++;

    if((sec += dt) >= 1)
    {
        fps = (float)frames / sec;
        char temp[16];
        snprintf(temp, sizeof(temp), "%.2ffps", fps);
        SDL_WM_SetCaption(temp, "OPL2");
        frames = sec = 0;
    }
#endif // NDEBUG

    //SDL_Delay(10);
    SDL_GL_SwapBuffers();

    return pl2_running;
}

/******************************************************************************/

void pl2SetText(const char *text)
{
    pl2Utf8ToUcs4(pl2_text, ARRLEN(pl2_text), text, -1);
}

void pl2SetName(const char *name, uint32_t color)
{
    pl2Utf8ToUcs4(pl2_name_text, ARRLEN(pl2_name_text), name, -1);
    pl2_name_color = color;
}

void pl2ShowText()
{
    pl2Tick();

    pl2_text_showing = 1;

    while(pl2_text_showing && pl2DoFrame());
}

void pl2TextAdvance()
{
    if(!pl2_hide_overlay)
    {
        pl2_text_showing = 0;
    }
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

        //DEBUGPRINT("%s: text == ", __func__);
        //int i;
        //for(i = 0; item->text[i]; i++)
        //    DEBUGPRINT("%08x ", item->text[i]);
        //DEBUGPRINT("\n");

        menu->numItems++;

        return 1;
    }
    return 0;
}

int pl2MenuSelectNext(pl2Menu *menu)
{
    if(menu && menu->numItems && pl2_menu_showing && !pl2_hide_overlay)
    {
        return menu->selection = (menu->selection + 1) % menu->numItems;
    }
    return -1;
}

int pl2MenuSelectPrev(pl2Menu *menu)
{
    if(menu && menu->numItems && pl2_menu_showing && !pl2_hide_overlay)
    {
        return menu->selection = (menu->selection + menu->numItems - 1) % menu->numItems;
    }
    return -1;
}

int pl2MenuSelect(pl2Menu *menu, uint32_t item)
{
    if(menu && pl2_menu_showing && !pl2_hide_overlay)
    {
        if((item < menu->numItems) && (menu->items[item].enabled))
        {
            menu->selection = item;
        }
        return menu->selection;
    }
    return -1;
}

int pl2ShowMenu(pl2Menu *menu)
{
    if(menu && menu->numItems)
    {
        pl2Tick();

        pl2_menu_showing = 1;

        menu->selection = 0;

        while(pl2_menu_showing && pl2DoFrame());

        return menu->selection;
    }
    return -1;
}

int pl2MenuConfirm(pl2Menu *menu)
{
    if(menu && pl2_menu_showing && !pl2_hide_overlay)
    {
        if(((unsigned)menu->selection < menu->numItems) && 
           (menu->items[menu->selection].enabled))
        {
            pl2MenuClear(menu);
            pl2_menu_showing = 0;
            return menu->selection;
        }
    }
    return -1;
}

/******************************************************************************/

void pl2ToggleOverlay()
{
    pl2_hide_overlay = !pl2_hide_overlay;
}

int pl2SetImage(const char *name)
{
    if(pl2_current_image)
    {
        pl2ImageFree(pl2_current_image);
        pl2_current_image = NULL;
    }
    
    pl2_current_image = pl2ImageLoad(name);
    
    return name ? (NULL != pl2_current_image) : 1;
}

/******************************************************************************/

void pl2Wait(float sec)
{
    pl2Tick();

    int until = SDL_GetTicks() + sec * 1000;

    while((SDL_GetTicks() < until) && pl2DoFrame());
}

void pl2Quit()
{
    exit(0);
}

/******************************************************************************/

static SDL_Surface *pl2_screen_surf = NULL;

static const char *pl2_script_name = NULL;

void pl2GameShutdown()
{
    pl2PackageClearIndex();
}

int pl2GameInit(int *argc, char *argv[])
{
    atexit(pl2GameShutdown);

    pl2PackageBuildIndex();

    pl2_font = pl2FontLoad("font");

    pl2AlInit(argc, argv);

    SDL_Init(SDL_INIT_TIMER | SDL_INIT_AUDIO | SDL_INIT_VIDEO);
    atexit(SDL_Quit);

    int width = 0, height = 0, bpp = 32;
    Uint32 flags = SDL_FULLSCREEN | SDL_OPENGL | SDL_HWSURFACE;

    int i;
    for(i = 1; i < *argc; i++)
    {
        if(!strcmp(argv[i], "-f") || !strcmp(argv[i], "--fullscreen"))
        {
            flags |= SDL_FULLSCREEN;

            if(((i + 1) < *argc) && (argv[i+1][0] != '-'))
            {
                i++;
                int tw, th, td;
                if(3 == sscanf(argv[i], "%dx%d:%d", &tw, &th, &td))
                {
                    DEBUGPRINT("%s: parsed display mode %dx%d:%d\n", __func__, tw, th, td);
                    width = tw; height = th; bpp = td;
                }
                else if(2 == sscanf(argv[i], "%dx%d", &tw, &th))
                {
                    DEBUGPRINT("%s: parsed display mode %dx%d\n", __func__, tw, th);
                    width = tw; height = th;
                }
                else
                {
                    fprintf(stderr, "%s: warning: invalid geometry\n", argv[0]);
                }
            }
        }
        else if(!strcmp(argv[i], "-w") || !strcmp(argv[i], "--window"))
        {
            flags &= ~SDL_FULLSCREEN;

            if(((i + 1) < *argc) && (argv[i+1][0] != '-'))
            {
                i++;
                int tw, th, td;
                if(3 == sscanf(argv[i], "%dx%d:%d", &tw, &th, &td))
                {
                    DEBUGPRINT("%s: parsed display mode %dx%d:%d\n", __func__, tw, th, td);
                    width = tw; height = th; bpp = td;
                }
                else if(2 == sscanf(argv[i], "%dx%d", &tw, &th))
                {
                    DEBUGPRINT("%s: parsed display mode %dx%d\n", __func__, tw, th);
                    width = tw; height = th;
                }
                else
                {
                    fprintf(stderr, "%s: warning: invalid geometry\n", argv[0]);
                }
            }
        }
        else if(!strcmp(argv[i], "-c") || !strcmp(argv[i], "--censor"))
        {
            pl2_censor = 1;
        }
        else if(!strcmp(argv[i], "-n") || !strcmp(argv[i], "--no-censor"))
        {
            pl2_censor = 0;
        }
        else if(argv[i][0] == '-')
        {
            fprintf(stderr, "%s: unrecognized option \"%s\"\n", argv[0], argv[i]);
        }
        else if(!pl2_script_name)
        {
            pl2_script_name = argv[i];
        }
        else
        {
            fprintf(stderr, "%s: extra arguments on command line\n", argv[0]);
        }
    }

    if(!(width && height))
    {
        /*
        SDL_Rect **sizes = SDL_ListModes(NULL, flags);

        if(sizes && (sizes != (SDL_Rect**)(-1)))
        {
            width  = (*sizes)[0].w;
            height = (*sizes)[0].h;
        }
        else
        */
        {
            width  = PL2_NOMINAL_SCREEN_WIDTH;
            height = PL2_NOMINAL_SCREEN_HEIGHT;
        }
    }

    bpp = SDL_VideoModeOK(width, height, bpp, flags);

    if(!bpp)
    {
        // Try again with default settings
        bpp = 32;
        width  = PL2_NOMINAL_SCREEN_WIDTH;
        height = PL2_NOMINAL_SCREEN_HEIGHT;

        bpp = SDL_VideoModeOK(width, height, bpp, flags);

        if(!bpp) return 0;
    }

    DEBUGPRINT("%s: using display mode %dx%d:%d\n", __func__, width, height, bpp);

    pl2_screen_surf = SDL_SetVideoMode(width, height, bpp, flags);

    if(!pl2_screen_surf)
    {
        fprintf(stderr, "%s: error setting display mode %dx%d:%d\n",
                argv[0], width, height, bpp);
        return 0;
    }

    SDL_WM_SetCaption("OPL2", "OPL2");

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE,     8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE,   8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE,    8);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE,   8);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE,  16);

    pl2Reshape(width, height);

    pl2GlInit();
    
    return 1;
}

static lua_State *pl2_L = NULL;

static lua_State *pl2LuaGetState()
{
    if(!pl2_L)
    {
        pl2_L = luaL_newstate();
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

    luaL_openlibs(pl2_L);
    luaopen_pl2(L);
    
    // disable os.* for security
    lua_pushnil(L); lua_setglobal(L, "os");

    if(!pl2_script_name) pl2_script_name = "script.lua";

    static const char *error_handler = "return debug.traceback(...,2)";
    
    luaL_loadbuffer(L, error_handler, strlen(error_handler), "@<error_handler>");
    
    int err = luaL_loadfile(L, pl2_script_name);

    if(!err)
    {
        err = lua_pcall(L, 0, 0, -2);
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

//#ifdef main
//#undef main
//#endif

int main(int argc, char *argv[])
{
#if _PSP_FW_VERSION
    atexit(sceKernelExitGame);
#else
    pl2DetectSSE();
#endif

    if(!pl2GameInit(&argc, argv))
        return 1;

    pl2GameRun();
    return 0;
}

