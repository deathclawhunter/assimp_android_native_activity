#include <string>
#include <android_native_app_glue.h>

#include "H264_Decoder.h"
#include "plugin.h"
#include "YUV420P_Player.h"

using namespace std;

/**
 * TODO: the width and height will cause crash if it does not
 * TODO: match the YUV arrays inside AVFrame in ffmpeg callback
 */
#define VIDEO_WIDTH 800
#define VIDEO_HEIGHT 600
#define MAX_WIN_WIDTH 960;
#define MAX_WIN_HEIGHT 960;

class VideoEngine : public IPlugin, IH264CallBack {
public:

    VideoEngine();
    ~VideoEngine();

    bool Init(int32_t width, int32_t height);
    bool Draw();
    int32_t KeyHandler(AInputEvent *event);
    void h264_decoder_callback(H264_DECODER_STATUS status, AVFrame* frame, AVPacket* pkt);

private:
    H264_Decoder *decoder;
    YUV420P_Player *player;
    int frameCount;
    int winWidth, winHeight;
    int videoWidth = VIDEO_WIDTH, videoHeight = VIDEO_HEIGHT;

    uint8_t *y = NULL;
    uint8_t *u = NULL;
    uint8_t *v = NULL;

    int scaleFactor = 1;
    bool scaleDown = false;

    int maxWidth = MAX_WIN_WIDTH;
    int maxHeight = MAX_WIN_HEIGHT;

private:
    void PrintFrame(AVFrame *frame);
};
