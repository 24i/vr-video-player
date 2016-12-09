#include <libavutil/imgutils.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>

#include "video.h"

AVFormatContext *ptrFormatContext = NULL;
AVCodecContext *ptrCodecContext = NULL;
AVFrame *ptrFrame = NULL;
AVPacket packet;

struct SwsContext* swsContext;

static uint8_t *ptrVideoData[4] = { NULL };
static int videoLinesize[4];
static int videoWidth;
static int videoHeight;
static int videoBufferSize;
static enum AVPixelFormat videoPixelFormat;

// Define player states
enum PLAYER_STATES {
    VR_VIDEO_PLAYER_STOPPED = 1,
    VR_VIDEO_PLAYER_PAUSED = 2,
    VR_VIDEO_PLAYER_PLAYING = 3
};
enum PLAYER_STATES vrVideoPlayerState = VR_VIDEO_PLAYER_STOPPED;

char* filename;

void VRVideoPlayerPlay () {
    if (filename == NULL) {
        Debug("VR Video Player error: no file set");
        return;
    }

    if (vrVideoPlayerState == VR_VIDEO_PLAYER_PLAYING) {
        Debug("VR Video Player is already playing");
        return;
    }

    vrVideoPlayerState = VR_VIDEO_PLAYER_PLAYING;
}

void VRVideoPlayerPause () {
    if (vrVideoPlayerState == VR_VIDEO_PLAYER_PLAYING) {
        vrVideoPlayerState = VR_VIDEO_PLAYER_PAUSED;
        return;
    }

    Debug("VR Video Player is not in playing state");
}

void VRVideoPlayerStop () {
    vrVideoPlayerState = VR_VIDEO_PLAYER_STOPPED;
    VRVideoPlayerDestroy();
}

void _VRVideoPlayerVideoSetup () {
    int returnValue;
    int videoStreamIndex = av_find_best_stream(ptrFormatContext, AVMEDIA_TYPE_VIDEO, -1, -1, NULL, 0);

    AVStream *ptrStream = ptrFormatContext->streams[videoStreamIndex];
    AVCodec *ptrCodec = avcodec_find_decoder(ptrStream->codecpar->codec_id);
    if (ptrCodec == NULL) {
        Debug("Decoder not found for video");
        return;
    }

    ptrCodecContext = avcodec_alloc_context3(ptrCodec);
    if (ptrCodecContext == NULL) {
        Debug("Decoder context could not be allocated");
        return;
    }
    avcodec_parameters_to_context(ptrCodecContext, ptrStream->codecpar);

    AVDictionary *opts = NULL;
    av_dict_set(&opts, "refcounted_frames", "0", 0);
    returnValue = avcodec_open2(ptrCodecContext, ptrCodec, &opts);
    if (returnValue < 0) {
        Debugf("Codec context could not be initialized: %d", returnValue);
        return;
    }

    videoWidth = ptrCodecContext->width;
    videoHeight = ptrCodecContext->height;
    videoPixelFormat = ptrCodecContext->pix_fmt;
    videoBufferSize = av_image_alloc(ptrVideoData, videoLinesize, videoWidth, videoHeight, AV_PIX_FMT_RGB24, 1);
    if (videoBufferSize < 0) {
        Debugf("Image could not be allocated: %d", videoBufferSize);
        return;
    }

    ptrFrame = av_frame_alloc();
    if (ptrFrame == NULL) {
        Debug("Frame could not be allocated");
    }

    av_init_packet(&packet);
    packet.data = NULL;
    packet.size = 0;

    swsContext = sws_getContext(ptrCodecContext->width, ptrCodecContext->height, AV_PIX_FMT_YUV420P, videoWidth, videoHeight, AV_PIX_FMT_RGB24, SWS_BILINEAR, NULL, NULL, NULL);
}

void VRVideoPlayerSetup (char* file) {
    int returnValue;

    filename = file;
    returnValue = avformat_open_input(&ptrFormatContext, file, NULL, NULL);
    if (returnValue < 0) {
        Debugf("Error opening input: %d", returnValue);
        return;
    }

    returnValue = avformat_find_stream_info(ptrFormatContext, NULL);
    if (returnValue < 0) {
        Debugf("Error finding stream info: %d", returnValue);
        return;
    }

    _VRVideoPlayerVideoSetup();
}

void VRVideoPlayerDestroy () {
    av_frame_free(&ptrFrame);
    avcodec_close(ptrCodecContext);
    avformat_close_input(&ptrFormatContext);
}

int DecodePacket (int *receivedFrame, int cached) {
    int decoded = packet.size, ret;

    // Did not receive frame by default
    receivedFrame = 0;

    // Try to decode the frame into a packet
    ret = avcodec_send_packet(ptrCodecContext, &packet);
    if (ret < 0 && ret != AVERROR(EAGAIN) && ret != AVERROR_EOF) {
        Debugf("Error creating packet: %s", av_err2str(ret));
        return -1;
    }

    if (ret >= 0) {
        packet.size = 0;
    }

    ret = avcodec_receive_frame(ptrCodecContext, ptrFrame);
    if (ret >= 0) {
        sws_scale(swsContext, (uint8_t const * const *)(ptrFrame->data), ptrFrame->linesize, 0, ptrCodecContext->height, ptrVideoData, videoLinesize);
    } else {
        Debugf("Error receiving frame: %s", av_err2str(ret));
        return ret;
    }

    av_packet_unref(&packet);
    av_frame_unref(ptrFrame);

    return decoded;
}

int GenerateVideoTexture () {

    // Do not generate anything when not in the playing state
    if (vrVideoPlayerState != VR_VIDEO_PLAYER_PLAYING) {
        return -1;
    }

    if (av_read_frame(ptrFormatContext, &packet) >= 0) {
        AVPacket originalPacket = packet;

        int receivedFrame;
        int ret = DecodePacket(&receivedFrame, 0);

        if (ret < 0) {
            Debugf("Error decoding packet: %s", av_err2str(ret));
            return -1;
        }

        packet.data += ret;
        packet.size -= ret;
    } else {
        Debug("Error reading frame");
    }

    return 0;
}