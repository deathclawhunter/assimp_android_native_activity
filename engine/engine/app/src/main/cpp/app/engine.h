#include <math.h>
#include <GLES/gl.h>
#include <GL/freeglut.h>
#include <string>
#include <EGL/egl.h>
#include <android/sensor.h>
#include <android_native_app_glue.h>

#include "texture.h"
#include "gl3stub.h"

struct engine {
    struct android_app *app;

    EGLDisplay display;
    EGLSurface surface;
    EGLContext context;
    int32_t width;
    int32_t height;

    int32_t touchX;
    int32_t touchY;

    bool initialized = false;

    // engine app plugin callbacks
    void* (*pfInit)(int32_t width, int32_t height);
    void (*pfDrawFrame)(void *pContext);
    int32_t (*pfKeyHandler)(void *pContext, AInputEvent *event);

    // private data storage for engine app plugins
    void *pContext;
};


