//
// Created by Davis Z on 1/5/17.
//

/*

  H264Decoder
  ---------------------------------------

  Example that shows how to use the libav parser system. This class forces a
  H264 parser and codec. You use it by opening a file that is encoded with x264
  using the `load()` function. You can pass the framerate you want to use for playback.
  If you don't pass the framerate, we will detect it as soon as the parser found
  the correct information.

  After calling load(), you can call readFrame() which will read a new frame when
  necessary. It will also make sure that it will read enough data from the buffer/file
  when there is not enough data in the buffer.

  `ReadFrame()` will trigger calls to the given `h264_decoder_callback` that you pass
  to the constructor.

 */
#ifndef H264_DECODER_H
#define H264_DECODER_H

#define H264_INBUF_SIZE 16384                                                           /* number of bytes we read per chunk */

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavutil/avutil.h>
}

enum H264_DECODER_STATUS {
    DEC_STATUS_FRAME,
    DEC_STATUS_ERROR_OPEN_FILE,
    DEC_STATUS_FINISH
};

class IH264CallBack {
public:
    virtual void h264_decoder_callback(H264_DECODER_STATUS status, AVFrame *frame,
                                       AVPacket *pkt) = 0;         /* the decoder callback, which will be called when we have decoded a frame */
};

typedef

class H264Decoder {

public:
    H264Decoder(IH264CallBack *frameCallback, void *user);

    /* pass in a callback function that is called whenever we decoded a video frame, make sure to call `ReadFrame()` repeatedly */
    ~H264Decoder();

    /* d'tor, cleans up the allocated objects and closes the codec context */
    bool load(const char *filepath, float fps = 0.0f);

    /* load a video file which is encoded with x264 */
    bool readFrame();                                                                      /* read a frame if necessary */

private:
    bool update(bool &needsMoreBytes);

    /* internally used to update/parse the data we read from the buffer or file */
    int readBuffer();

    /* read a bit more data from the buffer */
    void decodeFrame(uint8_t *data, int size);

    /* decode a frame we read from the buffer */

public:
    AVCodec *codec;
    /* the AVCodec* which represents the H264 decoder */
    AVCodecContext *codec_context;
    /* the context; keeps generic state */
    AVCodecParserContext *parser;
    /* parser that is used to decode the h264 bitstream */
    AVFrame *picture;
    /* will contain a decoded picture */
    uint8_t inbuf[H264_INBUF_SIZE + FF_INPUT_BUFFER_PADDING_SIZE];
    /* used to read chunks from the file */
    FILE *fp;
    /* file pointer to the file from which we read the h264 data */
    int frame;
    /* the number of decoded frames */
    IH264CallBack *cb_frame;
    /* the callback function which will receive the frame/packet data */
    void *cb_user;
    /* the void* with user data that is passed into the set callback */
    uint64_t frame_timeout;
    /* timeout when we need to parse a new frame */
    uint64_t frame_delay;
    /* delay between frames (in ns) */
    std::vector<uint8_t> buffer;                                                           /* buffer we use to keep track of read/unused bitstream data */
};

#endif

