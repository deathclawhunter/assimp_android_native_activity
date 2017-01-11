#include <string>
#include <android_native_app_glue.h>

#include "H264_Decoder.h"
#include "plugin.h"
#include "YUV420P_Player.h"

using namespace std;

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

private:
    void PrintFrame(AVFrame *frame);
};
