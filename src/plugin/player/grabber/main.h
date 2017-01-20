#ifndef MEDIAPLAYER_GRABBER_HEADER
#define MEDIAPLAYER_GRABBER_HEADER

#include <stdbool.h>

#include "../track.h"

void mediaplayer_start_grabber ();
void *mediaplayer_start_grabber_thread (void * args);
int mediaplayer_grabber_packet (bool isVideoPacket);

MediaPlayerTrack* mediaplayer_get_video_track();

#endif