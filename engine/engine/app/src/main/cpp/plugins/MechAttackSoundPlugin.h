#ifndef _MECH_ATTACK_SOUND_PLUGIN_H_
#define _MECH_ATTACK_SOUND_PLUGIN_H_


#include "AudioPlugin.h"


/**
 * A simple audio player for gaming
 */
class MechAttackSoundPlugin : public AudioPlugin {

public:
    // Plugin API
    bool Init(int32_t width, int32_t height) {
        return AudioPlugin::Init(width, height, "SampleAudio.mp3", false);
    }
};

#endif /* _MECH_ATTACK_SOUND_PLUGIN_H_ */

