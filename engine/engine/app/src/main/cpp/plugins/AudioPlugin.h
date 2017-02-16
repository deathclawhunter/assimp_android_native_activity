#ifndef _AUDIO_PLUGIN_H_
#define _AUDIO_PLUGIN_H_

#include <iostream>

extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
};

#include "plugin.h"

#include <assert.h>
#include <jni.h>
#include <string.h>
#include <pthread.h>

// for native audio
#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>

// for native asset manager
#include <sys/types.h>
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>

using namespace std;

/**
 * A simple audio player for gaming
 */
class AudioPlugin : public IPlugin {

public:
    // Plugin API
    bool Init(int32_t width, int32_t height, const char *fileName, bool status = true);

    bool Draw();

    int32_t KeyHandler(InputData *event);

    IPlugin::PLUGIN_STATUS Status();

    AudioPlugin();

    ~AudioPlugin();

    void Play();
    void Pause();

private:
    void CreateAudioEngine();

    bool CreateAssetAudioPlayer(const char *filename);

    void PlayingAssetAudioPlayer(bool isPlaying);

private:

    PLUGIN_STATUS pluginStatus;

    AVFrame *frame;
    AVFormatContext *formatContext;
    AVCodecContext *codecContext;

    // engine interfaces
    SLObjectItf engineObject = NULL;
    SLEngineItf engineEngine;

// output mix interfaces
    SLObjectItf outputMixObject = NULL;
    SLEnvironmentalReverbItf outputMixEnvironmentalReverb = NULL;

    // aux effect on the output mix, used by the buffer queue player
    const SLEnvironmentalReverbSettings reverbSettings =
            SL_I3DL2_ENVIRONMENT_PRESET_STONECORRIDOR;

// file descriptor player interfaces
    SLObjectItf fdPlayerObject = NULL;
    SLPlayItf fdPlayerPlay;
    SLSeekItf fdPlayerSeek;
    SLMuteSoloItf fdPlayerMuteSolo;
    SLVolumeItf fdPlayerVolume;

    bool m_PlayFlag = true;
};

#endif /* _AUDIO_PLUGIN_H_ */


