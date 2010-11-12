//#include "opl2.h"
#include "opl2_int.h"
#include "opl2_vm.h"
#include "opl2_gl.h"

static SDL_Surface *pl2_screen_surf = NULL;

enum
{
   MOVE_NONE = 0,
   MOVE_3P   = 1,
   MOVE_1P   = 2,
   MOVE_ZOOM = 3,
};

static void pl2SdlReshape(int w, int h)
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

int pl2SdlDoFrame()
{
    static int move_mode = 0;

    SDL_Event event;

    //SDL_PumpEvents();

    while(SDL_PollEvent(&event))
    {
        switch(event.type)
        {
            case SDL_KEYDOWN: {
                switch(event.key.keysym.sym)
                {
                    case SDLK_ESCAPE:
                        //pl2_running = 0;
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
                int dx = event.motion.xrel, dy = event.motion.yrel, buttons = event.motion.state;
                float x_angle = 2.0f * M_PI * (float)dx / (float)pl2_screen_width;
                float y_angle = 2.0f * M_PI * (float)dy / (float)pl2_screen_height;

                if(buttons)
                {
                    switch(move_mode)
                    {
                        case MOVE_3P:
                            pl2CameraRotate3P(pl2_active_camera, x_angle, y_angle);
                            break;

                        case MOVE_1P:
                            pl2CameraRotate1P(pl2_active_camera, x_angle, y_angle);
                            break;

                        case MOVE_ZOOM:
                            pl2CameraZoom(pl2_active_camera, -10.0f * (float)dy / (float)pl2_screen_height);
                            break;
                    }
                }
                break;
            }

            case SDL_MOUSEBUTTONDOWN:
            {
                switch(event.button.button)
                {
                    case SDL_BUTTON_LEFT:
                        move_mode |= MOVE_3P;
                        SDL_ShowCursor(0);
                        break;
                    case SDL_BUTTON_RIGHT:
                        move_mode |= MOVE_1P;
                        SDL_ShowCursor(0);
                        break;
                    case SDL_BUTTON_WHEELUP:
                        pl2CameraZoom(pl2_active_camera,  2.0f);
                        break;
                    case SDL_BUTTON_WHEELDOWN:
                        pl2CameraZoom(pl2_active_camera, -2.0f);
                        break;
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
                pl2SdlReshape(event.resize.w, event.resize.h);
                break;
            }

            case SDL_QUIT:
            {
                DEBUGPRINT("%s: exiting\n", __func__);
                exit(0);
                //pl2_running = 0;
                break;
            }

            default: break;
        }
    }

    static Uint32 ticks = 0;
    if(!ticks) ticks = SDL_GetTicks();
    Uint32 new_ticks = SDL_GetTicks();
    float dt = (float)(new_ticks - ticks) * 0.001f;
    ticks = new_ticks;
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

void pl2SdlShutdown()
{
    //SDL_RemoveTimer(pl2_timer);
    SDL_Quit();
}

int pl2SdlInit(int *argc, char *argv[])
{
    SDL_Init(SDL_INIT_TIMER | SDL_INIT_AUDIO | SDL_INIT_VIDEO);
    atexit(pl2SdlShutdown);

    int width = 0, height = 0, bpp = 32;
    Uint32 flags = SDL_FULLSCREEN | SDL_OPENGL | SDL_HWSURFACE;

    int i;
    for(i = 1; i < *argc; i++)
    {
        if(!strcmp(argv[i], "-f") || !strcmp(argv[i], "-fullscreen"))
        {
            flags |= SDL_FULLSCREEN;
        }
        else if(!strcmp(argv[i], "-w") || !strcmp(argv[i], "-window"))
        {
            flags &= ~SDL_FULLSCREEN;
        }
        else if(!strcmp(argv[i], "-s") || !strcmp(argv[i], "-size"))
        {
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
            else
            {
                fprintf(stderr, "%s: --size requires an argument\n", argv[0]);
            }
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

    bpp = SDL_VideoModeOK(width, height, bpp, flags);

    if(!bpp) return 0;

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

    pl2SdlReshape(width, height);

    pl2GlInit();

    return 1;
}

