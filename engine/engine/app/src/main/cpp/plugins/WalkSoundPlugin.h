#ifndef _WALK_SOUND_PLUGIN_H_
#define _WALK_SOUND_PLUGIN_H_

#include "AudioPlugin.h"


/**
 * A simple audio player for gaming
 */
class WalkSoundPlugin : public AudioPlugin {

public:
    // Plugin API
    bool Init(int32_t width, int32_t height) {
        AudioPlugin::Init(width, height, "SampleAudio.mp3", false);
    }
};

#endif /* _WALK_SOUND_PLUGIN_H_ */