//Platform supports
#include "platform/linux/px_display.h"
#include "PainterEngine_Application.h"
static px_dword lastupdatetime;

extern unsigned int rendertexture;
static px_int windowWidth;
static px_int windowHeight;

px_void PX_glutDisplayResize(px_int width,px_int height)
{
	PX_Object_Event e;
	e.Event=PX_OBJECT_EVENT_WINDOWRESIZE;
	PX_Object_Event_SetWidth(&e,width);
	PX_Object_Event_SetHeight(&e,height);
	PX_ApplicationPostEvent(&App,e);
}

void PX_glutDisplayFunction(void)
{
	px_dword elpased;
	px_dword timenow;
	px_surface *pRenderSurface=&App.runtime.RenderSurface;

	timenow=glutGet(GLUT_ELAPSED_TIME);
	elpased=timenow-lastupdatetime;
	lastupdatetime=timenow;
	PX_ApplicationUpdate(&App,elpased);
	PX_ApplicationRender(&App,elpased);
	glClear(GL_COLOR_BUFFER_BIT);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, pRenderSurface->width, pRenderSurface->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pRenderSurface->surfaceBuffer);
	glEnable(GL_TEXTURE_2D);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0, 0.0); glVertex3f(-1.0, 1.0, 0.0);
	glTexCoord2f(0.0, 1.0); glVertex3f(-1.0, -1.0, 0.0);
	glTexCoord2f(1.0, 1.0); glVertex3f(1.0, -1.0, 0.0);
	glTexCoord2f(1.0, 0.0); glVertex3f(1.0, 1.0, 0.0);
	glEnd();

	glutSwapBuffers();
}
px_void PX_glutOnAnyKeyDown(int key,px_int x,px_int y)
{
	PX_Object_Event e;
	e.Event=PX_OBJECT_EVENT_KEYDOWN;
	PX_Object_Event_SetKeyDown(&e,key);
	PX_ApplicationPostEvent(&App,e);
}
px_void PX_glutOnAnsiKeyDown(px_uchar key,px_int x,px_int y)
{
	PX_Object_Event e;
	px_char str[2]={0};
	e.Event=PX_OBJECT_EVENT_KEYDOWN;
	PX_Object_Event_SetKeyDown(&e,key);
	PX_ApplicationPostEvent(&App,e);
	str[0]=key;
	e.Event=PX_OBJECT_EVENT_STRING;
	PX_Object_Event_SetStringPtr(&e,str);
	PX_ApplicationPostEvent(&App,e);
}

px_void PX_glutOnMouseEvent(int button,int state,int x,int y)
{
	PX_Object_Event e;
	px_float cursorx_scale=App.runtime.surface_width*1.0f/App.runtime.window_width;
	px_float cursory_scale=App.runtime.surface_height*1.0f/App.runtime.window_height;
	switch(button)
	{
	case GLUT_LEFT_BUTTON:
		{
			if (state==GLUT_DOWN)
			{
				e.Event=PX_OBJECT_EVENT_CURSORDOWN;
			}
			else
			{
				e.Event=PX_OBJECT_EVENT_CURSORUP;
			}
		}
		break;
	case GLUT_RIGHT_BUTTON:
		{
			if (state==GLUT_DOWN)
			{
				e.Event=PX_OBJECT_EVENT_CURSORRDOWN;
			}
			else
			{
				e.Event=PX_OBJECT_EVENT_CURSORRUP;
			}
		}
		break;
	default:
		break;
	}
	PX_Object_Event_SetCursorX(&e,x*cursorx_scale);
	PX_Object_Event_SetCursorY(&e,y*cursorx_scale);
	PX_ApplicationPostEvent(&App,e);
}
px_void PX_glutOnMouseDragEvent(int x,int y)
{
	PX_Object_Event e;
	px_float cursorx_scale=App.runtime.surface_width*1.0f/App.runtime.window_width;
	px_float cursory_scale=App.runtime.surface_height*1.0f/App.runtime.window_height;
	e.Event=PX_OBJECT_EVENT_CURSORDRAG;
	PX_Object_Event_SetCursorX(&e,x*cursorx_scale);
	PX_Object_Event_SetCursorY(&e,y*cursorx_scale);
	PX_ApplicationPostEvent(&App,e);
}
px_void PX_glutOnMouseMoveEvent(int x,int y)
{
	PX_Object_Event e;
	px_float cursorx_scale=App.runtime.surface_width*1.0f/App.runtime.window_width;
	px_float cursory_scale=App.runtime.surface_height*1.0f/App.runtime.window_height;
	e.Event=PX_OBJECT_EVENT_CURSORMOVE;
	PX_Object_Event_SetCursorX(&e,x*cursorx_scale);
	PX_Object_Event_SetCursorY(&e,y*cursorx_scale);
	PX_ApplicationPostEvent(&App,e);
}


int main(int argc,char **argv)
{
	glutInit(&argc,argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA );

	PX_srand(314159);
	if(!PX_ApplicationInitialize(&App,PX_GetScreenWidth(),PX_GetScreenHeight()))return 0;
	if(!PX_CreateWindow(App.runtime.surface_width,App.runtime.surface_height,App.runtime.window_width,App.runtime.window_height,PX_APPLICATION_NAME)) 
	{
		return 0;
	}
	windowWidth=App.runtime.window_width;
	windowHeight=App.runtime.window_height;

	lastupdatetime=glutGet(GLUT_ELAPSED_TIME);
	glutDisplayFunc(PX_glutDisplayFunction);
	glutIdleFunc(PX_glutDisplayFunction);
	glutReshapeFunc(PX_glutDisplayResize);
	glutMouseFunc(PX_glutOnMouseEvent);
	glutMotionFunc(PX_glutOnMouseDragEvent);
	glutPassiveMotionFunc(PX_glutOnMouseMoveEvent);
	glutKeyboardFunc(PX_glutOnAnsiKeyDown);
	glutSpecialFunc(PX_glutOnAnyKeyDown);
	PX_SystemLoop();
	return 0;
}