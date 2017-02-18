#ifndef _PLAYER_PLUGIN_H_
#define _PLAYER_PLUGIN_H_

#include <GLES/gl.h>
#include <android_native_app_glue.h>

#include "GL3Stub.h"
#include "plugin.h"

#define LOG_TAG "Player"
#include "AppLog.h"
#include "Technique.h"
#include "ogldev_util.h"
#include "SpritePlugin.h"

class PlayerPlugin : public IPlugin {

public:
    // Plugin API
    bool Init(int32_t width, int32_t height);

    bool Draw();

    int32_t KeyHandler(InputData *event);

    IPlugin::PLUGIN_STATUS Status();

    PlayerPlugin();

    ~PlayerPlugin();

    float GetDamage();

    bool CanAttack();

    void GetShot(float d);

private:
    PLUGIN_STATUS m_Status = PLUGIN_STATUS_INIT_LATER;
    Weapon m_Weapons[MAX_WEAPON_SLOTS];
    int m_Equiped = 0;
    uint64_t m_LastFire = 0;

    float m_Life = 10.0f;
    float m_DR = 1.0f;
    float m_CurrentLife = m_Life;

private:
    void Dead();
};

#endif /* _PLAYER_PLUGIN_H_ */


