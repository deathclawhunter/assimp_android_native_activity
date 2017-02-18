#include <GLES/gl.h>
#include <android_native_app_glue.h>

#include "GL3Stub.h"
#include "plugin.h"

#define LOG_TAG "GamePlugin"
#include "AppLog.h"
#include "Technique.h"
#include "ogldev_util.h"

class GamePlugin : public IPlugin {

public:
    // Plugin API
    bool Init(int32_t width, int32_t height);

    bool Draw();

    int32_t KeyHandler(InputData *event);

    IPlugin::PLUGIN_STATUS Status();

    GamePlugin();

    ~GamePlugin();

    void PlayerWin();

    enum GAME_STATUS {
        GAME_STATUS_PLAYING,
        GAME_STATUS_PAUSE
    };

    void PlayerLose();

private:
    PLUGIN_STATUS m_Status = PLUGIN_STATUS_INIT_LATER;

    int32_t m_Width;
    int32_t m_Height;
    int m_GameStatus = GAME_STATUS_PAUSE;
};


