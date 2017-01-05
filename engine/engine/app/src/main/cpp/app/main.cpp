#define LOG_TAG "ENGINE_MAIN"

#include "engine.h"

using namespace std;

#define HELLOWORLD 0 // Use to test basic shader

#include "AppLog.h"
#include "GLError.h"


extern "C" {
    void extract_assets(struct android_app *app);

#include <libavutil/opt.h>
#include <libavcodec/avcodec.h>
#include <libavutil/channel_layout.h>
#include <libavutil/common.h>
#include <libavutil/imgutils.h>
#include <libavutil/mathematics.h>
#include <libavutil/samplefmt.h>


}
int init_display(struct engine *engine);
void terminate_display(struct engine *engine);

/**
 * Just the current frame in the display.
 */
void draw_frame(struct engine *engine) {

    // No display.
    if (engine->display == NULL) {
        return;
    }

    // FIXME: hack to test if opengl is ready for use,
    // sometimes the glCreateProgram() will fail even
    // when reach here
    if (!engine->initialized) {

        int prog = glCreateProgram();
        if (prog == 0) {
            checkGlError("test opengl: ");
        } else {

            static float grey;
            grey += 0.01f;
            if (grey > 1.0f) {
                grey = 0.0f;
            }
            glClearColor(grey, grey, grey, 1.0f);
            checkGlError("glClearColor");

            LOGI("draw_frame: got prog = 0x%x in main\n", prog);
            engine->pContext = engine->pfInit(engine->width, engine->height);
            engine->initialized = true;
        }
    } else {
        engine->pfDrawFrame(engine->pContext);
        eglSwapBuffers(engine->display, engine->surface);
    }
}

/**
 * Process the next input event.
 */
int32_t app_input_handler(struct android_app *app, AInputEvent *event) {
    if (AInputEvent_getType(event) == AINPUT_EVENT_TYPE_MOTION) {
        struct engine *engine = (struct engine *) app->userData;
        return engine->pfKeyHandler(engine->pContext, event);
    }
    return 0;
}

/**
 * Process the next main command.
 */
void app_cmd_handler(struct android_app *app, int32_t cmd) {
    struct engine *engine = (struct engine *) app->userData;
    switch (cmd) {
        case APP_CMD_SAVE_STATE:
            break;
        case APP_CMD_INIT_WINDOW:
            // The window is being shown, get it ready.
            if (engine->app->window != NULL) {
                init_display(engine);
                draw_frame(engine);
            }
            break;
        case APP_CMD_TERM_WINDOW:
            // The window is being hidden or closed, clean it up.
            terminate_display(engine);
            break;
        case APP_CMD_LOST_FOCUS:
            draw_frame(engine);
            break;
    }
}

#if HELLOWORLD
extern void* helloInit(int32_t width, int32_t height);
extern void helloDrawFrame(void *pContext);
extern int32_t helloKeyHandler(void *pContext, AInputEvent *event);
void initPlugins(struct engine *engine) {
    engine->pfInit = helloInit;
    engine->pfDrawFrame = helloDrawFrame;
    engine->pfKeyHandler = helloKeyHandler;
}
#else
extern void* appInit(int32_t width, int32_t height);
extern void appDrawFrame(void *pContext);
extern int32_t appKeyHandler(void *pContext, AInputEvent *event);

void initPlugins(struct engine *engine) {
    engine->pfInit = appInit;
    engine->pfDrawFrame = appDrawFrame;
    engine->pfKeyHandler = appKeyHandler;
}
#endif

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

    printf("Encode video file %s\n", filename);

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

/**
 * Main entry point, handles events
 */
void android_main(struct android_app *state) {
    app_dummy();

    LOGI("in android_main\n");

    struct engine engine;
    memset(&engine, 0, sizeof(engine));
    state->userData = &engine;
    state->onAppCmd = app_cmd_handler;
    state->onInputEvent = app_input_handler;
    engine.app = state;
    initPlugins(&engine);

    extract_assets(state);

    avcodec_register_all();
    video_encode_example("test.mpg", AV_CODEC_ID_MPEG1VIDEO);
    // video_decode_example("test%02d.pgm", "test.mpg");

    // Read all pending events.
    while (1) {
        int events;
        struct android_poll_source *source;

        while (ALooper_pollAll(0, NULL, &events, (void **) &source) >= 0) {

            // Process this event.
            if (source != NULL) {
                source->process(state, source);
            }

            // Check if we are exiting.
            if (state->destroyRequested != 0) {
                terminate_display(&engine);
                exit(1); // exit directly
            }
        }

        // Draw the current frame
        draw_frame(&engine);
    }
}


