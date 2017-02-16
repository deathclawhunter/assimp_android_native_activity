#include "PlayerPlugin.h"

PlayerPlugin::PlayerPlugin() {
}

PlayerPlugin::~PlayerPlugin() {
}

bool PlayerPlugin::Init(int32_t width, int32_t height) {

    LOGI("in Player init\n");

    m_Status = IPlugin::PLUGIN_STATUS_NEXT;

    return true;
}

bool PlayerPlugin::Draw() {

    return false;
}

int32_t PlayerPlugin::KeyHandler(InputData *event) {
    return 1;
}

IPlugin::PLUGIN_STATUS PlayerPlugin::Status() {
    return m_Status; // example of never finish a plugin
}
