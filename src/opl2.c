//#include "opl2.h"
#include "opl2_int.h"
#include "opl2_gl.h"
#include "opl2_al.h"
#include "opl2_lua.h"

#define ILUT_USE_OPENGL
#include <IL/il.h>
#include <IL/ilut.h>

#include <ctype.h>

int pl2_screen_width = -1, pl2_screen_height = -1;
float pl2_screen_aspect = 1, pl2_screen_scale = 1;

bool pl2_skip = 0;
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
    { { 0, 0, 0 }, { 0, 0, 1 }, { 0, 1, 0 }, 45, NULL, NULL, 0, false, false },
    { { 0, 0, 0 }, { 0, 0, 1 }, { 0, 1, 0 }, 45, NULL, NULL, 0, false, false },
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

#ifdef _PSP_FW_VERSION
SDL_Joystick *pl2_joystick = 0;
#endif

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
    TRACE;

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
    TRACE;

    static int move_mode = 0;

    SDL_Event event;

    //PRINTFREERAM();

    while(SDL_PollEvent(&event))
    {
        switch(event.type)
        {
#ifdef _PSP_FW_VERSION
            case SDL_JOYBUTTONDOWN:
            {
                DEBUGPRINT("button %d down\n", event.jbutton.button);

                switch(event.jbutton.button)
                {
                    case PSP_JB_CIRCLE:
                    case PSP_JB_CROSS:
                        pl2TextAdvance();
                        pl2MenuConfirm(&pl2_menu);
                        break;

                    case PSP_JB_SELECT:
                        pl2ToggleOverlay();
                        break;

                    case PSP_JB_UP:
                        pl2MenuSelectPrev(&pl2_menu);
                        break;

                    case PSP_JB_DOWN:
                        pl2MenuSelectNext(&pl2_menu);
                        break;

                    case PSP_JB_SQUARE:
                        move_mode = MOVE_1P;
                        break;

                    case PSP_JB_LTRIGGER:
                        pl2CameraZoom(pl2_active_camera, -2.0f);
                        break;

                    case PSP_JB_RTRIGGER:
                        pl2CameraZoom(pl2_active_camera,  2.0f);
                        break;

                    case PSP_JB_TRIANGLE:
                        pl2_skip = 1;
                        break;
                }
                break;
            }
            case SDL_JOYBUTTONUP:
            {
                DEBUGPRINT("button %d up\n", event.jbutton.button);

                switch(event.jbutton.button)
                {
                    case PSP_JB_SQUARE:
                        move_mode = MOVE_3P;
                        break;

                    case PSP_JB_TRIANGLE:
                        pl2_skip = 0;
                        break;
                }
                break;
            }
#else
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

                    case SDLK_LCTRL:
                    case SDLK_RCTRL:
                        pl2_skip = 1;
                        break;

                    default:
                        break;
                }
                break;
            }

            case SDL_KEYUP:
            {
                switch(event.key.keysym.sym)
                {
                    case SDLK_LCTRL:
                    case SDLK_RCTRL:
                        pl2_skip = 0;
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
#else
# warning "SDL 1.2.5 or higher required for mouse wheel support"
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
#endif

            case SDL_VIDEORESIZE:
            {
                pl2Reshape(event.resize.w, event.resize.h);
                break;
            }

            case SDL_QUIT:
            {
// PSP game must be able to quit any time
#ifndef _PSP_FW_VERSION
                if(pl2_can_quit)
#endif
                {
                    DEBUGPRINT("quit\n");
                    pl2Quit();
                }
                break;
            }

            default: break;
        }
    }

    if(pl2_skip) pl2TextAdvance(); // text skip

#ifdef _PSP_FW_VERSION
    if(!(pl2_active_camera->locked || pl2_active_camera->path))
    {
        float x_angle = (float)SDL_JoystickGetAxis(pl2_joystick, 0) * M_PI / 16384.f;
        float y_angle = (float)SDL_JoystickGetAxis(pl2_joystick, 1) * M_PI / 16384.f;

        if(x_angle || y_angle)
        {
            switch(move_mode)
            {
                case MOVE_3P:
                    pl2CameraRotate3P(pl2_active_camera, x_angle, y_angle);
                    break;

                case MOVE_1P:
                    pl2CameraRotate1P(pl2_active_camera, x_angle, y_angle);
                    break;
            }
        }
    }
#endif

    float dt = pl2Tick();

    TRACE;

    pl2GlRenderFrame(dt);

    TRACE;

#if 0
    pl2AlSetListenerPosition(&(pl2_active_camera->eye));
#endif

#ifndef NDEBUG
    static int frames = 0;
    static float sec = 0, fps = 0;

    frames++;

    if((sec += dt) >= 1)
    {
        fps = (float)frames / sec;
# ifdef _PSP_FW_VERSION
        printf("%.2ffps\n", fps);
# else
        char temp[16];
        snprintf(temp, sizeof(temp), "%.2ffps", fps);
        SDL_WM_SetCaption(temp, "OPL2");
# endif
        frames = sec = 0;
    }
#endif // NDEBUG

    TRACE;

    SDL_Delay(10);
    SDL_GL_SwapBuffers();

    TRACE;

    return pl2_running;
}

/******************************************************************************/

void pl2SetText(const char *text)
{
    TRACE;

    pl2Utf8ToUcs4(pl2_text, ARRLEN(pl2_text), text, -1);
}

void pl2SetName(const char *name, uint32_t color)
{
    TRACE;

    pl2Utf8ToUcs4(pl2_name_text, ARRLEN(pl2_name_text), name, -1);
    pl2_name_color = color;
}

void pl2ShowText()
{
    TRACE;

    pl2Tick();

    pl2_text_showing = 1;
    pl2_show_window = 1;

    DEBUGPRINT("%s: show text\n", __func__);

    while(pl2_text_showing && pl2DoFrame());

    TRACE;
}

void pl2TextAdvance()
{
    TRACE;

    if(pl2_text_showing && !pl2_hide_overlay)
    {
        DEBUGPRINT("%s: advance text\n", __func__);

        pl2_text_showing = 0;
    }
}

/******************************************************************************/

void pl2MenuClear(pl2Menu *menu)
{
    TRACE;

    menu->numItems = 0;
    //menu->selection = -1;
}

int pl2MenuAddItem(pl2Menu *menu, const char *text)
{
    TRACE;

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
    TRACE;

    if(menu && menu->numItems && pl2_menu_showing && !pl2_hide_overlay)
    {
        DEBUGPRINT("%s: select next menu item\n", __func__);

        return menu->selection = (menu->selection + 1) % menu->numItems;
    }
    return -1;
}

int pl2MenuSelectPrev(pl2Menu *menu)
{
    TRACE;

    if(menu && menu->numItems && pl2_menu_showing && !pl2_hide_overlay)
    {
        DEBUGPRINT("%s: select previous menu item\n", __func__);

        return menu->selection = (menu->selection + menu->numItems - 1) % menu->numItems;
    }
    return -1;
}

int pl2MenuSelect(pl2Menu *menu, uint32_t item)
{
    TRACE;

    if(menu && pl2_menu_showing && !pl2_hide_overlay)
    {
        if((item < menu->numItems) && (menu->items[item].enabled))
        {
            DEBUGPRINT("%s: select menu item %d\n", __func__, item);

            menu->selection = item;
        }
        return menu->selection;
    }
    return -1;
}

int pl2ShowMenu(pl2Menu *menu)
{
    TRACE;

    if(menu && menu->numItems)
    {
        pl2Tick();

        pl2_menu_showing = 1;

        menu->selection = 0;

        DEBUGPRINT("%s: show menu\n", __func__);

        while(pl2_menu_showing && pl2DoFrame());

        TRACE;

        return menu->selection;
    }

    TRACE;

    return -1;
}

int pl2MenuConfirm(pl2Menu *menu)
{
    TRACE;

    if(menu && pl2_menu_showing && !pl2_hide_overlay)
    {
        if(((unsigned)menu->selection < menu->numItems) &&
           (menu->items[menu->selection].enabled))
        {
            pl2MenuClear(menu);
            pl2_menu_showing = 0;

            TRACE;

            DEBUGPRINT("%s: menu dismissed\n", __func__);

            return menu->selection;
        }
    }

    TRACE;

    return -1;
}

/******************************************************************************/

void pl2ToggleOverlay()
{
    TRACE;

    pl2_hide_overlay = !pl2_hide_overlay;

    TRACE;
}

void pl2ShowOverlay()
{
    TRACE;

    pl2_hide_overlay = 0;

    TRACE;
}

void pl2HideOverlay()
{
    TRACE;

    pl2_hide_overlay = 1;

    TRACE;
}

int pl2SetImage(const char *name)
{
    TRACE;

    if(pl2_current_image)
    {
        pl2ImageFree(pl2_current_image);
        pl2_current_image = NULL;
    }

    pl2_current_image = pl2ImageLoad(name);

    TRACE;

    return name ? (NULL != pl2_current_image) : 1;
}

/******************************************************************************/

void pl2LayerFade(pl2Layer *layer, float target, float length)
{
    TRACE;

    if(layer)
    {
        target = (target < 0) ? 0 : (target > 1) ? 1 : target;

        if(length > 0)
        {
            layer->fade_target = target;
            layer->fade_length = length;
            layer->fade_time   = 0;
        }
        else
        {
            layer->fade_target = layer->fade_level = target;
            layer->fade_length = layer->fade_time = 0;
        }
    }

    TRACE;
}

void pl2Wait(float sec)
{
    TRACE;

    pl2Tick();

    int until = SDL_GetTicks() + sec * 1000;

    DEBUGPRINT("%s: wait %.2f seconds\n", __func__, sec);

    while((SDL_GetTicks() < until) && pl2DoFrame());

    TRACE;
}

void pl2Quit()
{
    TRACE;

    DEBUGPRINT("%s: quit game\n", __func__);

    pl2_running = 0;
    exit(0);
}

/******************************************************************************/

pl2Sound *pl2_sounds[PL2_NUM_CHANNELS] = { NULL };

static void pl2MixAudio(void *ud, Uint8 *stream, int len)
{
    int i, j; Uint32 n;

    SDL_AudioCVT cvt;
    int16_t buffer[4096];

    //DEBUGPRINT("%s: stream == %p, len == %d\n", __func__, stream, len);

    for(i = j = 0; i < PL2_NUM_CHANNELS; i++)
    {
        pl2Sound *sound = pl2_sounds[i];

        if(sound)
        {
            OggVorbis_File *vf = &(sound->vf);
            vorbis_info *info = ov_info(vf, -1);
            n = MIN(len >> (2 - info->channels), sizeof(buffer));

            int bytes = pl2SoundDecode(sound, buffer, n);
            //2*info->channels*(ov_pcm_total(vf, -1) - ov_pcm_tell(vf));
            //if(n > len) n = len;

            SDL_BuildAudioCVT(&cvt, AUDIO_S16, info->channels, info->rate, AUDIO_S16, 2, 44100);

            cvt.buf = (Uint8*)buffer;
            cvt.len = bytes;
            SDL_ConvertAudio(&cvt);

            SDL_MixAudio(stream, (Uint8*)(buffer), cvt.len_cvt,
                         (float)SDL_MIX_MAXVOLUME * sound->volume);

            j++;
        }
    }

    //DEBUGPRINT("%s: %d of %d channels playing\n", __func__, j, i);
}

/******************************************************************************/

static SDL_Surface *pl2_screen_surf = NULL;

static const char *pl2_script_name = NULL;

void pl2GameShutdown()
{
    TRACE;
    SDL_CloseAudio();
    pl2PackageClearIndex();
}

int pl2GameInit(int *argc, char *argv[])
{
    TRACE;

    atexit(pl2GameShutdown);

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
        else if(!strcmp(argv[i], "-C") || !strcmp(argv[i], "--no-censor"))
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
        SDL_Rect **sizes = SDL_ListModes(NULL, flags);

        if(sizes && (sizes != (SDL_Rect**)(-1)))
        {
            width  = (*sizes)[0].w;
            height = (*sizes)[0].h;
        }
        else
        {
            width  = PL2_NOMINAL_SCREEN_WIDTH;
            height = PL2_NOMINAL_SCREEN_HEIGHT;
        }
    }

#if 0
    pl2AlInit(argc, argv);
#else
    SDL_AudioSpec fmt = {
        freq: 44100,
        format: AUDIO_S16,
        channels: 2,
        samples: 1024,
        callback: pl2MixAudio,
        userdata: 0
    };

    if(SDL_OpenAudio(&fmt, &fmt) < 0)
    {
        fprintf(stderr, "%s: error opening audio device\n", argv[0]);
        return 0;
    }
    SDL_PauseAudio(0);
#endif

#ifdef _PSP_FW_VERSION
    pl2_joystick = SDL_JoystickOpen(0);
#endif

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

#if 1 //defined(_PSP_FW_VERSION)
    {
        void pl2GlBegin2D();
        void pl2GlEnd2D();

        GLuint splash = ilutGLLoadImage("splash.jpg");

        if(splash)
        {
            pl2GlBegin2D();

            float t = 0, a = 0;

            while(t < 5)
            {
                a = fmaxf(0, fminf(1, fminf(t, 5-t)));

                glClear(GL_COLOR_BUFFER_BIT);

                glColor4f(1,1,1,a);

                glBegin(GL_QUADS);
                    glTexCoord2f(0, 1); glVertex2f(  0,   0);
                    glTexCoord2f(0, 0); glVertex2f(  0, 272);
                    glTexCoord2f(1, 0); glVertex2f(480, 272);
                    glTexCoord2f(1, 1); glVertex2f(480,   0);
                glEnd();

                SDL_GL_SwapBuffers();

                t += pl2Tick();
            }

            pl2GlEnd2D();
            glDeleteTextures(1, &splash);
        }
    }
#endif

    pl2PackageBuildIndex();

    pl2_font = pl2FontLoad("font");

    return 1;
}

#ifdef _PSP_FW_VERSION
# include <pspdebug.h>
# include <pspctrl.h>
#endif

int pl2GameRun()
{
    TRACE;

    lua_State *L = pl2LuaGetState();
    atexit(pl2LuaCloseState);

    luaL_openlibs(pl2_L);
    luaopen_pl2(L);

    // disable os.*, io.* for security
    lua_pushnil(L); lua_setglobal(L, "os");
    //lua_pushnil(L); lua_setglobal(L, "io");

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

#ifdef _PSP_FW_VERSION
        pspDebugScreenInit();
        pspDebugScreenPrintf("Error: %s\n\nPress START to exit.", lua_tostring(L, -1));

        SceCtrlData pad;

        do {
            sceCtrlReadBufferPositive(&pad, 1);
        }
        while(0 == (pad.Buttons & PSP_CTRL_START));

        do {
            sceCtrlReadBufferPositive(&pad, 1);
        }
        while(0 != (pad.Buttons & PSP_CTRL_START));
#endif

        return 0;
    }

    //DEBUGPRINT("%s: entering main loop\n", __func__);
    //glutMainLoop();

    pl2LuaCloseState();

    return 1;
}

/******************************************************************************/

#ifdef _PSP_FW_VERSION

int pl2PspRegisterCallbacks();

int main(int argc, char *argv[])
{
    pl2PspRegisterCallbacks();
    atexit(sceKernelExitGame);

    //if(!freopen("stderr.txt", "at", stderr))
    //    return 2;

    if(!pl2GameInit(&argc, argv))
        return 1;

    pl2GameRun();
    return 0;
}

#else // _PSP_FW_VERSION

void pl2DetectSSE();

int main(int argc, char *argv[])
{
    pl2DetectSSE();

    if(!pl2GameInit(&argc, argv))
        return 1;

    pl2GameRun();
    return 0;
}

#endif // _PSP_FW_VERSION
