#ifndef _START_MUSIC_SOUND_PLUGIN_H_
#define _START_MUSIC_SOUND_PLUGIN_H_

#include "AudioPlugin.h"


/**
 * A simple audio player for gaming
 */
class StartMusicSoundPlugin : public AudioPlugin {

public:
    // Plugin API
    bool Init(int32_t width, int32_t height) {
        return AudioPlugin::Init(width, height, "SampleAudio.mp3");
    }
};

#endif /* _START_MUSIC_SOUND_PLUGIN_H_ */
