#ifndef _BACK_MUSIC_H_
#define _BACK_MUSIC_H_

#include "AudioPlugin.h"


/**
 * A simple audio player for gaming
 */
class BackMusicPlugin : public AudioPlugin {

public:
    // Plugin API
    bool Init(int32_t width, int32_t height) {
        AudioPlugin::Init(width, height, "SampleAudio.mp3", false);
    }
};

#endif /* _BACK_MUSIC_H_ */



