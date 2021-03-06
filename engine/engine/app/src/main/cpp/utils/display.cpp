#include "engine.h"

#define LOG_TAG "ENGINE_DISPLAY"
#include "AppLog.h"

/**
 * Initialize an EGL context for the current display.
 * TODO tidy this up, currently it's mostly Google example code
 */
int InitDisplay(struct engine *engine) {

    // Setup OpenGL ES 2
    // http://stackoverflow.com/questions/11478957/how-do-i-create-an-opengl-es-2-context-in-a-native-activity

    const EGLint attribs[] = {
            EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT, //important
            EGL_BLUE_SIZE, 8,
            EGL_GREEN_SIZE, 8,
            EGL_RED_SIZE, 8,
            EGL_NONE
    };

    EGLint attribList[] = {
            EGL_CONTEXT_CLIENT_VERSION, 2,
            EGL_NONE
    };

    EGLint dummy, format;
    EGLint numConfigs;
    EGLConfig config;
    EGLSurface surface;
    EGLContext context;

    EGLDisplay display = eglGetDisplay(EGL_DEFAULT_DISPLAY);

    eglInitialize(display, 0, 0);

    /* Here, the application chooses the configuration it desires. In this
     * sample, we have a very simplified selection process, where we pick
     * the first EGLConfig that matches our criteria */
    eglChooseConfig(display, attribs, &config, 1, &numConfigs);

    /* EGL_NATIVE_VISUAL_ID is an attribute of the EGLConfig that is
     * guaranteed to be accepted by ANativeWindow_setBuffersGeometry().
     * As soon as we picked a EGLConfig, we can safely reconfigure the
     * ANativeWindow buffers to match, using EGL_NATIVE_VISUAL_ID. */
    eglGetConfigAttrib(display, config, EGL_NATIVE_VISUAL_ID, &format);

    ANativeWindow_setBuffersGeometry(engine->m_App->window, 0, 0, format);

    surface = eglCreateWindowSurface(display, config, engine->m_App->window, NULL);

    context = eglCreateContext(display, config, NULL, attribList);

    if (eglMakeCurrent(display, surface, surface, context) == EGL_FALSE) {
        LOGW("Unable to eglMakeCurrent");
        return -1;
    }

    int w, h;
    // Grab the width and height of the surface
    eglQuerySurface(display, surface, EGL_WIDTH, &w);
    eglQuerySurface(display, surface, EGL_HEIGHT, &h);

    engine->m_Display = display;
    engine->m_Context = context;
    engine->m_Surface = surface;
    engine->m_Width = w;
    engine->m_Height = h;


    // Initialize GL state.
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);
    // glDisable(GL_DEPTH_TEST);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    // Enable culling front faces or back faces
    glEnable(GL_CULL_FACE);
    // Specify clockwise indexed are front face, clockwise is determined by the sequence in index buffer
    glFrontFace(GL_CW);
    // Cull the back face
    glCullFace(GL_BACK);

    glEnable(GL_TEXTURE_2D);
    glViewport(0, 0, w, h);

    // Enable alpha
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

    GLint texUnits = 0;
    glGetIntegerv(GL_MAX_TEXTURE_UNITS, &texUnits);
    LOGI("Maximum texture units : %d", texUnits);

    return 0;
}

/**
 * Tear down the EGL context currently associated with the display.
 */
void TerminateDisplay(struct engine *engine) {
    if (engine->m_Display != EGL_NO_DISPLAY) {
        eglMakeCurrent(engine->m_Display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        if (engine->m_Context != EGL_NO_CONTEXT) {
            eglDestroyContext(engine->m_Display, engine->m_Context);
        }
        if (engine->m_Surface != EGL_NO_SURFACE) {
            eglDestroySurface(engine->m_Display, engine->m_Surface);
        }
        eglTerminate(engine->m_Display);
    }
    engine->m_Display = EGL_NO_DISPLAY;
    engine->m_Context = EGL_NO_CONTEXT;
    engine->m_Surface = EGL_NO_SURFACE;
}
