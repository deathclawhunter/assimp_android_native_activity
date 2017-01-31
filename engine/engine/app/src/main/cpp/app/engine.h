#include <GLES/gl.h>
#include <GL/freeglut.h>
#include <EGL/egl.h>
#include <android/sensor.h>
#include <android_native_app_glue.h>

#include "GL3Stub.h"

struct engine {
    struct android_app *m_App;

    EGLDisplay m_Display;
    EGLSurface m_Surface;
    EGLContext m_Context;
    int32_t m_Width;
    int32_t m_Height;
    bool m_Initialized = false;

    ASensorManager *m_SensorManager;
    const ASensor *m_AccelerometerSensor;
    ASensorEventQueue *m_SensorEventQueue;

    float m_X, m_Y;
};


