#define LOG_TAG "ENGINE_MAIN"

#include <plugins/MuzzleEffect.h>
#include <plugins/CrossHair.h>
#include "engine.h"

#include "AppLog.h"
#include "GLError.h"
#include "VideoPlugin.h"
#include "ScenePlugin.h"
#include "PluginManager.h"
#include "Player.h"
#include "Config.h"

#define HELLOWORD 0

const Vector3f INIT_POSITION = Vector3f(0.0f, 0.0f, 6.0f);
const Vector3f INIT_ROTATION = Vector3f(270.0f, 180.0f, 0.0f);

extern "C" {
void extract_assets(struct android_app *app);
}

int InitDisplay(struct engine *engine);

void TerminateDisplay(struct engine *engine);

static void Init(struct engine *engine);

static void SensorHandler(struct engine *engine);

static const float ALPHA = 0.25f;
float g_Pitch = 0;
float g_Roll = 0;

/**
 * Just the current frame in the display.
 */
void DrawFrame(struct engine *engine) {

    // No display.
    if (engine->m_Display == NULL) {
        return;
    }

    // FIXME: hack to test if opengl is ready for use,
    // sometimes the glCreateProgram() will fail even
    // when reach here
    if (!engine->m_Initialized) {

        int prog = glCreateProgram();
        if (prog == 0) {
            checkGlError("test opengl: ");
        } else {

            LOGI("draw_frame: got prog = 0x%x in main\n", prog);
            engine->m_Initialized = true;

            Init(engine);
        }
    } else {

        if (PluginManager::GetInstance()->Draw()) {
            eglSwapBuffers(engine->m_Display, engine->m_Surface);
        }
    }
}

static void MovementHandler(struct engine *engine) {

    if (!engine->m_Initialized) {
        return;
    }

    if (engine->m_MovementCount > 10) {
        engine->m_MovementCount = 0;
    } else {
        engine->m_MovementCount++;
        return;
    }
    IPlugin::InputData data = {0};
    // Map to screen resolution
    data.m_ButtonType = IPlugin::ACTION_TYPE_TIMER;
    PluginManager::GetInstance()->KeyHandler(&data);
}

static void SensorHandler(struct engine *engine) {

    if (!engine->m_Initialized || engine->m_AccelerometerSensor == NULL) {
        return;
    }

    if (!Config::GetInstance(NULL)->GetbCfg(Config::CFG_USE_SENSOR, Config::DEFAULT_CFG_USE_SENSOR)) {
        if (engine->m_SensorEnabled) {
            ASensorEventQueue_disableSensor(engine->m_SensorEventQueue,
                                            engine->m_AccelerometerSensor);
            engine->m_SensorEnabled = false;
        }
        return;
    }

    if (!engine->m_SensorEnabled) {
        ASensorEventQueue_enableSensor(engine->m_SensorEventQueue,
                                       engine->m_AccelerometerSensor);
        // We'd like to get 60 events per second (in us).
        ASensorEventQueue_setEventRate(engine->m_SensorEventQueue,
                                       engine->m_AccelerometerSensor,
                                       (1000L / 60) * 1000);
        engine->m_SensorEnabled = true;
    }

    ASensorEvent event;
    while (ASensorEventQueue_getEvents(engine->m_SensorEventQueue,
                                       &event, 1) > 0) {

        /* LOGI("event.data[0] = %f, event.data[1] = %f, event.data[2] = %f",
             event.data[0], event.data[1], event.data[2]);
        LOGI("roll = %f, pitch = %f",
             event.acceleration.roll, // -10 ~ +10
             event.acceleration.pitch); // -10 ~ +10 */

        g_Pitch = g_Pitch + ALPHA * (event.acceleration.pitch - g_Pitch);
        g_Roll = g_Roll + ALPHA * (event.acceleration.roll - g_Roll);

        float x = g_Pitch + 10.0f;
        x = x > 0 ? x : 0;
        x = x * engine->m_Width / 10.0f;

        float y = g_Roll + 10.0f;
        y = y > 0 ? y : 0;
        y = y * engine->m_Height / 10.0f;

        // LOGI("x = %f, y = %f", x, y);

        if (fabs(engine->m_X - x) < 10.0f) {
            return;
        }

        y = engine->m_Height / 2.0f;

        IPlugin::InputData data = {0};
        data.m_ButtonCount = 1;
        // Map to screen resolution
        data.m_X0 = x;
        data.m_Y0 = y;

        // LOGI("m_X = %f, m_Y = %f", engine->m_X, engine->m_Y);

        // left half of the screen
        if ((engine->m_X < engine->m_Width && x > engine->m_X) ||
            (engine->m_X > engine->m_Width && x < engine->m_X)) {
            // LOGI("reset");
            data.m_ButtonType = IPlugin::ACTION_TYPE_RESET; // reset mouse
        } else {
            data.m_ButtonType = IPlugin::ACTION_TYPE_ROTATE;
        }
        engine->m_X = x;
        engine->m_Y = y;
        PluginManager::GetInstance()->KeyHandler(&data);
    }
}

static void Init(struct engine *engine) {
    // Initialize engine:: sensors
    engine->m_X = engine->m_Width / 2.0f;
    engine->m_Y = engine->m_Height / 2.0f;

    // Initialize game camera and player
    AppCamera::GetInstance(engine->m_Width, engine->m_Height);
    Player::GetInstance()->SetPosition(INIT_POSITION);
    Player::GetInstance()->SetRotation(INIT_ROTATION);

    // Initialize pulgins
    PluginManager::GetInstance()->Init(engine->m_Width, engine->m_Height);

    static float grey;
    grey += 0.01f;
    if (grey > 1.0f) {
        grey = 0.0f;
    }
    glClearColor(grey, grey, grey, 1.0f);
    checkGlError("glClearColor");
}

void CopyInputEvent(SysInputEvent *Copy, AInputEvent *Target) {
    Copy->m_EventType = AInputEvent_getType(Target);
    Copy->m_EventAction = AMotionEvent_getAction(Target);
    Copy->m_EventPointerCount = AMotionEvent_getPointerCount(Target);
    Copy->m_X0 = AMotionEvent_getX(Target, 0);
    Copy->m_Y0 = AMotionEvent_getY(Target, 0);

    if (Copy->m_EventPointerCount >= 2) {
        Copy->m_X1 = AMotionEvent_getX(Target, 1);
        Copy->m_Y1 = AMotionEvent_getY(Target, 1);
    }
}

bool IsNoise(struct android_app *app, AInputEvent *event) {
    struct engine *engine = (struct engine *) app->userData;

    if (AInputEvent_getType(event) != AINPUT_EVENT_TYPE_MOTION) {
        return true;
    }

    if (!engine->m_Initialized) {
        return true;
    }

    if (engine->m_InputEvent.m_EventType == -1) {
        CopyInputEvent(&engine->m_InputEvent, event);
        return false;
    }

    if (engine->m_InputEvent.m_EventAction != AMotionEvent_getAction(event)) {
        CopyInputEvent(&engine->m_InputEvent, event);
        return false;
    }

    if (engine->m_InputEvent.m_EventPointerCount != AMotionEvent_getPointerCount(event)) {
        CopyInputEvent(&engine->m_InputEvent, event);
        return false;
    }

    if (fabs(AMotionEvent_getX(event, 0) - engine->m_InputEvent.m_X0) > 5.0f ||
        fabs(AMotionEvent_getY(event, 0) - engine->m_InputEvent.m_Y0) > 5.0f) {
        CopyInputEvent(&engine->m_InputEvent, event);
        return false;
    }

    if (AMotionEvent_getPointerCount(event) >= 2) {
        if (fabs(AMotionEvent_getX(event, 1) - engine->m_InputEvent.m_X1) > 5.0f ||
            fabs(AMotionEvent_getY(event, 1) - engine->m_InputEvent.m_Y1) > 5.0f) {
            CopyInputEvent(&engine->m_InputEvent, event);
            return false;
        }
    }

    return true;
}

/**
 * Process the next input event.
 */
int32_t AppInputHandler(struct android_app *app, AInputEvent *event) {

    if (IsNoise(app, event)) {
        return 0;
    }

    struct engine *engine = (struct engine *) app->userData;

    // Map system input event to internal data structure
    int32_t action = AMotionEvent_getAction(event);
    size_t count = AMotionEvent_getPointerCount(event);
    IPlugin::InputData data = {0};
    data.m_ButtonCount = count;

    if (action == AMOTION_EVENT_ACTION_MOVE) {
        data.m_ButtonType = IPlugin::ACTION_TYPE_MOVE;
    } else if (action == AMOTION_EVENT_ACTION_UP) {
        data.m_ButtonType = IPlugin::ACTION_TYPE_UP;
    } else {
        return 1;
    }

    if (engine->m_PreviousCount == -1) {
        engine->m_PreviousCount = count;
    } else {
        if (engine->m_PreviousCount == 1 && count == 2) {
            data.m_ButtonType = IPlugin::ACTION_TYPE_UP;
        }

        engine->m_PreviousCount = count;
    }

    data.m_X0 = AMotionEvent_getX(event, 0);
    data.m_Y0 = AMotionEvent_getY(event, 0);

    if (count == 2) {
        data.m_X1 = AMotionEvent_getX(event, 1);
        data.m_Y1 = AMotionEvent_getY(event, 1);
    }
    return PluginManager::GetInstance()->KeyHandler(&data);
}

/**
 * Process the next main command.
 */
void AppCmdHandler(struct android_app *app, int32_t cmd) {
    struct engine *engine = (struct engine *) app->userData;
    switch (cmd) {
    case APP_CMD_SAVE_STATE:
        break;
    case APP_CMD_INIT_WINDOW:
        // The window is being shown, get it ready.
        if (engine->m_App->window != NULL) {
            InitDisplay(engine);
            DrawFrame(engine);
        }
        break;
    case APP_CMD_TERM_WINDOW:
        // The window is being hidden or closed, clean it up.
        TerminateDisplay(engine);
        break;
    case APP_CMD_GAINED_FOCUS:
        // When our app gains focus, we start monitoring the accelerometer.
        if (Config::GetInstance(NULL)->GetbCfg(Config::CFG_USE_SENSOR, Config::DEFAULT_CFG_USE_SENSOR) &&
            engine->m_AccelerometerSensor != NULL && !engine->m_SensorEnabled) {
            ASensorEventQueue_enableSensor(engine->m_SensorEventQueue,
                                           engine->m_AccelerometerSensor);
            // We'd like to get 60 events per second (in us).
            ASensorEventQueue_setEventRate(engine->m_SensorEventQueue,
                                           engine->m_AccelerometerSensor,
                                           (1000L / 60) * 1000);
            engine->m_SensorEnabled = true;
        }
        break;
    case APP_CMD_LOST_FOCUS:
        // When our app loses focus, we stop monitoring the accelerometer.
        // This is to avoid consuming battery while not being used.
        if (Config::GetInstance(NULL)->GetbCfg(Config::CFG_USE_SENSOR, Config::DEFAULT_CFG_USE_SENSOR) &&
            engine->m_AccelerometerSensor != NULL && engine->m_SensorEnabled) {
            ASensorEventQueue_disableSensor(engine->m_SensorEventQueue,
                                            engine->m_AccelerometerSensor);
            engine->m_SensorEnabled = false;
        }
        DrawFrame(engine);
        break;
    }
}

#include "DemoPlugin.h"
#include "AudioPlugin.h"
#include "HUDPlugin.h"
#include "TextPlugin.h"
#include "SkyBoxPlugin.h"
#include "Config.h"

void InitPlugins(struct engine *engine) {

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
    PluginManager::GetInstance()->AddPlugin(PluginManager::PLUGIN_TYPE_SKY, new SkyBox());
    PluginManager::GetInstance()->AddPlugin(PluginManager::PLUGIN_TYPE_SCENE, new ScenePlugin());
    PluginManager::GetInstance()->AddPlugin(PluginManager::PLUGIN_TYPE_HUD, new HUDPlugin);
    PluginManager::GetInstance()->AddPlugin(PluginManager::PLUGIN_TYPE_TEXT, new TextPlugin());
    PluginManager::GetInstance()->AddPlugin(PluginManager::PLUGIN_TYPE_MUZZLE_FLASH, new MuzzleEffectPlugin());
    PluginManager::GetInstance()->AddPlugin(PluginManager::PLUGIN_TYPE_CROSSHAIR, new CrossHairPlugin()); */

    PluginManager::GetInstance()->AddPlugin(PluginManager::PLUGIN_TYPE_SCENE, new ScenePlugin());

#endif

}

/**
 * Main entry point, handles events
 */
void android_main(struct android_app *state) {
    app_dummy();

    LOGI("in android_main\n");

    extract_assets(state);

    Config::GetInstance(state);

    struct engine engine;
    memset(&engine, 0, sizeof(struct engine));
    state->userData = &engine;
    state->onAppCmd = AppCmdHandler;
    state->onInputEvent = AppInputHandler;
    engine.m_InputEvent.m_EventType = -1; // initialize event type to -1
    engine.m_App = state;
    InitPlugins(&engine);

    // Prepare to monitor accelerometer
    engine.m_SensorManager = ASensorManager_getInstance();
    engine.m_AccelerometerSensor = ASensorManager_getDefaultSensor(
            engine.m_SensorManager,
            ASENSOR_TYPE_ACCELEROMETER);
    engine.m_SensorEventQueue = ASensorManager_createEventQueue(
            engine.m_SensorManager,
            state->looper, LOOPER_ID_USER,
            NULL, NULL);

    // Read all pending events.
    while (1) {
        int ident;
        int events;
        struct android_poll_source *source;

        while (ident = ALooper_pollAll(0, NULL, &events, (void **) &source) >= 0) {

            // Process this event.
            if (source != NULL) {
                source->process(state, source);
            }

            SensorHandler(&engine);

            // Check if we are exiting.
            if (state->destroyRequested != 0) {
                TerminateDisplay(&engine);
                Config::GetInstance(NULL)->FlushCfgs();
                exit(1); // exit directly
            }
        }

        MovementHandler(&engine);
        // Draw the current frame
        DrawFrame(&engine);
    }
}




