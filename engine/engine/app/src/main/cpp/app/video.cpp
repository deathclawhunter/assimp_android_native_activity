#define LOG_TAG "VIDEO"

#include <math.h>
#include <string>
#include <android_native_app_glue.h>

#include "ogldev_camera.h"

extern "C" {
#include <libavutil/opt.h>
#include <libavcodec/avcodec.h>
#include <libavutil/imgutils.h>
};

using namespace std;

#include "GLError.h"
#include "video.h"

#define INBUF_SIZE 4096

/**
 * TODO: the width and height will cause crash if it does not
 * TODO: match the YUV arrays inside AVFrame in ffmpeg callback
 */
#define VIDEO_WIDTH 960
#define VIDEO_HEIGHT 540

VideoEngine::VideoEngine() {
    decoder = new H264_Decoder(this, NULL);
    player = new YUV420P_Player();
}

VideoEngine::~VideoEngine() {

    LOGI("VideoEngine destructor called");

    delete decoder;
    delete player;
}

bool VideoEngine::Init(int32_t width, int32_t height) {

    avcodec_register_all();

    if (!decoder->load("0000-0100.avi", 30.0f)) {
        LOGE("fail to load video");
        return false;
    }

    if (!player->setup(VIDEO_WIDTH, VIDEO_HEIGHT)) {
        LOGE("fail to setup player");
        return false;
    }

    return true;
}

bool VideoEngine::Draw() {
    return decoder->readFrame();
}

void VideoEngine::h264_decoder_callback(H264_DECODER_STATUS status, AVFrame* frame, AVPacket* pkt) {
    if (status != DEC_STATUS_FRAME) {
        return;
    }

    PrintFrame(frame);

    // TODO: need to add mapping code here to map the video frame to actual screen resolution
    // otherwise, we have to use fixed video size, since setYPixels() is using the width and
    // height to access the frame data. Any mismatch will cause invalid address access issue.
    player->setYPixels(frame->data[0], frame->linesize[0]);
    player->setUPixels(frame->data[1], frame->linesize[1]);
    player->setVPixels(frame->data[2], frame->linesize[2]);
    player->draw(0, 0, VIDEO_WIDTH, VIDEO_HEIGHT);
}

// TODO: add video control later
int32_t VideoEngine::KeyHandler(AInputEvent *event) {
    return 1;
}

void VideoEngine::PrintFrame(AVFrame *frame) {
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

/*
 * Video encoding example
 */
static void video_encode_example(const char *filename, AVCodecID codec_id)
{
    AVCodec *codec;
    AVCodecContext *c= NULL;
    int i, ret, x, y, got_output;
    FILE *f;
    AVFrame *frame;
    AVPacket pkt;
    uint8_t endcode[] = { 0, 0, 1, 0xb7 };

    LOGI("Encode video file %s\n", filename);

    /* find the video encoder */
    codec = avcodec_find_encoder(codec_id);
    if (!codec) {
        fprintf(stderr, "Codec not found\n");
        exit(1);
    }

    c = avcodec_alloc_context3(codec);
    if (!c) {
        fprintf(stderr, "Could not allocate video codec context\n");
        exit(1);
    }

    /* put sample parameters */
    c->bit_rate = 400000;
    /* resolution must be a multiple of two */
    c->width = 352;
    c->height = 288;
    /* frames per second */
    c->time_base = (AVRational){1,25};
    /* emit one intra frame every ten frames
     * check frame pict_type before passing frame
     * to encoder, if frame->pict_type is AV_PICTURE_TYPE_I
     * then gop_size is ignored and the output of encoder
     * will always be I frame irrespective to gop_size
     */
    c->gop_size = 10;
    c->max_b_frames = 1;
    c->pix_fmt = AV_PIX_FMT_YUV420P;

    if (codec_id == AV_CODEC_ID_H264)
        av_opt_set(c->priv_data, "preset", "slow", 0);

    /* open it */
    if (avcodec_open2(c, codec, NULL) < 0) {
        fprintf(stderr, "Could not open codec\n");
        exit(1);
    }

    f = fopen(filename, "wb");
    if (!f) {
        fprintf(stderr, "Could not open %s\n", filename);
        exit(1);
    }

    frame = av_frame_alloc();
    if (!frame) {
        fprintf(stderr, "Could not allocate video frame\n");
        exit(1);
    }
    frame->format = c->pix_fmt;
    frame->width  = c->width;
    frame->height = c->height;

    /* the image can be allocated by any means and av_image_alloc() is
     * just the most convenient way if av_malloc() is to be used */
    ret = av_image_alloc(frame->data, frame->linesize, c->width, c->height,
                         c->pix_fmt, 32);
    if (ret < 0) {
        fprintf(stderr, "Could not allocate raw picture buffer\n");
        exit(1);
    }

    /* encode 1 second of video */
    for (i = 0; i < 25; i++) {
        av_init_packet(&pkt);
        pkt.data = NULL;    // packet data will be allocated by the encoder
        pkt.size = 0;

        fflush(stdout);
        /* prepare a dummy image */
        /* Y */
        for (y = 0; y < c->height; y++) {
            for (x = 0; x < c->width; x++) {
                frame->data[0][y * frame->linesize[0] + x] = x + y + i * 3;
            }
        }

        /* Cb and Cr */
        for (y = 0; y < c->height/2; y++) {
            for (x = 0; x < c->width/2; x++) {
                frame->data[1][y * frame->linesize[1] + x] = 128 + y + i * 2;
                frame->data[2][y * frame->linesize[2] + x] = 64 + x + i * 5;
            }
        }

        frame->pts = i;

        /* encode the image */
        ret = avcodec_encode_video2(c, &pkt, frame, &got_output);
        if (ret < 0) {
            fprintf(stderr, "Error encoding frame\n");
            exit(1);
        }

        if (got_output) {
            printf("Write frame %3d (size=%5d)\n", i, pkt.size);
            fwrite(pkt.data, 1, pkt.size, f);
            av_packet_unref(&pkt);
        }
    }

    /* get the delayed frames */
    for (got_output = 1; got_output; i++) {
        fflush(stdout);

        ret = avcodec_encode_video2(c, &pkt, NULL, &got_output);
        if (ret < 0) {
            fprintf(stderr, "Error encoding frame\n");
            exit(1);
        }

        if (got_output) {
            printf("Write frame %3d (size=%5d)\n", i, pkt.size);
            fwrite(pkt.data, 1, pkt.size, f);
            av_packet_unref(&pkt);
        }
    }

    /* add sequence end code to have a real MPEG file */
    fwrite(endcode, 1, sizeof(endcode), f);
    fclose(f);

    avcodec_close(c);
    av_free(c);
    av_freep(&frame->data[0]);
    av_frame_free(&frame);
    printf("\n");
}

static void pgm_save(unsigned char *buf, int wrap, int xsize, int ysize,
                     char *filename)
{
    FILE *f;
    int i;

    f = fopen(filename,"w");
    fprintf(f, "P5\n%d %d\n%d\n", xsize, ysize, 255);
    for (i = 0; i < ysize; i++)
        fwrite(buf + i * wrap, 1, xsize, f);
    fclose(f);
}

static int decode_write_frame(const char *outfilename, AVCodecContext *avctx,
                              AVFrame *frame, int *frame_count, AVPacket *pkt, int last)
{
    int len, got_frame;
    char buf[1024];

    len = avcodec_decode_video2(avctx, frame, &got_frame, pkt);
    if (len < 0) {
        fprintf(stderr, "Error while decoding frame %d\n", *frame_count);
        return len;
    }
    if (got_frame) {
        printf("Saving %sframe %3d\n", last ? "last " : "", *frame_count);
        fflush(stdout);

        /* the picture is allocated by the decoder, no need to free it */
        snprintf(buf, sizeof(buf), outfilename, *frame_count);
        pgm_save(frame->data[0], frame->linesize[0],
                 frame->width, frame->height, buf);
        (*frame_count)++;
    }
    if (pkt->data) {
        pkt->size -= len;
        pkt->data += len;
    }
    return 0;
}

static void video_decode_example(const char *outfilename, const char *filename)
{
    AVCodec *codec;
    AVCodecContext *c= NULL;
    int frame_count;
    FILE *f;
    AVFrame *frame;
    uint8_t inbuf[INBUF_SIZE + AV_INPUT_BUFFER_PADDING_SIZE];
    AVPacket avpkt;

    av_init_packet(&avpkt);

    /* set end of buffer to 0 (this ensures that no overreading happens for damaged MPEG streams) */
    memset(inbuf + INBUF_SIZE, 0, AV_INPUT_BUFFER_PADDING_SIZE);

    printf("Decode video file %s to %s\n", filename, outfilename);

    /* find the MPEG-1 video decoder */
    codec = avcodec_find_decoder(AV_CODEC_ID_MPEG1VIDEO);
    if (!codec) {
        fprintf(stderr, "Codec not found\n");
        exit(1);
    }

    c = avcodec_alloc_context3(codec);
    if (!c) {
        fprintf(stderr, "Could not allocate video codec context\n");
        exit(1);
    }

    if (codec->capabilities & AV_CODEC_CAP_TRUNCATED)
        c->flags |= AV_CODEC_FLAG_TRUNCATED; // we do not send complete frames

    /* For some codecs, such as msmpeg4 and mpeg4, width and height
       MUST be initialized there because this information is not
       available in the bitstream. */

    /* open it */
    if (avcodec_open2(c, codec, NULL) < 0) {
        fprintf(stderr, "Could not open codec\n");
        exit(1);
    }

    f = fopen(filename, "rb");
    if (!f) {
        fprintf(stderr, "Could not open %s\n", filename);
        exit(1);
    }

    frame = av_frame_alloc();
    if (!frame) {
        fprintf(stderr, "Could not allocate video frame\n");
        exit(1);
    }

    frame_count = 0;
    for (;;) {
        avpkt.size = fread(inbuf, 1, INBUF_SIZE, f);
        if (avpkt.size == 0)
            break;

        /* NOTE1: some codecs are stream based (mpegvideo, mpegaudio)
           and this is the only method to use them because you cannot
           know the compressed data size before analysing it.

           BUT some other codecs (msmpeg4, mpeg4) are inherently frame
           based, so you must call them with all the data for one
           frame exactly. You must also initialize 'width' and
           'height' before initializing them. */

        /* NOTE2: some codecs allow the raw parameters (frame size,
           sample rate) to be changed at any frame. We handle this, so
           you should also take care of it */

        /* here, we use a stream based decoder (mpeg1video), so we
           feed decoder and see if it could decode a frame */
        avpkt.data = inbuf;
        while (avpkt.size > 0)
            if (decode_write_frame(outfilename, c, frame, &frame_count, &avpkt, 0) < 0)
                exit(1);
    }

    /* Some codecs, such as MPEG, transmit the I- and P-frame with a
       latency of one frame. You must do the following to have a
       chance to get the last frame of the video. */
    avpkt.data = NULL;
    avpkt.size = 0;
    decode_write_frame(outfilename, c, frame, &frame_count, &avpkt, 1);

    fclose(f);

    avcodec_close(c);
    av_free(c);
    av_frame_free(&frame);
    printf("\n");
}

// video_encode_example("test.mpg", AV_CODEC_ID_MPEG1VIDEO);
// video_decode_example("test%02d.pgm", "test.mpg");
// video_decode_example("test%02d.pgm", "0000-0100.mp4");


