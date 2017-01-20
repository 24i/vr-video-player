#include <pthread.h>
#include <unistd.h>
#include <stdbool.h>
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>

#include "main.h"
#include "video.h"
#include "audio.h"
#include "../track.h"
#include "../../debug.h"
#include "../queue/video.h"

// Thread var
pthread_t grabberThread;

// Media tracks
MediaPlayerTrack *ptrVideoTrack;
MediaPlayerTrack *ptrAudioTrack;

// Video packet
AVPacket packet;

// Format context
AVFormatContext *ptrFormatContext;

// Frame container
AVFrame *ptrFrame;

/**
 * Start the the video grabber thread
 */
void mediaplayer_start_grabber (AVFormatContext *formatContext) {

    // Setup media tracks
    mediaplayer_setup_track(&ptrVideoTrack, ptrFormatContext, AVMEDIA_TYPE_VIDEO);
    mediaplayer_setup_track(&ptrAudioTrack, ptrFormatContext, AVMEDIA_TYPE_AUDIO);

    // Initialise packet & set to zero'd values
    av_init_packet(&packet);
    packet.data = NULL;
    packet.size = 0;

    // Keep reference to the format context
    ptrFormatContext = formatContext;

    // Allocate frame
    ptrFrame = av_frame_alloc();

    // Make sure context for converters is set
    mediaplayer_video_grabber_set_sws_context(ptrVideoTrack);
    mediaplayer_audio_grabber_set_swr_context(ptrAudioTrack);

    // Create grabber thread
    pthread_create(&grabberThread, NULL, mediaplayer_start_grabber_thread, NULL);
}

/**
 * Initialisation for grabbing video frames
 * @param  args [description]
 * @return      [description]
 */
void *mediaplayer_start_grabber_thread (void * args) {

    int ret;

    while (1) {

        // Arbritary check for queue size
        if (mediaplayer_video_queue_get_size() > 150) {
            continue;
        }

        ret = av_read_frame(ptrFormatContext, &packet);
        if (ret < 0) {
            if (ret == AVERROR_EOF) {
                break;
            }
            continue;
        }

        // Skip everything except the video track
        if (packet.stream_index != ptrVideoTrack->trackIndex && packet.stream_index != ptrAudioTrack->trackIndex) {
            continue;
        }

        ret = mediaplayer_grabber_packet((bool)packet.stream_index == ptrVideoTrack->trackIndex);

        if (ret < 0) {
            Debugf("Error decoding packet: %s", av_err2str(ret));
            continue;
        }

        packet.data += ret;
        packet.size -= ret;
    }

    if (ret < 0) {
        Debugf("Error reading frame: %s", av_err2str(ret));
    }

    return 0;
}

int mediaplayer_grabber_packet (bool isVideoPacket) {
    int decoded = packet.size,
        ret;

    // Codec context to use based on packet type
    AVCodecContext *ptrSelectedCodecContext = isVideoPacket ? ptrVideoTrack->codecContext : ptrAudioTrack->codecContext;
    ret = avcodec_send_packet(ptrSelectedCodecContext, &packet);

    if (ret < 0 && ret != AVERROR(EAGAIN) && ret != AVERROR_EOF) {
        Debugf("Error creating packet: %s", av_err2str(ret));
        return -1;
    }

    if (ret >= 0) {
        packet.size = 0;
    }

    ret = avcodec_receive_frame(ptrSelectedCodecContext, ptrFrame);
    if (ret >= 0) {
        if (isVideoPacket) {
            mediaplayer_grab_video_frame(ptrSelectedCodecContext, ptrFrame);
        } else {
            mediaplayer_grab_audio_frame(ptrFrame);
        }
    } else {
        Debugf("Error receiving frame: %s", av_err2str(ret));
        return ret;
    }

    av_packet_unref(&packet);
    av_frame_unref(ptrFrame);

    return decoded;
}

MediaPlayerTrack* mediaplayer_get_video_track () {
    return ptrVideoTrack;
}