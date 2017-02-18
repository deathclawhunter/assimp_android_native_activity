#include "PlayerPlugin.h"
#include "GamePlugin.h"
#include "PluginManager.h"

#include <utils/UtilTime.h>

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

float PlayerPlugin::GetDamage() {

    float Damage = 0.0f;
    uint64_t current = rx_hrtime();

    for (int i = 0; i < m_Equiped; i++) {
        if (current - m_LastFire < m_Weapons[i].CoolDown) {
            continue;
        }
        if (m_Weapons[i].AmmoCount == 0) {
            continue;
        }
        m_Weapons[i].AmmoCount--;
        Damage += m_Weapons[i].Damage;
    }


    return Damage;
}

bool PlayerPlugin::CanAttack() {

    srand(rx_hrtime());

    int dec = rand() % 100;

    if (dec <= 80 && GetDamage() > 0.001f) {

        m_LastFire = rx_hrtime();

        return true;
    }

    return false;
}

void PlayerPlugin::GetShot(float Damage) {
    srand(rx_hrtime());
    m_CurrentLife -= (Damage - (float) (rand() % (int) (m_DR * 100.0f)) / 100.0f);

    if (m_CurrentLife <= 0.0f) {
        Dead();
    }
}

void PlayerPlugin::Dead() {
    GamePlugin *Game = (GamePlugin *) PluginManager::GetInstance()->GetPlugin(PluginManager::PLUGIN_TYPE_GAME);
    Game->PlayerLose();
}






