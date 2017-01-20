#ifndef MEDIAPLAYER_VIDEO_GRABBER_HEADER
#define MEDIAPLAYER_VIDEO_GRABBER_HEADER

#include <libavcodec/avcodec.h>

#include "../track.h"

void mediaplayer_grab_video_frame (AVCodecContext *ptrCodecContext, AVFrame *ptrFrame);
void mediaplayer_video_grabber_set_sws_context (MediaPlayerTrack *ptrVideoTrack);

#endif