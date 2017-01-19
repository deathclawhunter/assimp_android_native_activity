#include <GLES/gl.h>
#include <GL/freeglut.h>
#include <EGL/egl.h>
#include <android/sensor.h>
#include <android_native_app_glue.h>

#include "gl3stub.h"

struct engine {
    struct android_app *app;

    EGLDisplay display;
    EGLSurface surface;
    EGLContext context;
    int32_t width;
    int32_t height;
    bool initialized = false;
};


