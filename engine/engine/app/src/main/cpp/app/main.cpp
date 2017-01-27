#define LOG_TAG "ENGINE_MAIN"

#include "engine.h"

using namespace std;

#include "AppLog.h"
#include "GLError.h"
#include "video.h"
#include "app.h"
#include "PluginManager.h"

#define HELLOWORD 0

extern "C" {
void extract_assets(struct android_app *app);
}

int init_display(struct engine *engine);

void terminate_display(struct engine *engine);

/**
 * Just the current frame in the display.
 */
void draw_frame(struct engine *engine) {

    // No display.
    if (engine->display == NULL) {
        return;
    }

    // FIXME: hack to test if opengl is ready for use,
    // sometimes the glCreateProgram() will fail even
    // when reach here
    if (!engine->initialized) {

        int prog = glCreateProgram();
        if (prog == 0) {
            checkGlError("test opengl: ");
        } else {

            static float grey;
            grey += 0.01f;
            if (grey > 1.0f) {
                grey = 0.0f;
            }
            glClearColor(grey, grey, grey, 1.0f);
            checkGlError("glClearColor");

            LOGI("draw_frame: got prog = 0x%x in main\n", prog);
            engine->initialized = true;

            // Initialize game camera
            AppCamera::GetInstance(engine->width, engine->height);

            // Initialize pulgins
            PluginManager::GetInstance()->Init(engine->width, engine->height);
        }
    } else {
        if (PluginManager::GetInstance()->Draw()) {
            eglSwapBuffers(engine->display, engine->surface);
        }
    }
}

/**
 * Process the next input event.
 */
int32_t app_input_handler(struct android_app *app, AInputEvent *event) {
    if (AInputEvent_getType(event) == AINPUT_EVENT_TYPE_MOTION) {
        return PluginManager::GetInstance()->KeyHandler(event);
    }

    return 0;
}

/**
 * Process the next main command.
 */
void app_cmd_handler(struct android_app *app, int32_t cmd) {
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

#include "helloworld.h"
#include "audio_plugin.h"
#include "HUD.h"
#include "text.h"
#include "skybox.h"

void initPlugins(struct engine *engine) {

#if HELLOWORD
    HelloWorldPlugin *helloWorld = new HelloWorldPlugin();
    engine->m_Plugins = helloWorld;
#else
    // Should be controlled by script, hard code right now for demo
    // sequence matters, check dev notes for game flow control
    /* PluginManager::GetInstance()->AddPlugin(PluginManager::PLUGIN_TYPE_START_MUSIC,
                                            new AudioPlugin());
    PluginManager::GetInstance()->AddPlugin(PluginManager::PLUGIN_TYPE_START_VIDEO,
                                            new VideoPlugin());
    PluginManager::GetInstance()->AddPlugin(PluginManager::PLUGIN_TYPE_SCENE, new ScenePlugin());
    PluginManager::GetInstance()->AddPlugin(PluginManager::PLUGIN_TYPE_HUD, new HUDPlugin);
    PluginManager::GetInstance()->AddPlugin(PluginManager::PLUGIN_TYPE_TEXT, new TextPlugin()); */

    PluginManager::GetInstance()->AddPlugin(PluginManager::PLUGIN_TYPE_SCENE, new SkyBox());
#endif

}

/**
 * Main entry point, handles events
 */
void android_main(struct android_app *state) {
    app_dummy();

    LOGI("in android_main\n");

    struct engine engine;
    memset(&engine, 0, sizeof(engine));
    state->userData = &engine;
    state->onAppCmd = app_cmd_handler;
    state->onInputEvent = app_input_handler;
    engine.app = state;
    initPlugins(&engine);

    extract_assets(state);

    // Read all pending events.
    while (1) {
        int events;
        struct android_poll_source *source;

        while (ALooper_pollAll(0, NULL, &events, (void **) &source) >= 0) {

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




