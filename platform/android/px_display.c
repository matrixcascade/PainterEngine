#include "px_display.h"

static const char PX_AndroidEngine_GL_VS[]="attribute vec4 vPosition;attribute vec4 vTex;varying vec2 textureCoordinate;void main(){gl_Position = vPosition;textureCoordinate = vTex.xy;}";
static const char PX_AndroidEngine_GL_PS[]="precision mediump float;varying highp vec2 textureCoordinate;uniform sampler2D tex;void main(){gl_FragColor = texture2D(tex, textureCoordinate);}";

static const GLfloat PX_AndroidEngine_squareVertices[] = {
        -1.0f, -1.0f,
        1.0f, -1.0f,
        -1.0f,  1.0f,
        1.0f,  1.0f,
};

static const GLfloat PX_AndroidEngine_texturePosition[] = {
        0.0f, 1.0f,
        1.0f, 1.0f,
        0.0f,  0.0f,
        1.0f,  0.0f,
};




static GLuint PX_AndroidEngine_LoadShader(GLenum type,const px_char *shaderSources)
{
    GLuint shader;
    GLint compiled;

    shader=glCreateShader(type);
    if(shader==0) return  PX_FALSE;

    glShaderSource(shader,1,&shaderSources,PX_NULL);
    glCompileShader(shader);

    glGetShaderiv(shader,GL_COMPILE_STATUS,&compiled);

    if(!compiled)
    {
      glDeleteShader(shader);
        return PX_FALSE;
    }
    return  shader;
}

px_bool  PX_AndroidEngine_InitializeDisplay(PX_AndroidEngine *engine)
{
    const int attrib_list[] = { EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE };
    const EGLint attribs[] = {
            EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
            EGL_BLUE_SIZE, 8,
            EGL_GREEN_SIZE, 8,
            EGL_RED_SIZE, 8,
            EGL_NONE
    };
    EGLint w, h, format;
    EGLint numConfigs;
    EGLConfig config;
    EGLSurface surface;
    EGLContext context;
    EGLConfig ConfigsDesc[64];
    px_int i;
    GLint linked;
    EGLDisplay display = eglGetDisplay(EGL_DEFAULT_DISPLAY);



    eglInitialize(display, 0, 0);
    eglChooseConfig(display, attribs, PX_NULL,0, &numConfigs);
    if(numConfigs>PX_COUNTOF(ConfigsDesc))
    {
        numConfigs=PX_COUNTOF(ConfigsDesc);
    }
    eglChooseConfig(display, attribs,ConfigsDesc, numConfigs, &numConfigs);

    for (i=0; i < numConfigs; i++)
    {
        EGLint r, g, b, d, a;
        if (eglGetConfigAttrib(display, ConfigsDesc[i], EGL_RED_SIZE, &r)   &&
            eglGetConfigAttrib(display, ConfigsDesc[i], EGL_GREEN_SIZE, &g) &&
            eglGetConfigAttrib(display, ConfigsDesc[i], EGL_BLUE_SIZE, &b)  &&
            eglGetConfigAttrib(display, ConfigsDesc[i], EGL_ALPHA_SIZE, &a)&&
            eglGetConfigAttrib(display, ConfigsDesc[i], EGL_DEPTH_SIZE, &d) &&
            r == 8 && g == 8 && b == 8 &&a == 8&&d==0 )
        {
            config = ConfigsDesc[i];
            break;
        }
    }

    if (i == numConfigs) {
        config = ConfigsDesc[0];
    }

    /* EGL_NATIVE_VISUAL_ID is an attribute of the EGLConfig that is
     * guaranteed to be accepted by ANativeWindow_setBuffersGeometry().
     * As soon as we picked a EGLConfig, we can safely reconfigure the
     * ANativeWindow buffers to match, using EGL_NATIVE_VISUAL_ID. */
    eglGetConfigAttrib(display, config, EGL_NATIVE_VISUAL_ID, &format);
    surface = eglCreateWindowSurface(display, config, engine->pAndroidApp->window, NULL);
    context = eglCreateContext(display, config, EGL_NO_CONTEXT, attrib_list);

    if (eglMakeCurrent(display, surface, surface, context) == EGL_FALSE)
    {
        return PX_FALSE;
    }

    eglQuerySurface(display, surface, EGL_WIDTH, &w);
    eglQuerySurface(display, surface, EGL_HEIGHT, &h);

    engine->display = display;
    engine->context = context;
    engine->surface = surface;
    engine->width = w;
    engine->height = h;

    // Initialize GL state.
    glEnable(GL_CULL_FACE);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_TEXTURE0);
    glEnable(GL_TEXTURE1);
    glDisable(GL_DEPTH_TEST);

    //Initialize GL shader
    engine->vertexShader=PX_AndroidEngine_LoadShader(GL_VERTEX_SHADER,PX_AndroidEngine_GL_VS);
    if(engine->vertexShader==0)
        return  PX_FALSE;
    engine->pixelsShader=PX_AndroidEngine_LoadShader(GL_FRAGMENT_SHADER,PX_AndroidEngine_GL_PS);
    if(engine->pixelsShader==0)
        return PX_FALSE;

    engine->programObject=glCreateProgram();
    if(engine->programObject==0)
        return PX_FALSE;

    glAttachShader(engine->programObject,engine->vertexShader);
    glAttachShader(engine->programObject,engine->pixelsShader);

    glLinkProgram(engine->programObject);

    glGetProgramiv(engine->programObject,GL_LINK_STATUS,&linked);
    if(!linked)
    {
        glDeleteProgram(engine->programObject);
        return PX_FALSE;
    }

    glGenTextures(1,&engine->renderTexture);

    engine->vPosition=glGetAttribLocation(engine->programObject,"vPosition");
    engine->vTex=glGetAttribLocation(engine->programObject,"vTex");

    glViewport(0,0,engine->width,engine->height);


    return PX_TRUE;
}

px_bool  PX_AndroidEngine_FreeDisplay(PX_AndroidEngine *engine)
{
    if (engine->display != EGL_NO_DISPLAY) {
        eglMakeCurrent(engine->display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        if (engine->context != EGL_NO_CONTEXT) {
            eglDestroyContext(engine->display, engine->context);
        }
        if (engine->surface != EGL_NO_SURFACE) {
            eglDestroySurface(engine->display, engine->surface);
        }
        eglTerminate(engine->display);
    }

    engine->display = EGL_NO_DISPLAY;
    engine->context = EGL_NO_CONTEXT;
    engine->surface = EGL_NO_SURFACE;

    glDetachShader(engine->programObject,engine->vertexShader);
    glDetachShader(engine->programObject,engine->pixelsShader);
    glDeleteShader(engine->pixelsShader);
    glDeleteShader(engine->vertexShader);
    glDeleteProgram(engine->programObject);

    return  PX_TRUE;
}

px_void PX_AndroidEngine_Render(PX_AndroidEngine* engine,px_int width,px_int height,px_byte *buffer) {
    if (engine->display == NULL)
    {
        // No display.
        return;
    }

    //vertex data
    glClearColor(0.0f,0.0f,0.0f,1.0f);
    glClear (GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

    glUseProgram(engine->programObject);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D,engine->renderTexture);
    glTexImage2D(GL_TEXTURE_2D,\
    0,\
    GL_RGBA,\
    width,\
    height,\
    0,\
    GL_RGBA,\
    GL_UNSIGNED_BYTE,\
    buffer\
    );
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glUniform1i(glGetAttribLocation(engine->programObject,"tex"), 0);

    glVertexAttribPointer(engine->vPosition, 2, GL_FLOAT, GL_FALSE, 0,PX_AndroidEngine_squareVertices);
    glEnableVertexAttribArray(engine->vPosition);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    glVertexAttribPointer(engine->vTex, 2, GL_FLOAT, 0, 0, PX_AndroidEngine_texturePosition);
    glEnableVertexAttribArray(engine->vTex);

    eglSwapBuffers(engine->display, engine->surface);
}