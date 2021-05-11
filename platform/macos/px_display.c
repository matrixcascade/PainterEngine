#include "px_display.h"
unsigned int rendertexture;

int PX_CreateWindow(int surfaceWidth,int surfaceHeight,int windowWidth,int windowHeight, const char *name)
{
	glutInitWindowSize(windowWidth, windowHeight);
	glutCreateWindow(name);
	glGenTextures(1,&rendertexture);
	glBindTexture(GL_TEXTURE_2D, rendertexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	return 1;
}

int PX_GetScreenHeight()
{
	return glutGet(GLUT_SCREEN_HEIGHT);
}

int PX_GetScreenWidth()
{
	return glutGet(GLUT_SCREEN_WIDTH);
}

int PX_SystemLoop()
{
	glutMainLoop();
	return 1;
}


