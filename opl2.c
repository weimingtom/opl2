#include "opl2.h"

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

#include <AL/al.h>
#include <AL/alut.h>

int main(int argc, char *argv[])
{
	alutInit(&argc, argv);

	glutInit(&argc, argv);


	alutExit();

	return 0;
}

