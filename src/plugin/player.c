#include <libavutil/imgutils.h>
#include <libavutil/opt.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libswresample/swresample.h>
#include <OpenGL/glu.h>
#include <stdbool.h>

#include "player.h"
#include "debug.h"
#include "player/track.h"
#include "player/queue/video.h"
#include "player/grabber/main.h"

AVFormatContext *ptrFormatContext = NULL;

// Define player states
enum PLAYER_STATES mediaPlayerState = MEDIA_PLAYER_STOPPED;

char* filename;

void mediaplayer_play () {
    if (filename == NULL) {
        Debug("No filename set, cannot play");
        return;
    }

    if (mediaPlayerState == MEDIA_PLAYER_PLAYING) {
        Debug("player is already playing");
        return;
    }

    mediaPlayerState = MEDIA_PLAYER_PLAYING;
}

void mediaplayer_pause () {
    if (mediaPlayerState == MEDIA_PLAYER_PLAYING) {
        mediaPlayerState = MEDIA_PLAYER_PAUSED;
        return;
    }

    Debug("player is not in playing state");
}

void mediaplayer_stop () {
    mediaPlayerState = MEDIA_PLAYER_STOPPED;
    mediaplayer_destroy();
}

void mediaplayer_setup (char* file) {
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

    mediaplayer_start_grabber(ptrFormatContext);
}

void mediaplayer_destroy () {
    avformat_close_input(&ptrFormatContext);
}

int mediaplayer_get_state () {
    return mediaPlayerState;
}

float mediaplayer_get_duration () {
    return (float)ptrFormatContext->duration / AV_TIME_BASE;
}

float mediaplayer_get_position () {

    /**
     * This does not feel like a very safe way to get the current
     * position.
     * TODO: see if this can be reworked
     */
    MediaPlayerVideoQueueNode* node = mediaplayer_video_queue_first();
    MediaPlayerTrack* videoTrack = mediaplayer_get_video_track();

    return (float)node->index / av_q2d(videoTrack->stream->avg_frame_rate);
}

int mediaplayer_get_audio_channels() {
    // TODO: implement
    return 2;
}

int mediaplayer_get_audio_sample_rate() {
    // TODO: implement
    return 44100;
}
