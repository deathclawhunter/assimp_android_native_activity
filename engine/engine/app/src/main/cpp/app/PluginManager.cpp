#define LOG_TAG "PluginManager"

#include <map>
#include "AppLog.h"

#include "PluginManager.h"


PluginManager::PluginManager() {

}

PluginManager* PluginManager::GetInstance() {

    static PluginManager *m_Instance = NULL;

    if (m_Instance == NULL) {
        m_Instance = new PluginManager();
    }

    return m_Instance;
}


IPlugin* PluginManager::GetPlugin(PLUGIN_TYPES type) {
    return (IPlugin *) m_PluginMap[type];
}

bool PluginManager::AddPlugin(PLUGIN_TYPES type, IPlugin* plugin) {

    if (m_Plugins == NULL) {
        m_Plugins = plugin;
        m_PluginMap[type] = (int) plugin;
        return true;
    }

    // do not handle duplicated plugin error, should be avoided in coding stage

    IPlugin* next = m_Plugins;
    IPlugin* last = m_Plugins;
    while (next) {
        last = next;
        next = next->next;
    }

    last->next = plugin;
    m_PluginMap[type] = (int) plugin;

    return true;
}

bool PluginManager::Init(int32_t width, int32_t height) {

    this->width = width;
    this->height = height;

    IPlugin *plugin = m_Plugins;
    while (plugin != NULL) {
        if (plugin->status() == IPlugin::PLUGIN_STATUS_INIT_RIGHT_NOW &&
            !plugin->Init(width, height)) {
            // Instead of deleting plugin, use plugin status finished or fail
            LOGE("fail to init plugin");
        } else {
            plugin = plugin->next;
        }
    }

    return true;
}

bool PluginManager::Draw() {
    IPlugin *plugin = m_Plugins;
    bool update = false;
    while (plugin != NULL) {

        if (plugin->status() == IPlugin::PLUGIN_STATUS_INIT_LATER) {
            if (!plugin->Init(width, height)) {
                LOGE("fail to init plugin");
                plugin = plugin->next;
                continue; // continue to next plugin if fail to initialize
            }
        }

        if (plugin->status() == IPlugin::PLUGIN_STATUS_FINISHED ||
            plugin->status() == IPlugin::PLUGIN_STATUS_INIT_FAIL) {
            plugin = plugin->next;
            continue;
        }

        // any one of draw will cause swap buffer to update
        if (update) {
            plugin->Draw();
        } else {
            update = plugin->Draw();
        }

        if (plugin->status() == IPlugin::PLUGIN_STATUS_LOOP_ME) {
            break;
        } else if (plugin->status() == IPlugin::PLUGIN_STATUS_NEXT) {
            plugin = plugin->next;
        } else {
            // undefined
            plugin = plugin->next;
        }
    }

    return update;
}

int32_t PluginManager::KeyHandler(AInputEvent *event) {
    IPlugin *plugin = m_Plugins;
    while (plugin != NULL) {

        if (plugin->status() == IPlugin::PLUGIN_STATUS_FINISHED ||
            plugin->status() == IPlugin::PLUGIN_STATUS_INIT_FAIL) {
            plugin = plugin->next;
            continue;
        }

        plugin->KeyHandler(event);

        if (plugin->status() == IPlugin::PLUGIN_STATUS_LOOP_ME) {
            break;
        }
        plugin = plugin->next;
    }
}

IPlugin::PLUGIN_STATUS PluginManager::status() {
    return IPlugin::PLUGIN_STATUS_FINISHED; // This should never be called.
}


