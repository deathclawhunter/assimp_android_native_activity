#include <GLES/gl.h>
#include <android_native_app_glue.h>

#include "GL3Stub.h"
#include "plugin.h"

#define LOG_TAG "Player"
#include "AppLog.h"
#include "Technique.h"
#include "ogldev_util.h"

class PlayerPlugin : public IPlugin {

public:
    // Plugin API
    bool Init(int32_t width, int32_t height);

    bool Draw();

    int32_t KeyHandler(InputData *event);

    IPlugin::PLUGIN_STATUS Status();

    PlayerPlugin();

    ~PlayerPlugin();

private:
    PLUGIN_STATUS m_Status = PLUGIN_STATUS_INIT_LATER;

};


