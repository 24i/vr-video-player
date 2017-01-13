#include <libavutil/imgutils.h>
#include <libavutil/opt.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libswresample/swresample.h>
#include <OpenGL/glu.h>
#include <stdbool.h>

#include "video.h"
#include "debug.h"
#include "player/track.h"

AVFormatContext *ptrFormatContext = NULL;
AVCodecContext *ptrCodecContext = NULL;
AVFrame *ptrFrame = NULL;
AVPacket packet;

VRPlayerTrack *audioTrack = NULL;
VRPlayerTrack *videoTrack = NULL;

struct SwsContext* swsContext;
struct SwrContext* swrContext;

static uint8_t *ptrVideoData[4] = { NULL };
static uint8_t *ptrAudioData = NULL;
static int videoLinesize[4];
static int videoWidth;
static int videoHeight;
static int videoBufferSize;
static enum AVPixelFormat videoPixelFormat;

// Define player states
enum PLAYER_STATES {
    VR_VIDEO_PLAYER_STOPPED = 1,
    VR_VIDEO_PLAYER_PAUSED = 2,
    VR_VIDEO_PLAYER_PLAYING = 3,
    VR_VIDEO_PLAYER_BUFFERING = 4
};
enum PLAYER_STATES vrVideoPlayerState = VR_VIDEO_PLAYER_STOPPED;

char* filename;

void vr_player_play () {
    if (filename == NULL) {
        Debug("No filename set, cannot play");
        return;
    }

    if (vrVideoPlayerState == VR_VIDEO_PLAYER_PLAYING) {
        Debug("player is already playing");
        return;
    }

    vrVideoPlayerState = VR_VIDEO_PLAYER_PLAYING;
}

void vr_player_pause () {
    if (vrVideoPlayerState == VR_VIDEO_PLAYER_PLAYING) {
        vrVideoPlayerState = VR_VIDEO_PLAYER_PAUSED;
        return;
    }

    Debug("player is not in playing state");
}

void vr_player_stop () {
    vrVideoPlayerState = VR_VIDEO_PLAYER_STOPPED;
    vr_player_destroy();
}

void vr_player_setup_video_track () {
    vr_player_setup_track(&videoTrack, ptrFormatContext, AVMEDIA_TYPE_VIDEO);
    if (videoTrack == NULL) {
        Debug("Could not initialize video track");
        return;
    }
    ptrCodecContext = videoTrack->codecContext;

    // TODO: refactor this into a separate struct
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

void vr_player_setup_audio_track () {
    vr_player_setup_track(&audioTrack, ptrFormatContext, AVMEDIA_TYPE_AUDIO);

    swrContext = swr_alloc();
    av_opt_set_int(swrContext, "in_channel_layout",  audioTrack->codecContext->channel_layout, 0);
    av_opt_set_int(swrContext, "out_channel_layout", audioTrack->codecContext->channel_layout,  0);
    av_opt_set_int(swrContext, "in_sample_rate",     audioTrack->codecContext->sample_rate, 0);
    av_opt_set_int(swrContext, "out_sample_rate",    audioTrack->codecContext->sample_rate, 0);
    av_opt_set_sample_fmt(swrContext, "in_sample_fmt",  AV_SAMPLE_FMT_FLTP, 0);
    av_opt_set_sample_fmt(swrContext, "out_sample_fmt", AV_SAMPLE_FMT_S16,  0);
    swr_init(swrContext);

}

void vr_player_setup (char* file) {
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

    vr_player_setup_video_track();
    vr_player_setup_audio_track();
}

void vr_player_destroy () {
    av_frame_free(&ptrFrame);
    avcodec_close(videoTrack->codecContext);
    avformat_close_input(&ptrFormatContext);
}

int vr_decode_packet () {
    int decoded = packet.size, ret;

    // TODO: find a better way to get codec context from tracks
    bool isVideoTrack = packet.stream_index == videoTrack->trackIndex;
    AVCodecContext *ptrCodecContextType = isVideoTrack ? videoTrack->codecContext : audioTrack->codecContext;

    // Try to decode the frame into a packet
    ret = avcodec_send_packet(ptrCodecContextType, &packet);
    if (ret < 0 && ret != AVERROR(EAGAIN) && ret != AVERROR_EOF) {
        Debugf("Error creating packet: %s", av_err2str(ret));
        return -1;
    }

    if (ret >= 0) {
        packet.size = 0;
    }

    ret = avcodec_receive_frame(ptrCodecContextType, ptrFrame);
    if (ret >= 0) {
        // Do stuff with frame

        if (isVideoTrack) {
            sws_scale(swsContext, (uint8_t const * const *)(ptrFrame->data), ptrFrame->linesize, 0, ptrCodecContext->height, ptrVideoData, videoLinesize);
        } else {

            //swr_convert(swrContext, &outputBuffer, ptrFrame->nb_samples, (uint8_t const **)ptrFrame->extended_data, ptrFrame->nb_samples);
            //AudioData(outputBuffer, 0);

            Debugf("Audio %d", ptrCodecContextType->channels);
        }
    } else {
        Debugf("Error receiving frame: %s", av_err2str(ret));
        return ret;
    }

    av_packet_unref(&packet);
    av_frame_unref(ptrFrame);

    return decoded;
}

int vr_player_generate_texture () {

    // Do not generate anything when not in the playing state
    if (vrVideoPlayerState != VR_VIDEO_PLAYER_PLAYING) {
        return -1;
    }

    if (av_read_frame(ptrFormatContext, &packet) >= 0) {

        /*
         * Ignore non selected tracks
         * TODO: find a better way to differentiate between active tracks
         */
        if (packet.stream_index != videoTrack->trackIndex && packet.stream_index != audioTrack->trackIndex) {
            return -1;
        }

        int ret = vr_decode_packet();
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

void vr_player_render_texture (void* ptrVideoTexId) {

    // Don't render anything without a texture to render on
    if (ptrVideoTexId == NULL) {
        return;
    }

    if (ptrVideoData[0] == NULL) {
        return;
    }

    GLuint gltex = (GLuint)(size_t)(ptrVideoTexId);
    glBindTexture(GL_TEXTURE_2D, gltex);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, videoWidth, videoHeight, GL_RGB, GL_UNSIGNED_BYTE, ptrVideoData[0]);

}

int vr_player_get_state () {
    return vrVideoPlayerState;
}