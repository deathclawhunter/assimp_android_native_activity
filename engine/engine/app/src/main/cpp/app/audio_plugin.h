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


class AudioPlugin : public IPlugin {

public:
    // Plugin API
    bool Init(int32_t width, int32_t height);
    bool Draw();
    int32_t KeyHandler(AInputEvent *event);
    IPlugin::PLUGIN_STATUS status();

    void ReleaseResampleBuf(void);

private:
    void PrintAudioFrameInfo(const AVCodecContext* codecContext, const AVFrame* frame);
    void CreateAudioEngine();
    short* CreateResampledBuf(uint32_t srcRate, unsigned *size);
    void CreateBufferQueueAudioPlayer(jint sampleRate, jint bufSize);
    bool Play();
    bool EnableReverb(bool enabled);

private:

    // engine interfaces
    SLObjectItf engineObject = NULL;
    SLEngineItf engineEngine;

// output mix interfaces
    SLObjectItf outputMixObject = NULL;
    SLEnvironmentalReverbItf outputMixEnvironmentalReverb = NULL;

// buffer queue player interfaces
    SLObjectItf bqPlayerObject = NULL;
    SLPlayItf bqPlayerPlay;
    SLEffectSendItf bqPlayerEffectSend;
    SLMuteSoloItf bqPlayerMuteSolo;
    SLVolumeItf bqPlayerVolume;
    SLmilliHertz bqPlayerSampleRate = 0;

    // aux effect on the output mix, used by the buffer queue player
    const SLEnvironmentalReverbSettings reverbSettings =
            SL_I3DL2_ENVIRONMENT_PRESET_STONECORRIDOR;

// URI player interfaces
    SLObjectItf uriPlayerObject = NULL;
    SLPlayItf uriPlayerPlay;
    SLSeekItf uriPlayerSeek;
    SLMuteSoloItf uriPlayerMuteSolo;
    SLVolumeItf uriPlayerVolume;

// file descriptor player interfaces
    SLObjectItf fdPlayerObject = NULL;
    SLPlayItf fdPlayerPlay;
    SLSeekItf fdPlayerSeek;
    SLMuteSoloItf fdPlayerMuteSolo;
    SLVolumeItf fdPlayerVolume;

// recorder interfaces
    SLObjectItf recorderObject = NULL;
    SLRecordItf recorderRecord;
    SLAndroidSimpleBufferQueueItf recorderBufferQueue;

    short *resampleBuf = NULL;

    int bqPlayerBufSize;

public:
    SLAndroidSimpleBufferQueueItf bqPlayerBufferQueue;
    // pointer and size of the next player buffer to enqueue, and number of remaining buffers
    short *nextBuffer;
    unsigned nextSize;
    int nextCount;
};


