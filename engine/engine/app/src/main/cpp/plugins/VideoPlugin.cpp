#define LOG_TAG "VIDEO"

#include <math.h>
#include <string>
#include <android_native_app_glue.h>

#include "ogldev_camera.h"

extern "C" {
#include <libavutil/opt.h>
#include <libavcodec/avcodec.h>
};

using namespace std;

#include "GLError.h"
#include "VideoPlugin.h"

VideoPlugin::VideoPlugin() {
    decoder = new H264Decoder(this, NULL);
    player = new YUV420PPlayer();
    playStatus = PLUGIN_STATUS_INIT_RIGHT_NOW;
}

VideoPlugin::~VideoPlugin() {

    LOGI("VideoPlugin destructor called");

    delete decoder;
    delete player;

    if (y != NULL) av_free(y);
    if (u != NULL) av_free(u);
    if (v != NULL) av_free(v);
}

bool VideoPlugin::Init(int32_t width, int32_t height) {

    avcodec_register_all();

    if (!decoder->load("0000-0100.avi", 30.0f)) {
        LOGE("fail to load video");
        playStatus = PLUGIN_STATUS_INIT_FAIL;
        return false;
    }

    while (width > maxWidth || height > maxHeight) {
        width >>= 1;
        height >>= 1;
    }
    winWidth = width;
    winHeight = height;
    if (videoWidth > winWidth) {
        winWidth = videoWidth;
    }
    if (videoHeight > winHeight) {
        winHeight = videoHeight;
    }

    if (videoWidth > winWidth || videoHeight > winHeight) {
        // need scale down
        float fScaleW = (float) videoWidth / (float) winWidth;
        float fScaleH = (float) videoHeight / (float) winHeight;

        if (fScaleW > fScaleH) {
            scaleFactor = fScaleW;
        } else {
            scaleFactor = fScaleH;
        }

        scaleDown = true;
    } else {
        // need scale up
        float fScaleW = (float) winWidth / (float) videoWidth;
        float fScaleH = (float) winHeight / (float) videoHeight;

        if (fScaleW > fScaleH) {
            scaleFactor = fScaleH;
        } else {
            scaleFactor = fScaleW;
        }

        scaleDown = false;
    }

    int len = winWidth * winHeight;
    y = (uint8_t *) av_malloc(len);
    memset(y, 0, len); // prefilled with black
    len = winWidth * winHeight >> 2;
    u = (uint8_t *) av_malloc(len);
    memset(u, 128, len); // prefilled with black
    v = (uint8_t *) av_malloc(len);
    memset(v, 128, len); // prefilled with black

    if (!player->setup(winWidth, winHeight, width, height)) {
        LOGE("fail to setup player");
        playStatus = PLUGIN_STATUS_INIT_FAIL;
        return false;
    }

    playStatus = PLUGIN_STATUS_LOOP_ME;

    return true;
}

bool VideoPlugin::Draw() {
    return decoder->readFrame();
}

void VideoPlugin::h264_decoder_callback(H264_DECODER_STATUS status, AVFrame *frame, AVPacket *pkt) {
    if (status != DEC_STATUS_FRAME) {

        if (status == DEC_STATUS_FINISH) {
            playStatus = PLUGIN_STATUS_FINISHED; // end this plugin after play finished
            player->tearDown();
        }
        return;
    }

    frameCount++;

    // PrintFrame(frame);

    // TODO: need to add mapping code here to map the video frame to actual screen resolution
    // otherwise, we have to use fixed video size, since setYPixels() is using the width and
    // height to access the frame data. Any mismatch will cause invalid address access issue.
    // Y len vid_w * vid_h
    // U len vid_w * 0.5 * vid_h * 0.5
    // v len vid_w * 0.5 * vid_h * 0.5

    if (!scaleDown) { // TODO: support clip and scale down
        /* Cb and Cr */
        int halfVideoWidth = videoWidth * scaleFactor >> 1;
        int halfWinHeight = winHeight >> 1;
        int halfWinWidth = winWidth >> 1;
        int startHeight = (winHeight - videoHeight * scaleFactor) >> 1;
        int startWidth = (winWidth - videoWidth * scaleFactor) >> 1;
        int endHeight = startHeight + videoHeight * scaleFactor;
        int startHalfWidth = (halfWinWidth - halfVideoWidth) >> 1;
        int n = frame->linesize[0] * scaleFactor;
        int n1 = frame->linesize[1] * scaleFactor;
        int n2 = frame->linesize[2] * scaleFactor;
        for (int i = startHeight; i < endHeight; i++) {
            // Y
            int base = i * winWidth + startWidth;
            int frameBase =
                    (i - startHeight) / scaleFactor * frame->linesize[0] + frame->linesize[0] - 1;
            for (int j = 0; j < n; j++) {
                y[base + j] = frame->data[0][frameBase - j / scaleFactor];
            }

            if (i >= halfWinHeight) { // u, v is half of y
                continue;
            }

            // U
            base = i * halfWinWidth + startHalfWidth;
            frameBase =
                    (i - startHeight) / scaleFactor * frame->linesize[1] + frame->linesize[1] - 1;
            for (int j = 0; j < n1; j++) {
                u[base + j] = frame->data[1][frameBase - j / scaleFactor];
            }

            // V
            // TODO: potentially can be combined with U
            frameBase =
                    (i - startHeight) / scaleFactor * frame->linesize[2] + frame->linesize[2] - 1;
            for (int j = 0; j < n2; j++) {
                v[base + j] = frame->data[2][frameBase - j / scaleFactor];
            }
        }
    }

    player->setYPixels(y, winWidth);
    player->setUPixels(u, winWidth / 2);
    player->setVPixels(v, winWidth / 2);
    player->draw(0, 0, 0, 0);

    return;
}

// TODO: add video control later
int32_t VideoPlugin::KeyHandler(InputData *event) {
    return 1;
}

void VideoPlugin::PrintFrame(AVFrame *frame) {
    LOGI("============== Y ============");
    for (int i = 0; i < 10; i++) {
        LOGI("0x%x", frame->data[0][i]);
    }

    LOGI("============== U ============");
    for (int i = 0; i < 10; i++) {
        LOGI("0x%x", frame->data[1][i]);
    }

    LOGI("============== V ============");
    for (int i = 0; i < 10; i++) {
        LOGI("0x%x", frame->data[2][i]);
    }

    switch (frame->pict_type) {
        case AV_PICTURE_TYPE_NONE:
            LOGI("AV_PICTURE_TYPE_NONE");
            break;
        case AV_PICTURE_TYPE_I:
            LOGI("AV_PICTURE_TYPE_I");
            break;
        case AV_PICTURE_TYPE_B:
            LOGI("AV_PICTURE_TYPE_B");
            break;
        case AV_PICTURE_TYPE_S:
            LOGI("AV_PICTURE_TYPE_S");
            break;
        case AV_PICTURE_TYPE_SI:
            LOGI("AV_PICTURE_TYPE_SI");
            break;
        case AV_PICTURE_TYPE_SP:
            LOGI("AV_PICTURE_TYPE_SP");
        case AV_PICTURE_TYPE_BI:
            LOGI("AV_PICTURE_TYPE_BI");
            break;
        case AV_PICTURE_TYPE_P:
            LOGI("AV_PICTURE_TYPE_P");
            break;
        default:
            LOGI("AV_PICTURE_TYPE_ unknown %d", frame->pict_type);
            break;
    }
}

IPlugin::PLUGIN_STATUS VideoPlugin::Status() {
    return playStatus;
}



