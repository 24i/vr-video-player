#ifndef MEDIAPLAYER_AUDIO_GRABBER_HEADER
#define MEDIAPLAYER_AUDIO_GRABBER_HEADER

#include "../track.h"

void mediaplayer_grab_audio_frame ();
void mediaplayer_audio_grabber_set_swr_context (MediaPlayerTrack *audioTrack);

#endif