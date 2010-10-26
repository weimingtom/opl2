//#include "opl2.h"
#include "opl2_int.h"
#include "opl2_vm.h"

static SDL_Surface *pl2_screen_surf = NULL;

enum
{
   MOVE_NONE  = 0,
   MOVE_3P    = 1,
   MOVE_ORTHO = 2,
   MOVE_1P    = 3,
};

static int move_mode = 0, mouse_x = -1, mouse_y = -1;

void pl2SdlRedraw()
{
    SDL_GL_SwapBuffers();
}

int pl2SdlDoFrame()
{
    SDL_Event event;

    SDL_PumpEvents();

    if(SDL_WaitEvent(&event))
    {
        switch(event.type)
        {
            case SDL_USEREVENT:
                //DEBUGPRINT("%s: redisplay\n", __func__);
                pl2GlutDisplayFunc();
                break;

            case SDL_KEYDOWN:
                break;

            case SDL_MOUSEMOTION: {
                int dx, dy, buttons = SDL_GetRelativeMouseState(&dx, &dy);
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

            case SDL_QUIT:
                DEBUGPRINT("%s: exiting\n", __func__);
                exit(0);
                //pl2_running = 0;
                break;

            default:
                break;
        }
    }

    return pl2_running;
}

int pl2SdlInit(int *argc, char *argv[])
{
    SDL_Init(SDL_INIT_TIMER | SDL_INIT_AUDIO | SDL_INIT_VIDEO);
    atexit(SDL_Quit);

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE,     8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE,   8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE,    8);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE,   8);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE,  16);

    int bpp = 32, flags = /*SDL_FULLSCREEN |*/ SDL_OPENGL | SDL_ANYFORMAT | SDL_HWSURFACE;

    bpp = SDL_VideoModeOK(640, 480, bpp, flags);

    if(bpp)
        pl2_screen_surf = SDL_SetVideoMode(640, 480, bpp, flags);
    else
        return 0;
}
