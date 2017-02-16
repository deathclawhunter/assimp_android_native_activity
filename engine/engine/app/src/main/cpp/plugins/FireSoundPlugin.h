#ifndef _FIRE_SOUND_PLUGIN_H_
#define _FIRE_SOUND_PLUGIN_H_


#include "AudioPlugin.h"


/**
 * A simple audio player for gaming
 */
class FireSoundPlugin : public AudioPlugin {

public:
    // Plugin API
    bool Init(int32_t width, int32_t height) {
        AudioPlugin::Init(width, height, "SampleAudio.mp3", false);
    }
};

#endif /* _FIRE_SOUND_PLUGIN_H_ */


