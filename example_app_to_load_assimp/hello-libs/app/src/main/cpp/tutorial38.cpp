/*

	Copyright 2011 Etay Meiri

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

    Tutorial 38 - Skinning
*/

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

#include <EGL/egl.h>
#include <GLES/gl.h>

#include <android/sensor.h>
#include <android/log.h>
#include <android_native_app_glue.h>

using namespace std;

#define WINDOW_WIDTH  1280  
#define WINDOW_HEIGHT 1024

#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "native-activity", __VA_ARGS__))
#define LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN, "native-activity", __VA_ARGS__))

extern "C" {
    void extract_assets(struct android_app* app);
}

bool initialized = false;

class Tutorial38 : public ICallbacks, public OgldevApp
{
public:

    Tutorial38() 
    {
        m_pGameCamera = NULL;
        m_pEffect = NULL;
        m_directionalLight.Color = Vector3f(1.0f, 1.0f, 1.0f);
        m_directionalLight.AmbientIntensity = 0.55f;
        m_directionalLight.DiffuseIntensity = 0.9f;
        m_directionalLight.Direction = Vector3f(1.0f, 0.0, 0.0);

        m_persProjInfo.FOV = 60.0f;
        m_persProjInfo.Height = WINDOW_HEIGHT;
        m_persProjInfo.Width = WINDOW_WIDTH;
        m_persProjInfo.zNear = 1.0f;
        m_persProjInfo.zFar = 100.0f;  
        
        m_position = Vector3f(0.0f, 0.0f, 6.0f);      
    }

    ~Tutorial38()
    {
        SAFE_DELETE(m_pEffect);
        SAFE_DELETE(m_pGameCamera);
    }    

    bool Init()
    {
        Vector3f Pos(0.0f, 3.0f, -1.0f);
        Vector3f Target(0.0f, 0.0f, 1.0f);
        Vector3f Up(0.0, 1.0f, 0.0f);

        m_pGameCamera = new Camera(WINDOW_WIDTH, WINDOW_HEIGHT, Pos, Target, Up);
      
        m_pEffect = new SkinningTechnique();

        if (!m_pEffect->Init()) {
            printf("Error initializing the lighting technique\n");
            return false;
        }

        m_pEffect->Enable();

        m_pEffect->SetColorTextureUnit(COLOR_TEXTURE_UNIT_INDEX);
        m_pEffect->SetDirectionalLight(m_directionalLight);
        m_pEffect->SetMatSpecularIntensity(0.0f);
        m_pEffect->SetMatSpecularPower(0);

        if (!m_mesh.LoadMesh("boblampclean.md5mesh")) {
            printf("Mesh load failed\n");
            return false;
        }
        
#ifndef WIN32
        /* if (!m_fontRenderer.InitFontRenderer()) {
            return false;
        } */
#endif
        initialized = true;
        return true;
    }

    void Run()
    {
        GLUTBackendRun(this);
    }
    

    virtual void RenderSceneCB()
    {

        if (!Init()) {
            return;
        }

        CalcFPS();
        
        m_pGameCamera->OnRender();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        m_pEffect->Enable();
        
        vector<Matrix4f> Transforms;
               
        float RunningTime = GetRunningTime();

        m_mesh.BoneTransform(RunningTime, Transforms);
        
        for (uint i = 0 ; i < Transforms.size() ; i++) {
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

        m_mesh.Render();
                              
        RenderFPS();
        
        glutSwapBuffers();
    }


	virtual void KeyboardCB(OGLDEV_KEY OgldevKey, OGLDEV_KEY_STATE State)
	{
		switch (OgldevKey) {
		case OGLDEV_KEY_ESCAPE:
		case OGLDEV_KEY_q:
			GLUTBackendLeaveMainLoop();
			break;
		default:
			m_pGameCamera->OnKeyboard(OgldevKey);
		}
	}


	virtual void PassiveMouseCB(int x, int y)
	{
		m_pGameCamera->OnMouse(x, y);
	}
    
    
private:      
 
    SkinningTechnique* m_pEffect;
    Camera* m_pGameCamera;
    DirectionalLight m_directionalLight;
    SkinnedMesh m_mesh;
    Vector3f m_position;            
    PersProjInfo m_persProjInfo;
};


void startPlay(struct android_app* app) {

    extract_assets(app);

    Tutorial38* pApp = new Tutorial38();

    LOGI("in android_main: tutorial38: 4\n");

    if (!pApp->Init()) {
        return;
    }

    LOGI("in android_main: tutorial38: 5\n");

    pApp->Run();

    LOGI("in android_main: tutorial38: 6\n");

    delete pApp;
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

    // LOGI("in android_main: tutorial38: 3\n");
    
    // Tutorial38* pApp = new Tutorial38();

    // LOGI("in android_main: tutorial38: 4\n");

    /* if (!pApp->Init()) {
        return -1;
    } */

    // LOGI("in android_main: tutorial38: 5\n");
    
    // pApp->Run();

    // LOGI("in android_main: tutorial38: 6\n");

    // delete pApp;

    // LOGI("in android_main: tutorial38: 7\n");

    return 0;
}

/// From helloworld

#include <cimport.h>
#include <GLES2/gl2.h>

struct engine {
    struct android_app* app;

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
int init_display(struct engine* engine) {

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

    EGLint w, h, dummy, format;
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

    engine->display = display;
    engine->context = context;
    engine->surface = surface;
    engine->width = w;
    engine->height = h;

    // Initialize GL state.
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);
    glEnable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
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
void draw_frame(struct engine* engine) {
    // No display.
    if (engine->display == NULL) {
        return;
    }

    glClearColor(255, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT);
    eglSwapBuffers(engine->display, engine->surface);

    /* if (!initializedGlut) {
        _android_main(engine->app);
        initializedGlut = true;
    } */

    // hack to test if opengl is ready for use
    if (!initialized) {
        int prog = glCreateProgram();
        if (prog == 0) {
            int err = GL_NO_ERROR;
            err = glGetError();
            LOGI("draw_frame: err = 0x%x\n", err);
        } else {
            LOGI("draw_frame: got prog = 0x%x in fg_main_android\n", prog);
            startPlay(engine->app);
            initialized = true;
        }
    }
}

/**
 * Tear down the EGL context currently associated with the display.
 */
void terminate_display(struct engine* engine) {
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

/**
 * Process the next input event.
 */
int32_t hello_handle_input(struct android_app *app, AInputEvent *event) {
    struct engine* engine = (struct engine*)app->userData;
    if (AInputEvent_getType(event) == AINPUT_EVENT_TYPE_MOTION) {
        engine->touchX = AMotionEvent_getX(event, 0);
        engine->touchY = AMotionEvent_getY(event, 0);
        LOGI("x %d\ty %d\n",engine->touchX,engine->touchY);
        return 1;
    }
    return 0;
}

/**
 * Process the next main command.
 */
void hello_handle_cmd(struct android_app *app, int32_t cmd) {
    struct engine* engine = (struct engine*)app->userData;
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
void android_main(struct android_app* state) {
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

    // Read all pending events.
    while (1) {
        int ident;
        int events;
        struct android_poll_source* source;

        while ((ident=ALooper_pollAll(0, NULL, &events,(void**)&source)) >= 0) {

            // Process this event.
            if (source != NULL) {
                source->process(state, source);
            }

            // Check if we are exiting.
            if (state->destroyRequested != 0) {
                terminate_display(&engine);
                return;
            }
        }

        // Draw the current frame
        draw_frame(&engine);
    }
}
