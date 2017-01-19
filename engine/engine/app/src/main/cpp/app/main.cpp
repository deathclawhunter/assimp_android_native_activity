#define LOG_TAG "ENGINE_MAIN"

#include "engine.h"

using namespace std;

#include "AppLog.h"
#include "GLError.h"
#include "video.h"
#include "app.h"

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

            // decode_h264("0001-0010.avi");
            // decode_h264("0000-0100.avi");

            LOGI("draw_frame: got prog = 0x%x in main\n", prog);
            engine->initialized = true;

            IPlugin *plugin = engine->m_Plugins;
            IPlugin *prev = plugin; // use for unlink
            while (plugin != NULL) {
                if (plugin->status() == IPlugin::PLUGIN_STATUS_INIT_RIGHT_NOW &&
                    !plugin->Init(engine->width, engine->height)) {
                    if (prev != plugin) {
                        prev->next = plugin->next; // unlink from the list if fail to initialize
                        delete plugin;
                        plugin = prev->next;
                    } else {
                        engine->m_Plugins = plugin->next;
                        delete plugin;
                        plugin = engine->m_Plugins;
                        prev = engine->m_Plugins;
                    }
                } else {
                    prev = plugin;
                    plugin = plugin->next;
                }
            }
        }
    } else {
        bool update = false;
        IPlugin *plugin = engine->m_Plugins;
        IPlugin *prev = plugin; // use for unlink
        while (plugin != NULL) {

            if (plugin->status() == IPlugin::PLUGIN_STATUS_INIT_LATER) {
                if (!plugin->Init(engine->width, engine->height)) {
                    if (prev != plugin) {
                        prev->next = plugin->next; // unlink from the list if fail to initialize
                        delete plugin;
                        plugin = prev->next;
                    } else {
                        engine->m_Plugins = plugin->next;
                        delete plugin;
                        plugin = engine->m_Plugins;
                        prev = engine->m_Plugins;
                    }

                    continue; // continue to next plugin if fail to initialize
                }
            }

            if (update) {
                plugin->Draw();
            } else {
                update = plugin->Draw();
            }

            if (plugin->status() == IPlugin::PLUGIN_STATUS_FINISHED) {
                if (prev != plugin) {
                    prev->next = plugin->next; // unlink from the list if fail to initialize
                    delete plugin;
                    plugin = prev->next;
                } else {
                    engine->m_Plugins = plugin->next;
                    delete plugin;
                    plugin = engine->m_Plugins;
                    prev = plugin;
                }
            } else if (plugin->status() == IPlugin::PLUGIN_STATUS_LOOP_ME) {
                break;
            } else if (plugin->status() == IPlugin::PLUGIN_STATUS_NEXT) {
                prev = plugin;
                plugin = plugin->next;
            } else {
                // undefined
            }
        }
        if (update) {
            eglSwapBuffers(engine->display, engine->surface);
        }
    }
}

/**
 * Process the next input event.
 */
int32_t app_input_handler(struct android_app *app, AInputEvent *event) {
    if (AInputEvent_getType(event) == AINPUT_EVENT_TYPE_MOTION) {
        struct engine *engine = (struct engine *) app->userData;
        IPlugin *plugin = engine->m_Plugins;
        while (plugin != NULL) {
            plugin->KeyHandler(event);

            if (plugin->status() == IPlugin::PLUGIN_STATUS_LOOP_ME) {
                break;
            }
            plugin = plugin->next;
        }

        return 1;
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

void initPlugins(struct engine *engine) {

#if HELLOWORD
    HelloWorldPlugin *helloWorld = new HelloWorldPlugin();
    engine->m_Plugins = helloWorld;
#else
    /* VideoPlugin *ve = new VideoPlugin();
    engine->m_Plugins = ve;
    ScenePlugin *scene = new ScenePlugin();
    ve->next = scene; */

    AudioPlugin *ap = new AudioPlugin();
    engine->m_Plugins = ap;
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


