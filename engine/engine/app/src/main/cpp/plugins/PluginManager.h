#include <map>
#include "Plugin.h"
#include "TextPlugin.h"

class PluginManager : public IPlugin {

public:
    enum PLUGIN_TYPES {
        PLUGIN_TYPE_START_MUSIC,
        PLUGIN_TYPE_START_VIDEO,
        PLUGIN_TYPE_SCENE,
        PLUGIN_TYPE_HUD,
        PLUGIN_TYPE_TEXT,
        PLUGIN_TYPE_SKY,
        PLUGIN_TYPE_MUZZLE_FLASH,
        PLUGIN_TYPE_CROSSHAIR,
        PLUGIN_TYPE_BACK_MUSIC,
        PLUGIN_TYPE_FIRE_SOUND,
        PLUGIN_TYPE_WALK_SOUND
        PLUGIN_TYPE_TERRAIN
    };

    static PluginManager *GetInstance();

    IPlugin *GetPlugin(PLUGIN_TYPES type);

    bool AddPlugin(PLUGIN_TYPES type, IPlugin *plugin);

    // Plugin API
    bool Init(int32_t width, int32_t height);

    bool Draw();

    int32_t KeyHandler(InputData *event);

    IPlugin::PLUGIN_STATUS Status();

    ~PluginManager();

private:
    PluginManager();

    std::map<PLUGIN_TYPES, int> m_PluginMap;
    IPlugin *m_Plugins = NULL;
    int width, height;
};


