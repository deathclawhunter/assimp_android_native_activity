#include <fcntl.h>
#include <sys/stat.h>
#include "AudioPlugin.h"

#define LOG_TAG "Audio"

#include "AppLog.h"

AudioPlugin::AudioPlugin() {
    pluginStatus = PLUGIN_STATUS_INIT_RIGHT_NOW;
}

AudioPlugin::~AudioPlugin() {
    // Clean up!
    avformat_close_input(&formatContext);
    av_free(frame);
    avcodec_close(codecContext);
}

void AudioPlugin::CreateAudioEngine() {
    SLresult result;

    // create engine
    result = slCreateEngine(&engineObject, 0, NULL, 0, NULL, NULL);
    assert(SL_RESULT_SUCCESS == result);
    (void) result;

    // realize the engine
    result = (*engineObject)->Realize(engineObject, SL_BOOLEAN_FALSE);
    assert(SL_RESULT_SUCCESS == result);
    (void) result;

    // get the engine interface, which is needed in order to create other objects
    result = (*engineObject)->GetInterface(engineObject, SL_IID_ENGINE, &engineEngine);
    assert(SL_RESULT_SUCCESS == result);
    (void) result;

    // create output mix, with environmental reverb specified as a non-required interface
    const SLInterfaceID ids[1] = {SL_IID_ENVIRONMENTALREVERB};
    const SLboolean req[1] = {SL_BOOLEAN_FALSE};
    result = (*engineEngine)->CreateOutputMix(engineEngine, &outputMixObject, 1, ids, req);
    assert(SL_RESULT_SUCCESS == result);
    (void) result;

    // realize the output mix
    result = (*outputMixObject)->Realize(outputMixObject, SL_BOOLEAN_FALSE);
    assert(SL_RESULT_SUCCESS == result);
    (void) result;

    // get the environmental reverb interface
    // this could fail if the environmental reverb effect is not available,
    // either because the feature is not present, excessive CPU load, or
    // the required MODIFY_AUDIO_SETTINGS permission was not requested and granted
    result = (*outputMixObject)->GetInterface(outputMixObject, SL_IID_ENVIRONMENTALREVERB,
                                              &outputMixEnvironmentalReverb);
    if (SL_RESULT_SUCCESS == result) {
        result = (*outputMixEnvironmentalReverb)->SetEnvironmentalReverbProperties(
                outputMixEnvironmentalReverb, &reverbSettings);
        (void) result;
    }
    // ignore unsuccessful result codes for environmental reverb, as it is optional for this example

}

// create asset audio player
bool AudioPlugin::CreateAssetAudioPlayer(const char *filename) {
    SLresult result;

    int fd = open(filename, O_RDONLY);
    if (fd < 0) {
        LOGE("fail to open file %s", filename);
        return false;
    }

    struct stat buf;
    int ret = fstat(fd, &buf);
    if (ret != 0) {
        LOGE("fail to get file status");
        return false;
    }

    // configure audio source
    SLDataLocator_AndroidFD loc_fd = {SL_DATALOCATOR_ANDROIDFD, fd, 0, buf.st_size};
    SLDataFormat_MIME format_mime = {SL_DATAFORMAT_MIME, NULL, SL_CONTAINERTYPE_UNSPECIFIED};
    SLDataSource audioSrc = {&loc_fd, &format_mime};

    // configure audio sink
    SLDataLocator_OutputMix loc_outmix = {SL_DATALOCATOR_OUTPUTMIX, outputMixObject};
    SLDataSink audioSnk = {&loc_outmix, NULL};

    // create audio player
    const SLInterfaceID ids[3] = {SL_IID_SEEK, SL_IID_MUTESOLO, SL_IID_VOLUME};
    const SLboolean req[3] = {SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE};
    result = (*engineEngine)->CreateAudioPlayer(engineEngine, &fdPlayerObject, &audioSrc, &audioSnk,
                                                3, ids, req);
    assert(SL_RESULT_SUCCESS == result);
    (void) result;

    // realize the player
    result = (*fdPlayerObject)->Realize(fdPlayerObject, SL_BOOLEAN_FALSE);
    assert(SL_RESULT_SUCCESS == result);
    (void) result;

    // get the play interface
    result = (*fdPlayerObject)->GetInterface(fdPlayerObject, SL_IID_PLAY, &fdPlayerPlay);
    assert(SL_RESULT_SUCCESS == result);
    (void) result;

    // get the seek interface
    result = (*fdPlayerObject)->GetInterface(fdPlayerObject, SL_IID_SEEK, &fdPlayerSeek);
    assert(SL_RESULT_SUCCESS == result);
    (void) result;

    // get the mute/solo interface
    result = (*fdPlayerObject)->GetInterface(fdPlayerObject, SL_IID_MUTESOLO, &fdPlayerMuteSolo);
    assert(SL_RESULT_SUCCESS == result);
    (void) result;

    // get the volume interface
    result = (*fdPlayerObject)->GetInterface(fdPlayerObject, SL_IID_VOLUME, &fdPlayerVolume);
    assert(SL_RESULT_SUCCESS == result);
    (void) result;

    // enable whole file looping
    result = (*fdPlayerSeek)->SetLoop(fdPlayerSeek, SL_BOOLEAN_TRUE, 0, SL_TIME_UNKNOWN);
    assert(SL_RESULT_SUCCESS == result);
    (void) result;

    return JNI_TRUE;
}

// set the playing state for the asset audio player
void AudioPlugin::PlayingAssetAudioPlayer(bool isPlaying) {
    SLresult result;

    // make sure the asset audio player was created
    if (NULL != fdPlayerPlay) {

        // set the player's state
        result = (*fdPlayerPlay)->SetPlayState(fdPlayerPlay, isPlaying ?
                                                             SL_PLAYSTATE_PLAYING
                                                                       : SL_PLAYSTATE_PAUSED);
        assert(SL_RESULT_SUCCESS == result);
        (void) result;
    }

}

bool AudioPlugin::Init(int32_t width, int32_t height) {

    /* if (!OpenDecoder()) {
        LOGE("fail to open audio decoder");
        pluginStatus = PLUGIN_STATUS_INIT_FAIL;
        return false;
    } */

    CreateAudioEngine();
    // CreateBufferQueueAudioPlayer(0, 0);
    // PlayHello();

    CreateAssetAudioPlayer("SampleAudio.mp3");
    PlayingAssetAudioPlayer(true);

    pluginStatus = PLUGIN_STATUS_NEXT; // ensure video or game can play at the same time

    return true;
}

IPlugin::PLUGIN_STATUS AudioPlugin::status() {
    return pluginStatus;
}

bool AudioPlugin::Draw() {

    return false; // Audio draws nothing
}

int32_t AudioPlugin::KeyHandler(InputData *event) {
    return 1;
}
