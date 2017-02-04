#include <GLES/gl.h>
#include <GL/freeglut.h>
#include <EGL/egl.h>
#include <android/sensor.h>
#include <android_native_app_glue.h>

#include "GL3Stub.h"

/**
 * Android does not expose AInputEvent directly, we have
 * to buffer this data structure in our own way.
 */
typedef struct SysInputEvent_struct {
    int32_t m_EventType;
    int32_t m_EventAction;
    size_t m_EventPointerCount;
    float m_X0, m_Y0, m_X1, m_Y1;
} SysInputEvent;

struct engine {
    struct android_app *m_App;

    EGLDisplay m_Display;
    EGLSurface m_Surface;
    EGLContext m_Context;
    int32_t m_Width;
    int32_t m_Height;
    bool m_Initialized = false;
    int m_MovementCount = 0;
    int m_PreviousCount = -1;

    ASensorManager *m_SensorManager = NULL;
    const ASensor *m_AccelerometerSensor = NULL;
    ASensorEventQueue *m_SensorEventQueue = NULL;
    bool m_SensorEnabled = false;

    float m_X, m_Y;
    SysInputEvent m_InputEvent;
};


