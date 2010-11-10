#ifndef __OPL2_GL_H__
#define __OPL2_GL_H__

#include <GL/gl.h>
#include <GL/glu.h>
//#include <GL/glut.h>
//#include <GL/freeglut.h>

int pl2GlInit();
int pl2GlutInit(int *argc, char *argv[]);
void pl2GlRenderFrame(float dt);

#endif // __OPL2_GL_H__

