//#include "opl2.h"
#include "opl2_int.h"
#include "opl2_vm.h"
#include "opl2_gl.h"

static SDL_Surface *pl2_screen_surf = NULL;

enum
{
   MOVE_NONE  = 0,
   MOVE_3P    = 1,
   MOVE_ORTHO = 2,
   MOVE_1P    = 3,
};

static int move_mode = 0, mouse_x = -1, mouse_y = -1;

static SDL_TimerID pl2_timer = 0;

static Uint32 pl2SdlRedraw(Uint32 interval, void *param)
{
    SDL_Event event;

    event.type = SDL_USEREVENT;
    event.user.code = 42;
    event.user.data1 = 0;
    event.user.data2 = 0;

    SDL_PushEvent(&event);

    return interval;
}

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
    SDL_Event event;

    //SDL_PumpEvents();

    while(SDL_PollEvent(&event))
    {
        switch(event.type)
        {
            case SDL_USEREVENT:
            {
                DEBUGPRINT("%s: user event\n", __func__);
                break;
            }

            case SDL_KEYDOWN: {
                switch(event.key.keysym.sym)
                {
                    case SDLK_ESCAPE:
                        pl2_running = 0;
                        break;
                    case SDLK_RETURN:
                        pl2TextAdvance();
                        pl2MenuConfirm(&pl2_menu);
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

            case SDL_MOUSEMOTION: {
                int dx = event.motion.xrel, dy = event.motion.yrel, buttons = event.motion.state;
                float x_angle = 2.0f * M_PI * (float)dx / (float)pl2_screen_width;
                float y_angle = 2.0f * M_PI * (float)dy / (float)pl2_screen_height;

                if(buttons)
                {
                    switch(move_mode)
                    {
                        case MOVE_3P:
                            pl2CameraRotate3P(&(pl2_cameras[0]), x_angle, y_angle);
                            break;

                        case MOVE_1P:
                            pl2CameraRotate1P(&(pl2_cameras[0]), x_angle, y_angle);
                            break;

                        case MOVE_ORTHO:
                            pl2CameraZoom(&(pl2_cameras[0]), -10.0f * (float)dy / (float)pl2_screen_height);
                            break;
                    }
                }
                break;
            }

            case SDL_MOUSEBUTTONDOWN:
                switch(event.button.button)
                {
                    case SDL_BUTTON_LEFT:
                        move_mode |= MOVE_3P;
                        SDL_ShowCursor(0);
                        break;
                    case SDL_BUTTON_RIGHT:
                        move_mode |= MOVE_ORTHO;
                        SDL_ShowCursor(0);
                        break;
                }
                break;

            case SDL_MOUSEBUTTONUP:
                break;
                
            case SDL_VIDEORESIZE:
                pl2SdlReshape(event.resize.w, event.resize.h);
                break;

            case SDL_QUIT:
                DEBUGPRINT("%s: exiting\n", __func__);
                exit(0);
                //pl2_running = 0;
                break;

            default:
                break;
        }
    }

    static Uint32 ticks = 0;
    if(!ticks) ticks = SDL_GetTicks();
    Uint32 new_ticks = SDL_GetTicks();
    float dt = (float)(new_ticks - ticks) * 0.001f;
    ticks = new_ticks;
    pl2GlRenderFrame(dt);
    
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

    SDL_Delay(10);
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

    int width = 800, height = 600, bpp = 32;
    Uint32 flags = /* SDL_FULLSCREEN | */ SDL_OPENGL | SDL_HWSURFACE;

    int i;
    for(i = 1; i < *argc; i++)
    {
        if(!strcmp(argv[i], "-fs") || !strcmp(argv[i], "-fullscreen"))
        {
            flags |= SDL_FULLSCREEN;
        }
    }

    SDL_Rect **sizes = SDL_ListModes(NULL, flags);
    
    if(sizes && (sizes != (SDL_Rect**)(-1)))
    {
        width  = (*sizes)[0].w;
        height = (*sizes)[0].h;
    }

    bpp = SDL_VideoModeOK(width, height, bpp, flags);

    if(bpp)
    {
        pl2_screen_surf = SDL_SetVideoMode(width, height, bpp, flags);
        
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
        SDL_GL_SetAttribute(SDL_GL_RED_SIZE,     8);
        SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE,   8);
        SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE,    8);
        SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE,   8);
        SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE,  16);

        pl2SdlReshape(width, height);
        
        pl2GlInit();
        
        //pl2_timer = SDL_AddTimer(100, pl2SdlRedraw, NULL);

        return 1;
    }
    return 0;
}

