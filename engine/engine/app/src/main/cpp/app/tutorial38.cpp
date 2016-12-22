#include <math.h>
// #include <GL/glew.h>
#include <GLES/gl.h>
#include <GL/freeglut.h>
#include <string>

#ifndef WIN32

#include <sys/time.h>
#include <unistd.h>

#endif

#include <sys/types.h>

#include "ogldev_engine_common.h"
#include "ogldev_app.h"
#include "ogldev_camera.h"
#include "ogldev_util.h"
#include "ogldev_pipeline.h"
#include "texture.h"
#include "skinning_technique.h"
#include "ogldev_glut_backend.h"
#include "ogldev_skinned_mesh.h"
#include "gl3stub.h"

#include <EGL/egl.h>
#include <GLES/gl.h>

#include <android/sensor.h>
#include <android/log.h>
#include <android_native_app_glue.h>

using namespace std;

#define WINDOW_WIDTH  1280
#define WINDOW_HEIGHT 1024

#define HELLOWORLD 0
#define LOG_TAG "3D_TEST"

#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "native-activity", __VA_ARGS__))
#define LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN, "native-activity", __VA_ARGS__))
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)

extern "C" {
void extract_assets(struct android_app *app);
}

bool initialized = false;

EGLint w, h;

GLuint pointHandler = 0;

GLuint gProgram;
GLuint gvPositionHandle;


static void printGLString(const char *name, GLenum s) {
    const char *v = (const char *) glGetString(s);
    LOGI("GL %s = %s\n", name, v);
}

static void checkGlError(const char *op) {
    for (GLint error = glGetError(); error; error
                                                    = glGetError()) {
        LOGI("after %s() glError (0x%x)\n", op, error);
    }
}

class Tutorial38 : public ICallbacks, public OgldevApp {
public:

    Tutorial38() {
        m_pGameCamera = NULL;
        m_pEffect = NULL;
        m_directionalLight.Color = Vector3f(1.0f, 1.0f, 1.0f);
        m_directionalLight.AmbientIntensity = 0.55f;
        m_directionalLight.DiffuseIntensity = 0.9f;
        m_directionalLight.Direction = Vector3f(1.0f, 0.0, 0.0);

        m_persProjInfo.FOV = 60.0f;
        m_persProjInfo.Height = w;
        m_persProjInfo.Width = h;
        m_persProjInfo.zNear = 1.0f;
        m_persProjInfo.zFar = 100.0f;

        m_position = Vector3f(0.0f, 0.0f, 6.0f);
    }

    ~Tutorial38() {
        SAFE_DELETE(m_pEffect);
        SAFE_DELETE(m_pGameCamera);
    }

    bool Init() {
        Vector3f Pos(0.0f, 3.0f, -6.0f);
        Vector3f Target(0.0f, 0.0f, 1.0f);
        Vector3f Up(0.0, 1.0f, 0.0f);

        m_pGameCamera = new Camera(w, h, Pos, Target, Up);

        m_pEffect = new SkinningTechnique();

        if (!m_pEffect->Init()) {
            printf("Error initializing the lighting technique\n");
            return false;
        }

        m_pEffect->Enable();

        std::string str;
        // str.append("boblampclean.md5mesh");
        // str.append("marcus.dae");
        // str.append("ArmyPilot.dae");
        str.append("sf2arms.dae");
        // str.append("monkey.dae");


        m_pEffect->SetColorTextureUnit(COLOR_TEXTURE_UNIT_INDEX);
        m_pEffect->SetDirectionalLight(m_directionalLight);
        m_pEffect->SetMatSpecularIntensity(0.0f);
        m_pEffect->SetMatSpecularPower(0);

        if (!m_mesh.LoadMesh(str)) {
            printf("Mesh load failed\n");
            return false;
        }

#ifndef WIN32
        /* if (!m_fontRenderer.InitFontRenderer()) {
            return false;
        } */
#endif
        return true;
    }

    void Run() {
        // GLUTBackendRun(this);

        RenderSceneCB();
    }


    // virtual void RenderSceneCB()
    void RenderSceneCB() {
        CalcFPS();

        m_pGameCamera->OnRender();

        static float grey;
        grey += 0.01f;
        if (grey > 1.0f) {
            grey = 0.0f;
        }
        // glClearColor(grey, grey, grey, 1.0f);
        checkGlError("glClearColor");
        // glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
        checkGlError("glClear");

        m_pEffect->Enable();

        vector<Matrix4f> Transforms;

        float RunningTime = GetRunningTime();

        m_mesh.BoneTransform(RunningTime, Transforms);

        for (uint i = 0; i < Transforms.size(); i++) {
            m_pEffect->SetBoneTransform(i, Transforms[i]);
        }

        m_pEffect->SetEyeWorldPos(m_pGameCamera->GetPos());

        Pipeline p;
        p.SetCamera(m_pGameCamera->GetPos(), m_pGameCamera->GetTarget(), m_pGameCamera->GetUp());
        p.SetPerspectiveProj(m_persProjInfo);
        p.Scale(0.1f, 0.1f, 0.1f);

        Vector3f Pos(m_position);
        p.WorldPos(Pos);
        p.Rotate(270.0f, 180.0f, 0.0f);
        m_pEffect->SetWVP(p.GetWVPTrans());
        m_pEffect->SetWorldMatrix(p.GetWorldTrans());

        /* glEnable(GL_BLEND);
        glBlendFunc(GL_ONE, GL_ONE);
        glDepthMask(false);
        glDepthFunc(GL_EQUAL); */

        m_mesh.Render();
        RenderFPS();

        /* glDepthFunc(GL_LESS);
        glDepthMask(true);
        glDisable(GL_BLEND); */

        // glutSwapBuffers();
    }


    virtual void KeyboardCB(OGLDEV_KEY OgldevKey, OGLDEV_KEY_STATE State) {
        switch (OgldevKey) {
            case OGLDEV_KEY_ESCAPE:
            case OGLDEV_KEY_q:
                GLUTBackendLeaveMainLoop();
                break;
            default:
                m_pGameCamera->OnKeyboard(OgldevKey);
        }
    }


    virtual void PassiveMouseCB(int x, int y) {
        m_pGameCamera->OnMouse(x, y);
    }


private:

    SkinningTechnique *m_pEffect;
    Camera *m_pGameCamera;
    DirectionalLight m_directionalLight;
    SkinnedMesh m_mesh;
    Vector3f m_position;
    PersProjInfo m_persProjInfo;
};

Tutorial38 *pApp = NULL;

void draw_app(struct android_app *app) {

    pApp->Run();
}

static bool addShader(GLuint prog, GLenum ShaderType, const char *pFilename) {
    string s;

    if (!ReadFile(pFilename, s)) {
        return false;
    }

    GLuint ShaderObj = glCreateShader(ShaderType);

    if (ShaderObj == 0) {
        fprintf(stderr, "Error creating shader type %d\n", ShaderType);
        return false;
    }

    const GLchar *p[1];
    p[0] = s.c_str();
    GLint Lengths[1] = {(GLint) s.size()};

    glShaderSource(ShaderObj, 1, p, Lengths);

    glCompileShader(ShaderObj);

    GLint success;
    glGetShaderiv(ShaderObj, GL_COMPILE_STATUS, &success);

    if (!success) {
        GLchar InfoLog[1024];
        glGetShaderInfoLog(ShaderObj, 1024, NULL, InfoLog);
        fprintf(stderr, "Error compiling '%s': '%s'\n", pFilename, InfoLog);
        LOGI("Error compiling '%s': '%s'\n", pFilename, InfoLog);
        return false;
    }

    glAttachShader(prog, ShaderObj);

    return true;
}

GLuint m_Buffers[2];

static void CreateVertexBuffer() {
    Vector3f Vertices[4];
    Vertices[0] = Vector3f(-1.0f, -1.0f, 0.0f);
    Vertices[1] = Vector3f(1.0f, -1.0f, 0.0f);
    Vertices[2] = Vector3f(0.0f, 1.0f, 0.0f);
    Vertices[3] = Vector3f(-1.0f, 1.0f, 0.0f);

    GLubyte Indices[] = {
            0, 1, 3
    };

    int a = sizeof(Vertices);

    printf("%d", a);

    glGenBuffers(1, &m_Buffers[0]);
    glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);

    glGenBuffers(1, &m_Buffers[1]);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Buffers[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices), Indices, GL_STATIC_DRAW);
}

static void initShaders() {
    GLint Success = 0;
    GLchar ErrorLog[1024] = {0};

    GLuint prog = glCreateProgram();

    if (!addShader(prog, GL_VERTEX_SHADER, "skinning2.vs")) {
        return;
    }

    if (!addShader(prog, GL_FRAGMENT_SHADER, "skinning2.fs")) {
        return;
    }

    glLinkProgram(prog);

    glGetProgramiv(prog, GL_LINK_STATUS, &Success);
    if (Success == 0) {
        glGetProgramInfoLog(prog, sizeof(ErrorLog), NULL, ErrorLog);
        fprintf(stderr, "Error linking shader program: '%s'\n", ErrorLog);
        return;
    }

    glValidateProgram(prog);
    glGetProgramiv(prog, GL_VALIDATE_STATUS, &Success);
    if (!Success) {
        glGetProgramInfoLog(prog, sizeof(ErrorLog), NULL, ErrorLog);
        fprintf(stderr, "Invalid shader program: '%s'\n", ErrorLog);
        return;
    }

    gvPositionHandle = glGetAttribLocation(prog, "Position");
    checkGlError("glGetAttribLocation");
    LOGI("glGetAttribLocation(\"Position\") = %d\n",
         gvPositionHandle);

    gProgram = prog;
}

void init() {
    if (pApp == NULL) {
        pApp = new Tutorial38();
    }

    LOGI("in init:\n");

    if (!pApp->Init()) {
        return;
    }
}

/* Helloworld for buffer rendering */
void helloWorld(struct android_app *app) {

    static float grey;
    grey += 0.01f;
    if (grey > 1.0f) {
        grey = 0.0f;
    }
    glClearColor(grey, grey, grey, 1.0f);
    checkGlError("glClearColor");
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    checkGlError("glClear");

    glUseProgram(gProgram);
    checkGlError("glUseProgram");

    glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[0]);
    glVertexAttribPointer(gvPositionHandle, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(gvPositionHandle);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Buffers[1]);
    glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_BYTE, 0);
}


int main(int argc, char **argv) {

    LOGI("in android_main: tutorial38\n");

    GLUTBackendInit(argc, argv, true, false);

    LOGI("in android_main: tutorial38: 1\n");

    if (!GLUTBackendCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, false, "Tutorial 38")) {
        return -1;
    }

    LOGI("in android_main: tutorial38: 2\n");

    SRANDOM;

    return 0;
}

#include <cimport.h>

struct engine {
    struct android_app *app;

    EGLDisplay display;
    EGLSurface surface;
    EGLContext context;
    int32_t width;
    int32_t height;

    int32_t touchX;
    int32_t touchY;
};

/**
 * Initialize an EGL context for the current display.
 * TODO tidy this up, currently it's mostly Google example code
 */
int init_display(struct engine *engine) {

    // Setup OpenGL ES 2
    // http://stackoverflow.com/questions/11478957/how-do-i-create-an-opengl-es-2-context-in-a-native-activity

    const EGLint attribs[] = {
            EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT, //important
            EGL_BLUE_SIZE, 8,
            EGL_GREEN_SIZE, 8,
            EGL_RED_SIZE, 8,
            EGL_NONE
    };

    EGLint attribList[] =
            {
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

    ANativeWindow_setBuffersGeometry(engine->app->window, 0, 0, format);

    surface = eglCreateWindowSurface(display, config, engine->app->window, NULL);

    context = eglCreateContext(display, config, NULL, attribList);

    if (eglMakeCurrent(display, surface, surface, context) == EGL_FALSE) {
        LOGW("Unable to eglMakeCurrent");
        return -1;
    }

    // Grab the width and height of the surface
    eglQuerySurface(display, surface, EGL_WIDTH, &w);
    eglQuerySurface(display, surface, EGL_HEIGHT, &h);

    if (w > h) {
        w = h;
    } else {
        h = w;
    }

    engine->display = display;
    engine->context = context;
    engine->surface = surface;
    engine->width = w;
    engine->height = h;

    // Initialize GL state.
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);
    glEnable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    // glEnable(GL_DEPTH_TEST);
    glFrontFace(GL_CW);
    glCullFace(GL_BACK);
    glEnable(GL_TEXTURE_2D);
    glViewport(0, 0, w, h);

    int prog = glCreateProgram();

    if (prog == 0) {
        int err = GL_NO_ERROR;
        err = glGetError();
        LOGI("err = 0x%x\n", err);
    } else {
        LOGI("got prog = 0x%x\n", prog);
    }

    return 0;
}

/**
 * Just the current frame in the display.
 */
void draw_frame(struct engine *engine) {
    // No display.
    if (engine->display == NULL) {
        return;
    }

    // hack to test if opengl is ready for use
    if (!initialized) {

        int prog = glCreateProgram();
        if (prog == 0) {
            int err = GL_NO_ERROR;
            err = glGetError();
            LOGI("draw_frame: err = 0x%x\n", err);
        } else {
            LOGI("draw_frame: got prog = 0x%x in fg_main_android\n", prog);
            static float grey;
            grey += 0.01f;
            if (grey > 1.0f) {
                grey = 0.0f;
            }
            glClearColor(grey, grey, grey, 1.0f);

#if HELLOWORLD
            CreateVertexBuffer();
            initShaders();
#else
            init();
#endif

            initialized = true;
        }
    } else {
#if HELLOWORLD
        helloWorld(engine->app);
#else
        draw_app(engine->app);
#endif
        eglSwapBuffers(engine->display, engine->surface);
    }
}

/**
 * Tear down the EGL context currently associated with the display.
 */
void terminate_display(struct engine *engine) {
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
}

#define KEY_COUNT 10
int KeyCounter = 0;
int KeyDirectionFlag = 0;
#define MAX_STRIKE_TIME 1000 // sec
long long KeyTimeStamp = 0;

/**
 * 0: left, 1: right, otherwise n/a
 */
int scroll(struct engine *engine, float touchX) {

    // invalidate keys if time passed MAX_STRIKE_TIME
    if (KeyTimeStamp == 0) {
        KeyTimeStamp = GetCurrentTimeMillis();
    } else {
        long long current = GetCurrentTimeMillis();
        if (current - KeyTimeStamp >= MAX_STRIKE_TIME) {
            // reset key buffer
            KeyCounter = 0;
            KeyDirectionFlag = 0;
            KeyTimeStamp= current;
            return 2;
        }
    }

    if (touchX < engine->touchX) {
        if (KeyDirectionFlag == 0) {
            KeyCounter++;
        } else {
            KeyCounter = 0;
            KeyDirectionFlag = 0;
        }
    } else if (touchX > engine->touchX) {

        if (KeyDirectionFlag == 0) {
            KeyCounter = 0;
            KeyDirectionFlag = 1;
        } else {
            KeyCounter++;
        }

        // LOGI("touchX > engine->touchX: KeyCounter = %d, KeyDirectionFlag = %d\n", KeyCounter, KeyDirectionFlag);
    }

    if (KeyCounter >= KEY_COUNT) {
        KeyCounter = 0;

        // LOGI("KeyCounter >= KEY_COUNT: KeyDirectionFlag = %d\n", KeyDirectionFlag);
        if (KeyDirectionFlag == 0) {
            KeyTimeStamp = GetCurrentTimeMillis();
            return 0;
        } else {
            KeyTimeStamp = GetCurrentTimeMillis();
            return 1;
        }
    }

    return -1;
}

/**
 * Process the next input event.
 */
int32_t hello_handle_input(struct android_app *app, AInputEvent *event) {
    struct engine *engine = (struct engine *) app->userData;
    if (AInputEvent_getType(event) == AINPUT_EVENT_TYPE_MOTION) {

        int32_t action = AMotionEvent_getAction(event);
        if (action == AMOTION_EVENT_ACTION_MOVE) {
            float touchX = AMotionEvent_getX(event, 0);
            float touchY = AMotionEvent_getY(event, 0);

            int scrollFlag = scroll(engine, touchX);
            if (scrollFlag == 0) {
                LOGI("scroll left\n");
                pApp->KeyboardCB(OGLDEV_KEY_LEFT, OGLDEV_KEY_STATE_PRESS);
            } else if (scrollFlag == 1) {
                LOGI("scroll right\n");
                pApp->KeyboardCB(OGLDEV_KEY_RIGHT, OGLDEV_KEY_STATE_PRESS);
            }

            engine->touchX = touchX;
            engine->touchY = touchY;

            // pApp->PassiveMouseCB(touchX, touchY);

            // LOGI("x %d\ty %d\n", engine->touchX, engine->touchY);
        }

        /* int32_t action = AMotionEvent_getAction(event);
        if (action == AMOTION_EVENT_ACTION_MOVE) {
            LOGI("AMOTION_EVENT_ACTION_MOVE\n");
            LOGI("XOffset: %f\n", AMotionEvent_getXOffset(event));
        } else if (action == AMOTION_EVENT_ACTION_DOWN) {
            LOGI("AMOTION_EVENT_ACTION_DOWN");
        } else if (action == AMOTION_EVENT_ACTION_UP) {
            LOGI("AMOTION_EVENT_ACTION_UP");
        } else if (action == AMOTION_EVENT_ACTION_HOVER_MOVE) {
            LOGI("AMOTION_EVENT_ACTION_HOVER_MOVE");
        } else if (action == AMOTION_EVENT_ACTION_SCROLL) {
            LOGI("AMOTION_EVENT_ACTION_SCROLL");
        } else if (action == AMOTION_EVENT_ACTION_POINTER_DOWN) {
            LOGI("AMOTION_EVENT_ACTION_POINTER_DOWN");
        } */
        return 1;
    }
    return 0;
}

/**
 * Process the next main command.
 */
void hello_handle_cmd(struct android_app *app, int32_t cmd) {
    struct engine *engine = (struct engine *) app->userData;
    switch (cmd) {
        case APP_CMD_SAVE_STATE:
            break;
        case APP_CMD_INIT_WINDOW:
            // The window is being shown, get it ready.
            if (engine->app->window != NULL) {
                init_display(engine);
                draw_frame(engine);
            }
            break;
        case APP_CMD_TERM_WINDOW:
            // The window is being hidden or closed, clean it up.
            terminate_display(engine);
            break;
        case APP_CMD_LOST_FOCUS:
            draw_frame(engine);
            break;
    }
}

/**
 * Main entry point, handles events
 */
void android_main(struct android_app *state) {
    app_dummy();

    LOGI("in android_main\n");

    aiGetErrorString();

    LOGI("after aiGetErrorString\n");

    struct engine engine;

    memset(&engine, 0, sizeof(engine));
    state->userData = &engine;
    state->onAppCmd = hello_handle_cmd;
    state->onInputEvent = hello_handle_input;
    engine.app = state;

    extract_assets(state);

    // Read all pending events.
    while (1) {
        int ident;
        int events;
        struct android_poll_source *source;

        while ((ident = ALooper_pollAll(0, NULL, &events, (void **) &source)) >= 0) {

            // Process this event.
            if (source != NULL) {
                source->process(state, source);
            }

            // Check if we are exiting.
            if (state->destroyRequested != 0) {
                terminate_display(&engine);
                exit(1); // exit directly
            }
        }

        // Draw the current frame
        draw_frame(&engine);
    }
}
