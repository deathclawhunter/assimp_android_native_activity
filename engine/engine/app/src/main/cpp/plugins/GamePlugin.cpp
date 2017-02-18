#include "GamePlugin.h"
#include "PluginManager.h"
#include "TextPlugin.h"

GamePlugin::GamePlugin() {
}

GamePlugin::~GamePlugin() {
}

bool GamePlugin::Init(int32_t width, int32_t height) {

    LOGI("in Player init\n");

    m_Status = IPlugin::PLUGIN_STATUS_NEXT;

    m_Width = width;
    m_Height = height;

    return true;
}

bool GamePlugin::Draw() {

    return false;
}

int32_t GamePlugin::KeyHandler(InputData *event) {
    return 1;
}

IPlugin::PLUGIN_STATUS GamePlugin::Status() {
    return m_Status; // example of never finish a plugin
}

void GamePlugin::PlayerWin() {
    m_GameStatus = GAME_STATUS_PAUSE;
    TextPlugin *Text = (TextPlugin *) PluginManager::GetInstance()->GetPlugin(PluginManager::PLUGIN_TYPE_TEXT);
    GLfloat Color[4] = {
            0, 255, 0, 255
    };
    Text->SetFontColor(Color);
    Text->SetFontSize(20);
    Text->DisplayText("You Win", m_Width / 2.0f, m_Height / 2.0f, 2.0f, 2.0f);

    // TODO: add some summary information like hit rate or score
}

void GamePlugin::PlayerLose() {
    m_GameStatus = GAME_STATUS_PAUSE;
    TextPlugin *Text = (TextPlugin *) PluginManager::GetInstance()->GetPlugin(PluginManager::PLUGIN_TYPE_TEXT);
    GLfloat Color[4] = {
            0, 255, 0, 255
    };
    Text->SetFontColor(Color);
    Text->SetFontSize(20);
    Text->DisplayText("You Lose", m_Width / 2.0f, m_Height / 2.0f, 2.0f, 2.0f);

    // TODO: add some summary information like hit rate or score
}




